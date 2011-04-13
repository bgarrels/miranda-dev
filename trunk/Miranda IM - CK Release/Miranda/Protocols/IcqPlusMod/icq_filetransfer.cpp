// ---------------------------------------------------------------------------80
//                ICQ plugin for Miranda Instant Messenger
//                ________________________________________
//
// Copyright © 2000,2001 Richard Hughes, Roland Rabien, Tristan Van de Vreede
// Copyright © 2001,2002 Jon Keating, Richard Hughes
// Copyright © 2002,2003,2004 Martin Öberg, Sam Kothari, Robert Rainwater
// Copyright © 2004,2005,2006,2007 Joe Kucera
// Copyright © 2006,2007 [sss], chaos.persei, [sin], Faith Healer, Theif, nullbie
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
// -----------------------------------------------------------------------------
//
// File name      : $Source$
// Revision       : $Revision: 36 $
// Last change on : $Date: 2007-08-05 03:45:10 +0300 (Ð’Ñ, 05 Ð°Ð²Ð³ 2007) $
// Last change by : $Author: sss123next $
//
// DESCRIPTION:
//
//  Describe me here please...
//
// -----------------------------------------------------------------------------

#include "icqoscar.h"
#include "icq_filetransfer.h"

extern DWORD _mirandaVersion;

static void file_buildProtoFileTransferStatus(filetransfer* ft, PROTOFILETRANSFERSTATUS* pfts)
{
    ZeroMemory(pfts, sizeof(PROTOFILETRANSFERSTATUS));
    pfts->cbSize = sizeof(PROTOFILETRANSFERSTATUS);
    pfts->hContact = ft->hContact;
    pfts->sending = ft->sending;
    if (ft->sending)
        pfts->files = ft->pszFiles;
    else
        pfts->files = NULL;  /* FIXME */
    pfts->totalFiles = ft->dwFileCount;
    pfts->currentFileNumber = ft->iCurrentFile;
    pfts->totalBytes = ft->dwTotalSize;
    pfts->totalProgress = ft->dwBytesDone;
    pfts->workingDir = ft->szSavePath;
    pfts->currentFile = ft->szThisFile;
    pfts->currentFileSize = ft->dwThisFileSize;
    pfts->currentFileTime = ft->dwThisFileDate;
    pfts->currentFileProgress = ft->dwFileBytesDone;
}
#ifndef _08CORE
static void file_buildProtoFileTransferStatus_w(filetransfer* ft, PROTOFILETRANSFERSTATUS_W* pfts) //this need to be fixed
{
    ZeroMemory(pfts, sizeof(PROTOFILETRANSFERSTATUS_W));
    pfts->cbSize = sizeof(PROTOFILETRANSFERSTATUS_W);
    pfts->hContact = ft->hContact;
    pfts->pszFiles =ft->pszFiles;
    pfts->flags = PFTS_UTF | ((ft->sending) ? PFTS_SENDING : PFTS_RECEIVING);
    pfts->totalFiles = ft->dwFileCount;
    pfts->currentFileNumber = ft->iCurrentFile;
    pfts->totalBytes=ft->dwTotalSize;
    pfts->totalProgress=ft->dwBytesDone;
    pfts->szWorkingDir= ft->szSavePath;
    pfts->currentFileNumber = ft->iCurrentFile;
    pfts->currentFileProgress = ft->dwFileBytesDone;
    pfts->currentFileSize = ft->dwThisFileSize;
    pfts->currentFileTime = ft->dwThisFileDate;
}
#endif

static void file_sendTransferSpeed(directconnect* dc)
{
    icq_packet packet;

    directPacketInit(&packet, 5);
    packByte(&packet, PEER_FILE_SPEED);    /* Ident */
    packLEDWord(&packet, dc->ft->dwTransferSpeed);
    sendDirectPacket(dc, &packet);
}



