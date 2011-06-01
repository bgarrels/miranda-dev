unit templates;
{$include compilers.inc}
interface
{$Resource templates.res}
implementation

uses
  messages,windows,commctrl,
  common,syswin,wrapper,
  m_api,dbsettings,mirutils,
  wat_api,global,macros,HlpDlg;

const
  splStopped:PWideChar = 'stopped';
  splPlaying:PWideChar = 'playing';
  splPaused :PWideChar = 'paused';
  chMono    :PWideChar = 'mono';
  chStereo  :PWideChar = 'stereo';
  ch51      :PWideChar = '5.1';
  chVBR     :PWideChar = 'VBR';
  chCBR     :PWideChar = 'CBR';

const
  LoCaseType  :integer=0;
  WriteCBR    :integer=0;
  ReplaceSpc  :integer=0;
  FSizeMode   :integer=1024*1024;
  FSizePost   :integer=2;
  FSPrecision :integer=2;
  PlayerCaps  :integer=0;
  ExportText:pWideChar=nil;

{$include i_tmpl_rc.inc}
{$include i_variables.inc}
{$include i_macro.inc}
{$include i_text.inc}
{$include i_opt_it.inc}
{$include i_tmpl_dlg.inc}
{$include i_expkey.inc}

function WATReplaceText(wParam:WPARAM;lParam:LPARAM):int_ptr;cdecl;
begin
  if (lParam<>0) and (pWideChar(lParam)^<>#0) then
  begin
    if isVarsInstalled then
      result:=int_ptr(ParseVarString(pWideChar(lParam)))
    else
      result:=int_ptr(ReplaceAll(pWideChar(lParam)));
    if (result<>0) and (pWideChar(result)^=#0) then
      mFreeMem(PWideChar(result));
  end
  else
    result:=0;
end;

// ------------ base interface functions -------------

function AddOptionsPage(var tmpl:pAnsiChar;var proc:pointer;var name:PAnsiChar):integer;
begin
  tmpl:='FORMAT';
  proc:=@DlgProcOptions;
  name:='Templates';
  result:=0;
end;

var
  hEXP,
  hMacro,
  hReplace:THANDLE;

function InitProc(aGetStatus:boolean=false):integer;
begin
  result:=1;
  hEXP    :=PluginLink^.CreateServiceFunction(MS_WAT_EXPORT     ,@ExportProc);
  hReplace:=PluginLink^.CreateServiceFunction(MS_WAT_REPLACETEXT,@WATReplaceText);
  hMacro  :=PluginLink^.CreateServiceFunction(MS_WAT_MACROHELP  ,@WATMacroHelp);
  loadopt;
  LoadAliases;
  RegisterVariables;
  reginshotkey;
end;

procedure DeInitProc(aSetDisable:boolean);
begin
  PluginLink^.DestroyServiceFunction(hReplace);
  PluginLink^.DestroyServiceFunction(hEXP);
  PluginLink^.DestroyServiceFunction(hMacro);
  FreeAliases;
  FreeOpt;
end;

var
  Tmpl:twModule;

procedure Init;
begin
  Tmpl.Next      :=ModuleLink;
  Tmpl.Init      :=@InitProc;
  Tmpl.DeInit    :=@DeInitProc;
  Tmpl.AddOption :=@AddOptionsPage;
  Tmpl.ModuleName:=nil;
  ModuleLink    :=@Tmpl;
end;

begin
  Init;
end.
