// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../stdafx.h"
#include "GVar.h"


CGVar::CGVar()
{
	color_noflag = MakeColor(0, 100, 100, 100);
	// Init constant length arrays
	ngv_min.resize(MAX_VOICE);
	ngv_max.resize(MAX_VOICE);
	instr.resize(MAX_VOICE);
	v_stage.resize(MAX_VOICE);
	v_itrack.resize(MAX_VOICE);
	t_instr.resize(MAX_VOICE);

	show_transpose.resize(MAX_VOICE);
	track_name.resize(MAX_VOICE);
	track_id.resize(MAX_VOICE);
	track_vid.resize(MAX_VOICE);
	itrack.resize(MAX_VOICE);
	// Set instrument
	instr[0] = 5;
	instr[1] = 6;
	nfreq.resize(256);
}

CGVar::~CGVar()
{
}

void CGVar::InitVectors()
{
	ResizeVectors(t_allocated);
	// Init ngv
	for (int v = 0; v < MAX_VOICE; v++) {
		ngv_min[v] = 1000;
		ngv_max[v] = 0;
	}
}

void CGVar::AddIcf() {
	if (icf.size() >= MAX_INSTR) {
		CString est;
		est.Format("Cannot create more instruments than MAX_INSTR (%d). Increase MAX_INSTR if needed", MAX_INSTR);
		WriteLog(5, est);
		return;
	}
	icf.resize(icf.size() + 1);
	icf[icf.size() - 1].KswGroup.resize(128);
	icf[icf.size() - 1].legato_ahead.resize(10);
	icf[icf.size() - 1].ahead_chrom.resize(16);
}

// If info2 is empty, it is not overwritten
void CGVar::AddMelody(int step1, int step2, int v, CString info, CString info2) {
	// Find existing mel_info
	int found = 1;
	if (mel_id[step1][v] > -1) {
		for (int i = step1+1; i <= step2; ++i) {
			if (mel_id[i][v] != mel_id[i - 1][v]) {
				found = 0;
				break;
			}
		}
		if (found) {
			// Update existing mel_info
			mel_info[mel_id[step1][v]] = info;
			if (!info2.IsEmpty()) mel_info2[mel_id[step1][v]] = info2;
			return;
		}
	}
	for (int i = step1; i <= step2; ++i) {
		mel_id[i][v] = mel_info.size();
	}
	mel_info.push_back(info);
	mel_info2.push_back(info2);
}

void CGVar::ResizeVectors(int size, int vsize)
{
	long long time_start = CGLib::time();
	if (!mutex_output.try_lock_for(chrono::milliseconds(5000))) {
		WriteLog(5, "Critical error: ResizeVectors mutex timed out");
	}
	if (vsize == -1) vsize = v_cnt;
	pause.resize(size);
	note.resize(size);
	note_muted.resize(size);
	midifile_out_mul.resize(size, 1);
	len.resize(size);
	coff.resize(size);
	poff.resize(size);
	noff.resize(size);
	tempo.resize(size);
	tempo_src.resize(size);
	stime.resize(size);
	etime.resize(size);
	smst.resize(size);
	smet.resize(size);
	dstime.resize(size);
	detime.resize(size);
 	sstime.resize(size);
	setime.resize(size);
	dyn.resize(size);
	vel.resize(size);
	vib.resize(size);
	vibf.resize(size);
	artic.resize(size);
	filter.resize(size);
	lengroup.resize(size);
	adapt_comment.resize(size);
	midi_ch.resize(size);

	ngraph.resize(size, vector<vector<float> >(vsize, vector<float>(ngraph_size)));
	graph.resize(size, vector<vector<float> >(vsize, vector<float>(graph_size)));
	tonic.resize(size);
	minor.resize(size);
	nlink.resize(size);
	lining.resize(size);
	mel_id.resize(size);
	mark.resize(size);
	mark_color.resize(size);
	linecolor.resize(size);
	lyrics.resize(size);
	comment.resize(size);
	ccolor.resize(size);
	comment2.resize(size);
	nsr1.resize(size);
	nsr2.resize(size);
	color.resize(size);

	int start = t_allocated;
	// Start from zero if we are allocating first time
	if (size == t_allocated) start = 0;
	if (vsize != v_cnt) start = 0;
	for (int i = start; i < size; i++) {
		pause[i].resize(vsize);
		note[i].resize(vsize);
		note_muted[i].resize(vsize);
		ngraph[i].resize(vsize);
		graph[i].resize(vsize);
		len[i].resize(vsize);
		coff[i].resize(vsize);
		poff[i].resize(vsize);
		noff[i].resize(vsize);
		tonic[i].resize(vsize);
		minor[i].resize(vsize);
		dyn[i].resize(vsize);
		vel[i].resize(vsize);
		vib[i].resize(vsize);
		vibf[i].resize(vsize);
		artic[i].resize(vsize);
		filter[i].resize(vsize);
		nlink[i].resize(vsize);
		lining[i].resize(vsize);
		lengroup[i].resize(vsize);
		lyrics[i].resize(vsize);
		comment[i].resize(vsize);
		ccolor[i].resize(vsize);
		comment2[i].resize(vsize);
		nsr1[i].resize(vsize);
		nsr2[i].resize(vsize);
		adapt_comment[i].resize(vsize);
		midi_ch[i].resize(vsize);
		dstime[i].resize(vsize);
		detime[i].resize(vsize);
		sstime[i].resize(vsize);
		setime[i].resize(vsize);
		smst[i].resize(vsize, -1);
		smet[i].resize(vsize, -1);
		color[i].resize(vsize);
		mel_id[i].resize(vsize, -1);
		mark[i].resize(vsize);
		mark_color[i].resize(vsize);
	}
	// Count time
	if (debug_level > 1) {
		long long time_stop = CGLib::time();
		CString st;
		st.Format("ResizeVectors from %d to %d steps, from %d to %d voices (in %lld ms)", 
			t_allocated, size, v_cnt, vsize, time_stop - time_start);
		WriteLog(0, st);
	}

	t_allocated = size;
	v_cnt = vsize;
	mutex_output.unlock();
}

