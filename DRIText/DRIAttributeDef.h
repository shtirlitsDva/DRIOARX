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
//----- DRIAttributeDef.h : Declaration of the DRIAttributeDef
//-----------------------------------------------------------------------------
#pragma once

//#ifdef DRITEXTOBJ_MODULE
//#define DLLIMPEXP __declspec(dllexport)
//#else
//#define DLLIMPEXP
//#endif
#define DRIATTRIBUTEDEFOBJ_DBXSERVICE _T("DRIATTRIBUTEDEFOBJ_DBXSERVICE")
//-----------------------------------------------------------------------------
#include "dbents.h"
#include "DRITextObject.h"

//-----------------------------------------------------------------------------
class DLLIMPEXP DRIAttributeDef : public AcDbAttributeDefinition {

public:
	ACRX_DECLARE_MEMBERS(DRIAttributeDef) ;

protected:
	static Adesk::UInt32 kCurrentVersionNumber ;

public:
	DRIAttributeDef () ;
	virtual ~DRIAttributeDef () ;

	//----- AcDbObject protocols
	//- Dwg Filing protocol
	virtual Acad::ErrorStatus dwgOutFields (AcDbDwgFiler *pFiler) const ;
	virtual Acad::ErrorStatus dwgInFields (AcDbDwgFiler *pFiler) ;

	//- Dxf Filing protocol
	virtual Acad::ErrorStatus dxfOutFields (AcDbDxfFiler *pFiler) const ;
	virtual Acad::ErrorStatus dxfInFields (AcDbDxfFiler *pFiler) ;

	//- SubXXX() methods (self notification)
	virtual Acad::ErrorStatus subOpen (AcDb::OpenMode mode) ;
	virtual Acad::ErrorStatus subErase (Adesk::Boolean erasing) ;
	virtual Acad::ErrorStatus subCancel () ;
	virtual Acad::ErrorStatus subClose () ;

	//- Persistent reactor callbacks
	virtual void openedForModify (const AcDbObject *pDbObj) ;
	virtual void cancelled (const AcDbObject *pDbObj) ;
	virtual void objectClosed (const AcDbObjectId objId) ;
	virtual void goodbye (const AcDbObject *pDbObj) ;
	virtual void copied (const AcDbObject *pDbObj, const AcDbObject *pNewObj) ;
	virtual void erased (const AcDbObject *pDbObj, Adesk::Boolean bErasing =true) ;
	virtual void modified (const AcDbObject *pDbObj) ;
	virtual void modifiedGraphics (const AcDbEntity *pDbEnt) ;
	virtual void modifiedXData (const AcDbObject *pDbObj) ;
	virtual void subObjModified (const AcDbObject *pMainbObj, const AcDbObject *pSubObj) ;
	virtual void modifyUndone (const AcDbObject *pDbObj) ;
	virtual void reappended (const AcDbObject *pDbObj) ;
	virtual void unappended (const AcDbObject *pDbObj) ;

	//----- deepClone
	virtual Acad::ErrorStatus subDeepClone (AcDbObject *pOwnerObject, AcDbObject *&pClonedObject, AcDbIdMapping &idMap, Adesk::Boolean isPrimary =true) const ;

	//----- wblockClone
	virtual Acad::ErrorStatus subWblockClone (AcRxObject *pOwnerObject, AcDbObject *&pClonedObject, AcDbIdMapping &idMap, Adesk::Boolean isPrimary =true) const ;

	//----- AcDbEntity protocols
	//- Graphics protocol
protected:
	virtual void subViewportDraw (AcGiViewportDraw *mode) ;
	virtual Adesk::UInt32 subViewportDrawLogicalFlags (AcGiViewportDraw *vd) ;
	virtual Adesk::Boolean subWorldDraw (AcGiWorldDraw *mode) ;
	virtual Adesk::UInt32 subSetAttributes (AcGiDrawableTraits *traits) ;

	//- Osnap points protocol
public:
	virtual Acad::ErrorStatus subGetOsnapPoints (
		AcDb::OsnapMode osnapMode,
		Adesk::GsMarker gsSelectionMark,
		const AcGePoint3d &pickPoint,
		const AcGePoint3d &lastPoint,
		const AcGeMatrix3d &viewXform,
		AcGePoint3dArray &snapPoints,
		AcDbIntArray &geomIds) const ;
	virtual Acad::ErrorStatus subGetOsnapPoints (
		AcDb::OsnapMode osnapMode,
		Adesk::GsMarker gsSelectionMark,
		const AcGePoint3d &pickPoint,
		const AcGePoint3d &lastPoint,
		const AcGeMatrix3d &viewXform,
		AcGePoint3dArray &snapPoints,
		AcDbIntArray &geomIds,
		const AcGeMatrix3d &insertionMat) const ;

	//- Grip points protocol
	virtual Acad::ErrorStatus subGetGripPoints (AcGePoint3dArray &gripPoints, AcDbIntArray &osnapModes, AcDbIntArray &geomIds) const ;
	virtual Acad::ErrorStatus subMoveGripPointsAt (const AcDbIntArray &indices, const AcGeVector3d &offset) ;
	virtual Acad::ErrorStatus subGetGripPoints (
		AcDbGripDataPtrArray &grips, const double curViewUnitSize, const int gripSize, 
		const AcGeVector3d &curViewDir, const int bitflags) const ;
	virtual Acad::ErrorStatus subMoveGripPointsAt (const AcDbVoidPtrArray &gripAppData, const AcGeVector3d &offset, const int bitflags) ;

} ;

#ifdef DRITEXTOBJ_MODULE
ACDB_REGISTER_OBJECT_ENTRY_AUTO(DRIAttributeDef)
#endif
