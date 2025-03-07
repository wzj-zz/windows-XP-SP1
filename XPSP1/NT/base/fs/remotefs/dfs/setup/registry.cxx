//+---------------------------------------------------------------------------
//
//  Copyright (C) 1992, Microsoft Corporation.
//
//  File:       registry.cxx
//
//  Contents:   implementations for CRegKey member Members
//
//  Members:    CRegKey::CRegKey - constructor for registry key object
//              CRegKey::CRegKey - constructor for registry key object
//              CRegKey::CreateKey - real worker for constructors
//              CRegKey::~CRegKey - destructor for registry key object
//              CRegKey::Delete - delete a registry key
//              CRegKey::EnumValues - enumerate values of a registry key
//              CRegKey::EnumKeys - enumerate subkeys of a registry key
//              CRegKey::NotifyChange - setup change notification for a key
//
//              CRegValue::GetValue - sets a registry value
//              CRegValue::SetValue - retrieves a registry value
//              CRegValue::Delete - deletes a registry value
//              CRegValue::GetTypeCode - returns the type code of the value
//
//              CRegMSZ::SetStrings - sets a multi-string registry value
//              CRegMSZ::GetStrings - retrieves a multi-string registry value
//
//  History:    09/30/92    Rickhi  Created
//              09/22/93    AlokS   Took out exception throwing code
//                                  and added proper return code for
//                                  each method.
//
//              07/26/94    AlokS   Made it real light weight for simple
//                                  registry set/get operations
//
//  Notes:      see notes in registry.hxx
//
//----------------------------------------------------------------------------

#include    <stdlib.h>
#include    <windows.h>
#include    <registry.hxx>

//+-------------------------------------------------------------------------
//
//  Member:     CRegKey::CRegKey
//
//  Synopsis:   Constructor for registry key object, using HKEY for parent
//
//  Arguments:  [hkParent] - handle to parent key
//              [pwszPath] - pathname to key
//              [samDesiredAccess] - desired access rights to the key
//              [pwszClass] - class for the key
//              [dwOptions] - options for the key eg volatile or not
//              [pdwDisposition] - to find out if key was opened or created
//              [pSecurityAttributes] - used only if the key is created
//              [fThrowExceptionOnError] - Constructor throw exception on error
//
//  Signals:    Internal error state is set if construction fails.
//
//  Returns:    -none-
//
//  History:    09/30/92    Rickhi  Created
//
//  Notes:      All except the hkParent and pwszPath are optional parameters.
//
//--------------------------------------------------------------------------

CRegKey::CRegKey (
        HKEY hkParent,
        const LPWSTR pwszPath,
        REGSAM samDesiredAccess,
        const LPWSTR pwszClass,
        DWORD dwOptions,
        DWORD *pdwDisposition,
        const LPSECURITY_ATTRIBUTES pSecurityAttributes )
    :_hkParent(hkParent),
     _hkThis(NULL),
     _dwErr (ERROR_SUCCESS)
{
    _dwErr = CreateKey( _hkParent,
                     pwszPath,
                     samDesiredAccess,
                     pwszClass,
                     dwOptions,
                     pdwDisposition,
                     pSecurityAttributes );
}


//+-------------------------------------------------------------------------
//
//  Member:     CRegKey::CRegKey
//
//  Synopsis:   Constructor for registry key object, using CRegKey for parent
//
//  Arguments:  [prkParent] - ptr to Parent CRegKey
//              [pwszPath] - pathname to key
//              [samDesiredAccess] - desired access rights to the key
//              [pwszClass] - class for the key
//              [dwOptions] - options for the key eg volatile or not
//              [pdwDisposition] - to find out if key was opened or created
//              [pSecurityAttributes] - used only if the key is created
//              [fThrowExceptionOnError] - Constructor throw exception on error
//
//  Signals:    Internal Error state is set if error occures during construction.
//
//  Returns:    nothing
//
//  History:    09/30/92    Rickhi  Created
//
//  Notes:      All except the prkParent and pwszPath are optional parameters.
//
//--------------------------------------------------------------------------

CRegKey::CRegKey (
        const CRegKey &crkParent,
        const LPWSTR pwszPath,
        REGSAM samDesiredAccess,
        const LPWSTR pwszClass,
        DWORD dwOptions,
        DWORD *pdwDisposition,
        const LPSECURITY_ATTRIBUTES pSecurityAttributes )
    :_hkParent(crkParent.GetHandle()),
     _hkThis(NULL),
     _dwErr(ERROR_SUCCESS)
{
    _dwErr = CreateKey ( _hkParent,
                      pwszPath,
                      samDesiredAccess,
                      pwszClass,
                      dwOptions,
                      pdwDisposition,
                      pSecurityAttributes );
}

//+-------------------------------------------------------------------------
//
//  Member:     CRegKey::CRegKey
//
//  Synopsis:   Constructor for registry key object, using HKEY for parent
//                              Merely opens the key, if exist
//
//  Arguments:  [hkParent] - HKEY to Parent
//                              [dwErr]      - Error code returned here
//                      [pwszPath] - pathname to key
//                      [samDesiredAccess] - desired access rights to the key
//
//  Signals:    Internal Error state is set if error occures during construction
//
//  Returns:    nothing
//
//  History:    09/22/93    AlokS  Created
//
//  Notes:      Check error status to determine if constructor succeeded
//
//--------------------------------------------------------------------------