static void file_sendNick(directconnect* dc)
{
    icq_packet packet;
    char* szNick;
    WORD wNickLen;
    DBVARIANT dbv = {DBVT_DELETED};


    dbv.type = DBVT_DELETED;
    if (getSettingString(NULL, "Nick", &dbv))
        szNick = "";
    else
        szNick = dbv.pszVal;

    wNickLen = strlennull(szNick);

    directPacketInit(&packet, (WORD)(8 + wNickLen));
    packByte(&packet, PEER_FILE_INIT_ACK); /* Ident */
    packLEDWord(&packet, dc->ft->dwTransferSpeed);
    packLEWord(&packet, (WORD)(wNickLen + 1));
    packBuffer(&packet, szNick, (WORD)(wNickLen + 1));
    sendDirectPacket(dc, &packet);
    ICQFreeVariant(&dbv);
}



static void file_sendNextFile(directconnect* dc)
{
    icq_packet packet;
    struct _stati64 statbuf;
    const char *pszThisFileName;
    char szThisSubDir[MAX_PATH];
    char *szThisFileNameAnsi = NULL, *szThisSubDirAnsi = NULL;
    WORD wThisFileNameLen, wThisSubDirLen;


    if (dc->ft->iCurrentFile >= (int)dc->ft->dwFileCount)
    {
        BroadcastAck(dc->ft->hContact, ACKTYPE_FILE, ACKRESULT_SUCCESS, dc->ft, 0);
        CloseDirectConnection(dc);
        dc->ft->hConnection = NULL;
        return;
    }

    dc->ft->szThisFile = dc->ft->pszFiles[dc->ft->iCurrentFile];
    if (FileStatUtf(dc->ft->szThisFile, &statbuf))
    {
        icq_LogMessage(LOG_ERROR, "Your file transfer has been aborted because one of the files that you selected to send is no longer readable from the disk. You may have deleted or moved it.");
        CloseDirectConnection(dc);
        dc->ft->hConnection = NULL;
        return;
    }

    pszThisFileName = FindFilePathContainer((const char**)dc->ft->pszFiles, dc->ft->iCurrentFile, szThisSubDir);

    if (statbuf.st_mode&_S_IFDIR)
    {
        dc->ft->currentIsDir = 1;
    }
    else
    {
        dc->ft->currentIsDir = 0;
        dc->ft->fileId = OpenFileUtf(dc->ft->szThisFile, _O_BINARY | _O_RDONLY, _S_IREAD);
        if (dc->ft->fileId == -1)
        {
            icq_LogMessage(LOG_ERROR, "Your file transfer has been aborted because one of the files that you selected to send is no longer readable from the disk. You may have deleted or moved it.");
            CloseDirectConnection(dc);
            dc->ft->hConnection = NULL;
            return;
        }

    }
    dc->ft->dwThisFileSize = statbuf.st_size;
    dc->ft->dwThisFileDate = statbuf.st_mtime;
    dc->ft->dwFileBytesDone = 0;

    /*  wThisFileNameLen = strlennull(pszThisFileName);
      wThisSubDirLen = strlennull(szThisSubDir); */


    szThisFileNameAnsi = mir_utf8decodeA(pszThisFileName);
    szThisSubDirAnsi = mir_utf8decodeA(szThisSubDir);
    wThisFileNameLen = strlennull(szThisFileNameAnsi);
    wThisSubDirLen = strlennull(szThisSubDirAnsi);


    directPacketInit(&packet, (WORD)(20 + wThisFileNameLen + wThisSubDirLen));
    packByte(&packet, PEER_FILE_NEXTFILE); /* Ident */
    packByte(&packet, (BYTE)((statbuf.st_mode & _S_IFDIR) != 0)); // Is subdir
    packLEWord(&packet, (WORD)(wThisFileNameLen + 1));
    packBuffer(&packet, szThisFileNameAnsi, (WORD)(wThisFileNameLen + 1));
    packLEWord(&packet, (WORD)(wThisSubDirLen + 1));
    packBuffer(&packet, szThisSubDirAnsi, (WORD)(wThisSubDirLen + 1));
    packLEDWord(&packet, dc->ft->dwThisFileSize);
    packLEDWord(&packet, statbuf.st_mtime);
    packLEDWord(&packet, dc->ft->dwTransferSpeed);
    mir_free(szThisFileNameAnsi);
    mir_free(szThisSubDirAnsi);

    sendDirectPacket(dc, &packet);

    BroadcastAck(dc->ft->hContact, ACKTYPE_FILE, ACKRESULT_NEXTFILE, dc->ft, 0);
}



