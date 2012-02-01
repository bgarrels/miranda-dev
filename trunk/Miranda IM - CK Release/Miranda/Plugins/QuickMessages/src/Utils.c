/*
Quick Messages plugin for Miranda IM

Copyright (C) 2008 Danil Mozhar

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "quickmessages.h"

ListData* ButtonsList[100];

SortedList* QuickList=NULL;


typedef void (*ItemDestuctor)(void*);


int  sstSortButtons(const void * vmtbi1, const void * vmtbi2)
	{
	ButtonData * mtbi1=(ButtonData *)*((ButtonData ** )vmtbi1);
	ButtonData * mtbi2=(ButtonData *)*((ButtonData ** )vmtbi2);
	if (mtbi1==NULL || mtbi2==NULL) return (mtbi1-mtbi2);
	return mtbi1->dwPos-mtbi2->dwPos;
	}

int  sstQuickSortButtons(const void * vmtbi1, const void * vmtbi2)
	{
	QuickData * mtbi1=(QuickData *)*((QuickData ** )vmtbi1);
	QuickData * mtbi2=(QuickData *)*((QuickData ** )vmtbi2);
	if (mtbi1==NULL || mtbi2==NULL) return (mtbi1-mtbi2);
	return mtbi1->dwPos-mtbi2->dwPos;
	}


int  sstOpSortButtons(const void * vmtbi1, const void * vmtbi2)
	{
	ButtonData * mtbi1=(ButtonData *)*((ButtonData ** )vmtbi1);
	ButtonData * mtbi2=(ButtonData *)*((ButtonData ** )vmtbi2);
	if (mtbi1==NULL || mtbi2==NULL) return (mtbi1-mtbi2);
	return mtbi1->dwOPPos-mtbi2->dwOPPos;
	}


void li_ListDestruct(SortedList *pList, ItemDestuctor pItemDestructor)
	{																			
	int i=0;
	if (!pList) return;
	for (i=0; i<pList->realCount; i++)	pItemDestructor(pList->items[i]);	
	li.List_Destroy(pList);																											
	mir_free(pList);
	}

void li_RemoveDestruct(SortedList *pList, int index, ItemDestuctor pItemDestructor)
	{																																
	if (index>=0 && index<pList->realCount)	
		{
		pItemDestructor(pList->items[index]);
		li.List_Remove(pList, index);
		}
	}

void li_RemovePtrDestruct(SortedList *pList, void * ptr, ItemDestuctor pItemDestructor)
	{																																
	if (li.List_RemovePtr(pList, ptr))
		pItemDestructor(ptr);
	}

void li_SortList(SortedList *pList, FSortFunc pSortFunct)
	{
	FSortFunc pOldSort=pList->sortFunc;
	int i;
	if (!pSortFunct) pSortFunct=pOldSort;
	pList->sortFunc=NULL;
	for (i=0; i<pList->realCount-1; i++)
		if (pOldSort(pList->items[i],pList->items[i+1])<0)
			{
			void * temp=pList->items[i];
			pList->items[i]=pList->items[i+1];
			pList->items[i+1]=temp;
			i--;
			if (i>0) i--;
			}
		pList->sortFunc=pOldSort;
	}

void li_ZeroQuickList(SortedList *pList)
	{
	int i;
	for (i=0; i<pList->realCount; i++)
		{
			QuickData * qd=(QuickData *)pList->items[i];
			qd->dwPos=0;
			qd->bIsService=0;
			qd->ptszValue=NULL;
			qd->ptszValueName=NULL;
			li.List_Remove(pList, i);
			i--;
		}
	}

static void listdestructor(void * input)
	{
	ButtonData * cbdi=(ButtonData *)input;
	
	if(cbdi->pszName!=cbdi->pszOpName)
		{
		if(cbdi->pszOpName)
			mir_free(cbdi->pszOpName);
		if(cbdi->pszName)
			mir_free(cbdi->pszName);
		}
	else if(cbdi->pszName)
			mir_free(cbdi->pszName);

	if(cbdi->pszValue!=cbdi->pszOpValue)
		{
		if(cbdi->pszOpValue)
			mir_free(cbdi->pszOpValue);
		if(cbdi->pszValue)
			mir_free(cbdi->pszValue);
		}
	else if(cbdi->pszValue)
			mir_free(cbdi->pszValue);

	mir_free(cbdi);
	}

void RemoveMenuEntryNode(SortedList *pList, int index)
	{
		li_RemoveDestruct(pList,index,listdestructor);
	}

void DestroyButton(int listnum)
	{
	int i=listnum;
	ListData* ld=ButtonsList[listnum];
	
	if(ld->ptszButtonName) mir_free(ld->ptszButtonName);

	if(ld->ptszOPQValue!=ld->ptszQValue)
		if(ld->ptszOPQValue) mir_free(ld->ptszOPQValue);
	
	if (ld->ptszQValue) mir_free(ld->ptszQValue);

	li_ListDestruct((SortedList*)ld->sl,listdestructor);
	

	mir_free(ld);
	ButtonsList[i]=NULL;
	while(ButtonsList[i+1])
	{
	ButtonsList[i]=ButtonsList[i+1];
	ButtonsList[i+1]=NULL;
	i++;
	}
}


void SaveModuleSettings(int buttonnum,ButtonData* bd)
	{
	char szMEntry[256]={'\0'};

	mir_snprintf(szMEntry,255,"EntryName_%u_%u",buttonnum,bd->dwPos);
	DBWriteContactSettingTString(NULL, PLGNAME,szMEntry,bd->pszName );

	mir_snprintf(szMEntry,255,"EntryValue_%u_%u",buttonnum,bd->dwPos);
	if(bd->pszValue)
		DBWriteContactSettingTString(NULL, PLGNAME,szMEntry,bd->pszValue );
	else
		DBDeleteContactSetting(NULL, PLGNAME,szMEntry);

	mir_snprintf(szMEntry,255,"EntryRel_%u_%u",buttonnum,bd->dwPos);
	DBWriteContactSettingByte(NULL, PLGNAME,szMEntry,bd->fEntryType );

	mir_snprintf(szMEntry,255,"EntryToQMenu_%u_%u",buttonnum,bd->dwPos);
	DBWriteContactSettingByte(NULL, PLGNAME,szMEntry,bd->bInQMenu);

	mir_snprintf(szMEntry,255,"EntryIsServiceName_%u_%u",buttonnum,bd->dwPos);
	DBWriteContactSettingByte(NULL, PLGNAME,szMEntry,bd->bIsServName);
	}

void CleanSettings(int buttonnum,int from)
	{
	char szMEntry[256]={'\0'};
	DBVARIANT dbv = {0};
	if(from==-1){
		mir_snprintf(szMEntry,255,"ButtonName_%u",buttonnum);
		DBDeleteContactSetting(NULL, PLGNAME,szMEntry);
		mir_snprintf(szMEntry,255,"ButtonValue_%u",buttonnum);
		DBDeleteContactSetting(NULL, PLGNAME,szMEntry);
		mir_snprintf(szMEntry,255,"RCEntryIsServiceName_%u",buttonnum);
		DBDeleteContactSetting(NULL, PLGNAME,szMEntry);
		}

	mir_snprintf(szMEntry,255,"EntryName_%u_%u",buttonnum,from);
	while(!DBGetContactSettingTString(NULL, PLGNAME,szMEntry,&dbv)){
		DBDeleteContactSetting(NULL, PLGNAME,szMEntry);
		mir_snprintf(szMEntry,255,"EntryValue_%u_%u",buttonnum,from);
		DBDeleteContactSetting(NULL, PLGNAME,szMEntry);
		mir_snprintf(szMEntry,255,"EntryRel_%u_%u",buttonnum,from);
		DBDeleteContactSetting(NULL, PLGNAME,szMEntry);
		mir_snprintf(szMEntry,255,"EntryToQMenu_%u_%u",buttonnum,from);
		DBDeleteContactSetting(NULL, PLGNAME,szMEntry);
		mir_snprintf(szMEntry,255,"EntryIsServiceName_%u_%u",buttonnum,from);
		DBDeleteContactSetting(NULL, PLGNAME,szMEntry);

		mir_snprintf(szMEntry,255,"EntryName_%u_%u",buttonnum,++from);
		}
	DBFreeVariant(&dbv);
	}

BYTE getEntryByte(int buttonnum,int entrynum,BOOL mode) 
	{	  
	char szMEntry[256]={'\0'};
	if		(mode==0)
		mir_snprintf(szMEntry,255,"EntryToQMenu_%u_%u",buttonnum,entrynum);
	else if (mode==1)
		mir_snprintf(szMEntry,255,"EntryRel_%u_%u",buttonnum,entrynum);
	else if (mode==2)
		mir_snprintf(szMEntry,255,"EntryIsServiceName_%u_%u",buttonnum,entrynum);
	else if (mode==3)
		mir_snprintf(szMEntry,255,"RCEntryIsServiceName_%u",buttonnum);
	return DBGetContactSettingByte(NULL, PLGNAME,szMEntry, 0); 
	}

DWORD BalanceButtons(int buttonsWas,int buttonsNow)
	{
	if  (ServiceExists(MS_BB_ADDBUTTON))
		{
		BBButton bb={0};
		bb.cbSize=sizeof(BBButton);
		bb.pszModuleName=PLGNAME;

		while(buttonsWas>(buttonsNow))
			{
			bb.dwButtonID=--buttonsWas;
			CallService(MS_BB_REMOVEBUTTON,0,(LPARAM)&bb);
			}	
		while (buttonsWas<(buttonsNow))
			{
			if  (ServiceExists(MS_BB_ADDBUTTON))
				{
				char iconname[20]={'\0'};
				mir_snprintf(iconname,SIZEOF(iconname),"QMessagesButton_%u",buttonsWas);
				bb.bbbFlags=BBBF_ISIMBUTTON|BBBF_ISCHATBUTTON|BBBF_ISLSIDEBUTTON;
				bb.dwButtonID=buttonsWas++;
				bb.dwDefPos=300+buttonsWas;
				bb.hIcon=(HANDLE)AddIcon(hIcon, iconname, iconname);
				CallService(MS_BB_ADDBUTTON, 0, (LPARAM)&bb);
				}
			}
		}
	return buttonsNow;
	}


void InitButtonsList()
	{  
	int i,j,k=0;
	QuickList=li.List_Create(0,1);
	for(i=0;i<g_iButtonsCount;i++)
		{
		TCHAR* pszBName=NULL;
		ListData* ld=NULL;
		if(!(pszBName=getMenuEntry(i,0,3))){ 
			g_iButtonsCount=i;
			DBWriteContactSettingByte(NULL, PLGNAME,"ButtonsCount", (BYTE)g_iButtonsCount);
			break;}
			
		ld=mir_alloc(sizeof(ListData));
		ButtonsList[i]=ld;
		ld->sl=li.List_Create(0,1);
		ld->ptszQValue=ld->ptszOPQValue=getMenuEntry(i,0,2);
		ld->ptszButtonName=pszBName;
		ld->dwPos=ld->dwOPPos=i;
		ld->dwOPFlags=0;
		ld->bIsServName=ld->bIsOpServName=getEntryByte(i,0,3);
		for(j=0;;j++)
			{	
			TCHAR* pszEntry=NULL;
			ButtonData *bd=NULL;
			
			if(!(pszEntry=getMenuEntry(i,j,0)))
				break;

			bd=mir_alloc(sizeof(ButtonData));
			memset(bd,0,sizeof(ButtonData));

			bd->dwPos=bd->dwOPPos=j;
			bd->pszName=bd->pszOpName=pszEntry;
			bd->pszValue=bd->pszOpValue=getMenuEntry(i,j,1);
			bd->fEntryType=bd->fEntryOpType=getEntryByte(i,j,1);
			bd->bInQMenu=bd->bOpInQMenu=getEntryByte(i,j,0);
			bd->bIsServName=bd->bIsOpServName=getEntryByte(i,j,2);
			if(bd->bInQMenu){
				QuickData* qd=mir_alloc(sizeof(QuickData));
				qd->dwPos=k++;
				qd->ptszValue=bd->pszValue;
				qd->ptszValueName=bd->pszName;
				li.List_InsertPtr(QuickList,qd);
				}
			li.List_InsertPtr((SortedList*)ld->sl,bd);
			}
		}

	}

void DestructButtonsList()
{
	int i=0;
//	for ( i=0; i < g_iButtonsCount; i++ )
while(ButtonsList[i])
		{
		li_ListDestruct(ButtonsList[i]->sl,listdestructor);
		mir_free(ButtonsList[i]->ptszButtonName);
		if(ButtonsList[i]->ptszOPQValue!=ButtonsList[i]->ptszQValue)
			if (ButtonsList[i]->ptszOPQValue) mir_free(ButtonsList[i]->ptszOPQValue);
		if (ButtonsList[i]->ptszQValue) mir_free(ButtonsList[i]->ptszQValue);
	i++;	
	}
if(QuickList)
	{
	li_ZeroQuickList(QuickList);
	li.List_Destroy(QuickList);
	}
}

TCHAR* getMenuEntry(int buttonnum,int entrynum,BYTE mode) 
	{	  
	TCHAR* buffer=NULL;
	char szMEntry[256]={'\0'};
	DBVARIANT dbv = {0};

	if(mode==0)
		mir_snprintf(szMEntry,255,"EntryName_%u_%u",buttonnum,entrynum);
	else if (mode==1)
		mir_snprintf(szMEntry,255,"EntryValue_%u_%u",buttonnum,entrynum);
	else if	(mode==2)
		mir_snprintf(szMEntry,255,"ButtonValue_%u",buttonnum);
	else if	(mode==3)
		mir_snprintf(szMEntry,255,"ButtonName_%u",buttonnum);


	DBGetContactSettingTString(NULL, PLGNAME,szMEntry, &dbv);

	if(dbv.ptszVal&&_tcslen(dbv.ptszVal))
		{
		buffer=mir_tstrdup(dbv.ptszVal);
		DBFreeVariant(&dbv);
		}

	return buffer; 
	}


int AddIcon(HICON icon, char *name, char *description)
	{
	SKINICONDESC sid = {0};
	sid.cbSize = sizeof(SKINICONDESC);
	sid.pszSection = "Quick Messages";
	sid.cx = sid.cy = 16;
	sid.pszDescription = description;
	sid.pszName = name;
	sid.hDefaultIcon = icon;

	return CallService(MS_SKIN2_ADDICON, 0, (LPARAM) &sid);
	}

int RegisterCustomButton(WPARAM wParam,LPARAM lParam)
	{
	if  (ServiceExists(MS_BB_ADDBUTTON))
		{
		int i;
		for(i=0;i<g_iButtonsCount;i++)
			{
			BBButton bbd={0};
			ListData* ld=ButtonsList[i];
			char iconname[20]={'\0'};
			mir_snprintf(iconname,SIZEOF(iconname),"QMessagesButton_%u",i);

			bbd.cbSize=sizeof(BBButton);
			bbd.bbbFlags=BBBF_ISIMBUTTON|BBBF_ISCHATBUTTON|BBBF_ISLSIDEBUTTON;
			bbd.dwButtonID=i;
			bbd.dwDefPos=320+i;
			bbd.hIcon=(HANDLE)AddIcon(hIcon, iconname, iconname);
			bbd.pszModuleName=PLGNAME;
			bbd.ptszTooltip=ld->ptszButtonName;
			CallService(MS_BB_ADDBUTTON, 0, (LPARAM)&bbd);
			}
		return 0;
		}
	return 1;
	}

TCHAR* ParseString(HANDLE hContact,TCHAR* ptszQValIn,TCHAR* ptszText,TCHAR* ptszClip,int QVSize,int TextSize ,int ClipSize)
	{
	int i=0,iOffset=0,iCount=0;
	TCHAR* tempPointer=NULL;
	TCHAR* ptszQValue=_tcsdup(ptszQValIn);
	TCHAR* tempQValue=ptszQValue;
	TCHAR varstr=_T('%');
 
 	if(TextSize&&ptszText[TextSize-1]=='\0') TextSize--;
 	if(ClipSize&&ptszClip[ClipSize-1]=='\0') ClipSize--;

	if(!_tcschr(ptszQValue,varstr))
		return ptszQValue;

	while(ptszQValue[i])
		{
		if(ptszQValue[i]=='%'){
			switch(ptszQValue[i+1])
				{
				case 't':{
					TCHAR* p=NULL;
					p=realloc(tempQValue, (QVSize+TextSize+1)*sizeof(TCHAR));
					if(p){
						int test=0;
						i=iOffset;
						tempQValue=ptszQValue=p;

						tempPointer=memmove(ptszQValue+i+TextSize,ptszQValue+i+2,(QVSize-i-1)*sizeof(TCHAR));
						
						if(TextSize) memcpy(ptszQValue+i, ptszText, TextSize*sizeof(TCHAR));
						
 						QVSize+=(TextSize-2);
 
 						ptszQValue[QVSize]='\0';
 
						if(!_tcschr(ptszQValue,varstr))
							return ptszQValue;

						ptszQValue=tempPointer;
						iOffset+=TextSize-1;
						i=-1;
						}
				}break;

				case 'c':{
					TCHAR* p=NULL;
					p=realloc(tempQValue, (QVSize+ClipSize+1)*sizeof(TCHAR));
					if(p){
						i=iOffset;
						tempQValue=ptszQValue=p;

						tempPointer=memmove(ptszQValue+i+ClipSize,ptszQValue+i+2,(QVSize-i-1)*sizeof(TCHAR));
						if(ClipSize) memcpy(ptszQValue+i, ptszClip, ClipSize*sizeof(TCHAR));
						QVSize+=(ClipSize-2);

						ptszQValue[QVSize]='\0';
						
						if(!_tcschr(ptszQValue,varstr))
							return ptszQValue;

						ptszQValue=tempPointer;
						iOffset+=ClipSize-1;
						i=-1;
						}
				}break;

				case 'P':{
					TCHAR* p=NULL;
					int NameLenght=0;
					TCHAR* ptszName=NULL;
#ifdef _UNICODE
					ptszName=mir_a2u((char*)CallService(MS_PROTO_GETCONTACTBASEPROTO, (WPARAM)hContact, 0));
#else
					ptszName=(char*)CallService(MS_PROTO_GETCONTACTBASEPROTO, (WPARAM)hContact, 0);
#endif
					NameLenght=_tcslen(ptszName);

					p=realloc(tempQValue, (QVSize+NameLenght+1)*sizeof(TCHAR));
					if(p){
						i=iOffset;
						tempQValue=ptszQValue=p;

						tempPointer=memmove(ptszQValue+i+NameLenght,ptszQValue+i+2,(QVSize-i-1)*sizeof(TCHAR));
						if(NameLenght)	memcpy(ptszQValue+i, ptszName, NameLenght*sizeof(TCHAR));
						QVSize+=(NameLenght-2);

						ptszQValue[QVSize]='\0';
						if(ptszName) mir_free(ptszName);

						if(!_tcschr(ptszQValue,varstr))
							return ptszQValue;

						ptszQValue=tempPointer;
						iOffset+=NameLenght-1;
						i=-1;
						}
					}break;

				case 'n':{
					TCHAR* p=NULL;
					int NameLenght=0;
					TCHAR* ptszName=NULL;

					ptszName=(TCHAR *)CallService(MS_CLIST_GETCONTACTDISPLAYNAME, (WPARAM)hContact, GCDNF_TCHAR);
					NameLenght=_tcslen(ptszName);
					p=realloc(tempQValue, (QVSize+NameLenght+1)*sizeof(TCHAR));
					if(p){
						i=iOffset;
						tempQValue=ptszQValue=p;

						tempPointer=memmove(ptszQValue+i+NameLenght,ptszQValue+i+2,(QVSize-i-1)*sizeof(TCHAR));
						if(NameLenght)memcpy(ptszQValue+i, ptszName, NameLenght*sizeof(TCHAR));
						QVSize+=(NameLenght-2);

						//if(ptszName) mir_free(ptszName);

						ptszQValue[QVSize]='\0';

						if(!_tcschr(ptszQValue,varstr))
							return ptszQValue;

						ptszQValue=tempPointer;
						iOffset+=NameLenght-1;
						i=-1;
						}
					}break;
				case 'F':{
					TCHAR* p=NULL;
					int NameLenght=0;
					TCHAR* ptszName=NULL;
					CONTACTINFO ci;
					ZeroMemory(&ci,sizeof(CONTACTINFO));
					ci.cbSize = sizeof(CONTACTINFO);
					ci.hContact = hContact;
#ifdef UNICODE
					ci.dwFlag =CNF_FIRSTNAME|CNF_UNICODE;
#else
					ci.dwFlag =CNF_FIRSTNAME;
#endif
					ci.szProto=(char*)CallService(MS_PROTO_GETCONTACTBASEPROTO, (WPARAM)hContact, 0);
					if(!CallService(MS_CONTACT_GETCONTACTINFO,(WPARAM)0,(LPARAM)&ci)&&ci.pszVal){
						NameLenght=_tcslen(ci.pszVal);
						ptszName=ci.pszVal;
						}
					p=realloc(tempQValue, (QVSize+NameLenght+1)*sizeof(TCHAR));
					if(p){
						i=iOffset;
						tempQValue=ptszQValue=p;

						tempPointer=memmove(ptszQValue+i+NameLenght,ptszQValue+i+2,(QVSize-i-1)*sizeof(TCHAR));
						if(NameLenght)memcpy(ptszQValue+i, ptszName, NameLenght*sizeof(TCHAR));
						QVSize+=(NameLenght-2);

						if(ptszName) mir_free(ptszName);

						ptszQValue[QVSize]='\0';

						if(!_tcschr(ptszQValue,varstr))
							return ptszQValue;

						ptszQValue=tempPointer;
						iOffset+=NameLenght-1;
						i=-1;
						}
					}break;
				case 'L':{
					TCHAR* p=NULL;
					int NameLenght=0;
					TCHAR* ptszName=NULL;
					CONTACTINFO ci;
					ZeroMemory(&ci,sizeof(CONTACTINFO));
					ci.cbSize = sizeof(CONTACTINFO);
					ci.hContact = hContact;
					ci.szProto=(char*)CallService(MS_PROTO_GETCONTACTBASEPROTO, (WPARAM)hContact, 0);
#ifdef UNICODE
					ci.dwFlag =CNF_LASTNAME|CNF_UNICODE;
#else
					ci.dwFlag =CNF_LASTNAME;
#endif
					if(!CallService(MS_CONTACT_GETCONTACTINFO,(WPARAM)0,(LPARAM)&ci)&&ci.pszVal){
						NameLenght=_tcslen(ci.pszVal);
						ptszName=ci.pszVal;
						}
					p=realloc(tempQValue, (QVSize+NameLenght+1)*sizeof(TCHAR));
					if(p){
						i=iOffset;
						tempQValue=ptszQValue=p;

						tempPointer=memmove(ptszQValue+i+NameLenght,ptszQValue+i+2,(QVSize-i-1)*sizeof(TCHAR));
						if(NameLenght)memcpy(ptszQValue+i, ptszName, NameLenght*sizeof(TCHAR));
						QVSize+=(NameLenght-2);

						ptszQValue[QVSize]='\0';

						if(!_tcschr(ptszQValue,varstr))
							return ptszQValue;

						ptszQValue=tempPointer;
						iOffset+=NameLenght-1;
						i=-1;
						}
					}break;
				}
			}
		iOffset++;
		i++;
		}
	return ptszQValue;
	}