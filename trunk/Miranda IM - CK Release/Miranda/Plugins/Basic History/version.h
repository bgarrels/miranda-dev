#define __MAJOR_VERSION				1
#define __MINOR_VERSION				0
#define __RELEASE_NUM				0
#define __BUILD_NUM					3
 
#define __FILEVERSION_STRING		__MAJOR_VERSION,__MINOR_VERSION,__RELEASE_NUM,__BUILD_NUM
#define __FILEVERSION_DOTS			__MAJOR_VERSION.__MINOR_VERSION.__RELEASE_NUM.__BUILD_NUM

#define __STRINGIFY_IMPL(x)			#x
#define __STRINGIFY(x)				__STRINGIFY_IMPL(x)
#define __VERSION_STRING			__STRINGIFY(__FILEVERSION_DOTS)

#define __PLUGIN_NAME				"Basic History"
#define __INTERNAL_NAME				"BasicHistory"
#define __FILENAME					"BasicHistory.dll"
#define __DESCRIPTION 				"History viewer for Miranda IM."
#define __AUTHOR					"Krzysztof Kral"
#define __AUTHOREMAIL				"programista@poczta.of.pl"
#define __AUTHORWEB					"http://programista.free.of.pl/miranda/"
#define __COPYRIGHT					"Copyright (c) 2011 KrzysztofKral"
