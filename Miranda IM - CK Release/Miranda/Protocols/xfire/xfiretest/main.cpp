#include "stdafx.h"
/*
Plugin of Miranda IM for Communicating with users of the XFire Network. 
 
Copyright (C) 2008-2012 by
           dufte aka andreas h. <dufte@justmail.de>
 

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
 
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 
Based on	J. Lawler              - BaseProtocol
			Herbert Poul/Beat Wolf - xfirelib

Miranda : the free icq client for MS Windows 
Copyright (C) 2000-2012  Richard Hughes, Roland Rabien & Tristan Van de Vreede
*/

//xfire stuff
#include "client.h"
#include "xfirepacket.h"
#include "loginfailedpacket.h"
#include "otherloginpacket.h"
#include "messagepacket.h"
#include "sendstatusmessagepacket.h"
#include "sendmessagepacket.h"
#include "invitebuddypacket.h"
#include "sendacceptinvitationpacket.h"
#include "senddenyinvitationpacket.h"
#include "sendremovebuddypacket.h"
#include "sendnickchangepacket.h"
#include "sendgamestatuspacket.h"
#include "sendgamestatus2packet.h"
#include "dummyxfiregameresolver.h"
#include "sendgameserverpacket.h"
#include "recvstatusmessagepacket.h"
#include "recvoldversionpacket.h"
#include "packetlistener.h"
#include "inviterequestpacket.h"
#include "buddylistgames2packet.h"
#include "dummyxfiregameresolver.h"
#include "sendtypingpacket.h" 
#include "xfireclanpacket.h"
#include "recvremovebuddypacket.h"

//miranda stuff
#include "baseProtocol.h"

#include <stdexcept>

HANDLE hLogEvent;
int bpStatus = ID_STATUS_OFFLINE;
int previousMode;
int OptInit(WPARAM wParam,LPARAM lParam);
int OnDetailsInit(WPARAM wParam,LPARAM lParam);
HANDLE hFillListEvent = 0;
CONTACT user;
HINSTANCE hinstance;
PLUGINLINK *pluginLink;
MM_INTERFACE		mmi;
UTF8_INTERFACE utfi;
HANDLE heventXStatusIconChanged;
HANDLE copyipport,gotoclansite,vipport,joingame;
pthread_t gamedetection;
XFire_FoundGame xf[255];
int foundgames=0;
GameIco icocache[256];
CRITICAL_SECTION modeMsgsMutex;
Gdiplus::GdiplusStartupInput gdiplusStartupInput;
ULONG_PTR           gdiplusToken;  
//CRITICAL_SECTION avatarMutex;
HANDLE	 XFireAvatarFolder = NULL;
HANDLE	 XFireWorkingFolder = NULL;

int EXTRA_ICON_ADV1 = 4;
int EXTRA_ICON_ADV2 = 5;

PLUGININFOEX pluginInfo={
		sizeof(PLUGININFOEX),
		"XFire Protocol",
		PLUGIN_MAKE_VERSION(0,1,3,3),
		"XFire Protocol Plugin by dufte [Built: "__DATE__" "__TIME__"]",
		"dufte",
		"dufte@justmail.de",
		"(c)2008-2012 XFirelib by Herbert Poul, XFire Miranda protocol plugin by dufte",
		"",
		0,
		0,
		// {9B8E1735-970D-4ce0-930C-A561956BDCA2}
		{ 0x9b8e1735, 0x970d, 0x4ce0, { 0x93, 0xc, 0xa5, 0x61, 0x95, 0x6b, 0xdc, 0xa2 } }

};


int FillList(WPARAM wParam, LPARAM lParam);
HANDLE CList_AddContact(XFireContact xfc, bool InList, bool SetOnline,int clan);
HANDLE CList_FindContact (int uid);
void CList_MakeAllOffline();
int RecvMessage(WPARAM wParam, LPARAM lParam);
int SendMessage(WPARAM wParam, LPARAM lParam);
static int UserIsTyping(WPARAM wParam, LPARAM lParam);
HANDLE LoadGameIcon(char* g, int id, HICON* ico,BOOL onyico=FALSE);
void SetIcon(HANDLE hcontact,HANDLE hicon,int ctype=EXTRA_ICON_ADV1);
BOOL GetAvatar(char* username,XFireAvatar* av);
//void SetAvatar(HANDLE hContact, char* username);
static void SetAvatar(LPVOID lparam);
static int GetIPPort(WPARAM /*wParam*/,LPARAM lParam);
static int GetVIPPort(WPARAM /*wParam*/,LPARAM lParam);
int RebuildContactMenu( WPARAM wParam, LPARAM lParam );
static int GotoProfile(WPARAM wParam,LPARAM lParam);
static int GotoXFireClanSite(WPARAM wParam,LPARAM lParam);
static int ReScanMyGames(WPARAM wParam,LPARAM lParam);
void *gamedetectiont(void *ptr);
static void Scan4Games( LPVOID lparam );
static int GotoProfile2(WPARAM wParam,LPARAM lParam);
void handlingBuddys(BuddyListEntry *entry, int clan,char* group=NULL);
int StatusIcon(WPARAM wParam,LPARAM lParam);
int AddtoList( WPARAM wParam, LPARAM lParam );
int BasicSearch(WPARAM wParam,LPARAM lParam);
static int SearchAddtoList(WPARAM wParam,LPARAM lParam);
void CreateGroup(char*grp);
int SetAwayMsg(WPARAM wParam, LPARAM lParam);
int GetAwayMsg(WPARAM /*wParam*/, LPARAM lParam);
int ContactDeleted(WPARAM wParam,LPARAM /*lParam*/);
static int StartGame(WPARAM wParam,LPARAM lParam,LPARAM fParam);
int JoinGame(WPARAM wParam,LPARAM lParam);

//XFire Stuff
using namespace xfirelib;

class XFireClient : public PacketListener {

  public:
    Client *client;

    XFireClient(string username, string password);
    ~XFireClient();
    void run();

    void Status(string s);

    void receivedPacket(XFirePacket *packet);

	void getBuddyList();
	void sendmsg(char*usr,char*msg);
	void setNick(char*nnick);

  private:
    vector<string> explodeString(string s, string e);
    string joinString(vector<string> s, int startindex, int endindex=-1, string delimiter=" ");
    void BuddyList();

    string *lastInviteRequest;

    string username;
    string password;
	BOOL connected;
 };

XFireClient* myClient;

void XFireClient::setNick(char*nnick) {
	if(strlen(nnick)==0)
		return;
	  SendNickChangePacket nick;
      nick.nick = mir_utf8encode(( char* )nnick);
	  client->send( &nick );
}


void XFireClient::sendmsg(char*usr,char*cmsg) {
		SendMessagePacket msg;
	//	if(strlen(cmsg)>255)
	//		*(cmsg+255)=0;
		msg.init(client, usr, cmsg);
		client->send( &msg );
	}


  XFireClient::XFireClient(string username_,string password_) 
    : username(username_), password(password_) {
    client = new Client();
    client->setGameResolver( new DummyXFireGameResolver() );
    lastInviteRequest = NULL;
	connected = FALSE;
  }

  XFireClient::~XFireClient() {
	if(client!=NULL) client->disconnect();
    if(client!=NULL) delete client;
    if(lastInviteRequest!=NULL) delete lastInviteRequest;
  }

  void XFireClient::run() {
	client->connect(username,password);
    client->addPacketListener(this);
  }

  void XFireClient::Status(string s) {
	  SendStatusMessagePacket *packet = new SendStatusMessagePacket();
	  packet->awaymsg = mir_utf8encode(s.c_str());
	  client->send( packet );
	  delete packet;
  }

  void XFireClient::receivedPacket(XFirePacket *packet) {
	  XFirePacketContent *content = packet->getContent();

	  if(XFIRE_BUDDYS_NAMES_ID)
	  {
			DBVARIANT dbv2;
			if(!DBGetContactSetting(NULL,protocolname,"Nick",&dbv2))
			{
				if(strlen(dbv2.pszVal)!=0)
				{
					myClient->setNick(dbv2.pszVal);
				}
			}
			if (bpStatus == ID_STATUS_AWAY)
			{
				if(myClient!=NULL)
				{
					if(myClient->client->connected)
					{
						myClient->Status("(AFK) Away From Keyboard");
					}
				}
			}
	  }

	  switch(content->getPacketId())
	  {
		  case XFIRE_RECVREMOVEBUDDYPACKET:
		  {
				RecvRemoveBuddyPacket *remove = (RecvRemoveBuddyPacket*)content;
				DBWriteContactSettingByte(remove->handle, protocolname, "DontSendRemovePacket", 1);
				CallService( MS_DB_CONTACT_DELETE, (WPARAM) remove->handle, 1 );
				break;
		  }
		  case XFIRE_BUDDYS_NAMES_ID:
		  {
				vector<BuddyListEntry*> *entries = client->getBuddyList()->getEntries();
				for(uint i = 0 ; i < entries->size() ; i ++) {
					BuddyListEntry *entry = entries->at(i);
					handlingBuddys(entry,0,NULL);
				}
				break;
		  }
		  case XFIRE_CLAN_BUDDYS_NAMES_ID:
		  {
				vector<BuddyListEntry*> *entries = client->getBuddyList()->getEntriesClan();

				char temp[255];
				char * dummy;
				ClanBuddyListNamesPacket *clan = (ClanBuddyListNamesPacket*)content;	
				sprintf(temp,"Clan_%d",clan->clanid);

				DBVARIANT dbv;
				if(!DBGetContactSetting(NULL,protocolname,temp,&dbv))
				{
					dummy=dbv.pszVal;
				}
				else
					dummy=NULL;

				for(uint i = 0 ; i < entries->size() ; i ++) {
					BuddyListEntry *entry = entries->at(i);
					if(entry->clanid==clan->clanid) handlingBuddys(entry,clan->clanid,dummy);
				}
				break;
		  }
		  case XFIRE_BUDDYS_ONLINE_ID:
		  {
			    for(uint i = 0 ; i < ((BuddyListOnlinePacket*)content)->userids->size() ; i++) {
					BuddyListEntry *entry = client->getBuddyList()->getBuddyById( ((BuddyListOnlinePacket*)content)->userids->at(i) );
					if(entry){
						handlingBuddys(entry,0,NULL);
					}
				}
				break;
		  }
		  case XFIRE_RECV_STATUSMESSAGE_PACKET_ID:
		  {
			  for(uint i=0;i<((RecvStatusMessagePacket*)content)->sids->size();i++)
			  {
				  BuddyListEntry *entry = this->client->getBuddyList()->getBuddyBySid( ((RecvStatusMessagePacket*)content)->sids->at(i) );
				  if(entry!=NULL) handlingBuddys(entry,0,NULL);
			  }
			  break;
		  }
		  case XFIRE_BUDDYS_GAMES_ID:
		  {
			  for(uint i=0;i<((BuddyListGamesPacket*)content)->sids->size();i++)
			  {
				  BuddyListEntry *entry = this->client->getBuddyList()->getBuddyBySid( ((BuddyListGamesPacket*)content)->sids->at(i) );
				  if(entry!=NULL) handlingBuddys(entry,0,NULL);
			  }
			  break;
		  }
		  case XFIRE_BUDDYS_GAMES2_ID:
		  {
			  for(uint i=0;i<((BuddyListGames2Packet*)content)->sids->size();i++)
			  {
				  BuddyListEntry *entry = this->client->getBuddyList()->getBuddyBySid( ((BuddyListGames2Packet*)content)->sids->at(i) );
				  if(entry!=NULL) handlingBuddys(entry,0,NULL);
			  }
			  break;
		  }
		  case XFIRE_PACKET_INVITE_REQUEST_PACKET: //freind request
		  {
			  InviteRequestPacket *invite = (InviteRequestPacket*)content;

			  XFireContact xfire_newc;
			  xfire_newc.username=(char*)invite->name.c_str();
			  xfire_newc.nick=(char*)invite->nick.c_str();
			  xfire_newc.id=0;
			  
			  HANDLE handle=CList_AddContact(xfire_newc,TRUE,TRUE,0);

			  if(handle) {  // invite nachricht mitsenden
	    		  string str;
				  CCSDATA ccs;
				  PROTORECVEVENT pre;

				  str=(char*)invite->msg.c_str();
					
				  time_t t = time(NULL);
				  ccs.szProtoService = PSR_MESSAGE;
				  ccs.hContact = handle;
				  ccs.wParam = 0;
				  ccs.lParam = (LPARAM) & pre;
				  pre.flags = 0;
				  pre.timestamp = t;
				  pre.szMessage = (char*)mir_utf8decode((char*)str.c_str(),NULL);
				  pre.lParam = 0;
				  CallService(MS_PROTO_CHAINRECV, 0, (LPARAM) &ccs);
			  }
			  break;
		  }
		case XFIRE_CLAN_PACKET:
		{
			char temp[100];
			XFireClanPacket *clan = (XFireClanPacket*)content;	

			for(int i=0;i<clan->count;i++)
			{
				sprintf(temp,"Clan_%d",clan->clanid[i]);
				DBWriteContactSettingTString(NULL, protocolname, temp, (char*)clan->name[i].c_str());

				sprintf(temp,"ClanUrl_%d",clan->clanid[i]);
				DBWriteContactSettingTString(NULL, protocolname, temp, (char*)clan->url[i].c_str());

				if(!DBGetContactSettingByte(NULL,protocolname,"noclangroups",0))
					CreateGroup((char*)clan->name[i].c_str());
			}
			break;
		}
		case XFIRE_LOGIN_FAILED_ID:
			MSGBOX(Translate("Login failed."));
			SetStatus(ID_STATUS_OFFLINE,NULL);
			break;

		case XFIRE_RECV_OLDVERSION_PACKET_ID:
			MSGBOX(Translate("The protocol version is too old."));
			SetStatus(ID_STATUS_OFFLINE,NULL);
			break;

		case XFIRE_OTHER_LOGIN:
			MSGBOX(Translate("Someone loged in with your account.disconnect."));
			SetStatus(ID_STATUS_OFFLINE,NULL);
			break;

		//ne nachricht für mich, juhu
		case XFIRE_MESSAGE_ID: {
			string str;
			CCSDATA ccs;
			PROTORECVEVENT pre;

			if( (( MessagePacket*)content)->getMessageType() == 0){
				BuddyListEntry *entry = client->getBuddyList()->getBuddyBySid( ((MessagePacket*)content)->getSid() );
				if(entry!=NULL)
				{
					str=((MessagePacket*)content)->getMessage();
					time_t t = time(NULL);
					ccs.szProtoService = PSR_MESSAGE;
					ccs.hContact = entry->hcontact;
					ccs.wParam = 0;
					ccs.lParam = (LPARAM) & pre;
					pre.flags = 0;
					pre.timestamp = t;
					pre.szMessage = (char*)mir_utf8decode((char*)str.c_str(),NULL);
					pre.lParam = 0;
					CallService(MS_PROTO_CHAINRECV, 0, (LPARAM) &ccs);
					CallService(MS_PROTO_CONTACTISTYPING,(WPARAM)ccs.hContact,0);
				}
			}
			else if( (( MessagePacket*)content)->getMessageType() == 3) {
				BuddyListEntry *entry = client->getBuddyList()->getBuddyBySid( ((MessagePacket*)content)->getSid() );
				if(entry!=NULL)
				{
					CallService(MS_PROTO_CONTACTISTYPING,(WPARAM)entry->hcontact,5);
				}
			}

			break;
		}

	    //refresh buddy's
	  /*  if(content->getPacketId()==XFIRE_RECV_STATUSMESSAGE_PACKET_ID||
			content->getPacketId()==XFIRE_BUDDYS_GAMES_ID||
			content->getPacketId()==XFIRE_BUDDYS_GAMES2_ID)
        CallService(MS_CLIST_FRAMES_UPDATEFRAME, (WPARAM)-1, (LPARAM)FU_TBREDRAW | FU_FMREDRAW);*/
	  }

	  //
  }

