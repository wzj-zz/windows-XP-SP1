$(OBJDIR)\debug.obj $(OBJDIR)\debug.lst: ..\debug.cpp \
	..\..\..\..\..\dev\inc\commctrl.h ..\..\..\..\..\dev\inc\commdlg.h \
	..\..\..\..\..\dev\inc\imm.h ..\..\..\..\..\dev\inc\mcx.h \
	..\..\..\..\..\dev\inc\mmsystem.h ..\..\..\..\..\dev\inc\netmpr.h \
	..\..\..\..\..\dev\inc\prsht.h ..\..\..\..\..\dev\inc\setupapi.h \
	..\..\..\..\..\dev\inc\shellapi.h ..\..\..\..\..\dev\inc\winbase.h \
	..\..\..\..\..\dev\inc\wincon.h ..\..\..\..\..\dev\inc\windef.h \
	..\..\..\..\..\dev\inc\windows.h ..\..\..\..\..\dev\inc\wingdi.h \
	..\..\..\..\..\dev\inc\winnetwk.h ..\..\..\..\..\dev\inc\winnls.h \
	..\..\..\..\..\dev\inc\winnt.h ..\..\..\..\..\dev\inc\winreg.h \
	..\..\..\..\..\dev\inc\winspool.h ..\..\..\..\..\dev\inc\winuser.h \
	..\..\..\..\..\dev\sdk\inc\basetyps.h \
	..\..\..\..\..\dev\sdk\inc\cguid.h ..\..\..\..\..\dev\sdk\inc\dlgs.h \
	..\..\..\..\..\dev\sdk\inc\excpt.h \
	..\..\..\..\..\dev\sdk\inc\oaidl.h \
	..\..\..\..\..\dev\sdk\inc\objbase.h \
	..\..\..\..\..\dev\sdk\inc\objidl.h ..\..\..\..\..\dev\sdk\inc\ole.h \
	..\..\..\..\..\dev\sdk\inc\ole2.h \
	..\..\..\..\..\dev\sdk\inc\ole2ver.h \
	..\..\..\..\..\dev\sdk\inc\oleauto.h \
	..\..\..\..\..\dev\sdk\inc\oleidl.h \
	..\..\..\..\..\dev\sdk\inc\poppack.h \
	..\..\..\..\..\dev\sdk\inc\pshpack1.h \
	..\..\..\..\..\dev\sdk\inc\pshpack2.h \
	..\..\..\..\..\dev\sdk\inc\pshpack4.h \
	..\..\..\..\..\dev\sdk\inc\pshpack8.h \
	..\..\..\..\..\dev\sdk\inc\unknwn.h \
	..\..\..\..\..\dev\sdk\inc\winerror.h \
	..\..\..\..\..\dev\sdk\inc\winsock.h \
	..\..\..\..\..\dev\sdk\inc\wtypes.h \
	..\..\..\..\..\dev\tools\c1032\inc\cderr.h \
	..\..\..\..\..\dev\tools\c1032\inc\ctype.h \
	..\..\..\..\..\dev\tools\c1032\inc\dde.h \
	..\..\..\..\..\dev\tools\c1032\inc\ddeml.h \
	..\..\..\..\..\dev\tools\c1032\inc\lzexpand.h \
	..\..\..\..\..\dev\tools\c1032\inc\malloc.h \
	..\..\..\..\..\dev\tools\c1032\inc\nb30.h \
	..\..\..\..\..\dev\tools\c1032\inc\rpc.h \
	..\..\..\..\..\dev\tools\c1032\inc\rpcndr.h \
	..\..\..\..\..\dev\tools\c1032\inc\rpcnsip.h \
	..\..\..\..\..\dev\tools\c1032\inc\stdarg.h \
	..\..\..\..\..\dev\tools\c1032\inc\stdlib.h \
	..\..\..\..\..\dev\tools\c1032\inc\string.h \
	..\..\..\..\..\dev\tools\c1032\inc\winperf.h \
	..\..\..\..\..\dev\tools\c1032\inc\winsvc.h \
	..\..\..\..\..\dev\tools\c1032\inc\winver.h \
	..\..\..\..\ddk\inc\hidclass.h ..\..\..\..\ddk\inc\hidpi.h \
	..\..\..\..\ddk\inc\hidsdi.h ..\..\..\..\ddk\inc\hidusage.h \
	..\..\inc\opos.h ..\oposserv.h
.PRECIOUS: $(OBJDIR)\debug.lst

