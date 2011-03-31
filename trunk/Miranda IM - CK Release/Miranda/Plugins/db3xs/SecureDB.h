#ifndef _SECURE_DB__
#define _SECURE_DB__

#include <m_clist.h>
#include <m_icolib.h>

typedef HRESULT (WINAPI *PFNDwmIsCompositionEnabled)(BOOL *);

int EncReadFile(HANDLE hFile,void* data,unsigned int toread,unsigned int* read,void* ov);
int EncWriteFile(HANDLE hFile,void* data,unsigned int towrite,unsigned int* written,void* ov);
int EncGetPassword(void* dbh,const char* dbase);
INT_PTR DB3XSSetPassword(WPARAM wParam, LPARAM lParam);
int DB3XSRemovePassword(WPARAM wParam, LPARAM lParam);
int dbxOnLoad();
int dbxMenusInit(WPARAM wParam, LPARAM lParam);
void xModifyMenu(HANDLE hMenu, TCHAR* name);
void LanguageChanged(hDlg);
HMODULE hDwmApi;
PFNDwmIsCompositionEnabled dwmIsCompositionEnabled;

extern int g_secured;
extern HINSTANCE g_hInst;
extern HANDLE hSetPwdMenu;

#endif //_SECURE_DB__
