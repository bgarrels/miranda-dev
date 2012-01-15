# Microsoft Developer Studio Generated NMAKE File, Based on langman.dsp
!IF "$(CFG)" == ""
CFG=langman - Win32 Release
!MESSAGE Keine Konfiguration angegeben. langman - Win32 Release wird als Standard verwendet.
!ENDIF 

!IF "$(CFG)" != "langman - Win32 Release" && "$(CFG)" != "langman - Win32 Debug" && "$(CFG)" != "langman - Win32 Release Unicode" && "$(CFG)" != "langman - Win32 Debug Unicode"
!MESSAGE UngÅltige Konfiguration "$(CFG)" angegeben.
!MESSAGE Sie kînnen beim AusfÅhren von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "langman.mak" CFG="langman - Win32 Release"
!MESSAGE 
!MESSAGE FÅr die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "langman - Win32 Release" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "langman - Win32 Debug" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "langman - Win32 Release Unicode" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "langman - Win32 Debug Unicode" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR Eine ungÅltige Konfiguration wurde angegeben.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "langman - Win32 Release"

OUTDIR=.\temp/Release/ANSI
INTDIR=.\temp/Release/ANSI
# Begin Custom Macros
OutDir=.\temp/Release/ANSI
# End Custom Macros

ALL : ".\release\ANSI\langman.dll" "$(OUTDIR)\langman.bsc"


CLEAN :
	-@erase "$(INTDIR)\langpack.obj"
	-@erase "$(INTDIR)\langpack.sbr"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\main.sbr"
	-@erase "$(INTDIR)\options.obj"
	-@erase "$(INTDIR)\options.sbr"
	-@erase "$(INTDIR)\resource.res"
	-@erase "$(INTDIR)\unzip.obj"
	-@erase "$(INTDIR)\unzip.sbr"
	-@erase "$(INTDIR)\update.obj"
	-@erase "$(INTDIR)\update.sbr"
	-@erase "$(INTDIR)\utils.obj"
	-@erase "$(INTDIR)\utils.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\langman.bsc"
	-@erase "$(OUTDIR)\langman.exp"
	-@erase ".\release\ANSI\langman.dll"
	-@erase ".\release\ANSI\langman.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W4 /GX /O2 /I ".\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "STRICT" /D "LANGMAN_EXPORTS" /U "NO_STRICT" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /opt:nowin98 /c 

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

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /win32 
RSC=rc.exe
RSC_PROJ=/l 0x809 /fo"$(INTDIR)\resource.res" /i ".\include" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\langman.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\langpack.sbr" \
	"$(INTDIR)\main.sbr" \
	"$(INTDIR)\options.sbr" \
	"$(INTDIR)\unzip.sbr" \
	"$(INTDIR)\update.sbr" \
	"$(INTDIR)\utils.sbr"

"$(OUTDIR)\langman.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib comctl32.lib gdi32.lib shell32.lib /nologo /base:"0x27000000" /dll /incremental:no /pdb:".\release\ANSI\langman.pdb" /debug /machine:I386 /nodefaultlib:"uuid.lib" /nodefaultlib:"OLDNAMES" /def:"langman.def" /out:".\release\ANSI\langman.dll" /implib:"$(OUTDIR)\langman.lib" /opt:nowin98 /ignore:4078 /release 
LINK32_OBJS= \
	"$(INTDIR)\langpack.obj" \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\options.obj" \
	"$(INTDIR)\unzip.obj" \
	"$(INTDIR)\update.obj" \
	"$(INTDIR)\utils.obj" \
	"$(INTDIR)\resource.res"

".\release\ANSI\langman.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "langman - Win32 Debug"

OUTDIR=.\temp/Debug/ANSI
INTDIR=.\temp/Debug/ANSI
# Begin Custom Macros
OutDir=.\temp/Debug/ANSI
# End Custom Macros

ALL : "..\Miranda IM\ANSI\Plugins\langman.dll" "$(OUTDIR)\langman.bsc"


