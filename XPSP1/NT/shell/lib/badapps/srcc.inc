!IF 0

Copyright (c) 1989-1996 Microsoft Corporation

Module Name:

    sources.

Abstract:

    This file specifies the target component being built and the list of
    sources files needed to build that component.  Also specifies optional
    compiler switches and libraries that are unique for the component being
    built.


History:
    Common ccshell makefile and cleanup 15-Apr-96 Scott Hysom (scotth)
    Created 27-Sep-94 by Bob Day (bobday)
    from template created 12-Apr-1990 by Steve Wood (stevewo)

NOTE:   Commented description of this file is in \nt\public\oak\bin\sources.tpl

!ENDIF

CCSHELL_DIR     = $(PROJECT_ROOT)

!include $(CCSHELL_DIR)\common.inc
SOURCES_USED    = $(SOURCES_USED) $(CCSHELL_DIR)\common.inc

# Winbasep.h
INCLUDES=$(INCLUDES);$(BASE_INC_PATH)

MAJORCOMP=shell
MINORCOMP=badapps

# target
TARGETPATH=obj
TARGETTYPE=LIBRARY

MSC_WARNING_LEVEL=/W3 /WX

SOURCES=\
        ..\utils.c     \
        ..\version.c   \
        ..\modules.c   \
        ..\badapps.c   \
