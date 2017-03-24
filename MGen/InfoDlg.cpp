// InfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MGen.h"
#include "InfoDlg.h"
#include "afxdialogex.h"
#include "MainFrm.h"
#include "GenTemplate.h"
#include "MGenView.h"

// CInfoDlg dialog

IMPLEMENT_DYNAMIC(CInfoDlg, CDialog)

CInfoDlg::CInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DIALOG_INFO, pParent)
{

}

CInfoDlg::~CInfoDlg()
{
}

void CInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RICHEDIT21, m_info);
}


BEGIN_MESSAGE_MAP(CInfoDlg, CDialog)
END_MESSAGE_MAP()


// CInfoDlg message handlers


BOOL CInfoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CMainFrame *mf = (CMainFrame *)AfxGetMainWnd();
	CGenTemplate *pGen = mf->pGen;
	if (pGen != 0) {
		if (!pGen->mutex_output.try_lock_for(chrono::milliseconds(1000))) {
			mf->WriteLog(1, "InfoDlg mutex timed out: showing nothing");
			return TRUE;
		}
		int ms = ((CMGenView*)(mf->GetActiveView()))->mouse_step;
		int mv = ((CMGenView*)(mf->GetActiveView()))->mouse_voice;
		//SetWindowText("Dialog: some text");
		CString st;
		st.Format("Step: %d (measure %d)\nVoice: %d\nNote: %s (midi %d)\nNote length: %d\n", ms, ms/8, mv, 
			CGenTemplate::GetNoteName(pGen->note[ms][mv]), pGen->note[ms][mv], pGen->len[ms][mv]);
		m_info.AddText(st, RGB(0, 0, 0), CFE_BOLD);
		st.Format("Current offset: %d steps\nPrevious offset: %d steps\nNext offset: %d steps\nAttack: %d\n", pGen->coff[ms][mv], pGen->poff[ms][mv], pGen->noff[ms][mv], pGen->dyn[ms][mv]);
		m_info.AddText(st, RGB(0, 0, 0), 0);
		st.Format("Tempo: %.1f bpm\nStep start time: %.4f s (%.2f ms long)\nPause indicator: %d\nComment: %s\n", pGen->tempo[ms], pGen->stime[ms] / 1000,
			pGen->etime[ms] - pGen->stime[ms], pGen->pause[ms][mv], pGen->comment[ms][mv]);
		m_info.AddText(st, RGB(0, 0, 0), 0);
		st.Format("Playback start delta: %.1f ms\nPlayback ending delta: %.1f ms", pGen->dstime[ms], pGen->detime[ms]);
		m_info.AddText(st, RGB(0, 170, 0), 0);
		//AddText("Some text text text\r\n", RGB(250, 100, 0), CFE_BOLD | CFE_ITALIC | CFE_STRIKEOUT | CFE_UNDERLINE);
		m_info.SetSel(0, 0);
		pGen->mutex_output.unlock();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
								// EXCEPTION: OCX Property Pages should return FALSE
}
