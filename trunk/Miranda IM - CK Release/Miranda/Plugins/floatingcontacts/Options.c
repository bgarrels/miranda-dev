
#include "stdhdr.h"

#include "resource.h"
#include "fltcont.h"

/////////////////////////////////////////////////////////////////////////////
//

#define SAMEASF_FACE   1
#define SAMEASF_SIZE   2
#define SAMEASF_STYLE  4
#define SAMEASF_COLOUR 8

typedef struct _SFontSettings
{
	BYTE sameAsFlags,sameAs;
	COLORREF colour;
	char size;
	BYTE style;
	BYTE charset;
	char szFace[LF_FACESIZE];
} SFontSettings;

static SFontSettings s_rgFontSettings[FLT_FONTIDS];
static SFontSettings s_rgFontSettingsMiranda[FLT_FONTIDS];
			

#define FLT_SAME_AS_NONE	((BYTE)0xFF)
#define FLT_SAME_AS_MIRANDA	((BYTE)0xFE)

static char* s_rgszFontSizes[]={"8","10","14","16","18","20","24","28"};

static const char* s_rgszFontIdDescr[FLT_FONTIDS] =
{
	"Standard contacts",
	"Online contacts to whom you have a different visibility",
	"Offline contacts",
	"Offline contacts to whom you have a different visibility",
	"Contacts which are 'not on list'",
};

static WORD s_rgwFontSameAsDefault[FLT_FONTIDS] =
{
	MAKEWORD(FLT_SAME_AS_MIRANDA, 0x0F),
	MAKEWORD(FLT_SAME_AS_MIRANDA, 0x0F),
	MAKEWORD(FLT_SAME_AS_MIRANDA, 0x0F),
	MAKEWORD(FLT_SAME_AS_MIRANDA, 0x0F),
	MAKEWORD(FLT_SAME_AS_MIRANDA, 0x0F),
};

static int s_rgnMirandaFontId[FLT_FONTIDS] =
{
	FONTID_CONTACTS,
	FONTID_INVIS,
	FONTID_OFFLINE,
	FONTID_OFFINVIS,
	FONTID_NOTONLIST
};

#define M_REBUILDFONTGROUP		(WM_USER + 10)
#define M_REMAKESAMPLE			(WM_USER + 11)
#define M_RECALCONEFONT			(WM_USER + 12)
#define M_RECALCOTHERFONTS		(WM_USER + 13)
#define M_SAVEFONT				(WM_USER + 14)
#define M_REFRESHSAMEASBOXES	(WM_USER + 15)
#define M_FILLSCRIPTCOMBO		(WM_USER + 16)
#define M_LOADFONT				(WM_USER + 17)
#define M_GUESSSAMEASBOXES		(WM_USER + 18)
#define M_SETSAMEASBOXES		(WM_USER + 19)

#define M_REFRESHBKGBOXES		(WM_USER + 20)
#define M_REFRESHBORDERPICKERS	(WM_USER + 21)


/////////////////////////////////////////////////////////////////////////////
//

static BOOL APIENTRY OptGeneralWndProc( IN HWND hwndDlg, IN UINT uMsg, IN WPARAM wParam, IN LPARAM lParam );
static BOOL APIENTRY OptBackgroundWndProc( IN HWND hwndDlg, IN UINT uMsg, IN WPARAM wParam, IN LPARAM lParam );


static HWND s_hwndTabGeneral;
static HWND s_hwndTabBackground;

/////////////////////////////////////////////////////////////////////////////
//

int OnOptionsInitialize( IN WPARAM wParam, IN LPARAM lParam )
{
	OPTIONSDIALOGPAGE odp = { 0 };

	odp.cbSize		= sizeof(odp);
	odp.position	= 100000000;
	odp.hInstance	= hInst;
	odp.pszGroup	= LPGEN( "Plugins" );
	odp.pszTitle	= LPGEN("Floating Contacts");
	odp.groupPosition	= 910000001;

	odp.pszTemplate = MAKEINTRESOURCEA( IDD_OPT_FLTCONT );
	odp.pszTab		= LPGEN( "General" );
	odp.pfnDlgProc	= OptGeneralWndProc;
	odp.flags		= ODPF_BOLDGROUPS;
	CallService( MS_OPT_ADDPAGE, wParam, (LPARAM) & odp );

	odp.pszTemplate = MAKEINTRESOURCEA( IDD_OPT_THUMB );
	odp.pszTab		= LPGEN( "Background" );
	odp.pfnDlgProc	= OptBackgroundWndProc;
	odp.nIDBottomSimpleControl = IDC_GROUP_OPACITY;
	CallService( MS_OPT_ADDPAGE, wParam, (LPARAM) & odp );

	return 0;
}

static int CALLBACK EnumFontsProc(	IN ENUMLOGFONTEX* lpelfe, 
									IN NEWTEXTMETRICEX* lpntme, 
									IN int fontType, 
									IN LPARAM lParam )
{
	lpntme;
	fontType;

	if (IsWindow((HWND)lParam))
	{
		if (CB_ERR == SendMessage((HWND)lParam, CB_FINDSTRINGEXACT, (WPARAM)-1, (LPARAM)lpelfe->elfLogFont.lfFaceName))
			SendMessage((HWND)lParam, CB_ADDSTRING, 0, (LPARAM)lpelfe->elfLogFont.lfFaceName);
		return TRUE;
	}
	return FALSE;
}

void FillFontListThread( IN HWND hwndDlg )
{
	LOGFONT lf = {0};
	HDC hdc = GetDC(hwndDlg);

	lf.lfCharSet		= DEFAULT_CHARSET;
	lf.lfFaceName[0]	= 0;
	lf.lfPitchAndFamily	= 0;
	EnumFontFamiliesEx(hdc, &lf, (FONTENUMPROC)EnumFontsProc, (LPARAM)GetDlgItem(hwndDlg,IDC_TYPEFACE), 0);
	ReleaseDC(hwndDlg, hdc);
	return;
}



static int CALLBACK EnumFontScriptsProc (	IN ENUMLOGFONTEX* lpelfe, 
											IN NEWTEXTMETRICEX* lpntme, 
											IN int fontType, 
											IN LPARAM lParam )
{
	if (CB_ERR == SendMessage((HWND)lParam, CB_FINDSTRINGEXACT, (WPARAM)-1, (LPARAM)lpelfe->elfScript))
	{
		int i = SendMessage((HWND)lParam, CB_ADDSTRING, 0, (LPARAM)lpelfe->elfScript);
		SendMessage((HWND)lParam, CB_SETITEMDATA, i, lpelfe->elfLogFont.lfCharSet);
	}
	return TRUE;
}



