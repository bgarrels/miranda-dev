#region ;**** Directives created by AutoIt3Wrapper_GUI ****
#AutoIt3Wrapper_Icon=mupdate.ico
#AutoIt3Wrapper_Outfile=mupdate.exe
#AutoIt3Wrapper_Res_Description=Miranda Updater
#AutoIt3Wrapper_Res_Fileversion=1.0.0
#AutoIt3Wrapper_Res_LegalCopyright=ProgAndy
#AutoIt3Wrapper_Res_Language=1031
#AutoIt3Wrapper_UseX64=n
#endregion ;**** Directives created by AutoIt3Wrapper_GUI ****
; #INDEX# =======================================================================================================================
; Title .........: Miranda Update
; AutoIt Version : 3.3
; Language ......: German (Deutsch)
; Description ...: Updater für Miranda
; Author(s) .....: ProgAndy
; Copyright .....: Copyright (C) ProgAndy. All rights reserved.
; License .......: Artistic License 2.0, see Artistic.txt
;
; This file is part of Miranda Update.
;
; Miranda Update is free software; you can redistribute it and/or modify
; it under the terms of the Artistic License as published by Larry Wall,
; either version 2.0, or (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
; See the Artistic License for more details.
;
; You should have received a copy of the Artistic License with this Kit,
; in the file named "Artistic.txt".  If not, you can get a copy from
; <http://www.perlfoundation.org/artistic_license_2_0> OR
; <http://www.opensource.org/licenses/artistic-license-2.0.php>
;
; ===============================================================================================================================

;~ $tagPLUGININFOEX = "int cbSize; ptr shortName; DWORD version; ptr description; ptr author; ptr authorEmail; ptr copyright; ptr homepage; BYTE flags; int replacesDefaultModule; byte uuid[16];"
;~ $d = DllOpen("D:\Programme\Miranda IM\Plugins\actman.dll")
;~ $r = DllCall($d, "ptr:cdecl", "MirandaPluginInfoEx", "dword", 0x000AFFFF)
;~ $t = DllStructCreate($tagPLUGININFOEX, $r[0])
;~ MsgBox(0, '', Hex(DllStructGetData($t, "version")))
;~ DllClose($d)
;~ Exit
#include<Misc.au3>
#include<WinApiEx.au3> ; source: www.autoitscript.com/forum/topic/98712-winapiex-udf/
#include<WinHttp.au3> ; source: www.autoitscript.com/forum/topic/84133-winhttp-functions/ (unter Artistic License 2)
#include<Date.au3>
#include<Array.au3>
#include<File.au3>
#include<GUIEdit.au3>
#include <ButtonConstants.au3>
#include <EditConstants.au3>
#include <GUIConstantsEx.au3>
#include <StaticConstants.au3>
#include <WindowsConstants.au3>
Global Const $INITIALWORKINGDIR = @WorkingDir
Global $fCanExit = True, $fCommandline = False
Global Const $oCOMError = ObjEvent("AutoIt.Error", "_COMError")
Global Const $sINI = @ScriptDir & "\MirandaUpdater.ini"

Global Enum Step *2 $FLAG_ALPHA, $FLAG_BETA, $FLAG_ANSI
Global $iOptionFlags = Number(IniRead($sINI, "Settings", "Flags", 0))
Global $fRunMiranda = Number(IniRead($sINI, "Settings", "Run", 0))
Global $sRunProfile = IniRead($sINI, "Settings", "RunParams", "")
Global $fSilent, $fAuto, $sMirandaPath
_ParseCMDLine()

Global $aUpdate, $sOnlineLangpack, $sCurrentLangpack, $fNewLangpack
Global $edtLog, $lblVersion, $lblNewVersion, $lblLangfile, $lblNewLanguage
HttpSetUserAgent("Miranda Updater (DE) v1.1")
Global $hInternet = _WinHttpOpen("Miranda Updater (DE) v1.1")
OnAutoItExitUnregister("_gc")


Global $sMirandaPath = _FindMiranda($sMirandaPath)
Global $sMirandaExe = "\miranda32.exe"

#region ### START Koda GUI section ### Form=
$guiMain = GUICreate("Miranda Updater", 615, 520)
$temp = GUICtrlCreateMenu("")
$miOptions = GUICtrlCreateMenuItem("&Optionen", -1)
$miAbout = GUICtrlCreateMenuItem("&Über", -1)
GUICtrlDelete($temp)
$Label1 = GUICtrlCreateLabel("Miranda-Pfad:", 8, 16, 70, 17)
$inpPfad = GUICtrlCreateInput("", 88, 14, 513, 21, BitOR($GUI_SS_DEFAULT_INPUT, $ES_READONLY))
$Label2 = GUICtrlCreateLabel("Version:", 8, 48, 42, 17)
$lblVersion = GUICtrlCreateLabel("", 88, 48, 380, 17)
GUICtrlCreateIcon("", 0, 473, 48, 128, 128)
If @Compiled Then
	GUICtrlSetImage(-1, @AutoItExe, 99)
Else
	GUICtrlSetImage(-1, @ScriptDir & "\mupdate.ico", 0)
EndIf
$Label3 = GUICtrlCreateLabel("Sprachpaket:", 8, 72, 68, 17)
$lblLangfile = GUICtrlCreateLabel("", 88, 72, 380, 17)
GUICtrlCreateLabel("Update: ", 8, 104, 80, 20)
GUICtrlSetFont(-1, 10, 800, 0)
$Label5 = GUICtrlCreateLabel("Version:", 8, 128, 68, 17)
$lblNewVersion = GUICtrlCreateLabel("", 88, 128, 380, 27)
$btnNotes = GUICtrlCreateButton("Releasenotes", 88, 160, 80, 20)
$Label7 = GUICtrlCreateLabel("Sprachpaket:", 8, 192, 76, 17)
$lblNewLanguage = GUICtrlCreateLabel("", 88, 192, 380, 17)
$btnUpdate = GUICtrlCreateButton("&Update", 8, 224, 201, 33, $BS_DEFPUSHBUTTON)
GUICtrlSetState($btnUpdate, $GUI_DISABLE)
$btnSearch = GUICtrlCreateButton("&Suchen", 220, 224, 100, 33)
$btnCancel = GUICtrlCreateButton("B&eenden", 400, 224, 201, 33)
$Label4 = GUICtrlCreateLabel("Installationslog: ", 8, 262, 79, 17)
$edtLog = GUICtrlCreateEdit("", 8, 280, 601, 215, BitOR($GUI_SS_DEFAULT_EDIT, $ES_READONLY))
GUISetState(@SW_HIDE)
#endregion ### END Koda GUI section ###

If Not $fSilent Then SplashTextOn("Miranda Updater", "Suche nach Updates...", 300, 80, Default, Default, 18)
If Not Ping("update.miranda-im.org", 2000) Then
	If Not $fSilent Then MsgBox(16, 'Miranda Updater', 'Keine Internetverbindung', $fAuto*2)
	If ($fSilent Or $fAuto) And $fRunMiranda Then Run(StringFormat('"%s%s" %s', $sMirandaPath, $sMirandaExe, $sRunProfile), $sMirandaPath, @SW_SHOW)
	Exit
EndIf
;~ _WinAPI_LoadBitmap
_PopulateGUI($iOptionFlags)
SplashOff()

If $sMirandaPath = "" Or Not FileExists($sMirandaPath) Then _Options($guiMain, True)

If Not $fSilent Then GUISetState(@SW_SHOW)
If $fSilent Or $fAuto Then
	_BtnUpdateClick()
	If $fRunMiranda Then Run(StringFormat('"%s%s" %s', $sMirandaPath, $sMirandaExe, $sRunProfile), $sMirandaPath, @SW_SHOW)
	Exit
EndIf

While 1
	$aMsg = GUIGetMsg(1)
	If @error Or $aMsg[1] <> $guiMain Then ContinueLoop
	Switch $aMsg[0]
		Case -3, $btnCancel
			If $fCanExit Then Exit
		Case $btnUpdate
			_BtnUpdateClick()
		Case $btnSearch
			_PopulateGUI($iOptionFlags)
		Case $btnNotes
			If IsArray($aUpdate) Then ShellExecute($aUpdate[7])
		Case $miAbout
			_About()
		Case $miOptions
			_Options($guiMain)
	EndSwitch
WEnd


Func _Options($hParent, $fNeedMiranda=False)

	GUISetState(@SW_DISABLE, $hParent)
	#region ### START Koda GUI section ### Form=
	Local $Form1 = GUICreate("Miranda Updater", 390, 200, Default, Default, Default, Default, $hParent)
	GUICtrlCreateLabel("Einstellungen für Update", 16, 8, 120, 17)
	GUICtrlCreateGroup("Entwicklungszweig", 16, 32, 358, 48)
	Local $rdoStable = GUICtrlCreateRadio("&Stable", 32, 56, 70, 17)
	GUICtrlSetTip(-1, "Diese Option hat keine Wirkung, falls eine Alpha- oder Beta-Version installiert ist.")
	If Not (BitAND($iOptionFlags, $FLAG_ALPHA) Or BitAND($iOptionFlags, $FLAG_BETA)) Then GUICtrlSetState(-1, $GUI_CHECKED)
	Local $rdoBeta = GUICtrlCreateRadio("&Beta", 102, 56, 70, 17)
	GUICtrlSetTip(-1, "Falls Stable aktueller, wird diese installiert." & @CRLF & "Diese Option hat keine Wirkung, falls eine Alphaversion installiert ist.")
	If BitAND($iOptionFlags, $FLAG_BETA) Then GUICtrlSetState(-1, $GUI_CHECKED)
	Local $rdoAlpha = GUICtrlCreateRadio("&Alpha", 172, 56, 70, 17)
	GUICtrlSetTip(-1, "Falls Beta oder Stable aktueller, wird diese installiert.")
	If BitAND($iOptionFlags, $FLAG_ALPHA) Then GUICtrlSetState(-1, $GUI_CHECKED)
	GUICtrlCreateGroup("", -99, -99, 1, 1)
	Local $chkAnsi = GUICtrlCreateCheckbox("verwende A&NSI-Version", 32, 90, 233, 17)
	GUICtrlSetTip(-1, "Diese Einstellung ist nur für Win95, 98, ME sinnvoll. Wenn möglich sollte immer die Unicode-Variante (Standard) genutzt werden.")
	If BitAND($iOptionFlags, $FLAG_ANSI) Then GUICtrlSetState(-1, $GUI_CHECKED)
	GUICtrlCreateLabel("Pfad zu Miranda", 16, 120, 120, 17)
	Local $inpPfad = GUICtrlCreateInput($sMirandaPath, 16, 140, 320, 21)
	GUICtrlSetTip(-1, "Pfad zu Miranda")
	Local $btnPfad = GUICtrlCreateButton("&...", 336, 140, 38, 21)
	Local $btnSearch = GUICtrlCreateButton("&Übernehmen && Schließen", 16, 170, 257, 25, $BS_DEFPUSHBUTTON)
	Local $btnCancel = GUICtrlCreateButton("Abbre&chen", 300, 170, 74, 25)
	GUISetState(@SW_SHOW)
	#endregion ### END Koda GUI section ###

	If $sMirandaPath = "" Then ControlClick($Form1, "", $btnPfad)
	While 1
		$nMsg = GUIGetMsg(1)
		If Not IsArray($nMsg) Or $nMsg[1] <> $Form1 Then ContinueLoop
		Switch $nMsg[0]
			Case $GUI_EVENT_CLOSE, $btnCancel
				If $fNeedMiranda Then Exit
				ExitLoop
			Case $btnPfad
				Local $sPfad = FileOpenDialog("Finde Miranda", GUICtrlRead($inpPfad), "Miranda (miranda32.exe)", 3, "miranda32.exe")
				If $sPfad And StringRight($sPfad, 13) = "miranda32.exe" And FileExists($sPfad) Then
					GUICtrlSetData($inpPfad, StringTrimRight($sPfad, 14))
				EndIf
			Case $btnSearch
				GUICtrlSetState($btnSearch, $GUI_DISABLE)
				$iOptionFlags = 0
				If BitAND(GUICtrlRead($rdoBeta), $GUI_CHECKED) Then $iOptionFlags = BitOR($iOptionFlags, $FLAG_BETA)
				If BitAND(GUICtrlRead($rdoAlpha), $GUI_CHECKED) Then $iOptionFlags = BitOR($iOptionFlags, $FLAG_ALPHA)
				If BitAND(GUICtrlRead($chkAnsi), $GUI_CHECKED) Then $iOptionFlags = BitOR($iOptionFlags, $FLAG_ANSI)
				If StringInStr(FileGetAttrib(GUICtrlRead($inpPfad)), "D") Then
					$sMirandaPath = GUICtrlRead($inpPfad)
				Else
					If $fNeedMiranda Then Exit
					MsgBox(16, 'Miranda Updater', 'Kein gültiger Pfad angegeben, behalte alten Pfad.')
				EndIf
				If Not $fCommandline Then
					IniWrite($sINI, "Settings", "Path", $sMirandaPath)
					IniWrite($sINI, "Settings", "Flags", $iOptionFlags)
				EndIf
				_PopulateGUI($iOptionFlags)
				ExitLoop
		EndSwitch
	WEnd
	GUISetState(@SW_ENABLE, $hParent)
	GUIDelete($Form1)
;~ 	WinActivate($hParent)
EndFunc

Func _PopulateGUI($iFlags)
	GUICtrlSetState($btnUpdate, $GUI_DISABLE)
	GUICtrlSetData($edtLog, "")
	Local $sMirandaArch = 'unicode'
	Local $iTemp = _WinAPI_GetBinaryType($sMirandaPath & $sMirandaExe)
	Switch @extended
		Case $SCS_32BIT_BINARY ; Oder nicht existent
			If BitAND($iFlags, $FLAG_ANSI) Then
				$sMirandaArch = 'ansi'
				_Log("ACHTUNG: ANSI-Version ist aktiviert (nur für Windows 98, ME und älter empfohlen!)")
			EndIf
			If Not $iTemp Then _Log("Kein Miranda gefunden, nehme portable Neuinstallation an.")
		Case $SCS_64BIT_BINARY
			$sMirandaArch = 'x64'
	EndSwitch

	Global $aUpdate = _FindUpdate($sMirandaPath, $iFlags, $sMirandaArch)
	Global $sOnlineLangpack = _GetOnlineLangpackVersion()
	Global $sCurrentLangpack = _GetCurrentLangpackVersion($sMirandaPath)
	Global $fNewLangpack = False
	If $sOnlineLangpack = "" Then
		_LogError("Neue Sprachpaketversion konnte nicht gefunden werden")
	Else
		If $sCurrentLangpack = "" Or _DateDiff("s", $sOnlineLangpack, $sCurrentLangpack) < 0 Then
			$fNewLangpack = True
		Else
			_Log("Sprachpaket ist aktuell")
		EndIf
	EndIf

	GUICtrlSetData($inpPfad, $sMirandaPath)
	GUICtrlSetData($lblVersion, FileGetVersion($sMirandaPath & $sMirandaExe, 'ProductVersion') & " (" & FileGetVersion($sMirandaPath & $sMirandaExe) & ")")
	GUICtrlSetData($lblLangfile, $sCurrentLangpack)
	If IsArray($aUpdate) Then
		GUICtrlSetData($lblNewVersion, $aUpdate[3] & " (" & $aUpdate[2] & ")" & @CRLF & "Typ: " & $aUpdate[8]); & @CRLF & "Datum: " & _DateAdd("s", $aUpdate[4], "1970/01/01 00:00:00"))
		GUICtrlSetState($btnNotes, $GUI_SHOW)
		GUICtrlSetState($btnUpdate, $GUI_ENABLE)
	Else
		GUICtrlSetData($lblNewVersion, "Kein Update vorhanden")
		GUICtrlSetState($btnNotes, $GUI_HIDE)
	EndIf
	If $fNewLangpack Then
		GUICtrlSetData($lblNewLanguage, $sOnlineLangpack)
		GUICtrlSetState($btnUpdate, $GUI_ENABLE)
	Else
		GUICtrlSetData($lblNewLanguage, "Keine neue Sprachdatei vorhanden")
	EndIf
