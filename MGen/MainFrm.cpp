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

// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "MGen.h"
#include "MainFrm.h"
#include "EditParamsDlg.h"
#include "AlgoDlg.h"
#include "MGenView.h"
#include "MFIDialog.h"
#include "MyVisualManagerOffice2007.h"
#include "MsgDlg.h"
#include "GLibrary\CsvDb.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

static UINT WM_GEN_FINISH = RegisterWindowMessage("MGEN_GEN_FINISH_MSG");
static UINT WM_DEBUG_MSG = RegisterWindowMessage("MGEN_DEBUG_MSG");
static UINT WM_STATUS_MSG = RegisterWindowMessage("MGEN_STATUS_MSG");

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
	ON_COMMAND(ID_FILE_PRINT, &CMainFrame::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CMainFrame::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CMainFrame::OnFilePrintPreview)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_PREVIEW, &CMainFrame::OnUpdateFilePrintPreview)
	ON_WM_SETTINGCHANGE()
	ON_COMMAND(ID_BUTTON_OUTPUTWND, &CMainFrame::ShowOutputWnd)
	ON_COMMAND(ID_BUTTON_PARAMS, &CMainFrame::OnButtonParams)
	ON_COMMAND(ID_BUTTON_GEN, &CMainFrame::OnButtonGen)
	ON_COMMAND(ID_BUTTON_PLAY, &CMainFrame::OnButtonPlay)
	ON_COMMAND(ID_BUTTON_SETTINGS, &CMainFrame::OnButtonSettings)
	ON_COMMAND(ID_BUTTON_ALGO, &CMainFrame::OnButtonAlgo)
	ON_COMMAND(ID_CHECK_OUTPUTWND, &CMainFrame::OnCheckOutputwnd)
	ON_UPDATE_COMMAND_UI(ID_CHECK_OUTPUTWND, &CMainFrame::OnUpdateCheckOutputwnd)
	ON_UPDATE_COMMAND_UI(ID_COMBO_ALGO, &CMainFrame::OnUpdateComboAlgo)
	ON_COMMAND(ID_COMBO_ALGO, &CMainFrame::OnComboAlgo)
	ON_REGISTERED_MESSAGE(WM_GEN_FINISH, &CMainFrame::OnGenFinish)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_GEN, &CMainFrame::OnUpdateButtonGen)
	ON_WM_CLOSE()
	ON_UPDATE_COMMAND_UI(ID_COMBO_MIDIOUT, &CMainFrame::OnUpdateComboMidiout)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_EPARAMS, &CMainFrame::OnUpdateButtonEparams)
	ON_COMMAND(ID_BUTTON_EPARAMS, &CMainFrame::OnButtonEparams)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_PLAY, &CMainFrame::OnUpdateButtonPlay)
	ON_COMMAND(ID_COMBO_MIDIOUT, &CMainFrame::OnComboMidiout)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_ALGO, &CMainFrame::OnUpdateButtonAlgo)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_PARAMS, &CMainFrame::OnUpdateButtonParams)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_SCONFIG, &CMainFrame::OnUpdateButtonSconfig)
	ON_COMMAND(ID_BUTTON_SCONFIG, &CMainFrame::OnButtonSconfig)
	ON_UPDATE_COMMAND_UI(ID_SPIN_PSPEED, &CMainFrame::OnUpdateSpinPspeed)
	ON_COMMAND(ID_SPIN_PSPEED, &CMainFrame::OnSpinPspeed)
	ON_COMMAND(ID_SPIN_ZOOM, &CMainFrame::OnSpinZoom)
	ON_WM_KEYDOWN()
	ON_UPDATE_COMMAND_UI(ID_BUTTON_OPENMIDI, &CMainFrame::OnUpdateButtonOpenmidi)
	ON_COMMAND(ID_BUTTON_OPENMIDI, &CMainFrame::OnButtonOpenmidi)
	ON_COMMAND(ID_BUTTON_EDITALGO, &CMainFrame::OnButtonEditalgo)
	ON_COMMAND(ID_BUTTON_EDITINST, &CMainFrame::OnButtonEditinst)
	ON_COMMAND(ID_BUTTON_RELOADALGO, &CMainFrame::OnButtonReloadalgo)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_RELOADALGO, &CMainFrame::OnUpdateButtonReloadalgo)
	ON_COMMAND(ID_BUTTON_RSETTINGS, &CMainFrame::OnButtonRsettings)
	ON_COMMAND(ID_BUTTON_SETTINGS_EDIT, &CMainFrame::OnButtonSettingsEdit)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_ECSV, &CMainFrame::OnUpdateButtonEcsv)
	ON_COMMAND(ID_BUTTON_ECSV, &CMainFrame::OnButtonEcsv)
	ON_COMMAND(ID_BUTTON_OPENCSV, &CMainFrame::OnButtonOpencsv)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_OPENCSV, &CMainFrame::OnUpdateButtonOpencsv)
	ON_COMMAND(ID_BUTTON_RELOAD_SETTINGS, &CMainFrame::OnButtonReloadSettings)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_RELOAD_SETTINGS, &CMainFrame::OnUpdateButtonReloadSettings)
	ON_COMMAND(ID_CHECK_TEMPO, &CMainFrame::OnCheckTempo)
	ON_UPDATE_COMMAND_UI(ID_CHECK_TEMPO, &CMainFrame::OnUpdateCheckTempo)
	ON_COMMAND(ID_CHECK_LINES, &CMainFrame::OnCheckLines)
	ON_UPDATE_COMMAND_UI(ID_CHECK_LINES, &CMainFrame::OnUpdateCheckLines)
	ON_COMMAND(ID_CHECK_MARKS, &CMainFrame::OnCheckMarks)
	ON_UPDATE_COMMAND_UI(ID_CHECK_MARKS, &CMainFrame::OnUpdateCheckMarks)
	ON_COMMAND(ID_CHECK_COMMENTS, &CMainFrame::OnCheckComments)
	ON_UPDATE_COMMAND_UI(ID_CHECK_COMMENTS, &CMainFrame::OnUpdateCheckComments)
	ON_COMMAND(ID_CHECK_LINING, &CMainFrame::OnCheckLining)
	ON_UPDATE_COMMAND_UI(ID_CHECK_LINING, &CMainFrame::OnUpdateCheckLining)
	ON_COMMAND(ID_CHECK_VEL, &CMainFrame::OnCheckVel)
	ON_UPDATE_COMMAND_UI(ID_CHECK_VEL, &CMainFrame::OnUpdateCheckVel)
	ON_COMMAND(ID_CHECK_NCOLORS, &CMainFrame::OnCheckNcolors)
	ON_UPDATE_COMMAND_UI(ID_CHECK_NCOLORS, &CMainFrame::OnUpdateCheckNcolors)
	ON_COMMAND(ID_BUTTON_ERULES, &CMainFrame::OnButtonErules)
	ON_COMMAND(ID_BUTTON_SSTATUS2, &CMainFrame::OnButtonSstatus2)
	ON_COMMAND(ID_CHECK_CURVE, &CMainFrame::OnCheckCurve)
	ON_UPDATE_COMMAND_UI(ID_CHECK_CURVE, &CMainFrame::OnUpdateCheckCurve)
	ON_COMMAND(ID_CHECK_NSR, &CMainFrame::OnCheckNsr)
	ON_UPDATE_COMMAND_UI(ID_CHECK_NSR, &CMainFrame::OnUpdateCheckNsr)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_FOLDER, &CMainFrame::OnUpdateButtonFolder)
	ON_COMMAND(ID_BUTTON_FOLDER, &CMainFrame::OnButtonFolder)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_LY, &CMainFrame::OnUpdateButtonLy)
	ON_COMMAND(ID_BUTTON_LY, &CMainFrame::OnButtonLy)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_PDF, &CMainFrame::OnUpdateButtonPdf)
	ON_COMMAND(ID_BUTTON_PDF, &CMainFrame::OnButtonPdf)
	ON_COMMAND(ID_CHECK_GRAPH1, &CMainFrame::OnCheckGraph1)
	ON_UPDATE_COMMAND_UI(ID_CHECK_GRAPH1, &CMainFrame::OnUpdateCheckGraph1)
	ON_COMMAND(ID_CHECK_GRAPH2, &CMainFrame::OnCheckGraph2)
	ON_UPDATE_COMMAND_UI(ID_CHECK_GRAPH2, &CMainFrame::OnUpdateCheckGraph2)
	ON_COMMAND(ID_CHECK_GRAPH3, &CMainFrame::OnCheckGraph3)
	ON_UPDATE_COMMAND_UI(ID_CHECK_GRAPH3, &CMainFrame::OnUpdateCheckGraph3)
	ON_COMMAND(ID_CHECK_PROGRESS, &CMainFrame::OnCheckProgress)
	ON_UPDATE_COMMAND_UI(ID_CHECK_PROGRESS, &CMainFrame::OnUpdateCheckProgress)
	ON_COMMAND(ID_CHECK_SHIFT, &CMainFrame::OnCheckShift)
	ON_UPDATE_COMMAND_UI(ID_CHECK_SHIFT, &CMainFrame::OnUpdateCheckShift)
	ON_COMMAND(ID_CHECK_DEBUGEXPECT, &CMainFrame::OnCheckDebugexpect)
	ON_UPDATE_COMMAND_UI(ID_CHECK_DEBUGEXPECT, &CMainFrame::OnUpdateCheckDebugexpect)
	ON_COMMAND(ID_BUTTON_EXP_ACCEPT, &CMainFrame::OnButtonExpAccept)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_EXP_ACCEPT, &CMainFrame::OnUpdateButtonExpAccept)
	ON_COMMAND(ID_CHECK_VIB, &CMainFrame::OnCheckVib)
	ON_UPDATE_COMMAND_UI(ID_CHECK_VIB, &CMainFrame::OnUpdateCheckVib)
	ON_COMMAND(ID_CHECK_VIBF, &CMainFrame::OnCheckVibf)
	ON_UPDATE_COMMAND_UI(ID_CHECK_VIBF, &CMainFrame::OnUpdateCheckVibf)
