# COM Wrapper Implementation Guide for Custom Entities

<overview>
When you create a custom entity in ObjectARX (like NSText which derives from AcDbText), AutoCAD's Properties Palette shows the base class name ("Text") instead of your custom class name ("NSText"). To fix this, you need a COM wrapper DLL that implements `IOPMPropertyExtension::GetDisplayName()`.

This guide is based on the official ObjectARX Wizard template (ArxAtlWizComWrapper) with custom additions for entity type name override.
</overview>

<architecture>
## Architecture

```
NSNorsynObjects.dbx          comNorsynObjects.dll
┌─────────────────┐          ┌─────────────────────┐
│  NSText entity  │◄────────►│  CComNSText wrapper │
│  (AcDbText)     │          │  (IOPMPropertyExt)  │
└─────────────────┘          └─────────────────────┘
        │                              │
        │                              │
        ▼                              ▼
   Drawing file              Registry mapping:
   stores NSText             NSText → CLSID
```

The COM wrapper must be a **separate DLL project** (ATL), not embedded in the DBX.
</architecture>

<wizard-template>
## ObjectARX Wizard Template Structure

The official ArxAtlWizComWrapper generates this class structure:

```cpp
class ATL_NO_VTABLE CComXXX :
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CComXXX, &CLSID_ComXXX>,
    public ISupportErrorInfo,
    public IOPMPropertyExtensionImpl<CComXXX>,
    public IAcadEntityDispatchImpl<CComXXX, &CLSID_ComXXX,
                                   IAcadXXX, &IID_IAcadXXX,
                                   &LIBID_YourLib>
```

<important>
**Order matters!** `IOPMPropertyExtensionImpl` must come BEFORE `IAcadEntityDispatchImpl` in the inheritance list.
</important>

<com-map>
### COM_MAP Order (from wizard)

```cpp
BEGIN_COM_MAP(CComXXX)
    COM_INTERFACE_ENTRY(IAcadXXX)           // Your specific interface (IAcadText, etc.)
    COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(ISupportErrorInfo)
    COM_INTERFACE_ENTRY(IConnectionPointContainer)
    COM_INTERFACE_ENTRY(IOPMPropertyExtension)
    COM_INTERFACE_ENTRY(ICategorizeProperties)
    COM_INTERFACE_ENTRY(IPerPropertyBrowsing)
    COM_INTERFACE_ENTRY(IAcadBaseObject)
    COM_INTERFACE_ENTRY(IAcadBaseObject2)
    COM_INTERFACE_ENTRY(IAcadObject)
    COM_INTERFACE_ENTRY(IAcadEntity)        // Only for entities
    COM_INTERFACE_ENTRY(IRetrieveApplication)
END_COM_MAP()
```
</com-map>

<required-methods>
### Required Methods (from wizard)

```cpp
DECLARE_PROTECT_FINAL_CONSTRUCT()

HRESULT FinalConstruct() { return S_OK; }
void FinalRelease() { }

//IAcadBaseObjectImpl
virtual HRESULT CreateNewObject(AcDbObjectId& objId, AcDbObjectId& ownerId, TCHAR* keyName);

//IAcadBaseObject2Impl
STDMETHOD(ForceDbResident)(VARIANT_BOOL *forceDbResident);
STDMETHOD(AddToDb)(AcDbObjectId& objId, AcDbObjectId ownerId = AcDbObjectId::kNull, TCHAR* keyName = NULL);
STDMETHOD(CreateObject)(AcDbObjectId ownerId = AcDbObjectId::kNull, TCHAR *keyName = NULL);

//IOPMPropertyExtensionImpl
virtual HINSTANCE GetResourceInstance() { return _hdllInstance; }
```
</required-methods>

<hdllinstance>
### Global _hdllInstance

The wizard uses `_hdllInstance` for resource access. Define it in dllmain.cpp:

```cpp
// dllmain.cpp
HINSTANCE _hdllInstance = NULL;

extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        _hdllInstance = hInstance;
    }
    return _AtlModule.DllMain(dwReason, lpReserved);
}
```

Declare extern in framework.h:
```cpp
extern HINSTANCE _hdllInstance;
```
</hdllinstance>
</wizard-template>

<custom-display-name>
## Custom Entity Type Display Name

The wizard does NOT generate the GetDisplayName override - this is the custom addition needed to show "NSText" instead of "Text":

```cpp
// DISPID for Entity Type property in OPM
#ifndef CYCAPPROP_OBJECTTYPE
#define CYCAPPROP_OBJECTTYPE 0x401
#endif

// In your class:
STDMETHOD(GetDisplayName)(DISPID dispID, BSTR *pBstr)
{
    if (dispID == CYCAPPROP_OBJECTTYPE)
    {
        *pBstr = ::SysAllocString(L"NSText");  // Your entity name
        return S_OK;
    }
    return IOPMPropertyExtensionImpl<CComNSText>::GetDisplayName(dispID, pBstr);
}
```

<dispid-reference>
### Common DISPID Values

