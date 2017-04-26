#include "stdafx.h"
#include "GVar.h"


CGVar::CGVar()
{
	// Init constant length arrays
	instr.resize(MAX_VOICE);
	instr_lib.resize(MAX_VOICE);
	instr_type.resize(MAX_INSTR);
	instr_used.resize(MAX_INSTR);
	instr_channel.resize(MAX_INSTR);
	instr_poly.resize(MAX_INSTR);
	instr_nmin.resize(MAX_INSTR);
	instr_nmax.resize(MAX_INSTR);
	instr_tmin.resize(MAX_INSTR);
	instr_tmax.resize(MAX_INSTR);
	vib_bell_top1.resize(MAX_INSTR);
	vib_bell_top2.resize(MAX_INSTR);
	vibf_bell_top1.resize(MAX_INSTR);
	vibf_bell_top2.resize(MAX_INSTR);
	vib_bell_freq.resize(MAX_INSTR);
	vib_bell_mindur.resize(MAX_INSTR);
	vib_bell_dur.resize(MAX_INSTR);
	vib_bell1.resize(MAX_INSTR);
	vibf_bell1.resize(MAX_INSTR);
	vib_bell2.resize(MAX_INSTR);
	vibf_bell2.resize(MAX_INSTR);
	vib_bell_exp.resize(MAX_INSTR);
	vibf_bell_exp.resize(MAX_INSTR);
	rnd_vel.resize(MAX_INSTR);
	rnd_dyn.resize(MAX_INSTR);
	rnd_vib.resize(MAX_INSTR);
	rnd_vibf.resize(MAX_INSTR);
	CC_vib.resize(MAX_INSTR);
	CC_vibf.resize(MAX_INSTR);
	CC_steps.resize(MAX_INSTR);
	CC_dyn.resize(MAX_INSTR);
	CC_dyn_ma.resize(MAX_INSTR);
	CC_retrigger.resize(MAX_INSTR);
	retrigger_freq.resize(MAX_INSTR);
	max_slur_count.resize(MAX_INSTR);
	max_slur_interval.resize(MAX_INSTR);
	slur_ks.resize(MAX_INSTR);
	legato_ahead.resize(MAX_INSTR);
	legato_ahead_exp.resize(MAX_INSTR);
	splitpo_freq.resize(MAX_INSTR);
	nonlegato_freq.resize(MAX_INSTR);
	nonlegato_minlen.resize(MAX_INSTR);
	lengroup2.resize(MAX_INSTR);
	lengroup3.resize(MAX_INSTR);
	lengroup4.resize(MAX_INSTR);
	lengroup_edt1.resize(MAX_INSTR);
	lengroup_edt2.resize(MAX_INSTR);
	rand_start.resize(MAX_INSTR);
	rand_end.resize(MAX_INSTR);
	retrigger_min_len.resize(MAX_INSTR);
	retrigger_rand_end.resize(MAX_INSTR);
	retrigger_rand_max.resize(MAX_INSTR);
	vel_harsh.resize(MAX_INSTR);
	vel_immediate.resize(MAX_INSTR);
	vel_normal.resize(MAX_INSTR);
	vel_gliss.resize(MAX_INSTR);
	vel_normal_minlen.resize(MAX_INSTR);
	gliss_minlen.resize(MAX_INSTR);
	gliss_freq.resize(MAX_INSTR);
	rand_start_max.resize(MAX_INSTR);
	rand_end_max.resize(MAX_INSTR);
	max_ahead_note.resize(MAX_INSTR);
	bell_start_mul.resize(MAX_INSTR);
	bell_end_mul.resize(MAX_INSTR);
	bell_start_len.resize(MAX_INSTR);
	bell_end_len.resize(MAX_INSTR);
	bell_mindur.resize(MAX_INSTR);
	rbell_freq.resize(MAX_INSTR);
	rbell_mindur.resize(MAX_INSTR);
	rbell_dur.resize(MAX_INSTR);
	rbell_mul.resize(MAX_INSTR);
	rbell_mul2.resize(MAX_INSTR);
	end_sfl_dur.resize(MAX_INSTR);
	end_sfl_freq.resize(MAX_INSTR);
	end_pbd_dur.resize(MAX_INSTR);
	end_pbd_freq.resize(MAX_INSTR);
	end_vib2_dur.resize(MAX_INSTR);
	end_vib2_freq.resize(MAX_INSTR);
	end_vib_dur.resize(MAX_INSTR);
	end_vib_freq.resize(MAX_INSTR);
	show_transpose.resize(MAX_VOICE);
	track_name.resize(MAX_VOICE);
	track_id.resize(MAX_VOICE);
	track_vid.resize(MAX_VOICE);
	// Set instrument
	instr[0] = 5;
	instr[1] = 6;
}

CGVar::~CGVar()
{
}

void CGVar::InitVectors()
{
	// Create vectors
	pause = vector<vector<unsigned char>>(t_allocated, vector<unsigned char>(v_cnt));
	note = vector<vector<unsigned char>>(t_allocated, vector<unsigned char>(v_cnt));
	len = vector<vector<unsigned short>>(t_allocated, vector<unsigned short>(v_cnt));
	coff = vector<vector<unsigned short>>(t_allocated, vector<unsigned short>(v_cnt));
	poff = vector<vector<unsigned short>>(t_allocated, vector<unsigned short>(v_cnt));
	noff = vector<vector<unsigned short>>(t_allocated, vector<unsigned short>(v_cnt));
	tonic = vector<vector<unsigned char>>(t_allocated, vector<unsigned char>(v_cnt));
	dyn = vector<vector<unsigned char>>(t_allocated, vector<unsigned char>(v_cnt));
	vel = vector<vector<unsigned char>>(t_allocated, vector<unsigned char>(v_cnt));
	vib = vector<vector<unsigned char>>(t_allocated, vector<unsigned char>(v_cnt));
	vibf = vector<vector<unsigned char>>(t_allocated, vector<unsigned char>(v_cnt));
	artic = vector<vector<unsigned char>>(t_allocated, vector<unsigned char>(v_cnt));
	lining = vector<vector<unsigned char>>(t_allocated, vector<unsigned char>(v_cnt));
	midi_ch = vector<vector<unsigned char>>(t_allocated, vector<unsigned char>(v_cnt));
	midi_delta = vector<vector<short>>(t_allocated, vector<short>(v_cnt));
	linecolor = vector<Color>(t_allocated);
	lengroup = vector<vector<char>>(t_allocated, vector<char>(v_cnt));
	comment = vector<vector<CString>>(t_allocated, vector<CString>(v_cnt));
	adapt_comment = vector<vector<CString>>(t_allocated, vector<CString>(v_cnt));
	color = vector<vector<Color>>(t_allocated, vector<Color>(v_cnt));
	tempo = vector<float>(t_allocated);
	stime = vector<float>(t_allocated);
	etime = vector<float>(t_allocated);
	dstime = vector<vector<float>>(t_allocated, vector<float>(v_cnt));
	detime = vector<vector<float>>(t_allocated, vector<float>(v_cnt));
	// Init ngv
	for (int v = 0; v < MAX_VOICE; v++) {
		ngv_min[v] = 1000;
		ngv_max[v] = 0;
	}
	// Init color
	for (int i = 0; i < t_allocated; i++) {
		linecolor[i] = Color(0);
		for (int v = 0; v < v_cnt; v++) {
			color[i][v] = Color(0);
		}
	}
}

