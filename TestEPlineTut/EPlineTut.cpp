// EPlineTut.cpp
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
//- EPlineTut.cpp : Initialization functions
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "resource.h"
#include <afxdllx.h>

//-----------------------------------------------------------------------------
//- Define the sole extension module object.
AC_IMPLEMENT_EXTENSION_MODULE(EPlineTutDLL)
//- Please do not remove the 3 following lines. These are here to make .NET MFC Wizards
//- running properly. The object will not compile but is require by .NET to recognize
//- this project as being an MFC project
#ifdef NEVER
AFX_EXTENSION_MODULE EPlineTutExtDLL ={ NULL, NULL } ;
#endif

//- Now you can use the CAcModuleResourceOverride class in
//- your application to switch to the correct resource instance.
//- Please see the ObjectARX Documentation for more details

//-----------------------------------------------------------------------------
//- DLL Entry Point
extern "C"
BOOL WINAPI DllMain (HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved) {
	//- Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved) ;

	if ( dwReason == DLL_PROCESS_ATTACH ) {
        _hdllInstance =hInstance ;
		EPlineTutDLL.AttachInstance (hInstance) ;
		InitAcUiDLL () ;
	} else if ( dwReason == DLL_PROCESS_DETACH ) {
		EPlineTutDLL.DetachInstance () ;
	}
	return (TRUE) ;
}