END_MESSAGE_MAP()

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	show_graph.resize(MAX_GRAPH);
	cur_ui_text.resize(MAX_UI_TEXT);
	AlgGroups.resize(MAX_ALGO);
	fill(begin(AlgID), end(AlgID), 0);
	fill(begin(AlgMFI), end(AlgMFI), 0);
	fill(begin(AlgXFI), end(AlgXFI), 0);
	fill(begin(ParamCount), end(ParamCount), 0);
	//HANDLE hThread = GetCurrentThread();
	//SetThreadPriority(hThread, THREAD_PRIORITY_TIME_CRITICAL);

	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_VS_2008);
}

CMainFrame::~CMainFrame()
{
	if (pGen != 0) delete pGen;
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;

	CGLib::log_buffer.resize(LOG_TABS);
	CGLib::log_buffer_size.resize(LOG_TABS);
	CGLib::warn_log_buffer.resize(LOG_TABS);
	CGLib::logs_sent.resize(LOG_TABS);

	CreateDirectory("log", NULL);
	m_wndRibbonBar.Create(this);
	m_wndRibbonBar.LoadFromResource(IDR_RIBBON);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	CString strTitlePane1;
	CString strTitlePane2 = "Release";
	bNameValid = strTitlePane1.LoadString(IDS_STATUS_PANE1);
	ASSERT(bNameValid);
	//bNameValid = strTitlePane2.LoadString(IDS_STATUS_PANE2);
	//ASSERT(bNameValid);

#if defined(_DEBUG)
	strTitlePane2 = "Debug";
#endif

#ifdef _WIN64
	strTitlePane2 += " x64";
#else
	strTitlePane2 += " x86";
#endif
	strTitlePane2 += " ";
	strTitlePane2 += CGLib::APP_VERSION;
	strTitlePane2 += " (compiled on ";
	strTitlePane2 += COMPILED;
	strTitlePane2 += ")";

	CMFCRibbonStatusBarPane* pPane = new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE1, strTitlePane1, TRUE);
	pPane->SetAlmostLargeText(_T("Connecting"));
	m_wndStatusBar.AddElement(pPane, strTitlePane1);
	m_wndStatusBar.AddExtendedElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE2, strTitlePane2, TRUE), strTitlePane2);

	// enable Visual Studio 2005 style docking window behavior
	CDockingManager::SetDockingMode(DT_SMART);
	// enable Visual Studio 2005 style docking window auto-hide behavior
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// create docking windows
	if (!CreateDockingWindows())
	{
		TRACE0("Failed to create docking windows\n");
		return -1;
	}

	m_wndOutput.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndOutput);

	// set the visual manager and style based on persisted value
	OnApplicationLook(theApp.m_nAppLook);

	LoadAlgo();

	// MIDI port
	CMFCRibbonComboBox *pCombo = DYNAMIC_DOWNCAST(CMFCRibbonComboBox,	m_wndRibbonBar.FindByID(ID_COMBO_MIDIOUT));
	CString st;
	MidiCount = 0;
	CString portName;
	pCombo->AddItem(DISABLE_PLAYBACK, -1);
	try {
		RtMidiOut rtmidi;
		unsigned int i = 0, nPorts = rtmidi.getPortCount();
		for (i = 0; i<nPorts; i++) {
			portName = rtmidi.getPortName(i).c_str();
			pCombo->AddItem(portName, i);
			MidiName[MidiCount] = portName;
			MidiCount++;
		}
	}
	catch (RtMidiError &error) {
		WriteLog(5, error.getMessage().c_str());
	}

	st.Format("Started MGen version %s", CGLib::APP_VERSION);
	WriteLog(0, st);
	AfxInitRichEdit2();

	LoadSettings();
	//m_wndRibbonBar.GetQuickAccessToolbar()->SetCompactMode(1);

	CGLib::can_send_log = 1;
	CGLib::m_hWnd = m_hWnd;
	CGLib::WM_DEBUG_MSG = WM_DEBUG_MSG;
	CGLib::WM_STATUS_MSG = WM_STATUS_MSG;

	//CGenRS1 gen;
	//gen.TestSmoothRandom();
	//ShowStatusText(0, "0");
	//ShowStatusText(1, "1");
	//ShowStatusText(2, "2");
	//ShowStatusText(3, "3");
	//ShowStatusText(4, "4");
	//ShowStatusText(5, "5");

	// Start log/status timer
	SetTimer(TIMER3, LOG_TIMER, NULL);

	// Parse command line after message
	SetTimer(TIMER5, 0, NULL);

	//CGLib::TestVSet();
	/*
	CCsvDb cdb;
	cdb.separator = ";";
	vector <CString> hdr;
	hdr.push_back("id");
	hdr.push_back("time");
	hdr.push_back("text");
	cdb.Create("test.csv", hdr);
	map <CString, CString> row;
	row["id"] = "1";
	row["time"].Format("%d", 1234);
	row["text"] = "some text";
	cdb.Insert(row);
	row["id"] = "1";
	row["time"].Format("%d", 2345);
	row["text"] = "other text";
	cdb.Insert(row);
	row["id"] = "2";
	row["time"].Format("%d", 3456);
	row["text"] = "more text";
	cdb.Insert(row);
	row["id"] = "2";
	row["time"].Format("%d", 4567);
	row["text"] = "even more text";
	cdb.Insert(row);

	CCsvDb cdb2;
	cdb2.Open("test.csv");
	cdb2.filter["id"] = "1";
	cdb2.Select();
	cdb2.Delete();
	*/

	/*
	// Test CsvDb performance. 200 queries with 2 conditions take 1 second on table with 1475 rows
	for (int i = 0; i < 200; ++i) {
		CString est;
		int fl;
		CCsvDb cdb;
		map <CString, CString> row;
		est = cdb.Open("db/expect.csv");
		if (est != "") WriteLog(5, est);
		cdb.filter["File"] = "midi\Counterpoint\test\test-sus.mid";
		cdb.filter["Cid"] = "10";
		est = cdb.Select();
		if (est != "") WriteLog(5, est);
	}
	*/

	return 0;
}

void CMainFrame::ParseCommandLine() {
	m_cline = ((CMGenApp*)::AfxGetApp())->m_lpCmdLine;
	CString st = m_cline;
	CString st2;
	int pos;
	// Detect switches
	while (st.GetLength() && st[0] == '-') {
		pos = st.Find(' ');
		st2 = st.Left(pos);
		st = st.Right(st.GetLength() - pos - 1);
		if (st2.Find("-test") == 0) {
			CGLib::m_testing = 1;
			if (st2.GetLength() > 6) CGLib::m_test_sec = atoi(st2.Right(st2.GetLength() - 6));
		}
		if (st2.Find("-job") == 0) {
			CGLib::m_testing = 2;
			if (st2.GetLength() > 5) CGLib::m_test_sec = atoi(st2.Right(st2.GetLength() - 5));
		}
	}
	// Overwrite parameters
	if (CGLib::m_testing) {
		m_pspeed = 100;
		ly_debugexpect = 0;
	}
	st.Replace("\"", "");
	CGLib::m_cline2 = st;
	if (CGLib::m_cline2 != "") LoadFile(CGLib::m_cline2);
	//AfxMessageBox(st);
	if (!CGLib::m_testing) ShowWindow(SW_SHOWMAXIMIZED);
}

void CMainFrame::LoadFile(CString abs_path) {
	if (!CGLib::fileExists(abs_path)) return;
	TCHAR buffer[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, buffer);
	CString path_old = string(buffer).c_str();
	CString ext = CGLib::ext_from_path(abs_path);
	if ((ext == "mid") || (ext == "midi") || ext == "xml" || ext == "mxl") {
		// Convert absolute to relative
		CString path, rel_path;
		path = abs_path;
		if (path.Find(path_old) > -1) {
			rel_path = path.Mid(path_old.GetLength() + 1);
			//mf->WriteLog(1, rel_path);
			// Check relative exists
			if (CGLib::fileExists(rel_path)) path = rel_path;
		}
		LoadMidi(path);
	}
	else if (ext == "pl") {
		int alg = GetAlgoByName(abs_path);
		if (alg == -1) {
			AfxMessageBox("Cannot detect algorithm of configuration file " + abs_path);
			return;
		}
		
		m_config = CGLib::bname_from_path(abs_path);
		m_algo = alg;
		m_algo_id = AlgID[m_algo];
		// Save this algorithm and config if we are not testing
		if (!CGLib::m_testing) SaveSettings();
		OnButtonGen();
		// Stop generation after a while
		if (CGLib::m_testing) {
			SetTimer(TIMER4, CGLib::m_test_sec * 1000, NULL);
		}
	}
	else if (ext == "mgr") {
		LoadResults(abs_path);
	}
	else {
		AfxMessageBox("Cannot open file with this extension: " + abs_path);
	}
}

