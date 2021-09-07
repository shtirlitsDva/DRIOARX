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


#pragma once
#include "c:\objectarx 2008\inc\dbjig.h"

class EPlineDxf;

class CEPlineTutJig :
	public AcEdJig
{
public:
	CEPlineTutJig(void);
	~CEPlineTutJig(void);

	virtual void CreateEntity(void);
	virtual DragStatus sampler(void);
	virtual Adesk::Boolean update(void);
	virtual AcDbEntity * entity() const;

private:
	enum PROMPT_TYPE { LINE = 0, ARC };
	enum LINE_PROMPT { None = 0, Arc, Length, Undo, Angle };

	PROMPT_TYPE m_promptType;
	LINE_PROMPT m_linePrompt;
	int m_nKeyword;
	AcGePoint3d m_pickedPt;
	BOOL m_bAddIt;
	double m_dBulge;
	EPlineDxf * m_pPline;

	virtual int GetStartPoint(AcGePoint3d & startPt);

	AcEdJig::DragStatus DragDistance(AcGePoint3d & pickedPt);
	AcEdJig::DragStatus DragArcAngle(AcGePoint3d & pickedPt);
	AcEdJig::DragStatus DragArc(AcGePoint3d & pickedPt);
	AcEdJig::DragStatus DragLine(AcGePoint3d & pickedPt);
	AcEdJig::DragStatus ProcessKeyword(AcEdJig::DragStatus stat);

	int TotalSegments(void);
	double LastEnteredAngle(void);
};