static void GetDefaultFontSetting ( IN BOOL bFltContacts, 
									IN int nFontId, 
									IN LOGFONT* lf, 
									IN COLORREF* colour	)
{
	SystemParametersInfo(SPI_GETICONTITLELOGFONT, sizeof(LOGFONT), lf, FALSE);
	*colour = GetSysColor(COLOR_WINDOWTEXT);
	if (bFltContacts)
	{
		switch (nFontId)
		{
		case FLT_FONTID_OFFINVIS:
		case FLT_FONTID_INVIS:
			lf->lfItalic = !lf->lfItalic;
			break;

		case FLT_FONTID_NOTONLIST:
			*colour = GetSysColor(COLOR_3DSHADOW);
			break;
		}
	}
	else
	{
		switch (s_rgnMirandaFontId[nFontId])
		{
		case FONTID_OFFINVIS:
		case FONTID_INVIS:
			lf->lfItalic = !lf->lfItalic;
			break;

		case FONTID_NOTONLIST:
			*colour = GetSysColor(COLOR_3DSHADOW);
			break;
		}
	}
}



void GetFontSetting(	IN BOOL bFltContacts, 
						IN int nFontId, 
						IN LOGFONT* lf, 
						IN COLORREF* colour	)
{
	DBVARIANT dbv;
	char idstr[10];
	BYTE style;
	const char* pModule = (bFltContacts ? szModuleDB : "CLC");

	GetDefaultFontSetting(bFltContacts, nFontId, lf, colour);
	if (!bFltContacts)
		nFontId = s_rgnMirandaFontId[nFontId];
	wsprintf(idstr, "Font%dName", nFontId);
	if (!DBGetContactSetting(NULL, pModule, idstr, &dbv))
	{
		lstrcpy(lf->lfFaceName, dbv.pszVal);
		DBFreeVariant(&dbv);
	}

	wsprintf(idstr, "Font%dCol", nFontId);
	*colour = DBGetContactSettingDword(NULL, pModule, idstr, *colour);

	wsprintf(idstr, "Font%dSize", nFontId);
	lf->lfHeight = (char)DBGetContactSettingByte(NULL, pModule, idstr, lf->lfHeight);

	wsprintf(idstr, "Font%dSty", nFontId);
	style = (BYTE)DBGetContactSettingByte(NULL, pModule, idstr
							, (lf->lfWeight == FW_NORMAL ? 0 : DBFONTF_BOLD)
								| (lf->lfItalic ? DBFONTF_ITALIC : 0)
								| (lf->lfUnderline ? DBFONTF_UNDERLINE : 0)
							);
	lf->lfWidth = lf->lfEscapement = lf->lfOrientation = 0;
	lf->lfWeight = ((style & DBFONTF_BOLD) ? FW_BOLD : FW_NORMAL);
	lf->lfItalic = (0 != (style & DBFONTF_ITALIC));
	lf->lfUnderline = (0 != (style & DBFONTF_UNDERLINE));
	lf->lfStrikeOut = 0;

	wsprintf(idstr, "Font%dSet", nFontId);
	lf->lfCharSet = (BYTE)DBGetContactSettingByte(NULL, pModule, idstr, lf->lfCharSet);
	lf->lfOutPrecision = OUT_DEFAULT_PRECIS;
	lf->lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf->lfQuality = DEFAULT_QUALITY;
	lf->lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;

	if (bFltContacts)
	{
		WORD wSameAs;
		BYTE bySameAs;
		BYTE bySameAsFlags;

		wsprintf(idstr, "Font%dAs", nFontId);
		wSameAs = (WORD)DBGetContactSettingWord(NULL, szModuleDB, idstr, s_rgwFontSameAsDefault[nFontId]);
		bySameAs=LOBYTE(wSameAs);
		bySameAsFlags=HIBYTE(wSameAs);

		if (FLT_SAME_AS_MIRANDA == bySameAs)
		{
			LOGFONT lfMiranda;
			COLORREF colourMiranda;

			GetFontSetting(FALSE, nFontId, &lfMiranda, &colourMiranda);
			if (bySameAsFlags & SAMEASF_FACE)
			{
				lstrcpy(lf->lfFaceName, lfMiranda.lfFaceName);
				lf->lfCharSet = lfMiranda.lfCharSet;
			}
			if (bySameAsFlags & SAMEASF_SIZE)
				lf->lfHeight = lfMiranda.lfHeight;
			if (bySameAsFlags & SAMEASF_STYLE)
			{
				lf->lfWeight = lfMiranda.lfWeight;
				lf->lfItalic = lfMiranda.lfItalic;
				lf->lfUnderline = lfMiranda.lfUnderline;
			}
			if (bySameAsFlags & SAMEASF_COLOUR)
				*colour = colourMiranda;
		}
	}
}