void CGVar::ResizeVectors(int size, int vsize)
{
	milliseconds time_start = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
	if (!mutex_output.try_lock_for(chrono::milliseconds(5000))) {
		WriteLog(1, new CString("Critical error: ResizeVectors mutex timed out"));
	}
	if (vsize == -1) vsize = v_cnt;
	pause.resize(size);
	note.resize(size);
	len.resize(size);
	coff.resize(size);
	poff.resize(size);
	noff.resize(size);
	tonic.resize(size);
	tempo.resize(size);
	stime.resize(size);
	etime.resize(size);
	dstime.resize(size);
	detime.resize(size);
	dyn.resize(size);
	vel.resize(size);
	vib.resize(size);
	vibf.resize(size);
	artic.resize(size);
	lining.resize(size);
	linecolor.resize(size, Color(0));
	lengroup.resize(size);
	comment.resize(size);
	adapt_comment.resize(size);
	midi_ch.resize(size);
	midi_delta.resize(size);
	color.resize(size);
	int start = t_allocated;
	if (vsize != v_cnt) start = 0;
	for (int i = start; i < size; i++) {
		pause[i].resize(vsize);
		note[i].resize(vsize);
		len[i].resize(vsize);
		coff[i].resize(vsize);
		poff[i].resize(vsize);
		noff[i].resize(vsize);
		tonic[i].resize(vsize);
		dyn[i].resize(vsize);
		vel[i].resize(vsize);
		vib[i].resize(vsize);
		vibf[i].resize(vsize);
		artic[i].resize(vsize);
		lining[i].resize(vsize);
		lengroup[i].resize(vsize);
		comment[i].resize(vsize);
		adapt_comment[i].resize(vsize);
		midi_ch[i].resize(vsize);
		midi_delta[i].resize(vsize);
		dstime[i].resize(vsize);
		detime[i].resize(vsize);
		color[i].resize(vsize, Color(0));
	}
	// Count time
	if (debug_level > 1) {
		milliseconds time_stop = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
		CString* st = new CString;
		st->Format("ResizeVectors from %d to %d steps, from %d to %d voices (in %d ms)", t_allocated, size, v_cnt, vsize, time_stop - time_start);
		WriteLog(0, st);
	}

	t_allocated = size;
	v_cnt = vsize;
	mutex_output.unlock();
}

void CGVar::LoadConfig(CString fname)
{
	CString st, st2, st3;
	ifstream fs;
	// Check file exists
	if (!fileExists(fname)) {
		CString* est = new CString;
		est->Format("LoadConfig cannot find file: %s", fname);
		WriteLog(1, est);
		return;
	}
	fs.open(fname);
	char pch[2550];
	int pos = 0;
	int i = 0;
	while (fs.good()) {
		i++;
		// Get line
		fs.getline(pch, 2550);
		st = pch;
		// Remove unneeded
		pos = st.Find("#");
		// Check if it is first symbol
		if (pos == 0)	st = st.Left(pos);
		pos = st.Find(" #");
		// Check if it is after space
		if (pos > -1)	st = st.Left(pos);
		st.Trim();
		pos = st.Find("=");
		if (pos != -1) {
			// Get variable name and value
			st2 = st.Left(pos);
			st3 = st.Mid(pos + 1);
			st2.Trim();
			st3.Trim();
			st2.MakeLower();
			// Load general variables
			int idata = atoi(st2);
			float fdata = atof(st3);
			parameter_found = 0;
			CheckVar(&st2, &st3, "v_cnt", &v_cnt);
			CheckVar(&st2, &st3, "t_cnt", &t_cnt);
			CheckVar(&st2, &st3, "t_allocated", &t_allocated);
			CheckVar(&st2, &st3, "t_send", &t_send);
			CheckVar(&st2, &st3, "midifile_in_mul", &midifile_in_mul);
			CheckVar(&st2, &st3, "midifile_out_mul", &midifile_out_mul);
			CheckVar(&st2, &st3, "sleep_ms", &sleep_ms);
			CheckVar(&st2, &st3, "adapt_enable", &adapt_enable);
			CheckVar(&st2, &st3, "comment_adapt", &comment_adapt);
			LoadVar(&st2, &st3, "instr_layout", &instr_layout);
			LoadVar(&st2, &st3, "instruments", &m_config_insts);
			//LoadVarInstr(&st2, &st3, "instruments", instr);
			LoadVectorPar(&st2, &st3, "show_transpose", show_transpose);
			// Load algorithm-specific variables
			LoadConfigLine(&st2, &st3, idata, fdata);
			if (!parameter_found) {
				WriteLog(1, "Unrecognized parameter '" + st2 + "' = '" + st3 + "' in file " + fname);
			}
		}
	}
	fs.close();
	CString* est = new CString;
	est->Format("LoadConfig loaded %d lines from %s", i, fname);
	WriteLog(0, est);
	// Load instruments layout
	if (instr_layout == "") instr_layout = "Default";
	LoadInstrumentLayout();
	// Load instruments
	LoadInstruments();
	// After loading global mapping of voices to instruments, load algorithm-specific mapping
	st2 = "instruments";
	LoadVarInstr(&st2, &m_algo_insts, "instruments", instr);
	LoadVarInstr(&st2, &m_config_insts, "instruments", instr);
}

