/*
UserinfoEx plugin for Miranda IM

Copyright:
� 2006-2010 DeathAxe, Yasnovidyashii, Merlin, K. Romanov, Kreol

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

===============================================================================

File name      : $HeadURL: http://userinfoex.googlecode.com/svn/trunk/mir_icolib.cpp $
Revision       : $Revision: 187 $
Last change on : $Date: 2010-09-08 16:05:54 +0400 (Ср, 08 сен 2010) $
Last change by : $Author: ing.u.horn $

===============================================================================
*/

#include "commonheaders.h"

typedef struct _ICODESC 
{
	LPSTR	pszName;
	LPSTR	pszDesc;
	LPSTR	pszSection;
	WORD	idResource;
	BYTE	size;
} ICODESC;

HICON ghDefIcon = NULL;

static ICODESC icoDesc[] = 
{
	// common
	{ ICO_COMMON_IM,		"IM Naming",				SECT_COMMON,	IDI_MIRANDA,			0	},
	{ ICO_COMMON_PASSWORD,	"Password",					SECT_COMMON,	IDI_PASSWORD,			0	},
	{ ICO_COMMON_FEMALE,	"Female",					SECT_COMMON,	IDI_FEMALE,				0	},
	{ ICO_COMMON_MALE,		"Male",						SECT_COMMON,	IDI_MALE,				0	},
	{ ICO_COMMON_BIRTHDAY,	"Birthday",					SECT_COMMON,	IDI_BIRTHDAY,			0	},
	{ ICO_COMMON_CLOCK,		"Timezone",					SECT_COMMON,	IDI_CLOCK,				1	},
	{ ICO_COMMON_MARITAL,	"Marital status",			SECT_COMMON,	IDI_MARITAL,			0	},
	{ ICO_COMMON_ADDRESS,	"Address",					SECT_COMMON,	IDI_TREE_ADDRESS,		0	},
	{ ICO_COMMON_ANNIVERSARY,"Anniversary",				SECT_COMMON,	IDI_ANNIVERSARY,		0	},

	//zodiac
	{ ICO_ZOD_AQUARIUS,		"Aquarius",					SECT_COMMON,	IDI_ZOD_AQUARIUS,		128	},
	{ ICO_ZOD_ARIES,		"Aries",					SECT_COMMON,	IDI_ZOD_ARIES,			128	},
	{ ICO_ZOD_CANCER,		"Cancer",					SECT_COMMON,	IDI_ZOD_CANCER,			128	},
	{ ICO_ZOD_CAPRICORN,	"Capricorn",				SECT_COMMON,	IDI_ZOD_CAPRICORN,		128	},
	{ ICO_ZOD_GEMINI,		"Gemini",					SECT_COMMON,	IDI_ZOD_GEMINI,			128	},
	{ ICO_ZOD_LEO,			"Leo",						SECT_COMMON,	IDI_ZOD_LEO,			128	},
	{ ICO_ZOD_LIBRA,		"Libra",					SECT_COMMON,	IDI_ZOD_LIBRA,			128	},
	{ ICO_ZOD_PISCES,		"Pisces",					SECT_COMMON,	IDI_ZOD_PISCES,			128	},
	{ ICO_ZOD_SAGITTARIUS,	"Sagittarius",				SECT_COMMON,	IDI_ZOD_SAGITTARIUS,	128	},
	{ ICO_ZOD_SCORPIO,		"Scorpio",					SECT_COMMON,	IDI_ZOD_SCORPIO,		128	},
	{ ICO_ZOD_TAURUS,		"Taurus",					SECT_COMMON,	IDI_ZOD_TAURUS,			128	},
	{ ICO_ZOD_VIRGO,		"Virgo",					SECT_COMMON,	IDI_ZOD_VIRGO,			128	},
	// later with new icon pack version
	//{ ICO_ZOD_UNKNOWN,		"Unknown",					SECT_COMMON,	IDI_ZOD_UNKNOWN,		128	},

	// lists
	{ ICO_LST_MODULES,		"Export: Modules",			SECT_COMMON,	IDI_LST_MODULES,		0	},
	{ ICO_LST_FOLDER,		"Export: Folder",			SECT_COMMON,	IDI_LST_FOLDER,			0	},
	{ ICO_TREE_DEFAULT,		"Default",					SECT_TREE,		IDI_LST_FOLDER,			0	},

	// dialogs
	{ ICO_DLG_DETAILS,		"Details Infobar",			SECT_DLG,		IDI_DLG_DETAILS,		48	},
	{ ICO_DLG_PHONE,		"Phone Infobar",			SECT_DLG,		IDI_DLG_PHONE,			1	},
	{ ICO_DLG_EMAIL,		"E-Mail Infobar",			SECT_DLG,		IDI_DLG_EMAIL,			1	},
	{ ICO_DLG_EXPORT,		"Export VCard",				SECT_DLG,		IDI_EXPORT,				1	},
	{ ICO_DLG_IMPORT,		"Import VCard",				SECT_DLG,		IDI_IMPORT,				1	},
	{ ICO_DLG_ANNIVERSARY,	"Anniversary Infobar",		SECT_DLG,		IDI_ANNIVERSARY,		1	},

	// button icons
	{ ICO_BTN_UPDATE,		"Update",					SECT_BUTTONS,	IDI_BTN_UPDATE,			0	},
	{ ICO_BTN_IMPORT,		"Import",					SECT_BUTTONS,	IDI_IMPORT,				0	},
	{ ICO_BTN_EXPORT,		"Export",					SECT_BUTTONS,	IDI_EXPORT,				0	},
	{ ICO_BTN_OK,			"Ok",						SECT_BUTTONS,	IDI_BTN_OK,				0	},
	{ ICO_BTN_CANCEL,		"Cancel",					SECT_BUTTONS,	IDI_BTN_CLOSE,			0	},
	{ ICO_BTN_APPLY,		"Apply",					SECT_BUTTONS,	IDI_BTN_APPLY,			0	},
	{ ICO_BTN_GOTO,			"Goto",						SECT_BUTTONS,	IDI_BTN_GOTO,			0	},
	{ ICO_BTN_PHONE,		"Phone",					SECT_BUTTONS,	IDI_BTN_PHONE,			0	},
	{ ICO_BTN_FAX,			"Fax",						SECT_BUTTONS,	IDI_BTN_FAX,			0	},
	{ ICO_BTN_CELLULAR,		"Cellular",					SECT_BUTTONS,	IDI_BTN_CELLULAR,		0	},
	{ ICO_BTN_CUSTOMPHONE,	"Custom Phone",				SECT_BUTTONS,	IDI_BTN_CUSTOMPHONE,	0	},
	{ ICO_BTN_EMAIL,		"e-mail",					SECT_BUTTONS,	IDI_BTN_EMAIL,			0	},
	{ ICO_BTN_DOWNARROW,	"Down arrow",				SECT_BUTTONS,	IDI_BTN_DOWNARROW,		0	},
	{ ICO_BTN_ADD,			"Add",						SECT_BUTTONS,	IDI_BTN_ADD,			0	},
	{ ICO_BTN_EDIT,			"Edit",						SECT_BUTTONS,	IDI_BTN_EDIT,			0	},
	{ ICO_BTN_DELETE,		"Delete",					SECT_BUTTONS,	IDI_BTN_DELETE,			0	},
	{ ICO_BTN_SEARCH,		"Search",					SECT_BUTTONS,	IDI_SEARCH,				0	},
	{ ICO_BTN_EXIMPORT,		"Ex-/Import",				SECT_BUTTONS,	IDI_BTN_EXIMPORT,		0	},
	{ ICO_BTN_BDAY_BACKUP,	"Backup Birthday",			SECT_BUTTONS,	IDI_BTN_BIRTHDAY_BACKUP,0	},
	//{ ICO_BTN_YES,			"Yes",					SECT_BUTTONS,	IDI_BTN_YES,			0	},
	//{ ICO_BTN_NO,			"No",						SECT_BUTTONS,	IDI_BTN_NO,				0	},
	//{ ICO_BTN_IGNORE,		"Ignore",					SECT_BUTTONS,	IDI_BTN_IGNORE,			0	},

	//birthday and anniversary
	{ ICO_RMD_DTB0,			"Birthday today",			SECT_REMIND,	IDI_RMD_DTB0,			0	},
	{ ICO_RMD_DTB1,			"Birthday tomorrow",		SECT_REMIND,	IDI_RMD_DTB1,			0	},
	{ ICO_RMD_DTB2,			"Birthday in 2 days",		SECT_REMIND,	IDI_RMD_DTB2,			0	},
	{ ICO_RMD_DTB3,			"Birthday in 3 days",		SECT_REMIND,	IDI_RMD_DTB3,			0	},
	{ ICO_RMD_DTB4,			"Birthday in 4 days",		SECT_REMIND,	IDI_RMD_DTB4,			0	},
	{ ICO_RMD_DTB5,			"Birthday in 5 days",		SECT_REMIND,	IDI_RMD_DTB5,			0	},
	{ ICO_RMD_DTB6,			"Birthday in 6 days",		SECT_REMIND,	IDI_RMD_DTB6,			0	},
	{ ICO_RMD_DTB7,			"Birthday in 7 days",		SECT_REMIND,	IDI_RMD_DTB7,			0	},
	{ ICO_RMD_DTB8,			"Birthday in 8 days",		SECT_REMIND,	IDI_RMD_DTB8,			0	},
	{ ICO_RMD_DTB9,			"Birthday in 9 days",		SECT_REMIND,	IDI_RMD_DTB9,			0	},
	{ ICO_RMD_DTBX,			"Birthday later",			SECT_REMIND,	IDI_RMD_DTBX,			0	},
		
	{ ICO_RMD_DTA0,			"Anniversary today",		SECT_REMIND,	IDI_RMD_DTA0,			0	},
	{ ICO_RMD_DTA1,			"Anniversary tomorrow",		SECT_REMIND,	IDI_RMD_DTA1,			0	},
	{ ICO_RMD_DTA2,			"Anniversary in 2 days",	SECT_REMIND,	IDI_RMD_DTA2,			0	},
	{ ICO_RMD_DTA3,			"Anniversary in 3 days",	SECT_REMIND,	IDI_RMD_DTA3,			0	},
	{ ICO_RMD_DTA4,			"Anniversary in 4 days",	SECT_REMIND,	IDI_RMD_DTA4,			0	},
	{ ICO_RMD_DTA5,			"Anniversary in 5 days",	SECT_REMIND,	IDI_RMD_DTA5,			0	},
	{ ICO_RMD_DTA6,			"Anniversary in 6 days",	SECT_REMIND,	IDI_RMD_DTA6,			0	},
	{ ICO_RMD_DTA7,			"Anniversary in 7 days",	SECT_REMIND,	IDI_RMD_DTA7,			0	},
	{ ICO_RMD_DTA8,			"Anniversary in 8 days",	SECT_REMIND,	IDI_RMD_DTA8,			0	},
	{ ICO_RMD_DTA9,			"Anniversary in 9 days",	SECT_REMIND,	IDI_RMD_DTA9,			0	},
	{ ICO_RMD_DTAX,			"Anniversary later",		SECT_REMIND,	IDI_RMD_DTAX,			0	},
};

