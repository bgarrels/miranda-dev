/*
Author Robert Rainwater

Miranda IM: the free IM client for Microsoft* Windows*

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

===============================================================================
*/

/*
 * test for gdi+
 */
#include <commonheaders.h>
#include <shobjidl.h>

#define _WIN32_WINNT 0x0601
#define _WIN32_IE 0x0601

#define _CLN_GDIP


#undef Translate

void RemoveFromTaskBar(HWND hWnd)
{
    ITaskbarList *pTaskbarList = NULL;

    if (SUCCEEDED(CoCreateInstance(CLSID_TaskbarList, 0, CLSCTX_INPROC_SERVER, IID_ITaskbarList,
                                   (void **)(&pTaskbarList))) &&	pTaskbarList != NULL) {
        if (SUCCEEDED(pTaskbarList->HrInit())) {
            pTaskbarList->DeleteTab(hWnd);
        }
        pTaskbarList->Release();
    }
}

void AddToTaskBar(HWND hWnd)
{
    ITaskbarList *pTaskbarList = NULL;

    if (SUCCEEDED(CoCreateInstance(CLSID_TaskbarList, 0, CLSCTX_INPROC_SERVER, IID_ITaskbarList,
                                   (void **)(&pTaskbarList))) &&	pTaskbarList != NULL) {
        if (SUCCEEDED(pTaskbarList->HrInit())) {
            pTaskbarList->AddTab(hWnd);
        }
        pTaskbarList->Release();
    }
}
