{$IMAGEBASE $13000000}
{$include compilers.inc}
library WATrack;
uses
  {$IFNDEF FPC}fastmm4,{$ENDIF}
  m_api,dbsettings,activex,winampapi,
  Windows,messages,commctrl,//uxtheme,
  srv_format,srv_player,wat_api,wrapper,
  common,syswin,HlpDlg,mirutils
  ,global,waticons,io,macros
  ,lastfm    in 'lastfm\lastfm.pas'
  ,statlog   in 'stat\statlog.pas'
  ,popups    in 'popup\popups.pas'
  ,proto     in 'proto\proto.pas'
  ,status    in 'status\status.pas'
  ,tmpl      in 'status\tmpl.pas'
  ,templates in 'templates\templates.pas'
{$IFDEF KOL_MCK}
  ,kolframe  in 'kolframe\kolframe.pas'
{$ENDIF}
  {$include lst_players.inc}
  {$include lst_formats.inc}
;

{$include res\i_const.inc}

{$Resource res\watrack.res}

{$include i_vars.inc}

var
  PluginInterfaces:array [0..1] of MUUID;

const
  MenuDisablePos = 500050000;

// Updater compatibility data
const
  VersionURL        = 'http://addons.miranda-im.org/details.php?action=viewfile&id=2345';
  VersionPrefix     = '<span class="fileNameHeader">WATrack ';
  UpdateURL         = 'http://addons.miranda-im.org/feed.php?dlfile=2345';
  BetaVersionURL    = 'http://awkward.miranda.im/index.htm';
  BetaVersionPrefix = 'WATrack beta version ';
  BetaUpdateURL     = 'http://awkward.miranda.im/watrack.zip';
  BetaChangelogURL  = nil; //'http://awkward.mirandaim.ru/watrack.txt';

function MirandaPluginInfoEx(mirandaVersion:DWORD):PPLUGININFOEX; cdecl;
begin
  result:=@PluginInfo;
  PluginInfo.cbSize     :=SizeOf(TPLUGININFOEX);
  PluginInfo.shortName  :=PluginName;
  PluginInfo.version    :=$0000060C;
  PluginInfo.description:='Paste played music info into message window or status text';
  PluginInfo.author     :='Awkward';
  PluginInfo.authorEmail:='panda75@bk.ru; awk1975@ya.ru';
  PluginInfo.copyright  :='(c) 2005-2011 Awkward';
  PluginInfo.homepage   :='http://code.google.com/p/delphi-miranda-plugins/';
  PluginInfo.flags      :=UNICODE_AWARE;
  PluginInfo.replacesDefaultModule:=0;
  PluginInfo.uuid       :=MIID_WATRACK;
end;

{$include i_options.inc}
{$include i_timer.inc}
{$include i_gui.inc}
{$include i_opt_dlg.inc}
{$include i_cover.inc}

function ReturnInfo(enc:WPARAM;cp:LPARAM=CP_ACP):pointer;
begin
  if enc<>WAT_INF_UNICODE then
  begin
    ClearSongInfoData(TSongInfo(SongInfoA),true);
    move(SongInfo,SongInfoA,SizeOf(tSongInfo));
    with SongInfoA do
    begin
      FastWideToANSI(SongInfo.url,url);
      if enc=WAT_INF_ANSI then
      begin
        WideToANSI(SongInfo.artist ,artist ,cp);
        WideToANSI(SongInfo.title  ,title  ,cp);
        WideToANSI(SongInfo.album  ,album  ,cp);
        WideToANSI(SongInfo.genre  ,genre  ,cp);
        WideToANSI(SongInfo.comment,comment,cp);
        WideToANSI(SongInfo.year   ,year   ,cp);
        WideToANSI(SongInfo.mfile  ,mfile  ,cp);
        WideToANSI(SongInfo.wndtext,wndtext,cp);
        WideToANSI(SongInfo.player ,player ,cp);
        WideToANSI(SongInfo.txtver ,txtver ,cp);
        WideToANSI(SongInfo.lyric  ,lyric  ,cp);
        WideToANSI(SongInfo.cover  ,cover  ,cp);
        WideToANSI(SongInfo.url    ,url    ,cp);
      end
      else
      begin
        WideToUTF8(SongInfo.artist ,artist);
        WideToUTF8(SongInfo.title  ,title);
        WideToUTF8(SongInfo.album  ,album);
        WideToUTF8(SongInfo.genre  ,genre);
        WideToUTF8(SongInfo.comment,comment);
        WideToUTF8(SongInfo.year   ,year);
        WideToUTF8(SongInfo.mfile  ,mfile);
        WideToUTF8(SongInfo.wndtext,wndtext);
        WideToUTF8(SongInfo.player ,player);
        WideToUTF8(SongInfo.txtver ,txtver);
        WideToUTF8(SongInfo.lyric  ,lyric);
        WideToUTF8(SongInfo.cover  ,cover);
        WideToUTF8(SongInfo.url    ,url);
      end;
    end;
    result:=@SongInfoA;
  end
  else
    result:=@SongInfo;
