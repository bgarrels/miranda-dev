/*

Copyright (c) 2010 Brook Miles

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/

#include "stdafx.h"

using namespace std;

// ConsumerKey and ConsumerSecret uniquely identify your application
// TODO Go to http://twitter.com/oauth_clients/new and register your own Client application.
// TODO Then replace the ConsumerKey and ConsumerSecret in the values below.

wstring ConsumerKey = L"T6XLGzrkfsJAgU59dbIjSA";
wstring ConsumerSecret = L"xsvm2NAksjsJGw63RMWAtec3Lz5uiBusfVt48gbdKLg";

const wstring HostName = L"http://twitter.com";
const wstring AccessUrl = L"http://twitter.com/oauth/access_token";
const wstring AuthorizeUrl = L"http://twitter.com/oauth/authorize?oauth_token=%s";
const wstring RequestUrl = L"http://twitter.com/oauth/request_token?some_other_parameter=hello&another_one=goodbye#meep"; // threw in some parameters for fun, and to test UrlGetQuery
const wstring UserTimelineUrl = L"http://twitter.com/statuses/user_timeline.xml";
const wstring UserStatusUpdateUrl = L"http://twitter.com/statuses/update.xml";

const wstring AuthFileName = L"tc2_saved.txt";

typedef std::map<wstring, wstring> HTTPParameters;

wstring UrlEncode( const wstring& url );

wstring UrlGetQuery( const wstring& url ) 
{
	wstring query;
	URL_COMPONENTS components = {sizeof(URL_COMPONENTS)};

	wchar_t buf[1024*4] = {};

	components.lpszExtraInfo = buf;
	components.dwExtraInfoLength = SIZEOF(buf);

	BOOL crackUrlOk = InternetCrackUrl(url.c_str(), url.size(), 0, &components);
	_ASSERTE(crackUrlOk);
	if(crackUrlOk)
	{
		query = components.lpszExtraInfo;

		wstring::size_type q = query.find_first_of(L'?');
		if(q != wstring::npos)
		{
			query = query.substr(q + 1);
		}

		wstring::size_type h = query.find_first_of(L'#');
		if(h != wstring::npos)
		{
			query = query.substr(0, h);
		}
	}
	return query;
}

HTTPParameters ParseQueryString( const wstring& url ) 
{
	HTTPParameters ret;

	vector<wstring> queryParams;
	Split(url, queryParams, L'&', false);

	for(size_t i = 0; i < queryParams.size(); ++i)
	{
		vector<wstring> paramElements;
		Split(queryParams[i], paramElements, L'=', true);
		_ASSERTE(paramElements.size() == 2);
		if(paramElements.size() == 2)
		{
			ret[paramElements[0]] = paramElements[1];
		}
	}
	return ret;
}

// parameters must already be URL encoded before calling BuildQueryString
wstring BuildQueryString( const HTTPParameters &parameters ) 
{
	wstring query;

	for(HTTPParameters::const_iterator it = parameters.begin(); 
		it != parameters.end(); 
		++it)
	{
		_TRACE("%s = %s", it->first.c_str(), it->second.c_str());

		if(it != parameters.begin())
		{
			query += L"&";
		}

		wstring pair;
		pair += it->first + L"=" + it->second + L"";
		query += pair;
	}	
	return query;
}

wstring OAuthBuildHeader( const HTTPParameters &parameters ) 
{
	wstring query;

	for(HTTPParameters::const_iterator it = parameters.begin(); 
		it != parameters.end(); 
		++it)
	{
		_TRACE("%s = %s", it->first.c_str(), it->second.c_str());

		if(it != parameters.begin())
		{
			query += L",";
		}

		wstring pair;
		pair += it->first + L"=\"" + it->second + L"\"";
		query += pair;
	}	
	return query;
}

wstring OAuthCreateNonce() 
{
	wchar_t ALPHANUMERIC[] = L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	wstring nonce;

	for(int i = 0; i <= 16; ++i)
	{
		nonce += ALPHANUMERIC[rand() % (SIZEOF(ALPHANUMERIC) - 1)]; // don't count null terminator in array
	}
	return nonce;
}

wstring OAuthCreateTimestamp() 
{
	__time64_t utcNow;
	__time64_t ret = _time64(&utcNow);
	_ASSERTE(ret != -1);

	wchar_t buf[100] = {};
	swprintf_s(buf, SIZEOF(buf), L"%I64u", utcNow);

	return buf;
}

string HMACSHA1( const string& keyBytes, const string& data ) 
{
	// based on http://msdn.microsoft.com/en-us/library/aa382379%28v=VS.85%29.aspx

	string hash;

	//--------------------------------------------------------------------
	// Declare variables.
	//
	// hProv:           Handle to a cryptographic service provider (CSP). 
	//                  This example retrieves the default provider for  
	//                  the PROV_RSA_FULL provider type.  
	// hHash:           Handle to the hash object needed to create a hash.
	// hKey:            Handle to a symmetric key. This example creates a 
	//                  key for the RC4 algorithm.
	// hHmacHash:       Handle to an HMAC hash.
	// pbHash:          Pointer to the hash.
	// dwDataLen:       Length, in bytes, of the hash.
	// Data1:           Password string used to create a symmetric key.
	// Data2:           Message string to be hashed.
	// HmacInfo:        Instance of an HMAC_INFO structure that contains 
	//                  information about the HMAC hash.
	// 
	HCRYPTPROV  hProv       = NULL;
	HCRYPTHASH  hHash       = NULL;
	HCRYPTKEY   hKey        = NULL;
	HCRYPTHASH  hHmacHash   = NULL;
	PBYTE       pbHash      = NULL;
	DWORD       dwDataLen   = 0;
	//BYTE        Data1[]     = {0x70,0x61,0x73,0x73,0x77,0x6F,0x72,0x64};
	//BYTE        Data2[]     = {0x6D,0x65,0x73,0x73,0x61,0x67,0x65};
	HMAC_INFO   HmacInfo;

	//--------------------------------------------------------------------
	// Zero the HMAC_INFO structure and use the SHA1 algorithm for
	// hashing.

	ZeroMemory(&HmacInfo, sizeof(HmacInfo));
	HmacInfo.HashAlgid = CALG_SHA1;

	//--------------------------------------------------------------------
	// Acquire a handle to the default RSA cryptographic service provider.

	if (!CryptAcquireContext(
		&hProv,                   // handle of the CSP
		NULL,                     // key container name
		NULL,                     // CSP name
		PROV_RSA_FULL,            // provider type
		CRYPT_VERIFYCONTEXT))     // no key access is requested
	{
		_TRACE(" Error in AcquireContext 0x%08x \n",
			GetLastError());
		goto ErrorExit;
	}

	//--------------------------------------------------------------------
	// Derive a symmetric key from a hash object by performing the
	// following steps:
	//    1. Call CryptCreateHash to retrieve a handle to a hash object.
	//    2. Call CryptHashData to add a text string (password) to the 
	//       hash object.
	//    3. Call CryptDeriveKey to create the symmetric key from the
	//       hashed password derived in step 2.
	// You will use the key later to create an HMAC hash object. 

	if (!CryptCreateHash(
		hProv,                    // handle of the CSP
		CALG_SHA1,                // hash algorithm to use
		0,                        // hash key
		0,                        // reserved
		&hHash))                  // address of hash object handle
	{
		_TRACE("Error in CryptCreateHash 0x%08x \n",
			GetLastError());
		goto ErrorExit;
	}

	if (!CryptHashData(
		hHash,                    // handle of the hash object
		(BYTE*)keyBytes.c_str(),                    // password to hash
		keyBytes.size(),            // number of bytes of data to add
		0))                       // flags
	{
		_TRACE("Error in CryptHashData 0x%08x \n", 
			GetLastError());
		goto ErrorExit;
	}

	// key creation based on 
	// http://mirror.leaseweb.com/NetBSD/NetBSD-release-5-0/src/dist/wpa/src/crypto/crypto_cryptoapi.c
	struct {
		BLOBHEADER hdr;
		DWORD len;
		BYTE key[1024]; // TODO might want to dynamically allocate this, Should Be Fine though
	} key_blob;

	key_blob.hdr.bType = PLAINTEXTKEYBLOB;
	key_blob.hdr.bVersion = CUR_BLOB_VERSION;
	key_blob.hdr.reserved = 0;
	/*
	* Note: RC2 is not really used, but that can be used to
	* import HMAC keys of up to 16 byte long.
	* CRYPT_IPSEC_HMAC_KEY flag for CryptImportKey() is needed to
	* be able to import longer keys (HMAC-SHA1 uses 20-byte key).
	*/
	key_blob.hdr.aiKeyAlg = CALG_RC2;
	key_blob.len = keyBytes.size();
	ZeroMemory(key_blob.key, sizeof(key_blob.key));

	_ASSERTE(keyBytes.size() <= SIZEOF(key_blob.key));
	CopyMemory(key_blob.key, keyBytes.c_str(), min(keyBytes.size(), SIZEOF(key_blob.key)));

	if (!CryptImportKey(
		hProv, 
		(BYTE *)&key_blob,
		sizeof(key_blob), 
		0, 
		CRYPT_IPSEC_HMAC_KEY,
		&hKey)) 
	{
		_TRACE("Error in CryptImportKey 0x%08x \n", GetLastError());
		goto ErrorExit;
	}

	//--------------------------------------------------------------------
	// Create an HMAC by performing the following steps:
	//    1. Call CryptCreateHash to create a hash object and retrieve 
	//       a handle to it.
	//    2. Call CryptSetHashParam to set the instance of the HMAC_INFO 
	//       structure into the hash object.
	//    3. Call CryptHashData to compute a hash of the message.
	//    4. Call CryptGetHashParam to retrieve the size, in bytes, of
	//       the hash.
	//    5. Call malloc to allocate memory for the hash.
	//    6. Call CryptGetHashParam again to retrieve the HMAC hash.

	if (!CryptCreateHash(
		hProv,                    // handle of the CSP.
		CALG_HMAC,                // HMAC hash algorithm ID
		hKey,                     // key for the hash (see above)
		0,                        // reserved
		&hHmacHash))              // address of the hash handle
	{
		_TRACE("Error in CryptCreateHash 0x%08x \n", 
			GetLastError());
		goto ErrorExit;
	}

	if (!CryptSetHashParam(
		hHmacHash,                // handle of the HMAC hash object
		HP_HMAC_INFO,             // setting an HMAC_INFO object
		(BYTE*)&HmacInfo,         // the HMAC_INFO object
		0))                       // reserved
	{
		_TRACE("Error in CryptSetHashParam 0x%08x \n", 
			GetLastError());
		goto ErrorExit;
	}

	if (!CryptHashData(
		hHmacHash,                // handle of the HMAC hash object
		(BYTE*)data.c_str(),                    // message to hash
		data.size(),            // number of bytes of data to add
		0))                       // flags
	{
		_TRACE("Error in CryptHashData 0x%08x \n", 
			GetLastError());
		goto ErrorExit;
	}

	//--------------------------------------------------------------------
	// Call CryptGetHashParam twice. Call it the first time to retrieve
	// the size, in bytes, of the hash. Allocate memory. Then call 
	// CryptGetHashParam again to retrieve the hash value.

	if (!CryptGetHashParam(
		hHmacHash,                // handle of the HMAC hash object
		HP_HASHVAL,               // query on the hash value
		NULL,                     // filled on second call
		&dwDataLen,               // length, in bytes, of the hash
		0))
	{
		_TRACE("Error in CryptGetHashParam 0x%08x \n", 
			GetLastError());
		goto ErrorExit;
	}

	pbHash = (BYTE*)malloc(dwDataLen);
	if(NULL == pbHash) 
	{
		_TRACE("unable to allocate memory\n");
		goto ErrorExit;
	}

	if (!CryptGetHashParam(
		hHmacHash,                 // handle of the HMAC hash object
		HP_HASHVAL,                // query on the hash value
		pbHash,                    // pointer to the HMAC hash value
		&dwDataLen,                // length, in bytes, of the hash
		0))
	{
		_TRACE("Error in CryptGetHashParam 0x%08x \n", GetLastError());
		goto ErrorExit;
	}

	for(DWORD i = 0 ; i < dwDataLen ; i++) 
	{
		hash.push_back((char)pbHash[i]);
	}

	// Free resources.
	// lol goto
