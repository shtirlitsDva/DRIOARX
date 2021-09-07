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
//----- acrxEntryPoint.h
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "resource.h"
#include "EPlineDxf.h"

//-----------------------------------------------------------------------------
#define szRDS _RXST("rpk")

//-----------------------------------------------------------------------------
//----- ObjectARX EntryPoint
class CEPlineTutDbApp : public AcRxDbxApp
{

public:
	CEPlineTutDbApp() : AcRxDbxApp() {}

	virtual AcRx::AppRetCode On_kInitAppMsg(void* pkt)
	{
		// TODO: Load dependencies here
		acrxDynamicLinker->unlockApplication(pkt);
		// You *must* call On_kInitAppMsg here
		AcRx::AppRetCode retCode = AcRxDbxApp::On_kInitAppMsg(pkt);

		// TODO: Add your initialization code here
		acrxRegisterService(EPLINETUTDB_DBXSERVICE);
		return (retCode);
	}

	virtual AcRx::AppRetCode On_kUnloadAppMsg(void* pkt)
	{
		// TODO: Add your code here
		delete acrxServiceDictionary->remove(EPLINETUTDB_DBXSERVICE);
		// You *must* call On_kUnloadAppMsg here
		AcRx::AppRetCode retCode = AcRxDbxApp::On_kUnloadAppMsg(pkt);

		// TODO: Unload dependencies here

		return (retCode);
	}

	virtual void RegisterServerComponents()
	{	}

};

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CEPlineTutDbApp)

