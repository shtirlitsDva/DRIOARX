// EPlineTutJig.cpp
//
// (C) Copyright 2009 by Paul Kohut, paulkohut@hotmail.com
//
// This file is part of EPlineTut and EPlineTutDb.
// 
// EPlineTut and EPlineTutDb is free software: you can redistribute it
// and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation, either version 3 of
// the License, or (at your option) any later version.
// 
// EPlineTut and EPlineTutDb is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with EPlineTut.  If not, see <http://www.gnu.org/licenses/>.



#include "StdAfx.h"
#include "..\EPlineTutDb\EPlineDxf.h"
#include "EPlineTutJig.h"
#include "ArcCalcs.h"

CEPlineTutJig::CEPlineTutJig(void)
{
	m_pPline = NULL;
}

CEPlineTutJig::~CEPlineTutJig(void)
{}

int CEPlineTutJig::TotalSegments(void)
{
	return m_pPline->numVerts() - 1;
}

// TODO: properly store and retrieve last entered value
// into AutoCAD's system variable (think it's lastangle)
// Otherwise for now just return 0 for last angle entered.
double CEPlineTutJig::LastEnteredAngle(void)
{
	return 0.0;
}

int CEPlineTutJig::GetStartPoint(AcGePoint3d& startPt)
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

AcDbEntity* CEPlineTutJig::entity() const
{
	return m_pPline;
}

// called by drag if entity data is changed by sampler
Adesk::Boolean CEPlineTutJig::update(void)
{
	int nVerts = m_pPline->numVerts();
	m_pPline->setPointAt(nVerts - 1, asPnt2d(asDblArray(m_pickedPt)));
	switch (m_promptType)
	{
	case LINE:
		m_pPline->setBulgeAt(nVerts - 2, 0.0);
		break;
	case ARC:
		m_pPline->setBulgeAt(nVerts - 2, m_dBulge);
		break;
	default:
		assert(0);
		break;
	}
	return Adesk::kTrue;
}

// called by drag()
AcEdJig::DragStatus CEPlineTutJig::sampler(void)
{
	DragStatus stat = kNoChange;
	setUserInputControls((UserInputControls)
		(AcEdJig::kAccept3dCoordinates | AcEdJig::kAcceptOtherInputString | AcEdJig::kAnyBlankTerminatesInput));

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

void CEPlineTutJig::CreateEntity(void)
{
	AcGePoint3d startPt;
	if (GetStartPoint(startPt) != RTNORM)
		return;

	m_pPline = new EPlineDxf;
	m_pPline->addVertexAt(0, asPnt2d(asDblArray(startPt)));
	m_pPline->addVertexAt(1, asPnt2d(asDblArray(startPt)));
	//////////////////////////////////////
	//////////////////////////////////////
	// TODO: Replace hard coded values with user supplied
	// values when the UI is implemented.
	m_pPline->AddOffsetProperty(5.0);
	m_pPline->AddOffsetProperty(10.0);
	m_pPline->AddOffsetProperty(15.0);
	//////////////////////////////////////
	//////////////////////////////////////

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
			m_pPline->getPointAt(m_pPline->numVerts() - 1, pt2d);
			m_pPline->addVertexAt(m_pPline->numVerts(), pt2d);
			m_linePrompt = None;
		}

		if (stat == kOther && m_nKeyword)
			stat = kNormal;
	}

	// Remove final vertex otherwise there will be 2 on top of each other
	// at the end.
	m_pPline->removeVertexAt(m_pPline->numVerts() - 1);

	if (m_pPline->numVerts() < 2)
	{
		// got to have more than 1 vertex
		delete m_pPline;
		m_pPline = NULL;
		return;
	}

	append();
}

// Called from sampler() when m_promptType == LINE and m_lineType == None
AcEdJig::DragStatus CEPlineTutJig::DragLine(AcGePoint3d& pickedPt)
{
	setKeywordList(_T("Arc Length Undo"));

	AcGePoint3d fromPt, toPt, dragPt;

	m_pPline->getPointAt(m_pPline->numVerts() - 1, fromPt);
	m_pPline->getPointAt(m_pPline->numVerts() - 2, dragPt);

	DragStatus stat = acquirePoint(toPt, dragPt);

	if (toPt != fromPt)
		pickedPt = toPt;
	else if (stat == kNormal)
		stat = kNoChange;

	m_dBulge = 0.0;

	return stat;
}

// Called from sampler() when m_promptType == LINE and m_lineType == Length
AcEdJig::DragStatus CEPlineTutJig::DragDistance(AcGePoint3d& pickedPt)
{
	AcGePoint3d toPt, fromPt, dragPt;
	m_pPline->getPointAt(m_pPline->numVerts() - 1, fromPt);
	m_pPline->getPointAt(m_pPline->numVerts() - 2, dragPt);

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

		AcDbPolyline::SegType segType = m_pPline->segType(TotalSegments() - 2);
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
				m_pPline->getLineSegAt(TotalSegments() - 2, ln);
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
				m_pPline->getArcSegAt(TotalSegments() - 2, arc);
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
AcEdJig::DragStatus CEPlineTutJig::DragArc(AcGePoint3d& pickedPt)
{
	setKeywordList(_T("Angle CEnter Direction Line Radius Second Undo"));

	AcGePoint3d fromPt, toPt, dragPt;
	m_pPline->getPointAt(m_pPline->numVerts() - 1, fromPt);
	m_pPline->getPointAt(m_pPline->numVerts() - 2, dragPt);

	DragStatus stat = acquirePoint(toPt, dragPt);

	if (toPt != fromPt)
		pickedPt = toPt;
	else if (stat == kNormal)
		stat = kNoChange;

	AcDbPolyline::SegType segType = m_pPline->segType(TotalSegments() - 2);
	if (segType == AcDbPolyline::kLine || segType == AcDbPolyline::kPoint || segType == AcDbPolyline::kEmpty)
	{
		AcGeLineSeg2d seg;
		if (m_pPline->getLineSegAt(TotalSegments() - 2, seg) == Acad::eInvalidIndex)
		{
			// Must be kPoint or kEmpty
			m_pPline->getPointAt(0, fromPt);
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
		m_pPline->getArcSegAt(TotalSegments() - 2, arc);
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
AcEdJig::DragStatus CEPlineTutJig::DragArcAngle(AcGePoint3d& pickedPt)
{
	AcGePoint3d fromPt, toPt, dragPt;
	m_pPline->getPointAt(m_pPline->numVerts() - 1, fromPt);
	m_pPline->getPointAt(m_pPline->numVerts() - 2, dragPt);

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
AcEdJig::DragStatus CEPlineTutJig::ProcessKeyword(AcEdJig::DragStatus stat)
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
			if (m_pPline->numVerts() > 2)
				m_pPline->removeVertexAt(m_pPline->numVerts() - 2);
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
			if (m_pPline->numVerts() > 2)
				m_pPline->removeVertexAt(m_pPline->numVerts() - 2);
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