int CMainFrame::GetAlgoByName(CString st) {
	for (int x = 0; x < AlgCount; x++) {
		if (st.Find("configs\\" + AlgFolder[x]) != -1) {
			return x;
		}
	}
	return -1;
}

void CMainFrame::ShowStatusText(int line, CString st)
{
	CMFCRibbonEdit *pEdit = 0;
	if (line == 0) pEdit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_OINFO));
	if (line == 1) pEdit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_OINFO2));
	if (line == 2) pEdit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_OINFO3));
	if (line == 3) pEdit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_OINFO4));
	if (line == 4) pEdit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_OINFO5));
	if (line == 5) pEdit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_OINFO6));
	if (line == 6) pEdit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_OINFO7));
	if (line == 7) pEdit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_OINFO8));
	if (line == 8) pEdit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_OINFO9));
	if (pEdit) pEdit->SetEditText(st);
}

void CMainFrame::WriteLog(int log, CString st)
{
	CGLib::WriteLog(log, st);
}

void CMainFrame::ShowLog(int log, CString st)
{
	// Add log to vector from this thread only
	if (pGen && pGen->can_send_log) {
		// Save log if not too much already
		if (pGen->logs[log].size() < MAX_SAVED_LOGS) 
			pGen->logs[log].push_back(st);
	}
	COutputList* pOL=0;
	if (log == 0) pOL = &m_wndOutput.m_wndOutputDebug;
	if (log == 1) {
		pOL = &m_wndOutput.m_wndOutputWarn;
		CGLib::AppendLineToFile("log\\warning.log", st + "\n");
	}
	if (log == 2) pOL = &m_wndOutput.m_wndOutputPerf;
	if (log == 3) pOL = &m_wndOutput.m_wndOutputAlgo;
	if (log == 4) pOL = &m_wndOutput.m_wndOutputMidi;
	if (log == 6) pOL = &m_wndOutput.m_wndOutputConfirm;
	if (log == 7) pOL = &m_wndOutput.m_wndOutputEmu;
	if (log == 8) pOL = &m_wndOutput.m_wndOutputCorack;
	if (pOL) {
		pOL->AddString(st);
		if (pOL->GetCount() > 1000) pOL->DeleteString(0);
		pOL->SetTopIndex(pOL->GetCount() - 1);
	}
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;

	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
		 | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_MAXIMIZE | WS_SYSMENU;

	return TRUE;
}

BOOL CMainFrame::CreateDockingWindows()
{
	BOOL bNameValid;
	// Create output window
	CString strOutputWnd;
	bNameValid = strOutputWnd.LoadString(IDS_OUTPUT_WND);
	ASSERT(bNameValid);
	if (!m_wndOutput.Create(strOutputWnd, this, CRect(0, 0, 100, 100), TRUE, ID_VIEW_OUTPUTWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Output window\n");
		return FALSE; // failed to create
	}

	SetDockingWindowIcons(theApp.m_bHiColorIcons);
	return TRUE;
}

void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{
	HICON hOutputBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_OUTPUT_WND_HC : IDI_OUTPUT_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndOutput.SetIcon(hOutputBarIcon, FALSE);

}

void CMainFrame::ShowOutputWnd()
{
	if (m_wndOutput.IsVisible()) m_wndOutput.ShowPane(FALSE, FALSE, FALSE);
	else m_wndOutput.ShowPane(TRUE, FALSE, TRUE);
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame message handlers

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_VS_2008:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_WINDOWS_7:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(TRUE);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMyVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
	}

	m_wndOutput.UpdateFonts();
	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}


void CMainFrame::OnFilePrint()
{
	if (IsPrintPreview())
	{
		PostMessage(WM_COMMAND, AFX_ID_PREVIEW_PRINT);
	}
}

void CMainFrame::OnFilePrintPreview()
{
	if (IsPrintPreview())
	{
		PostMessage(WM_COMMAND, AFX_ID_PREVIEW_CLOSE);  // force Print Preview mode closed
	}
}

void CMainFrame::OnUpdateFilePrintPreview(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(IsPrintPreview());
}

void CMainFrame::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CFrameWndEx::OnSettingChange(uFlags, lpszSection);
	m_wndOutput.UpdateFonts();
}


void CMainFrame::OnButtonParams()
{
	if (m_state_gen != 1 && m_algo > -1 && m_config != "") {
		CString path = "configs\\" + AlgFolder[m_algo] + "\\" + m_config + ".pl";
		::ShellExecute(GetDesktopWindow()->m_hWnd, "open", path, NULL, NULL, SW_SHOWNORMAL);
	}
}

void CMainFrame::ClearLogs() {
	// Clear only log tabs that are specific to an algorithm run
	m_wndOutput.m_wndOutputDebug.ResetContent();
	m_wndOutput.m_wndOutputWarn.ResetContent();
	m_wndOutput.m_wndOutputAlgo.ResetContent();
	m_wndOutput.m_wndOutputConfirm.ResetContent();
	m_wndOutput.m_wndOutputEmu.ResetContent();
	m_wndOutput.m_wndOutputCorack.ResetContent();
	// Clear status lines
	for (int i=0; i<STATUS_LINES; ++i) CGLib::SetStatusText(i, "");
}

void CMainFrame::LoadResults(CString path) {
	if (m_state_gen == 1) {
		AfxMessageBox("Please stop generation before opening saved results");
		return;
	}
	string::size_type pos = string(path).find_last_of("\\/");
	CString dir = string(path).substr(0, pos).c_str();
	string::size_type pos2 = string(path).find_last_of("./");
	CString fname = string(path).substr(pos + 1, pos2 - pos - 1).c_str();
	GetActiveDocument()->SetTitle(fname);
	m_fname = fname;
	m_dir = dir;
	// Stop playback
	if (m_state_play > 0) OnButtonPlay();
	m_state_play = 0;
	// Delete generator
	if (pGen != 0) {
		WriteLog(0, "Starting generation: Removing previous generator");
		delete pGen;
		m_state_gen = 0;
	}
	// Load algorithm
	m_algo_id = 101;
	NewGen();
	if (pGen != 0) {
		pGen->LoadResults(dir, fname);
		m_algo_id = pGen->m_algo_id;
		delete pGen;
	}
	NewGen();
	// Create algorithm
	if (pGen != 0) {
		pGen->m_loading = 1;
		// Clear note minimax
		ng_min = 0;
		ng_max = 0;
		// Clear scroll
		CSize DocSize(0, 0);
		((CMGenView*)(GetActiveView()))->SetScrollSizes(MM_TEXT, DocSize, CSize(500, 500), CSize(50, 50));
		WriteLog(0, _T("Loading file: ") + path);
		// Set pGen variables
		pGen->InitRandom();
		pGen->WM_GEN_FINISH = WM_GEN_FINISH;
		// Initialize MIDI
		pGen->StopMIDI();
		ClearLogs();
		pGen->StartMIDI(GetMidiI(), 0);
		pGen->gen_start_time = CGLib::time();
		// Load results
		pGen->LoadResults(dir, fname);
		pGen->LoadResultLogs(dir, fname);
		if (pGen->save_format_version != MGR_VERSION) MessageBox("This file was created with a different file format " + pGen->save_format_version + "\nYou can get errors trying to load it. This application version " + CGLib::APP_VERSION + " works with file format version " + MGR_VERSION + ". Loading of older file formats is sometimes supported.", "Error");
		m_algo_id = pGen->m_algo_id;
		if (GetAlgoById(m_algo_id) > -1) m_algo = GetAlgoById(m_algo_id);
		pGen->m_algo_folder = AlgFolder[m_algo];
		pGen->m_algo_name = AlgName[m_algo];
		pGen->ly_debugexpect = ly_debugexpect;
		m_config = pGen->m_config;
		// Load configs
		pGen->LoadConfig("configs\\" + AlgFolder[m_algo] + "\\" + m_config + ".pl");
		pGen->LoadConfig(dir + "\\" + fname + ".pl", 0);
		// Load music
		pGen->LoadResultMusic(dir, fname);
		// Adapt
		pGen->Adapt(0, pGen->t_sent - 1);
		m_state_gen = 2;
		GetActiveView()->SetFocus();
		Invalidate();
		GetActiveView()->Invalidate();
	}
}

void CMainFrame::LoadMidi(CString path) {
	CString ext = CGLib::ext_from_path(path);
	if (m_state_gen == 1) {
		AfxMessageBox("Please stop generation before opening music file");
		return;
	}
	CMFIDialog dlg;
	dlg.ext = ext;
	if (dlg.DoModal() == IDOK) {
		// Check default config exists
		if (!CGLib::fileExists("configs\\" + AlgFolder[m_algo] +".pl")) {
			AfxMessageBox("Not found default configuration for loading music file with this algorithm at configs\\" + AlgFolder[m_algo] + ".pl \nPlease create default configuration or use manual configuration.");
			return;
		}
		// Create config name
		CString time_str = CTime::GetCurrentTime().Format("%Y-%m-%d %H:%M:%S");
		m_config = CGLib::bname_from_path(path);
		CString fname = "configs\\" + AlgFolder[m_algo] + "\\" + m_config + ".pl";
		// Copy default config
		CGLib::copy_file("configs\\" + AlgFolder[m_algo] + ".pl", fname);
		// Append config name
		CGLib::AppendLineToFile(fname,
			"\n# This config was created from default config file configs\\" + AlgFolder[m_algo] + ".pl\n");
		CGLib::AppendLineToFile(fname,
			"# Created at " + time_str + "\n");
		if (ext == "xml" || ext == "mxl") CGLib::AppendLineToFile(fname, "MusicXML_file = " + path + "\n");
		else CGLib::AppendLineToFile(fname,	"Midi_file = " + path + "\n");
		// Load newly created config into table
		LoadAlgo();
		// Save settings
		SaveSettings();
		// Start generation
		OnButtonGen();
	}
}

