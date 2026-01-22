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

//-----------------------------------------------------------------------------
#define szRDS _RXST("DRI")

//-----------------------------------------------------------------------------
//----- ObjectARX EntryPoint
class CLoaderUnloaderApp : public AcRxArxApp {

public:
	CLoaderUnloaderApp () : AcRxArxApp () {}

	virtual AcRx::AppRetCode On_kInitAppMsg (void *pkt) {
		// TODO: Load dependencies here

		// You *must* call On_kInitAppMsg here
		AcRx::AppRetCode retCode =AcRxArxApp::On_kInitAppMsg (pkt) ;
		
		// TODO: Add your initialization code here

		return (retCode) ;
	}

	virtual AcRx::AppRetCode On_kUnloadAppMsg (void *pkt) {
		// TODO: Add your code here

		// You *must* call On_kUnloadAppMsg here
		AcRx::AppRetCode retCode =AcRxArxApp::On_kUnloadAppMsg (pkt) ;

		// TODO: Unload dependencies here

		return (retCode) ;
	}

	virtual void RegisterServerComponents () {
	}
	
	static void DRILoaderUnloaderLLoad()
	{
		//acrxLoadModule(_T("X:\\GitHub\\ShtirlitsDva\\DRIOARX\\x64\\Release\\DRIPipeLineObj.dbx"), true);
		//acrxLoadModule(_T("X:\\GitHub\\ShtirlitsDva\\DRIOARX\\x64\\Release\\DRIDRIText.dbx"), true);
		//acrxLoadModule(_T("X:\\GitHub\\ShtirlitsDva\\DRIOARX\\x64\\Release\\DRIDRIMText.dbx"), true);
		//acedArxLoad(_T("X:\\GitHub\\ShtirlitsDva\\DRIOARX\\x64\\Release\\DRIDRIPipeLineUi.arx"));
		//acedArxLoad(_T("X:\\GitHub\\ShtirlitsDva\\DRIOARX\\x64\\Release\\DRIDRICommands.arx"));
		acedArxLoad(_T("X:\\GitHub\\ShtirlitsDva\\DRIOARX\\x64\\Release\\NSCommands.arx"));
		//acrxLoadModule(_T("DRIDriPipelineUiProject.arx")); */
		/*acedArxLoad(_T("DRIPipelineProject.dbx"));
		acedArxLoad(_T("DRIDriPipelineUiProject.arx"));*/
	}

	static void DRILoaderUnloaderLLoadBg()
	{
		//acrxLoadModule(_T("X:\\GitHub\\ShtirlitsDva\\DRIOARX\\x64\\Debug\\DRIPipeLineObj.dbx"), true);
		//acrxLoadModule(_T("X:\\GitHub\\ShtirlitsDva\\DRIOARX\\x64\\Debug\\DRIDRIText.dbx"), true);
		//acrxLoadModule(_T("X:\\GitHub\\ShtirlitsDva\\DRIOARX\\x64\\Debug\\DRIDRIMText.dbx"), true);
		acrxLoadModule(_T("X:\\GitHub\\ShtirlitsDva\\DRIOARX\\x64\\Debug\\NSNorsynObjects.dbx"), true);
		//acedArxLoad(_T("X:\\26 ObjectArx\\ObjectArx2022Training\\Step01\\x64\\Debug\\DRIPipelineProject.dbx"));
		//acedArxLoad(_T("X:\\GitHub\\ShtirlitsDva\\DRIOARX\\x64\\Debug\\DRIDRIPipeLineUi.arx"));
		acedArxLoad(_T("X:\\GitHub\\ShtirlitsDva\\DRIOARX\\x64\\Debug\\NSCommands.arx"));
	}

	static void DRILoaderUnloaderULoad()
	{
		acedArxUnload(_T("NSCommands.arx"));
		//acrxUnloadModule(_T("DRIPipeLineObj.dbx"));
		acrxUnloadModule(_T("NSNorsynObjects.dbx"));
		//acrxUnloadModule(_T("DRIDRIText.dbx"));
		//acrxUnloadModule(_T("DRIDRIMText.dbx"));
		//acedArxUnload(_T("DRIPipelineProject.dbx"));
		//acedArxUnload(_T("DRIDRIPipeLineUi.arx"));		
	}
} ;

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CLoaderUnloaderApp)

//ACED_ARXCOMMAND_ENTRY_AUTO(CLoaderUnloaderApp, DRIMyGroup, MyCommand, MyCommandLocal, ACRX_CMD_MODAL, NULL)
//ACED_ARXCOMMAND_ENTRY_AUTO(CLoaderUnloaderApp, DRIMyGroup, MyPickFirst, MyPickFirstLocal, ACRX_CMD_MODAL | ACRX_CMD_USEPICKSET, NULL)
//ACED_ARXCOMMAND_ENTRY_AUTO(CLoaderUnloaderApp, DRIMyGroup, MySessionCmd, MySessionCmdLocal, ACRX_CMD_MODAL | ACRX_CMD_SESSION, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CLoaderUnloaderApp, DRILoaderUnloader, LLoad, _LLoad, ACRX_CMD_MODAL, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CLoaderUnloaderApp, DRILoaderUnloader, LLoadBg, _LLoadBg, ACRX_CMD_MODAL, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CLoaderUnloaderApp, DRILoaderUnloader, ULoad, _ULoad, ACRX_CMD_MODAL, NULL)
//ACED_ADSSYMBOL_ENTRY_AUTO(CLoaderUnloaderApp, MyLispFunction, false)

