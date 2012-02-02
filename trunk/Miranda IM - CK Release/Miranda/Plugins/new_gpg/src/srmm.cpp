// Copyright � 2010 SecureIM developers (baloo and others), sss
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.



#include "commonheaders.h"

void ShowStatusIcon(HANDLE hContact);
void setSrmmIcon(HANDLE hContact);

int __cdecl onWindowEvent(WPARAM wParam, LPARAM lParam) {

	MessageWindowEventData *mwd = (MessageWindowEventData *)lParam;
	if(mwd->uType == MSG_WINDOW_EVT_OPEN || mwd->uType == MSG_WINDOW_EVT_OPENING) 
	{
		setSrmmIcon(mwd->hContact);
	}
	return 0;
}


int __cdecl onIconPressed(WPARAM wParam, LPARAM lParam) {
	HANDLE hContact = (HANDLE)wParam;
	HANDLE hMeta = hContact;
	if(metaIsProtoMetaContacts(hContact))
		hContact = metaGetMostOnline(hContact); // ������� ���, ����� ������� ������ ���������
	else if(metaIsSubcontact(hContact))
		hMeta = metaGetContact(hContact);
	StatusIconClickData *sicd = (StatusIconClickData *)lParam;
	if(strcmp(sicd->szModule, szGPGModuleName)) 
		return 0; // not our event
	
	void setSrmmIcon(HANDLE);
	void setClistIcon(HANDLE);
	bool isContactHaveKey(HANDLE hContact);
	BYTE enc = DBGetContactSettingByte(hContact, szGPGModuleName, "GPGEncryption", 0);
	if(enc)
	{
		DBWriteContactSettingByte(hContact, szGPGModuleName, "GPGEncryption", 0);
		DBWriteContactSettingByte(hMeta, szGPGModuleName, "GPGEncryption", 0);
		setSrmmIcon(hContact);
		setClistIcon(hContact);
	}
	else if(!enc)
	{
		if(!isContactHaveKey(hContact))
		{
			void ShowLoadPublicKeyDialog();
			extern map<int, HANDLE> user_data;
			extern int item_num;
			item_num = 0;		 //black magic here
			user_data[1] = hContact;
			ShowLoadPublicKeyDialog();
		}
		else
		{
			DBWriteContactSettingByte(hContact, szGPGModuleName, "GPGEncryption", 1);
			DBWriteContactSettingByte(hMeta, szGPGModuleName, "GPGEncryption", 1);
			setSrmmIcon(hContact);
			setClistIcon(hContact);
			return 0;
		}
		if(isContactHaveKey(hContact))
		{
			DBWriteContactSettingByte(hContact, szGPGModuleName, "GPGEncryption", 1);
			DBWriteContactSettingByte(hMeta, szGPGModuleName, "GPGEncryption", 1);
			setSrmmIcon(hContact);
			setClistIcon(hContact);
		}
	}
	return 0;
}