bool CMainFrame::NewDocument()
{
	if ((m_state_gen == 2) && (m_state_play == 0)) if (pGen != 0) {
		WriteLog(0, "Removing previous generator");
		delete pGen;
		m_state_gen = 0;
		pGen = 0;
		return true;
	}
	return false;
}

void CMainFrame::NewGen()
{
	pGen = 0;
	if (m_algo_id == 101) pGen = new CGenCF1();
	if (m_algo_id == 112) pGen = new CGenCA2();
	if (m_algo_id == 113) pGen = new CGenCA3();
	if (m_algo_id == 111) pGen = new CGenCA1();
	if (m_algo_id == 121) pGen = new CGenCP1();
	if (m_algo_id == 122) pGen = new CGenCP2();
	if (m_algo_id == 1001) pGen = new CGenRS1();
	if (m_algo_id == 2001) pGen = new CGenMP1();
	if (m_algo_id == 1101) pGen = new CGenRL1();
}

void CMainFrame::OnButtonGen()
{
	if (m_state_gen == 1) {
		if (pGen != 0) {
			pGen->need_exit = 1;
			WriteLog(0, "Sent need_exit to generation thread");
		}
		return;
	}
	if ((m_state_gen == 2) || (pGen != 0)) {
		WriteLog(0, "Starting generation: Removing previous generator");
		delete pGen;
		m_state_gen = 0;
		// Clear scroll
		CSize DocSize(0, 0);
		((CMGenView*)(GetActiveView()))->SetScrollSizes(MM_TEXT, DocSize, CSize(500, 500), CSize(50, 50));
	}
	LoadAlgo();
	NewGen();
	if (pGen != 0) {
		// Clear document title
		GetActiveDocument()->SetTitle("Untitled");
		// Clear memory usage warning flag
		warn_memory_usage = 0;
		// Clear note minimax
		ng_min = 0;
		ng_max = 0;
		ClearLogs();
		WriteLog(0, _T("Started generator: ") + AlgName[m_algo]);
		// Clear current saved path
		m_fname.Empty();
		m_dir.Empty();
		// Set pGen variables
		pGen->InitRandom();
		pGen->WM_GEN_FINISH = WM_GEN_FINISH;
		pGen->m_algo_id = m_algo_id;
		pGen->m_algo_folder = AlgFolder[m_algo];
		pGen->m_algo_name = AlgName[m_algo];
		pGen->m_config = m_config;
		pGen->m_pspeed = m_pspeed;
		pGen->ly_debugexpect = ly_debugexpect;
		// Initialize variables
		pGen->LoadConfig("configs\\" + AlgFolder[m_algo] + "\\" + m_config + ".pl");
		pGen->InitVectors();
		// Initialize MIDI
		pGen->StopMIDI();
		pGen->StartMIDI(GetMidiI(), 0);
		pGen->gen_start_time = CGLib::time();
		// Initialize autosave document name
		m_fname = CTime::GetCurrentTime().Format("%Y-%m-%d_%H-%M-%S_") + m_config;
		m_dir = "autosaves\\" + AlgFolder[m_algo] + "\\" + m_fname;
		// Export last autosave name
		DeleteFile("log\\autosave.txt");
		CGLib::AppendLineToFile("log\\autosave.txt", m_dir + "\n");
		CGLib::AppendLineToFile("log\\autosave.txt", m_fname + "\n");

		pGen->as_fname = m_fname;
		pGen->as_dir = m_dir;
		// Start generation
		m_state_gen = 1;
		m_state_play = 0;
		m_GenThread = AfxBeginThread(CMainFrame::GenThread, pGen);
		// Start timer
		SetTimer(TIMER1, m_view_timer, NULL);
		::KillTimer(m_hWnd, TIMER2);
		if (autoplay && pGen->shuffle == 0 && GetMidiI() != -1) SetTimer(TIMER2, 1000, NULL);
	}
}

void CMainFrame::OnButtonSettings()
{
}

void CMainFrame::OnButtonAlgo()
{
	if (m_state_gen != 1) {
		LoadAlgo();
		CAlgoDlg dlg;
		if (dlg.DoModal() == IDOK) {
			OnButtonGen();
		}
	}
}


void CMainFrame::OnCheckOutputwnd() {
	if (m_wndOutput.IsVisible()) m_wndOutput.ShowPane(FALSE, FALSE, FALSE);
	else m_wndOutput.ShowPane(TRUE, FALSE, TRUE);
}

LRESULT CMainFrame::OnGenFinish(WPARAM wParam, LPARAM lParam) {
	if (wParam == 0) {
		if (m_state_play == 0) ::KillTimer(m_hWnd, TIMER1);
		GetActiveView()->Invalidate();
		WriteLog(0, "Generation finished");
		m_state_gen = 2;
		GetActiveDocument()->SetTitle(m_fname);
		// Start playback after shuffle
		if (pGen->shuffle) {
			if (autoplay && m_state_play == 0 && pGen->t_sent > 0) OnButtonPlay();
		}
		// Exit program
		if (CGLib::m_testing) {
			SetTimer(TIMER4, 300, NULL);
		}
	}
	if (wParam == 1) {
		// This message is sent from generation thread when t_sent is increased
		// Now it is processed OnTimer
	}
	return 0;
}

void CMainFrame::ExportAdaptedMidi() {
	pGen->m_pspeed = m_pspeed;
	if (pGen->m_pspeed != pGen->adapt_pspeed) pGen->Adapt(0, pGen->t_generated - 1);
	pGen->ExportAdaptedMidi(m_dir, m_fname);
}

void CMainFrame::OnUpdateCheckOutputwnd(CCmdUI *pCmdUI) {
	BOOL bEnable = m_wndOutput.IsVisible();
	pCmdUI->Enable();
	pCmdUI->SetCheck(bEnable);
}

void CMainFrame::OnUpdateComboAlgo(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
}

int CMainFrame::GetMidiI()
{
	CMFCRibbonComboBox *pCombo = DYNAMIC_DOWNCAST(CMFCRibbonComboBox,
		m_wndRibbonBar.FindByID(ID_COMBO_MIDIOUT));
	int i = pCombo->GetCurSel();
	if (i == -1) return -1;
	else return pCombo->GetItemData(i);
}

void CMainFrame::LoadAlgo()
{
	CString fname = "configs\\algorithms.txt";
	if (!CGLib::fileExists(fname)) {
		CString est;
		est.Format("Cannot find file: %s", fname);
		WriteLog(5, est);
		return;
	}
	ifstream fs;
	fs.open(fname);
	CString st, st2;
	char pch[2550];
	int pos = 0;
	// Load header
	//fs.getline(pch, 2550);
	ParamName.clear();
	for (int i = 0; i < MAX_ALGO; i++) ParamCount[i] = 0;
	AlgCount = 0;
	AlgGroups.clear();
	AlgGroups.resize(MAX_ALGO);
	AlgGCount = 0;
	while (fs.good()) {
		fs.getline(pch, 2550);
		st = pch;
		// Remove unneeded
		pos = st.Find("#");
		// Check if it is first symbol
		if (pos == 0)	st = st.Left(pos);
		pos = st.Find(" #");
		// Check if it is after space
		if (pos > -1)	st = st.Left(pos);
		st.Trim();
		pos = 0;
		if (st.Find("|") != -1) {
			st2 = st.Tokenize("|", pos);
			st2.Trim();
			AlgID[AlgCount] = atoi(st2);
			st2 = st.Tokenize("|", pos);
			st2.Trim();
			AlgFolder[AlgCount] = st2;
			st2 = st.Tokenize("|", pos);
			st2.Trim();
			AlgMFI[AlgCount] = atoi(st2);
			st2 = st.Tokenize("|", pos);
			st2.Trim();
			AlgXFI[AlgCount] = atoi(st2);
			st2 = st.Tokenize("|", pos);
			st2.Trim();
			AlgName[AlgCount] = st2;
			st2 = st.Tokenize("|", pos);
			st2.Trim();
			AlgGroup[AlgCount] = st2;
			// Load groups
			if (find(AlgGroups.begin(), AlgGroups.end(), st2) == AlgGroups.end()) {
				AlgGroups[AlgGCount] = st2;
				AlgGCount++;
			}
			if (AlgMFI[AlgCount]) {
				CString* pst = find(begin(AlgMFIGroups), end(AlgMFIGroups), st2);
				if (*pst != st2) {
					AlgMFIGroups[AlgMFIGCount] = st2;
					AlgMFIGCount++;
				}
			}
			if (AlgXFI[AlgCount]) {
				CString* pst = find(begin(AlgXFIGroups), end(AlgXFIGroups), st2);
				if (*pst != st2) {
					AlgXFIGroups[AlgXFIGCount] = st2;
					AlgXFIGCount++;
				}
			}
			// Load instruments
			st2 = st.Tokenize("|", pos);
			st2.Trim();
			AlgComment[AlgCount] = st2;
			// Check if folder exists
			CString path = "configs\\" + AlgFolder[AlgCount];
			if (!CGLib::dirExists(path)) {
				CreateDirectory(path, NULL);
				ofstream fs;
				fs.open(path + "\\Test.pl");
				fs.close();
			}
			// Get all configs
			DIR *dir;
			struct dirent *ent;
			ParamName.push_back(vector<CString>());
			if ((dir = opendir(path)) != NULL) {
				while ((ent = readdir(dir)) != NULL) {
					CString st = ent->d_name;
					if ((st.Left(1) != ".") && (st.Right(3) == ".pl")) {
						ParamName[AlgCount].push_back(st.Mid(0, st.GetLength()-3));
						ParamCount[AlgCount]++;
					}
				}
				closedir(dir);
			}
			AlgCount++;
		}
	}
	fs.close();
	if (AlgCount == 0) {
		WriteLog(5, "Error loading algorithms.txt");
	}
}

