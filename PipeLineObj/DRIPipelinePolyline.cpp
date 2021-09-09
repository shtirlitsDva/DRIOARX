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
//----- DRIPipelinePolyline.cpp : Implementation of DRIPipelinePolyline
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "DRIPipelinePolyline.h"
#include "Utilities.h"

//-----------------------------------------------------------------------------
Adesk::UInt32 DRIPipelinePolyline::kCurrentVersionNumber = 1;

//-----------------------------------------------------------------------------
ACRX_DXF_DEFINE_MEMBERS(
	DRIPipelinePolyline, AcDbPolyline,
	AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent,
	AcDbProxyEntity::kNoOperation, DRIPIPELINEPOLYLINE,
	DRIPIPELINEOBJAPP
	| Product Desc : A description for your object
	| Company : Your company name
	| WEB Address : Your company WEB site address
)

//-----------------------------------------------------------------------------
DRIPipelinePolyline::DRIPipelinePolyline() : AcDbPolyline()
{}

DRIPipelinePolyline::~DRIPipelinePolyline()
{}

void DRIPipelinePolyline::InitializeSegments()
{
	assertWriteEnabled();
	aSize.append(200);
}

void DRIPipelinePolyline::InsertSize(int newSize, ads_point pt)
{
	assertWriteEnabled();
	//Function assumes that segment tracking is in valid state
	AcGePoint3d closestPoint;
	if (getClosestPointTo(asPnt3d(pt), closestPoint) != Acad::eOk)
	{
		acutPrintf(_T("\ngetClosestPointTo failed for some reason!"));
		return;
	}
	double insertDist = 0;
	if (getDistAtPoint(closestPoint, insertDist) != Acad::eOk)
	{
		acutPrintf(_T("\ngetDistAtPoint failed for some reason!"));
		return;
	}
	//Detect existing segment
	int existingSegmentIdx = 0;
	for (int i = 0; i < aSegments.length(); i++)
	{
		if (insertDist < aSegments[i])
		{
			existingSegmentIdx = i;
		}
	}

	//Handle a special case of only one size at all
	//Here the size is inserted after the start of the first one
	if (aSegments.length() == 1)
	{
		aSize.append(newSize);
		aSegments[0] = insertDist;
		double endParam;
		if (getEndParam(endParam) != Acad::eOk)
			endParam = 0;

		double length;
		if (getDistAtParam(endParam, length) != Acad::eOk)
			length = 0;
		aSegments.append(length);

		//Always consolidate after size change
		ConsolidateSizes();
		UpdateLastSegment();
	}
	//Handle a special case where the selected segment is last
	//It needs to be split up
	else if (existingSegmentIdx == aSegments.length() - 1)
	{
		aSize.append(newSize);
		aSegments[existingSegmentIdx] = insertDist;
		UpdateLastSegment();
		ConsolidateSizes();
	}
	else
	{
		aSize.insertAt(existingSegmentIdx, newSize);
		aSegments.insertAt(existingSegmentIdx, insertDist);
		//Always consolidate after size change
		ConsolidateSizes();
		UpdateLastSegment();
	}
}

void DRIPipelinePolyline::ChangeSize(int newSize, ads_point pt)
{
	assertWriteEnabled();
	//Function assumes that segment tracking is in valid state
	AcGePoint3d closestPoint;
	if (getClosestPointTo(asPnt3d(pt), closestPoint) != Acad::eOk)
	{
		acutPrintf(_T("\ngetClosestPointTo failed for some reason!"));
		return;
	}
	double insertDist = 0;
	if (getDistAtPoint(closestPoint, insertDist) != Acad::eOk)
	{
		acutPrintf(_T("\ngetDistAtPoint failed for some reason!"));
		return;
	}
	//Detect existing segment
	int existingSegmentIdx = 0;
	for (int i = 0; i < aSegments.length(); i++)
	{
		if (insertDist < aSegments[i])
		{
			existingSegmentIdx = i;
		}
	}

	//Handle a special case of only one size at all
	//Here the size is inserted after the start of the first one
	
	if (aSegments.length() == 1)
	{
		aSize.append(newSize);
		aSegments[0] = insertDist;
		double endParam;
		if (getEndParam(endParam) != Acad::eOk)
			endParam = 0;

		double length;
		if (getDistAtParam(endParam, length) != Acad::eOk)
			length = 0;
		aSegments.append(length);

		//Always consolidate after size change
		ConsolidateSizes();
	}
}

