// Users.cpp: implementation of the CUsers class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "efsadu.h"
#include "Users.h"
#include <wincrypt.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUsers::CUsers()
{
    m_UsersRoot = NULL;
	m_UserAddedCnt = 0;
	m_UserRemovedCnt = 0;
}

//////////////////////////////////////////////////////////////////////
// Walk through the chain to free the memory
//////////////////////////////////////////////////////////////////////

CUsers::~CUsers()
{
    Clear();
}

PUSERSONFILE
CUsers::RemoveItemFromHead(void)
{
    PUSERSONFILE PItem = m_UsersRoot;
    if (m_UsersRoot){
        m_UsersRoot = m_UsersRoot->Next;
        if ((PItem->Flag & USERADDED) && !(PItem->Flag & USERREMOVED)){
            m_UserAddedCnt--;
        }
        if ((PItem->Flag & USERINFILE) && (PItem->Flag & USERREMOVED)){
            m_UserRemovedCnt--;
        }
    }
    return PItem;
}

DWORD
CUsers::Add( CUsers &NewUsers )
{
    PUSERSONFILE NewItem;

    while (NewItem = NewUsers.RemoveItemFromHead()){
        PUSERSONFILE    TmpItem = m_UsersRoot;
        
        while ( TmpItem ){

            if ((NewItem->UserName && TmpItem->UserName && !_tcsicmp(NewItem->UserName, TmpItem->UserName)) ||
                 ((NULL == NewItem->UserName) && (TmpItem->UserName == NULL))){

                //
                // User exist
                //

                if ( TmpItem->Flag & USERREMOVED ){

                    if ( TmpItem->Flag & USERADDED ){

                        ASSERT(!(TmpItem->Flag & USERINFILE));

                        //
                        //    User added and removed
                        //
                        m_UserAddedCnt++;

                    } else if ( TmpItem->Flag & USERINFILE ){

                        //
                        //    User added and removed
                        //
                        m_UserRemovedCnt--;

                    }
                    TmpItem->Flag &= ~USERREMOVED;
                }

                //
                // The caller will count on CUsers to release the memory
                //

                if (NewItem->UserName){
                    delete [] NewItem->UserName;
                }
                if ( NewItem->Context ) {
                    CertFreeCertificateContext((PCCERT_CONTEXT)NewItem->Context);
                }
                delete [] NewItem->Cert;
                if (NewItem->UserSid){
                    delete [] NewItem->UserSid;
                }
                delete NewItem;
                NewItem = NULL;                
                break;
            }
            TmpItem = TmpItem->Next;
        }

        if (NewItem ){ 
            //
            // New item. Insert into the head.
            //

            NewItem->Next = m_UsersRoot;
            m_UsersRoot = NewItem;
            m_UserAddedCnt++;
        }

    }

    return ERROR_SUCCESS;
}

DWORD
CUsers::Add(
    LPTSTR UserName,
    PVOID UserCert, 
    PSID UserSid, /* = NULL */
    DWORD Flag, /* = USERINFILE */
    PVOID Context /* = NULL */
    )
