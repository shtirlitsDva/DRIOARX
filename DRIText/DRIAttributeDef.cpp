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
//----- DRIAttributeDef.cpp : Implementation of DRIAttributeDef
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "DRIAttributeDef.h"
#include <math.h>
#include "TrigMacros.h"

//-----------------------------------------------------------------------------
Adesk::UInt32 DRIAttributeDef::kCurrentVersionNumber = 1;

//-----------------------------------------------------------------------------
ACRX_DXF_DEFINE_MEMBERS(
	DRIAttributeDef, AcDbAttributeDefinition,
	AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent,
	AcDbProxyEntity::kNoOperation, DRIATTRIBUTEDEF,
	DRIDRITEXTAPP
	| Product Desc : A description for your object
	| Company : Your company name
	| WEB Address : Your company WEB site address
)

//-----------------------------------------------------------------------------
DRIAttributeDef::DRIAttributeDef() : AcDbAttributeDefinition()
{}

DRIAttributeDef::~DRIAttributeDef()
{}

//-----------------------------------------------------------------------------
//----- AcDbObject protocols
//- Dwg Filing protocol
Acad::ErrorStatus DRIAttributeDef::dwgOutFields(AcDbDwgFiler * pFiler) const
{
	assertReadEnabled();
	//----- Save parent class information first.
	Acad::ErrorStatus es = AcDbAttributeDefinition::dwgOutFields(pFiler);
	if (es != Acad::eOk)
		return (es);
	//----- Object version number needs to be saved first
	if ((es = pFiler->writeUInt32(DRIAttributeDef::kCurrentVersionNumber)) != Acad::eOk)
		return (es);
	//----- Output params
	//.....

	return (pFiler->filerStatus());
}

Acad::ErrorStatus DRIAttributeDef::dwgInFields(AcDbDwgFiler * pFiler)
{
	assertWriteEnabled();
	//----- Read parent class information first.
	Acad::ErrorStatus es = AcDbAttributeDefinition::dwgInFields(pFiler);
	if (es != Acad::eOk)
		return (es);
	//----- Object version number needs to be read first
	Adesk::UInt32 version = 0;
	if ((es = pFiler->readUInt32(&version)) != Acad::eOk)
		return (es);
	if (version > DRIAttributeDef::kCurrentVersionNumber)
		return (Acad::eMakeMeProxy);
	//- Uncomment the 2 following lines if your current object implementation cannot
	//- support previous version of that object.
	//if ( version < DRIAttributeDef::kCurrentVersionNumber )
	//	return (Acad::eMakeMeProxy) ;
	//----- Read params
	//.....

	return (pFiler->filerStatus());
}

//- Dxf Filing protocol
Acad::ErrorStatus DRIAttributeDef::dxfOutFields(AcDbDxfFiler * pFiler) const
{
	assertReadEnabled();
	//----- Save parent class information first.
	Acad::ErrorStatus es = AcDbAttributeDefinition::dxfOutFields(pFiler);
	if (es != Acad::eOk)
		return (es);
	es = pFiler->writeItem(AcDb::kDxfSubclass, _RXST("DRIAttributeDef"));
	if (es != Acad::eOk)
		return (es);
	//----- Object version number needs to be saved first
	if ((es = pFiler->writeUInt32(kDxfInt32, DRIAttributeDef::kCurrentVersionNumber)) != Acad::eOk)
		return (es);
	//----- Output params
	//.....

	return (pFiler->filerStatus());
}