//=====================================================

/*extern "C" __declspec(dllexport) PLUGININFO*  MirandaPluginInfo(DWORD mirandaVersion)
{
	return &pluginInfo;
}*/

extern "C" __declspec(dllexport) PLUGININFOEX* MirandaPluginInfoEx(DWORD mirandaVersion)
{
	return &pluginInfo;
}

static const MUUID interfaces[] = {MIID_PROTOCOL,MIID_LAST};

extern "C" __declspec(dllexport) const MUUID * MirandaPluginInterfaces(void)
{
	return interfaces;
}

//=====================================================
// Unloads plugin
//=====================================================

extern "C" __declspec(dllexport) int  Unload(void)
{
	pthread_cancel (gamedetection);
	
	pthread_win32_process_detach_np ();

	for(int i=0;i<1024;i++)
		if(icocache[i].hicon!=0) DestroyIcon(icocache[i].hicon);

	DeleteCriticalSection(&modeMsgsMutex);
	//DeleteCriticalSection(&avatarMutex);

	Gdiplus::GdiplusShutdown(gdiplusToken);

	return 0;
}

//=====================================================
// WINAPI DllMain
//=====================================================

BOOL WINAPI DllMain(HINSTANCE hinst,DWORD fdwReason,LPVOID lpvReserved)
{
	hinstance=hinst;
	return TRUE;
}


//wenn alle module geladen sind
static int OnSystemModulesLoaded(WPARAM wParam,LPARAM lParam)
{
	char servicefunction[100];

	HookEvent(ME_USERINFO_INITIALISE, OnDetailsInit);
	HookEvent(ME_DB_CONTACT_DELETED, ContactDeleted);

	strcpy(servicefunction, protocolname);
	strcat(servicefunction, PS_SETAWAYMSG);
	CreateServiceFunction(servicefunction, SetAwayMsg);

	return 0;
}

//=====================================================
// Called when plugin is loaded into Miranda
//=====================================================

extern "C" __declspec(dllexport) int  Load(PLUGINLINK *link)
{
    int status = pthread_win32_process_attach_np();

	InitializeCriticalSection(&modeMsgsMutex);

	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	//void* init = GetProcAddress(LoadLibrary("atl"),"AtlAxWinInit"); _asm call init;

	//InitializeCriticalSection(&avatarMutex);

	XINFO("-----------------------------------------------------\n");

	PROTOCOLDESCRIPTOR pd;
	char servicefunction[100];

	pluginLink=link;

	HookEvent(ME_OPT_INITIALISE, OptInit);
	HookEvent(ME_SYSTEM_MODULESLOADED, OnSystemModulesLoaded);

	ZeroMemory(&pd,sizeof(pd));
	pd.cbSize=sizeof(pd);
	pd.szName=protocolname;
	pd.type=PROTOTYPE_PROTOCOL;
	CallService(MS_PROTO_REGISTERMODULE,0,(LPARAM)&pd);
	hLogEvent=CreateHookableEvent("XFireProtocol/Log");

	mir_getMMI( &mmi );
	mir_getUTFI( &utfi );

	CList_MakeAllOffline();

	strcpy(servicefunction, protocolname);
	strcat(servicefunction, PS_GETCAPS);
	CreateServiceFunction(servicefunction, GetCaps);
	strcpy(servicefunction, protocolname);
	strcat(servicefunction, PS_GETNAME);
	CreateServiceFunction(servicefunction, GetName);
	strcpy(servicefunction, protocolname);
	strcat(servicefunction, PS_LOADICON);
	CreateServiceFunction(servicefunction, TMLoadIcon);
	strcpy(servicefunction, protocolname);
	strcat(servicefunction, PS_SETSTATUS);
	CreateServiceFunction(servicefunction, SetStatus);
	strcpy(servicefunction, protocolname);
	strcat(servicefunction, PS_GETSTATUS);
	CreateServiceFunction(servicefunction, GetStatus);
	strcpy(servicefunction, protocolname);
	strcat(servicefunction, PSS_ADDED);
	CreateServiceFunction(servicefunction, AddtoList);
	strcpy(servicefunction, protocolname);
	strcat(servicefunction, PS_ADDTOLIST);
	CreateServiceFunction(servicefunction, SearchAddtoList);

	strcpy(servicefunction, protocolname);
	strcat(servicefunction, PS_BASICSEARCH);
	CreateServiceFunction(servicefunction, BasicSearch);

	strcpy(servicefunction, protocolname);
	strcat(servicefunction, PSS_MESSAGE);
	CreateServiceFunction( servicefunction,	SendMessage );

	strcpy(servicefunction, protocolname);
	strcat(servicefunction, PSS_USERISTYPING);
	CreateServiceFunction( servicefunction,	UserIsTyping );

	strcpy(servicefunction, protocolname);
	strcat(servicefunction, PSR_MESSAGE);
	CreateServiceFunction( servicefunction,	RecvMessage );

	strcpy(servicefunction, protocolname);
	strcat(servicefunction, PSS_GETAWAYMSG);
	CreateServiceFunction( servicefunction,	GetAwayMsg );

	memset(icocache,0,sizeof(icocache));

	char AvatarsFolder[MAX_PATH]= "";
	CallService(MS_DB_GETPROFILEPATH, (WPARAM) MAX_PATH, (LPARAM)AvatarsFolder);
	strcat(AvatarsFolder, "\\");
	strcat(AvatarsFolder, "XFire");
	XFireWorkingFolder = FoldersRegisterCustomPath(protocolname, "Working Folder", AvatarsFolder);
	strcat(AvatarsFolder, "\\Avatars");
	XFireAvatarFolder = FoldersRegisterCustomPath(protocolname, "Avatars", AvatarsFolder);

	/*
	CreateDirectory("XFire",NULL);
	CreateDirectory("XFire\\Avatars",NULL);
	*/

	//erweiterte Kontextmenüpunkte
	CLISTMENUITEM mi = { 0 };
	memset(&mi,0,sizeof(CLISTMENUITEM));
	mi.cbSize = sizeof( mi );
	mi.pszPopupName = protocolname;

	//gotoprofilemenüpunkt
	strcpy(servicefunction, protocolname);
	strcat(servicefunction, "GotoProfile");
	CreateServiceFunction(servicefunction,GotoProfile);
	mi.pszService = servicefunction;
	mi.position = 500090000;
	mi.pszContactOwner=protocolname;
	mi.hIcon = LoadIcon(hinstance,MAKEINTRESOURCE(ID_OP));
	mi.pszName = LPGEN("&XFire Online Profile");
	CallService( MS_CLIST_ADDCONTACTMENUITEM, 0, (LPARAM)&mi );

	//gotoxfireclansitemenüpunkt
	strcpy(servicefunction, protocolname);
	strcat(servicefunction, "GotoXFireClanSite");
	CreateServiceFunction(servicefunction,GotoXFireClanSite);
	mi.pszService = servicefunction;
	mi.position = 500090000;
	mi.pszContactOwner=protocolname;
	mi.hIcon = LoadIcon(hinstance,MAKEINTRESOURCE(ID_OP));
	mi.pszName = LPGEN("XFire &Clan Site");
	gotoclansite=(HANDLE)CallService( MS_CLIST_ADDCONTACTMENUITEM, 0, (LPARAM)&mi );

	//kopiermenüpunkt
	strcpy(servicefunction, protocolname);
	strcat(servicefunction, "GetIPPort");
	CreateServiceFunction(servicefunction,GetIPPort);
	mi.pszService = servicefunction;
	mi.position = 500090000;
	mi.hIcon = LoadIcon(hinstance,MAKEINTRESOURCE(ID_OP));
	mi.pszContactOwner=protocolname;
	mi.pszName = LPGEN("C&opy Server Address and Port");
	copyipport=(HANDLE)CallService( MS_CLIST_ADDCONTACTMENUITEM, 0, (LPARAM)&mi );

	//kopiermenüpunkt
	strcpy(servicefunction, protocolname);
	strcat(servicefunction, "VoiceIPPort");
	CreateServiceFunction(servicefunction,GetVIPPort);
	mi.pszService = servicefunction;
	mi.position = 500090000;
	mi.hIcon = LoadIcon(hinstance,MAKEINTRESOURCE(ID_OP));
	mi.pszContactOwner=protocolname;
	mi.pszName = LPGEN("Cop&y Voice Server Address and Port");
	vipport=(HANDLE)CallService( MS_CLIST_ADDCONTACTMENUITEM, 0, (LPARAM)&mi );

	//kopiermenüpunkt
	strcpy(servicefunction, protocolname);
	strcat(servicefunction, "JoinGame");
	CreateServiceFunction(servicefunction,JoinGame);
	mi.pszService = servicefunction;
	mi.position = 500090000;
	mi.hIcon = LoadIcon(hinstance,MAKEINTRESOURCE(ID_OP));
	mi.pszContactOwner=protocolname;
	mi.pszName = LPGEN("Join &Game ...");
	joingame=(HANDLE)CallService( MS_CLIST_ADDCONTACTMENUITEM, 0, (LPARAM)&mi );

	//my fire profile
	strcpy(servicefunction, protocolname);
	strcat(servicefunction, "GotoProfile2");
	CreateServiceFunction(servicefunction,GotoProfile2);
	mi.pszService = servicefunction;
	mi.position = 500090000;
	mi.hIcon = LoadIcon(hinstance,MAKEINTRESOURCE(ID_OP));
	mi.pszContactOwner=protocolname;
	mi.pszName = LPGEN("&My XFire Online Profile");
	CallService( MS_CLIST_ADDMAINMENUITEM, 0, (LPARAM)&mi );

	//rescan my games
	strcpy(servicefunction, protocolname);
	strcat(servicefunction, "ReScanMyGames");
	CreateServiceFunction(servicefunction,ReScanMyGames);
	mi.pszService = servicefunction;
	mi.position = 500090000;
	mi.hIcon = LoadIcon(hinstance,MAKEINTRESOURCE(ID_OP));
	mi.pszContactOwner=protocolname;
	mi.pszName = LPGEN("&Rescan my Games ...");
	CallService( MS_CLIST_ADDMAINMENUITEM, 0, (LPARAM)&mi );

	HookEvent( ME_CLIST_PREBUILDCONTACTMENU, RebuildContactMenu );

	//standartwert für icosslot belegn, wenn keins definiert
	if(DBGetContactSettingByte(NULL,protocolname,"gameico",-1)==-1)
	{
		DBWriteContactSettingByte(NULL,protocolname,"gameico",0);
		EXTRA_ICON_ADV1 = 4;
	}
	else
	{
		EXTRA_ICON_ADV1=icoslot[DBGetContactSettingByte(NULL,protocolname,"gameico",-1)];
	}
	
	if(DBGetContactSettingByte(NULL,protocolname,"voiceico",-1)==-1)
	{
		DBWriteContactSettingByte(NULL,protocolname,"voiceico",1);
		EXTRA_ICON_ADV2 = 5;
	}
	else
	{
		EXTRA_ICON_ADV2=icoslot[DBGetContactSettingByte(NULL,protocolname,"voiceico",-1)];
	}

	void* (*func)(void*) = &gamedetectiont;
	pthread_create( &gamedetection, NULL, func , NULL);

	return 0;
}

