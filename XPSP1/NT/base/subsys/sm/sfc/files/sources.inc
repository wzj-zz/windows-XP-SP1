TARGETNAME=sfcfiles
TARGETPATH=obj
TARGETTYPE=DYNLINK

DLLDEF=..\sfcfiles.def
DLLENTRY=SfcDllEntry

INCLUDES=\
         $(ADMIN_INC_PATH);

NO_STRING_POOLING=1

MSC_WARNING_LEVEL=/W3 /WX

USE_NTDLL=1

SOURCES=\
  ..\sfcfiles.c\
  ..\sfcfiles.rc
