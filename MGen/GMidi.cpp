#include "stdafx.h"
#include "GMidi.h"

#include "midifile/MidiFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW 
#endif

CGMidi::CGMidi()
{
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
		string st = InstName[instr[v]];
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
		st->Format("Detected only %d tracks while loading file %s. Probably MIDI type 0. Splitting midi tracks by channels.", midifile.getTrackCount(), path);
		WriteLog(0, st);
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
	//vector <CString> track_name2(MAX_VOICE);

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
	for (int z = 1; z < last_step; z++) {
		if (tempo[z] == 0) tempo[z] = tempo[z - 1];
	}
	UpdateTempoMinMax(0, last_step - 1);
	last_step = 0;
	int v1 = 0;
	int v2 = 0;
	int v = 0;
	int iname_id = 1;

	for (int track = 0; track < midifile.getTrackCount(); track++) {
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
			// Load name if we have MIDI type 0
			/*
			if (!midifile_type) {
				track_name[v] = track_name2[track];
			}
			*/
		}
		// Save track id
		track_id[v] = track;
		track_vid[v] = 0;
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
				}
				/*
				if (mev->getMetaType() == 0x04) {
					track_name2[iname_id] = "";
					for (int x = 0; x < mev->size(); x++) {
						track_name2[iname_id] += mev->data()[x];
					}
					// Remove first data items
					track_name2[iname_id] = track_name2[iname_id].Mid(3);
					st.Format("%d", iname_id);
					inames += " \n" + st + "=" + track_name2[iname_id];
					if (iname_id == 1) track_name[0] = track_name2[1];
					++iname_id;
				}
				*/
			}
			if (mev->isNoteOn()) {
				int pos = round(mev->tick / (float)tpc);
				int pitch = mev->getKeyNumber();
				int myvel = mev->getVelocity();
				// Fill tempo
				if (!tempo[pos]) for (int z = pos; z > 0; --z) {
					if (tempo[z] != 0) break;
					tempo[z] = tempo[z - 1];
				}
				if (!tempo[pos]) tempo[pos] = 100;
				int delta = (float)(mev->tick - pos*tpc) / (float)tpc * 30000.0 / (float)tempo[pos];
				// Check alignment
				if (abs(delta) > MAX_ALLOW_DELTA && (warning_loadmidi_align < MAX_WARN_MIDI_ALIGN)) {
					CString* st = new CString;
					st->Format("Note moved %d ms to fit step grid at %d track, %d tick with %d tpc (mul %.03f) approximated to %d step (deviation %d) in file %s. Increasing midifile_in_mul will improve approximation.", delta, track, mev->tick, tpc, midifile_in_mul, pos, mev->tick - pos*tpc, path);
					WriteLog(1, st);
					warning_loadmidi_align++;
				}
				// Find overlaps and distance
				for (int x = v1; x <= v2; ++x) {
					// Overlap happens only in case when real overlap time is greater than half of croche
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
				// Resize vectors for new voice number
				if (v > v_cnt - 1) ResizeVectors(t_allocated, v + 1);
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
				if (pos + nlen >= t_allocated) ResizeVectors(max(pos + nlen, t_allocated * 2));
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
				// Set midi delta only to first step of note, because later you can get different calculations for different tempo
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
	}
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
	CountOff(0, last_step);
	CountTime(0, last_step);
	//UpdateNoteMinMax(0, last_step);
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
							warning_loadmidi_align++;
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

void CGMidi::StartMIDI(int midi_device_i, int latency, int from)
{
	// Clear old sent messages
	midi_buf_next.clear();
	midi_sent_msg = 0;
	midi_sent_msg2 = 0;
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
	PmError pmerr = PmError();
	if (debug_level > 1) {
		CString* est = new CString;
		est->Format("Trying to open midi device %d with output buf size %d and latency %d...", midi_device_i, OUTPUT_BUF_SIZE, latency);
		WriteLog(4, est);
	}
	try {
		pmerr = Pm_OpenOutput(&midi, midi_device_i, NULL, OUTPUT_BUF_SIZE, TIME_PROC, NULL, latency);
	}
	catch (...) {
		CString* est = new CString;
		est->Format("Cannot open midi device %d with output buf size %d and latency %d", midi_device_i, OUTPUT_BUF_SIZE, latency);
		WriteLog(1, est);
	}
	if (pmerr) {
		CString* est = new CString;
		est->Format("Cannot open midi device %d with output buf size %d and latency %d", midi_device_i, OUTPUT_BUF_SIZE, latency);
		WriteLog(1, est);
	}
	CString* est = new CString;
	est->Format("Pm_OpenOutput: buf size %d, latency %d", OUTPUT_BUF_SIZE, latency);
	WriteLog(4, est);
	// Show instruments
	est = new CString;
	CString st, st2;
	st2 = "Voice to instrument mapping: ";
	for (int i = 0; i < MAX_VOICE; i++) {
		st.Format("%d ", instr[i]);
		st2 += st;
	}
	st2 += ". Instrument channels: ";
	for (int i = 0; i < MAX_INSTR; i++) {
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
			// Save maximum message
			if (real_timestamp > midi_sent_t2) midi_sent_t2 = real_timestamp;
			midi_sent_msg2 = event.message;
		}
	}
	else {
		CString* est = new CString;
		est->Format("Blocked AddMidiEvent to past %d step %d, type %02X, data %d/%d (before %d step %d, type %02X, data %d/%d) [start = %d]",
			timestamp, midi_current_step, mm_type, data1, data2, midi_sent_t - midi_start_time, midi_sent,
			Pm_MessageStatus(midi_sent_msg), Pm_MessageData1(midi_sent_msg), Pm_MessageData2(midi_sent_msg), midi_start_time, midi_buf_lim - midi_start_time);
		WriteLog(1, est);
	}
	// Debug log
	//CString st;
	//st.Format("%d: At %d type %d, data %d/%d\n", TIME_PROC(TIME_INFO), timestamp, mm_type, data1, data2);
	//AppendLineToFile("midi.log", st);
}

