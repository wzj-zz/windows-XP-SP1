/*++

Copyright (c) 1990  Microsoft Corporation

Module Name:

    eventlog.c

Abstract:

    This file contains the main routines for the NT Event Logging Service.

Author:

    Rajen Shah  (rajens)    1-Jul-1991

Revision History:

    02-Mar-01           drbeck
        Modified ElfWriteProductInfoEvent to utilize GetVersionEx for OS major
        and minor version numbers as well as for the build number. The
        value CurrentBuild under the HKLM/SOFTWARE/Microsoft/Windows NT/CurrentVersion 
        registry is obsolete.
        
    26-Jan-1994     Danl
        SetUpModules:  Fixed memory leak where the buffers for the enumerated
        key names were never free'd.  Also fixed problem where the size of
        the MULTI_SZ buffer used for the "Sources" key was calculated by
        using the names in the registry, while the copying was done
        using the names in the module list.  When registry keys are deleted,
        the module list entry is retained until the next boot.  Since the
        module list is larger, it would overwrite the MULTI_SZ buffer.

    1-Nov-1993      Danl
        Make Eventlog service a DLL and attach it to services.exe.
        Pass in GlobalData to Elfmain.  This GlobalData structure contains
        all well-known SIDs and pointers to the Rpc Server (Start & Stop)
        routines.  Get rid of the service process main function.

    1-Jul-1991      RajenS
        created

--*/

//
// INCLUDES
//

#include <eventp.h>
#include <ntrpcp.h>
#include <elfcfg.h>
#include <string.h>
#include <tstr.h>     // WCSSIZE
#include <alertmsg.h> // ALERT_ELF manifests


//
// Bit Flags used for Progress Reporting in SetupDataStruct().
//
#define LOGFILE_OPENED  0x00000001
#define MODULE_LINKED   0x00000002
#define LOGFILE_LINKED  0x00000004

HANDLE  g_hTimestampWorkitem;
HANDLE  g_hTimestampEvent;
ULONG   g_PreviousInterval = DEFAULT_INTERVAL;
long    g_lNumSecurityWriters = 0;

//
// Local Function Prorotypes
//
VOID
ElfInitMessageBoxTitle(
    VOID
    );



NTSTATUS
SetUpDataStruct (
    PUNICODE_STRING     LogFileName,
    ULONG               MaxFileSize,
    ULONG               Retention,
    ULONG               GuestAccessRestriction,
    PUNICODE_STRING     ModuleName,
    HANDLE              hLogFile,
    ELF_LOG_TYPE        LogType,
    LOGPOPUP            logpLogPopup,
    DWORD               dwAutoBackup
)

/*++

Routine Description:

    This routine sets up the information for one module. It is called from
    ElfSetUpConfigDataStructs for each module to be configured.

    Module information is passed into this routine and a LOGMODULE structure
    is created for it.  If the logfile associated with this module doesn't
    exist, a LOGFILE structure is created for it, and added to the linked
    list of LOGFILE structures.  The LOGMODULE is associated with the LOGFILE,
    and it is added to the linked list of LOGMODULE structures.  The logfile
    is opened and mapped to memory.

    Finally, at the end, this function calls SetUpModules, which looks at
    all the subkeys in the registry under this logfile, and adds any new ones
    to the linked list, and updates the Sources MULTI_SZ for the event viewer.

Arguments:

    LogFileName - Name of log file for this module.  If this routine needs
        a copy of this name it will make one, so that the caller can free
        the name afterwards if that is desired.

    MaxFileSize - Max size of the log file.
    Retention   - Max retention for the file.
    ModuleName  - Name of module that this file is associated with.
    RegistryHandle - Handle to the root node for this LogFile's info
                     in the registry.  This is used to enumerate all the
                     modules under this key.

Return Value:

    Pointer to Module structure that is allocated in this routine.
    NTSTATUS

Note:


--*/
{
    NTSTATUS        Status   = STATUS_SUCCESS;
    PLOGFILE        pLogFile = NULL;
    PLOGMODULE      pModule  = NULL;
    ANSI_STRING     ModuleNameA;
    DWORD           Type;
    BOOL            bAllocatedLogInfo   = FALSE;
    PUNICODE_STRING SavedBackupFileName = NULL;
    DWORD           StringLength;
    PLOGMODULE      OldDefaultLogModule = NULL;
    DWORD           Progress = 0L;

    //
    // Argument check.
    //

    if ((LogFileName == NULL)         ||
        (LogFileName->Buffer == NULL) ||
        (ModuleName == NULL))
    {
        return(STATUS_INVALID_PARAMETER);
    }

    // If the default log file for a module is also being used by another
    // module, then we just link that same file structure with the other
    // module.
    //
    // Truncate the maximum size of the log file to a 4K boundary.
    // This is to allow for page granularity.
    //

    pLogFile = FindLogFileFromName (LogFileName);

    pModule  = ElfpAllocateBuffer (sizeof (LOGMODULE) );

    if (pModule == NULL)
    {
        return(STATUS_NO_MEMORY);
    }

    if (pLogFile == NULL)
    {
        //
        //--------------------------------------
        // CREATE A NEW LOGFILE !!
        //--------------------------------------
        // A logfile by this name doesn't exist yet.  So we will create
        // one so that we can add the module to it.
        //

        ELF_LOG1(TRACE,
                "SetupDataStruct: Create new struct for %ws log\n",
                LogFileName->Buffer);

        pLogFile = ElfpAllocateBuffer(sizeof(LOGFILE));

        if (pLogFile == NULL)
        {
            ELF_LOG1(ERROR,
                     "SetupDataStruct: Unable to allocate struct for %ws log\n",
                     LogFileName->Buffer);

            ElfpFreeBuffer(pModule);
            return STATUS_NO_MEMORY;
        }

        //
        // Allocate a new LogFileName that can be attached to the
        // new pLogFile structure.
        //
        StringLength = LogFileName->Length + sizeof(WCHAR);
        SavedBackupFileName = (PUNICODE_STRING) ElfpAllocateBuffer(
            sizeof(UNICODE_STRING) + StringLength);

        if (SavedBackupFileName == NULL)
        {
            ELF_LOG1(ERROR,
                     "SetupDataStruct: Unable to allocate backup name for %ws log\n",
                     LogFileName->Buffer);

            ElfpFreeBuffer(pModule);
            ElfpFreeBuffer(pLogFile);
            return STATUS_NO_MEMORY;
        }

        SavedBackupFileName->Buffer = (LPWSTR)((LPBYTE) SavedBackupFileName +
            sizeof(UNICODE_STRING));

        SavedBackupFileName->Length = LogFileName->Length;
        SavedBackupFileName->MaximumLength = (USHORT) StringLength;
        RtlMoveMemory(SavedBackupFileName->Buffer, LogFileName->Buffer,
            LogFileName->Length);
        SavedBackupFileName->Buffer[SavedBackupFileName->Length / sizeof(WCHAR)] =
            L'\0';

        //
        // This is the first user - RefCount gets incrememted below
        //
        pLogFile->RefCount          = 0;
        pLogFile->FileHandle        = NULL;
        pLogFile->LogFileName       = SavedBackupFileName;
        pLogFile->ConfigMaxFileSize = ELFFILESIZE(MaxFileSize);
        pLogFile->Retention         = Retention;
        pLogFile->ulLastPulseTime   = 0;
        pLogFile->logpLogPopup      = logpLogPopup;
        pLogFile->bFullAlertDone = FALSE;
        pLogFile->AutoBackupLogFiles = dwAutoBackup;

        //
        // Save away the default module name for this file
        //
        pLogFile->LogModuleName = ElfpAllocateBuffer(
            sizeof(UNICODE_STRING) + ModuleName->MaximumLength);

        //
        // This flag can be set since pLogfile->LogModuleName
        // will be initialized after this point
        //
        bAllocatedLogInfo = TRUE;

        if (pLogFile->LogModuleName == NULL)
        {
            ELF_LOG1(ERROR,
                     "SetupDataStruct: Unable to allocate module name for %ws log\n",
                     LogFileName->Buffer);

            Status = STATUS_NO_MEMORY;
            goto ErrorExit;
        }

        pLogFile->LogModuleName->MaximumLength = ModuleName->MaximumLength;
        pLogFile->LogModuleName->Buffer =
            (LPWSTR)(pLogFile->LogModuleName + 1);
        RtlCopyUnicodeString(pLogFile->LogModuleName, ModuleName);

        InitializeListHead (&pLogFile->Notifiees);

        pLogFile->NextClearMaxFileSize = pLogFile->ConfigMaxFileSize;

        Status = ElfpInitResource(&pLogFile->Resource);

        if (!NT_SUCCESS(Status))
        {
            ELF_LOG1(ERROR,
                     "SetupDataStruct: Unable to init resource for %ws log\n",
                     LogFileName->Buffer);

            goto ErrorExit;
        }

        LinkLogFile ( pLogFile );   // Link it in

        Progress |= LOGFILE_LINKED;

    } // endif (pLogfile == NULL)

    //--------------------------------------
    // ADD THE MODULE TO THE LOG MODULE LIST
    //--------------------------------------
    // Set up the module data structure for the default (which is
    // the same as the logfile keyname).
    //

    pLogFile->RefCount++;
    pModule->LogFile = pLogFile;
    pModule->ModuleName = (LPWSTR) ModuleName->Buffer;

    Status = RtlUnicodeStringToAnsiString (
                    &ModuleNameA,
                    ModuleName,
                    TRUE);

    if (!NT_SUCCESS(Status))
    {
        ELF_LOG2(ERROR,
                 "SetupDataStruct: Unable to convert module name %ws to Ansi %#x\n",
                 ModuleName->Buffer,
                 Status);

        pLogFile->RefCount--;
        goto ErrorExit;
    }

    //
    // Link the new module in.
    //

    LinkLogModule(pModule, &ModuleNameA);

    RtlFreeAnsiString (&ModuleNameA);

    Progress |= MODULE_LINKED;

    //
    // Open up the file and map it to memory.  Impersonate the
    // caller so we can use UNC names
    //

    if (LogType == ElfBackupLog)
    {
        Status = RpcImpersonateClient(NULL);

        if (Status == RPC_S_OK)
        {
            Status = ElfOpenLogFile (pLogFile, LogType);
            RpcRevertToSelf();
        }
        else
        {
            ELF_LOG1(ERROR,
                     "SetupDataStruct: RpcImpersonateClient failed %#x\n",
                     Status);
        }
    }
    else
    {
        Status = ElfOpenLogFile (pLogFile, LogType);
    }

    if (!NT_SUCCESS(Status))
    {
        ELF_LOG3(ERROR,
                 "SetupDataStruct: Couldn't open %ws for module %ws %#x\n",
                 LogFileName->Buffer,
                 ModuleName->Buffer,
                 Status);

        if (LogType != ElfBackupLog)
        {
            ElfpCreateQueuedAlert(ALERT_ELF_LogFileNotOpened,
                                  1,
                                  &(ModuleName->Buffer));
        }

        pLogFile->RefCount--;
        goto ErrorExit;
    }

    Progress |= LOGFILE_OPENED;

    //
    // If this is the application module, remember the pointer
    // to use if a module doesn't have an entry in the registry
    //

    if (!_wcsicmp(ModuleName->Buffer, ELF_DEFAULT_MODULE_NAME))
    {
        OldDefaultLogModule = ElfDefaultLogModule;
        ElfDefaultLogModule = pModule;
    }

    //
    // Create the security descriptor for this logfile.  Only
    // the system and security modules are secured against
    // reads and writes by world.  Also, make sure we never
    // pop up a "log full" message for the Security log -- this
    // would be a C2 violation.
    //

    if (!_wcsicmp(ModuleName->Buffer, ELF_SYSTEM_MODULE_NAME))
    {
        Type = ELF_LOGFILE_SYSTEM;
    }
    else if (!_wcsicmp(ModuleName->Buffer, ELF_SECURITY_MODULE_NAME))
    {
        Type                   = ELF_LOGFILE_SECURITY;
        pLogFile->logpLogPopup = LOGPOPUP_NEVER_SHOW;
    }
    else
    {
        Type = ELF_LOGFILE_APPLICATION;
    }

    //
    // Create a Security Descriptor for this Logfile
    //   (RtlDeleteSecurityObject() can be used to free
    //    pLogFile->Sd).
    //
    Status = ElfpCreateLogFileObject(pLogFile, Type, GuestAccessRestriction);

    if (!NT_SUCCESS(Status))
    {
        ELF_LOG2(ERROR,
                 "SetupDataStruct: Unable to create SD for log %ws %#x\n",
                 ModuleName->Buffer,
                 Status);

        pLogFile->RefCount--;
        goto ErrorExit;
    }

    //
    // Now that we've added the default module name, see if there are any
    // modules configured to log to this file, and if so, create the module
    // structures for them.
    //

    SetUpModules(hLogFile, pLogFile, FALSE);

    return STATUS_SUCCESS;

ErrorExit:

    if (Progress & LOGFILE_OPENED)
    {
        ElfpCloseLogFile(pLogFile, ELF_LOG_CLOSE_BACKUP);
    }

    if (Progress & MODULE_LINKED)
    {
        UnlinkLogModule(pModule);
        DeleteAtom(pModule->ModuleAtom);
    }

    if (bAllocatedLogInfo)
    {
        if (Progress & LOGFILE_LINKED)
        {
            UnlinkLogFile(pLogFile);
            RtlDeleteResource (&pLogFile->Resource);
        }

        ElfpFreeBuffer(pLogFile->LogModuleName);
        ElfpFreeBuffer(SavedBackupFileName);
        ElfpFreeBuffer(pLogFile);
    }

    ElfpFreeBuffer(pModule);

    if (OldDefaultLogModule != NULL)
    {
        ElfDefaultLogModule = OldDefaultLogModule;
    }

    return Status;
}


