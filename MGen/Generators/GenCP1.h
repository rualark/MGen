#pragma once
#include "GenCA1.h"

// Movement types
#define mStay 0
#define mOblique 1
#define mDirect 2
#define mContrary 3

class CGenCP1 :
	public CGenCA1
{
public:
	CGenCP1();
	~CGenCP1();

	void Generate() override;

protected:
	void LoadConfigLine(CString * sN, CString * sV, int idata, float fdata);
	void InitCP();
	void MakeNewCP();
	void SingleCPInit();
	void MultiCPInit();
	inline void ReseedCP();
	inline int FailVIntervals();
	inline void CalcDpenaltyCP();
	inline void SaveCP();
	inline void SaveCPIfRp();
	void ScanCPInit();
	inline int SendCP();
	inline int FailSlurs(vector<int>& cc, int step1, int step2);
	inline int FailCPInterval(int step1, int step2);
	inline int FailOverlap();
	void ScanCP(int t, int v);

	// Counterpoint
	vector<vector<int>> ac; // Diatonic
	vector<vector<int>> acc; // Chromatic
	vector<vector<int>> acc_old; // Chromatic
	vector<vector<int>> apc; // Pitch class (diatonic)
	vector<vector<int>> apcc; // Pitch class (chromatic)
	vector<vector<int>> aleap;
	vector<vector<int>> asmooth;

	// Variables
	int cp_culm = 0; // Position of counterpoint culmination

	vector<int> ivl; // Diatonic interval between voices
	vector<int> civl; // Chromatic interval between voices
	vector<int> ivlc; // Diatonic interval between voices (class)
	vector<int> civlc; // Chromatic interval between voices (class)
	vector<int> tivl; // Type of interval between voices
	vector<int> motion; // Melody motion type

	// Cantus
	int cf_nmin; // Minimum note in cantus (chromatic)
	int cf_nmax; // Maximum note in cantus (chromatic)

	// Analysis
	vector <vector<int>> scpoint; // Source cpoint for processing

	// Parameters
	int cantus_high = 0; // Set to 1 to generate counterpoint under cantus
	int cantus_id2 = 0; // Select particular cantus id. Set to -1 to select random
	int slurs_window = 10; // Number of steps to check for slur limit
	int min_between = 0; // Minimum diatonic interval between voices
	int max_between = 11; // Maximum diatonic interval between voices
	int sum_interval = 22; // Maximum chromatic range of cantus and counterpoint
	int burst_steps = 3; // Maximum number of steps for which max_between can be exceeded
	int burst_between = 11; // Maximum interval between voices for burst_steps steps
	int tonic_window = 15; // Number of steps to search for adjacent tonic chords
	int contrary_min = 30; // Minimum percent of contrary motion (little)
	int contrary_min2 = 60; // Minimum percent of contrary motion (medium)
};