$(OBJDIR)\iclsfact.obj $(OBJDIR)\iclsfact.lst: ..\iclsfact.cpp \
	..\..\..\..\..\dev\inc\commctrl.h ..\..\..\..\..\dev\inc\commdlg.h \
	..\..\..\..\..\dev\inc\imm.h ..\..\..\..\..\dev\inc\mcx.h \
	..\..\..\..\..\dev\inc\mmsystem.h ..\..\..\..\..\dev\inc\netmpr.h \
	..\..\..\..\..\dev\inc\prsht.h ..\..\..\..\..\dev\inc\setupapi.h \
	..\..\..\..\..\dev\inc\shellapi.h ..\..\..\..\..\dev\inc\winbase.h \
	..\..\..\..\..\dev\inc\wincon.h ..\..\..\..\..\dev\inc\windef.h \
	..\..\..\..\..\dev\inc\windows.h ..\..\..\..\..\dev\inc\wingdi.h \
	..\..\..\..\..\dev\inc\winnetwk.h ..\..\..\..\..\dev\inc\winnls.h \
	..\..\..\..\..\dev\inc\winnt.h ..\..\..\..\..\dev\inc\winreg.h \
	..\..\..\..\..\dev\inc\winspool.h ..\..\..\..\..\dev\inc\winuser.h \
	..\..\..\..\..\dev\sdk\inc\basetyps.h \
	..\..\..\..\..\dev\sdk\inc\cguid.h ..\..\..\..\..\dev\sdk\inc\dlgs.h \
	..\..\..\..\..\dev\sdk\inc\excpt.h \
	..\..\..\..\..\dev\sdk\inc\oaidl.h \
	..\..\..\..\..\dev\sdk\inc\objbase.h \
	..\..\..\..\..\dev\sdk\inc\objidl.h ..\..\..\..\..\dev\sdk\inc\ole.h \
	..\..\..\..\..\dev\sdk\inc\ole2.h \
	..\..\..\..\..\dev\sdk\inc\ole2ver.h \
	..\..\..\..\..\dev\sdk\inc\oleauto.h \
	..\..\..\..\..\dev\sdk\inc\oleidl.h \
	..\..\..\..\..\dev\sdk\inc\poppack.h \
	..\..\..\..\..\dev\sdk\inc\pshpack1.h \
	..\..\..\..\..\dev\sdk\inc\pshpack2.h \
	..\..\..\..\..\dev\sdk\inc\pshpack4.h \
	..\..\..\..\..\dev\sdk\inc\pshpack8.h \
	..\..\..\..\..\dev\sdk\inc\unknwn.h \
	..\..\..\..\..\dev\sdk\inc\winerror.h \
	..\..\..\..\..\dev\sdk\inc\winsock.h \
	..\..\..\..\..\dev\sdk\inc\wtypes.h \
	..\..\..\..\..\dev\tools\c1032\inc\cderr.h \
	..\..\..\..\..\dev\tools\c1032\inc\ctype.h \
	..\..\..\..\..\dev\tools\c1032\inc\dde.h \
	..\..\..\..\..\dev\tools\c1032\inc\ddeml.h \
	..\..\..\..\..\dev\tools\c1032\inc\lzexpand.h \
	..\..\..\..\..\dev\tools\c1032\inc\malloc.h \
	..\..\..\..\..\dev\tools\c1032\inc\nb30.h \
	..\..\..\..\..\dev\tools\c1032\inc\rpc.h \
	..\..\..\..\..\dev\tools\c1032\inc\rpcndr.h \
	..\..\..\..\..\dev\tools\c1032\inc\rpcnsip.h \
	..\..\..\..\..\dev\tools\c1032\inc\stdarg.h \
	..\..\..\..\..\dev\tools\c1032\inc\stdlib.h \
	..\..\..\..\..\dev\tools\c1032\inc\string.h \
	..\..\..\..\..\dev\tools\c1032\inc\winperf.h \
	..\..\..\..\..\dev\tools\c1032\inc\winsvc.h \
	..\..\..\..\..\dev\tools\c1032\inc\winver.h \
	..\..\..\..\ddk\inc\hidclass.h ..\..\..\..\ddk\inc\hidpi.h \
	..\..\..\..\ddk\inc\hidsdi.h ..\..\..\..\ddk\inc\hidusage.h \
	..\..\inc\opos.h ..\oposserv.h
.PRECIOUS: $(OBJDIR)\iclsfact.lst

