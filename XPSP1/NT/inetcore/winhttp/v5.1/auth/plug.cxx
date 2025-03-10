#include <wininetp.h>
#include <splugin.hxx>
#include "auth.h"
#include "sspspm.h"
#include "winctxt.h"

extern SspData  *g_pSspData;
/*-----------------------------------------------------------------------------
    PLUG_CTX
-----------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
    Load
---------------------------------------------------------------------------*/
DWORD PLUG_CTX::Load()
{
    INET_ASSERT(_pSPMData == _pCreds->pSPM);

    DWORD_PTR dwAuthCode = 0;

    dwAuthCode = SSPI_InitScheme (GetScheme());

    if (!dwAuthCode)
    {
        _pSPMData->eState = STATE_ERROR;
        return ERROR_WINHTTP_INTERNAL_ERROR;
    }

    _pSPMData->eState = STATE_LOADED;
    return ERROR_SUCCESS;
}


/*---------------------------------------------------------------------------
    ClearAuthUser
---------------------------------------------------------------------------*/
DWORD PLUG_CTX::ClearAuthUser(LPVOID *ppvContext, LPSTR szServer)
{
    if (GetState() == AUTHCTX::STATE_LOADED)
    {
        __try
        {
            UnloadAuthenticateUser(ppvContext, szServer, GetScheme());
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            DEBUG_PRINT(HTTP, ERROR,
                ("UnloadAuthenticateUser call down faulted\n"));
        }
        ENDEXCEPT
    }
    *ppvContext = 0;
    return ERROR_SUCCESS;
}

/*-----------------------------------------------------------------------------
    wQueryHeadersAlloc

Routine Description:

    Allocates a HTTP Header String, and queries the HTTP handle for it.

Arguments:

    hRequestMapped          - An open HTTP request handle
                               where headers can be quiered
    dwQuery                 - The Query Type to pass to HttpQueryHeaders
    lpdwQueryIndex          - The Index of the header to pass to HttpQueryHeaders,
                              make sure to inialize to 0.
    lppszOutStr             - On success, a pointer to Allocated string with header string,
    lpdwSize                - size of the string returned in lppszOutStr

Return Value:

    DWORD
    Success - ERROR_SUCCESS

    Failure - One of Several Error codes defined in winerror.h or wininet.w

Comments:

    On Error, lppszOutStr may still contain an allocated string that will need to be
    freed.
-----------------------------------------------------------------------------*/
DWORD PLUG_CTX::wQueryHeadersAlloc
(
    IN HINTERNET hRequestMapped,
    IN DWORD dwQuery,
    OUT LPDWORD lpdwQueryIndex,
    OUT LPSTR *lppszOutStr,
    OUT LPDWORD lpdwSize
)
{
    LPSTR lpszRawHeaderBuf = NULL;
    DWORD dwcbRawHeaderBuf = 0;
    DWORD error;
    DWORD length;
    HTTP_REQUEST_HANDLE_OBJECT * pHttpRequest;

    INET_ASSERT(lppszOutStr);
    INET_ASSERT(hRequestMapped);
    INET_ASSERT(lpdwSize);
    INET_ASSERT((dwQuery & HTTP_QUERY_HEADER_MASK) != HTTP_QUERY_CUSTOM);

    *lppszOutStr = NULL;
    error = ERROR_SUCCESS;
    pHttpRequest = (HTTP_REQUEST_HANDLE_OBJECT *) hRequestMapped;

    // Attempt to determine whether our header is there.
    length = 0;
    if (pHttpRequest->QueryInfo(dwQuery, NULL, NULL, &length, lpdwQueryIndex)
          != ERROR_INSUFFICIENT_BUFFER)
    {
        // no authentication happening, we're done
        error = ERROR_HTTP_HEADER_NOT_FOUND;
        goto quit;
    }

    // Allocate a Fixed Size Buffer
    lpszRawHeaderBuf = (LPSTR) ALLOCATE_ZERO_MEMORY(length);
    dwcbRawHeaderBuf = length;

    if ( lpszRawHeaderBuf == NULL )
    {
        error = ERROR_NOT_ENOUGH_MEMORY;
        goto quit;
    }

    error = pHttpRequest->QueryInfo
        (dwQuery, NULL, lpszRawHeaderBuf, &dwcbRawHeaderBuf, lpdwQueryIndex);

    INET_ASSERT(error != ERROR_INSUFFICIENT_BUFFER );
    INET_ASSERT(error != ERROR_HTTP_HEADER_NOT_FOUND );

quit:

    if ( error != ERROR_SUCCESS  )
    {
        dwcbRawHeaderBuf = 0;

        if ( lpszRawHeaderBuf )
            *lpszRawHeaderBuf = '\0';
    }

    *lppszOutStr = lpszRawHeaderBuf;
    *lpdwSize = dwcbRawHeaderBuf;

    return error;
}

