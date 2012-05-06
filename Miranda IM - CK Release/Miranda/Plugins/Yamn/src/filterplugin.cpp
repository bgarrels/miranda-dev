/*
YAMN plugin for 
Miranda IM: the free IM client for Microsoft* Windows*

Author
			Copyright (C) 2002-2004 majvan
			Copyright (C) 2005-2007 tweety y_b

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

* YAMN plugin export functions for filtering
*/

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <newpluginapi.h>
#include <m_database.h>
#include "m_yamn.h"
#include "m_filterplugin.h"
#include "../mails/m_mails.h"
#include "debug.h"

//- imported ---------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------

//From main.cpp
extern LPCRITICAL_SECTION PluginRegCS;
extern YAMN_VARIABLES YAMNVar;
//From synchro.cpp
extern DWORD WINAPI WaitToWriteFcn(PSWMRG SObject,PSCOUNTER=NULL);
extern void WINAPI WriteDoneFcn(PSWMRG SObject,PSCOUNTER=NULL);
//From maild.cpp
extern INT_PTR LoadMailDataSvc(WPARAM wParam,LPARAM lParam);
extern INT_PTR UnloadMailDataSvc(WPARAM wParam,LPARAM);
extern INT_PTR SaveMailDataSvc(WPARAM wParam,LPARAM lParam);

//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------

PYAMN_FILTERPLUGINQUEUE FirstFilterPlugin=NULL;

INT_PTR RegisterFilterPluginSvc(WPARAM,LPARAM);

//Removes plugin from queue and deletes its structures
INT_PTR UnregisterFilterPlugin(HYAMNFILTERPLUGIN Plugin);

INT_PTR UnregisterFilterPluginSvc(WPARAM wParam,LPARAM lParam);

//Removes all filter plugins
INT_PTR UnregisterFilterPlugins();

INT_PTR FilterMailSvc(WPARAM,LPARAM);

//Sets imported functions for an plugin and therefore it starts plugin to be registered and running
// Plugin- plugin, which wants to set its functions
// Importance- importance of plugin (see m_filterplugin.h)
// YAMNFilterFcn- pointer to imported functions
// YAMNfilterFcnVer- version of YAMN_FILTERIMPORTFCN, use YAMN_FILTERIMPORTFCNVERSION
// returns nonzero if success
int WINAPI SetFilterPluginFcnImportFcn(HYAMNFILTERPLUGIN Plugin,DWORD Importance,PYAMN_FILTERIMPORTFCN YAMNFilterFcn,DWORD YAMNFilterFcnVer);

struct CExportedFunctions FilterPluginExportedFcn[]=
{
	{YAMN_SETFILTERPLUGINFCNIMPORTID,(void *)SetFilterPluginFcnImportFcn},
};

struct CExportedServices FilterPluginExportedSvc[]=
{
	{MS_YAMN_REGISTERFILTERPLUGIN,RegisterFilterPluginSvc},
	{MS_YAMN_UNREGISTERFILTERPLUGIN,UnregisterFilterPluginSvc},
};

//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------

INT_PTR RegisterFilterPluginSvc(WPARAM wParam,LPARAM lParam)
{
	PYAMN_FILTERREGISTRATION Registration=(PYAMN_FILTERREGISTRATION)wParam;
	HYAMNFILTERPLUGIN Plugin;

	if(lParam!=YAMN_FILTERREGISTRATIONVERSION)
		return 0;
	if((Registration->Name==NULL) || (Registration->Ver==NULL))
		return NULL;
	if(NULL==(Plugin=new YAMN_FILTERPLUGIN))
		return NULL;

	Plugin->PluginInfo=Registration;

	Plugin->FilterFcn=NULL;

#ifdef DEBUG_SYNCHRO
	DebugLog(SynchroFile,"::: YAMN- new filter registered: %0x (%s) :::\n",Plugin,Registration->Name);
#endif
	return (INT_PTR)Plugin;
}

INT_PTR UnregisterFilterPlugin(HYAMNFILTERPLUGIN Plugin)
{
	PYAMN_FILTERPLUGINQUEUE Parser,Found;

	if(FirstFilterPlugin->Plugin==Plugin)
	{
		Found=FirstFilterPlugin;
		FirstFilterPlugin=FirstFilterPlugin->Next;
	}
	else
	{
		for(Parser=FirstFilterPlugin;(Parser->Next!=NULL) && (Plugin!=Parser->Next->Plugin);Parser=Parser->Next);
		if(Parser->Next!=NULL)
		{
			Found=Parser->Next;
			Parser->Next=Parser->Next->Next;
		}
		else
			Found=NULL;
	}
	if(Found!=NULL)
	{
		if(Plugin->FilterFcn->UnLoadFcn!=NULL)
			Plugin->FilterFcn->UnLoadFcn((void *)0);
		
		delete Found->Plugin;
		delete Found;
#ifdef DEBUG_SYNCHRO
		DebugLog(SynchroFile,"::: YAMN- filter %0x unregistered :::\n",Plugin);
#endif
	}
	else
		return 0;
	return 1;
}

