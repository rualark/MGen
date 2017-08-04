#pragma once
#include "../GLibrary/GMidi.h"

class CGenRL1 :
	public CGMidi
{
public:
	CGenRL1();
	~CGenRL1();
	void LoadConfigLine(CString * sN, CString * sV, int idata, float fdata);
	void Generate() override;
private:
	int min_dyn = 90;
	int max_dyn = 120;
};