$(OBJDIR)\iunknown.obj $(OBJDIR)\iunknown.lst: ..\iunknown.cpp \
	..\..\..\..\..\dev\inc\commctrl.h ..\..\..\..\..\dev\inc\commdlg.h \
	..\..\..\..\..\dev\inc\imm.h ..\..\..\..\..\dev\inc\mcx.h \
	..\..\..\..\..\dev\inc\mmsystem.h ..\..\..\..\..\dev\inc\netmpr.h \
	..\..\..\..\..\dev\inc\prsht.h ..\..\..\..\..\dev\inc\setupapi.h \
	..\..\..\..\..\dev\inc\shellapi.h ..\..\..\..\..\dev\inc\winbase.h \
	..\..\..\..\..\dev\inc\wincon.h ..\..\..\..\..\dev\inc\windef.h \
	..\..\..\..\..\dev\inc\windows.h ..\..\..\..\..\dev\inc\wingdi.h \
	..\..\..\..\..\dev\inc\winnetwk.h ..\..\..\..\..\dev\inc\winnls.h \
	..\..\..\..\..\dev\inc\winnt.h ..\..\..\..\..\dev\inc\winreg.h \
	..\..\..\..\..\dev\inc\winspool.h ..\..\..\..\..\dev\inc\winuser.h \
	..\..\..\..\..\dev\sdk\inc\basetyps.h \
	..\..\..\..\..\dev\sdk\inc\cguid.h ..\..\..\..\..\dev\sdk\inc\dlgs.h \
	..\..\..\..\..\dev\sdk\inc\excpt.h \
	..\..\..\..\..\dev\sdk\inc\oaidl.h \
	..\..\..\..\..\dev\sdk\inc\objbase.h \
	..\..\..\..\..\dev\sdk\inc\objidl.h ..\..\..\..\..\dev\sdk\inc\ole.h \
	..\..\..\..\..\dev\sdk\inc\ole2.h \
	..\..\..\..\..\dev\sdk\inc\ole2ver.h \
	..\..\..\..\..\dev\sdk\inc\oleauto.h \
	..\..\..\..\..\dev\sdk\inc\oleidl.h \
	..\..\..\..\..\dev\sdk\inc\poppack.h \
	..\..\..\..\..\dev\sdk\inc\pshpack1.h \
	..\..\..\..\..\dev\sdk\inc\pshpack2.h \
	..\..\..\..\..\dev\sdk\inc\pshpack4.h \
	..\..\..\..\..\dev\sdk\inc\pshpack8.h \
	..\..\..\..\..\dev\sdk\inc\unknwn.h \
	..\..\..\..\..\dev\sdk\inc\winerror.h \
	..\..\..\..\..\dev\sdk\inc\winsock.h \
	..\..\..\..\..\dev\sdk\inc\wtypes.h \
	..\..\..\..\..\dev\tools\c1032\inc\cderr.h \
	..\..\..\..\..\dev\tools\c1032\inc\ctype.h \
	..\..\..\..\..\dev\tools\c1032\inc\dde.h \
	..\..\..\..\..\dev\tools\c1032\inc\ddeml.h \
	..\..\..\..\..\dev\tools\c1032\inc\lzexpand.h \
	..\..\..\..\..\dev\tools\c1032\inc\malloc.h \
	..\..\..\..\..\dev\tools\c1032\inc\nb30.h \
	..\..\..\..\..\dev\tools\c1032\inc\rpc.h \
	..\..\..\..\..\dev\tools\c1032\inc\rpcndr.h \
	..\..\..\..\..\dev\tools\c1032\inc\rpcnsip.h \
	..\..\..\..\..\dev\tools\c1032\inc\stdarg.h \
	..\..\..\..\..\dev\tools\c1032\inc\stdlib.h \
	..\..\..\..\..\dev\tools\c1032\inc\string.h \
	..\..\..\..\..\dev\tools\c1032\inc\winperf.h \
	..\..\..\..\..\dev\tools\c1032\inc\winsvc.h \
	..\..\..\..\..\dev\tools\c1032\inc\winver.h \
	..\..\..\..\ddk\inc\hidclass.h ..\..\..\..\ddk\inc\hidpi.h \
	..\..\..\..\ddk\inc\hidsdi.h ..\..\..\..\ddk\inc\hidusage.h \
	..\..\inc\opos.h ..\oposserv.h
.PRECIOUS: $(OBJDIR)\iunknown.lst

