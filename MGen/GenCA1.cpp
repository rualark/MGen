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
	CheckVar(sN, sV, "fullscan_max", &fullscan_max);
	CheckVar(sN, sV, "approximations", &approximations);

	CGenCF1::LoadConfigLine(sN, sV, idata, fdata);
}

// Get ids of N lowest penalties in INV to OUTV
// N must be lower or equal to INV.size()
void CGenCA1::GetMinIDs(vector <double> & inv, int n, vector <long> & outv)
{
	priority_queue<pair<double, int>> q;
	double v, min_v = MAX_PENALTY;
	outv.clear();
	// First n values always go to sorting
	for (int i = 0; i < n; ++i) {
		v = inv[i];
		q.push(pair<double, int>(v, i));
		if (v < min_v) min_v = v;
	}
	// Next values go only if they are low
	for (int i = n; i < inv.size(); ++i) if (inv[v] <= min_v) {
		q.push(pair<double, int>(inv[i], i));
		min_v = inv[i];
	}
	for (int i = 0; i < n; ++i) {
		outv.push_back(q.top().second);
		q.pop();
	}
}

void CGenCA1::Generate()
{
	CString st, st2;
	vector <double> dpenalty; // Penalty in terms of difference from user melody
	vector<char> c; // Local cantus
	int ccount = 0;
	vector <long> cids;
	double dpenalty_min;
	InitCantus();
	LoadCantus(midi_file);
	if (cantus.size() < 1) return;
	// Transpose corrected voice up for display
	show_transpose[1] = correct_transpose;
	int t_generated2 = 0; // Saved t_generated
	for (int i = 0; i < cantus.size(); i++) {
		if (need_exit) break;
		milliseconds time_start = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
		milliseconds time_stop;
		// Add line
		linecolor[step] = Color(255, 0, 0, 0);
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
		est->Format("Scan matrix for cantus %d created with %d noted: %s", i+1, smatrixc, st2);
		WriteLog(3, est);
		// Count additional variables
		CountOff(step, step + c_len);
		CountTime(step, step + c_len);
		UpdateNoteMinMax(step, step + c_len);
		UpdateTempoMinMax(step, step + c_len);
		
		// Tree Approximation
		if (c_len - 2 > fullscan_max) {
			// Save source rpenalty
			double rpenalty_source = rpenalty_cur;
			// Save cantus only if its penalty is less or equal to source rpenalty
			rpenalty_min = rpenalty_cur;
			dpenalty_min = MAX_PENALTY;
			cc = cantus[i];
			for (int a = 0; a < approximations; a++) {
				// Save previous minimum penalty
				int rpenalty_min_old = rpenalty_min;
				int dpenalty_min_old = dpenalty_min;
				// Clear before scan
				clib.clear();
				rpenalty.clear();
				dpenalty_min = MAX_PENALTY;
				// Add current cantus if this is not first run
				if (a > 0) {
					clib.push_back(cc);
					rpenalty.push_back(rpenalty_min_old);
				}
				// Sliding Windows Approximation
				ScanCantus(&cc, 2, 0);
				long cnum = clib.size();
				if (cnum == 0) break;
				// Count dpenalty for results, where rpenalty is minimal
				dpenalty.resize(cnum);
				for (int x = 0; x < cnum; x++) if (rpenalty[x] <= rpenalty_min) {
					dpenalty[x] = 0;
					for (int z = 0; z < c_len; z++) {
						int dif = abs(cantus[i][z] - clib[x][z]);
						if (dif) dpenalty[x] += step_penalty + pitch_penalty * dif;
					}
					if (dpenalty[x] < dpenalty_min) dpenalty_min = dpenalty[x];
					st.Format("rp %.0f, dp %0.f: ", rpenalty[x], dpenalty[x]);
					AppendLineToFile("temp.log", st);
					LogCantus(clib[x]);
				}
				// Get all best corrections
				cids.clear();
				for (int x = 0; x < cnum; x++) if (rpenalty[x] <= rpenalty_min && dpenalty[x] == dpenalty_min) {
					cids.push_back(x);
				}
				// Get random cid
				int cid = randbw(0, cids.size() - 1);
				// Get random cantus to continue
				cc = clib[cids[cid]];
				// Send log
				CString* est = new CString;
				est->Format("SWA #%d: rp %.0f from %.0f, dp %.0f, cnum %ld", a, rpenalty_min, rpenalty_source, dpenalty_min, cnum);
				WriteLog(3, est);
				// Abort SWA if dpenalty and rpenalty not decreasing
				if (rpenalty_min >= rpenalty_min_old && dpenalty_min >= dpenalty_min_old) break;
			}
		}
		else {
			clib.clear();
			rpenalty.clear();
			rpenalty_min = 0;
			// Full scan marked notes
			ScanCantus(&(cantus[i]), 1, 0);
			rpenalty_min = 0;
		}
		// Check if we have results
		if (clib.size()) {
			// Count penalty
			long cnum = clib.size();
			dpenalty.resize(cnum);
			for (int x = 0; x < cnum; x++) {
				dpenalty[x] = 0;
				for (int z = 0; z < c_len; z++) {
					int dif = abs(cantus[i][z] - clib[x][z]);
					if (dif) dpenalty[x] += step_penalty + pitch_penalty * dif;
				}
			}
			// Find minimum penalty
			ccount = 0;
			// Cycle through all best matches
			st2 = "";
			for (int p = 0; p < corrections; p++) {
				// Find minimum penalty
				cids.clear();
				double dpenalty_min = MAX_PENALTY;
				for (int x = 0; x < cnum; x++) if (dpenalty[x] < dpenalty_min) dpenalty_min = dpenalty[x];
				// Get all best corrections
				for (int x = 0; x < cnum; x++) if (dpenalty[x] == dpenalty_min) {
					cids.push_back(x);
				}
				if (!cids.size()) break;
				// Shuffle cids
				unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
				::shuffle(cids.begin(), cids.end(), default_random_engine(seed));
				for (int x = 0; x < cids.size(); x++) {
					ccount++;
					if (ccount > corrections) break;
					// Write log
					st.Format("%.0f/%d ", dpenalty_min, cids.size());
					st2 += st;
					// Clear penalty
					dpenalty[cids[x]] = MAX_PENALTY;
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
					if (step < 0) break;
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
			time_stop = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
			// Send log
			CString* est = new CString;
			est->Format("Sent corrections in %d ms with penalties/variants: %s", time_stop - time_start, st2);
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