ErrorExit:
	if(hHmacHash)
		CryptDestroyHash(hHmacHash);
	if(hKey)
		CryptDestroyKey(hKey);
	if(hHash)
		CryptDestroyHash(hHash);    
	if(hProv)
		CryptReleaseContext(hProv, 0);
	if(pbHash)
		free(pbHash);

	return hash;
}

wstring Base64String( const string& hash ) 
{
	Base64Coder coder;
	coder.Encode((BYTE*)hash.c_str(), hash.size());
	wstring encoded = UTF8ToWide(coder.EncodedMessage());
	return encoded;
}

// char2hex and urlencode from http://www.zedwood.com/article/111/cpp-urlencode-function
// modified according to http://oauth.net/core/1.0a/#encoding_parameters
//
//5.1.  Parameter Encoding
//
//All parameter names and values are escaped using the [RFC3986]  
//percent-encoding (%xx) mechanism. Characters not in the unreserved character set 
//MUST be encoded. Characters in the unreserved character set MUST NOT be encoded. 
//Hexadecimal characters in encodings MUST be upper case. 
//Text names and values MUST be encoded as UTF-8 
// octets before percent-encoding them per [RFC3629].
//
//  unreserved = ALPHA, DIGIT, '-', '.', '_', '~'

string char2hex( char dec )
{
	char dig1 = (dec&0xF0)>>4;
	char dig2 = (dec&0x0F);
	if ( 0<= dig1 && dig1<= 9) dig1+=48;    //0,48 in ascii
	if (10<= dig1 && dig1<=15) dig1+=65-10; //A,65 in ascii
	if ( 0<= dig2 && dig2<= 9) dig2+=48;
	if (10<= dig2 && dig2<=15) dig2+=65-10;

	string r;
	r.append( &dig1, 1);
	r.append( &dig2, 1);
	return r;
}