/*-----------------------------------------------------------------------------
    CrackAuthenticationHeader

Routine Description:

    Attempts to decode a HTTP 1.1 Authentication header into its
    components.

Arguments:

    hRequestMapped           - Mapped Request handle
    fIsProxy                 - Whether proxy or server auth
    lpdwAuthenticationIndex  - Index of current HTTP header. ( initally called with 0 )
    lppszAuthHeader          - allocated pointer which should be freed by client
    lppszAuthScheme          - Pointer to Authentication scheme string.
    lppszRealm               - Pointer to Realm string,
    lpExtra                  - Pointer to any Extra String data in the header that is not
                                   part of the Realm
    lpdwExtra                - Pointer to Size of Extra data.
    lppszAuthScheme

  Return Value:

    DWORD
    Success - ERROR_SUCCESS

    Failure - ERROR_NOT_ENOUGH_MEMORY,
              ERROR_HTTP_HEADER_NOT_FOUND

Comments:
-----------------------------------------------------------------------------*/
DWORD PLUG_CTX::CrackAuthenticationHeader
(
    IN HINTERNET hRequestMapped,
    IN BOOL      fIsProxy,
    IN     DWORD dwAuthenticationIndex,
    IN OUT LPSTR *lppszAuthHeader,
    IN OUT LPSTR *lppszExtra,
    IN OUT DWORD *lpdwExtra,
       OUT LPSTR *lppszAuthScheme
    )
{
    DWORD error = ERROR_SUCCESS;

    LPSTR lpszAuthHeader = NULL;
    DWORD cbAuthHeader = 0;
    LPSTR lpszExtra = NULL;
    LPSTR lpszAuthScheme = NULL;

    LPDWORD lpdwAuthenticationIndex = &dwAuthenticationIndex;
    INET_ASSERT(lpdwExtra);
    INET_ASSERT(lppszExtra);
    INET_ASSERT(lpdwAuthenticationIndex);

    DWORD dwQuery = fIsProxy?
        HTTP_QUERY_PROXY_AUTHENTICATE : HTTP_QUERY_WWW_AUTHENTICATE;

    error = wQueryHeadersAlloc (hRequestMapped, dwQuery,
        lpdwAuthenticationIndex, &lpszAuthHeader, &cbAuthHeader);

    if ( error != ERROR_SUCCESS )
    {
        INET_ASSERT(*lpdwAuthenticationIndex
            || error == ERROR_HTTP_HEADER_NOT_FOUND );
        goto quit;
    }


    //
    // Parse Header for Scheme type
    //
    lpszAuthScheme = lpszAuthHeader;

    while ( *lpszAuthScheme == ' ' )  // strip spaces
        lpszAuthScheme++;

    lpszExtra = strchr(lpszAuthScheme, ' ');

    if (lpszExtra)
        *lpszExtra++ = '\0';

    if (lstrcmpi(GetScheme(), lpszAuthScheme))
    {
        DEBUG_PRINT(HTTP, ERROR,
               ("Authentication: HTTP Scheme has changed!: Scheme=%q\n",
                lpszAuthScheme));
        goto quit;

    }


    DEBUG_PRINT (HTTP, INFO,
        ("Authentication: found in headers: Scheme=%q, Extra=%q\n",
        lpszAuthScheme, lpszExtra));

quit:
    *lppszExtra  = lpszExtra;
    *lpdwExtra   = lpszExtra ? lstrlen(lpszExtra) : 0;
    *lppszAuthHeader = lpszAuthHeader;
    *lppszAuthScheme = lpszAuthScheme;
    return error;
}


