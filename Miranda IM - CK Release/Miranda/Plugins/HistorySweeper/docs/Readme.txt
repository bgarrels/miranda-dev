History Sweeper plugin 
Version 0.2.0.2
License: Open Source (LGPL)
Written by and Copyright (c) Sergey V. Gershovich a.k.a. Jazzy$, 2002-2003
                             e-mail: jazzy@arcticsu.ru
Modified by and Copyright (c) Boris Krasnovskiy, 2006-2009
                             e-mail: borkra@miranda-im.org

This plugin can: 
 - sweep system history;
 - sweep history from all contacts;
 - sweep from chosen contacts only;
 - sweep history older than certain time;
 - sweep history at Miranda IM startup/shutdown.


***** DISCLAIMER OF WARRANTY ***** 

This software and the accompanying files are presented "AS IS" and without 
warranties whether expressed or implied. Use it at your own risk.

!!! Backup you database first !!!

********************************** 

What's new
-------------
0.2.0.2     - Fixed All Contacts option setting
            - Fixed Unicode flag in VI

Installation
-------------
1. Close Miranda IM.
2. Find Miranda IM main directory (search for miranda32.exe).
3. Put historysweeper.dll into plugins directory.
4. Start Miranda IM.
5. Go to Options/Plugins/History Sweeper to configure some stuff. 


Uninstall
-----------
1. Go to Options->Plugins->'History Sweeper' and uncheck all contacts
2. Press 'Apply' button
3. If you have 'Plugin sweeper' plugin installed go to it page and uninstall History Sweeper plugin
4. Close Miranda IM.
5. Find Miranda IM main directory (search for miranda32.exe).
6. Delete historysweeper.dll from Plugins directory.
7. Start Miranda IM.


Usage
------
!!! Close all opened message & history windows !!!

MainMenu -> Sweep system history (you may change it action in options)

Right click on contact -> Sweep history

Main menu -> Options -> History Sweeper  (for options)

note: if you have a large database you need a long time to sweep them all.


"UNSAFE mode" and perfomance
----------------------------
Miranda IM DevTeam says:
"Miranda's database is normally protected against corruption by agressively 
flushing data to the disk on writes. ("Safe mode") If you're doing a lot of writes 
(eg in an import plugin) it can sometimes be desirable to switch this feature off 
to speed up the process. If you do switch it off, you must remember that crashes are far 
more likely to be catastrophic, so switch it back on at the earliest possible opportunity."

"Use UNSAFE mode" switch affected ONLY to Histroy Sweeper plugin and switched off by default.


== TO PLUGIN DEVELOPERS ==

This plugin has its Base Address set to:
0x30030000


Version history
------------------
0.2.0.1     - Forcing upgrade to Unicode version
0.2.0.0     - Added selectable No History
            - Added IcoLib support
            - Added Unicode & x64 versions
            - Reworked options
0.1.3.2     - Fixed history clenaup on shutdown
            - Compatibility updates with Miranda 0.7
0.1.3.1     - Support for new plugin API
            - Fixed services/hooks removal on exit
0.1.2.2     - Reduced GDI resource utilization
0.1.2.1     - Fixed shutdown/startup scheduling
            - Added preservation of History++ bookmarked history

0.1.2.0     - Fixed few bugs
            - Vast performance improvements

0.1.1.3     - Fixed (maybe) bug with memory leaks
            - Fixed incorrect URL in options page
            - Add new time interval (3 Days)
            - New string in translation file

0.1.1.2     - Fixed bug with save/load settings

0.1.1.1     - Don't show contact menu item if history messages does not exists.
            - Contact menu item show number of history messages
            - Miranda IM installer support

0.1.1.0     - Two sweep modes:
              1.'Safe' (slow, but safe)
              2.'Unsafe' (sweep history as fast as possible, but unsafe)
            - Add ability to sweep ALL history events or older than:
              1 Day, 7 Days, 2 Weeks, 1 Month, 3 Months, 6 Months, 1 Year
            - Language strings added/removed/updated
            - Ported from Delphi to C++
            - Some optimisation in code made
            - Change image base address to: 0x30030000
            - Refer to 'Miranda IM' instead of simple 'Miranda'

0.1.0.8     - Custom action at Miranda IM startup/shutdown
            - Custom main menu action
            - Language strings added/updated

0.1.0.7     - Now, you may select contacts/groups from which, history 
              will be sweeped at Miranda IM startup/shutdow.
              (check option page: Plugins -> History Sweeper)
            - Replaced "Purge all history" -> "Sweep system history"
            - Language strings removed/updated
            - Some optimisation in code

0.1.0.4:    - Options dialog slightly redesigned
            - Language strings updated

0.1.0.3:    - Menu items can be removed with the options

0.1.0.2:    - Add ability to purge history at Miranda startup or shutdown (see options page)
            - "Plugin Sweeper" support

0.1.0.1:    - Change name of plugin to "History Sweeper"
            - Reduce size of DLL
            - Menu position & items redisigned
            - Translation support (see translation_historysweeper.txt)
            - Fixed icon in 'question message box'

0.1.0.0:    - Initial release