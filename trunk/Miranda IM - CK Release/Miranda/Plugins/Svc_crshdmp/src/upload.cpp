/*
Miranda Crash Dumper Plugin
Copyright (C) 2008 - 2009 Boris Krasnovskiy All Rights Reserved

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation version 2
of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "utils.h"
#include <m_netlib.h>

HANDLE hNetlibUser;

static void arrayToHex(BYTE* data, size_t datasz, char* res)
{
	char* resptr = res;
	for (unsigned i=0; i<datasz ; i++)
	{
		const BYTE ch = data[i];

		const char ch0 = (char)(ch >> 4);
		*resptr++ = (char)((ch0 <= 9) ? ('0' + ch0) : (('a' - 10) + ch0));

		const char ch1 = (char)(ch & 0xF);
		*resptr++ = (char)((ch1 <= 9) ? ('0' + ch1) : (('a' - 10) + ch1));
	}
	*resptr = '\0';
} 

void GetLoginStr(char* user, size_t szuser, char* pass)
{
	DBVARIANT dbv;

    if (DBGetContactSettingString(NULL, PluginName, "Username", &dbv) == 0)
	{
		mir_snprintf(user, szuser, "%s", dbv.pszVal);
		DBFreeVariant(&dbv);
	}
    else
        user[0] = 0;

    if (DBGetContactSettingString(NULL, PluginName, "Password", &dbv) == 0)
	{
		CallService(MS_DB_CRYPT_DECODESTRING, strlen(dbv.pszVal)+1, (LPARAM)dbv.pszVal);

		mir_md5_byte_t hash[16];
		mir_md5_state_t context;

		mir_md5_init(&context);
		mir_md5_append(&context, (BYTE*)dbv.pszVal, (int)strlen(dbv.pszVal));
		mir_md5_finish(&context, hash);

		arrayToHex(hash, sizeof(hash), pass);

		DBFreeVariant(&dbv);
	}
    else
        pass[0] = 0;
}

void OpenAuthUrl(const char* url)
{
    char user[64], pass[40];
    GetLoginStr(user, sizeof(user), pass);

    if (user[0] && pass[0])
    {
	    char str[256];

        mir_snprintf(str, sizeof(str), url, user);
        char* eurl = (char*)CallService(MS_NETLIB_URLENCODE, 0, (LPARAM)str);

        mir_snprintf(str, sizeof(str), "http://www.miranda-vi.org/cdlogin?name=%s&pass=%s&redir=%s", user, pass, eurl);
	    CallService(MS_UTILS_OPENURL, 1, (LPARAM)str);
        HeapFree(GetProcessHeap(), 0, eurl);
    }
    else
	    CallService(MS_UTILS_OPENURL, 1, (LPARAM)"http://www.miranda-vi.org/");
}

void CreateAuthString(char* auth)
{
    char user[64], pass[40];
    GetLoginStr(user, sizeof(user), pass);

	char str[110];
    int len = mir_snprintf(str, sizeof(str), "%s:%s", user, pass);

	strcpy(auth, "Basic ");
	NETLIBBASE64 nlb = { auth+6, 250, (PBYTE)str, len };
	CallService(MS_NETLIB_BASE64ENCODE, 0, LPARAM(&nlb));
}


bool InternetDownloadFile(const char *szUrl, VerTrnsfr* szReq) 
{
	int result = 0xBADBAD;
	char* szRedirUrl  = NULL;
	NETLIBHTTPREQUEST nlhr = {0};

	// initialize the netlib request
	nlhr.cbSize = sizeof(nlhr);
	nlhr.requestType = REQUEST_POST;
	nlhr.flags =  NLHRF_HTTP11 | NLHRF_NODUMP;
	nlhr.szUrl = (char*)szUrl;

	nlhr.headersCount = 6;
	nlhr.headers=(NETLIBHTTPHEADER*)mir_alloc(sizeof(NETLIBHTTPHEADER)*nlhr.headersCount);
	nlhr.headers[0].szName  = "Connection";
	nlhr.headers[0].szValue = "close";
	nlhr.headers[1].szName  = "Cache-Control";
	nlhr.headers[1].szValue = "no-cache";
	nlhr.headers[2].szName  = "Pragma";
	nlhr.headers[2].szValue = "no-cache";
	nlhr.headers[3].szName  = "Content-Type";
	nlhr.headers[3].szValue = "text/plain; charset=utf-8";
	nlhr.headers[4].szName  = "AutoUpload";
    nlhr.headers[4].szValue = (char*)(szReq->autot ? "1" : "0");
	nlhr.headers[5].szName  = "Authorization";

	char auth[256];
	CreateAuthString(auth);
	nlhr.headers[5].szValue = auth;

    nlhr.pData = szReq->buf;
    nlhr.dataLength = (int)strlen(szReq->buf);

	while (result == 0xBADBAD)
	{
		// download the page
		NETLIBHTTPREQUEST *nlhrReply = (NETLIBHTTPREQUEST*)CallService(MS_NETLIB_HTTPTRANSACTION,
			(WPARAM)hNetlibUser,(LPARAM)&nlhr);

		if (nlhrReply) 
		{
			int i;

			// if the recieved code is 200 OK
			switch(nlhrReply->resultCode)
			{
				case 200: 
					if (DBGetContactSettingByte(NULL, PluginName, "UploadChanged", 0))
						ProcessVIHash(true);

					for (i=nlhrReply->headersCount; i--; )
					{
						if (_stricmp(nlhrReply->headers[i].szName, "OldPlugins") == 0)
						{
							i = atoi(nlhrReply->headers[i].szValue);
							break;
						}
					}

					ShowMessage(1, TranslateT("VersionInfo upload successful,\n %d old plugins"), i);
					result = 0;
					break;

				case 401:
					ShowMessage(0, TranslateT("Cannot upload VersionInfo. Incorrect username or password"));
					result = 1;
					break;

				case 510:
					ShowMessage(0, TranslateT("Cannot upload VersionInfo. User is banned"));
					result = 1;
					break;

				case 511:
					ShowMessage(0, TranslateT("Cannot upload VersionInfo. Daily upload limit exceeded"));
					result = 1;
					break;

				case 301:
				case 302:
				case 307:
					// get the url for the new location and save it to szInfo
					// look for the reply header "Location"
					for (i=0; i<nlhrReply->headersCount; i++) 
					{
						if (!strcmp(nlhrReply->headers[i].szName, "Location")) 
						{
							size_t rlen = 0;
							if (nlhrReply->headers[i].szValue[0] == '/')
							{
								const char* szPath;
								const char* szPref = strstr(szUrl, "://");
								szPref = szPref ? szPref + 3 : szUrl;
								szPath = strchr(szPref, '/');
								rlen = szPath != NULL ? szPath - szUrl : strlen(szUrl); 
							}

							szRedirUrl = (char*)mir_realloc(szRedirUrl, 
								rlen + strlen(nlhrReply->headers[i].szValue)*3 + 1);

							strncpy(szRedirUrl, szUrl, rlen);
							strcpy(szRedirUrl+rlen, nlhrReply->headers[i].szValue); 
							
							nlhr.szUrl = szRedirUrl;
							break;
						}
					}
					break;

				default:
					result = 1;
					ShowMessage(0, TranslateT("Cannot upload VersionInfo. Unknown error"));
			}
		}
		else 
		{
			result = 1;
			ShowMessage(0, TranslateT("Cannot upload VersionInfo. Host unreachable."));
		}

		CallService(MS_NETLIB_FREEHTTPREQUESTSTRUCT,0,(LPARAM)nlhrReply);
	}

	mir_free(szRedirUrl);
	mir_free(nlhr.headers);

	return result == 0;
}

void __cdecl VersionInfoUploadThread(void* arg)
{
    VerTrnsfr* trn = (VerTrnsfr*)arg;
	InternetDownloadFile("http://www.miranda-vi.org/uploadpost", trn);
    mir_free(trn->buf);
	mir_free(trn);
}


void UploadInit(void) 
{
	NETLIBUSER nlu = {0};
	nlu.cbSize = sizeof(nlu);
	nlu.flags = NUF_OUTGOING | NUF_HTTPCONNS | NUF_NOHTTPSOPTION | NUF_TCHAR;
	nlu.szSettingsModule = (char*)PluginName;
	nlu.ptszDescriptiveName = TranslateT("Crash Dumper HTTP connections");
	hNetlibUser = (HANDLE)CallService(MS_NETLIB_REGISTERUSER, 0, (LPARAM)&nlu);
}

void UploadClose(void) 
{
	Netlib_CloseHandle(hNetlibUser);
}

bool ProcessVIHash(bool store)
{
	bkstring buffer;
	buffer.reserve(0x1800);
	PrintVersionInfo(buffer, 0);

	mir_md5_byte_t hash[16];
	mir_md5_state_t context;

	mir_md5_init(&context);
	mir_md5_append(&context, (PBYTE)buffer.c_str(), (int)buffer.sizebytes());
	mir_md5_finish(&context, hash);

	char hashstr[40];
	arrayToHex(hash, sizeof(hash), hashstr);

	bool result;
	if (store)
	{
		DBWriteContactSettingString(NULL, PluginName, "VIHash", hashstr);
		result = true;
	}
	else
	{
		DBVARIANT dbv;
		if (DBGetContactSettingString(NULL, PluginName, "VIHash", &dbv) == 0)
		{
			result = strcmp(hashstr, dbv.pszVal) == 0;
			DBFreeVariant(&dbv);
		}
		else
			result = false;
	}
	return result;
}
