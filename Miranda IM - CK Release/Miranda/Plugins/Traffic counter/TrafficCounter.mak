# Microsoft Developer Studio Generated NMAKE File, Based on TrafficCounter.dsp
!IF "$(CFG)" == ""
CFG=tcounter - Win32 Debug
!MESSAGE No configuration specified. Defaulting to tcounter - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "tcounter - Win32 Release" && "$(CFG)" != "tcounter - Win32 Debug" && "$(CFG)" != "tcounter - Win32 Release Unicode"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "TrafficCounter.mak" CFG="tcounter - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "tcounter - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "tcounter - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "tcounter - Win32 Release Unicode" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "tcounter - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\TrafficCounter.dll" "$(OUTDIR)\TrafficCounter.bsc"


CLEAN :
	-@erase "$(INTDIR)\commonheaders.obj"
	-@erase "$(INTDIR)\commonheaders.sbr"
	-@erase "$(INTDIR)\misc.obj"
	-@erase "$(INTDIR)\misc.sbr"
	-@erase "$(INTDIR)\opttree.obj"
	-@erase "$(INTDIR)\opttree.sbr"
	-@erase "$(INTDIR)\resource.res"
	-@erase "$(INTDIR)\statistics.obj"
	-@erase "$(INTDIR)\statistics.sbr"
	-@erase "$(INTDIR)\TrafficCounter.obj"
	-@erase "$(INTDIR)\TrafficCounter.sbr"
	-@erase "$(INTDIR)\vars.obj"
	-@erase "$(INTDIR)\vars.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\TrafficCounter.bsc"
	-@erase "$(OUTDIR)\TrafficCounter.dll"
	-@erase "$(OUTDIR)\TrafficCounter.exp"
	-@erase "$(OUTDIR)\TrafficCounter.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "..\..\Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "TCOUNTER_EXPORTS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\TrafficCounter.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\resource.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\TrafficCounter.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\commonheaders.sbr" \
	"$(INTDIR)\misc.sbr" \
	"$(INTDIR)\opttree.sbr" \
	"$(INTDIR)\statistics.sbr" \
	"$(INTDIR)\TrafficCounter.sbr" \
	"$(INTDIR)\vars.sbr"

"$(OUTDIR)\TrafficCounter.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib msimg32.lib comctl32.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\TrafficCounter.pdb" /machine:I386 /out:"$(OUTDIR)\TrafficCounter.dll" /implib:"$(OUTDIR)\TrafficCounter.lib" 
LINK32_OBJS= \
	"$(INTDIR)\commonheaders.obj" \
	"$(INTDIR)\misc.obj" \
	"$(INTDIR)\opttree.obj" \
	"$(INTDIR)\statistics.obj" \
	"$(INTDIR)\TrafficCounter.obj" \
	"$(INTDIR)\vars.obj" \
	"$(INTDIR)\resource.res"

"$(OUTDIR)\TrafficCounter.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "tcounter - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\TrafficCounter.dll" "$(OUTDIR)\TrafficCounter.bsc"


CLEAN :
	-@erase "$(INTDIR)\commonheaders.obj"
	-@erase "$(INTDIR)\commonheaders.sbr"
	-@erase "$(INTDIR)\misc.obj"
	-@erase "$(INTDIR)\misc.sbr"
	-@erase "$(INTDIR)\opttree.obj"
	-@erase "$(INTDIR)\opttree.sbr"
	-@erase "$(INTDIR)\resource.res"
	-@erase "$(INTDIR)\statistics.obj"
	-@erase "$(INTDIR)\statistics.sbr"
	-@erase "$(INTDIR)\TrafficCounter.obj"
	-@erase "$(INTDIR)\TrafficCounter.sbr"
	-@erase "$(INTDIR)\vars.obj"
	-@erase "$(INTDIR)\vars.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\TrafficCounter.bsc"
	-@erase "$(OUTDIR)\TrafficCounter.dll"
	-@erase "$(OUTDIR)\TrafficCounter.exp"
	-@erase "$(OUTDIR)\TrafficCounter.ilk"
	-@erase "$(OUTDIR)\TrafficCounter.lib"
	-@erase "$(OUTDIR)\TrafficCounter.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "TCOUNTER_EXPORTS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\TrafficCounter.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\resource.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\TrafficCounter.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\commonheaders.sbr" \
	"$(INTDIR)\misc.sbr" \
	"$(INTDIR)\opttree.sbr" \
	"$(INTDIR)\statistics.sbr" \
	"$(INTDIR)\TrafficCounter.sbr" \
	"$(INTDIR)\vars.sbr"

