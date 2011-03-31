#define __MAJOR_VERSION				0
#define __MINOR_VERSION				2
#define __RELEASE_NUM				5
#define __BUILD_NUM					0

#define __FILEVERSION_STRING		__MAJOR_VERSION,__MINOR_VERSION,__RELEASE_NUM,__BUILD_NUM
#define __FILEVERSION_DOTS			__MAJOR_VERSION.__MINOR_VERSION.__RELEASE_NUM.__BUILD_NUM

#define __STRINGIFY_IMPL(x)			#x
#define __STRINGIFY(x)				__STRINGIFY_IMPL(x)
#define __VERSION_STRING			__STRINGIFY(__FILEVERSION_DOTS)

#ifdef _UNICODE
#if defined(WIN64) || defined(_WIN64)
	#define __PLUGIN_NAME "Spell Checker (Unicode x64)"
#else
	#define __PLUGIN_NAME "Spell Checker (Unicode)"
#endif
#else
	#define __PLUGIN_NAME "Spell Checker"
#endif
#define __INTERNAL_NAME				"SpellChecker"
#ifdef _UNICODE
	#define __FILENAME					"SpellCheckerW.dll"
#else
	#define __FILENAME					"SpellChecker.dll"
#endif
#define __DESCRIPTION 				"Spell checker for the message windows. Uses Hunspell to do the checking."
#define __AUTHOR					"Ricardo Pescuma Domenecci"
#define __AUTHOREMAIL				"pescuma@miranda-im.org"
#define __AUTHORWEB					"http://pescuma.org/miranda/spellchecker"
#define __COPYRIGHT					"© 2006-2010 Ricardo Pescuma Domenecci"
