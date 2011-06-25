{.$DEFINE CHANGE_NAME_BUFFERED}
{$IMAGEBASE $13300000}
library MRadio;

uses
//  FastMM4,
  {$IFDEF KOL_MCK}kol,icobuttons,KOLCCtrls,{$ENDIF}Windows,messages,commctrl
  ,common,io,wrapper,wrapdlgs,syswin
  ,Dynamic_Bass,dynbasswma
  ,m_api,dbsettings,mirutils,playlist;

{$include mr_rc.inc}
{$r mradio.res}

{$include i_vars.inc}

const
  PluginName:PAnsiChar = 'mRadio';

var
  PluginInterfaces:array [0..1] of MUUID;

const
  VersionURL        = nil;//'http://addons.miranda-im.org/details.php?action=viewfile&id=3285';
  VersionPrefix     = nil;//'<span class="fileNameHeader">QuickSearch Mod ';
  UpdateURL         = nil;//'http://addons.miranda-im.org/feed.php?dlfile=3285';
  BetaVersionURL    = 'http://awkward.miranda.im/index.htm';
  BetaVersionPrefix = '>My mRadio mod ';
  BetaUpdateURL     = 'http://awkward.miranda.im/mradio.zip';
  BetaChangelogURL  = nil;

function MakeMessage:pWideChar;
var
  p,artist,title:pWideChar;
  len:cardinal;
begin
  artist:=DBReadUnicode(0,PluginName,optArtist);
  title :=DBReadUnicode(0,PluginName,optTitle);
  len:=StrLenW(artist);
  if (artist<>nil) and (title<>nil) then
    inc(len,3);
  inc(len,StrLenW(title));

  if len>0 then
  begin
    mGetMem(result,(len+1)*SizeOf(WideChar));
    p:=result;
    if artist<>nil then
    begin
      p:=StrCopyEW(p,artist);
      if title<>nil then
        p:=StrCopyEW(p,' - ');
      mFreeMem(artist);
    end;
    if title<>nil then
    begin
      StrCopyW(p,title);
      mFreeMem(title);
    end;
  end
  else
    result:=nil;
end;

procedure SetStatus(hContact:THANDLE;Status:integer);
begin
//  if Status=ID_STATUS_OFFLINE then
//    MyStopBass;

  if Status=ID_STATUS_OFFLINE then
  begin
    if (AsOffline=BST_UNCHECKED) or (PluginStatus<>ID_STATUS_OFFLINE) then
      Status:=ID_STATUS_INVISIBLE;
  end;

  if hContact=0 then
  begin
    hContact:=CallService(MS_DB_CONTACT_FINDFIRST,0,0);
    while hContact<>0 do
    begin
      if StrCmp(PAnsiChar(CallService(MS_PROTO_GETCONTACTBASEPROTO,hContact,0)),PluginName)=0 then
      begin
        DBWriteWord(hContact,PluginName,optStatus,Status);
      end;
      hContact:=CallService(MS_DB_CONTACT_FINDNEXT,hContact,0);
    end;
  end
  else
    DBWriteWord(hContact,PluginName,optStatus,Status);
end;

{$include i_search.inc}
{$include i_bass.inc}
{$include i_cc.inc}
{$include i_variables.inc}
{$include i_service.inc}
{$include i_myservice.inc}
{$include i_hotkey.inc}
{$IFDEF KOL_MCK}
  {$include i_frame.inc}
{$ELSE}
  {$include i_frameapi.inc}
{$ENDIF}
{$include i_tray.inc}
{$include i_visual.inc}
{$include i_optdlg.inc}

function MirandaPluginInfoEx(mirandaVersion:DWORD):PPLUGININFOEX; cdecl;
begin
  result:=@PluginInfo;
  PluginInfo.cbSize     :=SizeOf(TPLUGININFOEX);
  PluginInfo.shortName  :='mRadio Mod';
  PluginInfo.version    :=$00000201;
  PluginInfo.description:='This plugin plays and records Internet radio streams.'+
                          ' Also local media files can be played.';
  PluginInfo.author     :='Awkward';
  PluginInfo.authorEmail:='panda75@bk.ru; awk1975@ya.ru';
  PluginInfo.copyright  :='(c) 2007-2011 Awkward';
  PluginInfo.homepage   :='http://code.google.com/p/delphi-miranda-plugins/';
  PluginInfo.flags      :=UNICODE_AWARE;
  PluginInfo.replacesDefaultModule:=0;
  PluginInfo.uuid  :=MIID_MRADIO;
end;

function OnModulesLoaded(wParam:WPARAM;lParam:LPARAM):int;cdecl;
var
  nlu:TNETLIBUSER;
  szTemp:array [0..255] of AnsiChar;
  i:integer;
  upd:TUpdate;
