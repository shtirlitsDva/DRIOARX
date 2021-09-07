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
#include "EPlineTutJig.h"
#include "../EPlineTutDb/EPlineDxf.h"
#include "resource.h"


//-----------------------------------------------------------------------------
#define szRDS _RXST("rpk")

#define _CMD_ALIAS_ 1

//-----------------------------------------------------------------------------
//----- ObjectARX EntryPoint
class CEPlineTutApp : public AcRxArxApp
{

public:
	CEPlineTutApp() : AcRxArxApp() {}

	virtual AcRx::AppRetCode On_kInitAppMsg(void* pkt)
	{
		// TODO: Load dependencies here

		// You *must* call On_kInitAppMsg here
		AcRx::AppRetCode retCode = AcRxArxApp::On_kInitAppMsg(pkt);

		// TODO: Add your initialization code here

		return (retCode);
	}

	virtual AcRx::AppRetCode On_kUnloadAppMsg(void* pkt)
	{
		// TODO: Add your code here

		// You *must* call On_kUnloadAppMsg here
		AcRx::AppRetCode retCode = AcRxArxApp::On_kUnloadAppMsg(pkt);

		// TODO: Unload dependencies here

		return (retCode);
	}

	virtual void RegisterServerComponents()
	{}

#ifdef _CMD_ALIAS_
	// 
	// - rpkEPlineTut.EE command (do not rename)
	static void rpkEPlineTutEE(void)
	{
		rpkEPlineTutEPline();
	}
#endif

	// - rpkEPlineTut.EPline command (do not rename)
	static void rpkEPlineTutEPline(void)
	{
		CEPlineTutJig jig;
		jig.CreateEntity();
	}

	// - rpkEPlineTut.ConvertEPline command (do not rename)
	// Converts an LWPolyline to EPlineDxf and vise-aversa
	static void rpkEPlineTutConvertEPline(void)
	{
		ads_name ename;
		ads_point pt;
		int rVal = acedEntSel(_T("\nSelect a LWPolyline of EPlineDxf to convert: "), ename, pt);

		if (rVal == RTNORM)
		{
			AcDbObjectId objId;
			Acad::ErrorStatus es;
			if ((es = acdbGetObjectId(objId, ename)) == Acad::eOk)
			{
				AcDbEntity* pEnt;
				if ((es = acdbOpenAcDbEntity(pEnt, objId, AcDb::kForRead)) == Acad::eOk)
				{
					EPlineDxf* pEPline = EPlineDxf::cast(pEnt);
					if (pEPline)
					{
						AcDbPolyline* pNewPoly = pEPline->Convert();
						if (pNewPoly)
						{
							pEnt->upgradeOpen();
							es = pEPline->handOverTo(pNewPoly);
							if (es == Acad::eObjectToBeDeleted)
							{
								pNewPoly->close();
								delete pEnt;
								pEnt = NULL;
							}
							else
							{
								delete pNewPoly;
							}
						}
					}
					else
					{
						AcDbPolyline* pLine = AcDbPolyline::cast(pEnt);
						if (pLine)
						{
							EPlineDxf* pNewEPline = new EPlineDxf();
							pNewEPline->Convert(pLine);

							//////////////////////////////////////
							//////////////////////////////////////
							// TODO: Replace hard coded values with user supplied
							// values when the UI is implemented.
							pNewEPline->AddOffsetProperty(5.0);
							pNewEPline->AddOffsetProperty(10.0);
							pNewEPline->AddOffsetProperty(15.0);
							//////////////////////////////////////
							//////////////////////////////////////

							pEnt->upgradeOpen();
							es = pLine->handOverTo(pNewEPline);
							if (es == Acad::eObjectToBeDeleted)
							{
								pNewEPline->close();
								delete pEnt;
								pEnt = NULL;
							}
							else
							{
								delete pNewEPline;
							}
						}
					}
					if (pEnt)
						pEnt->close();
				}
			}
		}
	}
};

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CEPlineTutApp)

ACED_ARXCOMMAND_ENTRY_AUTO(CEPlineTutApp, rpkEPlineTut, EPline, EPline, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CEPlineTutApp, rpkEPlineTut, ConvertEPline, ConvertEPline, ACRX_CMD_TRANSPARENT, NULL)

#ifdef _CMD_ALIAS_
ACED_ARXCOMMAND_ENTRY_AUTO(CEPlineTutApp, rpkEPlineTut, EE, EE, ACRX_CMD_TRANSPARENT, NULL)
#endif