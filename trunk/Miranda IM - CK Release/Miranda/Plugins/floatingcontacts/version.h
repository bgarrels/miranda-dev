#define BUILD_NUM 2
#define BUILD_NUM_STR  "3"
#define REVISION  "$Revision: 2223 $"

#define COREVERSION_NUM 1, 0, 3,
#define COREVERSION_NUM_STR  "1, 0, 3"

#define MINIMAL_COREVERSION 0, 8, 0, 0
#define MINIMAL_COREVERSION_STR "0, 8, 0, 0"

#ifdef UNICODE
#define UNICODE_AWARE_STR " (Unicode)"
#else
#define UNICODE_AWARE_STR " (Ansi)"
#endif

#define FILE_VERSION	COREVERSION_NUM BUILD_NUM
#define FILE_VERSION_STR COREVERSION_NUM_STR  UNICODE_AWARE_STR " build " BUILD_NUM_STR	" " REVISION 

#define PRODUCT_VERSION	FILE_VERSION
#define PRODUCT_VERSION_STR	FILE_VERSION_STR

#define __PLUGIN_NAME				"Floating Contacts" UNICODE_AWARE_STR
#define __FILENAME					"FltContacts.dll"
#define __DESC						"Floating Contacts plugin for Miranda IM rewritten by CHEF-KOCH"
#define __AUTHOR					"Iavor Vajarov, Kosh&chka, Victor Pavlychko, CHEF-KOCH"
#define __AUTHOREMAIL				"ell-6@ya.ru"
#define __AUTHORWEB					"http://www.miranda-im.org/"
#define __COPYRIGHT					"� 2002-2004 I. Vajarov (ivajarov@code.bg), � 2008 Kosh&chka, V. Pavlychko, � 2011 CHEF-KOCH"
