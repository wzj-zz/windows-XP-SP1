/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    fmifs.h

Abstract:

    This header file contains the specification of the interface
    between the file manager and fmifs.dll for the purposes of
    accomplishing IFS functions.

Author:

    Norbert P. Kusters (norbertk) 6-Mar-92

--*/

#if !defined( _FMIFS_DEFN_ )

#define _FMIFS_DEFN_

typedef LONG    *PNTSTATUS;

//
// These are the defines for 'PacketType'.
// Revisions: 02/03/98: Dave Sheldon - Added FmIfsNoMediaInDevice
//

typedef enum _FMIFS_PACKET_TYPE {
    FmIfsPercentCompleted           = 0,
    FmIfsFormatReport               = 1,
    FmIfsInsertDisk                 = 2,
    FmIfsIncompatibleFileSystem     = 3,
    FmIfsFormattingDestination      = 4,
    FmIfsIncompatibleMedia          = 5,
    FmIfsAccessDenied               = 6,
    FmIfsMediaWriteProtected        = 7,
    FmIfsCantLock                   = 8,
    FmIfsCantQuickFormat            = 9,
    FmIfsIoError                    = 10,
    FmIfsFinished                   = 11,
    FmIfsBadLabel                   = 12,
    FmIfsCheckOnReboot              = 13,
    FmIfsTextMessage                = 14,
    FmIfsHiddenStatus               = 15,
    FmIfsClusterSizeTooSmall        = 16,
    FmIfsClusterSizeTooBig          = 17,
    FmIfsVolumeTooSmall             = 18,
    FmIfsVolumeTooBig               = 19,
    FmIfsNoMediaInDevice            = 20,
    FmIfsClustersCountBeyond32bits  = 21,
    FmIfsCantChkMultiVolumeOfSameFS = 22,
    FmIfsFormatFatUsing64KCluster   = 23
} FMIFS_PACKET_TYPE, *PFMIFS_PACKET_TYPE;

typedef struct _FMIFS_PERCENT_COMPLETE_INFORMATION {
    ULONG   PercentCompleted;
} FMIFS_PERCENT_COMPLETE_INFORMATION, *PFMIFS_PERCENT_COMPLETE_INFORMATION;

//
// These couple of bits are used in the ValuesInMB field of the structure
// FMIFS_FORMAT_REPORT_INFORMATION.  When set, the corresponding
// ULONG value becomes in units of megabytes.  If not set, the corresponding
// ULONG value is in units of kilobytes.
//
#define TOTAL_DISK_SPACE_IN_MB      1
#define BYTES_AVAILABLE_IN_MB       2

typedef struct _FMIFS_FORMAT_REPORT_INFORMATION {
    ULONG   KiloBytesTotalDiskSpace;
    ULONG   KiloBytesAvailable;
    ULONG   ValuesInMB;
} FMIFS_FORMAT_REPORT_INFORMATION, *PFMIFS_FORMAT_REPORT_INFORMATION;

// The packet for FmIfsDblspaceCreated is a Unicode string
// giving the name of the Compressed Volume File; it is not
// necessarily zero-terminated.
//

#define DISK_TYPE_GENERIC           0
#define DISK_TYPE_SOURCE            1
#define DISK_TYPE_TARGET            2
#define DISK_TYPE_SOURCE_AND_TARGET 3

typedef struct _FMIFS_INSERT_DISK_INFORMATION {
    ULONG   DiskType;
} FMIFS_INSERT_DISK_INFORMATION, *PFMIFS_INSERT_DISK_INFORMATION;

typedef struct _FMIFS_IO_ERROR_INFORMATION {
    ULONG   DiskType;
    ULONG   Head;
    ULONG   Track;
} FMIFS_IO_ERROR_INFORMATION, *PFMIFS_IO_ERROR_INFORMATION;

typedef struct _FMIFS_FINISHED_INFORMATION {
    BOOLEAN Success;
} FMIFS_FINISHED_INFORMATION, *PFMIFS_FINISHED_INFORMATION;

