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
#include "../DRIMText/DRIMTextObj.h"
#include "../DRIText/DRITextObject.h"

//-----------------------------------------------------------------------------
#define szRDS _RXST("DRI")

//-----------------------------------------------------------------------------
//----- ObjectARX EntryPoint
class CDRICommandsApp : public AcRxArxApp {

public:
	CDRICommandsApp() : AcRxArxApp() {}

	virtual AcRx::AppRetCode On_kInitAppMsg(void* pkt) {
		// TODO: Load dependencies here

		// You *must* call On_kInitAppMsg here
		AcRx::AppRetCode retCode = AcRxArxApp::On_kInitAppMsg(pkt);

		// TODO: Add your initialization code here

		return (retCode);
	}
	virtual AcRx::AppRetCode On_kUnloadAppMsg(void* pkt) {
		// TODO: Add your code here

		// You *must* call On_kUnloadAppMsg here
		AcRx::AppRetCode retCode = AcRxArxApp::On_kUnloadAppMsg(pkt);

		// TODO: Unload dependencies here

		return (retCode);
	}
	virtual void RegisterServerComponents() {
	}

	static void DRICommandsReplaceTextWithDRIText() {
		AcDbObjectIdArray ids{};
		AcDbBlockTablePointer pBlockTable(acdbCurDwg());
		AcDbObjectId modelSpaceId;
		pBlockTable->getAt(ACDB_MODEL_SPACE, modelSpaceId);
		AcDbBlockTableRecordPointer modelSpace(modelSpaceId, AcDb::kForRead, true);

		Acad::ErrorStatus es;
		AcDbBlockTableRecordIterator* pBtrIter;
		if ((es = modelSpace->newIterator(pBtrIter)) != Acad::eOk)
		{
			acutPrintf(_T("\nCouldn't create Model Space iterator."));
			return;
		}

		AcDbObjectId id;
		for (pBtrIter->start(); !pBtrIter->done(); pBtrIter->step())
		{
			if ((es = pBtrIter->getEntityId(id)) != Acad::eOk) continue;
			if (id.objectClass() == AcDbText::desc()) ids.append(id);
		}

		acutPrintf(_T("\nThere are %d objects in id array!"), ids.length());

		modelSpace->upgradeOpen(); //Upgrade for write

		for (auto curId = ids.begin(); curId != ids.end(); ++curId)
		{
			AcDbObjectPointer<AcDbText> originalText;
			originalText.open(*curId, AcDb::kForWrite);

			AcDbObjectPointer<DRIText> newText;
			newText.create();

			newText->setAlignmentPoint(originalText->alignmentPoint());
			newText->setPosition(originalText->position());
			newText->setLayer(originalText->layer());
			newText->setTextString(originalText->textString());
			newText->setRotation(originalText->rotation());
			newText->setHeight(originalText->height());
			newText->setHorizontalMode(originalText->horizontalMode());
			newText->setVerticalMode(originalText->verticalMode());

			if (modelSpace->appendAcDbEntity(newText) == Acad::eOk)
			{
				originalText->upgradeOpen();
				originalText->erase(true);
			}
		}
	}
	static void DRICommandsReplaceMTextWithDRIMText() {
		AcDbObjectIdArray ids{};
		AcDbBlockTablePointer pBlockTable(acdbCurDwg());
		AcDbObjectId modelSpaceId;
		pBlockTable->getAt(ACDB_MODEL_SPACE, modelSpaceId);
		AcDbBlockTableRecordPointer modelSpace(modelSpaceId, AcDb::kForRead, true);

		Acad::ErrorStatus es;
		AcDbBlockTableRecordIterator* pBtrIter;
		if ((es = modelSpace->newIterator(pBtrIter)) != Acad::eOk)
		{
			acutPrintf(_T("\nCouldn't create Model Space iterator."));
			return;
		}

		AcDbObjectId id;
		for (pBtrIter->start(); !pBtrIter->done(); pBtrIter->step())
		{
			if ((es = pBtrIter->getEntityId(id)) != Acad::eOk) continue;
			AcDbObjectPointer<AcDbMText> pEnt(id, AcDb::kForRead);
			if (pEnt.openStatus() != Acad::eOk) continue;
			if (!pEnt->isKindOf(AcDbMText::desc())) continue;

			// Get layer name
			AcString layerName;
			pEnt->layer(layerName);

			// Check layer name
			if (layerName == _T("FJV-DIM")) {
				ids.append(id);
			}
		}

		acutPrintf(_T("\nThere are %d objects in id array!"), ids.length());

		modelSpace->upgradeOpen(); //Upgrade for write

		for (auto curId = ids.begin(); curId != ids.end(); ++curId)
		{
			AcDbObjectPointer<AcDbMText> originalText;
			originalText.open(*curId, AcDb::kForWrite);

			AcDbObjectPointer<DRIMTextObj> newText;
			newText.create();

			newText->setLocation(originalText->location());
			//newText->setAlignmentPoint(originalText->alignmentPoint());
			//newText->setPosition(originalText->position());
			newText->setLayer(originalText->layer());
			//newText->setTextString(originalText->textString());
			newText->setContents(originalText->contents());
			newText->setRotation(originalText->rotation());
			newText->setTextHeight(originalText->textHeight());
			//newText->setHorizontalMode(originalText->horizontalMode());
			//newText->setVerticalMode(originalText->verticalMode());
			newText->setAttachment(AcDbMText::AttachmentPoint(5));
			newText->setBackgroundFill(true);
			newText->setUseBackgroundColor(true);
			newText->setBackgroundScaleFactor(1.0);

			if (modelSpace->appendAcDbEntity(newText) == Acad::eOk)
			{
				originalText->upgradeOpen();
				originalText->erase(true);
			}
		}
	}

};

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CDRICommandsApp)

ACED_ARXCOMMAND_ENTRY_AUTO(CDRICommandsApp, DRICommands, ReplaceTextWithDRIText, RTWDT, ACRX_CMD_MODAL, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CDRICommandsApp, DRICommands, ReplaceMTextWithDRIMText, RMTWDMT, ACRX_CMD_MODAL, NULL)

