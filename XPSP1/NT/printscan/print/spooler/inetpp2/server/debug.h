/*****************************************************************************\
* MODULE: debug.h
*
* Header file for (debug.c).
*
* Double braces are needed for output macros.
*
*     DBGMSG(DBG_LEV_ERROR, ("Error code %d", Error));
*
* This is because we can't use variable parameter lists in macros.
* The statement gets pre-processed to a semi-colon in non-debug mode.
*
* Set the global variable GLOBAL_DEBUG_FLAGS via the debugger.
* Setting the flag in the low word causes that level to be printed;
* setting the high word causes a break into the debugger.
* E.g. setting it to 0x00040006 will print out all warning and error
* messages, and break on errors.
*
*
* Copyright (C) 1996-1997 Microsoft Corporation
* Copyright (C) 1996-1997 Hewlett Packard
*
* History:
*   07-Oct-1996 HWP-Guys    Initiated port from win95 to winNT
*
\*****************************************************************************/
#ifndef _INETPP_DEBUG_H
#define _INETPP_DEBUG_H

#ifdef DEBUG

extern DWORD gdwDbgLevel;


// Maximum size of debug string
//
#define DBG_MAX_TEXT       256


// Debug output levels.  By masking in the various levels, you can receive
// all levels of output.
//
#define DBG_LEV_INFO       0x00000001
#define DBG_LEV_CALLTREE   0x00000002
#define DBG_LEV_WARN       0x00000004
#define DBG_LEV_ERROR      0x00000008
#define DBG_LEV_FATAL      0x00000010
#define DBG_CACHE_TRACE    0x00000020
#define DBG_CACHE_ERROR    0x00000040

#define DBG_LEV_ALL        0x0000007F


// Function prototype for debug-routine (debug.c)
//
VOID CDECL DbgMsgOut(
    LPCTSTR lpszMsgFormat,
    ...);

LPTSTR DbgGetTime (void);


VOID DbgMsg (LPCTSTR pszFormat, ...);


// Handy macros for use throughout the source.
//
#define DBG_BREAKPOINT()          DebugBreak();
#define DBG_MSG(Lev, MsgArgs)     {if (Lev & gdwDbgLevel) {DbgMsgOut MsgArgs;}}
#define DBG_ASSERT(Expr, MsgArgs) {if (!Expr) {DbgMsgOut MsgArgs; DebugBreak();}}
#define DBGMSGT(dwLevel, x) {if (dwLevel & gdwDbgLevel) {DbgMsg x;}}

#else

#define DBG_BREAKPOINT()
#define DBG_MSG(Lev, MsgArgs)
#define DBG_ASSERT(Expr, MsgArgs)
#define DBGMSGT(dwLevel, x)
#endif



#endif
