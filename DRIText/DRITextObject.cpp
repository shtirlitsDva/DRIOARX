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
#include "DRITextObject.h"
#include <math.h>
#include "TrigMacros.h"

//-----------------------------------------------------------------------------
Adesk::UInt32 DRIText::kCurrentVersionNumber = 1;

//-----------------------------------------------------------------------------
ACRX_DXF_DEFINE_MEMBERS(
	DRIText, AcDbText,
	AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent,
	AcDbProxyEntity::kNoOperation, DRITEXT,
	DRITEXTOBJAPP
	| Product Desc : A description for your object
	| Company : Your company name
	| WEB Address : Your company WEB site address
)

//-----------------------------------------------------------------------------
DRIText::DRIText() : AcDbText()
{
	assertWriteEnabled();
	setJustification(AcTextAlignment::kTextAlignmentMiddleCenter);
}

DRIText::~DRIText()
{}

//-----------------------------------------------------------------------------
//----- AcDbObject protocols
//- Dwg Filing protocol
Acad::ErrorStatus DRIText::dwgOutFields(AcDbDwgFiler * pFiler) const
{
	assertReadEnabled();
	//----- Save parent class information first.
	Acad::ErrorStatus es = AcDbText::dwgOutFields(pFiler);
	if (es != Acad::eOk)
		return (es);
	//----- Object version number needs to be saved first
	if ((es = pFiler->writeUInt32(DRIText::kCurrentVersionNumber)) != Acad::eOk)
		return (es);
	//----- Output params
	//.....

	return (pFiler->filerStatus());
}

Acad::ErrorStatus DRIText::dwgInFields(AcDbDwgFiler * pFiler)
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
	if (version > DRIText::kCurrentVersionNumber)
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
Acad::ErrorStatus DRIText::dxfOutFields(AcDbDxfFiler * pFiler) const
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
	if ((es = pFiler->writeUInt32(kDxfInt32, DRIText::kCurrentVersionNumber)) != Acad::eOk)
		return (es);
	//----- Output params
	//.....

	return (pFiler->filerStatus());
}

Acad::ErrorStatus DRIText::dxfInFields(AcDbDxfFiler * pFiler)
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
	if (version > DRIText::kCurrentVersionNumber)
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
Acad::ErrorStatus DRIText::subOpen(AcDb::OpenMode mode)
{
	return (AcDbText::subOpen(mode));
}

Acad::ErrorStatus DRIText::subErase(Adesk::Boolean erasing)
{
	return (AcDbText::subErase(erasing));
}

Acad::ErrorStatus DRIText::subCancel()
{
	return (AcDbText::subCancel());
}

Acad::ErrorStatus DRIText::subClose()
{
	return (AcDbText::subClose());
}

//- Persistent reactor callbacks
void DRIText::openedForModify(const AcDbObject * pDbObj)
{
	assertReadEnabled();
	AcDbText::openedForModify(pDbObj);
}

void DRIText::cancelled(const AcDbObject * pDbObj)
{
	assertReadEnabled();
	AcDbText::cancelled(pDbObj);
}

void DRIText::objectClosed(const AcDbObjectId objId)
{
	assertReadEnabled();
	AcDbText::objectClosed(objId);
}

void DRIText::goodbye(const AcDbObject * pDbObj)
{
	assertReadEnabled();
	AcDbText::goodbye(pDbObj);
}

void DRIText::copied(const AcDbObject * pDbObj, const AcDbObject * pNewObj)
{
	assertReadEnabled();
	AcDbText::copied(pDbObj, pNewObj);
}

void DRIText::erased(const AcDbObject * pDbObj, Adesk::Boolean bErasing)
{
	assertReadEnabled();
	AcDbText::erased(pDbObj, bErasing);
}

void DRIText::modified(const AcDbObject * pDbObj)
{
	assertReadEnabled();
	AcDbText::modified(pDbObj);
}

void DRIText::modifiedGraphics(const AcDbEntity * pDbEnt)
{
	assertReadEnabled();
	AcDbText::modifiedGraphics(pDbEnt);
}

void DRIText::modifiedXData(const AcDbObject * pDbObj)
{
	assertReadEnabled();
	AcDbText::modifiedXData(pDbObj);
}

void DRIText::subObjModified(const AcDbObject * pMainbObj, const AcDbObject * pSubObj)
{
	assertReadEnabled();
	AcDbText::subObjModified(pMainbObj, pSubObj);
}

void DRIText::modifyUndone(const AcDbObject * pDbObj)
{
	assertReadEnabled();
	AcDbText::modifyUndone(pDbObj);
}

void DRIText::reappended(const AcDbObject * pDbObj)
{
	assertReadEnabled();
	AcDbText::reappended(pDbObj);
}

void DRIText::unappended(const AcDbObject * pDbObj)
{
	assertReadEnabled();
	AcDbText::unappended(pDbObj);
}