/*---------------------------------------------------------------------------
    ResolveProtocol
---------------------------------------------------------------------------*/
VOID PLUG_CTX::ResolveProtocol()
{
    SECURITY_STATUS ssResult;
    PWINCONTEXT pWinContext;
    SecPkgContext_NegotiationInfo SecPkgCtxtInfo;

    INET_ASSERT(GetSchemeType() == WINHTTP_AUTH_SCHEME_NEGOTIATE);
    
    SecPkgCtxtInfo.PackageInfo = NULL;

    // Call QueryContextAttributes on the context handle.
    pWinContext = (PWINCONTEXT) (_pvContext);
    ssResult = (*(g_pSspData->pFuncTbl->QueryContextAttributes))
        (pWinContext->pSspContextHandle, SECPKG_ATTR_NEGOTIATION_INFO, &SecPkgCtxtInfo);

    if (ssResult == SEC_E_OK 
        && (SecPkgCtxtInfo.NegotiationState == SECPKG_NEGOTIATION_COMPLETE
            || (SecPkgCtxtInfo.NegotiationState == SECPKG_NEGOTIATION_OPTIMISTIC)))
    {
        // Resolve actual auth protocol from package name.
        // update both the auth context and Creds entry.
        if (!lstrcmpi(SecPkgCtxtInfo.PackageInfo->Name, "NTLM"))
        {
            _eSubScheme = WINHTTP_AUTH_SCHEME_NTLM;
            _dwSubFlags = PLUGIN_AUTH_FLAGS_NO_REALM;
        }
        else if (!lstrcmpi(SecPkgCtxtInfo.PackageInfo->Name, "Kerberos"))
        {
            _eSubScheme = WINHTTP_AUTH_SCHEME_KERBEROS;            
            _dwSubFlags = PLUGIN_AUTH_FLAGS_KEEP_ALIVE_NOT_REQUIRED | PLUGIN_AUTH_FLAGS_NO_REALM;
        }

// BUGBUG - This faults.
//        

    }

    if (SecPkgCtxtInfo.PackageInfo)
    {
        (*(g_pSspData->pFuncTbl->FreeContextBuffer))(SecPkgCtxtInfo.PackageInfo);
    }
}


/*---------------------------------------------------------------------------
    Constructor
---------------------------------------------------------------------------*/
PLUG_CTX::PLUG_CTX(HTTP_REQUEST_HANDLE_OBJECT *pRequest, BOOL fIsProxy,
                 SPMData *pSPM, AUTH_CREDS* pCreds)
    : AUTHCTX(pSPM, pCreds)
{
    _fIsProxy = fIsProxy;
    _pRequest = pRequest;
    _szAlloc = NULL;
    _szData = NULL;
    _cbData = 0;
    _pRequest->SetAuthState(AUTHSTATE_NONE);
    _fNTLMProxyAuth = _fIsProxy && (GetSchemeType() == WINHTTP_AUTH_SCHEME_NTLM);

    _pszFQDN = NULL;

    _fChallengeSeen = FALSE;
}

