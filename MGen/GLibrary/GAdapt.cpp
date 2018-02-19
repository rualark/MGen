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
}


CGAdapt::~CGAdapt()
{
}

void CGAdapt::CheckInstrumentRange(int v, int ii) {
	CString st;
	// Do not check for instruments with replaces or mapping
	if (icf[ii].replace_pitch > -1 || icf[ii].map_pitch.size()) return;
	// Fix transpose
	if (icf[ii].fix_transpose != 1000) {
		play_transpose[v] = icf[ii].fix_transpose;
		if ((ngv_min[v] + play_transpose[v] < icf[ii].nmin) || (ngv_max[v] + play_transpose[v] > icf[ii].nmax)) {
			st.Format("Notes transposed by %s semitones in config. Resulting notes range (%s - %s) is outside instrument %s/%s (voice %d) range (%s - %s).",
				play_transpose[v], GetNoteName(ngv_min[v] + play_transpose[v]), GetNoteName(ngv_max[v] + play_transpose[v]),
				icf[ii].group, icf[ii].name, v,
				GetNoteName(icf[ii].nmin), GetNoteName(icf[ii].nmax));
			warning_note_range[v] = 1;
			WriteLog(1, st);
		}
		return;
	}
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
					icf[ii].group, icf[ii].name, v,
					GetNoteName(icf[ii].nmin), GetNoteName(icf[ii].nmax));
				warning_note_range[v] = 1;
				WriteLog(1, st);
			}
		}
		else {
			st.Format("Generated notes range (%s - %s) is outside instrument %s/%s (voice %d) range (%s - %s). Transposed automatically to %d semitones. Consider changing instrument or generation range.",
				GetNoteName(ngv_min[v]), GetNoteName(ngv_max[v]), 
				icf[ii].group, icf[ii].name, v,
				GetNoteName(icf[ii].nmin), GetNoteName(icf[ii].nmax), play_transpose[v]);
			WriteLog(1, st);
		}
	}
}

void CGAdapt::CheckShortStep(int v, int x, int i, int ii, int ei, int pi, int pei)
{
	// Check if note is too short
	int ndur = (setime[ei][v] - sstime[i][v]) * 100 / m_pspeed;
	if (ndur < icf[ii].tmin && !note_muted[i][v]) {
		CString st; 
		if (warning_note_short[v] < 4) {
			st.Format("Recommended minimum note length for %s/%s instrument is %d ms. In voice %d note length at step %d is %d ms. Try to change playback speed, instrument or algorithm config.",
				icf[ii].group, icf[ii].name, icf[ii].tmin, v, i, ndur);
			warning_note_short[v] ++;
			WriteLog(0, st);
			if (comment_adapt) adapt_comment[i][v] += "Too short note. ";
		}
	}
}

void CGAdapt::CheckNoteBreath(int v, int x, int i, int ii, int ei, int pi, int pei)
{
	// Check if note is too long for this instrument
	int ndur = (setime[ei][v] - sstime[i][v]) * 100 / m_pspeed + detime[ei][v] - dstime[i][v];
	if (icf[ii].tmax && ndur > icf[ii].tmax) {
		CString st;
		if (warning_note_long[v] < 4) {
			st.Format("Recommended maximum note length for %s/%s instrument is %d ms. In voice %d note length at step %d is %d ms. Try to change playback speed, instrument or algorithm config. Some instruments may cut this note shorter.",
				icf[ii].group, icf[ii].name, icf[ii].tmax, v, i, ndur);
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
				detime[ei][v] = -min(-icf[ii].lengroup_edt1, (setime[ei][v] - sstime[i][v]) * 100 / m_pspeed / 3);
				artic[i][v] = aNONLEGATO;
				dstime[i][v] = -icf[ii].all_ahead;
				if (comment_adapt) adapt_comment[i][v] += "Lengroup edt1 nonlegato. ";
			}
			else {
				// Next line commented out, because it has no effect
				//if ((i > 0) && (note[pi][v] == note[i][v])) detime[ei][v] = -10;
				detime[ei][v] = icf[ii].lengroup_edt1;
				artic[i][v] = aLEGATO;
				if (comment_adapt) adapt_comment[i][v] += "Lengroup edt1 legato. ";
			}
		}
		if (lengroup[i][v] == 1) {
			if (icf[ii].lengroup_edt2 < 0) {
				detime[ei][v] = -min(-icf[ii].lengroup_edt2, (setime[ei][v] - sstime[i][v]) * 100 / m_pspeed / 3);
				artic[i][v] = aNONLEGATO;
				if (comment_adapt) adapt_comment[i][v] += "Lengroup edt2 nonlegato. ";
			}
			else {
				detime[ei][v] = icf[ii].lengroup_edt2;
				artic[i][v] = aLEGATO;
				if (comment_adapt) adapt_comment[i][v] += "Lengroup edt2 legato. ";
			}
		}
	}
}

