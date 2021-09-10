// (C) Copyright 2002-2012 by Autodesk, Inc. 
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
//----- acrxEntryPoint.cpp
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "resource.h"
#include "../PipeLineObj/DRIPipelinePolyline.h"
#include "../PipeLineObj/DRIPipeLabel.h"
#include "DRILineJig.h"
#include "tchar.h"

//-----------------------------------------------------------------------------
#define szRDS _RXST("DRI")

//-----------------------------------------------------------------------------
//----- ObjectARX EntryPoint
class CDRIPipeLineUiApp : public AcRxArxApp
{

public:
	CDRIPipeLineUiApp() : AcRxArxApp() {}

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

	static void DRIPipelineUiTestRotation()
	{
		ads_name ename;
		ads_point pt;
		if (acedEntSel(_T("Select label to set rotation: "), ename, pt) != RTNORM)
			return;

		AcDbObjectId id;
		if (acdbGetObjectId(id, ename) != Acad::eOk)
			return;

		AcDbObjectPointer<DRIPipeLabel> drilabel;
		if (drilabel.open(id, AcDb::kForWrite) != Acad::eOk)
		{
			acutPrintf(_T("\nNot a pipeline!"));
			return;
		}

		/*drilabel->assertWriteEnabled();
		drilabel->myRotation = drilabel->rotation();
		acutPrintf(_T("Rotation: %.6q20"), drilabel->rotation());
		acutPrintf(_T("myRotation: %.6q20"), drilabel->myRotation);
		drilabel->draw();*/
	}

