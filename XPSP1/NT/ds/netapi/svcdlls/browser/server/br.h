/*++

Copyright (c) 1991 Microsoft Corporation

Module Name:

    br.h

Abstract:

    Private header file for the NT Browser service included by every
    module of the Workstation service.

Author:

    Rita Wong (ritaw) 15-Feb-1991

Revision History:

--*/

#ifndef _BR_INCLUDED_
#define _BR_INCLUDED_


#include <nt.h>                   // NT definitions
#include <ntrtl.h>                // NT runtime library definitions
#include <nturtl.h>

#include <windef.h>               // Win32 type definitions
#include <winbase.h>              // Win32 base API prototypes
#include <winsvc.h>               // Win32 service control APIs

#include <lmcons.h>               // LAN Manager common definitions
#include <lmerr.h>                // LAN Manager network error definitions
#include <lmsname.h>              // LAN Manager service names
#include <lmapibuf.h>             // NetApiBufferFree
#include <lmserver.h>

#include <netlib.h>               // LAN Man utility routines
#include <netlibnt.h>             // NetpNtStatusToApiStatus
#include <netdebug.h>             // NetpDbgPrint
#include <tstring.h>              // Transitional string functions
#include <icanon.h>               // I_Net canonicalize functions
#include <align.h>                // ROUND_UP_COUNT macro
#include <svcs.h>                 // Intrinsic service definitions
#include <strarray.h>

#include <rpc.h>                  // DataTypes and runtime APIs
#include <rpcutil.h>              // Prototypes for MIDL user functions
#include <bowser.h>               // Generated by the MIDL complier
#include <winsvc.h>
#include <srvann.h>
#include <lmbrowsr.h>

#include <ntddbrow.h>
#include <brcommon.h>               // Common browser routines.
#include <rx.h>
#include <rxserver.h>

#include <brconst.h>
#include "bowqueue.h"
#include "brdomain.h"
#include "browsnet.h"
#include "browslst.h"
#include "brutil.h"
#include "brwan.h"
#include "brmain.h"
#include "brdevice.h"
#include "brconfig.h"
#include "browsdom.h"
#include "brbackup.h"
#include "brmaster.h"
#include "srvenum.h"

//
//  The following macros are used to establish the semantics needed
//  to do a return from within a try-finally clause.  As a rule every
//  try clause must end with a label call try_exit.  For example,
//
//      try {
//              :
//              :
//
//      try_exit: NOTHING;
//      } finally {
//
//              :
//              :
//      }
//
//  Every return statement executed inside of a try clause should use the
//  try_return macro.  If the compiler fully supports the try-finally construct
//  then the macro should be
//
//      #define try_return(S)  { return(S); }
//
//  If the compiler does not support the try-finally construct then the macro
//  should be
//

#define try_return(S)  { S; goto try_exit; }




#if DBG

#define BrPrint(_x_) BrowserTrace _x_


#else

#define BrPrint(_x_)

#endif // DBG


#endif // ifdef _BR_INCLUDED_