/**
 * This function finds the default iconpack file and return its path.
 *
 * @param		- none
 *
 * @return		This function returns the relative path to an existing icon pack.
 **/
LPTSTR IcoLib_GetDefaultIconFileName()
{
	static LPTSTR	path[] = {
		_T("Icons\\uinfoex_icons.dll"),
		_T("Plugins\\uinfoex_icons.dll"),
		_T("Customize\\Icons\\uinfoex_icons.dll")
	};
	TCHAR absolute[MAX_PATH];

	for (INT i = 0; i < SIZEOF(path); i++)
	{
		CallService(MS_UTILS_PATHTOABSOLUTET, (WPARAM)path[i], (LPARAM)absolute);
		if (PathFileExists(absolute))
		{
			return path[i];
		}
	}
	return NULL;
}

/**
 * This function checks the version of an iconpack.
 * If the icon pack's version differs from the desired one, 
 * dialog with a warning is displayed.
 *
 * @param		szIconPack	- This is the path to the icon pack. 
 *							  It can be absolute or relative.
 *
 * @return	nothing
 **/
static VOID IcoLib_CheckIconPackVersion(LPTSTR szIconPack)
{
	if (DB::Setting::GetByte(SET_ICONS_CHECKFILEVERSION, TRUE))
	{
		if (szIconPack)
		{
			TCHAR		szAbsolutePath[MAX_PATH];
			HMODULE hIconDll;

			CallService(MS_UTILS_PATHTOABSOLUTET, (WPARAM)szIconPack, (LPARAM)szAbsolutePath);

			hIconDll = LoadLibrary(szAbsolutePath);
			if (hIconDll) 
			{
				CHAR szFileVersion[64];

				if (!LoadStringA(hIconDll, IDS_ICOPACKVERSION, szFileVersion, sizeof(szFileVersion)) ||
						mir_strcmp(szFileVersion, "__UserInfoEx_IconPack_1.2__"))
				{
					MsgErr(NULL, LPGENT("Warning: Your current IconPack's version differs from the one UserInfoEx is designed for.\nSome icons may not be displayed correctly"));
				}
				FreeLibrary(hIconDll);
			}
		}
		else
		{
			MsgErr(NULL, LPGENT("Warning: No IconPack found in one of the following directories: 'customize\\icons', 'icons' or 'plugins'!"));
		}
	}
}