| DISPID | Hex | Property |
|--------|-----|----------|
| CYCAPPROP_OBJECTTYPE | 0x401 | Entity type name shown in Properties Palette |
</dispid-reference>
</custom-display-name>

<step-by-step>
## Step-by-Step: Adding a New Entity Wrapper

<step num="1">
### 1. Generate a GUID

```powershell
[guid]::NewGuid().ToString().ToUpper()
```
</step>

<step num="2">
### 2. Create Folder Structure

```
comNorsynObjects/
├── ComNSText/
│   ├── ComNSText.h
│   ├── ComNSText.cpp
│   └── ComNSText.rgs
├── ComNSMText/          # New entity
│   ├── ComNSMText.h
│   ├── ComNSMText.cpp
│   └── ComNSMText.rgs
```
</step>

<step num="3">
### 3. Create Header File (ComXXX.h)

```cpp
#pragma once
#include "resource.h"
#include "comNorsynObjects_i.h"

using namespace ATL;

#ifndef CYCAPPROP_OBJECTTYPE
#define CYCAPPROP_OBJECTTYPE 0x401
#endif

class ATL_NO_VTABLE CComNSMText :
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CComNSMText, &CLSID_ComNSMText>,
    public ISupportErrorInfo,
    public IOPMPropertyExtensionImpl<CComNSMText>,
    public IAcadEntityDispatchImpl<CComNSMText, &CLSID_ComNSMText,
                                   IAcadMText, &IID_IAcadMText,
                                   &LIBID_comNorsynObjectsLib>
{
public:
    CComNSMText() { }

DECLARE_REGISTRY_RESOURCEID(IDR_COMNSMTEXT)
DECLARE_NOT_AGGREGATABLE(CComNSMText)

BEGIN_COM_MAP(CComNSMText)
    COM_INTERFACE_ENTRY(IAcadMText)
    COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(ISupportErrorInfo)
    COM_INTERFACE_ENTRY(IConnectionPointContainer)
    COM_INTERFACE_ENTRY(IOPMPropertyExtension)
    COM_INTERFACE_ENTRY(ICategorizeProperties)
    COM_INTERFACE_ENTRY(IPerPropertyBrowsing)
    COM_INTERFACE_ENTRY(IAcadBaseObject)
    COM_INTERFACE_ENTRY(IAcadBaseObject2)
    COM_INTERFACE_ENTRY(IAcadObject)
    COM_INTERFACE_ENTRY(IAcadEntity)
    COM_INTERFACE_ENTRY(IRetrieveApplication)
END_COM_MAP()

    STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

    DECLARE_PROTECT_FINAL_CONSTRUCT()
    HRESULT FinalConstruct() { return S_OK; }
    void FinalRelease() { }

    virtual HRESULT CreateNewObject(AcDbObjectId& objId, AcDbObjectId& ownerId, TCHAR* keyName);
    STDMETHOD(ForceDbResident)(VARIANT_BOOL *forceDbResident);
    STDMETHOD(AddToDb)(AcDbObjectId& objId, AcDbObjectId ownerId = AcDbObjectId::kNull, TCHAR* keyName = NULL);
    STDMETHOD(CreateObject)(AcDbObjectId ownerId = AcDbObjectId::kNull, TCHAR *keyName = NULL);

BEGIN_OPMPROP_MAP()
END_OPMPROP_MAP()

    virtual HINSTANCE GetResourceInstance() { return _hdllInstance; }

    STDMETHOD(GetDisplayName)(DISPID dispID, BSTR *pBstr)
    {
        if (dispID == CYCAPPROP_OBJECTTYPE)
        {
            *pBstr = ::SysAllocString(L"NSMText");
            return S_OK;
        }
        return IOPMPropertyExtensionImpl<CComNSMText>::GetDisplayName(dispID, pBstr);
    }
};

OBJECT_ENTRY_AUTO(__uuidof(ComNSMText), CComNSMText)
```
</step>

<step num="4">
### 4. Create Implementation File (ComXXX.cpp)

```cpp
#include "pch.h"
#include "ComNSMText.h"

STDMETHODIMP CComNSMText::InterfaceSupportsErrorInfo(REFIID riid)
{
    static const IID* arr[] = { &IID_IAcadMText, &IID_IAcadEntity, &IID_IAcadObject };
    for (int i = 0; i < sizeof(arr) / sizeof(arr[0]); i++)
    {
        if (InlineIsEqualGUID(*arr[i], riid))
            return S_OK;
    }
    return S_FALSE;
}

HRESULT CComNSMText::CreateNewObject(AcDbObjectId& objId, AcDbObjectId& ownerId, TCHAR* keyName)
{
    return E_NOTIMPL;  // Objects created through DBX
}

STDMETHODIMP CComNSMText::ForceDbResident(VARIANT_BOOL* forceDbResident)
{
    if (!forceDbResident) return E_POINTER;
    *forceDbResident = VARIANT_TRUE;
    return S_OK;
}

STDMETHODIMP CComNSMText::AddToDb(AcDbObjectId& objId, AcDbObjectId ownerId, TCHAR* keyName)
{
    return E_NOTIMPL;
}

STDMETHODIMP CComNSMText::CreateObject(AcDbObjectId ownerId, TCHAR* keyName)
{
    return E_NOTIMPL;
}
```
</step>