void DRIPipelinePolyline::ConsolidateSizes()
{//Assumes state is valid
	assertWriteEnabled();
	//If length()==1 abort or there'll be out of bounds
	if (aSize.length() == 1) return;
	
	int length = aSize.length();
	//Forward looking -> length()-1
	for (int i = 0; i < length - 1; i++)
	{
		if (aSize[i] == aSize[i + 1])
		{
			//Detected equal sizes adjacent
			aSize.removeAt(i);
			aSegments.removeAt(i);
			length--;
		}
	}
}

void DRIPipelinePolyline::AddSize(int newSize)
{
	assertWriteEnabled();
	int sizeLength = aSize.length();
	int segmentsLength = aSegments.length();

	double endParam;
	if (getEndParam(endParam) != Acad::eOk)
		endParam = 0;

	double length;
	if (getDistAtParam(endParam, length) != Acad::eOk)
		length = 0;

	AcGeTol* tol = new AcGeTol();
	
	//if ((aSegments[segmentsLength - 1] - length) <= tol->equalPoint())
	//{
	//	//Means the size has just been added
	//	//second time without changing the length of the pline
	//	aSize[sizeLength - 1] = newSize;
	//}
	//else
	{
		aSize.append(newSize);
	}
}

//This method assumes that a size has just been added
void DRIPipelinePolyline::UpdateLastSegment()
{
	assertWriteEnabled();
	int sizeLength = aSize.length();
	int segmentsLength = aSegments.length();

	double endParam;
	if (getEndParam(endParam) != Acad::eOk)
		endParam = 0;

	double length;
	if (getDistAtParam(endParam, length) != Acad::eOk)
		length = 0;
	
	//The number of sizes corresponds to number of segments
	//Updating last segment
	if (sizeLength == segmentsLength)
	{
		aSegments[aSegments.length() - 1] = length;
	}
	//Missing a distance at last size
	if (sizeLength == segmentsLength + 1)
	{
		aSegments.append(length);
	}
}

void DRIPipelinePolyline::PrintInfo()
{
	assertReadEnabled();
	acutPrintf(_T("\n"));
	acutPrintf(_T("\naSize length: %d"), aSize.length());
	for (int i = 0; i < aSize.length(); i++)
	{
		acutPrintf(_T("\n%d: %d"), i, aSize[i]);
	}
	acutPrintf(_T("\naSegments length: %d"), aSegments.length());
	for (int i = 0; i < aSegments.length(); i++)
	{
		acutPrintf(_T("\n%d: %f"), i, aSegments[i]);
	}
}

Enums::Supplier DRIPipelinePolyline::Supplier() const
{
	assertReadEnabled();
	return mSupplier;
}

Acad::ErrorStatus DRIPipelinePolyline::SetSupplier(const Enums::Supplier supplier)
{
	assertWriteEnabled();
	mSupplier = supplier;
	return Acad::eOk;
}

Enums::Type DRIPipelinePolyline::PipeType() const
{
	assertReadEnabled();
	return mPipetype;
}

Acad::ErrorStatus DRIPipelinePolyline::SetPipeType(Enums::Type type)
{
	assertWriteEnabled();
	mPipetype = type;
	return Acad::eOk;
}

Enums::System DRIPipelinePolyline::System() const
{
	assertReadEnabled();
	return mSystem;
}

