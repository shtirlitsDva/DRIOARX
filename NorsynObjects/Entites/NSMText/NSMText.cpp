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
//----- NSMText.cpp : Implementation of NSMText
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "NSMText.h"
#include <math.h>
#include <vector>

//-----------------------------------------------------------------------------
static const double kPi = 3.14159265358979323846;

//-----------------------------------------------------------------------------
Adesk::UInt32 NSMText::kCurrentVersionNumber = 1;

//-----------------------------------------------------------------------------
ACRX_DXF_DEFINE_MEMBERS(
	NSMText, AcDbMText,
	AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent,
	AcDbProxyEntity::kNoOperation, NSMTEXT,
	NSNORSYNOBJECTSAPP
	| Product Desc : A description for your object
	| Company : Your company name
	| WEB Address : Your company WEB site address
)

//-----------------------------------------------------------------------------
NSMText::NSMText() : AcDbMText() {
}

NSMText::~NSMText() {
}

//-----------------------------------------------------------------------------
//----- AcDbObject protocols
//- Dwg Filing protocol
Acad::ErrorStatus NSMText::dwgOutFields(AcDbDwgFiler * pFiler) const {
	assertReadEnabled();
	//----- Save parent class information first.
	Acad::ErrorStatus es = AcDbMText::dwgOutFields(pFiler);
	if (es != Acad::eOk)
		return (es);
	//----- Object version number needs to be saved first
	if ((es = pFiler->writeUInt32(NSMText::kCurrentVersionNumber)) != Acad::eOk)
		return (es);
	//----- Output params
	//.....

	return (pFiler->filerStatus());
}

Acad::ErrorStatus NSMText::dwgInFields(AcDbDwgFiler * pFiler) {
	assertWriteEnabled();
	//----- Read parent class information first.
	Acad::ErrorStatus es = AcDbMText::dwgInFields(pFiler);
	if (es != Acad::eOk)
		return (es);
	//----- Object version number needs to be read first
	Adesk::UInt32 version = 0;
	if ((es = pFiler->readUInt32(&version)) != Acad::eOk)
		return (es);
	if (version > NSMText::kCurrentVersionNumber)
		return (Acad::eMakeMeProxy);
	//- Uncomment the 2 following lines if your current object implementation cannot
	//- support previous version of that object.
	//if ( version < NSMText::kCurrentVersionNumber )
	//	return (Acad::eMakeMeProxy) ;
	//----- Read params
	//.....

	return (pFiler->filerStatus());
}

//- Dxf Filing protocol
Acad::ErrorStatus NSMText::dxfOutFields(AcDbDxfFiler * pFiler) const {
	assertReadEnabled();
	//----- Save parent class information first.
	Acad::ErrorStatus es = AcDbMText::dxfOutFields(pFiler);
	if (es != Acad::eOk)
		return (es);
	es = pFiler->writeItem(AcDb::kDxfSubclass, _RXST("NSMText"));
	if (es != Acad::eOk)
		return (es);
	//----- Object version number needs to be saved first
	if ((es = pFiler->writeUInt32(kDxfInt32, NSMText::kCurrentVersionNumber)) != Acad::eOk)
		return (es);
	//----- Output params
	//.....

	return (pFiler->filerStatus());
}

Acad::ErrorStatus NSMText::dxfInFields(AcDbDxfFiler * pFiler) {
	assertWriteEnabled();
	//----- Read parent class information first.
	Acad::ErrorStatus es = AcDbMText::dxfInFields(pFiler);
	if (es != Acad::eOk || !pFiler->atSubclassData(_RXST("NSMText")))
		return (pFiler->filerStatus());
	//----- Object version number needs to be read first
	struct resbuf rb;
	pFiler->readItem(&rb);
	if (rb.restype != AcDb::kDxfInt32) {
		pFiler->pushBackItem();
		pFiler->setError(Acad::eInvalidDxfCode, _RXST("\nError: expected group code %d (version #)"), AcDb::kDxfInt32);
		return (pFiler->filerStatus());
	}
	Adesk::UInt32 version = (Adesk::UInt32)rb.resval.rlong;
	if (version > NSMText::kCurrentVersionNumber)
		return (Acad::eMakeMeProxy);
	//- Uncomment the 2 following lines if your current object implementation cannot
	//- support previous version of that object.
	//if ( version < NSMText::kCurrentVersionNumber )
	//	return (Acad::eMakeMeProxy) ;
	//----- Read params in non order dependant manner
#pragma warning( push )
#pragma warning( disable: 4065 )
	while (es == Acad::eOk && (es = pFiler->readResBuf(&rb)) == Acad::eOk) {
		switch (rb.restype) {
		default:
			//----- An unrecognized group. Push it back so that the subclass can read it again.
			pFiler->pushBackItem();
			es = Acad::eEndOfFile;
			break;
		}
	}
#pragma warning( pop )
	//----- At this point the es variable must contain eEndOfFile
	//----- - either from readResBuf() or from pushback. If not,
	//----- it indicates that an error happened and we should
	//----- return immediately.
	if (es != Acad::eEndOfFile)
		return (Acad::eInvalidResBuf);

	return (pFiler->filerStatus());
}

