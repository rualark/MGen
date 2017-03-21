#pragma once
#include "GenTemplate.h"
class CGenCF2 :
	public CGenTemplate
{
public:
	CGenCF2();
	~CGenCF2();
	void Generate() override;
	void LoadConfigLine(CString * sN, CString * sV, int idata, double fdata);
};

