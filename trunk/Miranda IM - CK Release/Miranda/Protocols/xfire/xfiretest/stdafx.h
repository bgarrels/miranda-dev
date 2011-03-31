// stdafx.h : Includedatei für Standardsystem-Includedateien
// oder häufig verwendete projektspezifische Includedateien,
// die nur in unregelmäßigen Abständen geändert werden.
//

#pragma once

#ifndef _WIN32_WINNT		// Lassen Sie die Verwendung spezifischer Features von Windows XP oder später zu.                   
#define _WIN32_WINNT 0x0501	// Ändern Sie dies in den geeigneten Wert für andere Versionen von Windows.
#endif						

#include <stdio.h>
#include <cstdlib>
#include <tchar.h>
#include <iostream>

#define uint unsigned int
#define socklen_t int

#pragma warning(disable:4267)
#pragma warning(disable:4244)
#pragma warning(disable:4312)
#pragma warning(disable:4099)
#pragma warning(disable:4311)

	#define     MSG_CTRUNC 0x08
    #define     MSG_PROXY 0x10
    #define     MSG_TRUNC 0x20
    #define     MSG_DONTWAIT 0x40
    #define     MSG_EOR 0x80
    #define     MSG_FIN 0x200
    #define     MSG_SYN 0x400
    #define     MSG_CONFIRM 0x800
    #define     MSG_RST 0x1000
    #define     MSG_ERRQUEUE 0x2000
    #define     MSG_NOSIGNAL 0x4000
    #define     MSG_MORE 0x8000 

#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"pthreads.lib")
#pragma comment(lib,"user32.lib")
#pragma comment(lib,"gdi32.lib")

// TODO: Hier auf zusätzliche Header, die das Programm erfordert, verweisen.
