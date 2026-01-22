//////////////////////////////////////////////////////////////////////////////
//
//  comNorsynObjects.cpp : Implementation of DLL Exports
//
//  COM wrapper DLL for Norsyn custom entities
//  Provides Properties Palette integration for custom entity display names
//
//////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"

#if defined(_DEBUG) && !defined(AC_FULL_DEBUG)
#error _DEBUG should not be defined except in internal Adesk debug builds
#endif

#include "resource.h"
#include "initguid.h"

#include "comNorsynObjects_i.h"
#include "comNorsynObjects_i.c"

#include "ComNSText/ComNSText.h"

CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
    OBJECT_ENTRY(CLSID_ComNSText, CComNSText)
    // Add entries for future COM wrappers:
    // OBJECT_ENTRY(CLSID_ComNSMText, CComNSMText)
END_OBJECT_MAP()

/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point
extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        _Module.Init(ObjectMap, hInstance);
        DisableThreadLibraryCalls(hInstance);

        // Check if the DBX is loaded, if not load it
        if (!::acrxAppIsLoaded(_T("NSNorsynObjects.dbx")))
        {
            if (!acrxLoadModule(_T("NSNorsynObjects.dbx"), false, true))
                return FALSE;
        }
        // Bump the reference count
        acrxLoadModule(_T("NSNorsynObjects.dbx"), false, false);
    }
    else if (dwReason == DLL_PROCESS_DETACH)
    {
        _Module.Term();
        // Decrease refcount on the dbx
        acrxUnloadModule(_T("NSNorsynObjects.dbx"));
    }
    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// Used to determine whether the DLL can be unloaded by OLE
STDAPI DllCanUnloadNow(void)
{
    return (_Module.GetLockCount() == 0) ? S_OK : S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Returns a class factory to create an object of the requested type
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    return _Module.GetClassObject(rclsid, riid, ppv);
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry
STDAPI DllRegisterServer(void)
{
    // Registers object, typelib and all interfaces in typelib
    return _Module.RegisterServer(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry
STDAPI DllUnregisterServer(void)
{
    _Module.UnregisterServer();
    return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// ObjectARX entry point - allows loading as ARX to self-register
extern "C" AcRx::AppRetCode __declspec(dllexport)
acrxEntryPoint(AcRx::AppMsgCode msg, void* pkt)
{
    switch (msg)
    {
    case AcRx::kInitAppMsg:
        // Unlock the application
        acrxDynamicLinker->unlockApplication(pkt);
        acrxRegisterAppMDIAware(pkt);
        // Register ourselves in the registry
        DllRegisterServer();
        break;
    case AcRx::kUnloadAppMsg:
        break;
    case AcRx::kOleUnloadAppMsg:
        // Respond according to our current state
        return DllCanUnloadNow() == S_OK ? AcRx::kRetOK : AcRx::kRetError;
    }
    return AcRx::kRetOK;
}
