#ifndef __87d726e0_26c6_485d_8016_1fba819b037d_EconomicRateInfo__
#define __87d726e0_26c6_485d_8016_1fba819b037d_EconomicRateInfo__

/*
Quotes (Forex) plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author
Copyright (C) Dioksin

Copyright 2000-2012 Miranda IM project,
all portions of this codebase are copyrighted to the people
listed in contributors.txt.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

===============================================================================

File name      : $HeadURL: 
Revision       : $Revision: 
Last change on : $Date: 
Last change by : $Author:
$Id$		   : $Id$:

===============================================================================
*/

#define QUOTES_PROTOCOL_NAME "Quotes"// protocol name

#define QUOTES_MODULE_NAME QUOTES_PROTOCOL_NAME          // db settings module path

enum ERefreshRateType
{
	RRT_SECONDS = 0,
	RRT_MINUTES = 1,
	RRT_HOURS = 2
};

#define DB_STR_ENABLE_LOG "EnableLog"
#define DB_STR_QUOTE_PROVIDER "QuoteProvider"
#define DB_STR_QUOTE_ID "QuoteID"
#define DB_STR_QUOTE_SYMBOL "QuoteSymbol"
#define DB_STR_QUOTE_DESCRIPTION "QuoteDescription"
#define DB_STR_QUOTE_PREV_VALUE "PreviousQuoteValue"
#define DB_STR_QUOTE_CURR_VALUE "CurrentQuoteValue"
#define DB_STR_QUOTE_FETCH_TIME "FetchTime"


enum ELogMode
{
	lmDisabled = 0x0000,
	lmInternalHistory = 0x0001,
	lmExternalFile = 0x0002,
	lmPopup = 0x0004,
};

#define DB_STR_CONTACT_SPEC_SETTINGS "ContactSpecSettings"
#define DB_STR_QUOTE_LOG "Log"
#define DB_STR_QUOTE_LOG_FILE "LogFile"
#define DB_STR_QUOTE_FORMAT_LOG_FILE "LogFileFormat"
#define DB_STR_QUOTE_FORMAT_HISTORY "HistoryFormat"
#define DB_STR_QUOTE_LOG_FILE_CONDITION "AddToLogOnlyIfValueIsChanged"
#define DB_STR_QUOTE_HISTORY_CONDITION "AddToHistoryOnlyIfValueIsChanged"
#define DB_STR_QUOTE_EXTRA_IMAGE_SLOT "ExtraImageSlot"
#define DB_STR_QUOTE_FORMAT_POPUP "PopupFormat"
#define DB_STR_QUOTE_POPUP_CONDITION "ShowPopupOnlyIfValueIsChanged"

#define DB_STR_QUOTE_POPUP_COLOUR_MODE "PopupColourMode"
#define DB_STR_QUOTE_POPUP_COLOUR_BK "PopupColourBk"
#define DB_STR_QUOTE_POPUP_COLOUR_TEXT "PopupColourText"
#define DB_STR_QUOTE_POPUP_DELAY_MODE "PopupDelayMode"
#define DB_STR_QUOTE_POPUP_DELAY_TIMEOUT "PopupDelayTimeout"
#define DB_STR_QUOTE_POPUP_HISTORY_FLAG "PopupHistoryFlag"


// #define DB_STR_NICK "Nick"
#define DB_STR_STATUS "Status"

#define LIST_MODULE_NAME "CList"
#define CONTACT_LIST_NAME "MyHandle"
#define STATUS_MSG_NAME "StatusMsg"

#endif //__87d726e0_26c6_485d_8016_1fba819b037d_EconomicRateInfo__
