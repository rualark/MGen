// InfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MGen.h"
#include "InfoDlg.h"
#include "afxdialogex.h"
#include "MainFrm.h"
#include "GMidi.h"
#include "MGenView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

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
	CGMidi *pGen = mf->pGen;
	if (pGen != 0) {
		if (!pGen->mutex_output.try_lock_for(chrono::milliseconds(1000))) {
			mf->WriteLog(1, "InfoDlg mutex timed out: showing nothing");
			return TRUE;
		}
		int ms = ((CMGenView*)(mf->GetActiveView()))->mouse_step;
		int mv = ((CMGenView*)(mf->GetActiveView()))->mouse_voice;
		//SetWindowText("Dialog: some text");
		CString st, st2;
		st.Format("Step: %d (measure %d)\nVoice: %d (%s, channel %d, type %d) ", ms, ms / 8, mv,
			InstName[pGen->instr[mv]], pGen->instr_channel[pGen->instr[mv]], pGen->instr_type[pGen->instr[mv]]);
		m_info.AddText(st, RGB(0, 0, 0), CFE_BOLD);
		if (pGen->tname[mv] != "") {
			st.Format("\"%s\" in file", pGen->tname[mv]);
			m_info.AddText(st, RGB(0, 0, 0), CFE_BOLD);
		}
		st.Format("\nNote: %s (midi %d)\nKey: %s\nNote length: %d\n",
			CGLib::GetNoteName(pGen->note[ms][mv]), pGen->note[ms][mv], NoteName[pGen->tonic[ms][mv]], pGen->len[ms][mv]);
		m_info.AddText(st, RGB(0, 0, 0), CFE_BOLD);
		st.Format("Current offset: %d steps\nPrevious offset: %d steps\nNext offset: %d steps\nDynamics: %d\n", pGen->coff[ms][mv], pGen->poff[ms][mv], pGen->noff[ms][mv], pGen->dyn[ms][mv]);
		m_info.AddText(st, RGB(0, 0, 0), 0);
		st.Format("Tempo: %.1f bpm\nStep start time: %.4f s (%.2f ms long)\nPause indicator: %d\nComment: %s\nAdapt comment: %s\n", 
			pGen->tempo[ms], pGen->stime[ms] / pGen->m_pspeed / 10,
			(pGen->etime[ms] - pGen->stime[ms]) * 100 / pGen->m_pspeed, pGen->pause[ms][mv], 
			pGen->comment[ms][mv], pGen->adapt_comment[ms][mv]);
		m_info.AddText(st, RGB(0, 0, 0), 0);
		st.Format("Playback step start delta: %.1f ms\nPlayback step ending delta: %.1f ms\n", pGen->dstime[ms][mv], pGen->detime[ms][mv]);
		m_info.AddText(st, RGB(0, 170, 0), 0);
		if (pGen->artic[ms][mv] == ARTIC_LEGATO) st2 = "Legato";
		if (pGen->artic[ms][mv] == ARTIC_EXACT) st2 = "Exact";
		if (pGen->artic[ms][mv] == ARTIC_SLUR) st2 = "Slur";
		if (pGen->artic[ms][mv] == ARTIC_NONLEGATO) st2 = "Nonlegato";
		if (pGen->artic[ms][mv] == ARTIC_STACCATO) st2 = "Staccato";
		if (pGen->artic[ms][mv] == ARTIC_RETRIGGER) st2 = "Retrigger";
		st.Format("Velocity: %d\nArticulation: %s\nLengroup: %d\n", pGen->vel[ms][mv], st2, pGen->lengroup[ms][mv]);
		m_info.AddText(st, RGB(170, 0, 0), 0);
		st.Format("Voice show transpose: %d semitones\n", pGen->show_transpose[mv]);
		m_info.AddText(st, RGB(170, 0, 0), 0);
		//AddText("Some text text text\r\n", RGB(250, 100, 0), CFE_BOLD | CFE_ITALIC | CFE_STRIKEOUT | CFE_UNDERLINE);
		m_info.SetSel(0, 0);
		pGen->mutex_output.unlock();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
								// EXCEPTION: OCX Property Pages should return FALSE
}
