@echo off
rem This is [xxxx] sting(s) dupes finder.
rem Used for finding duplicate strings in Mirana IM translation files
title FindDupes v0.5
setlocal ENABLEDELAYEDEXPANSION
set target="=Core=.txt" "=Unsorted=.txt" "=dbtool=.txt" "=Duplikate=.txt" .\plugins\*.txt 
@echo.
@echo Be informed, that very long sting not recognized and will be placed
@echo to %new% even if exist in any of %target%
@echo stings with " and ^! symbols are not recognized.
@echo.
@echo Enter source file name or single string:

set /p source=:
for /f "tokens=*" %%a in ("%source%") do set file=%%~na
set exist=exist_%file%.txt
set dupes=dupes_%file%.txt
set new=newlines_%file%.txt
del /f /q %exist% %dupes% %new% 2>nul
set str=null
if exist "%source%" (
	for /f "eol=; tokens=*" %%a in ('findstr /b [ %source%') do (
		echo %%a
		set str=%%a
		set str=!str:"=\"!
		set str=!str:\\"=\"!
		findstr /l /x /c:"!str!" %target%>>%exist% || echo !str!>>%new%
		)
) else (call :one_string)
echo.
echo DONE!
echo.
pause
goto :eof

:one_string
if not defined source (
	set /p source=:
	call :one_string
	) else (
		set source=!source:[=!
		set source=!source:]=!
		rem echo searching [!source!]:
		findstr /l /x /c:"[!source!]" %target%
		if not !ERRORLEVEL!==0 echo [!source!] not found! & echo.
		rem for /f "tokens=*" %%u in ('findstr /l /x /m /c:"[!source!]" %target%') do (echo [!source!]:%%u>>%exist%)
		set source=
		goto :one_string
		)