void CGMidi::AddTransitionKs(int i, int stimestamp, int ks)
{
	int v = midi_voice;
	int pi = i - poff[i][v];
	int ei = i + len[i][v] - 1;
	AddKsOn(stimestamp - ((stime[i] - stime[pi]) * 100 / m_pspeed + dstime[i][v] - dstime[pi][v]) / 10, ks, 10);
	AddKsOff(stimestamp + ((etime[ei] - stime[i]) * 100 / m_pspeed + detime[ei][v] - dstime[i][v]) / 10, ks, 0);
}

void CGMidi::AddTransitionCC(int i, int stimestamp, int CC, int value1, int value2)
{
	int v = midi_voice;
	int pi = i - poff[i][v];
	int ei = i + len[i][v] - 1;
	AddCC(stimestamp - ((stime[i] - stime[pi]) * 100 / m_pspeed + dstime[i][v] - dstime[pi][v]) / 10, CC, value1);
	AddCC(stimestamp + ((etime[ei] - stime[i]) * 100 / m_pspeed + detime[ei][v] - dstime[i][v]) / 10, CC, value2);
}

void CGMidi::SendMIDI(int step1, int step2)
{
	if (step2 == step1) return;
	milliseconds time_start = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
	PmTimestamp timestamp_current = TIME_PROC(TIME_INFO);
	PmTimestamp timestamp;
	PmTimestamp stimestamp; // Note start timestamp
	PmTimestamp etimestamp; // Note end timestamp
													// Check if this is first run
	if ((step1 == 0) || (!midi_sent_t) || (!midi_start_time)) midi_first_run = 1;
	else midi_first_run = 0;
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
		//CString* st = new CString;
		//st->Format("SendMIDI: no need to send (full buf = %d ms) (steps %d - %d) playback is at %d", 
		//	midi_sent_t - timestamp_current, step1, step2, timestamp_current - midi_start_time);
		//WriteLog(4, st);
		return;
	}
	int i;
	if (!mutex_output.try_lock_for(chrono::milliseconds(3000))) {
		WriteLog(0, new CString("SendMIDI mutex timed out: will try later"));
	}
	int step21; // Voice-dependent first step
	int step22; // Voice-independent last step
							// Find last step not too far
	float time;
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
	if (midi_buf_next.size() > 0) {
		midi_buf = midi_buf_next;
		midi_buf_next.clear();
		WriteLog(4, new CString("Postponed notes sent"));
	}
	midi_buf_lim = 0;
	for (int v = 0; v < v_cnt; v++) {
		// Initialize voice
		PmEvent event;
		int ei;
		int ndur;
		int ncount = 0;
		int last_i = step1; // first step of last full note
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
			last_i = i;
			// Set new buffer limit to beginning of last note
			if (noff[i][v] == 0) break;
			i += noff[i][v] - 1;
		}
		// Set midi_buf_lim only for first voice. Other voices use same midi_buf_lim
		if (midi_buf_lim == 0) midi_buf_lim = midi_start_time + stime[last_i] * 100 / m_pspeed;
		// Send notes
		i = step21;
		for (int x = 0; x < ncount; x++) {
			midi_current_step = i;
			ei = max(0, i + len[i][v] - 1);
			if (!pause[i][v]) {
				// Note ON if it is not blocked and was not yet sent
				stimestamp = stime[i] * 100 / m_pspeed + dstime[i][v];
				if ((stimestamp + midi_start_time >= midi_sent_t) && (i >= midi_sent)) AddNoteOn(stimestamp, note[i][v] + play_transpose[v], vel[i][v]);
				// Note OFF if it is in window
				if (ei <= step22) {
					// Note OFF
					// ndur = (etime[ei] - stime[i]) * 100 / m_pspeed + detime[ei][v] - dstime[i][v];
					etimestamp = etime[ei] * 100 / m_pspeed + detime[ei][v];
					AddNoteOff(etimestamp, note[ei][v] + play_transpose[v], 0);
				}
				// Send slur
				if (artic[i][v] == ARTIC_SLUR) {
					AddTransitionKs(i, stimestamp, slur_ks[ii]);
				}
				// Send retrigger
				if ((instr[v] == INSTR_VIOLIN) && (artic[i][v] == ARTIC_RETRIGGER)) {
					AddTransitionCC(i, stimestamp, CC_retrigger[ii], 100, 0);
				}
			}
			// Go to next note
			if (noff[i][v] == 0) break;
			i += noff[i][v];
		}
		// Send CC dynamics
		if (CC_dyn[ii]) {
			int cc_value;
			float cc_step; // Length of cc interpolation step
			float cc_pos1; // Middle of current note step
			float cc_pos2; // Middle of next note step
			for (int i = step21 - 2; i < step22 - 1; i++) {
				if (i < 0) continue;
				midi_current_step = i;
				vector <float> cc_lin; // Linear interpolation
				vector <float> cc_ma; // Moving average
				cc_lin.resize(CC_steps[ii] * 2);
				cc_ma.resize(CC_steps[ii]);
				// Calculate window
				cc_pos1 = (etime[i] + stime[i]) * 100 / m_pspeed / 2;
				cc_pos2 = (etime[i + 1] + stime[i + 1]) * 100 / m_pspeed / 2;
				cc_step = (cc_pos2 - cc_pos1) / CC_steps[ii];
				// Linear interpolation
				for (int c = 0; c < CC_steps[ii] * 2; c++) {
					// Left cc steps
					if (c < CC_steps[ii] / 2) {
						if (i == 0) cc_lin[c] = dyn[i][v];
						else cc_lin[c] = (floor(CC_steps[ii] * 0.5 - c) * dyn[i - 1][v] + floor(c + 1 + CC_steps[ii] / 2) * dyn[i][v]) / CC_steps[ii];
					}
					// Mid cc steps
					else if (c < CC_steps[ii] * 1.5) {
						cc_lin[c] = (floor(CC_steps[ii] * 1.5 - c) * dyn[i][v] + floor(c - CC_steps[ii] / 2) * dyn[i + 1][v]) / CC_steps[ii];
					}
					// Right cc steps
					else {
						if (i == step22 - 2) cc_lin[c] = dyn[i + 1][v];
						else cc_lin[c] = (floor(CC_steps[ii] * 2.5 - c) * dyn[i + 1][v] + floor(c - CC_steps[ii] * 1.5 + 1) * dyn[i + 2][v]) / CC_steps[ii];
					}
				}
				if (!CC_dyn_ma[ii]) {
					// Send linear CC
					for (int c = 0; c < CC_steps[ii]; c++) {
						AddCC(stime[i] * 100 / m_pspeed + (etime[i] - stime[i]) * 100 / m_pspeed*(float)c / (float)CC_steps[ii], CC_dyn[ii], cc_lin[c]);
					}
				}
				else {
					// First moving average
					cc_ma[0] = 0;
					for (int c = 0; c < CC_steps[ii]; c++) {
						cc_ma[0] += cc_lin[c] / (float)CC_steps[ii];
					}
					// Extend moving average
					for (int c = 1; c < CC_steps[ii]; c++) {
						cc_ma[c] = cc_ma[c - 1] + (cc_lin[c + CC_steps[ii] - 1] - cc_lin[c - 1]) / (float)CC_steps[ii];
					}
					// Send starting CC
					if (i == 0) AddCC(-1, CC_dyn[ii], dyn[i][v]);
					// Send ma CC of first note
					int hstep = CC_steps[ii] / 2;
					if (i > step21 - 2) for (int c = 0; c < hstep + 1; c++) {
						int t = stime[i] * 100 / m_pspeed + (etime[i] - stime[i]) * 100 / m_pspeed*(float)(c + hstep) / (float)CC_steps[ii];
						if (t >= midi_sent_t - midi_start_time) AddCC(t, CC_dyn[ii], cc_ma[c]);
					}
					// Send ma CC of second note
					if (i <  step22 - 2) for (int c = hstep + 1; c < CC_steps[ii]; c++) {
						int t = stime[i + 1] * 100 / m_pspeed + (etime[i + 1] - stime[i + 1]) * 100 / m_pspeed*(float)(c - hstep - 1) / (float)CC_steps[ii];
						if (t >= midi_sent_t - midi_start_time) AddCC(t, CC_dyn[ii], cc_ma[c]);
					}
				}
			}
		}
	}
	// Sort by timestamp before sending
	qsort(midi_buf.data(), midi_buf.size(), sizeof(PmEvent), PmEvent_comparator);
	// Send
	Pm_Write(midi, midi_buf.data(), midi_buf.size());
	// Count time
	milliseconds time_stop = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
	CString* st = new CString;
	st->Format("Pm_Write %d (%d postponed) events: steps %d/%d - %d/%d [to future %d to %d ms] (in %d ms) playback is at %d ms",
		midi_buf.size(), midi_buf_next.size(), step21, step1, step22, step2, midi_sent_t - timestamp_current, midi_sent_t2 - timestamp_current,
		time_stop - time_start, timestamp_current - midi_start_time);
	WriteLog(4, st);
	// Save last sent position
	midi_sent = step22;
	midi_sent_t = midi_sent_t2;
	midi_sent_msg = midi_sent_msg2;
	mutex_output.unlock();
}

void CGMidi::StopMIDI()
{
	WriteLog(4, new CString("Pm_Close"));
	if (midi != 0) Pm_Close(midi);
	midi = 0;
}

int CGMidi::GetPlayStep() {
	if (buf_underrun == 1) {
		midi_play_step = 0;
	}
	else {
		// Don't need lock, because this function is called from OnDraw, which already has lock
		/*
		if (!mutex_output.try_lock_for(chrono::milliseconds(100))) {
		WriteLog(1, new CString("GetPlayStep mutex timed out"));
		}
		*/
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
	if ((data1 == 0) || ((data1 >= instr_nmin[instr[midi_voice]]) && (data1 <= instr_nmax[instr[midi_voice]]))) {
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
	if ((data1 == 0) || ((data1 >= instr_nmin[instr[midi_voice]]) && (data1 <= instr_nmax[instr[midi_voice]]))) {
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