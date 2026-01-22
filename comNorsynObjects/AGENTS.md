# COM Wrapper Implementation Guide for Norsyn Custom Entities

This guide explains how to add COM wrappers for new custom ObjectARX entities to display proper names in AutoCAD's Properties Palette.

## Overview

When you create a custom entity in ObjectARX (like NSText which derives from AcDbText), AutoCAD's Properties Palette shows the base class name ("Text") instead of your custom class name ("NSText"). To fix this, you need a COM wrapper that implements `IOPMPropertyExtension::GetDisplayName()`.

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

## Step-by-Step: Adding a New Entity Wrapper

### 1. Generate a GUID

Generate a new GUID for your COM class. Use PowerShell:
```powershell
[guid]::NewGuid().ToString().ToUpper()
```

Example result: `A1B2C3D4-E5F6-7890-ABCD-EF1234567890`

### 2. Create the Folder Structure

Create a folder for your entity under `comNorsynObjects/`:
```
comNorsynObjects/
├── ComNSText/           # Existing
│   ├── ComNSText.h
│   ├── ComNSText.cpp
│   └── ComNSText.rgs
├── ComNSMText/          # New entity example
│   ├── ComNSMText.h
│   ├── ComNSMText.cpp
│   └── ComNSMText.rgs
```

### 3. Create the Header File (ComXXX.h)

Use this template, replacing:
- `XXX` with your entity name (e.g., `NSMText`)
- `CLSID_ComXXX` GUID with your generated GUID
- `IAcadXXX` with the appropriate AutoCAD COM interface (e.g., `IAcadMText` for MText-derived)

```cpp
//////////////////////////////////////////////////////////////////////////////
//
//  ComNSMText.h : Declaration of the CComNSMText
//
//  COM wrapper for NSMText entity to display "NSMText" in Properties Palette
//
//////////////////////////////////////////////////////////////////////////////
#pragma once

#include "resource.h"
#include "comNorsynObjects_i.h"

using namespace ATL;

// DISPID for Entity Type property in OPM
#ifndef CYCAPPROP_OBJECTTYPE
#define CYCAPPROP_OBJECTTYPE 0x401
#endif

/////////////////////////////////////////////////////////////////////////////
// CComNSMText
//
class ATL_NO_VTABLE CComNSMText :
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CComNSMText, &CLSID_ComNSMText>,
    public ISupportErrorInfo,
    public IAcadEntityDispatchImpl<CComNSMText, &CLSID_ComNSMText,
                                   IAcadMText, &IID_IAcadMText,
                                   &LIBID_comNorsynObjectsLib>,
    public IOPMPropertyExtensionImpl<CComNSMText>
{
public:
    CComNSMText()
    {
    }

DECLARE_REGISTRY_RESOURCEID(IDR_COMNSMTEXT)
DECLARE_NOT_AGGREGATABLE(CComNSMText)

BEGIN_COM_MAP(CComNSMText)
    COM_INTERFACE_ENTRY(IAcadMText)
    COM_INTERFACE_ENTRY(IAcadEntity)
    COM_INTERFACE_ENTRY(IAcadObject)
    COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(ISupportErrorInfo)
    COM_INTERFACE_ENTRY(IAcadBaseObject)
    COM_INTERFACE_ENTRY(IAcadBaseObject2)
    COM_INTERFACE_ENTRY(IRetrieveApplication)
    COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
    COM_INTERFACE_ENTRY(IOPMPropertyExtension)
    COM_INTERFACE_ENTRY(ICategorizeProperties)
    COM_INTERFACE_ENTRY(IPerPropertyBrowsing)
END_COM_MAP()

// ISupportsErrorInfo
    STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IAcadBaseObjectImpl
    virtual HRESULT CreateNewObject(AcDbObjectId& objId, AcDbObjectId& ownerId, TCHAR* keyName);

// IAcadBaseObject2Impl
    STDMETHOD(ForceDbResident)(VARIANT_BOOL *forceDbResident);
    STDMETHOD(AddToDb)(AcDbObjectId& objId, AcDbObjectId ownerId = AcDbObjectId::kNull, TCHAR* keyName = NULL);
    STDMETHOD(CreateObject)(AcDbObjectId ownerId = AcDbObjectId::kNull, TCHAR *keyName = NULL);

//
// IOPMPropertyExtension - Override GetDisplayName to return "NSMText"
//
BEGIN_OPMPROP_MAP()
    // No custom properties - inherit all from base class
END_OPMPROP_MAP()

    // Override GetDisplayName to return custom entity name
    STDMETHOD(GetDisplayName)(
        /* [in] */ DISPID dispID,
        /* [out] */ BSTR *pBstr)
    {
        if (dispID == CYCAPPROP_OBJECTTYPE) // Entity Type property
        {
            *pBstr = ::SysAllocString(L"NSMText");  // <-- Your display name
            return S_OK;
        }
        return IOPMPropertyExtensionImpl<CComNSMText>::GetDisplayName(dispID, pBstr);
    }

    STDMETHOD(GetCategoryName)(THIS_
                           /* [in]  */ PROPCAT propcat,
                           /* [in]  */ LCID lcid,
                           /* [out] */ BSTR* pbstrName)
    {
        return S_FALSE;
    }

    virtual HINSTANCE GetResourceInstance()
    {
        return _AtlBaseModule.GetResourceInstance();
    }
};

OBJECT_ENTRY_AUTO(__uuidof(ComNSMText), CComNSMText)
```

