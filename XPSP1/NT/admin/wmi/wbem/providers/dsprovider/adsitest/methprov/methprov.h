//***************************************************************************

//

//  methprov.h

//

//  Module: WBEM Method Test Provider code

//

//  Purpose: Method Provider Test Header File.

//

// Copyright (c) 1998-2001 Microsoft Corporation, All Rights Reserved
//
//***************************************************************************

#ifndef _methprov_H_
#define _methprov_H_

#include "wbemidl.h"

typedef LPVOID * PPVOID;

// Provider interfaces are provided by objects of this class
 
class CMethodPro : public IWbemServices, public IWbemProviderInit
    {
    protected:
        ULONG              m_cRef;         //Object reference count
        IWbemServices *  m_pWbemSvcs;
        IWbemClassObject* m_pOutClass;
     public:
        CMethodPro();
        ~CMethodPro(void);

        //Non-delegating object IUnknown

        STDMETHODIMP         QueryInterface(REFIID, PPVOID);
        STDMETHODIMP_(ULONG) AddRef(void);
        STDMETHODIMP_(ULONG) Release(void);

        STDMETHOD(GetTypeInfoCount)(THIS_ UINT FAR* pctinfo){return WBEM_E_NOT_SUPPORTED;};

        STDMETHOD(GetTypeInfo)(
           THIS_
           UINT itinfo,
           LCID lcid,
           ITypeInfo FAR* FAR* pptinfo){return WBEM_E_NOT_SUPPORTED;};

        STDMETHOD(GetIDsOfNames)(
          THIS_
          REFIID riid,
          OLECHAR FAR* FAR* rgszNames,
          UINT cNames,
          LCID lcid,
          DISPID FAR* rgdispid){return WBEM_E_NOT_SUPPORTED;};

        STDMETHOD(Invoke)(
          THIS_
          DISPID dispidMember,
          REFIID riid,
          LCID lcid,
          WORD wFlags,
          DISPPARAMS FAR* pdispparams,
          VARIANT FAR* pvarResult,
          EXCEPINFO FAR* pexcepinfo,
          UINT FAR* puArgErr){return WBEM_E_NOT_SUPPORTED;};


        SCODE GetByPath( BSTR Path, IWbemClassObject FAR* FAR* pObj, IWbemContext  *pCtx);

        //IWbemProviderInit

        HRESULT STDMETHODCALLTYPE Initialize(
             /* [in] */ LPWSTR pszUser,
             /* [in] */ LONG lFlags,
             /* [in] */ LPWSTR pszNamespace,
             /* [in] */ LPWSTR pszLocale,
             /* [in] */ IWbemServices *pNamespace,
             /* [in] */ IWbemContext *pCtx,
             /* [in] */ IWbemProviderInitSink *pInitSink
                        );

        //IWbemServices  

		HRESULT STDMETHODCALLTYPE OpenNamespace( 
            /* [in] */ BSTR Namespace,
            /* [in] */ long lFlags,
            /* [in] */ IWbemContext __RPC_FAR *pCtx,
            /* [unique][in][out] */ IWbemServices __RPC_FAR *__RPC_FAR *ppWorkingNamespace,
            /* [unique][in][out] */ IWbemCallResult __RPC_FAR *__RPC_FAR *ppResult) {return WBEM_E_NOT_SUPPORTED;};
        
        HRESULT STDMETHODCALLTYPE CancelAsyncCall( 
            /* [in] */ IWbemObjectSink __RPC_FAR *pSink) {return WBEM_E_NOT_SUPPORTED;};
        
        HRESULT STDMETHODCALLTYPE QueryObjectSink( 
            /* [in] */ long lFlags,
            /* [out] */ IWbemObjectSink __RPC_FAR *__RPC_FAR *ppResponseHandler) {return WBEM_E_NOT_SUPPORTED;};
        
        HRESULT STDMETHODCALLTYPE GetObject( 
            /* [in] */ BSTR ObjectPath,
            /* [in] */ long lFlags,
            /* [in] */ IWbemContext __RPC_FAR *pCtx,
            /* [unique][in][out] */ IWbemClassObject __RPC_FAR *__RPC_FAR *ppObject,
            /* [unique][in][out] */ IWbemCallResult __RPC_FAR *__RPC_FAR *ppCallResult) {return WBEM_E_NOT_SUPPORTED;};
        
        HRESULT STDMETHODCALLTYPE GetObjectAsync( 
            /* [in] */ BSTR ObjectPath,
            /* [in] */ long lFlags,
            /* [in] */ IWbemContext __RPC_FAR *pCtx,
            /* [in] */ IWbemObjectSink __RPC_FAR *pResponseHandler){return WBEM_E_NOT_SUPPORTED;}
        
        HRESULT STDMETHODCALLTYPE PutClass( 
            /* [in] */ IWbemClassObject __RPC_FAR *pObject,
            /* [in] */ long lFlags,
            /* [in] */ IWbemContext __RPC_FAR *pCtx,
            /* [unique][in][out] */ IWbemCallResult __RPC_FAR *__RPC_FAR *ppCallResult) {return WBEM_E_NOT_SUPPORTED;};
        
        HRESULT STDMETHODCALLTYPE PutClassAsync( 
            /* [in] */ IWbemClassObject __RPC_FAR *pObject,
            /* [in] */ long lFlags,
            /* [in] */ IWbemContext __RPC_FAR *pCtx,
            /* [in] */ IWbemObjectSink __RPC_FAR *pResponseHandler) {return WBEM_E_NOT_SUPPORTED;};
        
        HRESULT STDMETHODCALLTYPE DeleteClass( 
            /* [in] */ BSTR Class,
            /* [in] */ long lFlags,
            /* [in] */ IWbemContext __RPC_FAR *pCtx,
            /* [unique][in][out] */ IWbemCallResult __RPC_FAR *__RPC_FAR *ppCallResult) {return WBEM_E_NOT_SUPPORTED;};
        
        HRESULT STDMETHODCALLTYPE DeleteClassAsync( 
            /* [in] */ BSTR Class,
            /* [in] */ long lFlags,
            /* [in] */ IWbemContext __RPC_FAR *pCtx,
            /* [in] */ IWbemObjectSink __RPC_FAR *pResponseHandler) {return WBEM_E_NOT_SUPPORTED;};
        
        HRESULT STDMETHODCALLTYPE CreateClassEnum( 
            /* [in] */ BSTR Superclass,
            /* [in] */ long lFlags,
            /* [in] */ IWbemContext __RPC_FAR *pCtx,
            /* [out] */ IEnumWbemClassObject __RPC_FAR *__RPC_FAR *ppEnum) {return WBEM_E_NOT_SUPPORTED;};
        
        HRESULT STDMETHODCALLTYPE CreateClassEnumAsync( 
            /* [in] */ BSTR Superclass,
            /* [in] */ long lFlags,
            /* [in] */ IWbemContext __RPC_FAR *pCtx,
            /* [in] */ IWbemObjectSink __RPC_FAR *pResponseHandler) {return WBEM_E_NOT_SUPPORTED;};
        
        HRESULT STDMETHODCALLTYPE PutInstance( 
            /* [in] */ IWbemClassObject __RPC_FAR *pInst,
            /* [in] */ long lFlags,
            /* [in] */ IWbemContext __RPC_FAR *pCtx,
            /* [unique][in][out] */ IWbemCallResult __RPC_FAR *__RPC_FAR *ppCallResult) {return WBEM_E_NOT_SUPPORTED;};
        
        HRESULT STDMETHODCALLTYPE PutInstanceAsync( 
            /* [in] */ IWbemClassObject __RPC_FAR *pInst,
            /* [in] */ long lFlags,
            /* [in] */ IWbemContext __RPC_FAR *pCtx,
            /* [in] */ IWbemObjectSink __RPC_FAR *pResponseHandler) {return WBEM_E_NOT_SUPPORTED;};
        
        HRESULT STDMETHODCALLTYPE DeleteInstance( 
            /* [in] */ BSTR ObjectPath,
            /* [in] */ long lFlags,
            /* [in] */ IWbemContext __RPC_FAR *pCtx,
            /* [unique][in][out] */ IWbemCallResult __RPC_FAR *__RPC_FAR *ppCallResult) {return WBEM_E_NOT_SUPPORTED;};
        
        HRESULT STDMETHODCALLTYPE DeleteInstanceAsync( 
            /* [in] */ BSTR ObjectPath,
            /* [in] */ long lFlags,
            /* [in] */ IWbemContext __RPC_FAR *pCtx,
            /* [in] */ IWbemObjectSink __RPC_FAR *pResponseHandler) {return WBEM_E_NOT_SUPPORTED;};
        
        HRESULT STDMETHODCALLTYPE CreateInstanceEnum( 
            /* [in] */ BSTR Class,
            /* [in] */ long lFlags,
            /* [in] */ IWbemContext __RPC_FAR *pCtx,
            /* [out] */ IEnumWbemClassObject __RPC_FAR *__RPC_FAR *ppEnum) {return WBEM_E_NOT_SUPPORTED;};
        
        HRESULT STDMETHODCALLTYPE CreateInstanceEnumAsync( 
            /* [in] */ BSTR Class,
            /* [in] */ long lFlags,
            /* [in] */ IWbemContext __RPC_FAR *pCtx,
            /* [in] */ IWbemObjectSink __RPC_FAR *pResponseHandler){return WBEM_E_NOT_SUPPORTED;}
        
        HRESULT STDMETHODCALLTYPE ExecQuery( 
            /* [in] */ BSTR QueryLanguage,
            /* [in] */ BSTR Query,
            /* [in] */ long lFlags,
            /* [in] */ IWbemContext __RPC_FAR *pCtx,
            /* [out] */ IEnumWbemClassObject __RPC_FAR *__RPC_FAR *ppEnum) {return WBEM_E_NOT_SUPPORTED;};
        
        HRESULT STDMETHODCALLTYPE ExecQueryAsync( 
            /* [in] */ BSTR QueryLanguage,
            /* [in] */ BSTR Query,
            /* [in] */ long lFlags,
            /* [in] */ IWbemContext __RPC_FAR *pCtx,
            /* [in] */ IWbemObjectSink __RPC_FAR *pResponseHandler) {return WBEM_E_NOT_SUPPORTED;};
        
        HRESULT STDMETHODCALLTYPE ExecNotificationQuery( 
            /* [in] */ BSTR QueryLanguage,
            /* [in] */ BSTR Query,
            /* [in] */ long lFlags,
            /* [in] */ IWbemContext __RPC_FAR *pCtx,
            /* [out] */ IEnumWbemClassObject __RPC_FAR *__RPC_FAR *ppEnum) {return WBEM_E_NOT_SUPPORTED;};
        
        HRESULT STDMETHODCALLTYPE ExecNotificationQueryAsync( 
            /* [in] */ BSTR QueryLanguage,
            /* [in] */ BSTR Query,
            /* [in] */ long lFlags,
            /* [in] */ IWbemContext __RPC_FAR *pCtx,
            /* [in] */ IWbemObjectSink __RPC_FAR *pResponseHandler) {return WBEM_E_NOT_SUPPORTED;};
        
        // Supported !!!!

        HRESULT STDMETHODCALLTYPE ExecMethod( BSTR, BSTR, long, IWbemContext*,
            IWbemClassObject*, IWbemClassObject**, IWbemCallResult**){return WBEM_E_NOT_SUPPORTED;}

        HRESULT STDMETHODCALLTYPE ExecMethodAsync( BSTR, BSTR, long, 
            IWbemContext*, IWbemClassObject*, IWbemObjectSink*);

       
};

typedef CMethodPro *PCMethodPro;

// This class is the class factory for CMethodPro objects.

class CProvFactory : public IClassFactory
    {
    protected:
        ULONG           m_cRef;

    public:
        CProvFactory(void);
        ~CProvFactory(void);

        //IUnknown members
        STDMETHODIMP         QueryInterface(REFIID, PPVOID);
        STDMETHODIMP_(ULONG) AddRef(void);
        STDMETHODIMP_(ULONG) Release(void);

        //IClassFactory members
        STDMETHODIMP         CreateInstance(LPUNKNOWN, REFIID
                                 , PPVOID);
        STDMETHODIMP         LockServer(BOOL);
    };

typedef CProvFactory *PCProvFactory;



// These variables keep track of when the module can be unloaded

extern long       g_cObj;
extern long       g_cLock;

#endif
