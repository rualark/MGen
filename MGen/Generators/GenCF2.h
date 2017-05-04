#pragma once
#include "../GLibrary/GMidi.h"

class CGenCF2 :
	public CGMidi
{
public:
	CGenCF2();
	~CGenCF2();
	void Generate() override;
	void LoadConfigLine(CString * sN, CString * sV, int idata, float fdata);
};

