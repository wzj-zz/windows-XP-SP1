
# NOTE:
# this directory contains a makefile which contains a single line that
# includes the global build process makefile.def. If the
# NTTARGETFILE1 or NTTARGETFILE0 environment
# variable is set then makefile.def includes makefile.inc from the current
# directory. This makefile.inc creates an extra target for nmake to create
# when it is run. NTTARGETFILE0 is built before everything else, and
# NTTARGETFILE1 is built after everything else.

copyfiles:
  @if exist obj\$(TARGET_DIRECTORY)\$(TARGETNAME).dll  \
    copy obj\$(TARGET_DIRECTORY)\$(TARGETNAME).dll     \
         $(SDK_ROOT)\bin\*.*
  @if not exist $(QUARTZ)\lib\$(TARGET_DIRECTORY)\nul
         md $(QUARTZ)\lib\$(TARGET_DIRECTORY)
  @if exist obj\$(TARGET_DIRECTORY)\$(TARGETNAME).lib  \
    copy obj\$(TARGET_DIRECTORY)\$(TARGETNAME).lib     \
         $(QUARTZ)\lib\$(TARGET_DIRECTORY)\*.*