CRegKey::CRegKey (
        HKEY hkParent,
        DWORD *pdwErr,
        const LPWSTR pwszPath,
        REGSAM samDesiredAccess )
    :_hkParent(hkParent),
     _hkThis(NULL),
     _dwErr(ERROR_SUCCESS)
{
     *pdwErr = _dwErr = OpenKey  ( _hkParent, pwszPath, samDesiredAccess );
}

//+-------------------------------------------------------------------------
//
//  Member:     CRegKey::CRegKey
//
//  Synopsis:   Constructor for registry key object, using CRegKey for parent
//                              Merely opens the key, if exist
//
//  Arguments:  [prkParent] - ptr to Parent CRegKey
//              [dwErr]           -  Error code returned here.
//                      [pwszPath] - pathname to key
//                      [samDesiredAccess] - desired access rights to the key
//
//  Signals:    Internal Error state is set if error occures during construction
//
//  Returns:    nothing
//
//  History:    09/22/93    AlokS  Created
//
//  Notes:      Check error status to determine if constructor succeeded
//
//--------------------------------------------------------------------------

CRegKey::CRegKey (
        const CRegKey  &crkParent,
        DWORD *pdwErr,
        const LPWSTR pwszPath,
        REGSAM   samDesiredAccess )
    :_hkParent(crkParent.GetHandle()),
     _hkThis(NULL),
     _dwErr(ERROR_SUCCESS)
{
     *pdwErr = _dwErr = OpenKey ( _hkParent, pwszPath, samDesiredAccess );
}

//+-------------------------------------------------------------------------
//
//  Member:     CRegKey::~CRegKey, public
//
//  Synopsis:   Destructor for registry key object
//
//  Arguments:  none
//
//  Signals:    nothing
//
//  Returns:    nothing
//
//  History:    09/30/92    Rickhi  Created
//
//  Notes:
//
//--------------------------------------------------------------------------

CRegKey::~CRegKey()
{
    if (_hkThis != NULL)
        RegCloseKey(_hkThis);
}

//+-------------------------------------------------------------------------
//
//  Member:     CRegKey::CreateKey, private
//
//  Synopsis:   This method does the real work of the constructors.
//
//  Arguments:  [hkParent] - handle to parent key
//              [pwszPath] - pathname to key
//              [samDesiredAccess] - desired access rights to the key
//              [pwszClass] - class for the key
//              [dwOptions] - options for the key eg volatile or not
//              [pdwDisposition] - to find out if key was opened or created
//              [pSecurityAttributes] - used only if the key is created
//
//  Signals:    -none-
//
//  Returns:    ERROR_SUCCESS on success. Else error from either Registry APIs
//              or from Memory allocation
//
//  History:    09/30/92    Rickhi  Created
//
//  Notes:      All parameters are required.
//
//--------------------------------------------------------------------------

DWORD CRegKey::CreateKey (
        HKEY hkParent,
        const LPWSTR  pwszPath,
        REGSAM  samDesiredAccess,
        const LPWSTR  pwszClass,
        DWORD   dwOptions,
        DWORD   *pdwDisposition,
        const LPSECURITY_ATTRIBUTES pSecurityAttributes )
{
    DWORD   dwDisposition;
    DWORD   dwRc;
    DWORD dwErr = ERROR_SUCCESS;
    LPSECURITY_ATTRIBUTES lpsec = pSecurityAttributes;

    //  create/open the key
    if ((dwRc = RegCreateKeyEx(hkParent,
                           pwszPath,           //  path to key
                           0,                  //  title index
                           pwszClass,          //  class of key
                           dwOptions,          //  key options
                           samDesiredAccess,   //  desired access
                           lpsec,              //  if created
                           &_hkThis,           //  handle
                           &dwDisposition)     //  opened/created
                          )==ERROR_SUCCESS)
    {
        //  save away the name
        _cwszName.Set(pwszPath);

        //  setup the return parameters
        if (pdwDisposition != NULL)
            *pdwDisposition = dwDisposition;

    }
    else
        dwErr = Creg_ERROR(dwRc);

    return(dwErr);
}


//+-------------------------------------------------------------------------
//
//  Member:     CRegKey::OpenKey, private
//
//  Synopsis:   This method does the real work of the constructors.
//
//  Arguments:  [hkParent] - handle to parent key
//                      [pwszPath] - pathname to key
//                      [samDesiredAccess] - desired access rights to the key
//
//  Signals:    -none-
//
//  Returns:    ERROR_SUCCESS on success. Else error from either Registry APIs
//              or from Memory allocation
//
//  History:    09/22/93        AlokS  Created
//
//  Notes:      All parameters are required.
//
//--------------------------------------------------------------------------