NTSTATUS
SetUpModules(
    HANDLE      hLogFile,
    PLOGFILE    pLogFile,
    BOOLEAN     bAllowDupes
    )
/*++

Routine Description:

    This routine sets up the information for all modules for a logfile.

    The subkeys under a logfile in the eventlog portion of the registry
    are enumerated.  For each unique subkey, a LOGMODULE structure is
    created.  Each new structures is added to a linked list
    of modules for that logfile.

    If there was one or more unique subkeys, meaning the list has changed
    since we last looked, then we go through the entire linked list of
    log modules, and create a MULTI_SZ list of all the modules.  This list
    is stored in the Sources value for that logfile for the event viewer
    to use.

    NOTE:  A module is never un-linked from the linked list of log modules
    even if the registry subkey for it is removed.  This should probably
    be done sometime.  It would make the eventlog more robust.

Arguments:

    hLogFile    - Registry key for the Log File node
    pLogFile    - pointer to the log file structure
    bAllowDupes - If true, it's ok to already have a module with the same
                  name (used when processing change notify of registry)

Return Value:

    NTSTATUS - If unsuccessful, it is not a fatal error.

        Even if this status is unsuccessful, me may have been able
        to store some of the new subkeys in the LogModule list.  Also, we
        may have been able to update the Sources MULTI_SZ list.

--*/
{
    NTSTATUS    Status = STATUS_SUCCESS;
    BYTE        Buffer[ELF_MAX_REG_KEY_INFO_SIZE];
    PKEY_NODE_INFORMATION KeyBuffer = (PKEY_NODE_INFORMATION) Buffer;
    ULONG       ActualSize;
    PWCHAR      SubKeyString;
    UNICODE_STRING NewModule;
    ANSI_STRING ModuleNameA;
    PLOGMODULE  pModule;
    ULONG       Index = 0;
    ATOM        Atom;
    PWCHAR      pList;
    DWORD       ListLength = 0;
    UNICODE_STRING ListName;
    BOOLEAN     ListChanged = FALSE;
    PLIST_ENTRY pListEntry;

    //
    // Create the module structures for all modules under this logfile.  We
    // don't actually need to open the key, since we don't use any information
    // stored there, it's existence is all we care about here.  Any data is
    // used by the Event Viewer (or any viewing app).  If this is used to
    // setup a backup file, hLogFile is NULL since there aren't any other
    // modules to map to this file.
    //

    while (NT_SUCCESS(Status) && hLogFile)
    {
        Status = NtEnumerateKey(hLogFile,
                                Index++,
                                KeyNodeInformation,
                                KeyBuffer,
                                ELF_MAX_REG_KEY_INFO_SIZE,
                                &ActualSize);

        if (NT_SUCCESS(Status))
        {
            //
            // It turns out the Name isn't null terminated, so we need
            // to copy it somewhere and null terminate it before we use it
            //

            SubKeyString = ElfpAllocateBuffer(KeyBuffer->NameLength + sizeof(WCHAR));

            if (!SubKeyString)
            {
                return STATUS_NO_MEMORY;
            }

            memcpy(SubKeyString, KeyBuffer->Name, KeyBuffer->NameLength);
            SubKeyString[KeyBuffer->NameLength / sizeof(WCHAR)] = L'\0' ;

            //
            // Add the atom for this module name
            //

            RtlInitUnicodeString(&NewModule, SubKeyString);

            Status = RtlUnicodeStringToAnsiString (
                            &ModuleNameA,
                            &NewModule,
                            TRUE);

            if (!NT_SUCCESS(Status))
            {
                //
                // We can't continue, so we will leave the modules
                // we've linked so far, and move on in an attempt to
                // create the Sources MULTI_SZ list.
                //
                ELF_LOG1(TRACE,
                         "SetUpModules: Unable to convert name for module %ws\n",
                         SubKeyString);

                ElfpFreeBuffer(SubKeyString);
                break;
            }

            Atom = FindAtomA(ModuleNameA.Buffer);

            //
            // Make sure we've not already added one by this name
            //

            if (FindModuleStrucFromAtom(Atom))
            {
                //
                // We've already encountered a module by this name.  If
                // this is init time, it's a configuration error.  Report
                // it and move on.  If we're processing a change notify
                // from the registry, this is ok (it means we're rescanning
                // an existing Event Source for an existing log).
                //
                if (!bAllowDupes)
                {
                    ELF_LOG1(ERROR,
                             "SetUpModules: Module %ws exists in two log files.\n",
                             SubKeyString);
                }

                RtlFreeAnsiString(&ModuleNameA);
                ElfpFreeBuffer(SubKeyString);
                continue;
            }

            ListChanged = TRUE;

            pModule  = ElfpAllocateBuffer (sizeof (LOGMODULE) );

            if (!pModule)
            {
                ELF_LOG1(ERROR,
                         "SetUpModules: Unable to allocate structure for module %ws\n",
                         SubKeyString);

                RtlFreeAnsiString (&ModuleNameA);
                ElfpFreeBuffer(SubKeyString);
                return(STATUS_NO_MEMORY);
            }

            //
            // Set up a module data structure for this module
            //

            pModule->LogFile = pLogFile;
            pModule->ModuleName = SubKeyString;

            //
            // Link the new module in.
            //

            LinkLogModule(pModule, &ModuleNameA);

            ELF_LOG1(TRACE,
                     "SetUpModules: Module %ws successfully created/linked\n",
                     SubKeyString);

            RtlFreeAnsiString (&ModuleNameA);
        }
    }

    if (Status == STATUS_NO_MORE_ENTRIES)
    {
        //
        // It's not required that there are configured modules for a log
        // file.
        //

        Status = STATUS_SUCCESS;
    }

    //
    // If the list has changed, or if we've been called during init, and not
    // as the result of a changenotify on the registry (bAllowDupes == FALSE)
    // then create the sources key
    //

    if (hLogFile && (ListChanged || !bAllowDupes))
    {
        //
        // Now create a MULTI_SZ entry with all the module names for eventvwr
        //
        // STEP 1: Calculate amount of storage needed by running thru the
        //         module list, finding any module that uses this log file.
        //
        pListEntry = LogModuleHead.Flink;

        while (pListEntry != &LogModuleHead)
        {
            pModule = CONTAINING_RECORD (pListEntry, LOGMODULE, ModuleList);

            if (pModule->LogFile == pLogFile)
            {
                //
                // This one is for the log we're working on, get the
                // size of its name.
                //
                ListLength += WCSSIZE(pModule->ModuleName);

                ELF_LOG2(MODULES,
                         "SetUpModules: Adding module %ws to list for %ws log\n",
                         pModule->ModuleName,
                         pLogFile->LogFileName->Buffer);
            }

            pListEntry = pModule->ModuleList.Flink;
        }

        //
        // STEP 2:  Allocate storage for the MULTI_SZ.
        //
        pList = ElfpAllocateBuffer(ListLength + sizeof(WCHAR));

        //
        // If I can't allocate the list, just press on
        //

        if (pList)
        {
            //
            // STEP 3: Copy all the module names for this logfile into
            //         the MULTI_SZ string.
            //
            SubKeyString = pList; // Save this away

            pListEntry = LogModuleHead.Flink;

            while (pListEntry != &LogModuleHead)
            {
                pModule = CONTAINING_RECORD(pListEntry,
                                            LOGMODULE,
                                            ModuleList);

                if (pModule->LogFile == pLogFile)
                {
                    //
                    // This one is for the log we're working on, put it in the list
                    //

                    wcscpy(pList, pModule->ModuleName);
                    pList += wcslen(pModule->ModuleName);
                    pList++;
                }

                pListEntry = pModule->ModuleList.Flink;
            }

            *pList = L'\0'; // The terminating NULL

            RtlInitUnicodeString(&ListName, L"Sources");

            Status = NtSetValueKey(hLogFile,
                                   &ListName,
                                   0,
                                   REG_MULTI_SZ,
                                   SubKeyString,
                                   ListLength + sizeof(WCHAR));

            ElfpFreeBuffer(SubKeyString);
        }
        else
        {
            ELF_LOG1(ERROR,
                     "SetUpModules: Unable to allocate list for %ws log\n",
                     pLogFile->LogFileName->Buffer);
        }
    }

    return Status;
}


