#pragma once
#include "GMidi.h"

class CGenCP1 :
	public CGMidi
{
public:
	CGenCP1();
	~CGenCP1();

	void Generate() override;

protected:
	void LoadConfigLine(CString * sN, CString * sV, int idata, double fdata);

	int min_dyn = 80;
	int max_dyn = 110;
};