void CGVar::LoadVarInstr(CString * sName, CString * sValue, char* sSearch, vector<int> & Dest)
{
	if (*sName == sSearch) {
		int pos = 0;
		CString st;
		for (int ii = 0; ii<MAX_VOICE; ii++) {
			st = sValue->Tokenize(",", pos);
			st.Trim();
			if (st == "") break;
			int found = 0;
			for (int i = 0; i < InstGName.size(); i++) {
				if (InstGName[i] == st) {
					++found;
					if (instr_used[i] < instr_poly[i]) {
						++instr_used[i];
						Dest[ii] = i;
						break;
					}
				}
				if (i == InstGName.size() - 1) {
					if (!found) {
						CString* est = new CString;
						est->Format("Cannot find any instrument named %s (%d) in layout %s. Mapped to default instrument %s/%s (%d)", 
							st, ii, instr_layout, InstGName.back(), InstCName.back(), InstGName.size()-1);
						WriteLog(1, est);
					}
					else {
						CString* est = new CString;
						est->Format("Cannot map instrument named %s (%d) in layout %s. Probably too many voices for this instrument type.  Mapped to default instrument %s/%s (%d)", 
							st, ii, instr_layout, InstGName.back(), InstCName.back(), InstGName.size() - 1);
						WriteLog(1, est);
					}
				}
			}
		}
	}
}

void CGVar::LoadInstrumentLayout()
{
	CString fname = "instruments\\" + instr_layout + ".txt";
	// Check file exists
	if (!fileExists(fname)) {
		CString* est = new CString;
		est->Format("LoadInstrumentLayout cannot find file: %s", fname);
		WriteLog(1, est);
		return;
	}
	ifstream fs;
	fs.open(fname);
	CString st, st2, st3, st4, st5;
	char pch[2550];
	int pos = 0;
	// Clear instrument group names
	InstGName.clear();
	// Clear instrument config names
	InstCName.clear();
	int ii = 0;
	while (fs.good()) {
		pos = 0;
		fs.getline(pch, 2550);
		st = pch;
		// Remove unneeded
		pos = st.Find("#");
		// Check if it is first symbol
		if (pos == 0)	st = st.Left(pos);
		pos = st.Find(" #");
		// Check if it is after space
		if (pos > -1)	st = st.Left(pos);
		st.Trim();
		pos = 0;
		if (st.Find("|") != -1) {
			if (InstCName.size() >= MAX_INSTR) {
				CString* est = new CString;
				est->Format("LoadInstrumentLayout found more instruments than MAX_INSTR (%d) in file: %s. Increase MAX_INSTR if needed", MAX_INSTR, fname);
				WriteLog(1, est);
				break;
			}
			st2 = st.Tokenize("|", pos);
			st2.Trim();
			//if (find(begin(InstGName), end(InstGName), st2) != InstGName.end()) {
			InstGName.push_back(st2);
			st2 = st.Tokenize("|", pos);
			st2.Trim();
			InstCName.push_back(st2);
			st2 = st.Tokenize("|", pos);
			st2.Trim();
			instr_channel[InstCName.size() - 1] = atoi(st2);
			// Set default mapping
			instr[InstCName.size() - 1] = InstCName.size() - 1;
		}
	}
	fs.close();
	if (InstCName.size() == 0) {
		WriteLog(1, "Error loading instrument layout from " + fname);
	}
	// Set all instruments to default instrument
	for (int ii = 0; ii < MAX_VOICE; ii++) instr[ii] = InstGName.size() - 1;
}

