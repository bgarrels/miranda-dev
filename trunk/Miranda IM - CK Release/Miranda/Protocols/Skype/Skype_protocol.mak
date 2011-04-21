!IF "$(CFG)" == ""
CFG=Win32 Debug
!MESSAGE Keine Konfiguration angegeben. Win32 Debug wird als Standard verwendet.
!ENDIF 

!IF "$(CFG)" != "Win32 Release" && "$(CFG)" != "Win32 Debug" && "$(CFG)" != "Win64 Release" && "$(CFG)" != "Win64 Debug" && "$(CFG)" != "Win32 UNICODE Release" && "$(CFG)" != "Win32 UNICODE Debug" && "$(CFG)" != "Win64 UNICODE Release" && "$(CFG)" != "Win64 UNICODE Debug"
!MESSAGE Ung�ltige Konfiguration "$(CFG)" angegeben.
!MESSAGE Sie k�nnen beim Ausf�hren von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "Skype_protocol.mak" CFG="Win32 Debug"
!MESSAGE 
!MESSAGE F�r die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "Win32 Release"
!MESSAGE "Win32 Debug"
!MESSAGE "Win64 Release"
!MESSAGE "Win64 Debug"
!MESSAGE "Win32 UNICODE Release"
!MESSAGE "Win32 UNICODE Debug"
!MESSAGE "Win64 UNICODE Release"
!MESSAGE "Win64 UNICODE Debug"

!MESSAGE 
!ERROR Eine ung�ltige Konfiguration wurde angegeben.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe
LINK32=link.exe

LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib comctl32.lib Rpcrt4.lib shlwapi.lib /nologo /dll /pdb:"$(INTDIR)\skype.pdb" /out:"$(OUTDIR)/skype.dll" /implib:"$(INTDIR)\skype.lib" 

!IF  "$(CFG)" == "Win64 Release"
OUTDIR=.\Release64
INTDIR=.\Release64
CPP_PROJ=/nologo /MD /W3 /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SKYPE_PROTOCOL_EXPORTS" /Fp"$(INTDIR)\Skype_protocol.pch" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
LINK32_FLAGS=$(LINK32_FLAGS) /incremental:no 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0xc07 /fo"$(INTDIR)\Skript1.res" /d "NDEBUG" 
!ELSEIF  "$(CFG)" == "Win32 Release"
OUTDIR=.\Release
INTDIR=.\Release
CPP_PROJ=/nologo /MD /W3 /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SKYPE_PROTOCOL_EXPORTS" /Fp"$(INTDIR)\Skype_protocol.pch" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
LINK32_FLAGS=$(LINK32_FLAGS) /incremental:no 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0xc07 /fo"$(INTDIR)\Skript1.res" /d "NDEBUG" 
!ELSEIF  "$(CFG)" == "Win64 Debug"
OUTDIR=Debug64
INTDIR=$(OUTDIR)
CPP_PROJ=/nologo /MDd /W3 /Gm /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SKYPE_PROTOCOL_EXPORTS" /Fp"$(INTDIR)\Skype_protocol.pch" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
LINK32_FLAGS=$(LINK32_FLAGS) /incremental:yes /debug /pdbtype:sept 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0xc07 /fo"$(INTDIR)\Skript1.res" /d "_DEBUG" 
!ELSEIF  "$(CFG)" == "Win32 Debug"
OUTDIR=.\Debug
INTDIR=$(OUTDIR)
CPP_PROJ=/nologo /MDd /W3 /Gm /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SKYPE_PROTOCOL_EXPORTS" /Fp"$(INTDIR)\Skype_protocol.pch" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
LINK32_FLAGS=$(LINK32_FLAGS) /incremental:yes /debug /pdbtype:sept 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0xc07 /fo"$(INTDIR)\Skript1.res" /d "_DEBUG" 
!ELSEIF  "$(CFG)" == "Win64 UNICODE Release"
OUTDIR=.\Release64-UNICODE
INTDIR=.\Release64-UNICODE
CPP_PROJ=/nologo /MD /W3 /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SKYPE_PROTOCOL_EXPORTS" /D "UNICODE" /D "_UNICODE" /Fp"$(INTDIR)\Skype_protocol.pch" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
LINK32_FLAGS=$(LINK32_FLAGS) /incremental:no 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0xc07 /fo"$(INTDIR)\Skript1.res" /d "NDEBUG" 
!ELSEIF  "$(CFG)" == "Win32 UNICODE Release"
OUTDIR=.\Release-UNICODE
INTDIR=.\Release-UNICODE
CPP_PROJ=/nologo /MD /W3 /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SKYPE_PROTOCOL_EXPORTS" /D "UNICODE" /D "_UNICODE"  /Fp"$(INTDIR)\Skype_protocol.pch" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
LINK32_FLAGS=$(LINK32_FLAGS) /incremental:no 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0xc07 /fo"$(INTDIR)\Skript1.res" /d "NDEBUG" 
!ELSEIF  "$(CFG)" == "Win64 UNICODE Debug"
OUTDIR=Debug64-UNICODE
INTDIR=$(OUTDIR)
CPP_PROJ=/nologo /MDd /W3 /Gm /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SKYPE_PROTOCOL_EXPORTS" /D "UNICODE" /D "_UNICODE"  /Fp"$(INTDIR)\Skype_protocol.pch" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c
LINK32_FLAGS= $(LINK32_FLAGS) /incremental:yes /debug /pdbtype:sept 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0xc07 /fo"$(INTDIR)\Skript1.res" /d "_DEBUG" 
!ELSEIF  "$(CFG)" == "Win32 UNICODE Debug"
OUTDIR=.\Debug-UNICODE
INTDIR=$(OUTDIR)
CPP_PROJ=/nologo /MDd /W3 /Gm /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SKYPE_PROTOCOL_EXPORTS" /D "UNICODE" /D "_UNICODE"  /Fp"$(INTDIR)\Skype_protocol.pch" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
LINK32_FLAGS=$(LINK32_FLAGS) /incremental:yes /debug /pdbtype:sept 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0xc07 /fo"$(INTDIR)\Skript1.res" /d "_DEBUG" 
!ENDIF