void CMainFrame::LoadSettings()
{
	CString fname = "configs\\settings.pl";
	if (!CGLib::fileExists(fname)) {
		CString est;
		est.Format("Cannot find file: %s", fname);
		WriteLog(5, est);
		return;
	}
	ifstream fs;
	fs.open(fname);
	CString st, st2, st3, st4;
	char pch[2550];
	int pos = 0;
	while (fs.good()) {
		fs.getline(pch, 2550);
		st = pch;
		pos = st.ReverseFind('#');
		if (pos != -1) st = st.Left(pos);
		st.Trim();
		pos = st.Find("=");
		if (pos != -1) {
			st2 = st.Left(pos);
			st3 = st.Mid(pos + 1);
			st2.Trim();
			st3.Trim();
			st2.MakeLower();
			int idata = atoi(st3);
			if (st2 == "algorithm") {
				m_algo_id = idata;
				if (GetAlgoById(m_algo_id) > -1) m_algo = GetAlgoById(m_algo_id);
			}
			if (st2 == "midi_out") {
				CMFCRibbonComboBox *pCombo = DYNAMIC_DOWNCAST(CMFCRibbonComboBox,
					m_wndRibbonBar.FindByID(ID_COMBO_MIDIOUT));
				pCombo->SelectItem(st3);
				if (GetMidiI() == -1) {
					pCombo->SelectItem(DISABLE_PLAYBACK);
				}
			}
			if (st2 == "lilypond_path") m_lilypond_path = st3;
			if (st2 == "lilypond_param") m_lilypond_param = st3;
			if (st2 == "playback_speed") {
				CMFCRibbonEdit* pRibbonSpin = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_SPIN_PSPEED));
				pRibbonSpin->SetEditText(st3);
				if (!CGLib::m_testing) m_pspeed = atoi(st3);
			}
			if (st2 == "horizontal_zoom") {
				CMFCRibbonEdit* pRibbonSpin = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_SPIN_ZOOM));
				pRibbonSpin->SetEditText(st3);
				zoom_x = atoi(st3);
			}
			CGLib::CheckVar(&st2, &st3, "view_timer", &m_view_timer, MIN_VIEW_TIMER, MAX_VIEW_TIMER);
			CGLib::CheckVar(&st2, &st3, "step_dyn", &m_step_dyn);
			CGLib::CheckVar(&st2, &st3, "show_marks", &show_marks);
			if (!CGLib::m_testing) 
				CGLib::CheckVar(&st2, &st3, "ly_debugexpect", &ly_debugexpect);
			CGLib::CheckVar(&st2, &st3, "show_comments", &show_comments);
			CGLib::CheckVar(&st2, &st3, "show_shift", &show_shift);
			CGLib::CheckVar(&st2, &st3, "show_vib", &show_vib);
			CGLib::CheckVar(&st2, &st3, "show_vibf", &show_vibf);
			CGLib::CheckVar(&st2, &st3, "show_lining", &show_lining);
			CGLib::CheckVar(&st2, &st3, "show_lines", &show_lines);
			CGLib::CheckVar(&st2, &st3, "show_progress", &show_progress);
			CGLib::CheckVar(&st2, &st3, "autoplay", &autoplay);
			CGLib::CheckVar(&st2, &st3, "show_tempo", &show_tempo);
			CGLib::CheckVar(&st2, &st3, "show_vel", &show_vel);
			CGLib::CheckVar(&st2, &st3, "show_curve", &show_curve);
			CGLib::CheckVar(&st2, &st3, "show_nsr", &show_nsr);
			CGLib::CheckVar(&st2, &st3, "show_notecolors", &show_notecolors);
			CGLib::CheckVar(&st2, &st3, "debug_level", &m_debug_level);
			for (int i = 0; i < MAX_GRAPH; ++i) {
				st4.Format("show_graph%d", i);
				CGLib::CheckVar(&st2, &st3, st4.GetBuffer(), &show_graph[i], 0, 1);
			}
			CGLib::LoadVar(&st2, &st3, "config", &m_config);
			//CGLib::LoadVar(&st2, &st3, "midi_program", &midi_program);
		}
	}
	CGLib::show_lining = show_lining;
	CGLib::debug_level = m_debug_level;
	fs.close();
	// Resave file to update format
	SaveSettings();
}

int CMainFrame::GetAlgoById(int id) {
	int* found = find(begin(AlgID), end(AlgID), id);
	if (*found == id) {
		return distance(AlgID, found);
	}
	return -1;
}

void CMainFrame::SaveSettings()
{
	ofstream fs;
	fs.open("configs\\settings.pl");
	CString st;
	fs << "# Settings of MGen\n";
	fs << "# This file is loaded on MGen startup and automatically saved on every setting change\n";
	fs << "\n";
	st.Format("Algorithm = %d # Id of the currently selected algorithm\n", m_algo_id);
	fs << st;
	st.Format("Config = %s # Name of configuration file for selected algorithm\n", m_config);
	fs << st;
	int i = GetMidiI();
	//st.Format("MIDI_OUT_ID = %d\n", i);
	//fs << st;
	if (i > -1) {
		st.Format("MIDI_OUT = %s # Name of MIDI device used for playing notes\n", MidiName[i]);
		fs << st;
	}
	else {
		st.Format("MIDI_OUT = %s # Name of MIDI device used for playing notes\n", DISABLE_PLAYBACK);
		fs << st;
	}
	st.Format("Horizontal_zoom = %d # Zoom of the piano roll. Can be from 1 to %d\n", zoom_x, MAX_HZOOM);
	fs << st;
	st.Format("playback_speed = %d # Playback speed in percent\n", m_pspeed);
	fs << st;
	st.Format("show_marks = %d # Set to 1 to show note mark strings\n", show_marks);
	fs << st;
	st.Format("show_comments = %d # Set to 1 to surround notes which have comments with rectangles\n", show_comments);
	fs << st;
	st.Format("show_shift = %d # Set to 1 to show note shifts\n", show_shift);
	fs << st;
	st.Format("show_vib = %d # Set to 1 to show vibrato\n", show_vib);
	fs << st;
	st.Format("show_vibf = %d # Set to 1 to show vibrato freq\n", show_vibf);
	fs << st;
	st.Format("show_lining = %d # Set to 1 to show note lining\n", show_lining);
	fs << st;
	st.Format("show_lines = %d # Set to 1 to show line separators\n", show_lines);
	fs << st;
	st.Format("show_progress = %d # Set to 1 to show note scan progress\n", show_progress);
	fs << st;
	st.Format("ly_debugexpect = %d # Set to 1 to generate debug expect in lilypond files\n", ly_debugexpect);
	fs << st;
	st.Format("show_vel = %d # Set to 1 to show note velocity using alpha\n", show_vel);
	fs << st;
	st.Format("show_curve = %d # Set to 1 to show melody curves (MeloCurve)\n", show_curve);
	fs << st;
	st.Format("show_nsr = %d # Set to 1 to show note scan range\n", show_nsr);
	fs << st;
	st.Format("show_tempo = %d # Set to 1 to show tempo\n", show_tempo);
	fs << st;
	for (int i = 0; i < MAX_GRAPH; ++i) {
		st.Format("show_graph%d = %d # Set to 1 to show graph %d\n", i, show_graph[i], i);
		fs << st;
	}
	st.Format("show_notecolors = %d # Set to 1 to show note colors instead of instrument colors\n", show_notecolors);
	fs << st;
	fs << "\n";
	fs << "# The following settings cannot be changed in GUI. You can change them only in this file\n";
	st.Format("Autoplay = %d # Set to 1 to start playback as soon as first notes are generated\n", autoplay);
	fs << st;
	st.Format("View_timer = %d # ms between each screen update during generation and playback. 100 ms is recommended. Increase for slower computers\n", m_view_timer);
	fs << st;
	st.Format("Step_dyn = %d # Show dynamics with note opacity for each step of note. Disable for slower computers.\n", m_step_dyn);
	fs << st;
	st.Format("Debug_level = %d # Increase to show more debug logs\n", m_debug_level);
	fs << st;
	st.Format("Lilypond_path = %s # Path to lilypond or Frescobaldi executable\n", m_lilypond_path);
	fs << st;
	st.Format("Lilypond_param = %s # Parameter for lilypond executable\n", m_lilypond_param);
	fs << st;
	fs.close();
}

