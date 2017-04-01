#pragma once

#include "GMidi.h"
class CGenMP1 :
	public CGMidi
{
public:
	CGenMP1();
	~CGenMP1();
	void Generate() override;
	void LoadConfigLine(CString * sN, CString * sV, int idata, double fdata);

	CString midi_file;
	int adapt_enable = 1;
};
