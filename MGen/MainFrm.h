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

#define MIN_HZOOM 80
#define MAX_HZOOM 500

#define MAX_ALGO 100
#define MAX_MIDI_DEVICES 100

class CMainFrame : public CFrameWndEx
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:
	CGenTemplate* pGen = 0;
	int midi_device_i=-1; // Index of selected MIDI device
	CString midi_device_s; // Name of selected MIDI device
	int view_single_track = 1; // If all tracks are shown in one
	int zoom_x = 200; // Zoom for view
	int zoom_y = 200;
	int ng_min = 0;
	int ng_max = 0;
	int tg_min = 0;
	int tg_max = 0;
	int m_state_gen=0;
	// 0 = No music
	// 1 = Generation started
	// 2 = Generation finished
	int m_state_play=0;
	// 0 = No playback
	// 1 = Playback
	// 2 = Playback into buffer finished. Buffer is still playing
	CString AlgName[MAX_ALGO];
	CString AlgGroup[MAX_ALGO];
	CString AlgGroups[MAX_ALGO];
	CString AlgComment[MAX_ALGO];
	CString AlgFolder[MAX_ALGO];
	int AlgID[MAX_ALGO];
	int AlgCount = 0;
	int AlgGCount = 0;
	// MIDI device names
	CString MidiName[MAX_MIDI_DEVICES];
	int MidiCount = 0;

// Operations
public:

	void WriteLog(int log, CString st);

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

	CMFCRibbonStatusBar  m_wndStatusBar;
	CWinThread* m_GenThread;

protected:  // control bar embedded members
	CMFCRibbonBar     m_wndRibbonBar;
	CMFCRibbonApplicationButton m_MainButton;
	CMFCToolBarImages m_PanelImages;
	COutputWnd        m_wndOutput;

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
	afx_msg LRESULT OnWarnMsg(WPARAM wParam, LPARAM lParam);

	int GetAlgo();
	int GetMidiI();
	void LoadAlgo();
	void LoadSettings();
	void SaveSettings();

	static UINT GenThread(LPVOID pParam);
	afx_msg void OnUpdateButtonGen(CCmdUI *pCmdUI);
	afx_msg void OnClose();
	afx_msg void OnButtonHzoomDec();
	afx_msg void OnButtonHzoomInc();
	afx_msg void OnUpdateButtonHzoomDec(CCmdUI *pCmdUI);
	afx_msg void OnUpdateButtonHzoomInc(CCmdUI *pCmdUI);
	afx_msg void OnUpdateComboMidiout(CCmdUI *pCmdUI);
	afx_msg void OnUpdateButtonEparams(CCmdUI *pCmdUI);
	afx_msg void OnButtonEparams();
	afx_msg void OnUpdateButtonPlay(CCmdUI *pCmdUI);
	afx_msg void OnComboMidiout();
};


