call "C:\Program Files\Microsoft SDKs\Windows\v7.1\Bin\SetEnv.Cmd" /x64 /Release
nmake -f Skype_protocol.mak  CFG="Win64 Release" clean
nmake -f Skype_protocol.mak  CFG="Win64 Release"
nmake -f Skype_protocol.mak  CFG="Win64 UNICODE Release" clean
nmake -f Skype_protocol.mak  CFG="Win64 UNICODE Release"