typedef struct _FMIFS_CHECKONREBOOT_INFORMATION {
    OUT BOOLEAN QueryResult; // TRUE for "yes", FALSE for "no"
} FMIFS_CHECKONREBOOT_INFORMATION, *PFMIFS_CHECKONREBOOT_INFORMATION;

typedef enum _TEXT_MESSAGE_TYPE {
    MESSAGE_TYPE_PROGRESS,
    MESSAGE_TYPE_RESULTS,
    MESSAGE_TYPE_FINAL
} TEXT_MESSAGE_TYPE, *PTEXT_MESSAGE_TYPE;

typedef struct _FMIFS_TEXT_MESSAGE {
    IN TEXT_MESSAGE_TYPE    MessageType;
    IN PSTR                 Message;
} FMIFS_TEXT_MESSAGE, *PFMIFS_TEXT_MESSAGE;


#define FMIFS_SONY_MS_MASK                            0x00000001        // mask for all types of memory stick
#define FMIFS_SONY_MS                                 0x00000001        // device is a memory stick
#define FMIFS_SONY_MS_FMT_CMD_CAPABLE                 0x00000010        // reader supports fmt cmd
#define FMIFS_SONY_MS_PROGRESS_INDICATOR_CAPABLE      0x00000020        // reader supports progress indicator
#define FMIFS_NTFS_NOT_SUPPORTED                      0x00000040        // ntfs should not be used on this device

typedef struct _FMIFS_DEVICE_INFORMATION {
    ULONG       Flags;
} FMIFS_DEVICE_INFORMATION, *PFMIFS_DEVICE_INFORMATION;

//
// This is a list of supported floppy media types for format.
//

typedef enum _FMIFS_MEDIA_TYPE {
    FmMediaUnknown,
    FmMediaF5_160_512,      // 5.25", 160KB,  512 bytes/sector
    FmMediaF5_180_512,      // 5.25", 180KB,  512 bytes/sector
    FmMediaF5_320_512,      // 5.25", 320KB,  512 bytes/sector
    FmMediaF5_320_1024,     // 5.25", 320KB,  1024 bytes/sector
    FmMediaF5_360_512,      // 5.25", 360KB,  512 bytes/sector
    FmMediaF3_720_512,      // 3.5",  720KB,  512 bytes/sector
    FmMediaF5_1Pt2_512,     // 5.25", 1.2MB,  512 bytes/sector
    FmMediaF3_1Pt44_512,    // 3.5",  1.44MB, 512 bytes/sector
    FmMediaF3_2Pt88_512,    // 3.5",  2.88MB, 512 bytes/sector
    FmMediaF3_20Pt8_512,    // 3.5",  20.8MB, 512 bytes/sector
    FmMediaRemovable,       // Removable media other than floppy
    FmMediaFixed,
    FmMediaF3_120M_512,     // 3.5", 120M Floppy
    // FMR Sep.8.1994 SFT YAM
    // FMR Jul.14.1994 SFT KMR
    FmMediaF3_640_512,      // 3.5" ,  640KB,  512 bytes/sector
    FmMediaF5_640_512,      // 5.25",  640KB,  512 bytes/sector
    FmMediaF5_720_512,      // 5.25",  720KB,  512 bytes/sector
    // FMR Sep.8.1994 SFT YAM
    // FMR Jul.14.1994 SFT KMR
    FmMediaF3_1Pt2_512,     // 3.5" , 1.2Mb,   512 bytes/sector
    // FMR Sep.8.1994 SFT YAM
    // FMR Jul.14.1994 SFT KMR
    FmMediaF3_1Pt23_1024,   // 3.5" , 1.23Mb, 1024 bytes/sector
    FmMediaF5_1Pt23_1024,   // 5.25", 1.23MB, 1024 bytes/sector
    FmMediaF3_128Mb_512,    // 3.5" , 128MB,  512 bytes/sector  3.5"MO
    FmMediaF3_230Mb_512,    // 3.5" , 230MB,  512 bytes/sector  3.5"MO
    FmMediaF3_200Mb_512,    // 3.5" , 200MB,  512 bytes/sector  HiFD (200MB Floppy)
    FmMediaF3_240Mb_512,    // 3.5" , 240MB,  512 bytes/sector  HiFD (240MB Floppy)
    FmMediaEndOfData        // Total data count.
} FMIFS_MEDIA_TYPE, *PFMIFS_MEDIA_TYPE;

