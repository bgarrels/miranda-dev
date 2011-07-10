{$Include compilers.inc}
unit mirutils;

interface

uses windows,m_api;

// wrappers
function SetButtonIcon(btn:HWND;name:PAnsiChar):HICON;

// others

function ConvertFileName(src:pAnsiChar;hContact:THANDLE=0):pAnsiChar; overload;
function ConvertFileName(src:pWideChar;hContact:THANDLE=0):pWideChar; overload;
function ConvertFileName(src:pAnsiChar;dst:pAnsiChar;hContact:THANDLE=0):pAnsiChar; overload;
function ConvertFileName(src:pWideChar;dst:pWideChar;hContact:THANDLE=0):pWideChar; overload;

function RegisterSingleIcon(resname,ilname,descr,group:PAnsiChar):int;
procedure ShowPopupW(text:pWideChar;title:pWideChar=nil);
function GetAddonFileName(prefix,altname,path:PAnsiChar;ext:PAnsiChar):PAnsiChar;
function TranslateA2W(sz:PAnsiChar):PWideChar;
function MirandaCP:integer;

function isVarsInstalled:bool;
function ParseVarString(astr:pAnsiChar;aContact:THANDLE=0;extra:pAnsiChar=nil):pAnsiChar; overload;
function ParseVarString(astr:pWideChar;aContact:THANDLE=0;extra:pWideChar=nil):pWideChar; overload;
function ShowVarHelp(dlg:HWND;id:integer=0):integer;

function  IsChat(hContact:THANDLE):bool;
procedure SendToChat(hContact:THANDLE;pszText:PWideChar);

function LoadContact(group,setting:PAnsiChar):THANDLE;
function SaveContact(hContact:THANDLE;group,setting:PAnsiChar):integer;

function SetCListSelContact(hContact:THANDLE):THANDLE;
function GetCListSelContact:THANDLE; {$IFDEF DELPHI10_UP}inline;{$ENDIF}
function GetContactProtoAcc(hContact:THANDLE):PAnsiChar;
function  IsMirandaUser(hContact:THANDLE):integer; // >0=Miranda; 0=Not miranda; -1=unknown
procedure ShowContactDialog(hContact:THANDLE;DblClk:boolean=true;anystatus:boolean=true);
function  FindContactHandle(proto:pAnsiChar;const dbv:TDBVARIANT;is_chat:boolean=false):THANDLE;
function  WndToContact(wnd:hwnd):integer; overload;
function  WndToContact:integer; overload;
function  GetContactStatus(hContact:THANDLE):integer;
// -2 - deleted account, -1 - disabled account, 0 - hidden
// 1 - metacontact, 2 - submetacontact, positive - active
// proto - ASSIGNED buffer
function  IsContactActive(hContact:THANDLE;proto:pAnsiChar=nil):integer;

function CreateGroupW(name:pWideChar;hContact:THANDLE):integer;
function CreateGroup (name:pAnsiChar;hContact:THANDLE):integer;

function MakeGroupMenu(idxfrom:integer=100):HMENU;
function GetNewGroupName(parent:HWND):pWideChar;

const
  HKMT_CORE       = 1;
  HKMT_HOTKEYPLUS = 2;
  HKMT_HK         = 3;
  HKMT_HKSERVICE  = 4;

function DetectHKManager:dword;

const
  MAX_REDIRECT_RECURSE = 4;

function SendRequest(url:PAnsiChar;rtype:int;args:pAnsiChar=nil;hNetLib:THANDLE=0):pAnsiChar;

function GetFile(url:PAnsiChar;save_file:PAnsiChar;
                 hNetLib:THANDLE=0;recurse_count:integer=0):bool; overload;
// next is just wrapper
function GetFile(url:PWideChar;save_file:PWideChar;
                 hNetLib:THANDLE=0;recurse_count:integer=0):bool; overload;

function GetProxy(hNetLib:THANDLE):PAnsiChar;
function LoadImageURL(url:pAnsiChar;size:integer=0):HBITMAP;

implementation

uses Messages,dbsettings,common,io,freeimage,syswin{$IFDEF KOL_MCK},kol{$ENDIF};

// Save / Load contact 
const
  opt_cproto   = 'cproto';
  opt_cuid     = 'cuid';
  opt_ischat   = 'ischat';

function SetButtonIcon(btn:HWND;name:PAnsiChar):HICON;
begin
  result:=PluginLink^.CallService(MS_SKIN2_GETICON,0,LPARAM(name));
  SendMessage(btn,BM_SETIMAGE,IMAGE_ICON,result);
end;

function ConvertFileName(src:pWideChar;dst:pWideChar;hContact:THANDLE=0):pWideChar; overload;
var
  pc:pWideChar;