void CGVar::LoadInstruments()
{
	CString st, st2, st3;
	if (InstCName.size() == 0) {
		WriteLog(1, "No instruments loaded: layout empty");
		return;
	}
	for (int i = 0; i < InstCName.size(); ++i) {
		milliseconds time_start = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
		CString fname = "instruments\\" + InstGName[i] + "\\" + InstCName[i] + ".pl";
		// Load strings
		ifstream fs;
		if (!fileExists(fname)) {
			WriteLog(1, "Cannot find instrument config file " + fname);
			continue;
		}
		fs.open(fname);
		char pch[2550];
		int pos = 0;
		int x = 0;
		while (fs.good()) {
			x++;
			fs.getline(pch, 2550);
			st = pch;
			// Remove comments
			pos = st.Find("#");
			// Check if it is first symbol
			if (pos == 0)	st = st.Left(pos);
			pos = st.Find(" #");
			// Check if it is after space
			if (pos > -1)	st = st.Left(pos);
			st.Trim();
			// Find equals
			pos = st.Find("=");
			if (pos != -1) {
				st2 = st.Left(pos);
				st3 = st.Mid(pos + 1);
				st2.Trim();
				st3.Trim();
				st2.MakeLower();
				int idata = atoi(st3);
				// Initialize loading
				parameter_found = 0;
				LoadVar(&st2, &st3, "library", &instr_lib[i]);
				LoadNote(&st2, &st3, "n_min", &instr_nmin[i]);
				LoadNote(&st2, &st3, "n_max", &instr_nmax[i]);
				CheckVar(&st2, &st3, "t_min", &instr_tmin[i]);
				CheckVar(&st2, &st3, "t_max", &instr_tmax[i]);
				CheckVar(&st2, &st3, "poly", &instr_poly[i]);
				CheckVar(&st2, &st3, "type", &instr_type[i]);
				CheckVar(&st2, &st3, "channel", &instr_channel[i]);
				LoadRange(&st2, &st3, "vib_bell_top", &vib_bell_top1[i], &vib_bell_top2[i]);
				LoadRange(&st2, &st3, "vibf_bell_top", &vibf_bell_top1[i], &vibf_bell_top2[i]);
				LoadRange(&st2, &st3, "vib_bell", &vib_bell1[i], &vib_bell2[i]);
				LoadRange(&st2, &st3, "vibf_bell", &vibf_bell1[i], &vibf_bell2[i]);
				LoadRange(&st2, &st3, "vib_bell_dur", &vib_bell_mindur[i], &vib_bell_dur[i]);
				CheckVar(&st2, &st3, "vib_bell_freq", &vib_bell_freq[i]);
				CheckVar(&st2, &st3, "vib_bell_exp", &vib_bell_exp[i]);
				CheckVar(&st2, &st3, "vibf_bell_exp", &vibf_bell_exp[i]);
				CheckVar(&st2, &st3, "rnd_vel", &rnd_vel[i]);
				CheckVar(&st2, &st3, "rnd_dyn", &rnd_dyn[i]);
				CheckVar(&st2, &st3, "rnd_vib", &rnd_vib[i]);
				CheckVar(&st2, &st3, "rnd_vibf", &rnd_vibf[i]);
				CheckVar(&st2, &st3, "cc_vib", &CC_vib[i]);
				CheckVar(&st2, &st3, "cc_vibf", &CC_vibf[i]);
				CheckVar(&st2, &st3, "cc_steps", &CC_steps[i]);
				CheckVar(&st2, &st3, "cc_dynamics", &CC_dyn[i]);
				CheckVar(&st2, &st3, "cc_dyn_ma", &CC_dyn_ma[i]);
				CheckVar(&st2, &st3, "cc_retrigger", &CC_retrigger[i]);
				CheckVar(&st2, &st3, "retrigger_freq", &retrigger_freq[i]);
				CheckVar(&st2, &st3, "max_slur_count", &max_slur_count[i]);
				CheckVar(&st2, &st3, "max_slur_interval", &max_slur_interval[i]);
				CheckVar(&st2, &st3, "slur_ks", &slur_ks[i]);
				CheckVar(&st2, &st3, "legato_ahead", &legato_ahead[i]);
				CheckVar(&st2, &st3, "legato_ahead_exp", &legato_ahead_exp[i]);
				CheckVar(&st2, &st3, "splitpo_freq", &splitpo_freq[i]);
				CheckVar(&st2, &st3, "nonlegato_minlen", &nonlegato_minlen[i]);
				CheckVar(&st2, &st3, "nonlegato_freq", &nonlegato_freq[i]);
				CheckVar(&st2, &st3, "lengroup2", &lengroup2[i]);
				CheckVar(&st2, &st3, "lengroup3", &lengroup3[i]);
				CheckVar(&st2, &st3, "lengroup4", &lengroup4[i]);
				CheckVar(&st2, &st3, "lengroup_edt1", &lengroup_edt1[i]);
				CheckVar(&st2, &st3, "lengroup_edt2", &lengroup_edt2[i]);
				LoadRange(&st2, &st3, "rand_pos", &rand_start[i], &rand_end[i]);
				LoadRange(&st2, &st3, "rand_pos_max", &rand_start_max[i], &rand_end_max[i]);
				CheckVar(&st2, &st3, "retrigger_min_len", &retrigger_min_len[i]);
				CheckVar(&st2, &st3, "retrigger_rand_end", &retrigger_rand_end[i]);
				CheckVar(&st2, &st3, "retrigger_rand_max", &retrigger_rand_max[i]);
				CheckVar(&st2, &st3, "vel_harsh", &vel_harsh[i]);
				CheckVar(&st2, &st3, "vel_immediate", &vel_immediate[i]);
				CheckVar(&st2, &st3, "vel_normal", &vel_normal[i]);
				CheckVar(&st2, &st3, "vel_gliss", &vel_gliss[i]);
				CheckVar(&st2, &st3, "vel_normal_minlen", &vel_normal_minlen[i]);
				CheckVar(&st2, &st3, "gliss_minlen", &gliss_minlen[i]);
				CheckVar(&st2, &st3, "bell_mindur", &bell_mindur[i]);
				CheckVar(&st2, &st3, "gliss_freq", &gliss_freq[i]);
				CheckVar(&st2, &st3, "max_ahead_note", &max_ahead_note[i]);
				LoadRange(&st2, &st3, "bell_mul", &bell_start_mul[i], &bell_end_mul[i]);
				LoadRange(&st2, &st3, "bell_len", &bell_start_len[i], &bell_end_len[i]);
				CheckVar(&st2, &st3, "rbell_freq", &rbell_freq[i]);
				LoadRange(&st2, &st3, "rbell_dur", &rbell_mindur[i], &rbell_dur[i]);
				LoadRange(&st2, &st3, "rbell_mul", &rbell_mul[i], &rbell_mul2[i]);
				CheckVar(&st2, &st3, "end_sfl_dur", &end_sfl_dur[i]);
				CheckVar(&st2, &st3, "end_sfl_freq", &end_sfl_freq[i]);
				CheckVar(&st2, &st3, "end_pbd_dur", &end_pbd_dur[i]);
				CheckVar(&st2, &st3, "end_pbd_freq", &end_pbd_freq[i]);
				CheckVar(&st2, &st3, "end_vib2_dur", &end_vib2_dur[i]);
				CheckVar(&st2, &st3, "end_vib2_freq", &end_vib2_freq[i]);
				CheckVar(&st2, &st3, "end_vib_dur", &end_vib_dur[i]);
				CheckVar(&st2, &st3, "end_vib_freq", &end_vib_freq[i]);
				if (!parameter_found) {
					WriteLog(1, "Unrecognized parameter '" + st2 + "' = '" + st3 + "' in file " + fname);
				}
				//CGVar::LoadVar(&st2, &st3, "save_format_version", &save_format_version);
			}
		} // while (fs.good())
		fs.close();
		// Log
		milliseconds time_stop = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
		CString* est = new CString;
		est->Format("LoadInstruments loaded %d lines from " + fname + " in %d ms", x, time_stop-time_start);
		WriteLog(0, est);
	}
}

void CGVar::SaveVector2C(ofstream & fs, vector< vector<unsigned char> > &v2D, int i) {
	const char* pointer = reinterpret_cast<const char*>(&v2D[i][0]);
	size_t bytes = v_cnt * sizeof(v2D[i][0]);
	fs.write(pointer, bytes);
}

void CGVar::SaveVector2S(ofstream & fs, vector< vector<unsigned short> > &v2D, int i) {
	const char* pointer = reinterpret_cast<const char*>(&v2D[i][0]);
	size_t bytes = v_cnt * sizeof(v2D[i][0]);
	fs.write(pointer, bytes);
}

void CGVar::SaveVector2Color(ofstream & fs, vector< vector<Color> > &v2D, int i) {
	size_t bytes = 4;
	for (int v = 0; v < v_cnt; v++) {
		DWORD color = v2D[i][v].GetValue();
		fs.write((char*)&color, bytes);
	}
}

