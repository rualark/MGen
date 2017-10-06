#pragma once
#include "GenCA1.h"

// Movement types
#define mStay 0
#define mOblique 1
#define mDirect 2
#define mContrary 3

// Rhythm position types
#define pDownbeat 1
#define pLeap 2
#define pOffbeat 3
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
	inline int FailSus();
	inline int FailDisSus();
	inline int FailUnison();
	inline int FailDis();
	inline int FailPcoSus();
	inline int FailPco();
	inline int SkipSus(int notes);
	inline void DetectPDD();
	inline void DetectDNT();
	inline void DetectCambiata();
	inline void SavePattern(int pattern);
	inline void DetectPatterns();
	inline void GetBasicRpos();
	inline void SetRpos(int ls, vector<int> &l_rpos, int val);
	inline void ApplyPDD(int ls, vector<int> &l_rpos, int state);
	inline void ApplyDNT(int ls, vector<int> &l_rpos, int state);
	inline void ApplyCam(int ls, vector<int> &l_rpos, int state);
	inline void ApplyCam2(int ls, vector<int> &l_rpos, int state);
	inline void ApplyFixedPat();
	inline int FailRhythm3();
	inline int FailRhythm5();
	inline int FailPcoApart();
	inline int FailVIntervals();
	inline void SaveCP();
	inline void ShowBestRejectedCP();
	inline void SaveCPIfRp();
	inline int FailSlurs();
	void ScanCPInit();
	inline void SendRpos(int pos, int i, int v, int av, int x);
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
	inline int FailHarm();
	void ScanCP(int t, int v);

	void LoadCantusHigh();
	void LoadSpecies();

	void ShrinkCantus();

	// Variables
	int cp_culm = 0; // Position of counterpoint culmination
	int warn_wrong_fn = 0; // If warning of wrong fn has fired

	vector<int> ivl; // [s] Diatonic interval between voices
	vector<int> civl; // [s] Chromatic interval between voices
	vector<int> ivlc; // [s] Diatonic interval between voices (class)
	vector<int> civlc; // [s] Chromatic interval between voices (class)
	vector<int> tivl; // [s] Type of interval between voices
	vector<int> motion; // [s] Melody motion type
	vector<int> beat; // [ls] Beat type for each fli2: 0 = downbeat, 1 = beat 3
	vector<int> sus; // [ls] Note suspension flag (when above zero, links to first cantus-changing step)
	vector<int> mli; // [ms] Forward links to first steps of each measure
	vector<int> bmli; // [s] Backward links to measures from steps
	vector<int> cfli; // [cfs] Forward links to each cf note
	vector<int> hli; // Forward links to first notes of each harmonic change
	vector<int> rposb; // [ls] Rhythm position types for fli (basic without patterns)
	vector<int> rposf; // [ls] Rhythm position types for fli (with fixed patterns)
	vector<int> rpos; // [ls] Rhythm position types for fli
	vector<int> pat; // [ls] Pattern (cambiata, dnt...) for fli
	vector<int> pat_state; // [ls] Pattern (cambiata, dnt...) for fli state: 0 - not applied, 1 - fixed, 2,3 - variants
	int hli_size = 0; // Size of hli vector

	// Cantus
	int cf_nmin = 0; // Minimum note in cantus (chromatic)
	int cf_nmax = 0; // Maximum note in cantus (chromatic)

	// Analysis
	vector <vector<int>> scpoint; // [v][s] Source cpoint for processing

};
