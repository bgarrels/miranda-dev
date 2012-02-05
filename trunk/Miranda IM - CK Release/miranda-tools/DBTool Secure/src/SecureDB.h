#ifndef _SECURE_DB__
#define _SECURE_DB__


int EncReadFile(HANDLE hFile,void* data,unsigned long toread,unsigned long* read,void* ov);
int EncWriteFile(HANDLE hFile,void* data,unsigned long towrite,unsigned long* written,void* ov);
int EncGetPassword(void* dbh,const char* dbase);
void LanguageChanged(HWND hDlg);
//int EncInitMenus(WPARAM wParam, LPARAM lParam);
//int EncOnLoad();

#endif //_SECURE_DB__