string urlencode(const string &c)
{

	string escaped;
	int max = c.length();
	for(int i=0; i<max; i++)
	{
		if ( (48 <= c[i] && c[i] <= 57) ||//0-9
			(65 <= c[i] && c[i] <= 90) ||//ABC...XYZ
			(97 <= c[i] && c[i] <= 122) || //abc...xyz
			(c[i]=='~' || c[i]=='-' || c[i]=='_' || c[i]=='.')
			)
		{
			escaped.append( &c[i], 1);
		}
		else
		{
			escaped.append("%");
			escaped.append( char2hex(c[i]) );//converts char 255 to string "FF"
		}
	}
	return escaped;
}


wstring UrlEncode( const wstring& url ) 
{
	// multiple encodings r sux
	return UTF8ToWide(urlencode(WideToUTF8(url)));
}

wstring OAuthCreateSignature( const wstring& signatureBase, const wstring& consumerSecret, const wstring& requestTokenSecret ) 
{
	// URL encode key elements
	wstring escapedConsumerSecret = UrlEncode(consumerSecret);
	wstring escapedTokenSecret = UrlEncode(requestTokenSecret);

	wstring key = escapedConsumerSecret + L"&" + escapedTokenSecret;
	string keyBytes = WideToUTF8(key);

	string data = WideToUTF8(signatureBase);
	string hash = HMACSHA1(keyBytes, data);
	wstring signature = Base64String(hash);

	// URL encode the returned signature
	signature = UrlEncode(signature);
	return signature;
}


