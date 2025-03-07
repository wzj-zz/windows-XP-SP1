/*++

Copyright (C) 1995 Microsoft Corporation

Module Name:

    pdhitype.h

Abstract:

    data types used internally by the Data Provider Helper functions.

--*/

#ifndef _PDHI_TYPE_H_
#define _PDHI_TYPE_H_

#include <windows.h>
#include <stdio.h>
#include "pdh.h"
#include "perftype.h"

typedef double  DOUBLE;

// make signature into DWORDs to make this a little faster

#define SigQuery    ((DWORD)0x51484450)    // L"PDHQ"
#define SigCounter  ((DWORD)0x43484450)    // L"PDHC"
#define SigLog      ((DWORD)0x4C484450)    // L"PDHL"

typedef struct _PDHI_QUERY_MACHINE {
    PPERF_MACHINE   pMachine;       // pointer to the machine structure
    LPWSTR          szObjectList;   // list of objects to query on that machine
    PERF_DATA_BLOCK *pPerfData;     // query's perf data block
    LONG            lQueryStatus;   // status of last perf query
    LONGLONG        llQueryTime;    // timestamp from last query attempt
    struct _PDHI_QUERY_MACHINE *pNext;  // next machine in list
} PDHI_QUERY_MACHINE, *PPDHI_QUERY_MACHINE;

typedef struct _PDHI_COUNTER_PATH {
    // machine path
    LPWSTR  szMachineName;      // null = the local machine
    // object Info
    LPWSTR  szObjectName;
    // instance info
    LPWSTR  szInstanceName;     // NULL if no inst.
    LPWSTR  szParentName;       // points to name if instance has a parent
    DWORD   dwIndex;            // index (to support dup. names.) 0 = 1st inst.
    // counter info
    LPWSTR  szCounterName;
    // misc storage
    BYTE    pBuffer[1];         // beginning of string buffer space
} PDHI_COUNTER_PATH, *PPDHI_COUNTER_PATH;

typedef struct _PDHI_RAW_COUNTER_ITEM {
    LPWSTR      szName;
    DWORD       MultiCount;
    LONGLONG    FirstValue;
    LONGLONG    SecondValue;
} PDHI_RAW_COUNTER_ITEM, *PPDHI_RAW_COUNTER_ITEM;

typedef struct _PDHI_RAW_COUNTER_ITEM_BLOCK {
    DWORD                   dwLength;
    DWORD                   dwItemCount;
    DWORD                   dwReserved;
    LONG                    CStatus;
    FILETIME                TimeStamp;
    PDHI_RAW_COUNTER_ITEM   pItemArray[];
} PDHI_RAW_COUNTER_ITEM_BLOCK, *PPDHI_RAW_COUNTER_ITEM_BLOCK;

typedef struct  _PDHI_QUERY_LIST {
    struct _PDHI_QUERY  *flink;
    struct _PDHI_QUERY  *blink;
} PDHI_QUERY_LIST, *PPDHI_QUERY_LIST;

typedef struct  _PDHI_COUNTER_LIST {
    struct _PDHI_COUNTER    *flink;
    struct _PDHI_COUNTER    *blink;
} PDHI_COUNTER_LIST, *PPDHI_COUNTER_LIST;

typedef struct  _PDHI_LOG_LIST {
    struct _PDHI_LOG        *flink;
    struct _PDHI_LOG        *blink;
} PDHI_LOG_LIST, *PPDHI_LOG_LIST;

typedef double (APIENTRY COUNTERCALC) (PPDH_RAW_COUNTER, PPDH_RAW_COUNTER, LONGLONG*, LPDWORD);
typedef double (APIENTRY *LPCOUNTERCALC) (PPDH_RAW_COUNTER, PPDH_RAW_COUNTER, LONGLONG*, LPDWORD);

typedef PDH_STATUS (APIENTRY COUNTERSTAT) (struct _PDHI_COUNTER *, DWORD, DWORD, DWORD, PPDH_RAW_COUNTER, PPDH_STATISTICS);
typedef PDH_STATUS (APIENTRY *LPCOUNTERSTAT) (struct _PDHI_COUNTER *, DWORD, DWORD, DWORD, PPDH_RAW_COUNTER, PPDH_STATISTICS);

