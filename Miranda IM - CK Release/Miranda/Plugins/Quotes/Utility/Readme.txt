Quotes version 0.0.16.0 plugin for Miranda 

This plugin displays periodically updated economic quotes, currency exchanges and different economic and financial indexes in Miranda contact list.

Author: Dioksin.
You can always contact me via email dioksin@ua.fm

How to install:
unpack zip archive;
copy quotes.dll and Quotes subdirectory to miranda plugins floder.
copy proto_Quotes.dll to miranda icons folder

Changelog Release 0.0.17.0
1. The percent change with respecto to yesterday close variable was added to the Google Finance
2. The Vietnamese Dong was added to the Currency Converter

Changelog Release 0.0.16.0
1. It was impossible to get quotes from google finance on non-USA locale

Changelog Release 0.0.15.0
1. Crash if invalid display name format was used

Changelog Release 0.0.14.0
1. References to Microsoft's XML parser were removed.

Changelog Release 0.0.12.0
1. The Swap button had been added to the Currency Converter

Changelog Release 0.0.11.0
1. Google finance is supported
2. Occupied status was made optionally (To use it's necessary to set the 'ExtendedStatus' option in the Miranda's database).

Changelog Release 0.0.9.981
1. Currency converter
2. Refresh all Quotes\Rates
3. Refresh particular Quote\Rate
2. Minor fixings to better support national language pack

Changelog Release 0.0.0.8
1. Minor changing in logic of status
2. Minor resource modification to better support national language pack

Changelog Release 0.0.0.7
1. Status message may be set on per contact basis (second line in contact list)
2. Two new modes were added.  Occupied - it's set if error occurred during rate/quote updation.  DND - it's set if updation is in progress.
3. Open Log File menu item was added and Quotes related menu items are grouped in Quotes popup menu.
4. Several new variables were added: previous rate, fetch date, fetch time.
5. New empty icon was added. This icon is used when rate/quote was not changed.
6. Quotes\rate info page was changed to show both current and previous rates.
7.  Database ForceToAddArrowToNick setting was added. This value governs an up\down arrow appearance in contact name. If it's equal 0 (default value) - arrows will be added only if extraicons were set . It it' equal 1 - arrows will be shown always and if it's equal 2 - arrows will be never shown.

Changelog Release 0.0.0.6
1. Change to log and to history only if rate (quote) value changed option has been added
2. Several minor bugs were fixed

Changelog Release 0.0.0.5
1. Extraicons supporting
2. Ability to change name in contact list with variables
3. Log to file
4. Log to Miranda's history
5. Proxy server supporting
6. Protocol icons were changes and removed to separate dll

Changelog Release 0.0.0.4
1. It is possible to get currency exchange rates from Google site
2. Unicode supporting was improved
3. User info page was modified
4. Option pages were moved under Plugin section

Changelog Release 0.0.0.3
1. Fix bug when decimal separator was not dot
2. User info page was added

Changelog Release 0.0.0.2
1. The size of plugin was reduced
2. The plugin was statically link with CRT library to resolve some dependencies to system modules
3. The updater plugin is supported now.


