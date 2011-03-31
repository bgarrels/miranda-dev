unit UAthena;
{
  Athena: cryptor module for Miranda SecuredMMAP Database driver
  Copyright 2007-2008 Klyde

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
}


interface
uses
   SysUtils, md5_unit, windows;
Type
   Arr  = array of byte;
   PArr = ^Arr;
   
   Function MD5_Mod(s: string; block_count: byte): string;
   Procedure MakeKey(key: PArr; len: word; pwd: string);
   Procedure EncryptData(key: PArr; data: PByte; size: LongWord);
   Procedure DecryptData(key: PArr; data: PByte; size: LongWord);

implementation
//==============================================================================
Function str_back(s: string): string;
Var
   i: integer;
Begin
   result := '';
   for i := Length(s) downto 1 do result := result + s[i];
end;
//==============================================================================
Function MD5_Mod(s: string; block_count: byte): string;
Var
   s1, s2, sb : String;
   k          : word;
Begin
   sb := str_back(s);
   s2 := '';
   For k := 1 to block_count do
   Begin
      s1 := md5(s + sb);
      s2 := str_back(s2 + md5(s1+sb+s2));
   End;
   result := s2;
end;
//==============================================================================
Procedure MakeKey(key: PArr; len: word; pwd: string);
Var
   s : string;
   i : word;
Begin
   if len > 64 then Len := ((Len div 16) + 1)*16 else Len := 64;
   SetLength(key^, Len);
   s := MD5_mod(pwd, len div 16);
   for i := 1 to length(s) div 2 do key^[i-1] := strtoint('$' + copy(s, i*2 - 1, 2));
end;
//==============================================================================
Procedure GetNENum(key: arr; var n1, n2: LongWord);
Var
   i: LongWord;
Begin
   n1 := 0;
   n2 := 0;
   for i := 0 to Length(key) - 1 do
   Begin
      n1 := n1 + key[i] + (i + 1)*(n1+1);
      n2 := n2 + key[i] - (i + 1)*(n2+1);
   end;
   n1 := n1*2 + 1;
   n2 := n2*2 + 3;
end;

//==============================================================================
Procedure SimGamm(key: PArr; data: PByte; size: LongWord);
Var
   kg : Arr;
   i, n1, n2 : LongWord;
   lk, k1, k2 : word;
Begin
   lk := Length(key^);
   SetLength(kg, lk);
   for i := 0 to lk - 1 do kg[i] := key^[i];
   GetNENum(kg, n1, n2);
   For i := 1 to size - 1 do
   Begin
      if (i mod lk) = 0 then GetNENum(kg, n1, n2);
      k1 := (i+n1+7)*n2 mod lk;
      k2 := (i+n2+3)*n1 mod lk;

      PByte(LongWord(data)+i)^ := PByte(LongWord(data)+i)^ xor kg[k1] xor kg[k2];

      kg[k1] := kg[k1]*k1 + kg[k2] + i*k2;
      kg[k2] := kg[k2]*k2 + kg[k1] + i*k1;
   end;
end;
//==============================================================================
Procedure Left(key: PArr; data: PByte; size: LongWord);
Var
   k : Arr;
   i, n1, n2 : LongWord;
   lk, k1, k2 : word;

Begin
   lk := Length(key^);

   SetLength(k, lk);
   for i := 0 to lk - 1 do k[i] := key^[i];
   GetNENum(k, n1, n2);
   //---------------------------------------------------------------------------
   k1 := (n2 + lk)*n1 mod lk;
   k2 := (n1 + lk)*n2 mod lk;
   data^ := data^ xor k[k1] xor k[k2];

   //---------------------------------------------------------------------------
   For i := 1 to size - 1 do
   Begin
      k1 := (i+n1)*n2 mod lk;
      k2 := (i+n2)*n1 mod lk;

      PByte(LongWord(data)+i)^ := PByte(LongWord(data)+i)^ xor ((PByte(LongWord(data)+i-1)^ xor k[k1]) xor k[k2]);
   end;
end;
//==============================================================================
Procedure Right(key: PArr; data: PByte; size: LongWord);
Var
   k : Arr;
   i, n1, n2 : LongWord;
   lk, k1, k2 : word;
Begin
   lk := Length(key^);
   SetLength(k, lk);
   for i := 0 to lk - 1 do k[i] := key^[i];
   GetNENum(k, n1, n2);
   //---------------------------------------------------------------------------
   For i := size - 1 downto 1 do
   Begin
      k1 := (i+n1)*n2 mod lk;
      k2 := (i+n2)*n1 mod lk;
      PByte(LongWord(data) + i)^ := PByte(LongWord(data)+i)^ xor ((PByte(LongWord(data) + i - 1)^ xor k[k1]) xor k[k2]);
   end;
   //---------------------------------------------------------------------------
   k1 := (n2 + lk)*n1 mod lk;
   k2 := (n1 + lk)*n2 mod lk;
   data^ := data^ xor k[k1] xor k[k2];
end;
//==============================================================================
Procedure EncryptData(key: PArr; data: PByte; size: LongWord);
Begin
   Left(key, data, size);
   SimGamm(key, data, size);
end;
//==============================================================================
Procedure DecryptData(key: PArr; data: PByte; size: LongWord);
Begin
   SimGamm(key, data, size);
   Right(key, data, size);
end;
//==============================================================================
end.

