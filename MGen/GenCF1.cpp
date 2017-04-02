#include "stdafx.h"
#include "GenCF1.h"

#ifdef _DEBUG
#define new DEBUG_NEW 
#endif

#define MAX_FLAGS 36
// 
#define FLAG(id, i) { if ((!calculate_correlation) && (accept[id] < 1)) goto skip; flags[0] = 0; flags[id] = 1; nflags[i][nflagsc[i]] = id; nflagsc[i]++; }

const CString FlagName[MAX_FLAGS] = {
	"Strict", // 0
	"Seventh", // 1
	"Tritone resolved", // 2 
	"Many leaps", // 3 
	"Long smooth", // 4 
	"Long line", // 5 
	"Two 3rds", // 6 
	"Late <6th resolution", // 7 
	"Leap back <5th", // 8 
	"Close repeat", // 9 
	"Stagnation", // 10 
	"Leap pre/late fill", // 11 
	"Multiple culminations", // 12 
	"2nd to last not D", // 13
	"3rd to last is CEG", // 14
	"3 letters in a row", // 15
	"4 letters in a row", // 16
	">4 letters in a row", // 17
	"4 step miss", // 18
	"5 step miss", // 19
	">5 step miss", // 20
	"Late culmination", // 21
	"Leap back >4th", // 22
	"Last leap", // 23
	"Unfilled leap", // 24
	"Many leaps+", // 25
	"Leap unresolved", // 26
	"Leap chain", // 27
	"Two 3rds after 6/8", // 28
	"Late >5th resolution", // 29
	"Prepared unresolved 3rd", // 30
	"Tritone unresolved", // 31
	"Tritone culmination", // 32
	"Leap to leap resolution", // 33
	"3rd to last is leading", // 34
	"Prepared unfilled 3rd", // 35
};

const int SeverityFlag[MAX_FLAGS] = {
	0, // "Strict", // 0
	35, // "Prepared unfilled 3rd", // LEAP FILL
	30, // "Prepared unresolved 3rd", // LEAP RESOLUTION
	28, // "Two 3rds after 6/8", // LEAP RESOLUTION
	7, // "Late <6th resolution", // LEAP RESOLUTION
	8, // "Leap back <5th", // LEAP RESOLUTION 
	1, // "Seventh", // LEAPS
	11, // "Leap pre/late fill", // LEAP FILL 
	3, // "Many leaps", // LEAPS 
	14, // "3rd to last is CEG", // HARMONY
	23, // "Last leap", // END
	2, // "Tritone resolved", // TRITONE 
	15, // "3 letters in a row", // HARMONY
	24, // "Unfilled leap", // LEAP FILL
	33, // "Leap to leap resolution", // LEAP RESOLUTION
	6, // "Two 3rds", // LEAP RESOLUTION 
	18, // "4 step miss", // HARMONY

	4, // "Long smooth", // LEAPS 
	5, // "Long line", // LEAPS 
	9, // "Close repeat", // REPEATS 
	10, // "Stagnation", // REPEATS 
	12, // "Multiple culminations", // REPEATS 
	13, // "2nd to last not D", // HARMONY
	16, // "4 letters in a row", // HARMONY
	17, // ">4 letters in a row", // HARMONY
	19, // "5 step miss", // HARMONY
	20, // ">5 step miss", // HARMONY
	21, // "Late culmination", // END
	22, // "Leap back >4th", // LEAP RESOLUTION
	25, // "Many leaps+", // LEAPS
	26, // "Leap unresolved", // LEAP RESOLUTION
	27, // "Leap chain", // LEAP RESOLUTION
	29, // "Late >5th resolution", // LEAP RESOLUTION
	31, // "Tritone unresolved", // TRITONE
	32, // "Tritone culmination", // TRITONE
	34, // "3rd to last is leading", // HARMONY
};

const Color FlagColor[] = {
	Color(0, 100, 100, 100), // 0 S
};

// Unskippable rules:
// Total interval
// Note repeats note of previous measure

