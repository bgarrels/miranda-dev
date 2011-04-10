#define __MAJOR_VERSION				0
#define __MINOR_VERSION				0
#define __RELEASE_NUM				0
#define __BUILD_NUM					13

#define __FILEVERSION_STRING		__MAJOR_VERSION,__MINOR_VERSION,__RELEASE_NUM,__BUILD_NUM
#define __FILEVERSION_DOTS			__MAJOR_VERSION.__MINOR_VERSION.__RELEASE_NUM.__BUILD_NUM

#define __STRINGIFY_IMPL(x)			#x
#define __STRINGIFY(x)				__STRINGIFY_IMPL(x)
#define __VERSION_STRING			__STRINGIFY(__FILEVERSION_DOTS)

#ifdef _UNICODE
#if defined(WIN64) || defined(_WIN64)
	#define __PLUGIN_NAME "Bass_interface (Unicode x64) CK Release"
#else
	#define __PLUGIN_NAME "Bass_interface (Unicode) CK Release"
#endif
#else
	#define __PLUGIN_NAME "Bass_interface CK Release"
#endif
#define __INTERNAL_NAME				"Bass_interface"
#define __FILENAME					"Bass_interface.dll"
#define __DESCRIPTION 				"un4seen's bass interface Miranda IM plugin. Mod for CKs Pack."
#define __AUTHOR					"tico-tico"
#define __AUTHOREMAIL				""
#define __AUTHORWEB					""
#define __COPYRIGHT					"� 2010, 2011 tico-tico"
