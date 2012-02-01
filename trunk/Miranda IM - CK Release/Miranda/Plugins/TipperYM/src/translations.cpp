/*
Copyright (C) 2006-2007 Scott Ellis
Copyright (C) 2007-2011 Jan Holub

This is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this file; see the file license.txt.  If
not, write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  
*/

#include "common.h"
#include "translations.h"
#include "subst.h"
#include "str_utils.h"

int iTransFuncsCount = 0;
DBVTranslation *translations = 0;

DWORD dwNextFuncId;
HANDLE hServiceAdd;


void AddTranslation(DBVTranslation *newTrans) 
{
	iTransFuncsCount++;

	translations = (DBVTranslation *)mir_realloc(translations, sizeof(DBVTranslation) * iTransFuncsCount);
	translations[iTransFuncsCount - 1] = *newTrans;
	
	char *szName = mir_t2a(newTrans->swzName);
	char szSetting[256] = "Trans_";
	strcat(szSetting, szName);

	if (_tcscmp(newTrans->swzName, _T("[No translation]")) == 0) 
	{
		translations[iTransFuncsCount - 1].id = 0;
	} 
	else 
	{
		DWORD id = DBGetContactSettingDword(0, MODULE_ITEMS, szSetting, 0);
		if (id != 0) 
		{
			translations[iTransFuncsCount - 1].id = id;
			if (dwNextFuncId <= id) dwNextFuncId = id + 1;
		} 
		else
		{
			translations[iTransFuncsCount - 1].id = dwNextFuncId++;
			DBWriteContactSettingDword(0, MODULE_ITEMS, szSetting, translations[iTransFuncsCount - 1].id);
		}

		DBWriteContactSettingDword(0, MODULE_ITEMS, "NextFuncId", dwNextFuncId);
	}

	mir_free(szName);
}

TCHAR *NullTranslation(HANDLE hContact, const char *szModuleName, const char *szSettingName, TCHAR *buff, int bufflen) 
{
	if (DBGetContactSettingAsString(hContact, szModuleName, szSettingName, buff, bufflen))
		return buff;
	return NULL;
}

TCHAR *TimestampToShortDate(HANDLE hContact, const char *szModuleName, const char *szSettingName, TCHAR *buff, int bufflen) 
{
	DWORD ts = DBGetContactSettingDword(hContact, szModuleName, szSettingName, 0);
	if (ts == 0) return 0;
	
	DBTIMETOSTRINGT dbt = {0};
	dbt.cbDest = bufflen;
	dbt.szDest = buff;
	dbt.szFormat = _T("d");
	CallService(MS_DB_TIME_TIMESTAMPTOSTRINGT, (WPARAM)ts, (LPARAM)&dbt);
	return buff;
}

TCHAR *TimestampToLongDate(HANDLE hContact, const char *szModuleName, const char *szSettingName, TCHAR *buff, int bufflen) 
{
	DWORD ts = DBGetContactSettingDword(hContact, szModuleName, szSettingName, 0);
	if (ts == 0) return 0;
	
	DBTIMETOSTRINGT dbt = {0};
	dbt.cbDest = bufflen;
	dbt.szDest = buff;
	dbt.szFormat = _T("D");
	CallService(MS_DB_TIME_TIMESTAMPTOSTRINGT, (WPARAM)ts, (LPARAM)&dbt);
	return buff;
}

TCHAR *TimestampToTime(HANDLE hContact, const char *szModuleName, const char *szSettingName, TCHAR *buff, int bufflen) 
{
	DWORD ts = DBGetContactSettingDword(hContact, szModuleName, szSettingName, 0);
	if (ts == 0) return 0;
	
	DBTIMETOSTRINGT dbt = {0};
	dbt.cbDest = bufflen;
	dbt.szDest = buff;
	dbt.szFormat = _T("s");
	CallService(MS_DB_TIME_TIMESTAMPTOSTRINGT, (WPARAM)ts, (LPARAM)&dbt);
	return buff;
}

