///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1998, Microsoft Corp. All rights reserved.
//
// FILE
//
//    iasacct.cpp
//
// SYNOPSIS
//
//    Implementation of DLL exports for an ATL in proc server.
//
// MODIFICATION HISTORY
//
//    08/04/1998    Original version.
//
///////////////////////////////////////////////////////////////////////////////

#include <ias.h>
#include <iasutil.h>
#include <newop.cpp>

#include <account.h>

CComModule _Module;
#include <atlimpl.cpp>

BEGIN_OBJECT_MAP(ObjectMap)
   OBJECT_ENTRY(__uuidof(Accounting),
                IASTL::IASRequestHandlerObject<Accountant> )
END_OBJECT_MAP()


//////////
// DLL Entry Point
//////////
BOOL
WINAPI
DllMain(
    HINSTANCE hInstance,
    DWORD dwReason,
    LPVOID /*lpReserved*/
    )
{
   if (dwReason == DLL_PROCESS_ATTACH)
   {
     _Module.Init(ObjectMap, hInstance);

     DisableThreadLibraryCalls(hInstance);
   }
   else if (dwReason == DLL_PROCESS_DETACH)
   {
     _Module.Term();
   }

   return TRUE;
}


//////////
// Used to determine whether the DLL can be unloaded by OLE
//////////
STDAPI DllCanUnloadNow(void)
{
   return (_Module.GetLockCount()==0) ? S_OK : S_FALSE;
}


//////////
// Returns a class factory to create an object of the requested type.
//////////
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
   return _Module.GetClassObject(rclsid, riid, ppv);
}


//////////
// DllRegisterServer - Adds entries to the system registry
//////////
STDAPI DllRegisterServer(void)
{
   return  _Module.RegisterServer(FALSE);
}


//////////
// DllUnregisterServer - Removes entries from the system registry
//////////
STDAPI DllUnregisterServer(void)
{
   return _Module.UnregisterServer();
}
