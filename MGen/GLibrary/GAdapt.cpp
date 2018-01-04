// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../stdafx.h"
#include "GAdapt.h"
#include "SmRnd.h"

CGAdapt::CGAdapt()
{
	play_transpose.resize(MAX_VOICE);
	warning_note_range.resize(MAX_VOICE);
	warning_note_wrong.resize(MAX_VOICE);
	warning_note_short.resize(MAX_VOICE);
	warning_note_long.resize(MAX_VOICE);
	warning_poly.resize(MAX_INSTR);
}


CGAdapt::~CGAdapt()
{
}

void CGAdapt::CheckInstrumentRange(int v, int ii)
{
	// Check if notes are in instrument range
	if ((ngv_min[v] + play_transpose[v] < icf[ii].nmin) || (ngv_max[v] + play_transpose[v] > icf[ii].nmax)) {
		if (ngv_min[v] < icf[ii].nmin) {
			play_transpose[v] = ceil((icf[ii].nmin - ngv_min[v]) / 12.0) * 12;
		}
		if (ngv_max[v] > icf[ii].nmax) {
			int tr = -ceil((ngv_max[v] - icf[ii].nmax) / 12.0) * 12;
			if (tr != 0) play_transpose[v] = tr;
		}
		// Check if still have problem
		CString st;
		if ((ngv_min[v] + play_transpose[v] < icf[ii].nmin) || (ngv_max[v] + play_transpose[v] > icf[ii].nmax)) {
			if (!warning_note_range[v]) {
				st.Format("Generated notes range (%s - %s) is outside instrument %s/%s (voice %d) range (%s - %s). Cannot transpose automatically: range too wide.",
					GetNoteName(ngv_min[v]), GetNoteName(ngv_max[v]), 
					InstGName[ii], InstCName[ii], v,
					GetNoteName(icf[ii].nmin), GetNoteName(icf[ii].nmax));
				warning_note_range[v] = 1;
				WriteLog(1, st);
			}
		}
		else {
			st.Format("Generated notes range (%s - %s) is outside instrument %s/%s (voice %d) range (%s - %s). Transposed automatically to %d semitones. Consider changing instrument or generation range.",
				GetNoteName(ngv_min[v]), GetNoteName(ngv_max[v]), 
				InstGName[ii], InstCName[ii], v,
				GetNoteName(icf[ii].nmin), GetNoteName(icf[ii].nmax), play_transpose[v]);
			WriteLog(1, st);
		}
	}
}

void CGAdapt::CheckShortStep(int v, int x, int i, int ii, int ei, int pi, int pei)
{
	// Check if note is too short
	int ndur = (etime[ei] - stime[i]) * 100 / m_pspeed;
	if (ndur < icf[ii].tmin) {
		CString st;
		if (warning_note_short[v] < 4) {
			st.Format("Recommended minimum note length for %s/%s instrument is %d ms. In voice %d note length at step %d is %d ms. Try to change playback speed, instrument or algorithm config.",
				InstGName[ii], InstCName[ii], icf[ii].tmin, v, i, ndur);
			warning_note_short[v] ++;
			WriteLog(1, st);
			if (comment_adapt) adapt_comment[i][v] += "Too short note. ";
		}
	}
}

void CGAdapt::CheckNoteBreath(int v, int x, int i, int ii, int ei, int pi, int pei)
{
	// Check if note is too long for this instrument
	int ndur = (etime[ei] - stime[i]) * 100 / m_pspeed + detime[ei][v] - dstime[i][v];
	if (icf[ii].tmax && ndur > icf[ii].tmax) {
		CString st;
		if (warning_note_long[v] < 4) {
			st.Format("Recommended maximum note length for %s/%s instrument is %d ms. In voice %d note length at step %d is %d ms. Try to change playback speed, instrument or algorithm config. Some instruments may cut this note shorter.",
				InstGName[ii], InstCName[ii], icf[ii].tmax, v, i, ndur);
			warning_note_long[v] ++;
			WriteLog(1, st);
			if (comment_adapt) adapt_comment[i][v] += "Too long note. ";
		}
	}
}