CGenCF1::CGenCF1()
{
	//midifile_tpq_mul = 8;
	accept.resize(MAX_FLAGS);
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
	CheckVar(sN, sV, "fill_steps_mul", &fill_steps_mul);
	CheckVar(sN, sV, "max_repeat_mul", &max_repeat_mul);
	CheckVar(sN, sV, "max_smooth_direct", &max_smooth_direct);
	CheckVar(sN, sV, "max_smooth", &max_smooth);
	CheckVar(sN, sV, "max_leaps", &max_leaps);
	CheckVar(sN, sV, "max_leaps2", &max_leaps2);
	CheckVar(sN, sV, "max_leap_steps", &max_leap_steps);
	CheckVar(sN, sV, "stag_notes", &stag_notes);
	CheckVar(sN, sV, "stag_note_steps", &stag_note_steps);
	LoadRange(sN, sV, "tempo", &min_tempo, &max_tempo);
	CheckVar(sN, sV, "random_choose", &random_choose);
	CheckVar(sN, sV, "shuffle", &shuffle);
	CheckVar(sN, sV, "calculate_correlation", &calculate_correlation);
	// Load accept
	CString st;
	for (int i = 0; i < MAX_FLAGS; i++) {
		st = FlagName[i];
		st.MakeLower();
		if (*sN == st) {
			accept[i] = atoi(*sV);
		}
	}
}