/**
 * Returns a icon, identified by a name
 *
 * @param	pszIcon	- name of the icon
 *
 * @return:	HICON if the icon is loaded, NULL otherwise
 **/
HICON IcoLib_GetIcon(LPCSTR pszIcon)
{
	return (pszIcon) ? (HICON)CallService(MS_SKIN2_GETICON, NULL, (LPARAM) pszIcon) : NULL;
}

/**
 * Returns a icon, identified by a name
 *
 * @param	hIconItem	- this is the pointer to an IconItem structure in icolib.
 *
 * @return:	HICON if the icon is loaded, NULL otherwise
 **/
HICON IcoLib_GetIconByHandle(HANDLE hIconItem)
{
	return (HICON)CallService(MS_SKIN2_GETICONBYHANDLE, NULL, (LPARAM) hIconItem);
}

/**
 * Set the icon of each control in the list
 *
 * @param	hDlg		- handle to the dialog control, that owns the controls
 * @param	pCtrl		- list to all controls and its icon names
 * @param	numCtrls	- number of elements in the pCtrl list
 *
 * @return	nothing
 **/
VOID IcoLib_SetCtrlIcons(HWND hDlg, const ICONCTRL* pCtrl, BYTE numCtrls)
{
	HICON	hIcon;
	BYTE	i;
	HWND	hCtrl;

	for (i = 0; i < numCtrls; i++) 
	{
		hIcon = IcoLib_GetIcon(pCtrl[i].pszIcon);
		if (pCtrl[i].idCtrl)	
		{
			hCtrl = GetDlgItem(hDlg, pCtrl[i].idCtrl);
			switch (pCtrl[i].Message) 
			{
				case STM_SETICON:
				case STM_SETIMAGE:
						{
						ShowWindow(hCtrl, hIcon ? SW_SHOW : SW_HIDE);
					}
				case BM_SETIMAGE:
					{
						SendMessage(hCtrl, pCtrl[i].Message, IMAGE_ICON, (LPARAM) hIcon);
					}
			}
		}
		else
		{
			SendMessage(hDlg, pCtrl[i].Message, ICON_BIG, (LPARAM) hIcon);
		}
	}
}

