//////////////////////////////////////////////////////////////////////////////
//
//  ComNSText.cpp : Implementation of CComNSText
//
//////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "ComNSText.h"
#include "../../Entites/NSText/NSText.h"

//////////////////////////////////////////////////////////////////////////////
// ISupportsErrorInfo
STDMETHODIMP CComNSText::InterfaceSupportsErrorInfo(REFIID riid)
{
    static const IID* arr[] =
    {
        &IID_IAcadText,
        &IID_IOPMPropertyExtension,
    };
    for (int i = 0; i < sizeof(arr) / sizeof(arr[0]); i++)
    {
        if (InlineIsEqualGUID(*arr[i], riid))
            return S_OK;
    }
    return S_FALSE;
}

//////////////////////////////////////////////////////////////////////////////
// IAcadBaseObjectImpl
HRESULT CComNSText::CreateNewObject(AcDbObjectId& objId, AcDbObjectId& ownerId, TCHAR* keyName)
{
    try
    {
        AcDbBlockTableRecordPointer pBlockTableRecord(ownerId, AcDb::kForWrite);
        if (pBlockTableRecord.openStatus() != Acad::eOk)
            return E_FAIL;

        NSText* pNSText = new NSText();
        if (pBlockTableRecord->appendAcDbEntity(objId, pNSText) != Acad::eOk)
        {
            delete pNSText;
            return E_FAIL;
        }
        pNSText->close();
    }
    catch (...)
    {
        return E_FAIL;
    }
    return S_OK;
}

//////////////////////////////////////////////////////////////////////////////
// IAcadBaseObject2Impl
STDMETHODIMP CComNSText::ForceDbResident(VARIANT_BOOL *forceDbResident)
{
    if (forceDbResident == NULL)
        return E_POINTER;
    *forceDbResident = VARIANT_FALSE;
    return S_OK;
}

STDMETHODIMP CComNSText::AddToDb(AcDbObjectId& objId, AcDbObjectId ownerId, TCHAR* keyName)
{
    return CreateNewObject(objId, ownerId, keyName);
}

STDMETHODIMP CComNSText::CreateObject(AcDbObjectId ownerId, TCHAR *keyName)
{
    // NSText objects must be database-resident
    return E_NOTIMPL;
}