end;

function WATReturnGlobal(wParam:WPARAM;lParam:LPARAM):int_ptr;cdecl;
begin
  if wParam=0 then wParam:=WAT_INF_UNICODE;
  if lParam=0 then lParam:=MirandaCP;

  result:=int_ptr(ReturnInfo(wParam,lParam));
end;

function WATGetFileInfo(wParam:WPARAM;lParam:LPARAM):int;cdecl;
var
//  si:TSongInfo;
  dst:PSongInfo;
  extw:array [0..7] of WideChar;
  f:THANDLE;
  p:PWideChar;
begin
  result:=1;
  if (lParam=0) or (pSongInfo(lParam).mfile=nil) then exit;
  dst:=pointer(lParam);
  StrDupW(p,dst^.mfile);
  ClearTrackInfo(dst^,false); //!!!!
  dst^.mfile:=p;
//  FillChar(dst,SizeOf(dst),0);
//  FillChar(si,SizeOf(si),0);
{
  if flags and WAT_INF_ANSI<>0 then
    AnsiToWide(dst^.mfile,si.mfile)
  else if flags and WAT_INF_UTF<>0 then
    UTFToWide(dst^.mfile,si.mfile)
  else
    si.mfile:=dst^.mfile;
}  
  f:=Reset(dst^.mfile);
  if dword(f)<>INVALID_HANDLE_VALUE then
    GetFileTime(f,nil,nil,@dst^.date);
  CloseHandle(f);
  dst^.fsize:=GetFSize(dst^.mfile);
  GetExt(dst^.mfile,extw);
  if GetFileFormatInfo(dst^)<>WAT_RES_NOTFOUND then
  begin
    with dst^ do
    begin
      if (cover=nil) or (cover^=#0) then
        GetCover(cover,mfile);
      if (lyric=nil) or (lyric^=#0) then
        GetLyric(lyric,mfile);
    end;
    result:=0;
//    ReturnInfo(si,dst,wParam and $FF);
  end;
end;

function WATGetMusicInfo(wParam:WPARAM;lParam:LPARAM):int;cdecl;
type
  ppointer = ^pointer;
const
  giused:cardinal=0;
var
  flags:cardinal;
  buf:PWideChar;
  OldPlayerStatus:integer;
  stat:integer;
  newplayer:bool;
begin
  result:=WAT_RES_NOTFOUND;
  if DisablePlugin=dsPermanent then
    exit;

  //----- Return old info if main timer -----
  if giused<>0 then
  begin
    result:=WAT_RES_OK;
    if lParam<>0 then
      ppointer(lParam)^:=ReturnInfo(wParam and $FF);
    exit;
  end;

  giused:=1;

  OldPlayerStatus:=WorkSI.status;

  //----- Checking player -----
  // get player status too
  flags:=0;
  if CheckAll<>BST_UNCHECKED then flags:=flags or WAT_OPT_CHECKALL;
  // no need old data, clear
//  ClearPlayerInfo(WorkSI,false);
  result:=CheckPlayers(WorkSI,flags);
  if result=WAT_RES_NEWPLAYER then
  begin
    newplayer:=true;
    PluginLink^.NotifyEventHooks(hHookWATStatus,WAT_EVENT_NEWPLAYER,tlparam(@WorkSI));
    result:=WAT_RES_OK;
  end
  else // !!!! (need to add) must remember about same player, another instance
    newplayer:=false;

  // Checking player status
  if result=WAT_RES_OK then
  begin
    if not newplayer then //!!cheat
      SongInfo.plwnd:=WorkSI.plwnd;

    // player stopped - no need file info
    if WorkSI.status=WAT_MES_STOPPED then
    begin
      ClearFileInfo    (WorkSI,false);
      ClearChangingInfo(WorkSI,false);
      ClearTrackInfo   (WorkSI,false);

      if Hiword(OldPlayerStatus)<>WAT_MES_STOPPED then
      begin
        PluginLink^.NotifyEventHooks(hHookWATStatus,WAT_EVENT_PLAYERSTATUS,WAT_MES_STOPPED);
      end;

      ClearFileInfo    (SongInfo,true);
      ClearChangingInfo(SongInfo,true);
      ClearTrackInfo   (SongInfo,true);
      if newplayer then
      begin
        ClearPlayerInfo(SongInfo,true);
        CopyPlayerInfo (WorkSI,SongInfo);
      end;
      WorkSI.status:=(WAT_MES_STOPPED shl 16) or (WAT_PLS_NOMUSIC and $FFFF);
      SongInfo.status:=WorkSI.status;
    end
    else
    begin
      //----- Get file (no file, new file, maybe new) -----
      // file info will be replaced (name most important only)
      flags:=0;
      if CheckTime <>BST_UNCHECKED then flags:=flags or WAT_OPT_CHECKTIME;
      if UseImplant<>BST_UNCHECKED then flags:=flags or WAT_OPT_IMPLANTANT;
      if MTHCheck  <>BST_UNCHECKED then flags:=flags or WAT_OPT_MULTITHREAD;
      if KeepOld   <>BST_UNCHECKED then flags:=flags or WAT_OPT_KEEPOLD;

      // requirement - old file name
      result:=CheckFile(WorkSI,flags,TimeoutForThread);

      // here - place for Playerstatus event
      // high word - song status (play, pause,stop, nothing)
      // low  word - player status (normal,no music, nothing)
      case WorkSI.status of
        WAT_MES_PLAYING,
        WAT_MES_PAUSED:  stat:=WAT_PLS_NORMAL;
        WAT_MES_UNKNOWN: // depends of file search
        begin
          if result=WAT_RES_NOTFOUND then
            stat:=WAT_PLS_NOMUSIC
          else
            stat:=WAT_PLS_NORMAL;
        end;
      else // really, this way blocked already
        {WAT_MES_STOPPED:} stat:=WAT_PLS_NOMUSIC;
      end;
      WorkSI.status:=(WorkSI.status shl 16) or (stat and $FFFF);

      if OldPlayerStatus<>WorkSI.status then
      begin
        PluginLink^.NotifyEventHooks(hHookWATStatus,WAT_EVENT_PLAYERSTATUS,WorkSI.status);
      end;

      // no playing file - clear all file info
      if stat=WAT_PLS_NOMUSIC then
      begin
        ClearFileInfo    (WorkSI,false);
        ClearChangingInfo(WorkSI,false);
        ClearTrackInfo   (WorkSI,false);

        ClearFileInfo    (SongInfo,true);
        ClearChangingInfo(SongInfo,true);
        ClearTrackInfo   (SongInfo,true);

        if newplayer then
        begin
          ClearPlayerInfo(SongInfo,true);
          CopyPlayerInfo (WorkSI,SongInfo);
        end;
        SongInfo.status:=WorkSI.status;
      end;
      // now time for changes (window text, volume)
      // just when music presents
      if stat=WAT_PLS_NORMAL then
      begin
        GetChangingInfo(WorkSI,flags);

        // full info requires
        // "no music" case blocked
        if (result=WAT_RES_NEWFILE) or           // new file
           ((result=WAT_RES_OK) and              // if not new but...
           (((wParam and WAT_INF_CHANGES)=0) or  // ... ask for full info
           (StrPosW(WorkSI.mfile,'://')<>nil) or // ... or remote file
           isContainer(WorkSI.mfile))) then      // ... or container like CUE
        begin
          // requirement: old artist/title for remote files
          stat:=GetInfo(WorkSI,flags);

          // covers
          if (WorkSI.cover=nil) or (WorkSI.cover^=#0) then
            GetCover(WorkSI.cover,WorkSI.mfile)
          else
          begin
            mGetMem(buf,MAX_PATH*SizeOf(WideChar));
            GetTempPathW(MAX_PATH,buf);
            if StrCmpW(buf,WorkSI.cover,StrLenW(buf))=0 then
            begin
              GetExt(WorkSI.cover,StrCatEW(buf,'\wat_cover.'));
              DeleteFileW(buf);
              MoveFileW(WorkSI.cover,buf);
              mFreeMem(WorkSI.cover);
              WorkSI.cover:=buf;
            end
            else
              mFreeMem(buf);
          end;
          // lyric
          if (WorkSI.lyric=nil) or (WorkSI.lyric^=#0) then
            GetLyric(WorkSI.lyric,WorkSI.mfile);

// file info will be updated anyway, so - just update it
          if result=WAT_RES_NEWFILE then
          begin
            ClearFileInfo(SongInfo,true);
            CopyFileInfo (WorkSI,SongInfo);
          end;
          ClearTrackInfo(SongInfo,true);
          CopyTrackInfo (WorkSI,SongInfo);

          if newplayer then
          begin
            ClearPlayerInfo(SongInfo,true);
            CopyPlayerInfo (WorkSI,SongInfo);
          end;
          ClearChangingInfo(SongInfo,true);
          CopyChangingInfo (WorkSI,SongInfo);
          SongInfo.status:=WorkSI.status;

          if stat=WAT_RES_NEWFILE then
            result:=WAT_RES_NEWFILE;

          if result=WAT_RES_NEWFILE then
            PluginLink^.NotifyEventHooks(hHookWATStatus,WAT_EVENT_NEWTRACK,tlparam(@SongInfo));
        end
        else // just changing infos
        begin
          if newplayer then
          begin
            ClearPlayerInfo(SongInfo,true);
            CopyPlayerInfo (WorkSI,SongInfo);
          end;
          ClearChangingInfo(SongInfo,true);
          CopyChangingInfo (WorkSI,SongInfo);
          SongInfo.status:=WorkSI.status;
        end;
      end;
    end;

    if lParam<>0 then
      ppointer(lParam)^:=ReturnInfo(wParam and $FF);
  end
  //----- Player not found -----
  else
  begin
    if OldPlayerStatus<>WorkSI.status then
    begin
      ClearSongInfoData(WorkSI,false); // player info must be empty anyway
      ClearSongInfoData(SongInfo,true);
      SongInfo.status:=WAT_PLS_NOTFOUND+WAT_MES_UNKNOWN shl 16;

      PluginLink^.NotifyEventHooks(hHookWATStatus,WAT_EVENT_PLAYERSTATUS,
          WAT_PLS_NOTFOUND+WAT_MES_UNKNOWN shl 16);
    end;

{
    if OldPlayerStatus<>WorkSI.status then
    begin
      PluginLink^.NotifyEventHooks(hHookWATStatus,WAT_EVENT_PLAYERSTATUS,
          WAT_PLS_NOTFOUND+WAT_MES_UNKNOWN shl 16);
    end;

    ClearSongInfoData(WorkSI,false); // player info must be empty anyway
    WorkSI.status:=WAT_PLS_NOTFOUND+WAT_MES_UNKNOWN shl 16;

    ClearSongInfoData(SongInfo,true);
    SongInfo.status:=WAT_PLS_NOTFOUND+WAT_MES_UNKNOWN shl 16;
}

    if lParam<>0 then
      ppointer(lParam)^:=nil;
  end;

  giused:=0;
end;

function PressButton(wParam:WPARAM;lParam:LPARAM):int;cdecl;
var
  flags:integer;
begin
  if DisablePlugin=dsPermanent then
    result:=0
  else
  begin
    flags:=0;
    if UseImplant<>BST_UNCHECKED then flags:=flags or WAT_OPT_IMPLANTANT;
    if mmkeyemu  <>BST_UNCHECKED then flags:=flags or WAT_OPT_APPCOMMAND;
    if CheckAll  <>BST_UNCHECKED then flags:=flags or WAT_OPT_CHECKALL;
    result:=SendCommand(wParam,lParam,flags);
  end;
end;

function WATPluginStatus(wParam:WPARAM;lParam:LPARAM):int;cdecl;
var
  f1:integer;
begin
  if wParam=2 then
  begin
    result:=PluginInfo.version;
    exit;
  end;
  if DisablePlugin=dsPermanent then
    result:=1
  else
    result:=0;
  if (wParam<0) or (wParam=MenuDisablePos) then
  begin
    if result=0 then
      wParam:=1
    else
      wParam:=0;
  end;
  case wParam of
    0: begin
      if DisablePlugin=dsPermanent then //??
      begin
        StartTimer;
        DisablePlugin:=dsEnabled;
      end;
      f1:=0;
    end;
    1: begin
      StopTimer;
      DisablePlugin:=dsPermanent;
      f1:=CMIF_CHECKED;
    end;
  else
    exit;
  end;
  DBWriteByte(0,PluginShort,opt_disable,DisablePlugin);

  ChangeMenuIcons(f1);

  PluginLink^.NotifyEventHooks(hHookWATStatus,WAT_EVENT_PLUGINSTATUS,DisablePlugin);
end;

function WaitAllModules(wParam:WPARAM;lParam:LPARAM):int;cdecl;
var
  ptr:pwModule;
begin
  result:=0;

  CallService(MS_SYSTEM_REMOVEWAIT,wParam,0);

  ptr:=ModuleLink;
  while ptr<>nil do
  begin
    if @ptr^.Init<>nil then
      ptr^.ModuleStat:=ptr^.Init(true);
    ptr:=ptr^.Next;
  end;

  if mTimer<>0 then
    TimerProc(0,0,0,0);

  StartTimer;

  PluginLink^.NotifyEventHooks(hHookWATLoaded,0,0);
  CloseHandle(hEvent);
end;

function OnModulesLoaded(wParam:WPARAM;lParam:LPARAM):int;cdecl;
var
  buf:array [0..511] of AnsiChar;
  upd:TUpdate;
  p:PAnsiChar;
begin
   PluginLink^.UnhookEvent(onloadhook);

  if PluginLink^.ServiceExists(MS_UPDATE_REGISTER)<>0 then
  begin
    with upd do
    begin
      cbSize              :=SizeOf(upd);
      szComponentName     :=PluginInfo.ShortName;
      szVersionURL        :=VersionURL;
      pbVersionPrefix     :=VersionPrefix;
      cpbVersionPrefix    :=length(VersionPrefix);
      szUpdateURL         :=UpdateURL;
      szBetaVersionURL    :=BetaVersionURL;
      pbBetaVersionPrefix :=BetaVersionPrefix;
      cpbBetaVersionPrefix:=length(pbBetaVersionPrefix);
      szBetaUpdateURL     :=BetaUpdateURL;
      pbVersion           :=CreateVersionStringPlugin(@pluginInfo,buf);
      cpbVersion          :=StrLen(pbVersion);
      szBetaChangelogURL  :=BetaChangelogURL;
    end;
    PluginLink^.CallService(MS_UPDATE_REGISTER,0,tlparam(@upd));
  end;

  CallService('DBEditorpp/RegisterSingleModule',twparam(PluginShort),0);

  hTimer:=0;

  OleInitialize(nil);

  if RegisterIcons then
    wsic:=PluginLink^.HookEvent(ME_SKIN2_ICONSCHANGED,@IconChanged)
  else
    wsic:=0;

  CreateMenus;

  if PluginLink^.ServiceExists(MS_TTB_ADDBUTTON)<>0 then
    onloadhook:=pluginlink^.HookEvent(ME_TTB_MODULELOADED,@OnTTBLoaded);

  ProcessFormatLink;
  ProcessPlayerLink;
  p:=GetAddonFileName(nil,'player','plugins','ini');
  if p<>nil then
  begin
    LoadFromFile(p);
    mFreeMem(p);
  end;

  p:=GetAddonFileName(nil,'watrack_icons','icons','dll');
  if p<>nil then
  begin
    SetPlayerIcons(p);
    mFreeMem(p);
  end;

  hEvent:=CreateEvent(nil,true,true,nil);
  if hEvent<>0 then
  begin
    p:='WAT_INIT';
    hWATI:=CreateServiceFunction(p,@WaitAllModules);
    CallService(MS_SYSTEM_WAITONHANDLE,hEvent,tlparam(p));
  end;

  loadopt;
  if DisablePlugin=dsPermanent then
    CallService(MS_WAT_PLUGINSTATUS,1,0);

  result:=0;

end;

procedure FreeVariables;
begin
  ClearSongInfoData(SongInfo ,true);
  ClearSongInfoData(TSongInfo(SongInfoA),true);
  ClearSongInfoData(WorkSI   ,false); // not necessary really
  mFreeMem(CoverPaths);
  ClearFormats;
  ClearPlayers;
end;

procedure FreeServices;
begin
  PluginLink^.DestroyServiceFunction(hGFI);
  PluginLink^.DestroyServiceFunction(hRGS);

  PluginLink^.DestroyServiceFunction(hWI);
  PluginLink^.DestroyServiceFunction(hGMI);
  PluginLink^.DestroyServiceFunction(hPS);
  PluginLink^.DestroyServiceFunction(hPB);
  PluginLink^.DestroyServiceFunction(hWATI);
  PluginLink^.DestroyServiceFunction(hWC);

  PluginLink^.DestroyServiceFunction(hFMT);
  PluginLink^.DestroyServiceFunction(hPLR);
end;

function PreShutdown(wParam:WPARAM;lParam:LPARAM):int;cdecl;
var
  buf:array [0..511] of WideChar;
  fdata:WIN32_FIND_DATAW;
  fi:THANDLE;
  p:PWideChar;
  ptr:pwModule;
begin

  PluginLink^.NotifyEventHooks(hHookWATStatus,WAT_EVENT_PLAYERSTATUS,WAT_PLS_NOTFOUND);

  if hwndTooltip<>0 then
    DestroyWindow(hwndTooltip);

  StopTimer;
  ptr:=ModuleLink;
  while ptr<>nil do
  begin
    if @ptr^.DeInit<>nil then
      ptr^.DeInit(false);
    ptr:=ptr^.Next;
  end;

//  PluginLink^.UnhookEvent(plStatusHook);
  PluginLink^.UnhookEvent(hHookShutdown);
  PluginLink^.UnhookEvent(opthook);
  if wsic<>0 then PluginLink^.UnhookEvent(wsic);

  FreeServices;
  FreeVariables;

  PluginLink^.DestroyHookableEvent(hHookWATLoaded);
  PluginLink^.DestroyHookableEvent(hHookWATStatus);

  OleUnInitialize;

  //delete cover files
  buf[0]:=#0;
  GetTempPathW(511,buf);
  p:=StrEndW(buf);
  StrCopyW(p,'wat_cover.*');

  fi:=FindFirstFileW(buf,fdata);
  if dword(fi)<>INVALID_HANDLE_VALUE then
  begin
    repeat
      StrCopyW(p,fdata.cFileName);
      DeleteFileW(buf);
    until not FindNextFileW(fi,fdata);
    FindClose(fi);
  end;

  result:=0;
end;

function Load(link:PPLUGINLINK):int; cdecl;
begin
  result:=0;
  PluginLink:=Pointer(link);
  InitMMI;
  Langpack_register;

  DisablePlugin:=dsPermanent;

  hHookWATLoaded:=PluginLink^.CreateHookableEvent(ME_WAT_MODULELOADED);
  hHookWATStatus:=PluginLink^.CreateHookableEvent(ME_WAT_NEWSTATUS);
  hHookShutdown :=PluginLink^.HookEvent(ME_SYSTEM_OKTOEXIT,@PreShutdown);
  opthook       :=PluginLink^.HookEvent(ME_OPT_INITIALISE ,@OnOptInitialise);

  hGFI:=PluginLink^.CreateServiceFunction(MS_WAT_GETFILEINFO  ,@WATGetFileInfo);
  hRGS:=PluginLink^.CreateServiceFunction(MS_WAT_RETURNGLOBAL ,@WATReturnGlobal);

  hGMI:=PluginLink^.CreateServiceFunction(MS_WAT_GETMUSICINFO ,@WATGetMusicInfo);
  hPS :=PluginLink^.CreateServiceFunction(MS_WAT_PLUGINSTATUS ,@WATPluginStatus);
  hPB :=PluginLink^.CreateServiceFunction(MS_WAT_PRESSBUTTON  ,@PressButton);
  hWI :=PluginLink^.CreateServiceFunction(MS_WAT_WINAMPINFO   ,@WinampGetInfo);
  hWC :=PluginLink^.CreateServiceFunction(MS_WAT_WINAMPCOMMAND,@WinampCommand);

  hFMT:=PluginLink^.CreateServiceFunction(MS_WAT_FORMAT,@ServiceFormat);
  hPLR:=PluginLink^.CreateServiceFunction(MS_WAT_PLAYER,@ServicePlayer);

  FillChar(SongInfoA,SizeOf(SongInfoA),0);
  FillChar(SongInfo ,SizeOf(SongInfo ),0);
  FillChar(WorkSI   ,SizeOf(SongInfo ),0);
  onloadhook:=PluginLink^.HookEvent(ME_SYSTEM_MODULESLOADED,@OnModulesLoaded);
end;

function Unload:int; cdecl;
begin
  result:=0;
end;

function MirandaPluginInterfaces:PMUUID; cdecl;
begin
  PluginInterfaces[0]:=PluginInfo.uuid;
  PluginInterfaces[1]:=MIID_LAST;
  result:=@PluginInterfaces;
end;

exports
  Load, Unload,
  MirandaPluginInterfaces,MirandaPluginInfoEx;

begin
end.
