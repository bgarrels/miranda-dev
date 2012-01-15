# Microsoft Developer Studio Project File - Name="langman" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=langman - Win32 Release
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "langman.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "langman.mak" CFG="langman - Win32 Release"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "langman - Win32 Release" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "langman - Win32 Debug" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "langman - Win32 Release Unicode" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "langman - Win32 Debug Unicode" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "langman - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "release/ANSI"
# PROP BASE Intermediate_Dir "temp/Release/ANSI"
# PROP BASE Target_Dir "release/ANSI"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "temp/Release/ANSI"
# PROP Intermediate_Dir "temp/Release/ANSI"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir "release/ANSI"
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LANGMAN_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W4 /GX /O2 /I ".\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "STRICT" /D "LANGMAN_EXPORTS" /U "NO_STRICT" /FR /FD /opt:nowin98 /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /win32
# SUBTRACT MTL /mktyplib203 /Oicf
# ADD BASE RSC /l 0x1009 /d "NDEBUG"
# ADD RSC /l 0x809 /i ".\include" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib comctl32.lib gdi32.lib shell32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib comctl32.lib gdi32.lib shell32.lib /nologo /base:"0x24100000" /dll /pdb:".\release\ANSI\langman.pdb" /map:".\release\ANSI\langman.map" /debug /machine:I386 /nodefaultlib:"uuid.lib" /nodefaultlib:"OLDNAMES" /def:"langman.def" /out:".\release\ANSI\langman.dll" /mapinfo:lines /opt:nowin98 /ignore:4078 /release
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "langman - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "temp/Debug/ANSI"
# PROP BASE Intermediate_Dir "temp/Debug/ANSI"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "temp/Debug/ANSI"
# PROP Intermediate_Dir "temp/Debug/ANSI"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LANGMAN_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W4 /Gm /Gi /GX /ZI /Od /I ".\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "STRICT" /D "LANGMAN_EXPORTS" /U "NO_STRICT" /FR /FD /GZ /c
# SUBTRACT CPP /WX /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# SUBTRACT MTL /Oicf
# ADD BASE RSC /l 0x1009 /d "_DEBUG"
# ADD RSC /l 0x809 /i ".\include" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib comctl32.lib gdi32.lib shell32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib comctl32.lib gdi32.lib shell32.lib /nologo /base:"0x24100000" /dll /pdb:"D:\Miranda IM\ANSI\Plugins\langman.pdb" /map /debug /machine:I386 /nodefaultlib:"uuid.lib" /nodefaultlib:"OLDNAMES" /def:"langman.def" /out:"D:\Miranda IM\ANSI\Plugins\langman.dll" /pdbtype:sept /mapinfo:lines
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "langman - Win32 Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "release/Unicode"
# PROP BASE Intermediate_Dir "temp/Release/Unicode"
# PROP BASE Target_Dir "release/Unicode"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "temp/Release/Unicode"
# PROP Intermediate_Dir "temp/Release/Unicode"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir "release/Unicode"
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LANGMAN_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W4 /GX /O2 /I ".\include" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "STRICT" /D "LANGMAN_EXPORTS" /U "_MBCS" /U "NO_STRICT" /FR /FD /opt:nowin98 /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /win32
# SUBTRACT MTL /mktyplib203 /Oicf
# ADD BASE RSC /l 0x1009 /d "NDEBUG" /d "UNICODE" /d "_UNICODE"
# ADD RSC /l 0x809 /i ".\include" /d "NDEBUG" /d "UNICODE" /d "_UNICODE"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib comctl32.lib gdi32.lib shell32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib comctl32.lib gdi32.lib shell32.lib /nologo /base:"0x24100000" /dll /pdb:".\release\Unicode\langman.pdb" /map:".\release\Unicode\langman.map" /debug /machine:I386 /nodefaultlib:"uuid.lib" /nodefaultlib:"OLDNAMES" /def:"langman.def" /out:".\release\Unicode\langman.dll" /mapinfo:lines /opt:nowin98 /ignore:4078 /release
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "langman - Win32 Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "temp/Debug/Unicode"
# PROP BASE Intermediate_Dir "temp/Debug/Unicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "temp/Debug/Unicode"
# PROP Intermediate_Dir "temp/Debug/Unicode"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LANGMAN_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W4 /Gm /Gi /GX /ZI /Od /I ".\include" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "STRICT" /D "LANGMAN_EXPORTS" /U "_MBCS" /U "NO_STRICT" /FR /FD /GZ /c
# SUBTRACT CPP /WX /YX
# ADD BASE MTL /nologo /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /mktyplib203 /win32
# SUBTRACT MTL /Oicf
# ADD BASE RSC /l 0x1009 /d "_DEBUG" /d "UNICODE" /d "_UNICODE"
# ADD RSC /l 0x809 /i ".\include" /d "_DEBUG" /d "UNICODE" /d "_UNICODE"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib comctl32.lib gdi32.lib shell32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib comctl32.lib gdi32.lib shell32.lib /nologo /base:"0x24100000" /dll /pdb:"D:\Miranda IM\Unicode\Plugins\langman.pdb" /map /debug /machine:I386 /nodefaultlib:"uuid.lib" /nodefaultlib:"OLDNAMES" /def:"langman.def" /out:"D:\Miranda IM\Unicode\Plugins\langman.dll" /pdbtype:sept /mapinfo:lines
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "langman - Win32 Release"
# Name "langman - Win32 Debug"
# Name "langman - Win32 Release Unicode"
# Name "langman - Win32 Debug Unicode"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\langpack.c
# End Source File
# Begin Source File

