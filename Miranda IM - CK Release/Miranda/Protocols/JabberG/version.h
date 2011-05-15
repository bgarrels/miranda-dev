#include "../../include/m_version.h"

#define __FILEVERSION_STRING        MIRANDA_VERSION_FILEVERSION
#define __VERSION_STRING            MIRANDA_VERSION_STRING
#define __VERSION_DWORD             MIRANDA_VERSION_DWORD
#if defined (_UNICODE)
	#define __PLUGIN_NAME		    "Jabber Protocol (Unicode) Mataes Release"
#else
	#define __PLUGIN_NAME		    "Jabber Protocol Mataes Release"
#endif
