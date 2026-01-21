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

//-----------------------------------------------------------------------------
Adesk::UInt32 NSText::kCurrentVersionNumber =1 ;

//-----------------------------------------------------------------------------
ACRX_DXF_DEFINE_MEMBERS (
	NSText, AcDbText,
	AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent, 
	AcDbProxyEntity::kNoOperation, NSTEXT,
NSNORSYNOBJECTSAPP
|Product Desc:     A description for your object
|Company:          Your company name
|WEB Address:      Your company WEB site address
)

//-----------------------------------------------------------------------------
NSText::NSText () : AcDbText () {
}

NSText::~NSText () {
}

//-----------------------------------------------------------------------------
//----- AcDbObject protocols
//- Dwg Filing protocol
Acad::ErrorStatus NSText::dwgOutFields (AcDbDwgFiler *pFiler) const {
	assertReadEnabled () ;
	//----- Save parent class information first.
	Acad::ErrorStatus es =AcDbText::dwgOutFields (pFiler) ;
	if ( es != Acad::eOk )
		return (es) ;
	//----- Object version number needs to be saved first
	if ( (es =pFiler->writeUInt32 (NSText::kCurrentVersionNumber)) != Acad::eOk )
		return (es) ;
	//----- Output params
	//.....

	return (pFiler->filerStatus ()) ;
}

Acad::ErrorStatus NSText::dwgInFields (AcDbDwgFiler *pFiler) {
	assertWriteEnabled () ;
	//----- Read parent class information first.
	Acad::ErrorStatus es =AcDbText::dwgInFields (pFiler) ;
	if ( es != Acad::eOk )
		return (es) ;
	//----- Object version number needs to be read first
	Adesk::UInt32 version =0 ;
	if ( (es =pFiler->readUInt32 (&version)) != Acad::eOk )
		return (es) ;
	if ( version > NSText::kCurrentVersionNumber )
		return (Acad::eMakeMeProxy) ;
	//- Uncomment the 2 following lines if your current object implementation cannot
	//- support previous version of that object.
	//if ( version < NSText::kCurrentVersionNumber )
	//	return (Acad::eMakeMeProxy) ;
	//----- Read params
	//.....

	return (pFiler->filerStatus ()) ;
}

//- Dxf Filing protocol
Acad::ErrorStatus NSText::dxfOutFields (AcDbDxfFiler *pFiler) const {
	assertReadEnabled () ;
	//----- Save parent class information first.
	Acad::ErrorStatus es =AcDbText::dxfOutFields (pFiler) ;
	if ( es != Acad::eOk )
		return (es) ;
	es =pFiler->writeItem (AcDb::kDxfSubclass, _RXST("NSText")) ;
	if ( es != Acad::eOk )
		return (es) ;
	//----- Object version number needs to be saved first
	if ( (es =pFiler->writeUInt32 (kDxfInt32, NSText::kCurrentVersionNumber)) != Acad::eOk )
		return (es) ;
	//----- Output params
	//.....

	return (pFiler->filerStatus ()) ;
}

Acad::ErrorStatus NSText::dxfInFields (AcDbDxfFiler *pFiler) {
	assertWriteEnabled () ;
	//----- Read parent class information first.
	Acad::ErrorStatus es =AcDbText::dxfInFields (pFiler) ;
	if ( es != Acad::eOk || !pFiler->atSubclassData (_RXST("NSText")) )
		return (pFiler->filerStatus ()) ;
	//----- Object version number needs to be read first
	struct resbuf rb ;
	pFiler->readItem (&rb) ;
	if ( rb.restype != AcDb::kDxfInt32 ) {
		pFiler->pushBackItem () ;
		pFiler->setError (Acad::eInvalidDxfCode, _RXST("\nError: expected group code %d (version #)"), AcDb::kDxfInt32) ;
		return (pFiler->filerStatus ()) ;
	}
	Adesk::UInt32 version =(Adesk::UInt32)rb.resval.rlong ;
	if ( version > NSText::kCurrentVersionNumber )
		return (Acad::eMakeMeProxy) ;
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
	while ( es == Acad::eOk && (es =pFiler->readResBuf (&rb)) == Acad::eOk ) {
		switch ( rb.restype ) {
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
				pFiler->pushBackItem () ;
				es =Acad::eEndOfFile ;
				break ;
		}
	}
#pragma warning( pop )
	//----- At this point the es variable must contain eEndOfFile
	//----- - either from readResBuf() or from pushback. If not,
	//----- it indicates that an error happened and we should
	//----- return immediately.
	if ( es != Acad::eEndOfFile )
		return (Acad::eInvalidResBuf) ;

	return (pFiler->filerStatus ()) ;
}

//-----------------------------------------------------------------------------
//----- AcDbEntity protocols
Adesk::Boolean NSText::subWorldDraw (AcGiWorldDraw *mode) {
	assertReadEnabled () ;
	//------ Returning Adesk::kFalse here will force viewportDraw() call
	return (Adesk::kFalse) ;
}

void NSText::subViewportDraw (AcGiViewportDraw *mode) {
	assertReadEnabled () ;
	AcGePoint3d orgn ;
	AcGeVector3d xa, ya, za ;
	AcGeMatrix3d transform ;
	mode->viewport ().getEyeToWorldTransform (transform) ;
	transform.getCoordSystem (orgn, xa, ya, za) ;

	const double kPi =3.14159265358979323846 ;
	const double kTwoPi =2.0 *kPi ;
	double rot =rotation () ;
	while ( rot < 0.0 )
		rot +=kTwoPi ;
	while ( rot >= kTwoPi )
		rot -=kTwoPi ;

	double viewRot =atan2 (xa.y, xa.x) ;
	if ( viewRot < 0.0 )
		viewRot +=kTwoPi ;
	if ( mode->context ()->isPlotGeneration () ) {
		viewRot +=0.5 *kPi ;
		if ( viewRot >= kTwoPi )
			viewRot -=kTwoPi ;
	}

	double diff =rot -viewRot ;
	if ( diff < 0.0 )
		diff +=kTwoPi ;

	bool toRotate =(diff > (0.5 *kPi)) && (diff < (1.5 *kPi)) ;
	if ( toRotate ) {
		AcGeVector3d dir (cos (rot), sin (rot), 0.0) ;
		dir.normalize () ;
		AcGeVector3d dirPerp (cos (rot +0.5 *kPi), sin (rot +0.5 *kPi), 0.0) ;
		dirPerp.normalize () ;
		AcString text =textString () ;
		AcGiTextStyle style ;
		fromAcDbTextStyle (style, textStyle ()) ;
		style.loadStyleRec () ;
		AcGePoint2d extents =style.extents (textString (), Adesk::kFalse, text.length (), Adesk::kTrue) ;

		mode->geometry ().text (position () +dir *(extents.x / 2.0) +dirPerp *(extents.y / 2.0),
			AcGeVector3d::kZAxis, dir *-1.0, height (), 1.0, 0.0, text) ;
		return ;
	}

	AcDbText::subViewportDraw (mode) ;
}

Adesk::UInt32 NSText::subViewportDrawLogicalFlags (AcGiViewportDraw *vd) {
	assertReadEnabled () ;
	return (AcDbText::subViewportDrawLogicalFlags (vd)) ;
}

Adesk::UInt32 NSText::subSetAttributes (AcGiDrawableTraits *traits) {
	assertReadEnabled () ;
	return (AcDbText::subSetAttributes (traits)) ;
}


