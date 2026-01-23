// dllmain.cpp : Implementation of DllMain.

#include "pch.h"
#include "framework.h"
#include "resource.h"
#include "comNorsynObjects_i.h"
#include "dllmain.h"

CcomNorsynObjectsModule _AtlModule;

//----- Global instance handle for resource access
HINSTANCE _hdllInstance = NULL;

// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		_hdllInstance = hInstance;
	}
	return _AtlModule.DllMain(dwReason, lpReserved);
}