begin
  PluginLink^.UnhookEvent(onloadhook);

  DBWriteDWord(0,PluginName,optVersion,PluginInfo.version);

  if PluginLink^.ServiceExists(MS_UPDATE_REGISTER)<>0 then
  begin
    with upd do
    begin
      cbSize              :=SizeOf(upd);
      szComponentName     :=PluginInfo.ShortName;
      szVersionURL        :=VersionURL;
      pbVersionPrefix     :=VersionPrefix;
      cpbVersionPrefix    :=0;//length(VersionPrefix);
      szUpdateURL         :=UpdateURL;
      szBetaVersionURL    :=BetaVersionURL;
      pbBetaVersionPrefix :=BetaVersionPrefix;
      cpbBetaVersionPrefix:=length(pbBetaVersionPrefix);
      szBetaUpdateURL     :=BetaUpdateURL;
      pbVersion           :=CreateVersionStringPlugin(@pluginInfo,szTemp);
      cpbVersion          :=StrLen(pbVersion);
      szBetaChangelogURL  :=BetaChangelogURL;
    end;
    PluginLink^.CallService(MS_UPDATE_REGISTER,0,tlparam(@upd));
  end;

  szTemp[0]:='E';
  szTemp[1]:='Q';
  szTemp[2]:='_';
  szTemp[4]:=#0;
  for i:=0 to 9 do
  begin
    szTemp[3]:=AnsiChar(ORD('0')+i);
    eq[i].param.fGain:=DBReadByte(0,PluginName,szTemp,15)-15;
  end;
  LoadPresets;

  RegisterIcons;
  CreateMenu;
  CreateMIMTrayMenu;

  FillChar(nlu,SizeOf(nlu),0);
  StrCopy(szTemp,Translate('%s server connection'));
  StrReplace(szTemp,'%s',PluginName);
  nlu.szDescriptiveName.a:=szTemp;
  nlu.cbSize             :=SizeOf(nlu);
  nlu.flags              :=NUF_HTTPCONNS or NUF_NOHTTPSOPTION or NUF_OUTGOING;
  nlu.szSettingsModule   :=PluginName;
  hNetlib:=CallService(MS_NETLIB_REGISTERUSER,0,tlparam(@nlu));

//  CallService(MS_RADIO_COMMAND,MRC_RECORD,2); record off - not so necessary

  recpath:=DBReadUnicode(0,PluginName,optRecPath);

  sPreBuf:=DBReadWord(0,PluginName,optPreBuf,75);
  BASS_SetConfig(BASS_CONFIG_NET_PREBUF,sPreBuf);

  sBuffer:=DBReadWord(0,PluginName,optBuffer,5000);
  BASS_SetConfig(BASS_CONFIG_NET_BUFFER,sBuffer);

  sTimeout:=DBReadWord(0,PluginName,optTimeout,5000);
  BASS_SetConfig(BASS_CONFIG_NET_TIMEOUT,sTimeout);

  doLoop    :=DBReadByte(0,PluginName,optLoop);
  doShuffle :=DBReadByte(0,PluginName,optShuffle);
  doContRec :=DBReadByte(0,PluginName,optContRec);
  PlayFirst :=DBReadByte(0,PluginName,optPlay1st);
  isEQ_OFF  :=DBReadByte(0,PluginName,optEQ_OFF);
  AuConnect :=DBReadByte(0,PluginName,optConnect);
  AsOffline :=DBReadByte(0,PluginName,optOffline);
  gVolume   :=DBReadByte(0,PluginName,optVolume,50);
  NumTries  :=DBReadByte(0,PluginName,optNumTries,1);
  ForcedMono:=DBReadByte(0,PluginName,optForcedMono);
  if NumTries<1 then NumTries:=1;

  SetStatus(0,ID_STATUS_OFFLINE);
  
  StatusTmpl:=DBReadUnicode(0,PluginName,optStatusTmpl,'%radio_title%');

  if Auconnect<>BST_UNCHECKED then
    ActiveContact:=LoadContact(PluginName,optLastStn)
  else
    ActiveContact:=0;

  CallService(MS_RADIO_COMMAND,MRC_STATUS,RD_STATUS_NOSTATION);

  RegisterVariables;

  onsetting:=Pluginlink^.HookEvent(ME_DB_CONTACT_SETTINGCHANGED,@OnSettingsChanged);
  ondelete :=PluginLink^.HookEvent(ME_DB_CONTACT_DELETED       ,@OnContactDeleted);
  randomize;
  CreateFrame(0);

  RegisterHotKey;

  result:=0;
end;

