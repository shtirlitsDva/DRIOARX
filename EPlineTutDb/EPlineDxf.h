// acrxEntryPoint.cpp
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
//----- EPlineDxf.h : Declaration of the EPlineDxf
//-----------------------------------------------------------------------------
#pragma once

#ifdef EPLINETUTDB_MODULE
#define DLLIMPEXP __declspec(dllexport)
#else
//----- Note: we don't use __declspec(dllimport) here, because of the
//----- "local vtable" problem with msvc. If you use __declspec(dllimport),
//----- then, when a client dll does a new on the class, the object's
//----- vtable pointer points to a vtable allocated in that client
//----- dll. If the client dll then passes the object to another dll,
//----- and the client dll is then unloaded, the vtable becomes invalid
//----- and any virtual calls on the object will access invalid memory.
//-----
//----- By not using __declspec(dllimport), we guarantee that the
//----- vtable is allocated in the server dll during the ctor and the
//----- client dll does not overwrite the vtable pointer after calling
//----- the ctor. And, since we expect the server dll to remain in
//----- memory indefinitely, there is no problem with vtables unexpectedly
//----- going away.
#define DLLIMPEXP
#endif
#define EPLINETUTDB_DBXSERVICE _T("EPLINETUTDB_DBXSERVICE")
//-----------------------------------------------------------------------------
#include "dbpl.h"
#include "OffsetProperty.h"

//-----------------------------------------------------------------------------
class DLLIMPEXP EPlineDxf : public AcDbPolyline {

public:
	ACRX_DECLARE_MEMBERS(EPlineDxf) ;

protected:
	static Adesk::UInt32 kCurrentVersionNumber ;

public:
	EPlineDxf () ;
	virtual ~EPlineDxf () ;

	//----- AcDbObject protocols
	//- Dwg Filing protocol
	virtual Acad::ErrorStatus dwgOutFields (AcDbDwgFiler *pFiler) const ;
	virtual Acad::ErrorStatus dwgInFields (AcDbDwgFiler *pFiler) ;

	//----- AcDbEntity protocols
	//- Graphics protocol
	virtual Adesk::Boolean subWorldDraw (AcGiWorldDraw *mode) ;

	// Property getters and setters
	int NumberOfOffsetProperties(void) { return m_properties.length(); }
	Acad::ErrorStatus AddOffsetProperty(COffsetProperty & prop);
	Acad::ErrorStatus AddOffsetProperty(double dDist);
	Acad::ErrorStatus GetOffsetProperty(int nItem, COffsetProperty & prop );

	AcDbPolyline * Convert(void);
	void Convert(AcDbPolyline * pPline);

	
protected:
	AcDbObjectId CurrentLayer(void) { return acdbHostApplicationServices()->workingDatabase()->clayer(); }
	AcArray<COffsetProperty> m_properties;
} ;

#ifdef EPLINETUTDB_MODULE
ACDB_REGISTER_OBJECT_ENTRY_AUTO(EPlineDxf)
#endif