/*---------------------------------------------------------------------------
    Destructor
---------------------------------------------------------------------------*/
PLUG_CTX::~PLUG_CTX()
{
    if (GetState() == AUTHCTX::STATE_LOADED)
    {
        if (_pCreds)
        {
            if (_CtxCriSec.Lock())
            {
                ClearAuthUser(&_pvContext, _pCreds->lpszHost);

                _CtxCriSec.Unlock();
            }
        }
    }
    if (_pRequest)
    {
        _pRequest->SetAuthState(AUTHSTATE_NONE);
    }

    if (_pszFQDN)
    {
        FREE_MEMORY(_pszFQDN);
    }
}


/*---------------------------------------------------------------------------
    PreAuthUser
---------------------------------------------------------------------------*/
DWORD PLUG_CTX::PreAuthUser(OUT LPSTR pBuf, IN OUT LPDWORD pcbBuf)
{
    if (!_CtxCriSec.Lock())
    {
        return ERROR_NOT_ENOUGH_MEMORY;
    }

    INET_ASSERT(_pSPMData == _pCreds->pSPM);

    DWORD dwError;
    SECURITY_STATUS ssResult;

    //  Make sure the auth provider is loaded.
    if (GetState() != AUTHCTX::STATE_LOADED)
    {
        if (GetState() != AUTHCTX::STATE_ERROR )
            Load();
        if (GetState() != AUTHCTX::STATE_LOADED)
        {
            dwError = ERROR_WINHTTP_INTERNAL_ERROR;
            goto exit;
        }
    }

    BOOL fCanUseLogon = _fIsProxy 
        || _pRequest->SilentLogonOK(_pCreds->lpszHost);

    LPSTR lpszFQDN = GetFQDN(_pCreds->lpszHost);
    LPSTR lpszHostName =  lpszFQDN ? lpszFQDN : _pCreds->lpszHost;

    __try
    {
        ssResult = SEC_E_INTERNAL_ERROR;
        dwError = PreAuthenticateUser(&_pvContext,
                               lpszHostName,
                               GetScheme(),
                               fCanUseLogon,
                               0, // dwFlags
                               pBuf,
                               pcbBuf,
                               _pCreds->lpszUser,
                               _pCreds->lpszPass,
                               &ssResult);


        // Transit to the correct auth state.
        if (ssResult == SEC_E_OK || ssResult == SEC_I_CONTINUE_NEEDED)
        {
            if (GetSchemeType() == WINHTTP_AUTH_SCHEME_NEGOTIATE)
                ResolveProtocol();

            // Kerberos + SEC_E_OK or SEC_I_CONTINUE_NEEDED transits to challenge.
            // Negotiate does not transit to challenge.
            // Any other protocol + SEC_E_OK only transits to challenge.
            if ((GetSchemeType() == WINHTTP_AUTH_SCHEME_KERBEROS
                && (ssResult == SEC_E_OK || ssResult == SEC_I_CONTINUE_NEEDED))
                || (GetSchemeType() != WINHTTP_AUTH_SCHEME_NEGOTIATE && ssResult == SEC_E_OK))
            {
                _pRequest->SetAuthState(AUTHSTATE_CHALLENGE);
            }        
        }
    }

    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        DEBUG_PRINT (HTTP, ERROR, ("preAuthenticateUser call down faulted\n"));
        _pSPMData->eState = STATE_ERROR;
        dwError = ERROR_WINHTTP_INTERNAL_ERROR;
    }

    ENDEXCEPT

exit:
    _CtxCriSec.Unlock();
    return dwError;
}


