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
	inline int FailVIntervals();
	inline void CalcDpenaltyCP();
	inline void SaveCP();
	inline void SaveCPIfRp();
	void ScanCPInit();
	inline int SendCP();
	inline int FailSlurs(vector<int>& cc, int step1, int step2);
	inline int FailCPInterval();
	inline int FailOverlap();
	void RandomSWACP();
	void SWACP(int i, int dp);
	inline int FailLastIntervals();
	inline void GetNoteTypes();
	inline void GetMeasures();
	inline int FailGisTrail2();
	inline int FailHarm();
	void ScanCP(int t, int v);

	// Variables
	int cp_culm = 0; // Position of counterpoint culmination

	vector<int> ivl; // Diatonic interval between voices
	vector<int> civl; // Chromatic interval between voices
	vector<int> ivlc; // Diatonic interval between voices (class)
	vector<int> civlc; // Chromatic interval between voices (class)
	vector<int> tivl; // Type of interval between voices
	vector<int> motion; // Melody motion type
	vector<int> beat; // Beat type for each fli2
	vector<int> sus; // Note suspension flag (when above zero, links to first cantus-changing step)
	vector<int> mli; // Forward links to first notes of each measure
	vector<int> hli; // Forward links to first notes of each harmonic change
	vector<int> rpos; // Rhythm position types for fli
	int hli_size = 0; // Size of hli vector

	// Cantus
	int cf_nmin = 0; // Minimum note in cantus (chromatic)
	int cf_nmax = 0; // Maximum note in cantus (chromatic)

	// Analysis
	vector <vector<int>> scpoint; // Source cpoint for processing

};