static BOOL APIENTRY OptGeneralWndProc(	IN HWND hwndDlg, 
											IN UINT uMsg, 
											IN WPARAM wParam, 
											IN LPARAM lParam )
{
	static HFONT hFontSample;

	switch ( uMsg )
	{
		case WM_INITDIALOG:
		{
			BYTE btOpacity;
			char szPercent[20];

			TranslateDialogDefault(hwndDlg);

			// Properties
			CheckDlgButton(hwndDlg, IDC_CHK_HIDE_OFFLINE, (bHideOffline ? BST_CHECKED : BST_UNCHECKED));
			CheckDlgButton(hwndDlg, IDC_CHK_HIDE_ALL, (bHideAll ? BST_CHECKED : BST_UNCHECKED));
			CheckDlgButton(hwndDlg, IDC_CHK_HIDE_WHEN_FULSCREEN, (bHideWhenFullscreen ? BST_CHECKED : BST_UNCHECKED));
			CheckDlgButton(hwndDlg, IDC_CHK_STICK, (bMoveTogether ? BST_CHECKED : BST_UNCHECKED));
			CheckDlgButton(hwndDlg, IDC_CHK_WIDTH, (bFixedWidth ? BST_CHECKED : BST_UNCHECKED));
			
			EnableWindow(GetDlgItem(hwndDlg, IDC_LBL_WIDTH), bFixedWidth);
			EnableWindow(GetDlgItem(hwndDlg, IDC_TXT_WIDTH), bFixedWidth);
			EnableWindow(GetDlgItem(hwndDlg, IDC_WIDTHSPIN), bFixedWidth);

			SendDlgItemMessage(hwndDlg, IDC_WIDTHSPIN, UDM_SETRANGE, 0, MAKELONG(255,0));
			SendDlgItemMessage(hwndDlg, IDC_WIDTHSPIN, UDM_SETPOS, 0, nThumbWidth);

			// Windows 2K/XP
			btOpacity = (BYTE)DBGetContactSettingByte(NULL, szModuleDB, "Opacity", 100);
			SendDlgItemMessage(hwndDlg, IDC_SLIDER_OPACITY, TBM_SETRANGE, TRUE, MAKELONG(0, 100));
			SendDlgItemMessage(hwndDlg, IDC_SLIDER_OPACITY, TBM_SETPOS, TRUE, btOpacity);
				
			wsprintf(szPercent, "%d%%", btOpacity);
			SetDlgItemText(hwndDlg, IDC_OPACITY, szPercent);

			EnableWindow(GetDlgItem(hwndDlg, IDC_SLIDER_OPACITY), (NULL != pSetLayeredWindowAttributes));
			EnableWindow(GetDlgItem(hwndDlg, IDC_OPACITY), (NULL != pSetLayeredWindowAttributes));

			// Fonts
			hFontSample = NULL;
			SetDlgItemText(hwndDlg, IDC_SAMPLE, Translate("Sample"));
			FillFontListThread(hwndDlg);
			{
				int i;
				int itemId;
				LOGFONT lf;
				COLORREF colour;
				WORD sameAs;
				char str[32];

				for (i = 0; i < FLT_FONTIDS; i++)
				{
					// Floating contacts fonts
					GetFontSetting(TRUE, i, &lf, &colour);
					wsprintf(str, "Font%dAs", i);
					sameAs = (WORD)DBGetContactSettingWord(NULL, szModuleDB, str, s_rgwFontSameAsDefault[i]);
					s_rgFontSettings[i].sameAs = LOBYTE(sameAs);
					s_rgFontSettings[i].sameAsFlags = HIBYTE(sameAs);
					s_rgFontSettings[i].style = (FW_NORMAL == lf.lfWeight? 0 : DBFONTF_BOLD)
													| (lf.lfItalic ? DBFONTF_ITALIC : 0)
													| (lf.lfUnderline ? DBFONTF_UNDERLINE : 0);
					if (lf.lfHeight < 0)
					{
						HDC hdc;
						SIZE size;
						HFONT hFont = CreateFontIndirect(&lf);

						hdc=GetDC(hwndDlg);
						SelectObject(hdc, hFont);
						GetTextExtentPoint32(hdc, "x", 1, &size);
						ReleaseDC(hwndDlg, hdc);
						DeleteObject(hFont);
						s_rgFontSettings[i].size = (char)size.cy;
					}
					else
						s_rgFontSettings[i].size = (char)lf.lfHeight;
					s_rgFontSettings[i].charset = lf.lfCharSet;
					s_rgFontSettings[i].colour = colour;
					lstrcpy(s_rgFontSettings[i].szFace, lf.lfFaceName);
					itemId = SendDlgItemMessage(hwndDlg, IDC_FONTID, CB_ADDSTRING, 0, (LPARAM)Translate(s_rgszFontIdDescr[i]));
					SendDlgItemMessage(hwndDlg, IDC_FONTID, CB_SETITEMDATA, itemId, i);

					// Miranda contact list fonts
					GetFontSetting(FALSE, i, &lf, &colour);
					s_rgFontSettingsMiranda[i].sameAs = 0;
					s_rgFontSettingsMiranda[i].sameAsFlags = 0;
					lstrcpy(s_rgFontSettingsMiranda[i].szFace, lf.lfFaceName);
					s_rgFontSettingsMiranda[i].charset = lf.lfCharSet;
					s_rgFontSettingsMiranda[i].style = (FW_NORMAL == lf.lfWeight? 0 : DBFONTF_BOLD)
															| (lf.lfItalic ? DBFONTF_ITALIC : 0)
															| (lf.lfUnderline ? DBFONTF_UNDERLINE : 0);
					if (lf.lfHeight < 0)
					{
						HDC hdc;
						SIZE size;
						HFONT hFont = CreateFontIndirect(&lf);

						hdc = GetDC(hwndDlg);
						SelectObject(hdc, hFont);
						GetTextExtentPoint32(hdc, "x", 1, &size);
						ReleaseDC(hwndDlg, hdc);
						DeleteObject(hFont);
						s_rgFontSettingsMiranda[i].size = (char)size.cy;
					}
					else
						s_rgFontSettingsMiranda[i].size = (char)lf.lfHeight;
					s_rgFontSettingsMiranda[i].colour = colour;
				}
				SendDlgItemMessage(hwndDlg, IDC_FONTID, CB_SETCURSEL, 0, 0);
				for (i = 0; i < sizeof(s_rgszFontSizes)/sizeof(s_rgszFontSizes[0]); i++)
					SendDlgItemMessage(hwndDlg, IDC_FONTSIZE, CB_ADDSTRING, 0, (LPARAM)s_rgszFontSizes[i]);
			}
			SendMessage(hwndDlg, M_REBUILDFONTGROUP, 0, 0);
			SendMessage(hwndDlg, M_SAVEFONT, 0, 0);
			return TRUE;
		}
		// remake all the needed controls when the user changes the font selector at the top
		case M_REBUILDFONTGROUP:
		{
			int i = SendDlgItemMessage(hwndDlg, IDC_FONTID, CB_GETCURSEL, 0, 0);
			int j;
			int itemId;
			int nSameAs = FLT_SAME_AS_NONE;
			char szText[256];

			SendDlgItemMessage(hwndDlg, IDC_SAMEAS, CB_RESETCONTENT, 0, 0);
			itemId = SendDlgItemMessage(hwndDlg, IDC_SAMEAS, CB_ADDSTRING, 0, (LPARAM)Translate("<none>"));
			SendDlgItemMessage(hwndDlg, IDC_SAMEAS, CB_SETITEMDATA, itemId, FLT_SAME_AS_NONE);
			SendDlgItemMessage(hwndDlg, IDC_SAMEAS, CB_SETCURSEL, itemId, 0);
			itemId = SendDlgItemMessage(hwndDlg, IDC_SAMEAS, CB_ADDSTRING, 0, (LPARAM)Translate("<Contact List Text>"));
			SendDlgItemMessage(hwndDlg, IDC_SAMEAS, CB_SETITEMDATA, itemId, FLT_SAME_AS_MIRANDA);
			if (FLT_SAME_AS_MIRANDA == s_rgFontSettings[i].sameAs)
			{
				SendDlgItemMessage(hwndDlg, IDC_SAMEAS, CB_SETCURSEL, itemId, 0);
				nSameAs = FLT_SAME_AS_MIRANDA;
			}
			for (j = 0; j < FLT_FONTIDS; j++)
			{
				int nDependsOn = j;

				while (nDependsOn != i)
				{
					if (FLT_SAME_AS_NONE == nDependsOn || FLT_SAME_AS_MIRANDA == nDependsOn)
					{
						SendDlgItemMessage(hwndDlg, IDC_FONTID, CB_GETLBTEXT, j, (LPARAM)szText);
						itemId = SendDlgItemMessage(hwndDlg, IDC_SAMEAS, CB_ADDSTRING, 0, (LPARAM)szText);
						SendDlgItemMessage(hwndDlg, IDC_SAMEAS, CB_SETITEMDATA, itemId, j);
						if (j == s_rgFontSettings[i].sameAs)
						{
							SendDlgItemMessage(hwndDlg, IDC_SAMEAS, CB_SETCURSEL, itemId, 0);
							nSameAs = j;
						}
						break;
					}
					else
						nDependsOn = s_rgFontSettings[nDependsOn].sameAs;
				}
			}
			if (FLT_SAME_AS_NONE == nSameAs)
				s_rgFontSettings[i].sameAsFlags = 0;

			SendMessage(hwndDlg, M_LOADFONT, i, 0);
			SendMessage(hwndDlg, M_SETSAMEASBOXES, i, 0);
			SendMessage(hwndDlg, M_REFRESHSAMEASBOXES, i, 0);
			SendMessage(hwndDlg, M_REMAKESAMPLE, 0, 0);
			break;
		}
		//fill the script combo box and set the selection to the value for fontid wParam
		case M_FILLSCRIPTCOMBO:
		{	
			int i;
			HDC hdc = GetDC(hwndDlg);
			LOGFONT lf = {0};

			lf.lfCharSet = DEFAULT_CHARSET;
			lf.lfPitchAndFamily = 0;
			GetDlgItemText(hwndDlg, IDC_TYPEFACE, lf.lfFaceName, sizeof(lf.lfFaceName));
			SendDlgItemMessage(hwndDlg, IDC_SCRIPT, CB_RESETCONTENT, 0, 0);
			EnumFontFamiliesEx(hdc, &lf, (FONTENUMPROC)EnumFontScriptsProc, (LPARAM)GetDlgItem(hwndDlg, IDC_SCRIPT), 0);
			ReleaseDC(hwndDlg, hdc);
			for (i = SendDlgItemMessage(hwndDlg, IDC_SCRIPT, CB_GETCOUNT, 0, 0) - 1; i >= 0; i--)
			{
				if (SendDlgItemMessage(hwndDlg, IDC_SCRIPT, CB_GETITEMDATA, i, 0) == s_rgFontSettings[wParam].charset)
				{
					SendDlgItemMessage(hwndDlg, IDC_SCRIPT, CB_SETCURSEL, i, 0);
					break;
				}
			}
			if (i < 0)
				SendDlgItemMessage(hwndDlg, IDC_SCRIPT, CB_SETCURSEL, 0, 0);
			break;
		}
		// set the check mark in the 'same as' boxes to the right value for fontid wParam
		case M_SETSAMEASBOXES:
		{
			CheckDlgButton(hwndDlg, IDC_SAMETYPE, (s_rgFontSettings[wParam].sameAsFlags & SAMEASF_FACE ? BST_CHECKED : BST_UNCHECKED));
			CheckDlgButton(hwndDlg, IDC_SAMESIZE, (s_rgFontSettings[wParam].sameAsFlags & SAMEASF_SIZE ? BST_CHECKED : BST_UNCHECKED));
			CheckDlgButton(hwndDlg, IDC_SAMESTYLE, (s_rgFontSettings[wParam].sameAsFlags & SAMEASF_STYLE ? BST_CHECKED : BST_UNCHECKED));
			CheckDlgButton(hwndDlg, IDC_SAMECOLOUR, (s_rgFontSettings[wParam].sameAsFlags & SAMEASF_COLOUR ? BST_CHECKED : BST_UNCHECKED));
			break;
		}
		// set the disabled flag on the 'same as' checkboxes to the values for fontid wParam
		case M_REFRESHSAMEASBOXES:
		{
			BOOL bSameAsNone = (FLT_SAME_AS_NONE == s_rgFontSettings[wParam].sameAs);

			EnableWindow(GetDlgItem(hwndDlg, IDC_SAMETYPE), !bSameAsNone);
			EnableWindow(GetDlgItem(hwndDlg, IDC_SAMESIZE), !bSameAsNone);
			EnableWindow(GetDlgItem(hwndDlg, IDC_SAMESTYLE), !bSameAsNone);
			EnableWindow(GetDlgItem(hwndDlg, IDC_SAMECOLOUR), !bSameAsNone);
			EnableWindow(GetDlgItem(hwndDlg, IDC_TYPEFACE), bSameAsNone || !(s_rgFontSettings[wParam].sameAsFlags & SAMEASF_FACE));
			EnableWindow(GetDlgItem(hwndDlg, IDC_SCRIPT), bSameAsNone || !(s_rgFontSettings[wParam].sameAsFlags & SAMEASF_FACE));
			EnableWindow(GetDlgItem(hwndDlg, IDC_FONTSIZE), bSameAsNone || !(s_rgFontSettings[wParam].sameAsFlags & SAMEASF_SIZE));
			EnableWindow(GetDlgItem(hwndDlg, IDC_BOLD), bSameAsNone || !(s_rgFontSettings[wParam].sameAsFlags & SAMEASF_STYLE));
			EnableWindow(GetDlgItem(hwndDlg, IDC_ITALIC), bSameAsNone || !(s_rgFontSettings[wParam].sameAsFlags & SAMEASF_STYLE));
			EnableWindow(GetDlgItem(hwndDlg, IDC_UNDERLINE), bSameAsNone || !(s_rgFontSettings[wParam].sameAsFlags & SAMEASF_STYLE));
			EnableWindow(GetDlgItem(hwndDlg, IDC_COLOUR), bSameAsNone || !(s_rgFontSettings[wParam].sameAsFlags & SAMEASF_COLOUR));
			break;
		}
		// remake the sample edit box font based on the settings in the controls
		case M_REMAKESAMPLE:
		{
			LOGFONT lf;

			if (hFontSample)
			{
				SendDlgItemMessage(hwndDlg, IDC_SAMPLE, WM_SETFONT, SendDlgItemMessage(hwndDlg, IDC_FONTID, WM_GETFONT, 0, 0), 0);
				DeleteObject(hFontSample);
			}
			lf.lfHeight = GetDlgItemInt(hwndDlg, IDC_FONTSIZE, NULL, FALSE);
			lf.lfWidth = 0;
			lf.lfEscapement = 0;
			lf.lfOrientation = 0;
			lf.lfWeight = (IsDlgButtonChecked(hwndDlg, IDC_BOLD) ? FW_BOLD : FW_NORMAL);
			lf.lfItalic = (BYTE)IsDlgButtonChecked(hwndDlg, IDC_ITALIC);
			lf.lfUnderline = (BYTE)IsDlgButtonChecked(hwndDlg, IDC_UNDERLINE);
			lf.lfStrikeOut = 0;
			lf.lfCharSet = (BYTE)SendDlgItemMessage(hwndDlg, IDC_SCRIPT, CB_GETITEMDATA, SendDlgItemMessage(hwndDlg, IDC_SCRIPT, CB_GETCURSEL, 0, 0), 0);
			lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
			lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
			lf.lfQuality = DEFAULT_QUALITY;
			lf.lfPitchAndFamily = (DEFAULT_PITCH | FF_DONTCARE);
			GetDlgItemText(hwndDlg, IDC_TYPEFACE, lf.lfFaceName, sizeof(lf.lfFaceName));
			if (NULL != (hFontSample = CreateFontIndirect(&lf)))
				SendDlgItemMessage(hwndDlg, IDC_SAMPLE, WM_SETFONT, (WPARAM)hFontSample, TRUE);
			break;
		}
		// copy the 'same as' settings for fontid wParam from their sources
		case M_RECALCONEFONT:
		{
			if (FLT_SAME_AS_NONE != s_rgFontSettings[wParam].sameAs)
			{
				SFontSettings* pSameAs = ((FLT_SAME_AS_MIRANDA == s_rgFontSettings[wParam].sameAs)
											? &s_rgFontSettingsMiranda[wParam]
											: &s_rgFontSettings[s_rgFontSettings[wParam].sameAs]
											);

				if (s_rgFontSettings[wParam].sameAsFlags & SAMEASF_FACE)
				{
					lstrcpy(s_rgFontSettings[wParam].szFace, pSameAs->szFace);
					s_rgFontSettings[wParam].charset = pSameAs->charset;
				}
				if (s_rgFontSettings[wParam].sameAsFlags & SAMEASF_SIZE)
					s_rgFontSettings[wParam].size = pSameAs->size;
				if (s_rgFontSettings[wParam].sameAsFlags & SAMEASF_STYLE)
					s_rgFontSettings[wParam].style = pSameAs->style;
				if (s_rgFontSettings[wParam].sameAsFlags & SAMEASF_COLOUR)
					s_rgFontSettings[wParam].colour = pSameAs->colour;
			}
			break;
		}
		// recalculate the 'same as' settings for all fonts but wParam
		case M_RECALCOTHERFONTS:
		{	
			int nFont;
			int nDepth;
			int nRecalcedFonts = 1;
			int nRecalcDepth;
			int nFontId = (int)wParam;
			int nSameAs;

			for (nRecalcDepth = 0; nRecalcedFonts < FLT_FONTIDS && nRecalcDepth < FLT_FONTIDS; nRecalcDepth++)
			{
				for (nFont = 0; nFont < FLT_FONTIDS; nFont++)
				{
					if (nFontId == nFont)
						continue;

					nSameAs = s_rgFontSettings[nFont].sameAs;
					for (nDepth = 0; nDepth < nRecalcDepth; nDepth++)
					{
						if (FLT_SAME_AS_NONE == nSameAs || FLT_SAME_AS_MIRANDA == nSameAs || nFontId == nSameAs)
							break;

						nSameAs = s_rgFontSettings[nSameAs].sameAs;
					}
					if (nDepth == nRecalcDepth)
					{
						if (nFontId == nSameAs)
						{
							SendMessage(hwndDlg, M_RECALCONEFONT, nFont, 0);
							nRecalcedFonts++;
						}
						else if (FLT_SAME_AS_NONE == nSameAs || FLT_SAME_AS_MIRANDA == nSameAs)
							nRecalcedFonts++;
					}
				}
			}
			break;
		}
		//save the font settings from the controls to font wParam
		case M_SAVEFONT:
		{
			s_rgFontSettings[wParam].sameAsFlags = (IsDlgButtonChecked(hwndDlg, IDC_SAMETYPE) ? SAMEASF_FACE : 0)
													| (IsDlgButtonChecked(hwndDlg, IDC_SAMESIZE) ? SAMEASF_SIZE : 0)
													| (IsDlgButtonChecked(hwndDlg, IDC_SAMESTYLE) ? SAMEASF_STYLE : 0)
													| (IsDlgButtonChecked(hwndDlg, IDC_SAMECOLOUR) ? SAMEASF_COLOUR : 0);
			s_rgFontSettings[wParam].sameAs = (BYTE)SendDlgItemMessage(hwndDlg
														, IDC_SAMEAS
														, CB_GETITEMDATA
														, SendDlgItemMessage(hwndDlg, IDC_SAMEAS, CB_GETCURSEL, 0, 0)
														, 0
														);
			GetDlgItemText(hwndDlg, IDC_TYPEFACE, s_rgFontSettings[wParam].szFace, sizeof(s_rgFontSettings[wParam].szFace));
			s_rgFontSettings[wParam].charset = (BYTE)SendDlgItemMessage(hwndDlg
														, IDC_SCRIPT
														, CB_GETITEMDATA
														, SendDlgItemMessage(hwndDlg, IDC_SCRIPT, CB_GETCURSEL, 0, 0)
														, 0
														);
			s_rgFontSettings[wParam].size = (char)GetDlgItemInt(hwndDlg, IDC_FONTSIZE, NULL, FALSE);
			s_rgFontSettings[wParam].style= (IsDlgButtonChecked(hwndDlg, IDC_BOLD) ? DBFONTF_BOLD : 0)
												| (IsDlgButtonChecked(hwndDlg, IDC_ITALIC) ? DBFONTF_ITALIC : 0)
												| (IsDlgButtonChecked(hwndDlg, IDC_UNDERLINE) ? DBFONTF_UNDERLINE : 0);
			s_rgFontSettings[wParam].colour = SendDlgItemMessage(hwndDlg, IDC_COLOUR, CPM_GETCOLOUR, 0, 0);
			break;
		}
		// load font wParam into the controls
		case M_LOADFONT:
		{
			LOGFONT lf;
			COLORREF colour;

			SetDlgItemText(hwndDlg, IDC_TYPEFACE, s_rgFontSettings[wParam].szFace);
			SendMessage(hwndDlg, M_FILLSCRIPTCOMBO, wParam, 0);
			SetDlgItemInt(hwndDlg, IDC_FONTSIZE, s_rgFontSettings[wParam].size, FALSE);
			CheckDlgButton(hwndDlg, IDC_BOLD, ((s_rgFontSettings[wParam].style & DBFONTF_BOLD) ? BST_CHECKED : BST_UNCHECKED));
			CheckDlgButton(hwndDlg, IDC_ITALIC, ((s_rgFontSettings[wParam].style & DBFONTF_ITALIC) ? BST_CHECKED : BST_UNCHECKED));
			CheckDlgButton(hwndDlg, IDC_UNDERLINE, ((s_rgFontSettings[wParam].style & DBFONTF_UNDERLINE) ? BST_CHECKED : BST_UNCHECKED));
			GetDefaultFontSetting(TRUE, wParam, &lf, &colour);
			SendDlgItemMessage(hwndDlg, IDC_COLOUR, CPM_SETDEFAULTCOLOUR, 0, colour);
			SendDlgItemMessage(hwndDlg, IDC_COLOUR, CPM_SETCOLOUR, 0, s_rgFontSettings[wParam].colour);
			break;
		}
		// guess suitable values for the 'same as' checkboxes for fontId wParam
		case M_GUESSSAMEASBOXES:
		{
			s_rgFontSettings[wParam].sameAsFlags = 0;
			if (FLT_SAME_AS_NONE != s_rgFontSettings[wParam].sameAs)
			{
				SFontSettings* pSameAs = ((FLT_SAME_AS_MIRANDA == s_rgFontSettings[wParam].sameAs)
											? &s_rgFontSettingsMiranda[wParam]
											: &s_rgFontSettings[s_rgFontSettings[wParam].sameAs]
											);

				if (!lstrcmp(s_rgFontSettings[wParam].szFace, pSameAs->szFace) && s_rgFontSettings[wParam].charset == pSameAs->charset)
    				s_rgFontSettings[wParam].sameAsFlags |= SAMEASF_FACE;
				if (s_rgFontSettings[wParam].size == pSameAs->size)
    				s_rgFontSettings[wParam].sameAsFlags |= SAMEASF_SIZE;
				if (s_rgFontSettings[wParam].style == pSameAs->style)
    				s_rgFontSettings[wParam].sameAsFlags |= SAMEASF_STYLE;
				if (s_rgFontSettings[wParam].colour == pSameAs->colour)
    				s_rgFontSettings[wParam].sameAsFlags |= SAMEASF_COLOUR;
				SendMessage(hwndDlg,M_SETSAMEASBOXES,wParam,0);
			}
			break;
		}
		case WM_CTLCOLORSTATIC:
		{
			if ((HWND)lParam == GetDlgItem(hwndDlg, IDC_SAMPLE))
			{
				SetTextColor((HDC)wParam, SendDlgItemMessage(hwndDlg, IDC_COLOUR, CPM_GETCOLOUR, 0, 0));
				SetBkColor((HDC)wParam, GetSysColor(COLOR_3DFACE));
				return (BOOL)GetSysColorBrush(COLOR_3DFACE);
			}
			break;
		}
		case WM_HSCROLL:
		{
			if (wParam != TB_ENDTRACK)
			{
				int nPos;
				char szPercent[20];

				nPos = (BYTE)SendDlgItemMessage(hwndDlg, IDC_SLIDER_OPACITY, TBM_GETPOS, 0, 0);
				thumbAlpha	= (BYTE)(( nPos * 255 ) / 100 );
				SetThumbsOpacity(thumbAlpha);
				
				wsprintf(szPercent, "%d%%", nPos);
				SetDlgItemText(hwndDlg, IDC_OPACITY, szPercent);
				SendMessage(GetParent(hwndDlg), PSM_CHANGED, 0, 0);
			}
			break;
		}
		case WM_COMMAND:
		{
			int nFontId = SendDlgItemMessage(hwndDlg, IDC_FONTID, CB_GETITEMDATA
								, SendDlgItemMessage(hwndDlg, IDC_FONTID, CB_GETCURSEL, 0, 0)
								, 0
								);

			switch (LOWORD(wParam))
			{
				case IDC_CHK_WIDTH:
				{
					if (BN_CLICKED == HIWORD(wParam))
					{
						BOOL bChecked = (BOOL)IsDlgButtonChecked(hwndDlg, IDC_CHK_WIDTH);
						
						EnableWindow(GetDlgItem(hwndDlg, IDC_LBL_WIDTH ), bChecked);
						EnableWindow(GetDlgItem(hwndDlg, IDC_TXT_WIDTH ), bChecked);
						EnableWindow(GetDlgItem(hwndDlg, IDC_WIDTHSPIN), bChecked);
					}
					break;
				}
				case IDC_TXT_WIDTH:
				{
					if (EN_CHANGE != HIWORD(wParam) || (HWND)lParam != GetFocus())
						return 1;
					break;
				}
				case IDC_FONTID:
				{
					if (CBN_SELCHANGE == HIWORD(wParam))
						SendMessage(hwndDlg, M_REBUILDFONTGROUP, 0, 0);
					return FALSE;
				}
				case IDC_SAMETYPE:
				case IDC_SAMESIZE:
				case IDC_SAMESTYLE:
				case IDC_SAMECOLOUR:
				{
					SendMessage(hwndDlg, M_SAVEFONT, nFontId, 0);
					SendMessage(hwndDlg, M_RECALCONEFONT, nFontId, 0);
					SendMessage(hwndDlg, M_RECALCOTHERFONTS, nFontId, 0);
					SendMessage(hwndDlg, M_LOADFONT, nFontId, 0);
					SendMessage(hwndDlg, M_REFRESHSAMEASBOXES, nFontId, 0);
					SendMessage(hwndDlg, M_REMAKESAMPLE, 0, 0);
					break;
				}
				case IDC_SAMEAS:
				{
					if (CBN_SELCHANGE != HIWORD(wParam))
						return FALSE;
					SendMessage(hwndDlg, M_SAVEFONT, nFontId, 0);
					SendMessage(hwndDlg, M_GUESSSAMEASBOXES, nFontId, 0);
					SendMessage(hwndDlg, M_REFRESHSAMEASBOXES, nFontId, 0);
					break;
				}
				case IDC_TYPEFACE:
				case IDC_SCRIPT:
				case IDC_FONTSIZE:
				{
					if (CBN_EDITCHANGE != HIWORD(wParam) && CBN_SELCHANGE != HIWORD(wParam))
						return FALSE;
					if (CBN_SELCHANGE == HIWORD(wParam))
						SendDlgItemMessage(hwndDlg, LOWORD(wParam), CB_SETCURSEL, SendDlgItemMessage(hwndDlg, LOWORD(wParam), CB_GETCURSEL, 0, 0), 0);
					if (IDC_TYPEFACE == LOWORD(wParam))
						SendMessage(hwndDlg, M_FILLSCRIPTCOMBO, nFontId, 0);
					// FALL THRU
				}
				case IDC_BOLD:
				case IDC_ITALIC:
				case IDC_UNDERLINE:
				case IDC_COLOUR:
				{
					SendMessage(hwndDlg, M_SAVEFONT, nFontId, 0);
					//SendMessage(hwndDlg, M_GUESSSAMEASBOXES, nFontId, 0);
					//SendMessage(hwndDlg, M_REFRESHSAMEASBOXES, nFontId, 0);
					SendMessage(hwndDlg, M_RECALCOTHERFONTS, nFontId, 0);
					SendMessage(hwndDlg, M_REMAKESAMPLE, 0, 0);
					break;
				}
				case IDC_SAMPLE:
					return 0;
			}
			SendMessage(GetParent(hwndDlg), PSM_CHANGED, 0, 0);
			break;
		}
		case WM_NOTIFY:
		{
			LPNMHDR phdr = (LPNMHDR)(lParam);
			
			if (0 == phdr->idFrom)
			{
				switch (phdr->code) 
				{
					case PSN_APPLY:
					{
						int i;
						char str[20];
						BOOL bSuccess = FALSE;

						bHideOffline = (BOOL)IsDlgButtonChecked(hwndDlg, IDC_CHK_HIDE_OFFLINE);
						DBWriteContactSettingByte(NULL, szModuleDB, "HideOffline", (BYTE)bHideOffline);
						bHideAll = (BOOL)IsDlgButtonChecked(hwndDlg, IDC_CHK_HIDE_ALL);
						DBWriteContactSettingByte(NULL, szModuleDB, "HideAll", (BYTE)bHideAll);
						bHideWhenFullscreen = (BOOL)IsDlgButtonChecked(hwndDlg, IDC_CHK_HIDE_WHEN_FULSCREEN);
						DBWriteContactSettingByte(NULL, szModuleDB, "HideWhenFullscreen", (BYTE)bHideWhenFullscreen);
						bMoveTogether = (BOOL)IsDlgButtonChecked(hwndDlg, IDC_CHK_STICK);
						DBWriteContactSettingByte(NULL, szModuleDB, "MoveTogether", (BYTE)bMoveTogether);
						bFixedWidth = (BOOL)IsDlgButtonChecked(hwndDlg, IDC_CHK_WIDTH);
						DBWriteContactSettingByte(NULL, szModuleDB, "FixedWidth", (BYTE)bFixedWidth);
						nThumbWidth	= GetDlgItemInt(hwndDlg, IDC_TXT_WIDTH, &bSuccess, FALSE);
						DBWriteContactSettingDword(NULL, szModuleDB, "Width", nThumbWidth );

						DBWriteContactSettingByte(NULL, szModuleDB, "Opacity"
							, (BYTE)SendDlgItemMessage(hwndDlg, IDC_SLIDER_OPACITY, TBM_GETPOS, 0, 0)
							);

						for (i = 0; i < FLT_FONTIDS; i++)
						{
							wsprintf(str, "Font%dName", i);
							DBWriteContactSettingString(NULL, szModuleDB, str, s_rgFontSettings[i].szFace);
							wsprintf(str, "Font%dSet", i);
							DBWriteContactSettingByte(NULL, szModuleDB, str, s_rgFontSettings[i].charset);
							wsprintf(str, "Font%dSize", i);
							DBWriteContactSettingByte(NULL, szModuleDB, str, s_rgFontSettings[i].size);
							wsprintf(str, "Font%dSty", i);
							DBWriteContactSettingByte(NULL, szModuleDB, str, s_rgFontSettings[i].style);
							wsprintf(str, "Font%dCol", i);
							DBWriteContactSettingDword(NULL, szModuleDB, str, s_rgFontSettings[i].colour);
							wsprintf(str, "Font%dAs", i);
							DBWriteContactSettingWord(NULL, szModuleDB, str, MAKEWORD(s_rgFontSettings[i].sameAs, s_rgFontSettings[i].sameAsFlags));
						}

						ApplyOptionsChanges();
						OnStatusChanged();
						return TRUE;
					}
					case PSN_RESET:
					//case PSN_KILLACTIVE:
					{
						thumbAlpha = (BYTE)((double)DBGetContactSettingByte(NULL, szModuleDB, "Opacity", 100) * 2.55);
						SetThumbsOpacity(thumbAlpha);
						break;
					}
				}
			}
			break;
		}
		case WM_DESTROY:
		{
			if (hFontSample)
			{
				SendDlgItemMessage(hwndDlg, IDC_SAMPLE, WM_SETFONT, SendDlgItemMessage(hwndDlg, IDC_FONTID, WM_GETFONT, 0, 0), 0);
				DeleteObject(hFontSample);
			}
			break;
		}

	}
	return FALSE;
}




