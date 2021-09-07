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

//-----------------------------------------------------------------------------
//- DRIPolylineJig.h
#include "StdAfx.h"
#pragma once

//-----------------------------------------------------------------------------
class DRIPolylineJig : public AcEdJig
{

private:
	//- Member variables
	//- current input level, increment for each input
	int mCurrentInputLevel;
	//- Dynamic dimension info
	AcDbDimDataPtrArray mDimData;
	enum PROMPT_TYPE { LINE = 0, ARC };
	enum LINE_PROMPT { None = 0, Arc, Length, Undo, Angle };

	PROMPT_TYPE m_promptType;
	LINE_PROMPT m_linePrompt;
	int m_nKeyword;
	AcGePoint3d m_pickedPt;
	BOOL m_bAddIt;
	double m_dBulge;

public:
	//- Array of input points, each level corresponds to the mCurrentInputLevel
	AcGePoint3dArray mInputPoints;
	//- Entity being jigged
	AcDbPolyline* mpEntity;

public:
	DRIPolylineJig();
	~DRIPolylineJig();

	virtual void CreateEntity(void);
	virtual DragStatus sampler(void);
	virtual Adesk::Boolean update(void);
	virtual AcDbEntity* entity() const;
	AcEdJig::DragStatus startJig(AcDbPolyline* pEntityToJig);

protected:
	virtual int GetStartPoint(AcGePoint3d& startPt);
	virtual AcDbDimDataPtrArray* dimData(const double dimScale);
	virtual Acad::ErrorStatus setDimValue(const AcDbDimData* pDimData, const double dimValue);
	virtual Adesk::Boolean updateDimData();

	AcEdJig::DragStatus DragDistance(AcGePoint3d& pickedPt);
	AcEdJig::DragStatus DragArcAngle(AcGePoint3d& pickedPt);
	AcEdJig::DragStatus DragArc(AcGePoint3d& pickedPt);
	AcEdJig::DragStatus DragLine(AcGePoint3d& pickedPt);
	AcEdJig::DragStatus ProcessKeyword(AcEdJig::DragStatus stat);

	int TotalSegments(void);
	double LastEnteredAngle(void);

	//- Std input to get a point with no rubber band
	AcEdJig::DragStatus GetStartPoint();
	//- Std input to get a point with rubber band from point
	AcEdJig::DragStatus GetNextPoint();
};