TCHAR *TimestampToTimeNoSecs(HANDLE hContact, const char *szModuleName, const char *szSettingName, TCHAR *buff, int bufflen) 
{
	DWORD ts = DBGetContactSettingDword(hContact, szModuleName, szSettingName, 0);
	if (ts == 0) return 0;
	
	DBTIMETOSTRINGT dbt = {0};
	dbt.cbDest = bufflen;
	dbt.szDest = buff;
	dbt.szFormat = _T("t");
	CallService(MS_DB_TIME_TIMESTAMPTOSTRINGT, (WPARAM)ts, (LPARAM)&dbt);
	return buff;
}

TCHAR *TimestampToTimeDifference(HANDLE hContact, const char *szModuleName, const char *szSettingName, TCHAR *buff, int bufflen) 
{
	DWORD ts = DBGetContactSettingDword(hContact, szModuleName, szSettingName, 0);
	DWORD t = (DWORD)time(0);
	if (ts == 0) return 0;
	
	DWORD diff = (ts > t) ? 0 : (t - ts);
	int d = (diff / 60 / 60 / 24);
	int h = (diff - d * 60 * 60 * 24) / 60 / 60;
	int m = (diff  - d * 60 * 60 * 24 - h * 60 * 60) / 60;
	if (d > 0)
		mir_sntprintf(buff, bufflen, TranslateT("%dd %dh %dm"), d, h, m);
	else if (h > 0)
		mir_sntprintf(buff, bufflen, TranslateT("%dh %dm"), h, m);
	else
		mir_sntprintf(buff, bufflen, TranslateT("%dm"), m);

	return buff;
}

TCHAR *SecondsToTimeDifference(HANDLE hContact, const char *szModuleName, const char *szSettingName, TCHAR *buff, int bufflen) 
{
	DWORD diff = DBGetContactSettingDword(hContact, szModuleName, szSettingName, 0);
	int d = (diff / 60 / 60 / 24);
	int h = (diff - d * 60 * 60 * 24) / 60 / 60;
	int m = (diff  - d * 60 * 60 * 24 - h * 60 * 60) / 60;
	if (d > 0)
		mir_sntprintf(buff, bufflen, TranslateT("%dd %dh %dm"), d, h, m);
	else if (h > 0)
		mir_sntprintf(buff, bufflen, TranslateT("%dh %dm"), h, m);
	else
		mir_sntprintf(buff, bufflen, TranslateT("%dm"), m);

	return buff;
}

TCHAR *WordToStatusDesc(HANDLE hContact, const char *szModuleName, const char *szSettingName, TCHAR *buff, int bufflen) 
{
	WORD wStatus = DBGetContactSettingWord(hContact, szModuleName, szSettingName, ID_STATUS_OFFLINE);
	char *szStatus = (char *)CallService(MS_CLIST_GETSTATUSMODEDESCRIPTION, (WPARAM)wStatus, (LPARAM)0);
	a2t(szStatus, buff, bufflen);
	buff[bufflen - 1] = 0;
	return buff;
}

TCHAR *ByteToYesNo(HANDLE hContact, const char *szModuleName, const char *szSettingName, TCHAR *buff, int bufflen) 
{
	DBVARIANT dbv;
	if (!DBGetContactSetting(hContact, szModuleName, szSettingName, &dbv))
	{
		if (dbv.type == DBVT_BYTE)
		{
			if (dbv.bVal != 0)
				_tcsncpy(buff, _T("Yes"), bufflen);
			else
				_tcsncpy(buff, _T("No"), bufflen);
			buff[bufflen - 1] = 0;
			DBFreeVariant(&dbv);
			return buff;
		}
		DBFreeVariant(&dbv);
	}
	return 0;
}

TCHAR *ByteToGender(HANDLE hContact, const char *szModuleName, const char *szSettingName, TCHAR *buff, int bufflen) 
{
	BYTE val = (BYTE)DBGetContactSettingByte(hContact, szModuleName, szSettingName, 0);
	if (val == 'F')
		_tcsncpy(buff, TranslateT("Female"), bufflen);
	else if (val == 'M')
		_tcsncpy(buff, TranslateT("Male"), bufflen);
	else
		return 0;

	buff[bufflen - 1] = 0;
	return buff;
}

