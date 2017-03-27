#include "stdafx.h"
#include "GAdapt.h"


CGAdapt::CGAdapt()
{
	play_transpose.resize(MAX_VOICE);
	warning_note_range.resize(MAX_VOICE);
	warning_note_wrong.resize(MAX_VOICE);
	warning_note_short.resize(MAX_VOICE);
}


CGAdapt::~CGAdapt()
{
}

void CGAdapt::CheckRange(int v, int ii)
{
	// Check if notes are in instrument range
	if ((ngv_min[v] + play_transpose[v] < instr_nmin[ii]) || (ngv_max[v] + play_transpose[v] > instr_nmax[ii])) {
		if (ngv_min[v] < instr_nmin[ii]) {
			play_transpose[v] = ((instr_nmin[ii] - ngv_min[v]) / 12) * 12 + 12;
		}
		if (ngv_max[v] > instr_nmax[ii]) {
			play_transpose[v] = -((ngv_max[v] - instr_nmax[ii]) / 12) * 12 - 12;
		}
		// Check if still have problem
		CString* st = new CString;
		if ((ngv_min[v] + play_transpose[v] < instr_nmin[ii]) || (ngv_max[v] + play_transpose[v] > instr_nmax[ii])) {
			if (!warning_note_range[v]) {
				st->Format("Generated notes range (%s - %s) is outside instrument range (%s - %s). Cannot transpose automatically: range too wide.",
					GetNoteName(ngv_min[v]), GetNoteName(ngv_max[v]), GetNoteName(instr_nmin[ii]),
					GetNoteName(instr_nmax[ii]), play_transpose[v]);
				warning_note_range[v] = 1;
				WriteLog(1, st);
			}
		}
		else {
			st->Format("Generated notes range (%s - %s) is outside instrument range (%s - %s). Transposed automatically to %d semitones. Consider changing instrument or generation range.",
				GetNoteName(ngv_min[v]), GetNoteName(ngv_max[v]), GetNoteName(instr_nmin[ii]),
				GetNoteName(instr_nmax[ii]), play_transpose[v]);
			WriteLog(1, st);
		}
	}
}

void CGAdapt::CheckShortStep(int v, int x, int i, int ii, int ei, int pi, int pei)
{
	// Check if note is too short
	int ndur = (etime[ei] - stime[i]) * 100 / m_pspeed;
	if (ndur < instr_tmin[ii]) {
		CString* st = new CString;
		if (warning_note_short[v] < 4) {
			st->Format("Recommended minimum note length for %s instrument is %d ms. In voice %d note length at step %d is %d ms. Try to change playback speed, instrument or algorithm config.",
				InstName[ii], instr_tmin[ii], v, i, ndur);
			warning_note_short[v] ++;
			WriteLog(1, st);
		}
	}
}

void CGAdapt::AdaptLengroupStep(int v, int x, int i, int ii, int ei, int pi, int pei)
{
	// Check lengroups
	if ((i > 0) && (lengroup2[ii] + lengroup3[ii] + lengroup4[ii] > 0)) {
		if (lengroup[pi][v] < 2) {
			// Start new lengroup if was no lengroup or lengroup ended
			int r = randbw(0, 100);
			if (r < lengroup2[ii]) lengroup[i][v] = 2;
			else if (r < lengroup2[ii] + lengroup3[ii]) lengroup[i][v] = 3;
			else if (r < lengroup2[ii] + lengroup3[ii] + lengroup4[ii]) lengroup[i][v] = 4;
			else lengroup[i][v] = 0;
		}
		else if (i > 0) {
			// Continue lengroup
			lengroup[i][v] = lengroup[pi][v] - 1;
		}
		// Apply lengroups
		if (lengroup[i][v] > 1) {
			if (lengroup_edt1[ii] < 0) {
				detime[ei][v] = -min(-lengroup_edt1[ii], (etime[ei] - stime[i]) * 100 / m_pspeed / 3);
				artic[i][v] = ARTIC_NONLEGATO;
			}
			else {
				if ((i > 0) && (note[pi][v] == note[i][v])) detime[ei][v] = -10;
				detime[ei][v] = lengroup_edt1[ii];
				artic[i][v] = ARTIC_LEGATO;
			}
		}
		if (lengroup[i][v] == 1) {
			if (lengroup_edt2[ii] < 0) {
				detime[ei][v] = -min(-lengroup_edt2[ii], (etime[ei] - stime[i]) * 100 / m_pspeed / 3);
				artic[i][v] = ARTIC_NONLEGATO;
			}
			else {
				detime[ei][v] = lengroup_edt2[ii];
				artic[i][v] = ARTIC_LEGATO;
			}
		}
	}
}

void CGAdapt::AdaptSlurStep(int v, int x, int i, int ii, int ei, int pi, int pei)
{
	// Add slurs
	if ((i > 0) && (max_slur_interval[ii] > 0) && (abs(note[pi][v] - note[i][v]) <= max_slur_interval[ii]) && (note[pi][v] != note[i][v]) &&
		(slur_count <= max_slur_count[ii])) {
		artic[i][v] = ARTIC_SLUR;
		slur_count++;
	}
	else {
		slur_count = 0;
	}
}

