#pragma once
#include "GenCA1.h"

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
	void ScanCP(int t, int v);

	// Counterpoint
	vector<vector<int>> ac; // Diatonic
	vector<vector<int>> acc; // Chromatic
	vector<vector<int>> acc_old; // Chromatic
	vector<vector<int>> apc; // Pitch class (diatonic)
	vector<vector<int>> apcc; // Pitch class (chromatic)
	vector<vector<int>> aleap;
	vector<vector<int>> asmooth;

	vector<int> ivl; // Diatonic interval between voices
	vector<int> civl; // Chromatic interval between voices
	vector<int> ivlc; // Diatonic interval between voices (class)
	vector<int> civlc; // Chromatic interval between voices (class)
	vector<int> tivl; // Type of interval between voices
	vector<int> direct; // Above zero if voices move in same direction

	// Analysis
	vector <vector<int>> scpoint; // Source cpoint for processing

	// Parameters
	int cantus_high = 0; // Set to 1 to generate counterpoint under cantus
	int cantus_id2 = 0; // Select particular cantus id. Set to -1 to select random
};