void CGVar::SaveVector2ST(ofstream & fs, vector< vector<CString> > &v2D, int i) {
	for (int v = 0; v < v_cnt; v++) {
		unsigned short len = v2D[i][v].GetLength();
		fs.write((char*)&len, sizeof(len));
		if (len != 0) fs.write(v2D[i][v].GetBuffer(), len);
	}
}

void CGVar::SaveVector(ofstream &fs, vector<float> &v) {
	const char* pointer = reinterpret_cast<const char*>(&v[0]);
	size_t bytes = t_generated * sizeof(v[0]);
	fs.write(pointer, bytes);
}

void CGVar::SaveVector(ofstream &fs, vector<Color> &v) {
	const char* pointer = reinterpret_cast<const char*>(&v[0]);
	size_t bytes = t_generated * sizeof(v[0]);
	fs.write(pointer, bytes);
}

void CGVar::SaveResults(CString dir, CString fname)
{
	milliseconds time_start = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
	CreateDirectory(dir, NULL);
	ofstream fs;
	fs.open(dir + "\\" + fname + ".mgr", std::ofstream::binary);
	if (t_generated > 0) {
		for (size_t i = 0; i < t_generated; i++) {
			SaveVector2C(fs, pause, i);
			SaveVector2C(fs, note, i);
			SaveVector2S(fs, len, i);
			SaveVector2S(fs, coff, i);
			SaveVector2C(fs, dyn, i);
			SaveVector2ST(fs, comment, i);
			SaveVector2Color(fs, color, i);
		}
		SaveVector(fs, tempo);
		// Added in version 1.6
		for (size_t i = 0; i < t_generated; i++) {
			SaveVector2C(fs, lining, i);
		}
		SaveVector(fs, linecolor);
		// Added in version 1.6.1
		for (size_t i = 0; i < t_generated; i++) {
			SaveVector2C(fs, tonic, i);
		}
	}
	fs.close();
	// Save strings
	CString st;
	fs.open(dir + "\\" + fname + ".txt");
	fs << "save_format_version = " << MGR_VERSION << " # This is version of format used to save these files\n";
	fs << "m_config = " + m_config + " # Name of config file used for generation\n";
	st.Format("m_algo_id = %d\n", m_algo_id);
	fs << st;
	st.Format("t_cnt = %d # Number of steps I had to generate\n", t_cnt);
	fs << st;
	st.Format("v_cnt = %d # Number of voices\n", v_cnt);
	fs << st;
	st.Format("t_generated = %d # Number of steps I generated\n", t_generated);
	fs << st;
	st.Format("t_sent = %d # Number of steps I finished and sent (can be less than generated)\n", t_sent);
	fs << st;
	st.Format("t_send = %d # Send window\n", t_send);
	fs << st;
	st.Format("ng_min = %d # Minimum generated note\n", ng_min);
	fs << st;
	st.Format("ng_max = %d # Maximum generated note\n", ng_max);
	fs << st;
	st.Format("tg_min = %f # Minimum generated tempo\n", tg_min);
	fs << st;
	st.Format("tg_max = %f # Maximum generated tempo\n", tg_max);
	fs << st;
	st.Format("time_started = %d # Unix timestamp of generation start\n", time_started);
	fs << st;
	st.Format("time_stopped = %d # Unix timestamp of generation finish\n", time_stopped);
	fs << st;
	st.Format("need_exit = %d\n", need_exit);
	fs << st;
	fs.close();
	// Save logs
	fs.open(dir + "\\debug.log");
	for (int i = 0; i<logs[0].size(); i++) fs << logs[0][i] << "\n";
	fs.close();
	fs.open(dir + "\\warning.log");
	for (int i = 0; i<logs[1].size(); i++) fs << logs[1][i] << "\n";
	fs.close();
	fs.open(dir + "\\gui.log");
	for (int i = 0; i<logs[2].size(); i++) fs << logs[2][i] << "\n";
	fs.close();
	fs.open(dir + "\\algorithm.log");
	for (int i = 0; i<logs[3].size(); i++) fs << logs[3][i] << "\n";
	fs.close();
	fs.open(dir + "\\midi.log");
	for (int i = 0; i<logs[4].size(); i++) fs << logs[4][i] << "\n";
	fs.close();
	// Count time
	milliseconds time_stop = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
	CString* est = new CString;
	est->Format("Saved results to files in %d ms", time_stop - time_start);
	WriteLog(0, est);
}

void CGVar::ExportVectorsCSV(CString dir, CString fname)
{
	milliseconds time_start = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
	CString st;
	CreateDirectory(dir, NULL);
	ofstream fs;
	fs.open(dir + "\\" + fname + ".csv");
	fs << "Step;Tempo;Stime;Etime;";
	for (int v = 0; v < v_cnt; ++v) {
		st.Format("%d", v);
		fs << "Pause" + st + ";Note" + st + ";Len" + st + ";Dyn" + st +
			";Coff" + st + ";Poff" + st + ";Noff" + st + ";Comment" + st + ";Color" + st
			+ ";Lining" + st + ";Tonic" + st + ";";
	}
	fs << "\n";
	if (t_generated > 0) {
		for (int i = 0; i < t_generated; ++i) {
			fs << i << ";";
			fs << tempo[i] << ";";
			fs << stime[i] << ";";
			fs << etime[i] << ";";
			for (int v = 0; v < v_cnt; ++v) {
				fs << (int)pause[i][v] << ";";
				fs << (int)note[i][v] << ";";
				fs << (int)len[i][v] << ";";
				fs << (int)dyn[i][v] << ";";
				fs << (int)coff[i][v] << ";";
				fs << (int)poff[i][v] << ";";
				fs << (int)noff[i][v] << ";";
				fs << comment[i][v] << ";";
				fs << color[i][v].GetValue() << ";";
				fs << (int)lining[i][v] << ";";
				fs << (int)tonic[i][v] << ";";
			}
			fs << "\n";
		}
	}
	fs.close();
	// Count time
	milliseconds time_stop = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
	CString* est = new CString;
	est->Format("Saved CSV vectors to file %s\\%s.csv in %d ms", dir, fname, time_stop - time_start);
	WriteLog(0, est);
}

