#Region ;**** Directives created by AutoIt3Wrapper_GUI ****
#AutoIt3Wrapper_icon=Applic-Miranda.ico
#AutoIt3Wrapper_UseUpx=n
#AutoIt3Wrapper_UseX64=n
#EndRegion ;**** Directives created by AutoIt3Wrapper_GUI ****
#include <INet.au3>
#include <String.au3>
 
$src = _INetGetSource("http://www.miranda-im.org/download/")
$newv = _StringBetween($src,"title=""Download v",'" class="active"')
$newv = $newv[0]
$atmv = RegRead("HKEY_LOCAL_MACHINE\SOFTWARE\Wow6432Node\Microsoft\Windows\CurrentVersion\Uninstall\Miranda IM","DisplayName")
$atmv = _StringBetween($atmv & "<","IM ","<")
$atmv = $atmv[0]
 
If $atmv <> $newv Then
        TrayTip("Miranda Auto-Update","Update from " & $atmv & " to " & $newv & " running",2000)
        ProcessClose("miranda32.exe")
        InetGet("http://miranda.googlecode.com/files/miranda-im-v" & $newv & "-unicode.exe",@TempDir & "\update.exe",1)
        ShellExecute(@TempDir & "\update.exe","","","open")
 
        WinWait("Miranda IM " & $newv & " Setup","License Agreement")
        ControlClick("Miranda IM " & $newv & " Setup","","[CLASS:Button; INSTANCE:2]")
 
        WinWait("Miranda IM " & $newv & " Setup","Installation Mode")
        ControlClick("Miranda IM " & $newv & " Setup","","[CLASS:Button; INSTANCE:2]")
 
        WinWait("Miranda IM " & $newv & " Setup","Choose Install Location")
        ControlClick("Miranda IM " & $newv & " Setup","","[CLASS:Button; INSTANCE:2]")
 
        WinWait("Miranda IM " & $newv & " Setup","Choose Components")
        ControlClick("Miranda IM " & $newv & " Setup","","[CLASS:Button; INSTANCE:2]")
 
        WinWait("Miranda IM " & $newv & " Setup"," has been installed on your computer.")
        ControlClick("Miranda IM " & $newv & " Setup","","[CLASS:Button; INSTANCE:2]")
        Exit
EndIf
 
ShellExecute("miranda32.exe")
Exit