int RecvMessage(WPARAM wParam, LPARAM lParam)
{
    CCSDATA *ccs = (CCSDATA *) lParam;
    PROTORECVEVENT *pre = (PROTORECVEVENT *) ccs->lParam;

    DBDeleteContactSetting(ccs->hContact, "CList", "Hidden");

	return CallService( MS_PROTO_RECVMSG, wParam, lParam );
}

static void SetMeAFK( LPVOID param )
{
	if(bpStatus==ID_STATUS_ONLINE)
	{
		SetStatus(ID_STATUS_AWAY,(LPARAM)param);
	}
}

static void SendAck( LPVOID param )
{
	ProtoBroadcastAck(protocolname, param, ACKTYPE_MESSAGE, ACKRESULT_SUCCESS, (HANDLE) 1, 0);
}

static void SendBadAck( LPVOID param )
{
	ProtoBroadcastAck(protocolname, param, ACKTYPE_MESSAGE, ACKRESULT_FAILED, (HANDLE) 0, LPARAM(Translate("XFire does not support offline messaging!")));
}

static int UserIsTyping(WPARAM wParam, LPARAM lParam)
{
	HANDLE hContact = ( HANDLE )wParam;
    DBVARIANT dbv;

	if(lParam==PROTOTYPE_SELFTYPING_ON)
	{
		if(myClient!=NULL)
			if(myClient->client->connected)
				if(!DBGetContactSettingTString(hContact, protocolname, "Username",&dbv))
				{		
					SendTypingPacket typing;
					typing.init(myClient->client, dbv.pszVal);
					myClient->client->send( &typing ); 
					DBFreeVariant(&dbv);
				}
	}
	else if(lParam==PROTOTYPE_SELFTYPING_OFF)
	{
	}

	return 0;
}

int SendMessage(WPARAM wParam, LPARAM lParam)
{
    CCSDATA *ccs = (CCSDATA *) lParam;
	PROTORECVEVENT* pre = (PROTORECVEVENT*)ccs->lParam;
	ACKDATA * ack = (ACKDATA*) lParam;
    DBVARIANT dbv;
	int sended=0;

	DBGetContactSettingTString(ccs->hContact, protocolname, "Username",&dbv);
	if(myClient!=NULL)
		if(myClient->client->connected&&DBGetContactSettingWord(ccs->hContact, protocolname, "Status", -1)!=ID_STATUS_OFFLINE)
		{
			myClient->sendmsg(dbv.pszVal, mir_utf8encode(( char* )ccs->lParam));

			mir_forkthread(SendAck,ccs->hContact);
			sended=1;
		}
		else
		{
			mir_forkthread(SendBadAck,ccs->hContact);
		}
	DBFreeVariant(&dbv);

    return sended;
}

//=======================================================
//GetCaps
//=======================================================

int GetCaps(WPARAM wParam,LPARAM lParam)
{
	if(wParam==PFLAGNUM_1)
		return PF1_BASICSEARCH|PF1_MODEMSG|PF1_IM;
	else if(wParam==PFLAGNUM_2)
		return PF2_ONLINE|PF2_SHORTAWAY; // add the possible statuses here.
	else if(wParam==PFLAGNUM_3)
		return PF2_ONLINE;
	else if(wParam==PFLAGNUM_4)
		return PF4_SUPPORTTYPING|PF4_AVATARS;
	else if(wParam==PFLAG_UNIQUEIDTEXT)
		return (int) Translate("Username");
	else if(wParam==PFLAG_UNIQUEIDSETTING)
		return (int)"Username";
	else if(wParam==PFLAG_MAXLENOFMESSAGE)
		return 3996; //255;
	return 0;
}

//=======================================================
//GetName (tray icon)
//=======================================================
int GetName(WPARAM wParam,LPARAM lParam)
{
	lstrcpyn((char*)lParam,"XFire",wParam);
	return 0;
}

//=======================================================
//TMLoadIcon
//=======================================================
int TMLoadIcon(WPARAM wParam,LPARAM lParam)
{
	UINT id;

	switch(wParam & 0xFFFF) {
		case PLI_PROTOCOL: id=IDI_TM; break; // IDI_TM is the main icon for the protocol
		case PLI_ONLINE: id=IDI_TM; break; // IDI_TM is the main icon for the protocol
		case PLI_OFFLINE: id=IDI_TM; break; // IDI_TM is the main icon for the protocol
		default: return 0;	
	}
	return (int)LoadImage(hinstance, MAKEINTRESOURCE(id), IMAGE_ICON, GetSystemMetrics(wParam&PLIF_SMALL?SM_CXSMICON:SM_CXICON), GetSystemMetrics(wParam&PLIF_SMALL?SM_CYSMICON:SM_CYICON), 0);
}

//=======================================================
//SetStatus
//=======================================================
int SetStatus(WPARAM wParam,LPARAM lParam)
{
	int oldStatus;

	oldStatus = bpStatus;

	if(wParam!=ID_STATUS_ONLINE&&wParam!=ID_STATUS_OFFLINE&&wParam!=ID_STATUS_AWAY)
		if(DBGetContactSettingByte(NULL,protocolname,"oninsteadafk",0)==0)
			wParam=ID_STATUS_AWAY; //protokoll auf away schalten
		else
			wParam=ID_STATUS_ONLINE; //protokoll auf online schalten

	if (
		(wParam == ID_STATUS_ONLINE && bpStatus!=ID_STATUS_ONLINE) || // offline --> online
		(wParam == ID_STATUS_AWAY && bpStatus==ID_STATUS_OFFLINE) // offline --> away
		)
	{
		if(bpStatus == ID_STATUS_AWAY) // away --> online
		{
			myClient->Status("");
		}
		else
		{
			// the status has been changed to online (maybe run some more code)
			DBVARIANT dbv;
			DBVARIANT dbv2;	

			if(DBGetContactSetting(NULL,protocolname,"login",&dbv))
			{
				MSGBOX(Translate("No Loginname is set!"));
				wParam=ID_STATUS_OFFLINE;
			}
			else if(DBGetContactSetting(NULL,protocolname,"password",&dbv2))
			{
				MSGBOX(Translate("No Password is set!"));
				wParam=ID_STATUS_OFFLINE;
			}
			else
			{
				CallService(MS_DB_CRYPT_DECODESTRING,strlen(dbv2.pszVal)+1,(LPARAM)dbv2.pszVal);
				
				if(myClient!=NULL)
					delete myClient;

				myClient = new XFireClient(dbv.pszVal,dbv2.pszVal);
				myClient->run();

				if(myClient->client->connected)
				{
					DBFreeVariant(&dbv2);
					if(!DBGetContactSetting(NULL,protocolname,"Nick",&dbv2))
					{
						if(strlen(dbv2.pszVal)!=0)
						{
							//nothing
						}
						else
						{
							DBWriteContactSettingString(NULL,protocolname,"Nick",dbv.pszVal);
						}
					}
					else
					{
						DBWriteContactSettingString(NULL,protocolname,"Nick",dbv.pszVal);
					}
				}
				else
				{
					MSGBOX(Translate("Unable to connect to XFire."));
					wParam =ID_STATUS_OFFLINE;
				}

				DBFreeVariant(&dbv);
				DBFreeVariant(&dbv2);
			}
		}
	}
	else if (wParam == ID_STATUS_AWAY && bpStatus!=ID_STATUS_AWAY)
	{
		if(bpStatus == ID_STATUS_OFFLINE) // nix
		{
		} 
		else if(myClient!=NULL&&myClient->client->connected) // online --> afk
		{
			myClient->Status("(AFK) Away From Keyboard");
		}
	}
	else if (wParam == ID_STATUS_OFFLINE && bpStatus!=ID_STATUS_OFFLINE) // * --> offline
	{
		// the status has been changed to offline (maybe run some more code)
		if(myClient!=NULL)
			if(myClient->client->connected)
				myClient->client->disconnect();
		CList_MakeAllOffline();
	}
	else
	{
		// the status has been changed to unknown  (maybe run some more code)
	}
	//broadcast the message
	bpStatus = wParam;
	ProtoBroadcastAck(protocolname,NULL,ACKTYPE_STATUS,ACKRESULT_SUCCESS,(HANDLE)oldStatus,wParam);

	
	return 0;
}

//=======================================================
//GetStatus
//=======================================================
int GetStatus(WPARAM wParam,LPARAM lParam)
{
	if (bpStatus == ID_STATUS_ONLINE)
		return ID_STATUS_ONLINE;
	else if (bpStatus == ID_STATUS_AWAY)
		return ID_STATUS_AWAY;
	else
		return ID_STATUS_OFFLINE;
}

HANDLE CList_AddContact(XFireContact xfc, bool InList, bool SetOnline,int clan)
{
	HANDLE hContact; 

	if (xfc.username == NULL)
		return 0;
	
	// here we create a new one since no one is to be found
	hContact = (HANDLE) CallService( MS_DB_CONTACT_ADD, 0, 0);
	if ( hContact ) {
		CallService( MS_PROTO_ADDTOCONTACT, (WPARAM) hContact, (LPARAM)protocolname );

		if ( InList )
			DBDeleteContactSetting(hContact, "CList", "NotOnList");
		else
			DBWriteContactSettingByte(hContact, "CList", "NotOnList", 1);
		DBDeleteContactSetting(hContact, "CList", "Hidden");

		if(strlen(xfc.nick)>0)
			DBWriteContactSettingTString(hContact, protocolname, "Nick", mir_utf8decode(( char* )xfc.nick,NULL));
		else if(strlen(xfc.username)>0)
			DBWriteContactSettingTString(hContact, protocolname, "Nick", xfc.username);

		DBWriteContactSettingTString(hContact, protocolname, "Username", xfc.username);
		
		//DBWriteContactSettingTString(hContact, protocolname, "Screenname", xfc.nick);
		DBWriteContactSettingDword(hContact, protocolname, "UserId", xfc.id);

		if(clan>0)
			DBWriteContactSettingDword(hContact, protocolname, "Clan", clan);
		
		DBWriteContactSettingWord(hContact, protocolname, "Status", SetOnline ? ID_STATUS_ONLINE:ID_STATUS_OFFLINE);

		XFire_SetAvatar* xsa=new XFire_SetAvatar;
		xsa->hContact=hContact;
		xsa->username=new char[strlen(xfc.username)+1];
		strcpy(xsa->username,xfc.username);

		mir_forkthread(SetAvatar,(LPVOID)xsa);

		if (xfc.id==0) {
			DBWriteContactSettingByte( hContact, "CList", "NotOnList", 1 );
			DBWriteContactSettingByte( hContact, "CList", "Hidden", 1 );
		}	

		return hContact;
	}
	return false;
}

