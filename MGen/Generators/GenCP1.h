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
	void MultiCPInit();
	void ReseedCP();
	inline int FailVIntervals();
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

	// Parameters
	int cantus_high; // Set to 1 to generate counterpoint under cantus

};