void CGenCF1::Generate()
{
	vector<int> c(c_len); // cantus (diatonic)
	vector<int> cc(c_len); // cantus (chromatic)
	vector<int> pc(c_len); // pitch class
	vector<int> leap(c_len);
	vector<int> smooth(c_len);
	vector<int> nstat(max_interval * 2 + 1);
	vector<int> nstat2(max_interval * 2 + 1);
	vector<int> nstat3(max_interval * 2 + 1);
	vector<long> fstat(MAX_FLAGS); // number of canti with each flag
	vector<unsigned char>  flags(MAX_FLAGS); // Flags for whole cantus
	vector<unsigned char>  flag_sev(MAX_FLAGS); // Get severity by flag id
	vector<Color>  flag_color(MAX_FLAGS); // Flag colors
	vector<vector<long>> fcor = vector<vector<long>>(MAX_FLAGS, vector<long>(MAX_FLAGS)); // Flags correlation matrix
	vector<vector<unsigned char>> nflags = vector<vector<unsigned char>>(c_len, vector<unsigned char>(MAX_FLAGS)); // Flags for each note
	vector<unsigned char> nflagsc(c_len); // number of flags for each note
	// Set first and last notes
	c[0] = 0;
	c[c_len-1] = last_diatonic_int;
	// Set priority
	for (int i = 0; i < MAX_FLAGS; i++) {
		flag_sev[SeverityFlag[i]] = i;
		flag_color[SeverityFlag[i]] = Color(0, 255.0 / MAX_FLAGS*i, 255 - 255.0 / MAX_FLAGS*i, 0);
	}
	// Set middle notes to minimum
	for (int i = 1; i < c_len - 1; i++) c[i] = -max_interval;
	// Walk all variants
	int p = c_len - 2; // Minimal position in array to cycle
	double cycle = 0;
	long accepted = 0, accepted2 = 0, accepted3 = 0, accepted4 = 0;
	int finished = 0;
	int nmin, nmax, leap_sum, max_leap_sum, leap_sum_i, culm_sum, culm_step, smooth_sum, smooth_sum2, pos, ok, ok2;
	int dcount, scount, tcount, wdcount, wscount, wtcount, third_prepared;
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
			fill(flags.begin(), flags.end(), 0);
			flags[0] = 1;
			for (int i = 0; i < c_len; i++) {
				nflagsc[i] = 0;
				// Calculate chromatic positions
				cc[i] = dia_to_chrom[(c[i] + 56) % 7] + (((c[i] + 56) / 7) - 8) * 12 + first_note; // Negative eight octaves reserve
				// Calculate pitch class
				pc[i] = (c[i] + 56) % 7;
			}
			// Wrong second to last note
			if ((pc[c_len - 2] == 0) || (pc[c_len - 2] == 2) || (pc[c_len - 2] == 3) || (pc[c_len - 2] == 5)) FLAG(13, c_len - 2);
			// Wrong third to last note
			if ((pc[c_len - 3] == 0) || (pc[c_len - 3] == 2) || (pc[c_len - 3] == 4)) FLAG(14, c_len - 3);
			// Leading third to last note
			if (pc[c_len - 3] == 6) FLAG(34, c_len - 3);
			dcount = 0;
			scount = 0;
			tcount = 0;
			wdcount = 0;
			wscount = 0;
			wtcount = 0;
			for (int i = 0; i < c_len; i++) {
				// Count same and missing letters in a row
				if ((pc[i] == 0) || (pc[i] == 2) || (pc[i] == 5)) {
					if (wtcount == 4) FLAG(18, i - 1);
					if (wtcount == 5) FLAG(19, i - 1);
					if (wtcount > 5) FLAG(20, i - 1);
					tcount++;
					wtcount = 0;
				}
				else {
					if (tcount == 3) FLAG(15, i - 1);
					if (tcount == 4) FLAG(16, i - 1);
					if (tcount > 4) FLAG(17, i - 1);
					tcount = 0;
					wtcount++;
				}
				if ((pc[i] == 1) || (pc[i] == 2) || (pc[i] == 4) || (pc[i] == 6)) {
					if (wdcount == 4) FLAG(18, i - 1);
					if (wdcount == 5) FLAG(19, i - 1);
					if (wdcount > 5) FLAG(20, i - 1);
					dcount++;
					wdcount = 0;
				}
				else {
					if (dcount == 3) FLAG(15, i - 1);
					if (dcount == 4) FLAG(16, i - 1);
					if (dcount > 4) FLAG(17, i - 1);
					dcount = 0;
					wdcount++;
				}
				if ((pc[i] == 1) || (pc[i] == 3) || (pc[i] == 5)) {
					if (wscount == 4) FLAG(18, i - 1);
					if (wscount == 5) FLAG(19, i - 1);
					if (wscount > 5) FLAG(20, i - 1);
					scount++;
					wscount = 0;
				}
				else {
					if (scount == 3) FLAG(15, i - 1);
					if (scount == 4) FLAG(16, i - 1);
					if (scount > 4) FLAG(17, i - 1);
					scount = 0;
					wscount++;
				}
			}
			// Check same letters
			if ((tcount == 3) || (dcount == 3) || (scount == 3)) FLAG(15, c_len-1);
			if ((tcount == 4) || (dcount == 4) || (scount == 4)) FLAG(16, c_len - 1);
			if ((tcount > 4) || (dcount > 4) || (scount > 4)) FLAG(17, c_len - 1);
			// Check missing letters
			if ((wtcount == 4) || (wdcount == 4) || (wscount == 4)) FLAG(18, c_len - 1);
			if ((wtcount == 5) || (wdcount == 5) || (wscount == 5)) FLAG(19, c_len - 1);
			if ((wtcount > 5) || (wdcount > 5) || (wscount > 5)) FLAG(20, c_len - 1);
			for (int i = 0; i < c_len - 1; i++) {
				// Tritone prohibit
				if (abs(cc[i+1] - cc[i]) == 6) {
					// Check if tritone is highest leap
					if ((c[i] == nmax) || (c[i+1] == nmax)) FLAG(32, i)
					// Check if tritone is first or last step
					if (i > c_len - 3) FLAG(31, i)
					//if (i < 1) FLAG(31, i);
					// Check if resolution is correct
					else if (cc[i + 1] % 12 == 5) FLAG(31, i)
					else if (cc[i + 2] % 12 != 0) FLAG(31, i)
					else if (cc[i - 1] % 12 != 4) FLAG(31, i)
					// Record resolved tritone
					else FLAG(2, i);
				}
				// Sept prohibit
				if (abs(c[i + 1] - c[i]) == 6) FLAG(1, i);
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
			max_leap_sum = 0;
			smooth_sum = 0;
			smooth_sum2 = 0;
			for (int i = 0; i < c_len - 1; i++) {
				// Add new leap
				if (leap[i] != 0) {
					leap_sum++;
					// Check if this leap is 3rd
					third_prepared = 0;
					if (abs(c[i + 1] - c[i]) == 2) {
						pos = (c[i + 1] + c[i]) / 2;
						// Check if 3rd was pre-filled
						if ((i > 0) && (c[i - 1] == pos)) third_prepared = 1;
						else if ((i > 1) && (c[i - 2] == pos)) third_prepared = 1;
						else if ((i > 2) && (c[i - 3] == pos)) third_prepared = 1;
						else if ((i > 3) && (c[i - 4] == pos)) third_prepared = 1;
						// Check if 3rd has pre-leap
						else if ((i > 0) && ((c[i - 1] - c[i + 1])*leap[i] > 0)) third_prepared = 1;
						else if ((i > 1) && ((c[i - 2] - c[i + 1])*leap[i] > 0)) third_prepared = 1;
						else if ((i > 2) && ((c[i - 3] - c[i + 1])*leap[i] > 0)) third_prepared = 1;
					}
					// Check if  leap is filled
					pos = i + 2 + (abs(c[i + 1] - c[i]) - 1) * fill_steps_mul;
					if (pos > c_len - 1) pos = c_len - 1;
					// Zero array
					fill(nstat2.begin(), nstat2.end(), 0);
					fill(nstat3.begin(), nstat3.end(), 0);
					// Fill all notes
					for (int x = 0; x < c_len; x++) {
						// Update local fill
						if ((x > i + 1) && (x <= pos)) nstat3[c[x] + max_interval]++;
						// Update global fill
						nstat2[c[x] + max_interval]++;
					}
					// Check if leap is filled
					ok = 1; // Local fill
					ok2 = 1; // Global fill
					int pos1 = min(c[i], c[i + 1]);
					int pos2 = max(c[i], c[i + 1]);
					for (int x = pos1 + 1; x < pos2; x++) if (!nstat3[x + max_interval]) {
						ok = 0;
						if (!nstat2[x + max_interval]) ok2 = 0;
						break;
					}
					// Local not filled?
					if (!ok) {
						// Local not filled. Third prepared?
						if (third_prepared) FLAG(35, i)
							// Local not filled. Third not prepared. Global filled?
						else if (ok2) FLAG(11, i)
							// Local not filled. Third not prepared. Global not filled.
						else FLAG(24, i);
					}
				}
				// Subtract old leap
				if ((i >= max_leap_steps) && (leap[i - max_leap_steps] != 0)) leap_sum--;
				// Get maximum leap_sum
				if (leap_sum > max_leap_sum) {
					max_leap_sum = leap_sum;
					leap_sum_i = i;
				}
				// Prohibit long smooth movement
				if (smooth[i] != 0) smooth_sum++;
				else smooth_sum = 0;
				if (smooth_sum >= max_smooth) FLAG(4, i);
				if (i < c_len - 2) {
					// Prohibit long smooth movement in one direction
					if (smooth[i] == smooth[i + 1]) smooth_sum2++;
					else smooth_sum2 = 0;
					if (smooth_sum2 >= max_smooth_direct - 1) FLAG(5, i);
					// Check if leaps follow each other in same direction
					if (leap[i] * leap[i + 1] > 0) {
						// Check if leaps are long
						if (abs(c[i + 2] - c[i]) > 4) FLAG(27, i)
						// If leaps are 3rds
						else {
							if ((i>0) && ((leap[i] * (c[i] - c[i - 1]) == -5) || (leap[i]*(c[i] - c[i-1]) == -7))) FLAG(28, i)
							else FLAG(6, i);
						}
					}
					// Check if melody direction does not change after leap
					else if (leap[i] * (c[i + 2] - c[i + 1]) > 0) {
						// If this 3rd was prepared
						if (third_prepared) FLAG(30, i)
						else {
							if (i < c_len - 3) {
								// Check if melody direction does not change change second note after leap
								if (leap[i] * (c[i + 3] - c[i + 2]) > 0) FLAG(26, i)
								// If direction changes second note after leap
								else {
									// Check leap size
									if (abs(c[i + 1] - c[i]) > 4) FLAG(29, i)
									else FLAG(7, i);
								}
							}
							else FLAG(26, i);
						}
					}
					// If melody direction changes after leap
					else {
						// Check if it is a leap to leap resolution
						if (leap[i] * leap[i + 1] != 0) FLAG(33, i);
					}
					// Check if leap returns to same note
					if ((leap[i] != 0) && (leap[i + 1] != 0) && (c[i] == c[i + 2])) {
						if (abs(c[i] - c[i + 1]) > 3) FLAG(22, i)
						else FLAG(8, i);
					}
					// Check if two notes repeat
					if ((i > 0) && (c[i] == c[i + 2]) && (c[i - 1] == c[i + 1])) FLAG(9, i);
				}
			}
			// Check if too many leaps
			if (max_leap_sum > max_leaps) {
				if (leap_sum > max_leaps2) FLAG(25, leap_sum_i)
				else FLAG(3, leap_sum_i);
			}
			// Clear nstat
			for (int i = 0; i <= max_interval * 2; i++) {
				nstat[i] = 0;
			}
			for (int i = 0; i < c_len; i++) {
				// Prohibit stagnation
				// Add new note
				nstat[c[i] + max_interval]++; // Stagnation array
				// Subtract old note
				if ((i >= stag_note_steps)) nstat[c[i - stag_note_steps] + max_interval]--;
				// Check if too many repeating notes
				if (nstat[c[i] + max_interval] > stag_notes) FLAG(10, i);
			}
			// Prohibit multiple culminations
			culm_sum = 0;
			for (int i = 0; i < c_len; i++) {
				if (c[i] == nmax) {
					culm_sum++;
					culm_step = i;
					if (culm_sum > 1) FLAG(12, i);
				}
			}
			// Prohibit culminations at last steps
			if (culm_step > c_len - 4) FLAG(21, culm_step);
			// Prohibit last leap
			if (leap[c_len-2]) FLAG(23, c_len-1);
			accepted2++;
			// Calculate flag statistics
			for (int i = 0; i < MAX_FLAGS; i++) {
				if (flags[i]) {
					fstat[i]++;
					// Calculate correlation
					if (calculate_correlation) for (int z = 0; z < MAX_FLAGS; z++) {
						if (flags[z]) fcor[i][z]++;
					}
				}
			}
			// Check if flags are accepted
			for (int i = 0; i < MAX_FLAGS; i++) {
				if ((flags[i]) && (!accept[i])) goto skip;
				if ((!flags[i]) && (accept[i] == 2)) goto skip;
			}
			// Check random_choose
			if (random_choose < 100) if (rand2() >= (double)RAND_MAX*random_choose / 100.0) goto skip;
			// Accept cantus
			accepted4++;
			if (accepted < t_cnt) {
				accepted++;
				Sleep(sleep_ms);
				// Copy cantus to output
				if (step + c_len >= t_allocated) ResizeVectors(t_allocated * 2);
				for (int x = step; x < step + c_len; x++) {
					// Set flag color
					color[x][0] = FlagColor[0];
					int current_severity = -1;
					// Set nflag color
					note[x][0] = cc[x - step];
					if (nflagsc[x - step] > 0) for (int i = 0; i < nflagsc[x - step]; i++) {
						comment[x][0] += FlagName[nflags[x - step][i]] + ". ";
						// Set note color if this is maximum flag severity
						if (flag_sev[nflags[x - step][i]] > current_severity) {
							current_severity = flag_sev[nflags[x - step][i]];
							color[x][0] = flag_color[nflags[x - step][i]];
						}
					}
					len[x][0] = 1;
					pause[x][0] = 0;
					tempo[x] = 200;
					coff[x][0] = 0;
					if (x < step + c_len / 2)	dyn[x][0] = 60 + 40*(x - step)/c_len + 20 * rand2() / RAND_MAX;
					else dyn[x][0] = 60 + 40 * (step + c_len - x) / c_len + 20 * rand2() / RAND_MAX;
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
	// Write flag correlation
	if (calculate_correlation) {
		DeleteFile("cf1-cor.csv");
		CString st, st2, st3;
		st3 = "Flag; Total; ";
		for (int i = 0; i < MAX_FLAGS; i++) {
			int f1 = SeverityFlag[i];
			st2.Format("%s; %d; ", FlagName[f1], fcor[f1][f1]);
			st3 += FlagName[f1] + "; ";
			for (int z = 0; z < MAX_FLAGS; z++) {
				int f2 = SeverityFlag[z];
				st.Format("%ld; ", fcor[f1][f2]);
				st2 += st;
			}
			CGLib::AppendLineToFile("cf1-cor.csv", st2 + "\n");
		}
		CGLib::AppendLineToFile("cf1-cor.csv", st3 + "\n");
	}
	// Show flag statistics
	CString* est = new CString;
	CString st, st2;
	for (int i = 0; i < MAX_FLAGS; i++) {
		st.Format("%s-%.3f ", FlagName[i].Left(10), (double)fstat[i]/(double)1000);
		st2 += st;
	}
	est->Format("%d/%d: Accepted %.8f%% (%.3f/%.3f/%.3f/%.3f) variants of %.3f: %s", 
		c_len, max_interval, 100.0*(double)accepted / cycle, (double)accepted4/1000.0, (double)accepted/1000.0, (double)accepted2/1000.0, 
		(double)accepted3 / 1000.0, cycle/1000, st2);
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
		Adapt(0, t_generated-1);
		// Send
		t_sent = t_generated;
		::PostMessage(m_hWnd, WM_GEN_FINISH, 2, 0);
		CString* est = new CString;
		est->Format("Shuffle of %ld melodies finished", accepted);
		WriteLog(3, est);
	}
}