//-----------------------------------------------------------------------------
//----- AcDbEntity protocols
Adesk::Boolean NSMText::subWorldDraw(AcGiWorldDraw * mode) {
	assertReadEnabled();

	double rot = rotation();

	bool isPlotting = mode->context()->isPlotGeneration();

	AcGeVector3d dir(cos(rot), sin(rot), 0.0);
	dir.normalize();
	AcGeVector3d dirPerp(cos(rot + kPi * 0.5), sin(rot + kPi * 0.5), 0.0);
	dirPerp.normalize();

	//----- Draw background polygon for text masking
	AcGePoint3d pVertexList[4];
	pVertexList[0] = AcGePoint3d() + dir * actualWidth() / 2 + dirPerp * actualHeight() / 2;
	pVertexList[1] = AcGePoint3d() - dir * actualWidth() / 2 + dirPerp * actualHeight() / 2;
	pVertexList[2] = AcGePoint3d() - dir * actualWidth() / 2 - dirPerp * actualHeight() / 2;
	pVertexList[3] = AcGePoint3d() + dir * actualWidth() / 2 - dirPerp * actualHeight() / 2;

	Adesk::UInt32 numPolygonPoints[1] = { 4 };
	Adesk::UInt32 numPolygonIndices = 1;
	AcGePoint3d polygonPositions[1] = { location() };
	const AcCmEntityColor* outlineColors = nullptr;
	const AcGiLineType* outlineTypes = nullptr;

	AcCmEntityColor fillColor;
	if (isPlotting)
		fillColor = AcCmEntityColor(250, 250, 250);
	else
		fillColor = AcCmEntityColor(0, 0, 0);
	AcCmEntityColor fillColors[1] = { fillColor };
	AcCmTransparency fillOpacities[1] = { AcCmTransparency(Adesk::UInt8(255)) };

	mode->geometry().polyPolygon(
		numPolygonIndices,
		numPolygonPoints,
		polygonPositions,
		numPolygonPoints,
		pVertexList,
		outlineColors,
		outlineTypes,
		fillColors,
		fillOpacities
	);

	//------ Returning Adesk::kFalse here will force viewportDraw() call
	return (Adesk::kFalse);
}

void NSMText::subViewportDraw(AcGiViewportDraw * mode) {
	assertReadEnabled();

	//----- Get viewport camera up vector
	AcGeVector3d upVect;
	mode->viewport().getCameraUpVector(upVect);
	double rot = rotation();
	double vpRot = atan2(upVect.y, upVect.x);

	//----- Handle plotting
	bool isPlotting = mode->context()->isPlotGeneration();
	if (isPlotting) {
		vpRot -= kPi / 2.0;
		mode->subEntityTraits().setColor(0);
	}

	//----- Quadrant-based upside-down detection
	std::vector<bool> upsideDownQuadrants = { false, false, true, true };
	double diffRot = vpRot - rot;

	//----- Normalize to [-PI, PI]
	while (diffRot > kPi) diffRot -= 2.0 * kPi;
	while (diffRot < -kPi) diffRot += 2.0 * kPi;

	//----- Determine quadrant
	int quadrant;
	if (diffRot > 0) {
		quadrant = (diffRot <= kPi / 2.0) ? 0 : 1;
	}
	else {
		quadrant = (diffRot >= -kPi / 2.0) ? 3 : 2;
	}

	bool isUpsideDown = upsideDownQuadrants[quadrant];

	//----- Direction vectors
	AcGeVector3d dir(cos(rot), sin(rot), 0.0);
	dir.normalize();
	AcGeVector3d dirPerp(cos(rot + kPi * 0.5), sin(rot + kPi * 0.5), 0.0);
	dirPerp.normalize();

	AcString text = contents();

	if (isUpsideDown) {
		//----- Draw text rotated 180 degrees around center
		mode->geometry().text(
			location() + dir * actualWidth() / 2 + dirPerp * actualHeight() / 2,
			AcGeVector3d::kZAxis,
			dir * -1.0,
			textHeight(),
			1.0,
			0.0,
			text
		);
	}
	else {
		//----- Draw text normally
		mode->geometry().text(
			location() - dir * actualWidth() / 2 - dirPerp * actualHeight() / 2,
			AcGeVector3d::kZAxis,
			dir,
			textHeight(),
			1.0,
			0.0,
			text
		);
	}
}

Adesk::UInt32 NSMText::subViewportDrawLogicalFlags(AcGiViewportDraw * vd) {
	assertReadEnabled();
	return (AcDbMText::subViewportDrawLogicalFlags(vd));
}

Adesk::UInt32 NSMText::subSetAttributes(AcGiDrawableTraits * traits) {
	assertReadEnabled();
	return (AcDbMText::subSetAttributes(traits));
}
