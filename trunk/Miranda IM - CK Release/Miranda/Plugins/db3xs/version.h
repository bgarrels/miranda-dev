#define __FILE_VERSION 0,9,1,3
#define __FILE_VERSION_STR "0, 9, 1, 3"
#define __FILE_VERSION_DWORD 0x00090103

#if defined ( _WIN64 )
#define __MODULE_NAME "Miranda Secure database driver (x64)"
#elif defined ( _UNICODE )
#define __MODULE_NAME "Miranda Secure database driver (Unicode)"
#else
#define __MODULE_NAME "Miranda Secure database driver"
#endif

#define __PLUGIN_NAME "Miranda Secure database driver"
#define __DESC "Provides Miranda database support: global settings, contacts, history, settings per contact."
#define __AUTHORS "Miranda-IM project, Billy_Bons"
#define __EMAIL "tatarinov.sergey@gmail.com"
#define __COPYRIGHTS "(C) 2000-2011 Miranda IM project, sje, Piotr Pawluczuk, a0x, Billy_Bons"