EndFunc

Func _BtnUpdateClick()
	GUICtrlSetState($btnUpdate, $GUI_DISABLE)
	$fCanExit = False
	Local $sTempFolder = _TempFile(@TempDir, "mup", "", 5)
	DirCreate($sTempFolder)
	FileInstall("7za.exe", $sTempFolder & "\7za.exe")
	Local $sTargetPath = FileGetLongName($sMirandaPath, 1)
	If IsArray($aUpdate) Then
		If Not _GetFile($aUpdate[5], $sTempFolder & "\miranda.7z") Then
			_LogError("Miranda konnte nicht geladen werden")
		Else
			FileChangeDir($sTempFolder)
			_LogPart(@CRLF & "-----------------------------------------" & @CRLF)
			_Log("Miranda entpacken...")
			Local $iPID = Run('7za.exe x miranda.7z -y -x!mirandaboot.ini -o"' & $sTargetPath & '"', $sTempFolder, @SW_HIDE, 8)
			Local $s = StdoutRead($iPID)
			While Not @error
				Sleep(10)
				If $s Then _LogPart($s)
				$s = StdoutRead($iPID)
			WEnd
			StdioClose($iPID)
			_LogPart(@CRLF & "-----------------------------------------" & @CRLF)
		EndIf

		; download contrib
		If $aUpdate[6] And Not _GetFile($aUpdate[6], $sTempFolder & "\contrib.7z") Then
			_LogError("Contrib konnte nicht geladen werden")
		ElseIf $aUpdate[6] Then
			; extract contrib
			Local $sExclude = "", $sTestExclude
			; Nur bereits existierende DLLs aus dem Contrib-Paket holen
			For $sTestExclude In StringSplit('clist_modern.dll|clist_mw.dll|clist_nicer.dll|modernopt.dll|scriver.dll|tabsrmm.dll', '|', 2)
				If Not FileExists($sMirandaPath & "\Plugins\" & $sTestExclude) Then $sExclude &= " -x!Plugins/" & $sTestExclude
			Next
			FileChangeDir($sTempFolder)
			_LogPart(@CRLF & "-----------------------------------------" & @CRLF)
			_Log("Contrib entpacken...")
			Local $iPID = Run('7za.exe x contrib.7z -y -o"' & $sTargetPath & '" ' & $sExclude, $sTempFolder, @SW_HIDE, 8)
			Local $s = StdoutRead($iPID)
			While Not @error
				Sleep(10)
				If $s Then _LogPart($s)
				$s = StdoutRead($iPID)
			WEnd
			StdioClose($iPID)
			_LogPart(@CRLF & "-----------------------------------------" & @CRLF)
		EndIf
	EndIf
	If $fNewLangpack Then
		If Not $fSilent Then ProgressOn("Miranda Updater", "Sprachpaket herunterladen und installieren")
		If _DownloadLangpack($sMirandaPath, Not $fSilent) Then
			_Log("Sprachpaket wurde aktualisiert")
		Else
			_LogError("Sprachpaket konnte nicht aktualisiert werden")
		EndIf
		If Not $fSilent Then ProgressOff()
	EndIf
	FileChangeDir($INITIALWORKINGDIR)
	FileDelete($sTempFolder & "\*.*")
	DirRemove($sTempFolder, 1)
	_Log(@CRLF & "-----------------------------------------" & @CRLF & "Update beendet.")
	$fCanExit = True
EndFunc   ;==>_BtnUpdateClick

Func _FindUpdate($sMirandaPath, $iFlags, $sMirandaArch)
	$s = BinaryToString(InetRead("http://update.miranda-im.org/update.xml", 1), 4)
	If Not $s Then
		_LogError("Konnte Mirandaupdate nicht finden")
		Return SetError(1, 0, 0)
	EndIf


	$oXML = ObjCreate("Msxml2.DOMDocument")
	$oXML.loadXML($s)
	$oXML.setProperty("SelectionLanguage", "XPath")


	Local $fileVer = FileGetVersion($sMirandaPath & $sMirandaExe)
	Local $prodVer = FileGetVersion($sMirandaPath & $sMirandaExe, 'ProductVersion')
	Local $oData, $sContrib = '', $sBranch='Stable'
	Select
		Case StringInStr($prodVer, "alpha") Or BitAND($iFlags, $FLAG_ALPHA)
			Local $betaVer = _ObjGetText($oXML.selectSingleNode('//miranda/releases/releasebeta/versionreal'))
			Local $alphaVer = _ObjGetText($oXML.selectSingleNode('//miranda/releases/releasealpha/versionreal'))
			If _VersionCompare($betaVer, $alphaVer) > 0 Then ContinueCase
			$oData = $oXML.selectSingleNode('//miranda/releases/releasealpha')
			$sBranch = "Alpha"
		Case StringInStr($prodVer, "beta") Or BitAND($iFlags, $FLAG_BETA)
			Local $betaVer = _ObjGetText($oXML.selectSingleNode('//miranda/releases/releasebeta/versionreal'))
			Local $stableVer = _ObjGetText($oXML.selectSingleNode('//miranda/releases/releasestable/versionreal'))
			If _VersionCompare($stableVer, $betaVer) > 0 Then ContinueCase
			$oData = $oXML.selectSingleNode('//miranda/releases/releasebeta')
			$sContrib = "miranda-im-contrib-%s.7z"
			$sBranch = "Beta"
		Case Else
			$sContrib = "miranda-im-%s-%s-contrib.7z"
			$oData = $oXML.selectSingleNode('//miranda/releases/releasestable')
	EndSelect

	If Not IsObj($oData) Then
		_LogError("Kein Update vorhanden")
		Return SetError(2, 0, 0)
	EndIf
	If _VersionCompare(_ObjGetText($oData.selectSingleNode('versionreal')), $fileVer) <= 0 Then
		_Log("Miranda ist aktuell")
		Return SetExtended(1, True)
	EndIf
	Local $sDownload = _ObjGetText($oData.selectSingleNode('download' & $sMirandaArch & 'zip'))
	Local $sVersion = _StringSingleRegEx($sDownload, "-(v(?:[^-](?!7z))*)")
	If $sContrib Then $sContrib = StringRegExpReplace($sDownload, '[\\/][^\\/]*$', '/') & StringFormat($sContrib, $sVersion, $sMirandaArch)
	Local $aRet[9] = [$fileVer, $prodVer, _ObjGetText($oData.selectSingleNode('versionreal')), _
			_ObjGetText($oData.selectSingleNode('versionpublic')), _ObjGetText($oData.selectSingleNode('releasedate')), _
			$sDownload, $sContrib, _ObjGetText($oData.selectSingleNode('notesurl')), $sBranch]
	Return $aRet
EndFunc   ;==>_FindUpdate

Func _GetFile($sSource, $sTarget, $sText="Download...")
	If $fSilent Then Return InetGet($sSource, $sTarget, 10)
	ConsoleWrite($sSource & @CRLF)
	ProgressOn("Miranda Updater", $sText)
	ProgressSet(0, "Lade " & $sSource)
	Local $hDownload = InetGet($sSource, $sTarget, 10, 1)
	Do
		Sleep(20)
	Until InetGetInfo($hDownload, 0) Or InetGetInfo($hDownload, 2) ; Check if the download is complete.
	Do
		ProgressSet(InetGetInfo($hDownload, 0) / InetGetInfo($hDownload, 1) * 100)
		Sleep(20)
	Until InetGetInfo($hDownload, 2) ; Check if the download is complete.
	Local $res = InetGetInfo($hDownload, 3)
	InetClose($hDownload)
	ProgressOff()
	Return $res
	; TODO: Progress
EndFunc   ;==>_GetFile


Func _COMError()
	ConsoleWriteError("!COM Error: " & $oCOMError.number & " - " & $oCOMError.description & @CRLF)
EndFunc   ;==>_COMError

Func _ObjGetText(Const $o)
	If Not IsObj($o) Then Return SetError(1, 0, '')
	Return $o.text
EndFunc   ;==>_ObjGetText

Func _FindMiranda($sPath='')
	; Pfad in Kommandozeile hat Priorität 1
	If StringInStr(FileGetAttrib($sPath), "D") Then Return FileGetLongName($sPath, 1)
	; Eine INI hat Priorität 2
	$sPath = IniRead(@ScriptDir & "\MirandaUpdater.ini", "Settings", "Path", "")
	If $sPath And StringInStr(FileGetAttrib($sPath), "D") Then Return $sPath
	; 3: Arbeitsverzeichnis
	If FileExists(@WorkingDir & "\miranda32.exe") Then Return @WorkingDir
	; 4: Skriptverzeichnis
	If FileExists(@ScriptDir & "\miranda32.exe") Then Return @ScriptDir
	; 5: Uninstall-Pfad aus Registry
	$sPath = RegRead('HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Miranda', "UninstallString")
	If Not $sPath Then $sPath = RegRead('HKLM64\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Miranda', "UninstallString")
	If Not $sPath Then $sPath = RegRead('HKCU\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Miranda', "UninstallString")
	If Not $sPath Then $sPath = RegRead('HKCU64\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Miranda', "UninstallString")
	If StringLeft($sPath, 1) = '"' Then
		$sPath = StringRegExpReplace($sPath, '^"([^"]*)\\[^\\"]*".*', '\1')
	Else
		$sPath = StringRegExpReplace($sPath, '^([^\s]*?)\\[^\s\\].*', '\1')
	EndIf
	If $sPath Then Return $sPath
	; 6: anderer Pfad aus Registry
	$sPath = RegRead("HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\miranda32.exe", "Path")
	If Not $sPath Then $sPath = RegRead("HKLM64\SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\miranda32.exe", "Path")
	If Not $sPath Then $sPath = RegRead("HKCU\SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\miranda32.exe", "Path")
	If Not $sPath Then $sPath = RegRead("HKCU64\SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\miranda32.exe", "Path")
	If $sPath Then Return $sPath
	; 7: Standardinstallationspfad (32 bit)
	If FileExists(@ProgramFilesDir & "\Miranda IM\miranda32.exe") Then Return @ProgramFilesDir & "\Miranda IM"
	; sonst nichts
	Return ''
EndFunc   ;==>_FindMiranda

Func _gc()
	_WinHttpCloseHandle($hInternet)
EndFunc   ;==>_gc

Func _GetOnlineLangpackVersion()
	Local $hConn = _WinHttpConnect($hInternet, "miranda-im.de")
	Local $sFeed = _WinHttpSimpleRequest($hConn, "GET", "mediawiki/index.php?title=Spezial%3ALetzte+%C4nderungen&namespace=100&feed=rss")
	_WinHttpCloseHandle($hConn)
	Local $sDate = StringRegExp($sFeed, "<pubDate>([^<]+)</pubDate>", 1)
	If @error Then Return SetError(1, 0, "")
	Return _DateToCalcDate($sDate[0])
EndFunc   ;==>_GetOnlineLangpackVersion

Func _GetCurrentLangpackVersion($sMirandaPath)
	Local $s = FileRead($sMirandaPath & "\langpack_german.txt", 2048)
	Local $a = StringRegExp($s, "(?mi)^X-Langpack-creation-date: (.+)", 1)
	If @error Then Return SetError(1, 0, "")
	Return _DateToCalcDate($a[0])
EndFunc   ;==>_GetCurrentLangpackVersion


Func _DateToCalcDate($sDate)
	Local $a = StringRegExp($sDate, "(\d+)\s+(\w+)\s+(\w+)[^\d]+(\d+:\d+:\d+)(?:\s+(\w+))?", 1)
	If @error Then Return ""
	Local $aMonthNumberAbbrev[13] = ["", "Jan", "Feb", "Mar", "Apr", "May", "June", "July", "Aug", "Sept", "Oct", "Nov", "Dec"], $iMonth
	For $iMonth = 1 To 12
		If $a[1] = $aMonthNumberAbbrev[$iMonth] Then ExitLoop
	Next
	If $iMonth = 13 Then Return ""
	$sDate = $a[2] & "/" & $iMonth & "/" & $a[0] & " " & $a[3]
	If UBound($a) = 5 And $a[4] = "GMT" Then $sDate = _DateAdd("h", 1, $sDate)
	Return $sDate
EndFunc   ;==>_DateToCalcDate

Func _DownloadLangpack($sMirandaPath, $fProgress = False, $iProgress = 0)
	Local $iError = 0, $vData, $iRead = 0
	Local $hConn = _WinHttpConnect($hInternet, "miranda-im.de")
	If @error Then $iError = 1
	Local $hRequest = _WinHttpSimpleSendRequest($hConn, "POST", "mediawiki/index.php?title=Spezial:LanguagepackCreator", _
			"http://miranda-im.de/mediawiki/index.php?title=Spezial:LanguagepackCreator", "CreateFile=true")
	If _WinHttpQueryDataAvailable($hRequest) Then
		Local $iSize = -1
		Local $aCall = DllCall($hWINHTTPDLL__WINHTTP, "bool", "WinHttpQueryHeaders", _
				"handle", $hRequest, _
				"dword", BitOR($WINHTTP_QUERY_FLAG_NUMBER, $WINHTTP_QUERY_CONTENT_LENGTH), _
				"ptr", 0, _
				"dword*", 0, _
				"dword*", 4, _
				"ptr", 0)
		If Not @error Then $iSize = $aCall[4]
		Local $hFile = FileOpen($sMirandaPath & "\langpack_german.txt", 18)
		If @error Then $iError = 3
		While Not $iError
			$vData = _WinHttpReadData($hRequest, 2)
			If @error < 0 Then
				ExitLoop
			ElseIf @error Then
				$iError = 4
				ExitLoop
			EndIf
			$iRead += @extended
			If $fProgress Then
				If $iProgress Then
					GUICtrlSetData($iProgress, $iRead / $iSize * 100)
				Else
					ProgressSet($iRead / $iSize * 100)
				EndIf
			EndIf
			If Not FileWrite($hFile, $vData) Then $iError = 5
		WEnd
		ProgressOff()
		FileClose($hFile)
	Else
		$iError = 2
	EndIf
	_WinHttpCloseHandle($hRequest)
	_WinHttpCloseHandle($hConn)
	If $iError Then Return SetError($iError, 0, 0)
	Return 1
EndFunc   ;==>_DownloadLangpack

Func _StringSingleRegEx(ByRef $sString, $sRegex)
	Local $aData = StringRegExp($sString, $sRegex, 1)
	If @error Then Return SetError(1, 0, '')
	Return $aData[0]
EndFunc   ;==>_StringSingleRegEx

Func _LogPart($s)
	If $fSilent Then Return ConsoleWrite($s)
	_GUICtrlEdit_AppendText($edtLog, $s)
EndFunc   ;==>_LogPart

Func _Log($s)
	If $fSilent Then Return ConsoleWrite("> " & $s & @CRLF)
	_GUICtrlEdit_AppendText($edtLog, $s & @CRLF)
EndFunc   ;==>_Log

Func _LogError($err)
	If $fSilent Then Return ConsoleWriteError("!Error: " & $err & @CRLF)
	_GUICtrlEdit_AppendText($edtLog, "Fehler: " & $err & @CRLF)
EndFunc   ;==>_LogError

Func _ParseCMDLine()
	Local $sLast = ""
	For $i = 1 To $CMDLINE[0]
		If StringInStr("-+/", StringLeft($CMDLINE[$i], 1), 1) Then
			$sLast = StringTrimLeft($CMDLINE[$i], 1)
			Switch $sLast
				Case "help"
					_About()
					Exit
				Case "alpha"
					$iOptionFlags = BitOR($iOptionFlags, $FLAG_ALPHA)
					$fCommandline = True
				Case "beta"
					$iOptionFlags = BitOR($iOptionFlags, $FLAG_BETA)
					$fCommandline = True
				Case "ansi"
					$iOptionFlags = BitOR($iOptionFlags, $FLAG_ANSI)
					$fCommandline = True
				Case "silent"
					$fSilent = True
					$fCommandline = True
				Case "auto"
					$fAuto = True
					$fCommandline = True
				Case "run"
					$fRunMiranda = True
					$fCommandline = True
			EndSwitch
		Else
			If $sLast = "" Then
				$sMirandaPath = $CMDLINE[$i]
				$fCommandline = True
			ElseIf $sLast = "run" Then
				$sRunProfile = $CMDLINE[$i]
			EndIf
		EndIf
	Next
EndFunc   ;==>_ParseCMDLine

Func _About()
	MsgBox(0, 'Über Miranda Updater', "Miranda Updater aktualisiert einen deutschen Miranda-Kern. Andere Plugins werden mit dem Updater aktualisiert." & @CRLF & _
			"(C) by ProgAndy, Lizenz: Artistic License 2" & @CRLF & @CRLF & _
			"Aufruf: " & @ScriptName & " [MirandaPfad] [PARAMETER]" & @CRLF & @CRLF & "PARAMETER: " & @CRLF & "-help : Diese Hilfe" & @CRLF & _
			"-ansi : Lade ANSI-Version (x64 wird automatisch erkannt)" & @CRLF & _
			"-alpha : Lade Alpha wenn aktueller als Beta und Stable (wie wenn bereits Alpha installiert)" & @CRLF & _
			"-beta : Lade Beta wenn aktueller als Stable (wie wenn bereits Beta installiert)" & @CRLF & _
			"-silent : Automatisches Update ohne Anzeige" & @CRLF & _
			"-auto : Automatisches Update mit Anzeige" & @CRLF & _
			"-run [Params] : Starte anschließend Miranda mit den Werten aus mirandaboot.ini und den angegebenen Parametern" & @CRLF & _
			"____________________________________________________________________________________" & @CRLF & @CRLF & _
			"Dieses Programm verwendet 7za.exe welches unter LGPL steht. (http://7-zip.org)")
EndFunc   ;==>_About
