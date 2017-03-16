#include "stdafx.h"
#include "GenTemplate.h"


CGenTemplate::CGenTemplate()
{
}


CGenTemplate::~CGenTemplate()
{
}

void CGenTemplate::Generate()
{
}

void CGenTemplate::Init()
{
	pause = vector<vector<unsigned char>>(t_init, vector<unsigned char>(v_cnt));
	note = vector<vector<unsigned char>>(t_init, vector<unsigned char>(v_cnt));
	len = vector<vector<unsigned char>>(t_init, vector<unsigned char>(v_cnt));
	coff = vector<vector<unsigned char>>(t_init, vector<unsigned char>(v_cnt));
	poff = vector<vector<unsigned char>>(t_init, vector<unsigned char>(v_cnt));
	noff = vector<vector<unsigned char>>(t_init, vector<unsigned char>(v_cnt));
	tempo = vector<vector<unsigned char>>(t_init, vector<unsigned char>(v_cnt));
	att = vector<vector<unsigned char>>(t_init, vector<unsigned char>(v_cnt));
}
