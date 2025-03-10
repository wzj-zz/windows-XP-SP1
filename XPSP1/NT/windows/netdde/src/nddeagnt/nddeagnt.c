/* $Header: "%n;%v  %f  LastEdit=%w  Locker=%l" */
/* "NDDEAGNT.C;3  9-Feb-93,20:14:14  LastEdit=IGOR  Locker=IGOR" */
/************************************************************************
* Copyright (c) Wonderware Software Development Corp. 1991-1992.        *
*               All Rights Reserved.                                    *
*************************************************************************/
/* $History: Begin

    NDDEAGNT.C

    NetDDE Agent for Access to Apps in User Space

    Revisions:
    12-92   IgorM.  Wonderware initial create.
     4-93   IgorM.  Wonderware modify to use Trust Share API. Add Init verify.
    10-93   SanfordsS   Auto starting and linkup to Services added.

   $History: End */


#include <windows.h>
#include <dde.h>
#include <stdio.h>
#include <string.h>
#include "netbasic.h"
#include "ddepkts.h"
#include "netddesh.h"
#include "debug.h"
#include "nddeagnt.h"
#include "nddeagnp.h"
#include "nddeapi.h"
#include "shrtrust.h"
#include "proflspt.h"
#include "immp.h"

#define WAIT_FOR_NETDDE_TIMEOUT (10 * 1000) // 10 seconds allowed to start NetDDE

TCHAR   gszAppName[]            = SZ_NDDEAGNT_SERVICE;
TCHAR   szTitle[]               = SZ_NDDEAGNT_TITLE;
TCHAR   tmpBuf[128];
TCHAR   szComputerName[ 100 ];
DWORD   cbName                  = sizeof(szComputerName);
TCHAR   szServerName[ 100 ]     = "[none]";
LPTSTR  lpszServer              = szServerName;
TCHAR   szNetDDE[]              = NETDDE_TITLE;
TCHAR   szNetDDEDSDM[]          = TEXT("NetDDEDSDM");
TCHAR   szClipSrv[]             = TEXT("ClipSrv");

TCHAR   szStartServices[]       = SZ_NDDEAGNT_TOPIC;
TCHAR   szAgentExecRtn[]        = "NetddeAgentExecRtn";
UINT    wMsgNddeAgntExecRtn;
TCHAR   szAgentWakeUp[]         = "NetddeAgentWakeUp";
UINT    wMsgNddeAgntWakeUp;
TCHAR   szAgentAlive[]          = "NetddeAgentAlive";
UINT    wMsgNddeAgntAlive;
TCHAR   szAgentDying[]          = "NetddeAgentDying";
UINT    wMsgNddeAgntDying;

HWND    hWndNddeAgnt;
HANDLE  hSemNddeAgnt            = NULL;
HANDLE  hInst;
BOOL    fStartFailed            = 0;
SC_HANDLE hSvcNetDDE            = NULL;
SC_HANDLE hSvcNetDDEDSDM        = NULL;
SC_HANDLE hSvcClipSrv           = NULL;

#if DBG
BOOL    bDebugInfo              = FALSE;
BOOL    bDebugErrors            = FALSE;
#endif

BOOL IsServiceActive(SC_HANDLE *phSvc, LPTSTR pszSvcName, BOOL fCleanup);
BOOL CompareNddeModifyId(LPTSTR lpszShareName, LPDWORD lpdwIdNdde,
        LPDWORD lpdwOptions);


LPSTR MyLoadString(
int id)
{
    /*
     * Assumes ids start at 1 and are consecutive.
     */
    static LPSTR aStrings[STR_LAST - 1] = { NULL };
    CHAR szBuf[200];
    int length;

    if (aStrings[id - 1] == NULL) {
        length = LoadString(hInst, id, szBuf, sizeof(szBuf));
        if (length == 0) {
            return("");
        }
        aStrings[id - 1] = LocalAlloc(LPTR, length + 1);
        if (aStrings[id - 1] == NULL) {
            return("");
        }
        strcpy(aStrings[id - 1], szBuf);
    }
    return(aStrings[id - 1]);
}


/****************************************************************************

    FUNCTION: WinMain(HINSTANCE, HINSTANCE, LPTSTR, INT)

    PURPOSE: calls initialization function, processes message loop

****************************************************************************/