static void file_sendResume(directconnect* dc)
{
    icq_packet packet;

    directPacketInit(&packet, 17);
    packByte(&packet, PEER_FILE_RESUME);            /* Ident */
    packLEDWord(&packet, dc->ft->dwFileBytesDone);  /* file resume */
    packLEDWord(&packet, 0);                        /* unknown */
    packLEDWord(&packet, dc->ft->dwTransferSpeed);
    packLEDWord(&packet, dc->ft->iCurrentFile + 1); /* file number */
    sendDirectPacket(dc, &packet);
}



static void file_sendData(directconnect* dc)
{
    BYTE buf[2048];
    int bytesRead = 0;

    if (!dc->ft->currentIsDir)
    {
        icq_packet packet;

        if (dc->ft->fileId == -1)
            return;
        bytesRead = _read(dc->ft->fileId, buf, sizeof(buf));
        if (bytesRead == -1)
            return;

        directPacketInit(&packet, (WORD)(1 + bytesRead));
        packByte(&packet, PEER_FILE_DATA);   /* Ident */
        packBuffer(&packet, (const char*)&buf, (WORD)bytesRead);
        sendDirectPacket(dc, &packet);
    }

    dc->ft->dwBytesDone += bytesRead;
    dc->ft->dwFileBytesDone += bytesRead;

    if (GetTickCount() > dc->ft->dwLastNotify + 500 || bytesRead == 0)
    {
        if((_mirandaVersion < PLUGIN_MAKE_VERSION(0, 9, 0, 1)) || (_mirandaVersion > PLUGIN_MAKE_VERSION(0, 9, 0, 6))) //this will be removed in near future, if miranda api do not change again
        {
            PROTOFILETRANSFERSTATUS pfts;
            file_buildProtoFileTransferStatus(dc->ft, &pfts);
            BroadcastAck(dc->ft->hContact, ACKTYPE_FILE, ACKRESULT_DATA, dc->ft, (LPARAM)&pfts);
        }
#ifndef _08CORE
        else
        {
            PROTOFILETRANSFERSTATUS_W pfts;
            file_buildProtoFileTransferStatus_w(dc->ft, &pfts);
            BroadcastAck(dc->ft->hContact, ACKTYPE_FILE, ACKRESULT_DATA, dc->ft, (LPARAM)&pfts);
        }
#endif

        dc->ft->dwLastNotify = GetTickCount();
    }

    if (bytesRead == 0)
    {
        if (!dc->ft->currentIsDir) _close(dc->ft->fileId);
        dc->ft->fileId = -1;
        dc->wantIdleTime = 0;
        dc->ft->iCurrentFile++;
        file_sendNextFile(dc);   /* this will close the socket if no more files */
    }
}



void handleFileTransferIdle(directconnect* dc)
{
    file_sendData(dc);
}



