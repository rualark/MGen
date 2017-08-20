// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../stdafx.h"
#include "GMidi.h"
#include "SmRnd.h"

#include "../midifile/MidiFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW 
#endif

CGMidi::CGMidi()
{
	mo = 0;
}


CGMidi::~CGMidi()
{
	StopMIDI();
}

void CGMidi::SaveMidi(CString dir, CString fname)
{
	MidiFile midifile;
	midifile.addTracks(v_cnt);    // Add another two tracks to the MIDI file
	int tpq = 120;                // ticks per quarter note
	int tpc = 60 * midifile_out_mul; // ticks per croche
	midifile.setTicksPerQuarterNote(tpq);
	int track = 0;
	int channel = 0;
	// Add some expression track (track 0) messages:
	string st = fname;
	midifile.addTrackName(track, 0, st);
	// Save tempo
	midifile.addTempo(track, 0, tempo[0]);
	for (int i = 0; i < t_generated; i++) {
		midifile.addTempo(track, (tpq * 4) + tpc * i, tempo[i]);
	}
	// Save notes
	for (int v = 0; v < v_cnt; v++) {
		track = v + 1;
		if (track_id[v]) track = track_id[v];
		channel = v;
		string st = InstGName[instr[v]];
		// Replace piano with other instrument, because otherways it generates two-stave track in Sibelius
		if (st == "Piano") st = "Vibraphone";
		midifile.addTrackName(track, 0, st);
		midifile.addPatchChange(track, 0, channel, 0); // 40=violin
		for (int i = 0; i < t_generated; i++) if (pause[i][v] == 0) {
			midifile.addNoteOn(track, (tpq * 4) + tpc*i, channel, note[i][v], dyn[i][v]);
			midifile.addNoteOff(track, (tpq * 4) + tpc*(i + len[i][v]) - 1, channel, note[i][v], 0);
			if (midifile_export_comments && !comment2[i][v].IsEmpty()) {
				string st;
				st = comment2[i][v];
				midifile.addLyric(track, (tpq * 4) + tpc*i, st);
			}
			if (midifile_export_marks && !mark[i][v].IsEmpty()) {
				string st;
				st = mark[i][v];
				midifile.addLyric(track, (tpq * 4) + tpc*i, st);
			}
			if (noff[i][v] == 0) break;
			i += noff[i][v] - 1;
		}
	}
	midifile.sortTracks();         // ensure tick times are in correct order
	midifile.write(dir + "\\" + fname + ".mid");
}

