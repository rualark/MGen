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
	inline int FailSus();
	inline int FailDisSus();
	inline int FailUnison();
	inline int FailDis();
	inline int FailPcoSus();
	inline int FailPco();
	inline void GetRpos();
	inline int FailRhythm3();
	inline int FailRhythm5();
	inline int FailPcoApart();
	inline int FailVIntervals();
	inline void SaveCP();
	inline void ShowBestRejectedCP();
	inline void SaveCPIfRp();
	inline int FailSlurs();
	void ScanCPInit();
	inline int SendCP();
	inline int FailMissSlurs();
	inline int FailCPInterval();
	inline int FailOverlap();
	void RandomSWACP();
	void ShowLiningCP(vector<int>& cc);
	void SWACP(int i, int dp);
	inline int FailLastIntervals();
	inline void GetNoteTypes();
	inline void GetMeasures();
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

	vector<int> ivl; // [s] Diatonic interval between voices
	vector<int> civl; // [s] Chromatic interval between voices
	vector<int> ivlc; // [s] Diatonic interval between voices (class)
	vector<int> civlc; // [s] Chromatic interval between voices (class)
	vector<int> tivl; // [s] Type of interval between voices
	vector<int> motion; // [s] Melody motion type
	vector<int> beat; // [ls] Beat type for each fli2
	vector<int> sus; // [ls] Note suspension flag (when above zero, links to first cantus-changing step)
	vector<int> mli; // [ms] Forward links to first notes of each measure
	vector<int> cfli; // [cfs] Forward links to each cf note
	vector<int> hli; // Forward links to first notes of each harmonic change
	vector<int> rpos; // [ls] Rhythm position types for fli
	int hli_size = 0; // Size of hli vector

	// Cantus
	int cf_nmin = 0; // Minimum note in cantus (chromatic)
	int cf_nmax = 0; // Maximum note in cantus (chromatic)

	// Analysis
	vector <vector<int>> scpoint; // [v][s] Source cpoint for processing

};