NTSTATUS
ElfSetUpConfigDataStructs(
    VOID
    )

/*++

Routine Description:

    This routine sets up all the necessary data structures for the eventlog
    service.  It enumerates the keys in the Logfiles registry node to
    determine what to setup.

Arguments:

    NONE

Return Value:

    NONE

Note:


--*/
{
    NTSTATUS Status = STATUS_SUCCESS;
    HANDLE hLogFile;
    OBJECT_ATTRIBUTES ObjectAttributes;
    UNICODE_STRING SubKeyName;
    PUNICODE_STRING pLogFileName = NULL;
    PUNICODE_STRING pModuleName = NULL;
    UNICODE_STRING EventlogModuleName;
    UNICODE_STRING EventlogSecModuleName;
    ULONG Index = 0;
    BYTE Buffer[ELF_MAX_REG_KEY_INFO_SIZE];
    PKEY_NODE_INFORMATION KeyBuffer = (PKEY_NODE_INFORMATION) Buffer;
    ULONG ActualSize;
    LOG_FILE_INFO LogFileInfo;
    PWCHAR SubKeyString;
    LPWSTR ModuleName;

    ELF_LOG0(TRACE,
             "ElfSetUpConfigDataStructs: Entering\n");

    //
    // Initialize the Atom table whose size is the maximum number of
    // module structures possible, i.e. ELF_MAX_LOG_MODULES.
    //
    if (!InitAtomTable(ELF_MAX_LOG_MODULES))
    {
        return STATUS_UNSUCCESSFUL;
    }

    //
    // Get a handle to the Logfiles subkey.  If it doesn't exist, just use
    // the hard-coded defaults.
    //

    if (hEventLogNode)
    {
        //
        // Loop thru the subkeys under Eventlog and set up each logfile
        //

        while (NT_SUCCESS(Status))
        {
            Status = NtEnumerateKey(hEventLogNode,
                                    Index++,
                                    KeyNodeInformation,
                                    KeyBuffer,
                                    ELF_MAX_REG_KEY_INFO_SIZE,
                                    &ActualSize);

            if (NT_SUCCESS(Status))
            {
                //
                // It turns out the Name isn't null terminated, so we need
                // to copy it somewhere and null terminate it before we use it
                //

                SubKeyString = ElfpAllocateBuffer(KeyBuffer->NameLength + sizeof(WCHAR));

                if (!SubKeyString)
                {
                    return STATUS_NO_MEMORY;
                }

                memcpy(SubKeyString, KeyBuffer->Name, KeyBuffer->NameLength);
                SubKeyString[KeyBuffer->NameLength / sizeof(WCHAR)] = L'\0';

                //
                // Open the node for this logfile and extract the information
                // required by SetupDataStruct, and then call it.
                //

                RtlInitUnicodeString(&SubKeyName, SubKeyString);

                InitializeObjectAttributes(&ObjectAttributes,
                                           &SubKeyName,
                                           OBJ_CASE_INSENSITIVE,
                                           hEventLogNode,
                                           NULL);

                Status = NtOpenKey(&hLogFile,
                                   KEY_READ | KEY_SET_VALUE,
                                   &ObjectAttributes);

                if (!NT_SUCCESS(Status))
                {
                    //
                    // Unclear how this could happen since I just enum'ed
                    // it, but if I can't open it, I just pretend like it
                    // wasn't there to begin with.
                    //
                    ELF_LOG1(TRACE,
                             "ElfSetUpConfigDataStructs: Unable to open key for %ws log\n",
                             SubKeyName);

                    ElfpFreeBuffer(SubKeyString);
                    Status = STATUS_SUCCESS; // so we don't terminate the loop
                    continue;
                }

                //
                // Get the information from the registry.  Note that we have to
                // initialize the "log full" popup policy before doing so since
                // ReadRegistryInfo will compare the value found in the registry
                // (if there is one) to the current value.
                //

                LogFileInfo.logpLogPopup = IS_WORKSTATION() ? LOGPOPUP_NEVER_SHOW :
                                                              LOGPOPUP_CLEARED;

                Status = ReadRegistryInfo(hLogFile,
                                          &SubKeyName,
                                          &LogFileInfo);

                if (NT_SUCCESS(Status))
                {
                    //
                    // Now set up the actual data structures.  Failures are
                    // dealt with in the routine.  Note that the check for
                    // the security log (i.e., for LOGPOPUP_NEVER_SHOW) is
                    // made in SetUpDataStruct
                    //

                    SetUpDataStruct(LogFileInfo.LogFileName,
                                    LogFileInfo.MaxFileSize,
                                    LogFileInfo.Retention,
                                    LogFileInfo.GuestAccessRestriction,
                                    &SubKeyName,
                                    hLogFile,
                                    ElfNormalLog,
                                    LogFileInfo.logpLogPopup,
                                    LogFileInfo.dwAutoBackup);

                    NtClose(hLogFile);

                }
                else
                {
                    ELF_LOG1(ERROR,
                             "ElfSetUpConfigdataStructs: ReadRegistryInfo failed %#x\n",
                             Status);
                }
            }
        }
    } // if (hEventLogNode)
    else
    {
        LOGPOPUP  logpLogPopup = IS_WORKSTATION() ? LOGPOPUP_NEVER_SHOW :
                                                    LOGPOPUP_CLEARED;

        //
        // The information doesn't exist in the registry, set up the
        // three default logs.
        //
        pLogFileName = ElfpAllocateBuffer(sizeof(UNICODE_STRING));
        pModuleName =  ElfpAllocateBuffer(sizeof(UNICODE_STRING));

        if (!pLogFileName || !pModuleName)
        {
            ElfpFreeBuffer(pLogFileName);
            ElfpFreeBuffer(pModuleName);
            return STATUS_NO_MEMORY;
        }

        //
        // Application log
        //
        RtlInitUnicodeString(pLogFileName, ELF_APPLICATION_DEFAULT_LOG_FILE);
        RtlInitUnicodeString(pModuleName,  ELF_DEFAULT_MODULE_NAME);

        //
        // On success, don't free pModuleName as the pointer to it
        // is stored away in the LogFile struct
        //
        Status = SetUpDataStruct(pLogFileName,
                                 ELF_DEFAULT_MAX_FILE_SIZE,
                                 ELF_DEFAULT_RETENTION_PERIOD,
                                 ELF_GUEST_ACCESS_UNRESTRICTED,
                                 pModuleName,
                                 NULL,
                                 ElfNormalLog,
                                 logpLogPopup,
                                ELF_DEFAULT_AUTOBACKUP);

        ElfpFreeBuffer(pLogFileName);
        pLogFileName = NULL;

        if (!NT_SUCCESS(Status))
        {
            ELF_LOG1(ERROR,
                     "ElfSetUpConfigDatastructs: Unable to set up %ws log\n",
                     ELF_DEFAULT_MODULE_NAME);

            ElfpFreeBuffer(pModuleName);
            pModuleName = NULL;
        }

        pLogFileName = ElfpAllocateBuffer(sizeof(UNICODE_STRING));
        pModuleName =  ElfpAllocateBuffer(sizeof(UNICODE_STRING));

        if (!pLogFileName || !pModuleName)
        {
            ElfpFreeBuffer(pLogFileName);
            ElfpFreeBuffer(pModuleName);
            return(STATUS_NO_MEMORY);
        }

        //
        // System log
        //
        RtlInitUnicodeString(pLogFileName, ELF_SYSTEM_DEFAULT_LOG_FILE);
        RtlInitUnicodeString(pModuleName,  ELF_SYSTEM_MODULE_NAME);

        Status = SetUpDataStruct(pLogFileName,
                                 ELF_DEFAULT_MAX_FILE_SIZE,
                                 ELF_DEFAULT_RETENTION_PERIOD,
                                 ELF_GUEST_ACCESS_UNRESTRICTED,
                                 pModuleName,
                                 NULL,
                                 ElfNormalLog,
                                 logpLogPopup,
                                ELF_DEFAULT_AUTOBACKUP);

        ElfpFreeBuffer(pLogFileName);
        pLogFileName = NULL;

        if (!NT_SUCCESS(Status))
        {
            ELF_LOG1(ERROR,
                     "ElfSetUpConfigDatastructs: Unable to set up %ws log\n",
                     ELF_SYSTEM_MODULE_NAME);

            ElfpFreeBuffer(pModuleName);
            pModuleName = NULL;
        }

        pLogFileName = ElfpAllocateBuffer(sizeof(UNICODE_STRING));
        pModuleName  = ElfpAllocateBuffer(sizeof(UNICODE_STRING));

        if (!pLogFileName || !pModuleName)
        {
            ElfpFreeBuffer(pLogFileName);
            ElfpFreeBuffer(pModuleName);
            return(STATUS_NO_MEMORY);
        }

        //
        // Security log
        //
        RtlInitUnicodeString(pLogFileName, ELF_SECURITY_DEFAULT_LOG_FILE);
        RtlInitUnicodeString(pModuleName,  ELF_SECURITY_MODULE_NAME);

        Status = SetUpDataStruct(pLogFileName,
                                 ELF_DEFAULT_MAX_FILE_SIZE,
                                 ELF_DEFAULT_RETENTION_PERIOD,
                                 ELF_GUEST_ACCESS_UNRESTRICTED,
                                 pModuleName,
                                 NULL,
                                 ElfNormalLog,
                                 LOGPOPUP_NEVER_SHOW,
                                ELF_DEFAULT_AUTOBACKUP);    // Never popup for the security log

        ElfpFreeBuffer(pLogFileName);
        pLogFileName = NULL;

        if (!NT_SUCCESS(Status))
        {
            ELF_LOG1(ERROR,
                     "ElfSetUpConfigDatastructs: Unable to set up %ws log\n",
                     ELF_SECURITY_MODULE_NAME);

            ElfpFreeBuffer(pModuleName);
            pModuleName = NULL;
        }

        Status = STATUS_SUCCESS;
    }

    //
    // If we just ran out of keys, that's OK (unless there weren't any at all)
    //
    if (Status == STATUS_NO_MORE_ENTRIES && Index != 1)
    {
        Status = STATUS_SUCCESS;
    }

    if (NT_SUCCESS(Status))
    {
        //
        // Make sure we created the Application log file, since it is the
        // default.  If it wasn't created, use the first module created
        // (this is at the tail of the list since I insert them at the
        // head).  If this happens, send an alert to the admin.
        //

        if (!ElfDefaultLogModule)
        {
            ELF_LOG0(ERROR,
                     "ElfSetUpConfigDatastructs: No Application module -- creating default\n");

            if (IsListEmpty(&LogModuleHead))
            {
                //
                // No logs were created, might as well shut down
                //
                ELF_LOG0(ERROR,
                         "ElfSetUpConfigDatastructs: No logs created -- exiting\n");

                return STATUS_EVENTLOG_CANT_START;
            }

            ElfDefaultLogModule = CONTAINING_RECORD(LogModuleHead.Blink,
                                                    LOGMODULE,
                                                    ModuleList);

            ModuleName = ELF_DEFAULT_MODULE_NAME;

            ELF_LOG2(ERROR,
                     "ElfSetUpConfigDatastructs: Using file/default %ws/%ws as default log\n",
                     ElfDefaultLogModule->LogFile->LogFileName->Buffer,
                     ElfDefaultLogModule->LogFile->LogModuleName->Buffer);

            ElfpCreateQueuedAlert(ALERT_ELF_DefaultLogCorrupt,
                                  1,
                                  &(ElfDefaultLogModule->LogFile->LogModuleName->Buffer));
        }

        //
        // Now get the Module for the Eventlog service to use.  GetModuleStruc
        // always succeeds, returning the default log if the requested one
        // isn't configured.
        //

        RtlInitUnicodeString(&EventlogModuleName, L"eventlog");
        ElfModule = GetModuleStruc(&EventlogModuleName);
        RtlInitUnicodeString(&EventlogSecModuleName, L"SECURITY");
        ElfSecModule = GetModuleStruc(&EventlogSecModuleName);
        
    }

    return Status;
}


