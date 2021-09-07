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



#include "StdAfx.h"
#include "OffsetProperty.h"

COffsetProperty::COffsetProperty(void)
{
	m_dOffset = 0.0;
	// m_layerId = AcDbObjectId::kNull;
}

COffsetProperty::~COffsetProperty(void)
{
}

Acad::ErrorStatus COffsetProperty::dwgOutFields(AcDbDwgFiler * pFiler) const
{
	pFiler->writeDouble(m_dOffset);
	pFiler->writeHardPointerId(m_layerId);	

	return (pFiler->filerStatus());
}

Acad::ErrorStatus COffsetProperty::dwgInFields(AcDbDwgFiler * pFiler, Adesk::UInt32 version)
{
	if(version >= 1)
	{
		pFiler->readDouble(&m_dOffset);
		pFiler->readHardPointerId((AcDbHardPointerId *) &m_layerId);
	}

	return(pFiler->filerStatus());
}