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
//----- DRIPipeLabel.cpp : Implementation of DRIPipeLabel
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "DRIPipeLabel.h"
#include <math.h>
#include "TrigMacros.h"

//-----------------------------------------------------------------------------
Adesk::UInt32 DRIPipeLabel::kCurrentVersionNumber = 1;

//-----------------------------------------------------------------------------
ACRX_DXF_DEFINE_MEMBERS(
	DRIPipeLabel, AcDbText,
	AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent,
	AcDbProxyEntity::kNoOperation, DRIPIPELABEL,
	DRIPIPELINEOBJAPP
	| Product Desc : A description for your object
	| Company : Your company name
	| WEB Address : Your company WEB site address
)

//-----------------------------------------------------------------------------
DRIPipeLabel::DRIPipeLabel() : AcDbText()
{}

DRIPipeLabel::~DRIPipeLabel()
{}

//-----------------------------------------------------------------------------
//----- AcDbObject protocols
//- Dwg Filing protocol
Acad::ErrorStatus DRIPipeLabel::dwgOutFields(AcDbDwgFiler * pFiler) const
{
	assertReadEnabled();
	//----- Save parent class information first.
	Acad::ErrorStatus es = AcDbText::dwgOutFields(pFiler);
	if (es != Acad::eOk)
		return (es);
	//----- Object version number needs to be saved first
	if ((es = pFiler->writeUInt32(DRIPipeLabel::kCurrentVersionNumber)) != Acad::eOk)
		return (es);
	//----- Output params
	//.....

	return (pFiler->filerStatus());
}

Acad::ErrorStatus DRIPipeLabel::dwgInFields(AcDbDwgFiler * pFiler)
{
	assertWriteEnabled();
	//----- Read parent class information first.
	Acad::ErrorStatus es = AcDbText::dwgInFields(pFiler);
	if (es != Acad::eOk)
		return (es);
	//----- Object version number needs to be read first
	Adesk::UInt32 version = 0;
	if ((es = pFiler->readUInt32(&version)) != Acad::eOk)
		return (es);
	if (version > DRIPipeLabel::kCurrentVersionNumber)
		return (Acad::eMakeMeProxy);
	//- Uncomment the 2 following lines if your current object implementation cannot
	//- support previous version of that object.
	//if ( version < DRIPipeLabel::kCurrentVersionNumber )
	//	return (Acad::eMakeMeProxy) ;
	//----- Read params
	//.....

	return (pFiler->filerStatus());
}

//- Dxf Filing protocol
Acad::ErrorStatus DRIPipeLabel::dxfOutFields(AcDbDxfFiler * pFiler) const
{
	assertReadEnabled();
	//----- Save parent class information first.
	Acad::ErrorStatus es = AcDbText::dxfOutFields(pFiler);
	if (es != Acad::eOk)
		return (es);
	es = pFiler->writeItem(AcDb::kDxfSubclass, _RXST("DRIPipeLabel"));
	if (es != Acad::eOk)
		return (es);
	//----- Object version number needs to be saved first
	if ((es = pFiler->writeUInt32(kDxfInt32, DRIPipeLabel::kCurrentVersionNumber)) != Acad::eOk)
		return (es);
	//----- Output params
	//.....

	return (pFiler->filerStatus());
}