Acad::ErrorStatus DRIPipelinePolyline::SetSystem(Enums::System system)
{
	assertWriteEnabled();
	mSystem = system;
	return Acad::eOk;
}

Enums::Series DRIPipelinePolyline::Series() const
{
	assertReadEnabled();
	return mSeries;
}

Acad::ErrorStatus DRIPipelinePolyline::SetSeries(Enums::Series series)
{
	assertWriteEnabled();
	mSeries = series;
	return Acad::eOk;
}



//-----------------------------------------------------------------------------
//----- AcDbObject protocols
//- Dwg Filing protocol
Acad::ErrorStatus DRIPipelinePolyline::dwgOutFields(AcDbDwgFiler * pFiler) const
{
	assertReadEnabled();
	//----- Save parent class information first.
	Acad::ErrorStatus es = AcDbPolyline::dwgOutFields(pFiler);
	if (es != Acad::eOk)
		return (es);
	//----- Object version number needs to be saved first
	if ((es = pFiler->writeUInt32(DRIPipelinePolyline::kCurrentVersionNumber)) != Acad::eOk)
		return (es);
	//----- Output params
	//aSize
	pFiler->writeInt16(aSize.length());
	for (int i = 0; i < aSize.length(); i++)
	{
		pFiler->writeInt16(aSize[i]);
	}
	//aSegments
	pFiler->writeInt16(aSegments.length());
	for (int i = 0; i < aSegments.length(); i++)
	{
		pFiler->writeDouble(aSegments[i]);
	}
	{
		//Supplier
		pFiler->writeInt8((Adesk::Int8)Supplier());
		//Type
		pFiler->writeInt8((Adesk::Int8)PipeType());
		//System
		pFiler->writeInt8((Adesk::Int8)System());
		//Series
		pFiler->writeInt8((Adesk::Int8)Series());
	}

	return (pFiler->filerStatus());
}

Acad::ErrorStatus DRIPipelinePolyline::dwgInFields(AcDbDwgFiler * pFiler)
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
	if (version > DRIPipelinePolyline::kCurrentVersionNumber)
		return (Acad::eMakeMeProxy);
	//- Uncomment the 2 following lines if your current object implementation cannot
	//- support previous version of that object.
	//if ( version < DRIPipelinePolyline::kCurrentVersionNumber )
	//	return (Acad::eMakeMeProxy) ;
	//----- Read params
	//aSize
	Adesk::Int16 aSizeLength;
	pFiler->readInt16(&aSizeLength);
	for (int i = 0; i < (int)aSizeLength; i++)
	{
		Adesk::Int16 value;
		pFiler->readInt16(&value);
		aSize.append((int)value);
	}
	//aSegments
	Adesk::Int16 aSegmentsLength;
	pFiler->readInt16(&aSegmentsLength);
	for (int i = 0; i < (int)aSegmentsLength; i++)
	{
		double value;
		pFiler->readDouble(&value);
		aSegments.append(value);
	}
	{
		//Supplier
		Adesk::Int8 value;
		pFiler->readInt8(&value);
		SetSupplier((Enums::Supplier)value);
	}
	{
		//Type
		Adesk::Int8 value;
		pFiler->readInt8(&value);
		SetPipeType((Enums::Type)value);
	}
	{
		//System
		Adesk::Int8 value;
		pFiler->readInt8(&value);
		SetSystem((Enums::System)value);
	}
	{
		//Series
		Adesk::Int8 value;
		pFiler->readInt8(&value);
		SetSeries((Enums::Series)value);
	}

	return (pFiler->filerStatus());
}

//- Dxf Filing protocol
Acad::ErrorStatus DRIPipelinePolyline::dxfOutFields(AcDbDxfFiler * pFiler) const
{
	assertReadEnabled();
	//----- Save parent class information first.
	Acad::ErrorStatus es = AcDbPolyline::dxfOutFields(pFiler);
	if (es != Acad::eOk)
		return (es);
	es = pFiler->writeItem(AcDb::kDxfSubclass, _RXST("DRIPipelinePolyline"));
	if (es != Acad::eOk)
		return (es);
	//----- Object version number needs to be saved first
	if ((es = pFiler->writeUInt32(kDxfInt32, DRIPipelinePolyline::kCurrentVersionNumber)) != Acad::eOk)
		return (es);
	//----- Output params
	//.....

	return (pFiler->filerStatus());
}