VOID
ElfWriteTimeStamp(
    TIMESTAMPEVENT  EventType,
    BOOLEAN         CheckPreviousStamp
    )
/*++

Routine Description:

    This routine writes a time stamp in the form of a systemtime structure
    to the registry which is then used to extract reliability data.

Arguments:

    EventType          - Indicates what type of event we are logging
    CheckPreviousStamp - Whether we should check for the existance of a previous
                         time stamp which indicates a prior system crash.
Return Value:

    NONE

Note:


--*/
{
    SYSTEMTIME  stCurrentUTCTime;
    SYSTEMTIME  stPreviousUTCTime;
    SYSTEMTIME  stPreviousLocalTime;
    DWORD dwDirtyFlag = 1;

    HKEY        hKey;
    LONG        rc;
    DWORD       ValueSize;
    ULONG       Interval = DEFAULT_INTERVAL;
    ULONG       wchars;
    LPWSTR      DateTimeBuffer[2];

    rc = RegCreateKeyEx(HKEY_LOCAL_MACHINE,
                        REGSTR_PATH_RELIABILITY,
                        0,
                        NULL,
                        REG_OPTION_NON_VOLATILE,
                        KEY_ALL_ACCESS,
                        NULL,
                        &hKey,
                        NULL);

    if (rc != ERROR_SUCCESS)
    {
        return;
    }

    if (EventType == EVENT_NormalShutdown)
    {
        //
        // Delete the time stamp registry value, this is how we indicate a clean shutdown
        //
        RegDeleteValue(hKey, REGSTR_VAL_LASTALIVESTAMP);
        RegFlushKey(hKey);
        RegCloseKey(hKey);
        return;
    }

    //
    // Get the current UTC time
    //

    GetSystemTime(&stCurrentUTCTime);

    if (CheckPreviousStamp)
    {
        ValueSize = sizeof(SYSTEMTIME);

        rc = RegQueryValueEx(hKey,
                             REGSTR_VAL_LASTALIVESTAMP,
                             0,
                             NULL,
                             (PUCHAR) &stPreviousUTCTime,
                             &ValueSize);

        //
        // If we can successfully read a systemtime structure it indicates
        // that the previous shutdown was abnormal, i.e. we didn't execute
        // or normal shutdown cleanup code.
        //

        //
        // Format the time and date of the crash time stamp
        // appropriately for the locale and log a #6008 event
        //


        if ((rc == ERROR_SUCCESS) && (ValueSize == sizeof(SYSTEMTIME)))
        {
            SYSTEMTIME  lpData[2];          // Data for the event

            if (!SystemTimeToTzSpecificLocalTime(NULL,
                                                 &stPreviousUTCTime,
                                                 &stPreviousLocalTime))
            {
                //
                // Couldn't convert to the active time zone -- use UTC
                //
                stPreviousLocalTime = stPreviousUTCTime;
            }

            //
            // Write the local time and the UTC time for the "last alive"
            // timestamp since NT4SP5 shipped with only the local time
            // as the event data.  This allows tools that work on NT4SP5
            // to continue working on NT5.
            //
            lpData[0] = stPreviousLocalTime;
            lpData[1] = stPreviousUTCTime;

            wchars = GetTimeFormat(LOCALE_SYSTEM_DEFAULT,
                                   0,
                                   &stPreviousLocalTime,
                                   NULL,
                                   NULL,
                                   0);

            DateTimeBuffer[0] = ElfpAllocateBuffer(wchars * sizeof(WCHAR));

            if (DateTimeBuffer[0])
            {
                GetTimeFormat(LOCALE_SYSTEM_DEFAULT,
                              0,
                              &stPreviousLocalTime,
                              NULL,
                              DateTimeBuffer[0],
                              wchars);

                wchars = GetDateFormat(LOCALE_SYSTEM_DEFAULT,
                                       0,
                                       &stPreviousLocalTime,
                                       NULL,
                                       NULL,
                                       0);

                DateTimeBuffer[1] = ElfpAllocateBuffer(wchars * sizeof(WCHAR));

                if (DateTimeBuffer[1])
                {
                    GetDateFormat(LOCALE_SYSTEM_DEFAULT,
                                  0,
                                  &stPreviousLocalTime,
                                  NULL,
                                  DateTimeBuffer[1],
                                  wchars);

                    ElfpCreateElfEvent(
                        EVENT_EventlogAbnormalShutdown,
                        EVENTLOG_ERROR_TYPE,
                        0,                        // EventCategory
                        2,                        // NumberOfStrings
                        DateTimeBuffer,           // Strings
                        lpData,                   // "Last alive" times
                        2 * sizeof(SYSTEMTIME),   // Datalength
                        0,
                        FALSE);                       // flags

                    ElfpFreeBuffer(DateTimeBuffer[1]);
			        RegSetValueEx(hKey,
                     L"DirtyShutDown",
                     0,
                     REG_DWORD,
                     (PUCHAR) &dwDirtyFlag,
                     sizeof(DWORD));
                }

                ElfpFreeBuffer(DateTimeBuffer[0]);

            }
        }
    }

    //
    // Set the current time stamp
    //
    RegSetValueEx(hKey,
                  REGSTR_VAL_LASTALIVESTAMP,
                  0,
                  REG_BINARY,
                  (PUCHAR) &stCurrentUTCTime,
                  sizeof(SYSTEMTIME));

    RegFlushKey (hKey);
    RegCloseKey (hKey);
}


