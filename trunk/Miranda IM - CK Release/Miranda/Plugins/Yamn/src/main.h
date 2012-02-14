#ifndef __MAIN_H
#define __MAIN_H

//For updater
//#define YAMN_9x
#ifndef WIN2IN1
#ifdef YAMN_9x
	#define YAMN_SHORTNAME "YAMN CK Release"
	#define YAMN_FILENAME "yamn_9x"
#else
	#define YAMN_SHORTNAME "YAMN CK Release"
	#define YAMN_FILENAME "yamn"
#endif
#else
	#define YAMN_SHORTNAME "YAMN CK Release"
	#define YAMN_FILENAME "yamn"
#endif //WIN2IN1

#include "version.h"
#define YAMN_NEWMAILSNDDESC		"YAMN: new mail message"
#define YAMN_CONNECTFAILSNDDESC	"YAMN: connect failed"
#define	YAMN_CONNECTFAILSOUND	"YAMN/Sound/ConnectFail"
#define	YAMN_NEWMAILSOUND		"YAMN/Sound/NewMail"

#define YAMN_DBMODULE		"YAMN"
#define YAMN_DBPOSX			"MailBrowserWinX"
#define YAMN_DBPOSY			"MailBrowserWinY"
#define YAMN_DBSIZEX		"MailBrowserWinW"
#define YAMN_DBSIZEY		"MailBrowserWinH"
#define YAMN_DBMSGPOSX		"MailMessageWinX"
#define YAMN_DBMSGPOSY		"MailMessageWinY"
#define YAMN_DBMSGSIZEX		"MailMessageWinW"
#define YAMN_DBMSGSIZEY		"MailMessageWinH"
#define YAMN_DBMSGPOSSPLIT	"MailMessageSplitY"
#define YAMN_HKCHECKMAIL	"HKCheckMail"
#define	YAMN_TTBFCHECK		"ForceCheckTTB"
#define	YAMN_SHOWMAINMENU	"ShowMainMenu"
#define	YAMN_CLOSEDELETE	"CloseOnDelete"
#define YAMN_SHOWASPROTO	"ShowAsProtcol"
#define	YAMN_DBTIMEOPTIONS	"MailBrowserTimeOpts"

#define YAMN_DEFAULTHK		MAKEWORD(VK_F11,MOD_CONTROL)

#define SHOWDATELONG		0x01
#define SHOWDATENOTODAY		0x02
#define SHOWDATENOSECONDS	0x04

extern unsigned char optDateTime;

void UnloadPlugins();

//  Loading Icon and checking for icolib 
void LoadIcons();
extern int iconIndexes[];


#endif

