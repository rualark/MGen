#include "stdafx.h"
#include "GAdapt.h"


CGAdapt::CGAdapt()
{
	play_transpose.resize(MAX_VOICE);
	warning_note_range.resize(MAX_VOICE);
	warning_note_wrong.resize(MAX_VOICE);
	warning_note_short.resize(MAX_VOICE);
	warning_poly.resize(MAX_INSTR);
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
				st->Format("Generated notes range (%s - %s) is outside instrument %s/%s (voice %d) range (%s - %s). Cannot transpose automatically: range too wide.",
					GetNoteName(ngv_min[v]), GetNoteName(ngv_max[v]), 
					InstGName[ii], InstCName[ii], v,
					GetNoteName(instr_nmin[ii]), GetNoteName(instr_nmax[ii]), play_transpose[v]);
				warning_note_range[v] = 1;
				WriteLog(1, st);
			}
		}
		else {
			st->Format("Generated notes range (%s - %s) is outside instrument %s/%s (voice %d) range (%s - %s). Transposed automatically to %d semitones. Consider changing instrument or generation range.",
				GetNoteName(ngv_min[v]), GetNoteName(ngv_max[v]), 
				InstGName[ii], InstCName[ii], v,
				GetNoteName(instr_nmin[ii]), GetNoteName(instr_nmax[ii]), play_transpose[v]);
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
				InstGName[ii], instr_tmin[ii], v, i, ndur);
			warning_note_short[v] ++;
			WriteLog(1, st);
			if (comment_adapt) adapt_comment[i][v] += "Too short note. ";
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
				if (comment_adapt) adapt_comment[i][v] += "Lengroup edt1 nonlegato. ";
			}
			else {
				if ((i > 0) && (note[pi][v] == note[i][v])) detime[ei][v] = -10;
				detime[ei][v] = lengroup_edt1[ii];
				artic[i][v] = ARTIC_LEGATO;
				if (comment_adapt) adapt_comment[i][v] += "Lengroup edt1 legato. ";
			}
		}
		if (lengroup[i][v] == 1) {
			if (lengroup_edt2[ii] < 0) {
				detime[ei][v] = -min(-lengroup_edt2[ii], (etime[ei] - stime[i]) * 100 / m_pspeed / 3);
				artic[i][v] = ARTIC_NONLEGATO;
				if (comment_adapt) adapt_comment[i][v] += "Lengroup edt2 nonlegato. ";
			}
			else {
				detime[ei][v] = lengroup_edt2[ii];
				artic[i][v] = ARTIC_LEGATO;
				if (comment_adapt) adapt_comment[i][v] += "Lengroup edt2 legato. ";
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
		if (comment_adapt) adapt_comment[i][v] += "Slur. ";
	}
	else {
		slur_count = 0;
	}
}

void CGAdapt::AdaptRetriggerRebowStep(int v, int x, int i, int ii, int ei, int pi, int pei)
{
	// Retrigger notes
	if ((i > 0) && (pi < i) && (note[pi][v] == note[i][v])) {
		float ndur = (etime[ei] - stime[i]) * 100 / m_pspeed + detime[ei][v] - dstime[i][v];
		// Replace retrigger with non-legato
		if (((retrigger_freq[ii] > 0) && (randbw(0, 100) > retrigger_freq[ii]))
			|| (ndur < retrigger_min_len[ii])) {
			int max_shift = (etime[pei] - stime[pi]) * 100 / m_pspeed * (float)retrigger_rand_end[ii] / 100.0;
			if (max_shift > retrigger_rand_max[ii]) max_shift = retrigger_rand_max[ii];
			detime[pei][v] = -randbw(0, max_shift);
			artic[i][v] = ARTIC_NONLEGATO;
			if (comment_adapt) adapt_comment[i][v] += "Retrigger nonlegato. ";
		}
		else {
			if (comment_adapt) adapt_comment[i][v] += "Rebow retrigger. ";
			artic[i][v] = ARTIC_REBOW;
			detime[pei][v] = -1;
			dstime[i][v] = 0;
		}
	}
}

