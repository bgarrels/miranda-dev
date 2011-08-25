; by progandy - http://forum.miranda-im.de/index.php?topic=13928.msg79448#msg79448
#include<Date.au3>
#include<7zip.au3> ; http://www.autoitscript.com/forum/topic/85094-7zip/
 
; Aktuelle Version runterladen
$bData = InetRead("http://update.miranda-im.org/update.xml", 1)
 
; Wenn Daten vorhanden sind
If BinaryLen($bData) Then
        ; Daten in lesbaren Text umwandlen
        $sData = BinaryToString($bData, 4)
 
        ; Den Teil für die stable-Version auslesen
        $sStable = _StringSingleRegEx($sData, "(?s)<releasestable>(.*)</releasestable>")
 
        ; die echte Versionsnummer aus dem Stable-Teil
        $sRealVersion = _StringSingleRegEx($sStable, "(?s)<versionreal>\s*(.*?)\s*</versionreal>")
        ; die Anzeige-Versionsnummer aus dem Stable-Teil
        $sDisplayVersion = _StringSingleRegEx($sStable, "(?s)<versionpublic>\s*(.*?)\s*</versionpublic>")
 
        ; Das Veröffentlichungsdatum im Unix-Format
        $sDateUnix = _StringSingleRegEx($sStable, "(?s)<releasedate>\s*(.*?)\s*</releasedate>")
        ; Umrechnen in normale Datumsangabe
        $sDateCalc = _DateAdd("s", $sDateUnix, "1970/01/01 00:00:00")
 
        ; Link zur Unicode Exe
        $sUnicodeExe = _StringSingleRegEx($sStable, "(?s)<downloadunicodeexe>\s*(.*?)\s*</downloadunicodeexe>")
        ; Link zur Unicode Zip
        $sUnicodeZip = _StringSingleRegEx($sStable, "(?s)<downloadunicodezip>\s*(.*?)\s*</downloadunicodezip>")
        ; Link zur Ansi Exe
        $sAnsiExe = _StringSingleRegEx($sStable, "(?s)<downloadansiexe>\s*(.*?)\s*</downloadansiexe>")
        ; Link zur Ansi Zip
        $sAnsiZip = _StringSingleRegEx($sStable, "(?s)<downloadansizip>\s*(.*?)\s*</downloadansizip>")
        ; Link zur x64 Exe
        $sx64Exe = _StringSingleRegEx($sStable, "(?s)<downloadx64exe>\s*(.*?)\s*</downloadx64exe>")
        ; Link zur x64 zip
        $sx64Zip = _StringSingleRegEx($sStable, "(?s)<downloadx64zip>\s*(.*?)\s*</downloadx64zip>")
 
 
        ; Die Daten mal als MsgBox anzeigen
        MsgBox(0, 'Miranda Stable', "Displayed Version:         " & $sDisplayVersion & @CRLF & _
                        "Real Version:  " & $sRealVersion & @CRLF & _
                        "Release date:  " & $sDateCalc & @CRLF & _
                        "Unicode Exe:   " & $sUnicodeExe & @CRLF & _
                        "Unicode Zip:   " & $sUnicodeZip & @CRLF & _
                        "Ansi Exe:      " & $sAnsiExe & @CRLF & _
                        "Ansi Zip:      " & $sAnsiZip & @CRLF & _
                        "x64 Exe:       " & $sx64Exe & @CRLF & _
                        "x64 Zip:       " & $sx64Zip & @CRLF _
                        )
 
        ; Wähle Unicode Zip zu Download aus
        $sDownloadFile = $sUnicodeZip
 
        ; Erstelle Name für Zieldatei
        $sTarget = "miranda-" & _StringSingleRegEx($sDownloadFile, "-([^-]*)$")
 
        ; Starte Download ins Temporäre Verzeichnis
        Local $hDownload = InetGet($sDownloadFile, @TempDir & "\" & $sTarget, 1, 1)
 
        ; Erstelle Fortschrittsbalken
        ProgressOn("Downloading Miranda", "DL: v" & $sRealVersion & " to " & $sTarget, "Initialising DL ...", Default, Default, 16)
        ; Warte auf Downloadstart
        Do
                Sleep(200)
        Until InetGetInfo($hDownload, 2) Or InetGetInfo($hDownload, 0)
        ; Zeige Fortschritt unterschiedlich an, je nachdem ob die Dateigröße gesendet wurde
        If InetGetInfo($hDownload, 1) Then
                ; Wenn ja, dann zeige tatsächliche Fortschritt an
                Do
                        $read = InetGetInfo($hDownload, 0)
                        $complete = InetGetInfo($hDownload, 1)
                        ProgressSet($read/$complete*100, "Downloading " & Round($read/1024, 2) & "/" & Round($complete/1024, 2) & "kB")
                        Sleep(100)
                Until InetGetInfo($hDownload, 2) ; Check if the download is complete.
        Else
                ; Ansonsten einfach die Progressbar mehrmals durchlaufen lassen.
                $i = 0
                Do
                        $i += 5
                        If $i > 100 Then $i = 0
                        ProgressSet($i, "Downloading ...")
                        Sleep(100)
                Until InetGetInfo($hDownload, 2) ; Check if the download is complete.
        EndIf
        Local $aData = InetGetInfo($hDownload) ; Alle Informationen holen
        InetClose($hDownload) ; Download abschließen
        ProgressOff() ; Progressbar ausblenden
 
        ; Daten der Downloads anzeigen
        MsgBox(0, "", "Bytes read: " & $aData[0] & @CRLF & _
                        "Size: " & $aData[1] & @CRLF & _
                        "Complete?: " & $aData[2] & @CRLF & _
                        "Successful?: " & $aData[3] & @CRLF & _
                        "@error: " & $aData[4] & @CRLF & _
                        "@extended: " & $aData[5] & @CRLF)
 
        ; Bei erfolgreichem Download entpacken
        If $aData[3] Then
                DirCreate(@DesktopDir&"\TestMirandaDL")
                _7ZIPExtract(0, @TempDir & "\" & $sTarget, @DesktopDir&"\TestMirandaDL")
        EndIf
 
 
EndIf
 
Func _StringSingleRegEx(ByRef $sString, $sRegex)
        Local $aData = StringRegExp($sString, $sRegex, 1)
        If @error Then Return SetError(1, 0, '')
        Return $aData[0]
EndFunc   ;==>_StringSingleRegEx