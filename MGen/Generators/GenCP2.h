#pragma once
#include "GenCP1.h"

class CGenCP2 :
	public CGenCP1
{
public:
	CGenCP2();
	~CGenCP2();

	void Generate() override;

protected:
	void LoadConfigLine(CString * sN, CString * sV, int idata, float fdata);

	// CP species 2
	int notes_per_measure = 2; // How many notes in counterpoint per each measure

	vector<int> mpos; // Position inside measure
	vector<int> mid; // Measure id
	vector<int> vsize; // Size of each counterpoint voice
};

