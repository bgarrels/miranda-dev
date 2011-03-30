@echo off
c:\Program Files\batch\wget.exe http://code.google.com/p/miranda/downloads/list -O data.txt
type data.txt|findstr /i unicode.7z 1> data1.txt
del data.txt
ren data1.txt data.txt
type data.txt|findstr /i http://miranda.googlecode.com 1> data1.txt
del data.txt
ren data1.txt data.txt
echo ^> >> data.txt
C:\programy\batch\wget.exe -c -i data.txt --force-html
pause
for /f "tokens=1-3 delims=." %%a in ('dir *unicode.7z /b') do set ver=%%a_%%b_%%c
echo S|"c:\Program Files\7-Zip\7z.exe" x *unicode.7z -o%TEMP%\%ver%
del *unicode.7z
del data.txt
cd ..
echo "Entpacke Miranda IM Unicode und starte es anschliessend."
pause > nul
%windir%\system32\taskkill.exe /IM  miranda32.exe
ping localhost -n 5 > nul
%windir%\system32\taskkill.exe /F /IM miranda32.exe
for /f %%f in ('dir *.* /b/a:-d') do copy %TEMP%\%ver%\%%f %%f
cd Plugins
for /f %%f in ('dir *.* /b/a:-d') do copy %TEMP%\%ver%\Plugins\%%f %%f
cd ..
start miranda32.exe