void CGVar::LoadConfigFile(CString fname, int load_includes) {
	CString st, st2, st3, iname;
	ifstream fs;
	int instr_id = -1;
	int tr_id = -1;
	int st_id = -1;
	m_current_config = fname;
	// Check file exists
	if (!fileExists(fname)) {
		CString est;
		est.Format("LoadConfigFile cannot find file: %s", fname);
		WriteLog(5, est);
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
		st.Replace("\"", "");
		// Remove unneeded
		pos = st.Find("#");
		// Check if it is first symbol
		if (pos == 0)	st = st.Left(pos);
		pos = st.Find(" #");
		// Check if it is after space
		if (pos > -1)	st = st.Left(pos);
		st.Trim();
		// Load include
		if (load_includes && CheckInclude(st, fname, iname)) LoadConfigFile(iname);
		if (error) break;
		pos = st.Find("=");
		if (pos != -1) {
			// Get variable name and value
			st2 = st.Left(pos);
			st3 = st.Mid(pos + 1);
			st2.Trim();
			st3.Trim();
			st2.MakeLower();
			// Load general variables
			int idata = atoi(st3);
			float fdata = atof(st3);
			parameter_found = 0;
			CheckVar(&st2, &st3, "v_cnt", &v_cnt, 1, 10);
			CheckVar(&st2, &st3, "t_cnt", &t_cnt, 1);
			CheckVar(&st2, &st3, "t_allocated", &t_allocated, 0);
			CheckVar(&st2, &st3, "t_send", &t_send, 1);
			CheckVar(&st2, &st3, "midifile_in_mul", &midifile_in_mul, 0.00000001);
			CheckVar(&st2, &st3, "midifile_out_mul", &midifile_out_mul0, 0.00000001);
			CheckVar(&st2, &st3, "sleep_ms", &sleep_ms, 0);
			CheckVar(&st2, &st3, "midifile_export_marks", &midifile_export_marks, 0, 1);
			CheckVar(&st2, &st3, "midifile_export_comments", &midifile_export_comments, 0, 1);
			CheckVar(&st2, &st3, "adapt_enable", &adapt_enable, 0, 1);
			CheckVar(&st2, &st3, "autolegato", &auto_legato, 0, 1);
			CheckVar(&st2, &st3, "autononlegato", &auto_nonlegato, 0, 1);
			CheckVar(&st2, &st3, "grownotes", &grow_notes);
			CheckVar(&st2, &st3, "comment_adapt", &comment_adapt, 0, 1);
			CheckVar(&st2, &st3, "mastervolume", &master_vol, 0, 100);
			LoadVar(&st2, &st3, "instr_layout", &instr_layout);
			LoadVar(&st2, &st3, "instruments", &m_config_insts);
			if (st2 == "unison_mute") {
				++parameter_found;
				int val = atoi(st3);
				for (int ii = 0; ii < icf.size(); ++ii) {
					icf[ii].unis_mute = val;
				}
			}
			// Load midi file type
			if (st2 == "midifiletype") {
				++parameter_found;
				if (st3 == "Sibelius") {
					midi_file_type = mftSIB;
					grow_notes = 0;
				}
				if (st3 == "Finale") {
					midi_file_type = mftFIN;
					grow_notes = 0;
				}
				if (st3 == "MuseScore") {
					midi_file_type = mftMUS;
					grow_notes = 2;
				}
			}
			//LoadVarInstr(&st2, &st3, "instruments", instr);
			LoadVectorPar(&st2, &st3, "show_transpose", show_transpose, 0);
			// Load instrument layout overrides
			LoadInstrumentLayoutLine(st2, st3);
			// Load instrument id
			if (st2.Find(":") > 0) {
				++parameter_found;
				instr_id = -1;
				tr_id = -1;
				st_id = -1;
				CString ipath = st2.Left(st2.Find(":"));
				ipath.Replace("\"", "");
				ipath.Trim();
				vector<CString> sa;
				Tokenize(ipath, sa, "/");
				// Load instrument group
				if (sa.size() == 1) {
					int found = 0;
					for (int i = 0; i < icf.size(); ++i) {
						if (!ipath.CompareNoCase(icf[i].group)) {
							instr_id = i;
							++found;
						}
					}
					if (found > 1)
						WriteLog(5, "Instrument group " + ipath + 
							" is ambiguous. Please add instrument config name after slash in file " + fname);
				}
				// Load instrument
				else {
					for (int i = 0; i < icf.size(); ++i) {
						if (!sa[0].CompareNoCase(icf[i].group) && !sa[1].CompareNoCase(icf[i].name)) {
							instr_id = i;
							break;
						}
					}
				}
				if (instr_id != -1 && sa.size() > 2) {
					tr_id = atoi(sa[2]);
					// Find child
					if (icf[instr_id].child.find(tr_id) != icf[instr_id].child.end()) {
						instr_id = icf[instr_id].child[tr_id];
					}
					else {
						instr_id = CreateVirtualInstrument(instr_id, tr_id);
					}
				}
				if (instr_id != -1 && sa.size() == 4) {
					st_id = atoi(sa[3]);
					// Find child
					if (icf[instr_id].child.find(st_id) != icf[instr_id].child.end()) {
						instr_id = icf[instr_id].child[st_id];
					}
					else {
						instr_id = CreateVirtualInstrument(instr_id, st_id);
					}
				}
				if (instr_id == -1) {
					WriteLog(5, "Cannot find instrument " + ipath);
				}
				else {
					// Check if instrument was overridden
					if (icf[instr_id].child.size()) {
						WriteLog(5, "After overriding instrument track or stage config, you cannot reconfigure whole instrument at line " + st + " in file " + fname);
					}
				}
				// Load instrument overrides
				if (instr_id > -1) {
					CString st4 = st2.Mid(st2.Find(":") + 1);
					st4.Trim();
					LoadInstrumentLine(st4, st3, instr_id);
				}
			}
			// Load algorithm-specific variables if we are not loading saved results
			if (!m_loading) {
				LoadConfigLine(&st2, &st3, idata, fdata);
				if (!parameter_found) {
					WriteLog(5, "Unrecognized parameter '" + st2 + "' = '" + st3 + "' in file " + fname);
				}
			}
			if (error) break;
		}
	}
	fs.close();
	CString est;
	est.Format("LoadConfigFile loaded %d lines from %s", i, fname);
	WriteLog(0, est);
}

short CGVar::CreateVirtualInstrument(int instr_id, int child_id) {
	// Allocate new virtual instrument
	++virt_instr_count;
	AddIcf();
	int instr_id2 = icf.size() - 1;
	// Copy instrument config
	icf[instr_id2] = icf[instr_id];
	// Remove childs after copying
	icf[instr_id2].child.clear();
	// Save parent
	icf[instr_id2].parent = instr_id;
	// Save child
	icf[instr_id].child[child_id] = instr_id2;
	return instr_id2;
}

void CGVar::LoadConfig(CString fname, int load_includes) {
	CString st2;
	// Load instruments layout
	if (instr_layout.IsEmpty()) instr_layout = "Default";
	LoadInstrumentLayout();
	// Load instruments
	LoadInstruments();
	// Load configs again, overwriting instrument parameters
	LoadConfigFiles(fname, load_includes);
	// After loading global mapping of voices to instruments, load algorithm-specific mapping
	st2 = "instruments";
	LoadVarInstr(&st2, &m_algo_insts, "instruments", instr);
	LoadVarInstr(&st2, &m_config_insts, "instruments", instr);
}

void CGVar::LoadConfigFiles(CString fname, int load_includes) {
	LoadConfigFile(fname, load_includes);
	// Load autotest config
	if (m_testing == 1) {
		CString fname2 = "autotest\\configs\\" + m_algo_folder + ".pl";
		if (fileExists(fname2)) LoadConfigFile(fname2, load_includes);
	}
	if (m_testing == 2) {
		CString fname2 = "configs\\server\\" + m_algo_folder + ".pl";
		if (fileExists(fname2)) LoadConfigFile(fname2, load_includes);
	}
}

void CGVar::LoadVarInstr(CString * sName, CString * sValue, char* sSearch, vector<int> & Dest) {
	if (*sName == sSearch) {
		int pos = 0, ii = 0;
		CString st;
		for (int v = 0; v<MAX_VOICE; v++) {
			st = sValue->Tokenize(",", pos);
			st.Trim();
			if (st.IsEmpty()) break;
			int found = 0;
			if (st.Find("/") == -1) {
				for (ii = 0; ii < icf.size(); ii++) {
					if (icf[ii].group == st) {
						++found;
						Dest[v] = ii;
						break;
					}
				}
			}
			// Load particular config
			else {
				CString gname = st.Left(st.Find("/"));
				CString cname = st.Mid(st.Find("/") + 1);
				for (ii = 0; ii < icf.size(); ii++) {
					if (icf[ii].group == gname && icf[ii].name == cname) {
						++found;
						Dest[v] = ii;
						break;
					}
				}
			}
			if (!found) {
				CString est;
				est.Format("Cannot find any instrument named %s (%d) in layout %s. Mapped to default instrument %s/%s (%d)",
					st, v, instr_layout, icf[0].group, icf[0].name, 0);
				WriteLog(5, est);
			}
		}
	}
}

void CGVar::LoadInstrumentLayout()
{
	CString fname = "instruments\\" + instr_layout + ".txt";
	long long time_start = CGLib::time();
	// Check file exists
	if (!fileExists(fname)) {
		CString est;
		est.Format("LoadInstrumentLayout cannot find file: %s", fname);
		WriteLog(5, est);
		return;
	}
	ifstream fs;
	fs.open(fname);
	CString st, st2, st3, iclass;
	char pch[2550];
	int pos = 0;
	int x = 0;
	int ii = 0;
	// Clear instrument configs
	icf.clear();
	while (fs.good()) {
		++x;
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
			st2 = st.Tokenize("|", pos);
			st2.Trim();
			// Get instrument class
			if (st2.Find("/") != -1) {
				iclass = st2.Left(st2.Find("/"));
				st2 = st2.Mid(st2.Find("/") + 1);
			}
			else iclass = "";
			for (int x = 0; x < icf.size(); ++x) {
				if (icf[x].group == st2) {
					WriteLog(5, "Instrument layout should contain unique instrument groups. Detected duplicate: " + st2);
				}
			}
			AddIcf();
			ii = icf.size() - 1;
			icf[ii].iclass = iclass;
			icf[ii].group = st2;
			icf[ii].default_instr = ii;
			st2 = st.Tokenize("|", pos);
			st2.Trim();
			icf[ii].name = st2;
			st2 = st.Tokenize("|", pos);
			st2.Trim();
			icf[ii].channel = atoi(st2);
			st2 = st.Tokenize("|", pos);
			st2.Trim();
			icf[ii].track = atoi(st2);
			st2 = st.Tokenize("|", pos);
			st2.Trim();
			if (st2 == "+") icf[ii].port = 1;
			// Set default mapping
			instr[ii] = ii;
			InstDefaultConfig[icf[ii].group] = icf[ii].name;
		}
		pos = st.Find("=");
		if (pos != -1) {
			st2 = st.Left(pos);
			st3 = st.Mid(pos + 1);
			st2.Trim();
			st3.Trim();
			st2.MakeLower();
			parameter_found = 0;
			LoadInstrumentLayoutLine(st2, st3);
			if (!parameter_found) {
				WriteLog(5, "Unrecognized parameter '" + st2 + "' = '" + st3 + "' in file " + fname);
			}
		}
	}
	// 
	for (int i = 0; i < MAX_VOICE; i++) instr[i] = 0;
	fs.close();
	if (icf.size() == 0) {
		WriteLog(5, "No instruments loaded from " + fname);
	}
	// Log
	long long time_stop = CGLib::time();
	CString est;
	est.Format("LoadInstrumentLayout loaded %d lines from " + fname + " in %d ms", x, time_stop - time_start);
	WriteLog(0, est);
}

