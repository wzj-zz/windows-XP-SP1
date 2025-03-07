############################################################################

#

# Copyright (c) 1999-2001 Microsoft Corporation, All Rights Reserved
#
#   All rights reserved.
#
############################################################################

############################################################################
# Common Options to all ActiveXSuite OCXs
############################################################################
386_STDCALL=0

TARGETEXT=ocx
TARGETPATH=obj
TARGETTYPE=DYNLINK

C_DEFINES=$(C_DEFINES) /DSMSBUILD

USER_C_FLAGS=/EHa
MSC_OPTIMIZATION=/Ob2
DLLENTRY=_DllMainCRTStartup

USE_MSVCRT=1
USE_MFCVER=42
USE_MFCUNICODE=1
USE_MFC=1
PLATFORM_MFC_VER=0x0600

TARGETLIBS= $(TARGETLIBS) \
	$(ACTIVEXCONTROLS)\MFC60Hack\mfc42u.lib

# The INCLUDES variable specifies any include paths that are specific to
# this source directory. Separate multiple paths with single
# semicolons. Relative path specifications are okay.
INCLUDES=$(INCLUDES); \
	$(STDLIBRARY_INC); \
	$(WMIIDL_INC); \
	$(ACTIVEX_WBEMUTIL_INC); \
	$(ACTIVEX_WBEMLOGINDLG_INC); \
	$(ARTWORK)

# The TARGETLIBS macro specifies additional libraries to link against your target
# image. Each library path specification should contain an asterisk (*)
# where the machine-specific subdirectory name should go.
TARGETLIBS=$(TARGETLIBS) \
	$(WMIIDL_LIB) \
	$(WBEMINT_LIB) \
	$(STDLIBRARY_LIB) \
	$(ACTIVEX_WBEMUTIL_LIB) \
	$(ACTIVEX_WBEMLOGINDLG_LIB) \
	$(SDK_LIB_PATH)\kernel32.lib \
	$(SDK_LIB_PATH)\gdi32.lib \
	$(SDK_LIB_PATH)\advapi32.lib \
	$(SDK_LIB_PATH)\user32.lib \
	$(SDK_LIB_PATH)\version.lib \
	$(SDK_LIB_PATH)\oleaut32.lib \
	$(SDK_LIB_PATH)\ole32.lib \
	$(SDK_LIB_PATH)\uuid.lib \
	$(SDK_LIB_PATH)\mpr.lib\
	$(SDK_LIB_PATH)\odbccp32.lib\
	$(SDK_LIB_PATH)\vccomsup.lib\
	$(SDK_LIB_PATH)\comctl32.lib\
	$(SDK_LIB_PATH)\shell32.lib\
	$(SDK_LIB_PATH)\rpcrt4.lib \
	$(SDK_LIB_PATH)\Urlmon.lib \
	$(SDK_LIB_PATH)\comdlg32.lib \
	$(SDK_LIB_PATH)\htmlhelp.lib