function PreShutdown(wParam:WPARAM;lParam:LPARAM):int;cdecl;
begin
  CallService(MS_RADIO_COMMAND,MRC_STOP,1);
  UnRegisterHotKey;

  DestroyProtoServices;
  DestroyHiddenWindow;
  DestroyFrame();
  MyFreeBASS;
  DBWriteByte(0,PluginName,optVolume,gVolume);

  with PluginLink^ do
  begin
    DestroyServiceFunction(hsPlayStop);
    DestroyServiceFunction(hsRecord);
    DestroyServiceFunction(hsSettings);
    DestroyServiceFunction(hsSetVol);
    DestroyServiceFunction(hsGetVol);
    DestroyServiceFunction(hsMute);
    DestroyServiceFunction(hsCommand);
    DestroyServiceFunction(hsEqOnOff);

    DestroyServiceFunction(hsExport);
    DestroyServiceFunction(hsImport);

    DestroyHookableEvent(hhRadioStatus);

    UnhookEvent(onsetting);
    UnhookEvent(ondelete);
    UnhookEvent(hHookShutdown);
    UnhookEvent(hDblClick);
    UnhookEvent(opthook);
    UnhookEvent(contexthook);
  end;

  CallService(MS_NETLIB_CLOSEHANDLE,hNetLib,0);
  mFreeMem(storage);
  mFreeMem(storagep);
  mFreeMem(recpath);
  mFreeMem(StatusTmpl);
  mFreeMem(basspath);
  FreePresets;

  result:=0;
end;

function Load(link:PPLUGINLINK): int; cdecl;
var
  desc:TPROTOCOLDESCRIPTOR;
  szTemp:array [0..MAX_PATH-1] of WideChar;
  pc:pWideChar;
  custom:pWideChar;
begin
  PluginLink:=Pointer(link);
  InitMMI;
  Langpack_register;

  GetModuleFileNameW(0,szTemp,MAX_PATH-1);
  pc:=StrEndW(szTemp);
  repeat
    dec(pc);
  until pc^='\';
  inc(pc);
  pc^:=#0;

  custom:=DBReadUnicode(0,PluginName,optBASSpath,nil);

  if MyLoadBASS(szTemp,custom) then
  begin
    StrCopyW(pc,'plugins\mradio.ini');
//    StrDup(storage,szTemp);
    FastWideToAnsi(szTemp,storage);
    mGetMem(storagep,MAX_PATH+32);
    CallService(MS_DB_GETPROFILEPATH,MAX_PATH-1,lparam(storagep));
    StrCat(storagep,'\mradio.ini');

    FillChar(desc,SizeOf(desc),0);
    desc.cbSize:=PROTOCOLDESCRIPTOR_V3_SIZE;//SizeOf(desc);
    desc.szName:=PluginName;
    desc._type :=PROTOTYPE_PROTOCOL;
    CallService(MS_PROTO_REGISTERMODULE,0,lparam(@desc));

    with PluginLink^ do
    begin
      hhRadioStatus:=PluginLink^.CreateHookableEvent(ME_RADIO_STATUS);

      hsPlayStop:=CreateServiceFunction(MS_RADIO_PLAYSTOP,@Service_RadioPlayStop);
      hsRecord  :=CreateServiceFunction(MS_RADIO_RECORD  ,@Service_RadioRecord);
      hsSettings:=CreateServiceFunction(MS_RADIO_SETTINGS,@Service_RadioSettings);
      hsSetVol  :=CreateServiceFunction(MS_RADIO_SETVOL  ,@Service_RadioSetVolume);
      hsGetVol  :=CreateServiceFunction(MS_RADIO_GETVOL  ,@Service_RadioGetVolume);
      hsMute    :=CreateServiceFunction(MS_RADIO_MUTE    ,@Service_RadioMute);
      hsCommand :=CreateServiceFunction(MS_RADIO_COMMAND ,@ControlCenter);
      hsEqOnOff :=CreateServiceFunction(MS_RADIO_EQONOFF ,@Service_EqOnOff);

      hsExport  :=CreateServiceFunction(MS_RADIO_EXPORT ,@ExportAll);
      hsImport  :=CreateServiceFunction(MS_RADIO_IMPORT ,@ImportAll);

      CreateProtoServices;
      onloadhook   :=HookEvent(ME_SYSTEM_MODULESLOADED     ,@OnModulesLoaded);
      hHookShutdown:=HookEvent(ME_SYSTEM_SHUTDOWN{ME_SYSTEM_OKTOEXIT},@PreShutdown);
      hDblClick    :=HookEvent(ME_CLIST_DOUBLECLICKED      ,@Service_RadioPlayStop{@DblClickProc});
      opthook      :=HookEvent(ME_OPT_INITIALISE           ,@OnOptInitialise);
      contexthook  :=HookEvent(ME_CLIST_PREBUILDCONTACTMENU,@OnContactMenu);
    end;

    PluginStatus:=ID_STATUS_OFFLINE;
  end;
  mFreeMem(custom);

  Result:=0;
end;

function Unload: int; cdecl;
begin
  Unload_BASSDLL;
  Result:=0;
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
