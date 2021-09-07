// (C) Copyright 2005-2007 by Autodesk, Inc. 
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
#include "StdAfx.h"
#include "DRIPolylineJig.h"
#include "ArcCalcs.h"

//-----------------------------------------------------------------------------
DRIPolylineJig::DRIPolylineJig() : AcEdJig(),
mCurrentInputLevel(0), mpEntity(NULL)
{}

DRIPolylineJig::~DRIPolylineJig()
{}

int DRIPolylineJig::TotalSegments()
{
	return mpEntity->numVerts() - 1;
}

// TODO: properly store and retrieve last entered value
// into AutoCAD's system variable (think it's lastangle)
// Otherwise for now just return 0 for last angle entered.
double DRIPolylineJig::LastEnteredAngle(void)
{
	return 0.0;
}

//-----------------------------------------------------------------------------
int DRIPolylineJig::GetStartPoint(AcGePoint3d& startPt)
{
	AcGePoint3d pt;
	int rVal = acedGetPoint(NULL, _T("\nSpecify start point: "), asDblArray(pt));
	if (rVal == RTNORM)
	{
		// setup point transform
		resbuf rbFrom, rbTo;
		rbFrom.restype = RTSHORT;
		rbFrom.resval.rint = 1; // from UCS
		rbTo.restype = RTSHORT;
		rbTo.resval.rint = 0; // to WCS
		acedTrans(asDblArray(pt), &rbFrom, &rbTo, Adesk::kFalse, asDblArray(pt));
		startPt = pt;
	}
	return rVal;
}

AcDbEntity* DRIPolylineJig::entity() const
{
	return ((AcDbEntity*)mpEntity);
}

// called by drag if entity data is changed by sampler
Adesk::Boolean DRIPolylineJig::update(void)
{
	int nVerts = mpEntity->numVerts();
	mpEntity->setPointAt(nVerts - 1, asPnt2d(asDblArray(m_pickedPt)));
	switch (m_promptType)
	{
	case LINE:
		mpEntity->setBulgeAt(nVerts - 2, 0.0);
		break;
	case ARC:
		mpEntity->setBulgeAt(nVerts - 2, m_dBulge);
		break;
	default:
		assert(0);
		break;
	}
	return Adesk::kTrue;
}

// called by drag()
AcEdJig::DragStatus DRIPolylineJig::sampler(void)
{
	DragStatus stat = kNoChange;
	setUserInputControls((UserInputControls)
		(	
			//AcEdJig::kGovernedByOrthoMode |
			AcEdJig::kAccept3dCoordinates |
			AcEdJig::kAcceptOtherInputString |
			AcEdJig::kAnyBlankTerminatesInput));

	setSpecialCursorType(AcEdJig::kRubberBand);

	AcGePoint3d pickedPt; // new pick point selected by user

	// ProcessKeyword is responsible for setting the state of m_promptType
	// and m_linePrompt.  Make sure it and this if statement are in sync.
	if (m_promptType == LINE)
	{
		if (m_linePrompt == Length)
			stat = DragDistance(pickedPt);
		else if (m_linePrompt == None)
			stat = DragLine(pickedPt);
		else
			assert(0);

		if (stat == kNormal)
		{
			m_pickedPt = pickedPt;
			m_bAddIt = TRUE;
		}
	}

	// ProcessKeyword is responsible for setting the state of m_promptType
	// and m_linePrompt.  Make sure it and this if statement are in sync.
	if (m_promptType == ARC)
	{
		if (m_linePrompt == None)
			stat = DragArc(pickedPt);
		else if (m_linePrompt == Angle)
		{
			stat = DragArcAngle(pickedPt);
			int x = 0;
			x++;
		}
		else
		{
			assert(0);
		}

		if (stat == kNormal)
		{
			m_pickedPt = pickedPt;
			m_bAddIt = TRUE;
		}
	}

	// stat values from 1 to 30 represent keywords of
	// AcEdJig::kKW1 to 30, with 30 being the maximum allowed.
	if (stat >= 1 && stat <= 30)
	{
		stat = ProcessKeyword(stat);
		m_bAddIt = FALSE; // signal the CreateEntity loop not to add a vertex
		return stat;
	}
	return stat;
}

