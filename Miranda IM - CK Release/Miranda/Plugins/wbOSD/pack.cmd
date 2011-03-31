@echo off
nmake -f wbOSD.mak CFG="wbOSD - Win32 Release"
if errorlevel 1 (
   echo "Make failed"
   goto :eof )
nmake -f wbOSD.mak  CFG="wbOSD - Win32 Release Unicode"
if errorlevel 1 (
   echo "Make failed"
   goto :eof )

del "%temp%\wbOSD.zip" > nul
del "%temp%\wbOSDW.zip" > nul
del "%temp%\wbOSDSrc.zip" > nul
del *.user > nul

for /F "tokens=1-6 delims=, " %%i in (buildnumber.h) do call :Translate %%i %%j %%k %%l %%m %%n

"%PROGRAMFILES%\7-zip\7z.exe" a -tzip -r- -mx=9 "%temp%\wbOSD.zip" ./Release/wbOSD.dll wbOSD-translation.txt
"%PROGRAMFILES%\7-zip\7z.exe" a -tzip -r- -mx=9 "%temp%\wbOSDW.zip" ./Release_Unicode/wbOSD.dll wbOSD-translation.txt

rd /S /Q Release
rd /S /Q Release_Unicode

"%PROGRAMFILES%\7-zip\7z.exe" a -tzip -r0 -mx=9 "%temp%\wbOSDSrc.zip" -xr!.svn -x!*.cmd
goto :eof

:Translate
if %2 == __FILEVERSION_STRING (
   perl lpgen.pl wbOSD version %3 %4 %5 %6 )

goto :eof