wstring OAuthConcatenateRequestElements( const wstring& httpMethod, wstring url, const wstring& parameters ) 
{
	wstring escapedUrl = UrlEncode(url);
	wstring escapedParameters = UrlEncode(parameters);

	wstring ret = httpMethod + L"&" + escapedUrl + L"&" + escapedParameters;
	return ret;
}

wstring OAuthNormalizeRequestParameters( const HTTPParameters& requestParameters ) 
{
	list<wstring> sorted;
	for(HTTPParameters::const_iterator it = requestParameters.begin(); 
		it != requestParameters.end(); 
		++it)
	{
		wstring param = it->first + L"=" + it->second;
		sorted.push_back(param);
	}
	sorted.sort();

	wstring params;
	for(list<wstring>::iterator it = sorted.begin(); it != sorted.end(); ++it)
	{
		if(params.size() > 0)
		{
			params += L"&";
		}
		params += *it;
	}

	return params;
}

wstring OAuthNormalizeUrl( const wstring& url ) 
{
	wchar_t scheme[1024*4] = {};
	wchar_t host[1024*4] = {};
	wchar_t path[1024*4] = {};

	URL_COMPONENTS components = { sizeof(URL_COMPONENTS) };

	components.lpszScheme = scheme;
	components.dwSchemeLength = SIZEOF(scheme);

	components.lpszHostName = host;
	components.dwHostNameLength = SIZEOF(host);

	components.lpszUrlPath = path;
	components.dwUrlPathLength = SIZEOF(path);

	wstring normalUrl = url;

	BOOL crackUrlOk = InternetCrackUrl(url.c_str(), url.size(), 0, &components);
	_ASSERTE(crackUrlOk);
	if(crackUrlOk)
	{
		wchar_t port[10] = {};

		// The port number must only be included if it is non-standard
		if((Compare(scheme, L"http", false) && components.nPort != 80) || 
			(Compare(scheme, L"https", false) && components.nPort != 443))
		{
			swprintf_s(port, SIZEOF(port), L":%u", components.nPort);
		}

		// InternetCrackUrl includes ? and # elements in the path, 
		// which we need to strip off
		wstring pathOnly = path;
		wstring::size_type q = pathOnly.find_first_of(L"#?");
		if(q != wstring::npos)
		{
			pathOnly = pathOnly.substr(0, q);
		}

		normalUrl = wstring(scheme) + L"://" + host + port + pathOnly;
	}
	return normalUrl;
}