Acad::ErrorStatus DRIPipelinePolyline::dxfInFields(AcDbDxfFiler * pFiler)
{
	assertWriteEnabled();
	//----- Read parent class information first.
	Acad::ErrorStatus es = AcDbPolyline::dxfInFields(pFiler);
	if (es != Acad::eOk || !pFiler->atSubclassData(_RXST("DRIPipelinePolyline")))
		return (pFiler->filerStatus());
	//----- Object version number needs to be read first
	struct resbuf rb;
	pFiler->readItem(&rb);
	if (rb.restype != AcDb::kDxfInt32)
	{
		pFiler->pushBackItem();
		pFiler->setError(Acad::eInvalidDxfCode, _RXST("\nError: expected group code %d (version #)"), AcDb::kDxfInt32);
		return (pFiler->filerStatus());
	}
	Adesk::UInt32 version = (Adesk::UInt32)rb.resval.rlong;
	if (version > DRIPipelinePolyline::kCurrentVersionNumber)
		return (Acad::eMakeMeProxy);
	//- Uncomment the 2 following lines if your current object implementation cannot
	//- support previous version of that object.
	//if ( version < DRIPipelinePolyline::kCurrentVersionNumber )
	//	return (Acad::eMakeMeProxy) ;
	//----- Read params in non order dependant manner
	while (es == Acad::eOk && (es = pFiler->readResBuf(&rb)) == Acad::eOk)
	{
		switch (rb.restype)
		{
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
			pFiler->pushBackItem();
			es = Acad::eEndOfFile;
			break;
		}
	}
	//----- At this point the es variable must contain eEndOfFile
	//----- - either from readResBuf() or from pushback. If not,
	//----- it indicates that an error happened and we should
	//----- return immediately.
	if (es != Acad::eEndOfFile)
		return (Acad::eInvalidResBuf);

	return (pFiler->filerStatus());
}

//- SubXXX() methods (self notification)
Acad::ErrorStatus DRIPipelinePolyline::subOpen(AcDb::OpenMode mode)
{
	return (AcDbPolyline::subOpen(mode));
}

Acad::ErrorStatus DRIPipelinePolyline::subErase(Adesk::Boolean erasing)
{
	return (AcDbPolyline::subErase(erasing));
}

Acad::ErrorStatus DRIPipelinePolyline::subCancel()
{
	return (AcDbPolyline::subCancel());
}

Acad::ErrorStatus DRIPipelinePolyline::subClose()
{
	return (AcDbPolyline::subClose());
}

//- Persistent reactor callbacks
void DRIPipelinePolyline::openedForModify(const AcDbObject * pDbObj)
{
	assertReadEnabled();
	acutPrintf(_T("\nObject opened for modify!"));
	AcDbPolyline::openedForModify(pDbObj);
}

void DRIPipelinePolyline::cancelled(const AcDbObject * pDbObj)
{
	assertReadEnabled();
	AcDbPolyline::cancelled(pDbObj);
}

void DRIPipelinePolyline::objectClosed(const AcDbObjectId objId)
{
	assertReadEnabled();
	AcDbPolyline::objectClosed(objId);
}

void DRIPipelinePolyline::goodbye(const AcDbObject * pDbObj)
{
	assertReadEnabled();
	AcDbPolyline::goodbye(pDbObj);
}

void DRIPipelinePolyline::copied(const AcDbObject * pDbObj, const AcDbObject * pNewObj)
{
	assertReadEnabled();
	AcDbPolyline::copied(pDbObj, pNewObj);
}

