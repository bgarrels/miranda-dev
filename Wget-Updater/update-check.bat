@echo off

REM +--------------------------------------------------+
REM +------      MIRANDA-IM UPDATE-CHECKER       ------+
REM +--------------------------------------------------+

if exist old-MIM.txt goto :GETVERSION

echo.
echo.
echo.
echo.
echo.
echo.
echo.
echo.
echo.
echo		  First run of this Auto-Updater. Run it again, please.
echo.
echo		Erster Start dieses Auto-Updaters. Bitte nochmal starten.
echo.
echo.
echo.
echo.
echo.
echo.
echo.
echo.
echo.

ping -n 6 127.0.0.1 > nul

cls

echo. > .\old-MIM.txt

wget -qO - http://update.miranda-im.org/update.xml | xml sel -t -v miranda/releases/releasestable/downloadunicodezip > current-MIM.txt

del /Q old-MIM.txt

ren current-MIM.txt old-MIM.txt

goto :END

cls

:GETVERSION

wget -qO - http://update.miranda-im.org/update.xml | xml sel -t -v miranda/releases/releasestable/downloadunicodezip > current-MIM.txt

cls 

fc current-MIM.txt old-MIM.txt >nul

if errorlevel 1 goto :OLDVERSION

echo.
echo.
echo.
echo.
echo.
echo.
echo.
echo.
echo.
echo		 No MIM update available. Latest version installed already!
echo.
echo		Kein MIM-Update vorhanden. Installierte Version ist aktuell!
echo.
echo.
echo.
echo.
echo.
echo.
echo.
echo.
echo.

ping -n 6 127.0.0.1 > nul

goto :END

cls

:OLDVERSION

echo.
echo.
echo.
echo.
echo.
echo.
echo.
echo.
echo.
echo		Installed MIM version out-of-date. Starting to update now!
echo.
echo		 Installierte MIM-Version veraltet. Update startet jetzt!
echo.
echo.
echo.
echo.
echo.
echo.
echo.
echo.
echo.

ping -n 6 127.0.0.1 > nul

cls

call ".\MIM-Update.bat" 

del /Q old-MIM.txt

ren current-MIM.txt old-MIM.txt

:END

del /Q current-MIM.txt

cls