/*---------------------------------------------------------------------------
    UpdateFromHeaders
---------------------------------------------------------------------------*/
DWORD PLUG_CTX::UpdateFromHeaders(HTTP_REQUEST_HANDLE_OBJECT *pRequest, BOOL fIsProxy)
{
    DWORD dwError, cbExtra = 0, dwAuthIdx;
    LPSTR szAuthHeader = NULL, szExtra = NULL, szScheme;

    // Get the auth header index corresponding to the scheme of this ctx.
    if ((dwError = FindHdrIdxFromScheme(&dwAuthIdx)) != ERROR_SUCCESS)
        goto quit;

    // Get the scheme and any extra data.
    if ((dwError = CrackAuthenticationHeader(pRequest, fIsProxy, dwAuthIdx,
        &szAuthHeader, &szExtra, &cbExtra, &szScheme)) != ERROR_SUCCESS)
        goto quit;
    
    if (!cbExtra)
    {
        if (_fChallengeSeen)
        {
            dwError = ERROR_HTTP_HEADER_NOT_FOUND;
            goto quit;
        }

        _pRequest->SetAuthState(AUTHSTATE_NEGOTIATE);
    }

    // Check if auth scheme requires keep-alive.
    if (!(GetFlags() & PLUGIN_AUTH_FLAGS_KEEP_ALIVE_NOT_REQUIRED))
    {
        // if in negotiate phase check if we are going via proxy.
        if (pRequest->GetAuthState() == AUTHSTATE_NEGOTIATE)
        {
            // BUGBUG: if via proxy, we are not going to get keep-alive
            // connection to the server.  It would be nice if we knew
            // a priori the whether proxy would allow us to tunnel to
            // http port on the server.  Otherwise if we try and fail,
            // we look bad vs. other browsers who are ignorant of ntlm
            // and fall back to basic.
            CHAR szBuffer[64];
            DWORD dwBufferLength = sizeof(szBuffer);
            DWORD dwIndex = 0;
            BOOL fSessionBasedAuth = FALSE;
            if (pRequest->QueryResponseHeader(HTTP_QUERY_PROXY_SUPPORT, 
                                          szBuffer, &dwBufferLength, 
                                          0, &dwIndex) == ERROR_SUCCESS)
            {
                if (!_stricmp(szBuffer, "Session-Based-Authentication"))
                {
                    fSessionBasedAuth = TRUE;
                }
            }
            if (!fIsProxy && pRequest->IsRequestUsingProxy()
                && !pRequest->IsTalkingToSecureServerViaProxy() && !fSessionBasedAuth)
            {
                // Ignore NTLM via proxy since we won't get k-a to server.
                dwError = ERROR_HTTP_HEADER_NOT_FOUND;
                goto quit;
            }
        }

        // Else if in challenge phase, we require a persistent connection.
        else
        {
            // If we don't have a keep-alive connection ...
            if (!(pRequest->IsPersistentConnection (fIsProxy)))
            {
                dwError = ERROR_HTTP_HEADER_NOT_FOUND;
                goto quit;
            }
        }

    } // end if keep-alive required

    _fChallengeSeen = TRUE;

    quit:

    if (dwError == ERROR_SUCCESS)
    {
        // If no password cache is set in the auth context,
        // find or create one and set it in the handle.
        if (!_pCreds)
        {
            _pCreds = CreateCreds(pRequest, fIsProxy, _pSPMData, NULL);

            if (!_pCreds)
            {
                INET_ASSERT(FALSE);
                dwError = ERROR_WINHTTP_INTERNAL_ERROR;
            }
            else
            {
                INET_ASSERT(_pCreds->pSPM == _pSPMData);
            }
        }
    }

    if (dwError == ERROR_SUCCESS)
    {
        // Point to allocated data.
        _szAlloc = szAuthHeader;
        _szData = szExtra;
        _cbData = cbExtra;
    }
    else
    {
        // Free allocated data.
        if (_szAlloc)
            delete _szAlloc;
        _szAlloc = NULL;
        _szData = NULL;
        _cbData = 0;
    }

    // Return of non-success will cancel auth session.
    return dwError;
}