int WINAPI
WinMain(
    HINSTANCE  hInstance,
    HINSTANCE  hPrevInstance,
    LPTSTR     lpCmdLine,
    INT        nCmdShow)
{
    HWND hWnd;
    MSG msg;

#if DBG
    DebugInit( "NddeAgnt" );
    bDebugInfo = MyGetPrivateProfileInt("NddeAgnt",
        "DebugInfo", FALSE, "netdde.ini");
    bDebugErrors = MyGetPrivateProfileInt("NddeAgnt",
        "DebugErrors", FALSE, "netdde.ini");
#endif
    hSemNddeAgnt = CreateSemaphore(NULL, 1, 1, "NDDEAgent");
    if (hSemNddeAgnt != NULL && GetLastError() == ERROR_ALREADY_EXISTS) {
        CloseHandle(hSemNddeAgnt);
        MessageBox( NULL, MyLoadString(STR_RUNNING), gszAppName,
                    MB_TASKMODAL | MB_OK | MB_ICONEXCLAMATION );
        return (FALSE);
    }

    if (!NDDEAgntInit(hInstance)) {
        MessageBox( NULL, MyLoadString(STR_FAILED), gszAppName,
                    MB_TASKMODAL | MB_OK | MB_ICONEXCLAMATION );
        return (FALSE);
    }
    hInst = hInstance;

    hWnd = CreateWindowEx(WS_EX_TOPMOST, gszAppName,
        szTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL,
        NULL,
        hInstance,
        NULL);

    if (!hWnd)
        return (FALSE);

    hWndNddeAgnt = hWnd;
#ifdef HASUI
    ShowWindow(hWnd, SW_MINIMIZE);
    UpdateWindow(hWnd);
#endif

    /* set up lpszServer for NDDEAPI calls */
    GetComputerName( szComputerName, &cbName );
    strcpy( lpszServer, "\\\\" );
    strcat( lpszServer, szComputerName );

    /*
     * clean up any trusted shares that have been modified or deleted
     * since we last ran for this user
     */
    if (IsServiceActive(&hSvcNetDDE, szNetDDE, FALSE)) {
        CleanupTrustedShares();
    }

    while (GetMessage(&msg, 0, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (msg.wParam);
}


#define IMMMODULENAME L"IMM32.DLL"
#define PATHDLM     L'\\'
#define IMMMODULENAMELEN    ((sizeof PATHDLM + sizeof IMMMODULENAME) / sizeof(WCHAR))

VOID GetImmFileName(PWSTR wszImmFile)
{
    UINT i = GetSystemDirectoryW(wszImmFile, MAX_PATH);
    if (i > 0 && i < MAX_PATH - IMMMODULENAMELEN) {
        wszImmFile += i;
        if (wszImmFile[-1] != PATHDLM) {
            *wszImmFile++ = PATHDLM;
        }
    }
    wcscpy(wszImmFile, IMMMODULENAME);
}

/****************************************************************************

    FUNCTION: NDDEAgntInit(HANDLE)

    PURPOSE: Initializes window data and registers window class

****************************************************************************/

INT NDDEAgntInit(HANDLE hInstance)
{
    HANDLE hMemory;
    PWNDCLASS pWndClass;
    BOOL bSuccess;

    wMsgNddeAgntExecRtn = RegisterWindowMessage( szAgentExecRtn );
    wMsgNddeAgntWakeUp = RegisterWindowMessage( szAgentWakeUp );
    wMsgNddeAgntAlive = RegisterWindowMessage( szAgentAlive );
    wMsgNddeAgntDying = RegisterWindowMessage( szAgentDying );

#ifndef LATER
#define IMM_DISABLE_IME_NAME    L"ImmDisableIme"
#else
#define IMM_DISABLE_IME_NAME    L"ImmDisableIME"
#endif
    {
        WCHAR wszImmFile[MAX_PATH];
        BOOL (WINAPI* fpImmDisableIme)(DWORD);
        HMODULE hImm;

        GetImmFileName(wszImmFile);
        hImm = GetModuleHandleW(wszImmFile);
        if (hImm) {
            fpImmDisableIme = (BOOL (WINAPI*)(DWORD))GetProcAddress(hImm, "ImmDisableIme");
            if (fpImmDisableIme) {
#ifndef LATER
                fpImmDisableIme(0);
#else
                fpImmDisableIme(-1);
#endif
            }
        }
    }

    hMemory = LocalAlloc(LPTR, sizeof(WNDCLASS));
    if(!hMemory){
        MessageBox(NULL, MyLoadString(STR_LOWMEM), NULL, MB_OK | MB_ICONHAND);
        return(FALSE);
    }

    pWndClass = (PWNDCLASS) LocalLock(hMemory);
    pWndClass->hCursor = NULL;
    pWndClass->hIcon = NULL;
    pWndClass->lpszMenuName = NULL;
    pWndClass->lpszClassName = (LPTSTR)SZ_NDDEAGNT_CLASS;
    pWndClass->hbrBackground = GetStockObject(WHITE_BRUSH);
    pWndClass->hInstance = hInstance;
    pWndClass->style = 0;
    pWndClass->lpfnWndProc = NDDEAgntWndProc;

    bSuccess = RegisterClass((LPWNDCLASS) pWndClass);

    LocalUnlock(hMemory);
    LocalFree(hMemory);
    return (bSuccess);
}




/*
 * Returns TRUE if anything went wrong or NetDDE service is active.
 */
BOOL IsServiceActive(
SC_HANDLE *phSvc,
LPTSTR pszSvcName,
BOOL fCleanup)
{
    DWORD cServices;
    DWORD iEnum                 = 0;
    SC_HANDLE hSvcMgr;
    ENUM_SERVICE_STATUS *pess  = NULL;
    DWORD cbData;
    LONG status;

    if (fCleanup) {
        if (*phSvc != NULL) {
            CloseServiceHandle(*phSvc);
            *phSvc = NULL;
        }
        return(TRUE);   // fSuccess, NOT fActive
    }
    if (*phSvc != NULL) {
        SERVICE_STATUS SvcStatus;
        /*
         * FAST method:
         */
        if (QueryServiceStatus(*phSvc, &SvcStatus)) {
            return(SvcStatus.dwCurrentState == SERVICE_RUNNING);
        } else {
            return(FALSE);
        }
    }
    hSvcMgr = OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE);
    if (hSvcMgr) {
        cbData = 0;
        if (EnumServicesStatus(hSvcMgr, SERVICE_WIN32, SERVICE_ACTIVE,
                NULL, 0, &cbData, &cServices, &iEnum)) {
            /*
             * Success with cbData==0 implies no services are running
             * so return FALSE.
             */
            return(FALSE);
        }
        status = GetLastError();
        if (status != ERROR_MORE_DATA) {
            /*
             * Not what we expected.
             */
            return(TRUE);
        }
        pess = LocalAlloc(LPTR, cbData);
        if (pess == NULL) {
            CloseServiceHandle(hSvcMgr);
            return(TRUE);
        }
        iEnum = 0;
        if (!EnumServicesStatus(hSvcMgr, SERVICE_WIN32, SERVICE_ACTIVE,
                pess, cbData, &cbData, &cServices, &iEnum)) {
#if DBG
            GetLastError();
#endif
            LocalFree(pess);
            CloseServiceHandle(hSvcMgr);
            return(TRUE);
        }
        if (cServices > 0) {
            while (cServices--) {
                if (!_stricmp(pess[cServices].lpServiceName, pszSvcName)) {
                    /*
                     * Cache Service handle
                     */
                    *phSvc = OpenService(hSvcMgr, pszSvcName,
                            SERVICE_QUERY_STATUS);
                    LocalFree(pess);
                    CloseServiceHandle(hSvcMgr);
                    return(TRUE);
                }
            }
        }
        LocalFree(pess);
        CloseServiceHandle(hSvcMgr);
    }
    return(FALSE);
}


BOOL StartNetDDEServices(
BOOL fNotifyUser)
{
    SC_HANDLE hSvcMgr, hSvc;
    BOOL bOk = TRUE;
    HWND hwndDlg = NULL;

    if (fNotifyUser) {
        hwndDlg = CreateDialog(hInst, MAKEINTRESOURCE(IDD_STARTING), NULL, NULL);
        if (hwndDlg != NULL) {
            RECT rc;
            MSG msg;

            GetWindowRect(hwndDlg, &rc);
            SetWindowPos(hwndDlg, HWND_TOPMOST,
                    (GetSystemMetrics(SM_CXFULLSCREEN) - (rc.right - rc.left)) / 2,
                    (GetSystemMetrics(SM_CYFULLSCREEN) - (rc.bottom - rc.top)) / 2,
                    0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
            // SetForegroundWindow(hwndDlg);

            /*
             * Process dialog messages so it can paint, etc.
             */
            while (PeekMessage(&msg, hwndDlg, 0, 0, PM_REMOVE)) {
                DispatchMessage(&msg);
            }
        }
    }
    hSvcMgr = OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE);
    if (hSvcMgr) {
        /*
         * Because ClipSrv is dependent on NetDDE and NetDDE is dependent
         * on nddeDSDM, this call starts all three services.  In addition,
         * the service controller will not return from this call until
         * all dependent services are fully initialized so we are guarenteed
         * that NetDDE and nddeDSDM are ready to go by the time this returns.
         */
        hSvc = OpenService(hSvcMgr, "ClipSrv", SERVICE_START);
        if (hSvc) {
            StartService(hSvc, 0, NULL);
            CloseServiceHandle(hSvc);
        } else {
            bOk = FALSE;
        }
        CloseServiceHandle(hSvcMgr);
    } else {
        bOk = FALSE;
    }
    if (hwndDlg != NULL) {
        DestroyWindow(hwndDlg);
    }
    return(bOk);
}


HWND ConnectToNetDDEService(
    DWORD dwTimeout)
{
    HANDLE hPipe;
    NETDDE_PIPE_MESSAGE nameinfo;
    DWORD cbRead;
    DWORD tcStart;
    DWORD dwMode;
    MSG msg;

    /*
     * locate the NetDDE pipe .. give the system some time
     * to let the NetDDE service create it.
     */
    hPipe = CreateFileW(NETDDE_PIPE,
            GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL | SECURITY_SQOS_PRESENT |
            SECURITY_IMPERSONATION, NULL);
    if (hPipe == INVALID_HANDLE_VALUE && dwTimeout == 0) {
        return NULL;
    }

    tcStart = GetTickCount();
    while (hPipe == INVALID_HANDLE_VALUE &&
            ((tcStart + dwTimeout) >
            GetTickCount())) {

        /*
         * Wait this way so NetDDE can send us our wakeup call.
         */
        MsgWaitForMultipleObjects(0, NULL, TRUE, 1000, QS_ALLINPUT);
        hPipe = CreateFileW(NETDDE_PIPE,
                GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL | SECURITY_SQOS_PRESENT |
                SECURITY_IMPERSONATION, NULL);
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
            DispatchMessage(&msg);
    }
    if (hPipe == INVALID_HANDLE_VALUE) {
        return NULL;
    }

    /*
     * Package up the windowstation and desktop names and
     * send them to NetDDE.
     */
    wcscpy(nameinfo.awchNames, L"WinSta0");
    nameinfo.dwOffsetDesktop = wcslen(nameinfo.awchNames) + 1;
    wcscpy(&nameinfo.awchNames[nameinfo.dwOffsetDesktop],
            L"default");
    dwMode = PIPE_READMODE_MESSAGE | PIPE_WAIT;
    SetNamedPipeHandleState(hPipe, &dwMode, NULL, NULL);
    if (!TransactNamedPipe(hPipe, &nameinfo, sizeof(nameinfo),
            &hWndNddeAgnt, sizeof(HWND), &cbRead, NULL)) {
        hWndNddeAgnt = NULL;
    }
    CloseHandle(hPipe);

    return hWndNddeAgnt;
}


LONG
APIENTRY
NDDEAgntWndProc(
    HWND hWnd,         // window handle
    UINT message,      // type of message
    UINT uParam,       // additional information
    LONG lParam )      // additional information
{
    TCHAR szAppName[256];
    static BOOL fInitializing = FALSE;

    switch (message) {
    case WM_CREATE:
        DIPRINTF(("WM_CREATE: %0X", (UINT) hWnd));
        if (IsServiceActive(&hSvcNetDDE, szNetDDE, FALSE) &&
                (FindWindow(NETDDE_CLASS, NETDDE_TITLE) == NULL)) {

            /*
             * Connect to NetDDE and get the main window handle
             */
            ConnectToNetDDEService(0);
        }
        SendNotifyMessage( (HWND)-1, wMsgNddeAgntAlive, (UINT) hWnd, 0);
        break;

    case WM_DDE_INITIATE:
        TRACEINIT((szT, "NDDEAgntWndProc: WM_DDE_INITIATE"));
        if (GlobalGetAtomName(LOWORD(lParam), szAppName, sizeof(szAppName))) {
            HWND hwndNetDDEMain = NULL;

            if (_stricmp(szAppName, gszAppName) != 0) {
                if (strstr(szAppName, "NDDE$") == NULL &&
                    strstr(szAppName, "\\\\") == NULL) {
                    /*
                     * Not for NetDDE at all.  let it go.
                     */
                    TRACEINIT((szT, "NDDEAgntWndProc: WM_DDE_INITIATE - not for NetDDE"));
                    return(0);
                }
            } else {
                /*
                 * Its addressing us as a DDE service!
                 *
                 * We use the WM_DDE_INITIATE as a fast execute to this app.
                 * If the atoms are right, we will start the NetDDE services
                 * if needed.  This lets other components do this with
                 * one simple SendMessage() yet does not require new APIs.
                 */
                if (!GlobalGetAtomName(HIWORD(lParam), szAppName, sizeof(szAppName))) {
                    TRACEINIT((szT, "NDDEAgntWndProc: WM_DDE_INITIATE - Bad App Atom"));
                    return(0);
                }
                if (_stricmp(szAppName, szStartServices) != 0) {
                    TRACEINIT((szT, "NDDEAgntWndProc: WM_DDE_INITIATE - Not request to start services"));
                    return(0);
                }
            }

            /*
             * If we're already initializing, skip it.
             */
            if (fInitializing) {
                TRACEINIT((szT, "NDDEAgntWndProc: fInitializing==TRUE, blow it off"));
                return 0;
            }
            fInitializing = TRUE;
            if (IsServiceActive(&hSvcNetDDE, szNetDDE, FALSE) &&
                    IsServiceActive(&hSvcNetDDEDSDM, szNetDDEDSDM, FALSE) &&
                    IsServiceActive(&hSvcClipSrv, szClipSrv, FALSE)) {

                /*
                 * If there is no main window, connect to NetDDE
                 * and get the main window handle
                 */
                if (FindWindow(NETDDE_CLASS, NETDDE_TITLE) == NULL)
                    ConnectToNetDDEService(0);
            } else {

                /*
                 * Start the NetDDE service(s)
                 */
                if (fStartFailed == 0) {
                    if (!StartNetDDEServices(TRUE)) {
                        fStartFailed++;
                        MessageBox(NULL, MyLoadString(STR_CANTSTART), szTitle,
                                MB_OK | MB_SYSTEMMODAL);
                    }

                    /*
                     * Connect to NetDDE and get the main window handle
                     */
                    if (ConnectToNetDDEService(WAIT_FOR_NETDDE_TIMEOUT) != NULL) {

                        /*
                         * BUG:  If we pass on this message to netdde, things will
                         * always work the first time... but they may also be
                         * double connected because the app may happen to send
                         * another initiate message directly to the netdde
                         * service - depending on how it does its broadcast.
                         * There is no way to solve this for all cases so we
                         * don't pass it on.  Apps that want to work first time,
                         * everytime must use DDEML (its method of enumeration
                         * works here) or they must start the services themselves.
                         */
                        //return(SendMessage(hwndNetDDEMain, message, uParam, lParam));
                    }
                }
            }
            fInitializing = FALSE;
            return(0);
        }
        break;

    case WM_COPYDATA:
        DIPRINTF(("WM_COPYDATA: %0X %0X %0lX", (UINT) hWnd, uParam, lParam));
        HandleNetddeCopyData( hWnd, uParam, (PCOPYDATASTRUCT) lParam );
        return( TRUE ); // processed the msg */
        break;

    case WM_ENDSESSION:
        if( uParam == 0 )  {
            break;
        }
        /* intentional fall-through */
    case WM_DESTROY:  // message: window being destroyed
        DIPRINTF(("WM_DESTROY: %0X", (UINT) hWnd));
        SendNotifyMessage( (HWND)-1, wMsgNddeAgntDying,
                    (UINT) hWndNddeAgnt, 0);
        PostQuitMessage(0);
        IsServiceActive(&hSvcNetDDE, szNetDDE, TRUE);           // cleanup.
        IsServiceActive(&hSvcNetDDEDSDM, szNetDDEDSDM, TRUE);   // cleanup.
        IsServiceActive(&hSvcClipSrv, szClipSrv, TRUE);      // cleanup.

        break;

    default:          // Passes it on if unproccessed
        if (message == wMsgNddeAgntWakeUp) {
            TRACEINIT((szT, "WakeUp %x received.  Sending %x to %x.",
                    wMsgNddeAgntWakeUp,
                    wMsgNddeAgntAlive,
                    uParam));
            SendMessage( (HWND) uParam, wMsgNddeAgntAlive, (UINT) hWnd, 0);
            return( TRUE ); // processed the msg
        }
        return (DefWindowProc(hWnd, message, uParam, lParam));
    }
    return (0);
}



/*
 *  HandleNetddeCopyData()
 *
 *      This handles the WM_COPYDATA message from NetDDE to start an
 *      application in the user's context
 */
BOOL
HandleNetddeCopyData(
    HWND            hWndNddeAgnt,
    UINT            wParam,
    PCOPYDATASTRUCT pCopyDataStruct )
{
    PNDDEAGTCMD     pNddeAgtCmd;
    LPTSTR          lpszShareName;
    LPTSTR          lpszCmdLine;
    UINT            uAgntExecRtn = 0;
    UINT            uAgntInitRtn = 0;
    UINT            fuCmdShow = 0;
    DWORD           dwOptions;
    COPYDATASTRUCT  cds;
    BOOL            RetStatus = FALSE;

    /* sanity checks on the structure coming in */
    if( pCopyDataStruct->cbData < sizeof(NDDEAGTCMD) )  {
        return( FALSE );
    }
    pNddeAgtCmd = (PNDDEAGTCMD)(pCopyDataStruct->lpData);
    if( pNddeAgtCmd->dwMagic != NDDEAGT_CMD_MAGIC )  {
        return( FALSE );
    }
    if( pNddeAgtCmd->dwRev != 1 )  {
        return( FALSE );
    }

    /* passed the sanity checks ... lets's do the command */
    switch( pNddeAgtCmd->dwCmd )  {
    case NDDEAGT_CMD_WINEXEC:
        /* get the sharename and cmdline out of szData */
        lpszShareName = pNddeAgtCmd->szData;
        lpszCmdLine = lpszShareName + lstrlen(lpszShareName) + 1;

        DIPRINTF(("WinExec( %s:%s )", lpszShareName, lpszCmdLine));
        /*
         * make sure that no one changed the share since the user told
         * us he trusted it
         */
        if( CompareNddeModifyId( lpszShareName,
                &pNddeAgtCmd->qwModifyId[0], &dwOptions ) )  {
            /*
             * no one has modified the share ... start the app in the
             * user's context
             */
            if (dwOptions & NDDE_TRUST_SHARE_START) {
                fuCmdShow = pNddeAgtCmd->fuCmdShow;
                if( (dwOptions & NDDE_TRUST_CMD_SHOW) )  {
                    fuCmdShow = dwOptions & NDDE_CMD_SHOW_MASK;
                }
                uAgntExecRtn = WinExec( lpszCmdLine, fuCmdShow );
#if DBG
                if (uAgntExecRtn > 31) {
                    DIPRINTF(("WinExec successful, returns: %d", uAgntExecRtn));
                } else {
                    if (bDebugErrors) {
                        DPRINTF(("WinExec(%s:%d) failed, returns: %d",
                            lpszCmdLine, fuCmdShow, uAgntExecRtn));
                    }
                }
#endif
            } else {
#if DBG
                if (bDebugErrors) {
                    DPRINTF(("WinExec() Start app not allowed"));
                }
#endif
                uAgntExecRtn = NDDEAGT_START_NO;
            }
        } else {
#if DBG
            if (bDebugErrors) {
                DPRINTF(("WinExec() CompareNddeModifyId failed"));
            }
#endif
            uAgntExecRtn = NDDEAGT_START_NO;
        }
        RetStatus = TRUE;
        break;

    case NDDEAGT_CMD_WININIT:
        /* get the sharename and cmdline out of szData */
        lpszShareName = pNddeAgtCmd->szData;

        DIPRINTF(("WinInit( %s )", lpszShareName));
        /* make sure that no one changed the share since the user told
            us he trusted it */
        if( CompareNddeModifyId( lpszShareName,
                &pNddeAgtCmd->qwModifyId[0], &dwOptions ) )  {

            /* no one has modified the share ... start the app in the
                user's context */
            if (dwOptions & NDDE_TRUST_SHARE_INIT) {
                uAgntExecRtn = NDDEAGT_INIT_OK;
#if DBG
                if (bDebugInfo) {
                    DPRINTF(("WinInit() Init allowed."));
                }
#endif
            } else {
                uAgntExecRtn = NDDEAGT_INIT_NO;
#if DBG
                if (bDebugErrors) {
                    DPRINTF(("WinInit() Init not allowed."));
                }
#endif
            }
        } else {
#if DBG
            if (bDebugErrors) {
                DPRINTF(("WinInit() CompareNddeModifyId failed"));
            }
#endif
            uAgntExecRtn = NDDEAGT_INIT_NO;
        }

        RetStatus = TRUE;
        break;

    default:
        RetStatus = FALSE;
        break;
    }
    if (RetStatus) {
        /* send a COPYDATA message back to NetDDE */
        cds.dwData = wMsgNddeAgntExecRtn;
        cds.cbData = sizeof(uAgntExecRtn);
        cds.lpData = &uAgntExecRtn;
        SendMessage( (HWND)wParam, WM_COPYDATA, (UINT) hWndNddeAgnt,
            (LPARAM) &cds );
    }
    return( RetStatus );
}


/*
 *  Given a share name, GetNddeShareModifyId() will retrieve the modify id
 *  associated with the DSDM share
 */
BOOL
GetNddeShareModifyId(
    LPTSTR  lpszShareName,
    LPDWORD lpdwId )
{
    PNDDESHAREINFO      lpDdeI = NULL;
    DWORD               avail = 0;
    WORD                items = 0;
    UINT                nRet;
    BOOL                bRetrieved = FALSE;

    DIPRINTF(("GetNddeShareModifyId( %s ): %d", lpszShareName, lpdwId ));
    /* get the share information out of the DSDM DB */
    nRet = NDdeShareGetInfo ( lpszServer, lpszShareName, 2, (LPBYTE)NULL,
        0, &avail, &items );
    if (nRet == NDDE_BUF_TOO_SMALL) {
        lpDdeI = (PNDDESHAREINFO) LocalAlloc(LMEM_FIXED, avail);
        if (lpDdeI == NULL) {
#if DBG
            if (bDebugErrors) {
                DPRINTF(("Unable to allocate sufficient (%d) memory: %d",
                    avail, GetLastError()));
            }
#endif
            bRetrieved = FALSE;
        } else {
            items = 0;
            nRet = NDdeShareGetInfo ( lpszServer, lpszShareName, 2, (LPBYTE)lpDdeI,
                avail, &avail, &items );

            if( nRet == NDDE_NO_ERROR )  {
                /* compare modify ids */
                bRetrieved = TRUE;
                lpdwId[0] = lpDdeI->qModifyId[0];
                lpdwId[1] = lpDdeI->qModifyId[1];
            } else {
#if DBG
                if (bDebugErrors) {
                    DPRINTF(("Unable to access DDE share \"%s\" info: %d",
                        lpszShareName, nRet));
                }
#endif
                bRetrieved = FALSE;
            }
            LocalFree(lpDdeI);
        }
    } else {
#if DBG
        if (bDebugErrors) {
            DPRINTF(("Unable to probe DDE share \"%s\" info size: %d",
                lpszShareName, nRet));
        }
#endif
        bRetrieved = FALSE;
    }
    return( bRetrieved );
}

BOOL
CompareModifyIds( LPTSTR lpszShareName )
{
    DWORD       dwIdNdde[2];
    DWORD       dwIdTrusted[2];
    DWORD       dwOptions;
    UINT        RetCode;
    BOOL        bRetrievedNdde;

    DIPRINTF(("ComaperModifyIds(%s)", lpszShareName));
    bRetrievedNdde = GetNddeShareModifyId( lpszShareName, &dwIdNdde[0] );
    if (!bRetrievedNdde) {
        return(FALSE);
    }
    RetCode = NDdeGetTrustedShare( lpszServer,lpszShareName,
        &dwOptions, &dwIdTrusted[0], &dwIdTrusted[1] );
    if (RetCode != NDDE_NO_ERROR) {
#if DBG
        if (bDebugErrors) {
            DPRINTF(("Error getting trusted share \"%s\" modify id: %d",
                lpszShareName, RetCode));
        }
#endif
        return(FALSE);
    }
    DIPRINTF(("CompareModifyIds() returns: NddeId: %08X%08X, TrustedId: %08X%08X",
                dwIdNdde[0], dwIdNdde[1], dwIdTrusted[0], dwIdTrusted[1]));
    if( (dwIdNdde[0] == dwIdTrusted[0])
        && (dwIdNdde[1] == dwIdTrusted[1]) )  {
        return(TRUE);
    } else {
        return(FALSE);
    }
}





/*
 *  CleanupTrustedShares() goes through all the truested shares for this user
 *  on this machine and makes certain that no one has modified the shares
 *  since the time the user said they were ok.
 */
VOID
CleanupTrustedShares( void )
{
    UINT                RetCode;
    DWORD               avail, entries;
    LPBYTE              lpBuf;
    LPTSTR              lpShareName;

    RetCode = NDdeTrustedShareEnum ( lpszServer, 0, (LPBYTE)&avail, 0, &entries, &avail );
    if (RetCode != NDDE_BUF_TOO_SMALL) {
#if DBG
        if (bDebugErrors) {
            DPRINTF(("Probing for Number of Trusted Shares Failed: %d", RetCode));
        }
#endif
        return;
    }
    lpBuf = LocalAlloc(LPTR, avail);
    if (lpBuf == NULL) {
#if DBG
        if (bDebugErrors) {
            DPRINTF(("Unable to allocate sufficient memory to enumerate trusted shares. Needed: %d", avail));
        }
#endif
        return;
    }
    RetCode = NDdeTrustedShareEnum ( lpszServer, 0, lpBuf, avail,
            &entries, &avail );
    if (RetCode != NDDE_NO_ERROR) {
#if DBG
        if (bDebugErrors) {
            DPRINTF(("Unable to enumerate trusted shares: %d", RetCode));
        }
#endif
        LocalFree(lpBuf);
        return;
    }
    for ( lpShareName = (LPTSTR)lpBuf; *lpShareName;
            lpShareName += lstrlen(lpShareName) + 1 ) {
        if( !CompareModifyIds( lpShareName ) )  {
            /* if they don't match exactly ... get rid of it */
            RetCode = NDdeSetTrustedShare( lpszServer, lpShareName, 0);   /* delete option */
#if DBG
            if (RetCode != NDDE_NO_ERROR) {
                if (bDebugErrors) {
                    DPRINTF(("Unable to delete obsolete trusted share \"%s\": %d",
                        lpShareName, RetCode));
                }
            }
#endif
        }
    }
    LocalFree(lpBuf);
    return;
}

/*
 *  CompareNddeModifyId() takes in the computer name, sharename and
 *  the modify id from a NetDDE share and looks up the sharename in
 *  the truested share db and verifies the modify ids are the same.
 */
BOOL
CompareNddeModifyId(
    LPTSTR      lpszShareName,
    LPDWORD     lpdwIdNdde,
    LPDWORD     lpdwOptions )
{
    UINT        RetCode;
    DWORD       dwIdTrusted[2]  = {0, 0};
    BOOL        bMatch = FALSE;

    RetCode = NDdeGetTrustedShare(lpszServer, lpszShareName,
        lpdwOptions, &dwIdTrusted[0], &dwIdTrusted[1]);
    if (RetCode != NDDE_NO_ERROR) {
#if DBG
        if (bDebugErrors) {
            DPRINTF(("Unable to access trusted share \"%s\" info: %d",
                lpszShareName, RetCode));
        }
#endif
        return(FALSE);
    }

    DIPRINTF(("CompareNddeModifyId() returns: NddeId: %08X%08X, TrustedId: %08X%08X",
            lpdwIdNdde[0], lpdwIdNdde[1],
            dwIdTrusted[0], dwIdTrusted[1]));

    if( (lpdwIdNdde[0] == dwIdTrusted[0])
        && (lpdwIdNdde[1] == dwIdTrusted[1]) )  {
            bMatch = TRUE;
    }

    return( bMatch );
}