/**
 * This function manually registers a single icon from the default icon library.
 *
 * @param		szIconID		- This is the uniquely identifying string for an icon. 
 *								  This string is the setting name in the database and should 
 *								  only use ASCII characters.
 * @param		szDescription	- This is the description displayed in the options dialog.
 * @param		szSection		- This is the subsection, where the icon is organized in the options dialog.
 * @param		szDefaultFile	- This is the validated path to the default icon file.
 * @param		idIcon			- This is the ResourceID of the icon in the default file.
 * @param		Size			- This is the desired size of the icon to load.
 *								  0:	default size for small icons (16x16)
 *								  1:	default size for normal icons (32x32)
 * @param		hDefIcon		- This is the default icon to use if the default icon
 *								  file does not exist and no custom icon is set up in the config.
 *
 * @return	This function returns the HANDLE of the icon item.
 **/
static HANDLE IcoLib_RegisterIconHandleEx(LPSTR szIconID, LPSTR szDescription, LPSTR szSection, LPTSTR szDefaultFile, INT idIcon, INT Size, HICON hDefIcon)
{
	HANDLE hIconHandle = NULL;

	if (szIconID && szDescription && szSection)
	{
		SKINICONDESC sid;

		ZeroMemory(&sid, sizeof(sid));
		sid.cbSize = sizeof(sid);
		sid.flags = SIDF_ALL_TCHAR;
		sid.pszName = szIconID;
		sid.ptszDescription = mir_a2t(szDescription);
		sid.ptszSection = mir_a2t(szSection);

		if (sid.ptszDescription && sid.ptszSection)
		{
			switch (Size)
			{
				// small icons (16x16)
				case 0:
					{
						sid.cx = GetSystemMetrics(SM_CXSMICON);
						sid.cy = GetSystemMetrics(SM_CYSMICON);
						break;
					}
				// normal icons (32x32)
				case 1:
					{
						sid.cx = GetSystemMetrics(SM_CXICON);
						sid.cy = GetSystemMetrics(SM_CYICON);
						break;
					}
				// custom icon size
				default:
					{
						sid.cx = sid.cy = Size;
						break;
					}
			}

			sid.ptszDefaultFile = szDefaultFile;
			if (sid.ptszDefaultFile && sid.ptszDefaultFile[0])
			{
				sid.iDefaultIndex = ICONINDEX(idIcon);
			}
			else
			{
				sid.hDefaultIcon = hDefIcon;
				sid.iDefaultIndex = -1;
			}
			hIconHandle = (HANDLE) CallService(MS_SKIN2_ADDICON, 0, (LPARAM)&sid);
		}
		MIR_FREE(sid.ptszDescription);
		MIR_FREE(sid.ptszSection);
	}
	return hIconHandle;
}