<step num="5">
### 5. Create Registry Script (ComXXX.rgs)

```rgs
HKCR
{
    NoRemove CLSID
    {
        ForceRemove {YOUR-GUID-HERE} = s 'ComNSMText Class'
        {
            ProgID = s 'comNorsynObjects.ComNSMText.1'
            VersionIndependentProgID = s 'comNorsynObjects.ComNSMText'
            InprocServer32 = s '%MODULE%'
            {
                val ThreadingModel = s 'Apartment'
            }
            'TypeLib' = s '{YOUR-TYPELIB-GUID}'
        }
    }
    NoRemove AutoCAD.Application
    {
        NoRemove Classes
        {
            ForceRemove NSMText = s '{YOUR-GUID-HERE}'
        }
    }
}
```

<important>
The class name in `AutoCAD.Application\Classes\NSMText` must exactly match your entity's DXF class name from `ACRX_DXF_DEFINE_MEMBERS`.
</important>
</step>

<step num="6">
### 6. Update Project Files

**Resource.h:**
```cpp
#define IDR_COMNSMTEXT  103  // Next available ID
```

**comNorsynObjects.idl:**
```idl
[uuid(YOUR-GUID), helpstring("ComNSMText Class"), noncreatable]
coclass ComNSMText
{
    [default] interface IAcadMText;
    [source] interface IAcadObjectEvents;
};
```

**comNorsynObjects.rc:**
```rc
IDR_COMNSMTEXT REGISTRY "ComNSMText\\ComNSMText.rgs"
```

**comNorsynObjects.cpp:**
```cpp
#include "ComNSMText\ComNSMText.h"
```
</step>
</step-by-step>

<interface-mapping>
## Base Class to Interface Mapping

| Entity Base Class | COM Interface | IID |
|-------------------|---------------|-----|
| AcDbText | IAcadText | IID_IAcadText |
| AcDbMText | IAcadMText | IID_IAcadMText |
| AcDbLine | IAcadLine | IID_IAcadLine |
| AcDbCircle | IAcadCircle | IID_IAcadCircle |
| AcDbArc | IAcadArc | IID_IAcadArc |
| AcDbPolyline | IAcadLWPolyline | IID_IAcadLWPolyline |
| AcDb3dPolyline | IAcad3DPolyline | IID_IAcad3DPolyline |
| AcDbBlockReference | IAcadBlockReference | IID_IAcadBlockReference |
| AcDbHatch | IAcadHatch | IID_IAcadHatch |
| AcDbDimension | IAcadDimension | IID_IAcadDimension |

For other entities, check `axdb25enu.tlb` type library.
</interface-mapping>

<build-and-register>
## Build and Registration

1. **Build:** Select x64 platform (no 32-bit AutoCAD since 2020)
2. **Register:** Run as Administrator: `regsvr32 comNorsynObjects.dll`
3. **Unregister:** `regsvr32 /u comNorsynObjects.dll`
</build-and-register>

<troubleshooting>
## Troubleshooting

<issue name="Entity still shows base class name">
- Verify registry mapping matches DXF class name exactly
- Check DLL is registered (`regsvr32`)
- Ensure DBX is loaded before selecting entity
- Verify DISPID 0x401 is handled in GetDisplayName
</issue>

<issue name="Build errors with ObjectARX headers">
- Verify `ArxSdkDir` in vcxproj points to correct SDK
- Check ObjectARX version matches AutoCAD version
- Add `_ALLOW_RTCc_IN_STL` define before includes
</issue>

<issue name="COM registration fails">
- Run command prompt as Administrator
- Check for typos in .rgs file GUIDs
</issue>

<issue name="_hdllInstance undefined">
- Add `HINSTANCE _hdllInstance = NULL;` to dllmain.cpp
- Save it in DllMain when `dwReason == DLL_PROCESS_ATTACH`
- Add `extern HINSTANCE _hdllInstance;` to framework.h
</issue>
</troubleshooting>

<existing-wrappers>
## Existing Wrappers

| Entity | COM Class | CLSID |
|--------|-----------|-------|
| NSText | CComNSText | {6C8B3C59-8519-4172-A9C3-301CD4C55570} |
</existing-wrappers>

<library-guids>
## Library GUIDs

| Item | GUID |
|------|------|
| Type Library | {7c99a33f-a411-48d6-9707-81badecc45f7} |
</library-guids>

<references>
## References

- ObjectARX SDK: `ObjectARXWizard/` - ArxAtlWizComWrapper template
- ObjectARX SDK: `inc/axtempl.h` - IAcadEntityDispatchImpl
- ObjectARX SDK: `inc/dynprops.h` - IOPMPropertyExtension
</references>