VOID
ElfWriteProductInfoEvent (
    VOID
    )
/*++

Routine Description:

    This function writes an event #6009 which includes the OS version, build #,
    service pack level, MP/UP, and Free/Checked.

Arguments:

    NONE

Return Value:

    NONE

Note:


--*/

{

#define NUM_INFO_VALUES     4  //EVENT_EventLogProductInfo requires 4 parameters
#define NUM_VERSION_SIZE    10 //Digits in a DWORD

    NTSTATUS        Status      = STATUS_SUCCESS;
    HKEY            hKey        = NULL;
    ULONG           ValueSize   = 0;
    LPWSTR          NullString  = L"";

    LPWSTR          StringBuffers[NUM_INFO_VALUES] = {NULL, NULL, NULL, NULL};
    
    OSVERSIONINFOEX OsVersion;
    WCHAR           wszTemp[NUM_VERSION_SIZE];
    UINT            i;


    OsVersion.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    if( !GetVersionEx( (LPOSVERSIONINFO)&OsVersion ) )
    {
        return;
    }
    
    //Allocate storage

    //Buffer 0 holds the version number in the format of 5.xx.
    StringBuffers[0] = ElfpAllocateBuffer( (2*NUM_VERSION_SIZE + 2) * sizeof(WCHAR) );

    //Buffer 1 holds the build number
    StringBuffers[1] = ElfpAllocateBuffer( (NUM_VERSION_SIZE)       * sizeof(WCHAR) );

    //Buffer 2 holds the service pack
    StringBuffers[2] = ElfpAllocateBuffer( sizeof(OsVersion.szCSDVersion)           );

    if( StringBuffers[0] == NULL || 
        StringBuffers[1] == NULL || 
        StringBuffers[2] == NULL )
    {
        goto ErrorExit;
    }

    //
    //Add major version
    //
    _ltow (
        OsVersion.dwMajorVersion,
        wszTemp,
        10
    );

    wcscpy( StringBuffers[0], wszTemp );
    wcscat( StringBuffers[0], L"."    );

    //
    //Add minor version
    //
    _ltow (
        OsVersion.dwMinorVersion,
        wszTemp,
        10
    );

    if( OsVersion.dwMinorVersion < 10 )
    {
        wcscat( StringBuffers[0], L"0" );
    }

    wcscat( StringBuffers[0], wszTemp );
    wcscat( StringBuffers[0], L"."    );

    //
    //Get build number
    //
    _ltow (
        OsVersion.dwBuildNumber,
        wszTemp,
        10
    );

    wcscpy( StringBuffers[1], wszTemp );

    //Get service pack info
    wcscpy( StringBuffers[2], OsVersion.szCSDVersion );

    //
    // Get OS type (uniprocessor or multiprocessor chk or free)
    // Open HKLM\Software\Microsoft\Windows NT\CurrentVersion
    //
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                     REGSTR_PATH_NT_CURRENTVERSION,
                     0,
                     KEY_ALL_ACCESS,
                     &hKey)

        != ERROR_SUCCESS)
    {
        goto ErrorExit;
    }

    //
    // For each of the registry values, query for the string size, allocate storage,
    // and query the actual value
    //
    if ((RegQueryValueEx (hKey,
                          REGSTR_VAL_CURRENT_TYPE,
                          0,
                          NULL,
                          NULL,
                          &ValueSize)
         == ERROR_SUCCESS)

         &&

         ValueSize != 0)
    {
        StringBuffers[3] = ElfpAllocateBuffer(ValueSize);

        if (StringBuffers[3] != NULL) 
            {

                RegQueryValueEx(hKey,
                                REGSTR_VAL_CURRENT_TYPE,
                                0,
                                NULL,
                                (PUCHAR) StringBuffers[3],
                                &ValueSize);
    
                ValueSize = 0;
            }
    }
    else
    {
        StringBuffers[3] = NullString;
    }

    ElfpCreateElfEvent(
        EVENT_EventLogProductInfo,
        EVENTLOG_INFORMATION_TYPE,
        0,                            // EventCategory
        NUM_INFO_VALUES,              // NumberOfStrings
        StringBuffers,                // Strings
        NULL,                         // EventData
        0,                            // Datalength
        0,
        FALSE);                           // flags

    
