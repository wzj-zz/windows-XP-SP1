/*************************************************************************\
*
* icamsg.c
*
* Process ICA send message requests
*
* Copyright (c) 1985 - 1999, Microsoft Corporation
*
* $Author:
*
\*************************************************************************/

#include "precomp.h"
#pragma hdrstop

#include <dbt.h>
#include <ntdddisk.h>
#include "ntuser.h"

#include <winsta.h>
#include <wstmsg.h>


/*
 *  Local functions
 */
VOID     HardErrorRemove(PCTXHARDERRORINFO);
DWORD    RemoteMessageThread(PVOID);


/*
 *  External functions
 */
VOID     HardErrorInsert(PCSR_THREAD, PHARDERROR_MSG, PCTXHARDERRORINFO);
NTSTATUS ReplyMessageToTerminalServer(PCTXHARDERRORINFO);

/*
 *  Local data
 */

PCTXHARDERRORINFO gpchiList = NULL;
HANDLE g_hDoMessageEvent = NULL;


/*******************************************************************************
 *
 *  RemoteDoMessage
 *
 * ENTRY:
 *
 * EXIT:
 *    STATUS_SUCCESS - successful
 *
 ******************************************************************************/

NTSTATUS
RemoteDoMessage(
    PWINSTATION_APIMSG pMsg)
{
    WINSTATIONSENDMESSAGEMSG * pSMsg = &pMsg->u.SendMessage;
    PCTXHARDERRORINFO pchi;
    NTSTATUS Status;
    static HANDLE hMessageThread = NULL;

    EnterCrit();        // to synchronize heap calls

    /*
     *  Create list entry
     */
    if ((pchi = (PCTXHARDERRORINFO)LocalAlloc(LPTR, sizeof(CTXHARDERRORINFO))) == NULL) {
        LeaveCrit();
        return (STATUS_NO_MEMORY);
    } else if ((pchi->pTitle = LocalAlloc(LPTR, pSMsg->TitleLength + sizeof(TCHAR))) == NULL) {
        LocalFree(pchi);
        LeaveCrit();
        return (STATUS_NO_MEMORY);
    } else if ((pchi->pMessage = LocalAlloc(LPTR, pSMsg->MessageLength + sizeof(TCHAR))) == NULL) {
        LocalFree(pchi->pTitle);
        LocalFree(pchi);
        LeaveCrit();
        return (STATUS_NO_MEMORY);
    }

    /*
     * Initialize
     */
    pchi->ClientId  = pMsg->h.ClientId;
    pchi->MessageId = pMsg->MessageId;
    pchi->Timeout   = pSMsg->Timeout;
    pchi->pResponse = pSMsg->pResponse;
    pchi->hEvent    = pSMsg->hEvent;
    pchi->DoNotWait = pSMsg->DoNotWait;
    pchi->Style     = pSMsg->Style;

    pchi->pTitle[pSMsg->TitleLength/sizeof(TCHAR)] = L'\0';
    RtlMoveMemory(pchi->pTitle, pSMsg->pTitle, pSMsg->TitleLength);

    pchi->pMessage[pSMsg->MessageLength/sizeof(TCHAR)] = L'\0';
    RtlMoveMemory(pchi->pMessage, pSMsg->pMessage, pSMsg->MessageLength);

    RIPMSG1(RIP_WARNING, "RemoteDoMessage: pchi->pTitle   - %S", pchi->pTitle);
    RIPMSG1(RIP_WARNING, "RemoteDoMessage: pchi->pMessage - %S", pchi->pMessage);

    /*
     * Link in at head
     */
    pchi->pchiNext = gpchiList;
    gpchiList = pchi;

    LeaveCrit();

    /*
     * Start message thread if not running, otherwise signal thread.
     */
    if (hMessageThread == NULL) {
        DWORD dwThreadId;
        RIPMSG0(RIP_WARNING, "RemoteDoMessage: starting RemoteMessageThread ...");
        
        if ((hMessageThread = CreateThread(NULL,
                                            0,
                                            RemoteMessageThread,
                                            NULL,
                                            0,
                                            &dwThreadId)) == NULL) {

            RIPMSG1(RIP_WARNING, "RemoteDoMessage: Cannot start RemoteMessageThread, error 0x%x", GetLastError());
        }
    } else {
        if (g_hDoMessageEvent == NULL) {
            return STATUS_UNSUCCESSFUL;
        }
        Status = NtSetEvent(g_hDoMessageEvent, NULL);
        
        if (!NT_SUCCESS(Status)) {
            RIPMSG2(RIP_WARNING, "RemoteDoMessage: Error NtSetEvent failed, Status=%x, rc=%u", Status, GetLastError());
            return Status;
        }
    }

    return STATUS_SUCCESS;
}