$(OBJDIR)\main.obj $(OBJDIR)\main.lst: ..\main.cpp \
	..\..\..\..\..\dev\inc\commctrl.h ..\..\..\..\..\dev\inc\commdlg.h \
	..\..\..\..\..\dev\inc\imm.h ..\..\..\..\..\dev\inc\mcx.h \
	..\..\..\..\..\dev\inc\mmsystem.h ..\..\..\..\..\dev\inc\netmpr.h \
	..\..\..\..\..\dev\inc\prsht.h ..\..\..\..\..\dev\inc\setupapi.h \
	..\..\..\..\..\dev\inc\shellapi.h ..\..\..\..\..\dev\inc\winbase.h \
	..\..\..\..\..\dev\inc\wincon.h ..\..\..\..\..\dev\inc\windef.h \
	..\..\..\..\..\dev\inc\windows.h ..\..\..\..\..\dev\inc\wingdi.h \
	..\..\..\..\..\dev\inc\winnetwk.h ..\..\..\..\..\dev\inc\winnls.h \
	..\..\..\..\..\dev\inc\winnt.h ..\..\..\..\..\dev\inc\winreg.h \
	..\..\..\..\..\dev\inc\winspool.h ..\..\..\..\..\dev\inc\winuser.h \
	..\..\..\..\..\dev\sdk\inc\basetyps.h \
	..\..\..\..\..\dev\sdk\inc\cguid.h ..\..\..\..\..\dev\sdk\inc\dlgs.h \
	..\..\..\..\..\dev\sdk\inc\excpt.h \
	..\..\..\..\..\dev\sdk\inc\oaidl.h \
	..\..\..\..\..\dev\sdk\inc\objbase.h \
	..\..\..\..\..\dev\sdk\inc\objidl.h ..\..\..\..\..\dev\sdk\inc\ole.h \
	..\..\..\..\..\dev\sdk\inc\ole2.h \
	..\..\..\..\..\dev\sdk\inc\ole2ver.h \
	..\..\..\..\..\dev\sdk\inc\oleauto.h \
	..\..\..\..\..\dev\sdk\inc\oleidl.h \
	..\..\..\..\..\dev\sdk\inc\poppack.h \
	..\..\..\..\..\dev\sdk\inc\pshpack1.h \
	..\..\..\..\..\dev\sdk\inc\pshpack2.h \
	..\..\..\..\..\dev\sdk\inc\pshpack4.h \
	..\..\..\..\..\dev\sdk\inc\pshpack8.h \
	..\..\..\..\..\dev\sdk\inc\unknwn.h \
	..\..\..\..\..\dev\sdk\inc\winerror.h \
	..\..\..\..\..\dev\sdk\inc\winsock.h \
	..\..\..\..\..\dev\sdk\inc\wtypes.h \
	..\..\..\..\..\dev\tools\c1032\inc\cderr.h \
	..\..\..\..\..\dev\tools\c1032\inc\ctype.h \
	..\..\..\..\..\dev\tools\c1032\inc\dde.h \
	..\..\..\..\..\dev\tools\c1032\inc\ddeml.h \
	..\..\..\..\..\dev\tools\c1032\inc\lzexpand.h \
	..\..\..\..\..\dev\tools\c1032\inc\malloc.h \
	..\..\..\..\..\dev\tools\c1032\inc\nb30.h \
	..\..\..\..\..\dev\tools\c1032\inc\rpc.h \
	..\..\..\..\..\dev\tools\c1032\inc\rpcndr.h \
	..\..\..\..\..\dev\tools\c1032\inc\rpcnsip.h \
	..\..\..\..\..\dev\tools\c1032\inc\stdarg.h \
	..\..\..\..\..\dev\tools\c1032\inc\stdlib.h \
	..\..\..\..\..\dev\tools\c1032\inc\string.h \
	..\..\..\..\..\dev\tools\c1032\inc\winperf.h \
	..\..\..\..\..\dev\tools\c1032\inc\winsvc.h \
	..\..\..\..\..\dev\tools\c1032\inc\winver.h \
	..\..\..\..\ddk\inc\hidclass.h ..\..\..\..\ddk\inc\hidpi.h \
	..\..\..\..\ddk\inc\hidsdi.h ..\..\..\..\ddk\inc\hidusage.h \
	..\..\inc\opos.h ..\oposserv.h
.PRECIOUS: $(OBJDIR)\main.lst