//
// The structure below defines information to be passed into ChkdskEx.
// When new fields are added, the version number will have to be upgraded
// so that only new code will reference those new fields.
//
typedef struct {
    UCHAR   Major;      // initial version is 1.0
    UCHAR   Minor;
    ULONG   Flags;
} FMIFS_CHKDSKEX_PARAM, *PFMIFS_CHKDSKEX_PARAM;

//
// Internal definitions for Flags field in FMIFS_CHKDSKEX_PARAM
//
#define FMIFS_CHKDSK_RECOVER_FREE_SPACE       0x00000002UL
#define FMIFS_CHKDSK_RECOVER_ALLOC_SPACE      0x00000004UL

//
// External definitions for Flags field in FMIFS_CHKDSKEX_PARAM
//

// FMIFS_CHKDSK_VERBOSE
//  - For FAT, chkdsk will print every filename being processed
//  - For NTFS, chkdsk will print clean up messages
// FMIFS_CHKDSK_RECOVER
//  - Perform sector checking on free and allocated space
// FMIFS_CHKDSK_EXTEND
//  - For NTFS, chkdsk will extend a volume
// FMIFS_CHKDSK_DOWNGRADE (for NT5 or later but obsolete anyway)
//  - For NTFS, this downgrade a volume from most recent NTFS version
// FMIFS_CHKDSK_ENABLE_UPGRADE (for NT5 or later but obsolete anyway)
//  - For NTFS, this upgrades a volume to most recent NTFS version
// FMIFS_CHKDSK_CHECK_IF_DIRTY
//  - Perform consistency check only if the volume is dirty
// FMIFS_CHKDSK_FORCE (for NT5 or later)
//  - Forces the volume to dismount first if necessary
// FMIFS_CHKDSK_SKIP_INDEX_SCAN
//  - Skip the scanning of each index entry
// FMIFS_CHKDSK_SKIP_CYCLE_SCAN
//  - Skip the checking of cycles within the directory tree

#define FMIFS_CHKDSK_VERBOSE                  0x00000001UL
#define FMIFS_CHKDSK_RECOVER                  (FMIFS_CHKDSK_RECOVER_FREE_SPACE | \
                                               FMIFS_CHKDSK_RECOVER_ALLOC_SPACE)
#define FMIFS_CHKDSK_EXTEND                   0x00000008UL
#define FMIFS_CHKDSK_DOWNGRADE                0x00000010UL
#define FMIFS_CHKDSK_ENABLE_UPGRADE           0x00000020UL
#define FMIFS_CHKDSK_CHECK_IF_DIRTY           0x00000080UL
#define FMIFS_CHKDSK_FORCE                    0x00000100UL
#define FMIFS_CHKDSK_SKIP_INDEX_SCAN          0x00000200UL
#define FMIFS_CHKDSK_SKIP_CYCLE_SCAN          0x00000400UL

//
// The structure below defines information to be passed into FormatEx2.
// When new fields are added, the version number will have to be upgraded
// so that only new code will reference those new fields.
//
typedef struct {
    UCHAR       Major;          // initial version is 1.0
    UCHAR       Minor;
    ULONG       Flags;
    PWSTR       LabelString;    // supplies the volume's label
    ULONG       ClusterSize;    // supplies the cluster size for the volume
} FMIFS_FORMATEX2_PARAM, *PFMIFS_FORMATEX2_PARAM;

//
// External definitions for Flags field in FMIFS_FORMATEX2_PARAM
//

// FMIFS_FORMAT_QUICK
//  - Performs a quick format
// FMIFS_FORMAT_BACKWARD_COMPATIBLE
//  - Formats a volume to be backward compatible to an older version of NTFS volume
// FMIFS_FORMAT_FORCE
//  - Forces the volume to dismount first if necessary

#define FMIFS_FORMAT_QUICK                    0x00000001UL
#define FMIFS_FORMAT_BACKWARD_COMPATIBLE      0x00000002UL
#define FMIFS_FORMAT_FORCE                    0x00000004UL