DWORD CRegKey::OpenKey (
        HKEY    hkParent,
        const  LPWSTR  pwszPath,
        REGSAM  samDesiredAccess )
{
    DWORD   dwRc;
    DWORD dwErr = ERROR_SUCCESS;

    //  open the key
    if ((dwRc = RegOpenKeyEx(hkParent,
                         pwszPath,           //  path to key
                         0,                  //  reserved
                         samDesiredAccess,   //  desired access
                         &_hkThis            //  handle
                        ))==ERROR_SUCCESS)
    {
        //  save away the name
        _cwszName.Set(pwszPath);

    }
    else
        dwErr = Creg_ERROR(dwRc);

    return(dwErr);
}
//+-------------------------------------------------------------------------
//
//  Member:     CRegValue::GetValue, public
//
//  Purpose:    Returns the data associated with a registry value.
//
//  Arguements: [pbData] - ptr to buffer supplied by caller.
//              [cbData] - size of data buffer supplied.
//              [pdwTypeCode] - type of data returned.
//
//  Signals:
//
//  Returns:    ERROR_SUCCESS on success. Else error from either Registry APIs
//              or from Memory allocation
//
//  History:    09/30/92    Rickhi  Created
//
//  Notes:
//
//
//
//--------------------------------------------------------------------------

DWORD CRegValue::GetValue(LPBYTE pbData, ULONG* pcbData, DWORD *pdwTypeCode)
{
    DWORD dwRc = RegQueryValueEx(GetParentHandle(),
                                    (LPWSTR)_cwszValueID,    //  value id
                                    NULL,        //  title index
                                    pdwTypeCode, //  type of data returned
                                    pbData,       //  data
                                    pcbData);       // size of data
    return(dwRc);
}


//+-------------------------------------------------------------------------
//
//  Member:     CRegValue::SetValue
//
//  Purpose:    Writes the data associated with a registry value.
//
//  Arguements: [pbData] - ptr to data to write.
//                      [cbData] - size of data to write.
//                      [dwTypeCode] - type of data to write.
//
//  Signals:    -none-
//
//  Returns:    ERROR_SUCCESS on success. Else error from either Registry APIs
//              or from Memory allocation
//
//  History:    09/30/92    Rickhi  Created
//
//  Notes:
//
//--------------------------------------------------------------------------

DWORD CRegValue::SetValue(const LPBYTE pbData, ULONG cbData, DWORD dwTypeCode)
{
    DWORD   dwRc;
    DWORD dwErr = ERROR_SUCCESS;
    if ((dwRc = RegSetValueEx(GetParentHandle(),        //  key handle
                             (LPWSTR)_cwszValueID,  //  value id
                              NULL,      //  title index
                              dwTypeCode,    //  type of info in buffer
                              pbData,        //  data
                              cbData)        //  size of data
                             )!= ERROR_SUCCESS)
    {
        dwErr = Creg_ERROR(dwRc);
    }
    return(dwErr);
}


DWORD CRegKey::DeleteChildren()
{
    //  figure out how many keys are currently stored in this key

    ULONG   cSubKeys, cbMaxSubKeyLen;
    DWORD  dwErr = ERROR_SUCCESS;

    DWORD dwRc = RegQueryInfoKey(_hkThis,
                     NULL,
                     NULL,
                     NULL,
                     &cSubKeys,
                     &cbMaxSubKeyLen,
                     NULL,
                     NULL,
                     NULL,
                     NULL,
                     NULL,
                     NULL);

    if ( dwRc != ERROR_SUCCESS )
    {
         //  QueryInfo failed..
        dwErr = Creg_ERROR(dwRc);
    }

    if (dwErr != ERROR_SUCCESS)
    {
        return(dwErr);
    }
    //  loop enumerating and creating a RegKey object for each subkey
    DWORD   dwIndex=0;

    do
    {
        WCHAR   wszKeyName[MAX_PATH];
        ULONG   cbKeyName = sizeof(wszKeyName);
        FILETIME ft;

        if ((dwRc = RegEnumKeyEx(_hkThis,         //  handle
                                dwIndex,        //  index
                                wszKeyName,     //  key name
                                &cbKeyName,     //  length of key name
                                NULL,           //  title index
                                NULL,       //  class
                                NULL,       //  length of class
                                &ft             //  last write time
                              ))==ERROR_SUCCESS)
        {
            //  Create a CRegKey object for the subkey
            CRegKey *pRegKey = (CRegKey *) new CRegKey((const CRegKey &)*this, wszKeyName);
            if (ERROR_SUCCESS != (dwErr = pRegKey->QueryErrorStatus()))
            {
                break;
            }
            pRegKey->DeleteChildren();
            delete pRegKey;
            dwRc = RegDeleteKey(_hkThis, wszKeyName);
        }
        else
        {
            //  error, we're done with the enumeration
            break;
        }

    } while (dwIndex < cSubKeys);


    //  finished the enumeration, check the results
    if ((dwErr == ERROR_SUCCESS) && 
        (dwRc != ERROR_NO_MORE_ITEMS && dwRc != ERROR_SUCCESS))
    {
        dwErr = Creg_ERROR(dwRc);
    }

    return(dwErr);
}

