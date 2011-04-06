call "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\bin\VCVARS32.BAT"
nmake -f Skype_protocol.mak CFG="Win32 Release" clean
nmake -f Skype_protocol.mak CFG="Win32 Release"
nmake -f Skype_protocol.mak CFG="Win32 UNICODE Release" clean
nmake -f Skype_protocol.mak CFG="Win32 UNICODE Release"
