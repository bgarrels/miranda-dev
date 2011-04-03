@echo off
rem This is Miranda IM language pack generator from bunch of text files
rem It can be used (probably) with any localisation but tested only with german locale.

setlocal ENABLEDELAYEDEXPANSION
title Ru-Langpack Generator v0.4 by BasiL

rem set variables
set lp=Langpack_german.txt
set vi=VersionInfo.txt
set PluginsDir=.\Plugins
set WeatherDir=.\Weather
if /i "%1"=="fast" (
	set MirandaPath=d:\BasiL\miranda\
	rem DO NOT PUT .dat to profile name!
	set profile=tst)

if not exist %vi% echo %vi% not found, making full langpack...

rem remove old langpack if exist
del /f /q %lp% 2>nul

@echo Generating langpack...
rem Adding headers
type "=HEAD=.txt" >> %lp%

rem Generate "Plugins-included:"
echo.>>%lp%
if exist %vi% (
rem Generate plugin list from VI
for /f "tokens=1,2 delims=Âý¤. " %%a in ('more %vi% ^| find "dll v"') do if exist %PluginsDir%\%%a.txt (
		for %%b in (%PluginsDir%\%%a.txt) do set PluginsInc=!PluginsInc!, %%~nb
		) else (
		set nameW=%%a
		set name=!nameW:~0,-1!
		if exist %PluginsDir%\!name!.txt set PluginsInc=!PluginsInc!, !name!
		if not exist %PluginsDir%\!name!.txt (
				if /I !nameW!==versioninfo (set PluginsInc=!PluginsInc!, Svc_VI) else (
					if /I !nameW!==dbeditorpp (set PluginsInc=!PluginsInc!, Svc_DBEPP) else (
						if /I !nameW!==Import_sa (set PluginsInc=!PluginsInc!, Import) else (echo Translation for "%%a.dll" not found :()
					)	
				)
			)
		)
			
) else (
rem Generate plugin list based on filenames
for %%a in (%PluginsDir%\*.txt) do set PluginsInc=!PluginsInc!, %%~na
	)
rem now we have plugin list in %PluginsInc%, print it to langpack:
echo Plugins-included: %PluginsInc:~2%.>>%lp%
)
rem add "FLID" for full langpack. Updater support.
if not exist %vi% type "=VERSION=.txt">>%lp%
	
rem  Add MirandaIM Core, dbtool and dupes and My Strings.
echo.>>%lp%&echo.>>%lp%
type "My Strings.txt">>%lp%
echo.>>%lp%&echo.>>%lp%
type "=CORE=.txt">>%lp%
echo.>>%lp%&echo.>>%lp%
type "=dbtool=.txt">>%lp%
echo.>>%lp%&echo.>>%lp%
type "=DUPES=.txt">>%lp%
echo.>>%lp%&echo.>>%lp%

if exist %vi% (
	rem Making langpack with VI-only modules:
	for /f "tokens=1,2 delims=Âäý¤. " %%a in ('find "dll v" %vi%') do if exist %PluginsDir%\%%a.txt (type %PluginsDir%\%%a.txt >>%lp% && echo.>>%lp%&&echo.>>%lp%)
	rem support for old plugins, now they have new svc_*.dll names.
	find /i "versioninfo.dll" %vi% > nul && (type %PluginsDir%\Svc_VI.txt >>%lp% && echo.>>%lp%&&echo.>>%lp%)
	find /i "dbeditorpp.dll" %vi% > nul && (type %PluginsDir%\Svc_DBEPP.txt >>%lp% && echo.>>%lp%&&echo.>>%lp%)
	find /i "Import_sa.dll" %vi% > nul && (type %PluginsDir%\Import.txt >>%lp% && echo.>>%lp%&&echo.>>%lp%)
	rem add plugins translations with names *W.dll
	for /f "tokens=1,2 delims=Âäý¤. " %%b in ('more %vi% ^| find /i "w.dll"') do (
		if not exist %PluginsDir%\%%b.txt (
			set nameW=%%b
			set name=!nameW:~0,-1!
			type %PluginsDir%\!name!.txt >>%lp% && echo.>>%lp%&&echo.>>%lp%
			)
		)
	rem try to add weather ini file translation from ./weather folder
	for /f "tokens=1,2 delims=. " %%c in ('more %vi% ^| find /i ".ini"') do (
		if exist %WeatherDir%\%%c.txt (type %WeatherDir%\%%c.txt>>%lp% && echo.>>%lp%&&echo.>>%lp%
		) else echo translation for weather forecast from %%c.ini not found!
		)
) else (
	rem Generate langpack from *.txt
	for %%a in (%PluginsDir%\*.txt) do (
		type %%a>>%lp%
		echo.>>%lp%
		echo.>>%lp%
		)
	type "=UNsorted=.txt" >>%lp%
)
if /i "%1"=="fast" (
	copy /y %lp% %MirandaPath%
	%MirandaPath%\Plugins\MimCmd.exe quit
	ping -n 3 -w 1000 localhost >> nul
	start %MirandaPath%\miranda32.exe %profile%
	exit ) else (
echo Done!
pause)
exit