void DRIPipelinePolyline::erased(const AcDbObject * pDbObj, Adesk::Boolean bErasing)
{
	assertReadEnabled();
	AcDbPolyline::erased(pDbObj, bErasing);
}

void DRIPipelinePolyline::modified(const AcDbObject * pDbObj)
{
	assertReadEnabled();
	AcDbPolyline::modified(pDbObj);
}

void DRIPipelinePolyline::modifiedGraphics(const AcDbEntity * pDbEnt)
{
	assertReadEnabled();
	AcDbPolyline::modifiedGraphics(pDbEnt);
}

void DRIPipelinePolyline::modifiedXData(const AcDbObject * pDbObj)
{
	assertReadEnabled();
	AcDbPolyline::modifiedXData(pDbObj);
}

void DRIPipelinePolyline::subObjModified(const AcDbObject * pMainbObj, const AcDbObject * pSubObj)
{
	assertReadEnabled();
	AcDbPolyline::subObjModified(pMainbObj, pSubObj);
}

void DRIPipelinePolyline::modifyUndone(const AcDbObject * pDbObj)
{
	assertReadEnabled();
	AcDbPolyline::modifyUndone(pDbObj);
}

void DRIPipelinePolyline::reappended(const AcDbObject * pDbObj)
{
	assertReadEnabled();
	AcDbPolyline::reappended(pDbObj);
}

void DRIPipelinePolyline::unappended(const AcDbObject * pDbObj)
{
	assertReadEnabled();
	AcDbPolyline::unappended(pDbObj);
}

//-----------------------------------------------------------------------------
//----- AcDbEntity protocols
Adesk::Boolean DRIPipelinePolyline::subWorldDraw(AcGiWorldDraw * mode)
{
	assertReadEnabled();
	if (mode)
	{
		UpdateLastSegment();

		{
			/// <summary>
			/// Draws pipe size labels at specified interval.
			/// Change labelDist to adjust distance between labels.
			/// </summary>
			double labelDist = 50;
			double labelOffset = 1.2;
			double endParam;
			if (getEndParam(endParam) != Acad::eOk)
				endParam = 0;
			double pipelineLength;
			if (getDistAtParam(endParam, pipelineLength) != Acad::eOk)
				pipelineLength = 0;
			int numberOfLabels = pipelineLength / labelDist;
			for (int i = 0; i < numberOfLabels; i++)
			{
				double dist = labelDist * i;
				int size = SizeAtDist(dist);
				AcGePoint3d labelPt;
				getPointAtDist(dist, labelPt);
				AcString label;
				int key = calculateKey();
				double od = LookUps::GetOd(key, size);
				double kOd = LookUps::GetKOd(key, size);
				label.format(_T("DN%d-ø%.1f+%.1f/%d"), size, od, od, (int)kOd);
				AcGeVector3d deriv;
				getFirstDeriv(labelPt, deriv);
				deriv.normalize();
				AcGeVector3d perp{ AcGeVector3d(deriv) };
				perp.rotateBy(3.14159265359 / 2.0, AcGeVector3d::kZAxis);
				mode->geometry().text(labelPt + perp * labelOffset, AcGeVector3d::kZAxis, deriv, 1.2, 1.0, 0.0, label);
			}
		}
		{
			/// <summary>
			/// Draws perpendicular line at size changes.
			/// </summary
			for (int i = 0; i < aSegments.length(); i++)
			{
				AcGePoint3d pti;
				if (getPointAtDist(aSegments[i], pti) != Acad::eOk)
					continue;
				AcGeVector3d deriv;
				getFirstDeriv(pti, deriv);
				deriv.normalize();
				deriv.rotateBy(3.14159265359 / 2.0, AcGeVector3d::kZAxis);
				AcGePoint3d pt1 = pti + deriv * 3;
				AcGePoint3d pt2 = pti - deriv * 3;
				AcGePoint3d pts[2] = { pt1, pt2 };
				mode->geometry().worldLine(pts);
			}
		}
	}
	//------ Returning Adesk::kFalse here will force viewportDraw() call
	return (Adesk::kFalse);
}

