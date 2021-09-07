// OffsetProperty.cpp
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

#include "dbpl.h"

class COffsetProperty
{
public:
	COffsetProperty(void);
	COffsetProperty(AcDbObjectId layerId, double dOffsetDistance) : m_layerId(layerId), m_dOffset(dOffsetDistance) {};
	virtual ~COffsetProperty(void);

	double Offset() const { return m_dOffset; }
	void Offset(double val) { m_dOffset = val; }

	AcDbObjectId LayerId() const { return m_layerId; }
	void LayerId(AcDbObjectId val) { m_layerId = val; }

	Acad::ErrorStatus dwgOutFields(AcDbDwgFiler * pFiler) const;
	Acad::ErrorStatus dwgInFields(AcDbDwgFiler * pFiler, Adesk::UInt32 version);

protected:
	double m_dOffset;	
	AcDbObjectId m_layerId;
};
