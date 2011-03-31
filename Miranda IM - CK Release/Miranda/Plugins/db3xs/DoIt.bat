"%ProgramFiles%\Microsoft Visual Studio .NET 2003\Common7\IDE\devenv.exe" /REBUILD "Release Unicode" db3xS.sln
"%ProgramFiles%\Microsoft Visual Studio .NET 2003\Common7\IDE\devenv.exe" /REBUILD "Release" db3xS.sln
"%ProgramFiles%\Microsoft Visual Studio .NET 2003\Common7\IDE\devenv.exe" /REBUILD "Release Unicode" ..\db3x_mmap_secure_mod\db_autobackups\db_autobackups.sln
"%ProgramFiles%\Microsoft Visual Studio .NET 2003\Common7\IDE\devenv.exe" /REBUILD "Release" ..\db3x_mmap_secure_mod\db_autobackups\db_autobackups.sln
"%ProgramFiles%\Microsoft Visual Studio 10.0\Common7\IDE\devenv.exe" /REBUILD "Release Unicode|x64" db3xS_10.sln
"%ProgramFiles%\Microsoft Visual Studio 10.0\Common7\IDE\devenv.exe" /REBUILD "Release Unicode|x64" ..\db3x_mmap_secure_mod\db_autobackups\db_autobackups_10.sln
if not exist plugins md plugins
copy /Y Release\plugins\dbx_3xS.dll plugins\dbx_3xS.dll
copy /Y ..\db3x_mmap_secure_mod\db_autobackups\Release\plugins\db_autobackups.dll plugins\db_autobackups.dll
if exist dbx_3xs.zip del dbx_3xs.zip
"%ProgramFiles%\7-Zip\7z.exe" a -mx9 dbx_3xs.zip plugins\dbx_3xS.dll plugins\db_autobackups.dll docs\readme.txt docs\translation.txt docs\license.txt
copy /Y "Release Unicode\Plugins\dbx_3xS.dll" plugins\dbx_3xS.dll
copy /Y "..\db3x_mmap_secure_mod\db_autobackups\Release Unicode\plugins\db_autobackups.dll" plugins\db_autobackups.dll
if exist dbx_3xsw.zip del dbx_3xsw.zip
"%ProgramFiles%\7-Zip\7z.exe" a -mx9 dbx_3xsw.zip plugins\dbx_3xS.dll plugins\db_autobackups.dll docs\readme.txt docs\translation.txt docs\license.txt
copy /Y "Release Unicode x64\Plugins\dbx_3xS.dll" plugins\dbx_3xS.dll
copy /Y "..\db3x_mmap_secure_mod\db_autobackups\Release Unicode x64\plugins\db_autobackups.dll" plugins\db_autobackups.dll
if exist dbx_3xsxw.zip del dbx_3xsxw.zip
"%ProgramFiles%\7-Zip\7z.exe" a -mx9 dbx_3xsxw.zip plugins\dbx_3xS.dll plugins\db_autobackups.dll docs\readme.txt docs\translation.txt docs\license.txt
del plugins\dbx_3xS.dll
del plugins\db_autobackups.dll
