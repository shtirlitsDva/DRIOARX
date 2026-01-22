//////////////////////////////////////////////////////////////////////////////
//
//  StdAfx.h : include file for standard system include files,
//             or project specific include files that are used frequently,
//             but are changed infrequently
//
//////////////////////////////////////////////////////////////////////////////
#pragma once

#ifndef _ALLOW_RTCc_IN_STL
#define _ALLOW_RTCc_IN_STL
#endif

#ifndef STRICT
#define STRICT
#endif

// Modify the following defines if you have to target a platform prior to the ones specified below.
#define WINVER 0x0601           // Windows 7
#define _WIN32_WINNT 0x0601     // Windows 7
#define _WIN32_IE 0x0700        // IE 7.0

#define _ATL_APARTMENT_THREADED

#include <atlbase.h>
// You may derive a class from CComModule and use it if you want to override
// something, but do not change the name of _Module
extern CComModule _Module;
#include <atlcom.h>

#pragma warning(disable : 4786 4996)

#pragma pack(push, 8)

//- ObjectARX headers
#include "rxregsvc.h"
#include "dbapserv.h"
#include "dbents.h"
#include "dbsymtb.h"
#include "AcExtensionModule.h"
#include "axlock.h"

#pragma pack(pop)