int DRIPipelinePolyline::SizeAtDist(double dist) const
{
	assertReadEnabled();
	for (int i = 0; i < aSize.length(); i++)
		if (dist <= aSegments[i]) return aSize[i];
	return 0;
}

int DRIPipelinePolyline::calculateKey() const
{
	return
		(int)Supplier() * 1000 +
		(int)PipeType() * 100 +
		(int)System() * 10 +
		(int)Series();
}

void DRIPipelinePolyline::subViewportDraw(AcGiViewportDraw * mode)
{
	assertReadEnabled();
	AcDbPolyline::subViewportDraw(mode);
}

Adesk::UInt32 DRIPipelinePolyline::subViewportDrawLogicalFlags(AcGiViewportDraw * vd)
{
	assertReadEnabled();
	return (AcDbPolyline::subViewportDrawLogicalFlags(vd));
}

Adesk::UInt32 DRIPipelinePolyline::subSetAttributes(AcGiDrawableTraits * traits)
{
	assertReadEnabled();
	return (AcDbPolyline::subSetAttributes(traits));
}

//- Osnap points protocol
Acad::ErrorStatus DRIPipelinePolyline::subGetOsnapPoints(
	AcDb::OsnapMode osnapMode,
	Adesk::GsMarker gsSelectionMark,
	const AcGePoint3d & pickPoint,
	const AcGePoint3d & lastPoint,
	const AcGeMatrix3d & viewXform,
	AcGePoint3dArray & snapPoints,
	AcDbIntArray & geomIds) const
{
	assertReadEnabled();
	return (AcDbPolyline::subGetOsnapPoints(osnapMode, gsSelectionMark, pickPoint, lastPoint, viewXform, snapPoints, geomIds));
}

Acad::ErrorStatus DRIPipelinePolyline::subGetOsnapPoints(
	AcDb::OsnapMode osnapMode,
	Adesk::GsMarker gsSelectionMark,
	const AcGePoint3d & pickPoint,
	const AcGePoint3d & lastPoint,
	const AcGeMatrix3d & viewXform,
	AcGePoint3dArray & snapPoints,
	AcDbIntArray & geomIds,
	const AcGeMatrix3d & insertionMat) const
{
	assertReadEnabled();
	return (AcDbPolyline::subGetOsnapPoints(osnapMode, gsSelectionMark, pickPoint, lastPoint, viewXform, snapPoints, geomIds, insertionMat));
}

//- Grip points protocol
Acad::ErrorStatus DRIPipelinePolyline::subGetGripPoints(
	AcGePoint3dArray & gripPoints, AcDbIntArray & osnapModes, AcDbIntArray & geomIds
) const
{
	assertReadEnabled();
	//----- This method is never called unless you return eNotImplemented 
	//----- from the new getGripPoints() method below (which is the default implementation)

	return (AcDbPolyline::subGetGripPoints(gripPoints, osnapModes, geomIds));
}

Acad::ErrorStatus DRIPipelinePolyline::subMoveGripPointsAt(const AcDbIntArray & indices, const AcGeVector3d & offset)
{
	assertWriteEnabled();
	//----- This method is never called unless you return eNotImplemented 
	//----- from the new moveGripPointsAt() method below (which is the default implementation)

	return (AcDbPolyline::subMoveGripPointsAt(indices, offset));
}

Acad::ErrorStatus DRIPipelinePolyline::subGetGripPoints(
	AcDbGripDataPtrArray & grips, const double curViewUnitSize, const int gripSize,
	const AcGeVector3d & curViewDir, const int bitflags
) const
{
	assertReadEnabled();

	//----- If you return eNotImplemented here, that will force AutoCAD to call
	//----- the older getGripPoints() implementation. The call below may return
	//----- eNotImplemented depending of your base class.
	return (AcDbPolyline::subGetGripPoints(grips, curViewUnitSize, gripSize, curViewDir, bitflags));
}