void icq_sendFileResume(filetransfer* ft, int action, const char* szFilename)
{
    int openFlags;
    directconnect *dc;


    if (ft->hConnection == NULL)
        return;

    dc = FindFileTransferDC(ft);
    if (!dc) return; // something is broken...

    switch (action)
    {
    case FILERESUME_RESUME:
        openFlags = _O_BINARY | _O_WRONLY;
        break;

    case FILERESUME_OVERWRITE:
        openFlags = _O_BINARY | _O_CREAT | _O_TRUNC | _O_WRONLY;
        ft->dwFileBytesDone = 0;
        break;

    case FILERESUME_SKIP:
        openFlags = _O_BINARY | _O_WRONLY;
        ft->dwFileBytesDone = ft->dwThisFileSize;
        break;

    case FILERESUME_RENAME:
        openFlags = _O_BINARY | _O_CREAT | _O_TRUNC | _O_WRONLY;
        mir_free(ft->szThisFile);
        ft->szThisFile = null_strdup(szFilename);
        ft->dwFileBytesDone = 0;
        break;
    }

    ft->fileId = OpenFileUtf(ft->szThisFile, openFlags, _S_IREAD | _S_IWRITE);
    if (ft->fileId == -1)
    {
        icq_LogMessage(LOG_ERROR, "Your file receive has been aborted because Miranda could not open the destination file in order to write to it. You may be trying to save to a read-only folder.");
        NetLib_CloseConnection(&ft->hConnection, FALSE);
        return;
    }

    if (action == FILERESUME_RESUME)
        ft->dwFileBytesDone = _lseek(ft->fileId, 0, SEEK_END);
    else
        _lseek(ft->fileId, ft->dwFileBytesDone, SEEK_SET);

    ft->dwBytesDone += ft->dwFileBytesDone;

    file_sendResume(dc);

    BroadcastAck(ft->hContact, ACKTYPE_FILE, ACKRESULT_NEXTFILE, ft, 0);
}



// small utility function
void NormalizeBackslash(char* path)
{
    int len = strlennull(path);

    if (len && path[len-1] != '\\') strcat(path, "\\");
}



