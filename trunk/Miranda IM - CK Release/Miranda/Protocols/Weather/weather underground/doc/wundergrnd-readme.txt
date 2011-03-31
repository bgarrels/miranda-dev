Weather Underground Support for Weather Protocol  Version 2008.07.11
====================================================================

 This plugin retrieve weather information from www.weatherundergound.com, and it
 supports both US and international cities.

 Some limitations in this version:
   - The plugin currently does not parse information for:
  - Wind gusts
  - Any other info that does not appear in the variable list.

Latest Changes
--------------
  2008.12.06  Updated for new website
  2008.07.11  Updated for new website
  2008.06.07  Fixed wind display
  2008.04.26  Fixed Feels-Like temperature
  2008.04.25  Fixed Visibility
  2008.04.24  Fixed search and wind speed
  2008.02.19	Many fixes 
  2008.02.17	Many fixes 
  2008.02.07  Fixed time display
  2008.02.04  Updated for new website
  2006.12.23  Fixed US search to find only US cities
              Fixed International RSS ini
              Added ini for Canada (some cities should use Intl one)
  2006.12.01  Fixed pressure change indication 
  2006.11.19  Added Feel-Like temperature
              Fixed update time for RSS feeds
  2006.07.18  Updated for new website
  2006.07.16  Updated for new website / Fixed search
  2006.03.03  Updated for new website
  2006.03.02  Updated for new website
  2005.09.29  Updated for new website, provided ini file for RSS feed
  2004.06.19  Fixes in forecast, new UV variable
  2004.05.05  New WU webpage format update
  2004.04.29  Add variables (moonrise/set, light length, moon phase, text forecast)
              Fix again when wind is calm
  2004.04.28  Fix in searching US station, update when wind is calm
  2004.04.09  Some changes in format, plus some fixes
  2004.04.08  Update to v1.1, some new variables
  2004.03.23  Some fixes in visibility
  2004.03.22  Initial release


Files Included
--------------
 - plugins\weather\wundergrnd.ini
    - File for Weather Underground website - US cities
 - plugins\weather\wundergrnd_intl.ini
    - File for Weather Underground website - International cities
 - plugins\weather\wurss.ini
    - File for Weather Underground RSS feed - US cities
 - plugins\weather\wurss_intl.ini
    - File for Weather Underground RSS feed - International cities
 - plugins\weather-doc\wundergrnd-readme
    - The readme document (this one)


System Requirement
------------------
 - Weather Protocol v0.3.1.8 or higher (recommended: 0.3.2.9).
 - Everything else that are required for Weather Protocol.


Installation
------------
 For installation of weather protocol, it is recommended for installing it using Miranda
 Installer.  For manual installation, unzip the content in the zip file (the 2 ini's)
 into "<Miranda-Root>\Pugins\Weather".


Obtaining Station ID for Weather Underground Contacts
-----------------------------------------------------
 This section is included in the readme file in case if you cannot find your city
 using the internal find feature in add/remove contact.  To find a Weather Underground
 city ID, follow the 2 steps below:

   1. For international cities, go to
  http://www.wunderground.com/about/faq/international_cities.asp
      Find the name of your city in the list, and then get the ID from the WMO column.
      NOTE:  Some ID contains no data!
   2. For US Cities the ID consist of two character state code, follow by a backward
      slash, and then the name of your city.  All spaces in the city name should change
      into an underscore "_".  An example of the ID:  CA/Los_Angeles


Supported Variables
-------------------
 - Here is a list of the variables that are supported in this version

     The Basic Variables
  %c  current condition
  %d  current date
  %e  dewpoint
  %f  feel-like temperature
  %h  today's (or tomorrow's) high
  %i  wind direction
  %l  today's (or tomorrow's) low
  %m  humidity
  %n  station name
  %p  pressure
  %r  sunrise time
  %s  station ID
  %t  temperature
  %u  update time
  %v  visibility
  %w  wind speed
  %y  sun set

     The Special Variables (For US cities only, required Weather Protocol v0.3.2.0+)
  %[Alert]  Weather alert/advisory/statement/watch/warning title

     Forecast Variables (required Weather Protocol v0.3.2.0+)
  %[Forecast Day n Day] The day of week for forecast day n (ie. Mon, Tues, etc)
  %[Forecast Day n High]  The forecast high for day n.
  %[Forecast Day n Low] The forecast low for day n.
  %[Forecast Day n Condition] The weather condition for day n (ie. Sunny, etc)
  %[Forecast Day n] The complete forecast for day n (include all of above)
    -> Note that n can be anything from 1 to 5.

     Text Forecast  (work better with Weather Protocol v0.3.2.9+)
  %[Forecast Day n Text]    For daytime
  %[Forecast Day n Night Text]  For night
    -> Note that n can be anything from 1 to 7 (except Day 1 and Day 7 Night).

     Other Variables  (required Weather Protocol v0.3.2.0+)
  %[Pressure Change]  The change in pressure (ie. rising, falling, steady)
  %[Day Length]   The length of day
  %[Visible Light Length] The length of visible light
  %[Moonrise]   The time for moon rise
  %[Moonset]    The time for moon set
  %[Moon Phase]
  %[UVI]      UV index for today


Translation
-----------
 - Conditions -> Normal weather condition strings included in Weather Protocol.
 - Directions -> The short form included in Weather Protocol (ie. N, E, S, W).
  [Var] - An extra translation string for variable wind
 - Day of Week -> The short form included in Weather Protocol (ie. Mon, Tue).
 - Pressure Changes -> The capitalized form included in Weather Protocol.
 - Alerts -> Not in the Weather Protocol package, but too many to list here...
 - Text forecast -> Not translable.