void DRIPolylineJig::CreateEntity()
{
	AcGePoint3d startPt;
	if (GetStartPoint(startPt) != RTNORM)
		return;

	mpEntity = new AcDbPolyline();
	mpEntity->addVertexAt(0, asPnt2d(asDblArray(startPt)));
	mpEntity->addVertexAt(1, asPnt2d(asDblArray(startPt)));

	m_pickedPt = startPt;
	m_promptType = LINE;
	m_linePrompt = None;

	DragStatus stat = kNormal;

	while (stat == kNormal)
	{
		m_nKeyword = 0;
		if (m_promptType == LINE)
			setDispPrompt(_T("\nSpecify next point or [Arc/Length/Undo]: "));
		else if (m_promptType == ARC && m_linePrompt == None)
			setDispPrompt(_T("\nSpecify next point or [Angle/CEnter/Direction/Line/Radius/Second pt/Undo]: "));
		else if (m_promptType == ARC && m_linePrompt == Angle)
			setDispPrompt(_T("\nSpecify include angle: "));
		else
			assert(0); // should never get here
		m_bAddIt = FALSE;

		stat = drag(); // function drag will call into sampler()
		if (stat == kNormal && m_bAddIt == TRUE)
		{
			AcGePoint2d pt2d;
			mpEntity->getPointAt(mpEntity->numVerts() - 1, pt2d);
			mpEntity->addVertexAt(mpEntity->numVerts(), pt2d);
			m_linePrompt = None;
		}

		if (stat == kOther && m_nKeyword)
			stat = kNormal;
	}

	// Remove final vertex otherwise there will be 2 on top of each other
	// at the end.
	mpEntity->removeVertexAt(mpEntity->numVerts() - 1);

	if (mpEntity->numVerts() < 2)
	{
		// got to have more than 1 vertex
		delete mpEntity;
		mpEntity = NULL;
		return;
	}

	append();
}

// Called from sampler() when m_promptType == LINE and m_lineType == None
AcEdJig::DragStatus DRIPolylineJig::DragLine(AcGePoint3d& pickedPt)
{
	setKeywordList(_T("Arc Length Undo"));

	AcGePoint3d fromPt, toPt, dragPt;

	mpEntity->getPointAt(mpEntity->numVerts() - 1, fromPt);
	mpEntity->getPointAt(mpEntity->numVerts() - 2, dragPt);

	DragStatus stat = acquirePoint(toPt, dragPt);

	if (toPt != fromPt)
		pickedPt = toPt;
	else if (stat == kNormal)
		stat = kNoChange;

	m_dBulge = 0.0;

	return stat;
}

// Called from sampler() when m_promptType == LINE and m_lineType == Length
AcEdJig::DragStatus DRIPolylineJig::DragDistance(AcGePoint3d& pickedPt)
{
	AcGePoint3d toPt, fromPt, dragPt;
	mpEntity->getPointAt(mpEntity->numVerts() - 1, fromPt);
	mpEntity->getPointAt(mpEntity->numVerts() - 2, dragPt);

	double dDist;
	m_dBulge = 0.0;

	DragStatus stat = acquireDist(dDist, dragPt);
	if (stat != kNormal)
		return kOther;

	if (dDist == 0.0)
		m_pickedPt = dragPt;
	else
	{
		double dAngle;
		BOOL bAngleAquired = FALSE;

		AcDbPolyline::SegType segType = mpEntity->segType(TotalSegments() - 2);
		switch (segType)
		{
		case AcDbPolyline::kPoint:
			// can't determine an angle from a single point so get last entered angle
			dAngle = LastEnteredAngle();
			bAngleAquired = TRUE;
			// Haven't seen this condition fire yet so don't know if next function is
			// correct or not.  Going to assert just in case it fires during debug
			// testing.
			assert(0);
			// adjust picked point based on last angle entered and picked distance
			acutPolar(asDblArray(dragPt), dAngle, dDist, asDblArray(pickedPt));
			stat = kNormal;
			break;
		case AcDbPolyline::kLine:
			if (!bAngleAquired)
			{
				AcGeLineSeg2d ln;
				mpEntity->getLineSegAt(TotalSegments() - 2, ln);
				dAngle = acutAngle(asDblArray(ln.startPoint()), asDblArray(ln.endPoint()));
				// adjust picked point based on line segment angle and picked distance.
				acutPolar(asDblArray(ln.endPoint()), dAngle, dDist, asDblArray(pickedPt));
				bAngleAquired = TRUE;
				stat = kNormal;
			}
			break;
		case AcDbPolyline::kEmpty:
			dAngle = LastEnteredAngle();
			bAngleAquired = TRUE;
			stat = kNormal;
			acutPolar(asDblArray(dragPt), dAngle, dDist, asDblArray(pickedPt));
			break;
		case AcDbPolyline::kArc:
			if (!bAngleAquired)
			{
				AcGeCircArc2d arc;
				mpEntity->getArcSegAt(TotalSegments() - 2, arc);
				dAngle = CalcArcEndTangent(arc);
				// adjust picked point based on picked distance and tangent to end of arc segment
				acutPolar(asDblArray(arc.endPoint()), dAngle, dDist, asDblArray(pickedPt));
				stat = kNormal;
			}
			break;
		default:
			stat = kOther;
			break;
		}
	}

	return stat;
}