BOOL IsXFireContact(HANDLE hContact)
{
	char *szProto;
	szProto = ( char* ) CallService( MS_PROTO_GETCONTACTBASEPROTO, (WPARAM) hContact, 0);
	if ( szProto != NULL && !lstrcmpiA( szProto, protocolname )) {
		return TRUE;
	}
	else
		return FALSE;
}

HANDLE CList_FindContact (int uid)
{
	char *szProto;

	HANDLE hContact = (HANDLE) CallService( MS_DB_CONTACT_FINDFIRST, 0, 0);
	while (hContact) {
		szProto = ( char* ) CallService( MS_PROTO_GETCONTACTBASEPROTO, (WPARAM) hContact, 0);
		if ( szProto != NULL && !lstrcmpiA( szProto, protocolname )) {
				if ( DBGetContactSettingDword(hContact, protocolname, "UserId",-1)==uid)
				{
					return (HANDLE)hContact;
				}
		}		
		hContact = (HANDLE) CallService( MS_DB_CONTACT_FINDNEXT, (WPARAM) hContact, 0);
	}
	return 0;
}

void CList_MakeAllOffline()
{
	char *szProto;
	HANDLE hContact = (HANDLE) CallService( MS_DB_CONTACT_FINDFIRST, 0, 0);
	while (hContact) {
		szProto = ( char* ) CallService( MS_PROTO_GETCONTACTBASEPROTO, (WPARAM) hContact, 0);
		if ( szProto != NULL && !lstrcmpiA( szProto, protocolname )) {
			DBDeleteContactSetting(hContact, protocolname, "XStatusMsg");
			DBDeleteContactSetting(hContact, protocolname, "ServerIP");
			DBDeleteContactSetting(hContact, protocolname, "Port");

			if(DBGetContactSettingByte(NULL,protocolname,"noavatars",-1)==1)
			{
				DBDeleteContactSetting(hContact, "ContactPhoto", "File");
				DBDeleteContactSetting(hContact, "ContactPhoto", "RFile");
				DBDeleteContactSetting(hContact, "ContactPhoto", "Backup");
				DBDeleteContactSetting(hContact, "ContactPhoto", "Format");
				DBDeleteContactSetting(hContact, "ContactPhoto", "ImageHash");
			}
			else
			{
				//prüf ob der avatar noch existiert
				DBVARIANT dbv;
				if(!DBGetContactSettingTString(hContact, "ContactPhoto", "File",&dbv))
				{
					FILE*f=fopen(dbv.pszVal,"r");
					if(f==NULL)
					{
						DBDeleteContactSetting(hContact, "ContactPhoto", "File");
						DBDeleteContactSetting(hContact, "ContactPhoto", "RFile");
						DBDeleteContactSetting(hContact, "ContactPhoto", "Backup");
						DBDeleteContactSetting(hContact, "ContactPhoto", "Format");
						DBDeleteContactSetting(hContact, "ContactPhoto", "ImageHash");
					}
					else
					{
						fclose(f);
					}
					DBFreeVariant(&dbv);
				}
			}
			DBWriteContactSettingWord(hContact,protocolname,"Status",ID_STATUS_OFFLINE);
		}		
		hContact = (HANDLE) CallService( MS_DB_CONTACT_FINDNEXT, (WPARAM) hContact, 0);
	}
}

string GetGame(int id,int id2, HANDLE* hicon, HICON* ico, BOOL onlyico)
{
	string common;
	char ret[XFIRE_MAX_STATIC_STRING_LEN];
	char ret2[XFIRE_MAX_STATIC_STRING_LEN];
	char temp[XFIRE_MAX_STATIC_STRING_LEN];
	char path[XFIRE_MAX_STATIC_STRING_LEN]="";

	FoldersGetCustomPath( XFireWorkingFolder, path, 1024, 'W' );
	strcat(path,"\\");
	strcat(path,"xfire_games.ini");

	if(id2>0)
	{
		sprintf(temp,"%d_%d",id,id2);
		common=temp;
		GetPrivateProfileString (common.c_str(), "LongName", "", ret, 255, path);

		GetPrivateProfileString (common.c_str(), "ShortName", "", ret2, 255, path);

		if(strlen(ret)==0)
		{
			sprintf(temp,"%d",id);
			common=temp;
			GetPrivateProfileString (common.c_str(), "LongName", "", ret, 255, path);

			GetPrivateProfileString (common.c_str(), "ShortName", "", ret2, 255, path);
		}
		
		*hicon=LoadGameIcon(ret2,id,ico,onlyico);
	}
	else
	{
		sprintf(temp,"%d",id);
		common=temp;
		GetPrivateProfileString (common.c_str(), "LongName", "", ret, 255, path);

		GetPrivateProfileString (common.c_str(), "ShortName", "", ret2, 255, path);

		if(strlen(ret)==0)
		{
			sprintf(temp,"%d_1",id);
			common=temp;
			GetPrivateProfileString (common.c_str(), "LongName", "", ret, 255, path);

			GetPrivateProfileString (common.c_str(), "ShortName", "", ret2, 255, path);
		}

		*hicon=LoadGameIcon(ret2,id,ico,onlyico);
	}

	if(strlen(ret)>0)
	{
		sprintf(temp,"%s",ret);
	}

	return temp;
}

HANDLE LoadGameIcon(char* g, int id, HICON* ico,BOOL onlyico)
{
	char path[XFIRE_MAX_STATIC_STRING_LEN]="";
	char tmpfile[XFIRE_MAX_STATIC_STRING_LEN]="";
	char resname[XFIRE_MAX_STATIC_STRING_LEN]="XF_";
	int u;

	if(!onlyico)
	for(u=0;u<1024;u++)
	{
		if(icocache[u].gameid!=0)
		{
			if(icocache[u].gameid==id)
			{
				if(ico!=NULL)
					*ico=icocache[u].hicon;

				return icocache[u].handle;
			}
		}
		else
			break;
	}

	FoldersGetCustomPath( XFireWorkingFolder, path, 256, 'W' );
	strcat(path,"\\");

	//uppercase
	for(unsigned int i=0;i<strlen(g);i++)
		g[i]=toupper(g[i]);

	strcat(resname,g);
	strcat(resname,".ICO");
	
	
	strcpy(tmpfile,path);
	strcat(tmpfile,"temp.ico");
	
	strcat(path,"Icons.dll");

	HINSTANCE hDll = LoadLibrary(path);

	if(hDll==NULL)
		return (HANDLE)-1;

	HRSRC hrsrc = FindResource(hDll,resname,"ICONS");
	HGLOBAL hglobal=LoadResource(hDll,hrsrc);
	LPVOID lpResource = LockResource(hglobal); 

	FILE* f=fopen(tmpfile,"wb");

	if(f==NULL)
	{
		FreeLibrary(hDll);
		return (HANDLE)-1;
	}

	fwrite((char*)lpResource,SizeofResource(hDll, hrsrc),1,f);
	fclose(f);
	FreeLibrary(hDll);

	HICON hicon=(HICON)LoadImage( NULL, tmpfile, IMAGE_ICON, 0, 0, LR_LOADFROMFILE);

	if(onlyico)
	{
		*ico=hicon;
		return (HANDLE)-1;
	}

	HANDLE hand=(HANDLE)CallService(MS_CLIST_EXTRA_ADD_ICON, (WPARAM)hicon, 0);

	icocache[u].gameid=id;
	icocache[u].handle=hand;
	icocache[u].hicon=hicon;

	//DestroyIcon(hicon);
	if(ico!=NULL)
		*ico=icocache[u].hicon;

	return hand;
}

void SetIcon(HANDLE hcontact,HANDLE hicon,int ctype)
{
	IconExtraColumn iec;
	iec.cbSize = sizeof(iec);
	iec.hImage = hicon;
	iec.ColumnType = ctype;
	CallService(MS_CLIST_EXTRA_SET_ICON, (WPARAM)hcontact, (LPARAM)&iec);
}

void SetAvatar(LPVOID lparam)
//void SetAvatar(HANDLE hContact, char* username)
{
	//EnterCriticalSection(&avatarMutex);
	XFireAvatar av;

	XFire_SetAvatar* xsa=(XFire_SetAvatar*)lparam;

	if(xsa->hContact==NULL)
		return;

	if(DBGetContactSettingByte(NULL,protocolname,"noavatars",-1)==0)
		if(GetAvatar(xsa->username,&av))
		{
			DBWriteContactSettingTString(xsa->hContact, "ContactPhoto", "Backup", av.backup);
			DBWriteContactSettingTString(xsa->hContact, "ContactPhoto", "File", av.file);
			DBWriteContactSettingTString(xsa->hContact, "ContactPhoto", "RFile", av.rfile);
			DBWriteContactSettingWord(xsa->hContact, "ContactPhoto", "Format", av.type);
		}

	delete(xsa);
	//LeaveCriticalSection(&avatarMutex);
}