//
// Maximum length of file system format name string including version number in it
// (e.g. FAT32, NTFS 5.0, NTFS 4.0, FAT, etc)
//
#define MAX_FILE_SYSTEM_FORMAT_VERSION_NAME_LEN       16

//
// Maximum length of file system name string (e.g. NTFS, FAT32, FAT, etc)
//
#define MAX_FILE_SYSTEM_FORMAT_NAME_LEN                9

//
// Function types/interfaces.
//

typedef BOOLEAN
(*FMIFS_CALLBACK)(
    IN  FMIFS_PACKET_TYPE   PacketType,
    IN  ULONG               PacketLength,
    IN  PVOID               PacketData
    );

typedef
VOID
(*PFMIFS_FORMAT_ROUTINE)(
    IN  PWSTR               DriveName,
    IN  FMIFS_MEDIA_TYPE    MediaType,
    IN  PWSTR               FileSystemName,
    IN  PWSTR               Label,
    IN  BOOLEAN             Quick,
    IN  FMIFS_CALLBACK      Callback
    );

typedef
VOID
(*PFMIFS_FORMATEX_ROUTINE)(
    IN  PWSTR               DriveName,
    IN  FMIFS_MEDIA_TYPE    MediaType,
    IN  PWSTR               FileSystemName,
    IN  PWSTR               Label,
    IN  BOOLEAN             Quick,
    IN  ULONG               ClusterSize,
    IN  FMIFS_CALLBACK      Callback
    );

typedef
VOID
(*PFMIFS_FORMATEX2_ROUTINE)(
    IN  PWSTR                   DriveName,
    IN  FMIFS_MEDIA_TYPE        MediaType,
    IN  PWSTR                   FileSystemName,
    IN  PFMIFS_FORMATEX2_PARAM  Param,
    IN  FMIFS_CALLBACK          Callback
    );

typedef
BOOLEAN
(*PFMIFS_ENABLECOMP_ROUTINE)(
    IN  PWSTR               DriveName,
    IN  USHORT              CompressionFormat
    );

typedef
VOID
(*PFMIFS_CHKDSK_ROUTINE)(
    IN  PWSTR               DriveName,
    IN  PWSTR               FileSystemName,
    IN  BOOLEAN             Fix,
    IN  BOOLEAN             Verbose,
    IN  BOOLEAN             OnlyIfDirty,
    IN  BOOLEAN             Recover,
    IN  PWSTR               PathToCheck,
    IN  BOOLEAN             Extend,
    IN  FMIFS_CALLBACK      Callback
    );

typedef
VOID
(*PFMIFS_CHKDSKEX_ROUTINE)(
    IN  PWSTR                   DriveName,
    IN  PWSTR                   FileSystemName,
    IN  BOOLEAN                 Fix,
    IN  PFMIFS_CHKDSKEX_PARAM   Param,
    IN  FMIFS_CALLBACK          Callback
    );

typedef
VOID
(*PFMIFS_EXTEND_ROUTINE)(
    IN  PWSTR               DriveName,
    IN  BOOLEAN             Verify,
    IN  FMIFS_CALLBACK      Callback
    );

typedef
VOID
(*PFMIFS_DISKCOPY_ROUTINE)(
    IN  PWSTR           SourceDrive,
    IN  PWSTR           DestDrive,
    IN  BOOLEAN         Verify,
    IN  FMIFS_CALLBACK  Callback
    );

typedef
BOOLEAN
(*PFMIFS_SETLABEL_ROUTINE)(
    IN  PWSTR   DriveName,
    IN  PWSTR   Label
    );

typedef
BOOLEAN
(*PFMIFS_QSUPMEDIA_ROUTINE)(
    IN  PWSTR               DriveName,
    OUT PFMIFS_MEDIA_TYPE   MediaTypeArray  OPTIONAL,
    IN  ULONG               NumberOfArrayEntries,
    OUT PULONG              NumberOfMediaTypes
    );