void CGMidi::LoadMidi(CString path)
{
	PmTimestamp time_start = CGLib::time();
	if (!fileExists(path)) {
		CString est;
		est.Format("Cannot find file %s", path);
		WriteLog(5, est);
		return;
	}
	MidiFile midifile;
	if (!midifile.read(path)) {
		CString est;
		est.Format("Error reading midi file %s", path);
		WriteLog(5, est);
		return;
	}
	//midifile.joinTracks();
	midifile.linkNotePairs();
	midifile.doTimeAnalysis();

	if (midifile.getTrackCount() < 2) {
		CString st;
		st.Format("Detected only %d tracks while loading file %s. Probably MIDI type 0. Splitting midi tracks by channels. Track names are not supported for MIDI type 0 yet.", midifile.getTrackCount(), path);
		WriteLog(1, st);
		midifile.splitTracksByChannel();
		midifile_type = 0;
	}

	midifile.absoluteTicks();
	int tpq = midifile.getTicksPerQuarterNote();
	int tpc = (float)tpq / (float)2 / (float)midifile_in_mul; // ticks per croche
	vector<int> vlast_step(MAX_VOICE);
	vector<int> vlast_pitch(MAX_VOICE);
	vector<int> voverlap(MAX_VOICE);
	vector<int> vdist(MAX_VOICE);
	CString st, tnames = "", inames = "";
	// Convert track instrument ids to voice instrument ids
	vector<int> instr2 = instr;

	midifile_loaded = 1;
	int last_step = 0;
	// Load tempo
	for (int track = 0; track < midifile.getTrackCount(); track++) {
		for (int i = 0; i < midifile[track].size(); i++) {
			MidiEvent* mev = &midifile[track][i];
			if (mev->isTempo()) {
				int pos = round(mev->tick / (float)tpc);
				if (pos >= t_allocated) ResizeVectors(max(t_allocated * 2, pos + 1));
				tempo[pos] = mev->getTempoBPM() * midifile_in_mul;
				if (pos > last_step) last_step = pos;
			}
		}
	}
	// Fill tempo
	for (int z = 1; z <= last_step; z++) {
		if (tempo[z] == 0) tempo[z] = tempo[z - 1];
	}
	int last_step_tempo = last_step;
	UpdateTempoMinMax(0, last_step);
	CountTime(0, last_step);
	last_step = 0;
	int v1 = 0;
	int v2 = 0;
	int v = 0;

	for (int track = 1; track < midifile.getTrackCount(); track++) {
		if (need_exit) break;
		if (track > 1) {
			// Get next free voice
			v1 = v2 + 1;
			// Voice interval = 1
			v2 = v1;
			// Current voice is first voice in interval
			v = v1;
			if (v >= MAX_VOICE) {
				CString st;
				st.Format("Too many voices need to be created for loading file %s. Maximum number of voices %d. Increase MAX_VOICE", path, MAX_VOICE);
				WriteLog(5, st);
				break;
			}
			// Resize vectors for new voice number
			if (v > v_cnt - 1) ResizeVectors(t_allocated, v + 1);
		}
		// Save track id
		track_id[v] = track;
		track_vid[v] = 0;
		// Convert voice instrument to track instrument
		instr[v] = instr2[track_id[v]-1];
		for (int i = 0; i<midifile[track].size(); i++) {
			if (need_exit) break;
			MidiEvent* mev = &midifile[track][i];
			int chan = mev->getChannel();
			// Get track names
			if (mev->isMetaMessage()) {
				if (mev->getMetaType() == 0x03) {
					track_name[v].Empty();
					for (int x = 0; x < mev->size(); x++) {
						track_name[v] += mev->data()[x];
					}
					// Remove first data items
					track_name[v] = track_name[v].Mid(3);
					st.Format("%d", v);
					tnames += " \n" + st + "=" + track_name[v];
					// Map track name to instrument name
					for (int i = 0; i < MAX_INSTR; i++) {
						// Exact match
						//if (InstName[i] == track_name[v]) instr[v] = i;
						// Search inside track name
						//else if (track_name[v].Find(InstName[i], 0) != -1) instr[v] = i;
					}
				}
			}
			if (mev->isNoteOn()) {
				int pos = round(mev->tick / (float)tpc);
				int pitch = mev->getKeyNumber();
				int myvel = mev->getVelocity();
				int nlen = round((mev->tick + mev->getTickDuration()) / (float)tpc) - pos;
				// Check if note too long
				if (nlen > MAX_LEN) {
					if (warning_loadmidi_long < MAX_WARN_MIDI_LONG) {
						CString st;
						st.Format("Note too long and will be cut short at %d track %d tick with %d tpc (mul %.03f) approximated to %d step in file %s. Decrease midifile_in_mul can resolve this situation.", track, mev->tick, tpc, midifile_in_mul, pos, path);
						WriteLog(1, st);
						warning_loadmidi_long++;
					}
					nlen = MAX_LEN;
				}
				if (nlen < 1) nlen = 1;
				// Allocate one more step for note overwrite checking
				if (pos + nlen + 1 >= t_allocated) ResizeVectors(max(pos + nlen + 1, t_allocated * 2));
				// Fill tempo
				if (!tempo[pos + nlen - 1]) {
					for (int z = last_step_tempo + 1; z < pos + nlen; ++z) {
						if (!tempo[z]) tempo[z] = tempo[z - 1];
					}
					// Count new time
					CountTime(last_step_tempo + 1, pos + nlen - 1);
					// Set last step that has tempo
					last_step_tempo = pos + nlen - 1;
				}
				// Fallback
				if (!tempo[pos]) tempo[pos] = 100;
				int delta = (float)(mev->tick - pos*tpc) / (float)tpc * 30000.0 / (float)tempo[pos];
				// Check alignment
				if (abs(delta) > MAX_ALLOW_DELTA && (warning_loadmidi_align < MAX_WARN_MIDI_ALIGN)) {
					CString st;
					st.Format("Note moved %d ms to fit step grid at %d track, %d tick with %d tpc (mul %.03f) approximated to %d step (deviation %d ticks) in file %s. Increasing midifile_in_mul will improve approximation.", 
						delta, track, mev->tick, tpc, midifile_in_mul, pos, mev->tick - pos*tpc, path);
					WriteLog(1, st);
					warning_loadmidi_align++;
				}
				// Find overlaps and distance
				if (instr_poly[instr[v]] > 1) {
					for (int x = v1; x <= v2; ++x) {
						// Overlap happens only in case when positions overlap
						if (note[pos][x]) {
							voverlap[x] = 1;
							vdist[x] = 1000;
							// Check if note too short
							if (len[pos][x] < 2) {
								if (warning_loadmidi_short < MAX_WARN_MIDI_SHORT) {
									CString st;
									st.Format("Note %s too short and gets same step with next note %s at %d track, %d tick with %d tpc (mul %.03f) approximated to %d step in file %s. Increasing midifile_in_mul will improve approximation.", GetNoteName(note[pos][x]), GetNoteName(pitch), track, mev->tick, tpc, midifile_in_mul, pos, path);
									WriteLog(1, st);
									warning_loadmidi_short++;
								}
							}
						}
						else {
							voverlap[x] = 0;
							vdist[x] = abs(vlast_pitch[x] - pitch);
						}
					}
					// Find best voice
					int min_vdist = 1000;
					for (int x = v1; x <= v2; ++x) {
						if (vdist[x] < min_vdist) {
							min_vdist = vdist[x];
							v = x;
						}
					}
					// If no voice without overlaps, create new
					if (min_vdist == 1000) {
						v2++;
						v = v2;
						// Copy instrument
						instr[v] = instr[v1];
						if (v >= MAX_VOICE) {
							CString st;
							st.Format("Too many voices need to be created for loading file %s. Maximum number of voices %d. Increase MAX_VOICE", path, MAX_VOICE);
							WriteLog(5, st);
							break;
						}
						track_id[v] = track;
						track_vid[v] = v - v1;
						track_name[v] = track_name[v1];
					}
				} // if (instr_poly[instr[v]] > 1)
				else {
					// Check if overwriting long overlap
					if (!pause[pos][v] && noff[pos][v]) {
						float ndur = etime[pos + nlen - 1] - stime[pos];
						float ndur2 = etime[pos + noff[pos][v] - 1] - stime[pos - coff[pos][v]];
						// Calculate overlap (abs is protection from bugs)
						float ov = abs(etime[pos + noff[pos][v] - 1] - stime[pos]);
						// Is overlap long?
						if (ov > ndur * MAX_OVERLAP_MONO || ov > ndur2 * MAX_OVERLAP_MONO) if (warning_loadmidi_overlap < MAX_WARN_MIDI_OVERLAP) {
							CString st;
							st.Format("Error: too long overlap (voice %d) %.0f ms at step %d (note lengths %.0f, %.0f ms) in monophonic instrument %s/%s. Probably sending polyphonic instrument to monophonic.",
								v, ov, pos, ndur, ndur2, InstGName[instr[v]], InstCName[instr[v]]);
							WriteLog(1, st);
							++warning_loadmidi_overlap;
						}
					}
					// Clear any garbage after this note (can build up due to overwriting a longer note)
					if (len[pos + nlen][v]) {
						// Safe right limit
						for (int z = pos + nlen; z < len.size(); ++z) {
							// Stop clearing if current step is free
							if (!len[z][v]) break;
							// Clear step
							len[z][v] = 0;
							note[z][v] = 0;
							pause[z][v] = 0;
							dyn[z][v] = 0;
							coff[z][v] = 0;
						}
					}
				}
				// Resize vectors for new voice number
				if (v > v_cnt - 1) ResizeVectors(t_allocated, v + 1);
				// Search for last note
				if ((pos > 0) && (note[pos - 1][v] == 0)) {
					int last_pause = pos - 1;
					for (int z = pos - 1; z >= 0; z--) {
						if (note[z][v] != 0) break;
						last_pause = z;
					}
					// Set previous pause
					FillPause(last_pause, pos - last_pause - 1, v);
					// Set additional variables
					CountOff(last_pause, pos - 1);
				}
				// Set note steps
				for (int z = 0; z < nlen; z++) {
					note[pos + z][v] = pitch;
					len[pos + z][v] = nlen;
					dyn[pos + z][v] = myvel;
					midi_ch[pos + z][v] = chan;
					pause[pos + z][v] = 0;
					coff[pos + z][v] = z;
				}
				// Set midi delta only to first step of note, because in in-note steps you can get different calculations for different tempo
				midi_delta[pos][v] = delta;
				// Set additional variables
				CountOff(pos, pos + nlen - 1);
				UpdateNoteMinMax(pos, pos + nlen - 1);
				if (pos + nlen - 1 > last_step) last_step = pos + nlen - 1;
				if (pos + nlen - 1 > vlast_step[v]) vlast_step[v] = pos + nlen - 1;
				if (t_generated < pos) t_generated = pos;
				// Save last pitch
				vlast_pitch[v] = pitch;
			}
		}
		// If track is empty, create a single pause
		if (!note[0][v] && !pause[0][v] && !len[0][v]) {
			FillPause(0, 1, v);
		}
	} // for track
	if (need_exit) return;
	// Add closing pauses
	if (last_step + TAIL_STEPS + 1 >= t_allocated) ResizeVectors(max(last_step + TAIL_STEPS + 1, t_allocated * 2));
	for (int z = last_step + 1; z <= last_step + TAIL_STEPS; ++z) {
		if (!tempo[z]) tempo[z] = tempo[z - 1];
	}
	// Count new time
	CountTime(last_step + 1, last_step + TAIL_STEPS);
	last_step = last_step + TAIL_STEPS;
	for (int v = 0; v < v_cnt; v++) {
		if (vlast_step[v] < last_step) {
			int len2 = last_step - vlast_step[v];
			FillPause(vlast_step[v]+1, len2, v);
		}
	}
	// Merge small overlaps
	for (int i = 0; i <= last_step; ++i) {
		// Cycle through steps to ensure that moved note is checked later
		for (int v = 0; v < v_cnt; ++v) if (instr_poly[instr[v]] > 1) {
			// Look for note start
			if (!coff[i][v] && !pause[i][v]) {
				// Do not include dstime/detime in time calculation, because it can change result
				// Do not use playback speed in time calculation, because all calculateion are relative in this algorithm
				float nlen = etime[i + noff[i][v] - 1] - stime[i];
				// Find other voices of same track having notes at same step
				for (int v2 = 0; v2 <= v_cnt; ++v2) if (v != v2 && track_id[v] == track_id[v2] && !pause[i][v2]) {
					float nlen2 = etime[i + noff[i][v2] - 1] - stime[i - coff[i][v2]];
					// Calculate overlap (abs is protection from bugs)
					float ov = abs(etime[i + noff[i][v2] - 1] - stime[i]);
					// Is overlap small?
					if (ov > nlen * MAX_OVERLAP_POLY || ov > nlen2 * MAX_OVERLAP_POLY) continue;
					int free = 0;
					// Move note from v to v2 voice
					for (int z = i; z <= i + noff[i][v] - 1; ++z) {
						// Check if overwritten note finished
						if (pause[z][v2]) free = 1;
						// If overwritten note finished, do not overwrite next note
						if (free && !pause[z][v2]) break;
						// Copy note
						note[z][v2] = note[z][v];
						len[z][v2] = len[z][v];
						coff[z][v2] = coff[z][v];
						pause[z][v2] = pause[z][v];
						dyn[z][v2] = dyn[z][v];
						midi_ch[z][v2] = midi_ch[z][v];
						// Clear old note
						note[z][v] = 0;
						pause[z][v] = 1;
						dyn[z][v] = 0;
					}
					// Log
					if (debug_level > 1) {
						CString st;
						st.Format("Merged note %s at step %d to note %s from voice %d to voice %d (track %d)",
							GetNoteName(note[i][v2]), i, GetNoteName(note[i - 1][v2]), v, v2, track_id[v]);
						WriteLog(0, st);
					}
				}
			}
		}
	}
	// Check length of notes is correct
	FixLen(0, last_step);
	// Set additional variables
	CountOff(0, last_step);
	//CountTime(0, last_step);
	UpdateNoteMinMax(0, last_step);
	//UpdateTempoMinMax(0, last_step);
	// Send last
	t_generated = last_step + 1;
	if (tnames != "") {
		CString est;
		est.Format("MIDI file track names: %s", tnames);
		WriteLog(0, est);
	}
	if (inames != "") {
		CString est;
		est.Format("MIDI file instrument names: %s", inames);
		WriteLog(0, est);
	}
	// Count time
	PmTimestamp time_stop = CGLib::time();
	CString est;
	est.Format("LoadMidi successfully loaded %d steps (in %d ms)", t_generated, time_stop - time_start);
	WriteLog(0, est);
}