	static void DRIPipelineUiTestLabel()
	{
		while (true)
		{
			ads_point pt;
			if (acedGetPoint(NULL, _T("\nPlace label: "), pt) != RTNORM)
				return;

			AcString text;
			if (acedGetString(1, _T("\nEnter text: "), text) != RTNORM)
				return;

			AcDbObjectPointer<DRIPipeLabel> drilabel;
			drilabel.create();
			drilabel->assertWriteEnabled();
			drilabel->setPosition(asPnt3d(pt));
			drilabel->setTextString(text);

			//Add the pipeline to the database
			AcDbBlockTableRecordPointer pModelSpaceRecord;
			if (pModelSpaceRecord.open(
				ACDB_MODEL_SPACE, acdbHostApplicationServices()->workingDatabase(), AcDb::kForWrite) == Acad::eOk)
			{
				if (pModelSpaceRecord->appendAcDbEntity(drilabel) == Acad::eOk)
					acutPrintf(_T("\nLabel created!"));
				else acutPrintf(_T("\nLabel creation failed!"));
			}
			drilabel->draw();
		}
	}
	static void DRIPipelineUiCreatePipeline()
	{
		AcString result;
		acedInitGet(0, L"<New> Continue");
		int res = acedGetKword(_T("\n[<New> Continue]: "), result);

		//if (res == RTNORM) //{ //Got keyword //} 
		//Got escape key OR Ctrl+C
		if (res == RTCAN) return;
		//Enter pressed
		else if (res == RTNONE) result = _T("New");
		//Determine what kind of keyword we got back
		AcDbObjectPointer<DRIPipelinePolyline> dripl;
		if (result.compare(_T("New")) == 0)
		{
			//new object is created
			dripl.create();
			dripl->InitializeSegments();
		}
		else if (result.compare(_T("Continue")) == 0)
		{
			//Existing object is selected for continuation
			ads_name ename;
			ads_point pt;
			if (acedEntSel(_T("Select pipeline to continue: "), ename, pt) != RTNORM)
				return;

			AcDbObjectId driplId;
			if (acdbGetObjectId(driplId, ename) != Acad::eOk)
				return;

			dripl.open(driplId, AcDb::kForWrite);
			dripl->assertWriteEnabled();
		}

		bool continueLoop = true;
		while (continueLoop)
		{
			//If pipeline still has 0 vertices, it should mean
			//That it is a new object
			//And because I couldn't find a method for Jig to not draw a point from 0,0
			//We must first select a point to start with
			if (dripl->numVerts() == 0)
			{
				ads_point startingPoint;
				int RESULT = 0;
				//The loop loops until a point is given
				//The user can choose to add a size at the prompt
				//Or cancel and exit the command
				while (RESULT != RTNORM)
				{
					acedInitGet(1, L"Size");
					RESULT = acedGetPoint(NULL, _T("\nPlace starting point: [Size]"), startingPoint);
					switch (RESULT)
					{
					case RTCAN:
						return;
					case RTKWORD:
					{
						//The user can also choose to add a size now
						AcString kword;
						acedGetInput(kword);
						if (kword.compare(_T("Size")) == 0)
						{
							int newSize = 0;
							if (acedGetInt(_T("\nEnter new size: "), &newSize) == RTNORM)
							{
								dripl->aSize.at(0) = newSize;
								dripl->UpdateLastSegment();
								dripl->ConsolidateSizes();
							}
						}
					}
					break;
					default:
						break;
					}
				}

				//Add the received point to pline's vertices
				dripl->addVertexAt(dripl->numVerts(), asPnt2d(startingPoint));

				//Loop around until the next point is received
				//The user can choose to add a size
				//Or cancel and exit the command
				AcEdJig::DragStatus result = AcEdJig::kNull;
				while (result != kNormal)
				{
					//Start the acquisition of next point of line
					//Use a line for jigging using the already received point to start with
					//This is to avoid the rubberband from 0,0
					AcDbLine* line = new AcDbLine();
					line->setStartPoint(asPnt3d(startingPoint));
					DRILineJig* lJig = new DRILineJig();
					result = lJig->startJig(line);
					switch (result)
					{
					case AcEdJig::kCancel:
						delete lJig;
						return;
					case AcEdJig::kNormal:
						dripl->addVertexAt(dripl->numVerts(),
							asPnt2d(asDblArray(lJig->foundPoint)));
						dripl->UpdateLastSegment();
						delete lJig;
						break;
					case AcEdJig::kKW1:
					{
						int newSize = 0;
						if (acedGetInt(_T("\nEnter new size: "), &newSize) == RTNORM)
						{
							dripl->AddSize(newSize);
							dripl->UpdateLastSegment();
							dripl->ConsolidateSizes();
						}
						delete lJig;
					}
					break;
					default:
						delete lJig;
						return;
					}
				}

				//Plines with vertex counts of less than 2 may not be posted to database
				if (dripl->numVerts() < 2)
				{
					return;
				}
				//Add the pipeline to the database
				AcDbBlockTableRecordPointer pModelSpaceRecord;
				if (pModelSpaceRecord.open(
					ACDB_MODEL_SPACE, acdbHostApplicationServices()->workingDatabase(), AcDb::kForWrite) == Acad::eOk)
				{
					if (pModelSpaceRecord->appendAcDbEntity(dripl) == Acad::eOk)
					{
						acutPrintf(_T("\nObject created!"));
					}
					else
						acutPrintf(_T("\nObject creation failed!"));
				}
				//Update the last segment -> not sure it is needed now
				dripl->UpdateLastSegment();
				//update the pipline's graphics
				dripl->draw();
				//dripl->close();
			}
			else if (dripl->numVerts() == 1)
			{
				acutPrintf(_T("\nThe DRIpl is in invalid state! Only one vertex present! It has to be deleted!"));
				dripl->erase(true);
				return;
			}
			else
			{
				//Loop around until the next point is received
				//The user can choose to add a size
				//Or cancel and exit the command
				AcEdJig::DragStatus result = AcEdJig::kNull;
				//Setup a previous line to enable polar tracking to last segment

				while (result != kNormal)
				{
					//This code executes if the pipeline has not zero vertices
					// So it does not need to start with an external point
					// It takes the last point on the pipeline either added
					// In an earlier iteration or selected via prompt
					//Use a line for jigging
					//Set it's start point to the last point of previous part of pipeline
					AcDbLine* line = new AcDbLine();
					AcGePoint3d p3d;
					dripl->getPointAt(dripl->numVerts() - 1, p3d);
					line->setStartPoint(p3d);
					DRILineJig* lJig = new DRILineJig();
					result = lJig->startJig(line);
					switch (result)
					{
					case AcEdJig::kCancel:
						//in this CASE line is already deleted in jig
						delete lJig;
						return;
					case AcEdJig::kNormal:
						dripl->addVertexAt(dripl->numVerts(),
							asPnt2d(asDblArray(lJig->foundPoint)));
						dripl->UpdateLastSegment();
						delete lJig;
						break;
					case AcEdJig::kKW1:
					{
						int newSize = 0;
						if (acedGetInt(_T("\nEnter new size: "), &newSize) == RTNORM)
						{
							dripl->AddSize(newSize);
							dripl->UpdateLastSegment();
							dripl->ConsolidateSizes();
						}
						delete lJig;
					}
					break;
					default:
						delete lJig;
						return;
					}
				}

				dripl->UpdateLastSegment();
				dripl->draw();
			}
		}
	}
	static void DRIPipelineUiConvertPipes()
	{
		AcDbObjectId driplId;
		AcString result;
		acedInitGet(0, L"<New> Continue");
		int res = acedGetKword(_T("\n[<New> Continue]: "), result);

		//Got escape key OR Ctrl+C
		if (res == RTCAN) return;
		//Enter pressed
		else if (res == RTNONE) result = _T("New");
		//Determine what kind of keyword we got back
		AcDbObjectPointer<DRIPipelinePolyline> dripl;
		if (result.compare(_T("New")) == 0)
		{
			//new object is created
			dripl.create();
			dripl->InitializeSegments();

			//Add the pipeline to the database
			AcDbBlockTableRecordPointer pModelSpaceRecord;
			if (pModelSpaceRecord.open(
				ACDB_MODEL_SPACE, acdbHostApplicationServices()->workingDatabase(),
				AcDb::kForWrite) == Acad::eOk)
			{
				if (pModelSpaceRecord->appendAcDbEntity(dripl) == Acad::eOk)
				{
					driplId = dripl->id();
					acutPrintf(_T("\nObject created!"));
				}
				else
					acutPrintf(_T("\nObject creation failed!"));
			}
		}
		else if (result.compare(_T("Continue")) == 0)
		{
			//Existing object is selected for continuation
			ads_name ename;
			ads_point pt;
			if (acedEntSel(_T("Select pipeline to continue: "), ename, pt) != RTNORM)
			{
				//Plines with vertex counts of less than 2 may not be posted to database
				if (dripl->numVerts() < 2)
				{
					dripl->erase(true);
				}
				return;
			}

			if (acdbGetObjectId(driplId, ename) != Acad::eOk)
				return;

			if (dripl.open(driplId, AcDb::kForWrite) != Acad::eOk)
			{
				acutPrintf(_T("\nNot a pipeline!"));
				return;
			}
		}

		bool ContinueConvertLoop = true;

		while (ContinueConvertLoop)
		{
			//Existing object is selected for continuation
			ads_name ename;
			ads_point pt;
			if (acedEntSel(_T("Select (poly)line to convert: "), ename, pt) != RTNORM)
			{
				//Plines with vertex counts of less than 2 may not be posted to database
				if (dripl->numVerts() < 2)
				{
					dripl->erase(true);
				}
				return;
			}

			AcDbObjectId plineId;
			if (acdbGetObjectId(plineId, ename) != Acad::eOk)
				continue;

			AcDbObjectPointer<AcDbPolyline> pline;
			if (pline.open(plineId) != Acad::eOk)
			{
				acutPrintf(_T("Selected object is not a polyline!"));
				continue;
			}

			//////////////////////////////////////////////////////////////
			//Detect if polyline is reversed
			//////////////////////////////////////////////////////////////
			if (dripl->numVerts() != 0)
			{
				AcGePoint3d firstPoint; pline->getPointAt(0, firstPoint);
				AcGePoint3d lastPoint; pline->getPointAt(pline->numVerts() - 1, lastPoint);
				AcGePoint3d p2compare; dripl->getPointAt(dripl->numVerts() - 1, p2compare);

				double distToFirst = p2compare.distanceTo(firstPoint);
				double distToLast = p2compare.distanceTo(lastPoint);

				if (distToFirst > distToLast)
				{//Pline is reversed -> reverse it!
					pline->upgradeOpen();
					pline->reverseCurve();
					pline->downgradeOpen();
				}
			}

			{
				for (int i = 0; i < pline->numVerts(); i++)
				{
					AcGePoint3d pt;
					pline->getPointAt(i, pt);
					AcGePoint2d pt2d{ pt.x, pt.y };
					double bulge;
					pline->getBulgeAt(i, bulge);
					dripl->addVertexAt(dripl->numVerts(), pt2d, bulge, 0, 0);
				}
			}
			dripl->UpdateLastSegment();
			dripl->draw();
		}
	}
	static void DRIPipelineUiPrintInfo()
	{
		ads_name ename;
		ads_point pt;
		if (acedEntSel(_T("Select pipeline to continue: "), ename, pt) != RTNORM)
			return;

		AcDbObjectId idO;
		if (acdbGetObjectId(idO, ename) != Acad::eOk)
			return;

		AcDbObjectPointer<DRIPipelinePolyline> dripl;
		if (dripl.open(idO, AcDb::kForWrite) != Acad::eOk)
		{
			acutPrintf(_T("\nNot a pipeline!"));
			return;
		}
		dripl->PrintInfo();
	}
	static void DRIPipelineUiInsertSize()
	{
		ads_name ename;
		ads_point pt;
		if (acedEntSel(_T("Select pipeline to insert size: "), ename, pt) != RTNORM)
			return;

		AcDbObjectId idO;
		if (acdbGetObjectId(idO, ename) != Acad::eOk)
			return;

		AcDbObjectPointer<DRIPipelinePolyline> dripl;
		if (dripl.open(idO, AcDb::kForWrite) != Acad::eOk)
		{
			acutPrintf(_T("\nNot a pipeline!"));
			return;
		}

		ads_point location;
		if (acedGetPoint(NULL, _T("\nPick location where to insert size: "), location) == RTNORM)
		{
			int newSize = 0;
			if (acedGetInt(_T("\nEnter new size: "), &newSize) == RTNORM)
			{
				dripl->InsertSize(newSize, location);
				dripl->draw();
			}
		}
	}
	static void DRIPipelineUiChangeSize()
	{
		ads_name ename;
		ads_point pt;
		if (acedEntSel(_T("Select pipeline where to change size: "), ename, pt) != RTNORM)
			return;

		AcDbObjectId idO;
		if (acdbGetObjectId(idO, ename) != Acad::eOk)
			return;

		AcDbObjectPointer<DRIPipelinePolyline> dripl;
		if (dripl.open(idO, AcDb::kForWrite) != Acad::eOk)
		{
			acutPrintf(_T("\nNot a pipeline!"));
			return;
		}

		ads_point location;
		if (acedGetPoint(NULL, _T("\nPick location where to change size: "), location) == RTNORM)
		{
			int newSize = 0;
			if (acedGetInt(_T("\nEnter new size: "), &newSize) == RTNORM)
			{
				dripl->ChangeSize(newSize, location);
				dripl->draw();
			}
		}
	}
};

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CDRIPipeLineUiApp)