ErrorExit:

    for (i = 0; i < NUM_INFO_VALUES-1; i++)
    {
        if (StringBuffers[i] != NULL && StringBuffers[i] != NullString)
        {
            ElfpFreeBuffer(StringBuffers[i]);
        }
    }

    if( hKey != NULL )
    {
        RegCloseKey (hKey);
    }

#undef NUM_INFO_VALUES
#undef NUM_VERSION_SIZE

}


VOID
TimeStampProc(
    PVOID   Interval,
    BOOLEAN fWaitStatus
    )
{
    NTSTATUS ntStatus;
    HANDLE   hWaitHandle;
    ULONG    ValueSize;
    HKEY     hKey;
    ULONG    NewInterval;
    ULONG    rc;

    //
    // Deregister the wait (note that we must do this even
    // if the WT_EXECUTEONLYONCE flag is set)
    //
    ntStatus = RtlDeregisterWait(g_hTimestampWorkitem);

    if (!NT_SUCCESS(ntStatus))
    {
        ELF_LOG1(ERROR,
                 "TimeStampProc: RtlDeregister wait failed %#x\n",
                  ntStatus);
    }

    if (fWaitStatus == FALSE)
    {
        //
        // The event log service is stopping
        //

        return;
    }

    //
    // Note:  NewInterval is specified in minutes
    //
    NewInterval = (ULONG)((ULONG_PTR)Interval);

    //
    //  The event timed out -- write a timestamp
    //

    ElfWriteTimeStamp (EVENT_AbNormalShutdown, FALSE);

    //
    // recheck the time stamp interval value
    //

    rc = RegCreateKeyEx(HKEY_LOCAL_MACHINE,
                        REGSTR_PATH_RELIABILITY,
                        0,
                        NULL,
                        REG_OPTION_NON_VOLATILE,
                        KEY_ALL_ACCESS,
                        NULL,
                        &hKey,
                        NULL);

    if ( rc == ERROR_SUCCESS )
    {
        ValueSize = sizeof (ULONG);

        rc = RegQueryValueEx(hKey,
                             REGSTR_VAL_LASTALIVEINTERVAL,
                             0,
                             NULL,
                             (PUCHAR) &NewInterval,
                             &ValueSize);

        if ( rc != ERROR_SUCCESS )
        {
            //
            // Couldn't get the value -- stop timestamping
            //
            return;
        }

        RegCloseKey (hKey);
    }

    if (NewInterval != 0)
    {
        //
        // Reregister the wait
        //

        ntStatus = RtlRegisterWait(&g_hTimestampWorkitem,
                                   g_hTimestampEvent,
                                   TimeStampProc,           // Callback
                                   (PVOID) UlongToPtr(NewInterval),     // Context
                                   NewInterval * 60 * 1000, // Timeout, in ms
                                   WT_EXECUTEONLYONCE);
    }

    if (!NT_SUCCESS(ntStatus))
    {
        ELF_LOG1(ERROR,
                 "TimeStampProc: RtlRegisterWait failed %#x\n",
                 ntStatus);
    }
}


NTSTATUS EnsureComputerName(
	)
/*++

Routine Description:

    This routine ensures that the computer name.

Arguments:

Return Value:

    status value, STATUS_SUCCESS if all is well.

--*/

{
    NTSTATUS           Status;
    UNICODE_STRING     ValueName;
    ULONG              ulActualSize;
	WCHAR wComputerName[MAX_COMPUTERNAME_LENGTH + 1];
	DWORD dwComputerNameLen = MAX_COMPUTERNAME_LENGTH + 1;
	DWORD dwLen;
	BOOL bRet;
	BYTE            Buffer[ELF_MAX_REG_KEY_INFO_SIZE];
    PKEY_VALUE_PARTIAL_INFORMATION ValueBuffer =
        (PKEY_VALUE_PARTIAL_INFORMATION) Buffer;
    RtlInitUnicodeString(&ValueName, VALUE_COMPUTERNAME);

	// Determine if there is a String under the eventlog key that
	// contains the current name.

    Status = NtQueryValueKey(hEventLogNode,
                             &ValueName,
                             KeyValuePartialInformation,
                             ValueBuffer,
                             ELF_MAX_REG_KEY_INFO_SIZE,
                             &ulActualSize);
	
    if (NT_SUCCESS(Status))
    {
        if(ValueBuffer->DataLength != 0)
        	return STATUS_SUCCESS;	// all is well, there is already a string
    }

	// Get the computer name and write it

    bRet = GetComputerName(wComputerName, &dwComputerNameLen);
    if(bRet == FALSE)
    {
        ELF_LOG1(ERROR,
                 "EnsureComputerName: GetComputerName failed %#x\n",
                 GetLastError());
		return STATUS_UNSUCCESSFUL;
    }

	// calc size in byte including null

	dwLen = sizeof(WCHAR) * (dwComputerNameLen + 1);
    Status = NtSetValueKey(hEventLogNode,
                                   &ValueName,
                                   0,
                                   REG_SZ,
                                   wComputerName,
                                   dwLen);
	
    if (!NT_SUCCESS(Status))
        ELF_LOG1(ERROR,
                 "EnsureComputerName: NtSetValueKey failed %#x\n",
                 Status);
	return Status;
}

VOID
SvcEntry_Eventlog(
    DWORD               argc,
    LPWSTR              argv[],
    PSVCS_GLOBAL_DATA   SvcsGlobalData,
    HANDLE              SvcRefHandle
    )

