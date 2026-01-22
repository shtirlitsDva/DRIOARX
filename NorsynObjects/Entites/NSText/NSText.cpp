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
//----- NSText.cpp : Implementation of NSText
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "NSText.h"
#include <math.h>
#include <vector>

//-----------------------------------------------------------------------------
Adesk::UInt32 NSText::kCurrentVersionNumber = 1;

//-----------------------------------------------------------------------------
ACRX_DXF_DEFINE_MEMBERS(
	NSText, AcDbText,
	AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent,
	AcDbProxyEntity::kNoOperation, NSTEXT,
	NSNORSYNOBJECTSAPP
	| Product Desc : A description for your object
	| Company : Your company name
	| WEB Address : Your company WEB site address
)

//-----------------------------------------------------------------------------
NSText::NSText() : AcDbText() {
}

NSText::~NSText() {
}

//-----------------------------------------------------------------------------
//----- AcDbObject protocols
//- Dwg Filing protocol
Acad::ErrorStatus NSText::dwgOutFields(AcDbDwgFiler * pFiler) const {
	assertReadEnabled();
	//----- Save parent class information first.
	Acad::ErrorStatus es = AcDbText::dwgOutFields(pFiler);
	if (es != Acad::eOk)
		return (es);
	//----- Object version number needs to be saved first
	if ((es = pFiler->writeUInt32(NSText::kCurrentVersionNumber)) != Acad::eOk)
		return (es);
	//----- Output params
	//.....

	return (pFiler->filerStatus());
}

Acad::ErrorStatus NSText::dwgInFields(AcDbDwgFiler * pFiler) {
	assertWriteEnabled();
	//----- Read parent class information first.
	Acad::ErrorStatus es = AcDbText::dwgInFields(pFiler);
	if (es != Acad::eOk)
		return (es);
	//----- Object version number needs to be read first
	Adesk::UInt32 version = 0;
	if ((es = pFiler->readUInt32(&version)) != Acad::eOk)
		return (es);
	if (version > NSText::kCurrentVersionNumber)
		return (Acad::eMakeMeProxy);
	//- Uncomment the 2 following lines if your current object implementation cannot
	//- support previous version of that object.
	//if ( version < NSText::kCurrentVersionNumber )
	//	return (Acad::eMakeMeProxy) ;
	//----- Read params
	//.....

	return (pFiler->filerStatus());
}

//- Dxf Filing protocol
Acad::ErrorStatus NSText::dxfOutFields(AcDbDxfFiler * pFiler) const {
	assertReadEnabled();
	//----- Save parent class information first.
	Acad::ErrorStatus es = AcDbText::dxfOutFields(pFiler);
	if (es != Acad::eOk)
		return (es);
	es = pFiler->writeItem(AcDb::kDxfSubclass, _RXST("NSText"));
	if (es != Acad::eOk)
		return (es);
	//----- Object version number needs to be saved first
	if ((es = pFiler->writeUInt32(kDxfInt32, NSText::kCurrentVersionNumber)) != Acad::eOk)
		return (es);
	//----- Output params
	//.....

	return (pFiler->filerStatus());
}