// called by sampler when m_promptType == ARC and m_linePrompt == None
AcEdJig::DragStatus DRIPolylineJig::DragArc(AcGePoint3d& pickedPt)
{
	setKeywordList(_T("Angle CEnter Direction Line Radius Second Undo"));

	AcGePoint3d fromPt, toPt, dragPt;
	mpEntity->getPointAt(mpEntity->numVerts() - 1, fromPt);
	mpEntity->getPointAt(mpEntity->numVerts() - 2, dragPt);

	DragStatus stat = acquirePoint(toPt, dragPt);

	if (toPt != fromPt)
		pickedPt = toPt;
	else if (stat == kNormal)
		stat = kNoChange;

	AcDbPolyline::SegType segType = mpEntity->segType(TotalSegments() - 2);
	if (segType == AcDbPolyline::kLine || segType == AcDbPolyline::kPoint || segType == AcDbPolyline::kEmpty)
	{
		AcGeLineSeg2d seg;
		if (mpEntity->getLineSegAt(TotalSegments() - 2, seg) == Acad::eInvalidIndex)
		{
			// Must be kPoint or kEmpty
			mpEntity->getPointAt(0, fromPt);
			m_dBulge = CalcBulgeFactor(LastEnteredAngle(), fromPt, toPt);
		}
		else
		{
			// Must be kLine
			// calc arc segment tangent to previous line segment
			m_dBulge = CalcBulgeFactor(seg.direction().angle(), asPnt3d(asDblArray(seg.endPoint())), toPt);
		}
	}
	else if (segType == AcDbPolyline::kArc)
	{
		AcGeCircArc2d arc;
		mpEntity->getArcSegAt(TotalSegments() - 2, arc);
		double dTangentAngle = CalcArcEndTangent(arc);
		AcGePoint2d pt1 = arc.endPoint();
		acutPolar(asDblArray(pt1), dTangentAngle, 10, asDblArray(pt1));
		AcGeLineSeg2d ln(asPnt2d(asDblArray(arc.endPoint())), pt1);
		// calc arc segment tangent to previous arc segment
		m_dBulge = CalcBulgeFactor(ln.direction().angle(), asPnt3d(asDblArray(arc.endPoint())), asPnt3d(asDblArray(toPt)));
	}

	return stat;
}

// called by sampler when m_promptType == ARC and m_linePrompt == Angle
// TODO: Function is not completed.  Need to rework keyword logic a bit
// then refactor this fuction to support inlucde angle input, and arc
// end point input (or arc center point input).  See command
// PLINE->>Arc->>Angle->> in AutoCAD for details
AcEdJig::DragStatus DRIPolylineJig::DragArcAngle(AcGePoint3d& pickedPt)
{
	AcGePoint3d fromPt, toPt, dragPt;
	mpEntity->getPointAt(mpEntity->numVerts() - 1, fromPt);
	mpEntity->getPointAt(mpEntity->numVerts() - 2, dragPt);

	DragStatus stat = acquirePoint(toPt, dragPt);

	if (toPt == dragPt)
		return kNoChange;
	if (stat == kNoChange)
		return kNoChange;
	if (stat != kNormal)
		return kOther;

	double dIncludeAngle = acutAngle(asDblArray(fromPt), asDblArray(toPt));
	m_dBulge = tan(dIncludeAngle) * 0.25;

	return stat;
}

