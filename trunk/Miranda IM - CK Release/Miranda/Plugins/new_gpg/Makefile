CFLAGS=-g  -mdll -mwindows -I../miranda-im/miranda/include/ -I./ -D DEBUG -D _UNICODE -D UNICODE
CXXFLAGS=${CFLAGS}
LDFLAGS=-static-libgcc -Wl,-O1 -shared
CPPFLAGS =
CC=i686-pc-mingw32-gcc
CXX=i686-pc-mingw32-g++
STRIP=i686-pc-mingw32-strip
LD=i686-pc-mingw32-ld
LNK_COMMON=-lkernel32 -lgpgme -lgcrypt -lgpg-error -lksba -lassuan
MAINOBJS=init.o utilities.o main.o options.o
WINDRES=i686-pc-mingw32-windres

all: main
main: $(MAINOBJS)
	$(WINDRES) -i new_gpg.rc -o resources.o
	$(CXX) $(MAINOBJS) resources.o $(LNK_COMMON) $(LDFLAGS) -o new_gpg.dll
	#$(STRIP) new_gpg.dll
	#upx -9 new_gpg.dll
clean:
	rm *.o
	rm new_gpg.dll
