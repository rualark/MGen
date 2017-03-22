#include "stdafx.h"
#include "GenCF1.h"

#define MAX_FLAGS 13

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
	vector<int> c(c_len); // cantus (diatonic)
	vector<int> cc(c_len); // cantus (chromatic)
	vector<int> leap(c_len);
	vector<int> smooth(c_len);
	vector<int> nstat(max_interval * 2 + 1);
	vector<int> nstat2(max_interval * 2 + 1);
	vector<long> fstat(MAX_FLAGS); // number of canti with each flag
	string flags; // Flags for whole cantus
	vector< vector <char> > nflags = vector<vector<char>>(c_len, vector<char>(MAX_FLAGS)); // Flags for each note
	vector<char> nflagsc(c_len); // number of flags for each note
	// Set first and last notes
	c[0] = 0;
	c[c_len-1] = last_diatonic_int;
	// Set middle notes
	for (int i = 1; i < c_len-1; i++) c[i] = -max_interval;
	// Walk all variants
	int p = c_len - 2; // Minimal position in array to cycle
	long cycle = 0;
	long accepted = 0;
	int finished = 0;
	int nmin, nmax, leap_sum, culm_sum, smooth_sum, smooth_sum2;
	int step = 0; // Global step
	while (true) {
		if (need_exit) return;
		// Analyze combination
		if (cycle >= 0) { // Debug condition
    	// Local note repeat prohibited
			for (int i = 0; i < c_len - 1; i++) {
				if (c[i] == c[i + 1]) goto skip;
			}
			nmin = 0;
			nmax = 0;
			// Count limits
			for (int i = 0; i < c_len; i++) {
				if (c[i] < nmin) nmin = c[i];
				if (c[i] > nmax) nmax = c[i];
			}
			// Limit melody interval
			if (nmax - nmin > max_interval) goto skip;
			// Clear flags
			// 0  S - strict
			// 1  p - sept
			// 2  t - tritone
			// 3  j - too many leaps
			// 4  o - too long smooth movement
			// 5  l - too long smooth movement in one direction (linear)
			// 6  c - chain of leaps in one direction
			// 7  a - leap is resolved after a second note
			// 8  r - leap returns to same note
			// 9  d - two notes repeat in contact
			// 10 g - stagnation on one note
			// 11 f - leap is not filled
			// 12 m - multiple culminations

			// Unskippable rules:
			// Total interval
			// Note repeats note of previous measure
			// Tritone is incorrectly resolved
			// After leap two next notes move same direction
			flags = "Sptjolcardgfm";
			for (int i = 0; i < c_len; i++) {
				nflagsc[i] = 0;
				// Calculate chromatic positions
				cc[i] = dia_to_chrom[(c[i] + 56) % 7] + (c[i] / 7) * 12 + first_note; // Negative eight octaves reserve
				if (c[i] < 0) cc[i] -= 12; // Correct negative octaves
			}
			for (int i = 0; i < c_len - 1; i++) {
				// Tritone prohibit
				if (abs(cc[i+1] - cc[i]) == 6) {
				  // Check if tritone is first or last step
					if (i > c_len - 3) goto skip;
					if (i < 1) goto skip;
					// Check if resolution is correct
					if ((cc[i] % 12 == 11) && (cc[i - 1] % 12 != 0)) goto skip;
					if ((cc[i + 1] % 12 == 11) && (cc[i + 2] % 12 != 0)) goto skip;
					if ((cc[i] % 12 == 5) && (cc[i - 1] % 12 != 6)) goto skip;
					if ((cc[i + 1] % 12 == 5) && (cc[i + 2] % 12 != 6)) goto skip;
					// Record tritone
					flags[0] = 's';
					flags[2] = 'T';
					nflags[i][nflagsc[i]] = 'T';
					nflagsc[i]++;
				}
				// Sept prohibit
				if (abs(c[i + 1] - c[i]) == 6) {
					flags[0] = 's';
					flags[1] = 'P';
					nflags[i][nflagsc[i]] = 'P';
					nflagsc[i]++;
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
			for (int i = 0; i < c_len - 1; i++) {
				// Add new leap
				if (leap[i] != 0) leap_sum++;
				// Subtract old leap
				if ((i >= max_leap_steps) && (leap[i - max_leap_steps] != 0)) leap_sum--;
				// Check if too many leaps
				if (leap_sum > max_leaps) {
					flags[0] = 's';
					flags[3] = 'J';
					nflags[i][nflagsc[i]] = 'J';
					nflagsc[i]++;
				}
				// Prohibit long smooth movement
				if (smooth[i] != 0) smooth_sum++;
				else smooth_sum = 0;
				if (smooth_sum >= max_smooth) {
					flags[0] = 's';
					flags[4] = 'O';
					nflags[i][nflagsc[i]] = 'O';
					nflagsc[i]++;
				}
				if (i < c_len - 2) {
					// Prohibit long smooth movement in one direction
					if (smooth[i] == smooth[i + 1]) smooth_sum2++;
					else smooth_sum2 = 0;
					if (smooth_sum2 >= max_smooth_direct - 1) {
						flags[0] = 's';
						flags[5] = 'L';
						nflags[i][nflagsc[i]] = 'L';
						nflagsc[i]++;
					}
					// Check if leaps follow each other in same direction
					if (leap[i] * leap[i + 1] > 0) {
						flags[0] = 's';
						flags[6] = 'C';
						nflags[i][nflagsc[i]] = 'C';
						nflagsc[i]++;
					}
					// Check if melody direction changes after leap
					if (leap[i] * (c[i + 2] - c[i + 1]) > 0) {
						if (i < c_len - 3) {
							if (leap[i] * (c[i + 3] - c[i + 2]) > 0) goto skip;
							flags[0] = 's';
							flags[7] = 'A';
							nflags[i][nflagsc[i]] = 'A';
							nflagsc[i]++;
						}
						else goto skip;
					}
					// Check if leap returns to same note
					if ((leap[i] != 0) && (leap[i + 1] != 0) && (c[i] == c[i + 2])) {
						flags[0] = 's';
						flags[8] = 'R';
						nflags[i][nflagsc[i]] = 'R';
						nflagsc[i]++;
					}
					// Check if two notes repeat
					if ((i > 0) && (c[i] == c[i + 2]) && (c[i - 1] == c[i + 1])) {
						flags[0] = 's';
						flags[9] = 'D';
						nflags[i][nflagsc[i]] = 'D';
						nflagsc[i]++;
					}
				}
			}
			// Clear nstat
			for (int i = 0; i <= max_interval * 2; i++) {
				nstat[i] = 0;
				nstat2[i] = 0;
			}
			for (int i = 0; i < c_len; i++) {
				// Prohibit stagnation
				// Add new note
				nstat[c[i] + max_interval]++;
				nstat2[c[i] + max_interval]++;
				// Subtract old note
				if ((i >= stag_note_steps)) nstat[c[i - stag_note_steps] + max_interval]--;
				// Check if too many repeating notes
				if (nstat[c[i] + max_interval] > stag_notes) {
					flags[0] = 's';
					flags[10] = 'G';
					nflags[i][nflagsc[i]] = 'G';
					nflagsc[i]++;
				}
			}
			// Check note fill
			for (int i = nmin; i <= nmax; i++) {
				if (nstat2[i + max_interval] == 0) {
					flags[0] = 's';
					flags[11] = 'F';
					break;
				}
			}
			// Prohibit multiple culminations
			culm_sum = 0;
			for (int i = 0; i < c_len; i++) {
				if (c[i] == nmax) culm_sum++;
				if (culm_sum > 1) {
					flags[0] = 's';
					flags[12] = 'M';
					nflags[i][nflagsc[i]] = 'M';
					nflagsc[i]++;
				}
			}
			// Calculate flag statistics
			for (int i = 0; i < MAX_FLAGS; i++) {
				if (flags[i] <= 'Z') fstat[i]++;
			}
			// Check if flags are accepted
			for (int i = 0; i < MAX_FLAGS; i++) {
				if ((flags[i] <= 'Z') && (accept[i] > 'Z')) goto skip;
			}
			// Accept cantus
			accepted++;
			Sleep(sleep_ms);
			Color ccolor = Color(0, randbw(0, 180), randbw(0, 180), randbw(0, 180));
			// Copy cantus to output
			if (step + c_len >= t_allocated) ResizeVectors(t_allocated * 2);
			comment[step][0].Format("c%ld a%ld", cycle, accepted);
			for (int x = step; x < step + c_len; x++) {
				note[x][0] = dia_to_chrom[(c[x - step] + 56) % 7] + (c[x - step] / 7) * 12 + first_note; // Negative eight octaves reserve
				if (c[x - step] < 0) note[x][0] -= 12; // Correct negative octaves
				color[x][0] = ccolor;
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
		skip:
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