void CGVar::LoadInstrumentLayoutLine(CString &st2, CString &st3) {
	CheckVar(&st2, &st3, "rnd_tempo", &rnd_tempo);
	CheckVar(&st2, &st3, "rnd_tempo_step", &rnd_tempo_step);
}

void CGVar::LoadInstruments() {
	CString fname, cname;
	int found_default, ii2;
	for (int ii = 0; ii < icf.size(); ++ii) if (icf[ii].default_instr == ii) {
		CFileFind finder;
		CString strWildcard = "instruments\\" + icf[ii].group + "\\*.*";
		BOOL bWorking = finder.FindFile(strWildcard);
		found_default = 0;
		while (bWorking) {
			bWorking = finder.FindNextFile();
			if (finder.IsDots()) continue;
			fname = finder.GetFileName();
			if (fname[0] == '_') continue;
			if (fname.Right(3) != ".pl") continue;
			cname = fname.Left(fname.GetLength() - 3);
			if (cname.Find(".") != -1) {
				cname = cname.Mid(cname.Find(".") + 1);
			}
			// Is it default config?
			if (icf[ii].name == cname) {
				ii2 = ii;
				found_default = 1;
			}
			else {
				// If not, create copy of current config
				AddIcf();
				ii2 = icf.size() - 1;
				icf[ii2] = icf[ii];
			}
			icf[ii2].name = cname;
			icf[ii2].fname = fname;
			icf[ii].configs_count++;
			LoadInstrument(ii2, "instruments\\" + icf[ii2].group + "\\" + icf[ii2].fname);
		}
		finder.Close();
		if (!found_default) {
			WriteLog(5, "Not found file for default instrument config " + icf[ii].group + "/" + icf[ii].name);
		}
	}
}

void CGVar::LoadInstrument(int i, CString fname)
{
	CString st, st2, st3, iname;
	long long time_start = CGLib::time();
	ifstream fs;
	if (!fileExists(fname)) {
		WriteLog(5, "Cannot find instrument config file " + fname);
		return;
	}
	fs.open(fname);
	char pch[2550];
	int pos = 0;
	int x = 0;
	while (fs.good()) {
		x++;
		fs.getline(pch, 2550);
		st = pch;
		st.Replace("\"", "");
		// Remove comments
		pos = st.Find("#");
		// Check if it is first symbol
		if (pos == 0)	st = st.Left(pos);
		pos = st.Find(" #");
		// Check if it is after space
		if (pos > -1)	st = st.Left(pos);
		st.Trim();
		// Load include
		if (CheckInclude(st, fname, iname)) LoadInstrument(i, iname);
		// Find equals
		pos = st.Find("=");
		if (pos != -1) {
			st2 = st.Left(pos);
			st3 = st.Mid(pos + 1);
			st2.Trim();
			st3.Trim();
			st2.MakeLower();
			// Initialize loading
			parameter_found = 0;
			LoadInstrumentLine(st2, st3, i);
			if (!parameter_found) {
				WriteLog(5, "Unrecognized parameter '" + st2 + "' = '" + st3 + "' in file " + fname);
			}
			//CGVar::LoadVar(&st2, &st3, "save_format_version", &save_format_version);
		}
	} // while (fs.good())
	fs.close();
	// Log
	long long time_stop = CGLib::time();
	CString est;
	est.Format("LoadInstruments loaded %d lines from " + fname + " in %d ms", x, time_stop - time_start);
	//WriteLog(0, est);
}

void CGVar::LoadCCName(CString *sName, CString *sValue, CString sSearch, int i) {
	if (*sName != sSearch) return;
	++parameter_found;
	CString st = *sValue;
	st.Trim();
	int pos = st.Find(":");
	if (pos < 1) {
		WriteLog(5, "Cannot find colon in config line " + *sName + " = " + *sValue);
		error = 100;
		return;
	}
	CString st1 = st.Left(pos);
	CString st2 = st.Mid(pos + 1);
	st1.Trim();
	st2.Trim();
	//WriteLog(1, "Detected '" + st1 + "' -> '" + st2 + "'");
	char cc_id = atoi(st1);
	if (!cc_id && st1 != "0") {
		WriteLog(5, "Wrong format for CC id in config line: " + *sName + " = " + *sValue);
		error = 101;
	}
	if (icf[i].NameToCC.find(st2) != icf[i].NameToCC.end() && icf[i].NameToCC[st2] != cc_id) {
		WriteLog(5, "Duplicate CC name for different CC id in config line: " + *sName + " = " + *sValue);
		error = 102;
	}
	if (icf[i].NameToKsw.find(st2) != icf[i].NameToKsw.end()) {
		WriteLog(5, "You cannot use CC name same as KSW name in config line: " + *sName + " = " + *sValue);
		error = 103;
	}
	icf[i].CCToName[cc_id] = st2;
	icf[i].NameToCC[st2] = cc_id;
}

void CGVar::LoadKswGroup(CString *sName, CString *sValue, CString sSearch, int i) {
	if (*sName != sSearch) return;
	++parameter_found;
	CString st3 = *sValue;
	st3.Trim();
	vector <CString> sa;
	Tokenize(st3, sa, ",");
	for (int x = 0; x < sa.size(); ++x) {
		CString st = sa[x];
		st.Trim();
		int pos = st.Find(":");
		if (pos < 1) {
			WriteLog(5, "Cannot find colon in config line " + *sName + " = " + *sValue);
			error = 100;
			return;
		}
		CString st1 = st.Left(pos);
		CString st2 = st.Mid(pos + 1);
		st1.Trim();
		st2.Trim();
		//WriteLog(1, "Detected '" + st1 + "' -> '" + st2 + "'");
		char cnote = GetNoteI(st1);
		if (icf[i].NameToKsw.find(st2) != icf[i].NameToKsw.end() && icf[i].NameToKsw[st2] != cnote) {
			WriteLog(5, "Duplicate KSW name for different notes in config line: " + *sName + " = " + *sValue);
			error = 102;
		}
		if (icf[i].NameToCC.find(st2) != icf[i].NameToCC.end()) {
			WriteLog(5, "You cannot use KSW name same as CC name in config line: " + *sName + " = " + *sValue);
			error = 103;
		}
		icf[i].KswToName[cnote] = st2;
		icf[i].NameToKsw[st2] = cnote;
		icf[i].KswGroup[cnote] = icf[i].ksw_group_count;
	}
	++icf[i].ksw_group_count;
}

PmMessage CGVar::ParseMidiCommand(CString st, int i) {
	st.Trim();
	// Remove value for ksw velocity or CC value
	int value = -1;
	// Get value if specified
	int pos = st.Find(":");
	if (pos > 0) {
		CString st1 = st.Left(pos);
		CString st2 = st.Mid(pos + 1);
		st1.Trim();
		st2.Trim();
		st = st1;
		value = atoi(st2);
	}
	if (icf[i].NameToCC.find(st) != icf[i].NameToCC.end()) {
		// Default value if not specified
		if (value == -1) value = 100;
		int id = icf[i].NameToCC[st];
		//WriteLog(1, "Accepted InitCommand for CC: " + *sName + " = " + *sValue);
		return Pm_Message(MIDI_CC, id, value);
	}
	if (icf[i].NameToKsw.find(st) != icf[i].NameToKsw.end()) {
		// Default value if not specified
		if (value == -1) value = 101;
		int id = icf[i].NameToKsw[st];
		//WriteLog(1, "Accepted InitCommand for KSW: " + *sName + " = " + *sValue);
		return Pm_Message(MIDI_NOTEON, id, value);
	}
return 0;
}

