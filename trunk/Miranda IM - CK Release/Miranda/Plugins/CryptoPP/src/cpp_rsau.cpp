/*
Crypto++ plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Author
Copyright (C) 2006-2009 Baloo

Copyright 2000-2012 Miranda IM project,
all portions of this codebase are copyrighted to the people
listed in contributors.txt.

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

===============================================================================

File name      : $HeadURL: 
Revision       : $Revision: 
Last change on : $Date: 
Last change by : $Author:
$Id$		   : $Id$:

===============================================================================
*/

#include "commonheaders.h"

const string padding = "PADDING!PADDING!PADDING!PADDING!"; // 256 bit


string pad256(string& str) {
	string out = str;
	int pad = out.length() % 32;
	if( pad ) { // надо выровнять на 256 бит
	    if( pad>3 ) {
		out += tlv(0,padding.substr(0,32-3-pad));
	    }
	    else {
	    	if( pad==3 ) out += string("\0\0\0");
	    	else
	    	if( pad==2 ) out += string("\0\0");
	    	else         out += string("\0");
	    }
	}
	return out;
}


string& add_delim(string& str, const string& del, int len) {
	string val;
	for( u_int i=0; i<str.length(); i+=len ) {
		val += str.substr(i,len) + del;
	}
	str = val;
	return str;
}


string& del_delim(string& str, const string& del) {
	string val;
	for( u_int i=0; i<str.length(); i++ ) {
		if( del.find(str[i]) == string::npos )
			val += str[i];
	}
	str = val;
	return str;
}


string tlv(int t, const string& v) {
	string b;
	t |= v.length()<<8;
	b.assign((const char*)&t,3);
	b += v;
	return b;
}


string tlv(int t, const char* v) {
	return tlv(t, string(v));
}


string tlv(int t, int v) {
	string s;
	u_int l=(v<=0xFF)?1:((v<=0xFFFF)?2:((v<=0xFFFFFF)?3:4));
	s.assign((char*)&v,l);
	return tlv(t,s);
}


string& un_tlv(string& b, int& t, string& v) {
	string r; v = r;
	if( b.length() > 3 ) {
		t = 0;
		b.copy((char*)&t,3);
		u_int l = t>>8;
		t &= 0xFF;
		if( b.length() >= 3+l ) {
		  v = b.substr(3,l);
		  r = b.substr(3+l);
		}
	}
	if( !v.length() ) {
		t = -1;
	}
	b = r;
	return b;
}


string& un_tlv(string& b, int& t, int& v) {
	string s;
	un_tlv(b,t,s);
	v = 0;
	s.copy((char*)&v,s.length());
	return b;
}


int str2int(string& s) {
	int v = 0;
	if( s.length()<=sizeof(int) )
		s.copy((char*)&v,s.length());
	return v;
}


string hash(string& b)
{
	return hash((PBYTE)b.data(),b.length());
}


string hash(PBYTE b, int l)
{
	BYTE h[RSA_KEYSIZE];
	RSA_CalculateDigest(h, b, l);
	string s; s.assign((char*)&h,RSA_KEYSIZE);
	return s;
}


string hash128(string& b)
{
	return hash128((PBYTE)b.data(),b.length());
}


string hash128(LPSTR b)
{
	return hash128((PBYTE)b,strlen(b));
}


string hash128(PBYTE b, int l)
{
	BYTE h[RIPEMD128::DIGESTSIZE];
	RIPEMD128().CalculateDigest(h, b, l);
	string s; s.assign((char*)&h,sizeof(h));
	return s;
}


string hash256(string& b)
{
	return hash256((PBYTE)b.data(),b.length());
}


string hash256(LPSTR b)
{
	return hash256((PBYTE)b,strlen(b));
}


string hash256(PBYTE b, int l)
{
//	BYTE h[SHA256::DIGESTSIZE];
//	SHA256().CalculateDigest(h, b, l);
	BYTE h[RIPEMD256::DIGESTSIZE];
	RIPEMD256().CalculateDigest(h, b, l);
	string s; s.assign((char*)&h,sizeof(h));
	return s;
}


Integer BinaryToInteger(const string& data)
{
  StringSource ss(data, true, NULL);
  SecByteBlock result(ss.MaxRetrievable());
  ss.Get(result, result.size());
  return Integer(result, result.size());
}


string IntegerToBinary(const Integer& value)
{
  SecByteBlock sbb(value.MinEncodedSize());
  value.Encode(sbb, sbb.size());
  string data;
  StringSource(sbb, sbb.size(), true, new StringSink(data)); 
  return data;
}

/*
RandomPool& GlobalRNG()
{
	static RandomPool randomPool;
	return randomPool;
}
*/

AutoSeededRandomPool& GlobalRNG()
{
	static AutoSeededRandomPool randomPool;
	return randomPool;
}


void GenerateRSAKey(unsigned int keyLength, string& privKey, string& pubKey)
{
	RSAES_PKCS1v15_Decryptor priv(GlobalRNG(), keyLength);
	StringSink privFile(privKey);
	priv.DEREncode(privFile);
	privFile.MessageEnd();

	RSAES_PKCS1v15_Encryptor pub(priv);
	StringSink pubFile(pubKey);
	pub.DEREncode(pubFile);
	pubFile.MessageEnd();
}


string RSAEncryptString(const RSA::PublicKey& pubkey, const string& plaintext)
{
	RSAES_PKCS1v15_Encryptor pub(pubkey);

	string result;
	StringSource(plaintext, true, new PK_EncryptorFilter(GlobalRNG(), pub, new StringSink(result)));
	return result;
}


string RSADecryptString(const RSA::PrivateKey& privkey, const string& ciphertext)
{
	RSAES_PKCS1v15_Decryptor priv(privkey);

	string result;
	try {
		StringSource(ciphertext, true, new PK_DecryptorFilter(GlobalRNG(), priv, new StringSink(result)));
	}
	catch (...)	{
		;
	}
	return result;
}


string RSASignString(const RSA::PrivateKey& privkey, const string& plaintext)
{
	RSASSA_PKCS1v15_SHA_Signer priv(privkey);

	string result;
	try {
		StringSource(plaintext, true, new SignerFilter(GlobalRNG(), priv, new StringSink(result)));
	}
	catch (...)	{
		;
	}
	return result;
}


BOOL RSAVerifyString(const RSA::PublicKey& pubkey, const string& plaintext, const string& sig)
{
  RSASSA_PKCS1v15_SHA_Verifier ver(pubkey);

  return ver.VerifyMessage((PBYTE)plaintext.data(), plaintext.length(), (PBYTE)sig.data(), sig.length());
}


// EOF
