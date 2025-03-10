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

RCOPTIONS=-l 804

SYNCHRONIZE_DRAIN=1

C_DEFINES=$(C_DEFINES) -DCHINESES

MAJORCOMP=INTLWB
MINORCOMP=ChsBrKr

USE_MSVCRT=0

TARGETNAME=ChsBrKr
TARGETPATH=obj
TARGETTYPE=DYNLINK

SUBSYSTEM_VERSION=4.00


INCLUDES=..;$(INCLUDES)


SOURCES=..\ChsBrkr.rc \
        ..\wbclassf.cxx\
        ..\exports.cxx\
        ..\iwbreak.cxx\
        ..\stemcf.cxx\
        ..\stemmer.cxx\
        ..\register.cxx \
        ..\ctplus0.c\
        ..\log.c

UMTYPE=windows

DLLBASE=0x44e00000

DLLENTRY=DllMain

PRECOMPILED_INCLUDE=..\pch.cxx

CONDITIONAL_INCLUDES= \
        $(CONDITIONAL_INCLUDES) \
        thammer.h

DLLDEF=..\ChsBrKr.def

#
# Support for IceCap profiling.
#

!if "$(PERFFLAGS)" == "TRUE"
! if exist ($(SDK_LIB_PATH)\icap.lib)
!  if !defined(MSC_Optimiztion)
MSC_OPTIMIZATION=-Oxs -Gh -MD
!  elseif "$(MSC_OPTIMIZATION)" == "-GX"
MSC_OPTIMIZATION=-Oxs -GX -Gh -MD
!  else
MSC_OPTIMIZATION=$(MSC_OPTIMIZATION) -Gh -MD
!  endif

USE_PDB=1

!  if "$(USE_PENTER)" == "TRUE"
LINKLIBS=$(LINKLIBS) $(SDK_LIB_PATH)\penter.lib
UMLIBS=$(UMLIBS) $(SDK_LIB_PATH)\penter.lib
!  else
LINKLIBS=$(LINKLIBS) $(SDK_LIB_PATH)\ICAP.lib
UMLIBS=$(UMLIBS) $(SDK_LIB_PATH)\ICAP.lib
!  endif

!  if "$(DBG)" == "1"
!   message Warning: Profiling with DBG on!
!  endif

! else
!  error Cannot build for IceCap profiling without ICAP.LIB
! endif
!endif

LINKER_FLAGS=$(LINKER_FLAGS) -IGNORE:4077

LINKLIBS=$(LINKLIBS)\
         $(SDK_LIB_PATH)\ntdll.lib \
         $(SDK_LIB_PATH)\user32.lib\
         $(SDK_LIB_PATH)\kernel32.lib\
         $(SDK_LIB_PATH)\advapi32.lib \
         $(SDK_LIB_PATH)\uuid.lib\
         $(SDK_LIB_PATH)\query.lib

MISCFILES=..\noise.chs
