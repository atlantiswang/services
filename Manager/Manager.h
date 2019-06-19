
// Manager.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CManagerApp:
// See Manager.cpp for the implementation of this class
//

class CManagerApp : public CWinApp
{
public:
	CManagerApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
private:
    bool EnableDebugPrivilege();
public:
    virtual int ExitInstance();
private:
    HMODULE m_hModule;
};

extern CManagerApp theApp;