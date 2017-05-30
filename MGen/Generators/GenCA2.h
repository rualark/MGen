#pragma once
#include "GenCP1.h"
class CGenCA2 :
	public CGenCP1
{
public:
	CGenCA2();
	~CGenCA2();

	void Generate() override;

protected:
	void LoadConfigLine(CString * sN, CString * sV, int idata, float fdata);
};