void CGAdapt::AdaptLengroupStep(int v, int x, int i, int ii, int ei, int pi, int pei)
{
	// Check lengroups
	if ((i > 0) && (icf[ii].lengroup2 + icf[ii].lengroup3 + icf[ii].lengroup4 > 0)) {
		if (lengroup[pi][v] < 2) {
			// Start new lengroup if was no lengroup or lengroup ended
			int r = randbw(0, 100);
			if (r < icf[ii].lengroup2) lengroup[i][v] = 2;
			else if (r < icf[ii].lengroup2 + icf[ii].lengroup3) lengroup[i][v] = 3;
			else if (r < icf[ii].lengroup2 + icf[ii].lengroup3 + icf[ii].lengroup4) lengroup[i][v] = 4;
			else lengroup[i][v] = 0;
		}
		else {
			// Continue lengroup
			lengroup[i][v] = lengroup[pi][v] - 1;
		}
		// Apply lengroups
		if (lengroup[i][v] > 1) {
			if (icf[ii].lengroup_edt1 < 0) {
				detime[ei][v] = -min(-icf[ii].lengroup_edt1, (etime[ei] - stime[i]) * 100 / m_pspeed / 3);
				artic[i][v] = ARTIC_NONLEGATO;
				if (comment_adapt) adapt_comment[i][v] += "Lengroup edt1 nonlegato. ";
			}
			else {
				// Next line commented out, because it has no effect
				//if ((i > 0) && (note[pi][v] == note[i][v])) detime[ei][v] = -10;
				detime[ei][v] = icf[ii].lengroup_edt1;
				artic[i][v] = ARTIC_LEGATO;
				if (comment_adapt) adapt_comment[i][v] += "Lengroup edt1 legato. ";
			}
		}
		if (lengroup[i][v] == 1) {
			if (icf[ii].lengroup_edt2 < 0) {
				detime[ei][v] = -min(-icf[ii].lengroup_edt2, (etime[ei] - stime[i]) * 100 / m_pspeed / 3);
				artic[i][v] = ARTIC_NONLEGATO;
				if (comment_adapt) adapt_comment[i][v] += "Lengroup edt2 nonlegato. ";
			}
			else {
				detime[ei][v] = icf[ii].lengroup_edt2;
				artic[i][v] = ARTIC_LEGATO;
				if (comment_adapt) adapt_comment[i][v] += "Lengroup edt2 legato. ";
			}
		}
	}
}