void CGMidi::LoadCantus(CString path)
{
	PmTimestamp time_start = CGLib::time();
	if (!fileExists(path)) {
		CString est;
		est.Format("Cannot find file %s", path);
		WriteLog(5, est);
		return;
	}
	MidiFile midifile;
	if (!midifile.read(path)) {
		CString est;
		est.Format("Error reading midi file %s", path);
		WriteLog(5, est);
		return;
	}
	midifile.linkNotePairs();
	//midifile.joinTracks();
	midifile.doTimeAnalysis();

	midifile.absoluteTicks();

	int tpq = midifile.getTicksPerQuarterNote();
	// ticks per croche
	int tpc = (float)tpq / (float)2 / (float)midifile_in_mul;

	vector <float> tempo2;
	long tempo_count = 0;
	int last_step = 0;
	// Load tempo
	for (int track = 0; track < midifile.getTrackCount(); track++) {
		for (int i = 0; i < midifile[track].size(); i++) {
			MidiEvent* mev = &midifile[track][i];
			int pos = round(mev->tick / (float)tpc);
			if (pos >= tempo_count) {
				tempo_count = pos + 1;
				tempo2.resize(tempo_count);
			}
			if (mev->isTempo()) {
				tempo2[pos] = mev->getTempoBPM() * midifile_in_mul;
			}
			if (pos > last_step) last_step = pos;
		}
	}
	// Fill tempo
	for (int z = 1; z < last_step; z++) {
		if (tempo2[z] == 0) tempo2[z] = tempo2[z - 1];
	}

	int cid = 0;
	int nid = 0;
	vector<CString> incom; // Incoming comments
	vector <int> c;
	vector <int> cl;
	vector <float> ct;
	int bad = 0;
	int last_pos = 0;
	CString lyrics_pending;
	for (int track = 0; track < midifile.getTrackCount(); track++) {
		float last_tick = 0;
		for (int i = 0; i<midifile[track].size(); i++) {
			MidiEvent* mev = &midifile[track][i];
			float pos2 = mev->tick;
			int pos = round(mev->tick / (float)tpc);
			//float time = midifile.getTimeInSeconds(mev->tick);
			if (mev->isMetaMessage()) {
				// Lyrics
				if (mev->getMetaType() == 5) {
					CString st;
					st.Empty();
					for (int x = 0; x < mev->size(); x++) {
						st += mev->data()[x];
					}
					// Remove first data items
					st = st.Mid(3);
					st = st.Trim();
					st.MakeLower();
					// Assign lyrics if this position was already sent
					if (pos == last_pos) {
						incom.resize(c.size());
						incom[c.size()-1] = st;
						lyrics_pending.Empty();
					}
					// Else record lyrics
					else {
						lyrics_pending = st;
					}
				}
			}
			if (mev->isNoteOn()) {
				float nlen2 = mev->getTickDuration();
				int nlen = round((mev->tick + mev->getTickDuration()) / (float)tpc) - pos;
				// Check for pause
				if (pos2 - last_tick > (float)tpc / 2) {
					// Add cantus if it is long
					if (nid > 5 && !bad) {
						cantus.push_back(c);
						cantus_len.push_back(cl);
						cantus_tempo.push_back(ct);
						cantus_incom.push_back(incom);
						//lyrics_pending.Empty();
					}
					// Go to next cantus
					nid = 0;
				}
				if (nid == 0) {
					bad = 0;
					// Add new cantus
					cid++;
					c.clear();
					cl.clear();
					ct.clear();
					incom.clear();
				}
				// Add new note
				if ((nid == 0) || (c[nid - 1] != mev->getKeyNumber())) {
					// Check if current note already set
					if (!nlen) {
						if (warning_loadmidi_short < MAX_WARN_MIDI_SHORT) {
							CString st;
							st.Format("Note too short: tick %d, track %d, chan %d, tpc %d (mul %.03f) in file %s. Increasing midifile_in_mul will improve approximation.", mev->tick, track, mev->getChannel(), tpc, midifile_in_mul, path);
							WriteLog(1, st);
							warning_loadmidi_short++;
						}
						bad = 1;
					}
					int nlen = round((mev->tick + mev->getTickDuration()) / (float)tpc) - pos;
					// Check if note too long
					if (nlen > MAX_LEN) {
						if (warning_loadmidi_long < MAX_WARN_MIDI_LONG) {
							CString st;
							st.Format("Note too long: tick %d, track %d, chan %d, tpc %d (mul %.03f) in file %s. Decreasing midifile_in_mul can help.", mev->tick, track, mev->getChannel(), tpc, midifile_in_mul, path);
							WriteLog(1, st);
							warning_loadmidi_long++;
						}
						nlen = MAX_LEN;
						bad = 1;
					}
					// Avoid repeats
					if (c.size() == 0 || c[c.size() - 1] != mev->getKeyNumber()) {
						c.push_back(mev->getKeyNumber());
						cl.push_back(nlen);
						ct.push_back(tempo2[pos]);
						// Add pending lyrics
						if (!lyrics_pending.IsEmpty()) {
							incom.resize(c.size());
							incom[c.size() - 1] = lyrics_pending;
							lyrics_pending.Empty();
						}
						nid++;
					}
				}
				// Save last time
				last_tick = pos2 + nlen2;
				last_pos = pos;
			}
		}
		// Add cantus if it is long
		if (nid > 5 && !bad) {
			cantus.push_back(c);
			cantus_len.push_back(cl);
			cantus_tempo.push_back(ct);
			cantus_incom.push_back(incom);
			nid = 0;
		}
	}
	// Count time
	PmTimestamp time_stop = CGLib::time();
	CString st;
	st.Format("LoadCantus successfully loaded %d canti (in %d ms)", cid + 1, time_stop - time_start);
	WriteLog(0, st);
}

