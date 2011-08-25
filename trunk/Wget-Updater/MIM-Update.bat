@echo off

REM +--------------------------------------------------+
REM +--------    MIRANDA-IM AUTO-UPDATER    -----------+
REM +--------------------------------------------------+


REM ++++++++++++++++++++++++++++++++++++++++++++++++++++
REM    In case it is running, Miranda will be closed
REM ++++++++++++++++++++++++++++++++++++++++++++++++++++

"C:\Programme\Miranda IM\mimcmd.exe" quit

REM ++++++++++++++++++++++++++++++++++++++++++++++++++++
REM         Download of all required packages
REM ++++++++++++++++++++++++++++++++++++++++++++++++++++

REM ->> If you need both the unicode.7z and the ansi.7z packages to be downloaded, <<-
REM ->>          please remove the REM´s in front of the 2nd group                 <<-

wget -qO - http://update.miranda-im.org/update.xml | xml sel -t -v miranda/releases/releasestable/downloadunicodezip > dl1.txt
set /p file=<dl1.txt
set file2=%file:.7z=-contrib.7z%
echo.%file2% >> dl1.txt

REM wget -qO - http://update.miranda-im.org/update.xml | xml sel -t -v miranda/releases/releasestable/downloadansizip > dl2.txt
REM set /p file=<dl2.txt
REM set file3=%file:.7z=-contrib.7z%
REM echo.%file3% >> dl2.txt

REM ++++++++++++++++++++++++++++++++++++++++++++++++++++
REM            The dl.txt will be created 
REM         according to the wanted packages 
REM ++++++++++++++++++++++++++++++++++++++++++++++++++++

REM ->> If you need both the unicode.7z and the ansi.7z packages to be downloaded, <<-
REM ->>       please remove the REM in front of the next copy command below        <<-
REM ->>           and add it to the front of the next rename command               <<-

REM copy /b dl1.txt + dl2.txt dl.txt
rename dl1.txt dl.txt

if exist dl1.txt del /Q dl1.txt
if exist dl2.txt del /Q dl2.txt

REM ++++++++++++++++++++++++++++++++++++++++++++++++++++
REM      Now, the downloads will be performed here
REM ++++++++++++++++++++++++++++++++++++++++++++++++++++

wget -i dl.txt

del /Q dl.txt

REM ++++++++++++++++++++++++++++++++++++++++++++++++++++
REM             All packages will be copied 
REM            to the default download folder
REM +++++++++++++++++++++++++++++++++++++++++++++++++++++

copy miranda-im*.7z C:\Downloads

cls

echo.
echo.
echo.
echo.
echo.
echo.
echo.
echo.
echo.
echo.
echo        Downloads of all packages are done!

ping -n 4 127.0.0.1 >nul

cls

REM ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
REM   All packages will be extracted into the wanted folders
REM ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

REM ->> Please remove the REM´s for each path you want the packages to be extracted to <<-


REM ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
REM   The packages will now be extracted to thumb drive [O:\]
REM ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

REM C:\Programme\7-Zip\7z x -y miranda-im-*-unicode.7z -aoa -o"O:\Portable Apps\MirandaPortable\App\miranda" *.* -r -x!mirandaboot-example.ini

REM C:\Programme\7-Zip\7z x -y miranda-im-*-ansi.7z -aoa -o"O:\Portable Apps\MirandaPortable\App\miranda_ansi" *.* -r -x!mirandaboot-example.ini

REM C:\Programme\7-Zip\7z x -y miranda-im-*-unicode-contrib.7z -aoa -o"O:\Portable Apps\MirandaPortable\App\miranda" *.* -r -x!mirandaboot-example.ini

REM C:\Programme\7-Zip\7z x -y miranda-im-*-ansi-contrib.7z -aoa -o"O:\Portable Apps\MirandaPortable\App\miranda_ansi" *.* -r -x!mirandaboot-example.ini


REM ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
REM   The packages will now be extracted to thumb drive [P:\]
REM ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

REM C:\Programme\7-Zip\7z x -y miranda-im-*-unicode.7z -aoa -o"P:\- Mob.- PC\Portable Apps\MirandaPortable\App\miranda" *.* -r -x!mirandaboot-example.ini

REM C:\Programme\7-Zip\7z x -y miranda-im-*-ansi.7z -aoa -o"P:\- Mob.- PC\Portable Apps\MirandaPortable\App\miranda_ansi" *.* -r -x!mirandaboot-example.ini

REM C:\Programme\7-Zip\7z x -y miranda-im-*-unicode-contrib.7z -aoa -o"P:\- Mob.- PC\Portable Apps\MirandaPortable\App\miranda" *.* -r -x!mirandaboot-example.ini

REM C:\Programme\7-Zip\7z x -y miranda-im-*-ansi-contrib.7z -aoa -o"P:\- Mob.- PC\Portable Apps\MirandaPortable\App\miranda_ansi" *.* -r -x!mirandaboot-example.ini


REM ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
REM   The packages will now be extracted to the local machine
REM ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

C:\Programme\7-Zip\7z x -y miranda-im-*-unicode.7z -aoa -o"C:\Programme\Miranda IM" *.* -r -x!mirandaboot-example.ini

C:\Programme\7-Zip\7z x -y miranda-im-*-unicode-contrib.7z -aoa -o"C:\Programme\Miranda IM" *.* -r -x!mirandaboot-example.ini

ping -n 4 127.0.0.1 >nul

REM ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
REM             Since they are already copied 
REM             to the default download folder
REM                the downloaded packages 
REM          will be deleted in the current folder
REM ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

del .\miranda*.7z

cls

echo.
echo.
echo.
echo.
echo.
echo.
echo.
echo.
echo.
echo.
echo         All installations are done!

ping -n 8 127.0.0.1 >nul

cls

start "" "C:\Programme\Miranda IM\miranda32.exe" /b

cls