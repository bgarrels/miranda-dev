{.$DEFINE USE_MMI}
{$INCLUDE compilers.inc}
unit common;

interface

uses
windows
{$IFDEF USE_MMI}
,m_api
{$ENDIF}
;

Const {- Character sets -}
  sBinNum   = ['0'..'1'];
  sOctNum   = ['0'..'7'];
  sNum      = ['0'..'9'];
  sHexNum   = ['0'..'9','A'..'F','a'..'f'];
  sWord     = ['0'..'9','A'..'Z','a'..'z','_',#128..#255];
  sIdFirst  = ['A'..'Z','a'..'z','_'];
  sLatWord  = ['0'..'9','A'..'Z','a'..'z','_'];
  sWordOnly = ['A'..'Z','a'..'z'];
  sSpace    = [#9,' '];
  sEmpty    = [#9,#10,#13,' '];

const
  HexDigitChrLo: array [0..15] of AnsiChar = ('0','1','2','3','4','5','6','7',
                                              '8','9','a','b','c','d','e','f');

  HexDigitChr  : array [0..15] of AnsiChar = ('0','1','2','3','4','5','6','7',
                                              '8','9','A','B','C','D','E','F');

const
  mimecnt = 5;
  mimes:array [0..mimecnt-1] of record
     mime:PAnsiChar;
     ext:array [0..3] of AnsiChar
  end = (
  (mime:'image/gif' ; ext:'GIF'),
  (mime:'image/jpg' ; ext:'JPG'),
  (mime:'image/jpeg'; ext:'JPG'),
  (mime:'image/png' ; ext:'PNG'),
  (mime:'image/bmp' ; ext:'BMP')
);

var
  IsW2K,
  IsVista,
  IsAnsi:boolean;

const
  CP_UNICODE    = 1200;
  CP_REVERSEBOM = 65534;
const
  SIGN_UNICODE    = $FEFF;
  SIGN_REVERSEBOM = $FFFE;
  SIGN_UTF8       = $BFBBEF;

function Encode(dst,src:pAnsiChar):PAnsiChar;
function Decode(dst,src:pAnsiChar):PAnsiChar;
function GetTextFormat(Buffer:pByte;sz:cardinal):integer;

function IIF(cond:bool;ret1,ret2:integer  ):integer;   overload;
function IIF(cond:bool;ret1,ret2:PAnsiChar):PAnsiChar; overload;
function IIF(cond:bool;ret1,ret2:pWideChar):pWideChar; overload;
function IIF(cond:bool;ret1,ret2:Extended ):Extended;  overload;
function IIF(cond:bool;ret1,ret2:tDateTime):tDateTime; overload;
function IIF(cond:bool;ret1,ret2:pointer  ):pointer;   overload;
function IIF(cond:bool;ret1,ret2:string   ):string;    overload;
{$IFNDEF DELPHI7_UP}
function IIF(cond:bool;ret1,ret2:variant  ):variant;   overload;
{$ENDIF}

function GetImageType (buf:pByte;mime:PAnsiChar=nil):dword;
function GetImageTypeW(buf:pByte;mime:PWideChar=nil):int64;

procedure CopyToClipboard(txt:pointer; ansi:bool);
function  PasteFromClipboard(ansi:boolean;cp:dword=CP_ACP):pointer;

function  mGetMem (var dst;size:integer):pointer;
procedure mFreeMem(var ptr);
function  mReallocMem(var dst; size:integer):pointer;

// String processing
function WideToCombo(src:PWideChar;var dst;cp:integer=CP_ACP):integer;

function ChangeUnicode(str:PWideChar):PWideChar;
function UTF8Len(src:PAnsiChar):integer;
function WideToANSI(src:PWideChar;var dst:PAnsiChar;cp:dword=CP_ACP):PAnsiChar;
function ANSIToWide(src:PAnsiChar;var dst:PWideChar;cp:dword=CP_ACP):PWideChar;
function ANSIToUTF8(src:PAnsiChar;var dst:PAnsiChar;cp:dword=CP_ACP):PAnsiChar;
function UTF8toANSI(src:PAnsiChar;var dst:PAnsiChar;cp:dword=CP_ACP):PAnsiChar;
function UTF8toWide(src:PAnsiChar;var dst:PWideChar;len:cardinal=dword(-1)):PWideChar;
function WidetoUTF8(src:PWideChar;var dst:PAnsiChar):PAnsiChar;

function FastWideToAnsiBuf(src:PWideChar;dst:PAnsiChar;len:cardinal=dword(-1)):PAnsiChar;
function FastAnsiToWideBuf(src:PAnsiChar;dst:PWideChar;len:cardinal=dword(-1)):PWideChar;
function FastWideToAnsi   (src:PWideChar;var dst:PAnsiChar):PAnsiChar;
function FastAnsiToWide   (src:PAnsiChar;var dst:PWideChar):PWideChar;

function UnEscape(buf:PAnsiChar):PAnsiChar;
function Escape  (buf:PAnsiChar):PAnsiChar;

// ----- base strings functions -----
function StrDup (var dst:PAnsiChar;src:PAnsiChar;len:cardinal=0):PAnsiChar;
function StrDupW(var dst:PWideChar;src:PWideChar;len:cardinal=0):PWideChar;
function StrDelete (aStr:PAnsiChar;pos,len:cardinal):PAnsiChar;
function StrDeleteW(aStr:PWideChar;pos,len:cardinal):PWideChar;
function StrInsert (substr,src:PAnsiChar;pos:cardinal):PAnsiChar;
function StrInsertW(substr,src:PWideChar;pos:cardinal):PWideChar;
function StrReplace (src,SubStr,NewStr:PAnsiChar):PAnsiChar;
function StrReplaceW(src,SubStr,NewStr:pWideChar):PWideChar;
function CharReplace (dst:pAnsiChar;old,new:AnsiChar):PAnsiChar;
function CharReplaceW(dst:pWideChar;old,new:WideChar):PWideChar;
function StrCmp (a,b:PAnsiChar;n:cardinal=$FFFFFFFF):integer;
function StrCmpW(a,b:PWideChar;n:cardinal=$FFFFFFFF):integer;
function StrEnd (const a:PAnsiChar):PAnsiChar;
function StrEndW(const a:PWideChar):PWideChar;
function StrScan (src:PAnsiChar;c:AnsiChar):PAnsiChar;
function StrScanW(src:PWideChar;c:WideChar):PWideChar;
function StrRScan (src:PAnsiChar;c:AnsiChar):PAnsiChar;
function StrRScanW(src:PWideChar;c:WideChar):PWideChar;
function StrLen (Str: PAnsiChar): Cardinal;
function StrLenW(Str: PWideChar): Cardinal;
function StrCat (Dest: PAnsiChar; const Source: PAnsiChar): PAnsiChar;
function StrCatW(Dest: PWideChar; const Source: PWideChar): PWideChar;
function StrCatE (Dest: PAnsiChar; const Source: PAnsiChar): PAnsiChar;
function StrCatEW(Dest: PWideChar; const Source: PWideChar): PWideChar;
function StrCopyE (dst:PAnsiChar;src:PAnsiChar;len:cardinal=0):PAnsiChar;
function StrCopyEW(dst:PWideChar;src:PWideChar;len:cardinal=0):PWideChar;
function StrCopy (dst:PAnsiChar;src:PAnsiChar;len:cardinal=0):PAnsiChar;
function StrCopyW(dst:PWideChar;src:PWideChar;len:cardinal=0):PWideChar;
function StrPos (const aStr, aSubStr: PAnsiChar): PAnsiChar;
function StrPosW(const aStr, aSubStr: PWideChar): PWideChar;
function StrIndex (const aStr, aSubStr: PAnsiChar):integer;
function StrIndexW(const aStr, aSubStr: PWideChar):integer;

procedure FillWord(var buf;count:cardinal;value:word); register;
function CompareMem(P1, P2: Pointer; Length: Integer): Boolean;
function Min(a,b:integer):integer;
function Max(a,b:integer):integer;

function Timestamp(Year,Month,Day:cardinal;Hour:cardinal=0;Min:cardinal=0;Sec:cardinal=0):dword;
function GetCurrentTime:dword;

function TimeToInt(stime:PAnsiChar):integer; overload;
function TimeToInt(stime:PWideChar):integer; overload;
function IntToTime(dst:pWideChar;time:integer):pWideChar; overload;
function IntToTime(dst:PAnsiChar;time:integer):PAnsiChar; overload;

{
  filesize to string conversion
  value   - filelength
  divider - 1=byte; 1024=kbyte; 1024*1024 - Mbyte
  prec    - numbers after point (1-3)
  post    - 0=none
            1=(small)' bytes','kb','mb'
            2=(mix)  ' Bytes','Kb','Mb'
            3=(caps) ''      ,'KB','MB'
  postfix calculated from 'divider' value
}
function IntToK(dst:pWideChar;value,divider,prec,post:integer):pWideChar;

// string conversion
function IntToHex(dst:pWideChar;Value:dword  ;Digits:integer=0):pWideChar; overload;
function IntToHex(dst:PAnsiChar;Value:dword  ;Digits:integer=0):PAnsiChar; overload;
function IntToStr(dst:pWideChar;Value:integer;Digits:integer=0):pWideChar; overload;
function IntToStr(dst:PAnsiChar;Value:integer;Digits:integer=0):PAnsiChar; overload;
function StrToInt(src:pWideChar):int64; overload;
function StrToInt(src:PAnsiChar):int64; overload;
function HexToInt(src:pWideChar;len:cardinal=$FFFF):int64; overload;
function HexToInt(src:PAnsiChar;len:cardinal=$FFFF):int64; overload;

// filename work
function ChangeExt (src,ext:PAnsiChar):PAnsiChar;
function ChangeExtW(src,ext:PWideChar):PWideChar;
function Extract (s:PAnsiChar;name:Boolean=true):PAnsiChar;
function ExtractW(s:pWideChar;name:Boolean=true):pWideChar;
function GetExt(fname,dst:pWideChar;maxlen:dword=100):pWideChar; overload;
function GetExt(fname,dst:PAnsiChar;maxlen:dword=100):PAnsiChar; overload;

procedure UpperCase(src:pWideChar);
procedure LowerCase(src:pWideChar);
function  GetPairChar(ch:AnsiChar):AnsiChar; overload;
function  GetPairChar(ch:WideChar):WideChar; overload;

type
  lSortProc = function (First,Second:integer):integer;
  {0=equ; 1=1st>2nd; -1=1st<2nd }
procedure ShellSort(size:integer;Compare,Swap:lSortProc);

function isPathAbsolute(path:pWideChar):boolean; overload;
function isPathAbsolute(path:PAnsiChar):boolean; overload;

implementation

function Encode(dst,src:pAnsiChar):PAnsiChar;
begin
  while src^<>#0 do
  begin
    if not (src^ in [' ','%','+','&','?',#128..#255]) then
      dst^:=src^
    else
    begin
      dst^:='%'; inc(dst);
      dst^:=HexDigitChr[ord(src^) shr 4]; inc(dst);
      dst^:=HexDigitChr[ord(src^) and $0F];
    end;
    inc(src);
    inc(dst);
  end;
  dst^:=#0;
  result:=dst;
end;

function Decode(dst,src:pAnsiChar):PAnsiChar;
begin
  while (src^<>#0) and (src^<>'&') do
  begin
    if (src^='%') and ((src+1)^ in sHexNum) and ((src+2)^ in sHexNum) then
    begin
      inc(src);
      dst^:=chr(HexToInt(src,2));
      inc(src);
    end
    else
      dst^:=src^;
    inc(dst);
    inc(src);
  end;
  dst^:=#0;
  result:=dst;
end;

const
  IS_TEXT_UNICODE_ASCII16            = $1;
  IS_TEXT_UNICODE_REVERSE_ASCII16    = $10;
  IS_TEXT_UNICODE_STATISTICS         = $2;
  IS_TEXT_UNICODE_REVERSE_STATISTICS = $20;
  IS_TEXT_UNICODE_CONTROLS           = $4;
  IS_TEXT_UNICODE_REVERSE_CONTROLS   = $40;
  IS_TEXT_UNICODE_SIGNATURE          = $8;
  IS_TEXT_UNICODE_REVERSE_SIGNATURE  = $80;
  IS_TEXT_UNICODE_ILLEGAL_CHARS      = $100;
  IS_TEXT_UNICODE_ODD_LENGTH         = $200;
  IS_TEXT_UNICODE_DBCS_LEADBYTE      = $400;
  IS_TEXT_UNICODE_NULL_BYTES         = $1000;
  IS_TEXT_UNICODE_UNICODE_MASK       = $F;
  IS_TEXT_UNICODE_REVERSE_MASK       = $F0;
  IS_TEXT_UNICODE_NOT_UNICODE_MASK   = $F00;
  IS_TEXT_UNICODE_NOT_ASCII_MASK     = $F000;

function IsTextUTF8(Buffer:pbyte;Length:integer):boolean;
var
  Ascii:boolean;
  Octets:cardinal;
  c:byte;
begin
	Ascii:=true;
	Octets:=0;

	if Length=0 then
	  Length:=-1;
	repeat
	  if (Length=0) or (Buffer^=0) then
	    break;
	  dec(Length);
		c:=Buffer^;
		if (c and $80)<>0 then
			Ascii:=false;
		if Octets<>0 then
		begin
			if (c and $C0)<>$80 then
			begin
				result:=false;
				exit;
			end;
			dec(Octets);
		end
		else
		begin
			if (c and $80)<>0 then
			begin
				while (c and $80)<>0 do
				begin
					c:=c shl 1;
					inc(Octets);
				end;
				dec(Octets);
				if Octets=0 then
				begin
					result:=false;
					exit;
				end;
			end
		end;
		inc(buffer);
	until false;
	result:= not ((Octets>0) or Ascii);
end;

function GetTextFormat(Buffer:pByte;sz:cardinal):integer;
var
  test:integer;
begin
	result:=-1;

	if sz>=2 then
	begin
  	if       pword (Buffer)^               =SIGN_UNICODE    then result := CP_UNICODE
	  else if  pword (Buffer)^               =SIGN_REVERSEBOM then result := CP_REVERSEBOM
  	else if  (sz>=4) and 
           ((pdword(Buffer)^ and $00FFFFFF)=SIGN_UTF8)      then result := CP_UTF8;
  end;

	if result<0 then
	begin
		test:=
			IS_TEXT_UNICODE_STATISTICS         or
			IS_TEXT_UNICODE_REVERSE_STATISTICS or
			IS_TEXT_UNICODE_CONTROLS           or
			IS_TEXT_UNICODE_REVERSE_CONTROLS   or
			IS_TEXT_UNICODE_ILLEGAL_CHARS      or
			IS_TEXT_UNICODE_ODD_LENGTH         or
			IS_TEXT_UNICODE_NULL_BYTES;

		if not odd(sz) and IsTextUnicode(Buffer,sz,@test) then
		begin
			if (test and (IS_TEXT_UNICODE_ODD_LENGTH or IS_TEXT_UNICODE_ILLEGAL_CHARS))=0 then
			begin
				if (test and (IS_TEXT_UNICODE_NULL_BYTES or
				              IS_TEXT_UNICODE_CONTROLS   or
				              IS_TEXT_UNICODE_REVERSE_CONTROLS))<>0 then
				begin
					if (test and (IS_TEXT_UNICODE_CONTROLS or
					              IS_TEXT_UNICODE_STATISTICS))<>0 then
						result:=CP_UNICODE
					else if (test and (IS_TEXT_UNICODE_REVERSE_CONTROLS or
					                   IS_TEXT_UNICODE_REVERSE_STATISTICS))<>0 then
						result:=CP_REVERSEBOM;
				end
			end
		end
		else if IsTextUTF8(Buffer,sz) then
			result:=CP_UTF8
		else
		  result:=CP_ACP;
	end;
end;

function IIF(cond:bool;ret1,ret2:integer):integer; overload;
begin
  if cond then result:=ret1 else result:=ret2;
end;
function IIF(cond:bool;ret1,ret2:PAnsiChar):PAnsiChar; overload;
begin
  if cond then result:=ret1 else result:=ret2;
end;
function IIF(cond:bool;ret1,ret2:pWideChar):pWideChar; overload;
begin
  if cond then result:=ret1 else result:=ret2;
end;
function IIF(cond:bool;ret1,ret2:Extended):Extended; overload;
begin
  if cond then result:=ret1 else result:=ret2;
end;
function IIF(cond:bool;ret1,ret2:tDateTime):tDateTime; overload;
begin
  if cond then result:=ret1 else result:=ret2;
end;
function IIF(cond:bool;ret1,ret2:pointer):pointer; overload;
begin
  if cond then result:=ret1 else result:=ret2;
end;
function IIF(cond:bool;ret1,ret2:string):string; overload;
begin
  if cond then result:=ret1 else result:=ret2;
end;
{$IFNDEF DELPHI7_UP}
function IIF(cond:bool;ret1,ret2:variant):variant; overload;
begin
  if cond then result:=ret1 else result:=ret2;
end;
{$ENDIF}

function GetImageType(buf:pByte;mime:PAnsiChar=nil):dword;
var
  i:integer;
begin
  result:=0;
  if (mime<>nil) and (mime^<>#0) then
  begin
    for i:=0 to mimecnt-1 do
    begin
      if {lstrcmpia}StrCmp(mime,mimes[i].mime)=0 then
      begin
        result:=dword(mimes[i].ext);
        exit;
      end;
    end;
  end
  else if buf<>nil then
  begin
    if (pdword(buf)^ and $F0FFFFFF)=$E0FFD8FF then result:=$0047504A // 'JPG'
    else if pdword(buf)^=$38464947 then result:=$00464947 // 'GIF'
    else if pdword(buf)^=$474E5089 then result:=$00474E50 // 'PNG'
    else if pword (buf)^=$4D42     then result:=$00504D42 // 'BMP'
  end;
end;

function GetImageTypeW(buf:pByte;mime:PWideChar=nil):int64;
var
  i:integer;
  lmime:array [0..63] of AnsiChar;
begin
  result:=0;
  if (mime<>nil) and (mime^<>#0) then
  begin
    FastWideToAnsiBuf(mime,lmime);
    for i:=0 to mimecnt-1 do
    begin
      if {lstrcmpia}StrCmp(lmime,mimes[i].mime)=0 then
      begin
//        result:=dword(mimes[i].ext);
        FastAnsiToWideBuf(mimes[i].ext,PWideChar(@result));
        exit;
      end;
    end;
  end
  else if buf<>nil then
  begin
    if (pdword(buf)^ and $F0FFFFFF)=$E0FFD8FF then result:=$000000470050004A // 'JPG'
    else if pdword(buf)^=$38464947 then result:=$0000004600490047 // 'GIF'
    else if pdword(buf)^=$474E5089 then result:=$00000047004E0050 // 'PNG'
    else if pword (buf)^=$4D42     then result:=$00000050004D0042 // 'BMP'
  end;
end;

procedure CopyToClipboard(txt:pointer; ansi:bool);
var
  s:pointer;
  fh:THANDLE;
begin
  if pointer(txt)=nil then
    exit;
  if ansi then 
  begin
    if PAnsiChar(txt)^=#0 then exit
  end
  else
    if PWideChar(txt)^=#0 then exit;

  if OpenClipboard(0) then
  begin
    if ansi then
    begin
      fh:=GlobalAlloc(GMEM_MOVEABLE+GMEM_DDESHARE,(StrLen(PAnsiChar(txt))+1));
      s:=GlobalLock(fh);
      StrCopy(s,PAnsiChar(txt));
    end
    else
    begin
      fh:=GlobalAlloc(GMEM_MOVEABLE+GMEM_DDESHARE,
          (StrLenW(PWideChar(txt))+1)*SizeOf(WideChar));
      s:=GlobalLock(fh);
      StrCopyW(s,PWideChar(txt));
    end;
    GlobalUnlock(fh);
    EmptyClipboard;
    if ansi then
      SetClipboardData(CF_TEXT,fh)
    else
      SetClipboardData(CF_UNICODETEXT,fh);
    CloseClipboard;
  end;
end;

function PasteFromClipboard(ansi:boolean;cp:dword=CP_ACP):pointer;
var
  p:pWideChar;
  fh:tHandle;
begin
  if OpenClipboard(0) then
  begin
    if not ansi then
    begin
      fh:=GetClipboardData(CF_UNICODETEXT);
      if fh<>0 then
      begin
        p:=GlobalLock(fh);
        StrDupW(pWideChar(result),p);
      end
      else
      begin
        fh:=GetClipboardData(CF_TEXT);
        if fh<>0 then
        begin
          p:=GlobalLock(fh);
          AnsiToWide(PAnsiChar(p),pWideChar(result),cp);
        end;
      end;
    end
    else
    begin
      fh:=GetClipboardData(CF_TEXT);
      if fh<>0 then
      begin
        p:=GlobalLock(fh);
        StrDup(PAnsiChar(result),PAnsiChar(p));
      end;
    end;
    if fh<>0 then
      GlobalUnlock(fh);
    CloseClipboard;
  end
end;

procedure CheckSystem;
var
  ovi:TOSVersionInfo;
begin
  ovi.dwOSVersionInfoSize:=SizeOf(TOSVersionInfo);
  GetVersionEx(ovi);
//VER_PLATFORM_WIN32_NT for 2KXP
  with ovi do
  begin
    IsAnsi :=dwPlatformId=VER_PLATFORM_WIN32_WINDOWS;
    IsW2K  :=(dwMajorVersion=5) and (dwMinorVersion=0);
    IsVista:=(dwMajorVersion=6) and (dwMinorVersion=0);
  end;
end;

// --------- string conversion ----------

function WideToCombo(src:PWideChar;var dst;cp:integer=CP_ACP):integer;
var
  pc:PAnsiChar;
  i,j:Cardinal;
begin
  WideToAnsi(src,pc,cp);
  j:=StrLen(pc)+1;
  i:=j+(StrLenW(src)+1)*SizeOf(WideChar);
  mGetMem(PAnsiChar(dst),i);
  StrCopy(PAnsiChar(dst),pc);
  mFreeMem(pc);
  StrCopyW(pWideChar(PAnsiChar(dst)+j),src);
  result:=i;
end;

function ChangeUnicode(str:PWideChar):PWideChar;
var
  i,len:integer;
begin
  result:=str;
  if (str=nil) or (str^=#0) then
    exit;
  if (word(str^)=$FFFE) or (word(str^)=$FEFF) then
  begin
    len:=StrLenW(str);
    if word(str^)=$FFFE then
    begin
      i:=len-1;
      while i>0 do // str^<>#0
      begin
        pword(str)^:=swap(pword(str)^);
        inc(str);
        dec(i);
      end;
    end;
    move((result+1)^,result^,len*SizeOf(WideChar));
  end;
end;

function WideToANSI(src:PWideChar;var dst:PAnsiChar; cp:dword=CP_ACP):PAnsiChar;
var
  len,l:integer;
begin
  if (src=nil) or (src^=#0) then
  begin
    mGetMem(result,SizeOf(AnsiChar));
    result^:=#0;
  end
  else
  begin
    l:=StrLenW(src);
    len:=WideCharToMultiByte(cp,0,src,l,NIL,0,NIL,NIL)+1;
    mGetMem(result,len);
    FillChar(result^,len,0);
    WideCharToMultiByte(cp,0,src,l,result,len,NIL,NIL);
  end;
  dst:=result;
end;

function ANSIToWide(src:PAnsiChar;var dst:PWideChar; cp:dword=CP_ACP):PWideChar;
var
  len,l:integer;
begin
  if (src=nil) or (src^=#0) then
  begin
    mGetMem(result,SizeOf(WideChar));
    result^:=#0;
  end
  else
  begin
    l:=StrLen(src);
    len:=MultiByteToWideChar(cp,0,src,l,NIL,0)+1;
    mGetMem(result,len*SizeOf(WideChar));
    FillChar(result^,len*SizeOf(WideChar),0);
    MultiByteToWideChar(cp,0,src,l,result,len);
  end;
  dst:=result;
end;

function ANSIToUTF8(src:PAnsiChar;var dst:PAnsiChar;cp:dword=CP_ACP):PAnsiChar;
var
  tmp:PWideChar;
begin
  AnsiToWide(src,tmp,cp);
  result:=WideToUTF8(tmp,dst);
  mFreeMem(tmp);
end;

function UTF8Len(src:PAnsiChar):integer; // w/o zero
begin
  result:=0;
  if src<>nil then
  begin
    if (pdword(src)^ and $00FFFFFF)=SIGN_UTF8 then
      inc(src,3);
    while src^<>#0 do
    begin
      if (ord(src^) and $80)=0 then
      else if (ord(src^) and $E0)=$E0 then
        inc(src,2)
      else
        inc(src);
      inc(result);
      inc(src);
    end;
  end;
end;

function CalcUTF8Len(src:pWideChar):integer;
begin
  result:=0;
  if src<>nil then
  begin
    while src^<>#0 do
    begin
      if src^<#$0080 then
      else if src^<#$0800 then
        inc(result)
      else
        inc(result,2);
      inc(src);
      inc(result);
    end;
  end;
end;

function UTF8toWide(src:PAnsiChar; var dst:PWideChar; len:cardinal=dword(-1)):PWideChar;
var
  w:word;
  p:PWideChar;
begin
  mGetMem(dst,(UTF8Len(src)+1)*SizeOf(WideChar));
  p:=dst;
  if src<>nil then
  begin
    if (pdword(src)^ and $00FFFFFF)=SIGN_UTF8 then
      inc(src,3);
    while (src^<>#0) and (len>0) do
    begin
      if ord(src^)<$80 then
        w:=ord(src^)
      else if (ord(src^) and $E0)=$E0 then
      begin
        w:=(ord(src^) and $1F) shl 12;
        inc(src); dec(len);
        w:=w or (((ord(src^))and $3F) shl 6);
        inc(src); dec(len);
        w:=w or (ord(src^) and $3F);
      end
      else
      begin
        w:=(ord(src^) and $3F) shl 6;
        inc(src); dec(len);
        w:=w or (ord(src^) and $3F);
      end;
      p^:=WideChar(w);
      inc(p);
      inc(src); dec(len);
    end;
  end;
  p^:=#0;
  result:=dst;
end;

function UTF8toANSI(src:PAnsiChar;var dst:PAnsiChar;cp:dword=CP_ACP):PAnsiChar;
var
  tmp:pWideChar;
begin
  UTF8ToWide(src,tmp);
  result:=WideToAnsi(tmp,dst,cp);
  mFreeMem(tmp);
end;

function WidetoUTF8(src:PWideChar; var dst:PAnsiChar):PAnsiChar;
var
  p:PAnsiChar;
begin
  mGetMem(dst,CalcUTF8Len(src)+1);
  p:=dst;
  if src<>nil then
  begin
    while src^<>#0 do
    begin
      if src^<#$0080 then
        p^:=AnsiChar(src^)
      else if src^<#$0800 then
      begin
        p^:=AnsiChar($C0 or (ord(src^) shr 6));
        inc(p);
        p^:=AnsiChar($80 or (ord(src^) and $3F));
      end
      else
      begin
        p^:=AnsiChar($E0 or (ord(src^) shr 12));
        inc(p);
        p^:=AnsiChar($80 or ((ord(src^) shr 6) and $3F));
        inc(p);
        p^:=AnsiChar($80 or (ord(src^) and $3F));
      end;
      inc(p);
      inc(src);
    end;
  end;
  p^:=#0;
  result:=dst;
end;

procedure FillWord(var buf;count:cardinal;value:word); register; assembler;
{
  PUSH EDI 
  MOV EDI, ECX // Move Value To Write 
  MOV ECX, EDX // Move Number to ECX for countdown 
  MOV EDX, EAX // Move over buffer 
  MOV EAX, EDI // Value to Write needs to be here 
  MOV EDI, EDX // Pointer to Buffer[0] 
  REP STOSW 
  POP EDI
}
asm
  push edi
  mov  edi,buf // destination
  mov  ax,value   // value
  mov  ecx,count // count
  rep  stosw
  pop  edi
{
  push edi
  mov  edi,eax // destination
  mov  ax,cx   // value
  mov  ecx,edx // count
  rep  stosw
  pop  edi
}
end;

// from SysUtils
{ Delphi 7.0
function CompareMem(P1, P2: Pointer; Length: Integer): Boolean; assembler;
asm
     PUSH    ESI
     PUSH    EDI
     MOV     ESI,P1
     MOV     EDI,P2
     MOV     EDX,ECX
     XOR     EAX,EAX
     AND     EDX,3
     SAR     ECX,2
     JS      @@1     // Negative Length implies identity.
     REPE    CMPSD
     JNE     @@2
     MOV     ECX,EDX
     REPE    CMPSB
     JNE     @@2
@@1: INC     EAX
@@2: POP     EDI
     POP     ESI
end;
}
{$IFNDEF WIN64}
// Delphi 2009 realization
function CompareMem(P1, P2: Pointer; Length: Integer): Boolean; assembler;
asm
   add   eax, ecx
   add   edx, ecx
   xor   ecx, -1
   add   eax, -8
   add   edx, -8
   add   ecx, 9
   push  ebx
   jg    @Dword
   mov   ebx, [eax+ecx]
   cmp   ebx, [edx+ecx]
   jne   @Ret0
   lea   ebx, [eax+ecx]
   add   ecx, 4
   and   ebx, 3
   sub   ecx, ebx
   jg    @Dword
@DwordLoop:
   mov   ebx, [eax+ecx]
   cmp   ebx, [edx+ecx]
   jne   @Ret0
   mov   ebx, [eax+ecx+4]
   cmp   ebx, [edx+ecx+4]
   jne   @Ret0
   add   ecx, 8
   jg    @Dword
   mov   ebx, [eax+ecx]
   cmp   ebx, [edx+ecx]
   jne   @Ret0
   mov   ebx, [eax+ecx+4]
   cmp   ebx, [edx+ecx+4]
   jne   @Ret0
   add   ecx, 8
   jle   @DwordLoop
@Dword:
   cmp   ecx, 4
   jg    @Word
   mov   ebx, [eax+ecx]
   cmp   ebx, [edx+ecx]
   jne   @Ret0
   add   ecx, 4
@Word:
   cmp   ecx, 6
   jg    @Byte
   movzx ebx, word ptr [eax+ecx]
   cmp   bx, [edx+ecx]
   jne   @Ret0
   add   ecx, 2
@Byte:
   cmp   ecx, 7
   jg    @Ret1
   movzx ebx, byte ptr [eax+7]
   cmp   bl, [edx+7]
   jne   @Ret0
@Ret1:
   mov   eax, 1
   pop   ebx
   ret
@Ret0:
   xor   eax, eax
   pop   ebx
end;
{$ELSE}
function CompareMem(P1, P2: Pointer; Length: Integer): Boolean;
begin
  result:=CompareByte(P1,P2,Length)=0;
end; 
{$ENDIF}

function Min(a,b:integer):integer;
begin
  if a>b then
    result:=b
  else
    result:=a;
end;

function Max(a,b:integer):integer;
begin
  if a<b then
    result:=b
  else
    result:=a;
end;

function mGetMem(var dst;size:integer):pointer;
begin
{$IFDEF USE_MMI}
  if @mmi.malloc<>nil then
    pointer(dst):=mmi.malloc(size)
  else
{$ENDIF}
    GetMem(pointer(dst),size);
  result:=pointer(dst);
end;

procedure mFreeMem(var ptr);
begin
  if pointer(ptr)<>nil then
  begin
{$IFDEF USE_MMI}
    if @mmi.free<>nil then
      mmi.free(pointer(ptr))
    else
{$ENDIF}
      FreeMem(pointer(ptr));
    Pointer(ptr):=nil;
  end;
end;

function mReallocMem(var dst; size:integer):pointer;
begin
{$IFDEF USE_MMI}
  if @mmi.malloc<>nil then
    pointer(dst):=mmi.realloc(pointer(dst),size)
  else
{$ENDIF}
    ReallocMem(pointer(dst),size);
  result:=pointer(dst);
end;

function UnEscape(buf:PAnsiChar):PAnsiChar;
begin
  if (buf<>nil) and (buf^<>#0) then
  begin
    StrReplace(buf,PAnsiChar(#$7F'n'),PAnsiChar(#$0D#$0A));
    StrReplace(buf,PAnsiChar(#$7F't'),PAnsiChar(#$09));
  end;
  result:=buf;
end;

function Escape(buf:PAnsiChar):PAnsiChar;
var
  i:integer;
begin
  i:=StrLen(buf);
  if i<>0 then
  begin
    Move(buf^,(buf+1)^,i+1);
    buf^:=#39;
    (buf+i+1)^:=#39;
    (buf+i+2)^:=#0;
    StrReplace(buf,#$0D#$0A,#$7F'n');
    StrReplace(buf,#$09,#$7F't');
  end;
  result:=buf;
end;

procedure ShellSort(size:integer;Compare,Swap:lSortProc);
var
  i,j,gap:longint;
begin
  gap:=size shr 1;
  while gap>0 do
  begin
    for i:=gap to size-1 do
    begin
      j:=i-gap;
      while (j>=0) and (Compare(j,UInt(j+gap))>0) do
      begin
        Swap(j,UInt(j+gap));
        dec(j,gap);
      end;
    end;
    gap:=gap shr 1;
  end;
end;

const
  Posts:array [0..8] of PWideChar =
    (' bytes',' Bytes','','kb','Kb','KB','mb','Mb','MB');

function IntToK(dst:pWidechar;value,divider,prec,post:integer):pWidechar;
var
  tmp:integer;
  p:pWideChar;
  ls:array [0..4] of WideChar;
begin
  result:=dst;
  IntToStr(dst,value div divider);
  if divider=1 then prec:=0;
  while dst^<>#0 do inc(dst);
  if prec<>0 then
  begin
    if      prec=1 then prec:=10
    else if prec=2 then prec:=100
    else {if prec=3 then} prec:=1000;
    tmp:=round(frac(value*1.0/divider)*prec);
    dst^:='.'; inc(dst);
    IntToStr(ls,tmp);
    p:=ls;
    while p^<>#0 do
    begin
      dst^:=p^; inc(dst); inc(p);
    end;
    dst^:=#0;
  end;
  if post<>0 then
  begin
    if divider=1 then
      StrCatW(dst,Posts[post-1])
    else
    begin
      if divider=1024 then tmp:=1
      else {if divider=1024*1024 then} tmp:=2;
      p:=Posts[tmp*3+post-1];
      dst^:=p[0]; inc(dst);
      dst^:=p[1]; inc(dst);
      dst^:=#0;
    end;
  end;
end;

// ----- base string functions -----
function StrDup(var dst:PAnsiChar;src:PAnsiChar;len:cardinal=0):PAnsiChar;
var
  l:cardinal;
  p:pAnsiChar;
begin
  if (src=nil) or (src^=#0) then
    dst:=nil
  else
  begin
    if len=0 then
      len:=high(cardinal);
    p:=src;
    l:=len;
    while (p^<>#0) and (l>0) do
    begin
      inc(p); dec(l);
    end;
    l:=p-src;

    mGetMem(dst,l+1);
    move(src^, dst^,l);
    dst[l]:=#0;
  end;
  result:=dst;
end;

function StrDupW(var dst:PWideChar;src:PWideChar;len:cardinal=0):PWideChar;
var
  l:cardinal;
  p:pWideChar;
begin
  if (src=nil) or (src^=#0) then
    dst:=nil
  else
  begin
    if len=0 then
      len:=high(cardinal);
    p:=src;
    l:=len;
    while (p^<>#0) and (l>0) do
    begin
      inc(p); dec(l);
    end;
    l:=p-src;
    mGetMem(dst,(l+1)*SizeOf(WideChar));
    move(src^, dst^,l*SizeOf(WideChar));
    dst[l]:=#0;
  end;
  result:=dst;
end;

function StrCopyE(dst:PAnsiChar;src:PAnsiChar;len:cardinal=0):PAnsiChar;
var
  l:cardinal;
  p:pAnsiChar;
begin
  if dst<>nil then
  begin
    if (src=nil) or (src^=#0) then
      dst^:=#0
    else
    begin
      if len=0 then
        len:=high(cardinal);
      p:=src;
      l:=len;
      while (p^<>#0) and (l>0) do
      begin
        inc(p); dec(l);
      end;
      l:=p-src;
      move(src^, dst^,l);
      inc(dst,l);
      dst^:=#0;
    end;
  end;
  result:=dst;
end;

function StrCopyEW(dst:PWideChar;src:PWideChar;len:cardinal=0):PWideChar;
var
  l:cardinal;
  p:pWideChar;
begin
  if dst<>nil then
  begin
    if (src=nil) or (src^=#0) then
      dst^:=#0
    else
    begin
      if len=0 then
        len:=high(cardinal);
      p:=src;
      l:=len;
      while (p^<>#0) and (l>0) do
      begin
        inc(p); dec(l);
      end;
      l:=p-src;
      move(src^, dst^,l*SizeOf(WideChar));
      inc(dst,l);
      dst^:=#0;
    end;
  end;
  result:=dst;
end;

function StrCopy(dst:PAnsiChar;src:PAnsiChar;len:cardinal=0):PAnsiChar;
var
  l:cardinal;
  p:pAnsiChar;
begin
  if dst<>nil then
  begin
    if (src=nil) or (src^=#0) then
      dst^:=#0
    else
    begin
      if len=0 then
        len:=high(cardinal);
      p:=src;
      l:=len;
      while (p^<>#0) and (l>0) do
      begin
        inc(p); dec(l);
      end;
      l:=p-src;
      move(src^, dst^,l);
      dst[l]:=#0;
    end;
  end;
  result:=dst;
end;

function StrCopyW(dst:PWideChar;src:PWideChar;len:cardinal=0):PWideChar;
var
  l:cardinal;
  p:pWideChar;
begin
  if dst<>nil then
  begin
    if (src=nil) or (src^=#0) then
      dst^:=#0
    else
    begin
      if len=0 then
        len:=high(cardinal);
      p:=src;
      l:=len;
      while (p^<>#0) and (l>0) do
      begin
        inc(p); dec(l);
      end;
      l:=p-src;
      move(src^, dst^,l*SizeOf(WideChar));
      dst[l]:=#0;
    end;
  end;
  result:=dst;
end;

function StrDelete(aStr:PAnsiChar;pos,len:cardinal):PAnsiChar;
var
  i:cardinal;
begin
  if len>0 then
  begin
    i:=StrLen(aStr);
    if pos<i then
    begin
      if (pos+len)>i then
        len:=i-pos;
      StrCopy(aStr+pos,aStr+pos+len);
    end;
  end;
  result:=aStr;
end;

function StrDeleteW(aStr:PWideChar;pos,len:cardinal):PWideChar;
var
  i:cardinal;
begin
  if len>0 then
  begin
    i:=StrLenW(aStr);
    if pos<i then
    begin
      if (pos+len)>i then
        len:=i-pos;
      StrCopyW(aStr+pos,aStr+pos+len);
    end;
  end;
  result:=aStr;
end;

function StrInsert(substr,src:PAnsiChar;pos:cardinal):PAnsiChar;
var
  i:cardinal;
  p:PAnsiChar;
begin
  i:=StrLen(substr);
  if i<>0 then
  begin
    p:=src+pos;
    move(p^,(p+i)^,StrLen(src)-pos+1);
    move(substr^,p^,i);
  end;
  result:=src;
end;

function StrInsertW(substr,src:PWideChar;pos:cardinal):PWideChar;
var
  i:cardinal;
  p:PWideChar;
begin
  i:=StrLenW(substr);
  if i<>0 then
  begin
    p:=src+pos;
    move(p^,(p+i)^,(StrLenW(src)-pos+1)*SizeOf(PWideChar));
    move(substr^,p^,i*SizeOf(WideChar));
  end;
  result:=src;
end;

function StrReplace(src,SubStr,NewStr:PAnsiChar):PAnsiChar;
var
  i,j,l:integer;
  k:integer;
  p:PAnsiChar;
begin
  result:=src;
  p:=StrPos(src,SubStr);
  if p=nil then exit;
  i:=StrLen(SubStr);
  j:=StrLen(NewStr);
  l:=i-j;
  repeat
    if j=0 then
      StrCopy(p,p+i)
    else
    begin
      k:=StrLen(p)+1;
      if l>0 then
        move((p+l)^,p^,k-l)
      else if l<>0 then
        move(p^,(p-l)^,k);
      move(NewStr^,p^,j); {new characters}
      inc(p,j);
    end;
    p:=StrPos(p,SubStr);
    if p=nil then break;
  until false;
end;

function StrReplaceW(src,SubStr,NewStr:pWideChar):PWideChar;
var
  i,j,l:integer;
  k:integer;
  p:PWideChar;
begin
  result:=src;
  p:=StrPosW(src,SubStr);
  if p=nil then exit;
  i:=StrLenW(SubStr);
  j:=StrLenW(NewStr);
  l:=i-j;
  repeat
    if j=0 then
      StrCopyW(p,p+i)
    else
    begin
      k:=(StrLenW(p)+1)*SizeOf(WideChar);
      if l>0 then
        move((p+l)^,p^,k-l*SizeOf(WideChar))
      else if l<>0 then
        move(p^,(p-l)^,k);
      move(NewStr^,p^,j*SizeOf(WideChar)); {new characters}
      inc(p,j);
    end;
    p:=StrPosW(p,SubStr);
    if p=nil then break;
  until false;
end;

function CharReplace(dst:pAnsiChar;old,new:AnsiChar):PAnsiChar;
begin
  result:=dst;
  if dst<>nil then
  begin
    while dst^<>#0 do
    begin
      if dst^=old then dst^:=new;
      inc(dst);
    end;
  end;
end;

function CharReplaceW(dst:pWideChar;old,new:WideChar):PWideChar;
begin
  result:=dst;
  if dst<>nil then
  begin
    while dst^<>#0 do
    begin
      if dst^=old then dst^:=new;
      inc(dst);
    end;
  end;
end;

function StrCmp(a,b:PAnsiChar;n:cardinal=$FFFFFFFF):integer; // CompareString
begin
  result:=0;
  if (a=nil) and (b=nil) then
    exit;
  if (a=nil) or (b=nil) then
  begin
    result:=-1;
    exit;
  end;
  while n>0 do
  begin
    result:=ord(a^)-ord(b^);
    if (result<>0) or (a^=#0) then
      break;
    inc(a);
    inc(b);
    dec(n);
  end;
end;

function StrCmpW(a,b:PWideChar;n:cardinal=$FFFFFFFF):integer;
begin
  result:=0;
  if (a=nil) and (b=nil) then
    exit;
  if (a=nil) or (b=nil) then
  begin
    result:=-1;
    exit;
  end;
  while n>0 do
  begin
    result:=ord(a^)-ord(b^);
    if (result<>0) or (a^=#0) then
      break;
    inc(a);
    inc(b);
    dec(n);
  end;
end;

function StrEnd(const a:PAnsiChar):PAnsiChar;
begin
  result:=a;
  if result<>nil then
    while result^<>#0 do inc(result);
end;

function StrEndW(const a:PWideChar):PWideChar;
begin
  result:=a;
  if result<>nil then
    while result^<>#0 do inc(result);
end;

function StrScan(src:PAnsiChar;c:AnsiChar):PAnsiChar;
begin
  if src<>nil then
  begin
    while (src^<>#0) and (src^<>c) do inc(src);
    if src^<>#0 then
    begin
      result:=src;
      exit;
    end;
  end;
  result:=nil;
end;

function StrRScan(src:PAnsiChar;c:AnsiChar):PAnsiChar;
begin
  if src<>nil then
  begin
    result:=StrEnd(src);
    while (result>=src) and (result^<>c) do dec(result);
    if result<src then
      result:=nil;
  end
  else
    result:=nil;
end;

function StrScanW(src:PWideChar;c:WideChar):PWideChar;
begin
  if src<>nil then
  begin
    while (src^<>#0) and (src^<>c) do inc(src);
    if src^<>#0 then
    begin
      result:=src;
      exit;
    end;
  end;
  result:=nil;
end;

function StrRScanW(src:PWideChar;c:WideChar):PWideChar;
begin
  if src<>nil then
  begin
    result:=StrEndW(src);
    while (result>=src) and (result^<>c) do dec(result);
    if result<src then
      result:=nil;
  end
  else
    result:=nil;
end;

function StrLen(Str: PAnsiChar): Cardinal;
var
  P : PAnsiChar;
begin
  P := Str;
  if P<>nil then
    while (P^ <> #0) do Inc(P);
  Result := (P - Str);
end;

function StrLenW(Str: PWideChar): Cardinal;
var
  P : PWideChar;
begin
  P := Str;
  if P<>nil then
    while (P^ <> #0) do Inc(P);
  Result := (P - Str);
end;

function StrCat(Dest: PAnsiChar; const Source: PAnsiChar): PAnsiChar;
begin
  if dest<>nil then
    StrCopy(StrEnd(Dest), Source);
  Result := Dest;
end;

function StrCatW(Dest: PWideChar; const Source: PWideChar): PWideChar;
begin
  if dest<>nil then
    StrCopyW(StrEndW(Dest), Source);
  Result := Dest;
end;

function StrCatE(Dest: PAnsiChar; const Source: PAnsiChar): PAnsiChar;
begin
  if dest<>nil then
    result:=StrCopyE(StrEnd(Dest), Source)
  else
    result:=nil;
end;

function StrCatEW(Dest: PWideChar; const Source: PWideChar): PWideChar;
begin
  if dest<>nil then
    result:=StrCopyEW(StrEndW(Dest), Source)
  else
    result:=nil;
end;

function StrPos(const aStr, aSubStr: PAnsiChar): PAnsiChar;
var
  Str, SubStr: PAnsiChar;
  Ch: AnsiChar;
begin
  if (aStr = nil) or (aStr^ = #0) or (aSubStr = nil) or (aSubStr^ = #0) then
  begin
    Result := nil;
    Exit;
  end;
  Result := aStr;
  Ch := aSubStr^;
  repeat
    if Result^ = Ch then
    begin
      Str := Result;
      SubStr := aSubStr;
      repeat
        Inc(Str);
        Inc(SubStr);
        if SubStr^ = #0 then exit;
        if Str^ = #0 then
        begin
          Result := nil;
          exit;
        end;
        if Str^ <> SubStr^ then break;
      until (FALSE);
    end;
    Inc(Result);
  until (Result^ = #0);
  Result := nil;
end;

function StrIndex(const aStr, aSubStr: PAnsiChar):integer;
var
  p:pAnsiChar;
begin
  p:=StrPos(aStr,aSubStr);
  if p=nil then
    result:=0
  else
    result:=p-aStr+1;
end;

function StrPosW(const aStr, aSubStr: PWideChar): PWideChar;
var
  Str, SubStr: PWideChar;
  Ch: WideChar;
begin
  if (aStr = nil) or (aStr^ = #0) or (aSubStr = nil) or (aSubStr^ = #0) then
  begin
    Result := nil;
    Exit;
  end;
  Result := aStr;
  Ch := aSubStr^;
  repeat
    if Result^ = Ch then
    begin
      Str := Result;
       SubStr := aSubStr;
      repeat
        Inc(Str);
        Inc(SubStr);
        if SubStr^ = #0 then exit;
        if Str^ = #0 then
        begin
          Result := nil;
          exit;
        end;
        if Str^ <> SubStr^ then break;
      until (FALSE);
    end;
    Inc(Result);
  until (Result^ = #0);
  Result := nil;
end;

function StrIndexW(const aStr, aSubStr: PWideChar):integer;
var
  p:pWideChar;
begin
  p:=StrPosW(aStr,aSubStr);
  if p=nil then
    result:=0
  else
    result:=(p-aStr)+1; //!!!!
end;

// ----- filenames  -----

function ChangeExt(src,ext:PAnsiChar):PAnsiChar;
var
  i,j:integer;
begin
  i:=StrLen(src);
  j:=i;
  while (i>0) and (src[i]<>'\') and (src[i]<>':') and (src[i]<>'.') do dec(i);
  if src[i]<>'.' then
  begin
    i:=j;
    src[i]:='.';
  end;
  if ext=nil then
    ext:='';
  StrCopy(src+i+1,ext);
  result:=src;
end;

function ChangeExtW(src,ext:PWideChar):PWideChar;
var
  i,j:integer;
begin
  i:=StrLenW(src);
  j:=i;
  while (i>0) and (src[i]<>'\') and (src[i]<>':') and (src[i]<>'.') do dec(i);
  if src[i]<>'.' then
  begin
    i:=j;
    src[i]:='.';
  end;
  if ext=nil then
    ext:='';
  StrCopyW(src+i+1,ext);
  result:=src;
end;

function Extract(s:PAnsiChar;name:Boolean=true):PAnsiChar;
var
  i,j:integer;
begin
  i:=StrLen(s)-1;
  j:=i;
  while (i>=0) and ((s[i]<>'\') and (s[i]<>'/')) do dec(i);
  if name then
  begin
    mGetMem(result,(j-i+1));
    StrCopy(result,s+i+1);
  end
  else
  begin
    StrDup(result,s,i+1);
  end;
end;

function ExtractW(s:pWideChar;name:Boolean=true):pWideChar;
var
  i,j:integer;
begin
  i:=StrLenW(s)-1;
  j:=i;
  while (i>=0) and ((s[i]<>'\') and (s[i]<>'/')) do dec(i);
  if name then
  begin
    mGetMem(result,(j-i+1)*SizeOf(WideChar));
    StrCopyW(result,s+i+1);
  end
  else
  begin
    StrDupW(result,s,i+1);
  end;
end;

function GetExt(fname,dst:pWideChar;maxlen:dword=100):pWideChar;
var
  ppc,pc:PWideChar;
begin
  result:=dst;
  dst^:=#0;
  if fname<>nil then
  begin
    pc:=strendw(fname)-1;
    while (pc>fname) and ((pc^='"') or (pc^=' ')) do dec(pc);
    ppc:=pc+1;
    while (pc>fname) and (pc^<>'.') do
    begin
      if maxlen=0 then exit;
      if not (AnsiChar(pc^) in ['0'..'9','A'..'Z','_','a'..'z']) then exit;
      dec(maxlen);
      dec(pc); //empty name not allowed!
    end;
    if pc>fname then
    begin
      repeat
        inc(pc);
        if pc=ppc then
        begin
          dst^:=#0;
          break;
        end;
        if (pc^>='a') and (pc^<='z') then
          dst^:=WideChar(ord(pc^)-$20)
        else
          dst^:=pc^;
        inc(dst);
      until false;
    end;
  end;
end;

function GetExt(fname,dst:PAnsiChar;maxlen:dword=100):PAnsiChar;
var
  ppc,pc:PAnsiChar;
begin
  result:=dst;
  dst^:=#0;
  if fname<>nil then
  begin
    pc:=StrEnd(fname)-1;
    while (pc>fname) and ((pc^='"') or (pc^=' ')) do dec(pc);
    ppc:=pc+1;
    while (pc>fname) and (pc^<>'.') do
    begin
      if maxlen=0 then exit;
      if not (AnsiChar(pc^) in ['0'..'9','A'..'Z','_','a'..'z']) then exit;
      dec(maxlen);
      dec(pc); //empty name not allowed!
    end;
    if pc>fname then
    begin
      repeat
        inc(pc);
        if pc=ppc then
        begin
          dst^:=#0;
          break;
        end;
        if (pc^>='a') and (pc^<='z') then
          dst^:=AnsiChar(ord(pc^)-$20)
        else
          dst^:=pc^;
        inc(dst);
      until false;
    end;
  end;
end;

type
  PDayTable = ^TDayTable;
  TDayTable = array [0..11] of cardinal;

const
  MonthDays: array [Boolean] of TDayTable =
    ((31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31),
     (31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31));

const
  DateDelta = 693594;
{ Days between TDateTime basis (12/31/1899) and Unix time_t basis (1/1/1970) }
  UnixDateDelta = 25569;

function IsLeapYear(Year:Word):Boolean;
begin
  Result:=(Year mod 4=0) and ((Year mod 100<>0) or (Year mod 400=0));
end;

function EncodeTime(Hour, Min, Sec: cardinal): TDateTime;
begin
  result := (Hour*3600 + Min*60 + Sec) / 86400;
end;

function EncodeDate(Year, Month, Day: cardinal):TDateTime;
var
  DayTable: PDayTable;
begin
  DayTable := @MonthDays[IsLeapYear(Year)];
  dec(Month);
  while Month>0 do
  begin
    dec(Month);
    inc(Day,DayTable^[Month]);
  end;

  dec(Year);
  result := Year * 365 + Year div 4 - Year div 100 + Year div 400 + Day - DateDelta;
end;

function Timestamp(Year,Month,Day:cardinal;Hour:cardinal=0;Min:cardinal=0;Sec:cardinal=0):dword;
var
  t:tDateTime;
begin
  t := EncodeDate(Year, Month, Day);
  if t >= 0 then
    t := t + EncodeTime(Hour, Min, Sec)
  else
    t := t - EncodeTime(Hour, Min, Sec);
  result:=Round((t - UnixDateDelta) * 86400)
end;

function GetCurrentTime:dword;
var
  st:tSystemTime;
begin
  GetSystemTime(st);
  result:=Timestamp(st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
end;

function TimeToInt(stime:PAnsiChar):integer;
var
  hour,min,sec,len,i:integer;
begin
  len:=StrLen(stime);
  i:=0;
  sec :=0;
  min :=0;
  hour:=0;
  while i<len do
  begin
    if (stime[i]<'0') or (stime[i]>'9') then
    begin
      if min>0 then
        hour:=min;
      min:=sec;
      sec:=0;
    end
    else
      sec:=sec*10+ord(stime[i])-ord('0');
    inc(i);
  end;
  result:=hour*3600+min*60+sec;
end;

function TimeToInt(stime:PWideChar):integer;
var
  buf:array [0..63] of AnsiChar;
begin
  result:=TimeToInt(FastWideToAnsiBuf(stime,buf));
end;

function IntToTime(dst:PAnsiChar;time:integer):PAnsiChar;
var
  day,hour,min,sec:array [0..7] of AnsiChar;
  d,h:integer;
begin
  result:=dst;
  h:=time div 3600;
  dec(time,h*3600);
  IntToStr(sec,(time mod 60),2);
  d:=h div 24;
  if d>0 then
  begin
    h:=h mod 24;
    IntToStr(day,d);
    dst^:=day[0]; inc(dst);
    if day[1]<>#0 then        // now only 99 days max
    begin
      dst^:=day[1]; inc(dst);
    end;
    dst^:=' '; inc(dst);
  end;
  if h>0 then
  begin
    IntToStr(hour,h);
    IntToStr(min,(time div 60),2);
    dst^:=hour[0]; inc(dst);
    if hour[1]<>#0 then
    begin
      dst^:=hour[1]; inc(dst);
    end;
    dst^:=':';    inc(dst);
    dst^:=min[0]; inc(dst);
    dst^:=min[1]; inc(dst);
  end
  else
  begin
    IntToStr(min,time div 60);
    dst^:=min[0]; inc(dst);
    if min[1]<>#0 then
    begin
      dst^:=min[1]; inc(dst);
    end;
  end;
  dst^:=':';    inc(dst);
  dst^:=sec[0]; inc(dst);
  dst^:=sec[1]; inc(dst);
  dst^:=#0;
end;

function IntToTime(dst:pWideChar;time:integer):pWideChar;
var
  buf:array [0..63] of AnsiChar;
begin
  result:=FastAnsiToWideBuf(IntToTime(buf,time),dst);
end;

function StrToInt(src:pWideChar):int64;
var
  sign:boolean;
begin
  result:=0;
  if src<>nil then
  begin
    sign:=src^='-';
    if sign then inc(src);
    while src^<>#0 do
    begin
      if (src^>='0') and (src^<='9') then
        result:=result*10+ord(src^)-ord('0')
      else
        break;
      inc(src);
    end;
    if sign then result:=-result;
  end;
end;

function StrToInt(src:PAnsiChar):int64;
var
  sign:boolean;
begin
  result:=0;
  if src<>nil then
  begin
    sign:=src^='-';
    if sign then inc(src);
    while src^<>#0 do
    begin
      if (src^>='0') and (src^<='9') then
        result:=result*10+ord(src^)-ord('0')
      else
        break;
      inc(src);
    end;
    if sign then result:=-result;
  end;
end;

function IntToStr(dst:PAnsiChar;Value:integer;Digits:integer=0):PAnsiChar;
var
  i:dword;
begin
  if Digits<=0 then
  begin
    if Value<0 then
      Digits:=1
    else
      Digits:=0;
    i:=ABS(Value);
    repeat
      i:=i div 10;
      inc(Digits);
    until i=0;
  end;
  dst[Digits]:=#0;
  i:=ABS(Value);
  repeat
    dec(Digits);
    dst[Digits]:=AnsiChar(ord('0')+(i mod 10));
    i:=i div 10;
    if (Value<0) and (Digits=1) then
    begin
      dst[0]:='-';
      break;
    end;
  until Digits=0;
  result:=dst;
end;

function IntToStr(dst:pWideChar;Value:integer;Digits:integer=0):pWideChar;
var
  i:dword;
begin
  if Digits<=0 then
  begin
    if Value<0 then
      Digits:=1
    else
      Digits:=0;
    i:=ABS(Value);
    repeat
      i:=i div 10;
      inc(Digits);
    until i=0;
  end;
  dst[Digits]:=#0;
  i:=ABS(Value);
  repeat
    dec(Digits);
    dst[Digits]:=WideChar(ord('0')+(i mod 10));
    i:=i div 10;
    if (Value<0) and (Digits=1) then
    begin
      dst[0]:='-';
      break;
    end;
  until Digits=0;
  result:=dst;
end;

function HexToInt(src:pWideChar;len:cardinal=$FFFF):int64;
begin
  result:=0;
  while (src^<>#0) and (len>0) do
  begin
    if (src^>='0') and (src^<='9') then
      result:=result*16+ord(src^)-ord('0')
    else if ((src^>='A') and (src^<='F')) then
      result:=result*16+ord(src^)-ord('A')+10
    else if ((src^>='a') and (src^<='f')) then
      result:=result*16+ord(src^)-ord('a')+10
    else
      break;
    inc(src);
    dec(len);
  end;
end;

function HexToInt(src:PAnsiChar;len:cardinal=$FFFF):int64;
begin
  result:=0;
  while (src^<>#0) and (len>0) do
  begin
    if (src^>='0') and (src^<='9') then
      result:=result*16+ord(src^)-ord('0')
    else if ((src^>='A') and (src^<='F')) then
      result:=result*16+ord(src^)-ord('A')+10
    else if ((src^>='a') and (src^<='f')) then
      result:=result*16+ord(src^)-ord('a')+10
    else
      break;
    inc(src);
    dec(len);
  end;
end;

function IntToHex(dst:pWidechar;Value:dword;Digits:integer=0):pWideChar;
var
  i:dword;
begin
  if Digits<=0 then
  begin
    Digits:=0;
    i:=Value;
    repeat
      i:=i shr 4;
      inc(Digits);
    until i=0;
  end;
  dst[Digits]:=#0;
  repeat
    Dec(Digits);
    dst[Digits]:=WideChar(HexDigitChr[Value and $F]);
    Value:=Value shr 4;
  until Digits=0;
  result:=dst;
end;

function IntToHex(dst:PAnsiChar;Value:dword;Digits:integer=0):PAnsiChar;
var
  i:dword;
begin
  if Digits<=0 then
  begin
    Digits:=0;
    i:=Value;
    repeat
      i:=i shr 4;
      inc(Digits);
    until i=0;
  end;
  dst[Digits]:=#0;
  repeat
    Dec(Digits);
    dst[Digits]:=HexDigitChr[Value and $F];
    Value:=Value shr 4;
  until Digits=0;
  result:=dst;
end;

procedure UpperCase(src:pWideChar);
var
  c:WideChar;
begin
  if src<>nil then
  begin
    while src^<>#0 do
    begin
      c:=src^;
      if (c>='a') and (c<='z') then
        src^:=WideChar(ord(c)-$20);
      inc(src);
    end;
  end;
end;

procedure LowerCase(src:pWideChar);
var
  c:WideChar;
begin
  if src<>nil then
  begin
    while src^<>#0 do
    begin
      c:=src^;
      if (c>='A') and (c<='Z') then
        src^:=WideChar(ord(c)+$20);
      inc(src);
    end;
  end;
end;

function GetPairChar(ch:AnsiChar):AnsiChar;
begin
  case ch of
    '[': result:=']';
    '<': result:='>';
    '(': result:=')';
    '{': result:='}';
  else // ' and " too
    result:=ch;
  end;
end;

function GetPairChar(ch:WideChar):WideChar;
begin
  case ch of
    '[': result:=']';
    '<': result:='>';
    '(': result:=')';
    '{': result:='}';
  else // ' and " too
    result:=ch;
  end;
end;

function FastWideToAnsiBuf(src:PWideChar;dst:PAnsiChar;len:cardinal=dword(-1)):PAnsiChar;
begin
  result:=dst;
  if src<>nil then
  begin
    repeat
      dst^:=AnsiChar(src^);
      if src^=#0 then
        break;
      dec(len);
      if len=0 then
      begin
        (dst+1)^:=#0;
        break;
      end;
      inc(src);
      inc(dst);
    until false;
  end
  else
    dst^:=#0;
end;

function FastWideToAnsi(src:PWideChar;var dst:PAnsiChar):PAnsiChar;
begin
  if src=nil then
    dst:=nil
  else
  begin
    mGetMem(dst,StrLenW(src)+1);
    FastWideToAnsiBuf(src,dst);
  end;
  result:=dst;
end;

function FastAnsiToWideBuf(src:PAnsiChar;dst:PWideChar;len:cardinal=dword(-1)):PWideChar;
begin
  result:=dst;
  if src<>nil then
  begin
    repeat
      dst^:=WideChar(src^);
      if src^=#0 then
        break;
      dec(len);
      if len=0 then
      begin
        (dst+1)^:=#0;
        break;
      end;
      inc(src);
      inc(dst);
    until false;
  end
  else
    dst^:=#0;
end;

function FastAnsiToWide(src:PAnsiChar;var dst:PWideChar):PWideChar;
begin
  if src=nil then
    dst:=nil
  else
  begin
    mGetMem(dst,(StrLen(src)+1)*SizeOf(WideChar));
    FastAnsiToWideBuf(src,dst);
  end;
  result:=dst;
end;

function isPathAbsolute(path:pWideChar):boolean;
begin
  result:=((path[1]=':') and (path[2]='\')) or ((path[0]='\') {and (path[1]='\')}) or
          (StrPosW(path,'://')<>nil);
end;

function isPathAbsolute(path:PAnsiChar):boolean;
begin
  result:=((path[1]=':') and (path[2]='\')) or ((path[0]='\') {and (path[1]='\')}) or
          (StrPos(path,'://')<>nil);
end;

begin
  CheckSystem;
end.