/*++

Routine Description:

    This is the main routine for the Event Logging Service.

Arguments:

    Command-line arguments.

Return Value:

    NONE

--*/
{
    NTSTATUS           Status;
    OBJECT_ATTRIBUTES  ObjectAttributes;
    UNICODE_STRING     RootRegistryNode;
    UNICODE_STRING     ComputerNameRegistryNode;
    ULONG              Win32Error = NO_ERROR;
    ELF_REQUEST_RECORD FlushRequest;
    BYTE               Buffer[ELF_MAX_REG_KEY_INFO_SIZE];

    PKEY_VALUE_FULL_INFORMATION ValueBuffer = (PKEY_VALUE_FULL_INFORMATION) Buffer;

    SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;

    HKEY        hKey;
    ULONG       ValueSize = sizeof(ULONG);

#if DBG

    UNICODE_STRING     ValueName;
    ULONG              ulActualSize;

#endif  // DBG

    g_lNumSecurityWriters = 0;

    //
    // Set up the object that describes the root node for the eventlog service
    //
    RtlInitUnicodeString(&RootRegistryNode, REG_EVENTLOG_NODE_PATH);
    InitializeObjectAttributes(&ObjectAttributes,
                               &RootRegistryNode,
                               OBJ_CASE_INSENSITIVE,
                               NULL,
                               NULL);

    //
    // If this fails, we'll just use the defaults
    //
    Status = NtOpenKey(&hEventLogNode, KEY_READ | KEY_NOTIFY | KEY_SET_VALUE, &ObjectAttributes);
    if (NT_SUCCESS(Status))
    {
        Status = EnsureComputerName();
	    if (!NT_SUCCESS(Status))
	    {
	        //
	        // Not much we can do here as we don't even have a
	        // SERVICE_STATUS_HANDLE at this point.
	        //
	        return;
	    }
    }


    RtlInitUnicodeString(&ComputerNameRegistryNode, REG_COMPUTERNAME_NODE_PATH);

    InitializeObjectAttributes(&ObjectAttributes,
                               &ComputerNameRegistryNode,
                               OBJ_CASE_INSENSITIVE,
                               NULL,
                               NULL);

    Status = NtOpenKey(&hComputerNameNode, KEY_READ | KEY_NOTIFY, &ObjectAttributes);

    if (!NT_SUCCESS(Status))
    {
        ELF_LOG1(ERROR,
                 "SvcEntry_Eventlog: NtOpenKey for ComputerName failed %#x -- exiting\n",
                 Status);

        //
        // Not much we can do here as we don't even have a
        // SERVICE_STATUS_HANDLE at this point.
        //
        return;
    }

///////////////////////////////////////////////////////

#if DBG

    //
    // See if there's a debug value
    //
    RtlInitUnicodeString(&ValueName, VALUE_DEBUG);

    Status = NtQueryValueKey(hEventLogNode,
                             &ValueName,
                             KeyValuePartialInformation,
                             ValueBuffer,
                             ELF_MAX_REG_KEY_INFO_SIZE,
                             &ulActualSize);

    if (NT_SUCCESS(Status))
    {
        if (((PKEY_VALUE_PARTIAL_INFORMATION) ValueBuffer)->Type == REG_DWORD)
        {
            ElfDebugLevel = *(LPDWORD) (((PKEY_VALUE_PARTIAL_INFORMATION) ValueBuffer)->Data);
        }
    }

    ELF_LOG1(TRACE,
             "SvcEntry_Eventlog: ElfDebugLevel = %#x\n",
             ElfDebugLevel);

#endif  // DBG


    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argv);

    ElfGlobalSvcRefHandle = SvcRefHandle;
    ElfGlobalData         = SvcsGlobalData;

    //
    // Initialize the list heads for the modules and log files.
    //
    InitializeListHead(&LogFilesHead);
    InitializeListHead(&LogModuleHead);
    InitializeListHead(&QueuedEventListHead);
    InitializeListHead(&QueuedMessageListHead);

    //
    // Initialize to 0 so that we can clean up before exiting
    //
    EventFlags = 0;

    //
    // Create the Eventlog's private heap if possible.  This must be
    // done before any calls to ElfpAllocateBuffer are made.
    //
    ElfpCreateHeap();

    //
    // Initialize the status data.
    //
    Status = ElfpInitStatus();

    if (!NT_SUCCESS(Status))
    {
        ELF_LOG1(ERROR,
                 "SvcEntry_Eventlog: ElfpInitStatus failed %#x -- exiting\n",
                 Status);

        //
        // Not much we can do here as we don't even have a
        // SERVICE_STATUS_HANDLE at this point.
        //
        return;
    }

    //
    // Set up control handler
    //
    if ((ElfServiceStatusHandle = RegisterServiceCtrlHandler(
                                      EVENTLOG_SVC_NAMEW,
                                      ElfControlResponse)) == 0)
    {
        Win32Error = GetLastError();

        //
        // If we got an error, we need to set status to uninstalled, and end the
        // thread.
        //
        ELF_LOG1(ERROR,
                 "SvcEntry_Eventlog: RegisterServiceCtrlHandler failed %#x\n",
                 Win32Error);

        goto cleanupandexit;
    }

    //
    // Notify the Service Controller for the first time that we are alive
    // and are in a start pending state
    //
    //  *** UPDATE STATUS ***
    ElfStatusUpdate(STARTING);

    //
    // Get the localized title for message box popups.
    //
    ElfInitMessageBoxTitle();

    //
    // Initialize a critical section for use when adding or removing
    // LogFiles or LogModules. This must be done before we process any
    // file information.
    //
    Status = ElfpInitCriticalSection(&LogFileCritSec);

    if (!NT_SUCCESS(Status))
    {
        ELF_LOG1(ERROR,
                 "SvcEntry_Eventlog: Unable to create LogFileCritSec %#x\n",
                 Status);

        goto cleanupandexit;
    }

    EventFlags |= ELF_INIT_LOGFILE_CRIT_SEC;

    Status = ElfpInitCriticalSection(&LogModuleCritSec);

    if (!NT_SUCCESS(Status))
    {
        ELF_LOG1(ERROR,
                 "SvcEntry_Eventlog: Unable to create LogModuleCritSec %#x\n",
                 Status);

        goto cleanupandexit;
    }

    EventFlags |= ELF_INIT_LOGMODULE_CRIT_SEC;

    Status = ElfpInitCriticalSection(&QueuedEventCritSec);

    if (!NT_SUCCESS(Status))
    {
        ELF_LOG1(ERROR,
                 "SvcEntry_Eventlog: Unable to create QueuedEventCritSec %#x\n",
                 Status);

        goto cleanupandexit;
    }

    EventFlags |= ELF_INIT_QUEUED_EVENT_CRIT_SEC;

    Status = ElfpInitCriticalSection(&QueuedMessageCritSec);

    if (!NT_SUCCESS(Status))
    {
        ELF_LOG1(ERROR,
                 "SvcEntry_Eventlog: Unable to create QueuedMessageCritSec %#x\n",
                 Status);

        goto cleanupandexit;
    }

    EventFlags |= ELF_INIT_QUEUED_MESSAGE_CRIT_SEC;

    //
    // Initialize global anonymous logon sid for use in log ACL's.
    //

    Status = RtlAllocateAndInitializeSid(
                &NtAuthority,
                1,
                SECURITY_ANONYMOUS_LOGON_RID,
                0, 0, 0, 0, 0, 0, 0,
                &AnonymousLogonSid);

    if (!NT_SUCCESS(Status))
    {
        ELF_LOG1(ERROR,
                 "SvcEntry_Eventlog: Unable to create anonymous logon SID %#x\n",
                 Status);

        goto cleanupandexit;
    }

    //
    // Set up the data structures for the Logfiles and Modules.
    //

    Status = ElfSetUpConfigDataStructs();

    if (!NT_SUCCESS(Status))
    {
        ELF_LOG1(ERROR,
                 "SvcEntry_Eventlog: ElfSetUpConfigDataStructs failed %#x\n",
                 Status);

        goto cleanupandexit;
    }

    //
    // Tell service controller that we are making progress
    //
    ElfStatusUpdate(STARTING);

    //
    // Initialize a critical section for use when adding or removing
    // context handles (LogHandles).
    //
    Status = ElfpInitCriticalSection(&LogHandleCritSec);

    if (!NT_SUCCESS(Status))
    {
        ELF_LOG1(ERROR,
                 "SvcEntry_Eventlog: Unable to create LogHandleCritSec %#x\n",
                 Status);

        goto cleanupandexit;
    }

    EventFlags |= ELF_INIT_LOGHANDLE_CRIT_SEC;

    //
    // Initialize the context handle (log handle) list.
    //
    InitializeListHead( &LogHandleListHead );

    //
    // Initialize the Global Resource.
    //
    Status = ElfpInitResource(&GlobalElfResource);

    if (!NT_SUCCESS(Status))
    {
        ELF_LOG1(ERROR,
                 "SvcEntry_Eventlog: Unable to create GlobalElfResource %#x\n",
                 Status);

        goto cleanupandexit;
    }

    EventFlags |= ELF_INIT_GLOBAL_RESOURCE;

    //
    //Initialize a CritSec for clustering support
    //
    Status = ElfpInitCriticalSection(&gClPropCritSec);

    if (!NT_SUCCESS(Status))
    {
        ELF_LOG1(ERROR,
                 "SvcEntry_Eventlog: Unable to create gClPropCritSec %#x\n",
                 Status);

        goto cleanupandexit;
    }

    EventFlags |= ELF_INIT_CLUS_CRIT_SEC;

    //
    // Tell service controller that we are making progress
    //
    ElfStatusUpdate(STARTING);

    // Create a thread for watching the LPC port.
    //

    if (!StartLPCThread())
    {
        ELF_LOG0(ERROR,
                 "SvcEntry_Eventlog: StartLPCThread failed\n");

        Status = STATUS_UNSUCCESSFUL;
        goto cleanupandexit;
    }

    EventFlags |= ELF_STARTED_LPC_THREAD;

    //
    // Tell service controller of that we are making progress
    //
    ElfStatusUpdate(STARTING);

    //
    // Create a thread for watching for changes in the registry.
    //
    if (!ElfStartRegistryMonitor())
    {
        ELF_LOG0(ERROR,
                 "SvcEntry_Eventlog: ElfStartRegistryMonitor failed\n");

        Status = STATUS_UNSUCCESSFUL;
        goto cleanupandexit;
    }

    EventFlags |= ELF_STARTED_REGISTRY_MONITOR;

    //
    // Write out an event that says we started
    //

    ElfpCreateElfEvent(EVENT_EventlogStarted,
                       EVENTLOG_INFORMATION_TYPE,
                       0,                    // EventCategory
                       0,                    // NumberOfStrings
                       NULL,                 // Strings
                       NULL,                 // Data
                       0,                    // Datalength
                       0,
                       FALSE);                   // flags

    //
    // Write a boot event with version info
    //
    ElfWriteProductInfoEvent();

	// Write a computer name change event if that is applicable

	ElfCheckForComputerNameChange();
	
    //
    // Read from the registry to determine the time stamp interval, default to 5 minutes
    //
    Status = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                          REGSTR_PATH_RELIABILITY,
                          0,
                          KEY_ALL_ACCESS,
                          &hKey);

    if (Status == ERROR_SUCCESS)
    {
        RegQueryValueEx(hKey,
                        REGSTR_VAL_LASTALIVEINTERVAL,
                        0,
                        NULL,
                        (PUCHAR) &g_PreviousInterval,
                        &ValueSize);

        RegCloseKey (hKey);
    }
    //
    // If this is setup, then dont do the periodic timestamp writting
    // Setup has the feature where the last write is ignored and so
    // the code acted as if a dirty shutdown happened.
    //

    if(SvcsGlobalData->fSetupInProgress)
    {
        g_PreviousInterval = 0;     // stops the timer thread from starting    
        ElfWriteTimeStamp(EVENT_NormalShutdown,
                          FALSE);   // clears out the time stamp.
    }

    if (g_PreviousInterval != 0)
    {
        //
        // Write out the first timer based abnormal shutdown time stamp
        //

        ElfWriteTimeStamp (EVENT_AbNormalShutdown, TRUE);
    }

    //
    // Write out any events that were queued up during initialization
    //

    FlushRequest.Command = ELF_COMMAND_WRITE_QUEUED;

    ElfPerformRequest(&FlushRequest);

    //
    // Tell service controller that we are making progress
    //
    ElfStatusUpdate(STARTING);

    //
    // Finish setting up the RPC server
    //
    // NOTE:  Now all RPC servers in services.exe share the same pipe name.
    // However, in order to support communication with version 1.0 of WinNt,
    // it is necessary for the Client Pipe name to remain the same as
    // it was in version 1.0.  Mapping to the new name is performed in
    // the Named Pipe File System code.
    //
    Status = ElfGlobalData->StartRpcServer(
                ElfGlobalData->SvcsRpcPipeName,
                eventlog_ServerIfHandle);

    if (!NT_SUCCESS(Status))
    {
        ELF_LOG1(ERROR,
                 "SvcEntry_Eventlog: StartRpcServer failed %#x\n",
                 Status);

        goto cleanupandexit;
    }

    //
    // Tell service controller that we are making progress
    //
    ElfStatusUpdate(RUNNING);

    EventFlags |= ELF_STARTED_RPC_SERVER;

    if (GetElState() == RUNNING)
    {
        if ( g_PreviousInterval != 0 )
        {
            //
            // Create a thread to periodically write
            // a time stamp to the registry.
            //

            g_hTimestampEvent = CreateEvent (NULL, TRUE, FALSE, NULL);

            if (g_hTimestampEvent != NULL)
            {
                Status = RtlRegisterWait(&g_hTimestampWorkitem,
                                         g_hTimestampEvent,
                                         TimeStampProc,              // Callback
                                         (PVOID) UlongToPtr(g_PreviousInterval), // Context
                                         0,                          // Timeout
                                         WT_EXECUTEONLYONCE);

                if (!NT_SUCCESS(Status))
                {
                    ELF_LOG1(ERROR,
                             "SvcEntry_Eventlog: RtlRegisterWait failed %#x\n",
                             Status);
                }
            }
            else
            {
                ELF_LOG1(ERROR,
                         "SvcEntry_Eventlog: CreateEvent for timestamp failed %d\n",
                         GetLastError());
            }
        }

        ELF_LOG0(TRACE,
                 "SvcEntry_Eventlog: Service running -- main thread returning\n");

        return;
    }