// Load counterpoint
void CGMidi::LoadCP(CString path)
{
	PmTimestamp time_start = CGLib::time();
	if (!fileExists(path)) {
		CString est;
		est.Format("Cannot find file %s", path);
		WriteLog(5, est);
		return;
	}
	MidiFile midifile;
	if (!midifile.read(path)) {
		CString est;
		est.Format("Error reading midi file %s", path);
		WriteLog(5, est);
		return;
	}
	midifile.linkNotePairs();
	midifile.joinTracks();
	midifile.doTimeAnalysis();

	midifile.absoluteTicks();

	int tpq = midifile.getTicksPerQuarterNote();
	// ticks per croche
	int tpc = (float)tpq / (float)2 / (float)midifile_in_mul;

	vector <float> tempo2;
	long tempo_count = 0;
	int last_step = 0;
	// Load tempo
	for (int track = 0; track < midifile.getTrackCount(); track++) {
		for (int i = 0; i < midifile[track].size(); i++) {
			MidiEvent* mev = &midifile[track][i];
			int pos = round(mev->tick / (float)tpc);
			if (pos >= tempo_count) {
				tempo_count = pos + 1;
				tempo2.resize(tempo_count);
			}
			if (mev->isTempo()) {
				tempo2[pos] = mev->getTempoBPM() * midifile_in_mul;
			}
			if (pos > last_step) last_step = pos;
		}
	}
	// Fill tempo
	for (int z = 1; z < last_step; z++) {
		if (tempo2[z] == 0) tempo2[z] = tempo2[z - 1];
	}

	vector<vector<pair<int, int>>> inter; // Intermediate structure for loading counterpoint
	int cid = 0; // counterpoint
	int nid = 0; // note
	int hid = 0; // harmony
	int pos_old = -1, pos_new = 0;
	vector <vector<int>> cpos;
	vector <int> cl; // length
	vector <int> cp; // position
	vector <float> ct; // tempo
	vector <int> min_len, max_len;
	int bad = 0;
	for (int track = 0; track < midifile.getTrackCount(); track++) {
		float last_tick = 0, last_tick2 = numeric_limits<float>::infinity();
		for (int i = 0; i<midifile[track].size(); i++) {
			MidiEvent* mev = &midifile[track][i];
			//float time = midifile.getTimeInSeconds(mev->tick);
			float pos2 = mev->tick;
			int pos = round(mev->tick / (float)tpc);
			if (mev->isNoteOn()) {
				float nlen2 = mev->getTickDuration();
				int nlen = round((mev->tick + mev->getTickDuration()) / (float)tpc) - pos;
				// If new column and previous column had notes
				ProcessInter(pos, pos_old, inter, hid, min_len, max_len);
				// Check for pause
				if (pos2 - last_tick > (float)tpc / 2) {
					// Add cpoint if it is long
					if (inter.size() > 5 && !bad) {
						cid++;
						// Get maximum voice count
						int max_voice = 0;
						for (int x = 0; x < inter.size(); ++x) max_voice = max(max_voice, inter[x].size());
						cpoint.resize(cid);
						cpoint[cid-1].resize(max_voice, vector<int>(inter.size()));
						// Send cpoint
						for (int x = 0; x < inter.size(); ++x) {
							for (int i = 0; i < inter[x].size(); ++i) {
								cpoint[cid-1][i][x] = inter[x][i].first;
							}
						}
						cantus_len.push_back(cl);
						cantus_tempo.push_back(ct);
						cpos.push_back(cp);
					}
					// Go to next cantus
					nid = 0;
					hid = 0;
				}
				else if (pos2 - last_tick2 > (float)tpc / 2) {
					CString st;
					st.Format("Pauses are prohibited inside counterpoint: tick %d, track %d, chan %d, tpc %d (mul %.03f) in file %s. Cannot load.", mev->tick, track, mev->getChannel(), tpc, midifile_in_mul, path);
					WriteLog(1, st);
					bad = 1;
				}
				// If new column, then after checking for pause we can now reset last_tick2 to maximum
				if (pos != pos_old) last_tick2 = numeric_limits<float>::infinity();
				if (nid == 0) {
					bad = 0;
					// Add new cpoint
					cl.clear();
					ct.clear();
					cp.clear();
					inter.clear();
					min_len.clear();
					max_len.clear();
				}
				// Add new note
				if (!nlen) {
					if (warning_loadmidi_short < MAX_WARN_MIDI_SHORT) {
						CString st;
						st.Format("Note too short: tick %d, track %d, chan %d, tpc %d (mul %.03f) in file %s. Increasing midifile_in_mul will improve approximation.", mev->tick, track, mev->getChannel(), tpc, midifile_in_mul, path);
						WriteLog(1, st);
						warning_loadmidi_short++;
					}
					bad = 1;
				}
				if (pos != pos_old) {
					// Save old len
					if (hid > 0) cl[hid-1] = min_len[hid-1];
					hid++;
					inter.resize(hid);
					cl.push_back(nlen);
					ct.push_back(tempo2[pos]);
					cp.push_back(pos2);
					min_len.push_back(100000000);
					max_len.push_back(0);
				}
				inter[hid-1].push_back(make_pair(mev->getKeyNumber(), nlen));
				nid++;
				// Save last time
				last_tick = max(last_tick, pos2 + nlen2);
				last_tick2 = min(last_tick2, pos2 + nlen2);
				pos_old = pos;
				pos_new = pos + nlen;
			}
		}
		// Add cpoint if it is long
		if (inter.size() > 5 && !bad) {
			ProcessInter(pos_new, pos_old, inter, hid, min_len, max_len);
			cid++;
			// Get maximum voice count
			int max_voice = 0;
			for (int x = 0; x < inter.size(); ++x) max_voice = max(max_voice, inter[x].size());
			cpoint.resize(cid);
			cpoint[cid - 1].resize(max_voice, vector<int>(inter.size()));
			for (int x = 0; x < inter.size(); ++x) {
				for (int i = 0; i < inter[x].size(); ++i) {
					cpoint[cid - 1][i][x] = inter[x][i].first;
				}
			}
			cantus_len.push_back(cl);
			cantus_tempo.push_back(ct);
			cpos.push_back(cp);
			// Send incom
			cp_incom.resize(cid);
		}
	}
	// Load lyrics
	int dist, min_dist, my_c, my_x;
	cp_incom.resize(cpoint.size());
	for (int track = 0; track < midifile.getTrackCount(); track++) {
		for (int i = 0; i < midifile[track].size(); i++) {
			MidiEvent* mev = &midifile[track][i];
			float pos2 = mev->tick;
			if (mev->isMetaMessage()) {
				// Lyrics
				if (mev->getMetaType() == 5) {
					CString st;
					st.Empty();
					for (int x = 0; x < mev->size(); x++) {
						st += mev->data()[x];
					}
					// Remove first data items
					st = st.Mid(3);
					st = st.Trim();
					st.MakeLower();
					// Search for closest note
					min_dist = INT_MAX;
					for (int c = 0; c < cpos.size(); ++c) {
						for (int x = 0; x < cpos[c].size(); ++x) {
							dist = abs(cpos[c][x] - pos2);
							if (dist <= min_dist) {
								min_dist = dist;
								my_c = c;
								my_x = x;
							}
						}
					}
					if (min_dist < INT_MAX) {
						cp_incom[my_c].resize(my_x + 1);
						cp_incom[my_c][my_x] = st;
					}
				}
			}
		}
	}
	// Count time
	PmTimestamp time_stop = CGLib::time();
	CString st;
	st.Format("LoadCP successfully loaded %d cp (in %d ms)", cid + 1, time_stop - time_start);
	WriteLog(0, st);
}

