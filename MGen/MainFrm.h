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

// MainFrm.h : interface of the CMainFrame class
//

#pragma once
#include "OutputWnd.h"
#include "GenCF1.h"
#include "GenCF2.h"
#include "GenRS1.h"
#include "GenMP1.h"
#include "GenCA1.h"
#include "GenCP1.h"

#define MIN_HZOOM 1
#define MAX_HZOOM 500
#define MIN_PSPEED 1
#define MAX_PSPEED 5000
#define MIN_VIEW_TIMER 50
#define MAX_VIEW_TIMER 3000

#define MAX_ALGO 100
#define MAX_MIDI_DEVICES 100

class CMainFrame : public CFrameWndEx
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:
	CGMidi* pGen = 0;
	int midi_device_i=-1; // Index of selected MIDI device
	CString midi_device_s; // Name of selected MIDI device
	int view_single_track = 1; // If all tracks are shown in one
	int zoom_x = 200; // Zoom for view
	int zoom_y = 200;
	int ng_min = 0; // Limits for view
	int ng_max = 0;
	int tg_min = 0;
	int tg_max = 0;
	int m_algo = -1; // Current algorithm local ID
	int m_algo_id = -1; // Current algorithm global ID
	int m_view_timer = 100; // Delay between ondraw events in ms
	int m_step_dyn = 1; // Show dynamics with note opacity for each step of note.Disable for slower computers.
	int m_debug_level = 1; // Increase to show more debug logs
	int m_play_enabled = 1; // Disable playback to MIDI port by setting this to 0

	int m_pspeed = 100; // Playback speed in percent
	CString m_config; // Current config
	CString m_fname; // Current saved results filename
	CString m_dir; // Current saved results dir
	//CString midi_program; // Path to MIDI program to use
	int m_state_gen=0;
	// 0 = No music
	// 1 = Generation started
	// 2 = Generation finished
	int m_state_play=0;
	// 0 = No playback
	// 1 = Playback
	// 2 = Playback into buffer finished. Buffer is still playing
	CWinThread* m_GenThread;

	// Algorithm / configs constants
	CString AlgName[MAX_ALGO];
	CString AlgGroup[MAX_ALGO];
	CString AlgInsts[MAX_ALGO];
	CString AlgGroups[MAX_ALGO];
	CString AlgMFIGroups[MAX_ALGO];
	CString AlgComment[MAX_ALGO];
	CString AlgFolder[MAX_ALGO];
	int AlgMFI[MAX_ALGO]; // =1 if this algorithm supports Midi File Import
	int AlgID[MAX_ALGO];
	int ParamCount[MAX_ALGO];
	int AlgCount = 0;
	int AlgGCount = 0;
	int AlgMFIGCount = 0; // Number of groups which support Midi File Import
	vector< vector <CString> > ParamName; // 1 = pause, 0 = note
  // MIDI device names
	CString MidiName[MAX_MIDI_DEVICES];
	int MidiCount = 0;

// Operations
public:
	void ClearLogs();
	void WriteLog(int log, CString st);
	int GetMidiI();
	void LoadAlgo();
	void LoadSettings();
	int GetAlgoById(int id);
	void SaveSettings();
	void LoadResults(CString path);
	void LoadMidi(CString path);
	bool NewDocument();
	static UINT GenThread(LPVOID pParam);
	void StartPlay(int from);

	// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:  // control bar embedded members
	CMFCRibbonBar     m_wndRibbonBar;
	CMFCRibbonApplicationButton m_MainButton;
	CMFCToolBarImages m_PanelImages;
	COutputWnd        m_wndOutput;
	CMFCRibbonStatusBar  m_wndStatusBar;

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg void OnFilePrint();
	afx_msg void OnFilePrintPreview();
	afx_msg void OnUpdateFilePrintPreview(CCmdUI* pCmdUI);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg void ShowOutputWnd();
	DECLARE_MESSAGE_MAP()

	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);

public:
	afx_msg void OnButtonParams();
	afx_msg void OnButtonGen();
	afx_msg void OnButtonPlay();
	afx_msg void OnButtonSettings();
	afx_msg void OnButtonAlgo();
	afx_msg void OnCheckOutputwnd();
	afx_msg void OnUpdateCheckOutputwnd(CCmdUI *pCmdUI);
	afx_msg void OnUpdateComboAlgo(CCmdUI *pCmdUI);
	afx_msg void OnComboAlgo();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg LRESULT OnGenFinish(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDebugMsg(WPARAM wParam, LPARAM lParam);

	afx_msg void OnUpdateButtonGen(CCmdUI *pCmdUI);
	afx_msg void OnClose();
	afx_msg void OnUpdateComboMidiout(CCmdUI *pCmdUI);
	afx_msg void OnUpdateButtonEparams(CCmdUI *pCmdUI);
	afx_msg void OnButtonEparams();
	afx_msg void OnUpdateButtonPlay(CCmdUI *pCmdUI);
	afx_msg void OnComboMidiout();
	afx_msg void OnUpdateButtonAlgo(CCmdUI *pCmdUI);
	afx_msg void OnUpdateButtonParams(CCmdUI *pCmdUI);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	afx_msg void OnUpdateButtonSconfig(CCmdUI *pCmdUI);
	afx_msg void OnButtonSconfig();
	afx_msg void OnUpdateSpinPspeed(CCmdUI *pCmdUI);
	afx_msg void OnSpinPspeed();
	afx_msg void OnSpinZoom();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnUpdateButtonOpenmidi(CCmdUI *pCmdUI);
	afx_msg void OnButtonOpenmidi();
	afx_msg void OnButtonEditalgo();
	afx_msg void OnButtonEditinst();
	afx_msg void OnButtonReloadalgo();
	afx_msg void OnUpdateButtonReloadalgo(CCmdUI *pCmdUI);
	afx_msg void OnButtonRsettings();
	afx_msg void OnButtonSettingsEdit();
	afx_msg void OnUpdateButtonEcsv(CCmdUI *pCmdUI);
	afx_msg void OnButtonEcsv();
};