SOURCE=.\main.c
# End Source File
# Begin Source File

SOURCE=.\options.c
# End Source File
# Begin Source File

SOURCE=.\unzip.c
# End Source File
# Begin Source File

SOURCE=.\update.c
# End Source File
# Begin Source File

SOURCE=.\utils.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\common.h
# End Source File
# Begin Source File

SOURCE=.\langpack.h
# End Source File
# Begin Source File

SOURCE=.\m_langman.h
# End Source File
# Begin Source File

SOURCE=.\options.h
# End Source File
# Begin Source File

SOURCE=.\unzip.h
# End Source File
# Begin Source File

SOURCE=.\update.h
# End Source File
# Begin Source File

SOURCE=.\utils.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\resource.rc
# End Source File
# Begin Source File

SOURCE=.\version.h
# End Source File
# End Group
# Begin Group "Documentation"

# PROP Default_Filter "*.txt"
# Begin Source File

SOURCE=".\docs\LangMan-Developer.txt"
# End Source File
# Begin Source File

SOURCE=".\docs\LangMan-License.txt"
# End Source File
# Begin Source File

SOURCE=".\docs\LangMan-Readme.txt"
# End Source File
# Begin Source File

SOURCE=".\docs\LangMan-Translation.txt"
# End Source File
# Begin Source File

SOURCE=".\m_langman.inc"
# End Source File
# End Group
# Begin Group "SDK"

# PROP Default_Filter "c;h"
# Begin Source File

SOURCE=.\include\m_clist.h
# End Source File
# Begin Source File

SOURCE=.\include\m_database.h
# End Source File
# Begin Source File

SOURCE=.\include\m_flags.h
# End Source File
# Begin Source File

SOURCE=.\include\m_help.h
# End Source File
# Begin Source File

SOURCE=.\include\m_icolib.h
# End Source File
# Begin Source File

SOURCE=.\include\m_langpack.h
# End Source File
# Begin Source File

SOURCE=.\include\m_netlib.h
# End Source File
# Begin Source File

SOURCE=.\include\m_options.h
# End Source File
# Begin Source File

SOURCE=.\include\m_plugins.h
# End Source File
# Begin Source File

SOURCE=.\include\m_skin.h
# End Source File
# Begin Source File

SOURCE=.\include\m_system.h
# End Source File
# Begin Source File

SOURCE=.\include\m_updater.h
# End Source File
# Begin Source File

SOURCE=.\include\m_utils.h
# End Source File
# Begin Source File

SOURCE=.\include\newpluginapi.h
# End Source File
# Begin Source File

SOURCE=.\include\statusmodes.h
# End Source File
# Begin Source File

SOURCE=.\include\win2k.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Info_Src.txt
# End Source File
# Begin Source File

SOURCE=.\License_Appendix.txt
# End Source File
# End Target
# End Project
