/*++

Copyright (c) 1993 Microsoft Corporation

Module Name:

    spsif.h

Abstract:

    Header file for section names and other data used for indexing into
    setup information files.

Author:

    Ted Miller (tedm) 31-August-1993

Revision History:

--*/


#ifndef _SPSIF_
#define _SPSIF_



//
// Field indices for hardware-related sections in txtsetup.sif
//

// in [Map.<Component>] sections
#define INDEX_MAP_FWID          0

// in [<Component>] sections
#define INDEX_DESCRIPTION       0
#define INDEX_FILESECTION       1
#define INDEX_INFKEYNAME        2
#define INDEX_FIRSTAUXFIELD     3


//
// Macro to determine whether a string is present in a SIF.
//
#define SIF_SPECIFIED(string)   ((string) && *(string))

extern PWSTR SIF_DISKDRIVERMAP;

//
// Media description and file placement
//
extern PWSTR SIF_SETUPMEDIA;
extern PWSTR SIF_FILESONSETUPMEDIA;

//
// defines to index into SIF_FILESONSETUPMEDIA section
//
#define INDEX_WHICHMEDIA        0
#define INDEX_WHICHBOOTMEDIA    6
#define INDEX_DESTINATION       7
#define INDEX_UPGRADE           8
#define INDEX_WINNTFILE         9
#define INDEX_TARGETNAME        10

//
// File copy lists
//
extern PWSTR SIF_SYSPARTCOPYALWAYS;
extern PWSTR SIF_SYSPARTROOT;
extern PWSTR SIF_SYSPARTUTIL;                             
extern PWSTR SIF_SPECIALFILES;
extern PWSTR SIF_MPKERNEL;
extern PWSTR SIF_UPKERNEL;
extern PWSTR SIF_ATDISK;
extern PWSTR SIF_ABIOSDISK;
extern PWSTR SIF_MOUSECLASS;
extern PWSTR SIF_KEYBOARDCLASS;

extern PWSTR SIF_HAL;
extern PWSTR SIF_BOOTVID;

extern PWSTR SIF_BOOTBUSEXTENDERS;
extern PWSTR SIF_BUSEXTENDERS;
extern PWSTR SIF_INPUTDEVICESSUPPORT;

extern PWSTR SIF_NTDETECT;
extern PWSTR SIF_STANDARD;

extern PWSTR SIF_KEYBOARDLAYOUTFILES;
extern PWSTR SIF_KEYBOARDLAYOUTDESC;
extern PWSTR SIF_VGAFILES;

extern PWSTR SIF_NTDIRECTORIES;

extern PWSTR SIF_SCSICLASSDRIVERS;
extern PWSTR SIF_DISKDRIVERS;
extern PWSTR SIF_CDROMDRIVERS;
extern PWSTR SIF_FILESYSTEMS;

extern PWSTR SIF_KEYBOARDLAYOUT;

#if defined(REMOTE_BOOT)
extern PWSTR SIF_REMOTEBOOTFILES;
#endif // defined(REMOTE_BOOT)

extern PWSTR SIF_SETUPDATA;
extern PWSTR SIF_DISKSPACEREQUIREMENTS;
extern PWSTR SIF_PRODUCTTYPE;
extern PWSTR SIF_MAJORVERSION;
extern PWSTR SIF_MINORVERSION;
extern PWSTR SIF_WINDIRSPACE;
extern PWSTR SIF_FREESYSPARTDISKSPACE;
extern PWSTR SIF_DEFAULTPATH;
extern PWSTR SIF_LOADIDENTIFIER;
extern PWSTR SIF_BASEVIDEOLOADID;
extern PWSTR SIF_OSLOADOPTIONS;
extern PWSTR SIF_OSLOADOPTIONSVAR;
extern PWSTR SIF_OSLOADOPTIONSVARAPPEND;
extern PWSTR SIF_SETUPSOURCEDEVICE;
extern PWSTR SIF_SETUPSOURCEPATH;
extern PWSTR SIF_DONTCOPY;
extern PWSTR SIF_REQUIREDMEMORY;
extern PWSTR SIF_SETUPCMDPREPEND;
extern PWSTR SIF_PAGEFILE;

extern PWSTR SIF_NLS;
extern PWSTR SIF_ANSICODEPAGE;
extern PWSTR SIF_OEMCODEPAGE;
extern PWSTR SIF_MACCODEPAGE;
extern PWSTR SIF_UNICODECASETABLE;
extern PWSTR SIF_OEMHALFONT;
extern PWSTR SIF_DEFAULTLAYOUT;