//////////////////////////////////////////////////////////////////////
// Routine Description:
//      Create an item for a user
// Arguments:
//      UserName -- User's name
//      UserCert -- User's certificate blob or hash
//      UserSid -- User's ID. Can be NULL
//      Flag -- Indicate if the item is existing in the file, to be added or removed
//  Return Value:
//      NO_ERROR if succeed.
//      Will throw exception if memory allocation fails. ( From new.)
// 
//////////////////////////////////////////////////////////////////////
{

    PUSERSONFILE UserItem;
    PUSERSONFILE TmpUserItem = m_UsersRoot;
    PEFS_CERTIFICATE_BLOB CertBlob;
    PEFS_HASH_BLOB  CertHashBlob;
    DWORD   CertSize;
    DWORD   SidSize;

    if ( !UserCert ){
        return ERROR_INVALID_PARAMETER;
    }

    ASSERT ( (( Flag & USERADDED ) || ( Flag & USERINFILE )) &&
                       ( (Flag & (USERADDED | USERINFILE)) != (USERADDED | USERINFILE)));


    //
    // If the user already in the memory, no new item is to be created except for unknown user
    //

    while ( TmpUserItem ){
        if ( (UserName && TmpUserItem->UserName && !_tcsicmp(UserName, TmpUserItem->UserName)) ||
              ((NULL == UserName) && (TmpUserItem->UserName == NULL))){

            //
            // User exist
            //

            if ( TmpUserItem->Flag & USERREMOVED ){

                if ( TmpUserItem->Flag & USERADDED ){

                    ASSERT(!(TmpUserItem->Flag & USERINFILE));

                    //
                    //    User added and removed
                    //
                    m_UserAddedCnt++;

                } else if ( TmpUserItem->Flag & USERINFILE ){

                    //
                    //    User added and removed
                    //
                    m_UserRemovedCnt--;

                }
                TmpUserItem->Flag &= ~USERREMOVED;
            }

            //
            // The caller will count on CUsers to release 
            // the context if the call returns CRYPT_E_EXISTS. This is just for
            // performance reason.
            //
/*
            if (UserName){
                delete [] UserName;
            }
*/
            if ( Context ) {
                CertFreeCertificateContext((PCCERT_CONTEXT)Context);
                Context = NULL;
            }
            return CRYPT_E_EXISTS;
        }
        TmpUserItem = TmpUserItem->Next;
    }
    
    try {
        UserItem = new USERSONFILE;
        if ( NULL == UserItem ){
            AfxThrowMemoryException( );
        }

        UserItem->Next = NULL;

        //
        // In case exception raised, we can call delete.
        // Delete NULL is OK, but random data is not OK.
        //

        UserItem->UserSid = NULL;
        UserItem->Cert = NULL;
        UserItem->Context = NULL;

        if ( UserSid ){
            SidSize = GetLengthSid( UserSid );
            if (  SidSize > 0 ){
                UserItem->UserSid = new BYTE[SidSize];
                if ( NULL == UserItem->UserSid ){
                    AfxThrowMemoryException( );
                }
                if ( !CopySid(SidSize, UserItem->UserSid, UserSid)){
                    delete [] UserItem->UserSid;
                    delete UserItem;
                    return GetLastError();
                }
                
            } else {
                delete UserItem;
                return GetLastError();
            }
        } else {
            UserItem->UserSid = NULL;
        }
 
        if ( Flag & USERINFILE ){

            //
            // The info is from the file. Use the hash structure
            //

            CertHashBlob = ( PEFS_HASH_BLOB ) UserCert;
            CertSize = sizeof(EFS_HASH_BLOB) + CertHashBlob->cbData;
            UserItem->Cert = new BYTE[CertSize];
            if ( NULL == UserItem->Cert ){
                AfxThrowMemoryException( );
            }
            ((PEFS_HASH_BLOB)UserItem->Cert)->cbData = CertHashBlob->cbData;
            ((PEFS_HASH_BLOB)UserItem->Cert)->pbData = (PBYTE)(UserItem->Cert) + sizeof(EFS_HASH_BLOB);
            memcpy(((PEFS_HASH_BLOB)UserItem->Cert)->pbData, 
                   CertHashBlob->pbData,
                   CertHashBlob->cbData
                  );
        } else {

            //
            // The info is from the user picked cert. Use Cert Blob structure
            //

            CertBlob = ( PEFS_CERTIFICATE_BLOB ) UserCert;
            CertSize = sizeof(EFS_CERTIFICATE_BLOB) + CertBlob->cbData;
            UserItem->Cert = new BYTE[CertSize];
            if ( NULL == UserItem->Cert ){
                AfxThrowMemoryException( );
            }
            ((PEFS_CERTIFICATE_BLOB)UserItem->Cert)->cbData = CertBlob->cbData;
            ((PEFS_CERTIFICATE_BLOB)UserItem->Cert)->dwCertEncodingType = CertBlob->dwCertEncodingType;
            ((PEFS_CERTIFICATE_BLOB)UserItem->Cert)->pbData = (PBYTE)(UserItem->Cert) + sizeof(EFS_CERTIFICATE_BLOB);
            memcpy(((PEFS_CERTIFICATE_BLOB)UserItem->Cert)->pbData, 
                   CertBlob->pbData,
                   CertBlob->cbData
                  );

        }
 
        UserItem->UserName = UserName;
        UserItem->Context = Context;
        UserItem->Flag = Flag;
        if ( Flag & USERADDED ){
            m_UserAddedCnt ++;
        }
    }
    catch (...) {
        delete [] UserItem->UserSid;
        delete [] UserItem->Cert;
        delete UserItem;
        AfxThrowMemoryException( );
        return ERROR_NOT_ENOUGH_MEMORY; 
    }

    //
    // Add to the head
    //

    if ( NULL != m_UsersRoot ){
        UserItem->Next = m_UsersRoot;
    }
    m_UsersRoot = UserItem;

    return NO_ERROR;
}

