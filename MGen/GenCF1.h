#pragma once
#include "GenTemplate.h"
class CGenCF1 :
	public CGenTemplate
{
public:
	CGenCF1();
	~CGenCF1();
	void Generate() override;
};
