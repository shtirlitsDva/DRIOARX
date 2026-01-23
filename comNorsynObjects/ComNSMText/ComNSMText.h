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
// COM wrapper that inherits all AcDbMText properties but overrides
// the display name to show "NSMText" instead of "MText" in Properties Palette
//
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
    CComNSMText()
    {
    }

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

    //----- ISupportsErrorInfo
    STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

    DECLARE_PROTECT_FINAL_CONSTRUCT()

    HRESULT FinalConstruct()
    {
        return S_OK;
    }

    void FinalRelease()
    {
    }

    //IAcadBaseObjectImpl
    virtual HRESULT CreateNewObject(AcDbObjectId& objId, AcDbObjectId& ownerId, TCHAR* keyName);

    //IAcadBaseObject2Impl
    STDMETHOD(ForceDbResident)(VARIANT_BOOL* forceDbResident);
    STDMETHOD(AddToDb)(AcDbObjectId& objId, AcDbObjectId ownerId = AcDbObjectId::kNull, TCHAR* keyName = NULL);
    STDMETHOD(CreateObject)(AcDbObjectId ownerId = AcDbObjectId::kNull, TCHAR* keyName = NULL);

    //IOPMPropertyExtension
BEGIN_OPMPROP_MAP()
    //----- Use OPMPROP_ENTRY/OPMPROP_CAT_ENTRY macros for custom properties
END_OPMPROP_MAP()

    //IOPMPropertyExtensionImpl
    virtual HINSTANCE GetResourceInstance()
    {
        return _hdllInstance;
    }

    //----- Override GetDisplayName to return "NSMText" for entity type
    STDMETHOD(GetDisplayName)(
        /* [in] */ DISPID dispID,
        /* [out] */ BSTR* pBstr)
    {
        if (dispID == CYCAPPROP_OBJECTTYPE)
        {
            *pBstr = ::SysAllocString(L"NSMText");
            return S_OK;
        }
        return IOPMPropertyExtensionImpl<CComNSMText>::GetDisplayName(dispID, pBstr);
    }

public:
    //IAcadMText - inherited from IAcadEntityDispatchImpl

};

OBJECT_ENTRY_AUTO(__uuidof(ComNSMText), CComNSMText)