/* a file transfer looks like this:
S: 0
R: 5
R: 1
S: 2
R: 3
S: 6 * many
(for more files, send 2, 3, 6*many)
*/
void handleFileTransferPacket(directconnect* dc, PBYTE buf, WORD wLen)
{
    if (wLen < 1)
        return;

    NetLog_Direct("Handling file packet");

    switch (buf[0])
    {
    case PEER_FILE_INIT:   /* first packet of a file transfer */
        if (dc->initialised)
            return;
        if (wLen < 19)
            return;
        buf += 5;  /* id, and unknown 0 */
        dc->type = DIRECTCONN_FILE;
        {
            DWORD dwFileCount;
            DWORD dwTotalSize;
            DWORD dwTransferSpeed;
            WORD wNickLength;
            int bAdded;

            unpackLEDWord(&buf, &dwFileCount);
            unpackLEDWord(&buf, &dwTotalSize);
            unpackLEDWord(&buf, &dwTransferSpeed);
            unpackLEWord(&buf, &wNickLength);

            dc->ft = FindExpectedFileRecv(dc->dwRemoteUin, dwTotalSize);
            if (dc->ft == NULL)
            {
                NetLog_Direct("Unexpected file receive");
                CloseDirectConnection(dc);
                return;
            }
            dc->ft->dwFileCount = dwFileCount;
            dc->ft->dwTransferSpeed = dwTransferSpeed;
            dc->ft->hContact = HContactFromUIN(dc->ft->dwUin, &bAdded);
            dc->ft->dwBytesDone = 0;
            dc->ft->iCurrentFile = -1;
            dc->ft->fileId = -1;
            dc->ft->hConnection = dc->hConnection;
            dc->ft->dwLastNotify = GetTickCount();

            dc->initialised = 1;

            file_sendTransferSpeed(dc);
            file_sendNick(dc);
        }
        BroadcastAck(dc->ft->hContact, ACKTYPE_FILE, ACKRESULT_INITIALISING, dc->ft, 0);
        break;

    case PEER_FILE_INIT_ACK:
        if (wLen < 8)
            return;
        buf++;
        unpackLEDWord(&buf, &dc->ft->dwTransferSpeed);
        /* followed by nick */
        file_sendNextFile(dc);
        break;

    case PEER_FILE_NEXTFILE:
        if (wLen < 20)
            return;
        buf++;  /* id */
        {
            char *szAnsi;
            WORD wThisFilenameLen, wSubdirLen;
            BYTE isDirectory;
            char *szFullPath;

            unpackByte(&buf, &isDirectory);
            unpackLEWord(&buf, &wThisFilenameLen);
            if (wLen < 19 + wThisFilenameLen)
                return;
            mir_free(dc->ft->szThisFile);
            szAnsi = (char *)icq_alloc_zero(wThisFilenameLen + 1);
            memcpy(szAnsi, buf, wThisFilenameLen);
            szAnsi[wThisFilenameLen] = '\0';
            dc->ft->szThisFile = mir_utf8encode(szAnsi);
            buf += wThisFilenameLen;

            unpackLEWord(&buf, &wSubdirLen);
            if (wLen < 18 + wThisFilenameLen + wSubdirLen)
                return;
            mir_free(dc->ft->szThisSubdir);
            szAnsi = (char *)icq_alloc_zero(wSubdirLen + 1);
            memcpy(szAnsi, buf, wSubdirLen);
            szAnsi[wSubdirLen] = '\0';
            dc->ft->szThisSubdir = mir_utf8encode(szAnsi);
            buf += wSubdirLen;

            unpackLEDWord(&buf, &dc->ft->dwThisFileSize);
            unpackLEDWord(&buf,  &dc->ft->dwThisFileDate);
            unpackLEDWord(&buf,  &dc->ft->dwTransferSpeed);

            /* no cheating with paths */
            if (!IsValidRelativePath(dc->ft->szThisFile) || !IsValidRelativePath(dc->ft->szThisSubdir))
            {
                NetLog_Direct("Invalid path information");
                break;
            }

            szFullPath = (char*)icq_alloc_zero(strlennull(dc->ft->szSavePath)+strlennull(dc->ft->szThisSubdir)+strlennull(dc->ft->szThisFile)+3);
            strcpy(szFullPath, dc->ft->szSavePath);
            NormalizeBackslash(szFullPath);
            strcat(szFullPath, dc->ft->szThisSubdir);
            NormalizeBackslash(szFullPath);
//        _chdir(szFullPath); // set current dir - not very useful
            strcat(szFullPath, dc->ft->szThisFile);
            // we joined the full path to dest file
            mir_free(dc->ft->szThisFile);
            dc->ft->szThisFile = szFullPath;

            dc->ft->dwFileBytesDone = 0;
            dc->ft->iCurrentFile++;

            if (isDirectory)
            {
                MakeDirUtf(dc->ft->szThisFile);
                dc->ft->fileId = -1;
            }
            else
            {
                /* file resume */
                if((_mirandaVersion < PLUGIN_MAKE_VERSION(0, 9, 0, 1)) || (_mirandaVersion > PLUGIN_MAKE_VERSION(0, 9, 0, 6))) //this will be removed in near future, if miranda api do not change again
                {
                    PROTOFILETRANSFERSTATUS pfts = {0};
                    file_buildProtoFileTransferStatus(dc->ft, &pfts);
                    if (BroadcastAck(dc->ft->hContact, ACKTYPE_FILE, ACKRESULT_FILERESUME, dc->ft, (LPARAM)&pfts))
                        break;   /* UI supports resume: it will call PS_FILERESUME */
                    dc->ft->fileId = OpenFileUtf(dc->ft->szThisFile, _O_BINARY | _O_CREAT | _O_TRUNC | _O_WRONLY, _S_IREAD | _S_IWRITE);
                }
#ifndef _08CORE
                else
                {
                    PROTOFILETRANSFERSTATUS_W pfts = {0};
                    file_buildProtoFileTransferStatus_w(dc->ft, &pfts);
                    if (BroadcastAck(dc->ft->hContact, ACKTYPE_FILE, ACKRESULT_FILERESUME, dc->ft, (LPARAM)&pfts))
                        break;   /* UI supports resume: it will call PS_FILERESUME */
                    dc->ft->fileId = OpenFileUtf(dc->ft->szThisFile, _O_BINARY | _O_CREAT | _O_TRUNC | _O_WRONLY, _S_IREAD | _S_IWRITE);
                }
#endif

                if (dc->ft->fileId == -1)
                {
                    icq_LogMessage(LOG_ERROR, "Your file receive has been aborted because Miranda could not open the destination file in order to write to it. You may be trying to save to a read-only folder.");
                    CloseDirectConnection(dc);
                    dc->ft->hConnection = NULL;
                    break;
                }
            }
        }
        file_sendResume(dc);
        BroadcastAck(dc->ft->hContact, ACKTYPE_FILE, ACKRESULT_NEXTFILE, dc->ft, 0);
        break;

    case PEER_FILE_RESUME:
        if (dc->ft->fileId == -1 && !dc->ft->currentIsDir)
            return;
        if (wLen < 13)
            return;
        if (wLen < 17)
            NetLog_Direct("Warning: Received short PEER_FILE_RESUME");
        buf++;
        {
            DWORD dwRestartFrom;

            unpackLEDWord(&buf, &dwRestartFrom);
            if (dwRestartFrom > dc->ft->dwThisFileSize)
                return;
            buf += 4;  /* unknown. 0 */
            unpackLEDWord(&buf, &dc->ft->dwTransferSpeed);
            buf += 4;  /* unknown. 1 */
            if (!dc->ft->currentIsDir)
                _lseek(dc->ft->fileId, dwRestartFrom, 0);
            dc->wantIdleTime = 1;
            dc->ft->dwBytesDone += dwRestartFrom;
            dc->ft->dwFileBytesDone += dwRestartFrom;
        }
        break;

    case PEER_FILE_SPEED:
        if (wLen < 5)
            return;
        buf++;
        unpackLEDWord(&buf, &dc->ft->dwTransferSpeed);
        dc->ft->dwLastNotify = GetTickCount();
        break;

    case PEER_FILE_DATA:
        if (!dc->ft->currentIsDir)
        {
            if (dc->ft->fileId == -1)
                break;
            buf++;
            wLen--;
            _write(dc->ft->fileId, buf, wLen);
        }
        else
            wLen = 0;
        dc->ft->dwBytesDone += wLen;
        dc->ft->dwFileBytesDone += wLen;
        if (GetTickCount() > dc->ft->dwLastNotify + 500 || wLen < 2048)
        {
            if((_mirandaVersion < PLUGIN_MAKE_VERSION(0, 9, 0, 1)) || (_mirandaVersion > PLUGIN_MAKE_VERSION(0, 9, 0, 6))) //this will be removed in near future, if miranda api do not change again
            {
                PROTOFILETRANSFERSTATUS pfts;
                file_buildProtoFileTransferStatus(dc->ft, &pfts);
                BroadcastAck(dc->ft->hContact, ACKTYPE_FILE, ACKRESULT_DATA, dc->ft, (LPARAM)&pfts);
            }
#ifndef _08CORE
            else
            {
                PROTOFILETRANSFERSTATUS_W pfts;
                file_buildProtoFileTransferStatus_w(dc->ft, &pfts);
                BroadcastAck(dc->ft->hContact, ACKTYPE_FILE, ACKRESULT_DATA, dc->ft, (LPARAM)&pfts);
            }
#endif
            dc->ft->dwLastNotify = GetTickCount();
        }
        if (wLen < 2048)
        {
            /* EOF */
            if (!dc->ft->currentIsDir)
                _close(dc->ft->fileId);
            dc->ft->fileId = -1;
            if ((DWORD)dc->ft->iCurrentFile == dc->ft->dwFileCount - 1)
            {
                dc->type = DIRECTCONN_CLOSING;     /* this guarantees that we won't accept any more data but that the sender is still free to closesocket() neatly */
                BroadcastAck(dc->ft->hContact, ACKTYPE_FILE, ACKRESULT_SUCCESS, dc->ft, 0);
            }
        }
        break;

    default:
        NetLog_Direct("Unknown file transfer packet ignored.");
        break;
    }
}
