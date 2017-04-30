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
		st.Format("Step: %d (measure %d)\nVoice: %d (%s, channel %d, type %d)\n", ms, ms / 8, mv,
			pGen->InstGName[pGen->instr[mv]] + "/" + pGen->InstCName[pGen->instr[mv]], pGen->instr_channel[pGen->instr[mv]], pGen->instr_type[pGen->instr[mv]]);
		m_info.AddText(st, RGB(0, 0, 0), CFE_BOLD);
		st.Format("Note: %s (midi %d)\nKey: %s\nNote length: %d\n",
			CGLib::GetNoteName(pGen->note[ms][mv]), pGen->note[ms][mv], NoteName[pGen->tonic[ms][mv]], pGen->len[ms][mv]);
		m_info.AddText(st, RGB(0, 0, 0), CFE_BOLD);
		if (pGen->midifile_loaded) {
			st.Format("From MIDI file: track %d \"%s\" voice %d, channel %d, delta %d ms (%.0f%% croche)\n", pGen->track_id[mv], pGen->track_name[mv], pGen->track_vid[mv], pGen->midi_ch[ms][mv], pGen->midi_delta[ms][mv], (float)(pGen->midi_delta[ms][mv])/300.0*(float)(pGen->tempo[ms]));
			m_info.AddText(st, RGB(0, 0, 200), CFE_BOLD);
		}
		st.Format("Current offset: %d steps\nPrevious offset: %d steps\nNext offset: %d steps\nDynamics: %d\n", pGen->coff[ms][mv], pGen->poff[ms][mv], pGen->noff[ms][mv], pGen->dyn[ms][mv]);
		m_info.AddText(st, RGB(0, 0, 0), 0);
		st.Format("Tempo: %.1f bpm (randomized %.1f bpm)\nStep start time: %.3f s (%.2f ms long), end time %.3f\nPause indicator: %d\nComment: %s\nAdapt comment: %s\n", 
			pGen->tempo[ms], pGen->tempo_src[ms], pGen->stime[ms] / pGen->m_pspeed / 10,
			(pGen->etime[ms] - pGen->stime[ms]) * 100 / pGen->m_pspeed, pGen->etime[ms] / pGen->m_pspeed / 10, pGen->pause[ms][mv],
			pGen->comment[ms][mv], pGen->adapt_comment[ms][mv]);
		m_info.AddText(st, RGB(0, 0, 0), 0);
		st.Format("Playback step start delta: %.3f s (with delta %.3f s)\nPlayback step ending delta: %.3f s (with delta %.3f s)\n", 
			pGen->dstime[ms][mv]/1000.0, pGen->stime[ms] / pGen->m_pspeed / 10 + pGen->dstime[ms][mv]/1000.0,
			pGen->detime[ms][mv]/1000.0, pGen->etime[ms] / pGen->m_pspeed / 10 + pGen->detime[ms][mv]/1000.0);
		m_info.AddText(st, RGB(0, 170, 0), 0);
		st.Format("Instrument library: %s\n", pGen->instr_lib[pGen->instr[mv]]);
		m_info.AddText(st, RGB(0, 170, 0), 0);
		if (pGen->artic[ms][mv] == ARTIC_LEGATO) st2 = "Legato";
		if (pGen->artic[ms][mv] == ARTIC_EXACT) st2 = "Exact";
		if (pGen->artic[ms][mv] == ARTIC_SLUR) st2 = "Slur";
		if (pGen->artic[ms][mv] == ARTIC_NONLEGATO) st2 = "Nonlegato";
		if (pGen->artic[ms][mv] == ARTIC_STACCATO) st2 = "Staccato";
		if (pGen->artic[ms][mv] == ARTIC_REBOW) st2 = "Retrigger rebow";
		if (pGen->artic[ms][mv] == ARTIC_RETRIGGER) st2 = "Retrigger";
		if (pGen->artic[ms][mv] == ARTIC_SPLITPO_CHROM) st2 = "Split portamento chromatic";
		if (pGen->artic[ms][mv] == ARTIC_SPLITPO_MIX) st2 = "Split portamento mixed";
		if (pGen->artic[ms][mv] == ARTIC_SPLITPO_ARAB) st2 = "Split portamento arabic";
		if (pGen->artic[ms][mv] == ARTIC_SPLITPO_PENT) st2 = "Split portamento pentatonic";
		if (pGen->artic[ms][mv] == ARTIC_GLISS) st2 = "Gliss";
		if (pGen->artic[ms][mv] == ARTIC_GLISS2) st2 = "Gliss2";
		if (pGen->artic[ms][mv] == ARTIC_END_SFL) st2 = "Short fall ending";
		if (pGen->artic[ms][mv] == ARTIC_END_PBD) st2 = "Pitchbend down ending";
		if (pGen->artic[ms][mv] == ARTIC_END_VIB2) st2 = "Vibrato2 ending";
		if (pGen->artic[ms][mv] == ARTIC_END_VIB) st2 = "Vibrato ending";
		st.Format("Velocity: %d\nVibrato intensity: %d\nVibrato speed: %d\nArticulation: %s\nLengroup: %d\n", 
			pGen->vel[ms][mv], pGen->vib[ms][mv], pGen->vibf[ms][mv], st2, pGen->lengroup[ms][mv]);
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
