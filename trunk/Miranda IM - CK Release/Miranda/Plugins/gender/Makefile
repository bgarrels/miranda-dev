#################################################################
## This Makefile Exported by MinGW Developer Studio
## Copyright (c) 2002-2004 by Parinya Thipchart
#################################################################

ifneq (,$(findstring Release, $(CFG)))
  override CFG = Release
else
  override CFG = Debug
endif

PROJECT = Gender
CC = "C:\MinGWStudio\MinGW\bin\g++.exe"

WINDRES = "$(COMPILER_BIN)windres.exe"
ifeq ($(CFG),Debug)
  OBJ_DIR = Debug
  OUTPUT_DIR = Debug
  TARGET = Gender.a
  C_INCLUDE_DIRS = -I"G:/Install/Internet/Miranda/SVN/plugins" 
  C_PREPROC = 
  CFLAGS = -pipe  -Wall -g2 -Os -frtti -fexceptions 
  RC_INCLUDE_DIRS = 
  RC_PREPROC = 
  RCFLAGS = 
  LIB_DIRS = 
  LIBS = 
  LDFLAGS = -pipe -shared -Wl,--output-def,"$(OBJ_DIR)\Gender.def",--out-implib,"$(OBJ_DIR)\libGender.dll.a" 
endif

ifeq ($(CFG),Release)
  OBJ_DIR = Release
  OUTPUT_DIR = Release
  TARGET = Gender.dll
  C_INCLUDE_DIRS = -I"G:/Install/Internet/Miranda/SVN/include" 
  C_PREPROC = 
  CFLAGS = -pipe -fweb -frename-registers -Os -Wall -g0 -Os -fno-rtti -fno-exceptions 
  RC_INCLUDE_DIRS = 
  RC_PREPROC = 
  RCFLAGS = 
  LIB_DIRS = 
  LIBS = 
  LDFLAGS = -pipe -shared -Wl,--output-def,"$(OBJ_DIR)\Gender.def",--out-implib,"$(OBJ_DIR)\libGender.dll.a" -s 
endif

ifeq ($(OS),Windows_NT)
  NULL =
else
  NULL = nul
endif

SRC_OBJS = \
  $(OBJ_DIR)/main.o	\
  $(OBJ_DIR)/options.o

RSRC_OBJS = \
  $(OBJ_DIR)/gender.res

define build_target
@echo Linking...
@$(CC) -o "$(OUTPUT_DIR)\$(TARGET)" $(SRC_OBJS) $(RSRC_OBJS) $(LIB_DIRS) $(LIBS) $(LDFLAGS)
endef

define compile_resource
@echo Compiling $<
@$(WINDRES) $(RCFLAGS) $(RC_PREPROC) $(RC_INCLUDE_DIRS) -O COFF -i "$<" -o "$@"
endef

define compile_source
@echo Compiling $<
@$(CC) $(CFLAGS) $(C_PREPROC) $(C_INCLUDE_DIRS) -c "$<" -o "$@"
endef

.PHONY: print_header directories

$(TARGET): print_header directories $(RSRC_OBJS) $(SRC_OBJS)
	$(build_target)

.PHONY: clean cleanall

cleanall:
	@echo Deleting intermediate files for 'Gender - $(CFG)'
	-@del $(OBJ_DIR)\*.o
	-@del $(OBJ_DIR)\*.res
	-@del "$(OUTPUT_DIR)\$(TARGET)"
	-@del "$(OBJ_DIR)\$(PROJECT).def"
	-@del "$(OBJ_DIR)\lib$(PROJECT).dll.a"
	-@rmdir "$(OUTPUT_DIR)"

clean:
	@echo Deleting intermediate files for 'Gender - $(CFG)'
	-@del $(OBJ_DIR)\*.o
	-@del $(OBJ_DIR)\*.res

print_header:
	@echo ----------Configuration: Gender - $(CFG)----------

directories:
	-@if not exist "$(OUTPUT_DIR)\$(NULL)" mkdir "$(OUTPUT_DIR)"
	-@if not exist "$(OBJ_DIR)\$(NULL)" mkdir "$(OBJ_DIR)"

$(OBJ_DIR)/gender.res: gender.rc	\
resource.h	\
icons/male.ico	\
icons/female.ico
	$(compile_resource)

$(OBJ_DIR)/main.o: main.cpp	\
commonheaders.h	\
resource.h	\
sdk/m_genmenu.h	\
sdk/m_cluiframes.h	\
sdk/m_icolib.h	\
sdk/m_updater.h
	$(compile_source)

$(OBJ_DIR)/options.o: options.cpp	\
commonheaders.h	\
resource.h	\
sdk/m_genmenu.h	\
sdk/m_cluiframes.h	\
sdk/m_icolib.h	\
sdk/m_updater.h
	$(compile_source)