/*******************************************************************************
 *
 *  RemoteMessageThread
 *
 * ENTRY:
 *
 * EXIT:
 *    STATUS_SUCCESS - successful
 *
 ******************************************************************************/
DWORD
RemoteMessageThread(
    PVOID pVoid)
{
    HARDERROR_MSG hemsg;
    PCTXHARDERRORINFO pchi, *ppchi;
    UNICODE_STRING Message, Title;
    NTSTATUS Status;
    OBJECT_ATTRIBUTES ObjA;

    UNREFERENCED_PARAMETER(pVoid);

    /*
     * Create sync event
     */
    InitializeObjectAttributes(&ObjA, NULL, 0, NULL, NULL);
    Status = NtCreateEvent(&g_hDoMessageEvent, EVENT_ALL_ACCESS, &ObjA,
                           NotificationEvent, FALSE);
    
    if (!NT_SUCCESS(Status)) {
        RIPMSG1(RIP_WARNING, "RemoteMessageThread: Error NtCreateEvent failed, rc=0x%x", GetLastError());
        return GetLastError();
    }

    while (!gbExitInProgress) {

        EnterCrit();        // to synchronize heap calls

        /*
         * Valid list
         */
        if (gpchiList != NULL) {

            /*
             * Find last entry
             */
            for (ppchi = &gpchiList;
                 (*ppchi != NULL) && ((*ppchi)->pchiNext != NULL);
                 ppchi = &(*ppchi)->pchiNext) ;

            /*
             * Found it
             */
            if ((pchi = *ppchi) != NULL) {
                
                /*
                 * Unlink from the list.
                 */
                for (ppchi = &gpchiList; *ppchi != NULL && *ppchi != pchi;
                    ppchi = &(*ppchi)->pchiNext)
                    ;
                
                if (*ppchi != NULL) {
                    *ppchi = pchi->pchiNext;
                }

                LeaveCrit();

                /*
                 *  Make strings unicode
                 */
                RtlInitUnicodeString(&Title, pchi->pTitle);
                RtlInitUnicodeString(&Message, pchi->pMessage);

                /*
                 *  Initialize harderror message struct
                 */
                hemsg.h.ClientId = pchi->ClientId;
                hemsg.Status = STATUS_SERVICE_NOTIFICATION;
                hemsg.NumberOfParameters = 3;
                hemsg.UnicodeStringParameterMask = 3;
                hemsg.ValidResponseOptions = OptionOk;
                hemsg.Parameters[0] = (ULONG_PTR)&Message;
                hemsg.Parameters[1] = (ULONG_PTR)&Title;
                hemsg.Parameters[2] = (ULONG_PTR)pchi->Style;

                /*
                 *  Place message in harderror queue
                 */
                HardErrorInsert(NULL, &hemsg, pchi);
            } else {
                LeaveCrit();
            }
        } else {
            LeaveCrit();
        }

        if (gpchiList == NULL) {
            
            UserAssert(g_hDoMessageEvent != NULL);
            
            Status = NtWaitForSingleObject(g_hDoMessageEvent, FALSE, NULL);
            
            UserAssert(NT_SUCCESS(Status));
            
            NtResetEvent(g_hDoMessageEvent, NULL);
        }
    }

    NtClose(g_hDoMessageEvent);
    g_hDoMessageEvent = NULL;
    
    return 0;
}

/*******************************************************************************
 *
 *  HardErrorRemove
 *
 * ENTRY:
 *
 * EXIT:
 *    STATUS_SUCCESS - successful
 *
 ******************************************************************************/

VOID HardErrorRemove(
    PCTXHARDERRORINFO pchi)
{

    /*
     *  Notify ICASRV's RPC thread if waiting
     */
    if (!pchi->DoNotWait) {
        ReplyMessageToTerminalServer(pchi);
    }

    /*
     *  Free memory
     */
    LocalFree(pchi->pMessage);
    LocalFree(pchi->pTitle);
    LocalFree(pchi);
}