Acad::ErrorStatus NSText::dxfInFields(AcDbDxfFiler * pFiler) {
	assertWriteEnabled();
	//----- Read parent class information first.
	Acad::ErrorStatus es = AcDbText::dxfInFields(pFiler);
	if (es != Acad::eOk || !pFiler->atSubclassData(_RXST("NSText")))
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
	if (version > NSText::kCurrentVersionNumber)
		return (Acad::eMakeMeProxy);
	//- Uncomment the 2 following lines if your current object implementation cannot
	//- support previous version of that object.
	//if ( version < NSText::kCurrentVersionNumber )
	//	return (Acad::eMakeMeProxy) ;
	//----- Read params in non order dependant manner
//----- Suppress C4065: "switch statement contains 'default' but no 'case' labels"
//----- This is standard ObjectARX boilerplate - case labels would be added here
//----- when NSText has custom properties to serialize via DXF.
#pragma warning( push )
#pragma warning( disable: 4065 )
	while (es == Acad::eOk && (es = pFiler->readResBuf(&rb)) == Acad::eOk) {
		switch (rb.restype) {
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
Adesk::Boolean NSText::subWorldDraw(AcGiWorldDraw * mode) {
	assertReadEnabled();
	//------ Returning Adesk::kFalse here will force viewportDraw() call
	return (Adesk::kFalse);
}

void NSText::subViewportDraw(AcGiViewportDraw * mode) {
	assertReadEnabled();

	const double kPi = 3.14159265358979323846;

	//----- Get viewport camera up vector (same approach as DRIMTextObj)
	AcGeVector3d upVect;
	mode->viewport().getCameraUpVector(upVect);
	double rot = rotation();
	double vpRot = atan2(upVect.y, upVect.x);

	//----- Handle plotting
	if (mode->context()->isPlotGeneration()) {
		vpRot -= kPi / 2.0;
	}

	//----- Quadrant-based upside-down detection (from DRIMTextObj)
	std::vector<bool> upsideDownQuadrants = { false, false, true, true };
	double diffRot = vpRot - rot;

	//----- Normalize to [-PI, PI]
	while (diffRot > kPi) diffRot -= 2.0 * kPi;
	while (diffRot < -kPi) diffRot += 2.0 * kPi;

	//----- Determine quadrant
	int quadrant;
	if (diffRot > 0) {
		quadrant = (diffRot <= kPi / 2.0) ? 0 : 1;
	} else {
		quadrant = (diffRot >= -kPi / 2.0) ? 3 : 2;
	}

	bool isUpsideDown = upsideDownQuadrants[quadrant];

	if (isUpsideDown) {
		//----- Get actual geometric center using getGeomExtents()
		//----- This works regardless of text alignment/justification
		AcDbExtents ext;
		if (getGeomExtents(ext) != Acad::eOk) {
			AcDbText::subViewportDraw(mode);
			return;
		}

		//----- Calculate true geometric center from world-space bounding box
		AcGePoint3d center = ext.minPoint() + (ext.maxPoint() - ext.minPoint()) * 0.5;

		//----- Direction vectors for the text's rotation
		AcGeVector3d dir(cos(rot), sin(rot), 0.0);
		dir.normalize();
		AcGeVector3d dirPerp(cos(rot + kPi * 0.5), sin(rot + kPi * 0.5), 0.0);
		dirPerp.normalize();

		//----- Get text string
		AcString text;
		textString(text);

		//----- Use extentsBox() to get precise text bounds in local coordinates
		//----- This accounts for baseline offset and actual character bounds
		AcGiTextStyle style;
		fromAcDbTextStyle(style, textStyle());
		style.loadStyleRec();

		//----- Scale style to match entity's actual height
		double styleHeight = style.textSize();
		if (styleHeight <= 0.0) styleHeight = 1.0;
		double scaleFactor = height() / styleHeight;

		AcGePoint2d extMin, extMax;
		if (style.extentsBox(text, Adesk::kFalse, -1, Adesk::kFalse, extMin, extMax) != Acad::eOk) {
			AcDbText::subViewportDraw(mode);
			return;
		}

		//----- Calculate center offset in text's local coordinate system
		//----- Scale by height ratio to match actual rendered size
		double centerOffsetX = ((extMin.x + extMax.x) / 2.0) * scaleFactor;
		double centerOffsetY = ((extMin.y + extMax.y) / 2.0) * scaleFactor;

		//----- For flipped text (direction = -dir):
		//----- Local axes are reversed, so we ADD the offset instead of subtract
		AcGePoint3d insertionPt = center + dir * centerOffsetX + dirPerp * centerOffsetY;

		mode->geometry().text(
			insertionPt,
			AcGeVector3d::kZAxis,
			dir * -1.0,
			height(),
			1.0,
			0.0,
			text
		);
		return;
	}

	AcDbText::subViewportDraw(mode);
}

Adesk::UInt32 NSText::subViewportDrawLogicalFlags(AcGiViewportDraw * vd) {
	assertReadEnabled();
	return (AcDbText::subViewportDrawLogicalFlags(vd));
}

Adesk::UInt32 NSText::subSetAttributes(AcGiDrawableTraits * traits) {
	assertReadEnabled();
	return (AcDbText::subSetAttributes(traits));
}