void CGAdapt::AdaptRetriggerNonlegatoStep(int v, int x, int i, int ii, int ei, int pi, int pei)
{
	// Retrigger notes
	if ((i > 0) && (pi < i) && (note[pi][v] == note[i][v])) {
		float ndur = (etime[ei] - stime[i]) * 100 / m_pspeed + detime[ei][v] - dstime[i][v];
		// Replace retrigger with non-legato
		if (((retrigger_freq[ii] > 0) && (randbw(0, 100) > retrigger_freq[ii]))
			|| (ndur < retrigger_min_len[ii])) {
			int max_shift = (etime[pei] - stime[pi]) * 100 / m_pspeed * (float)retrigger_rand_end[ii] / 100.0;
			if (max_shift > retrigger_rand_max[ii]) max_shift = retrigger_rand_max[ii];
			detime[pei][v] = -randbw(0, max_shift);
			artic[i][v] = ARTIC_NONLEGATO;
			if (comment_adapt) adapt_comment[i][v] += "Retrigger nonlegato. ";
		}
		else {
			if (comment_adapt) adapt_comment[i][v] += "Retrigger. ";
			artic[i][v] = ARTIC_RETRIGGER;
			detime[pei][v] = -1;
			dstime[i][v] = 0;
		}
	}
}

void CGAdapt::AdaptNonlegatoStep(int v, int x, int i, int ii, int ei, int pi, int pei)
{
	// Randomly make some notes non-legato if they have enough length
	if ((i > 0) && ((etime[pei] - stime[pi]) * 100 / m_pspeed + detime[pei][v] - dstime[pi][v] > nonlegato_minlen[ii]) &&
		(randbw(0, 100) < nonlegato_freq[ii] * pow(abs(note[i][v] - note[pi][v]), 0.3))) {
		detime[pei][v] = -min(300, (etime[pei] - stime[pi]) * 100 / m_pspeed / 3);
		dstime[i][v] = 0;
		artic[i][v] = ARTIC_NONLEGATO;
		if (comment_adapt) adapt_comment[i][v] += "Random nonlegato. ";
	}
}

void CGAdapt::AdaptAheadStep(int v, int x, int i, int ii, int ei, int pi, int pei)
{
	// Advance start for legato (not longer than previous note length)
	if ((i > 0) && (pi < i) && (legato_ahead[ii] > 0) && (artic[i][v] == ARTIC_SLUR || artic[i][v] == ARTIC_LEGATO) &&
		(detime[i - 1][v] >= 0) && (!pause[pi][v]) && (abs(note[i][v] - note[i - 1][v]) <= max_ahead_note[ii])) {
		dstime[i][v] = -min(legato_ahead[ii], (etime[i - 1] - stime[pi]) * 100 / m_pspeed +
			detime[i - 1][v] - dstime[pi][v] - 1);
		detime[i - 1][v] = 0.9 * dstime[i][v];
		if (comment_adapt) adapt_comment[i][v] += "Ahead start. ";
		if (comment_adapt) adapt_comment[i - 1][v] += "Ahead end. ";
		// Add glissando if note is long
		float ndur = (etime[ei] - stime[i]) * 100 / m_pspeed + detime[ei][v] - dstime[i][v];
		if ((ndur > gliss_minlen[ii]) && (randbw(0, 100) < gliss_freq[ii])) {
			vel[i][v] = vel_gliss[ii];
			if (comment_adapt) adapt_comment[i][v] += "Gliss. ";
		}
	}
}