static BOOL APIENTRY OptBackgroundWndProc(	IN HWND hwndDlg, 
											IN UINT uMsg, 
											IN WPARAM wParam, 
											IN LPARAM lParam )
{
	switch ( uMsg )
	{
		case WM_INITDIALOG:
		{
			// Border
			CheckDlgButton(hwndDlg, IDC_DRAWBORDER
				, DBGetContactSettingByte(NULL, szModuleDB, "DrawBorder", FLT_DEFAULT_DRAWBORDER)
					? BST_CHECKED : BST_UNCHECKED
				);
			SendMessage(hwndDlg, M_REFRESHBORDERPICKERS, 0, 0);
			SendDlgItemMessage(hwndDlg, IDC_LTEDGESCOLOR, CPM_SETDEFAULTCOLOUR, 0, FLT_DEFAULT_LTEDGESCOLOR);
			SendDlgItemMessage(hwndDlg, IDC_LTEDGESCOLOR, CPM_SETCOLOUR, 0
				, DBGetContactSettingDword(NULL, szModuleDB, "LTEdgesColor", FLT_DEFAULT_LTEDGESCOLOR)
				);
			SendDlgItemMessage(hwndDlg, IDC_RBEDGESCOLOR, CPM_SETDEFAULTCOLOUR, 0, FLT_DEFAULT_RBEDGESCOLOR);
			SendDlgItemMessage(hwndDlg, IDC_RBEDGESCOLOR, CPM_SETCOLOUR, 0
				, DBGetContactSettingDword(NULL, szModuleDB, "RBEdgesColor", FLT_DEFAULT_RBEDGESCOLOR)
				);

			// Background
			CheckDlgButton(hwndDlg, IDC_CHK_WIDTH, (bFixedWidth ? BST_CHECKED : BST_UNCHECKED));

			SendDlgItemMessage(hwndDlg, IDC_BKGCOLOUR, CPM_SETDEFAULTCOLOUR, 0, FLT_DEFAULT_BKGNDCOLOR);
			SendDlgItemMessage(hwndDlg, IDC_BKGCOLOUR, CPM_SETCOLOUR, 0
				, DBGetContactSettingDword(NULL, szModuleDB, "BkColor", FLT_DEFAULT_BKGNDCOLOR)
				);
			CheckDlgButton(hwndDlg, IDC_BITMAP
				, DBGetContactSettingByte(NULL, szModuleDB, "BkUseBitmap", FLT_DEFAULT_BKGNDUSEBITMAP)
					? BST_CHECKED : BST_UNCHECKED
				);
			SendMessage(hwndDlg, M_REFRESHBKGBOXES, 0, 0);
			{
				DBVARIANT dbv;

				if (!DBGetContactSetting(NULL, szModuleDB, "BkBitmap", &dbv))
				{
					SetDlgItemText(hwndDlg, IDC_FILENAME, dbv.pszVal);
					DBFreeVariant(&dbv);
				}
			}
			{
				WORD bmpUse = (WORD)DBGetContactSettingWord(NULL, szModuleDB, "BkBitmapOpt", FLT_DEFAULT_BKGNDBITMAPOPT);

				CheckDlgButton(hwndDlg, IDC_STRETCHH, ((bmpUse & CLB_STRETCHH) ? BST_CHECKED : BST_UNCHECKED));
				CheckDlgButton(hwndDlg, IDC_STRETCHV, ((bmpUse & CLB_STRETCHV) ? BST_CHECKED : BST_UNCHECKED));
				CheckDlgButton(hwndDlg, IDC_TILEH, ((bmpUse & CLBF_TILEH) ? BST_CHECKED : BST_UNCHECKED));
				CheckDlgButton(hwndDlg, IDC_TILEV, ((bmpUse & CLBF_TILEV) ? BST_CHECKED : BST_UNCHECKED));
				CheckDlgButton(hwndDlg, IDC_PROPORTIONAL, ((bmpUse & CLBF_PROPORTIONAL) ? BST_CHECKED : BST_UNCHECKED));
			}
			{
				HRESULT (STDAPICALLTYPE *MySHAutoComplete)(HWND,DWORD);

				MySHAutoComplete = (HRESULT (STDAPICALLTYPE*)(HWND,DWORD))GetProcAddress(GetModuleHandle("shlwapi"), "SHAutoComplete");
				if (MySHAutoComplete)
					MySHAutoComplete(GetDlgItem(hwndDlg, IDC_FILENAME), 1);
			}
			return TRUE;
		}
		case M_REFRESHBKGBOXES:
		{
			BOOL bEnable = IsDlgButtonChecked(hwndDlg, IDC_BITMAP);

			EnableWindow(GetDlgItem(hwndDlg, IDC_FILENAME), bEnable);
			EnableWindow(GetDlgItem(hwndDlg, IDC_BROWSE), bEnable);
			EnableWindow(GetDlgItem(hwndDlg, IDC_STRETCHH), bEnable);
			EnableWindow(GetDlgItem(hwndDlg, IDC_STRETCHV), bEnable);
			EnableWindow(GetDlgItem(hwndDlg, IDC_TILEH), bEnable);
			EnableWindow(GetDlgItem(hwndDlg, IDC_TILEV), bEnable);
			EnableWindow(GetDlgItem(hwndDlg, IDC_PROPORTIONAL), bEnable);
			break;
		}
		case M_REFRESHBORDERPICKERS:
		{
			BOOL bEnable = IsDlgButtonChecked(hwndDlg, IDC_DRAWBORDER);

			EnableWindow(GetDlgItem(hwndDlg, IDC_LTEDGESCOLOR), bEnable);
			EnableWindow(GetDlgItem(hwndDlg, IDC_RBEDGESCOLOR), bEnable);
			break;
		}
		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				case IDC_DRAWBORDER:
				{
					SendMessage(hwndDlg, M_REFRESHBORDERPICKERS, 0, 0);
					break;
				}
				case IDC_BROWSE:
				{
					char str[MAX_PATH];
					OPENFILENAME ofn={0};
					char filter[512];

					GetDlgItemText(hwndDlg, IDC_FILENAME, str, sizeof(str));
					ofn.lStructSize = sizeof(OPENFILENAME);
					ofn.hwndOwner = hwndDlg;
					ofn.hInstance = NULL;
					CallService(MS_UTILS_GETBITMAPFILTERSTRINGS, sizeof(filter), (LPARAM)filter);
					ofn.lpstrFilter = filter;
					ofn.lpstrFile = str;
					ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
					ofn.nMaxFile = sizeof(str);
					ofn.nMaxFileTitle = MAX_PATH;
					ofn.lpstrDefExt = "bmp";
					if (!GetOpenFileName(&ofn))
						return FALSE;
					SetDlgItemText(hwndDlg, IDC_FILENAME, str);
					break;
				}
				case IDC_FILENAME:
				{
					if (EN_CHANGE != HIWORD(wParam) || (HWND)lParam != GetFocus())
						return FALSE;
					break;
				}
				case IDC_BITMAP:
				{
					SendMessage(hwndDlg, M_REFRESHBKGBOXES, 0, 0);
					break;
				}
			}
			SendMessage(GetParent(hwndDlg), PSM_CHANGED, 0, 0);
			break;
		}
		case WM_NOTIFY:
		{
			LPNMHDR phdr = (LPNMHDR)(lParam);
			
			if (0 == phdr->idFrom)
			{
				switch (phdr->code) 
				{
					case PSN_APPLY:
					{
						// Border
						DBWriteContactSettingByte(NULL, szModuleDB, "DrawBorder", (BYTE)IsDlgButtonChecked(hwndDlg, IDC_DRAWBORDER));
						{	
							COLORREF col;

							col = SendDlgItemMessage(hwndDlg, IDC_LTEDGESCOLOR, CPM_GETCOLOUR, 0, 0);
							DBWriteContactSettingDword(NULL, szModuleDB, "LTEdgesColor", col);
							col = SendDlgItemMessage(hwndDlg, IDC_RBEDGESCOLOR, CPM_GETCOLOUR, 0, 0);
							DBWriteContactSettingDword(NULL, szModuleDB, "RBEdgesColor", col);
						}

						// Backgroud
						{	
							COLORREF col;

							col = SendDlgItemMessage(hwndDlg, IDC_BKGCOLOUR, CPM_GETCOLOUR, 0, 0);
							DBWriteContactSettingDword(NULL, szModuleDB, "BkColor", col);
						}
						DBWriteContactSettingByte(NULL, szModuleDB, "BkUseBitmap", (BYTE)IsDlgButtonChecked(hwndDlg, IDC_BITMAP));
						{	
							char str[MAX_PATH];

							GetDlgItemText(hwndDlg, IDC_FILENAME, str, sizeof(str));
							DBWriteContactSettingString(NULL, szModuleDB, "BkBitmap", str);
						}
						{
							WORD flags = 0;

							if (IsDlgButtonChecked(hwndDlg, IDC_STRETCHH))
								flags |= CLB_STRETCHH;
							if (IsDlgButtonChecked(hwndDlg, IDC_STRETCHV))
								flags |= CLB_STRETCHV;
							if (IsDlgButtonChecked(hwndDlg, IDC_TILEH))
								flags |= CLBF_TILEH;
							if (IsDlgButtonChecked(hwndDlg, IDC_TILEV))
								flags |= CLBF_TILEV;
							if (IsDlgButtonChecked(hwndDlg, IDC_PROPORTIONAL))
								flags |= CLBF_PROPORTIONAL;
							DBWriteContactSettingWord(NULL, szModuleDB, "BkBitmapOpt", flags);
						}

						ApplyOptionsChanges();
						OnStatusChanged();
						return TRUE;
					}
				}
			}
			break;
		}
	}
	return FALSE;
}