void CGVar::SaveInitCommand(PmMessage msg, int i) {
	// Clear whole group
	if (Pm_MessageStatus(msg) == MIDI_NOTEON) {
		int id = Pm_MessageData1(msg);
		int gr = icf[i].KswGroup[id];
		auto it = icf[i].InitCommands.begin();
		while (it != icf[i].InitCommands.end()) {
			if (Pm_MessageStatus(*it) == MIDI_NOTEON && icf[i].KswGroup[Pm_MessageData1(*it)] == gr) {
				it = icf[i].InitCommands.erase(it);
			}
			else ++it;
		}
	}
	if (Pm_MessageStatus(msg) == MIDI_CC) {
		int id = Pm_MessageData1(msg);
		auto it = icf[i].InitCommands.begin();
		while (it != icf[i].InitCommands.end()) {
			if (Pm_MessageStatus(*it) == MIDI_CC && Pm_MessageData1(*it) == id) {
				it = icf[i].InitCommands.erase(it);
			}
			else ++it;
		}
	}
	icf[i].InitCommands.push_back(msg);
}

void CGVar::LoadInitCommand(CString *sName, CString *sValue, CString sSearch, int i) {
	if (*sName != sSearch) return;
	++parameter_found;
	CString st = *sValue;
	PmMessage msg = ParseMidiCommand(st, i);
	if (!msg) {
		WriteLog(5, "Unknown name. Please first bind CC name or KSW name in instrument config: " + *sName + " = " + *sValue);
		return;
	}
	SaveInitCommand(msg, i);
}

void CGVar::LoadTechnique(CString *sName, CString *sValue, CString sSearch, int i) {
	if (*sName != sSearch) return;
	++parameter_found;
	CString st = *sValue;
	st.Trim();
	int pos = st.Find(";");
	if (pos < 1) {
		WriteLog(5, "Cannot find semicolon in config line " + *sName + " = " + *sValue);
		error = 100;
		return;
	}
	CString st1 = st.Left(pos);
	CString st2 = st.Mid(pos + 1);
	st1.Trim();
	st2.Trim();
	vector<CString> sa;
	Tokenize(st2, sa, "+");
	int id;
	// Find tech
	if (icf[i].NameToTech.find(st1) != icf[i].NameToTech.end()) {
		id = icf[i].NameToTech[st1];
	}
	else {
		id = icf[i].tech.size();
		icf[i].tech.resize(id + 1);
		// Bind
		icf[i].TechToName[id] = st1;
		icf[i].NameToTech[st1] = id;
	}
	icf[i].tech[id].clear();
	for (int x = 0; x < sa.size(); ++x) {
		CString st = sa[x];
		st.Trim();
		PmMessage msg = ParseMidiCommand(st, i);
		// Push value
		icf[i].tech[id].push_back(msg);
	}
}

void CGVar::LoadInitTechnique(CString *sName, CString *sValue, CString sSearch, int i) {
	if (*sName != sSearch) return;
	++parameter_found;
	CString st = *sValue;
	if (icf[i].NameToTech.find(st) == icf[i].NameToTech.end()) {
		WriteLog(5, "Unknown name. Please first initialize technique name in instrument config: " + *sName + " = " + *sValue);
		return;
	}
	int id = icf[i].NameToTech[st];
	for (auto const& it : icf[i].tech[id]) {
		SaveInitCommand(it, i);
	}
}

void CGVar::LoadMapPitch(CString *sName, CString *sValue, CString sSearch, int i) {
	if (*sName != sSearch) return;
	++parameter_found;
	CString st = *sValue;
	vector<CString> sa;
	Tokenize(st, sa, ",");
	if (sa.size() != 2 && sa.size() != 3) {
		WriteLog(5, "Wrong format in instrument config: " + *sName + " = " + *sValue);
		return;
	}
	sa[0].Trim();
	sa[1].Trim();
	int n1 = GetNoteI(sa[0]);
	int n2 = GetNoteI(sa[1]);
	if (sa.size() == 3) {
		sa[2].Trim();
		int n3 = GetNoteI(sa[2]);
		icf[i].map_tremolo[n1] = n3;
	}
	icf[i].map_pitch[n1] = n2;
}