// For Samplemodeling
void CGAdapt::AdaptFlexAheadStep(int v, int x, int i, int ii, int ei, int pi, int pei)
{
	// Advance start for legato (not longer than previous note length)
	if ((i > 0) && (pi < i) && (legato_ahead[ii]) && (artic[i][v] == ARTIC_SLUR || artic[i][v] == ARTIC_LEGATO) &&
		(detime[i - 1][v] >= 0) && (!pause[pi][v]) && (abs(note[i][v] - note[i - 1][v]) <= max_ahead_note[ii])) {
		// Get current note length
		float ndur = (etime[ei] - stime[i]) * 100 / m_pspeed + detime[ei][v] - dstime[i][v];
		// Get previous note length
		float pdur = (etime[pei] - stime[pi]) * 100 / m_pspeed + detime[pei][v] - dstime[pi][v];
		// Get maximum legato_ahead possible
		float max_adur = min(ndur * leg_cdur[ii] / 100.0, pdur * leg_pdur[ii] / 100.0);
		// Get minimum velocity possible
		float min_vel = max(1, 128 - 
			pow(max_adur * pow(127, legato_ahead_exp[ii]) / legato_ahead[ii], 1 / legato_ahead_exp[ii]));
		// Make random velocity inside allowed range
		vel[i][v] = randbw(min_vel, min(min_vel + 50, 127));
		// Get ahead duration
		float adur = pow(128 - vel[i][v], legato_ahead_exp[ii]) * legato_ahead[ii] / pow(127, legato_ahead_exp[ii]);
		// Move notes
		dstime[i][v] = -adur;
		detime[i - 1][v] = 0.9 * dstime[i][v];
		// Add comments
		if (comment_adapt) adapt_comment[i][v] += "Ahead flex start. ";
		if (comment_adapt) adapt_comment[i - 1][v] += "Ahead flex end. ";
		// Select articulation
		if (adur > splitpo_mindur[ii] && abs(note[pi][v] - note[i][v]) > 1 && abs(note[pi][v] - note[i][v]) < 13 &&
			randbw(0, 100) < splitpo_freq[ii]) {
			// How many chromatic pitches per second
			float nspeed = abs(note[i][v] - note[pi][v]) / adur * 1000.0;
			if (nspeed < 8) {
				artic[i][v] = ARTIC_SPLITPO_CHROM;
				if (comment_adapt) adapt_comment[i][v] += "Split portamento chromatic. ";
			}
			else if (abs(note[pi][v] - note[i][v]) > splitpo_pent_minint[ii]) {
				artic[i][v] = ARTIC_SPLITPO_PENT;
				if (comment_adapt) adapt_comment[i][v] += "Split portamento pentatonic. ";
			}
		}
		else if (adur > gliss_mindur[ii] && abs(note[pi][v] - note[i][v]) > 1 && abs(note[pi][v] - note[i][v]) < 6 &&
			randbw(0, 100) < gliss_freq[ii]/(100-splitpo_freq[ii]+0.001)*100) {
			if (randbw(0, 100) < 50) {
				artic[i][v] = ARTIC_GLISS2;
				if (comment_adapt) adapt_comment[i][v] += "Gliss2. ";
			}
		}
	}
}

void CGAdapt::FixOverlap(int v, int x, int i, int ii, int ei, int pi, int pei)
{
	// Check if note overlapping occured
	if (i > 0) {
		// Local previous id
		int lpi = pi; 
		// Cycle through all notes backwards
		while (lpi >= 0) {
			if (note[lpi][v] == note[i][v]) {
				int lpei = lpi + len[lpi][v] - 1;
				if ((stime[i] - etime[lpei]) * 100 / m_pspeed + dstime[i][v] - detime[lpei][v] < 1) {
					//dstime[i][v] = (etime[lpei] - stime[i]) + detime[lpei] + 1;
					// Move ending of previous note to the left
					detime[lpei][v] = (stime[i] - etime[lpei]) * 100 / m_pspeed + dstime[i][v] - 1;
					if (comment_adapt) adapt_comment[lpei][v] += "Ending overlap fixed. ";
				}
				break;
			}
			if (poff[lpi][v] == 0) break;
			lpi = lpi - poff[lpi][v];
		}
	}
}

void CGAdapt::AdaptAttackStep(int v, int x, int i, int ii, int ei, int pi, int pei)
{
	// If nonlegato and short note, avoid slow sustain articulations for Friedlander violin
	if (artic[i][v] == ARTIC_NONLEGATO) {
		float ndur = (etime[ei] - stime[i]) * 100 / m_pspeed + detime[ei][v] - dstime[i][v];
		if (ndur < vel_normal_minlen[ii]) {
			vel[i][v] = randbw(vel_immediate[ii], 127);
			if (comment_adapt) adapt_comment[i][v] += "Vel random over normal. ";
		}
		//if (ndur < vel_normal_minlen[ii]) vel[i][v] = dyn[i][v] * (float)(127 - vel_immediate[ii]) / 127.0 + vel_immediate[ii];
		//else vel[i][v] = dyn[i][v] * (float)(vel_immediate[ii] - 1) / 127.0;
	}
}

