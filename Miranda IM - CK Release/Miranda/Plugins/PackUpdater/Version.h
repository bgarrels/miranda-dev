#define __MAJOR_VERSION				0
#define __MINOR_VERSION				0
#define __RELEASE_NUM				0
#define __BUILD_NUM					9

#define __FILEVERSION_STRING		__MAJOR_VERSION,__MINOR_VERSION,__RELEASE_NUM,__BUILD_NUM
#define __FILEVERSION_DOTS			__MAJOR_VERSION.__MINOR_VERSION.__RELEASE_NUM.__BUILD_NUM

#define __STRINGIFY_IMPL(x)			#x
#define __STRINGIFY(x)				__STRINGIFY_IMPL(x)
#define __VERSION_STRING			__STRINGIFY(__FILEVERSION_DOTS)

#ifdef _UNICODE
#if defined(WIN64) || defined(_WIN64)
	#define __PLUGIN_NAME "PackUpdater (Unicode x64)"
#else
	#define __PLUGIN_NAME "PackUpdater (Unicode)"
#endif
#else
	#define __PLUGIN_NAME "PackUpdater"
#endif
#define __INTERNAL_NAME				"PackUpdater"
#define __FILENAME					"PackUpdater.dll"
#define __DESCRIPTION 				"Simple updater for Miranda IM premodified packs."
#define __AUTHOR					"Mataes, ZERO_BiT"
#define __AUTHOREMAIL				"mataes2007@gmail.com, zero-bit@mail.ru"
#define __AUTHORWEB					"http://mataes.googlecode.com/svn/Miranda/Plugins/PackUpdater/"
#define __COPYRIGHT					"� 2010 Mataes, 2007 ZERO_BiT"