void CGVar::LoadInstrumentLine(CString st2, CString st3, int i) {
	LoadVar(&st2, &st3, "library", &icf[i].lib);
	CheckVar(&st2, &st3, "pan", &icf[i].pan, 0, 100);
	CheckVar(&st2, &st3, "volume", &icf[i].vol, 0, 100);
	CheckVar(&st2, &st3, "volume_default", &icf[i].vol_default, 0, 127);
	CheckVar(&st2, &st3, "ks1", &icf[i].ks1);
	LoadNote(&st2, &st3, "n_min", &icf[i].nmin);
	LoadNote(&st2, &st3, "n_max", &icf[i].nmax);
	LoadNote(&st2, &st3, "import_min", &icf[i].import_min);
	LoadNote(&st2, &st3, "import_max", &icf[i].import_max);
	LoadNote(&st2, &st3, "replacepitch", &icf[i].replace_pitch);
	LoadNote(&st2, &st3, "trem_replace", &icf[i].trem_replace);
	CheckVar(&st2, &st3, "trem_transpose", &icf[i].trem_transpose, -127, 127);
	CheckVar(&st2, &st3, "trem_chan", &icf[i].trem_chan, 0, 15);
	CheckVar(&st2, &st3, "bow_lock", &icf[i].bow_lock, 0, 2);
	LoadCCName(&st2, &st3, "cc_name", i);
	LoadKswGroup(&st2, &st3, "kswgroup", i);
	LoadTechnique(&st2, &st3, "technique", i);
	LoadInitCommand(&st2, &st3, "initcommand", i);
	LoadInitTechnique(&st2, &st3, "inittechnique", i);
	LoadMapPitch(&st2, &st3, "mappitch", i);
	CheckVar(&st2, &st3, "t_min", &icf[i].tmin);
	CheckVar(&st2, &st3, "t_max", &icf[i].tmax);
	CheckVar(&st2, &st3, "poly", &icf[i].poly);
	CheckVar(&st2, &st3, "single_stage", &icf[i].single_stage, 0, 1);
	CheckVar(&st2, &st3, "type", &icf[i].type);
	CheckVar(&st2, &st3, "channel", &icf[i].channel);
	CheckVar(&st2, &st3, "legato_long_minlen", &icf[i].legato_long_minlen);
	CheckVar(&st2, &st3, "vel_legato_long", &icf[i].vel_legato_long);
	CheckRange(&st2, &st3, "vib_bell_top", &icf[i].vib_bell_top1, &icf[i].vib_bell_top2);
	CheckRange(&st2, &st3, "vibf_bell_top", &icf[i].vibf_bell_top1, &icf[i].vibf_bell_top2);
	CheckRange(&st2, &st3, "vib_bell", &icf[i].vib_bell1, &icf[i].vib_bell2);
	CheckRange(&st2, &st3, "vibf_bell", &icf[i].vibf_bell1, &icf[i].vibf_bell2);
	CheckRange(&st2, &st3, "vib_bell_dur", &icf[i].vib_bell_mindur, &icf[i].vib_bell_dur);
	CheckRange(&st2, &st3, "dyn_range", &icf[i].dyn_range1, &icf[i].dyn_range2);
	CheckRange(&st2, &st3, "vel_range", &icf[i].vel_range1, &icf[i].vel_range2);
	CheckVar(&st2, &st3, "unison_mute", &icf[i].unis_mute, 0, 1);
	CheckVar(&st2, &st3, "unison_dyn_mul", &icf[i].unis_dyn_mul, 0);
	CheckVar(&st2, &st3, "vib_bell_freq", &icf[i].vib_bell_freq);
	CheckVar(&st2, &st3, "vib_bell_exp", &icf[i].vib_bell_exp);
	CheckVar(&st2, &st3, "vibf_bell_exp", &icf[i].vibf_bell_exp);
	CheckVar(&st2, &st3, "rnd_vel", &icf[i].rnd_vel);
	CheckVar(&st2, &st3, "rnd_vel_repeat", &icf[i].rnd_vel_repeat);
	CheckVar(&st2, &st3, "rnd_dyn", &icf[i].rnd_dyn);
	CheckVar(&st2, &st3, "rnd_vib", &icf[i].rnd_vib);
	CheckVar(&st2, &st3, "rnd_vibf", &icf[i].rnd_vibf);
	CheckVar(&st2, &st3, "splitpo_pent_minint", &icf[i].splitpo_pent_minint);
	CheckVar(&st2, &st3, "cc_vib", &icf[i].CC_vib);
	CheckVar(&st2, &st3, "cc_vibf", &icf[i].CC_vibf);
	CheckVar(&st2, &st3, "cc_steps", &icf[i].CC_steps);
	CheckVar(&st2, &st3, "cc_dynamics", &icf[i].CC_dyn);
	CheckVar(&st2, &st3, "cc_ma", &icf[i].CC_ma, 3);
	CheckVar(&st2, &st3, "cc_retrigger", &icf[i].CC_retrigger);
	CheckVar(&st2, &st3, "retrigger_freq", &icf[i].retrigger_freq);
	CheckVar(&st2, &st3, "max_slur_count", &icf[i].max_slur_count);
	CheckVar(&st2, &st3, "max_slur_interval", &icf[i].max_slur_interval);
	CheckVar(&st2, &st3, "nonlegato_maxgap", &icf[i].nonlegato_maxgap);
	CheckVar(&st2, &st3, "nonlegato_mingap", &icf[i].nonlegato_mingap);
	CheckVar(&st2, &st3, "retrigger_mingap", &icf[i].retrigger_mingap);
	CheckVar(&st2, &st3, "auto_legato", &icf[i].auto_legato);
	CheckVar(&st2, &st3, "fix_transpose", &icf[i].fix_transpose);
	CheckVar(&st2, &st3, "pedal_import", &icf[i].pedal_import);
	CheckVar(&st2, &st3, "trem_maxlen", &icf[i].trem_maxlen);
	CheckVar(&st2, &st3, "trem_dynamics", &icf[i].trem_dynamics);
	CheckVar(&st2, &st3, "trem_min_repeats", &icf[i].trem_min_repeats);
	CheckVar(&st2, &st3, "all_ahead", &icf[i].all_ahead);
	LoadVectorPar(&st2, &st3, "legato_ahead", icf[i].legato_ahead);
	LoadVectorPar(&st2, &st3, "ahead_chrom", icf[i].ahead_chrom);
	CheckVar(&st2, &st3, "leg_pdur", &icf[i].leg_pdur);
	CheckVar(&st2, &st3, "leg_cdur", &icf[i].leg_cdur);
	CheckVar(&st2, &st3, "legato_ahead_exp", &icf[i].legato_ahead_exp);
	CheckVar(&st2, &st3, "splitpo_freq", &icf[i].splitpo_freq);
	CheckVar(&st2, &st3, "splitpo_mindur", &icf[i].splitpo_mindur);
	CheckVar(&st2, &st3, "gliss_mindur", &icf[i].gliss_mindur);
	CheckVar(&st2, &st3, "nonlegato_minlen", &icf[i].nonlegato_minlen);
	CheckVar(&st2, &st3, "stac_auto", &icf[i].stac_auto, 0, 1);
	CheckVar(&st2, &st3, "stac_ahead", &icf[i].stac_ahead);
	CheckVar(&st2, &st3, "stac_maxlen", &icf[i].stac_maxlen);
	CheckVar(&st2, &st3, "stac_dynamics", &icf[i].stac_dynamics, 0, 200);
	CheckVar(&st2, &st3, "stac_dyn_add", &icf[i].stac_dyn_add, 0, 127);
	CheckVar(&st2, &st3, "pizz_import", &icf[i].pizz_import, 0, 1);
	CheckVar(&st2, &st3, "pizz_ahead", &icf[i].pizz_ahead);
	CheckVar(&st2, &st3, "mute_lock", &icf[i].mute_lock, 0, 1);
	CheckVar(&st2, &st3, "mute_import", &icf[i].mute_import, 0, 1);
	CheckVar(&st2, &st3, "trem_import", &icf[i].trem_import, 0, 1);
	CheckVar(&st2, &st3, "spic_import", &icf[i].spic_import, 0, 1);
	CheckVar(&st2, &st3, "stac_import", &icf[i].stac_import, 0, 1);
	CheckVar(&st2, &st3, "marc_import", &icf[i].marc_import, 0, 1);
	CheckVar(&st2, &st3, "tasto_import", &icf[i].tasto_import, 0, 1);
	CheckVar(&st2, &st3, "pizz_dynamics", &icf[i].pizz_dynamics, 0, 200);
	CheckVar(&st2, &st3, "pizz_dyn_add", &icf[i].pizz_dyn_add, 0, 127);
	CheckVar(&st2, &st3, "mute_predelay", &icf[i].pizz_dyn_add);
	CheckVar(&st2, &st3, "nonlegato_freq", &icf[i].nonlegato_freq);
	CheckVar(&st2, &st3, "lengroup2", &icf[i].lengroup2);
	CheckVar(&st2, &st3, "lengroup3", &icf[i].lengroup3);
	CheckVar(&st2, &st3, "lengroup4", &icf[i].lengroup4);
	CheckVar(&st2, &st3, "lengroup_edt1", &icf[i].lengroup_edt1);
	CheckVar(&st2, &st3, "lengroup_edt2", &icf[i].lengroup_edt2);
	CheckRange(&st2, &st3, "rand_pos", &icf[i].rand_start, &icf[i].rand_end);
	CheckRange(&st2, &st3, "rand_pos_max", &icf[i].rand_start_max, &icf[i].rand_end_max);
	CheckVar(&st2, &st3, "retrigger_min_len", &icf[i].retrigger_min_len);
	CheckVar(&st2, &st3, "retrigger_rand_end", &icf[i].retrigger_rand_end);
	CheckVar(&st2, &st3, "retrigger_rand_max", &icf[i].retrigger_rand_max);
	CheckVar(&st2, &st3, "harsh_freq", &icf[i].harsh_freq);
	CheckVar(&st2, &st3, "vel_harsh", &icf[i].vel_harsh);
	CheckVar(&st2, &st3, "vel_immediate", &icf[i].vel_immediate);
	CheckVar(&st2, &st3, "vel_normal", &icf[i].vel_normal);
	CheckVar(&st2, &st3, "vel_gliss", &icf[i].vel_gliss);
	CheckVar(&st2, &st3, "vel_normal_minlen", &icf[i].vel_normal_minlen);
	CheckVar(&st2, &st3, "gliss_minlen", &icf[i].gliss_minlen);
	CheckRange(&st2, &st3, "bell_mindur", &icf[i].bell_mindur, &icf[i].bell_mindur2);
	CheckVar(&st2, &st3, "gliss_freq", &icf[i].gliss_freq);
	CheckVar(&st2, &st3, "max_ahead_note", &icf[i].max_ahead_note);
	CheckRange(&st2, &st3, "bell_mul", &icf[i].bell_start_mul, &icf[i].bell_end_mul);
	CheckRange(&st2, &st3, "bell_len", &icf[i].bell_start_len, &icf[i].bell_end_len);
	CheckRange(&st2, &st3, "bell_vel", &icf[i].bell_start_vel, &icf[i].bell_end_vel);
	CheckVar(&st2, &st3, "rbell_freq", &icf[i].rbell_freq);
	CheckRange(&st2, &st3, "rbell_pos", &icf[i].rbell_pos1, &icf[i].rbell_pos2);
	CheckRange(&st2, &st3, "rbell_dur", &icf[i].rbell_mindur, &icf[i].rbell_dur);
	CheckRange(&st2, &st3, "rbell_mul", &icf[i].rbell_mul, &icf[i].rbell_mul2);
	CheckVar(&st2, &st3, "end_sfl_dur", &icf[i].end_sfl_dur);
	CheckVar(&st2, &st3, "end_sfl_freq", &icf[i].end_sfl_freq);
	CheckVar(&st2, &st3, "end_pbd_dur", &icf[i].end_pbd_dur);
	CheckVar(&st2, &st3, "end_pbd_freq", &icf[i].end_pbd_freq);
	CheckVar(&st2, &st3, "end_vib2_dur", &icf[i].end_vib2_dur);
	CheckVar(&st2, &st3, "end_vib2_freq", &icf[i].end_vib2_freq);
	CheckVar(&st2, &st3, "end_vib_dur", &icf[i].end_vib_dur);
	CheckVar(&st2, &st3, "end_vib_freq", &icf[i].end_vib_freq);
	if (st2 == "mute_activate") {
		++parameter_found;
		if (icf[i].NameToTech.find(st3) != icf[i].NameToTech.end()) {
			icf[i].mute_activate = icf[i].NameToTech[st3];
		}
		else WriteLog(5, "Unknown technique specified for mute_selected: " + st3);
	}
	if (st2 == "mute_deactivate") {
		++parameter_found;
		if (icf[i].NameToTech.find(st3) != icf[i].NameToTech.end()) {
			icf[i].mute_deactivate = icf[i].NameToTech[st3];
		}
		else WriteLog(5, "Unknown technique specified for mute_selected: " + st3);
	}
	if (st2 == "trem_activate") {
		++parameter_found;
		if (icf[i].NameToTech.find(st3) != icf[i].NameToTech.end()) {
			icf[i].trem_activate = icf[i].NameToTech[st3];
		}
		else WriteLog(5, "Unknown technique specified for trem_selected: " + st3);
	}
	if (st2 == "trem_deactivate") {
		++parameter_found;
		if (icf[i].NameToTech.find(st3) != icf[i].NameToTech.end()) {
			icf[i].trem_deactivate = icf[i].NameToTech[st3];
		}
		else WriteLog(5, "Unknown technique specified for trem_selected: " + st3);
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

void CGVar::SaveVector2Color(ofstream & fs, vector< vector<DWORD> > &v2D, int i) {
	size_t bytes = 4;
	for (int v = 0; v < v_cnt; v++) {
		fs.write((char*)&(v2D[i][v]), bytes);
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

void CGVar::SaveVector(ofstream &fs, vector<DWORD> &v) {
	const char* pointer = reinterpret_cast<const char*>(&v[0]);
	size_t bytes = t_generated * sizeof(v[0]);
	fs.write(pointer, bytes);
}

void CGVar::SaveResults(CString dir, CString fname)
{
	long long time_start = CGLib::time();
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
			//SaveVector2ST(fs, comment, i);
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
		// Added in MGR version 1.9
		for (size_t i = 0; i < t_generated; i++) {
			SaveVector2C(fs, minor, i);
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
	st.Format("gen_start_time = %lld # Unix timestamp of generation start\n", gen_start_time);
	fs << st;
	st.Format("time_stopped = %lld # Unix timestamp of generation finish\n", time_stopped);
	fs << st;
	st.Format("duration = %.0f # Length of total playback in ms\n", etime[max(0, t_generated - 1)]);
	fs << st;
	st.Format("need_exit = %d\n", need_exit);
	fs << st;
	fs.close();
	// Save logs
	if (mutex_log.try_lock_for(chrono::milliseconds(500))) {
		fs.open(dir + "\\log-debug.log");
		for (int i = 0; i < logs[0].size(); i++) fs << logs[0][i] << "\n";
		for (int i = 0; i < log_buffer[0].size(); i++) fs << log_buffer[0][i] << "\n";
		fs.close();
		fs.open(dir + "\\log-warning.log");
		for (int i = 0; i < logs[1].size(); i++) fs << logs[1][i] << "\n";
		for (int i = 0; i < log_buffer[1].size(); i++) fs << log_buffer[1][i] << "\n";
		fs.close();
		fs.open(dir + "\\log-perf.log");
		for (int i = 0; i < logs[2].size(); i++) fs << logs[2][i] << "\n";
		for (int i = 0; i < log_buffer[2].size(); i++) fs << log_buffer[2][i] << "\n";
		fs.close();
		fs.open(dir + "\\log-algorithm.log");
		for (int i = 0; i < logs[3].size(); i++) fs << logs[3][i] << "\n";
		for (int i = 0; i < log_buffer[3].size(); i++) fs << log_buffer[3][i] << "\n";
		fs.close();
		fs.open(dir + "\\log-midi.log");
		for (int i = 0; i < logs[4].size(); i++) fs << logs[4][i] << "\n";
		for (int i = 0; i < log_buffer[4].size(); i++) fs << log_buffer[4][i] << "\n";
		fs.close();
		fs.open(dir + "\\log-confirm.log");
		for (int i = 0; i < logs[6].size(); i++) fs << logs[6][i] << "\n";
		for (int i = 0; i < log_buffer[6].size(); i++) fs << log_buffer[6][i] << "\n";
		fs.close();
		fs.open(dir + "\\log-sasemu.log");
		for (int i = 0; i < logs[7].size(); i++) fs << logs[7][i] << "\n";
		for (int i = 0; i < log_buffer[7].size(); i++) fs << log_buffer[7][i] << "\n";
		fs.close();
		fs.open(dir + "\\log-corack.log");
		for (int i = 0; i < logs[8].size(); i++) fs << logs[8][i] << "\n";
		for (int i = 0; i < log_buffer[8].size(); i++) fs << log_buffer[8][i] << "\n";
		fs.close();
		mutex_log.unlock();
	}
	// Count time
	long long time_stop = CGLib::time();
	CString est;
	est.Format("Saved results to files in %lld ms", time_stop - time_start);
	WriteLog(0, est);
}

void CGVar::ExportVectorsCSV(CString dir, CString fname)
{
	long long time_start = CGLib::time();
	CString st;
	CreateDirectory(dir, NULL);
	ofstream fs;
	fs.open(dir + "\\" + fname + ".csv");
	fs << "Step;Tempo;Stime;Etime;";
	for (int v = 0; v < v_cnt; ++v) {
		st.Format("%d", v);
		fs << "Pause" + st + ";Note" + st + ";Len" + st + ";Dyn" + st +
			";Coff" + st + ";Poff" + st + ";Noff" + st + ";STick" + st + ";ETick" + st + ";Comment" + st + ";AComment" + st + ";Color" + st
			+ ";Lining" + st + ";Tonic" + st + ";Mode" + st + ";Dstime" + st + ";Detime" + st + ";Lengroup" + st + ";Articulation" + st +
			";Vib" + st + ";Vibf" + st + ";";
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
				fs << (int)smst[i][v] << ";";
				fs << (int)smet[i][v] << ";";
				st.Empty();
				for (int x = 0; x < comment[i][v].size(); ++x) st += comment[i][v][x];
				if (st.Left(1) == "\n") st = st.Right(st.GetLength() - 1);
				fs << "\"" << st << "\";";
				fs << adapt_comment[i][v] << ";";
				fs << color[i][v] << ";";
				fs << (int)lining[i][v] << ";";
				fs << (int)tonic[i][v] << ";";
				fs << (int)minor[i][v] << ";";
				fs << dstime[i][v] << ";";
				fs << detime[i][v] << ";";
				fs << (int)lengroup[i][v] << ";";
				fs << ArticName[artic[i][v]] << ";";
				fs << (int)vib[i][v] << ";";
				fs << (int)vibf[i][v] << ";";
			}
			fs << "\n";
		}
	}
	fs.close();
	// Count time
	long long time_stop = CGLib::time();
	CString est;
	est.Format("Saved CSV vectors to file %s\\%s.csv in %lld ms", dir, fname, time_stop - time_start);
	WriteLog(0, est);
}

void CGVar::LoadVector2C(ifstream& fs, vector< vector<unsigned char> > &v2D, int i) {
	v2D[i].resize(v_cnt);
	char* pointer = reinterpret_cast<char*>(&(v2D[i][0]));
	size_t bytes = v_cnt * sizeof(v2D[i][0]);
	fs.read(pointer, bytes);
	int read_count = fs.gcount();
	if (read_count != bytes && warning_loadvectors < MAX_WARN_LOADVECTORS) {
		CString est;
		est.Format("LoadVector2C: Error reading %d bytes from binary file (got %d bytes instead) at step %d", bytes, read_count, i);
		WriteLog(5, est);
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
		CString est;
		est.Format("LoadVector2S: Error reading %d bytes from binary file (got %d bytes instead) at step %d", bytes, read_count, i);
		WriteLog(5, est);
		warning_loadvectors++;
	}
}

void CGVar::LoadVector2Color(ifstream & fs, vector< vector<DWORD> > &v2D, int i) {
	size_t bytes = 4;
	for (int v = 0; v < v_cnt; v++) {
		fs.read((char*)&(v2D[i][v]), bytes);
		int read_count = fs.gcount();
		if (read_count != bytes && warning_loadvectors < MAX_WARN_LOADVECTORS) {
			CString est;
			est.Format("LoadVector2Color: Error reading %d bytes from binary file (got %d bytes instead) at step %d", bytes, read_count, i);
			WriteLog(5, est);
			warning_loadvectors++;
		}
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
			CString est;
			est.Format("LoadVector2ST len: Error reading %d bytes from binary file (got %d bytes instead) at step %d: %d", bytes, read_count, i, len);
			WriteLog(5, est);
			warning_loadvectors++;
		}
		if (len != 0) {
			bytes = len;
			fs.read((char*)&buf, bytes);
			v2D[i][v] = CString(buf, bytes);
			string st = v2D[i][v];
			int read_count = fs.gcount();
			if (read_count != bytes && warning_loadvectors < MAX_WARN_LOADVECTORS) {
				CString est;
				est.Format("LoadVector2ST: Error reading %d bytes from binary file (got %d bytes instead) at step %d: %s", bytes, read_count, i, v2D[i][v]);
				WriteLog(5, est);
				warning_loadvectors++;
			}
			if (st.find_first_not_of(" abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890`-=[]\\';/.,~!@#$%^&*()_+|{}:<>?1234567890") != string::npos && warning_loadvectors < MAX_WARN_LOADVECTORS) {
				CString est;
				est.Format("LoadVector2ST: String contains unprintable characters at step %d: %s", i, v2D[i][v]);
				WriteLog(5, est);
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
		CString est;
		est.Format("LoadVector float: Error reading %d bytes from binary file (got %d bytes instead)", bytes, read_count);
		WriteLog(5, est);
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
		CString est;
		est.Format("LoadVector uchar: Error reading %d bytes from binary file (got %d bytes instead)", bytes, read_count);
		WriteLog(5, est);
		warning_loadvectors++;
	}
}

void CGVar::LoadVector(ifstream &fs, vector<DWORD> &v) {
	v.clear();
	v.resize(t_generated);
	char* pointer = reinterpret_cast<char*>(&v[0]);
	size_t bytes = t_generated * sizeof(v[0]);
	fs.read(pointer, bytes);
	int read_count = fs.gcount();
	if (read_count != bytes && warning_loadvectors < MAX_WARN_LOADVECTORS) {
		CString est;
		est.Format("LoadVector Color: Error reading %d bytes from binary file (got %d bytes instead)", bytes, read_count);
		WriteLog(5, est);
		warning_loadvectors++;
	}
}

void CGVar::LoadResultMusic(CString dir, CString fname)
{
	long long time_start = CGLib::time();
	ifstream fs;
	CString path;
	// Load binary
	path = dir + "\\" + fname + ".mgr";
	if (!fileExists(path)) {
		CString est;
		est.Format("Cannot find file %s", path);
		WriteLog(5, est);
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
			//LoadVector2ST(fs, comment, i);
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
		// Added in version 1.9
		if (fs.peek() != EOF) {
			for (size_t i = 0; i < t_generated; i++) {
				LoadVector2C(fs, minor, i);
			}
		}
	}
	CountOff(0, t_generated - 1);
	CountTime(0, t_generated - 1);
	UpdateNoteMinMax(0, t_generated - 1);
	UpdateTempoMinMax(0, t_generated - 1);
	fs.close();
	// Count time
	long long time_stop = CGLib::time();
	CString est;
	est.Format("Loaded result music from folder %s in %lld ms", dir, time_stop - time_start);
	WriteLog(0, est);
	ValidateVectors(0, t_generated - 1);
}

void CGVar::ValidateVectors2(int step1, int step2) {
	long long time_start = CGLib::time();
	CString st;
	for (int i = step1; i <= step2; i++) {
		for (int v = 0; v < v_cnt; v++) {
			// Check vel is zero
			if (!vel[i][v] && !pause[i][v] && m_algo_id != 112 && m_algo_id != 121 && warning_valid < MAX_WARN_VALID) {
				st.Format("Validation failed at step %d voice %d: velocity cannot be zero", i, v);
				WriteLog(5, st);
				warning_valid++;
			}
			// Check vel is above 127
			if (vel[i][v] > 127 && !pause[i][v] && warning_valid < MAX_WARN_VALID) {
				st.Format("Validation failed at step %d voice %d: velocity cannot be above 127", i, v);
				WriteLog(5, st);
				warning_valid++;
			}
		}
	}
	// Count time
	if (debug_level > 1) {
		long long time_stop = CGLib::time();
		CString est;
		est.Format("Post-validated vectors steps %d-%d in %lld ms", step1, step2, time_stop - time_start);
		WriteLog(0, est);
	}
}

void CGVar::ValidateVectors(int step1, int step2) {
	long long time_start = CGLib::time();
	CString st;
	// Check first step
	if (!step1) for (int v = 0; v < v_cnt; v++) {
		if (coff[0][v] && warning_valid < MAX_WARN_VALID) {
			st.Format("Validation failed at step 0 voice %d: coff must be zero", v);
			WriteLog(5, st);
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
						WriteLog(5, st);
						warning_valid++;
					}
					if (note[i][v] != note[i - 1][v] && warning_valid < MAX_WARN_VALID) {
						st.Format("Validation failed at step %d voice %d: note change requires coff=0", i, v);
						WriteLog(5, st);
						warning_valid++;
					}
					if (len[i][v] != len[i - 1][v] && warning_valid < MAX_WARN_VALID) {
						st.Format("Validation failed at step %d voice %d: len change requires coff=0", i, v);
						WriteLog(5, st);
						warning_valid++;
					}
					if (coff[i][v] != coff[i - 1][v] + 1 && warning_valid < MAX_WARN_VALID) {
						st.Format("Validation failed at step %d voice %d: coff must increase by 1 each step inside note/pause", i, v);
						WriteLog(5, st);
						warning_valid++;
					}
					if (coff[i][v] >= len[i][v] && warning_valid < MAX_WARN_VALID) {
						st.Format("Validation failed at step %d voice %d: coff must be less than len", i, v);
						WriteLog(5, st);
						warning_valid++;
					}
					if (poff[i][v] != poff[i - 1][v] + 1 && i != coff[i][v] && warning_valid < MAX_WARN_VALID) {
						st.Format("Validation failed at step %d voice %d: poff must increase by 1 each step inside note/pause", i, v);
						WriteLog(5, st);
						warning_valid++;
					}
					if (noff[i][v] != noff[i - 1][v] - 1 && warning_valid < MAX_WARN_VALID) {
						st.Format("Validation failed at step %d voice %d: noff must decrease by 1 each step inside note/pause", i, v);
						WriteLog(5, st);
						warning_valid++;
					}
				}
				// Check poff and noff point at correct positions
				else {
					if (noff[i - 1][v] != 1 && warning_valid < MAX_WARN_VALID) {
						st.Format("Validation failed at step %d voice %d: noff must equal 1 one step before next note/pause", i, v);
						WriteLog(5, st);
						warning_valid++;
					}
					if (poff[i][v] != len[i-1][v] && warning_valid < MAX_WARN_VALID) {
						st.Format("Validation failed at step %d voice %d: poff must equal len of previous note at first step of note/pause", i, v);
						WriteLog(5, st);
						warning_valid++;
					}
				}
			}
			// Check len is not zero
			if (!len[i][v] && warning_valid < MAX_WARN_VALID) {
				st.Format("Validation failed at step %d voice %d: len cannot be zero", i, v);
				WriteLog(5, st);
				warning_valid++;
			}
			// Check key is correct
			if ((tonic[i][v]<0 || tonic[i][v]>11) && warning_valid < MAX_WARN_VALID) {
				st.Format("Validation failed at step %d voice %d: tonic must be in range 0-11", i, v);
				WriteLog(5, st);
				warning_valid++;
			}
			// Check mode is correct
			if ((minor[i][v]<0 || minor[i][v]>1) && warning_valid < MAX_WARN_VALID) {
				st.Format("Validation failed at step %d voice %d: minor must be in range 0-1", i, v);
				WriteLog(5, st);
				warning_valid++;
			}
			// Check sstime is not zero
			if (!sstime[i][v] && i > 0 && warning_valid < MAX_WARN_VALID) {
				st.Format("Validation failed at step %d voice %d: sstime can be zero only at first step", i, v);
				WriteLog(5, st);
				warning_valid++;
			}
			// Check setime is not zero
			if (!setime[i][v] && i > 0 && warning_valid < MAX_WARN_VALID) {
				st.Format("Validation failed at step %d voice %d: setime cannot be zero", i, v);
				WriteLog(5, st);
				warning_valid++;
			}
			/*
			// Do not check because sstime has meaning only for first step of note, setime has meaning only for last step of note
			// Comparing sstime and setime of the same step does not mean anything
			// Check setime is greater than sstime
			if (setime[i][v] <= sstime[i][v] && warning_valid < MAX_WARN_VALID) {
				st.Format("Validation failed at step %d voice %d: setime must be greater than sstime", i, v);
				WriteLog(5, st);
				warning_valid++;
			}
			*/
		}
		// Check tempo is not zero
		if (!tempo[i] && warning_valid < MAX_WARN_VALID) {
			st.Format("Validation failed at step %d: tempo cannot be zero", i);
			WriteLog(5, st);
			warning_valid++;
		}
		// Check stime is not zero
		if (!stime[i] && i > 0 && warning_valid < MAX_WARN_VALID) {
			st.Format("Validation failed at step %d: stime can be zero only at first step", i);
			WriteLog(5, st);
			warning_valid++;
		}
		// Check etime is not zero
		if (!etime[i] && i > 0 && warning_valid < MAX_WARN_VALID) {
			st.Format("Validation failed at step %d: etime cannot be zero", i);
			WriteLog(5, st);
			warning_valid++;
		}
		// Check etime is greater than stime
		if (etime[i] <= stime[i] && warning_valid < MAX_WARN_VALID) {
			st.Format("Validation failed at step %d: etime must be greater than stime", i);
			WriteLog(5, st);
			warning_valid++;
		}
	}
	// Count time
	if (debug_level > 1) {
		long long time_stop = CGLib::time();
		CString est;
		est.Format("Validated vectors steps %d-%d in %lld ms", step1, step2, time_stop - time_start);
		WriteLog(0, est);
	}
}

void CGVar::LoadResultLogs(CString dir, CString fname)
{
	ifstream fs;
	CString st, path;
	int pos, i;
	char pch[2550];
	// Load logs
	path = dir + "\\log-algorithm.log";
	if (!fileExists(path)) {
		CString est;
		est.Format("Cannot find file %s", path);
		WriteLog(5, est);
		return;
	}
	fs.open(path);
	pos = 0;
	i = 0;
	while (fs.good()) {
		fs.getline(pch, 2550);
		st = pch;
		if (!st.IsEmpty()) WriteLog(3, st);
		if (++i > MAX_LOAD_LOG) break;
	}
	fs.close();
	// Load logs
	path = dir + "\\log-debug.log";
	if (!fileExists(path)) {
		CString est;
		est.Format("Cannot find file %s", path);
		WriteLog(5, est);
		return;
	}
	fs.open(path);
	pos = 0;
	i = 0;
	while (fs.good()) {
		fs.getline(pch, 2550);
		st = pch;
		if (!st.IsEmpty()) WriteLog(0, st);
		if (++i > MAX_LOAD_LOG) break;
	}
	fs.close();
	// Load logs
	path = dir + "\\log-warning.log";
	if (!fileExists(path)) {
		CString est;
		est.Format("Cannot find file %s", path);
		WriteLog(5, est);
		return;
	}
	fs.open(path);
	pos = 0;
	i = 0;
	while (fs.good()) {
		fs.getline(pch, 2550);
		st = pch;
		if (!st.IsEmpty()) WriteLog(1, st);
		if (++i > MAX_LOAD_LOG) break;
	}
	fs.close();
}

void CGVar::LoadResults(CString dir, CString fname)
{
	long long time_start = CGLib::time();
	ifstream fs;
	CString st, st2, st3, path;
	int pos;
	char pch[2550];
	// Load strings
	path = dir + "\\" + fname + ".txt";
	if (!fileExists(path)) {
		CString est;
		est.Format("Cannot find file %s", path);
		WriteLog(5, est);
		return;
	}
	fs.open(path);
	pos = 0;
	int itemp;
	float ftemp;
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
			parameter_found = 0;
			CheckVar(&st2, &st3, "ng_min", &itemp);
			CheckVar(&st2, &st3, "ng_max", &itemp);
			CheckVar(&st2, &st3, "tg_min", &ftemp);
			CheckVar(&st2, &st3, "tg_max", &ftemp);
			CheckVar(&st2, &st3, "t_cnt", &t_cnt);
			CheckVar(&st2, &st3, "m_algo_id", &m_algo_id);
			CheckVar(&st2, &st3, "v_cnt", &v_cnt);
			CheckVar(&st2, &st3, "t_generated", &t_generated);
			CheckVar(&st2, &st3, "t_sent", &t_sent);
			CheckVar(&st2, &st3, "t_send", &t_send);
			CheckVar(&st2, &st3, "need_exit", &need_exit);
			LoadVar(&st2, &st3, "gen_start_time", &gen_start_time);
			LoadVar(&st2, &st3, "time_stopped", &time_stopped);
			LoadVar(&st2, &st3, "m_config", &m_config);
			LoadVar(&st2, &st3, "save_format_version", &save_format_version);
			if (!parameter_found) {
				WriteLog(5, "Unrecognized parameter '" + st2 + "' = '" + st3 + "' in file " + path);
			}
		}
	}
	fs.close();
	// Allocate
	t_allocated = t_generated;
	InitVectors();
	// Count time
	long long time_stop = CGLib::time();
	CString est;
	est.Format("Loaded result info from folder %s in %lld ms", dir, time_stop - time_start);
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
					if (real_len != len[i-1][v]) {
						for (int x = i - real_len; x < i; ++x) {
							if (x >= 0) len[x][v] = real_len;
						}
					}
				}
				real_len = 1;
			}
		}
	}
}

