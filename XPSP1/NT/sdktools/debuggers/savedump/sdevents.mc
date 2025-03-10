;/*++ BUILD Version: 0001    // Increment this if a change has global effects
;
;Copyright (c) 1994  Microsoft Corporation
;
;Module Name:
;
;    sdevents.h
;
;Abstract:
;
;    Definitions for Save Dump Utility.
;
;Revision History:
;
;Notes:
;
;    This file is generated by the MC tool from the sdevents.mc file.
;
;--*/
;
;
;#ifndef _SDEVENT_
;#define _SDEVENT_
;


SeverityNames=(Success=0x0:STATUS_SEVERITY_SUCCESS
               Informational=0x1:STATUS_SEVERITY_INFORMATIONAL
               Warning=0x2:STATUS_SEVERITY_WARNING
               Error=0x3:STATUS_SEVERITY_ERROR
              )

;
;/////////////////////////////////////////////////////////////////////////
;//
;// SaveDump Events (1000 - 1999)
;//
;/////////////////////////////////////////////////////////////////////////
;

MessageId=1000 Severity=Informational SymbolicName=EVENT_BUGCHECK
Language=English
The computer has rebooted from a bugcheck.  The bugcheck was:
%1.
A full dump was not saved.
.

MessageId=1001 Severity=Informational SymbolicName=EVENT_BUGCHECK_SAVED
Language=English
The computer has rebooted from a bugcheck.  The bugcheck was:
%1.
A dump was saved in: %2.
.

;
;#endif // _SDEVENT_
;
