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
#include <memory>
#include "../NorsynObjects/Entites/NSText/NSText.h"
#include "../NorsynObjects/Entites/NSMText/NSMText.h"
#include "../DRIMText/DRIMTextObj.h"
#include "../DRIText/DRITextObject.h"

//-----------------------------------------------------------------------------
#define szRDS _RXST("NS")

//-----------------------------------------------------------------------------
//----- ObjectARX EntryPoint
class CCommandsApp : public AcRxArxApp {

public:
	CCommandsApp() : AcRxArxApp() {}

	virtual AcRx::AppRetCode On_kInitAppMsg(void* pkt) {
		// TODO: Load dependencies here

		// You *must* call On_kInitAppMsg here
		AcRx::AppRetCode retCode = AcRxArxApp::On_kInitAppMsg(pkt);

		// TODO: Add your initialization code here
		acutPrintf(_T("\nRTWNST -> Replace AcDbText with NSText."));
		acutPrintf(_T("\nRTWNDT -> Replace DRIText with NSText."));
		acutPrintf(_T("\nRTWNSMT -> Replace AcDbMText with NSMText."));
		acutPrintf(_T("\nRTWNDMT -> Replace DRIMTextObj with NSMText."));
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

	static void CommandsReplaceTextWithNSText() {
		AcDbObjectIdArray ids{};
		AcDbBlockTablePointer pBlockTable(acdbCurDwg());
		AcDbObjectId modelSpaceId;
		pBlockTable->getAt(ACDB_MODEL_SPACE, modelSpaceId);
		AcDbBlockTableRecordPointer modelSpace(modelSpaceId, AcDb::kForRead, true);

		Acad::ErrorStatus es;
		AcDbBlockTableRecordIterator* rawIter = nullptr;
		if ((es = modelSpace->newIterator(rawIter)) != Acad::eOk)
		{
			acutPrintf(_T("\nCouldn't create Model Space iterator."));
			return;
		}
		std::unique_ptr<AcDbBlockTableRecordIterator> pBtrIter(rawIter);

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

			AcDbObjectPointer<NSText> newText;
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

	static void CommandsReplaceDRITextWithNSText() {
		AcDbObjectIdArray ids{};
		AcDbBlockTablePointer pBlockTable(acdbCurDwg());
		AcDbObjectId modelSpaceId;
		pBlockTable->getAt(ACDB_MODEL_SPACE, modelSpaceId);
		AcDbBlockTableRecordPointer modelSpace(modelSpaceId, AcDb::kForRead, true);

		Acad::ErrorStatus es;
		AcDbBlockTableRecordIterator* rawIter = nullptr;
		if ((es = modelSpace->newIterator(rawIter)) != Acad::eOk)
		{
			acutPrintf(_T("\nCouldn't create Model Space iterator."));
			return;
		}
		std::unique_ptr<AcDbBlockTableRecordIterator> pBtrIter(rawIter);

		AcDbObjectId id;
		for (pBtrIter->start(); !pBtrIter->done(); pBtrIter->step())
		{
			if ((es = pBtrIter->getEntityId(id)) != Acad::eOk) continue;
			if (id.objectClass() == DRIText::desc()) ids.append(id);
		}

		acutPrintf(_T("\nThere are %d DRIText objects in id array!"), ids.length());

		modelSpace->upgradeOpen(); //Upgrade for write

		for (auto curId = ids.begin(); curId != ids.end(); ++curId)
		{
			AcDbObjectPointer<DRIText> originalText;
			originalText.open(*curId, AcDb::kForWrite);

			AcDbObjectPointer<NSText> newText;
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

	static void CommandsReplaceMTextWithNSMText() {
		AcDbObjectIdArray ids{};
		AcDbBlockTablePointer pBlockTable(acdbCurDwg());
		AcDbObjectId modelSpaceId;
		pBlockTable->getAt(ACDB_MODEL_SPACE, modelSpaceId);
		AcDbBlockTableRecordPointer modelSpace(modelSpaceId, AcDb::kForRead, true);

		Acad::ErrorStatus es;
		AcDbBlockTableRecordIterator* rawIter = nullptr;
		if ((es = modelSpace->newIterator(rawIter)) != Acad::eOk)
		{
			acutPrintf(_T("\nCouldn't create Model Space iterator."));
			return;
		}
		std::unique_ptr<AcDbBlockTableRecordIterator> pBtrIter(rawIter);

		AcDbObjectId id;
		for (pBtrIter->start(); !pBtrIter->done(); pBtrIter->step())
		{
			if ((es = pBtrIter->getEntityId(id)) != Acad::eOk) continue;
			if (id.objectClass() == AcDbMText::desc()) ids.append(id);
		}
		acutPrintf(_T("\nThere are %d MText objects in id array!"), ids.length());

		modelSpace->upgradeOpen();

		for (auto curId = ids.begin(); curId != ids.end(); ++curId)
		{
			AcDbObjectPointer<AcDbMText> originalMText;
			originalMText.open(*curId, AcDb::kForWrite);

			AcDbObjectPointer<NSMText> newMText;
			newMText.create();

			newMText->setLocation(originalMText->location());
			newMText->setRotation(originalMText->rotation());
			newMText->setTextHeight(originalMText->textHeight());
			newMText->setWidth(originalMText->width());
			newMText->setTextStyle(originalMText->textStyle());
			newMText->setAttachment(originalMText->attachment());
			newMText->setFlowDirection(originalMText->flowDirection());
			newMText->setContents(originalMText->contents());
			newMText->setLayer(originalMText->layer());

			if (modelSpace->appendAcDbEntity(newMText) == Acad::eOk)
			{
				originalMText->upgradeOpen();
				originalMText->erase(true);
			}
		}
	}

	static void CommandsReplaceDRIMTextWithNSMText() {
		AcDbObjectIdArray ids{};
		AcDbBlockTablePointer pBlockTable(acdbCurDwg());
		AcDbObjectId modelSpaceId;
		pBlockTable->getAt(ACDB_MODEL_SPACE, modelSpaceId);
		AcDbBlockTableRecordPointer modelSpace(modelSpaceId, AcDb::kForRead, true);

		Acad::ErrorStatus es;
		AcDbBlockTableRecordIterator* rawIter = nullptr;
		if ((es = modelSpace->newIterator(rawIter)) != Acad::eOk)
		{
			acutPrintf(_T("\nCouldn't create Model Space iterator."));
			return;
		}
		std::unique_ptr<AcDbBlockTableRecordIterator> pBtrIter(rawIter);

		AcDbObjectId id;
		for (pBtrIter->start(); !pBtrIter->done(); pBtrIter->step())
		{
			if ((es = pBtrIter->getEntityId(id)) != Acad::eOk) continue;
			if (id.objectClass() == DRIMTextObj::desc()) ids.append(id);
		}
		acutPrintf(_T("\nThere are %d DRIMText objects in id array!"), ids.length());

		modelSpace->upgradeOpen();

		for (auto curId = ids.begin(); curId != ids.end(); ++curId)
		{
			AcDbObjectPointer<DRIMTextObj> originalMText;
			originalMText.open(*curId, AcDb::kForWrite);

			AcDbObjectPointer<NSMText> newMText;
			newMText.create();

			newMText->setLocation(originalMText->location());
			newMText->setRotation(originalMText->rotation());
			newMText->setTextHeight(originalMText->textHeight());
			newMText->setWidth(originalMText->width());
			newMText->setTextStyle(originalMText->textStyle());
			newMText->setAttachment(originalMText->attachment());
			newMText->setFlowDirection(originalMText->flowDirection());
			newMText->setContents(originalMText->contents());
			newMText->setLayer(originalMText->layer());

			if (modelSpace->appendAcDbEntity(newMText) == Acad::eOk)
			{
				originalMText->upgradeOpen();
				originalMText->erase(true);
			}
		}
	}
};

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CCommandsApp)

ACED_ARXCOMMAND_ENTRY_AUTO(CCommandsApp, Commands, ReplaceTextWithNSText, RTWNST, ACRX_CMD_MODAL, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CCommandsApp, Commands, ReplaceDRITextWithNSText, RTWNDT, ACRX_CMD_MODAL, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CCommandsApp, Commands, ReplaceMTextWithNSMText, RTWNSMT, ACRX_CMD_MODAL, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CCommandsApp, Commands, ReplaceDRIMTextWithNSMText, RTWNDMT, ACRX_CMD_MODAL, NULL)
