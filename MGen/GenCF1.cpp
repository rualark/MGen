#include "stdafx.h"
#include "GenCF1.h"

#ifdef _DEBUG
#define new DEBUG_NEW 
#endif

#define MAX_FLAGS 37
#define MAX_WIND 50
// 
#define FLAG(id, i) { if ((skip_flags) && (accept[id] < 1)) goto skip; flags[0] = 0; flags[id] = 1; nflags[i][nflagsc[i]] = id; nflagsc[i]++; }

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
	"Leap pre-late fill", // 11 
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
	"Outstanding repeat", // 36
};

const int SeverityFlag[MAX_FLAGS] = {
	0, // "Strict", // 0
	35, // "Prepared unfilled 3rd", // LEAP FILL
	30, // "Prepared unresolved 3rd", // LEAP RESOLUTION
	28, // "Two 3rds after 6/8", // LEAP RESOLUTION
	7, // "Late <6th resolution", // LEAP RESOLUTION
	8, // "Leap back <5th", // LEAP RESOLUTION 
	1, // "Seventh", // LEAPS
	11, // "Leap pre-late fill", // LEAP FILL 
	3, // "Many leaps", // LEAPS 
	14, // "3rd to last is CEG", // HARMONY
	23, // "Last leap", // END
	2, // "Tritone resolved", // TRITONE 
	15, // "3 letters in a row", // HARMONY
	24, // "Unfilled leap", // LEAP FILL
	33, // "Leap to leap resolution", // LEAP RESOLUTION
	6, // "Two 3rds", // LEAP RESOLUTION 
	18, // "4 step miss", // HARMONY
	36, // "Outstanding repeat", // REPEATS

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
	flag_sev.resize(MAX_FLAGS);
	flag_color.resize(MAX_FLAGS);
}

CGenCF1::~CGenCF1()
{
}

