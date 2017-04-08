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
	CheckVar(sN, sV, "corrections", &corrections);
	CheckVar(sN, sV, "pre_bad", &pre_bad);
	CheckVar(sN, sV, "post_bad", &post_bad);
	CheckVar(sN, sV, "step_penalty", &step_penalty);
	CheckVar(sN, sV, "pitch_penalty", &pitch_penalty);
	CheckVar(sN, sV, "correct_transpose", &correct_transpose);

	CGenCF1::LoadConfigLine(sN, sV, idata, fdata);
}

void CGenCA1::Generate()
{
	CString st, st2;
	vector <double> cpenalty; // Penalty in terms of difference from user melody
	InitCantus();
	LoadCantus(midi_file);
	if (cantus.size() < 1) return;
	// Transpose corrected voice up for display
	show_transpose[1] = correct_transpose;
	int t_generated2 = 0; // Saved t_generated
	for (int i = 0; i < cantus.size(); i++) {
		clib.clear();
		if (need_exit) break;
		// Add line
		linecolor[step] = Color(255, 0, 0, 0);
		// This flag is needed to prevent flag skipping
		skip_flags2 = 1;
		// Show imported melody
		ScanCantus(&(cantus[i]), 0, 0);
		// Check if cantus was shown
		if (t_generated2 == t_generated) continue;
		t_generated2 = t_generated;
		// If no corrections needed
		if (!corrections) {
			v_cnt = 1;
			// Go forward
			Adapt(step - c_len - 1, step - 1);
			t_generated = step;
			t_sent = t_generated;
			continue;
		}
		step -= c_len + 1;
		// Fill pauses if no results generated
		for (int x = step; x <= step + c_len; x++) {
			pause[x][1] = 1;
			note[x][1] = 0;
			len[x][1] = c_len + 1;
			coff[x][1] = x - step;
		}
		// Clear scan matrix
		smatrixc = 0;
		smatrix.resize(c_len);
		fill(smatrix.begin(), smatrix.end(), 0);
		// Search each note
		for (int x = 0; x < c_len; x++) {
			// Search each flag
			if (nflagsc[x] > 0) for (int f = 0; f < nflagsc[x]; f++) {
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
		st2 = "";
		for (int x = 0; x < c_len; x++) {
			st.Format("%d ", smatrix[x]);
			st2 += st;
		}
		CString* est = new CString;
		est->Format("Scan matrix created: %s", st2);
		WriteLog(3, est);
		// Count additional variables
		CountOff(step, step + c_len);
		CountTime(step, step + c_len);
		UpdateNoteMinMax(step, step + c_len);
		UpdateTempoMinMax(step, step + c_len);
		// Here we can skip flags if no calculations specified
		skip_flags2 = 0;
		// Full scan marked notes
		ScanCantus(&(cantus[i]), 1, 0);
		// Check if we have results
		if (clib.size()) {
			// Count penalty
			int cnum = clib.size();
			cpenalty.resize(cnum);
			for (int x = 0; x < cnum; x++) {
				cpenalty[x] = 0;
				for (int z = 0; z < c_len; z++) {
					int dif = abs(cantus[i][z] - clib[x][z]);
					if (dif) cpenalty[x] += step_penalty + pitch_penalty * dif;
				}
			}
			// Find minimum penalty
			int ccount = 0;
			vector <long> cids;
			// Cycle through all best matches
			st2 = "";
			for (int p = 0; p < corrections; p++) {
				// Find minimum penalty
				cids.clear();
				double min_penalty = MAX_PENALTY;
				for (int x = 0; x < cnum; x++) if (cpenalty[x] < min_penalty) min_penalty = cpenalty[x];
				// Get all best corrections
				for (int x = 0; x < cnum; x++) if (cpenalty[x] == min_penalty) {
					cids.push_back(x);
				}
				// Shuffle cids
				unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
				::shuffle(cids.begin(), cids.end(), default_random_engine(seed));
				for (int x = 0; x < cids.size(); x++) {
					ccount++;
					if (ccount > corrections) break;
					// Write log
					st.Format("%.0f/%d ", min_penalty, cids.size());
					st2 += st;
					// Clear penalty
					cpenalty[cids[x]] = MAX_PENALTY;
					// This flag is needed to prevent flag skipping
					skip_flags2 = 1;
					// Show initial melody again if this is not first iteration
					if (ccount > 1) {
						ScanCantus(&(cantus[i]), 0, 0);
						step -= c_len + 1;
					}
					// Get cantus
					cc = clib[cids[x]];
					// Show result
					ScanCantus(&(cc), 0, 1);
					// Go back
					step -= c_len + 1;
					// Add lining
					for (int z = 0; z < c_len; z++) {
						if (cantus[i][z] != clib[cids[x]][z]) {
							lining[step + z][0] = 1;
						}
					}
					// Go forward
					step += c_len + 1;
					Adapt(step - c_len - 1, step - 1);
					t_generated = step;
					t_sent = t_generated;
				}
			}
			// Send log
			CString* est = new CString;
			est->Format("Sent corrections with penalties/variants: %s", st2);
			WriteLog(3, est);
		}
		else {
			// Go forward
			step += c_len + 1;
			Adapt(step - c_len - 1, step - 1);
			t_generated = step;
			t_sent = t_generated;
		}
	}
}