INT_PTR UnregisterFilterPluginSvc(WPARAM wParam,LPARAM lParam)
{
	HYAMNFILTERPLUGIN Plugin=(HYAMNFILTERPLUGIN)wParam;

	EnterCriticalSection(PluginRegCS);
	UnregisterFilterPlugin(Plugin);
	LeaveCriticalSection(PluginRegCS);
	return 1;
}

INT_PTR UnregisterFilterPlugins()
{
	EnterCriticalSection(PluginRegCS);
//We remove protocols from the protocol list
	while(FirstFilterPlugin!=NULL)
		UnregisterFilterPlugin(FirstFilterPlugin->Plugin);
	LeaveCriticalSection(PluginRegCS);
	return 1;
}

int WINAPI SetFilterPluginFcnImportFcn(HYAMNFILTERPLUGIN Plugin,DWORD Importance,PYAMN_FILTERIMPORTFCN YAMNFilterFcn,DWORD YAMNFilterFcnVer)
{
	PYAMN_FILTERPLUGINQUEUE Parser,Previous;

	if(YAMNFilterFcnVer!=YAMN_FILTERIMPORTFCNVERSION)
		return 0;
	if(YAMNFilterFcn==NULL)
		return 0;

#ifdef DEBUG_SYNCHRO
	DebugLog(SynchroFile,"::: YAMN- filter %0x import succeed :::\n",Plugin);
#endif
	Plugin->Importance=Importance;
	Plugin->FilterFcn=YAMNFilterFcn;

	EnterCriticalSection(PluginRegCS);
//We add protocol to the protocol list
	for(Previous=NULL,Parser=FirstFilterPlugin;Parser!=NULL && Parser->Next!=NULL && Parser->Plugin->Importance<=Importance;Previous=Parser,Parser=Parser->Next);
	if(Previous==NULL)	//insert to the beginnig of queue
	{
		FirstFilterPlugin=new YAMN_FILTERPLUGINQUEUE;
		FirstFilterPlugin->Plugin=Plugin;
		FirstFilterPlugin->Next=Parser;
	}
	else
	{
		Previous->Next=new YAMN_FILTERPLUGINQUEUE;
		Previous=Previous->Next;			//leave previous, go to actual plugin
		Previous->Plugin=Plugin;
		Previous->Next=Parser;				//and in actual plugin set, that next plugin is the one we insert in front of
	}

	LeaveCriticalSection(PluginRegCS);
	return 1;
}

INT_PTR FilterMailSvc(WPARAM wParam,LPARAM lParam)
{
	HACCOUNT Account=(HACCOUNT)wParam;
	HYAMNMAIL Mail=(HYAMNMAIL)lParam;
	PYAMN_FILTERPLUGINQUEUE ActualPlugin;

	EnterCriticalSection(PluginRegCS);
#ifdef DEBUG_SYNCHRO
	DebugLog(SynchroFile,"FilterMail:ActualAccountMsgsSO-write wait\n");
#endif
	WaitToWriteFcn(Account->MessagesAccessSO);
#ifdef DEBUG_SYNCHRO
	DebugLog(SynchroFile,"FilterMail:ActualAccountMsgsSO-write enter\n");
#endif
	for(ActualPlugin=FirstFilterPlugin;ActualPlugin!=NULL;ActualPlugin=ActualPlugin->Next)
	{
		if(ActualPlugin->Plugin->FilterFcn->FilterMailFcnPtr!=NULL)
		{
#ifdef DEBUG_SYNCHRO
			DebugLog(SynchroFile,"\tFiltering Mail, running plugin %0x to filter mail\n",ActualPlugin->Plugin);
#endif
			ActualPlugin->Plugin->FilterFcn->FilterMailFcnPtr(Account,YAMN_ACCOUNTVERSION,Mail,YAMN_MAILVERSION);
#ifdef DEBUG_SYNCHRO
			DebugLog(SynchroFile,"\tFiltering Mail done\n");
#endif
		}
	}
	Mail->Flags|=YAMN_MSG_FILTERED;

//Set mail flags according to spamlevel settings
	if((Mail->Flags & YAMN_MSG_SPAMMASK) > YAMN_MSG_SPAML1)
		Mail->Flags=Mail->Flags & ~(YAMN_MSG_BROWSER | YAMN_MSG_POPUP | YAMN_MSG_SYSTRAY | YAMN_MSG_SOUND | YAMN_MSG_APP | YAMN_MSG_NEVENT);
	if(YAMN_MSG_SPAML(Mail->Flags,YAMN_MSG_SPAML3) || YAMN_MSG_SPAML(Mail->Flags,YAMN_MSG_SPAML4))
		Mail->Flags=Mail->Flags | (YAMN_MSG_AUTODELETE | YAMN_MSG_DELETEOK);	//set message to delete
	if(YAMN_MSG_SPAML(Mail->Flags,YAMN_MSG_SPAML3))
		Mail->Flags=Mail->Flags & ~(YAMN_MSG_MEMDELETE);	//set message not to delete it immidiatelly from memory
#ifdef DEBUG_SYNCHRO
	DebugLog(SynchroFile,"FilterMail:ActualAccountMsgsSO-write done\n");
#endif
	WriteDoneFcn(Account->MessagesAccessSO);
	LeaveCriticalSection(PluginRegCS);
	return 1;
}