Acad::ErrorStatus DRIPipeLabel::dxfInFields(AcDbDxfFiler * pFiler)
{
	assertWriteEnabled();
	//----- Read parent class information first.
	Acad::ErrorStatus es = AcDbText::dxfInFields(pFiler);
	if (es != Acad::eOk || !pFiler->atSubclassData(_RXST("DRIPipeLabel")))
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
	if (version > DRIPipeLabel::kCurrentVersionNumber)
		return (Acad::eMakeMeProxy);
	//- Uncomment the 2 following lines if your current object implementation cannot
	//- support previous version of that object.
	//if ( version < DRIPipeLabel::kCurrentVersionNumber )
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
Acad::ErrorStatus DRIPipeLabel::subOpen(AcDb::OpenMode mode)
{
	return (AcDbText::subOpen(mode));
}

Acad::ErrorStatus DRIPipeLabel::subErase(Adesk::Boolean erasing)
{
	return (AcDbText::subErase(erasing));
}

Acad::ErrorStatus DRIPipeLabel::subCancel()
{
	return (AcDbText::subCancel());
}

Acad::ErrorStatus DRIPipeLabel::subClose()
{
	return (AcDbText::subClose());
}

//- Persistent reactor callbacks
void DRIPipeLabel::openedForModify(const AcDbObject * pDbObj)
{
	assertReadEnabled();
	AcDbText::openedForModify(pDbObj);
}

void DRIPipeLabel::cancelled(const AcDbObject * pDbObj)
{
	assertReadEnabled();
	AcDbText::cancelled(pDbObj);
}

void DRIPipeLabel::objectClosed(const AcDbObjectId objId)
{
	assertReadEnabled();
	AcDbText::objectClosed(objId);
}

void DRIPipeLabel::goodbye(const AcDbObject * pDbObj)
{
	assertReadEnabled();
	AcDbText::goodbye(pDbObj);
}

void DRIPipeLabel::copied(const AcDbObject * pDbObj, const AcDbObject * pNewObj)
{
	assertReadEnabled();
	AcDbText::copied(pDbObj, pNewObj);
}

void DRIPipeLabel::erased(const AcDbObject * pDbObj, Adesk::Boolean bErasing)
{
	assertReadEnabled();
	AcDbText::erased(pDbObj, bErasing);
}

void DRIPipeLabel::modified(const AcDbObject * pDbObj)
{
	assertReadEnabled();
	AcDbText::modified(pDbObj);
}

void DRIPipeLabel::modifiedGraphics(const AcDbEntity * pDbEnt)
{
	assertReadEnabled();
	AcDbText::modifiedGraphics(pDbEnt);
}

void DRIPipeLabel::modifiedXData(const AcDbObject * pDbObj)
{
	assertReadEnabled();
	AcDbText::modifiedXData(pDbObj);
}

void DRIPipeLabel::subObjModified(const AcDbObject * pMainbObj, const AcDbObject * pSubObj)
{
	assertReadEnabled();
	AcDbText::subObjModified(pMainbObj, pSubObj);
}

void DRIPipeLabel::modifyUndone(const AcDbObject * pDbObj)
{
	assertReadEnabled();
	AcDbText::modifyUndone(pDbObj);
}

void DRIPipeLabel::reappended(const AcDbObject * pDbObj)
{
	assertReadEnabled();
	AcDbText::reappended(pDbObj);
}

void DRIPipeLabel::unappended(const AcDbObject * pDbObj)
{
	assertReadEnabled();
	AcDbText::unappended(pDbObj);
}

//-----------------------------------------------------------------------------
//----- AcDbEntity protocols
Adesk::Boolean DRIPipeLabel::subWorldDraw(AcGiWorldDraw * mode)
{
	assertReadEnabled();
	//------ Returning Adesk::kFalse here will force viewportDraw() call
	return (Adesk::kFalse);
}

void DRIPipeLabel::subViewportDraw(AcGiViewportDraw * mode)
{
	assertReadEnabled();
	//AcGePoint2d pt1, pt2;
	//mode->viewport().getViewportDcCorners(pt1, pt2);

	//mode->viewport().viewportId()

	//AcGePoint3d pts[2] = { AcGePoint3d(pt1.x,pt1.y,0), AcGePoint3d(pt2.x,pt2.y,0) };
	//mode->geometry().worldLine(pts);
	AcGePoint3d orgn;
	AcGeVector3d xa, ya, za;
	AcGeMatrix3d transform;
	mode->viewport().getEyeToWorldTransform(transform);
	//AcGeVector3d rotVec{ 0.0,0.0,0.0 };
	transform.getCoordSystem(orgn, xa, ya, za);
	//AcString text; text.format(_T("%.3f %.3f %.3f %.3f ->"), orgn.x, orgn.y, orgn.z);
	AcString text; text.format(_T("%.2f %.2f -> %.6q2"), RTD(acos(xa.x)), RTD(asin(xa.y)), this->rotation());
	//AcString text2; text2.format(_T("%.3f %.3f %.3f %.3f ->"), ya.x, ya.y, ya.z);
	//AcString text3; text3.format(_T("%.3f %.3f %.3f %.3f"), za.x, za.y, za.z);
	//text = text + text1 + text2 + text3;
	
	AcGeVector3d displ{ AcGeVector3d(1.0,1.0,0.0) };
	mode->geometry().text(position() + displ.normalize() * 5.0, AcGeVector3d::kZAxis, AcGeVector3d::kXAxis,
		1.2, 1.0, 0.0, text);

	AcDbText::subViewportDraw(mode);
}

Adesk::UInt32 DRIPipeLabel::subViewportDrawLogicalFlags(AcGiViewportDraw * vd)
{
	assertReadEnabled();
	return (AcDbText::subViewportDrawLogicalFlags(vd));
}

Adesk::UInt32 DRIPipeLabel::subSetAttributes(AcGiDrawableTraits * traits)
{
	assertReadEnabled();
	return (AcDbText::subSetAttributes(traits));
}

//- Grip points protocol
Acad::ErrorStatus DRIPipeLabel::subGetGripPoints(
	AcGePoint3dArray & gripPoints, AcDbIntArray & osnapModes, AcDbIntArray & geomIds
) const
{
	assertReadEnabled();
	//----- This method is never called unless you return eNotImplemented 
	//----- from the new getGripPoints() method below (which is the default implementation)

	return (AcDbText::subGetGripPoints(gripPoints, osnapModes, geomIds));
}

Acad::ErrorStatus DRIPipeLabel::subMoveGripPointsAt(const AcDbIntArray & indices, const AcGeVector3d & offset)
{
	assertWriteEnabled();
	//----- This method is never called unless you return eNotImplemented 
	//----- from the new moveGripPointsAt() method below (which is the default implementation)

	return (AcDbText::subMoveGripPointsAt(indices, offset));
}

Acad::ErrorStatus DRIPipeLabel::subGetGripPoints(
	AcDbGripDataPtrArray & grips, const double curViewUnitSize, const int gripSize,
	const AcGeVector3d & curViewDir, const int bitflags
) const
{
	assertReadEnabled();

	//----- If you return eNotImplemented here, that will force AutoCAD to call
	//----- the older getGripPoints() implementation. The call below may return
	//----- eNotImplemented depending of your base class.
	return (AcDbText::subGetGripPoints(grips, curViewUnitSize, gripSize, curViewDir, bitflags));
}

Acad::ErrorStatus DRIPipeLabel::subMoveGripPointsAt(
	const AcDbVoidPtrArray & gripAppData, const AcGeVector3d & offset,
	const int bitflags
)
{
	assertWriteEnabled();

	//----- If you return eNotImplemented here, that will force AutoCAD to call
	//----- the older getGripPoints() implementation. The call below may return
	//----- eNotImplemented depending of your base class.
	return (AcDbText::subMoveGripPointsAt(gripAppData, offset, bitflags));
}