TCHAR *WordToCountry(HANDLE hContact, const char *szModuleName, const char *szSettingName, TCHAR *buff, int bufflen) 
{
	char *szCountryName = 0;
	WORD cid = (WORD)DBGetContactSettingWord(hContact, szModuleName, szSettingName, (WORD)-1);
	if (cid != (WORD)-1 && ServiceExists(MS_UTILS_GETCOUNTRYBYNUMBER) && (szCountryName = (char *)CallService(MS_UTILS_GETCOUNTRYBYNUMBER, cid, 0)) != 0)
	{
		if (strcmp(szCountryName, "Unknown") == 0)
			return 0;
		a2t(szCountryName, buff, bufflen);
		buff[bufflen - 1] = 0;
		return buff;
	}
	return 0;
}

TCHAR *DwordToIp(HANDLE hContact, const char *szModuleName, const char *szSettingName, TCHAR *buff, int bufflen) 
{
	DWORD ip = DBGetContactSettingDword(hContact, szModuleName, szSettingName, 0);
	if (ip) {
		unsigned char *ipc = (unsigned char*)&ip;
		mir_sntprintf(buff, bufflen, _T("%u.%u.%u.%u"), ipc[3], ipc[2], ipc[1], ipc[0]);
		return buff;
	}
	return 0;
}

bool GetInt(const DBVARIANT &dbv, int *iVal) 
{
	if (!iVal) return false;

	switch(dbv.type) 
	{
		case DBVT_BYTE:
			if (iVal) *iVal = (int)dbv.bVal;
			return true;
		case DBVT_WORD:
			if (iVal) *iVal = (int)dbv.wVal;
			return true;
		case DBVT_DWORD:
			if (iVal) *iVal = (int)dbv.dVal;
			return true;
	}
	return false;
}

TCHAR *DayMonthYearToDate(HANDLE hContact, const char *szModuleName, const char *prefix, TCHAR *buff, int bufflen) 
{
	DBVARIANT dbv;
	char szSettingName[256];
	mir_snprintf(szSettingName, 256, "%sDay", prefix);
	if (!DBGetContactSetting(hContact, szModuleName, szSettingName, &dbv)) 
	{
		int day = 0;
		if (GetInt(dbv, &day))
		{
			DBFreeVariant(&dbv);
			mir_snprintf(szSettingName, 256, "%sMonth", prefix);
			int month = 0;
			if (!DBGetContactSetting(hContact, szModuleName, szSettingName, &dbv))
			{
				if (GetInt(dbv, &month))
				{
					DBFreeVariant(&dbv);
					mir_snprintf(szSettingName, 256, "%sYear", prefix);
					int year = 0;
					if (!DBGetContactSetting(hContact, szModuleName, szSettingName, &dbv)) 
					{
						if (GetInt(dbv, &year))
						{
							DBFreeVariant(&dbv);

							SYSTEMTIME st = {0};
							st.wDay = day;
							st.wMonth = month;
							st.wYear = year;

							GetDateFormat(LOCALE_USER_DEFAULT, 0, &st, 0, buff, bufflen); 
							return buff;
						} 
						else 
							DBFreeVariant(&dbv);
					}
				} 
				else 
					DBFreeVariant(&dbv);
			}
		} 
		else 
			DBFreeVariant(&dbv);
	}
	return 0;
}

TCHAR *DayMonthYearToAge(HANDLE hContact, const char *szModuleName, const char *szPrefix, TCHAR *buff, int bufflen) 
{
	DBVARIANT dbv;
	char szSettingName[256];
	mir_snprintf(szSettingName, 256, "%sDay", szPrefix);
	if (!DBGetContactSetting(hContact, szModuleName, szSettingName, &dbv)) 
	{
		int day = 0;
		if (GetInt(dbv, &day))
		{
			DBFreeVariant(&dbv);
			mir_snprintf(szSettingName, 256, "%sMonth", szPrefix);
			int month = 0;
			if (!DBGetContactSetting(hContact, szModuleName, szSettingName, &dbv)) 
			{
				if (GetInt(dbv, &month))
				{
					DBFreeVariant(&dbv);
					mir_snprintf(szSettingName, 256, "%sYear", szPrefix);
					int year = 0;
					if (!DBGetContactSetting(hContact, szModuleName, szSettingName, &dbv)) 
					{
						if (GetInt(dbv, &year)) 
						{
							DBFreeVariant(&dbv);

							SYSTEMTIME now;
							GetLocalTime(&now);

							int age = now.wYear - year;
							if (now.wMonth < month || (now.wMonth == month && now.wDay < day))
								age--;
							mir_sntprintf(buff, bufflen, _T("%d"), age);
							return buff;
						} 
						else
							DBFreeVariant(&dbv);
					}
				} 
				else
					DBFreeVariant(&dbv);
			}
		} 
		else
			DBFreeVariant(&dbv);
	}
	return 0;
}