void CGVar::LoadVector2C(ifstream& fs, vector< vector<unsigned char> > &v2D, int i) {
	v2D[i].resize(v_cnt);
	char* pointer = reinterpret_cast<char*>(&(v2D[i][0]));
	size_t bytes = v_cnt * sizeof(v2D[i][0]);
	fs.read(pointer, bytes);
	int read_count = fs.gcount();
	if (read_count != bytes && warning_loadvectors < MAX_WARN_LOADVECTORS) {
		CString* est = new CString;
		est->Format("LoadVector2C: Error reading %d bytes from binary file (got %d bytes instead) at step %d", bytes, read_count, i);
		WriteLog(1, est);
		warning_loadvectors++;
	}
}

void CGVar::LoadVector2S(ifstream& fs, vector< vector<unsigned short> > &v2D, int i) {
	v2D[i].resize(v_cnt);
	char* pointer = reinterpret_cast<char*>(&(v2D[i][0]));
	size_t bytes = v_cnt * sizeof(v2D[i][0]);
	fs.read(pointer, bytes);
	int read_count = fs.gcount();
	if (read_count != bytes && warning_loadvectors < MAX_WARN_LOADVECTORS) {
		CString* est = new CString;
		est->Format("LoadVector2S: Error reading %d bytes from binary file (got %d bytes instead) at step %d", bytes, read_count, i);
		WriteLog(1, est);
		warning_loadvectors++;
	}
}

void CGVar::LoadVector2Color(ifstream & fs, vector< vector<Color> > &v2D, int i) {
	size_t bytes = 4;
	for (int v = 0; v < v_cnt; v++) {
		DWORD color;
		fs.read((char*)&color, bytes);
		int read_count = fs.gcount();
		if (read_count != bytes && warning_loadvectors < MAX_WARN_LOADVECTORS) {
			CString* est = new CString;
			est->Format("LoadVector2Color: Error reading %d bytes from binary file (got %d bytes instead) at step %d", bytes, read_count, i);
			WriteLog(1, est);
			warning_loadvectors++;
		}
		v2D[i][v].SetValue(color);
	}
}

void CGVar::LoadVector2ST(ifstream & fs, vector< vector<CString> > &v2D, int i) {
	for (int v = 0; v < v_cnt; v++) {
		WORD len = 0;
		int bytes = sizeof(len);
		char buf[5000];
		fs.read((char*)&len, bytes);
		int read_count = fs.gcount();
		if (read_count != bytes && warning_loadvectors < MAX_WARN_LOADVECTORS) {
			CString* est = new CString;
			est->Format("LoadVector2ST len: Error reading %d bytes from binary file (got %d bytes instead) at step %d: %d", bytes, read_count, i, len);
			WriteLog(1, est);
			warning_loadvectors++;
		}
		if (len != 0) {
			bytes = len;
			fs.read((char*)&buf, bytes);
			v2D[i][v] = CString(buf, bytes);
			string st = v2D[i][v];
			int read_count = fs.gcount();
			if (read_count != bytes && warning_loadvectors < MAX_WARN_LOADVECTORS) {
				CString* est = new CString;
				est->Format("LoadVector2ST: Error reading %d bytes from binary file (got %d bytes instead) at step %d: %s", bytes, read_count, i, v2D[i][v]);
				WriteLog(1, est);
				warning_loadvectors++;
			}
			if (st.find_first_not_of(" abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890`-=[]\\';/.,~!@#$%^&*()_+|{}:<>?1234567890") != string::npos && warning_loadvectors < MAX_WARN_LOADVECTORS) {
				CString* est = new CString;
				est->Format("LoadVector2ST: String contains unprintable characters at step %d: %s", i, v2D[i][v]);
				WriteLog(1, est);
				warning_loadvectors++;
			}
		}
	}
}

void CGVar::LoadVector(ifstream &fs, vector<float> &v) {
	v.clear();
	v.resize(t_generated);
	char* pointer = reinterpret_cast<char*>(&v[0]);
	size_t bytes = t_generated * sizeof(v[0]);
	fs.read(pointer, bytes);
	int read_count = fs.gcount();
	if (read_count != bytes && warning_loadvectors < MAX_WARN_LOADVECTORS) {
		CString* est = new CString;
		est->Format("LoadVector float: Error reading %d bytes from binary file (got %d bytes instead)", bytes, read_count);
		WriteLog(1, est);
		warning_loadvectors++;
	}
}

void CGVar::LoadVector(ifstream &fs, vector<unsigned char> &v) {
	v.clear();
	v.resize(t_generated);
	char* pointer = reinterpret_cast<char*>(&v[0]);
	size_t bytes = t_generated * sizeof(v[0]);
	fs.read(pointer, bytes);
	int read_count = fs.gcount();
	if (read_count != bytes && warning_loadvectors < MAX_WARN_LOADVECTORS) {
		CString* est = new CString;
		est->Format("LoadVector uchar: Error reading %d bytes from binary file (got %d bytes instead)", bytes, read_count);
		WriteLog(1, est);
		warning_loadvectors++;
	}
}

void CGVar::LoadVector(ifstream &fs, vector<Color> &v) {
	v.clear();
	v.resize(t_generated);
	char* pointer = reinterpret_cast<char*>(&v[0]);
	size_t bytes = t_generated * sizeof(v[0]);
	fs.read(pointer, bytes);
	int read_count = fs.gcount();
	if (read_count != bytes && warning_loadvectors < MAX_WARN_LOADVECTORS) {
		CString* est = new CString;
		est->Format("LoadVector Color: Error reading %d bytes from binary file (got %d bytes instead)", bytes, read_count);
		WriteLog(1, est);
		warning_loadvectors++;
	}
}

