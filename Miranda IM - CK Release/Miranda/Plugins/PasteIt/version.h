#define __MAJOR_VERSION				1
#define __MINOR_VERSION				0
#define __RELEASE_NUM				0
#define __BUILD_NUM					0

#define __FILEVERSION_STRING		__MAJOR_VERSION,__MINOR_VERSION,__RELEASE_NUM,__BUILD_NUM
#define __FILEVERSION_DOTS			__MAJOR_VERSION.__MINOR_VERSION.__RELEASE_NUM.__BUILD_NUM

#define __STRINGIFY_IMPL(x)			#x
#define __STRINGIFY(x)				__STRINGIFY_IMPL(x)
#define __VERSION_STRING			__STRINGIFY(__FILEVERSION_DOTS)

#ifdef _UNICODE
#if defined(WIN64) || defined(_WIN64)
	#define __PLUGIN_NAME "Paste It (Unicode x64)"
#else
	#define __PLUGIN_NAME "Paste It (Unicode)"
#endif
#else
	#define __PLUGIN_NAME "Paste It"
#endif
#define __INTERNAL_NAME				"PasteIt"
#define __FILENAME					"PasteIt.dll"
#define __DESCRIPTION 				"Uploads the text to web page and sends the URL to your friends."
#define __AUTHOR					"Krzysztof Kral"
#define __AUTHOREMAIL				"programista@poczta.of.pl"
#define __AUTHORWEB					"http://programista.free.of.pl/miranda/"
#define __COPYRIGHT					"© 2011 Krzysztof Kral"