void CGMidi::ProcessInter(int pos, int pos_old, std::vector<std::vector<std::pair<int, int>>> &inter, int hid, std::vector<int> &min_len, std::vector<int> &max_len)
{
	if (pos != pos_old && inter.size()) {
		if (hid > 1) {
			// Find slurred notes
			for (int i = 0; i < inter[hid - 2].size(); ++i) {
				if (inter[hid - 2][i].second > 0) {
					inter[hid - 1].push_back(make_pair(inter[hid - 2][i].first, inter[hid - 2][i].second));
				}
			}
		}
		// Get min len
		min_len[hid - 1] = pos - pos_old;
		for (int i = 0; i < inter[hid - 1].size(); ++i) {
			if (inter[hid - 1][i].second && min_len[hid - 1] > inter[hid - 1][i].second) min_len[hid - 1] = inter[hid - 1][i].second;
		}
		// Decrease length
		for (int i = 0; i < inter[hid - 1].size(); ++i) {
			if (inter[hid - 1][i].second) inter[hid - 1][i].second -= min_len[hid - 1];
		}
		// Get max len
		for (int i = 0; i < inter[hid - 1].size(); ++i) {
			if (max_len[hid - 1] < inter[hid - 1][i].second) max_len[hid - 1] = inter[hid - 1][i].second;
		}
		// Sort
		sort(inter[hid - 1].begin(), inter[hid - 1].end());
	}
}

void CGMidi::StartMIDI(int midi_device_i, int from)
{
	if (midi_device_i == -1) return;
	// Clear old sent messages
	midi_buf_next.clear();
	midi_sent_msg = 0;
	midi_sent_msg2 = 0;
	midi_sent_t2 = 0;
	midi_sent_t3 = 0;
	// Clear flags
	for (int i = 0; i < v_cnt; i++) warning_note_short[i] = 0;
	midi_last_run = 1;
	buf_underrun = 0;
	midi_play_step = 0;
	midi_start_time = 0;
	if (from > 0) {
		midi_sent = from;
		midi_sent_t = 0; // CGLib::time() + MIDI_BUF_PROTECT
	}
	else {
		midi_sent_t = 0;
		midi_sent = 0;
	}
	if (debug_level > 1) {
		CString est;
		est.Format("Trying to open midi device %d...", midi_device_i);
		WriteLog(4, est);
	}
	mo = new CMidiOut;
	if (mo->StartMidi(midi_device_i)) {
		CString est;
		est.Format("Cannot open midi device %d: %s", midi_device_i, mo->m_error);
		WriteLog(5, est);
	}
	CString est;
	est.Format("Open MIDI: device %d", midi_device_i);
	WriteLog(4, est);
}

void CGMidi::LogInstruments() {
	// Show instruments
	CString est;
	CString st, st2;
	/*
	int v_cnt2;
	// Get maximum voice mapped
	for (int i = MAX_VOICE - 1; i >= 0; --i) {
		if (instr[i] < InstGName.size() - 1) {
			v_cnt2 = i + 1;
			break;
		}
	}
	*/
	st2 = "Voice to instrument mapping: ";
	for (int i = 0; i < v_cnt; i++) {
		st.Format("%d ", instr[i]);
		st2 += st;
	}
	st2 += ". Instrument channels: ";
	for (int i = 0; i < InstGName.size(); i++) {
		st.Format("%d ", instr_channel[i]);
		st2 += st;
	}
	est.Format("%s", st2);
	WriteLog(4, est);
}

void CGMidi::AddMidiEvent(PmTimestamp timestamp, int mm_type, int data1, int data2)
{
	PmTimestamp real_timestamp = timestamp + midi_start_time;
	// Check if event is in future
	if (real_timestamp >= midi_sent_t) {
		PmEvent event;
		event.timestamp = real_timestamp;
		event.message = Pm_Message(mm_type, data1, data2);
		// If it is not the last SendMIDI, postpone future events
		if ((!midi_last_run) && (real_timestamp > midi_buf_lim)) {
			midi_buf_next.push_back(event);
			// Save maximum message and its time
			if (real_timestamp > midi_sent_t3) {
				midi_sent_t3 = real_timestamp;
				midi_sent_msg3 = event.message;
			}
			if (debug_level > 1) {
				CString est;
				est.Format("Postponed AddMidiEvent to %d step %d, type %02X, data %d/%d (after %d step %d, type %02X, data %d/%d) [start = %d, lim = %d]",
					timestamp, midi_current_step, mm_type, data1, data2, midi_sent_t - midi_start_time, midi_sent,
					Pm_MessageStatus(midi_sent_msg), Pm_MessageData1(midi_sent_msg), Pm_MessageData2(midi_sent_msg), midi_start_time, midi_buf_lim - midi_start_time);
				WriteLog(4, est);
			}
		}
		else {
			midi_buf.push_back(event);
			// Save maximum message and its time
			if (real_timestamp > midi_sent_t2) {
				midi_sent_t2 = real_timestamp;
				midi_sent_msg2 = event.message;
			}
		}
	}
	else {
		CString est;
		est.Format("Blocked AddMidiEvent to past %d step %d, type %02X, data %d/%d (before %d step %d, type %02X, data %d/%d) [start = %d]",
			timestamp, midi_current_step, mm_type, data1, data2, midi_sent_t - midi_start_time, midi_sent,
			Pm_MessageStatus(midi_sent_msg), Pm_MessageData1(midi_sent_msg), Pm_MessageData2(midi_sent_msg), midi_start_time); // , midi_buf_lim - midi_start_time
		WriteLog(5, est);
	}
	// Debug log
	//CString st;
	//st.Format("%d: At %d type %d, data %d/%d blocked %d\n", CGLib::time(), timestamp, mm_type, data1, data2, midi_sent_t-midi_start_time);
	//AppendLineToFile("midi.log", st);
}

void CGMidi::AddTransitionKs(int i, PmTimestamp stimestamp, int ks)
{
	int v = midi_voice;
	int pi = i - poff[i][v];
	int ei = i + len[i][v] - 1;
	AddKsOn(stimestamp - min(MAX_TRANS_DELAY,
		((stime[i] - stime[pi]) * 100 / m_pspeed + dstime[i][v] - dstime[pi][v]) / 10), ks, 10);
	AddKsOff(stimestamp + min(MAX_TRANS_DELAY,
		((etime[ei] - stime[i]) * 100 / m_pspeed + detime[ei][v] - dstime[i][v]) / 10), ks, 0);
}

void CGMidi::AddKs(PmTimestamp stimestamp, int ks)
{
	AddKsOn(stimestamp, ks, 100);
	AddKsOff(stimestamp + 1, ks, 0);
}