typedef struct _PDHI_COUNTER {
    CHAR   signature[4];                // should be "PDHC" for counters
    DWORD   dwLength;                   // length of this structure
    struct _PDHI_QUERY *pOwner;         // pointer to owning query
    LPWSTR  szFullName;                 // full counter path string
    PDHI_COUNTER_LIST next;             // list links
    DWORD   dwUserData;                 // user defined DWORD
    LONG    lScale;                     // integer scale exponent
    // this information is obtained from the system
    DWORD    CVersion;                  // system perfdata version
    DWORD   dwFlags;                    // flags
    PPDHI_QUERY_MACHINE pQMachine;      // pointer to the machine structure
    PPDHI_COUNTER_PATH  pCounterPath;   // parsed counter path
    PDH_RAW_COUNTER ThisValue;          // most recent value
    PDH_RAW_COUNTER LastValue;          // previous value
    LPWSTR  szExplainText;              // pointer to the explain text buffer
    LPCOUNTERCALC       CalcFunc;       // pointer to the calc function
    LPCOUNTERSTAT       StatFunc;       // pointer to the statistics function
    // this field is specific to the Perflib implementation
    LONGLONG    TimeBase;               // freq. of timer used by this counter
    PERFLIB_COUNTER plCounterInfo;      // perflib specific counter data
    // these fields are used by "wildcard" counter handles
    PPDHI_RAW_COUNTER_ITEM_BLOCK    pThisRawItemList;   // pointer to current data set
    PPDHI_RAW_COUNTER_ITEM_BLOCK    pLastRawItemList;   // pointer to previous data set
} PDHI_COUNTER, *PPDHI_COUNTER;

// flags for the PDHI_COUNTER data structure.
#define  PDHIC_MULTI_INSTANCE       ((DWORD)0x00000001)
#define  PDHIC_ASYNC_TIMER          ((DWORD)0x00000002)

typedef struct  _PDHI_QUERY {
    CHAR   signature[4];        // should be "PDHQ" for queries
    PDHI_QUERY_LIST next;       // pointer to next query in list
    PPDHI_QUERY_MACHINE pFirstQMachine; // pointer to first machine in list
    PPDHI_COUNTER   pCounterListHead; // pointer to first counter in list
    DWORD   dwLength;           // length of this structure
    DWORD   dwUserData;
    DWORD   dwInterval;         // interval in seconds
    DWORD   dwNotifyFlags;      // notification flags
    PDH_TIME_INFO   TimeRange;  // query time range
    HLOG    hLog;               // handle to log file (for data source)
    DWORD   dwLastLogIndex;     // the last log record returned to a Get Value call
    HANDLE  hMutex;             // mutex to sync changes to data.
    HANDLE  hNewDataEvent;      // handle to event that is sent when data is collected
    HANDLE  hAsyncThread;       // thread handle for async collection
    HANDLE  hExitEvent;         // event to set for thread to terminate
} PDHI_QUERY, *PPDHI_QUERY;

typedef struct _PDHI_LOG {
    CHAR    signature[4];       // should be "PDHL" for log entries
    PDHI_LOG_LIST   next;       // links to next and previous entries
    DWORD   dwLength;           // the size of this structure
    LPWSTR  szLogFileName;      // full file name for this log file
    HANDLE  hLogFileHandle;     // handle to open log file
    HANDLE  hMappedLogFile;     // handle for memory mapped files
    LPVOID  lpMappedFileBase;   // starting address for mapped log file
    FILE    *StreamFile;        // stream pointer for text files
    LONGLONG llFileSize;        // file size (used only for reading)
    DWORD   dwRecord1Size;      // size of ID record in BLG files, not used by text files
    DWORD   dwLastRecordRead;   // index of last record read from the file
    LPVOID  pLastRecordRead;    // pointer to buffer containing the last record
    LPWSTR  szCatFileName;      // catalog file name
    HANDLE  hCatFileHandle;     // handle to the open catalog file
    PPDHI_QUERY pQuery;         // pointer to the query associated with the log
    LONGLONG llMaxSize;         // max size of a circular log file
    DWORD   dwLogFormat;        // log type and access flags
    DWORD   dwMaxRecordSize;    // size of longest record in log
} PDHI_LOG, *PPDHI_LOG;

#endif // _PDH_TYPE_H_