DWORD
CUsers::Remove(
    LPCTSTR UserName
    )
//////////////////////////////////////////////////////////////////////
// Routine Description:
//      Remove a user from the list. Actually just mark for remove.
// Arguments:
//      UserName -- User's name
//  Return Value:
//      NO_ERROR if succeed.
//      ERROR_NOT_FOUND if the user cannot be found.
// 
//////////////////////////////////////////////////////////////////////
{
    PUSERSONFILE TmpUserItem = m_UsersRoot;

    BOOL    UserMatched =FALSE;

    while ( TmpUserItem ){
        if (((NULL==UserName) && ( NULL == TmpUserItem->UserName)) || 
            ( UserName && TmpUserItem->UserName && !_tcsicmp(UserName, TmpUserItem->UserName))){

            //
            // User exist, mark it for remove
            //

            if ( TmpUserItem->Flag & USERINFILE ){
                m_UserRemovedCnt++;
            } else if ( TmpUserItem->Flag & USERADDED ) {
                m_UserAddedCnt--;
            }
            TmpUserItem->Flag |= USERREMOVED;
            return NO_ERROR;
        }
        TmpUserItem = TmpUserItem->Next;
    }
    return ERROR_NOT_FOUND;
}

PVOID
CUsers::StartEnum()
//////////////////////////////////////////////////////////////////////
// Routine Description:
//      Prepare for GetNextUser
// Arguments:
//
//  Return Value:
//      A pointer used for GetNextUser
// 
//////////////////////////////////////////////////////////////////////
{
    return ((PVOID)m_UsersRoot);
}

PVOID
CUsers::GetNextUser(
    PVOID Token, 
    CString &UserName,
    CString &CertHash
    )
//////////////////////////////////////////////////////////////////////
// Routine Description:
//      Get next user in the list.(Not removed).
// Arguments:
//      UserName -- Next User's name
//      CertHash -- Certificate Thumbprinter
//      Token -- A pointer returned by previous GetNextUser or StartEnum. 
// Return Value:
//      A pointer for GetNextUser()
// 
//////////////////////////////////////////////////////////////////////
{

    PUSERSONFILE   TmpItem = (PUSERSONFILE) Token;
    PVOID   RetPointer = NULL;

    while ( TmpItem ){

        if ( TmpItem->Flag & USERREMOVED ){
            TmpItem = TmpItem->Next;
            continue;
        }

        try{    
            LPWSTR     HashString = NULL;

            UserName = TmpItem->UserName;

            if (TmpItem->Flag & USERINFILE){

                PEFS_HASH_BLOB UserHashBlob;

                UserHashBlob = (PEFS_HASH_BLOB)TmpItem->Cert;
                HashString = new WCHAR[((((UserHashBlob->cbData + 1)/2) * 5) + 1)];
                if (HashString) {
                    ConvertHashToStr(UserHashBlob->pbData, UserHashBlob->cbData, HashString);
                }

            } else if ( TmpItem->Context ){

                DWORD cbHash;
                PBYTE pbHash;

                if (CertGetCertificateContextProperty(
                             (PCCERT_CONTEXT)TmpItem->Context,
                             CERT_HASH_PROP_ID,
                             NULL,
                             &cbHash
                             )) {

                    pbHash = (PBYTE)new BYTE[cbHash];

                    if (pbHash != NULL) {

                        if (CertGetCertificateContextProperty(
                                     (PCCERT_CONTEXT)TmpItem->Context,
                                     CERT_HASH_PROP_ID,
                                     pbHash,
                                     &cbHash
                                     )) {

                            HashString = new WCHAR[((((cbHash + 1)/2) * 5) + 1)];
                            if (HashString) {
                                ConvertHashToStr(pbHash, cbHash, HashString);
                            }
                        }
                      
                        delete [] pbHash;

                    }
                }

            }
            
            CertHash = HashString;
            if (HashString){
                delete [] HashString;
            }
            RetPointer = TmpItem->Next;
        }
        catch (...){

            //
            // Out of memory
            //

            TmpItem = NULL;
            RetPointer = NULL;
        }
        break;
    }

    if ( NULL == TmpItem ){
        UserName.Empty();
        CertHash.Empty();
    }
    return RetPointer;

}

DWORD CUsers::GetUserAddedCnt()
{
    return m_UserAddedCnt;
}

DWORD CUsers::GetUserRemovedCnt()
{
    return m_UserRemovedCnt;
}

