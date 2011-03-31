# Microsoft Developer Studio Project File - Name="wbOSD" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=wbOSD - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wbOSD.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wbOSD.mak" CFG="wbOSD - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "wbOSD - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wbOSD - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "wbOSD - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "wbOSD_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O1 /I "../../include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "wbOSD_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40d /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib wsock32.lib /nologo /base:"0x32240000" /dll /pdb:none /map /machine:I386
# SUBTRACT LINK32 /verbose

!ELSEIF  "$(CFG)" == "wbOSD - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "C:\MirTest\plugins"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "wbOSD_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "wbOSD_EXPORTS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40d /d "_DEBUG"
# ADD RSC /l 0x40d /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib wsock.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /verbose /map

!ENDIF 

# Begin Target

# Name "wbOSD - Win32 Release"
# Name "wbOSD - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "3rd-Party"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\3rd-Party\bzip2-1.0.3\blocksort.c"
# End Source File
# Begin Source File

SOURCE=".\3rd-Party\bzip2-1.0.3\bzlib.c"
# End Source File
# Begin Source File

SOURCE=".\3rd-Party\bzip2-1.0.3\bzlib.h"
# End Source File
# Begin Source File

SOURCE=".\3rd-Party\bzip2-1.0.3\compress.c"
# End Source File
# Begin Source File

SOURCE=".\3rd-Party\bzip2-1.0.3\crctable.c"
# End Source File
# Begin Source File

SOURCE=".\3rd-Party\bzip2-1.0.3\decompress.c"
# End Source File
# Begin Source File

SOURCE=".\3rd-Party\bzip2-1.0.3\huffman.c"
# End Source File
# Begin Source File

SOURCE=".\3rd-Party\bzip2-1.0.3\randtable.c"
# End Source File
# End Group
# Begin Source File

SOURCE=.\AutoCU.cpp
# End Source File
# Begin Source File

SOURCE=.\Cache.cpp
# End Source File
# Begin Source File

SOURCE=.\Handlers.cpp
# End Source File
# Begin Source File

SOURCE=.\HTTP.cpp
# End Source File
# Begin Source File

SOURCE=.\Misc.cpp
# End Source File
# Begin Source File

SOURCE=.\Options.cpp
# End Source File
# Begin Source File

SOURCE=.\Pages.cpp
# End Source File
# Begin Source File

SOURCE=.\Procs.cpp
# End Source File
# Begin Source File

SOURCE=.\Session.cpp
# End Source File
# Begin Source File

SOURCE=.\Transfer.cpp
# End Source File
# Begin Source File

SOURCE=.\wbOSD.cpp
# End Source File
# Begin Source File

SOURCE=.\XMLParse.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AggressiveOptimize.h
# End Source File
# Begin Source File

SOURCE=.\AutoCU.h
# End Source File
# Begin Source File

SOURCE=.\Cache.h
# End Source File
# Begin Source File

SOURCE=.\Globals.h
# End Source File
# Begin Source File

SOURCE=.\Handlers.h
# End Source File
# Begin Source File

SOURCE=.\HTTP.h
# End Source File
# Begin Source File

SOURCE=.\Misc.h
# End Source File
# Begin Source File

SOURCE=.\Options.h
# End Source File
# Begin Source File

SOURCE=.\Session.h
# End Source File
# Begin Source File

SOURCE=.\Transfer.h
# End Source File
# Begin Source File

SOURCE=.\wbOSD.h
# End Source File
# Begin Source File

SOURCE=.\XMLParse.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\ascbitma.bmp
# End Source File
# Begin Source File

SOURCE=.\descbitm.bmp
# End Source File
# Begin Source File

SOURCE=.\hand.cur
# End Source File
# Begin Source File

SOURCE=.\ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\lisa.ico
# End Source File
# Begin Source File

SOURCE=.\listchec.ico
# End Source File
# Begin Source File

SOURCE=.\magic.ico
# End Source File
# Begin Source File

SOURCE=.\nightly.ico
# End Source File
# Begin Source File

SOURCE=.\ok.ico
# End Source File
# Begin Source File

SOURCE=.\package.ico
# End Source File
# Begin Source File

SOURCE=.\rabbit.ico
# End Source File
# Begin Source File

SOURCE=.\Resources.rc
# End Source File
# Begin Source File

SOURCE=.\stop.ico
# End Source File
# Begin Source File

SOURCE=.\transferin.ico
# End Source File
# Begin Source File

SOURCE=.\transferout.ico
# End Source File
# Begin Source File

SOURCE=.\ttbbutton.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\copying.txt
# End Source File
# Begin Source File

SOURCE=.\Notes.txt
# End Source File
# End Target
# End Project