### 4. Create the Implementation File (ComXXX.cpp)

```cpp
//////////////////////////////////////////////////////////////////////////////
//
//  ComNSMText.cpp : Implementation of CComNSMText
//
//////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "ComNSMText.h"

STDMETHODIMP CComNSMText::InterfaceSupportsErrorInfo(REFIID riid)
{
    static const IID* arr[] =
    {
        &IID_IAcadMText,
        &IID_IAcadEntity,
        &IID_IAcadObject
    };

    for (int i = 0; i < sizeof(arr) / sizeof(arr[0]); i++)
    {
        if (InlineIsEqualGUID(*arr[i], riid))
            return S_OK;
    }
    return S_FALSE;
}

HRESULT CComNSMText::CreateNewObject(AcDbObjectId& objId, AcDbObjectId& ownerId, TCHAR* keyName)
{
    return E_NOTIMPL;  // Objects created through DBX, not COM
}

STDMETHODIMP CComNSMText::ForceDbResident(VARIANT_BOOL* forceDbResident)
{
    if (forceDbResident == NULL)
        return E_POINTER;
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

### 5. Create the Registry Script (ComXXX.rgs)

Replace GUIDs and class names:

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
            'TypeLib' = s '{7c99a33f-a411-48d6-9707-81badecc45f7}'
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

**Important:** The `NSMText` in `AutoCAD.Application\Classes\NSMText` must exactly match your entity's DXF class name as registered in the DBX.

### 6. Update Resource.h

Add a resource ID for your new class:

```cpp
#define IDR_COMNSMTEXT                  103  // Next available ID
```

### 7. Update comNorsynObjects.idl

Add the coclass definition:

```idl
// ComNSMText coclass - wrapper for NSMText entity
[
    uuid(YOUR-GUID-HERE),
    helpstring("ComNSMText Class"),
    noncreatable
]
coclass ComNSMText
{
    [default] interface IAcadMText;
    [source] interface IAcadObjectEvents;
};
```

### 8. Update comNorsynObjects.rc

Add the registry resource:

```rc
IDR_COMNSMTEXT   REGISTRY "ComNSMText\\ComNSMText.rgs"
```

### 9. Update comNorsynObjects.cpp

Include the header to trigger OBJECT_ENTRY_AUTO registration:

```cpp
#include "ComNSMText\ComNSMText.h"
```

### 10. Update comNorsynObjects.vcxproj

Add the files to the project:

```xml
<ItemGroup>
    <ClInclude Include="ComNSMText\ComNSMText.h" />
</ItemGroup>
<ItemGroup>
    <ClCompile Include="ComNSMText\ComNSMText.cpp" />
</ItemGroup>
<ItemGroup>
    <None Include="ComNSMText\ComNSMText.rgs" />
</ItemGroup>
```

### 11. Update comNorsynObjects.vcxproj.filters (Optional)

Add filter entries for Solution Explorer organization:

```xml
<Filter Include="ComNSMText">
    <UniqueIdentifier>{GENERATE-NEW-GUID}</UniqueIdentifier>
</Filter>
```

## Base Class to Interface Mapping

| Entity Base Class | COM Interface | IID |
|-------------------|---------------|-----|
| AcDbText          | IAcadText     | IID_IAcadText |
| AcDbMText         | IAcadMText    | IID_IAcadMText |
| AcDbLine          | IAcadLine     | IID_IAcadLine |
| AcDbCircle        | IAcadCircle   | IID_IAcadCircle |
| AcDbArc           | IAcadArc      | IID_IAcadArc |
| AcDbPolyline      | IAcadLWPolyline | IID_IAcadLWPolyline |
| AcDb3dPolyline    | IAcad3DPolyline | IID_IAcad3DPolyline |
| AcDbBlockReference | IAcadBlockReference | IID_IAcadBlockReference |
| AcDbHatch         | IAcadHatch    | IID_IAcadHatch |
| AcDbDimension     | IAcadDimension | IID_IAcadDimension |

For other entities, check `axdb25enu.tlb` type library.

## Build and Registration

1. **Build:** Select x64 platform (no 32-bit AutoCAD since 2020)
2. **Register:** Run as Administrator: `regsvr32 comNorsynObjects.dll`
3. **Unregister:** `regsvr32 /u comNorsynObjects.dll`

## Troubleshooting

### Entity still shows base class name
- Verify the registry mapping matches the DXF class name exactly
- Check that the DLL is registered (`regsvr32`)
- Ensure DBX is loaded before selecting the entity

### Build errors with ObjectARX headers
- Verify `ArxSdkDir` in vcxproj points to correct SDK location
- Check that ObjectARX version matches AutoCAD version

### COM registration fails
- Run command prompt as Administrator
- Check for typos in .rgs file GUIDs

## Existing Wrappers

| Entity | COM Class | CLSID |
|--------|-----------|-------|
| NSText | CComNSText | {6C8B3C59-8519-4172-A9C3-301CD4C55570} |

## Library GUID

Type Library GUID: `{7c99a33f-a411-48d6-9707-81badecc45f7}`
