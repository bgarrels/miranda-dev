/*
Miranda SmileyAdd Plugin
Copyright (C) 2005 - 2011 Boris Krasnovskiy
Copyright (C) 2003 - 2004 Rein-Peter de Boer

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

#ifndef SMILEYADD_OPTIONS_H_
#define SMILEYADD_OPTIONS_H_

int SmileysOptionsInitialize(WPARAM wParam, LPARAM lParam);

class OptionsType
{
public:
	bool PluginSupportEnabled;
	bool EnforceSpaces;
	bool ScaleToTextheight;
	bool UseOneForAll;
	bool SurroundSmileyWithSpaces;
	bool ScaleAllSmileys;
	bool IEViewStyle;
	bool AnimateSel;
	bool AnimateDlg;
	bool InputSmileys;
	bool DCursorSmiley;
	bool DisableCustom;
    bool HQScaling;
	unsigned ButtonStatus;
	unsigned SelWndBkgClr;
	unsigned MaxCustomSmileySize;
    unsigned MinSmileySize;

	void Load(void);
	void Save(void);
	static void ReadPackFileName(bkstring& filename, const bkstring& name, 
		const bkstring& defaultFilename);
	static void ReadCustomCategories(bkstring& cats);
	static void ReadContactCategory(HANDLE hContact, bkstring& cats);
	static void WritePackFileName(const bkstring& filename, const bkstring& name);
	static void WriteCustomCategories(const bkstring& cats);
	static void WriteContactCategory(HANDLE hContact, const bkstring& cats);
};

extern OptionsType opt;

#endif // SMILEYADD_OPTIONS_H_

