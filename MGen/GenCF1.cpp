#include "stdafx.h"
#include "GenCF1.h"

CGenCF1::CGenCF1()
{
	c_len = 9;
	max_interval = 5;
	midifile_tpq_mul = 8;
	sleep_ms = 0;
}

CGenCF1::~CGenCF1()
{
}

void CGenCF1::Generate()
{
	vector<int> c(c_len), leap(c_len), smooth(c_len);
	vector<int> nstat(max_interval * 2 + 1);
	vector<int> nstat2(max_interval * 2 + 1);
	// Set first and last notes
	c[0] = 0;
	c[c_len-1] = 0;
	// Set middle notes
	for (int i = 1; i < c_len-1; i++) c[i] = -max_interval;
	// Walk all variants
	int p = c_len - 2; // Minimal position in array to cycle
	long cycle = 0;
	long accepted = 0;
	int finished = 0;
	int nmin, nmax, good, leap_sum, culm_sum, smooth_sum, smooth_sum2;
	int step = 0; // Global step
	while (true) {
		if (need_exit) return;
		if (cycle >= 2143582) { // Debug condition
			// Analyze combination
			nmin = 0;
			nmax = 0;
			good = 1;
			for (int i = 0; i < c_len - 1; i++) {
				// Local note repeat prohibited
				if (c[i] == c[i + 1]) {
					good = 0;
					break;
				}
				// Sept prohibit
				if ((!allow_sept) && (abs(c[i + 1] - c[i]) == 6)) {
					good = 0;
					break;
				}
				// Tritone prohibit
				if (!allow_tritone) if (((c[i + 1] == -1) && (c[i] == 3)) ||
					((c[i + 1] == 3) && (c[i] == -1)) ||
					((c[i + 1] == -4) && (c[i] == -1)) ||
					((c[i + 1] == -1) && (c[i] == -4))) {
					good = 0;
					break;
				}
				// Find all leaps
				leap[i] = 0;
				smooth[i] = 0;
				if (c[i + 1] - c[i] > 1) leap[i] = 1;
				else if (c[i + 1] - c[i] < -1) leap[i] = -1;
				// Find all smooth
				else if (c[i + 1] - c[i] == 1) smooth[i] = 1;
				else if (c[i + 1] - c[i] == -1) smooth[i] = -1;
			}
			leap_sum = 0;
			smooth_sum = 0;
			smooth_sum2 = 0;
			if (good) for (int i = 0; i < c_len - 1; i++) {
				// Add new leap
				if (leap[i] != 0) leap_sum++;
				// Subtract old leap
				if ((i >= max_leap_steps) && (leap[i - max_leap_steps] != 0)) leap_sum--;
				// Check if too many leaps
				if (leap_sum > max_leaps) {
					good = 0;
					break;
				}
				// Prohibit long smooth movement
				if (smooth[i] != 0) smooth_sum++;
				else smooth_sum = 0;
				if (smooth_sum >= max_smooth) {
					good = 0;
					break;
				}
				if (i < c_len - 2) {
					// Prohibit long smooth movement in one direction
					if (smooth[i] == smooth[i + 1]) smooth_sum2++;
					else smooth_sum2 = 0;
					if (smooth_sum2 >= max_smooth_direct - 1) {
						good = 0;
						break;
					}
					// Check if leaps follow each other in same direction
					if (leap[i] * leap[i + 1] > 0) {
						good = 0;
						break;
					}
					// Check if melody direction changes after leap
					if (leap[i] * (c[i + 2] - c[i + 1]) > 0) {
						if ((allow_leap_second_release) && (i < c_len - 3)) {
							if (leap[i] * (c[i + 3] - c[i + 2]) > 0) {
								good = 0;
								break;
							}
						}
						else {
							good = 0;
							break;
						}
					}
					// Check if leap returns to same note
					if ((leap[i] != 0) && (leap[i + 1] != 0) && (c[i] == c[i + 2])) {
						good = 0;
						break;
					}
					// Check if two notes repeat
					if ((!allow_repeat2) && (i > 0) && (c[i] == c[i + 2]) && (c[i - 1] == c[i + 1])) {
						good = 0;
						break;
					}
				}
			}
			// Clear nstat
			if (good) for (int i = 0; i <= max_interval * 2; i++) {
				nstat[i] = 0;
				nstat2[i] = 0;
			}
			if (good) for (int i = 0; i < c_len; i++) {
				// Prohibit stagnation
				// Add new note
				nstat[c[i] + max_interval]++;
				nstat2[c[i] + max_interval]++;
				// Subtract old note
				if ((i >= stag_note_steps)) nstat[c[i - stag_note_steps] + max_interval]--;
				// Check if too many repeating notes
				if (nstat[c[i] + max_interval] > stag_notes) {
					good = 0;
					break;
				}
				// Count limits
				if (c[i] < nmin) nmin = c[i];
				if (c[i] > nmax) nmax = c[i];
			}
			// Check note fill
			if ((!allow_unfilled_leaps) && (good)) for (int i = nmin; i <= nmax; i++) {
				if (nstat2[i + max_interval] == 0) {
					good = 0;
					break;
				}
			}
			// Prohibit multiple culminations
			culm_sum = 0;
			if (good) for (int i = 0; i < c_len; i++) {
				if (c[i] == nmax) culm_sum++;
				if (culm_sum > 1) {
					good = 0;
					break;
				}
			}
			// Limit melody interval
			if (nmax - nmin > max_interval) good = 0;
			if (good) {
				accepted++;
				if (accepted < 1000000) {
					Sleep(sleep_ms);
					//Color ccolor = Color(randbw(50, 200), randbw(0, 180), randbw(0, 180), randbw(0, 180));
					Color ccolor = Color(0, randbw(0, 180), randbw(0, 180), randbw(0, 180));
					// Copy cantus to output
					if (step + c_len >= t_allocated) ResizeVectors(t_allocated * 2);
					comment[step][0].Format("c%ld a%ld", cycle, accepted);
					for (int x = step; x < step + c_len; x++) {
						note[x][0] = dia_to_chrom[(c[x - step] + 56) % 7] + (c[x - step] / 7) * 12 + first_note; // Negative eight octaves reserve
						color[x][0] = ccolor;
						if (c[x - step] < 0) note[x][0] -= 12; // Correct negative octaves
						len[x][0] = 1;
						pause[x][0] = 0;
						tempo[x] = 200;
						coff[x][0] = 0;
						att[x][0] = 80 + 30 * rand2() / RAND_MAX;
						if (x == 0) {
							tempo[x] = min_tempo + (double)(max_tempo - min_tempo) * (double)rand2() / (double)RAND_MAX;
						}
						else {
							tempo[x] = tempo[x - 1] + randbw(-1, 1);
							if (tempo[x] > max_tempo) tempo[x] = 2 * max_tempo - tempo[x];
							if (tempo[x] < min_tempo) tempo[x] = 2 * min_tempo - tempo[x];
						}
					}
					// Create pause
					step += c_len;
					note[step][0] = 0;
					len[step][0] = 1;
					pause[step][0] = 1;
					att[step][0] = 0;
					tempo[step] = tempo[step - 1];
					coff[step][0] = 0;
					step++;
					// Count additional variables
					CountOff(step - c_len - 1, step - 1);
					CountTime(step - c_len - 1, step - 1);
					UpdateNoteMinMax(step - c_len - 1, step - 1);
					UpdateTempoMinMax(step - c_len - 1, step - 1);
					// Send
					t_generated = step;
					t_sent = t_generated;
					::PostMessage(m_hWnd, WM_GEN_FINISH, 1, 0);
				}
			}
		}
		while (true) {
			if (c[p] < max_interval) break;
			// If current element is max, make it minimum
			c[p] = -max_interval;
			// Move left one element
			if (p == 1) {
				finished = 1;
				break;
			}
			p--;
		}
		if (finished) break;
		// Increase rightmost element, which was not reset to minimum
		c[p]++;
		// Go to rightmost element
		p = c_len - 2;
		cycle++;
		//if (cycle > 100) break;
	}
	CString* est = new CString;
	est->Format("Accepted %.8f%% (%ld) variants of %ld", 100.0*(double)accepted/(double)cycle, accepted, cycle);
	WriteLog(1, est);
}

void CGenCF1::LoadConfigLine(CString* sN, CString* sV, int idata, double fdata)
{
}