HTTPParameters BuildSignedOAuthParameters( const HTTPParameters& requestParameters, 
								   const wstring& url, 
								   const wstring& httpMethod, 
								   const HTTPParameters* postParameters, 
								   const wstring& consumerKey, 
								   const wstring& consumerSecret,
								   const wstring& requestToken = L"", 
								   const wstring& requestTokenSecret = L"", 
								   const wstring& pin = L"" )
{
	wstring timestamp = OAuthCreateTimestamp();
	wstring nonce = OAuthCreateNonce();

	// create oauth requestParameters
	HTTPParameters oauthParameters;

	oauthParameters[L"oauth_timestamp"] = timestamp;
	oauthParameters[L"oauth_nonce"] = nonce;
	oauthParameters[L"oauth_version"] = L"1.0";
	oauthParameters[L"oauth_signature_method"] = L"HMAC-SHA1";
	oauthParameters[L"oauth_consumer_key"] = consumerKey;

	// add the request token if found
	if (!requestToken.empty())
	{
		oauthParameters[L"oauth_token"] = requestToken;
	}

	// add the authorization pin if found
	if (!pin.empty())
	{
		oauthParameters[L"oauth_verifier"] = pin;
	}

	// create a parameter list containing both oauth and original parameters
	// this will be used to create the parameter signature
	HTTPParameters allParameters = requestParameters;
	if(Compare(httpMethod, L"POST", false) && postParameters)
	{
		allParameters.insert(postParameters->begin(), postParameters->end());
	}
	allParameters.insert(oauthParameters.begin(), oauthParameters.end());

	// prepare a signature base, a carefully formatted string containing 
	// all of the necessary information needed to generate a valid signature
	wstring normalUrl = OAuthNormalizeUrl(url);
	wstring normalizedParameters = OAuthNormalizeRequestParameters(allParameters);
	wstring signatureBase = OAuthConcatenateRequestElements(httpMethod, normalUrl, normalizedParameters);

	// obtain a signature and add it to header requestParameters
	wstring signature = OAuthCreateSignature(signatureBase, consumerSecret, requestTokenSecret);
	oauthParameters[L"oauth_signature"] = signature;

	return oauthParameters;
}