/*---------------------------------------------------------------------------
    PostAuthUser
---------------------------------------------------------------------------*/
DWORD PLUG_CTX::PostAuthUser()
{
    if (!_CtxCriSec.Lock())
    {
        return ERROR_NOT_ENOUGH_MEMORY;
    }

    INET_ASSERT(_pSPMData == _pCreds->pSPM);
    DWORD dwError;

    //  Make sure the auth provider is loaded.
    if (GetState() != AUTHCTX::STATE_LOADED)
    {
        if (GetState() != AUTHCTX::STATE_ERROR )
            Load();
        if (GetState() != AUTHCTX::STATE_LOADED)
        {
            dwError = ERROR_WINHTTP_INTERNAL_ERROR;
            goto exit;
        }
    }

    BOOL fCanUseLogon = _fIsProxy 
        || _pRequest->SilentLogonOK(_pCreds->lpszHost);

    if (_fDisableAutoLogon)
    {
        fCanUseLogon = FALSE;
    }

    LPSTR lpszFQDN = GetFQDN(_pCreds->lpszHost);
    LPSTR lpszHostName = lpszFQDN ? lpszFQDN : _pCreds->lpszHost;

    SECURITY_STATUS ssResult;
    __try
    {
        ssResult = SEC_E_INTERNAL_ERROR;
        dwError = AuthenticateUser(&_pvContext,
                                   lpszHostName,
                                   GetScheme(),
                                   fCanUseLogon,
                                   _szData,
                                   _cbData,
                                   _pCreds->lpszUser,
                                   _pCreds->lpszPass,
                                   &ssResult);



        // Kerberos package can get into a bad state.
        if (GetSchemeType() == WINHTTP_AUTH_SCHEME_KERBEROS && ssResult == SEC_E_WRONG_PRINCIPAL)
            dwError = ERROR_WINHTTP_INCORRECT_PASSWORD;
            
        // Transit to the correct auth state.
        if (ssResult == SEC_E_OK || ssResult == SEC_I_CONTINUE_NEEDED)
        {
            if (GetSchemeType() == WINHTTP_AUTH_SCHEME_NEGOTIATE)
                ResolveProtocol();

            // Kerberos + SEC_E_OK or SEC_I_CONTINUE_NEEDED transits to challenge.
            // Negotiate does not transit to challenge.
            // Any other protocol + SEC_E_OK only transits to challenge.
            if ((GetSchemeType() == WINHTTP_AUTH_SCHEME_KERBEROS
                && (ssResult == SEC_E_OK || ssResult == SEC_I_CONTINUE_NEEDED))
                || (GetSchemeType() != WINHTTP_AUTH_SCHEME_NEGOTIATE && ssResult == SEC_E_OK))
            {
                _pRequest->SetAuthState(AUTHSTATE_CHALLENGE);
            }        
        }
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        DEBUG_PRINT (HTTP, ERROR, ("AuthenticateUser faulted!\n"));
        dwError = ERROR_BAD_FORMAT;
        _pSPMData->eState = STATE_ERROR;
    }
    ENDEXCEPT

    if (_szAlloc)
    {
        delete _szAlloc;
        _szAlloc = NULL;
        _szData = NULL;
    }

    _cbData = 0;

exit:
    _CtxCriSec.Unlock();
    return dwError;
}

LPSTR PLUG_CTX::GetFQDN(LPSTR lpszHostName)
{
    if (lstrcmpi(GetScheme(), "Negotiate")) // only need to get FQDN for Kerberos
    {
        return NULL;
    }

    if (_pszFQDN)
    {
        return _pszFQDN;
    }

    SERIALIZED_LIST* pResolverCache = GetRootHandle(_pRequest)->GetResolverCache()->GetResolverCacheList();
    LPADDRINFO lpAddrInfo;
    DWORD TTL;
    LPRESOLVER_CACHE_ENTRY lpResolverCacheEntry;
    if ((lpResolverCacheEntry=QueryResolverCache(pResolverCache,
                                            (LPSTR)lpszHostName, 
                                            NULL, 
                                            &lpAddrInfo, 
                                            &TTL)) != NULL) 
    {
        _pszFQDN = (lpAddrInfo->ai_canonname ? NewString(lpAddrInfo->ai_canonname) : NULL);
        ReleaseResolverCacheEntry(pResolverCache, lpResolverCacheEntry);
        return _pszFQDN;
    }

    return NULL;
}
