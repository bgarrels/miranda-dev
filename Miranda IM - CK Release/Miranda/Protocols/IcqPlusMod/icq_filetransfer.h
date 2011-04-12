typedef struct tagPROTOFILETRANSFERSTATUS_W
{
    size_t cbSize;
    HANDLE hContact;
    DWORD  flags;      // one of PFTS_* constants

    union
    {
        char **pszFiles;
        TCHAR **ptszFiles;
        WCHAR **pwszFiles;
    };

    int totalFiles;
    int currentFileNumber;
    unsigned __int64 totalBytes;
    unsigned __int64 totalProgress;

    union
    {
        char *szWorkingDir;
        TCHAR *tszWorkingDir;
        WCHAR *wszWorkingDir;
    };

    union
    {
        char *szCurrentFile;
        TCHAR *tszCurrentFile;
        WCHAR *wszCurrentFile;
    };

    unsigned __int64 currentFileSize;
    unsigned __int64 currentFileProgress;
    unsigned __int64 currentFileTime;  //as seconds since 1970
}
PROTOFILETRANSFERSTATUS_W;