void CMainFrame::OnComboAlgo()
{
	SaveSettings();
}

void CMainFrame::CheckMemoryUsage() {
	if (warn_memory_usage) return;
	PROCESS_MEMORY_COUNTERS_EX pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
	SIZE_T virtualMemUsedByMe = pmc.PrivateUsage / 1024 / 1024;
	if (virtualMemUsedByMe > MAX_MEMORY_WARN) {
		CString st;
		st.Format("Memory usage is above limit %d Mb", (int)MAX_MEMORY_WARN);
		WriteLog(5, st);
		warn_memory_usage = 1;
	}
}

void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	CFrameWndEx::OnTimer(nIDEvent);
	if (nIDEvent == TIMER1)
	{
		CheckMemoryUsage();
		GetActiveView()->Invalidate();
		//int play_time = CGLib::time() - pGen->midi_start_time;
		// Stop playback
		if ((m_state_gen != 1) && (m_state_play == 2) && pGen && (CGLib::time() > pGen->midi_sent_t)) {
			OnButtonPlay();
		}
	}
	if ((nIDEvent == TIMER2) && (pGen != 0))
	{
		if (pGen->t_sent < 2) return;
		// Decide if we can play
		if (m_state_play == 0) {
			if (m_state_gen == 2) {
				m_state_play = 1;
				SetTimer(TIMER1, m_view_timer, NULL);
			}
			if (m_state_gen == 1) {
				if (!pGen->mutex_output.try_lock_for(chrono::milliseconds(1000))) {
					WriteLog(4, "OnGenFinish mutex timed out: playback not started");
					return;
				}
				// Check generation speed
				double gtime = pGen->stime[pGen->t_sent - 1]*100/pGen->m_pspeed;
				double ptime = CGLib::time() - pGen->gen_start_time;
				if ((gtime / ptime > 2) || (gtime > 30000) || ((gtime / ptime > 1.2) && (gtime > 5000))) {
					m_state_play = 1;
					SetTimer(TIMER1, m_view_timer, NULL);
				}
				else {
					CString st;
					st.Format("Only %.1f s generated after %.1f s. Playback is postponed to avoid buffer underruns", gtime / 1000, ptime / 1000);
					WriteLog(4, st);
				}
				pGen->mutex_output.unlock();
			}
		}
		if (m_state_play == 1) {
			if ((pGen->t_sent > pGen->midi_sent)) {
				//CString st;
				//st.Format("Chunk received up to %d", pGen->t_sent);
				//WriteLog(0, st);
				pGen->m_pspeed = m_pspeed;
				// If generation finished set last run
				if (m_state_gen == 2) pGen->midi_last_run = 1;
				pGen->SendMIDI(pGen->midi_sent, pGen->t_sent);
			}
			if ((pGen->buf_underrun) || ((pGen->t_sent == pGen->midi_sent) && (m_state_gen == 2))) {
				::KillTimer(m_hWnd, TIMER2);
				m_state_play = 2;
				//GetActiveView()->Invalidate();
			}
		}
	}
	if (nIDEvent == TIMER3 && CGLib::can_send_log) {
		if (CGLib::mutex_log.try_lock_for(chrono::milliseconds(100))) {
			// Show status
			if (CGLib::oinfo.size()) for (int line=0; line<STATUS_LINES; ++line) {
				if (CGLib::oinfo_changed[line]) {
					ShowStatusText(line, CGLib::oinfo[line]);
					CGLib::oinfo_changed[line] = 0;
				}
			}
			// Show logs
			for (int log = 0; log < LOG_TABS; ++log) {
				if (CGLib::log_buffer_size[log]) {
					int sent = 0;
					while (!CGLib::log_buffer[log].empty()) {
						ShowLog(log, CGLib::log_buffer[log].front());
						CGLib::log_buffer[log].pop_front();
						--CGLib::log_buffer_size[log];
						++sent;
						if (sent >= LOG_MAX_SEND) break;
					}
				}
			}
			CGLib::mutex_log.unlock();
		}
	}
	if (nIDEvent == TIMER4) {
		PostMessage(WM_CLOSE);
	}
	if (nIDEvent == TIMER5) {
		::KillTimer(m_hWnd, TIMER5);
		ParseCommandLine();
	}
}

UINT CMainFrame::GenThread(LPVOID pParam) {
	CGTemplate* pGen = (CGTemplate*)pParam;

	if (pGen == NULL) return 1;   // if Object is not valid  

	// Clear frequency stats
	fill(CGLib::status_updates.begin(), CGLib::status_updates.end(), (long long)0);
	fill(CGLib::logs_sent.begin(), CGLib::logs_sent.end(), (long long)0);

	CreateDirectory("autosaves", NULL);
	CreateDirectory("autosaves\\" + pGen->m_algo_folder, NULL);
	CreateDirectory(pGen->as_dir, NULL);

	pGen->InitRandom();
	pGen->Generate();
	pGen->time_stopped = CGLib::time();
	pGen->CheckMemoryUsage();

	// Update expect
	if (CGLib::fileExists(pGen->as_dir + "\\edb-" + pGen->as_fname + ".csv") &&
		CGLib::fileExists("db\\expect-auto.csv")) {
		CConf::ReplaceCsvDb(pGen->as_dir + "\\edb-" + pGen->as_fname + ".csv", 
			"db\\expect-auto.csv",
			"File", pGen->midi_file);
	}
	// Save results
	pGen->SaveResults(pGen->as_dir, pGen->as_fname);
	pGen->SaveMidi(pGen->as_dir, pGen->as_fname);
	if (pGen->m_algo_id != 2001)
		pGen->SaveLy(pGen->as_dir, pGen->as_fname);
	// Copy config
	CGLib::copy_file("configs\\" + pGen->m_algo_folder + "\\" + pGen->m_config + ".pl", pGen->as_dir + "\\" + pGen->as_fname + ".pl");
	// Append config name
	CGLib::AppendLineToFile(pGen->as_dir + "\\" + pGen->as_fname + ".pl",
		"\n# This config was copied from file " + pGen->m_algo_folder + "\\" + pGen->m_config + ".pl\n");
	CGLib::AppendLineToFile(pGen->as_dir + "\\" + pGen->as_fname + ".pl",
		"# Originally autosaved at " + pGen->as_dir + "\n");
	//WriteLog(1, "configs\\" + AlgFolder[m_algo] + "\\" + m_config + ".pl");
	//WriteLog(1, dir + "\\config.pl");
	// Export adapted MIDI if not playing
	if (pGen->t_sent > 0) pGen->ExportAdaptedMidi(pGen->as_dir, pGen->as_fname);

	::PostMessage(pGen->m_hWnd, WM_GEN_FINISH, 0, 0);

	if (CGLib::can_send_log) {
		// Show updates frequency
		CString st, st2;
		long long total;
		if (CGLib::status_updates.size() == STATUS_LINES) {
			total = 0;
			for (int i = 0; i < STATUS_LINES; ++i) {
				total += CGLib::status_updates[i];
				st.Format("\n%lld per second (%lld in %lld seconds) ",
					CGLib::status_updates[i] * 1000 / (pGen->time_stopped - pGen->gen_start_time + 1),
					CGLib::status_updates[i], (pGen->time_stopped - pGen->gen_start_time) / 1000);
				st2 += st;
			}
			st.Format("Status updates: %lld per second (%lld in %lld seconds). Detailed: ",
				total * 1000 / (pGen->time_stopped - pGen->gen_start_time + 1),
				total, (pGen->time_stopped - pGen->gen_start_time) / 1000);
			st2 = st + st2;
			CGLib::WriteLog(2, st2);
			if (pGen->time_stopped - pGen->gen_start_time > 1000 && total * 1000 / (pGen->time_stopped - pGen->gen_start_time + 1) > WARN_STATUS_FREQ) {
				st.Format("Algorithm status update is %lld per second (above recommended %d). This can decrease speed of your algorithm. Please check algorithm.",
					total * 1000 / (pGen->time_stopped - pGen->gen_start_time + 1), WARN_STATUS_FREQ);
				CGLib::WriteLog(1, st);
			}
		}

		// Show logs frequency
		if (CGLib::logs_sent.size() == LOG_TABS) {
			st2.Empty();
			total = 0;
			for (int i = 0; i < LOG_TABS; ++i) {
				total += CGLib::logs_sent[i];
				st.Format("\n%lld per second (%lld in %lld seconds) ",
					CGLib::logs_sent[i] * 1000 / (pGen->time_stopped - pGen->gen_start_time + 1),
					CGLib::logs_sent[i], (pGen->time_stopped - pGen->gen_start_time) / 1000);
				st2 += st;
			}
			st.Format("Logs sent: %lld per second (%lld in %lld seconds). Detailed: ",
				total * 1000 / (pGen->time_stopped - pGen->gen_start_time + 1),
				total, (pGen->time_stopped - pGen->gen_start_time) / 1000);
			st2 = st + st2;
			CGLib::WriteLog(2, st2);
			if (pGen->time_stopped - pGen->gen_start_time > 1000 && total * 1000 / (pGen->time_stopped - pGen->gen_start_time) > WARN_LOG_FREQ) {
				st.Format("Algorithm sends %lld logs per second (above recommended %d). This can decrease speed of your algorithm. Please check algorithm.",
					total * 1000 / (pGen->time_stopped - pGen->gen_start_time + 1), WARN_LOG_FREQ);
				CGLib::WriteLog(1, st);
			}
		}
	}

	return 0;   // thread completed successfully 
}

