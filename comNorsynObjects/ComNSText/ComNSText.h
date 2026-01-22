//////////////////////////////////////////////////////////////////////////////
//
//  ComNSText.h : Declaration of the CComNSText
//
//  COM wrapper for NSText entity to display "NSText" in Properties Palette
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
// CComNSText
//
// Minimal COM wrapper that inherits all AcDbText properties but overrides
// the display name to show "NSText" instead of "Text" in Properties Palette
//
class ATL_NO_VTABLE CComNSText :
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CComNSText, &CLSID_ComNSText>,
    public ISupportErrorInfo,
    public IAcadEntityDispatchImpl<CComNSText, &CLSID_ComNSText,
                                   IAcadText, &IID_IAcadText,
                                   &LIBID_comNorsynObjectsLib>,
    public IOPMPropertyExtensionImpl<CComNSText>
{
public:
    CComNSText()
    {
    }

DECLARE_REGISTRY_RESOURCEID(IDR_COMNSTEXT)
DECLARE_NOT_AGGREGATABLE(CComNSText)

BEGIN_COM_MAP(CComNSText)
    COM_INTERFACE_ENTRY(IAcadText)
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
// IOPMPropertyExtension - Override GetDisplayName to return "NSText"
//
BEGIN_OPMPROP_MAP()
    // No custom properties - we just inherit all from AcDbText
END_OPMPROP_MAP()

    // Override GetDisplayName to return "NSText" for the entity type
    STDMETHOD(GetDisplayName)(
        /* [in] */ DISPID dispID,
        /* [out] */ BSTR *pBstr)
    {
        if (dispID == CYCAPPROP_OBJECTTYPE) // Entity Type property
        {
            *pBstr = ::SysAllocString(L"NSText");
            return S_OK;
        }
        // For other properties, use default implementation
        return IOPMPropertyExtensionImpl<CComNSText>::GetDisplayName(dispID, pBstr);
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

OBJECT_ENTRY_AUTO(__uuidof(ComNSText), CComNSText)
