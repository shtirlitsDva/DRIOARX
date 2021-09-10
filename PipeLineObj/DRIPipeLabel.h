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
//----- DRIPipeLabel.h : Declaration of the DRIPipeLabel
//-----------------------------------------------------------------------------
#pragma once
#include "DRIPipelinePolyline.h" //This is to reuse the DLLIMPEXP macro

#ifdef PIPELINELABELOBJ_MODULE
//#define DLLIMPEXP __declspec(dllexport)
//#else
////----- Note: we don't use __declspec(dllimport) here, because of the
////----- "local vtable" problem with msvc. If you use __declspec(dllimport),
////----- then, when a client dll does a new on the class, the object's
////----- vtable pointer points to a vtable allocated in that client
////----- dll. If the client dll then passes the object to another dll,
////----- and the client dll is then unloaded, the vtable becomes invalid
////----- and any virtual calls on the object will access invalid memory.
////-----
////----- By not using __declspec(dllimport), we guarantee that the
////----- vtable is allocated in the server dll during the ctor and the
////----- client dll does not overwrite the vtable pointer after calling
////----- the ctor. And, since we expect the server dll to remain in
////----- memory indefinitely, there is no problem with vtables unexpectedly
////----- going away.
//#define DLLIMPEXP
#endif
#define DRIPIPELABEL_DBXSERVICE _T("DRIPIPELABEL_DBXSERVICE")
//-----------------------------------------------------------------------------
#include "dbents.h"
#include "StdAfx.h"

//-----------------------------------------------------------------------------
class DLLIMPEXP DRIPipeLabel : public AcDbText
{

public:
	ACRX_DECLARE_MEMBERS(DRIPipeLabel);

protected:
	static Adesk::UInt32 kCurrentVersionNumber;

public:
	DRIPipeLabel();
	virtual ~DRIPipeLabel();

	//----- AcDbObject protocols
	//- Dwg Filing protocol
	virtual Acad::ErrorStatus dwgOutFields(AcDbDwgFiler* pFiler) const;
	virtual Acad::ErrorStatus dwgInFields(AcDbDwgFiler* pFiler);

	//- Dxf Filing protocol
	virtual Acad::ErrorStatus dxfOutFields(AcDbDxfFiler* pFiler) const;
	virtual Acad::ErrorStatus dxfInFields(AcDbDxfFiler* pFiler);

	//- SubXXX() methods (self notification)
	virtual Acad::ErrorStatus subOpen(AcDb::OpenMode mode);
	virtual Acad::ErrorStatus subErase(Adesk::Boolean erasing);
	virtual Acad::ErrorStatus subCancel();
	virtual Acad::ErrorStatus subClose();

	//- Persistent reactor callbacks
	virtual void openedForModify(const AcDbObject* pDbObj);
	virtual void cancelled(const AcDbObject* pDbObj);
	virtual void objectClosed(const AcDbObjectId objId);
	virtual void goodbye(const AcDbObject* pDbObj);
	virtual void copied(const AcDbObject* pDbObj, const AcDbObject* pNewObj);
	virtual void erased(const AcDbObject* pDbObj, Adesk::Boolean bErasing = true);
	virtual void modified(const AcDbObject* pDbObj);
	virtual void modifiedGraphics(const AcDbEntity* pDbEnt);
	virtual void modifiedXData(const AcDbObject* pDbObj);
	virtual void subObjModified(const AcDbObject* pMainbObj, const AcDbObject* pSubObj);
	virtual void modifyUndone(const AcDbObject* pDbObj);
	virtual void reappended(const AcDbObject* pDbObj);
	virtual void unappended(const AcDbObject* pDbObj);

	//----- AcDbEntity protocols
	//- Graphics protocol
protected:
	virtual void subViewportDraw(AcGiViewportDraw* mode);
	virtual Adesk::UInt32 subViewportDrawLogicalFlags(AcGiViewportDraw* vd);
	virtual Adesk::Boolean subWorldDraw(AcGiWorldDraw* mode);
	virtual Adesk::UInt32 subSetAttributes(AcGiDrawableTraits* traits);

	//- Grip points protocol
	virtual Acad::ErrorStatus subGetGripPoints(AcGePoint3dArray& gripPoints, AcDbIntArray& osnapModes, AcDbIntArray& geomIds) const;
	virtual Acad::ErrorStatus subMoveGripPointsAt(const AcDbIntArray& indices, const AcGeVector3d& offset);
	virtual Acad::ErrorStatus subGetGripPoints(
		AcDbGripDataPtrArray& grips, const double curViewUnitSize, const int gripSize,
		const AcGeVector3d& curViewDir, const int bitflags) const;
	virtual Acad::ErrorStatus subMoveGripPointsAt(const AcDbVoidPtrArray& gripAppData, const AcGeVector3d& offset, const int bitflags);

};

#ifdef PIPELINELABELOBJ_MODULE
ACDB_REGISTER_OBJECT_ENTRY_AUTO(DRIPipeLabel)
#endif
