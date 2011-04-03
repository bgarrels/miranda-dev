@echo off
if /i '%1' == 'fpc' ..\..\Delphi\Awkward\FPC\bin\fpc.exe mRadio.dpr %2 %3 %4 %5 %6 %7 %8 %9
if /i '%1' == 'fpc64' ..\..\Delphi\Awkward\FPC\bin64\ppcrossx64.exe mRadio.dpr %2 %3 %4 %5 %6 %7 %8 %9
if /i '%1' == '' ..\..\Delphi\Awkward\delphi\dcc32 mRadio.dpr  %2 %3 %4 %5 %6 %7 %8 %9
