!IF 0

Copyright (c) 1989  Microsoft Corporation

Module Name:

    sources.

Abstract:

    This file specifies the target component being built and the list of
    sources files needed to build that component.  Also specifies optional
    compiler switches and libraries that are unique for the component being
    built.


Author:

    Steve Wood (stevewo) 12-Apr-1990

NOTE:   Commented description of this file is in \nt\bak\bin\sources.tpl

!ENDIF

TARGETNAME=themes
TARGETPATH=obj
TARGETTYPE=DYNLINK
TARGETEXT=cpl
DLLENTRY=DllMain
DLLDEF=..\thm_cpl.def

SOURCES=          \
    ..\thm_cpl.rc \
    ..\thm_cpl.c

MSC_WARNING_LEVEL=/W3 /WX

UMTYPE=windows

USE_NOLIBS=1
TARGETLIBS= \
    $(SDK_LIB_PATH)\kernel32.lib  \
    $(SDK_LIB_PATH)\user32.lib    \
    $(SDK_LIB_PATH)\shell32.lib
