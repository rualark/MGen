#pragma once
#include "GenCA1.h"

// Movement types
#define mStay 0
#define mParallel 1
#define mDirect 2
#define mOblique 3
#define mContrary 4

// Rhythm position types
#define pDownbeat 1
#define pLeap 2
#define pOffbeat 3
#define pLong 4
#define pAux -1
#define pPass -2

// Patterns
#define pCam 1 // Cambiata
#define pDNT 2 // Double-neighbour tone
#define pPDD 3 // Passing downbeat dissonance

class CGenCP1 :
	public CGenCA1
{
public:
	CGenCP1();
	~CGenCP1();

	void Generate() override;

protected:
	void LoadConfigLine(CString * sN, CString * sV, int idata, float fdata);
	int InitCP();
	void MakeNewCP();
	void SingleCPInit();
	void MultiCPInit();
	inline void ReseedCP();
	inline int FailAlteredInt2(int i, int c1, int c2, int flag);
	inline int FailAlteredInt();
	inline int FailCrossInt2(int i, int i_1, int c1, int c2, int flag);
	inline int FailCrossInt();
	inline void GetVIntervals();
	inline int FailVMotion();
	inline int FailSusResolution(int s3);
	inline int FailSus1();
	inline int FailSus2();
	inline int FailSus();
	inline int FailDisSus();
	inline int FailUnison();
	inline int FailDis();
	inline int FailPcoSus();
	inline int FailPco();
	inline int SkipSus(int notes);
	inline int DetectPDD();
	inline void DetectDNT();
	inline void DetectCambiata();
	inline void SavePattern(int pattern);
	inline int DetectPatterns();
	inline void GetBasicRpos();
	inline void SetRpos(int ls, vector<int> &l_rpos, int val);
	inline void ApplyPDD(int ls, vector<int> &l_rpos, int state);
	inline void ApplyDNT(int ls, vector<int> &l_rpos, int state);
	inline void ApplyCam(int ls, vector<int> &l_rpos, int state);
	inline void ApplyCam2(int ls, vector<int> &l_rpos, int state);
	inline void ApplyFixedPat();
	inline int FailRhythm();
	inline int FailRhythm2();
	inline int FailRhythm4();
	inline int FailRhythm3();
	inline int FailRhythm5();
	inline int FailPcoApart();
	inline int FailPcoApartStep();
	inline int FailVIntervals();
	inline void SaveCP();
	inline void ShowBestRejectedCP();
	inline void SaveCPIfRp();
	inline int FailSlurs();
	void ScanCPInit();
	inline void SendRpos(int pos, int i, int v, int av, int x);
	void CalcPmap2();
	void GetPmap2();
	CString GetPmapLogHeader2();
	CString GetPmapLogSt2();
	void LogPmap2();
	inline void SendHarmColorCP(int pos, int v, int chm_id);
	inline int SendCP();
	inline int FailMissSlurs();
	inline int FailCPInterval();
	inline int FailOverlap();
	void RandomSWACP();
	void ShowLiningCP(vector<int>& cc);
	void SWACP(int i, int dp);
	inline int FailFirstIntervals();
	inline int FailLastIntervals();
	inline void GetNoteTypes();
	inline void GetMeasures();
	inline int FailStartPause();
	void EmulateSASCP();
	inline void GetCfli();
	inline int FailGisTrail2();
	inline int EvalHarm();
	inline int FailTonicCP();
	inline void RemoveHarmDuplicate();
	inline int FailHarm();
	inline void GetHarmBass();
	void ScanCP(int t, int v);

	void LoadCantusHigh();
	void LoadSpecies();

	void ShrinkCantus();

	// Variables
	int cp_culm = 0; // Position of counterpoint culmination
	int warn_wrong_fn = 0; // If warning of wrong fn has fired

	// Cantus
	int cf_nmin = 0; // Minimum note in cantus (chromatic)
	int cf_nmax = 0; // Maximum note in cantus (chromatic)

	// Analysis
	vector <vector<int>> scpoint; // [v][s] Source cpoint for processing

	// PcoApart
	int pco5_last; // Last note start
	int pco8_last;
	int pco5_last2; // Last note end
	int pco8_last2;
	int mli5_last; // Last note measure
	int mli8_last;
};
