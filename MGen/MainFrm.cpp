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

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TIMER1 1

// CMainFrame

static UINT WM_GEN_FINISH = RegisterWindowMessage("MGEN_GEN_FINISH_MSG");
static UINT WM_DEBUG_MSG = RegisterWindowMessage("MGEN_DEBUG_MSG");

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
	ON_REGISTERED_MESSAGE(WM_DEBUG_MSG, &CMainFrame::OnDebugMsg)
	ON_COMMAND(ID_BUTTON_STOPGEN, &CMainFrame::OnButtonStopgen)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_STOPGEN, &CMainFrame::OnUpdateButtonStopgen)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_GEN, &CMainFrame::OnUpdateButtonGen)
	ON_WM_CLOSE()
	ON_COMMAND(ID_BUTTON_HZOOM_DEC, &CMainFrame::OnButtonHzoomDec)
	ON_COMMAND(ID_BUTTON_HZOOM_INC, &CMainFrame::OnButtonHzoomInc)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_HZOOM_DEC, &CMainFrame::OnUpdateButtonHzoomDec)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_HZOOM_INC, &CMainFrame::OnUpdateButtonHzoomInc)
	ON_UPDATE_COMMAND_UI(ID_COMBO_MIDIOUT, &CMainFrame::OnUpdateComboMidiout)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_EPARAMS, &CMainFrame::OnUpdateButtonEparams)
	ON_COMMAND(ID_BUTTON_EPARAMS, &CMainFrame::OnButtonEparams)
END_MESSAGE_MAP()

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
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

	m_wndRibbonBar.Create(this);
	m_wndRibbonBar.LoadFromResource(IDR_RIBBON);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	CString strTitlePane1;
	CString strTitlePane2;
	bNameValid = strTitlePane1.LoadString(IDS_STATUS_PANE1);
	ASSERT(bNameValid);
	bNameValid = strTitlePane2.LoadString(IDS_STATUS_PANE2);
	ASSERT(bNameValid);
	CMFCRibbonStatusBarPane* pPane = new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE1, strTitlePane1, TRUE);
	pPane->SetAlmostLargeText(_T("Connecting Connecting Connecting Connecting Connecting Connecting"));
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

	// Algorithm combo
	CMFCRibbonComboBox *pCombo = DYNAMIC_DOWNCAST(CMFCRibbonComboBox,
		m_wndRibbonBar.FindByID(ID_COMBO_ALGO));
	for (int i = 1; i <= GAlgNum; i++) {
		//pCombo->AddItem(GAlgName[i]);
		//TCHAR st[100];
		//_stprintf_s(st, _T("%d"), i);
		//pCombo->AddItem(st);
		pCombo->AddItem(GAlgName[i]);
	}

	// MIDI port
	pCombo = DYNAMIC_DOWNCAST(CMFCRibbonComboBox,	m_wndRibbonBar.FindByID(ID_COMBO_MIDIOUT));
	CString st;
	int default_out = Pm_GetDefaultOutputDeviceID();
	for (int i = 0; i < Pm_CountDevices(); i++) {
		const PmDeviceInfo *info = Pm_GetDeviceInfo(i);
		if (info->output) {
			st.Format("%s, %s [%d]", info->name, info->interf, i);
			if (i == default_out) {
				st += " (default)";
			}
		}
		pCombo->AddItem(st, i);
	}

	WriteLog(0, "Started MGen version 1.1.5");
	AfxInitRichEdit2();

	return 0;
}

void CMainFrame::WriteLog(int log, CString st)
{
	COutputList* pOL=0;
	if (log == 0) pOL = &m_wndOutput.m_wndOutputDebug;
	if (log == 1) pOL = &m_wndOutput.m_wndOutputWarn;
	if (log == 2) pOL = &m_wndOutput.m_wndOutputPerf;
	if (log == 3) pOL = &m_wndOutput.m_wndOutputAlgo;
	if (log == 4) pOL = &m_wndOutput.m_wndOutputMidi;
	pOL->AddString(CTime::GetCurrentTime().Format("%H:%M:%S") + " " + st);
	if (pOL->GetCount() > 1000) pOL->DeleteString(0);
	pOL->SetTopIndex(pOL->GetCount() - 1);
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

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

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
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
}


void CMainFrame::OnButtonGen()
{
	if (m_state_gen == 1) {
		WriteLog(1, "Cannot start generation: generation in progress");
		pGen->need_exit = 1;
		return;
	}
	if (m_state_gen == 2) {
		WriteLog(0, "Starting generation: Removing previous generator");
		delete pGen;
		m_state_gen = 0;
	}
	pGen = 0;
	int Algo = GetAlgo();
	if (Algo == 1) {
		pGen = new CGenCF1();
	}
	if (Algo == 2) {
		pGen = new CGenCF2();
	}
	if (pGen != 0) {
		WriteLog(0, _T("Started generator: ") + GAlgName[Algo]);
		pGen->m_hWnd = m_hWnd;
		pGen->WM_GEN_FINISH = WM_GEN_FINISH;
		pGen->WM_DEBUG_MSG = WM_DEBUG_MSG;
		pGen->time_started = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
		pGen->StartMIDI(GetMidiI(), 100);
		m_GenThread = AfxBeginThread(CMainFrame::GenThread, pGen);
		m_state_gen = 1;
		// Start timer
		m_nTimerID = SetTimer(TIMER1, 100, NULL);
	}
}

void CMainFrame::OnButtonStopgen()
{
	if (m_state_gen == 1) {
		if (pGen != 0) {
			pGen->need_exit = 1;
			WriteLog(0, "Sent need_exit to generation thread");
		}
	}
}