TCHAR *HoursMinutesSecondsToTime(HANDLE hContact, const char *szModuleName, const char *szPrefix, TCHAR *buff, int bufflen) 
{
	DBVARIANT dbv;
	char szSettingName[256];
	mir_snprintf(szSettingName, 256, "%sHours", szPrefix);
	if (!DBGetContactSetting(hContact, szModuleName, szSettingName, &dbv)) 
	{
		int hours = 0;
		if (GetInt(dbv, &hours))
		{
			DBFreeVariant(&dbv);
			mir_snprintf(szSettingName, 256, "%sMinutes", szPrefix);
			int minutes = 0;
			if (!DBGetContactSetting(hContact, szModuleName, szSettingName, &dbv))
			{
				if (GetInt(dbv, &minutes))
				{
					DBFreeVariant(&dbv);
					mir_snprintf(szSettingName, 256, "%sSeconds", szPrefix);
					int seconds = 0;
					if (!DBGetContactSetting(hContact, szModuleName, szSettingName, &dbv)) 
					{
						GetInt(dbv, &seconds);
						DBFreeVariant(&dbv);
					}

					SYSTEMTIME st = {0};
					st.wHour = hours;
					st.wMinute = minutes;
					st.wSecond = seconds;

					GetTimeFormat(LOCALE_USER_DEFAULT, 0, &st, 0, buff, bufflen); 
					return buff;
				} 
				else
					DBFreeVariant(&dbv);
			}
		} 
		else
			DBFreeVariant(&dbv);
	}
	return 0;
}

TCHAR *HoursMinutesToTime(HANDLE hContact, const char *szModuleName, const char *szPrefix, TCHAR *buff, int bufflen) 
{
	DBVARIANT dbv;
	char szSettingName[256];
	mir_snprintf(szSettingName, 256, "%sHours", szPrefix);
	if (!DBGetContactSetting(hContact, szModuleName, szSettingName, &dbv)) 
	{
		int hours = 0;
		if (GetInt(dbv, &hours)) 
		{
			DBFreeVariant(&dbv);
			mir_snprintf(szSettingName, 256, "%sMinutes", szPrefix);
			int minutes = 0;
			if (!DBGetContactSetting(hContact, szModuleName, szSettingName, &dbv))
			{
				if (GetInt(dbv, &minutes))
				{
					DBFreeVariant(&dbv);

					SYSTEMTIME st = {0};
					st.wHour = hours;
					st.wMinute = minutes;

					GetTimeFormat(LOCALE_USER_DEFAULT, TIME_NOSECONDS, &st, 0, buff, bufflen); 
					return buff;
				} 
				else
					DBFreeVariant(&dbv);
			}
		} 
		else
			DBFreeVariant(&dbv);
	}
	return 0;
}

