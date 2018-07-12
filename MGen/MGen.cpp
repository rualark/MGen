// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface 
// (the "Fluent UI") and is provided only as referential material to supplement the 
// Microsoft Foundation Classes Reference and related electronic documentation 
// included with the MFC C++ library software.  
// License terms to copy, use or distribute the Fluent UI are available separately.  
// To learn more about our Fluent UI licensing program, please visit 
// http://go.microsoft.com/fwlink/?LinkId=238214.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// MGen.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h" 
#include "MGen.h"
#include "MainFrm.h"

#include "MGenDoc.h"
#include "MGenView.h"
#include "GLibrary/GLib.h"
#include "git-version.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMGenApp

BEGIN_MESSAGE_MAP(CMGenApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CMGenApp::OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	//ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
	ON_COMMAND(ID_FILE_OPEN, &CMGenApp::OnFileOpen)
END_MESSAGE_MAP()


// CMGenApp construction

CMGenApp::CMGenApp()
{
	// Load current version
	CGLib::APP_VERSION = GEN_VER_VERSION_STRING;
	m_bHiColorIcons = TRUE;

	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// If the application is built using Common Language Runtime support (/clr):
	//     1) This additional setting is needed for Restart Manager support to work properly.
	//     2) In your project, you must add a reference to System.Windows.Forms in order to build.
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// Recommended format for string is CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("MGen.MGen.2"));

	// Add construction code here,
	// Place all significant initialization in InitInstance
}

// The one and only CMGenApp object

CMGenApp theApp;


// CMGenApp initialization

BOOL CMGenApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();


	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// AfxInitRichEdit2() is required to use RichEdit control	
	// AfxInitRichEdit2();

	// Initialize GDI+
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// Name of your company or organization
	SetRegistryKey(_T("MGen"));
	LoadStdProfileSettings(10);  // Load standard INI file options (including MRU)

	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CMGenDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CMGenView));
	//if (!pDocTemplate)
		//return FALSE;
	AddDocTemplate(pDocTemplate);


	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	//ParseCommandLine(cmdInfo);

	// Enable DDE Execute open
	EnableShellOpen();
	//RegisterShellFileTypes(TRUE);


	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand
	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();
	return TRUE;
}

int CMGenApp::ExitInstance()
{
	//Handle additional resources you may have added
	Gdiplus::GdiplusShutdown(m_gdiplusToken);
	AfxOleTerm(FALSE);

	CWinAppEx::ExitInstance();
	// Log successful exit
	if (CGLib::m_testing) CGLib::AppendLineToFile("autotest\\exit.log", m_lpCmdLine);
	// Return error code for testing purposes
	return CGLib::exitcode;
}

// CMGenApp message handlers


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	CString m_ver;
	virtual BOOL OnInitDialog();
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
, m_ver(_T(""))
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_VER, m_ver);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// App command to run the dialog
void CMGenApp::OnAppAbout()
{
	CAboutDlg aboutDlg; 
	// APP_VERSION
	aboutDlg.m_ver.Format("Music Generators Laboratory, Version %s (compiled on %s)", 
		CGLib::APP_VERSION, COMPILED);
	aboutDlg.DoModal();
}

// CMGenApp customization load/save methods

void CMGenApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
}

void CMGenApp::LoadCustomState()
{
}

void CMGenApp::SaveCustomState()
{
}

// CMGenApp message handlers

void CMGenApp::OnFileOpen()
{
	CMainFrame* mf = (CMainFrame *)AfxGetMainWnd();
	if (mf->m_state_gen == 1) {
		AfxMessageBox("Please stop generation before opening files");
		return;
	}
	TCHAR buffer[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, buffer);
	CString path_old = string(buffer).c_str();
	// szFilters is a text string that includes two file name filters:
	// "*.my" for "MyType Files" and "*.*' for "All Files."
	TCHAR szFilters[] = _T("All supported formats (*.mgr;*.mid;*.midi;*.xml;*.mxl)|*.mgr;*.mid;*.midi;*.xml;*.mxl|MGen result files (*.mgr)|*.mgr|MIDI files (*.mid)|*.mid|MIDI files (*.midi)|*.midi|MusicXML files (*.xml)|*.xml|Compressed MusicXML files (*.mxl)|*.mxl||");

	// Create an Open dialog; the default file name extension is ".my".
	CFileDialog fileDlg(TRUE, "", "", // path_old
		OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR, szFilters, mf, 0, false);
	fileDlg.m_ofn.lpstrInitialDir = path_old;

	// Display the file dialog. When user clicks OK, fileDlg.DoModal() 
	// returns IDOK.
	if (fileDlg.DoModal() == IDOK) {
		mf->LoadFile(fileDlg.GetPathName());
		AddToRecentFileList(fileDlg.GetPathName());
	}
}

CString CMGenApp::getRecentFile(int index)
{
	CString st = string((*m_pRecentFileList)[index]).c_str();
	return st;
}

BOOL CAboutDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
								// EXCEPTION: OCX Property Pages should return FALSE
}