LINK32_OBJS= \
	"$(INTDIR)\contacts.obj" \
	"$(INTDIR)\debug.obj" \
	"$(INTDIR)\gchat.obj" \
	"$(INTDIR)\pthread.obj" \
	"$(INTDIR)\skype.obj" \
	"$(INTDIR)\skypeapi.obj" \
	"$(INTDIR)\skypeopt.obj" \
	"$(INTDIR)\skypeprofile.obj" \
	"$(INTDIR)\skypesvc.obj" \
	"$(INTDIR)\utf8.obj" \
	"$(INTDIR)\voiceservice.obj" \
	"$(INTDIR)\msglist.obj" \
	"$(INTDIR)\memlist.obj" \
	"$(INTDIR)\util.obj" \
	"$(INTDIR)\Skript1.res"

ALL : "$(OUTDIR)\skype.dll"


CLEAN :
	-@erase "$(INTDIR)\contacts.obj"
	-@erase "$(INTDIR)\debug.obj"
	-@erase "$(INTDIR)\gchat.obj"
	-@erase "$(INTDIR)\pthread.obj"
	-@erase "$(INTDIR)\Skript1.res"
	-@erase "$(INTDIR)\skype.obj"
	-@erase "$(INTDIR)\skypeapi.obj"
	-@erase "$(INTDIR)\skypeopt.obj"
	-@erase "$(INTDIR)\skypeprofile.obj"
	-@erase "$(INTDIR)\skypesvc.obj"
	-@erase "$(INTDIR)\msglist.obj"
	-@erase "$(INTDIR)\memlist.obj"
	-@erase "$(INTDIR)\util.obj"
	-@erase "$(INTDIR)\utf8.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\voiceservice.obj"
	-@erase "$(INTDIR)\skype.exp"
	-@erase "$(INTDIR)\skype.pdb"
	-@erase "$(INTDIR)\skype.lib"
	-@erase "$(OUTDIR)\skype.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


"$(OUTDIR)\skype.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("Skype_protocol.dep")
!INCLUDE "Skype_protocol.dep"
!ELSE 
!MESSAGE Warning: cannot find "Skype_protocol.dep"
!ENDIF 
!ENDIF 


SOURCE=.\contacts.c

"$(INTDIR)\contacts.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\debug.c

"$(INTDIR)\debug.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\gchat.c

"$(INTDIR)\gchat.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\pthread.c

"$(INTDIR)\pthread.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\skype.c

"$(INTDIR)\skype.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\skypeapi.c

"$(INTDIR)\skypeapi.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\skypeopt.c

"$(INTDIR)\skypeopt.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\skypeprofile.c

"$(INTDIR)\skypeprofile.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\skypesvc.c

"$(INTDIR)\skypesvc.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\utf8.c

"$(INTDIR)\utf8.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\voiceservice.c

"$(INTDIR)\voiceservice.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Skript1.rc

"$(INTDIR)\Skript1.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)

