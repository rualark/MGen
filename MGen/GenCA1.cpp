#include "stdafx.h"
#include "GenCA1.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CGenCA1::CGenCA1()
{
	v_cnt = 2;
}

CGenCA1::~CGenCA1()
{
}

void CGenCA1::LoadConfigLine(CString* sN, CString* sV, int idata, double fdata)
{
	LoadVar(sN, sV, "midi_file", &midi_file);

	CGenCF1::LoadConfigLine(sN, sV, idata, fdata);
}

void CGenCA1::Generate()
{
	InitCantus();
	LoadCantus(midi_file);
	if (cantus.size() < 1) return;
	// This flag is needed to prevent flag skipping
	calculate_stat = 1;
	for (int i = 0; i < cantus.size(); i++) {
		ScanCantus(&(cantus[i]), 0, 0);
		step -= c_len + 1;
		// Clear scan matrix
		smatrixc = 0;
		smatrix.resize(c_len);
		fill(smatrix.begin(), smatrix.end(), 0);
		// Search each note
		for (int x = 0; x < c_len; x++) {
			// Search each flag
			if (nflagsc[x] > 0) for (int f = 0; f < nflagsc[x]; x++) {
				// Find prohibited flag
				if (accept[nflags[x][f]] == 0) {
					// Create matrix window
					int pos1 = x - pre_bad;
					int pos2 = x + post_bad;
					// Do not rescan first and last step
					if (pos1 < 1) pos1 = 1;
					if (pos2 > c_len - 2) pos2 = c_len - 2;
					// Set smatrix values
					for (int z = pos1; z <= pos2; z++) {
						if (smatrix[z] == 0) smatrixc++;
						smatrix[z] = 1;
					}
				}
			}
		}
		// Full scan marked notes if did not scan this length yet
		if ((clib.size() <= c_len) || (!clib[c_len].size())) ScanCantus(&(cantus[i]), 1, 0);
		// Check if we have results
		if ((clib.size() > c_len) && (clib[c_len].size())) {
			cc = clib[c_len][0];
			// Show result
			ScanCantus(&(cc), 0, 1);
			// Go back
			step -= c_len + 1;
		}
		else {
			// Fill pauses if no results generated
			for (int x = step; x <= step + c_len; x++) {
				pause[x][1] = 1;
				note[x][1] = 0;
				len[x][1] = c_len+1;
				coff[x][1] = x - step;
			}
			// Count additional variables
			CountOff(step, step + c_len);
			CountTime(step, step + c_len);
			UpdateNoteMinMax(step, step + c_len);
			UpdateTempoMinMax(step, step + c_len);
		}
		// Go forward
		step += c_len + 1;
		Adapt(step - c_len - 1, step - 1);
		t_generated = step;
		t_sent = t_generated;
	}
}