wstring OAuthWebRequestSignedSubmit( 
	const HTTPParameters& oauthParameters, 
	const wstring& url,
	const wstring& httpMethod, 
	const HTTPParameters* postParameters
	) 
{
	_TRACE("OAuthWebRequestSignedSubmit(%s)", url.c_str());

	wstring oauthHeader = L"Authorization: OAuth ";
	oauthHeader += OAuthBuildHeader(oauthParameters);
	oauthHeader += L"\r\n";

	_TRACE("%s", oauthHeader.c_str());

	wchar_t host[1024*4] = {};
	wchar_t path[1024*4] = {};

	URL_COMPONENTS components = { sizeof(URL_COMPONENTS) };

	components.lpszHostName = host;
	components.dwHostNameLength = SIZEOF(host);

	components.lpszUrlPath = path;
	components.dwUrlPathLength = SIZEOF(path);

	wstring normalUrl = url;

	BOOL crackUrlOk = InternetCrackUrl(url.c_str(), url.size(), 0, &components);
	_ASSERTE(crackUrlOk);

	wstring result;

	// TODO you'd probably want to InternetOpen only once at app initialization
	HINTERNET hINet = InternetOpen(L"tc2/1.0", 
		INTERNET_OPEN_TYPE_PRECONFIG, 
		NULL, 
		NULL, 
		0 );
	_ASSERTE( hINet != NULL );
	if ( hINet != NULL )
	{
		// TODO add support for HTTPS requests
		HINTERNET hConnection = InternetConnect( 
			hINet, 
			host, 
			components.nPort, 
			NULL, 
			NULL, 
			INTERNET_SERVICE_HTTP, 
			0, 0 );
		_ASSERTE(hConnection != NULL);
		if ( hConnection != NULL)
		{
			HINTERNET hData = HttpOpenRequest( hConnection, 
				httpMethod.c_str(), 
				path, 
				NULL, 
				NULL, 
				NULL, 
				INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_NO_AUTH | INTERNET_FLAG_RELOAD, 
				0 );
			_ASSERTE(hData != NULL);
			if ( hData != NULL )
			{
				BOOL oauthHeaderOk = HttpAddRequestHeaders(hData, 
					oauthHeader.c_str(), 
					oauthHeader.size(), 
					0);
				_ASSERTE(oauthHeaderOk);

				// NOTE POST requests are supported, but the MIME type is hardcoded to application/x-www-form-urlencoded (aka. form data)
				// TODO implement support for posting image, raw or other data types
				string postDataUTF8;
				if(Compare(httpMethod, L"POST", false) && postParameters)
				{
					wstring contentHeader = L"Content-Type: application/x-www-form-urlencoded\r\n";
					BOOL contentHeaderOk = HttpAddRequestHeaders(hData, 
						contentHeader.c_str(), 
						contentHeader.size(), 
						0);
					_ASSERTE(contentHeaderOk);

					postDataUTF8 = WideToUTF8(BuildQueryString(*postParameters));
					_TRACE("POST DATA: %S", postDataUTF8.c_str());
				}

				BOOL sendOk = HttpSendRequest( hData, NULL, 0, (LPVOID)(postDataUTF8.size() > 0 ? postDataUTF8.c_str() : NULL), postDataUTF8.size());
				_ASSERTE(sendOk);

				// TODO dynamically allocate return buffer
				BYTE buffer[1024*32] = {};
				DWORD dwRead = 0;
				while( 
					InternetReadFile( hData, buffer, SIZEOF(buffer) - 1, &dwRead ) &&
					dwRead > 0
					)
				{
					buffer[dwRead] = 0;
					result += UTF8ToWide((char*)buffer);
				}

				_TRACE("%s", result.c_str());

				InternetCloseHandle(hData);
			}
			InternetCloseHandle(hConnection);
		}
		InternetCloseHandle(hINet);
	}

	return result;
}

// OAuthWebRequest used for all OAuth related queries
//
// consumerKey and consumerSecret - must be provided for every call, they identify the application
// oauthToken and oauthTokenSecret - need to be provided for every call, except for the first token request before authorizing
// pin - only used during authorization, when the user enters the PIN they received from the twitter website
wstring OAuthWebRequestSubmit( 
    const wstring& url, 
	const wstring& httpMethod, 
	const HTTPParameters* postParameters,
    const wstring& consumerKey, 
    const wstring& consumerSecret, 
    const wstring& oauthToken = L"", 
    const wstring& oauthTokenSecret = L"", 
    const wstring& pin = L""
    )
{
    wstring query = UrlGetQuery(url);
    HTTPParameters getParameters = ParseQueryString(query);

    HTTPParameters oauthSignedParameters = BuildSignedOAuthParameters(
        getParameters,
        url,
        httpMethod,
		postParameters,
        consumerKey, consumerSecret,
        oauthToken, oauthTokenSecret,
        pin );
	return OAuthWebRequestSignedSubmit(oauthSignedParameters, url, httpMethod, postParameters);
}

