Es wird vorausgesetzt, dass die als Plugin verf�gbare mimcmd.exe und dessen CmdLine.dll im Miranda IM Hauptordner sowie Plugins-Ordner liegen und dass sich die MIM-Update.bat hier befindet:

C:\Programme\GnuWin32\bin\


Hierf�r muss das Tool Wget for Windows installiert sein.
Es wird ferner vorausgesetzt, dass auf C: ein Standard-Download-Ordner namens Downloads existiert und die Freeware 7-Zip muss installiert sein um zum Beispiel alle Archive lesen zu k�nnen.


Die Installationen f�r die USB-Sticks (auf dem f�r das Script benutzten Rechner O: und P:) sind hier per REM auskommentiert, also deaktiviert und m�ssen bei Bedarf hinsichtlich der Pfade an die eigenen Verh�ltnisse angepasst werden. N�here Informationen stehen in den REM-Comments �ber den jeweiligen Abschnitten.

Solttet Ihr andere Parameter und Pfade nutzen wollen muss eine Editierung im Script erfolgen.


-- All credits goes to madmax25 -- 


*** Update *****

Dieses neue Script pr�ft die Verf�gbarkeit der Seite und setzt das Update fort, sobald es feststellt, dass die Seite online ist.
Ist die Seite dagegen offline, bricht es den Update-Vorgang ab.
Beim n�chsten Update-Versuch, am n�chsten Tag oder beim n�chsten Booten (oder wann auch immer man es per Autostart oder Taskplaner startet), wird das Update durchgef�hrt, sofern die Update-Seite dann wieder online ist.

Ich habe es MIMOCU.bat genannt, also "MIM-Online-Checker+Updater" (...was "Geistreicheres" fiel mir leider nicht ein!  )

Wer sein Miranda mit Hilfe der hier angebotenen Scripte updaten m�chte, sollte den folgenden Code als MIMOCU.bat im gleichen Ordner wie die bisherigen Scripte speichern und den Update-Vorgang ab sofort nicht mehr �ber die MIM Update-Checker.bat, sondern �ber dieses neue Script starten (wie gesagt, entweder durch eine Autostart-Verkn�pfung oder indem man damit z.B. einen t�glichen Task im Taskplaner erstellt).