//-----------------------------------------------------------------------------
//----- AcDbEntity protocols
Adesk::Boolean DRIText::subWorldDraw(AcGiWorldDraw * mode)
{
	assertReadEnabled();
	//------ Returning Adesk::kFalse here will force viewportDraw() call
	return (Adesk::kFalse);
}

void DRIText::subViewportDraw(AcGiViewportDraw * mode)
{
	assertReadEnabled();
	AcGePoint3d orgn;
	AcGeVector3d xa, ya, za;
	AcGeMatrix3d transform;
	mode->viewport().getEyeToWorldTransform(transform);
	transform.getCoordSystem(orgn, xa, ya, za);

	double lLimit, uLimit, vpRotCos, vpRotSin;
	vpRotCos = acos(xa.x);
	vpRotSin = asin(xa.y);
	double rot = rotation();

	lLimit = (PI / 2) - vpRotCos;
	uLimit = (1.5 * PI) - vpRotCos;

	bool toRotate = false;

	if (vpRotCos < PI / 2 && vpRotSin <= 0)
	{
		if (rot > lLimit && rot < uLimit) toRotate = true;
	}
	else if (vpRotCos >= PI / 2 && vpRotCos < PI && vpRotSin < 0)
	{
		if (rot >= 0 && rot < uLimit) toRotate = true;
		else if (rot > lLimit + 2 * PI) toRotate = true;
	}
	else if (vpRotCos >= PI / 2 && vpRotCos <= PI && vpRotSin >= -0.0001)
	{
		lLimit = (PI / 2) - vpRotSin - PI;
		uLimit = (1.5 * PI) - vpRotSin - PI;
		if (rot > lLimit && rot < uLimit) toRotate = true;
		else if (rot > lLimit + 2 * PI) toRotate = true;
	}
	else if (vpRotCos < PI / 2 && vpRotSin > 0)
	{
		lLimit = vpRotSin + PI / 2;
		uLimit = vpRotSin + 3 * PI / 2;
		if (rot > lLimit && rot < uLimit) toRotate = true;
	}

	//AcString text; text.format(_T("vpRot:%.8f:%.8f --> L: %.3f > %.3f U: %.3f < %.3f"),
	//	vpRotCos, vpRotSin, rot, lLimit, rot, uLimit);
	//AcGeVector3d dirPerp{ AcGeVector3d(cos(rotation() + PI / 2),sin(rotation() + PI / 2),0.0) };
	//mode->geometry().text(position() + dirPerp.normalize() * 5.0, AcGeVector3d::kZAxis, dirPerp,//AcGeVector3d::kXAxis,
	//	1.2, 1.0, 0.0, text);

	if (toRotate)
	{
		AcGeVector3d dir{ AcGeVector3d(cos(rot),sin(rot),0.0) };
		AcGeVector3d dirPerp{ AcGeVector3d(cos(rot + PI / 2),sin(rot + PI / 2),0.0) };
		AcString text = textString();
		AcGiTextStyle style;
		fromAcDbTextStyle(style, textStyle());
		AcGePoint2d extents = style.extents(textString(), Adesk::kFalse, text.length(), Adesk::kTrue);

		mode->geometry().text(position() + dir * extents.x + dirPerp * extents.y,
			AcGeVector3d::kZAxis, dir * -1, height(), 1.0, 0.0, text);
	}
	else AcDbText::subViewportDraw(mode);
}

Adesk::UInt32 DRIText::subViewportDrawLogicalFlags(AcGiViewportDraw * vd)
{
	assertReadEnabled();
	return (AcDbText::subViewportDrawLogicalFlags(vd));
}

Adesk::UInt32 DRIText::subSetAttributes(AcGiDrawableTraits * traits)
{
	assertReadEnabled();
	return (AcDbText::subSetAttributes(traits));
}

//- Grip points protocol
Acad::ErrorStatus DRIText::subGetGripPoints(
	AcGePoint3dArray & gripPoints, AcDbIntArray & osnapModes, AcDbIntArray & geomIds
) const
{
	assertReadEnabled();
	//----- This method is never called unless you return eNotImplemented 
	//----- from the new getGripPoints() method below (which is the default implementation)

	return (AcDbText::subGetGripPoints(gripPoints, osnapModes, geomIds));
}

Acad::ErrorStatus DRIText::subMoveGripPointsAt(const AcDbIntArray & indices, const AcGeVector3d & offset)
{
	assertWriteEnabled();
	//----- This method is never called unless you return eNotImplemented 
	//----- from the new moveGripPointsAt() method below (which is the default implementation)

	return (AcDbText::subMoveGripPointsAt(indices, offset));
}

Acad::ErrorStatus DRIText::subGetGripPoints(
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

Acad::ErrorStatus DRIText::subMoveGripPointsAt(
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