Acad::ErrorStatus DRIPipelinePolyline::subMoveGripPointsAt(
	const AcDbVoidPtrArray & gripAppData, const AcGeVector3d & offset,
	const int bitflags
)
{
	assertWriteEnabled();

	//----- If you return eNotImplemented here, that will force AutoCAD to call
	//----- the older getGripPoints() implementation. The call below may return
	//----- eNotImplemented depending of your base class.
	return (AcDbPolyline::subMoveGripPointsAt(gripAppData, offset, bitflags));
}

//-----------------------------------------------------------------------------
//----- AcDbCurve protocols
//- Curve property tests.
Adesk::Boolean DRIPipelinePolyline::isClosed() const
{
	assertReadEnabled();
	return (AcDbPolyline::isClosed());
}

Adesk::Boolean DRIPipelinePolyline::isPeriodic() const
{
	assertReadEnabled();
	return (AcDbPolyline::isPeriodic());
}

//- Get planar and start/end geometric properties.
Acad::ErrorStatus DRIPipelinePolyline::getStartParam(double& param) const
{
	assertReadEnabled();
	return (AcDbPolyline::getStartParam(param));
}

Acad::ErrorStatus DRIPipelinePolyline::getEndParam(double& param) const
{
	assertReadEnabled();
	return (AcDbPolyline::getEndParam(param));
}

Acad::ErrorStatus DRIPipelinePolyline::getStartPoint(AcGePoint3d & point) const
{
	assertReadEnabled();
	return (AcDbPolyline::getStartPoint(point));
}

Acad::ErrorStatus DRIPipelinePolyline::getEndPoint(AcGePoint3d & point) const
{
	assertReadEnabled();
	return (AcDbPolyline::getEndPoint(point));
}

//- Conversions to/from parametric/world/distance.
Acad::ErrorStatus DRIPipelinePolyline::getPointAtParam(double param, AcGePoint3d & point) const
{
	assertReadEnabled();
	return (AcDbPolyline::getPointAtParam(param, point));
}

Acad::ErrorStatus DRIPipelinePolyline::getParamAtPoint(const AcGePoint3d & point, double& param) const
{
	assertReadEnabled();
	return (AcDbPolyline::getParamAtPoint(point, param));
}

Acad::ErrorStatus DRIPipelinePolyline::getDistAtParam(double param, double& dist) const
{
	assertReadEnabled();
	return (AcDbPolyline::getDistAtParam(param, dist));
}

Acad::ErrorStatus DRIPipelinePolyline::getParamAtDist(double dist, double& param) const
{
	assertReadEnabled();
	return (AcDbPolyline::getParamAtDist(dist, param));
}

Acad::ErrorStatus DRIPipelinePolyline::getDistAtPoint(const AcGePoint3d & point, double& dist) const
{
	assertReadEnabled();
	return (AcDbPolyline::getDistAtPoint(point, dist));
}

Acad::ErrorStatus DRIPipelinePolyline::getPointAtDist(double dist, AcGePoint3d & point) const
{
	assertReadEnabled();
	return (AcDbPolyline::getPointAtDist(dist, point));
}

//- Derivative information.
Acad::ErrorStatus DRIPipelinePolyline::getFirstDeriv(double param, AcGeVector3d & firstDeriv) const
{
	assertReadEnabled();
	return (AcDbPolyline::getFirstDeriv(param, firstDeriv));
}

Acad::ErrorStatus DRIPipelinePolyline::getFirstDeriv(const AcGePoint3d & point, AcGeVector3d & firstDeriv) const
{
	assertReadEnabled();
	return (AcDbPolyline::getFirstDeriv(point, firstDeriv));
}