extern PWSTR SIF_REPAIRWINNTFILES;
extern PWSTR SIF_REPAIRSYSPARTFILES;


//
// Upgrade sections, values
//

extern PWSTR SIF_NET_SERVICES_TO_DISABLE;
extern PWSTR SIF_KEYS_TO_DELETE;
extern PWSTR SIF_KEYS_TO_ADD;

extern PWSTR SIF_SYSTEM_HIVE;
extern PWSTR SIF_SOFTWARE_HIVE;
extern PWSTR SIF_DEFAULT_HIVE;
extern PWSTR SIF_CONTROL_SET;

extern PWSTR SIF_REG_SZ;
extern PWSTR SIF_REG_DWORD;
extern PWSTR SIF_REG_EXPAND_SZ;
extern PWSTR SIF_REG_MULTI_SZ;
extern PWSTR SIF_REG_BINARY;
extern PWSTR SIF_REG_BINARY_DWORD;

extern PWSTR SIF_FILESDELETEONUPGRADE;
extern PWSTR SIF_FILESBACKUPONUPGRADE;
extern PWSTR SIF_FILESBACKUPONOVERWRITE;

extern PWSTR SIF_DIRSDELETEONUPGRADE;

#ifdef _X86_
extern PWSTR SIF_FILESMOVEBEFOREMIGRATION;
extern PWSTR SIF_FILESDELETEBEFOREMIGRATION;
#endif

extern PWSTR SIF_FILESUPGRADEWIN31;
extern PWSTR SIF_FILESNEWHIVES;

//
// New sections and keys added to setup.log
//

extern PWSTR SIF_NEW_REPAIR_WINNTFILES;
extern PWSTR SIF_NEW_REPAIR_SYSPARTFILES;
extern PWSTR SIF_NEW_REPAIR_SIGNATURE;
extern PWSTR SIF_NEW_REPAIR_VERSION_KEY;
extern PWSTR SIF_NEW_REPAIR_NT_VERSION_TEMPLATE;
extern PWSTR SIF_NEW_REPAIR_NT_VERSION;
extern PWSTR SIF_NEW_REPAIR_PATHS;
extern PWSTR SIF_NEW_REPAIR_PATHS_SYSTEM_PARTITION_DEVICE;
extern PWSTR SIF_NEW_REPAIR_PATHS_SYSTEM_PARTITION_DIRECTORY;
extern PWSTR SIF_NEW_REPAIR_PATHS_TARGET_DEVICE;
extern PWSTR SIF_NEW_REPAIR_PATHS_TARGET_DIRECTORY;

extern PWSTR SETUP_REPAIR_DIRECTORY;
extern PWSTR SETUP_LOG_FILENAME;

extern PWSTR SIF_NEW_REPAIR_FILES_IN_REPAIR_DIR;

extern PWSTR SIF_DATA;
extern PWSTR SIF_UNATTENDED;
extern PWSTR SIF_SETUPPARAMS;
extern PWSTR SIF_CONFIRMHW;
extern PWSTR SIF_GUI_UNATTENDED;
extern PWSTR SIF_UNATTENDED_INF_FILE;
extern PWSTR SIF_UNIQUEID;
extern PWSTR SIF_EXTENDOEMPART;
extern PWSTR SIF_ACCESSIBILITY;

extern PWSTR SIF_REMOTEINSTALL;
#if defined(REMOTE_BOOT)
extern PWSTR SIF_REMOTEBOOT;
extern PWSTR SIF_ENABLEIPSECURITY;
#endif // defined(REMOTE_BOOT)
extern PWSTR SIF_REPARTITION;
extern PWSTR SIF_USEWHOLEDISK;

//
// Alternate Source data
//
extern PWSTR SIF_UPDATEDSOURCES;
//extern PWSTR SIF_ALTCOPYFILESSECTION;

extern PWSTR SIF_INCOMPATIBLE_TEXTMODE;


PWSTR
SpPlatformSpecificLookup(
    IN PVOID   SifHandle,
    IN PWSTR   Section,
    IN PWSTR   Key,
    IN ULONG   Index,
    IN BOOLEAN Fatal
    );

PWSTR
SpLookUpTargetDirectory(
    IN PVOID SifHandle,
    IN PWSTR Symbol
    );

PWSTR
SpLookUpValueForFile(
    IN PVOID   SifHandle,
    IN PWSTR   File,
    IN ULONG   Index,
    IN BOOLEAN Fatal
    );

BOOLEAN
IsFileFlagSet(
    IN PVOID SifHandle,
    IN PWSTR FileName,
    IN ULONG Flag
    );

#endif // ndef _SPSIF_


