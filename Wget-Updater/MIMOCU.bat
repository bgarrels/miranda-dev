@echo off

REM +--------------------------------------------------+
REM +------        MIRANDA-IM MIMOCU.BAT        -------+
REM +--------------------------------------------------+

REM ++++++++++++++++++++++++++++++++++++++++++++++++++++
REM          This 'MIMOCU.bat' will find out 
REM      if the MIM update server is online or not.
REM
REM         If yes, the 'MIM-Update-Checker.bat' 
REM   will be started to see if there's a new version, 
REM  and the 'MIM-Update.bat' will download and install 
REM               the required packages.
REM ++++++++++++++++++++++++++++++++++++++++++++++++++++

set server=update.miranda-im.org

goto :CHECK

REM ++++++++++++++++++++++++++++++++++++++++++++++++++++
REM   If server is offline, update will be cancelled
REM            and this batch will be closed
REM ++++++++++++++++++++++++++++++++++++++++++++++++++++

:END

echo.
echo.
echo.
echo.
echo.
echo                   MIM update server currently not available.
echo.
echo                              Update cancelled!
echo.
echo         More updates will be attempted according task planer or start-up!
echo.
echo.
echo.
echo                   MIM-Update-Server derzeit nicht erreichbar.
echo.
echo                             Abbruch des Updates!
echo.
echo       Weitere Update-Versuche folgen entsprechend Taskplaner oder Autostart!

ping -n 8 127.0.0.1 > nul

cls

goto :CANCEL

:CHECK

REM ++++++++++++++++++++++++++++++++++++++++++++++++++++
REM      Update server status will be checked here
REM ++++++++++++++++++++++++++++++++++++++++++++++++++++

ping -n 1 %server% | find /i "TTL" && (goto :PROCEED) || (goto :END)

REM ping -n 10 127.0.0.1 > nul

cls

REM ++++++++++++++++++++++++++++++++++++++++++++++++++++
REM     If server is online, update will proceed by 
REM          checking the MIM version by means
REM           of the 'MIM Update-Checker.bat'. 
REM 
REM          If the 'Update-Checker.bat' finds 
REM     a new MIM version, update will be performed
REM             using the 'MIM-Updater.bat'.
REM ++++++++++++++++++++++++++++++++++++++++++++++++++++

:PROCEED

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
echo         MIM update server currently online. Update will be proceeded now!
echo.
echo              MIM-Update-Server online. Update wird nun fortgesetzt!

ping -n 6 127.0.0.1 > nul

cls

call ".\MIM Update-Checker.bat"

:CANCEL

cls 