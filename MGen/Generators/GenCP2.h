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

	void InitMeasures();

	void MakeNewCP2();

	void SingleCP2Init();

	void MultiCP2Init();

	void ScanCP2Init();

	int SendCP2();

	void ReseedCP2();

	void ScanCP2(int t, int v);

	int notes_per_measure = 2; // How many notes in counterpoint per each measure

	vector<int> mpos; // Position inside measure
	vector<int> mid; // Measure id
};

