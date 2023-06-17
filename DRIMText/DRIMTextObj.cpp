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
	assertReadEnabled();
	AcGePoint3d orgn;
	AcGeVector3d xa, ya, za;
	AcGeVector3d upVect;
	mode->viewport().getCameraUpVector(upVect);
	double rot = rotation();
	double vpRot = atan2(xa.y, xa.x);
	double angleDiff = fabs(rot - vpRot);
	if (angleDiff > PI / 2) {
		AcGeVector3d dir{ AcGeVector3d(cos(rot),sin(rot),0.0) }; dir.normalize();
		AcGeVector3d dirPerp{ AcGeVector3d(cos(rot + PI * 0.5),sin(rot + PI * 0.5),0.0) }; dirPerp.normalize();
		AcString text = contents();
		AcGiTextStyle style;
		fromAcDbTextStyle(style, this->textStyle());
		style.loadStyleRec();
		AcGePoint2d extents = style.extents(contents(), Adesk::kFalse, text.length(), Adesk::kTrue);
		//AcString ext; ext.format(_T(" *"));//ext.format(_T(" x: %.3f, y: %.3f"), extents.x, extents.y);
		//text.append(ext);
		mode->geometry().text(location() + dir * extents.x + dirPerp * extents.y,
			AcGeVector3d::kZAxis, dir * -1, height(), 1.0, 0.0, text);
	}
	else
	AcDbMText::subViewportDraw(mode);
}

Adesk::UInt32 DRIMTextObj::subViewportDrawLogicalFlags(AcGiViewportDraw * vd) {
	assertReadEnabled();
	return (AcDbMText::subViewportDrawLogicalFlags(vd));
}

Adesk::UInt32 DRIMTextObj::subSetAttributes(AcGiDrawableTraits * traits) {
	assertReadEnabled();
	return (AcDbMText::subSetAttributes(traits));
}