/**
 * This function manually registers a single icon from the default icon library.
 *
 * @param		szIconID		- This is the uniquely identifying string for an icon. 
 *								  This string is the setting name in the database and should 
 *								  only use ASCII characters.
 * @param		szDescription	- This is the description displayed in the options dialog.
 * @param		szSection		- This is the subsection, where the icon is organized in the options dialog.
 * @param		idIcon			- This is the ResourceID of the icon in the default file
 * @param		Size			- This is the desired size of the icon to load.
 *								  0:	default size for small icons (16x16)
 *								  1:	default size for normal icons (32x32)
 *
 * @return	This function returns the HANDLE of the icon item.
 **/
HANDLE IcoLib_RegisterIconHandle(LPSTR szIconID, LPSTR szDescription, LPSTR szSection, INT idIcon, INT Size)
{
	return IcoLib_RegisterIconHandleEx(szIconID, szDescription, szSection, IcoLib_GetDefaultIconFileName(), idIcon, Size, ghDefIcon);
}

/**
 * This function manually registers a single icon from the default icon library.
 *
 * @param		szIconID		- This is the uniquely identifying string for an icon. 
 *								  This string is the setting name in the database and should 
 *								  only use ASCII characters.
 * @param		szDescription	- This is the description displayed in the options dialog.
 * @param		szSection		- This is the subsection, where the icon is organized in the options dialog.
 * @param		idIcon			- This is the ResourceID of the icon in the default file
 * @param		Size			- This is the desired size of the icon to load.
 *								  0:	default size for small icons (16x16)
 *								  1:	default size for normal icons (32x32)
 *
 * @return	 This function returns the HICON of the icon itself.
 **/
HICON IcoLib_RegisterIcon(LPSTR szIconID, LPSTR szDescription, LPSTR szSection, INT idIcon, INT Size)
{
	return IcoLib_GetIconByHandle(IcoLib_RegisterIconHandle(szIconID, szDescription, szSection, idIcon, Size));
}

/**
 * Add default icons to the skin library or load customized icons
 *
 * @param		none
 *
 * @return		nothing
 **/
VOID IcoLib_LoadModule()
{
	LPTSTR	szDefaultFile;
	INT_PTR i;

	// search for default icon file
	szDefaultFile = IcoLib_GetDefaultIconFileName();
	
	IcoLib_CheckIconPackVersion(szDefaultFile);

	// load default icon if required
	ghDefIcon = (HICON)LoadImage(ghInst, MAKEINTRESOURCE(IDI_DEFAULT), IMAGE_ICON, 
							 GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 0);

	for (i = 0; i < SIZEOF(icoDesc); i++) 
	{	
		IcoLib_RegisterIconHandleEx(
			icoDesc[i].pszName, icoDesc[i].pszDesc, icoDesc[i].pszSection, 
			szDefaultFile, icoDesc[i].idResource, icoDesc[i].size, ghDefIcon);
	}
}