int _tmain(int argc, _TCHAR* argv[])
{
	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	srand(_time32(NULL));

	string savedAccessTokenString;

	ifstream inFile;
	inFile.open(AuthFileName.c_str());
	inFile >> savedAccessTokenString;
	inFile.close();

	HTTPParameters savedParameters = ParseQueryString(UTF8ToWide(savedAccessTokenString));

	wstring oauthAccessToken = savedParameters[L"oauth_token"];
	wstring oauthAccessTokenSecret = savedParameters[L"oauth_token_secret"];
	wstring screenName = savedParameters[L"screen_name"];

	if( oauthAccessToken.empty() || oauthAccessTokenSecret.empty() )
	{
		// Overall OAuth flow based on 
		// Professional Twitter Development: With Examples in .NET 3.5 by Daniel Crenna

		wstring requestToken = OAuthWebRequestSubmit(RequestUrl, L"GET", NULL, ConsumerKey, ConsumerSecret);

		HTTPParameters response = ParseQueryString(requestToken);
		wstring oauthToken = response[L"oauth_token"];
		wstring oauthTokenSecret = response[L"oauth_token_secret"];
		if(!oauthToken.empty())
		{
			wchar_t buf[1024] = {};
			swprintf_s(buf, SIZEOF(buf), AuthorizeUrl.c_str(), oauthToken.c_str());

			wprintf(L"Launching %s\r\n", buf);
			ShellExecute(NULL, L"open", buf, NULL, NULL, SW_SHOWNORMAL);
		}

		wchar_t pin[1024] = {};
		wprintf(L"\r\n");
		wprintf(L"Enter the PIN you receive after authorizing this program in your web browser: ");
		_getws_s(pin, SIZEOF(pin));

		// exchange the request token for an access token
		wstring accessTokenString = OAuthWebRequestSubmit(AccessUrl, L"GET", NULL, ConsumerKey, ConsumerSecret, 
			oauthToken, oauthTokenSecret, pin);

		HTTPParameters accessTokenParameters = ParseQueryString(accessTokenString);
		oauthAccessToken = accessTokenParameters[L"oauth_token"];
		oauthAccessTokenSecret = accessTokenParameters[L"oauth_token_secret"];
		screenName = accessTokenParameters[L"screen_name"];

		ofstream outFile;
		outFile.open(AuthFileName.c_str(), ios_base::out | ios_base::trunc);
		outFile << WideToUTF8(accessTokenString);
		outFile.close();
	}

	wprintf(L"\r\n");
	wprintf(L"Authorized screen_name: %s\r\n", screenName.c_str());
	wprintf(L"Your oauth_token is: %s\r\n", oauthAccessToken.c_str());
	wprintf(L"Your oauth_token_secret is: %s\r\n", oauthAccessTokenSecret.c_str());
	wprintf(L"\r\n");

	wprintf(L"Press enter to request your time line...\r\n");
	getchar();

	// access a protected API call on Twitter using our access token
	wstring userTimeline = OAuthWebRequestSubmit(UserTimelineUrl, L"GET", NULL, ConsumerKey, ConsumerSecret, 
		oauthAccessToken, oauthAccessTokenSecret);

	wprintf(L"\r\nYour timeline:\r\n%s\r\n", userTimeline.c_str());

	wprintf(L"Enter your status update (Enter to skip): ");

	wchar_t buf[500] = {L""};
	_getws_s(buf, SIZEOF(buf));

	wstring input = buf;
	if(input.size() > 0)
	{
		wprintf(L"\r\nUpdating status: %s\r\n", input.c_str());

		HTTPParameters postParams;
		postParams[L"status"] = UrlEncode(input);

		wstring submitResult = OAuthWebRequestSubmit(UserStatusUpdateUrl, L"POST", &postParams, ConsumerKey, ConsumerSecret, 
			oauthAccessToken, oauthAccessTokenSecret);

		wprintf(L"\r\nUpdate Result:\r\n%s\r\n", submitResult.c_str());
	}


	wprintf(L"Press enter to exit...\r\n");
	getchar();

	return 0;
}