void CMainFrame::OnClose()
{
	// Stop log/status timer
	::KillTimer(m_hWnd, TIMER3);
	// Prevent new logs to be added
	CGLib::can_send_log = 0;
	if (m_state_gen == 1) {
		OnButtonGen();
		// If cannot stop generator, report error
		if (WaitForSingleObject(m_GenThread->m_hThread, 100000) == WAIT_TIMEOUT)
			CGLib::exitcode = 11;
		delete pGen;
		pGen = 0;
	}
	if (pGen != 0) {
		delete pGen;
		pGen = 0;
	}

	CFrameWndEx::OnClose();
}

void CMainFrame::OnUpdateComboMidiout(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_state_play == 0);
}

void CMainFrame::OnButtonEparams()
{
	//CEditParamsDlg dlg;
	//dlg.DoModal();
}

void CMainFrame::OnUpdateButtonPlay(CCmdUI *pCmdUI)
{
	if ((m_state_gen == 1) && (m_state_play == 0)) pCmdUI->Enable(0);
	else if (m_state_gen != 0) pCmdUI->Enable(1);
	else pCmdUI->Enable(0);
	if (m_state_play > 0) UpdateUIText(pCmdUI, 3, "Stop Playback");
	else UpdateUIText(pCmdUI, 3, "Play");
}

void CMainFrame::OnUpdateButtonGen(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
	if (m_state_gen == 1) UpdateUIText(pCmdUI, 2, "Stop Generator");
	else UpdateUIText(pCmdUI, 2, "Generate");
}

void CMainFrame::OnButtonPlay()
{
	if (m_state_play > 0) {
		m_state_play = 0;
		if (pGen != 0) {
			::KillTimer(m_hWnd, TIMER2);
			if (m_state_gen == 2) ::KillTimer(m_hWnd, TIMER1);
			pGen->StopMIDI();
		}
		WriteLog(4, "Stopped playback");
	}
	else if ((m_state_gen == 2) && (GetMidiI() > -1)) {
		StartPlay(0);
	}
}

void CMainFrame::StartPlay(int from)
{
	pGen->m_pspeed = m_pspeed;
	if (pGen->m_pspeed != pGen->adapt_pspeed) pGen->Adapt(0, pGen->t_generated - 1);
	pGen->StopMIDI();
	if (GetMidiI() == -1) return;
	pGen->StartMIDI(GetMidiI(), from);
	m_state_play = 1;
	// Start timer
	// If generation finished set last run
	if (m_state_gen == 2) pGen->midi_last_run = 1;
	pGen->SendMIDI(pGen->midi_sent, pGen->t_sent);
	SetTimer(TIMER1, m_view_timer, NULL);
	SetTimer(TIMER2, 1000, NULL);
}

void CMainFrame::OnComboMidiout()
{
	SaveSettings();
}


void CMainFrame::OnUpdateButtonAlgo(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_state_gen != 1);
	if (m_algo != -1) UpdateUIText(pCmdUI, 0, "Algorithm: " + AlgName[m_algo]);
	else UpdateUIText(pCmdUI, 0, "Algorithm: click to select");
}


void CMainFrame::OnUpdateButtonParams(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_state_gen != 1 && m_algo > -1 && m_config != "");
	if (m_config != "") {
		UpdateUIText(pCmdUI, 1, "Config: " + m_config);
	}
	else UpdateUIText(pCmdUI, 1, "Config: ");
}

void CMainFrame::OnUpdateButtonEparams(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_state_gen != 1 && m_algo > -1 && m_config != "");
}


BOOL CMainFrame::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (wParam >= ID_FILE_MRU_FILE1 && wParam <= ID_FILE_MRU_FILE16)
	{
		int nIndex = wParam - ID_FILE_MRU_FILE1;
		CString path = ((CMGenApp*)::AfxGetApp())->getRecentFile(nIndex);
		LoadResults(path);
	}

	return CFrameWndEx::OnCommand(wParam, lParam);
}


void CMainFrame::OnUpdateButtonSconfig(CCmdUI *pCmdUI)
{
	pCmdUI->Enable((m_fname != "") && (m_dir != ""));
}


void CMainFrame::OnButtonSconfig()
{
	if ((m_dir != "") && (m_fname != "")) {
		CString path = m_dir + "\\" + m_fname + ".pl";
		::ShellExecute(GetDesktopWindow()->m_hWnd, "open", path, NULL, NULL, SW_SHOWNORMAL);
	}
}


void CMainFrame::OnUpdateSpinPspeed(CCmdUI *pCmdUI)
{
	pCmdUI->Enable((m_state_play == 0) && (m_state_gen != 1));
}


void CMainFrame::OnSpinPspeed()
{
	CMFCRibbonEdit* pRibbonSpin = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_SPIN_PSPEED));
	CString st = pRibbonSpin->GetEditText();
	if (m_pspeed != atoi(st)) {
		m_pspeed = atoi(st);
		if (m_pspeed < MIN_PSPEED) m_pspeed = MIN_HZOOM;
		if (m_pspeed > MAX_PSPEED) m_pspeed = MAX_HZOOM;
		st.Format("%d", m_pspeed);
		pRibbonSpin->SetEditText(st);
		if (pGen != 0) pGen->m_pspeed = m_pspeed;
		GetActiveView()->Invalidate();
		SaveSettings();
	}
}


void CMainFrame::OnSpinZoom()
{
	CMFCRibbonEdit* pRibbonSpin = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_SPIN_ZOOM));
	CString st = pRibbonSpin->GetEditText();
	if (zoom_x != atoi(st)) {
		zoom_x = atoi(st);
		if (zoom_x < MIN_HZOOM) zoom_x = MIN_HZOOM;
		if (zoom_x > MAX_HZOOM) zoom_x = MAX_HZOOM;
		st.Format("%d", zoom_x);
		pRibbonSpin->SetEditText(st);
		GetActiveView()->Invalidate();
		SaveSettings();
	}
}


void CMainFrame::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
  CFrameWndEx::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CMainFrame::OnUpdateButtonOpenmidi(CCmdUI *pCmdUI) {
	if (m_state_gen == 2 && pGen && m_fname != "" && pGen->midi_saved) {
		pCmdUI->Enable(1);
	}
	else pCmdUI->Enable(0);
}

void CMainFrame::OnButtonOpenmidi() {
	::ShellExecute(GetDesktopWindow()->m_hWnd, "open", m_dir + "\\" + m_fname + ".mid", NULL, NULL, SW_SHOWNORMAL);
}

void CMainFrame::OnButtonEditalgo() {
	::ShellExecute(GetDesktopWindow()->m_hWnd, "open", "configs\\algorithms.txt", NULL, NULL, SW_SHOWNORMAL);
}

void CMainFrame::OnButtonEditinst() {
	//::ShellExecute(GetDesktopWindow()->m_hWnd, "open", "instruments.pl", NULL, NULL, SW_SHOWNORMAL);
	::ShellExecute(GetDesktopWindow()->m_hWnd, "open", "instruments", NULL, NULL, SW_SHOWNORMAL);
}

void CMainFrame::OnButtonReloadalgo() {
	LoadAlgo();
}

void CMainFrame::OnUpdateButtonReloadalgo(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_state_gen != 1);
}

void CMainFrame::OnButtonRsettings()
{
	LoadSettings();
}

void CMainFrame::OnButtonSettingsEdit()
{
	::ShellExecute(GetDesktopWindow()->m_hWnd, "open", "configs\\settings.pl", NULL, NULL, SW_SHOWNORMAL);
}

void CMainFrame::OnUpdateButtonEcsv(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_state_gen == 2);
}

void CMainFrame::OnButtonEcsv()
{
	if (m_state_gen == 2 && pGen && m_fname != "") {
		pGen->ExportVectorsCSV(m_dir, m_fname);
	}
}

void CMainFrame::OnButtonOpencsv() {
	::ShellExecute(GetDesktopWindow()->m_hWnd, "open", m_dir + "\\" + m_fname + ".csv", NULL, NULL, SW_SHOWNORMAL);
}

void CMainFrame::OnUpdateButtonOpencsv(CCmdUI *pCmdUI) {
	pCmdUI->Enable(m_state_gen == 2 && CGLib::fileExists(m_dir + "\\" + m_fname + ".csv"));
}

void CMainFrame::OnButtonReloadSettings() {
	LoadSettings();
}

void CMainFrame::OnUpdateButtonReloadSettings(CCmdUI *pCmdUI) {
	pCmdUI->Enable();
}

void CMainFrame::OnCheckTempo() {
	show_tempo = !show_tempo;
	SaveSettings();
	GetActiveView()->Invalidate();
}

void CMainFrame::OnUpdateCheckTempo(CCmdUI *pCmdUI) {
	pCmdUI->SetCheck(show_tempo);
}

void CMainFrame::OnCheckLines() {
	show_lines = !show_lines;
	SaveSettings();
	GetActiveView()->Invalidate();
}

void CMainFrame::OnUpdateCheckLines(CCmdUI *pCmdUI) {
	pCmdUI->SetCheck(show_lines);
}

void CMainFrame::OnCheckMarks() {
	show_marks = !show_marks;
	SaveSettings();
	GetActiveView()->Invalidate();
}

void CMainFrame::OnUpdateCheckMarks(CCmdUI *pCmdUI) {
	pCmdUI->SetCheck(show_marks);
}