TCHAR *DayMonthYearHoursMinutesSecondsToTimeDifference(HANDLE hContact, const char *szModuleName, const char *szPrefix, TCHAR *buff, int bufflen) 
{
	DBVARIANT dbv;
	char szSettingName[256];
	mir_snprintf(szSettingName, 256, "%sDay", szPrefix);
	if (!DBGetContactSetting(hContact, szModuleName, szSettingName, &dbv))
	{
		int day = 0;
		if (GetInt(dbv, &day))
		{
			DBFreeVariant(&dbv);
			mir_snprintf(szSettingName, 256, "%sMonth", szPrefix);
			int month = 0;
			if (!DBGetContactSetting(hContact, szModuleName, szSettingName, &dbv)) 
			{
				if (GetInt(dbv, &month)) 
				{
					DBFreeVariant(&dbv);
					mir_snprintf(szSettingName, 256, "%sYear", szPrefix);
					int year = 0;
					if (!DBGetContactSetting(hContact, szModuleName, szSettingName, &dbv)) 
					{
						if (GetInt(dbv, &year)) 
						{
							DBFreeVariant(&dbv);
							mir_snprintf(szSettingName, 256, "%sHours", szPrefix);
							if (!DBGetContactSetting(hContact, szModuleName, szSettingName, &dbv))
							{
								int hours = 0;
								if (GetInt(dbv, &hours))
								{
									DBFreeVariant(&dbv);
									mir_snprintf(szSettingName, 256, "%sMinutes", szPrefix);
									int minutes = 0;
									if (!DBGetContactSetting(hContact, szModuleName, szSettingName, &dbv))
									{
										if (GetInt(dbv, &minutes)) 
										{
											DBFreeVariant(&dbv);
											mir_snprintf(szSettingName, 256, "%sSeconds", szPrefix);
											int seconds = 0;
											if (!DBGetContactSetting(hContact, szModuleName, szSettingName, &dbv)) 
											{
												GetInt(dbv, &seconds);
												DBFreeVariant(&dbv);
											}

											SYSTEMTIME st = {0}, st_now;
											st.wDay = day;
											st.wMonth = month;
											st.wYear = year;
											st.wHour = hours;
											st.wMinute = minutes;
											st.wSecond = seconds;
											GetLocalTime(&st_now);

											FILETIME ft, ft_now;
											SystemTimeToFileTime(&st, &ft);
											SystemTimeToFileTime(&st_now, &ft_now);
											
											LARGE_INTEGER li, li_now;
											li.HighPart = ft.dwHighDateTime; li.LowPart = ft.dwLowDateTime;
											li_now.HighPart = ft_now.dwHighDateTime; li_now.LowPart = ft_now.dwLowDateTime;

											long diff = (long)((li_now.QuadPart - li.QuadPart) / (LONGLONG)10000000L);
											int y = diff / 60 / 60 / 24 / 365;
											int d = (diff - y * 60 * 60 * 24 * 365) / 60 / 60 / 24;
											int h = (diff - y * 60 * 60 * 24 * 365 - d * 60 * 60 * 24) / 60 / 60;
											int m = (diff - y * 60 * 60 * 24 * 365  - d * 60 * 60 * 24 - h * 60 * 60) / 60;
											if (y != 0)
												mir_sntprintf(buff, bufflen, TranslateT("%dy %dd %dh %dm"), y, d, h, m);
											else if (d != 0)
												mir_sntprintf(buff, bufflen, TranslateT("%dd %dh %dm"), d, h, m);
											else if (h != 0)
												mir_sntprintf(buff, bufflen, TranslateT("%dh %dm"), h, m);
											else
												mir_sntprintf(buff, bufflen, TranslateT("%dm"), m);

											return buff;
										} 
										else
											DBFreeVariant(&dbv);
									}
								} 
								else
									DBFreeVariant(&dbv);
							}
						} 
						else
							DBFreeVariant(&dbv);
					}
				} 
				else
					DBFreeVariant(&dbv);
			}
		} 
		else
			DBFreeVariant(&dbv);
	}
	return 0;
}

TCHAR *DayMonthToDaysToNextBirthday(HANDLE hContact, const char *szModuleName, const char *szPrefix, TCHAR *buff, int bufflen) 
{
	DBVARIANT dbv;
	char szSettingName[256];
	mir_snprintf(szSettingName, 256, "%sDay", szPrefix);
	if (!DBGetContactSetting(hContact, szModuleName, szSettingName, &dbv)) 
	{
		int day = 0;
		if (GetInt(dbv, &day))
		{
			DBFreeVariant(&dbv);
			mir_snprintf(szSettingName, 256, "%sMonth", szPrefix);
			int month = 0;
			if (!DBGetContactSetting(hContact, szModuleName, szSettingName, &dbv)) 
			{
				if (GetInt(dbv, &month)) 
				{
					DBFreeVariant(&dbv);
					time_t now = time(NULL);
					struct tm *ti = localtime(&now);
					int yday_now = ti->tm_yday;

					ti->tm_mday = day;
					ti->tm_mon = month - 1;
					mktime(ti);

					int yday_birth = ti->tm_yday;
					if (yday_birth < yday_now)
					{
						yday_now -= 365;
						yday_now -= (ti->tm_year % 4) ? 0 : 1; 
					}

					int diff = yday_birth - yday_now;
					mir_sntprintf(buff, bufflen, TranslateT("%dd"), diff);
					
					return buff;
				}
				else
				{
					DBFreeVariant(&dbv);
				}
			}
		} 
		else
		{
			DBFreeVariant(&dbv);
		}
	}
	return 0;
}


