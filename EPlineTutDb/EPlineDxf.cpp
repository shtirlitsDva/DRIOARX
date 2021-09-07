// EPlineDxf.cpp
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


//-----------------------------------------------------------------------------
//----- EPlineDxf.cpp : Implementation of EPlineDxf
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "EPlineDxf.h"

//-----------------------------------------------------------------------------
Adesk::UInt32 EPlineDxf::kCurrentVersionNumber = 1;

// Only allow 5 items in the COffsetProperty collection.  This can be removed
// in the future.  Try and develop the UI EPlineTut so it can handle however
// many values are in the COffsetProperty collection.
#define MAX_PROPERTIES 5

//-----------------------------------------------------------------------------
ACRX_DXF_DEFINE_MEMBERS(
	EPlineDxf, AcDbPolyline,
	AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent,
	AcDbProxyEntity::kNoOperation, EPLINEDXF,
	"EPLINETUTDBAPP"
	"|Product Desc:     Enhanced polyline tutorial"
	"|Company:          Paul Kohut"
	"|WEB Address:      www.pkautomation.com"
)

//-----------------------------------------------------------------------------
EPlineDxf::EPlineDxf() : AcDbPolyline()
{}

EPlineDxf::~EPlineDxf()
{}

//-----------------------------------------------------------------------------
//----- AcDbObject protocols
//- Dwg Filing protocol
Acad::ErrorStatus EPlineDxf::dwgOutFields(AcDbDwgFiler* pFiler) const
{
	assertReadEnabled();
	//----- Save parent class information first.
	Acad::ErrorStatus es = AcDbPolyline::dwgOutFields(pFiler);
	if (es != Acad::eOk)
		return (es);
	//----- Object version number needs to be saved first
	if ((es = pFiler->writeUInt32(EPlineDxf::kCurrentVersionNumber)) != Acad::eOk)
		return (es);
	//----- Output params
	//.....

	pFiler->writeInt32(m_properties.length());
	for (int i = 0; i < m_properties.length(); i++)
		m_properties[i].dwgOutFields(pFiler);

	return (pFiler->filerStatus());
}

Acad::ErrorStatus EPlineDxf::dwgInFields(AcDbDwgFiler* pFiler)
{
	assertWriteEnabled();
	//----- Read parent class information first.
	Acad::ErrorStatus es = AcDbPolyline::dwgInFields(pFiler);
	if (es != Acad::eOk)
		return (es);
	//----- Object version number needs to be read first
	Adesk::UInt32 version = 0;
	if ((es = pFiler->readUInt32(&version)) != Acad::eOk)
		return (es);
	if (version > EPlineDxf::kCurrentVersionNumber)
		return (Acad::eMakeMeProxy);
	//- Uncomment the 2 following lines if your current object implementation cannot
	//- support previous version of that object.
	//if ( version < EPlineDxf::kCurrentVersionNumber )
	//	return (Acad::eMakeMeProxy) ;
	//----- Read params
	//.....

	Adesk::Int32 nProperties;
	if ((es = pFiler->readInt32(&nProperties)) != Acad::eOk)
		return es;

	for (int i = 0; i < nProperties; i++)
	{
		COffsetProperty prop;
		if ((es = prop.dwgInFields(pFiler, version)) != Acad::eOk)
			return es;
		m_properties.append(prop);
	}

	return (pFiler->filerStatus());
}

