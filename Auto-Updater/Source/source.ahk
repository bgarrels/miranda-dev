#NoTrayIcon
?2.?
#Region ;**** Directives created by AutoIt3Wrapper_GUI ****
?3.?
#AutoIt3Wrapper_icon=Applic-Miranda.ico
?4.?
#AutoIt3Wrapper_UseUpx=n
?5.?
#AutoIt3Wrapper_UseX64=n
?6.?
#EndRegion ;**** Directives created by AutoIt3Wrapper_GUI ****
?7.?
#include <INet.au3>
?8.?
#include <String.au3>
?9.?
#include <Process.au3>
?10.?
 
?11.?
If Not FileExists(@ScriptDir & "\miranda32.exe") Then
?12.?
        MsgBox(16,"Error","Du hast den Updater falsch installiert" & @crlf & "Verschiebe diese Datei in dasselbe Verzeichnis wie die miranda32.exe")
?13.?
        Exit
?14.?
EndIf
?15.?
 
?16.?
If Not FileExists(@ScriptDir & "\7za.exe") Then
?17.?
        MsgBox(16,"Error","Es fehlt die 7za.exe")
?18.?
        Exit
?19.?
EndIf
?20.?
 
?21.?
$src = _INetGetSource("http://update.miranda-im.org/update.xml")
?22.?
$atmv = FileGetVersion(@ScriptDir & "\miranda32.exe","ProductVersion")
?23.?
If StringInStr($atmv,"alpha") Then
?24.?
        $newv1 = _StringBetween($src,"<releasealpha>",'</releasealpha>')
?25.?
        $newv = _StringBetween($newv1[0],"<versionpublic>",'</versionpublic>')
?26.?
ElseIf StringInStr($atmv,"beta") Then
?27.?
        $newv1 = _StringBetween($src,"<releasebeta>",'</releasebeta>')
?28.?
        $newv = _StringBetween($newv1[0],"<versionpublic>",'</versionpublic>')
?29.?
Else
?30.?
        $newv1 = _StringBetween($src,"<releasestable>",'</releasestable>')
?31.?
        $newv = _StringBetween($newv1[0],"<versionpublic>",'</versionpublic>')
?32.?
EndIf
?33.?
$newv = $newv[0]
?34.?
$dlurl = _StringBetween($newv1[0],"<downloadunicodezip>","</downloadunicodezip")
?35.?
 
?36.?
If $atmv <> $newv Then
?37.?
        ProgressOn("","Update von " & $atmv & " auf " & $newv,"Vorgang wird initiiert",-1,-1,19)
?38.?
        ProcessClose("miranda32.exe")
?39.?
        DirCreate(@TempDir & "\mirupdate")
?40.?
        $in = InetGet($dlurl[0],@TempDir & "\mirupdate\packed.7z",1,1)
?41.?
        ProgressSet(0,"Download")
?42.?
        Do
?43.?
                $dlinf = InetGetInfo($in,-1)
?44.?
                ProgressSet(($dlinf[0]/$dlinf[1])*100)
?45.?
                Sleep(50)
?46.?
        Until $dlinf[2]
?47.?
        InetClose($in)
?48.?
        ProgressOff()
?49.?
 
?50.?
        If Not FileExists(@TempDir & "\mirupdate\packed.7z") Or Not $dlinf[3] Then
?51.?
                MsgBox(16,"Error","Update konnte nicht heruntergeladen werden" & @crlf & "Downloadpfad: " & $dlurl[0] & @crlf & @error)
?52.?
                ShellExecute(@ScriptDir & "\miranda32.exe")
?53.?
                Exit
?54.?
        EndIf
?55.?
 
?56.?
        $query = "7za.exe x """ & @TempDir & "\mirupdate\packed.7z"" -o""" & @TempDir & "\mirupdate"" -aoa"
?57.?
        _RunDOS($query)
?58.?
 
?59.?
        FileDelete(@TempDir & "\mirupdate\packed.7z")
?60.?
        FileMove(@TempDir & "\mirupdate\*.*",@ScriptDir & "\*.*",1)
?61.?
        FileMove(@TempDir & "\mirupdate\Icons\*.*",@ScriptDir & "\Icons\*.*",1)
?62.?
        ShellExecute(@ScriptDir & "\miranda32.exe")
?63.?
        Exit
?64.?
EndIf
?65.?
 
?66.?
ShellExecute(@ScriptDir & "\miranda32.exe")
?67.?
Exit