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
#include <string>
#include <ShObjIdl.h>
#include "../NorsynObjects/Entites/NSText/NSText.h"
#include "../NorsynObjects/Entites/NSMText/NSMText.h"
#include "../DRIMText/DRIMTextObj.h"
#include "../DRIText/DRITextObject.h"

//-----------------------------------------------------------------------------
#define szRDS _RXST("NS")

// Folder picker using modern IFileOpenDialog (looks like a file dialog, not the old SHBrowseForFolder)
static bool PickFolderDialog(std::wstring& outPath)
{
	IFileOpenDialog* pDlg = nullptr;
	HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
		IID_IFileOpenDialog, reinterpret_cast<void**>(&pDlg));
	if (FAILED(hr)) return false;

	DWORD dwOptions = 0;
	pDlg->GetOptions(&dwOptions);
	pDlg->SetOptions(dwOptions | FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM);
	pDlg->SetTitle(L"Select folder containing *-rot.dwg files");

	hr = pDlg->Show(adsw_acadMainWnd());
	if (FAILED(hr)) { pDlg->Release(); return false; }

	IShellItem* pItem = nullptr;
	hr = pDlg->GetResult(&pItem);
	if (SUCCEEDED(hr))
	{
		PWSTR pszPath = nullptr;
		hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszPath);
		if (SUCCEEDED(hr))
		{
			outPath = pszPath;
			CoTaskMemFree(pszPath);
		}
		pItem->Release();
	}
	pDlg->Release();
	return SUCCEEDED(hr);
}

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
		acutPrintf(_T("\nBRTWNDT -> Batch convert DRIText to NSText in *-rot.dwg files from a folder."));
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

	static void CommandsBatchReplaceDRITextWithNSText() {
		std::wstring folderPath;
		if (!PickFolderDialog(folderPath))
		{
			acutPrintf(_T("\nFolder selection cancelled."));
			return;
		}

		std::wstring searchPattern = folderPath + L"\\*-rot.dwg";
		WIN32_FIND_DATA fd;
		HANDLE hFind = FindFirstFile(searchPattern.c_str(), &fd);
		if (hFind == INVALID_HANDLE_VALUE)
		{
			acutPrintf(_T("\nNo *-rot.dwg files found in: %s"), folderPath.c_str());
			return;
		}

		int totalFiles = 0, totalConverted = 0;

		do
		{
			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;

			std::wstring fileName = fd.cFileName;
			std::wstring fullPath = folderPath + L"\\" + fileName;

			// Create backup: something-rot.dwg -> something-rot.bak.dwg
			std::wstring backupPath = fullPath;
			size_t dotPos = backupPath.rfind(L".dwg");
			if (dotPos != std::wstring::npos)
				backupPath.insert(dotPos, L".bak");

			if (!CopyFile(fullPath.c_str(), backupPath.c_str(), FALSE))
			{
				acutPrintf(_T("\nFailed to create backup for: %s"), fileName.c_str());
				continue;
			}
			acutPrintf(_T("\nBackup created: %s"), backupPath.c_str());

			// Side-load the drawing as a separate database
			AcDbDatabase* pDb = new AcDbDatabase(false, true);
			if (pDb->readDwgFile(fullPath.c_str()) != Acad::eOk)
			{
				acutPrintf(_T("\nFailed to read: %s"), fileName.c_str());
				delete pDb;
				continue;
			}

			// Get model space from the side-loaded database
			AcDbBlockTable* pBlockTable = nullptr;
			if (pDb->getBlockTable(pBlockTable, AcDb::kForRead) != Acad::eOk)
			{
				acutPrintf(_T("\nFailed to get block table: %s"), fileName.c_str());
				delete pDb;
				continue;
			}

			AcDbObjectId modelSpaceId;
			pBlockTable->getAt(ACDB_MODEL_SPACE, modelSpaceId);
			pBlockTable->close();

			AcDbBlockTableRecord* pModelSpace = nullptr;
			if (acdbOpenObject(pModelSpace, modelSpaceId, AcDb::kForRead) != Acad::eOk)
			{
				acutPrintf(_T("\nFailed to open model space: %s"), fileName.c_str());
				delete pDb;
				continue;
			}

			// Collect DRIText object IDs
			AcDbObjectIdArray ids;
			AcDbBlockTableRecordIterator* pIter = nullptr;
			if (pModelSpace->newIterator(pIter) == Acad::eOk)
			{
				for (pIter->start(); !pIter->done(); pIter->step())
				{
					AcDbObjectId id;
					if (pIter->getEntityId(id) == Acad::eOk)
					{
						if (id.objectClass() == DRIText::desc())
							ids.append(id);
					}
				}
				delete pIter;
			}

			acutPrintf(_T("\n%s: %d DRIText objects found."), fileName.c_str(), ids.length());

			if (ids.length() > 0)
			{
				pModelSpace->upgradeOpen();

				for (auto curId = ids.begin(); curId != ids.end(); ++curId)
				{
					AcDbObjectPointer<DRIText> originalText;
					originalText.open(*curId, AcDb::kForWrite);
					if (originalText.openStatus() != Acad::eOk) continue;

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

					if (pModelSpace->appendAcDbEntity(newText) == Acad::eOk)
					{
						originalText->erase(true);
						totalConverted++;
					}
				}
			}

			pModelSpace->close();

			if (pDb->saveAs(fullPath.c_str()) != Acad::eOk)
				acutPrintf(_T("\nFailed to save: %s"), fileName.c_str());
			else
				acutPrintf(_T("\nSaved: %s"), fileName.c_str());

			delete pDb;
			totalFiles++;

		} while (FindNextFile(hFind, &fd));

		FindClose(hFind);

		acutPrintf(_T("\nBatch complete. %d files processed, %d DRIText objects converted."),
			totalFiles, totalConverted);
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
ACED_ARXCOMMAND_ENTRY_AUTO(CCommandsApp, Commands, BatchReplaceDRITextWithNSText, BRTWNDT, ACRX_CMD_MODAL, NULL)