// TODO: Refactor to better support nexted keyword inputs
AcEdJig::DragStatus DRIPolylineJig::ProcessKeyword(AcEdJig::DragStatus stat)
{
	DragStatus drStat = kOther;
	m_nKeyword = stat;

	switch (m_promptType)
	{
	case LINE:
		switch (stat)
		{
		case 1:
			m_promptType = ARC;
			m_linePrompt = None;
			break;
		case 2:
			m_linePrompt = Length;
			break;
		case 3:
			m_promptType = LINE;
			if (mpEntity->numVerts() > 2)
				mpEntity->removeVertexAt(mpEntity->numVerts() - 2);
			m_linePrompt = None;
			break;
		default:
			break;
		}
		break;
	case ARC:
		// for now just angle, line and are handled, need to add support for the
		// reset of the keywords.
		switch (stat)
		{
		case 1:
			m_promptType = ARC;
			m_linePrompt = Angle;
			break;
		case 4:
			m_promptType = LINE;
			break;
		case 7:
			m_promptType = LINE;
			if (mpEntity->numVerts() > 2)
				mpEntity->removeVertexAt(mpEntity->numVerts() - 2);
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
	return drStat;
}

//-----------------------------------------------------------------------------
//- Std input to get a point with rubber band from point
AcEdJig::DragStatus DRIPolylineJig::GetNextPoint()
{
	AcGePoint3d oldPnt = mInputPoints[mCurrentInputLevel];
	AcGePoint3d newPnt;
	//- Get the point 
	AcEdJig::DragStatus status = acquirePoint(newPnt, oldPnt);
	//- If valid input
	if (status == AcEdJig::kNormal)
	{
		//- If there is no difference
		if (newPnt.isEqualTo(mInputPoints[mCurrentInputLevel]))
			return (AcEdJig::kNoChange);
		//- Otherwise update the point
		mInputPoints[mCurrentInputLevel] = newPnt;
	}
	return (status);
}

//-----------------------------------------------------------------------------
//- Dynamic dimension data setup
AcDbDimDataPtrArray* DRIPolylineJig::dimData(const double dimScale)
{

	/* SAMPLE CODE:
	AcDbAlignedDimension *dim =new AcDbAlignedDimension () ;
	dim->setDatabaseDefaults () ;
	dim->setNormal (AcGeVector3d::kZAxis) ;
	dim->setElevation (0.0) ;
	dim->setHorizontalRotation (0.0) ;
	dim->setXLine1Point (m_originPoint) ;
	dim->setXLine2Point (m_lastPoint) ;
	//- Get the dimPoint, first the midpoint
	AcGePoint3d dimPoint =m_originPoint + ((m_lastPoint - m_originPoint) / 2.0) ;
	//- Then the offset
	dim->setDimLinePoint (dimPoint) ;
	dim->setDimtad (1) ;

	AcDbDimData *dimData = new AcDbDimData (dim) ;
	//AppData *appData =new AppData (1, dimScale) ;
	//dimData.setAppData (appData) ;
	dimData->setDimFocal (true) ;
	dimData->setDimHideIfValueIsZero (true) ;

	//- Check to see if it is required
	if ( getDynDimensionRequired (m_inputNumber) )
		dimData->setDimInvisible (false) ;
	else
		dimData->setDimInvisible (true) ;

	//- Make sure it is editable TODO:
	dimData->setDimEditable (true) ;
	mDimData.append (dimData) ;

	return (&mDimData) ;
	*/
	return (NULL);
}

//-----------------------------------------------------------------------------
//- Dynamic dimension data update
Acad::ErrorStatus DRIPolylineJig::setDimValue(const AcDbDimData* pDimData, const double dimValue)
{
	Acad::ErrorStatus es = Acad::eOk;

	/* SAMPLE CODE:
	//- Convert the const pointer to non const
	AcDbDimData *dimDataNC =const_cast<AcDbDimData *>(pDimData) ;
	int inputNumber =-1 ;
	//- Find the dim data being passed so we can determine the input number
	if ( mDimData.find (dimDataNC, inputNumber) ) {
		//- Now get the dimension
		AcDbDimension *pDim =(AcDbDimension *)dimDataNC->dimension () ;
		//- Check it's the type of dimension we want
		AcDbAlignedDimension *pAlnDim =AcDbAlignedDimension::cast (pDim) ;
		//- If ok
		if ( pAlnDim ) {
			//- Extract the dimensions as they are now
			AcGePoint3d dimStart =pAlnDim->xLine1Point () ;
			AcGePoint3d dimEnd =pAlnDim->xLine2Point () ;
			//- Lets get the new point entered by the user
			AcGePoint3d dimEndNew =dimStart + (dimEnd - dimStart).normalize () * dimValue ;
			//- Finally set the end dim point
			pAlnDim->setXLine2Point (dimEndNew) ;
			//- Now update the jig data to reflect the dynamic dimension input
			mInputPoints [mCurrentInputLevel] =dimEndNew ;
		}
	}*/
	return (es);
}

//-----------------------------------------------------------------------------
//- Various helper functions
//- Dynamic dimdata update function
Adesk::Boolean DRIPolylineJig::updateDimData()
{
	//- Check the dim data store for validity
	if (mDimData.length() <= 0)
		return (true);

	/* SAMPLE CODE :
	//- Extract the individual dimData
	AcDbDimData *dimData =mDimData [m_inputNumber] ;
	//- Now get the dimension
	AcDbDimension *pDim =(AcDbDimension *)dimData->dimension () ;
	//- Check it's the type of dimension we want
	AcDbAlignedDimension *pAlnDim =AcDbAlignedDimension::cast (pDim) ;
	//- If ok
	if ( pAlnDim ) {
		//- Check to see if it is required
		if ( getDynDimensionRequired (m_inputNumber) )
			dimData->setDimInvisible (false) ;
		else
			dimData->setDimInvisible (true) ;
		pAlnDim->setXLine1Point (m_originPoint) ;
		pAlnDim->setXLine2Point (m_lastPoint) ;
		//- Get the dimPoint, first the midpoint
		AcGePoint3d dimPoint =m_originPoint + ((m_lastPoint - m_originPoint) / 2.0) ;
		//- Then the offset
		pAlnDim->setDimLinePoint (dimPoint) ;
	} */
	return (true);
}