void CGAdapt::AdaptLongBell(int v, int x, int i, int ii, int ei, int pi, int pei, int ncount)
{
	float ndur = (etime[ei] - stime[i]) * 100 / m_pspeed + detime[ei][v] - dstime[i][v];
	// Create bell if long length, not high velocity, after pause or first note
	if ((ndur > bell_mindur[ii]) && (len[i][v] > 2) && (!i || pause[pi][v]) && vel[i][v] < 120) {
		int pos = i + (float)(len[i][v]) * bell_start_len[ii] / 100.0;
		int ok = 1;
		// Check if dynamics is even
		if (pos - i > 1) for (int z = i + 1; z < pos; z++) {
			if (dyn[z][v] != dyn[z - 1][v]) {
				ok = 0;
				break;
			}
		}
		if (ok) {
			for (int z = i; z < pos; z++) {
				dyn[z][v] = dyn[z][v] * (bell_start_mul[ii] + (float)(z - i) / (pos - i) * (1.0 - bell_start_mul[ii]));
			}
			if (comment_adapt) adapt_comment[i][v] += "Long bell start. ";
		}
	}
	int ni = i + noff[i][v];
	// Create bell if long length, not pause and not last note (because can be just end of adapt window)
	if ((ndur > bell_mindur[ii]) && (len[i][v] > 2) && (x == ncount - 1 || pause[ni][v])) {
		int pos = round(i + (float)(len[i][v]) * 2.0 * bell_start_len[ii] / 100.0);
		int ok = 1;
		int end = i + len[i][v];
		// Check if dynamics is even
		if (end - pos > 1) for (int z = pos; z < end; z++) {
			if (dyn[z][v] != dyn[z - 1][v]) {
				ok = 0;
				break;
			}
		}
		if (ok) {
			for (int z = pos; z < end; z++) {
				dyn[z][v] = dyn[z][v] * (bell_end_mul[ii] + (float)(end - z - 1) / (end - pos) * (1.0 - bell_end_mul[ii]));
			}
			if (comment_adapt) adapt_comment[i + len[i][v] - 1][v] += "Long bell end. ";
		}
	}
}

void CGAdapt::AdaptReverseBell(int v, int x, int i, int ii, int ei, int pi, int pei)
{
	float ndur = (etime[ei] - stime[i]) * 100 / m_pspeed + detime[ei][v] - dstime[i][v];
	int ni = i + noff[i][v];
	// Create rbell if long length and no pauses
	if ((ndur > rbell_mindur[ii]) && (len[i][v] > 2) &&	(randbw(0, 100) < rbell_freq[ii])) {
		int pos1 = i;
		int pos2 = ei;
		int ok = 1;
		// Find even dynamics window
		for (int z = i + 1; z <= (i + ei) / 2; z++) {
			if (dyn[z][v] != dyn[z - 1][v]) {
				pos1 = z;
			}
		}
		for (int z = ei - 1; z >= (i + ei) / 2; --z) {
			if (dyn[z][v] != dyn[z + 1][v]) {
				pos2 = z;
			}
		}
		// Check if window too small
		float ndur2 = (etime[pos2] - stime[pos1]) * 100 / m_pspeed + detime[pos2][v] - dstime[pos1][v];
		if (pos2 - pos1 < 2 || ndur2 < rbell_mindur[ii]) return;
		// Calculate multiplier
		float mul = rbell_mul[ii] - (ndur2 - rbell_mindur[ii]) *
			(rbell_mul[ii] - rbell_mul2[ii]) / (rbell_dur[ii] - rbell_mindur[ii] + 0.0001);
		mul = max(min(mul, rbell_mul[ii]), rbell_mul2[ii]);
		for (int z = pos1; z <= pos2; z++) {
			dyn[z][v] = dyn[z][v] *
				(abs(z - (pos1 + pos2) / 2.0) / (pos2 - pos1) * 2.0 * (1.0 - mul) + mul);
		}
		if (comment_adapt) adapt_comment[i][v] += "Reverse bell. ";
	}
}

