//////////////////////////////////////////////////////////////////////////////
//
//  ComNSText.cpp : Implementation of CComNSText
//
//  COM wrapper for NSText entity - provides Properties Palette support
//
//////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "ComNSText.h"

//---------------------------------------------------------------------------
// InterfaceSupportsErrorInfo
// Returns whether a given interface supports IErrorInfo
//---------------------------------------------------------------------------
STDMETHODIMP CComNSText::InterfaceSupportsErrorInfo(REFIID riid)
{
    static const IID* arr[] =
    {
        &IID_IAcadText,
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

//---------------------------------------------------------------------------
// CreateNewObject
// Called when creating a new object through COM automation
//---------------------------------------------------------------------------
HRESULT CComNSText::CreateNewObject(AcDbObjectId& objId, AcDbObjectId& ownerId, TCHAR* keyName)
{
    // NSText objects should be created through the NSNorsynObjects module
    // This wrapper is for existing objects only (noncreatable)
    return E_NOTIMPL;
}

//---------------------------------------------------------------------------
// ForceDbResident
// Reports whether the object must be database-resident
//---------------------------------------------------------------------------
STDMETHODIMP CComNSText::ForceDbResident(VARIANT_BOOL* forceDbResident)
{
    if (forceDbResident == NULL)
        return E_POINTER;
    *forceDbResident = VARIANT_TRUE;
    return S_OK;
}

//---------------------------------------------------------------------------
// AddToDb
// Adds the object to the database - not supported for this wrapper
//---------------------------------------------------------------------------
STDMETHODIMP CComNSText::AddToDb(AcDbObjectId& objId, AcDbObjectId ownerId, TCHAR* keyName)
{
    return E_NOTIMPL;
}

//---------------------------------------------------------------------------
// CreateObject
// Creates the underlying AcDbObject - not supported for this wrapper
//---------------------------------------------------------------------------
STDMETHODIMP CComNSText::CreateObject(AcDbObjectId ownerId, TCHAR* keyName)
{
    return E_NOTIMPL;
}
