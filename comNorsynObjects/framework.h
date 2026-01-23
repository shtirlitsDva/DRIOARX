#pragma once

#ifndef STRICT
#define STRICT
#endif

#ifndef _ALLOW_RTCc_IN_STL
#define _ALLOW_RTCc_IN_STL
#endif

#include "targetver.h"

#define _ATL_APARTMENT_THREADED

#define _ATL_NO_AUTOMATIC_NAMESPACE

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

#define ATL_NO_ASSERT_ON_DESTROY_NONEXISTENT_WINDOW

#include "resource.h"
#include <atlbase.h>
#include <atlcom.h>
#include <atlctl.h>

#pragma warning(disable : 4786 4996)

//-----------------------------------------------------------------------------
// ObjectARX headers
//-----------------------------------------------------------------------------
#pragma pack(push, 8)

#include "rxregsvc.h"
#include "dbapserv.h"
#include "dbents.h"
#include "dbsymtb.h"
#include "AcExtensionModule.h"
#include "axlock.h"
#include "axtempl.h"
#include "dynprops.h"
#include <tchar.h>

#pragma pack(pop)

//-----------------------------------------------------------------------------
// Global instance handle for resource access (defined in dllmain.cpp)
//-----------------------------------------------------------------------------
extern HINSTANCE _hdllInstance;
