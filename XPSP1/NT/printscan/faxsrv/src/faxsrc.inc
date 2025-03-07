#
# All binaries will end up in the fax directory
#

# Mapping of old fax image names to new (Whistler) image names.
# fax projects uses this macros to figure out the name of the image they should generate.
# If you want to change the name of an image you should change the macro here and not
# change the project SOURCE file directly.

!ifndef FAXROOT
FAXROOT=$(BASEDIR)\printscan\faxsrv\src
!endif


FAXSETUP=SETUP
FAXSVC=FXSSVC
FAXADMIN=FXSADMIN
WINFAX=FXSAPI
WINFAXP=FXSAPIP
FAXCOM=FXSCOM
FAXCOMEX=FXSCOMEX
FAXDRV=FXSDRV
FAXEVENT=FXSEVENT
FAXMON=FXSMON
FAXPERF=FXSPERF
FAXROUTE=FXSROUTE
FAXTIFF=FXSTIFF
FAXT30=FXST30
FAXUI=FXSUI
FAXXP32=FXSXP32
FAXPNTPRT=FXSPTPRT
ROUTEEXT=FXSRTEXT
FAXQUEUE=FXSQUEUE
FAXCOVER=FXSCOVER
FAXSEND=FXSSEND
FAXEXT32=FXSEXT32
FAXOCM=FXSOCM
FAXCTXTMNU=FXSCTXT
FAXWZRD=FXSWZRD
FAXRES=FXSRES
T30PROP=FXST30P
FxsRouteMethodSnp=FXSRTMTD
FxsRouteExtSnp=FXSRTEXT
MSFXSSNP=FXSADMIN
FAXCOUNT=FXSCOUNT
FAXPERF=FXSPERF
FAXEXT_ECF=FXSEXT
FAXCLIENT=FXSCLNT
FAXCLIENTRES=FXSCLNTR
FAXST=FXSST
FAXCFGWZRD=FXSCFGWZ


BINPLACE_OVERRIDE_FLAGS=
C_DEFINES=$(C_DEFINES) -DBUILD_PRODUCT=$(BUILD_PRODUCT)

!if !$(FREEBUILD)
C_DEFINES=$(C_DEFINES) -DDEBUG -DFAX_HEAP_DEBUG
!if "$(PROCESSOR_ARCHITECTURE)" == "x86"
MSC_OPTIMIZATION=-Od
!endif
!endif

!ifndef NOUNICODE
C_DEFINES=$(C_DEFINES) -DUNICODE -D_UNICODE
LINKLIBS=$(FAXROOT)\util\unicode\$O\faxutil.lib
!else
C_DEFINES=$(C_DEFINES) -D_MBCS
LINKLIBS=$(FAXROOT)\util\ansii\$O\faxutil.lib
!endif

C_DEFINES=$(C_DEFINES) -DTAPI_CURRENT_VERSION=0x00020000 -DNT5BETA2

FAXLIB=$(FAXROOT)\lib

INCLUDES=$(FAXROOT)\inc;$(FAXROOT)\build

MSC_WARNING_LEVEL=/W3 /WX
LINKER_FLAGS=-IGNORE:4006,4088,4104

COMPILER_WARNINGS=-FI$(SDK_INC_PATH)\warning.h -FI$(FAXROOT)\inc\warning.h

!if $(PPC)
COMPILER_WARNINGS=$(COMPILER_WARNINGS) /W3 /WX
!endif

#
# force the build lab's checked
# build machines to have full symbolic
# debug info
#
!if "$(NTDEBUG)" == "cvp"
BINPLACE_FLAGS=$(BINPLACE_FLAGS) -Z
NTDEBUG=ntsd
NTDEBUGTYPE=windbg
!endif

#
# make us completely self contained
#
COFFBASE_TXT_FILE=$(FAXROOT)\NT_coffbase.txt
BINPLACE_PLACEFILE=$(FAXROOT)\NT_placefil.txt


#
# Figure out where to place the images
#

!ifndef OVERRIDE_DEFAULT_BINPLACE
_NTTREE=$(_NTTREE)\fax\i386
!endif



