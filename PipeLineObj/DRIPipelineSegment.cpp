// (C) Copyright 2002-2007 by Autodesk, Inc. 
//
// Permission to use, copy, modify, and distribute this software in
// object code form for any purpose and without fee is hereby granted, 
// provided that the above copyright notice appears in all copies and 
// that both that copyright notice and the limited warranty and
// restricted rights notice below appear in all supporting 
// documentation.
//
// AUTODESK PROVIDES THIS PROGRAM "AS IS" AND WITH ALL FAULTS. 
// AUTODESK SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF
// MERCHANTABILITY OR FITNESS FOR A PARTICULAR USE.  AUTODESK, INC. 
// DOES NOT WARRANT THAT THE OPERATION OF THE PROGRAM WILL BE
// UNINTERRUPTED OR ERROR FREE.
//
// Use, duplication, or disclosure by the U.S. Government is subject to 
// restrictions set forth in FAR 52.227-19 (Commercial Computer
// Software - Restricted Rights) and DFAR 252.227-7013(c)(1)(ii)
// (Rights in Technical Data and Computer Software), as applicable.
//

//-----------------------------------------------------------------------------
//----- DRIPipelineSegment.cpp : Implementation of DRIPipelineSegment
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "DRIPipelineSegment.h"

//-----------------------------------------------------------------------------
Adesk::UInt32 DRIPipelineSegment::kCurrentVersionNumber = 1;

//-----------------------------------------------------------------------------
ACRX_DXF_DEFINE_MEMBERS(
	DRIPipelineSegment, AcDbObject,
	AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent,
	AcDbProxyEntity::kNoOperation, DRIPIPELINESEGMENT,
	DRIPIPELINEOBJAPP
	| Product Desc : A description for your object
	| Company : Your company name
	| WEB Address : Your company WEB site address
)

//-----------------------------------------------------------------------------
DRIPipelineSegment::DRIPipelineSegment() : AcDbObject(), StartDistance(0), EndDistance(0)
{}

DRIPipelineSegment::~DRIPipelineSegment()
{}

double DRIPipelineSegment::startDistance() const
{
	assertReadEnabled();
	return StartDistance;
}

Acad::ErrorStatus DRIPipelineSegment::setStartDistance(const double distance)
{
	assertWriteEnabled();
	StartDistance = distance;
	return Acad::eOk;
}

double DRIPipelineSegment::endDistance() const
{
	assertReadEnabled();
	return EndDistance;
}

Acad::ErrorStatus DRIPipelineSegment::setEndDistance(const double distance)
{
	assertWriteEnabled();
	EndDistance = distance;
	return Acad::eOk;
}

//-----------------------------------------------------------------------------
//----- AcDbObject protocols
//- Dwg Filing protocol
Acad::ErrorStatus DRIPipelineSegment::dwgOutFields(AcDbDwgFiler * pFiler) const
{
	assertReadEnabled();
	//----- Save parent class information first.
	Acad::ErrorStatus es = AcDbObject::dwgOutFields(pFiler);
	if (es != Acad::eOk)
		return (es);
	//----- Object version number needs to be saved first
	if ((es = pFiler->writeUInt32(DRIPipelineSegment::kCurrentVersionNumber)) != Acad::eOk)
		return (es);
	//----- Output params
	//.....

	return (pFiler->filerStatus());
}

Acad::ErrorStatus DRIPipelineSegment::dwgInFields(AcDbDwgFiler * pFiler)
{
	assertWriteEnabled();
	//----- Read parent class information first.
	Acad::ErrorStatus es = AcDbObject::dwgInFields(pFiler);
	if (es != Acad::eOk)
		return (es);
	//----- Object version number needs to be read first
	Adesk::UInt32 version = 0;
	if ((es = pFiler->readUInt32(&version)) != Acad::eOk)
		return (es);
	if (version > DRIPipelineSegment::kCurrentVersionNumber)
		return (Acad::eMakeMeProxy);
	//- Uncomment the 2 following lines if your current object implementation cannot
	//- support previous version of that object.
	//if ( version < DRIPipelineSegment::kCurrentVersionNumber )
	//	return (Acad::eMakeMeProxy) ;
	//----- Read params
	//.....

	return (pFiler->filerStatus());
}

//- Dxf Filing protocol
Acad::ErrorStatus DRIPipelineSegment::dxfOutFields(AcDbDxfFiler * pFiler) const
{
	assertReadEnabled();
	//----- Save parent class information first.
	Acad::ErrorStatus es = AcDbObject::dxfOutFields(pFiler);
	if (es != Acad::eOk)
		return (es);
	es = pFiler->writeItem(AcDb::kDxfSubclass, _RXST("DRIPipelineSegment"));
	if (es != Acad::eOk)
		return (es);
	//----- Object version number needs to be saved first
	if ((es = pFiler->writeUInt32(kDxfInt32, DRIPipelineSegment::kCurrentVersionNumber)) != Acad::eOk)
		return (es);
	//----- Output params
	//.....

	return (pFiler->filerStatus());
}

