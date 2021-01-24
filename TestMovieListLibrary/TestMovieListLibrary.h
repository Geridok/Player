
// TestMovieListLibrary.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CTestMovieListLibraryApp:
// See TestMovieListLibrary.cpp for the implementation of this class
//

class CTestMovieListLibraryApp : public CWinAppEx
{
public:
	CTestMovieListLibraryApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CTestMovieListLibraryApp theApp;