Acad::ErrorStatus DRIPipelinePolyline::getSecondDeriv(double param, AcGeVector3d & secDeriv) const
{
	assertReadEnabled();
	return (AcDbPolyline::getSecondDeriv(param, secDeriv));
}

Acad::ErrorStatus DRIPipelinePolyline::getSecondDeriv(const AcGePoint3d & point, AcGeVector3d & secDeriv) const
{
	assertReadEnabled();
	return (AcDbPolyline::getSecondDeriv(point, secDeriv));
}

//- Closest point on curve.
Acad::ErrorStatus DRIPipelinePolyline::getClosestPointTo(const AcGePoint3d & givenPnt, AcGePoint3d & pointOnCurve, Adesk::Boolean extend /*=Adesk::kFalse*/) const
{
	assertReadEnabled();
	return (AcDbPolyline::getClosestPointTo(givenPnt, pointOnCurve, extend));
}

Acad::ErrorStatus DRIPipelinePolyline::getClosestPointTo(const AcGePoint3d & givenPnt, const AcGeVector3d & direction, AcGePoint3d & pointOnCurve, Adesk::Boolean extend /*=Adesk::kFalse*/) const
{
	assertReadEnabled();
	return (AcDbPolyline::getClosestPointTo(givenPnt, direction, pointOnCurve, extend));
}

//- Get a projected copy of the curve.
Acad::ErrorStatus DRIPipelinePolyline::getOrthoProjectedCurve(const AcGePlane & plane, AcDbCurve * &projCrv) const
{
	assertReadEnabled();
	return (AcDbPolyline::getOrthoProjectedCurve(plane, projCrv));
}

Acad::ErrorStatus DRIPipelinePolyline::getProjectedCurve(const AcGePlane & plane, const AcGeVector3d & projDir, AcDbCurve * &projCrv) const
{
	assertReadEnabled();
	return (AcDbPolyline::getProjectedCurve(plane, projDir, projCrv));
}

//- Get offset, spline and split copies of the curve.
Acad::ErrorStatus DRIPipelinePolyline::getOffsetCurves(double offsetDist, AcDbVoidPtrArray & offsetCurves) const
{
	assertReadEnabled();
	return (AcDbPolyline::getOffsetCurves(offsetDist, offsetCurves));
}

Acad::ErrorStatus DRIPipelinePolyline::getOffsetCurvesGivenPlaneNormal(const AcGeVector3d & normal, double offsetDist, AcDbVoidPtrArray & offsetCurves) const
{
	assertReadEnabled();
	return (AcDbPolyline::getOffsetCurvesGivenPlaneNormal(normal, offsetDist, offsetCurves));
}

Acad::ErrorStatus DRIPipelinePolyline::getSpline(AcDbSpline * &spline) const
{
	assertReadEnabled();
	return (AcDbPolyline::getSpline(spline));
}

Acad::ErrorStatus DRIPipelinePolyline::getSplitCurves(const AcGeDoubleArray & params, AcDbVoidPtrArray & curveSegments) const
{
	assertReadEnabled();
	return (AcDbPolyline::getSplitCurves(params, curveSegments));
}

Acad::ErrorStatus DRIPipelinePolyline::getSplitCurves(const AcGePoint3dArray & points, AcDbVoidPtrArray & curveSegments) const
{
	assertReadEnabled();
	return (AcDbPolyline::getSplitCurves(points, curveSegments));
}

//- Extend the curve.
Acad::ErrorStatus DRIPipelinePolyline::extend(double newParam)
{
	assertReadEnabled();
	return (AcDbPolyline::extend(newParam));
}

Acad::ErrorStatus DRIPipelinePolyline::extend(Adesk::Boolean extendStart, const AcGePoint3d & toPoint)
{
	assertReadEnabled();
	return (AcDbPolyline::extend(extendStart, toPoint));
}

//- Area calculation.
Acad::ErrorStatus DRIPipelinePolyline::getArea(double& area) const
{
	assertReadEnabled();
	return (AcDbPolyline::getArea(area));
}