CLEAN :
	-@erase "$(INTDIR)\langpack.obj"
	-@erase "$(INTDIR)\langpack.sbr"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\main.sbr"
	-@erase "$(INTDIR)\options.obj"
	-@erase "$(INTDIR)\options.sbr"
	-@erase "$(INTDIR)\resource.res"
	-@erase "$(INTDIR)\unzip.obj"
	-@erase "$(INTDIR)\unzip.sbr"
	-@erase "$(INTDIR)\update.obj"
	-@erase "$(INTDIR)\update.sbr"
	-@erase "$(INTDIR)\utils.obj"
	-@erase "$(INTDIR)\utils.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\langman.bsc"
	-@erase "$(OUTDIR)\langman.exp"
	-@erase "$(OUTDIR)\langman.map"
	-@erase "..\Miranda IM\ANSI\Plugins\langman.dll"
	-@erase "..\Miranda IM\ANSI\Plugins\langman.ilk"
	-@erase "..\Miranda IM\ANSI\Plugins\langman.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W4 /Gm /Gi /GX /ZI /Od /I ".\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "STRICT" /D "LANGMAN_EXPORTS" /U "NO_STRICT" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x809 /fo"$(INTDIR)\resource.res" /i ".\include" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\langman.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\langpack.sbr" \
	"$(INTDIR)\main.sbr" \
	"$(INTDIR)\options.sbr" \
	"$(INTDIR)\unzip.sbr" \
	"$(INTDIR)\update.sbr" \
	"$(INTDIR)\utils.sbr"

"$(OUTDIR)\langman.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib comctl32.lib gdi32.lib shell32.lib /nologo /base:"0x27000000" /dll /incremental:yes /pdb:"C:\Dokumente und Einstellungen\bib-nutzer\Desktop\Miranda IM\ANSI\Plugins\langman.pdb" /map:"$(INTDIR)\langman.map" /debug /machine:I386 /nodefaultlib:"uuid.lib" /nodefaultlib:"OLDNAMES" /def:"langman.def" /out:"C:\Dokumente und Einstellungen\bib-nutzer\Desktop\Miranda IM\ANSI\Plugins\langman.dll" /implib:"$(OUTDIR)\langman.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\langpack.obj" \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\options.obj" \
	"$(INTDIR)\unzip.obj" \
	"$(INTDIR)\update.obj" \
	"$(INTDIR)\utils.obj" \
	"$(INTDIR)\resource.res"

"..\Miranda IM\ANSI\Plugins\langman.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "langman - Win32 Release Unicode"

OUTDIR=.\temp/Release/Unicode
INTDIR=.\temp/Release/Unicode
# Begin Custom Macros
OutDir=.\temp/Release/Unicode
# End Custom Macros

ALL : ".\release\Unicode\langman.dll" "$(OUTDIR)\langman.bsc"


CLEAN :
	-@erase "$(INTDIR)\langpack.obj"
	-@erase "$(INTDIR)\langpack.sbr"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\main.sbr"
	-@erase "$(INTDIR)\options.obj"
	-@erase "$(INTDIR)\options.sbr"
	-@erase "$(INTDIR)\resource.res"
	-@erase "$(INTDIR)\unzip.obj"
	-@erase "$(INTDIR)\unzip.sbr"
	-@erase "$(INTDIR)\update.obj"
	-@erase "$(INTDIR)\update.sbr"
	-@erase "$(INTDIR)\utils.obj"
	-@erase "$(INTDIR)\utils.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\langman.bsc"
	-@erase "$(OUTDIR)\langman.exp"
	-@erase ".\release\Unicode\langman.dll"
	-@erase ".\release\Unicode\langman.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W4 /GX /O2 /I ".\include" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "STRICT" /D "LANGMAN_EXPORTS" /U "_MBCS" /U "NO_STRICT" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /opt:nowin98 /c 

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

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /win32 
RSC=rc.exe
RSC_PROJ=/l 0x809 /fo"$(INTDIR)\resource.res" /i ".\include" /d "NDEBUG" /d "UNICODE" /d "_UNICODE" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\langman.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\langpack.sbr" \
	"$(INTDIR)\main.sbr" \
	"$(INTDIR)\options.sbr" \
	"$(INTDIR)\unzip.sbr" \
	"$(INTDIR)\update.sbr" \
	"$(INTDIR)\utils.sbr"