typedef
BOOLEAN
(*PFMIFS_QUERY_DEVICE_INFO_BY_HANDLE_ROUTINE)(
    IN  HANDLE                      DriveHandle,
    OUT PFMIFS_DEVICE_INFORMATION   DevInfo,
    IN  ULONG                       DevInfoSize
    );

typedef
BOOLEAN
(*PFMIFS_QUERY_DEVICE_INFO_ROUTINE)(
    IN  PWSTR                       DriveName,
    OUT PFMIFS_DEVICE_INFORMATION   DevInfo,
    IN  ULONG                       DevInfoSize
    );

typedef
VOID
(*PFMIFS_DOUBLESPACE_CREATE_ROUTINE)(
    IN PWSTR           HostDriveName,
    IN ULONG           Size,
    IN PWSTR           Label,
    IN PWSTR           NewDriveName,
    IN FMIFS_CALLBACK  Callback
    );

#if defined( DBLSPACE_ENABLED )
typedef
VOID
(*PFMIFS_DOUBLESPACE_DELETE_ROUTINE)(
    IN PWSTR           DblspaceDriveName,
    IN FMIFS_CALLBACK  Callback
    );

typedef
VOID
(*PFMIFS_DOUBLESPACE_MOUNT_ROUTINE)(
    IN PWSTR           HostDriveName,
    IN PWSTR           CvfName,
    IN PWSTR           NewDriveName,
    IN FMIFS_CALLBACK  Callback
    );

typedef
VOID
(*PFMIFS_DOUBLESPACE_DISMOUNT_ROUTINE)(
    IN PWSTR           DblspaceDriveName,
    IN FMIFS_CALLBACK  Callback
    );

typedef
BOOLEAN
(*PFMIFS_DOUBLESPACE_QUERY_INFO_ROUTINE)(
    IN  PWSTR       DosDriveName,
    OUT PBOOLEAN    IsRemovable,
    OUT PBOOLEAN    IsFloppy,
    OUT PBOOLEAN    IsCompressed,
    OUT PBOOLEAN    Error,
    OUT PWSTR       NtDriveName,
    IN  ULONG       MaxNtDriveNameLength,
    OUT PWSTR       CvfFileName,
    IN  ULONG       MaxCvfFileNameLength,
    OUT PWSTR       HostDriveName,
    IN  ULONG       MaxHostDriveNameLength
    );

typedef
BOOLEAN
(*PFMIFS_DOUBLESPACE_SET_AUTMOUNT_ROUTINE)(
    IN  BOOLEAN EnableAutomount
    );

#endif // DBLSPACE_ENABLED

FMIFS_MEDIA_TYPE
ComputeFmMediaType(
    IN  MEDIA_TYPE  MediaType
    );

VOID
Format(
    IN  PWSTR               DriveName,
    IN  FMIFS_MEDIA_TYPE    MediaType,
    IN  PWSTR               FileSystemName,
    IN  PWSTR               Label,
    IN  BOOLEAN             Quick,
    IN  FMIFS_CALLBACK      Callback
    );

VOID
FormatEx(
    IN  PWSTR               DriveName,
    IN  FMIFS_MEDIA_TYPE    MediaType,
    IN  PWSTR               FileSystemName,
    IN  PWSTR               Label,
    IN  BOOLEAN             Quick,
    IN  ULONG               ClusterSize,
    IN  FMIFS_CALLBACK      Callback
    );

VOID
FormatEx2(
    IN  PWSTR                   DriveName,
    IN  FMIFS_MEDIA_TYPE        MediaType,
    IN  PWSTR                   FileSystemName,
    IN  PFMIFS_FORMATEX2_PARAM  Param,
    IN  FMIFS_CALLBACK          Callback
    );

BOOLEAN
EnableVolumeCompression(
    IN  PWSTR               DriveName,
    IN  USHORT              CompressionFormat
    );

VOID
Chkdsk(
    IN  PWSTR               DriveName,
    IN  PWSTR               FileSystemName,
    IN  BOOLEAN             Fix,
    IN  BOOLEAN             Verbose,
    IN  BOOLEAN             OnlyIfDirty,
    IN  BOOLEAN             Recover,
    IN  PWSTR               PathToCheck,
    IN  BOOLEAN             Extend,
    IN  FMIFS_CALLBACK      Callback
    );

