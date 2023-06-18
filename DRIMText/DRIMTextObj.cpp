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
//----- DRIMTextObj.cpp : Implementation of DRIMTextObj
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "DRIMTextObj.h"
#include <math.h>
#include "TrigMacros.h"

//-----------------------------------------------------------------------------
Adesk::UInt32 DRIMTextObj::kCurrentVersionNumber = 1;

//-----------------------------------------------------------------------------
ACRX_DXF_DEFINE_MEMBERS(
	DRIMTextObj, AcDbMText,
	AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent,
	AcDbProxyEntity::kNoOperation, DRIMTEXTOBJ,
	DRIDRIMTEXTAPP
	| Product Desc : A description for your object
	| Company : Your company name
	| WEB Address : Your company WEB site address
)

//-----------------------------------------------------------------------------
DRIMTextObj::DRIMTextObj() : AcDbMText() {
}

DRIMTextObj::~DRIMTextObj() {
}

//-----------------------------------------------------------------------------
//----- AcDbObject protocols
//- Dwg Filing protocol
Acad::ErrorStatus DRIMTextObj::dwgOutFields(AcDbDwgFiler * pFiler) const {
	assertReadEnabled();
	//----- Save parent class information first.
	Acad::ErrorStatus es = AcDbMText::dwgOutFields(pFiler);
	if (es != Acad::eOk)
		return (es);
	//----- Object version number needs to be saved first
	if ((es = pFiler->writeUInt32(DRIMTextObj::kCurrentVersionNumber)) != Acad::eOk)
		return (es);
	//----- Output params
	//.....

	return (pFiler->filerStatus());
}

Acad::ErrorStatus DRIMTextObj::dwgInFields(AcDbDwgFiler * pFiler) {
	assertWriteEnabled();
	//----- Read parent class information first.
	Acad::ErrorStatus es = AcDbMText::dwgInFields(pFiler);
	if (es != Acad::eOk)
		return (es);
	//----- Object version number needs to be read first
	Adesk::UInt32 version = 0;
	if ((es = pFiler->readUInt32(&version)) != Acad::eOk)
		return (es);
	if (version > DRIMTextObj::kCurrentVersionNumber)
		return (Acad::eMakeMeProxy);
	//- Uncomment the 2 following lines if your current object implementation cannot
	//- support previous version of that object.
	//if ( version < DRIMTextObj::kCurrentVersionNumber )
	//	return (Acad::eMakeMeProxy) ;
	//----- Read params
	//.....

	return (pFiler->filerStatus());
}

//-----------------------------------------------------------------------------
//----- AcDbEntity protocols
Adesk::Boolean DRIMTextObj::subWorldDraw(AcGiWorldDraw * mode) {
	assertReadEnabled();
	//------ Returning Adesk::kFalse here will force viewportDraw() call
	return (Adesk::kFalse);
}

void DRIMTextObj::subViewportDraw(AcGiViewportDraw * mode) {
	assertReadEnabled();
	//See this article in case of problems with print
	//https://adn-cis.org/risovanie-gorizontalnogo-teksta-v-sobstvennom-primitive.html
	//http://adndevblog.typepad.com/autocad/2012/08/draw-horizontal-text-in-custom-entity.html
	AcGePoint3d orgn;
	AcGeVector3d upVect;
	mode->viewport().getCameraUpVector(upVect);
	double rot = rotation();
	double vpRot = atan2(upVect.y, upVect.x);

	// Handle plotting
	bool isPlotting = false;
	if (mode->context()->isPlotGeneration()) {
		isPlotting = true;
		vpRot -= PI / 2;
	}

	// Create a vector to specify which quadrants should be considered "upside down".
	// Here, for example, we're setting quadrants 3 and 4 to be "upside down".
	std::vector<bool> upsideDownQuadrants = { false, false, true, true };

	// Compute the difference between the viewport rotation and the DRIMtext rotation.
	double diffRot = vpRot - rot;

	// Adjust the difference to be within the range [-pi, pi] in radians.
	while (diffRot > PI) {
		diffRot -= 2 * PI;
	}
	while (diffRot < -PI) {
		diffRot += 2 * PI;
	}

	// Determine the quadrant of the difference in rotation.
	int quadrant;
	if (diffRot > 0) {
		if (diffRot <= PI / 2) quadrant = 0;
		else quadrant = 1;
	}
	else {
		if (diffRot >= -PI / 2) quadrant = 3;
		else quadrant = 2;
	}

	// Check whether the DRIMtext is "upside-down".
	bool isUpsideDown = upsideDownQuadrants[quadrant];

	if (isUpsideDown) {
		AcGeVector3d dir{ AcGeVector3d(cos(rot),sin(rot),0.0) }; dir.normalize();
		AcGeVector3d dirPerp{ AcGeVector3d(cos(rot + PI * 0.5),sin(rot + PI * 0.5),0.0) }; dirPerp.normalize();
		AcString text = contents();

		// Define the points for the polygon
		AcGePoint3d pVertexList[4];
		pVertexList[0] = AcGePoint3d() + dir * actualWidth() / 2 + dirPerp * actualHeight() / 2;
		pVertexList[1] = AcGePoint3d() - dir * actualWidth() / 2 + dirPerp * actualHeight() / 2;
		pVertexList[2] = AcGePoint3d() - dir * actualWidth() / 2 - dirPerp * actualHeight() / 2;
		pVertexList[3] = AcGePoint3d() + dir * actualWidth() / 2 - dirPerp * actualHeight() / 2;
		// Define the number of points in the polygon
		Adesk::UInt32 numPolygonPoints[1] = { 4 };
		// Define the number of polygons
		Adesk::UInt32 numPolygonIndices = 1;
		// Define the position of the polygon
		AcGePoint3d polygonPositions[1] = { location() };
		// Define the outline colors of the polygons
		const AcCmEntityColor* outlineColors = nullptr;
		// Define the line types of the polygons
		const AcGiLineType* outlineTypes = nullptr;
		// Define the fill color of the polygon
		AcCmEntityColor fillColor;
		fillColor.setColorIndex(0);
		AcCmEntityColor fillColors[1] = { fillColor };
		// Define the fill opacities of the polygons
		AcCmTransparency fillOpacities[1] = { AcCmTransparency(Adesk::UInt8(255)) };

		// Draw the polygon
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

		mode->geometry().text(location() + dir * actualWidth() / 2 + dirPerp * actualHeight() / 2,
			AcGeVector3d::kZAxis, dir * -1.0, textHeight(), 1.0, 0.0, text);

		/*AcGePoint3d pVertexList[2];
		//pVertexList[0] = location();
		pVertexList[1] = location() + dir * extents.x + dirPerp * extents.y;
		mode->geometry().polyline(2, pVertexList);*/
	}
	else AcDbMText::subViewportDraw(mode);

	/*acutPrintf(_T("\nRot: %.2frad %.2f°; vpRot: %.2frad %.2f°; isUpsideDown: %d;"),
		rot, RTD(rot), vpRot, RTD(vpRot), isUpsideDown);*/
}

Adesk::UInt32 DRIMTextObj::subViewportDrawLogicalFlags(AcGiViewportDraw * vd) {
	assertReadEnabled();
	return (AcDbMText::subViewportDrawLogicalFlags(vd));
}

Adesk::UInt32 DRIMTextObj::subSetAttributes(AcGiDrawableTraits * traits) {
	assertReadEnabled();
	return (AcDbMText::subSetAttributes(traits));
}