void CMainFrame::OnCheckComments() {
	show_comments = !show_comments;
	SaveSettings();
	GetActiveView()->Invalidate();
}

void CMainFrame::OnUpdateCheckComments(CCmdUI *pCmdUI) {
	pCmdUI->SetCheck(show_comments);
}

void CMainFrame::OnCheckLining() {
	show_lining = !show_lining;
	CGLib::show_lining = show_lining;
	SaveSettings();
	GetActiveView()->Invalidate();
}

void CMainFrame::OnUpdateCheckLining(CCmdUI *pCmdUI) {
	pCmdUI->SetCheck(show_lining);
}

void CMainFrame::OnCheckVel() {
	show_vel = !show_vel;
	SaveSettings();
	GetActiveView()->Invalidate();
}

void CMainFrame::OnCheckCurve() {
	show_curve = !show_curve;
	SaveSettings();
	GetActiveView()->Invalidate();
}

void CMainFrame::OnUpdateCheckCurve(CCmdUI *pCmdUI) {
	pCmdUI->SetCheck(show_curve);
}

void CMainFrame::OnCheckNsr() {
	show_nsr = !show_nsr;
	SaveSettings();
	GetActiveView()->Invalidate();
}

void CMainFrame::OnUpdateCheckNsr(CCmdUI *pCmdUI) {
	pCmdUI->SetCheck(show_nsr);
}

void CMainFrame::OnUpdateCheckVel(CCmdUI *pCmdUI) {
	pCmdUI->SetCheck(show_vel);
}

void CMainFrame::OnCheckNcolors() {
	show_notecolors = !show_notecolors;
	SaveSettings();
	GetActiveView()->Invalidate();
}

void CMainFrame::OnUpdateCheckNcolors(CCmdUI *pCmdUI) {
	pCmdUI->SetCheck(show_notecolors);
}

void CMainFrame::OnButtonErules()
{
	::ShellExecute(GetDesktopWindow()->m_hWnd, "open", "configs\\rules\\rules.xlsm", NULL, NULL, SW_SHOWNORMAL);
}

void CMainFrame::OnButtonSstatus2()
{
	CString st;
	CMFCRibbonEdit* pEdit;
	pEdit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_OINFO));
	st += "1. " + pEdit->GetEditText() + "\n";
	pEdit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_OINFO2));
	st += "2. " + pEdit->GetEditText() + "\n";
	pEdit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_OINFO3));
	st += "3. " + pEdit->GetEditText() + "\n";
	pEdit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_OINFO4));
	st += "4. " + pEdit->GetEditText() + "\n";
	pEdit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_OINFO5));
	st += "5. " + pEdit->GetEditText() + "\n";
	pEdit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_OINFO6));
	st += "6. " + pEdit->GetEditText() + "\n";
	pEdit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_OINFO7));
	st += "7. " + pEdit->GetEditText() + "\n";
	pEdit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_OINFO8));
	st += "8. " + pEdit->GetEditText() + "\n";
	pEdit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_OINFO9));
	st += "9. " + pEdit->GetEditText() + "\n";
	CMsgDlg dlg;
	dlg.m_text = st;
	dlg.DoModal();
}

void CMainFrame::OnUpdateButtonFolder(CCmdUI *pCmdUI) {
	pCmdUI->Enable(!m_fname.IsEmpty());
}

void CMainFrame::OnButtonFolder() {
	if (!m_fname.IsEmpty()) 
		::ShellExecute(GetDesktopWindow()->m_hWnd, "open", m_dir, NULL, NULL, SW_SHOWNORMAL);
}

void CMainFrame::OnUpdateButtonLy(CCmdUI *pCmdUI) {
	if (m_state_gen == 2 && pGen && m_fname != "" && pGen->ly_saved) {
		pCmdUI->Enable(1);
	}
	else pCmdUI->Enable(0);
}

void CMainFrame::OnButtonLy() {
	//::ShellExecute(GetDesktopWindow()->m_hWnd, "open", m_dir + "\\" + m_fname + ".ly", NULL, NULL, SW_SHOWNORMAL);
	if (m_state_gen == 2 && pGen && m_fname != "" && pGen->ly_saved) {
		CString par = m_lilypond_param;

		par.Replace("%1", m_dir + "\\" + m_fname + ".ly");
		int ret = CGLib::RunBackground(m_lilypond_path, par, 200, SW_SHOWNORMAL);
		if (ret) {
			CString est;
			est.Format("Error during executing lilypond %s %s: %d", m_lilypond_path, par, ret);
			WriteLog(5, est);
		}
	}
}

void CMainFrame::OnUpdateButtonPdf(CCmdUI *pCmdUI) {
	pCmdUI->Enable(!m_fname.IsEmpty() && CGLib::fileExists(m_dir + "\\" + m_fname + ".pdf"));
}

void CMainFrame::OnButtonPdf() {
	::ShellExecute(GetDesktopWindow()->m_hWnd, "open", 
		m_dir + "\\" + m_fname + ".pdf",
		NULL, NULL, SW_SHOWNORMAL);
}

void CMainFrame::OnCheckGraph1() {
	show_graph[0] = !show_graph[0];
	SaveSettings();
	GetActiveView()->Invalidate();
}

void CMainFrame::OnUpdateCheckGraph1(CCmdUI *pCmdUI) {
	pCmdUI->SetCheck(show_graph[0]);
	if (pGen && pGen->graph_name.size()) {
		UpdateUIText(pCmdUI, 4, pGen->graph_name[0]);
	}
	else UpdateUIText(pCmdUI, 4, "Unnamed graph #1");
}

void CMainFrame::UpdateUIText(CCmdUI *pCmdUI, int UI_id, CString st) {
	if (cur_ui_text[UI_id] != st) {
		pCmdUI->SetText(st);
		cur_ui_text[UI_id] = st;
		m_wndRibbonBar.ForceRecalcLayout();
	}
}

void CMainFrame::OnCheckGraph2() {
	show_graph[1] = !show_graph[1];
	SaveSettings();
	GetActiveView()->Invalidate();
}

void CMainFrame::OnUpdateCheckGraph2(CCmdUI *pCmdUI) {
	pCmdUI->SetCheck(show_graph[1]);
	if (pGen && pGen->graph_name.size() > 1) {
		UpdateUIText(pCmdUI, 5, pGen->graph_name[1]);
	}
	else UpdateUIText(pCmdUI, 5, "Unnamed graph #2");
}

void CMainFrame::OnCheckGraph3() {
	show_graph[2] = !show_graph[2];
	SaveSettings();
	GetActiveView()->Invalidate();
}

void CMainFrame::OnUpdateCheckGraph3(CCmdUI *pCmdUI) {
	pCmdUI->SetCheck(show_graph[2]);
	if (pGen && pGen->graph_name.size() > 2) {
		UpdateUIText(pCmdUI, 6, pGen->graph_name[2]);
	}
	else UpdateUIText(pCmdUI, 6, "Unnamed graph #3");
}

void CMainFrame::OnCheckProgress() {
	show_progress = !show_progress;
	SaveSettings();
	GetActiveView()->Invalidate();
}

void CMainFrame::OnUpdateCheckProgress(CCmdUI *pCmdUI) {
	pCmdUI->SetCheck(show_progress);
}

void CMainFrame::OnCheckShift() {
	show_shift = !show_shift;
	SaveSettings();
	GetActiveView()->Invalidate();
}

void CMainFrame::OnUpdateCheckShift(CCmdUI *pCmdUI) {
	pCmdUI->SetCheck(show_shift);
}

void CMainFrame::OnCheckDebugexpect() {
	ly_debugexpect = !ly_debugexpect;
	SaveSettings();
}

void CMainFrame::OnUpdateCheckDebugexpect(CCmdUI *pCmdUI) {
	pCmdUI->SetCheck(ly_debugexpect);
}

void CMainFrame::OnButtonExpAccept() {
	CreateDirectory("db\\archive", NULL);
	CGLib::copy_file("db\\expect.csv",
		"db\\archive\\expect-" + CTime::GetCurrentTime().Format("%Y-%m-%d_%H-%M-%S") + ".csv");
	CConf::ReplaceCsvDb(m_dir + "\\edb-" + m_fname + ".csv", "db\\expect.csv",
		"File", pGen->midi_file);
	WriteLog(1, "All expected flags accepted in expect.csv database for " + pGen->midi_file);
}

void CMainFrame::OnUpdateButtonExpAccept(CCmdUI *pCmdUI) {
	pCmdUI->Enable(m_state_gen == 2 && pGen && (m_algo_id == 111 || m_algo_id == 112) &&
		!m_fname.IsEmpty() && !pGen->midi_file.IsEmpty() && 
		CGLib::fileExists(m_dir + "\\edb-" + m_fname + ".csv"));
}

void CMainFrame::OnCheckVib() {
	show_vib = !show_vib;
	SaveSettings();
	GetActiveView()->Invalidate();
}

void CMainFrame::OnUpdateCheckVib(CCmdUI *pCmdUI) {
	pCmdUI->SetCheck(show_vib);
}

void CMainFrame::OnCheckVibf() {
	show_vibf = !show_vibf;
	SaveSettings();
	GetActiveView()->Invalidate();
}

void CMainFrame::OnUpdateCheckVibf(CCmdUI *pCmdUI) {
	pCmdUI->SetCheck(show_vibf);
}