void CGMidi::AddTransitionCC(int i, PmTimestamp stimestamp, int CC, int value1, int value2)
{
	int v = midi_voice;
	int pi = i - poff[i][v];
	int ei = i + len[i][v] - 1;
	AddCC(stimestamp - min(MAX_TRANS_DELAY, 
		((stime[i] - stime[pi]) * 100 / m_pspeed + dstime[i][v] - dstime[pi][v]) / 10), CC, value1);
	AddCC(stimestamp + min(MAX_TRANS_DELAY, 
		((etime[ei] - stime[i]) * 100 / m_pspeed + detime[ei][v] - dstime[i][v]) / 10), CC, value2);
}

// Check that dstime is not too low
void CGMidi::CheckDstime(int i, int v)
{
	if (dstime[i][v] - MAX_TRANS_DELAY < -MAX_AHEAD && warning_ahead > MAX_WARN_MIDI_AHEAD) {
		CString st;
		st.Format("Warning: step %d, voice %d has dstime %.0f, while MAX_AHEAD=%d, MAX_TRANS_DELAY=%d. Risk of event blocking (can be not seen in logs)! Probably too long legato_ahead or random_start. Or you have to increase MAX_AHEAD.",
			i, v, dstime[i][v], MAX_AHEAD, MAX_TRANS_DELAY);
		WriteLog(5, st);
		++warning_ahead;
	}
}

void CGMidi::SendMIDI(int step1, int step2)
{
	if (step2 == step1) return;
	PmTimestamp time_start = CGLib::time();
	PmTimestamp timestamp_current = CGLib::time();
	// Note start timestamp
	PmTimestamp stimestamp; 
	// Note end timestamp
	PmTimestamp etimestamp; 
	// Check if this is first run
	if ((step1 == 0) || (!midi_sent_t) || (!midi_start_time)) midi_first_run = 1;
	else midi_first_run = 0;
	if (midi_first_run) LogInstruments();
	// Set real time when playback started
	if (!midi_start_time) midi_start_time = timestamp_current + MIDI_BUF_PROTECT - stime[step1] / m_pspeed * 100;
	// Set real time when playback started
	if (!midi_sent_t) midi_sent_t = stime[step1] / m_pspeed * 100 + midi_start_time - 100;
	// Check if we have buf underrun
	if (midi_sent_t < timestamp_current) {
		CString st;
		st.Format("SendMIDI got buf underrun in %d ms (steps %d - %d)", timestamp_current - midi_sent_t, step1, step2);
		WriteLog(5, st);
		buf_underrun = 1;
		return;
	}
	// Check if buf is full
	if (midi_sent_t - timestamp_current > MIN_MIDI_BUF_MSEC) {
		if (debug_level > 1) {
			CString st;
			st.Format("SendMIDI: no need to send (full buf = %d ms) (steps %d - %d) playback is at %d",
				midi_sent_t - timestamp_current, step1, step2, timestamp_current - midi_start_time);
			WriteLog(4, st);
		}
		return;
	}
	if (debug_level > 1) {
		CString est;
		est.Format("SendMIDI: need to send (full buf = %d ms) (steps %d - %d) playback is at %d",
			midi_sent_t - timestamp_current, step1, step2, timestamp_current - midi_start_time);
		WriteLog(4, est);
	}
	int i;
	if (!mutex_output.try_lock_for(chrono::milliseconds(3000))) {
		WriteLog(0, "SendMIDI mutex timed out: will try later");
	}
	int step21 = 0; // Voice-dependent first step
	int step22 = 0; // Voice-independent last step
	float time = 0;
	// Find last step not too far
	for (i = step1; i <= step2; i++) {
		step22 = i;
		if (i == 0) time = stime[i] * 100 / m_pspeed;
		else time = etime[i - 1] * 100 / m_pspeed;
		if (time + midi_start_time - timestamp_current > MAX_MIDI_BUF_MSEC) break;
	}
	// If we cut notes, this is not last run
	if (step22 < step2) midi_last_run = 0;
	// Send previous buffer if exists
	midi_buf.clear();
	// Calculate midi right limit
	midi_buf_lim = midi_start_time + stime[step22] * 100.0 / m_pspeed;
	// Decrease right limit to allow for legato ahead, random start and ks/cc transitions
	if (!midi_last_run) midi_buf_lim -= MAX_AHEAD;
	// Sort by timestamp before sending
	qsort(midi_buf_next.data(), midi_buf_next.size(), sizeof(PmEvent), PmEvent_comparator);
	if (midi_buf_next.size() > 0) {
		vector <PmEvent> mbn = midi_buf_next; 
		midi_buf_next.clear();
		// Set step to zero, because we do not know real steps of postponed notes
		midi_current_step = 0;
		for (int i = 0; i < mbn.size(); ++i) {
			AddMidiEvent(mbn[i].timestamp - midi_start_time, Pm_MessageStatus(mbn[i].message),
				Pm_MessageData1(mbn[i].message), Pm_MessageData2(mbn[i].message));
		}
		if (debug_level > 1) {
			CString est;
			est.Format("Postponed events sent (%d) - now postponed %d",
				mbn.size(), midi_buf_next.size());
			WriteLog(4, est);
		}
	}
	for (int v = 0; v < v_cnt; v++) {
		// Initialize voice
		int ei;
		int ncount = 0;
		int ii = instr[v];
		midi_channel = instr_channel[ii];
		midi_voice = v;
		// Move to note start
		if (coff[step1][v] > 0) {
			if (midi_first_run) step21 = step1 + noff[step1][v];
			else step21 = step1 - coff[step1][v];
		}
		else step21 = step1;
		// Count notes
		for (i = step21; i < step22; i++) {
			ncount++;
			if (i + len[i][v] > step22) break;
			// Set new buffer limit to beginning of last note
			if (noff[i][v] == 0) break;
			i += noff[i][v] - 1;
		}
		// Send notes
		i = step21;
		for (int x = 0; x < ncount; x++) {
			midi_current_step = i;
			ei = max(0, i + len[i][v] - 1);
			if (!pause[i][v]) {
				// Note ON if it is not blocked and was not yet sent
				stimestamp = stime[i] * 100 / m_pspeed + dstime[i][v];
				CheckDstime(i, v);
				if ((stimestamp + midi_start_time >= midi_sent_t) && (i >= midi_sent)) {
					AddNoteOn(stimestamp, note[i][v] + play_transpose[v], vel[i][v]);
					// Send slur
					if (artic[i][v] == ARTIC_SLUR) {
						AddTransitionKs(i, stimestamp, slur_ks[ii]);
					}
					// Send transition ks
					if ((instr_type[ii] == 2) && (artic[i][v] == ARTIC_SPLITPO_CHROM)) {
						AddTransitionKs(i, stimestamp, ks1[ii] + 12);
						AddTransitionKs(i, stimestamp, ks1[ii] + 0);
					}
					if ((instr_type[ii] == 2) && (artic[i][v] == ARTIC_SPLITPO_MIX)) {
						AddTransitionKs(i, stimestamp, ks1[ii] + 12);
						AddTransitionKs(i, stimestamp, ks1[ii] + 1);
					}
					if ((instr_type[ii] == 2) && (artic[i][v] == ARTIC_SPLITPO_ARAB)) {
						AddTransitionKs(i, stimestamp, ks1[ii] + 12);
						AddTransitionKs(i, stimestamp, ks1[ii] + 3);
					}
					if ((instr_type[ii] == 2) && (artic[i][v] == ARTIC_SPLITPO_PENT)) {
						AddTransitionKs(i, stimestamp, ks1[ii] + 12);
						AddTransitionKs(i, stimestamp, ks1[ii] + 4);
					}
					if ((instr_type[ii] == 2) && (artic[i][v] == ARTIC_GLISS)) {
						AddTransitionKs(i, stimestamp, ks1[ii] + 12);
						AddTransitionKs(i, stimestamp, ks1[ii] + 2);
					}
					if ((instr_type[ii] == 2) && (artic[i][v] == ARTIC_GLISS2)) {
						AddTransitionKs(i, stimestamp, ks1[ii] + 12);
						AddTransitionKs(i, stimestamp, ks1[ii] + 5);
					}
					// Send rebow retrigger
					if ((instr_type[ii] == 1) && (artic[i][v] == ARTIC_REBOW)) {
						AddTransitionCC(i, stimestamp, CC_retrigger[ii], 100, 0);
					}
				}
				// Note OFF if it is in window
				if (ei <= step22) {
					// Note OFF
					// ndur = (etime[ei] - stime[i]) * 100 / m_pspeed + detime[ei][v] - dstime[i][v];
					etimestamp = etime[ei] * 100 / m_pspeed + detime[ei][v];
					AddNoteOff(etimestamp, note[ei][v] + play_transpose[v], 0);
					// Send note ending ks
					if ((instr_type[ii] == 2) && (artic[ei][v] == ARTIC_END_SFL)) {
						AddKs(etimestamp - end_sfl_dur[ii], ks1[ii] + 11);
					}
					if ((instr_type[ii] == 2) && (artic[ei][v] == ARTIC_END_PBD)) {
						AddKs(etimestamp - end_pbd_dur[ii], ks1[ii] + 4);
					}
					if ((instr_type[ii] == 2) && (artic[ei][v] == ARTIC_END_VIB2)) {
						AddKs(etimestamp - end_vib2_dur[ii], ks1[ii] + 6);
					}
					if ((instr_type[ii] == 2) && (artic[ei][v] == ARTIC_END_VIB)) {
						AddKs(etimestamp - end_vib_dur[ii], ks1[ii] + 5);
					}
				}
			}
			// Go to next note
			if (noff[i][v] == 0) break;
			i += noff[i][v];
		}
		// Send CC
		InterpolateCC(CC_dyn[ii], rnd_dyn[ii], step1, step22, dyn, ii, v);
		InterpolateCC(CC_vib[ii], rnd_vib[ii], step1, step22, vib, ii, v);
		InterpolateCC(CC_vibf[ii], rnd_vibf[ii], step1, step22, vibf, ii, v);
	}
	// Sort by timestamp before sending
	qsort(midi_buf.data(), midi_buf.size(), sizeof(PmEvent), PmEvent_comparator);
	// Send
	for (int i = 0; i < midi_buf.size(); i++) {
		mo->QueueEvent(midi_buf[i]);
	}
	// Count time
	PmTimestamp time_stop = CGLib::time();
	CString st;
	st.Format("MIDI write %d (%d postponed) events: steps %d/%d - %d/%d (%d to %d ms) [to future %d to %d ms] (in %d ms) playback is at %d ms. Limit %d. Last postponed %d. Step22 stopped increasing at %.0f ms. Start time: %d, current time: %d",
		midi_buf.size(), midi_buf_next.size(), step21, step1, step22, step2, 
		midi_sent_t-midi_start_time, midi_sent_t2 - midi_start_time, 
		midi_sent_t - timestamp_current, midi_sent_t2 - timestamp_current,
		time_stop - time_start, timestamp_current - midi_start_time, midi_buf_lim - midi_start_time, 
		midi_sent_t3 - midi_start_time,	time, midi_start_time, timestamp_current);
	WriteLog(4, st);
	// Save last sent position
	midi_sent = step22;
	midi_sent_t = midi_sent_t2;
	midi_sent_msg = midi_sent_msg2;
	mutex_output.unlock();
}