"$(OUTDIR)\TrafficCounter.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib msimg32.lib comctl32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\TrafficCounter.pdb" /debug /machine:I386 /out:"$(OUTDIR)\TrafficCounter.dll" /implib:"$(OUTDIR)\TrafficCounter.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\commonheaders.obj" \
	"$(INTDIR)\misc.obj" \
	"$(INTDIR)\opttree.obj" \
	"$(INTDIR)\statistics.obj" \
	"$(INTDIR)\TrafficCounter.obj" \
	"$(INTDIR)\vars.obj" \
	"$(INTDIR)\resource.res"

"$(OUTDIR)\TrafficCounter.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "tcounter - Win32 Release Unicode"

OUTDIR=.\ReleaseUnicode
INTDIR=.\ReleaseUnicode\obj
# Begin Custom Macros
OutDir=.\ReleaseUnicode
# End Custom Macros

ALL : "$(OUTDIR)\TrafficCounter.dll"


CLEAN :
	-@erase "$(INTDIR)\commonheaders.obj"
	-@erase "$(INTDIR)\misc.obj"
	-@erase "$(INTDIR)\opttree.obj"
	-@erase "$(INTDIR)\resource.res"
	-@erase "$(INTDIR)\statistics.obj"
	-@erase "$(INTDIR)\TrafficCounter.obj"
	-@erase "$(INTDIR)\vars.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\TrafficCounter.dll"
	-@erase "$(OUTDIR)\TrafficCounter.exp"
	-@erase "$(OUTDIR)\TrafficCounter.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MD /W3 /Zi /O2 /I "../../Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "TCOUNTER_EXPORTS" /D "_UNICODE" /D "UNICODE" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\resource.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\TrafficCounter.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=msimg32.lib comctl32.lib kernel32.lib user32.lib gdi32.lib oleaut32.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\TrafficCounter.pdb" /debug /machine:I386 /out:"$(OUTDIR)\TrafficCounter.dll" /implib:"$(OUTDIR)\TrafficCounter.lib" 
LINK32_OBJS= \
	"$(INTDIR)\commonheaders.obj" \
	"$(INTDIR)\misc.obj" \
	"$(INTDIR)\opttree.obj" \
	"$(INTDIR)\statistics.obj" \
	"$(INTDIR)\TrafficCounter.obj" \
	"$(INTDIR)\vars.obj" \
	"$(INTDIR)\resource.res"

"$(OUTDIR)\TrafficCounter.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("TrafficCounter.dep")
!INCLUDE "TrafficCounter.dep"
!ELSE 
!MESSAGE Warning: cannot find "TrafficCounter.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "tcounter - Win32 Release" || "$(CFG)" == "tcounter - Win32 Debug" || "$(CFG)" == "tcounter - Win32 Release Unicode"
SOURCE=.\commonheaders.c

!IF  "$(CFG)" == "tcounter - Win32 Release"


"$(INTDIR)\commonheaders.obj"	"$(INTDIR)\commonheaders.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "tcounter - Win32 Debug"


"$(INTDIR)\commonheaders.obj"	"$(INTDIR)\commonheaders.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "tcounter - Win32 Release Unicode"


"$(INTDIR)\commonheaders.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\misc.c

!IF  "$(CFG)" == "tcounter - Win32 Release"


"$(INTDIR)\misc.obj"	"$(INTDIR)\misc.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "tcounter - Win32 Debug"


"$(INTDIR)\misc.obj"	"$(INTDIR)\misc.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "tcounter - Win32 Release Unicode"


"$(INTDIR)\misc.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\opttree.c

!IF  "$(CFG)" == "tcounter - Win32 Release"


"$(INTDIR)\opttree.obj"	"$(INTDIR)\opttree.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "tcounter - Win32 Debug"


"$(INTDIR)\opttree.obj"	"$(INTDIR)\opttree.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "tcounter - Win32 Release Unicode"


"$(INTDIR)\opttree.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\statistics.c

!IF  "$(CFG)" == "tcounter - Win32 Release"


"$(INTDIR)\statistics.obj"	"$(INTDIR)\statistics.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "tcounter - Win32 Debug"


"$(INTDIR)\statistics.obj"	"$(INTDIR)\statistics.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "tcounter - Win32 Release Unicode"


"$(INTDIR)\statistics.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\TrafficCounter.c

!IF  "$(CFG)" == "tcounter - Win32 Release"


"$(INTDIR)\TrafficCounter.obj"	"$(INTDIR)\TrafficCounter.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "tcounter - Win32 Debug"


"$(INTDIR)\TrafficCounter.obj"	"$(INTDIR)\TrafficCounter.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "tcounter - Win32 Release Unicode"


"$(INTDIR)\TrafficCounter.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\vars.c

!IF  "$(CFG)" == "tcounter - Win32 Release"


"$(INTDIR)\vars.obj"	"$(INTDIR)\vars.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "tcounter - Win32 Debug"


"$(INTDIR)\vars.obj"	"$(INTDIR)\vars.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "tcounter - Win32 Release Unicode"


"$(INTDIR)\vars.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\resource.rc

"$(INTDIR)\resource.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