TCHAR *EmptyXStatusToDefaultName(HANDLE hContact, const char *szModuleName, const char *szSettingName, TCHAR *buff, int bufflen) 
{
	TCHAR szDefaultName[1024];
	ICQ_CUSTOM_STATUS xstatus = {0};
	DBVARIANT dbv;

	// translate jabber mood
	if (ProtoServiceExists(szModuleName, "/SendXML")) // jabber protocol?
	{ 
		if (!DBGetContactSettingTString(hContact, szModuleName, szSettingName, &dbv))
		{
			_tcsncpy(buff, TranslateTS(dbv.ptszVal), bufflen);
			buff[bufflen - 1] = 0;
			return buff;
		}
	}

	if (NullTranslation(hContact, szModuleName, szSettingName, buff, bufflen))
	   return buff;
	
	int status = DBGetContactSettingByte(hContact, szModuleName, "XStatusId", 0);
	if (!status) return 0;
	
	if (ProtoServiceExists(szModuleName, PS_ICQ_GETCUSTOMSTATUSEX))
	{ 
		xstatus.cbSize = sizeof(ICQ_CUSTOM_STATUS);
		xstatus.flags = CSSF_MASK_NAME | CSSF_DEFAULT_NAME | CSSF_TCHAR;
		xstatus.ptszName = szDefaultName;
		xstatus.wParam = (WPARAM *)&status;
		if (CallProtoService(szModuleName, PS_ICQ_GETCUSTOMSTATUSEX, 0, (LPARAM)&xstatus))
		   return 0;
		
		_tcsncpy(buff, TranslateTS(szDefaultName), bufflen);
		buff[bufflen - 1] = 0;
		return buff;
	} 

	return 0;
}

TCHAR *TimezoneToTime(HANDLE hContact, const char *szModuleName, const char *szSettingName, TCHAR *buff, int bufflen) 
{
	int timezone = DBGetContactSettingByte(hContact,szModuleName,szSettingName,256);
	if (timezone==256 || (char)timezone==-100) 
		return 0;

	TIME_ZONE_INFORMATION tzi;
	FILETIME ft;
	LARGE_INTEGER lift;
	SYSTEMTIME st;

	timezone=(char)timezone;
	GetSystemTimeAsFileTime(&ft);
	if (GetTimeZoneInformation(&tzi) == TIME_ZONE_ID_DAYLIGHT)
		timezone += tzi.DaylightBias / 30;

	lift.QuadPart = *(__int64*)&ft;
	lift.QuadPart -= (__int64)timezone * BIGI(30) * BIGI(60) * BIGI(10000000);
	*(__int64*)&ft = lift.QuadPart;
	FileTimeToSystemTime(&ft, &st);
	GetTimeFormat(LOCALE_USER_DEFAULT, TIME_NOSECONDS, &st, NULL, buff, bufflen);

	return buff;
}

TCHAR *ByteToDay(HANDLE hContact, const char *szModuleName, const char *szSettingName, TCHAR *buff, int bufflen) 
{
	int iDay = DBGetContactSettingWord(hContact, szModuleName, szSettingName, -1);
	if (iDay > -1 && iDay < 7)
	{
		a2t(Translate(days[iDay]), buff, bufflen);
		buff[bufflen - 1] = 0;
		return buff;
	}

	return 0;
}

TCHAR *ByteToMonth(HANDLE hContact, const char *szModuleName, const char *szSettingName, TCHAR *buff, int bufflen) 
{
	int iMonth = DBGetContactSettingWord(hContact, szModuleName, szSettingName, 0);
	if (iMonth > 0 && iMonth < 13) 
	{
		a2t(Translate(months[iMonth - 1]), buff, bufflen);
		buff[bufflen - 1] = 0;
		return buff;
	}

	return 0;
}