void CGVar::LoadResultMusic(CString dir, CString fname)
{
	milliseconds time_start = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
	ifstream fs;
	CString path;
	// Load binary
	path = dir + "\\" + fname + ".mgr";
	if (!fileExists(path)) {
		CString* est = new CString;
		est->Format("Cannot find file %s", path);
		WriteLog(1, est);
		return;
	}
	fs.open(path, ifstream::binary);
	fs.unsetf(ios::skipws);
	if (t_generated != 0) {
		for (size_t i = 0; i < t_generated; i++) {
			LoadVector2C(fs, pause, i);
			LoadVector2C(fs, note, i);
			LoadVector2S(fs, len, i);
			LoadVector2S(fs, coff, i);
			LoadVector2C(fs, dyn, i);
			LoadVector2ST(fs, comment, i);
			LoadVector2Color(fs, color, i);
		}
		LoadVector(fs, tempo);
		// Added with version 1.6
		if (fs.peek() != EOF) {
			for (size_t i = 0; i < t_generated; i++) {
				LoadVector2C(fs, lining, i);
			}
			LoadVector(fs, linecolor);
		}
		// Added in version 1.6.1
		if (fs.peek() != EOF) {
			for (size_t i = 0; i < t_generated; i++) {
				LoadVector2C(fs, tonic, i);
			}
		}
	}
	CountOff(0, t_generated - 1);
	CountTime(0, t_generated - 1);
	UpdateNoteMinMax(0, t_generated - 1);
	UpdateTempoMinMax(0, t_generated - 1);
	fs.close();
	// Count time
	milliseconds time_stop = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
	CString* est = new CString;
	est->Format("Loaded result music from folder %s in %d ms", dir, time_stop - time_start);
	WriteLog(0, est);
	ValidateVectors(0, t_generated - 1);
}

void CGVar::ValidateVectors(int step1, int step2) {
	milliseconds time_start = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
	CString st;
	// Check first step
	if (!step1) for (int v = 0; v < v_cnt; v++) {
		if (coff[0][v] && warning_valid < MAX_WARN_VALID) {
			st.Format("Validation failed at step 0 voice %d: coff must be zero", v);
			WriteLog(1, st);
			warning_valid++;
		}
	}
	for (int i = step1; i <= step2; i++) {
		for (int v = 0; v < v_cnt; v++) {
			if (i > 0) {
				// Check len, pause and note do not change with coff > 0, while coff increases, poff increases, noff decreases
				if (coff[i][v]) {
					if (pause[i][v] != pause[i - 1][v] && warning_valid < MAX_WARN_VALID) {
						st.Format("Validation failed at step %d voice %d: pause change requires coff=0", i, v);
						WriteLog(1, st);
						warning_valid++;
					}
					if (note[i][v] != note[i - 1][v] && warning_valid < MAX_WARN_VALID) {
						st.Format("Validation failed at step %d voice %d: note change requires coff=0", i, v);
						WriteLog(1, st);
						warning_valid++;
					}
					if (len[i][v] != len[i - 1][v] && warning_valid < MAX_WARN_VALID) {
						st.Format("Validation failed at step %d voice %d: len change requires coff=0", i, v);
						WriteLog(1, st);
						warning_valid++;
					}
					if (coff[i][v] != coff[i - 1][v] + 1 && warning_valid < MAX_WARN_VALID) {
						st.Format("Validation failed at step %d voice %d: coff must increase by 1 each step inside note/pause", i, v);
						WriteLog(1, st);
						warning_valid++;
					}
					if (poff[i][v] != poff[i - 1][v] + 1 && i != coff[i][v] && warning_valid < MAX_WARN_VALID) {
						st.Format("Validation failed at step %d voice %d: poff must increase by 1 each step inside note/pause", i, v);
						WriteLog(1, st);
						warning_valid++;
					}
					if (noff[i][v] != noff[i - 1][v] - 1 && warning_valid < MAX_WARN_VALID) {
						st.Format("Validation failed at step %d voice %d: noff must decrease by 1 each step inside note/pause", i, v);
						WriteLog(1, st);
						warning_valid++;
					}
				}
				// Check poff and noff point at correct positions
				else {
					if (noff[i - 1][v] != 1 && warning_valid < MAX_WARN_VALID) {
						st.Format("Validation failed at step %d voice %d: noff must equal 1 one step before next note/pause", i, v);
						WriteLog(1, st);
						warning_valid++;
					}
					if (poff[i][v] != len[i-1][v] && warning_valid < MAX_WARN_VALID) {
						st.Format("Validation failed at step %d voice %d: poff must len of previous note at first step of note/pause", i, v);
						WriteLog(1, st);
						warning_valid++;
					}
				}
			}
			// Check len is not zero
			if (!len[i][v] && warning_valid < MAX_WARN_VALID) {
				st.Format("Validation failed at step %d voice %d: len cannot be zero", i, v);
				WriteLog(1, st);
				warning_valid++;
			}
			// Check key is correct
			if ((tonic[i][v]<0 || tonic[i][v]>11) && warning_valid < MAX_WARN_VALID) {
				st.Format("Validation failed at step %d voice %d: tonic must be in range 0-11", i, v);
				WriteLog(1, st);
				warning_valid++;
			}
		}
		// Check tempo is not zero
		if (!tempo[i] && warning_valid < MAX_WARN_VALID) {
			st.Format("Validation failed at step %d: tempo cannot be zero", i);
			WriteLog(1, st);
			warning_valid++;
		}
		// Check stime is not zero
		if (!stime[i] && i > 0 && warning_valid < MAX_WARN_VALID) {
			st.Format("Validation failed at step %d: stime can be zero only at first step", i);
			WriteLog(1, st);
			warning_valid++;
		}
		// Check etime is not zero
		if (!stime[i] && i > 0 && warning_valid < MAX_WARN_VALID) {
			st.Format("Validation failed at step %d: etime cannot be zero", i);
			WriteLog(1, st);
			warning_valid++;
		}
		// Check etime is greater than stime
		if (etime[i] <= stime[i] && warning_valid < MAX_WARN_VALID) {
			st.Format("Validation failed at step %d: etime must be greater than stime", i);
			WriteLog(1, st);
			warning_valid++;
		}
	}
	// Count time
	if (debug_level > 1) {
		milliseconds time_stop = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
		CString* est = new CString;
		est->Format("Validated vectors steps %d-%d in %d ms", step1, step2, time_stop - time_start);
		WriteLog(0, est);
	}
}

