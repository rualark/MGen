#include "stdafx.h"
#include "GenCF1.h"

#define MAX_FLAGS 13


const CString FlagName[] = {
	"Strict", // 0 S
	"Seventh", // 1 p
	"Tritone", // 2 t
	"Many leaps", // 3 j
	"Long smooth", // 4 o
	"Long line", // 5 l
	"Leaps chain", // 6 c
	"Late leap resolution", // 7 a
	"Leap back", // 8 r
	"Close repeat", // 9 d
	"Stagnation", // 10 g
	"Unfilled leap", // 11 f
	"Multiple culminations" // 12 m
};

const Color FlagColor[] = {
	Color(0, 100, 100, 100), // 0 S
	Color(0, 180, 100, 100), // 1 p
	Color(0, 255, 0, 0), // 2 t
	Color(0, 0, 255, 0), // 3 j
	Color(0, 120, 120, 255), // 4 o
	Color(0, 120, 120, 0), // 5 l
	Color(0, 255, 0, 255), // 6 c
	Color(0, 0, 255, 255), // 7 a
	Color(0, 0, 150, 150), // 8 r
	Color(0, 150, 0, 150), // 9 d
	Color(0, 0, 150, 0), // 10 g
	Color(0, 120, 0, 250), // 11 f
	Color(0, 250, 100, 160) // 12 m
};

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
// Leap chain, when one of leaps is longer then 3rd

CGenCF1::CGenCF1()
{
	//midifile_tpq_mul = 8;
}

CGenCF1::~CGenCF1()
{
}