cleanupandexit:

    //
    // Come here if there is cleanup necessary.
    //
    ELF_LOG0(ERROR,
             "SvcEntry_Eventlog: Exiting on error\n");

    if (Win32Error == NO_ERROR)
    {
        Win32Error = RtlNtStatusToDosError(Status);
    }

    ElfBeginForcedShutdown(PENDING, Win32Error, Status);

    //
    // If the registry monitor has been initialized, then
    // let it do the shutdown cleanup.  All we need to do
    // here is wake it up.
    // Otherwise, this thread will do the cleanup.
    //
    if (EventFlags & ELF_STARTED_REGISTRY_MONITOR)
    {
        StopRegistryMonitor();
    }
    else
    {
        ElfpCleanUp(EventFlags);
    }

    return;
}


VOID
ElfInitMessageBoxTitle(
    VOID
    )

/*++

Routine Description:

    Obtains the title text for the message box used to display messages.
    If the title is successfully obtained from the message file, then
    that title is pointed to by GlobalAllocatedMsgTitle and
    GlobalMessageBoxTitle.  If unsuccessful, then GlobalMessageBoxTitle
    left pointing to the DefaultMessageBoxTitle.

    NOTE:  If successful, a buffer is allocated by this function.  The
    pointer stored in GlobalAllocatedMsgTitle and it should be freed when
    done with this buffer.

Arguments:

Return Value:

    none

--*/
{
    LPVOID      hModule;
    DWORD       msgSize;

    //
    // This function should be called only once during initialization.  Note
    // that it needs to be called before the Eventlog's RPC server is started
    // or else it's possible for the log to fill up, which will generate a
    // "log full" popup with no title (since GlobalMessageBoxTitle is NULL).
    //
    ASSERT(GlobalMessageBoxTitle == NULL);

    hModule = LoadLibraryEx(L"netevent.dll",
                            NULL,
                            LOAD_LIBRARY_AS_DATAFILE);

    if ( hModule == NULL)
    {
        ELF_LOG1(ERROR,
                 "ElfInitMessageBoxTitle: LoadLibrary of netevent.dll failed %d\n",
                 GetLastError());

        return;
    }

    msgSize = FormatMessageW(
                FORMAT_MESSAGE_FROM_HMODULE |       //  dwFlags
                  FORMAT_MESSAGE_ARGUMENT_ARRAY |
                  FORMAT_MESSAGE_ALLOCATE_BUFFER,
                hModule,
                TITLE_EventlogMessageBox,           //  MessageId
                0,                                  //  dwLanguageId
                (LPWSTR) &GlobalMessageBoxTitle,    //  lpBuffer
                0,                                  //  nSize
                NULL);

    if (msgSize == 0)
    {
        ELF_LOG2(ERROR,
                 "ElfInitMessageBoxTitle: FormatMessage failed %d -- using %ws\n",
                 GetLastError(),
                 ELF_DEFAULT_MESSAGE_BOX_TITLE);

        GlobalMessageBoxTitle = ELF_DEFAULT_MESSAGE_BOX_TITLE;
    }

    FreeLibrary(hModule);
    return;
}


#ifdef EXIT_PROCESS

//
// This code is compiled into the Eventlog to track down a DLL that's loaded
// into services.exe and calls ExitProcess.  Since this DLL should never be
// unloaded, we break into the debugger on DLL_PROCESS_DETACH.  To use this,
// the following need to be added to the sources file:
//
// DLLENTRY=  DllInit
//
// -DEXIT_PROCESS  (to the C_DEFINES line)
//

BOOL
DllInit(
    IN  HINSTANCE   hDll,
    IN  DWORD       dwReason,
    IN  PCONTEXT    pContext OPTIONAL
    )
{
    switch (dwReason) {

        case DLL_PROCESS_ATTACH:

            //
            // No notification of THREAD_ATTACH and THREAD_DETACH
            //
            DisableThreadLibraryCalls(hDll);
            break;

        case DLL_PROCESS_DETACH:

            //
            // This should NEVER happen -- it means services.exe
            // is exiting via an ExitProcess call
            //
            DebugBreak();
            break;
    }

    return TRUE;
}

#endif  // EXIT_PROCESS