void CGAdapt::AdaptVibBell(int v, int x, int i, int ii, int ei, int pi, int pei)
{
	float ndur = (etime[ei] - stime[i]) * 100 / m_pspeed + detime[ei][v] - dstime[i][v];
	int ni = i + noff[i][v];
	// Create rbell if long length and no pauses
	if ((ndur > vib_bell_mindur[ii]) && (len[i][v] > 2) && (randbw(0, 100) < vib_bell_freq[ii])) {
		// Steps range
		int pos1 = i;
		int pos2 = ei + 1;
		// Center positions
		int pos = pos1 + (pos2 - pos1) * randbw(vib_bell_top1[ii], vib_bell_top2[ii]) / 100.0;
		int posf = pos1 + (pos2 - pos1) * randbw(vibf_bell_top1[ii], vibf_bell_top2[ii]) / 100.0;
		int ok = 1;
		// Check if vib and vibf are zero
		for (int z = i; z <= ei; z++) {
			if (vib[z][v] || vibf[z][v]) {
				ok = 0;
				break;
			}
		}
		if (ok) {
			// Calculate allowed maximum
			float vb0 = vib_bell1[ii] + (ndur - vib_bell_mindur[ii]) *
				(vib_bell2[ii] - vib_bell1[ii]) / (vib_bell_dur[ii] - vib_bell_mindur[ii] + 0.0001);
			vb0 = max(min(vb0, vib_bell2[ii]), vib_bell1[ii]);
			float vbf0 = vibf_bell1[ii] + (ndur - vib_bell_mindur[ii]) *
				(vibf_bell2[ii] - vibf_bell1[ii]) / (vib_bell_dur[ii] - vib_bell_mindur[ii] + 0.0001);
			vbf0 = max(min(vbf0, vib_bell2[ii]), vib_bell1[ii]);
			// Calculate random maximum
			float vb = randbw(5, max(5, vb0));
			float vbf = randbw(5, max(5, vbf0));
			// Left part
			for (int z = pos1; z < pos; z++) { 
				vib[z][v] = vb * (float)pow(z - pos1, vib_bell_exp[ii]) / (float)pow(pos - pos1, vib_bell_exp[ii]);
			}
			// Right part
			for (int z = pos; z < pos2; z++) {
				vib[z][v] = vb * (float)pow(pos2 - z, vib_bell_exp[ii]) / (float)pow(pos2 - pos, vib_bell_exp[ii]);
			}
			// Left part speed
			for (int z = pos1; z < posf; z++) {
				vibf[z][v] = vbf * (float)pow(z - pos1, vibf_bell_exp[ii]) / (float)pow(posf - pos1, vibf_bell_exp[ii]);
			}
			// Right part speed
			for (int z = posf; z < pos2; z++) {
				vibf[z][v] = vbf * (float)pow(pos2 - z, vibf_bell_exp[ii]) / (float)pow(pos2 - posf, vibf_bell_exp[ii]);
			}
			if (comment_adapt) adapt_comment[i][v] += "Vibrato bell. ";
		}
	}
}

void CGAdapt::AdaptNoteEndStep(int v, int x, int i, int ii, int ei, int pi, int pei, int ncount)
{
	float ndur = (etime[ei] - stime[i]) * 100 / m_pspeed + detime[ei][v] - dstime[i][v];
	int ni = i + noff[i][v];
	// Create ending articulation only if ending dynamics is low
	if (dyn[ei][v] > 40) return;
	// Check if it is last note in current melody
	if (x == ncount - 1 || pause[ni][v]) {
		if (ndur > end_sfl_dur[ii] * 3 && randbw(0, 100) < end_sfl_freq[ii]) {
			artic[ei][v] = ARTIC_END_SFL;
			if (comment_adapt) adapt_comment[ei][v] += "Short fall ending. ";
		}
		else if (ndur > end_pbd_dur[ii] * 3 && randbw(0, 100) < end_pbd_freq[ii]) {
			artic[ei][v] = ARTIC_END_PBD;
			if (comment_adapt) adapt_comment[ei][v] += "Pitchbend down ending. ";
		}
		else if (ndur > end_vib2_dur[ii] * 3 && randbw(0, 100) < end_vib2_freq[ii]) {
			artic[ei][v] = ARTIC_END_VIB2;
			if (comment_adapt) adapt_comment[ei][v] += "Vibrato2 ending. ";
		}
		else if (ndur > end_vib_dur[ii] * 3 && randbw(0, 100) < end_vib_freq[ii]) {
			artic[ei][v] = ARTIC_END_VIB;
			if (comment_adapt) adapt_comment[ei][v] += "Vibrato ending. ";
		}
	}
}