VOID
ChkdskEx(
    IN  PWSTR                   DriveName,
    IN  PWSTR                   FileSystemName,
    IN  BOOLEAN                 Fix,
    IN  PFMIFS_CHKDSKEX_PARAM   Param,
    IN  FMIFS_CALLBACK          Callback
    );

VOID
Extend(
    IN  PWSTR               DriveName,
    IN  BOOLEAN             Verify,
    IN  FMIFS_CALLBACK      Callback
    );

VOID
DiskCopy(
    IN  PWSTR           SourceDrive,
    IN  PWSTR           DestDrive,
    IN  BOOLEAN         Verify,
    IN  FMIFS_CALLBACK  Callback
    );

BOOLEAN
SetLabel(
    IN  PWSTR   DriveName,
    IN  PWSTR   Label
    );

BOOLEAN
QuerySupportedMedia(
    IN  PWSTR               DriveName,
    OUT PFMIFS_MEDIA_TYPE   MediaTypeArray  OPTIONAL,
    IN  ULONG               NumberOfArrayEntries,
    OUT PULONG              NumberOfMediaTypes
    );

BOOLEAN
QueryAvailableFileSystemFormat(
    IN     ULONG            Index,
    OUT    PWSTR            FileSystemName,
    OUT    PUCHAR           MajorVersion,
    OUT    PUCHAR           MinorVersion,
    OUT    PBOOLEAN         Latest
);

BOOLEAN
QueryFileSystemName(
    IN     PWSTR        DriveName,
    OUT    PWSTR        FileSystemName,
    OUT    PUCHAR       MajorVersion,
    OUT    PUCHAR       MinorVersion,
    OUT    PNTSTATUS    ErrorCode
);

BOOLEAN
QueryLatestFileSystemVersion(
    IN  PWSTR   FileSystemName,
    OUT PUCHAR  MajorVersion,
    OUT PUCHAR  MinorVersion
);

BOOLEAN
QueryDeviceInformation(
    IN     PWSTR                                DriveName,
       OUT PFMIFS_DEVICE_INFORMATION            DevInfo,
    IN     ULONG                                DevInfoSize
);

BOOLEAN
QueryDeviceInformationByHandle(
    IN     HANDLE                               DriveHandle,
       OUT PFMIFS_DEVICE_INFORMATION            DevInfo,
    IN     ULONG                                DevInfoSize
);

VOID
DoubleSpaceCreate(
    IN PWSTR           HostDriveName,
    IN ULONG           Size,
    IN PWSTR           Label,
    IN PWSTR           NewDriveName,
    IN FMIFS_CALLBACK  Callback
    );

#if defined( DBLSPACE_ENABLED )

VOID
DoubleSpaceDelete(
    IN PWSTR           DblspaceDriveName,
    IN FMIFS_CALLBACK  Callback
    );

VOID
DoubleSpaceMount(
    IN PWSTR           HostDriveName,
    IN PWSTR           CvfName,
    IN PWSTR           NewDriveName,
    IN FMIFS_CALLBACK  Callback
    );

VOID
DoubleSpaceDismount(
    IN PWSTR           DblspaceDriveName,
    IN FMIFS_CALLBACK  Callback
    );

// Miscellaneous prototypes:
//
BOOLEAN
FmifsQueryDriveInformation(
    IN  PWSTR       DosDriveName,
    OUT PBOOLEAN    IsRemovable,
    OUT PBOOLEAN    IsFloppy,
    OUT PBOOLEAN    IsCompressed,
    OUT PBOOLEAN    Error,
    OUT PWSTR       NtDriveName,
    IN  ULONG       MaxNtDriveNameLength,
    OUT PWSTR       CvfFileName,
    IN  ULONG       MaxCvfFileNameLength,
    OUT PWSTR       HostDriveName,
    IN  ULONG       MaxHostDriveNameLength
    );

BOOLEAN
FmifsSetAutomount(
    IN  BOOLEAN EnableAutomount
    );

#endif


#endif // _FMIFS_DEFN_