"$(OUTDIR)\langman.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib comctl32.lib gdi32.lib shell32.lib /nologo /base:"0x27000000" /dll /incremental:no /pdb:".\release\Unicode\langman.pdb" /debug /machine:I386 /nodefaultlib:"uuid.lib" /nodefaultlib:"OLDNAMES" /def:"langman.def" /out:".\release\Unicode\langman.dll" /implib:"$(OUTDIR)\langman.lib" /opt:nowin98 /ignore:4078 /release 
LINK32_OBJS= \
	"$(INTDIR)\langpack.obj" \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\options.obj" \
	"$(INTDIR)\unzip.obj" \
	"$(INTDIR)\update.obj" \
	"$(INTDIR)\utils.obj" \
	"$(INTDIR)\resource.res"

".\release\Unicode\langman.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "langman - Win32 Debug Unicode"

OUTDIR=.\temp/Debug/Unicode
INTDIR=.\temp/Debug/Unicode
# Begin Custom Macros
OutDir=.\temp/Debug/Unicode
# End Custom Macros

ALL : "..\Miranda IM\Unicode\Plugins\langman.dll" "$(OUTDIR)\langman.bsc"


CLEAN :
	-@erase "$(INTDIR)\langpack.obj"
	-@erase "$(INTDIR)\langpack.sbr"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\main.sbr"
	-@erase "$(INTDIR)\options.obj"
	-@erase "$(INTDIR)\options.sbr"
	-@erase "$(INTDIR)\resource.res"
	-@erase "$(INTDIR)\unzip.obj"
	-@erase "$(INTDIR)\unzip.sbr"
	-@erase "$(INTDIR)\update.obj"
	-@erase "$(INTDIR)\update.sbr"
	-@erase "$(INTDIR)\utils.obj"
	-@erase "$(INTDIR)\utils.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\langman.bsc"
	-@erase "$(OUTDIR)\langman.exp"
	-@erase "$(OUTDIR)\langman.map"
	-@erase "..\Miranda IM\Unicode\Plugins\langman.dll"
	-@erase "..\Miranda IM\Unicode\Plugins\langman.ilk"
	-@erase "..\Miranda IM\Unicode\Plugins\langman.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W4 /Gm /Gi /GX /ZI /Od /I ".\include" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "STRICT" /D "LANGMAN_EXPORTS" /U "_MBCS" /U "NO_STRICT" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x809 /fo"$(INTDIR)\resource.res" /i ".\include" /d "_DEBUG" /d "UNICODE" /d "_UNICODE" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\langman.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\langpack.sbr" \
	"$(INTDIR)\main.sbr" \
	"$(INTDIR)\options.sbr" \
	"$(INTDIR)\unzip.sbr" \
	"$(INTDIR)\update.sbr" \
	"$(INTDIR)\utils.sbr"

"$(OUTDIR)\langman.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib comctl32.lib gdi32.lib shell32.lib /nologo /base:"0x27000000" /dll /incremental:yes /pdb:"C:\Dokumente und Einstellungen\bib-nutzer\Desktop\Miranda IM\Unicode\Plugins\langman.pdb" /map:"$(INTDIR)\langman.map" /debug /machine:I386 /nodefaultlib:"uuid.lib" /nodefaultlib:"OLDNAMES" /def:"langman.def" /out:"C:\Dokumente und Einstellungen\bib-nutzer\Desktop\Miranda IM\Unicode\Plugins\langman.dll" /implib:"$(OUTDIR)\langman.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\langpack.obj" \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\options.obj" \
	"$(INTDIR)\unzip.obj" \
	"$(INTDIR)\update.obj" \
	"$(INTDIR)\utils.obj" \
	"$(INTDIR)\resource.res"

"..\Miranda IM\Unicode\Plugins\langman.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("langman.dep")
!INCLUDE "langman.dep"
!ELSE 
!MESSAGE Warning: cannot find "langman.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "langman - Win32 Release" || "$(CFG)" == "langman - Win32 Debug" || "$(CFG)" == "langman - Win32 Release Unicode" || "$(CFG)" == "langman - Win32 Debug Unicode"
SOURCE=.\langpack.c

"$(INTDIR)\langpack.obj"	"$(INTDIR)\langpack.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\main.c

"$(INTDIR)\main.obj"	"$(INTDIR)\main.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\options.c

"$(INTDIR)\options.obj"	"$(INTDIR)\options.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\unzip.c

"$(INTDIR)\unzip.obj"	"$(INTDIR)\unzip.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\update.c

"$(INTDIR)\update.obj"	"$(INTDIR)\update.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\utils.c

"$(INTDIR)\utils.obj"	"$(INTDIR)\utils.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\resource.rc

"$(INTDIR)\resource.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