BOOL GetAvatar(char* username,XFireAvatar* av)
{	DWORD dwBytesRead = 0;
	DWORD dwFileSize = 0;
	char buffer[20*XFIRE_MAX_STATIC_STRING_LEN]="";
	char path[XFIRE_MAX_STATIC_STRING_LEN]="";
	char address[XFIRE_MAX_STATIC_STRING_LEN]="http://de.xfire.com/profile/";
	char bufQuery[XFIRE_MAX_STATIC_STRING_LEN]="";
	BOOL bRead;
	DWORD dwIndex;
	char* filetype;
	string avatarlink;
	char findone[]="class=\"profile_avatar\"";
	BOOL std=FALSE;
	
	dwIndex=0;

	if(strlen(username)==0)
		return FALSE;

	strcat(address,username);
	strcat(address,"/");

	//verbindung zur xfireseite herrstellen
	HINTERNET hNet = InternetOpen("XFire",PRE_CONFIG_INTERNET_ACCESS,	NULL,INTERNET_INVALID_PORT_NUMBER,0);
	if(hNet == NULL)
		return FALSE;

	//profilseite laden
	HINTERNET hUrlFile = InternetOpenUrl(hNet,address,NULL,0,INTERNET_FLAG_RELOAD,0);
	if(hUrlFile == NULL)
	{
		InternetCloseHandle(hNet);
		return FALSE;
	}

	//in buffer laden
	bRead = InternetReadFile(hUrlFile,buffer,sizeof(buffer)-1, &dwBytesRead);
	if(!bRead)
	{
		InternetCloseHandle(hUrlFile);
		InternetCloseHandle(hNet);
		return FALSE;
	}
	InternetCloseHandle(hUrlFile);
	buffer[dwBytesRead-1]=0;

	/*****************/
	/*harcoded parser*/
	/*****************/
	char* pos=buffer;
	char* fi=findone;
	BOOL weiter=TRUE;
	while(weiter)
	{
		if(*pos==*fi)
		{
			fi++;
		}
		else
			fi=findone;
		pos++;

		if(*pos==0)
			weiter=FALSE;		
		if(*fi==0)
			weiter=FALSE;
	}
	if(fi==findone)
	{
		InternetCloseHandle(hNet);
		return FALSE;
	}

	while(*pos!='\'') pos++;
	pos++;
	fi=pos;
	while(*fi!='\'') fi++;
	*fi=0;

	//eventuell prameter sign am ende der url entfernen
	filetype=strrchr(pos,'?');
	if(filetype!=0)
		*filetype=0;

	/*****************/
	/*****************/
	/*****************/

	//BUG: stringklasse craschte hier, weis nicht warum
	avatarlink = pos;

	//avatarbild laden
	hUrlFile = InternetOpenUrl(hNet,avatarlink.c_str(),NULL,0,INTERNET_FLAG_RELOAD,0);
	if(hUrlFile == NULL)
	{
		InternetCloseHandle(hNet);
		return FALSE;
	}

	//überprüf, obs das xfire stdicon is
	if(avatarlink.size()>10)
	{
		//prüf die letzten stellen
		//
		// /xfire.gif

		char * avav=(char*)avatarlink.c_str();
		avav+=avatarlink.size();
		avav-=10;

		//prüf xfirebild auf stdbild
		if(*avav==		'/'&&
		   *(avav+1)==	'x'&&
		   *(avav+2)==	'f'&&
		   *(avav+3)==	'i'&&
		   *(avav+4)==	'r'&&
		   *(avav+5)==	'e'&&
		   *(avav+6)==	'.'&&
		   *(avav+7)==	'g'&&
		   *(avav+8)==	'i'&&
		   *(avav+9)==	'f')
		{
			strcpy(buffer,"xfire");
			std=TRUE;
		}
		else
			strcpy(buffer,username);
	}
	else
		strcpy(buffer,username);

	//dateityp festhalten
	avatarlink=avatarlink.substr(avatarlink.rfind("."),avatarlink.size());
	
	strcat(buffer,avatarlink.c_str());


	/*GetModuleFileNameA( NULL, path, sizeof( path ));
	filetype=strrchr(path,'\\');
	*filetype=0;

	strcat(path,"\\XFire\\Avatars\\");
	strcat(path,buffer);*/
	FoldersGetCustomPath( XFireAvatarFolder, path, 1024, 'A' );
	strcat(path,"\\");
	strcat(path,buffer);


	//bild ist standartbild , guggen ob schone xistiert
	if(std==TRUE)
	{
		if(GetFileAttributes(path)!=0xFFFFFFFF)
		{
		}
		else
		{
			std=FALSE;
		}
	}
	
	
	
	if(std==FALSE)
	{
		//avatar speichern
		FILE* f=fopen(path,"wb");
		if(f==NULL)
		{
			InternetCloseHandle(hUrlFile);
			InternetCloseHandle(hNet);
			return FALSE;
		}

		//bild empfangen
		do
		{
			bRead = InternetReadFile(hUrlFile,bufQuery,512,&dwBytesRead);
			fwrite(bufQuery,dwBytesRead,1,f);
		}
		while(dwBytesRead);
		fclose(f);

		InternetCloseHandle(hUrlFile);
		InternetCloseHandle(hNet);
	}


	//avatarstruct füllen
	avatarlink=avatarlink.substr(1,avatarlink.size());
	filetype=(char*)avatarlink.c_str();

	if(	
		(filetype[0]=='B'||filetype[0]=='b') &&
		(filetype[1]=='M'||filetype[1]=='m') &&
		(filetype[2]=='P'||filetype[2]=='p')
		)
		av->type=PA_FORMAT_BMP;
	else if(
		(filetype[0]=='G'||filetype[0]=='g') &&
		(filetype[1]=='I'||filetype[1]=='i') &&
		(filetype[2]=='F'||filetype[2]=='f')
		)
		av->type=PA_FORMAT_GIF;
	else if(
		(filetype[0]=='P'||filetype[0]=='p') &&
		(filetype[1]=='N'||filetype[1]=='n') &&
		(filetype[2]=='G'||filetype[2]=='g')
		)
		av->type=PA_FORMAT_PNG;
	else
		av->type=PA_FORMAT_JPEG;

	strcpy(av->file,path);

	FoldersGetCustomPath( XFireAvatarFolder, path, 1024, 'A' );
	strcat(path,"\\");
	strcat(path,buffer);

	//strcpy(path,"XFire\\Avatars\\");
	//strcat(path,buffer);
	strcpy(av->rfile,path);
	strcpy(av->backup,path);

	return TRUE;
}

static int GetIPPort(WPARAM wParam,LPARAM lParam)
{
	char temp[XFIRE_MAX_STATIC_STRING_LEN];
    HGLOBAL clipbuffer;
	char* buffer;

	if(DBGetContactSettingWord((HANDLE)wParam, protocolname, "Port", -1)==0)
		return 0;

	DBVARIANT dbv;
	if(DBGetContactSettingTString((HANDLE)wParam, protocolname, "ServerIP",&dbv))
		return 0;

	sprintf(temp,"%s:%d",dbv.pszVal,DBGetContactSettingWord((HANDLE)wParam, protocolname, "Port", -1));

	DBFreeVariant(&dbv);

	if(OpenClipboard(NULL))
	{
		EmptyClipboard();

		clipbuffer = GlobalAlloc(GMEM_DDESHARE, strlen(temp)+1);
		buffer = (char*)GlobalLock(clipbuffer);
		strcpy(buffer, LPCSTR(temp));
		GlobalUnlock(clipbuffer);

		SetClipboardData(CF_TEXT, clipbuffer);
		CloseClipboard();
	}

	return 0;
}

static int GetVIPPort(WPARAM wParam,LPARAM lParam)
{
	char temp[XFIRE_MAX_STATIC_STRING_LEN];
    HGLOBAL clipbuffer;
	char* buffer;

	if(DBGetContactSettingWord((HANDLE)wParam, protocolname, "VPort", -1)==0)
		return 0;

	DBVARIANT dbv;
	if(DBGetContactSettingTString((HANDLE)wParam, protocolname, "VServerIP",&dbv))
		return 0;

	sprintf(temp,"%s:%d",dbv.pszVal,DBGetContactSettingWord((HANDLE)wParam, protocolname, "VPort", -1));

	DBFreeVariant(&dbv);

	if(OpenClipboard(NULL))
	{
		EmptyClipboard();

		clipbuffer = GlobalAlloc(GMEM_DDESHARE, strlen(temp)+1);
		buffer = (char*)GlobalLock(clipbuffer);
		strcpy(buffer, LPCSTR(temp));
		GlobalUnlock(clipbuffer);

		SetClipboardData(CF_TEXT, clipbuffer);
		CloseClipboard();
	}

	return 0;
}

static int GotoProfile(WPARAM wParam,LPARAM lParam)
{
	DBVARIANT dbv;
	char temp[XFIRE_MAX_STATIC_STRING_LEN]="";

	if(DBGetContactSettingTString((HANDLE)wParam, protocolname, "Username",&dbv))
		return 0;

	strcpy(temp,"http://xfire.com/profile/");
	strcat(temp,dbv.pszVal);
	DBFreeVariant(&dbv);

	CallService( MS_UTILS_OPENURL, 1, (LPARAM)temp );

	return 0;
}

static int GotoXFireClanSite(WPARAM wParam,LPARAM lParam) {
	DBVARIANT dbv;
	char temp[XFIRE_MAX_STATIC_STRING_LEN]="";

	int clanid=DBGetContactSettingDword((HANDLE)wParam, protocolname, "Clan",-1);
	sprintf(temp,"ClanUrl_%d",clanid);

	if(DBGetContactSettingTString(NULL, protocolname, temp,&dbv))
		return 0;

	strcpy(temp,"http://xfire.com/clans/");
	strcat(temp,dbv.pszVal);
	DBFreeVariant(&dbv);

	CallService( MS_UTILS_OPENURL, 1, (LPARAM)temp );

	return 0;
}

static int GotoProfile2(WPARAM wParam,LPARAM lParam)
{
	DBVARIANT dbv;
	char temp[XFIRE_MAX_STATIC_STRING_LEN]="";

	if(DBGetContactSettingTString(NULL, protocolname, "login",&dbv))
		return 0;

	strcpy(temp,"http://xfire.com/profile/");
	strcat(temp,dbv.pszVal);
	DBFreeVariant(&dbv);

	CallService( MS_UTILS_OPENURL, 1, (LPARAM)temp );

	return 0;
}

int RebuildContactMenu( WPARAM wParam, LPARAM lParam )
{
	CLISTMENUITEM clmi = { 0 };
	clmi.cbSize = sizeof( clmi );
	CLISTMENUITEM clmi2 = { 0 };
	clmi2.cbSize = sizeof( clmi2 );
	CLISTMENUITEM clmi3 = { 0 };
	clmi3.cbSize = sizeof( clmi3 );
	CLISTMENUITEM clmi4 = { 0 };
	clmi4.cbSize = sizeof( clmi4 );

	//kopieren von port und ip nur erlauben, wenn verfügbar
	clmi.flags = CMIM_FLAGS;
	clmi2.flags = CMIM_FLAGS;
	clmi3.flags = CMIM_FLAGS;
	clmi4.flags = CMIM_FLAGS;

	DBVARIANT dbv;
	if(DBGetContactSettingTString((HANDLE)wParam, protocolname, "ServerIP",&dbv))
		clmi.flags|= CMIF_HIDDEN;
	DBFreeVariant(&dbv);

	CallService( MS_CLIST_MODIFYMENUITEM, ( WPARAM )copyipport, ( LPARAM )&clmi );

	//kopieren von voice port und ip nur erlauben, wenn verfügbar
	DBVARIANT dbv2;
	if(DBGetContactSettingTString((HANDLE)wParam, protocolname, "VServerIP",&dbv2))
	{
		clmi2.flags|= CMIF_HIDDEN;
	}
	DBFreeVariant(&dbv2);

	CallService( MS_CLIST_MODIFYMENUITEM, ( WPARAM )vipport, ( LPARAM )&clmi2 ); 

	//clansite nur bei clanmembern anbieten
	if(DBGetContactSettingDword((HANDLE)wParam, protocolname, "Clan",0)==0)
		clmi3.flags|= CMIF_HIDDEN;

	CallService( MS_CLIST_MODIFYMENUITEM, ( WPARAM )gotoclansite, ( LPARAM )&clmi3 );


	//join nur anbieten, wenn 1. buddy ein installiertes game spielt, 2. eine ip/port hinterlegt ist
	if(DBGetContactSettingDword((HANDLE)wParam, protocolname, "Port",0)!=0)
	{
		BOOL found=FALSE;
		for(int i=0;i<(int)foundgames;i++)
		{
			if(xf[i].gameid==DBGetContactSettingWord((HANDLE)wParam, protocolname, "GameId",0))
			{
				char temp[128];
				found=TRUE;

				//wenn kein networkstring, dann kein join anbieten, spiel ohne nw unterstützung
				sprintf(temp,"gamenetargs_%d",i);

				if(!DBGetContactSettingTString(NULL, protocolname, temp ,&dbv2))
				{
					if(strlen(dbv2.pszVal)==0) {
						found=FALSE;
					}
					DBFreeVariant(&dbv2);
				}
				else
					found=FALSE;

				break;
			}
		}
		if(found==FALSE)
			clmi4.flags|= CMIF_HIDDEN;
	}
	else
		clmi4.flags|= CMIF_HIDDEN;

	CallService( MS_CLIST_MODIFYMENUITEM, ( WPARAM )joingame, ( LPARAM )&clmi4 );


	return 0;
}

//versucht die * in den pfaden der ini mit dem korrekten eintrag zu ersetzen
BOOL CheckPath(char*ppath)
{
	char* pos=0;
	char* pos2=0;

	pos = strchr(ppath,'*');
	if(pos)
	{	
		HANDLE fHandle;
		WIN32_FIND_DATA wfd; 
		BOOL weiter=TRUE;

		pos++;
		*pos=0;
		pos++;
	
		//versuch die exe zu finden
		fHandle=FindFirstFile(ppath,&wfd);  // . skippen
		FindNextFile(fHandle,&wfd); // .. auch skippen

		while ((FindNextFile(fHandle,&wfd)&&weiter==TRUE)) // erstes file
		{
			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) // nur verzeichnisse sind interessant
			{
				char temp[XFIRE_MAX_STATIC_STRING_LEN];

				strcpy(temp,ppath);
				*(temp+strlen(temp)-1)=0;
				strcat(temp,wfd.cFileName);
				strcat(temp,"\\");
				strcat(temp,pos);

				if(GetFileAttributes(temp)!=0xFFFFFFFF) { //exe vorhanden???? unt hint?
					//gefundenes in path kopieren
					FindClose(fHandle);
					strcpy(ppath,temp);
					return TRUE;
				}
			}
		}
		FindClose(fHandle);
	}
	else
	{
		if(GetFileAttributes(ppath)!=0xFFFFFFFF) { //exe vorhanden???? unt hint?
		//gefundenes in path kopieren
			return TRUE;
		}
	}

	return FALSE;
}


