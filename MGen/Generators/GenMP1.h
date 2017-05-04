#pragma once

#include "../GLibrary/GMidi.h"
class CGenMP1 :
	public CGMidi
{
public:
	CGenMP1();
	~CGenMP1();
	void Generate() override;
	void LoadConfigLine(CString * sN, CString * sV, int idata, float fdata);

	CString midi_file;
};
