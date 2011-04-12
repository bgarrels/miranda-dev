// ---------------------------------------------------------------------------80
//                ICQ plugin for Miranda Instant Messenger
//                ________________________________________
//
// Copyright � 2000,2001 Richard Hughes, Roland Rabien, Tristan Van de Vreede
// Copyright � 2001,2002 Jon Keating, Richard Hughes
// Copyright � 2002,2003,2004 Martin �berg, Sam Kothari, Robert Rainwater
// Copyright � 2004,2005,2006,2007 Joe Kucera
// Copyright � 2006,2007 [sss], chaos.persei, [sin], Faith Healer, Theif, nullbie
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
//
// -----------------------------------------------------------------------------
//
// File name      : $Source$
// Revision       : $Revision: 43 $
// Last change on : $Date: 2007-08-20 01:51:06 +0300 (Пн, 20 авг 2007) $
// Last change by : $Author: sss123next $
//
// DESCRIPTION:
//
//  ChangeInfo Plugin stuff
//
// -----------------------------------------------------------------------------

#include "../icqoscar.h"



int StringToListItemId(const char *szSetting,int def)
{
    int i,listCount;
    char szTmp[256];
    ListTypeDataItem *list;

    for(i=0; i<settingCount; i++)
        if(!strcmpnull(szSetting,setting[i].szDbSetting))
            break;

    if(i==settingCount) return def;

    list=(ListTypeDataItem*)setting[i].pList;
    listCount=setting[i].listCount;

    if(ICQGetContactStaticString(NULL, szSetting, szTmp, sizeof(szTmp)))
        return def;

    for(i=0; i<listCount; i++)
        if(!strcmpnull(list[i].szValue, szTmp)) break;

    if(i==listCount) return def;

    return list[i].id;
}



int UploadSettings(HWND hwndParent)
{
    PBYTE buf = NULL;
    int buflen = 0;
    /*  BYTE b;
      WORD w;*/

    if (!icqOnline)
    {
        MessageBoxUtf(hwndParent, LPGEN("You are not currently connected to the ICQ network. You must be online in order to update your information on the server."), LPGEN("Change ICQ Details"), MB_OK);
        return 0;
    }


    hUpload[0] = (HANDLE)IcqChangeInfoEx(CIXT_FULL, 0);

    //password
    {
        char* tmp;

        tmp = GetUserPassword(TRUE);
        if(tmp)
        {
            if (strlennull(Password) > 0 && strcmpnull(Password, tmp))
            {
                hUpload[1] = (HANDLE)icq_changeUserPasswordServ(tmp);


//        hUpload[1] = (HANDLE)icq_changeUserDirectoryInfoServ(buf,(WORD)buflen, DIRECTORYREQUEST_UPDATEOWNER);

                {
                    char szPwd[9] = {0};

                    if (!ICQGetContactStaticString(NULL, "Password", szPwd, 9) && strlennull(szPwd))
                    {
                        // password is stored in DB, update
                        char ptmp[9];

                        strcpy(ptmp, tmp);

                        CallService(MS_DB_CRYPT_ENCODESTRING, sizeof(ptmp), (LPARAM)ptmp);

                        ICQWriteContactSettingString(NULL, "Password", ptmp);
                    }
                }
            }
        }
    }

    mir_free(buf);

    return 1;
}

HANDLE hUpload[2];