//-----------------------------------------------------------------------------
//----- AcDbEntity protocols
Adesk::Boolean EPlineDxf::subWorldDraw(AcGiWorldDraw* mode)
{
	assertReadEnabled();

	// need more than one vertex to perform offset curves
	if (numVerts() > 1)
	{
		AcGePoint3d pt1, pt2;
		getPointAt(numVerts() - 1, pt1);
		getPointAt(numVerts() - 2, pt2);
		AcGiSubEntityTraits& traits = mode->subEntityTraits();

		// make sure the 2 points one the vertex are not the same point
		if (pt1 != pt2)
		{
			for (int nOffset = 0; nOffset < NumberOfOffsetProperties(); nOffset++)
			{
				COffsetProperty prop;
				GetOffsetProperty(nOffset, prop);

				if (prop.Offset() != 0.0)
				{
					// Offset distance is not 0 so go ahead and create the offset curves
					// Look at ObjectARX docs for full description of how a positive and
					// negative offset distance will effect the new offset geometry.

					// Stange and bizare effects happen when the line geometry crosses on
					// an arc (same effect can be seen with the "offset" command in AutoCAD.
					// Fixing this is probably pretty complicated and way outside the realm
					// of this program.
					AcDbVoidPtrArray offsetCurves;
					if (getOffsetCurves(prop.Offset(), offsetCurves) == Acad::eOk)
					{
						for (int i = 0; i < offsetCurves.length(); i++)
						{
							// AcDbPolyline * pPline = (AcDbPolyline *) offsetCurves[i];
							AcDbPolyline* pPline = AcDbPolyline::cast((AcDbEntity*)offsetCurves[i]);
							if (pPline)
							{
								pPline->setLayer(prop.LayerId());
								mode->geometry().draw(pPline);
							}
							delete (AcDbEntity*)offsetCurves[i];
						}
						offsetCurves.setPhysicalLength(0);
					}
					// create offset curves on the opposite side of the entity.
					if (getOffsetCurves(-prop.Offset(), offsetCurves) == Acad::eOk)
					{
						AcGePoint3d pt1, pt2;
						getPointAt(numVerts() - 1, pt1);
						for (int i = 0; i < offsetCurves.length(); i++)
						{
							//AcDbPolyline * pPline = (AcDbPolyline *) offsetCurves[i];
							AcDbPolyline* pPline = AcDbPolyline::cast((AcDbEntity*)offsetCurves[i]);
							if (pPline)
							{
								pPline->setLayer(prop.LayerId());
								mode->geometry().draw(pPline);
							}
							delete (AcDbEntity*)offsetCurves[i];
						}
						offsetCurves.setPhysicalLength(0);
					}
				}
			}
			traits.setLayer(layerId());
		}
	}

	return (AcDbPolyline::worldDraw(mode));
}




Acad::ErrorStatus EPlineDxf::GetOffsetProperty(int nItem, COffsetProperty& prop)
{
	if (nItem < 0 || nItem >= NumberOfOffsetProperties())
		return Acad::eOutOfRange;

	prop = m_properties[nItem];
	return Acad::eOk;
}

Acad::ErrorStatus EPlineDxf::AddOffsetProperty(COffsetProperty& prop)
{
	// for purposes of the tut and to keep rendering speeds up lets
	// limit the number of offset properties EPlineDxf is allowed to
	// have.
	if (m_properties.length() > MAX_PROPERTIES - 1)
		return Acad::eOutOfRange;

	m_properties.append(prop);
	return Acad::eOk;
}

Acad::ErrorStatus EPlineDxf::AddOffsetProperty(double dDist)
{
	return AddOffsetProperty(COffsetProperty(CurrentLayer(), dDist));
}

// Convert EPlineDxf to AcDbPolyine
AcDbPolyline* EPlineDxf::Convert(void)
{
	AcDbPolyline* pPline = new AcDbPolyline;
	pPline->setPropertiesFrom(this);
	pPline->setElevation(elevation());
	pPline->setClosed(isClosed());
	pPline->setConstantWidth(0);
	Acad::ErrorStatus es;
	for (uint i = 0; i < numVerts(); i++)
	{
		double dBulge;
		AcGePoint2d pt;
		es = getPointAt(i, pt);
		es = pPline->addVertexAt(pPline->numVerts(), pt);
		es = getBulgeAt(i, dBulge);
		es = pPline->setBulgeAt(i, dBulge);
	}
	return pPline;
}

// Convert AcDbPolyline to EPlineDxf
void EPlineDxf::Convert(AcDbPolyline* pPline)
{
	setPropertiesFrom(pPline);
	setElevation(pPline->elevation());
	setClosed(pPline->isClosed());
	setConstantWidth(0);
	Acad::ErrorStatus es;
	for (uint i = 0; i < pPline->numVerts(); i++)
	{
		double dBulge;
		AcGePoint2d pt;
		es = pPline->getPointAt(i, pt);
		es = addVertexAt(numVerts(), pt);
		es = pPline->getBulgeAt(i, dBulge);
		es = setBulgeAt(i, dBulge);
	}
}