//eigene string replace funktion, da die von der std:string klasse immer abstürzt
BOOL str_replace(char*src,char*find,char*rep)
{
	string strpath = src;
	int pos = strpath.find(find);
	if(pos>-1)
	{
		char *temp=new char[strlen(src)+strlen(rep)];

		strcpy(temp,src);
		*(temp+pos)=0;

		strcat(temp,rep);
		strcat(temp,(src+pos+strlen(find)));
		strcpy(src,temp);

		delete temp;

		return TRUE;
	}
	return FALSE;
}

static void Scan4Games( LPVOID lparam  )
{
	unsigned int i=2;
	unsigned int i2=1;
	BOOL split=FALSE;
	int notfound=0;
	char* pos=0;
	char* pos2=0;
	char *cutforlaunch = 0;
	char temp[10]="";
	char inipath[XFIRE_MAX_STATIC_STRING_LEN]="";
	char ret[XFIRE_MAX_STATIC_STRING_LEN]="";
	char gamename[XFIRE_MAX_STATIC_STRING_LEN]="";
	char ret2[XFIRE_MAX_STATIC_STRING_LEN]="";
	char gamelist[XFIRE_MAX_STATIC_STRING_LEN]="";
	char servicefunction[100];
	
	CLISTMENUITEM mi = { 0 };


	DWORD gpps=1;
	DWORD last_gpps=0;

	//WaitForSingleObject(ghMutex, INFINITE);

	//menuvorbereitungen
	memset(&mi,0,sizeof(CLISTMENUITEM));
	strcpy(servicefunction, protocolname);
	strcat(servicefunction, "StartGame%d");
	mi.popupPosition = 500084000;
	mi.pszPopupName = LPGEN("Start game");
	mi.pszContactOwner=protocolname;
	mi.cbSize = sizeof( mi );

	mi.position = 500090001;
	mi.hIcon = NULL;

	//prüfe ob schon gescannt wurde, dann das aus der db nehmen, beschleunigt den start
	foundgames=DBGetContactSettingWord(NULL, protocolname, "foundgames",-1);

	//um bei einer neuen version einen rescan zuforcen, bei bestimmten wert found auf 0 resetten
	if(DBGetContactSettingWord(NULL, protocolname, "scanver",0)!=XFIRE_SCAN_VAL)
		foundgames=-1;

	if(!DBGetContactSettingByte(NULL,protocolname,"scanalways",0))
		if(foundgames>0)
		{
			foundgames=foundgames%255;
			//daten aus der db nehmen
			DBVARIANT dbv;

			for(int i=0;i<foundgames;i++)
			{
				memset(&xf[i],0,sizeof(XFire_FoundGame));
				
				sprintf(ret2,"gameid_%i",i);
				xf[i].gameid=DBGetContactSettingWord(NULL, protocolname, ret2, -1);

				sprintf(ret2,"gamepath_%i",i);
				if(!DBGetContactSettingTString(NULL, protocolname, ret2,&dbv))
				{
					strcpy(xf[i].path,dbv.pszVal);
					DBFreeVariant(&dbv);
				}

				sprintf(ret2,"gamelaunch_%i",i);
				if(!DBGetContactSettingTString(NULL, protocolname, ret2,&dbv))
				{
					strcpy(xf[i].launchparams,dbv.pszVal);
					DBFreeVariant(&dbv);
				}

				sprintf(ret2,"gamenetargs_%i",i);
				if(!DBGetContactSettingTString(NULL, protocolname, ret2,&dbv))
				{
					strcpy(xf[i].networkparams,dbv.pszVal);
					DBFreeVariant(&dbv);
				}

				HICON hicon;
				HANDLE handl;
				string gname=GetGame(xf[i].gameid,xf[i].gameid2,&handl,&hicon,TRUE);

				sprintf(ret,servicefunction,i);
				CreateServiceFunctionParam(ret,StartGame,i);
				mi.pszService = ret;
				mi.position = 500090002+i;
				mi.hIcon = hicon;
				mi.pszName = (char*)gname.c_str();
				CallService( MS_CLIST_ADDMAINMENUITEM, 0, (LPARAM)&mi );
			}
			return;
		}
		else if(foundgames==0)
			return;
		else
			foundgames=0;
	else
		foundgames=0;

	FoldersGetCustomPath( XFireWorkingFolder, inipath, 1024, 'W' );
	strcat(inipath,"\\");
	strcat(inipath,"xfire_games.ini");

	//erstmal db säubern
	BOOL somethingfound=TRUE;
	i=0;
	DBVARIANT dbv;
	while(somethingfound)
	{
		somethingfound=FALSE;
		sprintf(ret2,"gameid_%i",i);
		if(DBGetContactSettingWord(NULL, protocolname, ret2,0))
		{
			somethingfound=TRUE;
			DBDeleteContactSetting(NULL, protocolname, ret2);
		}
		sprintf(ret2,"gamepath_%i",i);
		if(!DBGetContactSettingTString(NULL, protocolname, ret2,&dbv))
		{
			somethingfound=TRUE;
			DBDeleteContactSetting(NULL, protocolname, ret2);
		}
		sprintf(ret2,"gamelaunch_%i",i);
		if(!DBGetContactSettingTString(NULL, protocolname, ret2,&dbv))
		{
			somethingfound=TRUE;
			DBDeleteContactSetting(NULL, protocolname, ret2);
		}
		sprintf(ret2,"gamenetargs_%i",i);
		if(!DBGetContactSettingTString(NULL, protocolname, ret2,&dbv))
		{
			somethingfound=TRUE;
			DBDeleteContactSetting(NULL, protocolname, ret2);
		}
		i++;	
	}

	//maximal 200 notfounds, um die nicht belegten id's zu überspringen
	while(notfound<200)
	{
		//2 gameids?
		if(split)
			sprintf(temp,"%i_%i",i,i2);
		else
			sprintf(temp,"%i",i);

		//letztes ergeniss sichern
		last_gpps=gpps;

		//las ma mal suchen ....
		gpps=GetPrivateProfileString (temp, "LongName", "", gamename, 255, inipath);

		if(gpps!=NULL) //was gefunden
		{

			//Registryschlüssel auslesen und pfad auf exe prüfen
			GetPrivateProfileString (temp, "LauncherDirKey", "", ret, 255, inipath);
			strcpy(ret2,ret);

			//ersten part des registry schlüssel raustrennen
			pos=strchr(ret2,'\\');
			if(pos!=0)
			{
				HKEY hkey,hsubk;

				*pos=0; //string trennen
				pos++;

				pos2=strrchr(pos,'\\'); //key trennen
				*pos2=0;
				pos2++;

				//HKEY festlegen
				switch(*(ret2+6))
				{
				case 'L':
					hkey = HKEY_CLASSES_ROOT;
					break;
				case 'U':
					hkey = HKEY_CURRENT_USER;
					break;
				case 'O':
					hkey = HKEY_LOCAL_MACHINE;
					break;
				default: 
					hkey = NULL;
				}

				if(hkey) //nur wenn der key erkannt wurde
				{
					if(RegOpenKeyA(hkey,pos,&hsubk) == ERROR_SUCCESS) //key versuchen zu "öffnen"
					{
						char path[XFIRE_MAX_STATIC_STRING_LEN];
						DWORD size=sizeof(path);
						
						//key lesen
						if(RegQueryValueEx(hsubk,pos2,NULL,NULL,(LPBYTE)path,&size)== ERROR_SUCCESS)
						{
							//zusätzlichen pfad anhängen
							if(GetPrivateProfileString (temp, "LauncherDirAppend", "", ret2, 255, inipath))
							{
								if(*(path+strlen(path)-1)=='\\'&&*(ret2)=='\\')
									strcat(path,(ret2+1));
								else
									strcat(path,ret2);
							}

							if(GetPrivateProfileString (temp, "LauncherDirTruncAt", "", ret2, 255, inipath))
							{
								//mögliches erstes anführungszeichen entfernen
								if(*(path)='"')
								{
									pos2=path;
									pos2++;

									strcpy(path,pos2);
								}
								
								//mögliche weitere anführungszeichen entfernen
								pos=strchr(path,'"');
								if(pos!=0)
									*pos=0;

								*(path+strlen(path)-strlen(ret2))=0;
							}

							if(*(path+strlen(path)-1)!='\\')
								strcat(path,"\\");


							//dateiname auslesen
							if(GetPrivateProfileString (temp, "InstallHint", "", ret2, 255, inipath))
							{
								char pathtemp[XFIRE_MAX_STATIC_STRING_LEN];
								strcpy(pathtemp,path);
								strcat(pathtemp,ret2);
								
								if(CheckPath(pathtemp))
								{
									if(GetPrivateProfileString (temp, "DetectExe", "", ret, 255, inipath))
									{
										cutforlaunch=path+strlen(path);
										strcpy(pathtemp,path);
										strcat(pathtemp,ret);

										if(CheckPath(pathtemp))
										{
											strcpy(path, pathtemp);
										}
										else
										{
											*(path)=0;
										}
									}
								}
								else
								{
									*(path)=0;
								}
							}
							else if(GetPrivateProfileString (temp, "DetectExe", "", ret2, 255, inipath))
							{
								cutforlaunch=path+strlen(path);
								strcat(path,ret2);
								if(!CheckPath(path))
								{
									*(path)=0;
								}
							}
							else if(GetPrivateProfileString (temp, "LauncherExe", "", ret2, 255, inipath)) 
							{
								cutforlaunch=path+strlen(path);
								strcat(path,ret2);
							}

							//prüfe ob existent, dann ist das spiel installiert
							if(GetFileAttributes(path)!=0xFFFFFFFF)
							{
								char temp3[XFIRE_MAX_STATIC_STRING_LEN];

								memset(&xf[foundgames],0,sizeof(XFire_FoundGame));
								xf[foundgames].gameid=i;
//								xf[foundgames].gameid2=i2;

								for(unsigned int i=0;i<strlen(path);i++)
									path[i]=tolower(path[i]);

								strcpy(xf[foundgames].path,path);

								//für launcherstring anpassen
								char* pos=strrchr(path,'\\');
								if(pos!=0)
								{
									pos++;
									*pos=0;
								}
								GetPrivateProfileString (temp, "LauncherExe", "", ret2, 255, inipath); // anfügen
								*cutforlaunch=0;
								strcat(path,ret2);

								strcpy(xf[foundgames].launchparams,path);
								strcat(xf[foundgames].launchparams," ");
								GetPrivateProfileString (temp, "Launch", "", ret2, 512, inipath);
								str_replace(ret2,"%UA_LAUNCHER_EXE_PATH%",""); //erstmal unwichtige sachen entfernen
								str_replace(ret2,"%UA_LAUNCHER_EXTRA_ARGS%",""); // - auch entfernen	
								str_replace(ret2,"%UA_LAUNCHER_LOGIN_ARGS%",""); // - auch entfernen	
								strcat(xf[foundgames].launchparams,ret2);

								//networkstring
								if(GetPrivateProfileString (temp, "LauncherNetworkArgs", "", ret2, 512, inipath))
									strcpy(xf[foundgames].networkparams,ret2);

								//in die miranda db speichern
								sprintf(temp3,"gameid_%i",foundgames);
								DBWriteContactSettingWord(NULL, protocolname, temp3, xf[foundgames].gameid);

								sprintf(temp3,"gamepath_%i",foundgames);
								DBWriteContactSettingString(NULL,protocolname,temp3,xf[foundgames].path);

								sprintf(temp3,"gamelaunch_%i",foundgames);
								DBWriteContactSettingString(NULL,protocolname,temp3,xf[foundgames].launchparams);

								sprintf(temp3,"gamenetargs_%i",foundgames);
								if(xf[foundgames].networkparams!=NULL) DBWriteContactSettingString(NULL,protocolname,temp3,xf[foundgames].networkparams);

								//menueintrag durchführen
								HICON hicon;
								HANDLE handl;
								string gname=GetGame(xf[foundgames].gameid,xf[foundgames].gameid2,&handl,&hicon,TRUE);

								sprintf(ret,servicefunction,foundgames);
								CreateServiceFunctionParam(ret,StartGame,foundgames);
								mi.pszService = ret;
								mi.position = 500090002+foundgames;
								mi.hIcon = hicon;
								mi.pszName = (char*)gname.c_str();
								CallService( MS_CLIST_ADDMAINMENUITEM, 0, (LPARAM)&mi );

								foundgames++;
								DBWriteContactSettingWord(NULL, protocolname, "foundgames", foundgames);

								strcat(gamelist,gamename);
								strcat(gamelist,"\r\n");

								split=FALSE;
							}
							
						}

						RegCloseKey(hsubk);
					}
				}

			}
			else if(GetPrivateProfileString (temp, "LauncherDirDefault", "", ret2, 255, inipath))
			{
				if(GetPrivateProfileString (temp, "LauncherExe", "", ret, 255, inipath))
				{
					strcat(ret2,"\\");
					strcat(ret2,ret);
				}

				str_replace(ret2,"%WINDIR%",getenv("WINDIR"));
				str_replace(ret2,"%ProgramFiles%",getenv("ProgramFiles"));

				//prüfe ob existent, dann ist das spiel installiert
				if(GetFileAttributes(ret2)!=0xFFFFFFFF)
				{

					memset(&xf[foundgames],0,sizeof(XFire_FoundGame));
					xf[foundgames].gameid=i;
					xf[foundgames].gameid2=i2;
					

					for(unsigned int i=0;i<strlen(ret2);i++)
						ret2[i]=tolower(ret2[i]);

					strcpy(xf[foundgames].path,ret2);

					//launch parameterstring
					strcpy(xf[foundgames].launchparams,ret2);
					strcat(xf[foundgames].launchparams," ");
					GetPrivateProfileString (temp, "Launch", "", ret2, 512, inipath);
					str_replace(ret2,"%UA_LAUNCHER_EXE_PATH%",""); //unwichtige sachen entfernen
					str_replace(ret2,"%UA_LAUNCHER_EXTRA_ARGS%",""); // - auch entfernen	
					str_replace(ret2,"%UA_LAUNCHER_LOGIN_ARGS%",""); // - auch entfernen	
					strcat(xf[foundgames].launchparams,ret2);

					//networkstring
					if(GetPrivateProfileString (temp, "LauncherNetworkArgs", "", ret2, 512, inipath))
						strcpy(xf[foundgames].networkparams,ret2);

					//in die miranda db speichern
					sprintf(temp,"gameid_%i",foundgames);
					DBWriteContactSettingWord(NULL, protocolname, temp, xf[foundgames].gameid);

					sprintf(temp,"gamepath_%i",foundgames);
					DBWriteContactSettingString(NULL,protocolname,temp,xf[foundgames].path);

					sprintf(temp,"gamelaunch_%i",foundgames);
					DBWriteContactSettingString(NULL,protocolname,temp,xf[foundgames].launchparams);

					sprintf(temp,"gamenetargs_%i",foundgames);
					DBWriteContactSettingString(NULL,protocolname,temp,xf[foundgames].networkparams);

					//menueintrag durchführen
					HICON hicon;
					HANDLE handl;
					string gname=GetGame(xf[foundgames].gameid,xf[foundgames].gameid2,&handl,&hicon,TRUE);

					sprintf(ret,servicefunction,foundgames);
					CreateServiceFunctionParam(ret,StartGame,foundgames);
					mi.pszService = ret;
					mi.position = 500090002+foundgames;
					mi.hIcon = hicon;
					mi.pszName = (char*)gname.c_str();
					CallService( MS_CLIST_ADDMAINMENUITEM, 0, (LPARAM)&mi );

					foundgames++;
					DBWriteContactSettingWord(NULL, protocolname, "foundgames", foundgames);

					strcat(gamelist,gamename);
					strcat(gamelist,"\r\n");

					split=FALSE;
				}

			}

			if(split) 
				i2++;
			else
				i++;
			notfound=0;
		}
		else if(split==FALSE) // nichts gefunden, dann die 2. gameid probieren (games die zb über steam und normal installiert werden können, haben eine 2. id)
		{
			i2=1;
			split=TRUE;
		}
		else if(split==TRUE&&last_gpps!=NULL) // keine weiteren einträge mit der 2. id gefunden, also wieder mit der normalen weitersuchen
		{
			split=FALSE;
			i++;
			i2=0;
		}
		else if(split==TRUE&&last_gpps==NULL) // überhaupt nix gefunden, vllt nicht belegete id's, überspringen und "notfound" hochsetzen
		{
			split=FALSE;
			i2=0;
			if(i==34) //großer sprung unbenutzer id's
				i+=4000;
			i++;
			notfound++;
		}
	}
	
	DBWriteContactSettingWord(NULL, protocolname, "foundgames", foundgames);

	//scanversion setzen, um ungewollten  rescan zu vermeiden
	DBWriteContactSettingWord(NULL, protocolname, "scanver", XFIRE_SCAN_VAL);

	if(!DBGetContactSettingByte(NULL,protocolname,"scanalways",0))
	{
		sprintf(ret,Translate("Found %i Games!%s%s"),foundgames,"\r\n\r\n",gamelist);
		MSGBOX(ret);
	}

	//ReleaseMutex(ghMutex);
}