$(OBJDIR)\oposserv.obj $(OBJDIR)\oposserv.lst: ..\oposserv.cpp \
	..\..\..\..\..\dev\inc\commctrl.h ..\..\..\..\..\dev\inc\commdlg.h \
	..\..\..\..\..\dev\inc\imm.h ..\..\..\..\..\dev\inc\mcx.h \
	..\..\..\..\..\dev\inc\mmsystem.h ..\..\..\..\..\dev\inc\netmpr.h \
	..\..\..\..\..\dev\inc\prsht.h ..\..\..\..\..\dev\inc\setupapi.h \
	..\..\..\..\..\dev\inc\shellapi.h ..\..\..\..\..\dev\inc\winbase.h \
	..\..\..\..\..\dev\inc\wincon.h ..\..\..\..\..\dev\inc\windef.h \
	..\..\..\..\..\dev\inc\windows.h ..\..\..\..\..\dev\inc\wingdi.h \
	..\..\..\..\..\dev\inc\winnetwk.h ..\..\..\..\..\dev\inc\winnls.h \
	..\..\..\..\..\dev\inc\winnt.h ..\..\..\..\..\dev\inc\winreg.h \
	..\..\..\..\..\dev\inc\winspool.h ..\..\..\..\..\dev\inc\winuser.h \
	..\..\..\..\..\dev\sdk\inc\basetyps.h \
	..\..\..\..\..\dev\sdk\inc\cguid.h ..\..\..\..\..\dev\sdk\inc\dlgs.h \
	..\..\..\..\..\dev\sdk\inc\excpt.h \
	..\..\..\..\..\dev\sdk\inc\oaidl.h \
	..\..\..\..\..\dev\sdk\inc\objbase.h \
	..\..\..\..\..\dev\sdk\inc\objidl.h ..\..\..\..\..\dev\sdk\inc\ole.h \
	..\..\..\..\..\dev\sdk\inc\ole2.h \
	..\..\..\..\..\dev\sdk\inc\ole2ver.h \
	..\..\..\..\..\dev\sdk\inc\oleauto.h \
	..\..\..\..\..\dev\sdk\inc\oleidl.h \
	..\..\..\..\..\dev\sdk\inc\poppack.h \
	..\..\..\..\..\dev\sdk\inc\pshpack1.h \
	..\..\..\..\..\dev\sdk\inc\pshpack2.h \
	..\..\..\..\..\dev\sdk\inc\pshpack4.h \
	..\..\..\..\..\dev\sdk\inc\pshpack8.h \
	..\..\..\..\..\dev\sdk\inc\unknwn.h \
	..\..\..\..\..\dev\sdk\inc\winerror.h \
	..\..\..\..\..\dev\sdk\inc\winsock.h \
	..\..\..\..\..\dev\sdk\inc\wtypes.h \
	..\..\..\..\..\dev\tools\c1032\inc\cderr.h \
	..\..\..\..\..\dev\tools\c1032\inc\ctype.h \
	..\..\..\..\..\dev\tools\c1032\inc\dde.h \
	..\..\..\..\..\dev\tools\c1032\inc\ddeml.h \
	..\..\..\..\..\dev\tools\c1032\inc\lzexpand.h \
	..\..\..\..\..\dev\tools\c1032\inc\malloc.h \
	..\..\..\..\..\dev\tools\c1032\inc\nb30.h \
	..\..\..\..\..\dev\tools\c1032\inc\rpc.h \
	..\..\..\..\..\dev\tools\c1032\inc\rpcndr.h \
	..\..\..\..\..\dev\tools\c1032\inc\rpcnsip.h \
	..\..\..\..\..\dev\tools\c1032\inc\stdarg.h \
	..\..\..\..\..\dev\tools\c1032\inc\stdlib.h \
	..\..\..\..\..\dev\tools\c1032\inc\string.h \
	..\..\..\..\..\dev\tools\c1032\inc\winioctl.h \
	..\..\..\..\..\dev\tools\c1032\inc\winperf.h \
	..\..\..\..\..\dev\tools\c1032\inc\winsvc.h \
	..\..\..\..\..\dev\tools\c1032\inc\winver.h \
	..\..\..\..\ddk\inc\hidclass.h ..\..\..\..\ddk\inc\hidpi.h \
	..\..\..\..\ddk\inc\hidsdi.h ..\..\..\..\ddk\inc\hidusage.h \
	..\..\inc\opos.h ..\oposserv.h
.PRECIOUS: $(OBJDIR)\oposserv.lst

$(OBJDIR)\oposserv.res $(OBJDIR)\oposserv.lst: ..\oposserv.rc
.PRECIOUS: $(OBJDIR)\oposserv.lst

