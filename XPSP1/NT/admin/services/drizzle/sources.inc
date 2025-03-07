!IF 0

Copyright (C) Microsoft Corporation, 2000 - 2000

Module Name:

    sources.inc

Note:

    Common Drizzle build switches.

!ENDIF

# USE_WININET=1
BITS_V12=1

MSC_WARNING_LEVEL=/WX /W3
USE_MSVCRT=1
USE_NATIVE_EH=1

C_DEFINES = $(C_DEFINES) -DUNICODE -D_UNICODE  -DWPP_CHECK_FOR_NULL_STRING

!if defined(USE_WININET)
C_DEFINES = $(C_DEFINES) -DUSE_WININET
!endif

!if defined(BITS_V12_ON_NT4)
C_DEFINES = $(C_DEFINES) -DBITS_V12_ON_NT4 -DBITS_DONT_USE_ASYNC_DCOM
BITS_V12=1
MIDL_OPTIMIZATION = /Oicf -error all
!endif

!if defined(BITS_V12)
C_DEFINES = $(C_DEFINES) -DBITS_V12
!endif

LINKER_FLAGS = $(LINKER_FLAGS) -ignore:4049,4217