void CGVar::LoadResults(CString dir, CString fname)
{
	milliseconds time_start = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
	ifstream fs;
	CString st, st2, st3, path;
	int pos;
	char pch[2550];
	// Load logs
	path = dir + "\\algorithm.log";
	if (!fileExists(path)) {
		CString* est = new CString;
		est->Format("Cannot find file %s", path);
		WriteLog(1, est);
		return;
	}
	fs.open(path);
	pos = 0;
	while (fs.good()) {
		fs.getline(pch, 2550);
		st = pch;
		if (st != "") WriteLog(3, st);
	}
	fs.close();
	// Load logs
	path = dir + "\\debug.log";
	if (!fileExists(path)) {
		CString* est = new CString;
		est->Format("Cannot find file %s", path);
		WriteLog(1, est);
		return;
	}
	fs.open(path);
	pos = 0;
	while (fs.good()) {
		fs.getline(pch, 2550);
		st = pch;
		if (st != "") WriteLog(0, st);
	}
	fs.close();
	// Load logs
	path = dir + "\\warning.log";
	if (!fileExists(path)) {
		CString* est = new CString;
		est->Format("Cannot find file %s", path);
		WriteLog(1, est);
		return;
	}
	fs.open(path);
	pos = 0;
	while (fs.good()) {
		fs.getline(pch, 2550);
		st = pch;
		if (st != "") WriteLog(1, st);
	}
	fs.close();
	// Load strings
	path = dir + "\\" + fname + ".txt";
	if (!fileExists(path)) {
		CString* est = new CString;
		est->Format("Cannot find file %s", path);
		WriteLog(1, est);
		return;
	}
	fs.open(path);
	pos = 0;
	while (fs.good()) {
		fs.getline(pch, 2550);
		st = pch;
		pos = st.Find("#");
		if (pos != -1) st = st.Left(pos);
		st.Trim();
		pos = st.Find("=");
		if (pos != -1) {
			st2 = st.Left(pos);
			st3 = st.Mid(pos + 1);
			st2.Trim();
			st3.Trim();
			st2.MakeLower();
			int idata = atoi(st3);
			parameter_found = 0;
			CheckVar(&st2, &st3, "t_cnt", &t_cnt);
			CheckVar(&st2, &st3, "m_algo_id", &m_algo_id);
			CheckVar(&st2, &st3, "v_cnt", &v_cnt);
			CheckVar(&st2, &st3, "t_generated", &t_generated);
			CheckVar(&st2, &st3, "t_sent", &t_sent);
			CheckVar(&st2, &st3, "t_send", &t_send);
			CheckVar(&st2, &st3, "need_exit", &need_exit);
			CheckVar(&st2, &st3, "time_started", &time_started);
			CheckVar(&st2, &st3, "time_stopped", &time_stopped);
			LoadVar(&st2, &st3, "m_config", &m_config);
			LoadVar(&st2, &st3, "save_format_version", &save_format_version);
			if (!parameter_found) {
				WriteLog(1, "Unrecognized parameter '" + st2 + "' = '" + st3 + "' in file " + path);
			}
		}
	}
	fs.close();
	// Allocate
	t_allocated = t_generated;
	InitVectors();
	// Count time
	milliseconds time_stop = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
	CString* est = new CString;
	est->Format("Loaded result info from folder %s in %d ms", dir, time_stop - time_start);
	WriteLog(0, est);
}

// Calculate noff, poff
void CGVar::CountOff(int step1, int step2)
{
	for (int i = step1; i <= step2; i++) {
		for (int v = 0; v < v_cnt; v++) {
			noff[i][v] = len[i][v] - coff[i][v];
			if (i - coff[i][v] - 1 >= 0) poff[i][v] = len[i - coff[i][v] - 1][v] + coff[i][v];
			else poff[i][v] = 0;
		}
	}
}

// Find notes that have wrong len and cut them
void CGVar::FixLen(int step1, int step2)
{
	int real_len;
	for (int v = 0; v < v_cnt; v++) {
		// Calculate real length
		real_len = 1;
		for (int i = step1; i <= step2; i++) {
			// If this is not note start, increase real_length
			if (coff[i][v]) {
				++real_len;
			}
			// If this is note start, recalculate length of previous note
			else {
				// Has to be not first note
				if (i > 0) {
					// Does len differ?
					if (real_len != len[i][v]) {
						for (int x = i - real_len; x < i; ++x) {
							if (x >= 0) len[x][v] = real_len;
						}
					}
					real_len = 1;
				}
			}
		}
	}
}

void CGVar::CountTime(int step1, int step2)
{
	for (int i = step1; i <= step2; i++) {
		if (i > 0) stime[i] = stime[i - 1] + 30000.0 / tempo[i - 1];
		else stime[i] = 0;
		etime[i] = stime[i] + 30000.0 / tempo[i];
	}
}

void CGVar::CopyVoice(int v1, int v2, int step1, int step2, int interval)
{
	for (int i = step1; i <= step2; i++) {
		pause[i][v2] = pause[i][v1];
		if (!pause[i][v1]) note[i][v2] = note[i][v1] + interval;
		len[i][v2] = len[i][v1];
		dyn[i][v2] = dyn[i][v1];
		coff[i][v2] = coff[i][v1];
		poff[i][v2] = poff[i][v1];
		noff[i][v2] = noff[i][v1];
	}
}

void CGVar::UpdateNoteMinMax(int step1, int step2)
{
	for (int i = step1; i <= step2; i++) {
		for (int v = 0; v < v_cnt; v++) if ((pause[i][v] == 0) && (note[i][v] != 0)) {
			// Global minimax includes show_transpose, because it is used only for visualization
			if (ng_min > note[i][v] + show_transpose[v]) ng_min = note[i][v] + show_transpose[v];
			if (ng_max < note[i][v] + show_transpose[v]) ng_max = note[i][v] + show_transpose[v];
			// Voice minimax does not include show_transpose, because it is used for Adaptation
			if (ngv_min[v] > note[i][v]) ngv_min[v] = note[i][v];
			if (ngv_max[v] < note[i][v]) ngv_max[v] = note[i][v];
		}
	}
}

void CGVar::UpdateTempoMinMax(int step1, int step2)
{
	for (int i = step1; i <= step2; i++) {
		if (tg_min > tempo[i]) tg_min = tempo[i];
		if (tg_max < tempo[i]) tg_max = tempo[i];
	}
}

void CGVar::AddNote(int pos, int v, char note2, int len2, int dyn2)
{
	for (int i = 0; i < len2; i++) {
		note[pos + i][v] = note2;
		len[pos][v] = len2;
		coff[pos][v] = i;
		dyn[pos][v] = dyn2;
	}
}