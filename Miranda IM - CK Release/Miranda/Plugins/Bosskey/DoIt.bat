"%ProgramFiles%\Microsoft Visual Studio .NET 2003\Common7\IDE\devenv.exe" /REBUILD "Release" BossKey.sln
"%ProgramFiles%\Microsoft Visual Studio .NET 2003\Common7\IDE\devenv.exe" /REBUILD "Release_Unicode" BossKey.sln
"%ProgramFiles%\Microsoft Visual Studio 9.0\Common7\IDE\devenv.exe" /REBUILD "Release_Unicode|x64" BossKey_9.sln
if not exist plugins md plugins
copy /Y Release\Plugins\BossKey.dll plugins\BossKey.dll
if exist bosskeyplus.zip del bosskeyplus.zip
"%ProgramFiles%\7-Zip\7z.exe" a -mx9 bosskeyplus.zip plugins\BossKey.dll docs\*.txt
copy /Y Release_Unicode\Plugins\BossKey.dll plugins\BossKey.dll
if exist bosskeyplusw.zip del bosskeyplusw.zip
"%ProgramFiles%\7-Zip\7z.exe" a -mx9 bosskeyplusw.zip plugins\BossKey.dll docs\*.txt
copy /Y "Release_Unicode x64\Plugins\BossKey.dll" plugins\BossKey.dll
if exist bosskeyplusxw.zip del bosskeyplusxw.zip
"%ProgramFiles%\7-Zip\7z.exe" a -mx9 bosskeyplusxw.zip plugins\BossKey.dll docs\*.txt
del plugins\BossKey.dll
if exist bosskeyplus_src.zip del bosskeyplus_src.zip
"%ProgramFiles%\7-Zip\7z.exe" a -mx9 bosskeyplus_src.zip *.cpp *.h *.rc *.vcproj *.vcxproj *.vcxproj.filters docs\*.txt sdk\*.h icos\*.ico
