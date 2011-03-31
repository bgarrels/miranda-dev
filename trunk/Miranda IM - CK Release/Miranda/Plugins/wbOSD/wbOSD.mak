# Microsoft Developer Studio Generated NMAKE File, Based on wbOSD.dsp
!IF "$(CFG)" == ""
CFG=wbOSD - Win32 Release Unicode
!MESSAGE No configuration specified. Defaulting to wbOSD - Win32 Release Unicode.
!ENDIF 

!IF "$(CFG)" != "wbOSD - Win32 Release" && "$(CFG)" != "wbOSD - Win32 Debug" && "$(CFG)" != "wbOSD - Win32 Debug Unicode" && "$(CFG)" != "wbOSD - Win32 Release Unicode"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wbOSD.mak" CFG="wbOSD - Win32 Release Unicode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "wbOSD - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wbOSD - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wbOSD - Win32 Debug Unicode" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wbOSD - Win32 Release Unicode" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "wbOSD - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\wbOSD.dll"


CLEAN :
	-@erase "$(INTDIR)\events.obj"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\options.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\wbOSD.obj"
	-@erase "$(INTDIR)\wbOSD.pch"
	-@erase "$(INTDIR)\wbOSD.res"
	-@erase "$(OUTDIR)\wbOSD.dll"
	-@erase "$(OUTDIR)\wbOSD.exp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /Zi /O1 /I "../../include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /Fp"$(INTDIR)\wbOSD.pch" /Yu"wbOSD.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\wbOSD.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\wbOSD.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib wsock32.lib /nologo /base:"0x32240000" /dll /incremental:no /pdb:"$(OUTDIR)\wbOSD.pdb" /machine:I386 /out:"$(OUTDIR)\wbOSD.dll" /implib:"$(OUTDIR)\wbOSD.lib" /filealign:512 
LINK32_OBJS= \
	"$(INTDIR)\events.obj" \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\options.obj" \
	"$(INTDIR)\wbOSD.obj" \
	"$(INTDIR)\wbOSD.res"

"$(OUTDIR)\wbOSD.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "wbOSD - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\wbOSD.dll"


CLEAN :
	-@erase "$(INTDIR)\events.obj"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\options.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\wbOSD.obj"
	-@erase "$(INTDIR)\wbOSD.pch"
	-@erase "$(INTDIR)\wbOSD.res"
	-@erase "$(OUTDIR)\wbOSD.dll"
	-@erase "$(OUTDIR)\wbOSD.exp"
	-@erase "$(OUTDIR)\wbOSD.ilk"
	-@erase "$(OUTDIR)\wbOSD.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /Fp"$(INTDIR)\wbOSD.pch" /Yu"wbOSD.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x40d /fo"$(INTDIR)\wbOSD.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\wbOSD.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\wbOSD.pdb" /debug /machine:I386 /out:"$(OUTDIR)\wbOSD.dll" /implib:"$(OUTDIR)\wbOSD.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\events.obj" \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\options.obj" \
	"$(INTDIR)\wbOSD.obj" \
	"$(INTDIR)\wbOSD.res"

"$(OUTDIR)\wbOSD.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "wbOSD - Win32 Debug Unicode"

OUTDIR=.\Debug_Unicode
INTDIR=.\Debug_Unicode
# Begin Custom Macros
OutDir=.\Debug_Unicode
# End Custom Macros

ALL : "$(OUTDIR)\wbOSD.dll"


CLEAN :
	-@erase "$(INTDIR)\events.obj"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\options.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\wbOSD.obj"
	-@erase "$(INTDIR)\wbOSD.pch"
	-@erase "$(INTDIR)\wbOSD.res"
	-@erase "$(OUTDIR)\wbOSD.dll"
	-@erase "$(OUTDIR)\wbOSD.exp"
	-@erase "$(OUTDIR)\wbOSD.ilk"
	-@erase "$(OUTDIR)\wbOSD.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /D "_USRDLL" /Fp"$(INTDIR)\wbOSD.pch" /Yu"wbOSD.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x40d /fo"$(INTDIR)\wbOSD.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\wbOSD.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib wsock.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\wbOSD.pdb" /debug /machine:I386 /out:"$(OUTDIR)\wbOSD.dll" /implib:"$(OUTDIR)\wbOSD.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\events.obj" \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\options.obj" \
	"$(INTDIR)\wbOSD.obj" \
	"$(INTDIR)\wbOSD.res"

"$(OUTDIR)\wbOSD.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "wbOSD - Win32 Release Unicode"

OUTDIR=.\Release_Unicode
INTDIR=.\Release_Unicode
# Begin Custom Macros
OutDir=.\Release_Unicode
# End Custom Macros

ALL : "$(OUTDIR)\wbOSD.dll"


CLEAN :
	-@erase "$(INTDIR)\events.obj"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\options.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\wbOSD.obj"
	-@erase "$(INTDIR)\wbOSD.pch"
	-@erase "$(INTDIR)\wbOSD.res"
	-@erase "$(OUTDIR)\wbOSD.dll"
	-@erase "$(OUTDIR)\wbOSD.exp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /Zi /O1 /I "../../include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /D "_USRDLL" /D "wbOSD_EXPORTS" /Fp"$(INTDIR)\wbOSD.pch" /Yu"wbOSD.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\wbOSD.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\wbOSD.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib wsock32.lib /nologo /base:"0x32240000" /dll /incremental:no /pdb:"$(OUTDIR)\wbOSD.pdb" /machine:I386 /out:"$(OUTDIR)\wbOSD.dll" /implib:"$(OUTDIR)\wbOSD.lib" /filealign:512 
LINK32_OBJS= \
	"$(INTDIR)\events.obj" \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\options.obj" \
	"$(INTDIR)\wbOSD.obj" \
	"$(INTDIR)\wbOSD.res"

"$(OUTDIR)\wbOSD.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("wbOSD.dep")
!INCLUDE "wbOSD.dep"
!ELSE 
!MESSAGE Warning: cannot find "wbOSD.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "wbOSD - Win32 Release" || "$(CFG)" == "wbOSD - Win32 Debug" || "$(CFG)" == "wbOSD - Win32 Debug Unicode" || "$(CFG)" == "wbOSD - Win32 Release Unicode"
SOURCE=.\events.cpp

"$(INTDIR)\events.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\wbOSD.pch"


SOURCE=.\main.cpp

!IF  "$(CFG)" == "wbOSD - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /Zi /O1 /I "../../include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /Fp"$(INTDIR)\wbOSD.pch" /Yc"wbOSD.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\main.obj"	"$(INTDIR)\wbOSD.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "wbOSD - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /Fp"$(INTDIR)\wbOSD.pch" /Yc"wbOSD.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\main.obj"	"$(INTDIR)\wbOSD.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "wbOSD - Win32 Debug Unicode"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /D "_USRDLL" /Fp"$(INTDIR)\wbOSD.pch" /Yc"wbOSD.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\main.obj"	"$(INTDIR)\wbOSD.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "wbOSD - Win32 Release Unicode"

CPP_SWITCHES=/nologo /MD /W3 /GX /Zi /O1 /I "../../include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /D "_USRDLL" /D "wbOSD_EXPORTS" /Fp"$(INTDIR)\wbOSD.pch" /Yc"wbOSD.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\main.obj"	"$(INTDIR)\wbOSD.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\options.cpp

"$(INTDIR)\options.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\wbOSD.pch"


SOURCE=.\wbOSD.cpp

"$(INTDIR)\wbOSD.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\wbOSD.pch"


SOURCE=.\wbOSD.rc

"$(INTDIR)\wbOSD.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