void CGAdapt::AdaptSlurStep(int v, int x, int i, int ii, int ei, int pi, int pei)
{
	// Add slurs
	if ((i > 0) && (icf[ii].max_slur_interval > 0) && 
		(abs(note[pi][v] - note[i][v]) <= icf[ii].max_slur_interval) && 
		(note[pi][v] != note[i][v]) &&
		artic[i][v] == aLEGATO &&
		(slur_count <= icf[ii].max_slur_count)) {
		artic[i][v] = aSLUR;
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
	if ((i > 0) && (pi < i) && (note[pi][v] == note[i][v]) && artic[i][v] == aLEGATO) {
		float ndur = (setime[ei][v] - sstime[i][v]) * 100 / m_pspeed + detime[ei][v] - dstime[i][v];
		// Replace retrigger with non-legato
		if (((icf[ii].retrigger_freq > 0) && (randbw(0, 100) > icf[ii].retrigger_freq))
			|| (ndur < icf[ii].retrigger_min_len)) {
			int max_shift = (setime[pei][v] - sstime[pi][v]) * 100 / m_pspeed * (float)icf[ii].retrigger_rand_end / 100.0;
			if (max_shift > icf[ii].retrigger_rand_max) max_shift = icf[ii].retrigger_rand_max;
			detime[pei][v] = -randbw(0, max_shift);
			artic[i][v] = aNONLEGATO;
			dstime[i][v] = -icf[ii].all_ahead;
			if (comment_adapt) adapt_comment[i][v] += "Retrigger nonlegato. ";
		}
		else {
			if (comment_adapt) adapt_comment[i][v] += "Rebow retrigger. ";
			artic[i][v] = aREBOW;
			detime[pei][v] = -1;
			dstime[i][v] = -icf[ii].all_ahead;
		}
	}
}

void CGAdapt::AdaptRetriggerNonlegatoStep(int v, int x, int i, int ii, int ei, int pi, int pei)
{
	// Retrigger notes
	if ((i > 0) && (pi < i) && (note[pi][v] == note[i][v]) && artic[i][v] == aLEGATO) {
		float ndur = (setime[ei][v] - sstime[i][v]) * 100 / m_pspeed + detime[ei][v] - dstime[i][v];
		// Replace retrigger with non-legato
		if (((icf[ii].retrigger_freq > 0) && (randbw(0, 100) > icf[ii].retrigger_freq))
			|| (ndur < icf[ii].retrigger_min_len)) {
			int max_shift = (setime[pei][v] - sstime[pi][v]) * 100 / m_pspeed * (float)icf[ii].retrigger_rand_end / 100.0;
			if (max_shift > icf[ii].retrigger_rand_max) max_shift = icf[ii].retrigger_rand_max;
			detime[pei][v] = -icf[ii].all_ahead - randbw(0, max_shift);
			artic[i][v] = aNONLEGATO;
			dstime[i][v] = -icf[ii].all_ahead;
			if (comment_adapt) adapt_comment[i][v] += "Retrigger nonlegato. ";
		}
		else {
			if (comment_adapt) adapt_comment[i][v] += "Retrigger. ";
			artic[i][v] = aRETRIGGER;
			detime[pei][v] = - icf[ii].all_ahead - 1;
			dstime[i][v] = -icf[ii].all_ahead;
		}
	}
}

void CGAdapt::AdaptAutoLegatoStep(int v, int x, int i, int ii, int ei, int pi, int pei) {
	// Process only legato transitions
	if (artic[i][v] != aLEGATO) return;
	// Set nonlegato for separate notes
	// If previous step is not a note, then there is definitely no overlap
	if (i == 0 || pause[pi][v] || smet[pei][v] < smst[i][v]) {
		artic[i][v] = aNONLEGATO;
		dstime[i][v] = -icf[ii].all_ahead;
		if (comment_adapt) adapt_comment[i][v] += "Separate note nonlegato. ";
	} 
	else {
		float ndur = (setime[pei][v] - sstime[pi][v]) * 100 / m_pspeed + detime[pei][v] - dstime[pi][v];
		// Convert legato to non-legato if previous note is short
		// Do not check this for SM brass, because flex legato allows for any legato length
		if (ndur <= icf[ii].legato_ahead[0] + 1 && icf[ii].type != itSMB) {
			artic[i][v] = aNONLEGATO;
			dstime[i][v] = -icf[ii].all_ahead;
			if (comment_adapt) adapt_comment[i][v] += "Nonlegato after short. ";
		}
		// Convert legato to non-legato if previous note is not legato or non-legato
		else if (artic[pi][v] == aSTAC || artic[pi][v] == aTREM || artic[pi][v] == aPIZZ) {
			artic[i][v] = aNONLEGATO;
			dstime[i][v] = -icf[ii].all_ahead;
			if (comment_adapt) adapt_comment[i][v] += "Nonlegato after other articulation. ";
		}
		// Convert legato to non-legato if notes are touching
		// If instrument auto_legato is not set, then use global value. If set, use instrument value
		else if ((icf[ii].auto_legato == 0 || (auto_legato == 0 && icf[ii].auto_legato == -1)) && smet[pei][v] == smst[i][v]) {
			artic[i][v] = aNONLEGATO;
			dstime[i][v] = -icf[ii].all_ahead;
			if (comment_adapt) adapt_comment[i][v] += "Touching note nonlegato. ";
		}
		// If note is not separate, convert it to legato in auto_legato mode
	}
}

void CGAdapt::AdaptNonlegatoStep(int v, int x, int i, int ii, int ei, int pi, int pei) {
	// Randomly make some notes non-legato if they have enough length
	if (auto_nonlegato && (i > 0) && (artic[i][v] == aLEGATO || artic[i][v] == aSLUR) &&
		((setime[pei][v] - sstime[pi][v]) * 100 / m_pspeed + detime[pei][v] - dstime[pi][v] > icf[ii].nonlegato_minlen) &&
		(randbw(0, 100) < icf[ii].nonlegato_freq * pow(abs(note[i][v] - note[pi][v]), 0.3))) {
		detime[pei][v] = -min(icf[ii].nonlegato_maxgap, (setime[pei][v] - sstime[pi][v]) * 100 / m_pspeed / 3);
		dstime[i][v] = -icf[ii].all_ahead;
		artic[i][v] = aNONLEGATO;
		if (comment_adapt) adapt_comment[i][v] += "Random nonlegato. ";
	}
}

void CGAdapt::AdaptStaccatoStep(int v, int x, int i, int ii, int ei, int pi, int pei) {
	if (!icf[ii].stac_auto) return;
	// Make short non-legato notes (on both sides) staccato
	if (x && artic[pi][v] != aLEGATO && artic[pi][v] != aSLUR && artic[pi][v] != aPIZZ &&
		artic[i][v] != aLEGATO && artic[i][v] != aSLUR &&
		(setime[pei][v] - sstime[pi][v]) * 100 / m_pspeed + detime[pei][v] - dstime[pi][v] <= icf[ii].stac_maxlen) {
		if (icf[ii].stac_ahead > -1) dstime[pi][v] = -icf[ii].stac_ahead;
		else dstime[pi][v] = -icf[ii].all_ahead;
		artic[pi][v] = aSTAC;
		vel[pi][v] = max(1, min(127, dyn[pi][v] * icf[ii].stac_dynamics / 100 + icf[ii].stac_dyn_add));
		// Next note cannot be legato/slur
		dstime[i][v] = -icf[ii].all_ahead;
		artic[i][v] = aNONLEGATO;
		vel[i][v] = max(1, dyn[i][v]);
		if (comment_adapt) adapt_comment[pi][v] += "Staccato. ";
	}
	// Same process for current note
	if (artic[i][v] != aLEGATO && artic[i][v] != aSLUR && artic[i][v] != aPIZZ &&
		(ei == t_generated - 1 || pause[ei + 1][v]) &&
		(setime[ei][v] - sstime[i][v]) * 100 / m_pspeed + detime[ei][v] - dstime[i][v] <= icf[ii].stac_maxlen) {
		if (icf[ii].stac_ahead > -1) dstime[i][v] = -icf[ii].stac_ahead;
		else dstime[i][v] = -icf[ii].all_ahead;
		artic[i][v] = aSTAC;
		vel[i][v] = max(1, min(127, dyn[i][v] * icf[ii].stac_dynamics / 100 + icf[ii].stac_dyn_add));
		if (comment_adapt) adapt_comment[i][v] += "Staccato. ";
	}
}

void CGAdapt::AdaptPizzStep(int v, int x, int i, int ii, int ei, int pi, int pei) {
	// Change pizz dynamics
	if (artic[i][v] == aPIZZ) {
		vel[i][v] = max(1, min(127, dyn[i][v] * icf[ii].pizz_dynamics / 100 + icf[ii].pizz_dyn_add));
		if (icf[ii].pizz_ahead > -1) {
			dstime[i][v] = -icf[ii].pizz_ahead;
		}
		if (comment_adapt) adapt_comment[i][v] += "Pizz. ";
	}
}

void CGAdapt::AdaptAheadStep(int v, int x, int i, int ii, int ei, int pi, int pei) {
	float max_shift = (setime[ei][v] - sstime[i][v]) * 100 / m_pspeed * icf[ii].rand_start / 100;
	if ((icf[ii].rand_start_max > 0) && (max_shift > icf[ii].rand_start_max)) max_shift = icf[ii].rand_start_max;
	int rand_ahead = max(1, icf[ii].legato_ahead[0] - (rand01() - 0.5) * max_shift);
	// Advance start for legato (not longer than previous note length)
	if (i > 0 && pi < i) {
		if (icf[ii].legato_ahead[0] > 0 && (artic[i][v] == aSLUR || artic[i][v] == aLEGATO) &&
			(!pause[pi][v]) && (abs(note[i][v] - note[i - 1][v]) <= icf[ii].max_ahead_note)) {
			dstime[i][v] = -min(rand_ahead, (setime[i - 1][v] - sstime[pi][v]) * 100 / m_pspeed +
				detime[i - 1][v] - dstime[pi][v] - 1);
			detime[i - 1][v] = 0.9 * dstime[i][v];
			if (comment_adapt) {
				adapt_comment[i][v] += "Ahead legato start. ";
				adapt_comment[i - 1][v] += "Ahead legato end. ";
			}
			// Add glissando if note is long
			float ndur = (setime[ei][v] - sstime[i][v]) * 100 / m_pspeed + detime[ei][v] - dstime[i][v];
			if (icf[ii].gliss_freq && ndur > icf[ii].gliss_minlen && 
				randbw(0, 100) < icf[ii].gliss_freq) {
					vel[i][v] = icf[ii].vel_gliss;
					if (comment_adapt) adapt_comment[i][v] += "Gliss. ";
			}
			else {
				if (ndur > icf[ii].legato_long_minlen) {
					vel[i][v] = randbw(icf[ii].vel_gliss + 1, icf[ii].vel_legato_long);
				}
				else {
					vel[i][v] = randbw(icf[ii].vel_legato_long + 1, 127);
				}
			}
		}
	}
}

void CGAdapt::AdaptAllAheadStep(int v, int x, int i, int ii, int ei, int pi, int pei) {
	// Advance start for legato (not longer than previous note length)
	if (i > 0 && pi < i && icf[ii].all_ahead > 0) {
		dstime[i][v] = -icf[ii].all_ahead;
		if (comment_adapt) {
			adapt_comment[i][v] += "Ahead start. ";
		}
	}
}

// For Samplemodeling
void CGAdapt::AdaptFlexAheadStep(int v, int x, int i, int ii, int ei, int pi, int pei)
{
	// Advance start for legato (not longer than previous note length)
	if ((i > 0) && (pi < i) && (icf[ii].legato_ahead[0]) && (artic[i][v] == aSLUR || artic[i][v] == aLEGATO) &&
		(detime[i - 1][v] >= 0) && (!pause[pi][v]) && (abs(note[i][v] - note[i - 1][v]) <= icf[ii].max_ahead_note)) {
		// Get current note length
		float ndur = (setime[ei][v] - sstime[i][v]) * 100 / m_pspeed + detime[ei][v] - dstime[i][v];
		// Get previous note length
		float pdur = (setime[pei][v] - sstime[pi][v]) * 100 / m_pspeed + detime[pei][v] - dstime[pi][v];
		// Get maximum legato_ahead possible
		float min_adur = 0;
		float max_adur = max(min(ndur * icf[ii].leg_cdur / 100.0, pdur * icf[ii].leg_pdur / 100.0) - icf[ii].all_ahead, 0);
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
				artic[i][v] = aSPLITPO_CHROM;
				if (comment_adapt) adapt_comment[i][v] += "Split portamento chromatic. ";
				min_adur = (float)max(icf[ii].splitpo_mindur, abs(note[i][v] - note[pi][v]) / 8 * 1000);
				if (icf[ii].legato_ahead[1]) adur0 = icf[ii].legato_ahead[1];
				if (iv < icf[ii].ahead_chrom.size() && icf[ii].ahead_chrom[iv]) adur0 = icf[ii].ahead_chrom[iv];
				//CString st;
				//st.Format("Added chromatic split portamento at step %d", i);
				//WriteLog(0, st);
			}
			else if (abs(note[pi][v] - note[i][v]) > icf[ii].splitpo_pent_minint) {
				artic[i][v] = aSPLITPO_PENT;
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
			artic[i][v] = aGLISS2;
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
		dstime[i][v] = - adur - icf[ii].all_ahead;
		detime[i - 1][v] = 0.9 * dstime[i][v];
		// Add comments
		if (comment_adapt) {
			adapt_comment[i][v] += "Ahead flex start. ";
			adapt_comment[i - 1][v] += "Ahead flex end. ";
		}
	}
}

void CGAdapt::FixOverlap(int v, int x, int i, int ii, int ei, int pi, int pei) {
	// Check if note overlapping occured
	if (i > 0) {
		// Local previous id
		int lpi = pi; 
		// Cycle through all notes backwards
		while (lpi >= 0) {
			if (note[lpi][v] == note[i][v] || 
				(!pause[lpi][v] && icf[ii].poly == 1 && (artic[i][v] == aSTAC || artic[i][v] == aPIZZ || artic[i][v] == aNONLEGATO ||
					artic[i][v] == aREBOW || artic[i][v] == aRETRIGGER || artic[i][v] == aTREM))) {
				int lpei = lpi + len[lpi][v] - 1;
				float gap = (sstime[i][v] - setime[lpei][v]) * 100 / m_pspeed + dstime[i][v] - detime[lpei][v];
				if (gap <	icf[ii].nonlegato_mingap) {
					// Move ending of previous note to the left but not further than previous note start
					detime[lpei][v] = max(
						// Push back
						(sstime[i][v] - setime[lpei][v]) * 100 / m_pspeed + dstime[i][v] - icf[ii].nonlegato_mingap,
						// Maximum push
						(sstime[lpi][v] - setime[lpei][v]) * 100 / m_pspeed + dstime[lpei][v] + 1);
					if (comment_adapt) adapt_comment[lpei][v] += "Ending overlap fixed. ";
				}
				gap = (sstime[i][v] - setime[lpei][v]) * 100 / m_pspeed + dstime[i][v] - detime[lpei][v];
				if (note[lpi][v] == note[i][v] && gap < icf[ii].retrigger_mingap) {
					// Move ending of previous note to the left but not further than previous note start
					detime[lpei][v] = max(
						// Push back
						(sstime[i][v] - setime[lpei][v]) * 100 / m_pspeed + dstime[i][v] - icf[ii].retrigger_mingap,
						// Maximum push
						(sstime[lpi][v] - setime[lpei][v]) * 100 / m_pspeed + dstime[lpei][v] + 1);
					if (comment_adapt) adapt_comment[lpei][v] += "Retrigger overlap fixed. ";
				}
				break;
			}
			if (poff[lpi][v] == 0) break;
			lpi = lpi - poff[lpi][v];
		}
	}
}

void CGAdapt::AdaptAttackStep(int v, int x, int i, int ii, int ei, int pi, int pei) {
	// If nonlegato and short note, avoid slow sustain articulations
	if (artic[i][v] == aNONLEGATO || icf[ii].type == itSMW) {
		float ndur = (setime[ei][v] - sstime[i][v]) * 100 / m_pspeed + detime[ei][v] - dstime[i][v];
		if (ndur < icf[ii].vel_normal_minlen && vel[i][v] < icf[ii].vel_immediate) {
			vel[i][v] = randbw(icf[ii].vel_immediate, icf[ii].vel_immediate + 2);
			if (comment_adapt) adapt_comment[i][v] += "Vel immediate. ";
		}
	}
	if (artic[i][v] == aNONLEGATO) {
		// Limit harsh sustains
		if (vel[i][v] >= icf[ii].vel_harsh && icf[ii].harsh_freq < 100) {
			if (randbw(0, 100) >= icf[ii].harsh_freq) {
				vel[i][v] = randbw(icf[ii].vel_immediate, icf[ii].vel_harsh - 1);
				if (comment_adapt) adapt_comment[i][v] += "Vel harsh limited. ";
			}
		}
	}
}

void CGAdapt::AdaptLongBell(int v, int x, int i, int ii, int ei, int pi, int pei, int ncount) {
	float ndur = (setime[ei][v] - sstime[i][v]) * 100 / m_pspeed + detime[ei][v] - dstime[i][v];
	// Create bell if long length, not high velocity, after pause or first note
	if (ndur > icf[ii].bell_mindur && len[i][v] > 2 && artic[i][v] != aSTAC && artic[i][v] != aPIZZ &&
		(!i || pause[pi][v]) && vel[i][v] < 120) {
		int pos = i + (float)(len[i][v]) * icf[ii].bell_start_len / 100.0;
		int ok = 1;
		// Check if dynamics does not decrease
		if (pos - i > 1) for (int z = i + 1; z < pos; z++) {
			if (dyn[z][v] < dyn[i][v]) {
				ok = 0;
				break;
			}
		}
		if (ok) {
			for (int z = i; z < pos; z++) {
				dyn[z][v] = dyn[pos - 1][v] * (icf[ii].bell_start_mul + (float)(z - i) / (pos - i) * (1.0 - icf[ii].bell_start_mul));
			}
			if (comment_adapt) adapt_comment[i][v] += "Long bell start. ";
			// Decrease starting velocity
			if (icf[ii].bell_end_vel) vel[i][v] = max(1, 
				randbw(dyn[i][v] * icf[ii].bell_end_vel / 100.0, dyn[i][v] * icf[ii].bell_start_vel / 100.0)); //-V550
		}
	}
	int ni = i + noff[i][v];
	// Create bell if long length, not pause and not last note (because can be just end of adapt window)
	if ((ndur > (float)icf[ii].bell_mindur2/2) && len[i][v] > 2 && artic[i][v] != aSTAC && artic[i][v] != aPIZZ 
		&& (x == ncount - 1 || pause[ni][v])) {
		int pos = round(i + (float)(len[i][v]) * 2.0 * icf[ii].bell_start_len / 100.0);
		int ok = 1;
		int end = i + len[i][v];
		// Check if dynamics does not increase
		if (end - pos > 1) for (int z = pos; z < end; z++) {
			if (dyn[z - 1][v] < dyn[end - 1][v]) {
				ok = 0;
				break;
			}
		}
		if (ok) {
			for (int z = pos; z < end; z++) {
				dyn[z][v] = dyn[pos][v] * (icf[ii].bell_end_mul + (float)(end - z - 1) / (end - pos) * (1.0 - icf[ii].bell_end_mul));
			}
			if (comment_adapt) adapt_comment[i + len[i][v] - 1][v] += "Long bell end. ";
		}
	}
}

void CGAdapt::AdaptReverseBell(int v, int x, int i, int ii, int ei, int pi, int pei)
{
	float ndur = (setime[ei][v] - sstime[i][v]) * 100 / m_pspeed + detime[ei][v] - dstime[i][v];
	int ni = i + noff[i][v];
	// Create rbell if long length and no pauses
	if ((ndur > icf[ii].rbell_mindur) && len[i][v] > 2 && artic[i][v] != aSTAC && artic[i][v] != aPIZZ &&	
		(randbw(0, 100) < icf[ii].rbell_freq)) {
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
		float ndur2 = (setime[pos2][v] - sstime[pos1][v]) * 100 / m_pspeed + detime[pos2][v] - dstime[pos1][v];
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
	float ndur = (setime[ei][v] - sstime[i][v]) * 100 / m_pspeed + detime[ei][v] - dstime[i][v];
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
	float ndur = (setime[ei][v] - sstime[i][v]) * 100 / m_pspeed + detime[ei][v] - dstime[i][v];
	int ni = i + noff[i][v];
	// Create ending articulation only if ending dynamics is low
	if (dyn[ei][v] > 40) return;
	// Check if it is last note in current melody
	if (x == ncount - 1 || pause[ni][v]) {
		if (ndur > icf[ii].end_sfl_dur * 3 && randbw(0, 100) < icf[ii].end_sfl_freq) {
			artic[ei][v] = aEND_SFL;
			if (comment_adapt) adapt_comment[ei][v] += "Short fall ending. ";
		}
		else if (ndur > icf[ii].end_pbd_dur * 3 && randbw(0, 100) < icf[ii].end_pbd_freq) {
			artic[ei][v] = aEND_PBD;
			if (comment_adapt) adapt_comment[ei][v] += "Pitchbend down ending. ";
		}
		else if (ndur > icf[ii].end_vib2_dur * 3 && randbw(0, 100) < icf[ii].end_vib2_freq) {
			artic[ei][v] = aEND_VIB2;
			if (comment_adapt) adapt_comment[ei][v] += "Vibrato2 ending. ";
		}
		else if (ndur > icf[ii].end_vib_dur * 3 && randbw(0, 100) < icf[ii].end_vib_freq) {
			artic[ei][v] = aEND_VIB;
			if (comment_adapt) adapt_comment[ei][v] += "Vibrato ending. ";
		}
	}
}

void CGAdapt::ApplyTrem(int &started, int step1, int step2, int v, int ii) {
	if (!started) return;
	started = 0;
	for (int i = step1; i <= step2; ++i) {
		note[i][v] = note[step1][v];
		pause[i][v] = 0;
		len[i][v] = step2 - step1 + 1;
		coff[i][v] = i - step1;
		if (!dyn[i][v]) dyn[i][v] = dyn[i - 1][v];
		dyn[i][v] = (dyn[i][v] * icf[ii].trem_dynamics) / 100;
		midi_ch[i][v] = midi_ch[step1][v];
	}
	int step22 = step2;
	if (step2 < t_generated - 1) {
		step22 = step2 + 1;
		if (step22 + noff[step22][v] < t_generated) {
			step22 += noff[step22][v];
		}
	}
	CountOff(step1, step22);
	artic[step1][v] = aTREM;
	if (comment_adapt) adapt_comment[step1][v] += "Tremolo. ";
}

void CGAdapt::AdaptTrem(int step1, int step2, int v, int ii) {
	if (!icf[ii].trem_maxlen) return;
	int i = step1;
	int first_step = -1;
	int pi = -1;
	int pei = -1;
	int pndur = -1;
	int short_count = 0;
	int started = 0;
	for (int x = 0; x < INT_MAX; x++) {
		if (need_exit) break;
		int ei = max(0, i + len[i][v] - 1);
		if (!pause[i][v]) {
			int ndur = (setime[ei][v] - sstime[i][v]) * 100 / m_pspeed + detime[ei][v] - dstime[i][v];
			if (ndur < icf[ii].trem_maxlen) {
				++short_count;
				if (pi > -1) {
					// Start to start time
					int nss = (sstime[i][v] - sstime[pi][v]) * 100 / m_pspeed + dstime[i][v] - dstime[pi][v];
					// Two short notes follow
					if (nss < icf[ii].trem_maxlen && note[i][v] == note[pi][v] && ndur < pndur * 1.1 && ndur > pndur * 0.9) {
						if (short_count >= icf[ii].trem_min_repeats) {
							started = 1;
						}
					}
					else {
						ApplyTrem(started, first_step, pei, v, ii);
						short_count = 1;
						first_step = i;
					}
				}
				else {
					first_step = i;
				}
				pi = i;
				pei = i + len[i][v] - 1;
				pndur = ndur;
			}
			else {
				ApplyTrem(started, first_step, pei, v, ii);
				pi = -1;
				short_count = 0;
				first_step = -1;
			}
		}
		if (noff[i][v] == 0) break;
		i += noff[i][v];
		if (i >= step2) break;
	}
	ApplyTrem(started, first_step, pei, v, ii);
}

// Randomize note velocity
void CGAdapt::AdaptRndVel(int v, int x, int i, int ii, int ei, int pi, int pei)
{
	float rv = icf[ii].rnd_vel;
	int ok = 1;
	if (icf[ii].rnd_vel > 0) {
		if (icf[ii].type == itEIS) {
			// Prevent velocity randomization of legato transitions, because this can create gliss
			if (i && !pause[i - 1][v] && note[i-1][v] != note[i][v]) ok = 0;
			// Prevent velocity randomization of short nonlegato notes, because they sound bad at low velocity with Friedlander
			if ((setime[ei][v] - sstime[i][v]) * 100 / m_pspeed + detime[ei][v] - dstime[i][v] < icf[ii].vel_normal_minlen) ok = 0;
		}
		if (icf[ii].type == itSMB || icf[ii].type == itSIVOR || icf[ii].type == itSMW) {
			// Prevent velocity randomization of flexible legato transitions, because this can shift tempo
			if (i && !pause[i - 1][v] && note[i - 1][v] != note[i][v]) ok = 0;
		}
		if (ok) {
			// If note repeats, increase randomization range
			if (i > 0 && note[i - 1][v] == note[i][v] && icf[ii].rnd_vel_repeat) {
				rv = icf[ii].rnd_vel_repeat;
				if (comment_adapt) adapt_comment[i][v] += "Repeat note random vel. ";
			}
			int max_shift = vel[i][v] * rv / 100.0;
			vel[i][v] = randbw(max(1, vel[i][v] - max_shift), min(127, vel[i][v] + max_shift));
		}
	}
}

void CGAdapt::CalculateVoiceStages() {
	// Detect voice stages
	if (stages_calculated) return;
	vector<int> voices_in_instr(icf.size()); // How many voices use each instrument
	vector<int> voices_in_track(icf.size()); // How many voices use each initial track
	vector<map<int, int>> tracks_in_instr(icf.size()); // How many tracks use each instrument
	map<int, int> voices_in_trackchan; // How many voices use each resulting track/channel
	map<int, int> stages_in_trackchan; // How many stages use each resulting track/channel
	map<int, map<int, int>> tcs_instr; // instrument for each trackchan/stage
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
		// if instrument is solo or stage 0 is occupied with a different instrument
		if (icf[ii].poly > 1) {
			// Assign single-stage instrument
			if (icf[ii].single_stage) v_stage[v] = 0;
			else {
				// Scan each stage for this trackchan
				int found = -1;
				for (auto const& it : tcs_instr[trackchan]) {
					if (it.second == ii) {
						found = it.first;
					}
				}
				if (found > -1) {
					// Use same stage if this poly instrument was already sent
					v_stage[v] = found;
				}
				else {
					// Create new stage for this trackchan and send voice there
					v_stage[v] = tcs_instr[trackchan].size();
				}
			}
		}
		else {
			// For solo instrument alwas create new stage
			v_stage[v] = tcs_instr[trackchan].size();
		}
		// Calculate instrument config for voice
		if (icf[ii].child.find(v_itrack[v]) != icf[ii].child.end()) {
			ii = icf[ii].child[v_itrack[v]];
			if (icf[ii].child.find(v_stage[v]) != icf[ii].child.end()) {
				ii = icf[ii].child[v_stage[v]];
			}
			instr[v] = ii;
		}
		// Set instrument for each exported track
		t_instr[icf[ii].track] = ii;
		tcs_instr[trackchan][v_stage[v]] = ii;
	}
	stages_calculated = 1;
}

void CGAdapt::ExportVoiceStages() {
	ofstream fs;
	CreateDirectory(as_dir, NULL);
	fs.open(as_dir + "\\" + as_fname + ".csv");
	fs << "SRC;SRC track;Voice;IGroup;Instr;Stage;Track;Chan;Port;Poly;\n";
	for (int v = 0; v < v_cnt; v++) {
		int ii = instr[v];
		fs << track_id[v] << ";";
		fs << track_name[v] << ";";
		fs << v << ";";
		fs << icf[ii].group << ";";
		fs << icf[ii].name << ";";
		fs << v_stage[v] << ";";
		fs << icf[ii].track << ";";
		fs << icf[ii].channel << ";";
		fs << icf[ii].port << ";";
		fs << icf[ii].poly << ";";
		fs << "\n";
	}
	fs.close();
}

void CGAdapt::SetPauseDyn(int v, int step1, int step2) {
	int pos1 = -1;
	int pos2 = -1;
	int step11 = min(step2, max(1, step1));
	for (int i = step11; i <= step2; i++) {
		if (pos1 == -1) {
			if (pause[i][v]) {
				pos1 = i;
			}
		}
		else {
			if (!pause[i][v]) {
				pos2 = i;
				for (int x = pos1; x < pos2; ++x) {
					dyn[x][v] = (dyn[pos1 - 1][v] * (pos2 - x) + dyn[pos2][v] * (x - pos1 + 1)) / (pos2 - pos1 + 1);
				}
				pos1 = -1;
			}
		}
		// Fill all pauses
		if (i && pause[i][v]) dyn[i][v] = dyn[i - 1][v];
	}
}

void CGAdapt::Adapt(int step1, int step2) {
	if (step2 < 0) return;
	if (step1 > t_adapted) {
		CString est;
		est.Format("Attempt to adapt from %d to %d steps, while last adapted step was %d. This means that some part of music is going to be not validated and not adapted.",
			step1, step2, t_adapted);
		WriteLog(5, est);
	}
	// Calculate source time where it was not set
	CountSTime(step1, step2);
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
	ExportVoiceStages();
	for (int v = 0; v < v_cnt; v++) {
		int ii = instr[v]; // Instrument id
		int ncount = 0;
		// Move to note start
		if (coff[step1][v] > 0) step1 = step1 - coff[step1][v];
		AdaptTrem(step1, step2, v, ii);
		// Count notes
		for (int i = step1; i <= step2; i++) {
			if (i + len[i][v] > step2 + 1) break;
			ncount++;
			if (noff[i][v] == 0) break;
			i += noff[i][v] - 1;
			// Clear adaptation comment
			adapt_comment[i][v].Empty();
		}
		// Scale dynamics 
		for (int i = step1; i <= step2; i++) {
			dyn[i][v] = max(0, min(127,
				dyn[i][v] * (icf[ii].dyn_range2 - icf[ii].dyn_range1) / 100.0 +
				icf[ii].dyn_range1 * 127.0 / 100.0));
		}
		// Set vel to dyn
		for (int i = step1; i <= step2; i++) {
			vel[i][v] = max(1, dyn[i][v]);
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
			if (!pause[i][v]) {
				AdaptAutoLegatoStep(v, x, i, ii, ei, pi, pei);
				CheckShortStep(v, x, i, ii, ei, pi, pei);
				// Instrument-specific adaptation
				// Piano
				if (icf[ii].type == itPerc) {
					AdaptAllAheadStep(v, x, i, ii, ei, pi, pei);
					AdaptLengroupStep(v, x, i, ii, ei, pi, pei);
				}
				// Embertone Intimate Strings
				if (icf[ii].type == itEIS) {
					AdaptAllAheadStep(v, x, i, ii, ei, pi, pei);
					AdaptLongBell(v, x, i, ii, ei, pi, pei, ncount);
					AdaptReverseBell(v, x, i, ii, ei, pi, pei);
					AdaptVibBell(v, x, i, ii, ei, pi, pei);
					AdaptSlurStep(v, x, i, ii, ei, pi, pei);
					AdaptRetriggerRebowStep(v, x, i, ii, ei, pi, pei);
					AdaptNonlegatoStep(v, x, i, ii, ei, pi, pei);
					AdaptStaccatoStep(v, x, i, ii, ei, pi, pei);
					AdaptPizzStep(v, x, i, ii, ei, pi, pei);
					AdaptAheadStep(v, x, i, ii, ei, pi, pei);
					AdaptAttackStep(v, x, i, ii, ei, pi, pei);
				}
				// Samplemodeling Brass
				if (icf[ii].type == itSMB) {
					AdaptLongBell(v, x, i, ii, ei, pi, pei, ncount);
					AdaptReverseBell(v, x, i, ii, ei, pi, pei);
					AdaptVibBell(v, x, i, ii, ei, pi, pei);
					AdaptRetriggerNonlegatoStep(v, x, i, ii, ei, pi, pei);
					AdaptNonlegatoStep(v, x, i, ii, ei, pi, pei);
					AdaptFlexAheadStep(v, x, i, ii, ei, pi, pei);
					AdaptNoteEndStep(v, x, i, ii, ei, pi, pei, ncount);
				}
				// Soundiron Voices of Rapture
				if (icf[ii].type == itSIVOR) {
					AdaptLongBell(v, x, i, ii, ei, pi, pei, ncount);
					AdaptReverseBell(v, x, i, ii, ei, pi, pei);
					AdaptVibBell(v, x, i, ii, ei, pi, pei);
					AdaptAllAheadStep(v, x, i, ii, ei, pi, pei);
					AdaptNonlegatoStep(v, x, i, ii, ei, pi, pei);
					//vel[i][v] = randbw(1, 126);
				}
				// Samplemodeling Woodwinds
				if (icf[ii].type == itSMW) {
					AdaptLongBell(v, x, i, ii, ei, pi, pei, ncount);
					AdaptReverseBell(v, x, i, ii, ei, pi, pei);
					AdaptVibBell(v, x, i, ii, ei, pi, pei);
					AdaptRetriggerNonlegatoStep(v, x, i, ii, ei, pi, pei);
					AdaptNonlegatoStep(v, x, i, ii, ei, pi, pei);
					AdaptAheadStep(v, x, i, ii, ei, pi, pei);
					AdaptAttackStep(v, x, i, ii, ei, pi, pei);
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
				// Randomize note starts for piano and non-legato solo instruments
				if (icf[ii].rand_start > 0 && (icf[ii].type == itPerc || artic[i][v] == aNONLEGATO || artic[i][v] == aSTAC ||
					artic[i][v] == aPIZZ || artic[i][v] == aTREM)) {
					float max_shift = (setime[ei][v] - sstime[i][v]) * 100 / m_pspeed * icf[ii].rand_start / 100;
					if ((icf[ii].rand_start_max > 0) && (max_shift > icf[ii].rand_start_max)) max_shift = icf[ii].rand_start_max;
					dstime[i][v] += (rand01() - 0.5) * max_shift;
				}
				// Randomize note ends
				if (icf[ii].rand_end > 0) {
					float max_shift = (setime[ei][v] - sstime[i][v]) * 100 / m_pspeed * icf[ii].rand_end / 100;
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
		SetPauseDyn(v, step1, step2);
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
