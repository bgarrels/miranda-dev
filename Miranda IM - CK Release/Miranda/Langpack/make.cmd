@echo off
rem This is Miranda IM language pack generator from bunch of text files
rem It can be used (probably) with any localisation but tested only with russian locale.
rem Положить рядом с make.cmd файл VersionInfo.txt - получите лангпак под вашу сборку.
rem ПЕРЕВОДЫ ПЛАГИНОВ ДОЛЖНЫ НАХОДИТЬСЯ В ПАПКЕ \Plugins (не стоит складывать все в папку с мирандой)
setlocal ENABLEDELAYEDEXPANSION
title DE-Langpack Generator v1.2 by CK

rem set variables
set lp=langpack_german.txt
set vi=VersionInfo.txt
set PluginsDir=.\Plugins
set WeatherDir=.\Weather

if not exist %vi% echo %vi% not found, making full langpack...

rem remove old langpack file if exist
del /f /q %lp% 2>nul

@echo Generating langpack...
rem Adding headers
type "=HEAD=.txt" >> %lp%

rem Generate "Plugins-included:"
echo.>>%lp%
if exist %vi% (
rem Generate plugin list from VI
for /f "tokens=1,2 delims=Вэ¤. " %%a in ('more %vi% ^| find "dll v"') do if exist %PluginsDir%\%%a.txt (
		for %%b in (%PluginsDir%\%%a.txt) do set PluginsInc=!PluginsInc!, %%~nb
		) else (
			rem add to pluginlist module names, which names end with "W"
			set nameW=%%a
			set name=!nameW:~0,-1!
			if exist %PluginsDir%\!name!.txt set PluginsInc=!PluginsInc!, !name!
			rem if still module not found, do additional check
			call :check_oldplug
			)
) else (
rem VI does not exist, so generate plugin list based on filenames in plugins dir
for %%a in (%PluginsDir%\*.txt) do set PluginsInc=!PluginsInc!, %%~na
	)
rem now we have plugin list in %PluginsInc%, add it to langpack:
echo Plugins-included: %PluginsInc:~2%.>>%lp%
)
rem add "FLID" for full langpack. Updater support. But if VI exist, so this is vi-based langpack and FLID are useless
if not exist %vi% type "=VERSION=.txt">>%lp%
	
rem  Add MirandaIM Core, dbtool, dupes and My Strings.
call :newline
type "My Strings.txt">>%lp%
call :newline
type "=CORE=.txt">>%lp%
call :newline
type "=dbtool=.txt">>%lp%
call :newline
type "=DUPES=.txt">>%lp%
call :newline

rem Adding module translations one-by-one form VI.
if exist %vi% (
	rem Making langpack with VI-only modules:
	for /f "tokens=1,2 delims=Вдэ¤. " %%a in ('find "dll v" %vi%') do (
		rem if next module are weather.dll, than add weather ini file next to weather module itself
		if /I %%a == weather (
			call :weather
			rem try to add weather ini file translation from .\weather folder
			for /f "tokens=1,2 delims=. " %%c in ('more %vi% ^| find /i ".ini"') do (
				if exist %WeatherDir%\%%c.txt (type %WeatherDir%\%%c.txt>>%lp% && call :newline) else (echo translation for weather forecast from %%c.ini not found!)
				)
		) else (if exist %PluginsDir%\%%a.txt (type %PluginsDir%\%%a.txt >>%lp% && call :newline))
	)	
	call :old_plug
	call :w_plug
rem No VI found, so generate full langpack
) else (
	rem Generate langpack from *.txt
	for %%a in (%PluginsDir%\*.txt) do (
		rem if next module are Weather.txt, than add weather ini file next to weather module itself
		if /I %%a == %PluginsDir%\weather.txt (
			call :weather
			rem adding weather ini file translation from .\weather folder
			for %%c in (%WeatherDir%\*.txt) do (type %%c>>%lp% && call :newline)
		) else (type %%a>>%lp% && call :newline)
	)
	rem add unsorted as last part of langpack
	type "=UNsorted=.txt" >>%lp%
)


rem if parameter "fast" specified for make.cmd, do additional subrutine
if /i "%1"=="fast" (call :restart) else (echo Done!)
rem The End.
goto :EOF

rem Subrutines

rem add new line to langpack
:newline
echo.>>%lp%
echo.>>%lp%
goto :EOF

:restart
rem init variables. DO NOT PUT .dat to profile name!
set MirandaPath=C:\Program Files (x86)\Miranda IM\
set profile=CHEF-KOCH
rem replace langpack in miranda path
copy /y %lp% %MirandaPath%
rem quit miranda using mimcmd command, so mimcmd.exe must exist in plugins dir.
%MirandaPath%\Plugins\MimCmd.exe quit
rem M$ don't have sleep comand, so ping 3 times localhost, giving a time for miranda to quit normally
ping -n 3 -w 1000 localhost >> nul
rem start Miranda with new langpak and specified profile
start %MirandaPath%\miranda32.exe %profile%
exit
goto :EOF

:old_plug
rem support for old plugins, now they have new svc_*.dll names.
find /i "versioninfo.dll" %vi% > nul && (type %PluginsDir%\Svc_VI.txt >>%lp% && call :newline)
find /i "dbeditorpp.dll" %vi% > nul && (type %PluginsDir%\Svc_DBEPP.txt >>%lp% && call :newline)
find /i "Import_sa.dll" %vi% > nul && (type %PluginsDir%\Import.txt >>%lp% && call :newline)
goto :EOF

:w_plug
rem add plugins translations with names ending *W.dll
for /f "tokens=1,2 delims=Вдэ¤. " %%b in ('more %vi% ^| find /i "w.dll"') do (
	if not exist %PluginsDir%\%%b.txt (
		set nameW=%%b
		set name=!nameW:~0,-1!
		type %PluginsDir%\!name!.txt >>%lp% && call :newline
		)
	)
goto :EOF

:weather
rem add weather module
type %PluginsDir%\weather.txt >>%lp%
call :newline
goto :EOF

:check_oldplug
rem if there is no translation, perhaps this is a renamed plugins
if not exist %PluginsDir%\!name!.txt (
	rem if name are not versioninfo, dbeditorpp or import_sa, so translation not found
	if /I !nameW!==versioninfo (set PluginsInc=!PluginsInc!, Svc_VI) else (
		if /I !nameW!==dbeditorpp (set PluginsInc=!PluginsInc!, Svc_DBEPP) else (
			if /I !nameW!==Import_sa (set PluginsInc=!PluginsInc!, Import) else (echo Translation for "!nameW!.dll" not found :()
			)
		)	
	)
goto :EOF