void CGenCF1::LoadConfigLine(CString* sN, CString* sV, int idata, double fdata)
{
	CheckVar(sN, sV, "min_interval", &min_interval);
	CheckVar(sN, sV, "max_interval", &max_interval);
	CheckVar(sN, sV, "c_len", &c_len);
	LoadNote(sN, sV, "first_note", &first_note);
	CheckVar(sN, sV, "last_diatonic_int", &last_diatonic_int);
	CheckVar(sN, sV, "max_unfill_steps", &max_unfill_steps);
	CheckVar(sN, sV, "max_repeat_mul", &max_repeat_mul);
	CheckVar(sN, sV, "max_smooth_direct", &max_smooth_direct);
	CheckVar(sN, sV, "max_smooth", &max_smooth);
	CheckVar(sN, sV, "max_leaps", &max_leaps);
	CheckVar(sN, sV, "max_leap_steps", &max_leap_steps);
	CheckVar(sN, sV, "stag_notes", &stag_notes);
	CheckVar(sN, sV, "stag_note_steps", &stag_note_steps);
	CheckVar(sN, sV, "min_tempo", &min_tempo);
	CheckVar(sN, sV, "max_tempo", &max_tempo);
	CheckVar(sN, sV, "random_choose", &random_choose);
	CheckVar(sN, sV, "shuffle", &shuffle);
	LoadVar(sN, sV, "accept", &accept);
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
	double cycle = 0;
	long accepted = 0, accepted2 = 0, accepted3 = 0, accepted4 = 0;
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
			if (nmax - nmin < min_interval) goto skip;
			// Clear flags

			accepted3++;
			flags = "Sptjolcardgfm";
			for (int i = 0; i < c_len; i++) {
				nflagsc[i] = 0;
				// Calculate chromatic positions
				cc[i] = dia_to_chrom[(c[i] + 56) % 7] + (((c[i]+56) / 7)-8) * 12 + first_note; // Negative eight octaves reserve
				//if ((c[i] < 0) && (c[i] % 7 != 0)) cc[i] -= 12; // Correct negative octaves
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
					if ((cc[i] % 12 == 5) && (cc[i - 1] % 12 != 4)) goto skip;
					if ((cc[i + 1] % 12 == 5) && (cc[i + 2] % 12 != 4)) goto skip;
					// Record tritone
					if (accept[2] != 'T') goto skip;
					flags[0] = 's';
					flags[2] = 'T';
					nflags[i][nflagsc[i]] = 2;
					nflagsc[i]++;
				}
				// Sept prohibit
				if (abs(c[i + 1] - c[i]) == 6) {
					if (accept[1] != 'P') goto skip;
					flags[0] = 's';
					flags[1] = 'P';
					nflags[i][nflagsc[i]] = 1;
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
					if (accept[3] != 'J') goto skip;
					flags[0] = 's';
					flags[3] = 'J';
					nflags[i][nflagsc[i]] = 3;
					nflagsc[i]++;
				}
				// Prohibit long smooth movement
				if (smooth[i] != 0) smooth_sum++;
				else smooth_sum = 0;
				if (smooth_sum >= max_smooth) {
					if (accept[4] != 'O') goto skip;
					flags[0] = 's';
					flags[4] = 'O';
					nflags[i][nflagsc[i]] = 4;
					nflagsc[i]++;
				}
				if (i < c_len - 2) {
					// Prohibit long smooth movement in one direction
					if (smooth[i] == smooth[i + 1]) smooth_sum2++;
					else smooth_sum2 = 0;
					if (smooth_sum2 >= max_smooth_direct - 1) {
						if (accept[5] != 'L') goto skip;
						flags[0] = 's';
						flags[5] = 'L';
						nflags[i][nflagsc[i]] = 5;
						nflagsc[i]++;
					}
					// Check if leaps follow each other in same direction
					if (leap[i] * leap[i + 1] > 0) {
						// Check if leaps are long
						if (c[i + 2] - c[i] > 4) goto skip;
						if (accept[6] != 'C') goto skip;
						flags[0] = 's';
						flags[6] = 'C';
						nflags[i][nflagsc[i]] = 6;
						nflagsc[i]++;
					}
					// Check if melody direction changes after leap
					if (leap[i] * (c[i + 2] - c[i + 1]) > 0) {
						if (i < c_len - 3) {
							if (leap[i] * (c[i + 3] - c[i + 2]) > 0) goto skip;
							if (flags[6] != 'C') {
								if (accept[7] != 'A') goto skip;
								flags[0] = 's';
								flags[7] = 'A';
								nflags[i][nflagsc[i]] = 7;
								nflagsc[i]++;
							}
						}
						else goto skip;
					}
					// Check if leap returns to same note
					if ((leap[i] != 0) && (leap[i + 1] != 0) && (c[i] == c[i + 2])) {
						if (accept[8] != 'R') goto skip;
						flags[0] = 's';
						flags[8] = 'R';
						nflags[i][nflagsc[i]] = 8;
						nflagsc[i]++;
					}
					// Check if two notes repeat
					if ((i > 0) && (c[i] == c[i + 2]) && (c[i - 1] == c[i + 1])) {
						if (accept[9] != 'D') goto skip;
						flags[0] = 's';
						flags[9] = 'D';
						nflags[i][nflagsc[i]] = 9;
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
					if (accept[10] != 'G') goto skip;
					flags[0] = 's';
					flags[10] = 'G';
					nflags[i][nflagsc[i]] = 10;
					nflagsc[i]++;
				}
			}
			// Check note fill
			for (int i = nmin; i <= nmax; i++) {
				if (nstat2[i + max_interval] == 0) {
					if (accept[11] != 'F') goto skip;
					flags[0] = 's';
					flags[11] = 'F';
					break;
				}
			}
			// Prohibit multiple culminations
			culm_sum = 0;
			for (int i = 0; i < c_len; i++) {
				if (c[i] == nmax) {
					culm_sum++;
					if (culm_sum > 1) {
						if (accept[12] != 'M') goto skip;
						flags[0] = 's';
						flags[12] = 'M';
						nflags[i][nflagsc[i]] = 12;
						nflagsc[i]++;
					}
				}
			}
			accepted2++;
			// Calculate flag statistics
			for (int i = 0; i < MAX_FLAGS; i++) {
				if (flags[i] <= 'Z') fstat[i]++;
			}
			// Check if flags are accepted
			for (int i = 0; i < MAX_FLAGS; i++) {
				if ((flags[i] <= 'Z') && (accept[i] > 'Z')) goto skip;
			}
			// Check random_choose
			if (random_choose < 100) if (rand2() >= (double)RAND_MAX*random_choose / 100.0) goto skip;
			// Accept cantus
			accepted4++;
			if (accepted < t_cnt) {
				accepted++;
				Sleep(sleep_ms);
				//Color ccolor = Color(0, randbw(0, 180), randbw(0, 180), randbw(0, 180));
				// Copy cantus to output
				if (step + c_len >= t_allocated) ResizeVectors(t_allocated * 2);
				//comment[step][0].Format("c%ld a%ld", cycle, accepted);
				for (int x = step; x < step + c_len; x++) {
					//color[x][0] = ccolor;
					// Set flag color
					color[x][0] = FlagColor[0];
					//for (int i = 0; i < MAX_FLAGS; i++) if (flags[i] <= 'Z') color[x][0] = FlagColor[i];
					if (flags[11] == 'F') {
						color[x][0] = FlagColor[11];
						if (x == step)  comment[step][0] += "This cantus has unfilled loop. ";
					}
					// Set nflag color
					if (nflagsc[x - step] > 0) color[x][0] = FlagColor[nflags[x - step][0]];
					note[x][0] = cc[x - step];
					//color[x][0] = FlagColor[accepted%13];
					if (nflagsc[x - step] > 0) for (int i = 0; i < nflagsc[x - step]; i++) {
						comment[x][0] += FlagName[nflags[x - step][i]] + ". ";
					}
					len[x][0] = 1;
					pause[x][0] = 0;
					tempo[x] = 200;
					coff[x][0] = 0;
					if (x < step + c_len / 2)	dyn[x][0] = 70 + 40*(x - step)/c_len + 10 * rand2() / RAND_MAX;
					else dyn[x][0] = 70 + 40*(step + c_len - x) / c_len + 10 * rand2() / RAND_MAX;
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
				dyn[step][0] = 0;
				tempo[step] = tempo[step - 1];
				coff[step][0] = 0;
				step++;
				// Count additional variables
				CountOff(step - c_len - 1, step - 1);
				CountTime(step - c_len - 1, step - 1);
				UpdateNoteMinMax(step - c_len - 1, step - 1);
				UpdateTempoMinMax(step - c_len - 1, step - 1);
				if (!shuffle) {
					Adapt(step - c_len - 1, step - 1);
				}
				// Send
				t_generated = step;
				if (!shuffle) {
					t_sent = t_generated;
				}
				::PostMessage(m_hWnd, WM_GEN_FINISH, 1, 0);
			}
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
	// Show flag statistics
	CString* est = new CString;
	CString st, st2;
	for (int i = 0; i < MAX_FLAGS; i++) {
		st.Format("%c-%.3f ", accept[i], (double)fstat[i]/(double)1000);
		st2 += st;
	}
	est->Format("%d/%d: Accepted %.8f%% (%.3f/%.3f/%.3f/%.3f) variants of %.3f: %s", c_len, max_interval, 100.0*(double)accepted / cycle, 100.0*(double)accepted4 / cycle, (double)accepted/1000, (double)accepted2/1000, (double)accepted3 / 1000, cycle/1000, st2);
	AppendLineToFile("GenCF1.log", *est + "\n");
	WriteLog(3, est);
	// Random shuffle
	if (shuffle) {
		vector<unsigned short> ci(accepted); // cantus indexes
		vector<unsigned char> note2(t_generated);
		vector<CString> comment2(t_generated);
		vector<Color> color2(t_generated);
		for (int i = 0; i < accepted; i++) ci[i] = i;
		// Shuffled indexes
		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		::shuffle(ci.begin(), ci.end(), default_random_engine(seed));
		// Swap
		int i1, i2;
		for (int i = 0; i < accepted; i++) {
			for (int x = 0; x < c_len; x++) {
				i1 = i*(c_len + 1) + x;
				i2 = ci[i]*(c_len + 1) + x;
				note2[i1] = note[i2][0];
				comment2[i1] = comment[i2][0];
				color2[i1] = color[i2][0];
			}
		}
		// Replace
		for (int i = 0; i < accepted; i++) {
			for (int x = 0; x < c_len; x++) {
				i1 = i*(c_len + 1) + x;
				note[i1][0] = note2[i1];
				comment[i1][0] = comment2[i1];
				color[i1][0] = color2[i1];
			}
		}
		// Adapt
		Adapt(0, t_generated);
		// Send
		t_sent = t_generated;
		::PostMessage(m_hWnd, WM_GEN_FINISH, 2, 0);
		CString* est = new CString;
		est->Format("Shuffle of %ld melodies finished", accepted);
		WriteLog(3, est);
	}
}