void CGenCF1::LoadConfigLine(CString* sN, CString* sV, int idata, double fdata)
{
	CheckVar(sN, sV, "min_interval", &min_interval);
	CheckVar(sN, sV, "max_interval", &max_interval);
	CheckVar(sN, sV, "c_len", &c_len);
	CheckVar(sN, sV, "s_len", &s_len);
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
	CheckVar(sN, sV, "random_seed", &random_seed);
	CheckVar(sN, sV, "repeat_steps", &repeat_steps);
	CheckVar(sN, sV, "shuffle", &shuffle);
	CheckVar(sN, sV, "show_severity", &show_severity);
	CheckVar(sN, sV, "calculate_correlation", &calculate_correlation);
	CheckVar(sN, sV, "calculate_stat", &calculate_stat);
	CheckVar(sN, sV, "calculate_blocking", &calculate_blocking);
	CheckVar(sN, sV, "late_require", &late_require);
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

void CGenCF1::ScanCantus() {
	CString st, st2;
	int wid; // Window id
	int seed_cycle = 0; // Number of cycles in case of random_seed
	vector<char> c(c_len); // cantus (diatonic)
	vector<unsigned char> cc(c_len); // cantus (chromatic)
	vector<unsigned char> pc(c_len); // pitch class
	vector<char> leap(c_len);
	vector<char> smooth(c_len);
	vector<int> wpos1(c_len / s_len + 1);
	vector<int> wpos2(c_len / s_len + 1);
	vector<int> nstat(max_interval * 2 + 1);
	vector<int> nstat2(max_interval * 2 + 1);
	vector<int> nstat3(max_interval * 2 + 1);
	vector<long long> wscans(MAX_WIND); // number of full scans per window
	vector<long long> accepted4(MAX_WIND); // number of accepted canti per window
	vector<long long> accepted5(MAX_WIND); // number of canti with neede flags per window
	vector<long long> fstat(MAX_FLAGS); // number of canti with each flag
	vector<vector<vector<long>>> fblock(MAX_WIND, vector<vector<long>>(MAX_FLAGS, vector<long>(MAX_FLAGS))); // number of canti rejected with foreign flags
	vector<unsigned char>  flags(MAX_FLAGS); // Flags for whole cantus
	vector<vector<long long>> fcor(MAX_FLAGS, vector<long long>(MAX_FLAGS)); // Flags correlation matrix
	vector<vector<unsigned char>> nflags(c_len, vector<unsigned char>(MAX_FLAGS)); // Flags for each note
	vector<unsigned char> nflagsc(c_len); // number of flags for each note
	int skip_flags = !calculate_blocking && !calculate_correlation && !calculate_stat;
	int flags_need2 = 0; // Number of second level flags set
	// Calculate second level flags count
	for (int i = 0; i < MAX_FLAGS; i++) {
		if (accept[i] == 2) flags_need2++;
	}
	// Check that at least one rule accepted
	for (int i = 0; i < MAX_FLAGS; i++) {
		if (accept[i]) break;
		if (i == MAX_FLAGS - 1) WriteLog(1, "Warning: all rules are rejected (0) in configuration file");
	}
	// Set first and last notes
	c[0] = 0;
	c[c_len - 1] = last_diatonic_int;
	// Set priority
	for (int i = 0; i < MAX_FLAGS; i++) {
		flag_sev[SeverityFlag[i]] = i;
		flag_color[SeverityFlag[i]] = Color(0, 255.0 / MAX_FLAGS*i, 255 - 255.0 / MAX_FLAGS*i, 0);
	}
	// Set middle notes to minimum
	for (int i = 1; i < c_len - 1; i++) c[i] = -max_interval;
	if (random_seed)
		for (int i = 1; i < c_len - 1; i++) c[i] = -randbw(-max_interval, max_interval);
	// Walk all variants
	long long cycle = 0;
	long long accepted2 = 0, accepted3 = 0;
	int finished = 0;
	int nmin, nmax, leap_sum, max_leap_sum, leap_sum_i, culm_sum, culm_step, smooth_sum, smooth_sum2, pos, ok, ok2;
	int dcount, scount, tcount, wdcount, wscount, wtcount, third_prepared;
	step = 0; // Global step
						// Select window
	int wcount = 1; // Number of windows created
	int sp1 = 1; // Start of search window
	int sp2 = sp1 + s_len; // End of search window
	if (sp2 > c_len - 1) sp2 = c_len - 1;
	int ep2 = sp2; // End of evaluation window
								 // Record window
	wid = 0;
	wpos1[wid] = sp1;
	wpos2[wid] = sp2;
	// Add last note if this is last window
	if (ep2 == c_len - 1) ep2 = c_len;
	int p = sp2 - 1; // Minimal position in array to cycle
									 // Check if too many windows
	if ((c_len - 2) / (double)s_len > MAX_WIND) {
		CString* est = new CString;
		est->Format("Error: generating %d notes with search window %d requires more than %d windows. Change MAX_WIND to allow more.",
			c_len, s_len, MAX_WIND);
		WriteLog(1, est);
		return;
	}
	while (true) {
		if (need_exit) break;
		// Analyze combination
		if (cycle >= 0) { // Debug condition
											// Local note repeat prohibited
			for (int i = sp1 - 1; i < ep2 - 1; i++) {
				if (c[i] == c[i + 1]) goto skip;
			}
			nmin = 0;
			nmax = 0;
			// Count limits
			for (int i = 0; i < ep2; i++) {
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
			for (int i = 0; i < ep2; i++) {
				nflagsc[i] = 0;
				// Calculate chromatic positions
				cc[i] = dia_to_chrom[(c[i] + 56) % 7] + (((c[i] + 56) / 7) - 8) * 12 + first_note; // Negative eight octaves reserve
																																													 // Calculate pitch class
				pc[i] = (c[i] + 56) % 7;
			}
			// Wrong second to last note
			if (ep2 > c_len - 2)
				if ((pc[c_len - 2] == 0) || (pc[c_len - 2] == 2) || (pc[c_len - 2] == 3) || (pc[c_len - 2] == 5)) FLAG(13, c_len - 2);
			// Wrong third to last note
			if (ep2 > c_len - 3) {
				if ((pc[c_len - 3] == 0) || (pc[c_len - 3] == 2) || (pc[c_len - 3] == 4)) FLAG(14, c_len - 3);
				// Leading third to last note
				if (pc[c_len - 3] == 6) FLAG(34, c_len - 3);
			}
			dcount = 0;
			scount = 0;
			tcount = 0;
			wdcount = 0;
			wscount = 0;
			wtcount = 0;
			for (int i = 0; i < ep2; i++) {
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
			if ((tcount == 3) || (dcount == 3) || (scount == 3)) FLAG(15, ep2 - 1);
			if ((tcount == 4) || (dcount == 4) || (scount == 4)) FLAG(16, ep2 - 1);
			if ((tcount > 4) || (dcount > 4) || (scount > 4)) FLAG(17, ep2 - 1);
			// Check missing letters
			if ((wtcount == 4) || (wdcount == 4) || (wscount == 4)) FLAG(18, ep2 - 1);
			if ((wtcount == 5) || (wdcount == 5) || (wscount == 5)) FLAG(19, ep2 - 1);
			if ((wtcount > 5) || (wdcount > 5) || (wscount > 5)) FLAG(20, ep2 - 1);
			for (int i = 0; i < ep2 - 1; i++) {
				// Tritone prohibit
				if (abs(cc[i + 1] - cc[i]) == 6) {
					// Check if tritone is highest leap if this is last window
					if (ep2 == c_len)
						if ((c[i] == nmax) || (c[i + 1] == nmax)) FLAG(32, i)
							// Check if tritone is first or last step
							if (i > c_len - 3) FLAG(31, i)
								//if (i < 1) FLAG(31, i);
								// Check if resolution is correct
							else if (i < ep2 - 2) {
								if (cc[i + 1] % 12 == 5) FLAG(31, i)
								else if (cc[i + 2] % 12 != 0) FLAG(31, i)
								else if (cc[i - 1] % 12 != 4) FLAG(31, i)
									// Record resolved tritone
								else FLAG(2, i);
							}
					// Do not check tritone if it is at the end of not-last window (after ep2 - 2)
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
			// Search for outstanding repeats
			if (ep2 > 6) for (int i = 0; i < ep2 - 6; i++) {
				// Check if note changes direction or is a leap
				if ((i == 0) || (leap[i - 1]) || ((c[i] - c[i - 1])*(c[i + 1] - c[i]) < 0)) {
					// Search for repeat of note at same beat until last three notes
					int finish = i + repeat_steps;
					if (finish > ep2 - 2) finish = ep2 - 2;
					for (int x = i + 2; x < finish; x += 2) {
						// Check if same note with direction change or leap
						if ((c[x] == c[i]) && ((leap[x - 1]) || ((c[x] - c[x - 1])*(c[x + 1] - c[x]) < 0))) {
							// Check that two more notes repeat
							if ((c[x + 1] == c[i + 1]) && (c[x + 2] == c[i + 2])) {
								FLAG(36, i);
							}
						}
					}
				}
			}
			for (int i = 0; i < ep2 - 1; i++) {
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
					// Do not check fill if search window is cut by end of current not-last scan window
					if ((pos < ep2) || (c_len == ep2)) {
						if (pos > ep2 - 1) pos = ep2 - 1;
						// Zero array
						fill(nstat2.begin(), nstat2.end(), 0);
						fill(nstat3.begin(), nstat3.end(), 0);
						// Fill all notes
						for (int x = 0; x < ep2; x++) {
							// Update local fill
							if ((x > i + 1) && (x <= pos)) nstat3[c[x] - nmin]++;
							// Update global fill
							nstat2[c[x] - nmin]++;
						}
						// Check if leap is filled
						ok = 1; // Local fill
						ok2 = 1; // Global fill
						int pos1 = min(c[i], c[i + 1]);
						int pos2 = max(c[i], c[i + 1]);
						for (int x = pos1 + 1; x < pos2; x++) if (!nstat3[x - nmin]) {
							ok = 0;
							if (!nstat2[x - nmin]) ok2 = 0;
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
				if (i < ep2 - 2) {
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
							if ((i>0) && ((leap[i] * (c[i] - c[i - 1]) == -5) || (leap[i] * (c[i] - c[i - 1]) == -7))) FLAG(28, i)
							else FLAG(6, i);
						}
					}
					// Check if melody direction does not change after leap
					else if (leap[i] * (c[i + 2] - c[i + 1]) > 0) {
						// If this 3rd was prepared
						if (third_prepared) FLAG(30, i)
						else {
							if (i < ep2 - 3) {
								// Check if melody direction does not change second note after leap
								if (leap[i] * (c[i + 3] - c[i + 2]) > 0) FLAG(26, i)
									// If direction changes second note after leap
								else {
									// Check leap size
									if (abs(c[i + 1] - c[i]) > 4) FLAG(29, i)
									else FLAG(7, i);
								}
							}
							else {
								// Mark leap unresolved if this is end of cantus
								if (c_len == ep2) FLAG(26, i);
							}
						}
					}
					// If melody direction changes after leap
					else {
						// Check if it is a leap to leap resolution
						if (leap[i] * leap[i + 1] != 0) {
							// Check if leap returns to same note
							if ((leap[i] != 0) && (leap[i + 1] != 0) && (c[i] == c[i + 2])) {
								if (abs(c[i] - c[i + 1]) > 3) FLAG(22, i)
								else FLAG(8, i);
							}
							// Check if two thirds go after leap
							else if ((i < ep2 - 3) && (leap[i + 1] == leap[i + 2]) && (abs(c[i + 3] - c[i + 1]) == 4)) {
								// Do nothing (leap will be marked later)
							}
							// Else it is a simple leap-to-leap
							else FLAG(33, i);
						}
					}
					// Check if two notes repeat
					if ((i > 0) && (c[i] == c[i + 2]) && (c[i - 1] == c[i + 1])) FLAG(9, i);
				}
			}
			// Check if too many leaps
			if (max_leap_sum > max_leaps) {
				if (max_leap_sum > max_leaps2) FLAG(25, leap_sum_i)
				else FLAG(3, leap_sum_i);
			}
			// Clear nstat
			for (int i = 0; i <= max_interval * 2; i++) {
				nstat[i] = 0;
			}
			// Prohibit stagnation
			for (int i = 0; i < ep2; i++) {
				// Add new note
				nstat[c[i] - nmin]++; // Stagnation array
															// Subtract old note
				if ((i >= stag_note_steps)) nstat[c[i - stag_note_steps] - nmin]--;
				// Check if too many repeating notes
				if (nstat[c[i] - nmin] > stag_notes) FLAG(10, i);
			}
			// Prohibit multiple culminations
			culm_sum = 0;
			for (int i = 0; i < ep2; i++) {
				if (c[i] == nmax) {
					culm_sum++;
					culm_step = i;
					if (culm_sum > 1) FLAG(12, i);
				}
			}
			// Prohibit culminations at last steps
			if (culm_step > c_len - 4) FLAG(21, culm_step);
			// Prohibit last leap
			if (ep2 == c_len)
				if (leap[c_len - 2]) FLAG(23, c_len - 1);
			accepted2++;
			// Calculate flag statistics
			if (calculate_stat || calculate_correlation) {
				if (ep2 == c_len) for (int i = 0; i < MAX_FLAGS; i++) {
					if (flags[i]) {
						fstat[i]++;
						// Calculate correlation
						if (calculate_correlation) for (int z = 0; z < MAX_FLAGS; z++) {
							if (flags[z]) fcor[i][z]++;
						}
					}
				}
			}
			// Calculate flag blocking
			if (calculate_blocking) {
				int flags_found = 0;
				int flags_found2 = 0;
				int flags_conflict = 0;
				// Find if any of accepted flags set
				for (int i = 0; i < MAX_FLAGS; i++) {
					if ((flags[i]) && (accept[i])) flags_found++;
					if ((flags[i]) && (!accept[i])) flags_conflict++;
					if ((flags[i]) && (accept[i] == 2)) flags_found2++;
				}
				// Skip only if flags required
				if ((!late_require) || (ep2 == c_len)) {
					// Check if no needed flags set
					if (flags_found == 0) goto skip;
					// Check if not enough 2 flags set
					if (flags_found2 < flags_need2) goto skip;
				}
				accepted5[wid]++;
				// Find flags that are blocking
				for (int i = 0; i < MAX_FLAGS; i++) {
					if ((flags[i]) && (!accept[i]))
						fblock[wid][flags_conflict][i]++;
				}
			}
			// Check if flags are accepted
			for (int i = 0; i < MAX_FLAGS; i++) {
				if ((flags[i]) && (!accept[i])) goto skip;
				if ((!late_require) || (ep2 == c_len))
					if ((!flags[i]) && (accept[i] == 2)) goto skip;
			}
			accepted4[wid]++;
			// If this is not last window, go to next window
			if (ep2 < c_len) {
				sp1 = sp2;
				sp2 = sp1 + s_len; // End of search window
				if (sp2 > c_len - 1) sp2 = c_len - 1;
				// Reserve last window with maximum length
				if ((c_len - sp1 - 1 < s_len * 2) && (c_len - sp1 - 1 > s_len)) sp2 = (c_len + sp1) / 2;
				// Record window
				wid++;
				wpos1[wid] = sp1;
				wpos2[wid] = sp2;
				wscans[wid]++;
				// End of evaluation window
				ep2 = sp2;
				// Add last note if this is last window
				if (ep2 == c_len - 1) ep2 = c_len;
				// Go to rightmost element
				p = sp2 - 1;
				if (wcount < wid + 1) {
					wcount = wid + 1;
					if (ep2 == c_len) {
						// Show window statistics
						CString* est = new CString;
						CString st, st2;
						for (int i = 0; i < wcount; i++) {
							if (i > 0) st2 += ", ";
							st.Format("%d-%d", wpos1[i], wpos2[i]);
							st2 += st;
						}
						est->Format("Algorithm created %d windows: %s", wcount, st2);
						WriteLog(3, est);
					}
				}
				goto skip;
			}
			// Check random_choose
			if (random_choose < 100) if (rand2() >= (double)RAND_MAX*random_choose / 100.0) goto skip;
			// Accept cantus
			accepted++;
			if (accepted >= t_cnt) {
				WriteLog(3, "Generation reached t_cnt. Breaking.");
				break;
			}
			else {
				SendCantus(c, cc, nflags, nflagsc);
			} // t_cnt limit
		} // cycle debug condition
	skip:
		while (true) {
			if (c[p] < max_interval) break;
			// If current element is max, make it minimum
			c[p] = -max_interval;
			// Move left one element
			if (p == sp1) {
				finished = 1;
				break;
			}
			p--;
		}
		if (finished) {
			// Finish if this is last variant in first window
			if ((p == 1) || (wid == 0)) {
				// If we started from random seed, allow one more full cycle
				if (random_seed) {
					if (seed_cycle) break;
					WriteLog(3, "Random seed allows one more full cycle: restarting");
					seed_cycle++;
				}
				else break;
			}
			// Clear current window
			for (int i = sp1; i < sp2; i++) c[i] = -max_interval;
			// If this is not first window, go to previous window
			if (wid > 0) wid--;
			sp1 = wpos1[wid];
			sp2 = wpos2[wid];
			// End of evaluation window
			ep2 = sp2;
			// Add last note if this is last window
			if (ep2 == c_len - 1) ep2 = c_len;
			// Go to rightmost element
			p = sp2 - 1;
			finished = 0;
		}
		// Increase rightmost element, which was not reset to minimum
		c[p]++;
		// Go to rightmost element
		p = sp2 - 1;
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
				st.Format("%lld; ", fcor[f1][f2]);
				st2 += st;
			}
			CGLib::AppendLineToFile("cf1-cor.csv", st2 + "\n");
		}
		CGLib::AppendLineToFile("cf1-cor.csv", st3 + "\n");
	}
	// Show flag statistics
	if (calculate_stat) {
		CString* est = new CString;
		for (int i = 0; i < MAX_FLAGS; i++) {
			int f1 = SeverityFlag[i];
			st.Format("\n%lld %s ", fstat[f1], FlagName[f1]);
			st2 += st;
		}
		est->Format("%d/%d: Accepted %lld/%lld/%lld/%lld variants of %lld: %s",
			c_len, max_interval, accepted4[wcount - 1], accepted, accepted2,
			accepted3, cycle, st2);
		WriteLog(3, est);
	}
	// Show blocking statistics
	if (calculate_blocking) {
		for (int w = 0; w < wcount; w++) {
			int lines = 0;
			CString* est = new CString;
			st2 = "";
			for (int d = 1; d < MAX_FLAGS; d++) {
				if (lines > 100) break;
				int flagc = 0;
				for (int x = 0; x < MAX_FLAGS; x++) {
					if (fblock[w][d][x] > 0) flagc++;
				}
				if (!flagc) continue;
				int max_flag = 0;
				long max_value = -1;
				st.Format("\nTIER %d: ", d);
				st2 += st;
				for (int x = 0; x < MAX_FLAGS; x++) {
					max_value = -1;
					// Find biggest value
					for (int i = 0; i < MAX_FLAGS; i++) {
						if (fblock[w][d][i] > max_value) {
							max_value = fblock[w][d][i];
							max_flag = i;
						}
					}
					if (max_value < 1) break;
					st.Format("\n%ld %s, ", max_value, FlagName[max_flag]);
					st2 += st;
					lines++;
					// Clear biggest value to search for next
					fblock[w][d][max_flag] = -1;
				}
			}
			est->Format("Window %d: %lld scans, %lld of %lld variants blocked: %s", w, wscans[w], accepted5[w] - accepted4[w], accepted5[w], st2);
			WriteLog(3, est);
		}
	}
}

void CGenCF1::SendCantus(vector<char> &c, vector<unsigned char> &cc, vector<vector<unsigned char>> &nflags, vector<unsigned char> &nflagsc) {
	CString st;
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
			comment[x][0] += FlagName[nflags[x - step][i]];
			st.Format(" [%d]", flag_sev[nflags[x - step][i]]);
			if (show_severity) comment[x][0] += st;
			comment[x][0] += ". ";
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
		if (x < step + c_len / 2)	dyn[x][0] = 60 + 40 * (x - step) / c_len + 20 * rand2() / RAND_MAX;
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
}

void CGenCF1::Generate()
{
	ScanCantus();
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
		est->Format("Shuffle of %lld melodies finished", accepted);
		WriteLog(3, est);
	}
}