begin
  result:=dst;
  dst^:=#0;
  if (src<>nil) and (src^<>#0) then
  begin
    if isVarsInstalled then
    begin
      pc:=ParseVarString(src,hContact);
      src:=pc;
    end
    else
      pc:=nil;
    PluginLink^.CallService(MS_UTILS_PATHTOABSOLUTEW,wparam(src),lparam(dst));
    mFreeMem(pc);
  end;
end;

function ConvertFileName(src:pWideChar;hContact:THANDLE=0):pWideChar; overload;
var
  buf1:array [0..511] of WideChar;
begin
  if (src<>nil) and (src^<>#0) then
    StrDupW(result,ConvertFileName(src,buf1,hContact))
  else
    result:=nil;
end;

function ConvertFileName(src:pAnsiChar;dst:pAnsiChar;hContact:THANDLE=0):pAnsiChar; overload;
var
  pc:pAnsiChar;
begin
  result:=dst;
  dst^:=#0;
  if (src<>nil) and (src^<>#0) then
  begin
    if isVarsInstalled then
    begin
      pc:=ParseVarString(src,hContact);
      src:=pc;
    end
    else
      pc:=nil;
    PluginLink^.CallService(MS_UTILS_PATHTOABSOLUTE,wparam(src),lparam(dst));
    mFreeMem(pc);
  end;
end;

function ConvertFileName(src:pAnsiChar;hContact:THANDLE=0):pAnsiChar; overload;
var
  buf1:array [0..511] of AnsiChar;
begin
  if (src<>nil) and (src^<>#0) then
    StrDup(result,ConvertFileName(src,buf1,hContact))
  else
    result:=nil;
end;

const
  IsVars:integer=-1;
  MirCP:integer=-1;
const
  HKManager:integer=-1;

function MirandaCP:integer;
begin
  if MirCP<0 then
    MirCP:=CallService(MS_LANGPACK_GETCODEPAGE,0,0);
  result:=MirCP;
end;

function IsChat(hContact:THANDLE):bool;
begin
  result:=DBReadByte(hContact,
    PAnsiChar(CallService(MS_PROTO_GETCONTACTBASEPROTO,hContact,0)),
    'ChatRoom',0)=1;
end;

function isVarsInstalled:bool;
begin
  if IsVars<0 then
    IsVars:=PluginLink^.ServiceExists(MS_VARS_FORMATSTRING);
  result:=IsVars<>0;
end;

function ParseVarString(astr:pAnsiChar;aContact:THANDLE=0;extra:pAnsiChar=nil):pAnsiChar;
var
  tfi:TFORMATINFO;
  tmp,pc:pAnsiChar;
  dat:TREPLACEVARSDATA;
begin
  if PluginLink^.ServiceExists(MS_UTILS_REPLACEVARS)<>0 then
  begin
    FillChar(dat,SizeOf(TREPLACEVARSDATA),0);
    dat.cbSize :=SizeOf(TREPLACEVARSDATA);
    pc:=pAnsiChar(PluginLink^.CallService(MS_UTILS_REPLACEVARS,wparam(astr),lparam(@dat)));
    astr:=pc;
  end
  else
    pc:=nil;

  if isVarsInstalled then
  begin
    FillChar(tfi,SizeOf(tfi),0);
    with tfi do
    begin
      cbSize       :=SizeOf(TFORMATINFO);
      szFormat.a   :=astr;
      szExtraText.a:=extra;
      hContact     :=aContact;
    end;
    tmp:=pointer(CallService(MS_VARS_FORMATSTRING,wparam(@tfi),0));
    StrDup(result,tmp);
    PluginLink^.CallService(MS_VARS_FREEMEMORY,wparam(tmp),0);
  end
  else
  begin
    StrDup(result,astr);
  end;
  mmi.free(pc);
end;

function ParseVarString(astr:pWideChar;aContact:THANDLE=0;extra:pWideChar=nil):pWideChar;
var
  tfi:TFORMATINFO;
  tmp,pc:pWideChar;
  dat:TREPLACEVARSDATA;
begin
  if PluginLink^.ServiceExists(MS_UTILS_REPLACEVARS)<>0 then
  begin
    FillChar(dat,SizeOf(TREPLACEVARSDATA),0);
    dat.cbSize :=SizeOf(TREPLACEVARSDATA);
    dat.dwflags:=RVF_UNICODE;
    pc:=pWideChar(PluginLink^.CallService(MS_UTILS_REPLACEVARS,wparam(astr),lparam(@dat)));
    astr:=pc;
  end
  else
    pc:=nil;

  if isVarsInstalled then
  begin
    FillChar(tfi,SizeOf(tfi),0);
    with tfi do
    begin
      cbSize       :=SizeOf(TFORMATINFO);
      flags        :=FIF_UNICODE;
      szFormat.w   :=astr;
      szExtraText.w:=extra;
      hContact     :=aContact;
    end;
    tmp:=pointer(CallService(MS_VARS_FORMATSTRING,wparam(@tfi),0));
    StrDupW(result,tmp);
    PluginLink^.CallService(MS_VARS_FREEMEMORY,wparam(tmp),0);
  end
  else
  begin
    StrDupW(result,astr);
  end;
  mmi.free(pc); // forced!
//  mFreeMem(pc);
end;

function ShowVarHelp(dlg:HWND;id:integer=0):integer;
var
  vhi:TVARHELPINFO;
begin
  FillChar(vhi,SizeOf(vhi),0);
  with vhi do
  begin
    cbSize:=SizeOf(vhi);
    if id=0 then
      flags:=VHF_NOINPUTDLG
    else
    begin
      flags   :=VHF_FULLDLG or VHF_SETLASTSUBJECT;
      hwndCtrl:=GetDlgItem(dlg,id);
    end;
  end;
  result:=PluginLink^.CallService(MS_VARS_SHOWHELPEX,dlg,lparam(@vhi));
end;

function DetectHKManager:dword;
begin
  if HKManager<0 then
  begin
    with PluginLink^ do
      if      ServiceExists('CoreHotkeys/Register'       )<>0 then HKManager:=HKMT_CORE
      else if ServiceExists('HotkeysPlus/Add'            )<>0 then HKManager:=HKMT_HOTKEYPLUS
      else if ServiceExists('HotKey/CatchHotkey'         )<>0 then HKManager:=HKMT_HK
      else if ServiceExists('HotkeysService/RegisterItem')<>0 then HKManager:=HKMT_HKSERVICE
      else HKManager:=0;
  end;
  result:=HKManager;
//  else if (CallService(MS_SYSTEM_GETVERSION,0,0) and $FFFF0000)>=$00080000 then // core
end;

procedure ShowPopupW(text:pWideChar;title:pWideChar=nil);
var
  ppdu:TPOPUPDATAW;
begin
  FillChar(ppdu,SizeOf(TPOPUPDATAW),0);
  if CallService(MS_POPUP_ISSECONDLINESHOWN,0,0)<>0 then
  begin
    StrCopyW(ppdu.lpwzText,text,MAX_SECONDLINE-1);
    if title<>nil then
      StrCopyW(ppdu.lpwzContactName,title,MAX_CONTACTNAME-1)
    else
      ppdu.lpwzContactName[0]:=' ';
  end
  else
  begin
    StrCopyW(ppdu.lpwzContactName,text,MAX_CONTACTNAME-1);
    ppdu.lpwzText[0]:=' ';
  end;
  PluginLink^.CallService(MS_POPUP_ADDPOPUPW,wparam(@ppdu),APF_NO_HISTORY);
end;

function TranslateA2W(sz:PAnsiChar):PWideChar;
var
  tmp:pWideChar;
begin
  mGetMem(tmp,(StrLen(sz)+1)*SizeOf(WideChar));
  Result:=PWideChar(PluginLink^.CallService(MS_LANGPACK_TRANSLATESTRING,LANG_UNICODE,
          lParam(FastAnsiToWideBuf(sz,tmp))));
  if Result<>tmp then
  begin
    StrDupW(Result,Result);
    mFreeMem(tmp);
  end;
end;

function GetContactProtoAcc(hContact:THANDLE):PAnsiChar;
begin
  if PluginLink^.ServiceExists(MS_PROTO_GETCONTACTBASEACCOUNT)<>0 then
    result:=PAnsiChar(CallService(MS_PROTO_GETCONTACTBASEACCOUNT,hContact,0))
  else
    result:=PAnsiChar(CallService(MS_PROTO_GETCONTACTBASEPROTO,hContact,0));
end;

function IsMirandaUser(hContact:THANDLE):integer; // >0=Miranda; 0=Not miranda; -1=unknown
var
  sz:PAnsiChar;
begin
  sz:=PAnsiChar(CallService(MS_PROTO_GETCONTACTBASEPROTO,hContact,0));
  sz:=DBReadString(hContact,sz,'MirVer');
  if sz<>nil then
  begin
    result:=int_ptr(StrPos(sz,'Miranda'));
    mFreeMem(sz);
  end
  else
    result:=-1;
end;

function SetCListSelContact(hContact:THANDLE):THANDLE;
var
  wnd:HWND;
begin
  wnd:=CallService(MS_CLUI_GETHWNDTREE,0,0);
  result:=hContact;
//  hContact:=SendMessage(wnd,CLM_FINDCONTACT  ,hContact,0);
  SendMessage(wnd,CLM_SELECTITEM   ,hContact,0);
//  SendMessage(wnd,CLM_ENSUREVISIBLE,hContact,0);
end;

function GetCListSelContact:THANDLE;
begin
  result:=SendMessageW(CallService(MS_CLUI_GETHWNDTREE,0,0),CLM_GETSELECTION,0,0);
end;

function LoadContact(group,setting:PAnsiChar):THANDLE;
var
  p,proto:pAnsiChar;
  section:array [0..63] of AnsiChar;
  dbv:TDBVARIANT;
  is_chat:boolean;
begin
  p:=StrCopyE(section,setting);
  StrCopy(p,opt_cproto); proto  :=DBReadString(0,group,section);
  StrCopy(p,opt_ischat); is_chat:=DBReadByte  (0,group,section,0)<>0;
  StrCopy(p,opt_cuid  );
  if is_chat then
    dbv.szVal.W:=DBReadUnicode(0,group,section,@dbv)
  else
    DBReadSetting(0,group,section,@dbv);

  result:=FindContactHandle(proto,dbv,is_chat);

  mFreeMem(proto);
  if not is_chat then
    DBFreeVariant(@dbv)
  else
    mFreeMem(dbv.szVal.W);
end;

function SaveContact(hContact:THANDLE;group,setting:PAnsiChar):integer;
var
  p,proto,uid:pAnsiChar;
  cws:TDBVARIANT;
  section:array [0..63] of AnsiChar;
  pw:pWideChar;
  is_chat:boolean;
begin
  result:=0;
  proto:=GetContactProtoAcc(hContact);
  if proto<>nil then
  begin
    p:=StrCopyE(section,setting);
    is_chat:=IsChat(hContact);
    if is_chat then
    begin
      pw:=DBReadUnicode(hContact,proto,'ChatRoomID');
      StrCopy(p,opt_cuid); DBWriteUnicode(0,group,section,pw);
      mFreeMem(pw);
      result:=1;
    end
    else
    begin
      uid:=pAnsiChar(CallProtoService(proto,PS_GETCAPS,PFLAG_UNIQUEIDSETTING,0));
      if DBReadSetting(hContact,proto,uid,@cws)=0 then
      begin
        StrCopy(p,opt_cuid); DBWriteSetting(0,group,section,@cws);
        DBFreeVariant(@cws);
        result:=1;
      end;
    end;
    if result<>0 then
    begin
      StrCopy(p,opt_cproto); DBWriteString(0,group,section,proto);
      StrCopy(p,opt_ischat); DBWriteByte  (0,group,section,ord(is_chat));
    end;
  end;
end;

function WndToContact(wnd:hwnd):integer; overload;
var
  hContact:integer;
  mwid:TMessageWindowInputData;
  mwod:TMessageWindowOutputData;
begin
  wnd:=GetParent(wnd); //!!
  hContact:=PluginLink^.CallService(MS_DB_CONTACT_FINDFIRST,0,0);
  with mwid do
  begin
    cbSize:=SizeOf(mwid);
    uFlags:=MSG_WINDOW_UFLAG_MSG_BOTH;
  end;
  mwod.cbSize:=SizeOf(mwod);
  while hContact<>0 do
  begin
    mwid.hContact:=hContact;
    if PluginLink^.CallService(MS_MSG_GETWINDOWDATA,wparam(@mwid),lparam(@mwod))=0 then
    begin
      if {((mwod.uState and MSG_WINDOW_STATE_FOCUS)<>0) and} (mwod.hwndWindow=wnd) then
      begin
        result:=mwid.hContact;
        exit;
      end
    end;
    hContact:=PluginLink^.CallService(MS_DB_CONTACT_FINDNEXT,hContact,0);
  end;
  result:=0;
end;

function WndToContact:integer; overload;
var
  wnd:HWND;
begin
  wnd:=GetFocus;
  if wnd=0 then
    wnd:=WaitFocusedWndChild(GetForegroundWindow);
  if wnd<>0 then
    result:=WndToContact(wnd)
  else
    result:=0;
  if result=0 then
    result:=GetCListSelContact;
end;

function GetContactStatus(hContact:THANDLE):integer;
var
  szProto:PAnsiChar;
begin
  szProto:=PAnsiChar(PluginLink^.CallService(MS_PROTO_GETCONTACTBASEPROTO,hContact,0));
  if szProto=NIL then
    result:=ID_STATUS_OFFLINE
  else
    result:=DBReadWord(hContact,szProto,'Status',ID_STATUS_OFFLINE);
end;

function CheckPath(filename,profilepath,path:PAnsiChar):PAnsiChar;
var
  buf:array [0..511] of AnsiChar;
  f:THANDLE;
  p:PAnsiChar;
begin
  result:=nil;
  if profilepath<>nil then
    StrCopy(buf,profilepath)
  else
    buf[0]:=#0;
  StrCat(buf,filename);
  f:=Reset(buf);
  if f=THANDLE(INVALID_HANDLE_VALUE) then
  begin
    if path<>nil then
    begin
      CallService(MS_UTILS_PATHTOABSOLUTE,wparam(path),lparam(@buf));
      p:=StrEnd(buf);
      if p^<>'\' then
      begin
        p^:='\';
        inc(p);
        p^:=#0;
      end;
    end
    else if profilepath=nil then
      exit
    else
      buf[0]:=#0;
    StrCat(buf,filename); //path\prefix+name
    f:=Reset(buf);
  end;
  if f<>THANDLE(INVALID_HANDLE_VALUE) then
  begin
    CloseHandle(f);
    StrDup(result,buf);
  end;
end;

function GetAddonFileName(prefix,altname,path:PAnsiChar;ext:PAnsiChar):PAnsiChar;
var
  profilepath:array [0..511] of AnsiChar;
  altfilename,filename:array [0..127] of AnsiChar;
  p:PAnsiChar;
begin
  CallService(MS_DB_GETPROFILEPATH,300,lparam(@profilepath));
  p:=StrEnd(profilepath);
  p^:='\'; inc(p);
  p^:=#0;
  if prefix<>nil then
  begin
    StrCopy(filename,prefix);
    p:=StrEnd(filename);
    CallService(MS_DB_GETPROFILENAME,SizeOf(filename)-integer(p-pAnsiChar(@filename)),lparam(p));
    ChangeExt(filename,ext);
    result:=CheckPath(filename,profilepath,path);
  end
  else
    result:=nil;

  if (result=nil) and (altname<>nil) then
  begin
    StrCopy(altfilename,altname);
    ChangeExt(altfilename,ext);
    result:=CheckPath(altfilename,profilepath,path);
  end;
  if result=nil then
  begin
    StrCat(profilepath,filename);
    StrDup(result,profilepath);
  end;
end;

procedure ShowContactDialog(hContact:THANDLE;DblClk:boolean=true;anystatus:boolean=true);
var
  pc:array [0..127] of AnsiChar;
begin
{
CallService(MS_CLIST_CONTACTDOUBLECLICKED,hContact,0);
}
  if (hContact<>0) and (CallService(MS_DB_CONTACT_IS,hContact,0)<>0) then
  begin
    StrCopy(pc,PAnsiChar(CallService(MS_PROTO_GETCONTACTBASEPROTO,hContact,0)));
    if DblClk or (DBReadByte(hContact,pc,'ChatRoom',0)=1) then // chat room
    begin
      if not anystatus then
      begin
        StrCat(pc,PS_GETSTATUS);
        anystatus:=(CallService(pc,0,0)<>ID_STATUS_OFFLINE);
      end;
      if anystatus then
      begin
        CallService(MS_CLIST_CONTACTDOUBLECLICKED,hContact,0); //??
      // if chat exist, open chat
      // else create new session
      end;
    end
    else
    begin
      if PluginLink^.ServiceExists(MS_MSG_CONVERS)<>0 then // Convers compat.
        CallService(MS_MSG_CONVERS,hContact,0)
      else
        CallService(MS_MSG_SENDMESSAGE,hContact,0)
    end;
  end;
end;

procedure SendChatText(pszID:pointer;pszModule:PAnsiChar;pszText:pointer);
var
  gcd:TGCDEST;
  gce:TGCEVENT;
begin
  gcd.pszModule:=pszModule;
  gcd.iType    :=GC_EVENT_SENDMESSAGE;
  gcd.szID.w   :=pszID;

  FillChar(gce,SizeOf(TGCEVENT),0);
  gce.cbSize  :=SizeOf(TGCEVENT);
  gce.pDest   :=@gcd;
  gce.bIsMe   :=true;
  gce.szText.w:=pszText;
  gce.dwFlags :=GCEF_ADDTOLOG+GC_UNICODE;
  gce.time    :=GetCurrentTime;
  
  PluginLink^.CallServiceSync(MS_GC_EVENT,0,lparam(@gce));
end;

procedure SendToChat(hContact:THANDLE;pszText:PWideChar);
var
  gci:TGC_INFO;
  pszModule:PAnsiChar;
  i,cnt:integer;
begin
  pszModule:=PAnsiChar(CallService(MS_PROTO_GETCONTACTBASEPROTO,hContact,0));
  cnt:=CallService(MS_GC_GETSESSIONCOUNT,0,lparam(pszModule));
  i:=0;
  gci.pszModule:=pszModule;
  while i<cnt do
  begin
    gci.iItem:=i;
    gci.Flags:=GCI_BYINDEX+GCI_HCONTACT+GCI_ID;
    CallService(MS_GC_GETINFO,0,lparam(@gci));
    if gci.hContact=hContact then
    begin
      SendChatText(gci.pszID.w,pszModule,pszText);
      break;
    end;
    inc(i);
  end;
end;

function FindContactHandle(proto:pAnsiChar;const dbv:TDBVARIANT;is_chat:boolean=false):THANDLE;
var
  uid:pAnsiChar;
  ldbv:TDBVARIANT;
  hContact:THANDLE;
  pw:pWideChar;
begin
  result:=0;
  uid:=nil;
  if not is_chat then
  begin
    uid:=pAnsiChar(CallProtoService(proto,PS_GETCAPS,PFLAG_UNIQUEIDSETTING,0));
    if uid=pAnsiChar(CALLSERVICE_NOTFOUND) then exit;
  end;

  hContact:=CallService(MS_DB_CONTACT_FINDFIRST,0,0);
  while hContact<>0 do
  begin
    if is_chat then
    begin
      if IsChat(hContact) then
      begin
        pw:=DBReadUnicode(hContact,proto,'ChatRoomID');
        if StrCmpW(pw,dbv.szVal.W)=0 then result:=hContact;
        mFreeMem(pw);
      end
    end
    else
    begin
      if DBReadSetting(hContact,proto,uid,@ldbv)=0 then
      begin
        if dbv._type=ldbv._type then
        begin
          case dbv._type of
            DBVT_DELETED: ;
            DBVT_BYTE   : if dbv.bVal=ldbv.bVal then result:=hContact;
            DBVT_WORD   : if dbv.wVal=ldbv.wVal then result:=hContact;
            DBVT_DWORD  : if dbv.dVal=ldbv.dVal then result:=hContact;
            DBVT_UTF8,
            DBVT_ASCIIZ : if StrCmp (dbv.szVal.A,ldbv.szVal.A)=0 then result:=hContact;
            DBVT_WCHAR  : if StrCmpW(dbv.szVal.W,ldbv.szVal.W)=0 then result:=hContact;
            DBVT_BLOB   : begin
              if dbv.cpbVal = ldbv.cpbVal then
              begin
                if CompareMem(dbv.pbVal,ldbv.pbVal,dbv.cpbVal) then
                  result:=hContact;
              end;
            end;
          end;
        end;
        DBFreeVariant(@ldbv);
      end;
    end;
    // added 2011.04.20
    if result<>0 then break;
    hContact:=CallService(MS_DB_CONTACT_FINDNEXT,hContact,0);
  end;
end;

function IsContactActive(hContact:THANDLE;proto:pAnsiChar=nil):integer;
var
  p:PPROTOACCOUNT;
  dbv  :TDBVARIANT;
  dbcgs:TDBCONTACTGETSETTING;
  name: array [0..31] of AnsiChar;
begin

  dbv._type  :=DBVT_ASCIIZ;
  dbv.szVal.a:=@name;
  dbv.cchVal :=SizeOf(name);
  dbcgs.pValue   :=@dbv;
  dbcgs.szModule :='Protocol';
  dbcgs.szSetting:='p';

  if PluginLink^.CallService(MS_DB_CONTACT_GETSETTINGSTATIC,hContact,lparam(@dbcgs))=0 then
  begin
    result:=0;

    if PluginLink^.ServiceExists(MS_PROTO_GETACCOUNT)<>0 then
    begin
      p:=PPROTOACCOUNT(CallService(MS_PROTO_GETACCOUNT,0,lparam(dbv.szVal.a)));
      if p=nil then
        result:=-2 // deleted
      else if (p^.bIsEnabled=0) or p^.bDynDisabled then
        result:=-1; // disabled
    end
    else
    begin
      if CallService(MS_PROTO_ISPROTOCOLLOADED,0,lparam(dbv.szVal.a))=0 then
        result:=-1;
    end;

    if (result=0) and (DBReadByte(hContact,strCList,'Hidden',0)=0) then
    begin
      result:=255;
      if PluginLink^.ServiceExists(MS_MC_GETMETACONTACT)<>0 then
      begin
        if CallService(MS_MC_GETMETACONTACT,hContact,0)<>0 then
          result:=2;
        if StrCmp(
           PAnsiChar(CallService(MS_PROTO_GETCONTACTBASEPROTO,hContact,0)),
           PAnsiChar(CallService(MS_MC_GETPROTOCOLNAME,0,0)))=0 then
         result:=1;
      end;
    end;
    if proto<>nil then
      StrCopy(proto,dbv.szVal.a);
  end
  else
  begin
    result:=-2;
    if proto<>nil then
      proto^:=#0;
  end;
 
end;

// Import plugin function adaptation
function CreateGroupW(name:pWideChar;hContact:THANDLE):integer;
var
  groupId:integer;
  groupIdStr:array [0..10] of AnsiChar;
  dbv:TDBVARIANT;
  cgs:TDBCONTACTGETSETTING;
  grbuf:array [0..127] of WideChar;
  p:pWideChar;
begin
  if (name=nil) or (name^=#0) then
  begin
    result:=0;
    exit;
  end;

  StrCopyW(@grbuf[1],name);
  grbuf[0]:=WideChar(1 or GROUPF_EXPANDED);

  // Check for duplicate & find unused id
  groupId:=0;
  cgs.szModule:='CListGroups';
  cgs.pValue  :=@dbv;
  repeat
    dbv._type:=DBVT_WCHAR;
    cgs.szSetting:=IntToStr(groupIdStr,groupId);
    if PluginLink^.CallService(MS_DB_CONTACT_GETSETTING_STR,0,lParam(@cgs))<>0 then
      break;

    if StrCmpW(dbv.szVal.w+1,@grbuf[1])=0 then
    begin
      if hContact<>0 then
        DBWriteUnicode(hContact,strCList,'Group',@grbuf[1]);

      DBFreeVariant(@dbv);
      result:=0;
      exit;
    end;

    DBFreeVariant(@dbv);
    inc(groupid);
  until false;

  DBWriteUnicode(0,'CListGroups',groupIdStr,grbuf);

  if hContact<>0 then
    DBWriteUnicode(hContact,strCList,'Group',@grbuf[1]);

  p:=StrRScanW(grbuf,'\');
  if p<>nil then
  begin
    p^:=#0;
    CreateGroupW(grbuf+1,0);
  end;

  result:=1;
end;

function CreateGroup(name:pAnsiChar;hContact:THANDLE):integer;
var
  groupId:integer;
  groupIdStr:array [0..10] of AnsiChar;
  dbv:TDBVARIANT;
  cgs:TDBCONTACTGETSETTING;
  grbuf:array [0..127] of AnsiChar;
  p:pAnsiChar;
begin
  if (name=nil) or (name^=#0) then
  begin
    result:=0;
    exit;
  end;

  StrCopy(@grbuf[1],name);
  grbuf[0]:=CHAR(1 or GROUPF_EXPANDED);

  // Check for duplicate & find unused id
  groupId:=0;
  cgs.szModule:='CListGroups';
  cgs.pValue  :=@dbv;
  repeat
    dbv._type:=DBVT_ASCIIZ;
    cgs.szSetting:=IntToStr(groupIdStr,groupId);
    if PluginLink^.CallService(MS_DB_CONTACT_GETSETTING_STR,0,lParam(@cgs))<>0 then
      break;

    if StrCmp(dbv.szVal.a+1,@grbuf[1])=0 then
    begin
      if hContact<>0 then
      DBWriteString(hContact,strCList,'Group',@grbuf[1]);

      DBFreeVariant(@dbv);
      result:=0;
      exit;
    end;

    DBFreeVariant(@dbv);
    inc(groupid);
  until false;

  DBWriteString(0,'CListGroups',groupIdStr,grbuf);

  if hContact<>0 then
    DBWriteString(hContact,strCList,'Group',@grbuf[1]);

  p:=StrRScan(grbuf,'\');
  if p<>nil then
  begin
    p^:=#0;
    CreateGroup(grbuf+1,0);
  end;

  result:=1;
end;

{$IFDEF KOL_MCK}
function MakeGroupMenu(idxfrom:integer=100):HMENU;
var
  sl:PWStrList;
  i:integer;
  b:array [0..15] of AnsiChar;
  p:pWideChar;
begin
  result:=CreatePopupMenu;
  i:=0;
  AppendMenuW(result,MF_STRING,idxfrom,TranslateW('<Root Group>'));
  AppendMenuW(result,MF_SEPARATOR,0,nil);
  sl:=NewWStrList;
  repeat
    p:=DBReadUnicode(0,'CListGroups',IntToStr(b,i),nil);
    if p=nil then break;
    sl.Add(p+1);
    mFreeMem(p);
    inc(i);
  until false;
  sl.Sort(false);
  inc(idxfrom);
  for i:=0 to sl.Count-1 do
  begin
    AppendMenuW(result,MF_STRING,idxfrom+i,pWideChar(sl.Items[i]));
  end;
  sl.Clear;
  sl.Free;
end;
{$ELSE}
function MakeGroupMenu(idxfrom:integer=100):HMENU;
var
  i:integer;
  b:array [0..15] of AnsiChar;
  p:pWideChar;
begin
  result:=CreatePopupMenu;
  i:=0;
  AppendMenuW(result,MF_STRING,idxfrom,TranslateW('<Root Group>'));
  AppendMenuW(result,MF_SEPARATOR,0,nil);
  inc(idxfrom);
  repeat
    p:=DBReadUnicode(0,'CListGroups',IntToStr(b,i),nil);
    if p=nil then break;
    AppendMenuW(result,MF_STRING,idxfrom,p+1);
    mFreeMem(p);
    inc(i);
    inc(idxfrom);
  until false;
end;
{$ENDIF}

function GetNewGroupName(parent:HWND):pWideChar;
var
  mmenu:HMENU;
  i:integer;
  buf:array [0..63] of WideChar;
  pt:TPoint;
begin
  result:=nil;
  mmenu:=MakeGroupMenu(100);
  GetCursorPos(pt);
  i:=integer(TrackPopupMenu(mmenu,TPM_RETURNCMD+TPM_NONOTIFY,pt.x,pt.y,0,parent,nil));
  if i>100 then // no root or cancel
  begin
    GetMenuStringW(mmenu,i,buf,HIGH(buf)+1,MF_BYCOMMAND);
    StrDupW(result,buf);
  end;
  DestroyMenu(mmenu);
end;

function SendRequest(url:PAnsiChar;rtype:int;args:pAnsiChar=nil;hNetLib:THANDLE=0):pAnsiChar;
var
  nlu:TNETLIBUSER;
  req :TNETLIBHTTPREQUEST;
  resp:PNETLIBHTTPREQUEST;
  hTmpNetLib:THANDLE;
  nlh:array [0..1] of TNETLIBHTTPHEADER;
  buf:array [0..31] of AnsiChar;
begin
  result:=nil;

  FillChar(req,SizeOf(req),0);
  req.cbSize     :=NETLIBHTTPREQUEST_V1_SIZE;//SizeOf(req);
  req.requestType:=rtype;
  req.szUrl      :=url;
  req.flags      :=NLHRF_NODUMP or NLHRF_HTTP11;
  if args<>nil then
  begin
    nlh[0].szName :='Content-Type';
    nlh[0].szValue:='application/x-www-form-urlencoded';
    nlh[1].szName :='Content-Length';
    nlh[1].szValue:=IntToStr(buf,StrLen(args));
    req.headers     :=@nlh;
    req.headersCount:=2;
    req.pData       :=args;
    req.dataLength  :=StrLen(args);
  end;

  if hNetLib=0 then
  begin
    FillChar(nlu,SizeOf(nlu),0);
    nlu.cbSize          :=SizeOf(nlu);
    nlu.flags           :=NUF_HTTPCONNS or NUF_NOHTTPSOPTION or NUF_OUTGOING or NUF_NOOPTIONS;
    nlu.szSettingsModule:='dummy';
    hTmpNetlib:=CallService(MS_NETLIB_REGISTERUSER,0,lparam(@nlu));
  end
  else
    hTmpNetLib:=hNetLib;

  resp:=pointer(CallService(MS_NETLIB_HTTPTRANSACTION,hTmpNetLib,lparam(@req)));

  if resp<>nil then
  begin
    if resp^.resultCode=200 then
    begin
      StrDup(result,resp.pData,resp.dataLength);
    end
    else
    begin
    end;
    CallService(MS_NETLIB_FREEHTTPREQUESTSTRUCT,0,lparam(resp));
  end;

  if (hNetLib=0) and (nlu.cbSize<>0) then
    CallService(MS_NETLIB_CLOSEHANDLE,hTmpNetLib,0);
end;

(*
static int __inline NLog(AnsiChar *msg) {
  return CallService(MS_NETLIB_LOG, (WPARAM)hNetlibUser, (LPARAM)msg);
}
*)
function GetFile(url:PAnsiChar;save_file:PAnsiChar;
                 hNetLib:THANDLE=0;recurse_count:integer=0):bool;
var
  nlu:TNETLIBUSER;
  req :TNETLIBHTTPREQUEST;
  resp:PNETLIBHTTPREQUEST;
  hSaveFile:THANDLE;
  i:integer;
begin
  result:=false;
  if recurse_count>MAX_REDIRECT_RECURSE then
    exit;
  if (url=nil) or (url^=#0) or (save_file=nil) or (save_file^=#0) then
    exit;

  FillChar(req,SizeOf(req),0);
  req.cbSize     :=NETLIBHTTPREQUEST_V1_SIZE;//SizeOf(req);
  req.requestType:=REQUEST_GET;
  req.szUrl      :=url;
  req.flags      :=NLHRF_NODUMP;


  FillChar(nlu,SizeOf(nlu),0);
  if hNetLib=0 then
  begin
    nlu.cbSize          :=SizeOf(nlu);
    nlu.flags           :=NUF_HTTPCONNS or NUF_NOHTTPSOPTION or NUF_OUTGOING or NUF_NOOPTIONS;
    nlu.szSettingsModule:='dummy';
    hNetlib:=CallService(MS_NETLIB_REGISTERUSER,0,lparam(@nlu));
  end;

  resp:=pointer(CallService(MS_NETLIB_HTTPTRANSACTION,hNetlib,lparam(@req)));

  if resp<>nil then
  begin
    if resp^.resultCode=200 then
    begin
      hSaveFile:=Rewrite(save_file);
      if hSaveFile<>THANDLE(INVALID_HANDLE_VALUE) then
      begin
        BlockWrite(hSaveFile,resp^.pData^,resp^.dataLength);
        CloseHandle(hSaveFile);
        result:=true;
      end
    end
    else if (resp.resultCode>=300) and (resp.resultCode<400) then
    begin
      // get new location
      for i:=0 to resp^.headersCount-1 do
      begin
        //MessageBox(0,resp^.headers[i].szValue, resp^.headers[i].szName,MB_OK);
        if StrCmp(resp^.headers^[i].szName,'Location')=0 then
        begin
          result:=GetFile(resp^.headers^[i].szValue,save_file,hNetLib,recurse_count+1);
          break;
        end
      end;
    end
    else
    begin
{
      _stprintf(buff, TranslateT("Failed to download \"%s\" - Invalid response, code %d"), plugin_name, resp->resultCode);

      ShowError(buff);
      AnsiChar *ts = GetAString(buff);
      NLog(ts);
}
    end;
    CallService(MS_NETLIB_FREEHTTPREQUESTSTRUCT,0,lparam(resp));

    if nlu.cbSize<>0 then
      CallService(MS_NETLIB_CLOSEHANDLE,hNetLib,0);
  end;
end;

function GetFile(url:PWideChar;save_file:PWideChar;
                 hNetLib:THANDLE=0;recurse_count:integer=0):bool;
var
  aurl,asave:array [0..MAX_PATH-1] of AnsiChar;
begin
  FastWideToAnsiBuf(url,aurl);
  FastWideToAnsiBuf(save_file,asave);
  result:=GetFile(aurl,asave,hNetLib,0);
end;

function GetProxy(hNetLib:THANDLE):PAnsiChar;
var
  nlus:TNETLIBUSERSETTINGS;
  pc:PAnsiChar;
  proxy:array [0..127] of AnsiChar;
begin
  result:=nil;
  nlus.cbSize:=SizeOf(nlus);
  if CallService(MS_NETLIB_GETUSERSETTINGS,hNetLib,lparam(@nlus))<>0 then
  begin
    if nlus.useProxy<>0 then
    begin
      if nlus.proxyType<>PROXYTYPE_IE then
      begin
        pc:=@proxy;
        if nlus.szProxyServer<>nil then
        begin
          if nlus.useProxyAuth<>0 then
          begin
            if nlus.szProxyAuthUser<>nil then
            begin
              pc:=StrCopyE(proxy,nlus.szProxyAuthUser);
              if nlus.szProxyAuthPassword<>nil then
              begin
                pc^:=':'; inc(pc);
                pc:=StrCopyE(pc,nlus.szProxyAuthPassword);
              end;
              pc^:='@';
              inc(pc);
            end;
          end;
          pc:=StrCopyE(pc,nlus.szProxyServer);
          if nlus.wProxyPort<>0 then
          begin
            pc^:=':'; inc(pc);
            IntToStr(pc,nlus.wProxyPort);
          end;
        end;
        StrDup(result,proxy);
      end
      else // use IE proxy
      begin
        mGetMem(result,1);
        result^:=#0;
      end;
    end;
  end;
end;

function LoadImageURL(url:pAnsiChar;size:integer=0):HBITMAP;
var
  nlu:TNETLIBUSER;
  req :TNETLIBHTTPREQUEST;
  resp:PNETLIBHTTPREQUEST;
  hNetLib:THANDLE;
  im:TIMGSRVC_MEMIO;
begin
  result:=0;
  if (url=nil) or (url^=#0) then
    exit;

  FillChar(req,SizeOf(req),0);
  req.cbSize     :=NETLIBHTTPREQUEST_V1_SIZE;//SizeOf(req);
  req.requestType:=REQUEST_GET;
  req.szUrl      :=url;
  req.flags      :=NLHRF_NODUMP;

  FillChar(nlu,SizeOf(nlu),0);
  nlu.cbSize          :=SizeOf(nlu);
  nlu.flags           :=NUF_HTTPCONNS or NUF_NOHTTPSOPTION or NUF_OUTGOING or NUF_NOOPTIONS;
  nlu.szSettingsModule:='dummy';
  hNetlib:=CallService(MS_NETLIB_REGISTERUSER,0,lparam(@nlu));

  resp:=pointer(CallService(MS_NETLIB_HTTPTRANSACTION,hNetlib,lparam(@req)));

  if resp<>nil then
  begin
    if resp^.resultCode=200 then
    begin
      im.iLen :=resp.dataLength;
      im.pBuf :=resp.pData;
      im.flags:=size shl 16;
      im.fif  :=FIF_JPEG;
      result  :=CallService(MS_IMG_LOADFROMMEM,wparam(@im),0);
//      if result<>0 then
//        DeleteObject(SendMessage(wnd,STM_SETIMAGE,IMAGE_BITMAP,result)); //!!
    end;
    CallService(MS_NETLIB_FREEHTTPREQUESTSTRUCT,0,lparam(resp));
  end;
  CallService(MS_NETLIB_CLOSEHANDLE,hNetLib,0);
end;

function RegisterSingleIcon(resname,ilname,descr,group:PAnsiChar):int;
var
  sid:TSKINICONDESC;
begin
  FillChar(sid,SizeOf(TSKINICONDESC),0);
  sid.cbSize     :=SizeOf(TSKINICONDESC);
  sid.cx         :=16;
  sid.cy         :=16;
  sid.flags      :=0;
  sid.szSection.a:=group;

  sid.hDefaultIcon   :=LoadImageA(hInstance,resname,IMAGE_ICON,16,16,0);
  sid.pszName        :=ilname;
  sid.szDescription.a:=descr;
  result:=PluginLink^.CallService(MS_SKIN2_ADDICON,0,lparam(@sid));
  DestroyIcon(sid.hDefaultIcon);
end;

end.