Acad::ErrorStatus DRIPipelineSegment::dxfInFields(AcDbDxfFiler * pFiler)
{
	assertWriteEnabled();
	//----- Read parent class information first.
	Acad::ErrorStatus es = AcDbObject::dxfInFields(pFiler);
	if (es != Acad::eOk || !pFiler->atSubclassData(_RXST("DRIPipelineSegment")))
		return (pFiler->filerStatus());
	//----- Object version number needs to be read first
	struct resbuf rb;
	pFiler->readItem(&rb);
	if (rb.restype != AcDb::kDxfInt32)
	{
		pFiler->pushBackItem();
		pFiler->setError(Acad::eInvalidDxfCode, _RXST("\nError: expected group code %d (version #)"), AcDb::kDxfInt32);
		return (pFiler->filerStatus());
	}
	Adesk::UInt32 version = (Adesk::UInt32)rb.resval.rlong;
	if (version > DRIPipelineSegment::kCurrentVersionNumber)
		return (Acad::eMakeMeProxy);
	//- Uncomment the 2 following lines if your current object implementation cannot
	//- support previous version of that object.
	//if ( version < DRIPipelineSegment::kCurrentVersionNumber )
	//	return (Acad::eMakeMeProxy) ;
	//----- Read params in non order dependant manner
	while (es == Acad::eOk && (es = pFiler->readResBuf(&rb)) == Acad::eOk)
	{
		switch (rb.restype)
		{
			//----- Read params by looking at their DXF code (example below)
			//case AcDb::kDxfXCoord:
			//	if ( version == 1 )
			//		cen3d =asPnt3d (rb.resval.rpoint) ;
			//	else 
			//		cen2d =asPnt2d (rb.resval.rpoint) ;
			//	break ;
			//.....

		default:
			//----- An unrecognized group. Push it back so that the subclass can read it again.
			pFiler->pushBackItem();
			es = Acad::eEndOfFile;
			break;
		}
	}
	//----- At this point the es variable must contain eEndOfFile
	//----- - either from readResBuf() or from pushback. If not,
	//----- it indicates that an error happened and we should
	//----- return immediately.
	if (es != Acad::eEndOfFile)
		return (Acad::eInvalidResBuf);

	return (pFiler->filerStatus());
}

//- SubXXX() methods (self notification)
Acad::ErrorStatus DRIPipelineSegment::subOpen(AcDb::OpenMode mode)
{
	return (AcDbObject::subOpen(mode));
}

Acad::ErrorStatus DRIPipelineSegment::subErase(Adesk::Boolean erasing)
{
	return (AcDbObject::subErase(erasing));
}

Acad::ErrorStatus DRIPipelineSegment::subCancel()
{
	return (AcDbObject::subCancel());
}

Acad::ErrorStatus DRIPipelineSegment::subClose()
{
	return (AcDbObject::subClose());
}

//- Persistent reactor callbacks
void DRIPipelineSegment::openedForModify(const AcDbObject * pDbObj)
{
	assertReadEnabled();
	AcDbObject::openedForModify(pDbObj);
}

void DRIPipelineSegment::cancelled(const AcDbObject * pDbObj)
{
	assertReadEnabled();
	AcDbObject::cancelled(pDbObj);
}

void DRIPipelineSegment::objectClosed(const AcDbObjectId objId)
{
	assertReadEnabled();
	AcDbObject::objectClosed(objId);
}

void DRIPipelineSegment::goodbye(const AcDbObject * pDbObj)
{
	assertReadEnabled();
	AcDbObject::goodbye(pDbObj);
}

void DRIPipelineSegment::copied(const AcDbObject * pDbObj, const AcDbObject * pNewObj)
{
	assertReadEnabled();
	AcDbObject::copied(pDbObj, pNewObj);
}

void DRIPipelineSegment::erased(const AcDbObject * pDbObj, Adesk::Boolean bErasing)
{
	assertReadEnabled();
	AcDbObject::erased(pDbObj, bErasing);
}

void DRIPipelineSegment::modified(const AcDbObject * pDbObj)
{
	assertReadEnabled();
	AcDbObject::modified(pDbObj);
}

void DRIPipelineSegment::modifiedGraphics(const AcDbEntity * pDbEnt)
{
	assertReadEnabled();
	AcDbObject::modifiedGraphics(pDbEnt);
}

void DRIPipelineSegment::modifiedXData(const AcDbObject * pDbObj)
{
	assertReadEnabled();
	AcDbObject::modifiedXData(pDbObj);
}

void DRIPipelineSegment::subObjModified(const AcDbObject * pMainbObj, const AcDbObject * pSubObj)
{
	assertReadEnabled();
	AcDbObject::subObjModified(pMainbObj, pSubObj);
}

void DRIPipelineSegment::modifyUndone(const AcDbObject * pDbObj)
{
	assertReadEnabled();
	AcDbObject::modifyUndone(pDbObj);
}

void DRIPipelineSegment::reappended(const AcDbObject * pDbObj)
{
	assertReadEnabled();
	AcDbObject::reappended(pDbObj);
}

void DRIPipelineSegment::unappended(const AcDbObject * pDbObj)
{
	assertReadEnabled();
	AcDbObject::unappended(pDbObj);
}

