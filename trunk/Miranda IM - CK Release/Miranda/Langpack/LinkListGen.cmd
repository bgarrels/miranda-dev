@echo off
title LinkList generator v0.1
rem this file will generate file LinkList.txt
rem LinkList.txt can be imported to any downloader program and download all langpack files
rem lagpack files can be used to generate VersionInfo-based langpack with reduced size.
set out=LinkList.txt
set URL=http://svn.miranda.im/langpack
echo Please wait

rem delete old file
del /f /q %out% 2>nul

rem adding links to all =*=.txt files
for /r %%a in ("=*=.txt") do echo %URL%/%%~nxa>> %out%

rem adding link to make.cmd
echo %URL%/make.cmd>> %out%

rem adding all plugins
for /r ./Plugins %%a in (*.txt) do echo %URL%/Plugins/%%~nxa>> %out%
for /r ./Weather %%b in (*.txt) do echo %URL%/Weather/%%~nxb>> %out%