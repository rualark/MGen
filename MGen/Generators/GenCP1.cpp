#include "../stdafx.h"
#include "GenCP1.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CGenCP1::CGenCP1() {
}

CGenCP1::~CGenCP1() {
}

void CGenCP1::LoadConfigLine(CString * sN, CString * sV, int idata, float fdata) {
	CGenCA1::LoadConfigLine(sN, sV, idata, fdata);
}

void CGenCP1::Generate() {
	CString st;
	InitCantus();
	LoadCantus(midi_file);
	if (cantus.size() < 1) return;
	// Choose cantus to use
	cantus_id = randbw(0, cantus.size() - 1);
	// Load first voice
	// Generate second voice
}