void *gamedetectiont(void *ptr)
{
	int currentgame=-1; //id des spiel im array's
	DWORD ec; //exitcode der processid
	DWORD pid=NULL; //processid des gefunden spiels

	Scan4Games(NULL);

	while(1)
	{
		Sleep(12000);

		pthread_testcancel();

		if(myClient!=NULL)
			if(myClient->client->connected)
			{
				if(currentgame!=-1)
				{
					SendGameStatusPacket *packet = new SendGameStatusPacket() ;

					//prüf ob das spiel noch offen
					ec=0;
					HANDLE op=OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
					if(op!=NULL) GetExitCodeProcess(op,&ec);

					if(GetLastError()==5) //anwendung ist noch offen und der zugriff wird noch darauf blockiert
					{
						packet->gameid=xf[currentgame].gameid;
					}
					else if(ec!=STILL_ACTIVE) //nicht mehr offen
					{
						packet->gameid=0;
						currentgame=-1;
						pid=NULL;
					}
					else //noch offen
					{
						packet->gameid=xf[currentgame].gameid;
						//packet->=xf[currentgame].gameid2;
					}

					if(op!=NULL) CloseHandle(op);

					if(myClient!=NULL)
						myClient->client->send( packet );
					delete packet; 
				}
				else
				{
					//hardcoded game detection
					HANDLE hSnapShot = CreateToolhelp32Snapshot ( TH32CS_SNAPALL, 0);
					PROCESSENTRY32* processInfo = new PROCESSENTRY32;
					processInfo->dwSize = sizeof ( PROCESSENTRY32);

					while ( Process32Next ( hSnapShot,processInfo ) != FALSE && currentgame==-1)
					{
						char fpath[XFIRE_MAX_STATIC_STRING_LEN]="";

						HANDLE op=OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ, FALSE, processInfo->th32ProcessID);
						
						if(op!=NULL&&processInfo->th32ProcessID!=0)
						{
							GetModuleFileNameEx(op,NULL,fpath,sizeof(fpath));
							
							//lowercase pfad
							for(int i=0;i<(int)strlen(fpath);i++)
								fpath[i]=tolower(fpath[i]);

							//prüfe ob das game läuft, über pfad
							for(int i=0;i<(int)foundgames;i++)
							{

								if(strcmp((xf[i].path),(fpath))==0)
								{
									currentgame=i;
									pid=processInfo->th32ProcessID;
									
									SendGameStatusPacket *packet = new SendGameStatusPacket() ;
									packet->gameid=xf[i].gameid;

									if(myClient!=NULL)
										if(myClient->client->connected)
										{
											myClient->client->send( packet );
										}

									delete packet;

									break;
								}
							}
						}
						else //wenn die exe durch antihacksoftware geschützt ist, nur exenamen prüfen
						{
							for(int i=0;i<(int)strlen(processInfo->szExeFile);i++)
								processInfo->szExeFile[i]=tolower(processInfo->szExeFile[i]);	

							for(int i=0;i<(int)foundgames;i++)
							{

								char* exename=(strrchr(xf[i].path,'\\'))+1;
								if(strcmp((exename),(processInfo->szExeFile))==0)
								{
									currentgame=i;
									pid=processInfo->th32ProcessID;
									
									SendGameStatusPacket *packet = new SendGameStatusPacket() ;
									packet->gameid=xf[i].gameid;

									if(myClient!=NULL)
										if(myClient->client->connected)
										{
											myClient->client->send( packet );
										}

									delete packet;

									break;
								}
							}
						}
						if(op!=NULL) CloseHandle(op);
					}
					CloseHandle ( hSnapShot);	
				}
			}
	}
}

static int ReScanMyGames(WPARAM wParam,LPARAM lParam)
{
	DBDeleteContactSetting(NULL, protocolname, "foundgames");

	mir_forkthread(Scan4Games,NULL);

	return 0;
}

