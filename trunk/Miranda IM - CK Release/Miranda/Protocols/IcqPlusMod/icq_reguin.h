#ifndef __ICQ_REGUIN_H
#define __ICQ_REGUIN_H

extern int icq_regNewUin;
extern char icq_regpassword[10];
extern DWORD icq_regedUin;
extern HWND hwndRegImageDialog;

extern BOOL bImageRequested; //need to simply connect to ICQ if network error during registering

void InitReguin();
void icq_requestRegImage(HWND hwndDlg);
void icq_registerNewUin(char* password, char* image);

#endif /* __ICQ_REGUIN_H */
