#include "stdafx.h"
#include "GMidi.h"
#include "SmRnd.h"

#include "midifile/MidiFile.h"

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
	int tpñ = 60 * midifile_out_mul; // ticks per croche
	midifile.setTicksPerQuarterNote(tpq);
	int track = 0;
	int channel = 0;
	// Add some expression track (track 0) messages:
	string st = fname;
	midifile.addTrackName(track, 0, st);
	// Save tempo
	midifile.addTempo(track, 0, tempo[0]);
	for (int i = 0; i < t_generated; i++) {
		midifile.addTempo(track, (tpq * 4) + tpñ * i, tempo[i]);
	}
	// Save notes
	for (int v = 0; v < v_cnt; v++) {
		track = v + 1;
		channel = v;
		string st = InstGName[instr[v]];
		// Replace piano with other instrument, because otherways it generates two-stave track in Sibelius
		if (st == "Piano") st = "Vibraphone";
		midifile.addTrackName(track, 0, st);
		midifile.addPatchChange(track, 0, channel, 0); // 40=violin
		for (int i = 0; i < t_generated; i++) if (pause[i][v] == 0) {
			midifile.addNoteOn(track, (tpq * 4) + tpñ*i, channel, note[i][v], dyn[i][v]);
			midifile.addNoteOff(track, (tpq * 4) + tpñ*(i + len[i][v]) - 1, channel, note[i][v], 0);
			if (comment[i][v] != "") {
				string st;
				st = comment[i][v];
				midifile.addLyric(track, (tpq * 4) + tpñ*i, st);
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
	milliseconds time_start = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
	if (!fileExists(path)) {
		CString* est = new CString;
		est->Format("Cannot find file %s", path);
		WriteLog(1, est);
		return;
	}
	MidiFile midifile;
	if (!midifile.read(path)) {
		CString* est = new CString;
		est->Format("Error reading midi file %s", path);
		WriteLog(1, est);
		return;
	}
	//midifile.joinTracks();
	midifile.linkNotePairs();
	midifile.doTimeAnalysis();

	if (midifile.getTrackCount() < 2) {
		CString* st = new CString;
		st->Format("Detected only %d tracks while loading file %s. Probably MIDI type 0. Splitting midi tracks by channels. Track names are not supported for MIDI type 0 yet.", midifile.getTrackCount(), path);
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
	float lastNoteFinished = 0.0;
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
	UpdateTempoMinMax(0, last_step - 1);
	last_step = 0;
	int v1 = 0;
	int v2 = 0;
	int v = 0;
	int iname_id = 1;

	for (int track = 1; track < midifile.getTrackCount(); track++) {
		if (track > 1) {
			// Get next free voice
			v1 = v2 + 1;
			// Voice interval = 1
			v2 = v1;
			// Current voice is first voice in interval
			v = v1;
			if (v >= MAX_VOICE) {
				CString* st = new CString;
				st->Format("Too many voices need to be created for loading file %s. Maximum number of voices %d. Increase MAX_VOICE", path, MAX_VOICE);
				WriteLog(1, st);
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
					track_name[v] = "";
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
						CString* st = new CString;
						st->Format("Note too long and will be cut short at %d track %d tick with %d tpc (mul %.03f) approximated to %d step in file %s. Decrease midifile_in_mul can resolve this situation.", track, mev->tick, tpc, midifile_in_mul, pos, path);
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
					// Set last step that has tempo
					last_step_tempo = pos + nlen - 1;
				}
				// Fallback
				if (!tempo[pos]) tempo[pos] = 100;
				int delta = (float)(mev->tick - pos*tpc) / (float)tpc * 30000.0 / (float)tempo[pos];
				// Check alignment
				if (abs(delta) > MAX_ALLOW_DELTA && (warning_loadmidi_align < MAX_WARN_MIDI_ALIGN)) {
					CString* st = new CString;
					st->Format("Note moved %d ms to fit step grid at %d track, %d tick with %d tpc (mul %.03f) approximated to %d step (deviation %d ticks) in file %s. Increasing midifile_in_mul will improve approximation.", 
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
									CString* st = new CString;
									st->Format("Note %s too short and gets same step with next note %s at %d track, %d tick with %d tpc (mul %.03f) approximated to %d step in file %s. Increasing midifile_in_mul will improve approximation.", GetNoteName(note[pos][x]), GetNoteName(pitch), track, mev->tick, tpc, midifile_in_mul, pos, path);
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
							CString* st = new CString;
							st->Format("Too many voices need to be created for loading file %s. Maximum number of voices %d. Increase MAX_VOICE", path, MAX_VOICE);
							WriteLog(1, st);
							break;
						}
						track_id[v] = track;
						track_vid[v] = v - v1;
						track_name[v] = track_name[v1];
					}
				} // if (instr_poly[instr[v]] > 1)
				else {
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
					for (int z = last_pause; z < pos; z++) {
						len[z][v] = 1;
						dyn[z][v] = 0;
						pause[z][v] = 1;
						coff[z][v] = 0;
						if (tempo[z] == 0) tempo[z] = tempo[z - 1];
					}
					// Set additional variables
					CountOff(last_pause, pos - 1);
					CountTime(last_pause, pos - 1);
				}
				// Set note steps
				for (int z = 0; z < nlen; z++) {
					note[pos + z][v] = pitch;
					len[pos + z][v] = nlen;
					dyn[pos + z][v] = myvel;
					midi_ch[pos + z][v] = chan;
					pause[pos + z][v] = 0;
					coff[pos + z][v] = z;
					if (tempo[pos + z] == 0) tempo[pos + z] = tempo[pos + z - 1];
				}
				// Set midi delta only to first step of note, because in in-note steps you can get different calculations for different tempo
				midi_delta[pos][v] = delta;
				// Set additional variables
				CountOff(pos, pos + nlen - 1);
				CountTime(pos, pos + nlen - 1);
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
			len[0][v] = 1;
			pause[0][v] = 1;
		}
	} // for track
	if (need_exit) return;
	// Add closing pauses
	for (int v = 0; v < v_cnt; v++) {
		if (vlast_step[v] < last_step) {
			int len2 = last_step - vlast_step[v];
			for (int i = 1; i <= len2; i++) {
				pause[vlast_step[v] + i][v] = 1;
				len[vlast_step[v] + i][v] = 1;
				coff[vlast_step[v] + i][v] = 0;
			}
		}
	}
	// Set additional variables
	FixLen(0, last_step);
	CountOff(0, last_step);
	CountTime(0, last_step);
	UpdateNoteMinMax(0, last_step);
	UpdateTempoMinMax(0, last_step);
	// Send last
	t_generated = last_step + 1;
	if (tnames != "") {
		CString* est = new CString;
		est->Format("MIDI file track names: %s", tnames);
		WriteLog(0, est);
	}
	if (inames != "") {
		CString* est = new CString;
		est->Format("MIDI file instrument names: %s", inames);
		WriteLog(0, est);
	}
	// Count time
	milliseconds time_stop = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
	CString* est = new CString;
	est->Format("LoadMidi successfully loaded %d steps (in %d ms)", t_generated, time_stop - time_start);
	WriteLog(0, est);
}

void CGMidi::LoadCantus(CString path)
{
	milliseconds time_start = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
	if (!fileExists(path)) {
		CString* est = new CString;
		est->Format("Cannot find file %s", path);
		WriteLog(1, est);
		return;
	}
	MidiFile midifile;
	if (!midifile.read(path)) {
		CString* est = new CString;
		est->Format("Error reading midi file %s", path);
		WriteLog(1, est);
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
	vector <int> c;
	vector <int> cl;
	vector <float> ct;
	int bad = 0;
	for (int track = 0; track < midifile.getTrackCount(); track++) {
		float last_tick = 0;
		for (int i = 0; i<midifile[track].size(); i++) {
			MidiEvent* mev = &midifile[track][i];
			float time = midifile.getTimeInSeconds(mev->tick);
			if (mev->isNoteOn()) {
				float pos2 = mev->tick;
				int pos = round(mev->tick / (float)tpc);
				float nlen2 = mev->getTickDuration();
				int nlen = round((mev->tick + mev->getTickDuration()) / (float)tpc) - pos;
				// Check for pause
				if (pos2 - last_tick > tpc / 2) {
					// Add cantus if it is long
					if (nid > 5 && !bad) {
						cantus.push_back(c);
						cantus_len.push_back(cl);
						cantus_tempo.push_back(ct);
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
				}
				// Add new note
				if ((nid == 0) || (c[nid-1] != mev->getKeyNumber())) {
					// Check if current note already set
					if (!nlen) {
						if (warning_loadmidi_short < MAX_WARN_MIDI_SHORT) {
							CString* st = new CString;
							st->Format("Note too short: tick %d, track %d, chan %d, tpc %d (mul %.03f) in file %s. Increasing midifile_in_mul will improve approximation.", mev->tick, track, mev->getChannel(), tpc, midifile_in_mul, path);
							WriteLog(1, st);
							warning_loadmidi_short++;
						}
						bad = 1;
					}
					int nlen = round((mev->tick + mev->getTickDuration()) / (float)tpc) - pos;
					// Check if note too long
					if (nlen > MAX_LEN) {
						if (warning_loadmidi_long < MAX_WARN_MIDI_LONG) {
							CString* st = new CString;
							st->Format("Note too long: tick %d, track %d, chan %d, tpc %d (mul %.03f) in file %s. Decreasing midifile_in_mul can help.", mev->tick, track, mev->getChannel(), tpc, midifile_in_mul, path);
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
						nid++;
					}
				}
				// Save last time
				last_tick = pos2 + nlen2;
			}
		}
		// Add cantus if it is long
		if (nid > 5 && !bad) {
			cantus.push_back(c);
			cantus_len.push_back(cl);
			cantus_tempo.push_back(ct);
			nid = 0;
		}
	}
	// Count time
	milliseconds time_stop = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
	CString* st = new CString;
	st->Format("LoadCantus successfully loaded %d canti (in %d ms)", cid+1, time_stop - time_start);
	WriteLog(0, st);
}

void CGMidi::StartMIDI(int midi_device_i, int from)
{
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
		midi_sent_t = 0; // TIME_PROC(TIME_INFO) + MIDI_BUF_PROTECT
	}
	else {
		midi_sent_t = 0;
		midi_sent = 0;
	}
	TIME_START;
	if (debug_level > 1) {
		CString* est = new CString;
		est->Format("Trying to open midi device %d...", midi_device_i);
		WriteLog(4, est);
	}
	mo = new CMidiOut;
	if (mo->StartMidi(midi_device_i)) {
		CString* est = new CString;
		est->Format("Cannot open midi device %d: %s", midi_device_i, mo->m_error);
		WriteLog(1, est);
	}
	CString* est = new CString;
	est->Format("Open MIDI: device %d", midi_device_i);
	WriteLog(4, est);
}

void CGMidi::LogInstruments() {
	// Show instruments
	CString* est = new CString;
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
	est->Format("%s", st2);
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
				CString* est = new CString;
				est->Format("Postponed AddMidiEvent to %d step %d, type %02X, data %d/%d (after %d step %d, type %02X, data %d/%d) [start = %d, lim = %d]",
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
		CString* est = new CString;
		est->Format("Blocked AddMidiEvent to past %d step %d, type %02X, data %d/%d (before %d step %d, type %02X, data %d/%d) [start = %d]",
			timestamp, midi_current_step, mm_type, data1, data2, midi_sent_t - midi_start_time, midi_sent,
			Pm_MessageStatus(midi_sent_msg), Pm_MessageData1(midi_sent_msg), Pm_MessageData2(midi_sent_msg), midi_start_time); // , midi_buf_lim - midi_start_time
		WriteLog(1, est);
	}
	// Debug log
	//CString st;
	//st.Format("%d: At %d type %d, data %d/%d blocked %d\n", TIME_PROC(TIME_INFO), timestamp, mm_type, data1, data2, midi_sent_t-midi_start_time);
	//AppendLineToFile("midi.log", st);
}

void CGMidi::AddTransitionKs(int i, int stimestamp, int ks)
{
	int v = midi_voice;
	int pi = i - poff[i][v];
	int ei = i + len[i][v] - 1;
	AddKsOn(stimestamp - min(MAX_TRANS_DELAY,
		((stime[i] - stime[pi]) * 100 / m_pspeed + dstime[i][v] - dstime[pi][v]) / 10), ks, 10);
	AddKsOff(stimestamp + min(MAX_TRANS_DELAY,
		((etime[ei] - stime[i]) * 100 / m_pspeed + detime[ei][v] - dstime[i][v]) / 10), ks, 0);
}

void CGMidi::AddKs(int stimestamp, int ks)
{
	AddKsOn(stimestamp, ks, 100);
	AddKsOff(stimestamp + 1, ks, 0);
}

void CGMidi::AddTransitionCC(int i, int stimestamp, int CC, int value1, int value2)
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
		CString* st = new CString;
		st->Format("Warning: step %d, voice %d has dstime %.0f, while MAX_AHEAD=%d, MAX_TRANS_DELAY=%d. Risk of event blocking (can be not seen in logs)! Probably too long legato_ahead or random_start. Or you have to increase MAX_AHEAD.",
			i, v, dstime[i][v], MAX_AHEAD, MAX_TRANS_DELAY);
		WriteLog(1, st);
		++warning_ahead;
	}
}

void CGMidi::SendMIDI(int step1, int step2)
{
	if (step2 == step1) return;
	milliseconds time_start = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
	PmTimestamp timestamp_current = TIME_PROC(TIME_INFO);
	PmTimestamp timestamp;
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
		CString* st = new CString;
		st->Format("SendMIDI got buf underrun in %d ms (steps %d - %d)", timestamp_current - midi_sent_t, step1, step2);
		WriteLog(1, st);
		buf_underrun = 1;
		return;
	}
	// Check if buf is full
	if (midi_sent_t - timestamp_current > MIN_MIDI_BUF_MSEC) {
		if (debug_level > 1) {
			CString* st = new CString;
			st->Format("SendMIDI: no need to send (full buf = %d ms) (steps %d - %d) playback is at %d",
				midi_sent_t - timestamp_current, step1, step2, timestamp_current - midi_start_time);
			WriteLog(4, st);
		}
		return;
	}
	if (debug_level > 1) {
		CString* est = new CString;
		est->Format("SendMIDI: need to send (full buf = %d ms) (steps %d - %d) playback is at %d",
			midi_sent_t - timestamp_current, step1, step2, timestamp_current - midi_start_time);
		WriteLog(4, est);
	}
	int i;
	if (!mutex_output.try_lock_for(chrono::milliseconds(3000))) {
		WriteLog(0, new CString("SendMIDI mutex timed out: will try later"));
	}
	int step21; // Voice-dependent first step
	int step22; // Voice-independent last step
	float time;
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
			CString* est = new CString;
			est->Format("Postponed events sent (%d) - now postponed %d",
				mbn.size(), midi_buf_next.size());
			WriteLog(4, est);
		}
	}
	for (int v = 0; v < v_cnt; v++) {
		// Initialize voice
		PmEvent event;
		int ei;
		int ndur;
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
		InterpolateCC(CC_dyn[ii], CC_dyn_ma[ii], step1, step22, dyn, ii, v);
		InterpolateCC(CC_vib[ii], 1, step1, step22, vib, ii, v);
		InterpolateCC(CC_vibf[ii], 1, step1, step22, vibf, ii, v);
	}
	// Sort by timestamp before sending
	qsort(midi_buf.data(), midi_buf.size(), sizeof(PmEvent), PmEvent_comparator);
	// Send
	for (int i = 0; i < midi_buf.size(); i++) {
		mo->QueueEvent(midi_buf[i]);
	}
	// Count time
	milliseconds time_stop = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
	CString* st = new CString;
	st->Format("MIDI write %d (%d postponed) events: steps %d/%d - %d/%d (%d to %d ms) [to future %d to %d ms] (in %d ms) playback is at %d ms. Limit %d. Last postponed %d. Step22 stopped increasing at %.0f ms. Start time: %d, current time: %d",
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

// First cc sent by this function is with i = step1 - 1, time = (stime[i] + etime[i]) / 2, which is half step to step1
// Last cc sent by this function is with i = step2 - 2, time = (stime[i+1] + etime[i+1]) / 2, which is half step to step2
void CGMidi::InterpolateCC(int CC, int ma, int step1, int step2, vector< vector <unsigned char> > & dv, int ii, int v)
{
	CString st;
	st.Format("Send CC%d from %d to %d", CC, step1, step2);
	WriteLog(4, st);
	if (!CC) return;
	CSmoothRandom sr;
	int cc_value;
	int steps, skip;
	float fsteps;
	// Time of last cc sent here
	float last_time = (stime[step2 - 1] + etime[step2 - 1]) / 2;
	float cc_step; // Length of cc interpolation step
	float cc_pos1; // Middle of current note step
	float cc_pos2; // Middle of next note step
	for (int i = step1 - 2; i < step2 - 1; i++) {
		if (i < 0) continue;
		midi_current_step = i;
		// Linear interpolation
		vector <float> cc_lin; 
		// Moving average
		vector <float> cc_ma; 
		// Get CC steps count
		fsteps = (float)CC_steps[ii] / 1000.0 * (etime[i] - stime[i]);
		// Check if need to skip note steps
		skip = 1.0 / max(0.0000001, fsteps);
		if (skip > 1 && i % skip && coff[i][v] && noff[i][v] != 1) continue;
		steps = max(1, fsteps);
		if (steps % 2 == 0) steps++;
		cc_lin.resize(steps * 2);
		cc_ma.resize(steps);
		// Linear interpolation
		for (int c = 0; c < steps * 2; c++) {
			// Left cc steps
			if (c < steps / 2) {
				if (i == 0) cc_lin[c] = dv[i][v];
				else cc_lin[c] = (floor(steps * 0.5 - c) * dv[i - 1][v] + floor(c + 1 + steps / 2) * dv[i][v]) / steps;
			}
			// Mid cc steps
			else if (c < steps * 1.5) {
				cc_lin[c] = (floor(steps * 1.5 - c) * dv[i][v] + floor(c - steps / 2) * dv[i + 1][v]) / steps;
			}
			// Right cc steps
			else {
				if (i == step2 - 2) cc_lin[c] = dv[i + 1][v];
				else cc_lin[c] = (floor(steps * 2.5 - c) * dv[i + 1][v] + floor(c - steps * 1.5 + 1) * dv[i + 2][v]) / steps;
			}
		}
		if (!ma) {
			// Send linear CC
			for (int c = 0; c < steps; c++) {
				AddCC(stime[i] * 100 / m_pspeed + (etime[i] - stime[i]) * 100 / m_pspeed*(float)c / (float)steps, CC, cc_lin[c]);
			}
		}
		else {
			// First moving average
			cc_ma[0] = 0;
			for (int c = 0; c < steps; c++) {
				cc_ma[0] += cc_lin[c] / (float)steps;
			}
			// Extend moving average
			for (int c = 1; c < steps; c++) {
				cc_ma[c] = cc_ma[c - 1] + (cc_lin[c + steps - 1] - cc_lin[c - 1]) / (float)steps;
			}
			// Send starting CC
			if (i == 0) AddCC(-1, CC, cc_ma[0]);
			int hstep = steps / 2;
			// Send ma CC of first note
			if (i > step1 - 2) for (int c = 0; c < hstep + 1; c++) {
				int t = stime[i] * 100 / m_pspeed + (etime[i] - stime[i]) * 100 / m_pspeed*(float)(c + hstep) / (float)steps;
				if (t >= midi_sent_t - midi_start_time) {
					// Calculate fadeout
					float fadeout = 1;
					if (last_time - CC_FADEOUT_RESERVE - t < CC_FADEOUT) fadeout = max(0, last_time - CC_FADEOUT_RESERVE - t) / CC_FADEOUT;
					// Add random
					sr.MakeNext();
					cc_ma[c] = 64;
					cc_ma[c] += sr.sig / sr.s_range * (float)rnd_dyn[ii] * 2.0 * fadeout;
					// Check limits
					if (cc_ma[c] < 1) cc_ma[c] = 1;
					if (cc_ma[c] > 127) cc_ma[c] = 127;
					// Send
					AddCC(t, CC, cc_ma[c]);
				}
			}
			// Send ma CC of second note
			if (i <  step2 - 2) for (int c = hstep + 1; c < steps; c++) {
				int t = stime[i + 1] * 100 / m_pspeed + (etime[i + 1] - stime[i + 1]) * 100 / m_pspeed*(float)(c - hstep - 1) / (float)steps;
				if (t >= midi_sent_t - midi_start_time) {
					// Calculate fadeout
					float fadeout = 1;
					if (last_time - CC_FADEOUT_RESERVE - t < CC_FADEOUT) fadeout = max(0, last_time - CC_FADEOUT_RESERVE - t) / CC_FADEOUT;
					// Add random
					sr.MakeNext();
					cc_ma[c] = 64;
					cc_ma[c] += sr.sig / sr.s_range * (float)rnd_dyn[ii] * 2.0 * fadeout;
					// Check limits
					if (cc_ma[c] < 1) cc_ma[c] = 1;
					if (cc_ma[c] > 127) cc_ma[c] = 127;
					// Send
					AddCC(t, CC, cc_ma[c]);
				}
			} // for c
		} // Moving average
	} // for i
}

void CGMidi::StopMIDI()
{
	WriteLog(4, new CString("Stop MIDI"));
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
		int cur_step, currentElement;
		int searchElement = TIME_PROC(TIME_INFO) - midi_start_time;
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
			CString* st = new CString;
			st->Format("Blocked note %d/%d time %d in voice %d instrument %d out of range %d-%d",
				data1, data2, timestamp, midi_voice, instr[midi_voice], instr_nmin[instr[midi_voice]], instr_nmax[instr[midi_voice]]);
			WriteLog(1, st);
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
			CString* st = new CString;
			st->Format("Blocked keyswitch %d/%d time %d in voice %d instrument %d in note range %d-%d",
				data1, data2, timestamp, midi_voice, instr[midi_voice], instr_nmin[instr[midi_voice]], instr_nmax[instr[midi_voice]]);
			WriteLog(1, st);
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
			CString* st = new CString;
			st->Format("Blocked note %d/%d time %d in voice %d instrument %d out of range %d-%d",
				data1, data2, timestamp, midi_voice, instr[midi_voice], instr_nmin[instr[midi_voice]], instr_nmax[instr[midi_voice]]);
			WriteLog(1, st);
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
			CString* st = new CString;
			st->Format("Blocked keyswitch %d/%d time %d in voice %d instrument %d in note range %d-%d",
				data1, data2, timestamp, midi_voice, instr[midi_voice], instr_nmin[instr[midi_voice]], instr_nmax[instr[midi_voice]]);
			WriteLog(1, st);
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