//ACED_ARXCOMMAND_ENTRY_AUTO(CDRIPipeLineUiApp, DRIMyGroup, MyCommand, MyCommandLocal, ACRX_CMD_MODAL, NULL)
//ACED_ARXCOMMAND_ENTRY_AUTO(CDRIPipeLineUiApp, DRIMyGroup, MyPickFirst, MyPickFirstLocal, ACRX_CMD_MODAL | ACRX_CMD_USEPICKSET, NULL)
//ACED_ARXCOMMAND_ENTRY_AUTO(CDRIPipeLineUiApp, DRIMyGroup, MySessionCmd, MySessionCmdLocal, ACRX_CMD_MODAL | ACRX_CMD_SESSION, NULL)
//ACED_ADSSYMBOL_ENTRY_AUTO(CDRIPipeLineUiApp, MyLispFunction, false)
ACED_ARXCOMMAND_ENTRY_AUTO(CDRIPipeLineUiApp, DRIPipelineUi, CreatePipeline, _cpl, ACRX_CMD_MODAL, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CDRIPipeLineUiApp, DRIPipelineUi, PrintInfo, _pinfo, ACRX_CMD_MODAL, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CDRIPipeLineUiApp, DRIPipelineUi, InsertSize, _is, ACRX_CMD_MODAL, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CDRIPipeLineUiApp, DRIPipelineUi, ChangeSize, _cs, ACRX_CMD_MODAL, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CDRIPipeLineUiApp, DRIPipelineUi, ConvertPipes, _convp, ACRX_CMD_MODAL, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CDRIPipeLineUiApp, DRIPipelineUi, TestLabel, _testlabel, ACRX_CMD_MODAL, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CDRIPipeLineUiApp, DRIPipelineUi, TestRotation, _testrot, ACRX_CMD_MODAL, NULL)