void CGAdapt::AdaptSlurStep(int v, int x, int i, int ii, int ei, int pi, int pei)
{
	// Add slurs
	if ((i > 0) && (icf[ii].max_slur_interval > 0) && (abs(note[pi][v] - note[i][v]) <= icf[ii].max_slur_interval) && (note[pi][v] != note[i][v]) &&
		(slur_count <= icf[ii].max_slur_count)) {
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
		if (((icf[ii].retrigger_freq > 0) && (randbw(0, 100) > icf[ii].retrigger_freq))
			|| (ndur < icf[ii].retrigger_min_len)) {
			int max_shift = (etime[pei] - stime[pi]) * 100 / m_pspeed * (float)icf[ii].retrigger_rand_end / 100.0;
			if (max_shift > icf[ii].retrigger_rand_max) max_shift = icf[ii].retrigger_rand_max;
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
		if (((icf[ii].retrigger_freq > 0) && (randbw(0, 100) > icf[ii].retrigger_freq))
			|| (ndur < icf[ii].retrigger_min_len)) {
			int max_shift = (etime[pei] - stime[pi]) * 100 / m_pspeed * (float)icf[ii].retrigger_rand_end / 100.0;
			if (max_shift > icf[ii].retrigger_rand_max) max_shift = icf[ii].retrigger_rand_max;
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

void CGAdapt::AdaptNonlegatoStep(int v, int x, int i, int ii, int ei, int pi, int pei) {
	// Randomly make some notes non-legato if they have enough length
	if ((i > 0) && ((etime[pei] - stime[pi]) * 100 / m_pspeed + detime[pei][v] - dstime[pi][v] > icf[ii].nonlegato_minlen) &&
		(randbw(0, 100) < icf[ii].nonlegato_freq * pow(abs(note[i][v] - note[pi][v]), 0.3))) {
		detime[pei][v] = -min(icf[ii].nonlegato_maxgap, (etime[pei] - stime[pi]) * 100 / m_pspeed / 3);
		dstime[i][v] = -icf[ii].all_ahead;
		artic[i][v] = ARTIC_NONLEGATO;
		if (comment_adapt) adapt_comment[i][v] += "Random nonlegato. ";
	}
}

void CGAdapt::AdaptAheadStep(int v, int x, int i, int ii, int ei, int pi, int pei) {
	// Advance start for legato (not longer than previous note length)
	if ((i > 0) && (pi < i) && (icf[ii].legato_ahead[0] > 0) && (artic[i][v] == ARTIC_SLUR || artic[i][v] == ARTIC_LEGATO) &&
		(detime[i - 1][v] >= 0) && (!pause[pi][v]) && (abs(note[i][v] - note[i - 1][v]) <= icf[ii].max_ahead_note)) {
		dstime[i][v] = -min(icf[ii].legato_ahead[0], (etime[i - 1] - stime[pi]) * 100 / m_pspeed +
			detime[i - 1][v] - dstime[pi][v] - 1);
		detime[i - 1][v] = 0.9 * dstime[i][v];
		if (comment_adapt) {
			adapt_comment[i][v] += "Ahead start. ";
			adapt_comment[i - 1][v] += "Ahead end. ";
		}
		// Add glissando if note is long
		if (icf[ii].gliss_freq) {
			float ndur = (etime[ei] - stime[i]) * 100 / m_pspeed + detime[ei][v] - dstime[i][v];
			if ((ndur > icf[ii].gliss_minlen) && (randbw(0, 100) < icf[ii].gliss_freq)) {
				vel[i][v] = icf[ii].vel_gliss;
				if (comment_adapt) adapt_comment[i][v] += "Gliss. ";
			}
		}
	}
}

void CGAdapt::AdaptAllAheadStep(int v, int x, int i, int ii, int ei, int pi, int pei) {
	// Advance start for legato (not longer than previous note length)
	if (i > 0 && pi < i && icf[ii].all_ahead > 0 &&
		detime[i - 1][v] >= 0) {
		dstime[i][v] = -min(icf[ii].all_ahead, (etime[i - 1] - stime[pi]) * 100 / m_pspeed +
			detime[i - 1][v] - dstime[pi][v] - 1);
		if (comment_adapt) {
			adapt_comment[i][v] += "Ahead start. ";
		}
		if (!pause[pi][v]) {
			detime[i - 1][v] = 0.9 * dstime[i][v];
			if (comment_adapt) {
				adapt_comment[i - 1][v] += "Ahead end. ";
			}
		}
	}
}

// For Samplemodeling
void CGAdapt::AdaptFlexAheadStep(int v, int x, int i, int ii, int ei, int pi, int pei)
{
	// Advance start for legato (not longer than previous note length)
	if ((i > 0) && (pi < i) && (icf[ii].legato_ahead[0]) && (artic[i][v] == ARTIC_SLUR || artic[i][v] == ARTIC_LEGATO) &&
		(detime[i - 1][v] >= 0) && (!pause[pi][v]) && (abs(note[i][v] - note[i - 1][v]) <= icf[ii].max_ahead_note)) {
		// Get current note length
		float ndur = (etime[ei] - stime[i]) * 100 / m_pspeed + detime[ei][v] - dstime[i][v];
		// Get previous note length
		float pdur = (etime[pei] - stime[pi]) * 100 / m_pspeed + detime[pei][v] - dstime[pi][v];
		// Get maximum legato_ahead possible
		float min_adur = 0;
		float max_adur = min(ndur * icf[ii].leg_cdur / 100.0, pdur * icf[ii].leg_pdur / 100.0);
		// Set default ahead type to non-ks
		int adur0 = icf[ii].legato_ahead[0];
		// Select articulation
		if (max_adur > icf[ii].splitpo_mindur && abs(note[pi][v] - note[i][v]) > 1 && abs(note[pi][v] - note[i][v]) < 13 &&
			randbw(0, 100) < icf[ii].splitpo_freq) {
			// How many chromatic pitches per second
			int iv = abs(note[i][v] - note[pi][v]);
			float nspeed = iv / max_adur * 1000.0;
			//CString st;
			//st.Format("nspeed %f, max_adur %f, step %d", nspeed, max_adur, i);
			//WriteLog(1, st);
			if (nspeed < 8) {
				artic[i][v] = ARTIC_SPLITPO_CHROM;
				if (comment_adapt) adapt_comment[i][v] += "Split portamento chromatic. ";
				min_adur = (float)max(icf[ii].splitpo_mindur, abs(note[i][v] - note[pi][v]) / 8 * 1000);
				if (icf[ii].legato_ahead[1]) adur0 = icf[ii].legato_ahead[1];
				if (iv < icf[ii].ahead_chrom.size() && icf[ii].ahead_chrom[iv]) adur0 = icf[ii].ahead_chrom[iv];
				//CString st;
				//st.Format("Added chromatic split portamento at step %d", i);
				//WriteLog(0, st);
			}
			else if (abs(note[pi][v] - note[i][v]) > icf[ii].splitpo_pent_minint) {
				artic[i][v] = ARTIC_SPLITPO_PENT;
				if (comment_adapt) adapt_comment[i][v] += "Split portamento pentatonic. ";
				min_adur = icf[ii].splitpo_mindur;
				if (icf[ii].legato_ahead[1]) adur0 = icf[ii].legato_ahead[2];
				//CString st;
				//st.Format("Added pentatonic split portamento at step %d", i);
				//WriteLog(0, st);
			}
		}
		else if (max_adur > icf[ii].gliss_mindur && abs(note[pi][v] - note[i][v]) > 1 && abs(note[pi][v] - note[i][v]) < 6 &&
			randbw(0, 100) < icf[ii].gliss_freq/(100-icf[ii].splitpo_freq+0.001)*100) {
			artic[i][v] = ARTIC_GLISS2;
			if (comment_adapt) adapt_comment[i][v] += "Gliss2. ";
			min_adur = icf[ii].gliss_mindur;
			if (icf[ii].legato_ahead[1]) adur0 = icf[ii].legato_ahead[3];
			//CString st;
			//st.Format("Added gliss2 at step %d", i);
			//WriteLog(0, st);
		}
		// Get minimum and maximum velocity possible
		float min_vel = max(1, 127 -
			pow(max_adur * pow(127, icf[ii].legato_ahead_exp) / adur0, 1 / icf[ii].legato_ahead_exp));
		float max_vel = max(1, 127 -
			pow(min_adur * pow(127, icf[ii].legato_ahead_exp) / adur0, 1 / icf[ii].legato_ahead_exp));
		// Make random velocity inside allowed range
		vel[i][v] = randbw(min_vel, max_vel);
		// Get ahead duration
		float adur = pow(128 - vel[i][v], icf[ii].legato_ahead_exp) * adur0 / pow(127, icf[ii].legato_ahead_exp);
		// Move notes
		dstime[i][v] = -adur;
		detime[i - 1][v] = 0.9 * dstime[i][v];
		// Add comments
		if (comment_adapt) {
			adapt_comment[i][v] += "Ahead flex start. ";
			adapt_comment[i - 1][v] += "Ahead flex end. ";
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
		if (ndur < icf[ii].vel_normal_minlen) {
			vel[i][v] = randbw(icf[ii].vel_immediate, icf[ii].vel_immediate + 2);
			if (comment_adapt) adapt_comment[i][v] += "Vel immediate. ";
		}
		//if (ndur < icf[ii].vel_normal_minlen) vel[i][v] = dyn[i][v] * (float)(127 - icf[ii].vel_immediate) / 127.0 + icf[ii].vel_immediate;
		//else vel[i][v] = dyn[i][v] * (float)(icf[ii].vel_immediate - 1) / 127.0;
	}
}

void CGAdapt::AdaptLongBell(int v, int x, int i, int ii, int ei, int pi, int pei, int ncount)
{
	float ndur = (etime[ei] - stime[i]) * 100 / m_pspeed + detime[ei][v] - dstime[i][v];
	// Create bell if long length, not high velocity, after pause or first note
	if ((ndur > icf[ii].bell_mindur) && (len[i][v] > 2) && (!i || pause[pi][v]) && vel[i][v] < 120) {
		int pos = i + (float)(len[i][v]) * icf[ii].bell_start_len / 100.0;
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
				dyn[z][v] = dyn[z][v] * (icf[ii].bell_start_mul + (float)(z - i) / (pos - i) * (1.0 - icf[ii].bell_start_mul));
			}
			if (comment_adapt) adapt_comment[i][v] += "Long bell start. ";
			// Decrease starting velocity
			if (icf[ii].bell_end_vel) vel[i][v] = randbw(dyn[i][v] * icf[ii].bell_end_vel / 100.0, dyn[i][v] * icf[ii].bell_start_vel / 100.0); //-V550
		}
	}
	int ni = i + noff[i][v];
	// Create bell if long length, not pause and not last note (because can be just end of adapt window)
	if ((ndur > (float)icf[ii].bell_mindur/2) && (len[i][v] > 2) && (x == ncount - 1 || pause[ni][v])) {
		int pos = round(i + (float)(len[i][v]) * 2.0 * icf[ii].bell_start_len / 100.0);
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
				dyn[z][v] = dyn[z][v] * (icf[ii].bell_end_mul + (float)(end - z - 1) / (end - pos) * (1.0 - icf[ii].bell_end_mul));
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
	if ((ndur > icf[ii].rbell_mindur) && (len[i][v] > 2) &&	(randbw(0, 100) < icf[ii].rbell_freq)) {
		int pos1 = i;
		int pos2 = ei;
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
		if (pos2 - pos1 < 2 || ndur2 < icf[ii].rbell_mindur) return;
		// Center position
		int pos = pos1 + (pos2 - pos1) * randbw(icf[ii].rbell_pos1, icf[ii].rbell_pos2) / 100.0;
		// Calculate multiplier
		float mul0 = icf[ii].rbell_mul - (ndur2 - icf[ii].rbell_mindur) *
			(icf[ii].rbell_mul - icf[ii].rbell_mul2) / (icf[ii].rbell_dur - icf[ii].rbell_mindur + 0.0001);
		mul0 = max(min(mul0, icf[ii].rbell_mul), icf[ii].rbell_mul2);
		// Calculate random maximum
		float mul = 1.0 - rand01() * (1.0 - mul0);
		// Left part
		for (int z = pos1; z < pos; z++) {
			dyn[z][v] = dyn[z][v] *
				(abs(z - pos) / (float)(pos - pos1) * (1.0 - mul) + mul);
		}
		// Right part
		for (int z = pos; z <= pos2; z++) {
			dyn[z][v] = dyn[z][v] *
				(abs(z - pos) / (float)(pos2 - pos) * (1.0 - mul) + mul);
		}
		if (comment_adapt) adapt_comment[i][v] += "Reverse bell. ";
	}
}

void CGAdapt::AdaptVibBell(int v, int x, int i, int ii, int ei, int pi, int pei)
{
	float ndur = (etime[ei] - stime[i]) * 100 / m_pspeed + detime[ei][v] - dstime[i][v];
	int ni = i + noff[i][v];
	// Create rbell if long length and no pauses
	if ((ndur > icf[ii].vib_bell_mindur) && (len[i][v] > 2) && (randbw(0, 100) < icf[ii].vib_bell_freq)) {
		// Steps range
		int pos1 = i;
		int pos2 = ei + 1;
		// Center positions
		int pos = pos1 + (pos2 - pos1) * randbw(icf[ii].vib_bell_top1, icf[ii].vib_bell_top2) / 100.0;
		int posf = pos1 + (pos2 - pos1) * randbw(icf[ii].vibf_bell_top1, icf[ii].vibf_bell_top2) / 100.0;
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
			float vb0 = icf[ii].vib_bell1 + (ndur - icf[ii].vib_bell_mindur) *
				(icf[ii].vib_bell2 - icf[ii].vib_bell1) / (icf[ii].vib_bell_dur - icf[ii].vib_bell_mindur + 0.0001);
			vb0 = max(min(vb0, icf[ii].vib_bell2), icf[ii].vib_bell1);
			float vbf0 = icf[ii].vibf_bell1 + (ndur - icf[ii].vib_bell_mindur) *
				(icf[ii].vibf_bell2 - icf[ii].vibf_bell1) / (icf[ii].vib_bell_dur - icf[ii].vib_bell_mindur + 0.0001);
			vbf0 = max(min(vbf0, icf[ii].vib_bell2), icf[ii].vib_bell1);
			// Calculate random maximum
			float vb = randbw(5, max(5, vb0));
			float vbf = randbw(5, max(5, vbf0));
			// Left part
			for (int z = pos1; z < pos; z++) { 
				vib[z][v] = vb * (float)pow(z - pos1, icf[ii].vib_bell_exp) / (float)pow(pos - pos1, icf[ii].vib_bell_exp);
			}
			// Right part
			for (int z = pos; z < pos2; z++) {
				vib[z][v] = vb * (float)pow(pos2 - z, icf[ii].vib_bell_exp) / (float)pow(pos2 - pos, icf[ii].vib_bell_exp);
			}
			// Left part speed
			for (int z = pos1; z < posf; z++) {
				vibf[z][v] = vbf * (float)pow(z - pos1, icf[ii].vibf_bell_exp) / (float)pow(posf - pos1, icf[ii].vibf_bell_exp);
			}
			// Right part speed
			for (int z = posf; z < pos2; z++) {
				vibf[z][v] = vbf * (float)pow(pos2 - z, icf[ii].vibf_bell_exp) / (float)pow(pos2 - posf, icf[ii].vibf_bell_exp);
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
		if (ndur > icf[ii].end_sfl_dur * 3 && randbw(0, 100) < icf[ii].end_sfl_freq) {
			artic[ei][v] = ARTIC_END_SFL;
			if (comment_adapt) adapt_comment[ei][v] += "Short fall ending. ";
		}
		else if (ndur > icf[ii].end_pbd_dur * 3 && randbw(0, 100) < icf[ii].end_pbd_freq) {
			artic[ei][v] = ARTIC_END_PBD;
			if (comment_adapt) adapt_comment[ei][v] += "Pitchbend down ending. ";
		}
		else if (ndur > icf[ii].end_vib2_dur * 3 && randbw(0, 100) < icf[ii].end_vib2_freq) {
			artic[ei][v] = ARTIC_END_VIB2;
			if (comment_adapt) adapt_comment[ei][v] += "Vibrato2 ending. ";
		}
		else if (ndur > icf[ii].end_vib_dur * 3 && randbw(0, 100) < icf[ii].end_vib_freq) {
			artic[ei][v] = ARTIC_END_VIB;
			if (comment_adapt) adapt_comment[ei][v] += "Vibrato ending. ";
		}
	}
}

// Randomize note velocity
void CGAdapt::AdaptRndVel(int v, int x, int i, int ii, int ei, int pi, int pei)
{
	float rv = icf[ii].rnd_vel;
	int ok = 1;
	if (icf[ii].rnd_vel > 0) {
		if (icf[ii].type == 1) {
			// Prevent velocity randomization of flexible legato transitions, because this can shift tempo
			if (i && !pause[i - 1][v] && note[i-1][v] != note[i][v]) ok = 0;
			// Prevent velocity randomization of short nonlegato notes, because they sound bad at low velocity with Friedlander
			if ((etime[ei] - stime[i]) * 100 / m_pspeed + detime[ei][v] - dstime[i][v] < icf[ii].vel_normal_minlen) ok = 0;
		}
		if (icf[ii].type == 2 || icf[ii].type == 3) {
			// Prevent velocity randomization of flexible legato transitions, because this can shift tempo
			if (i && !pause[i - 1][v] && note[i - 1][v] != note[i][v]) ok = 0;
		}
		if (ok) {
			// If note repeats, increase randomization range
			if (i > 0 && note[i - 1][v] == note[i][v] && icf[ii].rnd_vel_repeat) 
				rv = icf[ii].rnd_vel_repeat;
			int max_shift = vel[i][v] * rv / 100.0;
			vel[i][v] = randbw(max(1, vel[i][v] - max_shift), min(127, vel[i][v] + max_shift));
		}
	}
}

void CGAdapt::CalculateVoiceStages() {
	// Detect voice stages
	if (stages_calculated) return;
	vector<int> voices_in_instr(MAX_INSTR); // How many voices use each instrument
	vector<int> voices_in_track(MAX_INSTR); // How many voices use each initial track
	vector<map<int, int>> tracks_in_instr(MAX_INSTR); // How many tracks use each instrument
	map<int, int> voices_in_trackchan; // How many voices use each resulting track/channel
	v_stage.resize(MAX_VOICE);
	for (int v = 0; v < v_cnt; v++) {
		// Calculate parameters
		int track = track_id[v];
		int ii = instr[v];
		int trackchan = icf[ii].track * 16 + icf[ii].channel;
		// Calculate stats
		++voices_in_instr[ii];
		++voices_in_track[track];
		tracks_in_instr[ii][track] = 1;
		++voices_in_trackchan[trackchan];
		// Record stats
		v_itrack[v] = tracks_in_instr[ii].size();
		itrack[track] = tracks_in_instr[ii].size();
		if (icf[ii].poly < 2) v_stage[v] = voices_in_trackchan[trackchan] - 1;
	}
	stages_calculated = 1;
}

void CGAdapt::Adapt(int step1, int step2)
{
	if (step2 < 0) return;
	if (step1 > t_adapted) {
		CString est;
		est.Format("Attempt to adapt from %d to %d steps, while last adapted step was %d. This means that some part of music is going to be not validated and not adapted.",
			step1, step2, t_adapted);
		WriteLog(5, est);
	}
	// Set new adapted limit
	t_adapted = step2 + 1;
	ValidateVectors(step1, step2);
	long long time_start = CGLib::time();
	int ei; // ending step
	int pi; // previous note step
	int pei; // previous note ending step
	// Save current play speed
	adapt_pspeed = m_pspeed;
	CalculateVoiceStages();
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
			// Clear adaptation comment
			adapt_comment[i][v].Empty();
		}
		// Set vel to dyn
		for (int i = step1; i <= step2; i++) {
			vel[i][v] = dyn[i][v];
		}
		CheckInstrumentRange(v, ii);
		if (!adapt_enable) continue;
		slur_count = 0;
		int i = step1;
		for (int x = 0; x < ncount; x++) {
			if (need_exit) break;
			ei = max(0, i + len[i][v] - 1);
			pi = max(0, i - poff[i][v]);
			pei = i - 1;
			// Set nonlegato for separate notes
			if ((i == 0) || (pause[pi][v])) {
				artic[i][v] = ARTIC_NONLEGATO;
				if (comment_adapt && !pause[pi][v]) adapt_comment[i][v] += "Separate note nonlegato. ";
			}
			if (!pause[i][v]) {
				CheckShortStep(v, x, i, ii, ei, pi, pei);
				// Instrument-specific adaptation
				if (icf[ii].type == 0) {
					AdaptLengroupStep(v, x, i, ii, ei, pi, pei);
				}
				if (icf[ii].type == 1) {
					AdaptLongBell(v, x, i, ii, ei, pi, pei, ncount);
					AdaptReverseBell(v, x, i, ii, ei, pi, pei);
					AdaptVibBell(v, x, i, ii, ei, pi, pei);
					AdaptSlurStep(v, x, i, ii, ei, pi, pei);
					AdaptRetriggerRebowStep(v, x, i, ii, ei, pi, pei);
					AdaptNonlegatoStep(v, x, i, ii, ei, pi, pei);
					AdaptAheadStep(v, x, i, ii, ei, pi, pei);
					AdaptAttackStep(v, x, i, ii, ei, pi, pei);
				}
				if (icf[ii].type == 2) {
					AdaptLongBell(v, x, i, ii, ei, pi, pei, ncount);
					AdaptReverseBell(v, x, i, ii, ei, pi, pei);
					AdaptVibBell(v, x, i, ii, ei, pi, pei);
					AdaptRetriggerNonlegatoStep(v, x, i, ii, ei, pi, pei);
					AdaptNonlegatoStep(v, x, i, ii, ei, pi, pei);
					AdaptFlexAheadStep(v, x, i, ii, ei, pi, pei);
					AdaptNoteEndStep(v, x, i, ii, ei, pi, pei, ncount);
				}
				if (icf[ii].type == 3) {
					AdaptLongBell(v, x, i, ii, ei, pi, pei, ncount);
					AdaptReverseBell(v, x, i, ii, ei, pi, pei);
					AdaptVibBell(v, x, i, ii, ei, pi, pei);
					AdaptAllAheadStep(v, x, i, ii, ei, pi, pei);
					AdaptNonlegatoStep(v, x, i, ii, ei, pi, pei);
					//vel[i][v] = randbw(1, 126);
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
				if (icf[ii].rand_start > 0) {
					float max_shift = (etime[ei] - stime[i]) * 100 / m_pspeed * icf[ii].rand_start / 100;
					if ((icf[ii].rand_start_max > 0) && (max_shift > icf[ii].rand_start_max)) max_shift = icf[ii].rand_start_max;
					dstime[i][v] += (rand01() - 0.5) * max_shift;
				}
				// Randomize note ends
				if (icf[ii].rand_end > 0) {
					float max_shift = (etime[ei] - stime[i]) * 100 / m_pspeed * icf[ii].rand_end / 100;
					if ((icf[ii].rand_end_max > 0) && (max_shift > icf[ii].rand_end_max)) max_shift = icf[ii].rand_end_max;
					detime[ei][v] += (rand01() - 0.5) * max_shift;
				}
				AdaptRndVel(v, x, i, ii, ei, pi, pei);
				FixOverlap(v, x, i, ii, ei, pi, pei);
			}
			CheckNoteBreath(v, x, i, ii, ei, pi, pei);
			if (noff[i][v] == 0) break;
			i += noff[i][v];
		} // for x
	} // for v
	CSmoothRandom sr;
	float tr;
	for (int i = step1; i <= step2; i++) {
		// Load tempo if it was randomized before
		if (tempo_src[i]) { //-V550
			tempo[i] = tempo_src[i];
		}
		// Save source tempo
		tempo_src[i] = tempo[i];
		// Randomize tempo
		if (i > 0) {
			// Calculate fadeout
			float fadeout = 1;
			if (stime[step2] - CC_FADEOUT_RESERVE - stime[i] < CC_FADEOUT) 
				fadeout = max(0, stime[step2] - CC_FADEOUT_RESERVE - stime[i]) / CC_FADEOUT;
			// Create random
			sr.MakeNext();
			tr = sr.sig / sr.s_range * (float)rnd_tempo * (float)tempo_src[i] / 200.0 * fadeout;
			//tr = tempo[i - 1] - tempo_src[i - 1] + randbw(-rnd_tempo_step * tempo[i] / 100.0, rnd_tempo_step * tempo[i] / 100.0);
			// Correct tempo range
			//tr = max(tr, -tempo[i] * (rnd_tempo / 2.0) / 100.0);
			//tr = min(tr, tempo[i] * (rnd_tempo / 2.0) / 100.0);
			// Apply tempo randomization
			tempo[i] += tr;
		}
	}
	// Count time
	if (debug_level > 1) {
		long long time_stop = CGLib::time();
		CString st;
		st.Format("Adapt steps %d-%d in %lld ms", step1, step2, time_stop - time_start);
		WriteLog(0, st);
	}
	// Tempo could change
	UpdateTempoMinMax(step1, step2);
	// Check adaptation results
	ValidateVectors2(step1, step2);
}

