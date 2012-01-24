Es wird vorausgesetzt, dass die als Plugin verfügbare mimcmd.exe und dessen CmdLine.dll im Miranda IM Hauptordner sowie Plugins-Ordner liegen und dass sich die MIM-Update.bat hier befindet:

C:\Programme\GnuWin32\bin\


Hierfür muss das Tool Wget for Windows installiert sein.
Es wird ferner vorausgesetzt, dass auf C: ein Standard-Download-Ordner namens Downloads existiert und die Freeware 7-Zip muss installiert sein um zum Beispiel alle Archive lesen zu können.


Die Installationen für die USB-Sticks (auf dem für das Script benutzten Rechner O: und P:) sind hier per REM auskommentiert, also deaktiviert und müssen bei Bedarf hinsichtlich der Pfade an die eigenen Verhältnisse angepasst werden. Nähere Informationen stehen in den REM-Comments über den jeweiligen Abschnitten.

Solttet Ihr andere Parameter und Pfade nutzen wollen muss eine Editierung im Script erfolgen.


-- All credits goes to madmax25 -- 


*** Update *****

Dieses neue Script prüft die Verfügbarkeit der Seite und setzt das Update fort, sobald es feststellt, dass die Seite online ist.
Ist die Seite dagegen offline, bricht es den Update-Vorgang ab.
Beim nächsten Update-Versuch, am nächsten Tag oder beim nächsten Booten (oder wann auch immer man es per Autostart oder Taskplaner startet), wird das Update durchgeführt, sofern die Update-Seite dann wieder online ist.

Ich habe es MIMOCU.bat genannt, also "MIM-Online-Checker+Updater" (...was "Geistreicheres" fiel mir leider nicht ein!  )

Wer sein Miranda mit Hilfe der hier angebotenen Scripte updaten möchte, sollte den folgenden Code als MIMOCU.bat im gleichen Ordner wie die bisherigen Scripte speichern und den Update-Vorgang ab sofort nicht mehr über die MIM Update-Checker.bat, sondern über dieses neue Script starten (wie gesagt, entweder durch eine Autostart-Verknüpfung oder indem man damit z.B. einen täglichen Task im Taskplaner erstellt).