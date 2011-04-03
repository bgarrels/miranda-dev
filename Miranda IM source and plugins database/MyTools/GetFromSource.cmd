@echo off
Title GetFromSource v0.8
rem FileMask: *.h;*.c;*.cpp;*.rc
rem RegExp:
rem \b(LPGENT?|TranslateT?|_T)\("text_here"\)
rem \b(CONTROL|(DEF)?PUSHBUTTON|GROUPBOX|CAPTION|MENUITEM|POPUP|[LRC]TEXT)\b.*"text_here"


rem get translatable strings from source files.
rem WARNING! Due to restrictions of cmd parser, strings with "!" sign cames out with "#1#", replace after parsing complete.
rem ATTENTION! If you get "hang" more than 5 seconds, press Ctrl+C but don't brake operations!
rem (i.e. press "N" for next question)
rem this "hang" coused by findstr with asteriks inside.

set /p source="directory:"
for /f "tokens=*" %%a in ("%source%") do set file=%%~na
set target=%cd%\%file%.tmp
set output=%cd%\%file%.txt
set tempfile=%cd%\%file%.temp

rem delete old file.
del /f /q %output% 2>nul

rem collect all strings with translations to one file - %tempfile%
for /r %source% %%a in (*.h,*.c,*.cpp) do (
	echo %%a
	rem remove "!" sign, it broke parse logic. Totaly 5 "!" will be removed. Hope it's enough
	rem trail "!" does not make any sense
	rem added "eol=/" to remove commeted strings from parsing.
	for /f "tokens=1-5 delims=! eol=/" %%b in ('"findstr /c:"LPGEN(" /c:"LPGENT(" /c:"TranslateT(" /c:"Translate(" /c:"_T(" "%%a""') do (
		echo %%b#1#%%c#1#%%d#1#%%e#1#%%f>>%tempfile%
        	)
	)
echo Start parsing %tempfile%
setlocal enabledelayedexpansion
for /f "tokens=*" %%b in (%tempfile%) do (
		set "str=%%b"
		rem change \' to '
		set str=!str:^\'='!
		rem now change start ( " and end of thanslation - " ) without spases.
		set str=!str:^( "=("!
		set str=!str:" )="^)!
		rem remove empty translations
		set str=!str:""=!
		rem change all service signs to #?#
        	set str=!str:^\"=#0#!
		set str=!str:^&=#2#!
		set str=!str:^>=#3#!
		set str=!str:^<=#4#!
		set str=!str:^|=#5#!
		set str=!str:$=#6#!
		set str=!str:^("=($!
		set str=!str:")=$)!
		for %%c in ("_T(" "LPGEN(" "LPGENT(" "TranslateT(" "Translate(") do (
			set strT=!str!
			echo "!str!" | find %%c
			if !errorlevel!==0 call :parse %%c
			)
		)

rem Sources analyzed, now analyze resources (*.rc)
echo Now parsing *.RC files
for /r %source% %%a in (*.rc) do (
	echo %%a
	for /f "tokens=1-5 delims=!" %%b in ('"findstr /i /r /c:"\^<CONTROL\^>" /c:"\^<PUSHBUTTON\^>" /c:"\^<DEFPUSHBUTTON\^>" /c:"\^<GROUPBOX\^>" /c:"\^<CAPTION\^>" /c:"\^<MENUITEM\^>" /c:"\^<POPUP\^>" /c:"\^<[LRC]TEXT\^>" "%%a""') do (
        	set "str=%%b#1#%%c#1#%%d#1#%%e#1#%%f"
		rem change \" to @@
		set str=!str:^\"=@@!
		rem change "" to @@
		set str=!str:""=@@!
		rem change " to $ - our separator
		set str=!str:"=$!
		rem change \' to '. We don't need slash in translation
		set str=!str:^\'='!
		for /f "tokens=2 delims=, " %%A in ("!str!") do (
			if not "%%A"=="@@" (
				rem rollback @@ to "
				set str=!str:@@="!
				for /f "delims=$ tokens=2" %%e in ("!str!") do echo [%%e]>>%target%
				)
			)	
		)
	)

echo.
echo Strings collected. Sorting and removing dupes...
echo.
del /f /q %tempfile% 2>nul
findstr /l /x /v "[Tree1] [Tree2] [Spin1] [Spin5] [Custom1] [Slider2] [ ] [-] [+] [V] [x] [...] [&¤] [x] [V] [rb] [wb]" %target% >> %tempfile%

set str=
for /f "tokens=*" %%y in ('sort /+1 %tempfile%') do (
	if not "%%y"=="!str!" (
		findstr /l /x /c:"%%y" %output% 2>nul
		if not !ERRORLEVEL!==0 echo %%y>>%output%
		)
	set "str=%%y"
	)
del /f /q %target% %tempfile% 2>nul
echo.
echo Done!
pause
goto :eof
:parse
set strT=!strT:^*%~1=(!
set strT=!strT:^("=($!
set strT=!strT:")=$)!
for /f "tokens=2 delims=$" %%d in ("!strT!") do (
	set out=[%%d]
	set out=!out:#0#="!
	rem set out=!out:#1#==^!!^!
	set out=!out:#2#=^&!
	set out=!out:#3#=^>!
	set out=!out:#4#=^<!
	set out=!out:#5#=^|!
	set out=!out:#6#=$!
	echo !out!>>%target%
	set str=!str:"%%d"=!
	set strT=!strT:$%%d$=!
	)
rem if we have anoter entry with same function, parse it again, if not, search next function
echo "!strT!" | find %1
if !errorlevel!==0 (call :parse %1)
goto :eof