// First cc sent by this function is with i = step1 - 2, time = stime[i + 1] = stime[step1-1]
// Last cc sent by this function is with i = step2 - 2, time = etime[i] = etime[step2-2] = stime[step2-1]
void CGMidi::InterpolateCC(int CC, float rnd, int step1, int step2, vector< vector <unsigned char> > & dv, int ii, int v)
{
	//CString st;
	//st.Format("Send CC%d from %d to %d", CC, step1, step2);
	//WriteLog(4, st);
	if (!CC) return;
	CSmoothRandom sr;
	int steps;
	float fsteps;
	// Linear interpolation
	vector <float> cc_lin;
	// Moving average
	vector <float> cc_ma;
	// Time of cc step
	vector <float> cc_time;
	// Time of last cc sent here
	float last_time = stime[step2 - 1];
	int first_cc = 0;
	int last_cc = 0;
	// Find step that will give enough information for ma junction
	int pre_cc = 0;
	int first_step = step1 - 2;
	for (int i = step1 - 3; i >= 0; --i) {
		// Get CC steps count
		fsteps = (float)CC_steps[ii] / 1000.0 * (etime[i] - stime[i]);
		steps = max(1, fsteps);
		pre_cc += steps;
		if (pre_cc > CC_ma[ii]) {
			first_step = i;
			break;
		}
	}
	for (int i = first_step; i < step2; i++) {
		if (i < 0) continue;
		midi_current_step = i;
		// Get CC steps count
		fsteps = (float)CC_steps[ii] / 1000.0 * (etime[i] - stime[i]);
		// Check if need to skip note steps
		//skip = 1.0 / max(0.0000001, fsteps);
		//if (skip > 1 && i % skip && coff[i][v] && noff[i][v] != 1 && i != step1 - 2 && i != step2 - 2) continue;
		steps = max(1, fsteps);
		if (steps % 2 == 0) steps++;
		// Calculate first and last ma positions to send
		if (i == step1 - 1) first_cc = cc_lin.size();
		if (i == step2 - 1) last_cc = cc_lin.size() - 1;
		// Linear interpolation
		for (int c = 0; c < steps; c++) {
			cc_time.push_back(stime[i] * 100 / m_pspeed + (etime[i] - stime[i]) * 100 / m_pspeed*(float)c / (float)steps);
			// Left cc steps
			if (c < steps / 2) {
				if (i == 0) cc_lin.push_back(dv[i][v]);
				else cc_lin.push_back((floor(steps * 0.5 - c) * dv[i - 1][v] + floor(c + 1 + steps / 2) * dv[i][v]) / steps);
			}
			// Right cc steps
			else {
				cc_lin.push_back((floor(steps * 1.5 - c) * dv[i][v] + floor(c - steps / 2) * dv[i + 1][v]) / steps);
			}
		} // for c
	} // for i
	// Detect abrupt changes and additionally smoothen them
	for (int c = 2; c < cc_lin.size(); ++c) {
		// Wait until change is abrupt
		if (abs(cc_lin[c] - cc_lin[c - 1]) < 20) continue;
		int left = c;
		int left0 = max(0, c - 10);
		// Find leftmost unchanged value
		for (int i = c-2; i >= left0; --i) {
			if (cc_lin[c - 1] != cc_lin[i]) break;
			left = i;
		}
		// Exit if value is unstable
		if (left > c-1) continue;
		// Interpolate
		for (int i = left; i < c; ++i) {
			cc_lin[i] = cc_lin[left] + (cc_lin[c] - cc_lin[left]) * (float)(i - left) / (c - left);
		}
	}
	cc_ma.resize(cc_lin.size());
	int CC_ma2 = CC_ma[ii] / 2;
	// Move cc sending ma window to the left
	first_cc = max(0, first_cc - CC_ma2 - 1);
	last_cc = max(0, last_cc - CC_ma2 - 1);
	// Set border ma
	cc_ma[0] = cc_lin[0];
	cc_ma[cc_lin.size()-1] = cc_lin[cc_lin.size() - 1];
	// First moving averages
	for (int c = 1; c <= CC_ma2; c++) {
		int lsteps = c * 2 + 1;
		cc_ma[c] = 0;
		for (int i = 0; i < lsteps; ++i) {
			cc_ma[c] += cc_lin[i] / (float)lsteps;
		}
	}
	// Extend moving average
	for (int c = CC_ma2 + 1; c < cc_lin.size() - CC_ma2 - 1; ++c) {
		cc_ma[c] = cc_ma[c - 1] + (cc_lin[c + CC_ma2] - cc_lin[c  - CC_ma2 - 1]) / (float)CC_ma[ii];
	}
	// Last moving averages
	cc_ma[0] = cc_lin[0];
	for (int c = cc_lin.size() - CC_ma2 - 1; c < cc_lin.size() - 1; c++) {
		int lsteps = (cc_lin.size() - 1 - c) * 2 + 1;
		cc_ma[c] = 0;
		for (int i = cc_lin.size() - lsteps; i < cc_lin.size(); ++i) {
			cc_ma[c] += cc_lin[i] / (float)lsteps;
		}
	}
	// Randomize from first_cc
	for (int c = first_cc; c < cc_lin.size(); ++c) {
		float t = cc_time[c];
		// Calculate fadeout
		float fadeout = 1;
		if (last_time - CC_FADEOUT_RESERVE - t < CC_FADEOUT) fadeout = max(0, last_time - CC_FADEOUT_RESERVE - t) / CC_FADEOUT;
		// Add random
		sr.MakeNext();
		cc_ma[c] += sr.sig / sr.s_range * (float)rnd * (float)cc_ma[c] / 200.0 * fadeout;
		// Check limits
		if (cc_ma[c] < 1) cc_ma[c] = 1;
		if (cc_ma[c] > 127) cc_ma[c] = 127;
	}
	// Send starting CC
	if (step1 == 0) AddCC(-1, CC, cc_ma[0]);
	// Send ma CC
	for (int c = first_cc; c <= last_cc; c++) {
		float t = cc_time[c];
		if (t >= midi_sent_t - midi_start_time) {
			AddCC(t, CC, cc_ma[c]);
		}
	}
}