void CGVar::CountTime(int step1, int step2) {
	for (int i = step1; i <= step2; i++) {
		if (i > 0) stime[i] = stime[i - 1] + 30000.0 / tempo[i - 1];
		else stime[i] = 0;
		etime[i] = stime[i] + 30000.0 / tempo[i];
	}
}

void CGVar::CountSTime(int step1, int step2) {
	for (int v = 0; v < v_cnt; v++) {
		for (int i = step1; i <= step2; i++) {
			if (!sstime[i][v]) sstime[i][v] = stime[i];
			if (!setime[i][v]) setime[i][v] = etime[i];
		}
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

void CGVar::UpdateNoteMinMax(int step1, int step2, int final_run)
{
	for (int i = step1; i <= step2; i++) {
		for (int v = 0; v < v_cnt; v++) if ((pause[i][v] == 0) && (note[i][v] != 0)) {
			// Global minimax includes show_transpose, because it is used only for visualization
			if (ng_min > note[i][v] + show_transpose[v]) ng_min = note[i][v] + show_transpose[v];
			if (ng_max < note[i][v] + show_transpose[v]) ng_max = note[i][v] + show_transpose[v];
			// Calculate range including note scan range
			if (ng_min2 > note[i][v] + show_transpose[v]) ng_min2 = note[i][v] + show_transpose[v];
			if (ng_max2 < note[i][v] + show_transpose[v]) ng_max2 = note[i][v] + show_transpose[v];
			if (nsr1[i][v] && ng_min2 > nsr1[i][v] + show_transpose[v]) ng_min2 = nsr1[i][v] + show_transpose[v];
			if (nsr2[i][v] && ng_max2 < nsr2[i][v] + show_transpose[v]) ng_max2 = nsr2[i][v] + show_transpose[v];
			// Voice minimax does not include show_transpose, because it is used for Adaptation
			if (ngv_min[v] > note[i][v]) ngv_min[v] = note[i][v];
			if (ngv_max[v] < note[i][v]) ngv_max[v] = note[i][v];
			if (final_run) {
				// Count note frequency
				++nfreq[note[i][v] + show_transpose[v]];
				// Count graph maximum
				for (int n = 0; n < graph_size; ++n) {
					if (graph_max[n] < graph[i][v][n]) graph_max[n] = graph[i][v][n];
				}
			}
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

void CGVar::AddNote(int pos, int v, char note2, int len2, int dyn2) {
	if (pos + len2 >= t_allocated) ResizeVectors(max(pos + len2 + 1, t_allocated * 2));
	for (int i = 0; i < len2; i++) {
		note[pos + i][v] = note2;
		len[pos + i][v] = len2;
		coff[pos + i][v] = i;
		dyn[pos + i][v] = dyn2;
		pause[pos + i][v] = 0;
	}
}

// Fill pause from start step to (start+length) step inclusive
void CGVar::FillPause(int start, int length, int v) {
	if (start + length >= t_allocated) ResizeVectors(max(start + length + 1, t_allocated * 2));
	for (int x = start; x <= start + length; ++x) {
		pause[x][v] = 1;
		note[x][v] = 0;
		len[x][v] = 1;
		coff[x][v] = 0;
		vel[x][v] = 0;
		tonic[x][v] = tonic_cur;
		minor[x][v] = minor_cur;
		midifile_out_mul[x] = midifile_out_mul0 * midifile_out_mul2;
		comment[x][v].clear();
		comment2[x][v].Empty();
	}
}

// Adds new graph, 
void CGVar::RegisterGraph(CString name, float scale) {
	graph_name.push_back(name);
	graph_scale.push_back(scale);
	graph_max.push_back(0);
	++graph_size;
}