void CGAdapt::AdaptRetriggerStep(int v, int x, int i, int ii, int ei, int pi, int pei)
{
	// Retrigger notes
	if ((i > 0) && (note[pi][v] == note[i][v])) {
		artic[i][v] = ARTIC_RETRIGGER;
		detime[pei][v] = -1;
		dstime[i][v] = 0;
		// Replace retrigger with non-legato
		if ((retrigger_freq[ii] > 0) && (randbw(0, 100) > retrigger_freq[ii])) {
			detime[pei][v] = -min(300, (etime[pei] - stime[pei]) * 100 / m_pspeed / 3);
			artic[i][v] = ARTIC_NONLEGATO;
		}
	}
}

void CGAdapt::AdaptNonlegatoStep(int v, int x, int i, int ii, int ei, int pi, int pei)
{
	// Randomly make some notes non-legato if they have enough length
	if ((i > 0) && ((etime[pei] - stime[pi]) * 100 / m_pspeed + detime[pei][v] - dstime[pi][v] > nonlegato_minlen[ii]) &&
		(randbw(0, 100) < nonlegato_freq[ii])) {
		detime[pei][v] = -min(300, (etime[pei] - stime[pei]) * 100 / m_pspeed / 3);
		dstime[i][v] = 0;
		artic[i][v] = ARTIC_NONLEGATO;
	}
}

void CGAdapt::AdaptAheadStep(int v, int x, int i, int ii, int ei, int pi, int pei)
{
	// Advance start for legato (not longer than previous note length)
	if ((i > 0) && (legato_ahead[ii] > 0) && (artic[i][v] == ARTIC_SLUR || artic[i][v] == ARTIC_LEGATO) && (detime[i - 1][v] >= 0) && (!pause[pi][v])) {
		dstime[i][v] = -min(legato_ahead[ii], (etime[i - 1] - stime[pi]) * 100 / m_pspeed +
			detime[i - 1][v] - dstime[pi][v] - 1);
		detime[i - 1][v] = 0.9 * dstime[i][v];
	}
}

void CGAdapt::FixOverlap(int v, int x, int i, int ii, int ei, int pi, int pei)
{
	// Check if note overlapping occured
	if (i > 0) {
		int lpi = pi; // Local previous id
		// Cycle through all notes backwards
		while (true) {
			if (note[lpi][v] == note[i][v]) {
				int lpei = lpi + len[lpi][v] - 1;
				if ((stime[i] - etime[lpei]) * 100 / m_pspeed + dstime[i][v] - detime[lpei][v] < 1) {
					//dstime[i][v] = (etime[lpei] - stime[i]) + detime[lpei] + 1;
					// Move ending of previous note to the left
					detime[lpei][v] = (stime[i] - etime[lpei]) * 100 / m_pspeed + dstime[i][v] - 1;
				}
				break;
			}
			if (poff[lpi][v] == 0) break;
			lpi = lpi - poff[lpi][v];
		}
	}
}

void CGAdapt::Adapt(int step1, int step2)
{
	int ei; // ending step
	int pi; // previous note step
	int pei; // previous note ending step
	for (int v = 0; v < v_cnt; v++) {
		int ii = instr[v]; // Instrument id
		int ncount = 0;
		// Move to note start
		if (coff[step1][v] > 0) step1 = step1 - coff[step1][v];
		// Count notes
		for (int i = step1; i <= step2; i++) {
			if (i + len[i][v] > step2 + 1) break;
			ncount++;
			if (noff[i][v] == 0) break;
			i += noff[i][v] - 1;
		}
		// Set vel to dyn
		for (int i = step1; i <= step2; i++) {
			vel[i][v] = dyn[i][v];
		}
		CheckRange(v, ii);
		// Calculate delta: dstime / detime
		slur_count = 0;
		int i = step1;
		for (int x = 0; x < ncount; x++) {
			ei = i + len[i][v] - 1;
			pi = i - poff[i][v];
			pei = i - 1;
			if (!pause[i][v]) {
				CheckShortStep(v, x, i, ii, ei, pi, pei);
				// Instrument-specific adaptation
				if (instr_type[ii] == 0) AdaptLengroupStep(v, x, i, ii, ei, pi, pei);
				if (instr_type[ii] == 1) {
					AdaptSlurStep(v, x, i, ii, ei, pi, pei);
					AdaptRetriggerStep(v, x, i, ii, ei, pi, pei);
					AdaptNonlegatoStep(v, x, i, ii, ei, pi, pei);
					AdaptAheadStep(v, x, i, ii, ei, pi, pei);
				}
				// Randomize note starts
				if (rand_start[ii] > 0) dstime[i][v] += (rand01() - 0.5) * (etime[ei] - stime[i]) * 100 / m_pspeed * rand_start[ii] / 100;
				// Randomize note ends
				if (rand_end[ii] > 0) detime[ei][v] += (rand01() - 0.5) * (etime[ei] - stime[i]) * 100 / m_pspeed * rand_end[ii] / 100;
				FixOverlap(v, x, i, ii, ei, pi, pei);
			} // !pause
			if (noff[i][v] == 0) break;
			i += noff[i][v];
		} // for x
	} // for v
}