PVOID
CUsers::GetNextChangedUser(
    PVOID Token, 
    LPTSTR * UserName,
    PSID * UserSid, 
    PVOID * CertData, 
    DWORD * Flag
    )
//////////////////////////////////////////////////////////////////////
// Routine Description:
//      Get the info for changed users. This method is not well behaved in the
//  sense of OOP. It exposes internal pointers to the ouside world. The gain
//  is performance. At this moment, CUsers is a supporting class and used only
//  by USERLIST and CAddSheet (single thread). We can make USERLIST a 
//  friend of CUsers if such concerns are raised in the future or reimplement this. 
//  The same issue applies to the enumerate methods.
//
// Arguments:
//      Token -- A pointer to the item returned in previous GetNextChangedUser or StartEnum.
//      UserName -- User's name
//      CertData -- User's certificate blob or hash
//      UserSid -- User's ID. Can be NULL
//      Flag -- Indicate if the item is existing in the file, to be added or removed
//  Return Value:
//      Next item pointer.
// 
//////////////////////////////////////////////////////////////////////
{
    BOOL    ChangedUserFound = FALSE;

    while ( Token ){

        *Flag = ((PUSERSONFILE) Token)->Flag;

        if ( ( *Flag & USERADDED ) && !( *Flag & USERREMOVED )){

            //
            // The user is to to be added to the file
            //

            *Flag = USERADDED;
            ChangedUserFound = TRUE;

        } else if ( ( *Flag & USERREMOVED ) && ( *Flag & USERINFILE)){

            //
            // The user is to be removed from the file
            //

            *Flag = USERREMOVED;
            ChangedUserFound = TRUE;

        }

        if ( ChangedUserFound ){

            *UserName = ((PUSERSONFILE) Token)->UserName;
            *UserSid = ((PUSERSONFILE) Token)->UserSid;
            *CertData = ((PUSERSONFILE) Token)->Cert;
            return ((PUSERSONFILE) Token)->Next;

        } else {

            Token = ((PUSERSONFILE) Token)->Next;

        }

    }

    *UserName = NULL;
    *UserSid = NULL;
    *CertData = NULL;
    *Flag = 0;
    return NULL;
}

void CUsers::Clear()
{

    PUSERSONFILE TmpUserItem = m_UsersRoot;
    while (TmpUserItem){
        m_UsersRoot = TmpUserItem->Next;
        delete [] TmpUserItem->UserName;
        delete [] TmpUserItem->Cert;
        if (TmpUserItem->UserSid){
            delete [] TmpUserItem->UserSid;
        }
        if (TmpUserItem->Context){
            CertFreeCertificateContext((PCCERT_CONTEXT)TmpUserItem->Context);
        }
        delete TmpUserItem;
        TmpUserItem = m_UsersRoot;
    }

    m_UsersRoot = NULL;
	m_UserAddedCnt = 0;
	m_UserRemovedCnt = 0;

}

void CUsers::ConvertHashToStr(
    PBYTE pHashData,
    DWORD cbData,
    LPWSTR OutHashStr
    )
{

    DWORD Index = 0;
    BOOLEAN NoLastZero = FALSE;

    for (; Index < cbData; Index+=2) {

        BYTE HashByteLow = pHashData[Index] & 0x0f;
        BYTE HashByteHigh = (pHashData[Index] & 0xf0) >> 4;

        OutHashStr[Index * 5/2] = HashByteHigh > 9 ? (WCHAR)(HashByteHigh - 9 + 0x40): (WCHAR)(HashByteHigh + 0x30);
        OutHashStr[Index * 5/2 + 1] = HashByteLow > 9 ? (WCHAR)(HashByteLow - 9 + 0x40): (WCHAR)(HashByteLow + 0x30);

        if (Index + 1 < cbData) {
            HashByteLow = pHashData[Index+1] & 0x0f;
            HashByteHigh = (pHashData[Index+1] & 0xf0) >> 4;
    
            OutHashStr[Index * 5/2 + 2] = HashByteHigh > 9 ? (WCHAR)(HashByteHigh - 9 + 0x40): (WCHAR)(HashByteHigh + 0x30);
            OutHashStr[Index * 5/2 + 3] = HashByteLow > 9 ? (WCHAR)(HashByteLow - 9 + 0x40): (WCHAR)(HashByteLow + 0x30);
    
            OutHashStr[Index * 5/2 + 4] = L' ';

        } else {
            OutHashStr[Index * 5/2 + 2] = 0;
            NoLastZero = TRUE;
        }

    }

    if (!NoLastZero) {
        OutHashStr[Index*5/2] = 0;
    }

}