void CMainFrame::OnButtonPlay()
{
	// TODO: Add your command handler code here
}



void CMainFrame::OnButtonSettings()
{
	// TODO: Add your command handler code here
}


void CMainFrame::OnButtonAlgo()
{
	//map <string, int> p;
	//p["param1"] = 100;
	//p["param2"] = 200;
	//TCHAR st[100];
	//_stprintf_s(st, _T("%d"), p["param1"] + p["param2"]);
	//WriteLog(0, st);
}


void CMainFrame::OnCheckOutputwnd()
{
	if (m_wndOutput.IsVisible()) m_wndOutput.ShowPane(FALSE, FALSE, FALSE);
	else m_wndOutput.ShowPane(TRUE, FALSE, TRUE);
}

LRESULT CMainFrame::OnGenFinish(WPARAM wParam, LPARAM lParam)
{
	if (wParam == 0) {
		GetActiveView()->Invalidate();
		WriteLog(0, "Generation finished");
		::KillTimer(m_hWnd, TIMER1);
		m_state_gen = 2;
	}
	if (wParam == 1) {
		//if (pGen->midi_sent > 0) return 0;
		pGen->SendMIDI(pGen->midi_sent, pGen->t_sent);
	}
	return 0;
}

LRESULT CMainFrame::OnDebugMsg(WPARAM wParam, LPARAM lParam)
{
	CString* pSt = (CString*)lParam;
	WriteLog(wParam, *pSt);
	delete pSt;
	return LRESULT();
}

void CMainFrame::OnUpdateCheckOutputwnd(CCmdUI *pCmdUI)
{
	BOOL bEnable = m_wndOutput.IsVisible();
	pCmdUI->Enable();
	pCmdUI->SetCheck(bEnable);
}


void CMainFrame::OnUpdateComboAlgo(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
}

int CMainFrame::GetAlgo()
{
	CMFCRibbonComboBox *pCombo = DYNAMIC_DOWNCAST(CMFCRibbonComboBox,
		m_wndRibbonBar.FindByID(ID_COMBO_ALGO));
	return pCombo->GetCurSel() + 1;
}

int CMainFrame::GetMidiI()
{
	CMFCRibbonComboBox *pCombo = DYNAMIC_DOWNCAST(CMFCRibbonComboBox,
		m_wndRibbonBar.FindByID(ID_COMBO_MIDIOUT));
	return pCombo->GetItemData(pCombo->GetCurSel());
}

void CMainFrame::OnComboAlgo()
{
	// TODO: Add your command handler code here
}

void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == m_nTimerID)
	{
		GetActiveView()->Invalidate();
	}
	CFrameWndEx::OnTimer(nIDEvent);
}

UINT CMainFrame::GenThread(LPVOID pParam)
{
	CGenTemplate* pGen = (CGenTemplate*)pParam;

	if (pGen == NULL) return 1;   // if Object is not valid  

	//::PostMessage(pGen->m_hWnd, WM_DEBUG_MSG, 0, (LPARAM)new CString("Thread started"));
	pGen->Generate();
	//Sleep(2000);
	::PostMessage(pGen->m_hWnd, WM_GEN_FINISH, 0, 0);
	pGen->time_stopped = duration_cast< milliseconds >(system_clock::now().time_since_epoch());

	//::PostMessage(pGen->m_hWnd, WM_DEBUG_MSG, 0, (LPARAM)new CString("Thread stopped"));
	return 0;   // thread completed successfully 
}



void CMainFrame::OnUpdateButtonStopgen(CCmdUI *pCmdUI)
{
	BOOL bEnable = m_state_gen == 1;
	pCmdUI->Enable(bEnable);
}


void CMainFrame::OnUpdateButtonGen(CCmdUI *pCmdUI)
{
	BOOL bEnable = m_state_gen != 1;
	pCmdUI->Enable(bEnable);
}


void CMainFrame::OnClose()
{
	if (m_state_gen == 1) {
		OnButtonStopgen();
		WaitForSingleObject(m_GenThread->m_hThread, 10000);
		delete pGen;
		pGen = 0;
	}
	if (pGen != 0) {
		delete pGen;
		pGen = 0;
	}
	Pm_Terminate();

	CFrameWndEx::OnClose();
}


void CMainFrame::OnButtonHzoomDec()
{
	zoom_x = (int)(zoom_x*0.8);
	if (zoom_x < MIN_HZOOM) zoom_x = MIN_HZOOM;
	CString st;
	st.Format("New zoom %d", zoom_x);
	WriteLog(2, st);
	GetActiveView()->Invalidate();
}


void CMainFrame::OnButtonHzoomInc()
{
	zoom_x = (int)(zoom_x*1.2);
	if (zoom_x > MAX_HZOOM) zoom_x = MAX_HZOOM;
	CString st;
	st.Format("New zoom %d", zoom_x);
	WriteLog(2, st);
	GetActiveView()->Invalidate();
}


void CMainFrame::OnUpdateButtonHzoomDec(CCmdUI *pCmdUI)
{
	BOOL bEnable = zoom_x > MIN_HZOOM;
	pCmdUI->Enable(bEnable);
}


void CMainFrame::OnUpdateButtonHzoomInc(CCmdUI *pCmdUI)
{
	BOOL bEnable = zoom_x < MAX_HZOOM;
	pCmdUI->Enable(bEnable);
}


void CMainFrame::OnUpdateComboMidiout(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
}


void CMainFrame::OnUpdateButtonEparams(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetAlgo() != 0);
}


void CMainFrame::OnButtonEparams()
{
	CEditParamsDlg dlg;
	dlg.DoModal();
}