Acad::ErrorStatus DRIAttributeDef::dxfInFields(AcDbDxfFiler * pFiler)
{
	assertWriteEnabled();
	//----- Read parent class information first.
	Acad::ErrorStatus es = AcDbAttributeDefinition::dxfInFields(pFiler);
	if (es != Acad::eOk || !pFiler->atSubclassData(_RXST("DRIAttributeDef")))
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
	if (version > DRIAttributeDef::kCurrentVersionNumber)
		return (Acad::eMakeMeProxy);
	//- Uncomment the 2 following lines if your current object implementation cannot
	//- support previous version of that object.
	//if ( version < DRIAttributeDef::kCurrentVersionNumber )
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
Acad::ErrorStatus DRIAttributeDef::subOpen(AcDb::OpenMode mode)
{
	return (AcDbAttributeDefinition::subOpen(mode));
}

Acad::ErrorStatus DRIAttributeDef::subErase(Adesk::Boolean erasing)
{
	return (AcDbAttributeDefinition::subErase(erasing));
}

Acad::ErrorStatus DRIAttributeDef::subCancel()
{
	return (AcDbAttributeDefinition::subCancel());
}

Acad::ErrorStatus DRIAttributeDef::subClose()
{
	return (AcDbAttributeDefinition::subClose());
}

//- Persistent reactor callbacks
void DRIAttributeDef::openedForModify(const AcDbObject * pDbObj)
{
	assertReadEnabled();
	AcDbAttributeDefinition::openedForModify(pDbObj);
}

void DRIAttributeDef::cancelled(const AcDbObject * pDbObj)
{
	assertReadEnabled();
	AcDbAttributeDefinition::cancelled(pDbObj);
}

void DRIAttributeDef::objectClosed(const AcDbObjectId objId)
{
	assertReadEnabled();
	AcDbAttributeDefinition::objectClosed(objId);
}

void DRIAttributeDef::goodbye(const AcDbObject * pDbObj)
{
	assertReadEnabled();
	AcDbAttributeDefinition::goodbye(pDbObj);
}

void DRIAttributeDef::copied(const AcDbObject * pDbObj, const AcDbObject * pNewObj)
{
	assertReadEnabled();
	AcDbAttributeDefinition::copied(pDbObj, pNewObj);
}

void DRIAttributeDef::erased(const AcDbObject * pDbObj, Adesk::Boolean bErasing)
{
	assertReadEnabled();
	AcDbAttributeDefinition::erased(pDbObj, bErasing);
}

void DRIAttributeDef::modified(const AcDbObject * pDbObj)
{
	assertReadEnabled();
	AcDbAttributeDefinition::modified(pDbObj);
}

void DRIAttributeDef::modifiedGraphics(const AcDbEntity * pDbEnt)
{
	assertReadEnabled();
	AcDbAttributeDefinition::modifiedGraphics(pDbEnt);
}

void DRIAttributeDef::modifiedXData(const AcDbObject * pDbObj)
{
	assertReadEnabled();
	AcDbAttributeDefinition::modifiedXData(pDbObj);
}

void DRIAttributeDef::subObjModified(const AcDbObject * pMainbObj, const AcDbObject * pSubObj)
{
	assertReadEnabled();
	AcDbAttributeDefinition::subObjModified(pMainbObj, pSubObj);
}

void DRIAttributeDef::modifyUndone(const AcDbObject * pDbObj)
{
	assertReadEnabled();
	AcDbAttributeDefinition::modifyUndone(pDbObj);
}

void DRIAttributeDef::reappended(const AcDbObject * pDbObj)
{
	assertReadEnabled();
	AcDbAttributeDefinition::reappended(pDbObj);
}

void DRIAttributeDef::unappended(const AcDbObject * pDbObj)
{
	assertReadEnabled();
	AcDbAttributeDefinition::unappended(pDbObj);
}

//----- deepClone
Acad::ErrorStatus DRIAttributeDef::subDeepClone(AcDbObject * pOwnerObject, AcDbObject * &pClonedObject, AcDbIdMapping & idMap, Adesk::Boolean isPrimary) const
{
	assertReadEnabled();
	return (AcDbAttributeDefinition::subDeepClone(pOwnerObject, pClonedObject, idMap, isPrimary));
}

//----- wblockClone
Acad::ErrorStatus DRIAttributeDef::subWblockClone(AcRxObject * pOwnerObject, AcDbObject * &pClonedObject, AcDbIdMapping & idMap, Adesk::Boolean isPrimary) const
{
	assertReadEnabled();
	return (AcDbAttributeDefinition::subWblockClone(pOwnerObject, pClonedObject, idMap, isPrimary));
}

//-----------------------------------------------------------------------------
//----- AcDbEntity protocols
Adesk::Boolean DRIAttributeDef::subWorldDraw(AcGiWorldDraw * mode)
{
	assertReadEnabled();
	//------ Returning Adesk::kFalse here will force viewportDraw() call
	return (Adesk::kFalse);
}

void DRIAttributeDef::subViewportDraw(AcGiViewportDraw * mode)
{
	////See this article in case of problems with print
	////https://adn-cis.org/risovanie-gorizontalnogo-teksta-v-sobstvennom-primitive.html
	////http://adndevblog.typepad.com/autocad/2012/08/draw-horizontal-text-in-custom-entity.html
	assertReadEnabled();
	AcGePoint3d orgn;
	AcGeVector3d xa, ya, za;
	AcGeVector3d upVect;
	AcGeMatrix3d transform;
	//mode->viewport().getCameraUpVector(upVect);
	mode->viewport().getEyeToWorldTransform(transform);
	transform.getCoordSystem(orgn, xa, ya, za);

	double lLimit, uLimit;
	double rot = rotation();
	double vpRot = atan2(xa.y, xa.x);
	//double vpRot = atan2(upVect.x, upVect.y);
	//Counterclockwise rotation
	if (vpRot < 0) vpRot *= -1;
	else vpRot = 2.0 * PI - vpRot;
	//Limit modifiers
	double lLimMod = PI * 0.5;
	double uLimMod = PI * 1.5;

	//When plotting, works, probably, only for landscape setups
	if (mode->context()->isPlotGeneration())
	{
		vpRot += 0.5 * PI;
		//lLimMod += 0.5 * PI; uLimMod += 0.5 * PI;
	}
	else
	{
		//vpRot += 0.5 * PI;
	}

	lLimit = lLimMod - vpRot;
	uLimit = uLimMod - vpRot;
	if (lLimit < 0) lLimit += 2.0 * PI;
	if (uLimit <= 0) uLimit += 2.0 * PI;

	bool toRotate = false;
	//AcString helperText;
	//General case, quadrant I and IV
	if ((vpRot < (PI * 0.5)) || (vpRot >= (1.5 * PI)))
	{
		/*helperText.format(_T("vpRot_1: %.3f --> L: %.3q2 > %.3q2 && U: %.3q2 < %.3q2"),
			vpRot, rot, lLimit, rot, uLimit);*/
		if (rot > lLimit && rot < uLimit) toRotate = true;
	}
	//General case, quadrant II and III
	else if ((vpRot >= (PI * 0.5)) && (vpRot < (1.5 * PI)))
	{
		//helperText.format(_T("vpRot_2: %.3f --> L: %.3q2 > %.3q2 || U: %.3q2 < %.3q2"),
		//	vpRot, rot, lLimit, rot, uLimit);
		if (rot < uLimit || rot > lLimit) toRotate = true;
	}

	//AcGeVector3d dirPerp{ AcGeVector3d(cos(rot + PI * 0.5),sin(rot + PI * 0.5),0.0) };
	//mode->geometry().text(position() + dirPerp.normalize() * 5.0, AcGeVector3d::kZAxis, dirPerp,//AcGeVector3d::kXAxis,
	//	1.2, 1.0, 0.0, helperText);

	if (toRotate)
	{
		AcGeVector3d dir{ AcGeVector3d(cos(rot),sin(rot),0.0) }; dir.normalize();
		AcGeVector3d dirPerp{ AcGeVector3d(cos(rot + PI * 0.5),sin(rot + PI * 0.5),0.0) }; dirPerp.normalize();
		AcString text = textString();
		AcGiTextStyle style;
		fromAcDbTextStyle(style, this->textStyle());
		style.loadStyleRec();
		AcGePoint2d extents = style.extents(textString(), Adesk::kFalse, text.length(), Adesk::kTrue);
		//AcString ext; ext.format(_T(" *"));//ext.format(_T(" x: %.3f, y: %.3f"), extents.x, extents.y);
		//text.append(ext);
		mode->geometry().text(position() + dir * extents.x + dirPerp * extents.y,
			AcGeVector3d::kZAxis, dir * -1, height(), 1.0, 0.0, text);
	}
	else AcDbAttributeDefinition::subViewportDraw(mode);
}

Adesk::UInt32 DRIAttributeDef::subViewportDrawLogicalFlags(AcGiViewportDraw * vd)
{
	assertReadEnabled();
	return (AcDbAttributeDefinition::subViewportDrawLogicalFlags(vd));
}

Adesk::UInt32 DRIAttributeDef::subSetAttributes(AcGiDrawableTraits * traits)
{
	assertReadEnabled();
	return (AcDbAttributeDefinition::subSetAttributes(traits));
}

//- Osnap points protocol
Acad::ErrorStatus DRIAttributeDef::subGetOsnapPoints(
	AcDb::OsnapMode osnapMode,
	Adesk::GsMarker gsSelectionMark,
	const AcGePoint3d & pickPoint,
	const AcGePoint3d & lastPoint,
	const AcGeMatrix3d & viewXform,
	AcGePoint3dArray & snapPoints,
	AcDbIntArray & geomIds) const
{
	assertReadEnabled();
	return (AcDbAttributeDefinition::subGetOsnapPoints(osnapMode, gsSelectionMark, pickPoint, lastPoint, viewXform, snapPoints, geomIds));
}

Acad::ErrorStatus DRIAttributeDef::subGetOsnapPoints(
	AcDb::OsnapMode osnapMode,
	Adesk::GsMarker gsSelectionMark,
	const AcGePoint3d & pickPoint,
	const AcGePoint3d & lastPoint,
	const AcGeMatrix3d & viewXform,
	AcGePoint3dArray & snapPoints,
	AcDbIntArray & geomIds,
	const AcGeMatrix3d & insertionMat) const
{
	assertReadEnabled();
	return (AcDbAttributeDefinition::subGetOsnapPoints(osnapMode, gsSelectionMark, pickPoint, lastPoint, viewXform, snapPoints, geomIds, insertionMat));
}

//- Grip points protocol
Acad::ErrorStatus DRIAttributeDef::subGetGripPoints(
	AcGePoint3dArray & gripPoints, AcDbIntArray & osnapModes, AcDbIntArray & geomIds
) const
{
	assertReadEnabled();
	//----- This method is never called unless you return eNotImplemented 
	//----- from the new getGripPoints() method below (which is the default implementation)

	return (AcDbAttributeDefinition::subGetGripPoints(gripPoints, osnapModes, geomIds));
}

Acad::ErrorStatus DRIAttributeDef::subMoveGripPointsAt(const AcDbIntArray & indices, const AcGeVector3d & offset)
{
	assertWriteEnabled();
	//----- This method is never called unless you return eNotImplemented 
	//----- from the new moveGripPointsAt() method below (which is the default implementation)

	return (AcDbAttributeDefinition::subMoveGripPointsAt(indices, offset));
}

Acad::ErrorStatus DRIAttributeDef::subGetGripPoints(
	AcDbGripDataPtrArray & grips, const double curViewUnitSize, const int gripSize,
	const AcGeVector3d & curViewDir, const int bitflags
) const
{
	assertReadEnabled();

	//----- If you return eNotImplemented here, that will force AutoCAD to call
	//----- the older getGripPoints() implementation. The call below may return
	//----- eNotImplemented depending of your base class.
	return (AcDbAttributeDefinition::subGetGripPoints(grips, curViewUnitSize, gripSize, curViewDir, bitflags));
}

Acad::ErrorStatus DRIAttributeDef::subMoveGripPointsAt(
	const AcDbVoidPtrArray & gripAppData, const AcGeVector3d & offset,
	const int bitflags
)
{
	assertWriteEnabled();

	//----- If you return eNotImplemented here, that will force AutoCAD to call
	//----- the older getGripPoints() implementation. The call below may return
	//----- eNotImplemented depending of your base class.
	return (AcDbAttributeDefinition::subMoveGripPointsAt(gripAppData, offset, bitflags));
}