void handlingBuddys(BuddyListEntry *entry, int clan,char*group)
{
	  HANDLE gameico;
	  HANDLE hContact;
	  string game;

	  if(entry==NULL)
		  return;

		if(entry->hcontact==NULL)
			entry->hcontact=CList_FindContact(entry->userid);

		hContact=entry->hcontact;

		if(hContact!=0)
		{
			if(strlen(entry->nick.c_str())>0)
				DBWriteContactSettingTString(hContact, protocolname, "Nick", mir_utf8decode(( char* )entry->nick.c_str(),NULL));
				//DBWriteContactSettingStringUtf(hContact, protocolname, "Nick", entry->nick.c_str());
			else
				DBWriteContactSettingString(hContact, protocolname, "Nick", entry->username.c_str());

			if(!entry->isOnline())
			{
				DBWriteContactSettingWord(hContact, protocolname, "Status", ID_STATUS_OFFLINE);
				DBDeleteContactSetting(hContact, protocolname, "XStatusMsg");
				DBDeleteContactSetting(hContact, protocolname, "XStatusId");
				DBDeleteContactSetting(hContact, protocolname, "XStatusName");
				DBDeleteContactSetting(hContact, protocolname, "ServerIP");
				DBDeleteContactSetting(hContact, protocolname, "Port");
				DBDeleteContactSetting(hContact, protocolname, "VServerIP");
				DBDeleteContactSetting(hContact, protocolname, "VPort");
				DBDeleteContactSetting(hContact, protocolname, "RVoice");
				DBDeleteContactSetting(hContact, protocolname, "RGame");
				DBDeleteContactSetting(hContact, protocolname, "GameId");
				DBDeleteContactSetting(hContact, protocolname, "VoiceId");
			}
			else if(entry->game>0||entry->game2>0)
			{
				char temp[XFIRE_MAX_STATIC_STRING_LEN]="";
				DummyXFireGame *gameob;

				//beim voicechat foglendes machn
				if(entry->game2>0) 
				{
					game=GetGame(entry->game2,0,&gameico,NULL); //name und icon vom voicechat programm
					gameob=(DummyXFireGame*)entry->game2Obj; //obj wo ip und port sind auslesen

					DBWriteContactSettingTString(hContact, protocolname, "RVoice", game.c_str());

					if((unsigned char)gameob->ip[3]!=0) // wenn ip, dann speichern
					{
						sprintf(temp,"%d.%d.%d.%d",(unsigned char)gameob->ip[3],(unsigned char)gameob->ip[2],(unsigned char)gameob->ip[1],(unsigned char)gameob->ip[0]);
						DBWriteContactSettingTString(hContact, protocolname, "VServerIP", temp);
						DBWriteContactSettingWord(hContact, protocolname, "VPort", (unsigned long)gameob->port);
					}
					else
					{
						DBDeleteContactSetting(hContact, protocolname, "VServerIP");
						DBDeleteContactSetting(hContact, protocolname, "VPort");
					}
					
					DBWriteContactSettingWord(hContact, protocolname, "VoiceId", entry->game2);

					SetIcon(hContact,gameico,EXTRA_ICON_ADV2);	//icon seperat setzen
				}
				else
				{
					DBDeleteContactSetting(hContact, protocolname, "VServerIP");
					DBDeleteContactSetting(hContact, protocolname, "VPort");
					DBDeleteContactSetting(hContact, protocolname, "RVoice");
					DBDeleteContactSetting(hContact, protocolname, "VoiceId");
					SetIcon(hContact,(HANDLE)-1,EXTRA_ICON_ADV2);
				}
				
				//beim game folgendes machen
				if(entry->game>0)
				{
					game=GetGame(entry->game,0,&gameico,NULL);

					DBWriteContactSettingTString(hContact, protocolname, "RGame", game.c_str());
										
					gameob=(DummyXFireGame*)entry->gameObj;

					if((unsigned char)gameob->ip[3]!=0)
					{
						//ip und port in kontakt speichern
						sprintf(temp,"%d.%d.%d.%d",(unsigned char)gameob->ip[3],(unsigned char)gameob->ip[2],(unsigned char)gameob->ip[1],(unsigned char)gameob->ip[0]);
						DBWriteContactSettingTString(hContact, protocolname, "ServerIP", temp);
						DBWriteContactSettingWord(hContact, protocolname, "Port", (unsigned long)gameob->port);
						
						if(DBGetContactSettingByte(NULL,protocolname,"noipportinstatus",0)==0)
							sprintf(temp,"%s (%d.%d.%d.%d:%d)",mir_utf8decode((char*)game.c_str(),NULL),(unsigned char)gameob->ip[3],
																									(unsigned char)gameob->ip[2],
																									(unsigned char)gameob->ip[1],
																									(unsigned char)gameob->ip[0],
																									(unsigned long)gameob->port);
						else
							sprintf(temp,"%s",mir_utf8decode((char*)game.c_str(),NULL));

					}
					else
					{
						DBDeleteContactSetting(hContact, protocolname, "ServerIP");
						DBDeleteContactSetting(hContact, protocolname, "Port");

						sprintf(temp,"%s",mir_utf8decode((char*)game.c_str(),NULL));
					}

					if(strlen(entry->statusmsg.c_str())>0)
					{
						char* status=mir_utf8decode((char*)entry->statusmsg.c_str(),NULL);
						if(status!=NULL)
						{
							strcat(temp," - ");
							strcat(temp,status);
						}
					}

					SetIcon(hContact,gameico);

					DBWriteContactSettingWord(hContact, protocolname, "Status", ID_STATUS_ONLINE);
					DBWriteContactSettingTString(hContact, protocolname, "XStatusMsg", temp);
					DBWriteContactSettingTString(hContact, protocolname, "XStatusName", Translate("Playing"));
					DBWriteContactSettingByte(hContact, protocolname, "XStatusId", 1);
					DBWriteContactSettingWord(hContact, protocolname, "GameId", entry->game);
				}
				else
				{
					SetIcon(hContact,(HANDLE)-1);
					DBDeleteContactSetting(hContact, protocolname, "XStatusMsg");
					DBDeleteContactSetting(hContact, protocolname, "ServerIP");
					DBDeleteContactSetting(hContact, protocolname, "Port");
					DBDeleteContactSetting(hContact, protocolname, "XStatusId");
					DBDeleteContactSetting(hContact, protocolname, "XStatusName");
					DBDeleteContactSetting(hContact, protocolname, "RGame");
					DBDeleteContactSetting(hContact, protocolname, "GameId");
				}
			}
			else if(strlen(entry->statusmsg.c_str())>0)
			{
				int mystatus=ID_STATUS_ONLINE;

				//den away overlay setzen lassen, wenn AFK erkannt wird
				if(strlen(entry->statusmsg.c_str())>4)
				{
					char* away=(char*)entry->statusmsg.c_str();
					if(*(away+1)=='A'
						&& *(away+2)=='F'
						&& *(away+3)=='K')
					{
						mystatus=ID_STATUS_AWAY;
					}
				}

				SetIcon(hContact,(HANDLE)-1);
				SetIcon(hContact,(HANDLE)-1,EXTRA_ICON_ADV2);
				DBWriteContactSettingWord(hContact, protocolname, "Status", mystatus);
				DBWriteContactSettingTString(hContact, protocolname, "XStatusMsg", mir_utf8decode((char*)entry->statusmsg.c_str(),NULL));
				DBWriteContactSettingByte(hContact, protocolname, "XStatusId", 2);
				DBWriteContactSettingTString(hContact, protocolname, "XStatusName", "");
				DBDeleteContactSetting(hContact, protocolname, "ServerIP");
				DBDeleteContactSetting(hContact, protocolname, "Port");
				DBDeleteContactSetting(hContact, protocolname, "VServerIP");
				DBDeleteContactSetting(hContact, protocolname, "VPort");
				DBDeleteContactSetting(hContact, protocolname, "RVoice");
				DBDeleteContactSetting(hContact, protocolname, "RGame");
				DBDeleteContactSetting(hContact, protocolname, "GameId");
				DBDeleteContactSetting(hContact, protocolname, "VoiceId");
			}
			else
			{
				DBVARIANT dbv;

				if(DBGetContactSettingWord(hContact, protocolname, "Status", -1)==ID_STATUS_OFFLINE)
				{
					if(DBGetContactSettingByte(hContact, "ContactPhoto", "Locked", -1)!=1)
					{
						if(DBGetContactSetting(hContact,"ContactPhoto", "File",&dbv))
						{
							XFire_SetAvatar* xsa=new XFire_SetAvatar;
							xsa->hContact=hContact;
							xsa->username=new char[strlen(entry->username.c_str())+1];
							strcpy(xsa->username,entry->username.c_str());

							mir_forkthread(SetAvatar,(LPVOID)xsa);
						}
					}
				}

				SetIcon(hContact,(HANDLE)-1);
				SetIcon(hContact,(HANDLE)-1,EXTRA_ICON_ADV2);
				DBWriteContactSettingWord(hContact, protocolname, "Status", ID_STATUS_ONLINE);
				if(clan!=0) DBWriteContactSettingDword(hContact, protocolname, "Clan", clan);
				DBDeleteContactSetting(hContact, protocolname, "XStatusMsg");
				DBDeleteContactSetting(hContact, protocolname, "ServerIP");
				DBDeleteContactSetting(hContact, protocolname, "Port");
				DBDeleteContactSetting(hContact, protocolname, "VServerIP");
				DBDeleteContactSetting(hContact, protocolname, "VPort");
				DBDeleteContactSetting(hContact, protocolname, "XStatusId");
				DBDeleteContactSetting(hContact, protocolname, "XStatusName");
				DBDeleteContactSetting(hContact, protocolname, "RVoice");
				DBDeleteContactSetting(hContact, protocolname, "RGame");
				DBDeleteContactSetting(hContact, protocolname, "GameId");
				DBDeleteContactSetting(hContact, protocolname, "VoiceId");
			}
		}
		else
		{
			XFireContact xfire_newc;
			xfire_newc.username=(char*)entry->username.c_str();
			xfire_newc.nick=(char*)entry->nick.c_str();
			xfire_newc.id=entry->userid;

			entry->hcontact=CList_AddContact(xfire_newc,TRUE,entry->isOnline()?TRUE:FALSE,clan);
		}
		if(group!=NULL)
		{
			if(!DBGetContactSettingByte(NULL,protocolname,"noclangroups",0))
				DBWriteContactSettingTString(entry->hcontact, "CList", "Group", group);			 
		}
}

int AddtoList( WPARAM wParam, LPARAM lParam ) {
    CCSDATA* ccs = (CCSDATA*)lParam;

    if (ccs->hContact)
    {
		DBVARIANT dbv2;	
		if(!DBGetContactSetting(ccs->hContact,protocolname,"Username",&dbv2)) {

			if(myClient!=NULL)
				if(myClient->client->connected)
				{
					SendAcceptInvitationPacket accept;
					accept.name = dbv2.pszVal;
					myClient->client->send(&accept );
				}
			
			//temporären buddy entfernen, da eh ein neues packet kommt
			CallService(MS_DB_CONTACT_DELETE, (WPARAM) ccs->hContact, 0);
		}
	}
	return 0;
}


static void __cdecl AckBasicSearch(void * pszNick)
{

	PROTOSEARCHRESULT psr;
	ZeroMemory(&psr, sizeof(psr));

	if(pszNick!=NULL)
	{
		psr.cbSize = sizeof(psr);
		psr.nick = ( char* )pszNick;
		ProtoBroadcastAck(protocolname, NULL, ACKTYPE_SEARCH, ACKRESULT_DATA, (HANDLE) 1, (LPARAM) & psr);
	}
	ProtoBroadcastAck(protocolname, NULL, ACKTYPE_SEARCH, ACKRESULT_SUCCESS, (HANDLE) 1, 0);
}

int BasicSearch(WPARAM wParam,LPARAM lParam) {
	static char buf[50];
	if ( lParam ) {
		if(myClient!=NULL)
			if(myClient->client->connected)
			{
				lstrcpynA(buf, (const char *)lParam, 50);
				mir_forkthread(AckBasicSearch, &buf );
				return 1;
			}
	}

	return 0;
}



static int SearchAddtoList(WPARAM wParam,LPARAM lParam)
{
	PROTOSEARCHRESULT *psr = ( PROTOSEARCHRESULT* ) lParam;

	if ( psr->cbSize != sizeof( PROTOSEARCHRESULT ))
		return 0;

	if(myClient!=NULL)
		if(myClient->client->connected)
		{
			InviteBuddyPacket invite;
			invite.addInviteName( psr->nick, Translate("Add me to your friends list."));
			myClient->client->send(&invite );
		}

	return -1;
}


void CreateGroup(char*grp) {
    DBVARIANT dbv;
    char group[255]="";
	char temp[10];
	int i=0;
	for (i = 0;; i++)
	{
		sprintf(temp,"%d",i);
		if (DBGetContactSettingString(NULL, "CListGroups", temp, &dbv))
		{
			i--;
			break;
		}
		if (dbv.pszVal[0] != '\0' && !lstrcmp(dbv.pszVal + 1, grp))	{
			DBFreeVariant(&dbv);
			return;
		}
		DBFreeVariant(&dbv);
	}
	strcpy(group,"D");
	strcat(group,grp);
	group[0]= 1 | GROUPF_EXPANDED;
	sprintf(temp,"%d",i+1);
	DBWriteContactSettingString(NULL, "CListGroups", temp, group);
	CallServiceSync(MS_CLUI_GROUPADDED, i + 1, 0);
}


int SetAwayMsg(WPARAM wParam, LPARAM lParam) {
	EnterCriticalSection(&modeMsgsMutex);
	if(lParam!=NULL)
		if (bpStatus == ID_STATUS_ONLINE)
		{
			if(myClient!=NULL)
			{
				if(myClient->client->connected)
				{
					myClient->Status(( char* )lParam);
					LeaveCriticalSection(&modeMsgsMutex);
					return 0;
				}
			}
		}
	LeaveCriticalSection(&modeMsgsMutex);
	return 1;
}

static void SendAMAck( LPVOID param )
{
	DBVARIANT dbv;
	
	if(!DBGetContactSettingTString((HANDLE)param, protocolname, "XStatusMsg",&dbv))
	{
		ProtoBroadcastAck(protocolname, (HANDLE)param, ACKTYPE_AWAYMSG, ACKRESULT_SUCCESS, (HANDLE) 1, LPARAM(dbv.pszVal));
	}
	else
		ProtoBroadcastAck(protocolname, (HANDLE)param, ACKTYPE_AWAYMSG, ACKRESULT_SUCCESS, (HANDLE) 1, LPARAM(""));
}

int GetAwayMsg(WPARAM /*wParam*/, LPARAM lParam)
{
	CCSDATA* ccs = (CCSDATA*)lParam;

	mir_forkthread(SendAMAck,ccs->hContact);
	return 1;
}

int ContactDeleted(WPARAM wParam,LPARAM lParam) {
		if(!DBGetContactSettingByte((HANDLE)wParam, protocolname, "DontSendRemovePacket", 0))
			if(myClient!=NULL)
			{
				if(myClient->client->connected)
				{
					SendRemoveBuddyPacket removeBuddy;

					removeBuddy.userid=DBGetContactSettingDword((HANDLE)wParam,protocolname,"UserId",0);

					if(removeBuddy.userid!=0)
					{
						myClient->client->send(&removeBuddy);
					}
					return 0;
				}
			}

	return 0;
}

static int StartGame(WPARAM wParam,LPARAM lParam,LPARAM fParam) {

	return 0;
}

int JoinGame(WPARAM wParam,LPARAM lParam) {

	return 0;
}