void CGMidi::StopMIDI()
{
	WriteLog(4, "Stop MIDI");
	if (mo) delete mo;
	mo = 0;
}

int CGMidi::GetPlayStep() {
	if (buf_underrun == 1) {
		midi_play_step = 0;
	}
	else {
		// Don't need lock, because this function is called from OnDraw, which already has lock
		int step1 = midi_play_step;
		int step2 = midi_sent;
		int cur_step = 0, currentElement;
		PmTimestamp searchElement = CGLib::time() - midi_start_time;
		while (step1 <= step2) {
			cur_step = (step1 + step2) / 2;
			currentElement = stime[cur_step] * 100 / m_pspeed;
			if (currentElement < searchElement) {
				step1 = cur_step + 1;
			}
			else if (currentElement > searchElement) {
				step2 = cur_step - 1;
			}
			else {
				break;
			}
		}
		midi_play_step = cur_step;
		//mutex_output.unlock();
	}
	return midi_play_step;
}

void CGMidi::AddNoteOn(PmTimestamp timestamp, int data1, int data2)
{
	// Check if range valid
	if ((data1 < instr_nmin[instr[midi_voice]]) || (data1 > instr_nmax[instr[midi_voice]])) {
		if (warning_note_wrong[midi_voice] < 4) {
			CString st;
			st.Format("Blocked note %d/%d time %d in voice %d instrument %d out of range %d-%d",
				data1, data2, timestamp, midi_voice, instr[midi_voice], instr_nmin[instr[midi_voice]], instr_nmax[instr[midi_voice]]);
			WriteLog(5, st);
			warning_note_wrong[midi_voice] ++;
		}
		return;
	}
	AddMidiEvent(timestamp, MIDI_NOTEON + midi_channel, data1, data2);
}

void CGMidi::AddKsOn(PmTimestamp timestamp, int data1, int data2)
{
	// Check if range valid
	if ((data1 >= instr_nmin[instr[midi_voice]]) && (data1 <= instr_nmax[instr[midi_voice]])) {
		if (warning_note_wrong[midi_voice] < 4) {
			CString st;
			st.Format("Blocked keyswitch %d/%d time %d in voice %d instrument %d in note range %d-%d",
				data1, data2, timestamp, midi_voice, instr[midi_voice], instr_nmin[instr[midi_voice]], instr_nmax[instr[midi_voice]]);
			WriteLog(5, st);
			warning_note_wrong[midi_voice] ++;
		}
		return;
	}
	AddMidiEvent(timestamp, MIDI_NOTEON + midi_channel, data1, data2);
}

void CGMidi::AddNoteOff(PmTimestamp timestamp, int data1, int data2)
{
	// Check if range valid
	if ((data1 < instr_nmin[instr[midi_voice]]) || (data1 > instr_nmax[instr[midi_voice]])) {
		if (warning_note_wrong[midi_voice] < 4) {
			CString st;
			st.Format("Blocked note %d/%d time %d in voice %d instrument %d out of range %d-%d",
				data1, data2, timestamp, midi_voice, instr[midi_voice], instr_nmin[instr[midi_voice]], instr_nmax[instr[midi_voice]]);
			WriteLog(5, st);
			warning_note_wrong[midi_voice] ++;
		}
		return;
	}
	AddMidiEvent(timestamp, MIDI_NOTEOFF + midi_channel, data1, data2);
}

void CGMidi::AddKsOff(PmTimestamp timestamp, int data1, int data2)
{
	// Check if range valid
	if ((data1 >= instr_nmin[instr[midi_voice]]) && (data1 <= instr_nmax[instr[midi_voice]])) {
		if (warning_note_wrong[midi_voice] < 4) {
			CString st;
			st.Format("Blocked keyswitch %d/%d time %d in voice %d instrument %d in note range %d-%d",
				data1, data2, timestamp, midi_voice, instr[midi_voice], instr_nmin[instr[midi_voice]], instr_nmax[instr[midi_voice]]);
			WriteLog(5, st);
			warning_note_wrong[midi_voice] ++;
		}
		return;
	}
	AddMidiEvent(timestamp, MIDI_NOTEOFF + midi_channel, data1, data2);
}

void CGMidi::AddCC(PmTimestamp timestamp, int data1, int data2)
{
	AddMidiEvent(timestamp, MIDI_CC + midi_channel, data1, data2);
}

int CGMidi::PmEvent_comparator(const void * v1, const void * v2)
{
	const PmEvent *p1 = (PmEvent *)v1;
	const PmEvent *p2 = (PmEvent *)v2;
	//return (p2->timestamp - p1->timestamp);
	if (p1->timestamp < p2->timestamp)
		return -1;
	else if (p1->timestamp > p2->timestamp)
		return +1;
	else
		return 0;
}

void CGMidi::LoadWav(CString fname)
{

}
