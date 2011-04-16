@echo off
..\delphi\brcc32.exe res\watrack.rc         -fores\watrack.res
..\delphi\brcc32.exe lastfm\lastfm.rc       -folastfm\lastfm.res
..\delphi\brcc32.exe players\mradio.rc      -foplayers\mradio.res
..\delphi\brcc32.exe kolframe\frm.rc        -fokolframe\frm.res
..\delphi\brcc32.exe popup\popup.rc         -fopopup\popup.res
..\delphi\brcc32.exe proto\proto.rc         -foproto\proto.res
..\delphi\brcc32.exe stat\stat.rc           -fostat\stat.res
..\delphi\brcc32.exe status\status.rc       -fostatus\status.res
..\delphi\brcc32.exe templates\templates.rc -fotemplates\templates.res
if /i '%1' == 'fpc' (
  ..\FPC\bin\fpc.exe watrack.dpr %2 %3 %4 %5 %6 %7 %8 %9
) else if /i '%1' == 'fpc64' (
  ..\FPC\bin64\ppcrossx64.exe watrack.dpr %2 %3 %4 %5 %6 %7 %8 %9
) else (
  ..\delphi\dcc32 watrack.dpr %2 %3 %4 %5 %6 %7 %8 %9
)
