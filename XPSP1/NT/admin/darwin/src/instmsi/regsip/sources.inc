
TARGETNAME=regsip
!INCLUDE ..\..\..\MsiMake.inc

TARGETTYPE=DYNLINK
TARGETLIBS=$(SDK_LIB_PATH)\kernel32.lib 

C_DEFINES=$(C_DEFINES)
USE_NOLIBS=1
USE_NATIVE_EH=1
DLLDEF=..\regsip.def

INCLUDES=$(INCLUDES);$(INC_DIR);$(TOOLS_INC_DIR);$(RES_OBJDIR)

SOURCES=..\regsip.cpp \
	..\regsip.rc