// Randomize note velocity
void CGAdapt::AdaptRndVel(int v, int x, int i, int ii, int ei, int pi, int pei)
{
	float rv = rnd_vel[ii];
	int ok = 1;
	if (rnd_vel[ii] > 0) {
		if (instr_type[ii] == 1) {
			if (i && !pause[i - 1][v] && note[i-1][v] != note[i][v]) ok = 0;
		}
		if (instr_type[ii] == 2) {
			if (i && !pause[i - 1][v] && note[i - 1][v] != note[i][v]) ok = 0;
		}
		if (ok) {
			// If note repeats, increase randomization range
			if (i > 0 && note[i - 1][v] == note[i][v] && rnd_vel_repeat[ii]) 
				rv = rnd_vel_repeat[ii];
			int max_shift = vel[i][v] * rv / 100.0;
			vel[i][v] = randbw(max(1, vel[i][v] - max_shift), min(127, vel[i][v] + max_shift));
		}
	}
}

void CGAdapt::Adapt(int step1, int step2)
{
	if (step2 < 0) return;
	ValidateVectors(step1, step2);
	milliseconds time_start = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
	int ei; // ending step
	int pi; // previous note step
	int pei; // previous note ending step
	// Save current play speed
	adapt_pspeed = m_pspeed;
	vector<int> isent(MAX_INSTR);
	for (int v = 0; v < v_cnt; v++) {
		int ii = instr[v]; // Instrument id
		// Check if sending multiple voices to monophonic instrument
		isent[ii]++;
		int ncount = 0;
		// Move to note start
		if (coff[step1][v] > 0) step1 = step1 - coff[step1][v];
		// Count notes
		for (int i = step1; i <= step2; i++) {
			if (i + len[i][v] > step2 + 1) break;
			ncount++;
			if (noff[i][v] == 0) break;
			i += noff[i][v] - 1;
			// Clear adaptation comment
			adapt_comment[i][v] = "";
		}
		// Set vel to dyn
		for (int i = step1; i <= step2; i++) {
			vel[i][v] = dyn[i][v];
		}
		CheckRange(v, ii);
		if (!adapt_enable) continue;
		slur_count = 0;
		int i = step1;
		for (int x = 0; x < ncount; x++) {
			ei = max(0, i + len[i][v] - 1);
			pi = max(0, i - poff[i][v]);
			pei = i - 1;
			// Set nonlegato for separate notes
			if ((i == 0) || (pause[pi][v])) {
				artic[i][v] = ARTIC_NONLEGATO;
				if (comment_adapt) adapt_comment[i][v] += "Separate note nonlegato. ";
			}
			if (!pause[i][v]) {
				CheckShortStep(v, x, i, ii, ei, pi, pei);
				// Instrument-specific adaptation
				if (instr_type[ii] == 0) {
					AdaptLengroupStep(v, x, i, ii, ei, pi, pei);
				}
				if (instr_type[ii] == 1) {
					AdaptLongBell(v, x, i, ii, ei, pi, pei, ncount);
					AdaptReverseBell(v, x, i, ii, ei, pi, pei);
					AdaptVibBell(v, x, i, ii, ei, pi, pei);
					AdaptSlurStep(v, x, i, ii, ei, pi, pei);
					AdaptRetriggerRebowStep(v, x, i, ii, ei, pi, pei);
					AdaptNonlegatoStep(v, x, i, ii, ei, pi, pei);
					AdaptAheadStep(v, x, i, ii, ei, pi, pei);
					AdaptAttackStep(v, x, i, ii, ei, pi, pei);
				}
				if (instr_type[ii] == 2) {
					AdaptLongBell(v, x, i, ii, ei, pi, pei, ncount);
					AdaptReverseBell(v, x, i, ii, ei, pi, pei);
					AdaptVibBell(v, x, i, ii, ei, pi, pei);
					AdaptRetriggerNonlegatoStep(v, x, i, ii, ei, pi, pei);
					AdaptNonlegatoStep(v, x, i, ii, ei, pi, pei);
					AdaptFlexAheadStep(v, x, i, ii, ei, pi, pei);
					AdaptNoteEndStep(v, x, i, ii, ei, pi, pei, ncount);
				}
			} // !pause
			if (noff[i][v] == 0) break;
			i += noff[i][v];
		} // for x
		i = step1;
		for (int x = 0; x < ncount; x++) {
			ei = max(0, i + len[i][v] - 1);
			pi = max(0, i - poff[i][v]);
			pei = i - 1;
			if (!pause[i][v]) {
				// Randomize note starts
				if (rand_start[ii] > 0) {
					float max_shift = (etime[ei] - stime[i]) * 100 / m_pspeed * rand_start[ii] / 100;
					if ((rand_start_max[ii] > 0) && (max_shift > rand_start_max[ii])) max_shift = rand_start_max[ii];
					dstime[i][v] += (rand01() - 0.5) * max_shift;
				}
				// Randomize note ends
				if (rand_end[ii] > 0) {
					float max_shift = (etime[ei] - stime[i]) * 100 / m_pspeed * rand_end[ii] / 100;
					if ((rand_end_max[ii] > 0) && (max_shift > rand_end_max[ii])) max_shift = rand_end_max[ii];
					detime[ei][v] += (rand01() - 0.5) * max_shift;
				}
				AdaptRndVel(v, x, i, ii, ei, pi, pei);
				FixOverlap(v, x, i, ii, ei, pi, pei);
			}
			if (noff[i][v] == 0) break;
			i += noff[i][v];
		} // for x
		for (int i = step1; i <= step2; i++) {
			// Randomize dynamics
			int max_shift = dyn[i][v] * rnd_dyn[ii] / 100.0;
			dyn[i][v] = randbw(max(1, dyn[i][v] - max_shift), min(127, dyn[i][v] + max_shift));
			// Randomize vib
			max_shift = vib[i][v] * rnd_dyn[ii] / 100.0;
			vib[i][v] = randbw(max(1, vib[i][v] - max_shift), min(127, vib[i][v] + max_shift));
			// Randomize vibf
			max_shift = vibf[i][v] * rnd_dyn[ii] / 100.0;
			vibf[i][v] = randbw(max(1, vibf[i][v] - max_shift), min(127, vibf[i][v] + max_shift));
		} // for i
	} // for v
	for (int i = step1; i <= step2; i++) {
		// Randomize tempo
		if (i > 0) {
			tempo_rnd[i] = tempo_rnd[i - 1] + randbw(-rnd_tempo_step * tempo[i] / 100.0, rnd_tempo_step * tempo[i] / 100.0);
			// Correct tempo range
			tempo_rnd[i] = max(tempo_rnd[i], -tempo[i] * (rnd_tempo / 2.0) / 100.0);
			tempo_rnd[i] = min(tempo_rnd[i], tempo[i] * (rnd_tempo / 2.0) / 100.0);
			// Apply tempo randomization
			tempo[i] += tempo_rnd[i];
		}
	}
	for (int v = 0; v < v_cnt; v++) {
		// Instrument id
		int ii = instr[v]; 
		// Check if sending multiple voices to monophonic instrument
		if ((isent[ii] > instr_poly[ii]) && (!warning_poly[ii])) {
			CString* est = new CString;
			est->Format("Warning: sending %d voices to instrument %s [%d] with polyphony = %d", isent[ii], InstGName[ii], ii, instr_poly[ii]);
			WriteLog(1, est);
			warning_poly[ii]++;
		}
	}
	// Count time
	if (debug_level > 1) {
		milliseconds time_stop = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
		CString* st = new CString;
		st->Format("Adapt steps %d-%d in %d ms", step1, step2, time_stop - time_start);
		WriteLog(0, st);
	}
	// Tempo could change
	UpdateTempoMinMax(step1, step2);
}