TCHAR *ByteToLanguage(HANDLE hContact, const char *szModuleName, const char *szSettingName, TCHAR *buff, int bufflen) 
{
	int iLang = DBGetContactSettingByte(hContact, szModuleName, szSettingName, 0);
	if (iLang)
	{
		for (int i = 0; i < SIZEOF(languages); i++)
		{
			if (iLang == languages[i].id)
			{
				a2t(Translate(languages[i].szValue), buff, bufflen);
				buff[bufflen - 1] = 0;
				return buff;
			}
		}
	}

	return 0;
}

INT_PTR ServiceAddTranslation(WPARAM wParam, LPARAM lParam) 
{
	if (!lParam) return 1;

	DBVTranslation *trans = (DBVTranslation *)lParam;
	AddTranslation(trans);

	return 0;
}

#define INT_TRANS_COUNT 23
void InitTranslations() 
{
	dwNextFuncId = DBGetContactSettingDword(0, MODULE_ITEMS, "NextFuncId", 1);
	DBVTranslation internalTranslations[INT_TRANS_COUNT] = 
	{
		{
			(TranslateFunc *)NullTranslation,
			_T("[No translation]"),
		},
		{
			(TranslateFunc *)WordToStatusDesc,
			_T("WORD to status description")
		},
		{
			(TranslateFunc *)TimestampToTime,
			_T("DWORD timestamp to time")
		},
		{
			(TranslateFunc *)TimestampToTimeDifference,
			_T("DWORD timestamp to time difference")
		},
		{
			(TranslateFunc *)ByteToYesNo,
			_T("BYTE to Yes/No")
		},
		{
			(TranslateFunc *)ByteToGender,
			_T("BYTE to Male/Female (ICQ)")
		},
		{
			(TranslateFunc *)WordToCountry,
			_T("WORD to country name")
		},
		{
			(TranslateFunc *)DwordToIp,
			_T("DWORD to ip address")
		},
		{
			(TranslateFunc *)DayMonthYearToDate,
			_T("<prefix>Day|Month|Year to date")
		},
		{
			(TranslateFunc *)DayMonthYearToAge,
			_T("<prefix>Day|Month|Year to age")
		},
		{
			(TranslateFunc *)HoursMinutesSecondsToTime,
			_T("<prefix>Hours|Minutes|Seconds to time")
		},
		{
			(TranslateFunc *)DayMonthYearHoursMinutesSecondsToTimeDifference,
			_T("<prefix>Day|Month|Year|Hours|Minutes|Seconds to time difference")
		},
		{
			(TranslateFunc *)DayMonthToDaysToNextBirthday,
			_T("<prefix>Day|Month to days to next birthday")
		},
		{
			(TranslateFunc *)TimestampToTimeNoSecs,
			_T("DWORD timestamp to time (no seconds)")
		},
		{
			(TranslateFunc *)HoursMinutesToTime,
			_T("<prefix>Hours|Minutes to time")
		},
		{
			(TranslateFunc *)TimestampToShortDate,
			_T("DWORD timestamp to date (short)")
		},
		{
			(TranslateFunc *)TimestampToLongDate,
			_T("DWORD timestamp to date (long)")
		},
		{
			(TranslateFunc *)EmptyXStatusToDefaultName,
			_T("xStatus: empty xStatus name to default name")
		},
		{
			(TranslateFunc *)SecondsToTimeDifference,
			_T("DWORD seconds to time difference")
		},
		{
			(TranslateFunc *)TimezoneToTime,
			_T("BYTE timezone to time")
		},
		{
			(TranslateFunc *)ByteToDay,
			_T("WORD to name of a day (0..6, 0 is Sunday)")
		},
		{
			(TranslateFunc *)ByteToMonth,
			_T("WORD to name of a month (1..12, 1 is January)")
		},
		{
			(TranslateFunc *)ByteToLanguage,
			_T("BYTE to language (ICQ)")
		}
	};

	for (int i = 0; i < INT_TRANS_COUNT; i++) 
		AddTranslation(&internalTranslations[i]);

	hServiceAdd = CreateServiceFunction(MS_TIPPER_ADDTRANSLATION, ServiceAddTranslation);
}

void DeinitTranslations() 
{
	DestroyServiceFunction(hServiceAdd);
	mir_free(translations);
}

