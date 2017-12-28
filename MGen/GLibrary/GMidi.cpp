// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../stdafx.h"
#include "GMidi.h"
#include "SmRnd.h"

#include "../midifile/MidiFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW 
#endif

CGMidi::CGMidi() {
	vtype_sev.resize(MAX_VIZ);
	mo = 0;
	//BuildKeyMatrix();
}

// One-time function
void CGMidi::BuildKeyMatrix() {
	CString st, st2;
	vector<int> v;
	for (int k = 0; k < 12; ++k) {
		v.clear();
		v.resize(12, -1);
		int d0 = CC_C(key_base[k], 0, 0);
		st2.Empty();
		for (int n = 0; n < 7; ++n) {
			int d = (d0 + n) % 7;
			int c0 = (C_CC(d, 0, 0) + 12) % 12;
			int c = (C_CC(n, k, 0) + 12) % 12;
			v[c] = c0;
			st.Format("%d, ", d);
			st2 += st;
		}
		//AppendLineToFile("log\\key_build.log", "  { " + st2 + "}, \n");
		st2.Empty();
		for (int i = 0; i < 12; ++i) {
			st.Format("%d, ", v[i]);
			st2 += st;
		}
		//AppendLineToFile("log\\key_build.log", "  { " + st2 + "}, \n");
	}
	for (int k = 0; k < 12; ++k) {
		v.clear();
		v.resize(12, -1);
		int d0 = CC_C(key_base_m[k], 0, 0);
		st2.Empty();
		for (int n = 0; n < 7; ++n) {
			int d = (d0 + n) % 7;
			int c0 = (C_CC(d, 0, 0) + 12) % 12;
			int c = (C_CC(n, k, 1) + 12) % 12;
			v[c] = c0;
			st.Format("%d, ", d);
			st2 += st;
		}
		//AppendLineToFile("log\\key_build.log", "  { " + st2 + "}, \n");
		st2.Empty();
		for (int i = 0; i < 12; ++i) {
			st.Format("%d, ", v[i]);
			st2 += st;
		}
		AppendLineToFile("log\\key_build.log", "  { " + st2 + "}, \n");
	}
}

CGMidi::~CGMidi()
{
	StopMIDI();
}

void CGMidi::GetLyRange(int step1, int step2, vector<int> &vm_min, vector<int> &vm_max) {
	vm_min.clear();
	vm_max.clear();
	vm_min.resize(v_cnt, 128);
	vm_max.resize(v_cnt, 0);
	for (int s = step1; s < step2; ++s) {
		for (int v = v_cnt - 1; v >= 0; --v) {
			if (!pause[s][v]) {
				if (vm_min[v] > note[s][v]) vm_min[v] = note[s][v];
				if (vm_max[v] < note[s][v]) vm_max[v] = note[s][v];
			}
		}
	}
}

int CGMidi::GetLyVcnt(int step1, int step2, vector<int> &vm_max) {
	int vm_cnt = 0;
	for (int v = v_cnt - 1; v >= 0; --v) {
		if (vm_max[v]) ++vm_cnt;
	}
	return vm_cnt;
}

CString CGMidi::GetLyAlter(int alter) {
	if (alter == -1) return "f";
	else if (alter == -2) return "ff";
	else if (alter == 1) return "s";
	else if (alter == 2) return "ss";
	return "";
}

CString CGMidi::GetLyAlterVisual(int alter) {
	if (alter > 2) alter -= 12;
	if (alter < -2) alter += 12;
	if (alter == -1) return "\\flat ";
	else if (alter == -2) return "\\doubleflat ";
	else if (alter == 1) return "\"#\"";
	else if (alter == 2) return "\\doublesharp ";
	return "";
}

CString CGMidi::GetLyNote(int i, int v) {
	int no2, oct, alter;
	GetRealNote(note[i][v], tonic[i][v], minor[i][v], no2, oct, alter);
	return LyNoteSharp[no2] + GetLyAlter(alter) + LyOctave[oct];
}

CString CGMidi::GetLyNoteVisual(int i, int v) {
	int no2, oct, alter;
	GetRealNote(note[i][v], tonic[i][v], minor[i][v], no2, oct, alter);
	return NoteName[no2] + GetLyAlterVisual(alter);
}

CString CGMidi::GetLyLen(int length) {
	if (length == 1) return "8";
	else if (length == 2) return "4";
	else if (length == 3) return "4.";
	else if (length == 4) return "2";
	else if (length == 5) return "2"; // This is wrong length
	else if (length == 6) return "2.";
	else if (length == 7) return "2..";
	else if (length == 8) return "1";
	//else if (length == 16) return "\\breve";
	//else if (length == 32) return "\\longa";
	else return "";
}

// Split note of length 5
void CGMidi::SplitLyNote5(int pos, vector<int> &la) {
	if (pos % 4 == 0) {
		la.push_back(4);
		la.push_back(1);
	}
	else if (pos % 4 == 1) {
		la.push_back(3);
		la.push_back(2);
	}
	else if (pos % 4 == 2) {
		la.push_back(2);
		la.push_back(3);
	}
	else if (pos % 4 == 3) {
		la.push_back(1);
		la.push_back(4);
	}
}

// Split note at first measure border
void CGMidi::SplitLyNoteMeasure(int pos, int le, vector<int> &la) {
	int left = 8 - (pos % 8);
	if (la[0] > left) {
		// Convert first part to second
		la[0] = la[0] - left;
		// Add first part
		vpush_front(la, left, 1);
	}
}

// Create la array of common lengths if note is too long for single note
void CGMidi::SplitLyNote(int pos, int le, vector<int> &la) {
	la.clear();
	if (le < 5 || le == 6 || le == 7 || le == 8) {
		la.push_back(le);
		return;
	}
	if (le == 5) {
		SplitLyNote5(pos, la);
	}
	// All other lengths starting from 9
	else {
		// Get first length
		if (pos % 8 == 3) {
			la.push_back(1);
			la.push_back(4);
			le -= 5;
			pos += 5;
		}
		else {
			la.push_back(8 - pos % 8);
			le -= 8 - pos % 8;
			pos += 8 - pos % 8;
		}
		// Create middle lengths
		while (le > 8) {
			la.push_back(8);
			le -= 8;
			pos += 8;
		}
		// Get last length
		if (le == 5) {
			la.push_back(4);
			la.push_back(1);
		}
		else {
			if (le) la.push_back(le);
		}
	}
}

void CGMidi::GetLySev(ofstream &fs, int pos, CString &ev, int le, int i, int v) {
	fill(vtype_sev.begin(), vtype_sev.end(), -1);
	if (!ly_fa.size()) return;
	if (!rule_viz.size()) return;
	if (!severity.size()) return;
	// Find worst flag
	for (int x = 0; x < ly_fa.size(); ++x) {
		int fl = ly_fa[x];
		vtype_sev[rule_viz[fl]] = max(vtype_sev[rule_viz[fl]], severity[fl]);
		if (rule_viz[fl] == vLines)
			vtype_sev[vLine] = max(vtype_sev[vLine], severity[fl]);
	}
}

void CGMidi::SendLyViz(ofstream &fs, int pos, CString &ev, int le, int i, int v, int phase) {
	// Show worst flag
	for (int x = 0; x < vtype_sev.size(); ++x) {
		int sev = vtype_sev[x];
		if (sev > -1 && x == vLine) {
			if (phase == 1) {
				fs << " \\override Glissando.color=#(rgb-color " 
					<< GetLyColor(flag_color[sev]) 
					<< ") ";
			}
			if (phase == 10)
				fs << " \\glissando ";
		}
	}
}

// Send note or pause
void CGMidi::SendLyEvent(ofstream &fs, int pos, CString ev, int le, int i, int v) {
	int mv;
	// Length array
	vector<int> la;
	SplitLyNote(pos, le, la);
	SplitLyNoteMeasure(pos, le, la);
	GetLySev(fs, pos, ev, le, i, v);
	SendLyViz(fs, pos, ev, le, i, v, 1);
	if (ev != 'r' && ly_flag_style == 1) {
		if (vtype_sev[vDefault] > -1) SendLyNoteColor(fs, flag_color[vtype_sev[vDefault]]);
	}
	for (int lc = 0; lc < la.size(); ++lc) {
		if (show_lining && ev != "r") {
			if (la[lc] == 8) {
				if (lining[i][v] == HatchStyleNarrowHorizontal) fs << " \\speakOff \\override NoteHead.style = #'xcircle ";
				else if (lining[i][v] == HatchStyleLargeConfetti) fs << " \\speakOff \\override NoteHead.style = #'petrucci ";
				else fs << " \\speakOff \\revert NoteHead.style ";
			}
			else {
				if (lining[i][v] == HatchStyleNarrowHorizontal) fs << " \\revert NoteHead.style \\speakOn ";
				else if (lining[i][v] == HatchStyleLargeConfetti) fs << " \\speakOff \\override NoteHead.style = #'petrucci ";
				else fs << " \\speakOff \\revert NoteHead.style ";
			}
			if (lining[i][v] == HatchStyleLightUpwardDiagonal) fs << " \\circle ";
		}
		fs << ev + GetLyLen(la[lc]);
		if (lc < la.size() - 1 && ev != "r") fs << "~";
		fs << "\n";
		mv = v;
		if (vm_cnt > 1) mv = (v / 2) * 2 + !(v % 2);
		if (midifile_export_marks && !mark[i][v].IsEmpty()) {
			CString st = mark[i][v];
			st.Replace("\n", "");
			if (st == "PD" || st == "CA" || st == "DN") {
				if (!ly_msh) continue;
				if (GetGreen(mark_color[i][v]) == GetRed(mark_color[i][v])) {
					fs << " \\staccato ";
				}
				else {
					fs << "  \\staccatissimo ";
				}
			}
		}
		if (midifile_export_marks && !mark[i][mv].IsEmpty()) {
			// Search for conflicting harmonies
			CString st = mark[i][mv];
			st.Replace("\n", "");
			if (st != "PD" && st != "CA" && st != "DN") {
				fs << "_\\markup{ ";
				int found = 0;
				// Replace dominant symbol
				st.Replace("#", " \"#\" ");
				if (st[0] == 'D') {
					st = "\\concat { \\char ##x00D0 " + st.Right(st.GetLength() - 1) + " } ";
				}
				else if (st[0] == 'd') {
					st = "\\concat { \\char ##x0111 " + st.Right(st.GetLength() - 1) + " } ";
				}
				else st = "\\concat { " + st + " } ";
				st.Replace("6", " \\raise #0.7 6");
				//if (found) st = ", " + st;
				found = 1;
				fs << "\\teeny \\on-color #(rgb-color ";
				fs << GetLyMarkColor(mark_color[i][mv]);
				fs << ") \\pad-markup #0.4 " << st << " ";
				fs << "}\n";
			}
		}
		if (!lc && ev != 'r' && ly_flag_style == 2) {
			SendLyFlagColor(fs, i, v);
		}
		if (i > -1) i += la[lc] / midifile_out_mul[i];
		SendLyViz(fs, pos, ev, le, i, v, 10);
	}
}

CString CGMidi::GetLyColor(DWORD col) {
	float coef = 1.5;
	if (col == color_noflag) return "0 0 0";
	CString st;
	if (GetGreen(col) == GetRed(col) && GetRed(col) == GetBlue(col)) coef = 1;
	st.Format("%.3f %.3f %.3f",
		GetRed(col) / 255.0, GetGreen(col) / coef / 255.0, GetBlue(col) / 255.0);
	return st;
}

CString CGMidi::GetLyMarkColor(DWORD col) {
	if (col == color_noflag) return "1 1 1";
	CString st;
	if (GetGreen(col) == GetRed(col) && GetRed(col) == GetBlue(col)) return "1 1 1";
	st.Format("%.3f %.3f %.3f",
		Lighten(GetRed(col), 4) / 255.0,
		Lighten(GetGreen(col) * 1.5, 4) / 255.0,
		Lighten(GetBlue(col), 4) / 255.0);
	return st;
}

CString CGMidi::GetLyMarkColor2(DWORD col) {
	if (col == color_noflag) return "1 1 1";
	CString st;
	if (GetGreen(col) == GetRed(col) && GetRed(col) == GetBlue(col)) return "1 1 1";
	st.Format("%.3f %.3f %.3f",
		Lighten(GetRed(col), 4) / 255.0, 
		Lighten(GetGreen(col), 4) / 255.0, 
		Lighten(GetBlue(col), 4) / 255.0);
	return st;
}

void CGMidi::SendLyNoteColor(ofstream &fs, DWORD col) {
	fs << "\n    \\override NoteHead.color = #(rgb-color " << GetLyColor(col) << ") ";
	fs << "\n    \\override Stem.color = #(rgb-color " << GetLyColor(col) << ") ";
}

CString CGMidi::GetIntName(int iv) {
	if (iv == 0) return "0";
	else if (iv == 1) return "m2";
	else if (iv == 2) return "M2";
	else if (iv == 3) return "m3";
	else if (iv == 4) return "M3";
	else if (iv == 5) return "4";
	else if (iv == 6) return "tr";
	else if (iv == 7) return "5";
	else if (iv == 8) return "m6";
	else if (iv == 9) return "M6";
	else if (iv == 10) return "m7";
	else if (iv == 11) return "M7";
	else return "8";
}

void CGMidi::SendLyFlagColor(ofstream &fs, int i, int v) {
	if (interval[i][v] > -1) {
		CString st = GetIntName(interval[i][v]);
		fs << "^\\markup{ ";
		fs << "\\teeny ";
		if (vtype_sev[vDefault] > -1) {
			DWORD col = flag_color[vtype_sev[vDefault]];
			if (col && col != color_noflag)
				fs << " \\on-color #(rgb-color " << GetLyMarkColor2(col) << ") ";
		}
		fs << " \\pad-markup #0.4 " << st << " ";
		fs << "}\n";
	}
	else {
		if (vtype_sev[vDefault] == -1) return;
		DWORD col = flag_color[vtype_sev[vDefault]];
		if (!col || col == color_noflag) return;
		fs << "^\\markup{ \\tiny \\with-color #(rgb-color ";
		fs << GetLyColor(col);
		fs << ") \\char ##x2605  }\n";
	}
}

void CGMidi::ParseLyComments(int i, int v, int foreign) {
	CString st, com, note_st;
	int pos1, pos2, fl;
	ly_fa.clear();
	if (comment[i][v].size()) {
		for (int c = 0; c < comment[i][v].size(); ++c) {
			com = comment[i][v][c];
			// Do not show hidden rules
			if (com[0] == '$') continue;
			// Remove technical information
			pos1 = com.Find('[');
			pos2 = com.Find(']');
			if (pos1 != -1 && pos2 != -1) {
				fl = atoi(com.Mid(pos1 + 1, pos2 - pos1 - 1));
				if (foreign && rule_viz[fl] != vLines) continue;
				ly_fa.push_back(fl);
			}
		}
	}
}

void CGMidi::SaveLyComments(CString &com_st, int i, int v, int nnum, int pos) {
	CString st, com, note_st;
	int pos1, pos2, found;
	if (comment[i][v].size()) {
		note_st = "\\markup \\wordwrap \\bold {\n  ";
		// Show voice number if more than 1 voice
		if (vm_cnt > 1) {
			st.Format("\\char ##x246%d ", v);
			note_st += st;
		} 
		st.Format("NOTE %d at %d:%d - %s\n",
			nnum, pos / 8 + 1, pos % 8 + 1, GetLyNoteVisual(i, v));
		// NoteName[note[i][v] % 12]
		note_st += st + "\n}\n";
		found = 0;
		for (int c = 0; c < comment[i][v].size(); ++c) {
			com = comment[i][v][c];
			// Do not show hidden rules
			if (com[0] == '$') continue;
			// Remove technical information
			pos1 = com.Find('[');
			pos2 = com.Find(']');
			if (pos1 != -1 && pos2 != -1) {
				com = com.Left(pos1 - 1) + com.Right(com.GetLength() - pos2 - 1);
			}
			// Send note number with first comment
			if (!found) {
				found = 1;
				com_st += note_st;
			}
			com_st += "\\markup \\wordwrap \\with-color #(rgb-color " +
				GetLyColor(ccolor[i][v][c]) + ") {\n  ";
			com.Replace("#", "\"#\"");
			com_st += com + "\n";
			com_st += "\n}\n";
		}
	}
}

CString CGMidi::DetectLyClef(int vmin, int vmax) {
	vector<int> clef_penalty;
	int min_penalty = INT_MAX;
	int best_clef = 4;
	clef_penalty.resize(MAX_CLEF);
	// Calculate penalty
	for (int c = 0; c < MAX_CLEF; ++c) {
		clef_penalty[c] = max(abs(vmax - LyClefCenter[c]), abs(vmin - LyClefCenter[c]));
	}
	// Get best clef
	for (int c = 0; c < MAX_CLEF; ++c) {
		if (clef_penalty[c] < min_penalty) {
			min_penalty = clef_penalty[c];
			best_clef = c;
		}
	}
	return LyClef[best_clef];
}

void CGMidi::SaveLySegment(ofstream &fs, CString st, CString st2, int step1, int step2) {
	vector<CString> sv;
	CString comm_st, clef, key, key_visual;
	int pos, pos2, le, le2, nnum, pause_accum, pause_pos, pause_i;
	float mul;
	// Voice melody min pitch
	vector<int> vm_min;
	// Voice melody max pitch
	vector<int> vm_max;
	// Calculate stats
	GetLyRange(step1, step2, vm_min, vm_max);
	vm_cnt = GetLyVcnt(step1, step2, vm_max);
	mul = midifile_out_mul[step1];
	//if (vm_cnt == 1 && (m_algo_id == 121 || m_algo_id == 112)) mul = 8;
	// Key
	if (minor[step1][0]) {
		key = LyMinorKey[tonic[step1][0]];
	}
	else {
		key = LyMajorKey[tonic[step1][0]];
	}
	key_visual = key[0];
	key_visual.MakeUpper();
	if (key[1] == 'f') key_visual += "\\flat ";
	if (key[1] == 's') key_visual = "\"" + key_visual + "#\"";
	// First info
	st.Replace("\n", ", ");
	st.Replace("#", "\"#\"");
	st.Replace("\\", "/");
	st.Replace("=>", " \\char ##x27F9 ");
	st.Replace("->", " \\char ##x27F6 ");
	fs << "\\markup \\wordwrap \\bold {\n  ";
	fs << "    \\vspace #2\n";
	fs << st << ", Key: " << key_visual << (minor[step1][0]?"m":"") << "\n}\n";
	// Save notes
	fs << "<<\n";
	for (int v = v_cnt - 1; v >= 0; --v) {
		// Do not show voice if no notes inside
		if (!vm_max[v]) continue;
		// Select bass clef if melody goes mostly below middle C
		clef = DetectLyClef(vm_min[v], vm_max[v]);
		//if (60 - vm_min[v] > vm_max[v] - 60) clef = "bass";
		fs << "\\new Staff {\n";
		st.Format("  \\set Staff.instrumentName = #\"%d\"\n", v + 1); //InstGName[instr[v]]
		fs << st;
		fs << "  \\clef \"" << clef << "\" \\key " << key;
		fs << " \\" << (minor[step1][0] ? "minor" : "major");
		read_file_sv("configs\\ly\\staff.ly", sv);
		write_file_sv(fs, sv);
		fs << "  { ";
		nnum = 0;
		pause_accum = 0;
		pause_pos = -1;
		for (int i = step1; i < step2; i++) {
			++nnum;
			pos = mul * (i - step1);
			le = mul * len[i][v];
			if (pause[i][v]) {
				pause_accum += le;
				if (pause_pos == -1) {
					pause_pos = pos;
					pause_i = i;
				}
			}
			else {
				ParseLyComments(i, v, 0);
				SaveLyComments(comm_st, i, v, nnum, pos);
				// If no flags, parse second voice
				if (!ly_fa.size() && v_cnt > 1) {
					int v2 = (v / 2) * 2 + !(v % 2);
					int i2 = abs(i + len[i][v] - poff[i + len[i][v]][v2]);
					ParseLyComments(i2, v2, 1);
				}
				SendLyEvent(fs, pos, GetLyNote(i, v), le, i, v);
			}
			if (pause_accum && (i == step2 - 1 || !pause[i + 1][v])) {
				SendLyEvent(fs, pause_pos, "r", pause_accum, pause_i, v);
				pause_accum = 0;
				pause_pos = -1;
			}
			if (noff[i][v] == 0) break;
			i += noff[i][v] - 1;
		}
		// Finish with pause
		if ((pos + le) % 8) {
			SendLyEvent(fs, pos, "r", 8 - (pos + le) % 8, 0, 0);
		}
		fs << "\n  }\n";
		fs << "}\n";
	}
	fs << ">>\n";
	fs << comm_st;
	// Second info
	//st2.Replace("\n", "\n}\n\\markup \\wordwrap \\italic {\n  ");
	//st2.Replace("#", "\"#\"");
	//fs << "\\markup \\wordwrap \\italic {\n  \\vspace #2\n  " << st2 << "\n}\n";
}

void CGMidi::SaveLy(CString dir, CString fname) {
	ofstream fs;
	vector<CString> sv;
	CString title;
	title = m_algo_folder + ": " + m_config + " (" + 
		CTime::GetCurrentTime().Format("%Y-%m-%d %H:%M") + ")";
	fs.open(dir + "\\" + fname + ".ly");
	read_file_sv("configs\\ly\\header.ly", sv);
	for (int i = 0; i < sv.size(); ++i) {
		sv[i].Replace("$TITLE$", title);
		fs << sv[i] << "\n";
	}

	if (!mel_info.size()) {
		CString st;
		st = "Whole piece";
		SaveLySegment(fs, st, "", 0, t_generated);
	}
	else {
		int first_step = 0;
		int last_step = 0;
		int found, s;
		for (int m = 0; m < mel_info.size(); ++m) {
			found = 0;
			for (s = last_step; s < t_generated; ++s) {
				if (!found && mel_id[s][0] == m && mel_id[s][0] > -1) {
					first_step = s;
					last_step = s;
					found = 1;
				}
				if (found && mel_id[s][0] != m) {
					last_step = s;
					break;
				}
			}
			if (s >= t_generated - 1 && mel_id[t_generated - 1][0] > -1 && 
				found && first_step == last_step)	last_step = t_generated - 1;
			if (found) SaveLySegment(fs, mel_info[m], mel_info2[m], first_step, last_step);
			//if (m < mel_info.size() - 1) fs << "\\pageBreak\n";
		}
	}
	fs << "\\header {tagline = \"This file was created by MGen ";
	fs << APP_VERSION << " and engraved by LilyPond\"}\n";
	read_file_sv("configs\\ly\\footer.ly", sv);
	write_file_sv(fs, sv);
	fs.close();
	/*
	if (m_testing) {
		CreateDirectory("autotest\\ly", NULL);
		// This will rewrite file if exists (tested)
		copy_file(dir + "\\" + fname + ".ly", 
			"autotest\\ly\\" + m_algo_folder + "-" + m_config + ".ly");
		// Test run lilypond
		CString par =
			"-dgui --output autotest\\ly" 
			" autotest\\ly\\" + m_algo_folder + "-" + m_config + ".ly";
		Run("autotest\\LilyPond\\usr\\bin\\lilypond.exe", par, 0);
	}
	*/
}

void CGMidi::ExportAdaptedMidi(CString dir, CString fname) {
	long long time_start = CGLib::time();
	// If generation finished set last run
	midi_last_run = 1;
	StopMIDI();
	amidi_export = 1;
	midifile_buf.clear();
	midifile_buf.resize(MAX_STAGE);
	for (int i = 0; i < MAX_STAGE; ++i) {
		midifile_buf[i].resize(MAX_VOICE);
	}
	SendMIDI(midi_sent, t_sent);
	// Get maximum stage
	int stage_max = vmax(v_stage) + 1;
	vector <int> stracks_cnt;
	stracks_cnt.resize(MAX_STAGE);
	int tracks_cnt = 0;
	// Get maximum used track
	for (int sta = 0; sta < stage_max; ++sta) {
		for (int ch = MAX_VOICE - 1; ch >= 0; --ch) {
			if (midifile_buf[sta][ch].size()) {
				if (!stracks_cnt[sta]) stracks_cnt[sta] = ch + 1;
				++tracks_cnt;
			}
		}
	}
	if (!tracks_cnt) return;
	// Save to file
	MidiFile midifile;
	vector <MidiFile> smidifile;
	smidifile.resize(stage_max);
	float tps = 200;                // ticks per second
	float spq = 0.5;              // Seconds per quarter note
	int tpq = tps / spq;          // ticks per quarter note
	int track = 0;
	int strack = 0; // stage track
	int channel = 0;
	int tick = 0;
	int type, data1, data2;
	CString st2;
	// Add some expression track (track 0) messages:
	string st = fname;
	for (int sta = 0; sta < stage_max; ++sta) {
		smidifile[sta].setTicksPerQuarterNote(tpq);
		smidifile[sta].addTracks(stracks_cnt[sta] + 1);
		smidifile[sta].addTrackName(track, 0, st);
		smidifile[sta].addTempo(track, 0, 60 / spq);
	}
	midifile.addTracks(tracks_cnt + 1);
	midifile.setTicksPerQuarterNote(tpq);
	midifile.addTrackName(track, 0, st);
	midifile.addTempo(track, 0, 60 / spq);
	track = 0;
	for (int sta = 0; sta < stage_max; ++sta) {
		for (int ch = 0; ch < MAX_VOICE; ++ch) {
			if (!midifile_buf[sta][ch].size()) continue;
			// Convert channel to midi file channel and track number
			strack = ch + 1;
			++track;
			channel = ch;
			// Send instrument name
			CString st;
			if (sta == 0) st = InstGName[instr[ch]];
			else st.Format("%s %d", InstGName[instr[ch]], sta);
			string st2 = st;
			midifile.addTrackName(track, 0, st2);
			midifile.addPatchChange(track, 0, channel, 0); // 0=piano, 40=violin, 70=bassoon
			smidifile[sta].addPatchChange(strack, 0, channel, 0); // 0=piano, 40=violin, 70=bassoon
			for (int i = 0; i < midifile_buf[sta][ch].size(); i++) {
				tick = midifile_buf[sta][ch][i].timestamp / 1000.0 / spq * tpq;
				type = Pm_MessageStatus(midifile_buf[sta][ch][i].message) & 0xF0;
				data1 = Pm_MessageData1(midifile_buf[sta][ch][i].message);
				data2 = Pm_MessageData2(midifile_buf[sta][ch][i].message);
				if (type == MIDI_NOTEON) {
					if (data2) {
						midifile.addNoteOn(track, tick, channel, data1, data2);
						smidifile[sta].addNoteOn(strack, tick, channel, data1, data2);
					}
					else {
						midifile.addNoteOff(track, tick, channel, data1, 0);
						smidifile[sta].addNoteOff(strack, tick, channel, data1, 0);
					}
				}
				if (type == MIDI_CC) {
					midifile.addController(track, tick, channel, data1, data2);
					smidifile[sta].addController(strack, tick, channel, data1, data2);
				}
			}
		}
	}
	for (int sta = 0; sta < stage_max; ++sta) {
		smidifile[sta].sortTracks();         // ensure tick times are in correct order
		st2.Format(dir + "\\" + fname + "_%d.midi", sta);
		smidifile[sta].write(st2);
	}
	midifile.sortTracks();         // ensure tick times are in correct order
	midifile.write(dir + "\\" + fname + ".midi");

	amidi_export = 0;
	// Log
	long long time_stop = CGLib::time();
	CString est;
	est.Format("Exported adapted midi file %s in %lld ms",
		as_fname, time_stop - time_start);
	WriteLog(0, est);
}

void CGMidi::SaveMidi(CString dir, CString fname) {
	MidiFile midifile;
	midifile.addTracks(v_cnt);    // Add another two tracks to the MIDI file
	int tpq = 120;                // ticks per quarter note
	int tpc = 60; // ticks per croche
	midifile.setTicksPerQuarterNote(tpq);
	int track = 0;
	int channel = 0;
	int pos = tpq * 4;
	// Add some expression track (track 0) messages:
	string st = fname;
	midifile.addTrackName(track, 0, st);
	// Save tempo
	midifile.addTempo(track, 0, tempo[0]);
	for (int i = 0; i < t_generated; i++) {
		midifile.addTempo(track, pos, tempo[i] * midifile_out_mul[i]);
		pos += tpc * midifile_out_mul[i];
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
		//if (ngv_min[v] < 57) midifile.addPatchChange(track, 0, channel, bass_program[v]); // 0=piano, 40=violin, 70=bassoon
		midifile.addPatchChange(track, 0, channel, 0); // 0=piano, 40=violin, 70=bassoon
		pos = tpq * 4;
		for (int i = 0; i < t_generated; i++) {
			if (pause[i][v]) {
				pos += tpc * midifile_out_mul[i];
				continue;
			}
			midifile.addNoteOn(track, pos, channel, note[i][v], dyn[i][v]);
			midifile.addNoteOff(track, pos + tpc*midifile_out_mul[i]*(len[i][v]) - 1, channel, note[i][v], 0);
			if (midifile_export_comments && !comment2[i][v].IsEmpty()) {
				string st;
				st = comment2[i][v];
				midifile.addLyric(track, pos, st);
			}
			if (midifile_export_marks && !mark[i][v].IsEmpty()) {
				string st;
				st = mark[i][v];
				midifile.addLyric(track, pos, st);
			}
			if (noff[i][v] == 0) break;
			pos += tpc * midifile_out_mul[i] * noff[i][v];
			i += noff[i][v] - 1;
		}
	}
	midifile.sortTracks();         // ensure tick times are in correct order
	midifile.write(dir + "\\" + fname + ".mid");
}

void CGMidi::LoadMidi(CString path)
{
	long long time_start = CGLib::time();
	if (!fileExists(path)) {
		CString est;
		est.Format("Cannot find file %s", path);
		WriteLog(5, est);
		return;
	}
	if (!FileHasHeader(path, "MThd")) {
		CString est;
		est.Format("This file does not have MIDI header: %s", path);
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
	//MergeSmallOverlaps(0, last_step);
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
	long long time_stop = CGLib::time();
	CString est;
	est.Format("LoadMidi successfully loaded %d steps (in %lld ms)", 
		t_generated, time_stop - time_start);
	WriteLog(0, est);
}

void CGMidi::MergeSmallOverlaps(int step1, int step2) {
	// Merge small overlaps
	for (int i = step1; i <= step2; ++i) {
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
}

void CGMidi::LoadCantus(CString path)
{
	long long time_start = CGLib::time();
	if (!fileExists(path)) {
		CString est;
		est.Format("Cannot find file %s", path);
		WriteLog(5, est);
		return;
	}
	if (!FileHasHeader(path, "MThd")) {
		CString est;
		est.Format("This file does not have MIDI header: %s", path);
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
	int last_pos = -1;
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
						if (c.size()) {
							if (!incom[c.size() - 1].IsEmpty()) incom[c.size() - 1] += ",";
							incom[c.size() - 1] += st;
						}
						else {
							CString est;
							est.Format("Error assigning lyrics '%s' for already sent position %d", st, pos);
							WriteLog(5, est);
						}
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
					if (nid >= MIN_CANTUS_SIZE && !bad) {
						cantus.push_back(c);
						cantus_len.push_back(cl);
						cantus_tempo.push_back(ct);
						cantus_incom.push_back(incom);
						//lyrics_pending.Empty();
					}
					else {
						if (nid < MIN_CANTUS_SIZE && nid > 0) {
							CString st;
							st.Format("Melody #%d is shorter (%d steps) than minimum length (%d steps): tick %d, track %d, chan %d, tpc %d (mul %.03f) in file %s", 
								cantus.size(), nid, (int)MIN_CANTUS_SIZE, mev->tick, track, mev->getChannel(), tpc, midifile_in_mul, path);
							WriteLog(5, st);
						}
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
							WriteLog(5, st);
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
							WriteLog(5, st);
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
		else {
			if (nid < MIN_CANTUS_SIZE && nid > 0) {
				CString st;
				st.Format("Melody #%d is shorter (%d steps) than minimum length (%d steps): tick %.0f, track %d, tpc %d (mul %.03f) in file %s. Not loaded.", 
					cantus.size(), nid, (int)MIN_CANTUS_SIZE, last_tick, track, tpc, midifile_in_mul, path);
				WriteLog(5, st);
			}
		}
	}
	// Count time
	long long time_stop = CGLib::time();
	CString st;
	st.Format("LoadCantus successfully loaded %d canti (in %lld ms)", cid + 1, time_stop - time_start);
	WriteLog(0, st);
}

// Load counterpoint
void CGMidi::LoadCP(CString path)
{
	long long time_start = CGLib::time();
	if (!fileExists(path)) {
		CString est;
		est.Format("Cannot find file %s", path);
		WriteLog(5, est);
		return;
	}
	if (!FileHasHeader(path, "MThd")) {
		CString est;
		est.Format("This file does not have MIDI header: %s", path);
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
					if (inter.size() >= MIN_CP_SIZE && !bad) {
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
					else {
						if (inter.size() < MIN_CP_SIZE && inter.size() > 0) {
							CString st;
							st.Format("Counterpoint #%d is shorter (%d steps) than minimum length (%d steps): tick %d, track %d, chan %d, tpc %d (mul %.03f) in file %s", cpoint.size(), inter.size(), (int)MIN_CP_SIZE, mev->tick, track, mev->getChannel(), tpc, midifile_in_mul, path);
							WriteLog(5, st);
						}
					}
					// Go to next cantus
					nid = 0;
					hid = 0;
				}
				else if (pos2 - last_tick2 > (float)tpc / 2) {
					CString st;
					st.Format("Pauses are prohibited inside counterpoint: tick %d, track %d, chan %d, tpc %d (mul %.03f) in file %s. Cannot load.", mev->tick, track, mev->getChannel(), tpc, midifile_in_mul, path);
					WriteLog(5, st);
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
						WriteLog(5, st);
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
		if (inter.size() >= MIN_CP_SIZE && !bad) {
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
			cantus_incom.resize(cid);
		}
		else {
			if (inter.size() < MIN_CP_SIZE && inter.size() > 0) {
				CString st;
				st.Format("Counterpoint #%d is shorter (%d steps) than minimum length (%d steps): tick %.0f, track %d, tpc %d (mul %.03f) in file %s", cpoint.size(), inter.size(), (int)MIN_CP_SIZE, last_tick, track, tpc, midifile_in_mul, path);
				WriteLog(5, st);
			}
		}
	}
	// Load lyrics
	int dist, min_dist, my_c, my_x;
	cantus_incom.resize(cpoint.size());
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
						cantus_incom[my_c].resize(my_x + 1);
						cantus_incom[my_c][my_x] = st;
					}
				}
			}
		}
	}
	// Count time
	long long time_stop = CGLib::time();
	CString st;
	st.Format("LoadCP successfully loaded %d cp (in %lld ms)", cid + 1, time_stop - time_start);
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
	start_time();
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

void CGMidi::AddMidiEvent(long long timestamp, int mm_type, int data1, int data2)
{
	long long real_timestamp = timestamp + midi_start_time;
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
				est.Format("Postponed AddMidiEvent to %lld step %d, type %02X, data %d/%d (after %lld step %d, type %02X, data %d/%d) [start = %lld, lim = %lld]",
					timestamp, midi_current_step, mm_type, data1, data2, midi_sent_t - midi_start_time, midi_sent,
					Pm_MessageStatus(midi_sent_msg), Pm_MessageData1(midi_sent_msg), Pm_MessageData2(midi_sent_msg), midi_start_time, midi_buf_lim - midi_start_time);
				WriteLog(4, est);
			}
		}
		else {
			if (amidi_export) {
				midifile_buf[midi_stage][mm_type & 0xF].push_back(event);
			}
			else {
				midi_buf.push_back(event);
			}
			// Save maximum message and its time
			if (real_timestamp > midi_sent_t2) {
				midi_sent_t2 = real_timestamp;
				midi_sent_msg2 = event.message;
			}
		}
	}
	else {
		CString est;
		est.Format("Blocked AddMidiEvent to past %lld step %d, type %02X, data %d/%d (before %lld step %d, type %02X, data %d/%d) [start = %lld]",
			timestamp, midi_current_step, mm_type, data1, data2, midi_sent_t - midi_start_time, midi_sent,
			Pm_MessageStatus(midi_sent_msg), Pm_MessageData1(midi_sent_msg), Pm_MessageData2(midi_sent_msg), midi_start_time); // , midi_buf_lim - midi_start_time
		WriteLog(5, est);
	}
	// Debug log
	//CString st;
	//st.Format("%d: At %d type %d, data %d/%d blocked %d\n", CGLib::time(), timestamp, mm_type, data1, data2, midi_sent_t-midi_start_time);
	//AppendLineToFile("midi.log", st);
}

void CGMidi::AddTransitionKs(int i, long long stimestamp, int ks)
{
	int v = midi_voice;
	int pi = i - poff[i][v];
	int ei = i + len[i][v] - 1;
	AddKsOn(stimestamp - min(MAX_TRANS_DELAY,
		((stime[i] - stime[pi]) * 100 / m_pspeed + dstime[i][v] - dstime[pi][v]) / 10), ks, 10);
	AddKsOff(stimestamp + min(MAX_TRANS_DELAY,
		((etime[ei] - stime[i]) * 100 / m_pspeed + detime[ei][v] - dstime[i][v]) / 10), ks, 0);
}

void CGMidi::AddKs(long long stimestamp, int ks)
{
	AddKsOn(stimestamp, ks, 100);
	AddKsOff(stimestamp + 1, ks, 0);
}

void CGMidi::AddTransitionCC(int i, long long stimestamp, int CC, int value1, int value2)
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
	long long time_start = CGLib::time();
	long long timestamp_current = CGLib::time();
	// Note start timestamp
	long long stimestamp; 
	// Note end timestamp
	long long etimestamp; 
	// Check if this is first run
	if ((step1 == 0) || (!midi_sent_t) || (!midi_start_time)) midi_first_run = 1;
	else midi_first_run = 0;
	if (midi_first_run) LogInstruments();
	// Set real time when playback started
	if (!midi_start_time) midi_start_time = timestamp_current + MIDI_BUF_PROTECT - (long long)(stime[step1] / m_pspeed * 100);
	// Set real time when playback started
	if (!midi_sent_t) midi_sent_t = (long long)(stime[step1] / m_pspeed * 100) + midi_start_time - 100;
	// Check if we have buf underrun
	if (midi_sent_t < timestamp_current) {
		CString st;
		st.Format("SendMIDI got buf underrun in %lld ms (steps %d - %d)", 
			timestamp_current - midi_sent_t, step1, step2);
		WriteLog(5, st);
		buf_underrun = 1;
		return;
	}
	// Check if buf is full
	if (midi_sent_t - timestamp_current > MIN_MIDI_BUF_MSEC) {
		if (debug_level > 1) {
			CString st;
			st.Format("SendMIDI: no need to send (full buf = %lld ms) (steps %d - %d) playback is at %lld",
				midi_sent_t - timestamp_current, step1, step2, timestamp_current - midi_start_time);
			WriteLog(4, st);
		}
		return;
	}
	if (debug_level > 1) {
		CString est;
		est.Format("SendMIDI: need to send (full buf = %lld ms) (steps %d - %d) playback is at %lld",
			midi_sent_t - timestamp_current, step1, step2, timestamp_current - midi_start_time);
		WriteLog(4, est);
	}
	int i;
	if (!amidi_export) {
		if (!mutex_output.try_lock_for(chrono::milliseconds(3000))) {
			WriteLog(0, "SendMIDI mutex timed out: will try later");
		}
	}
	int step21 = 0; // Voice-dependent first step
	int step22 = 0; // Voice-independent last step
	float time = 0;
	// Find last step not too far
	for (i = step1; i <= step2; i++) {
		step22 = i;
		if (i == 0) time = stime[i] * 100 / m_pspeed;
		else time = etime[i - 1] * 100 / m_pspeed;
		if (!amidi_export)
			if ((long long)time + midi_start_time - timestamp_current > MAX_MIDI_BUF_MSEC) break;
	}
	// If we cut notes, this is not last run
	if (step22 < step2) midi_last_run = 0;
	// Send previous buffer if exists
	midi_buf.clear();
	// Calculate midi right limit
	midi_buf_lim = midi_start_time + (long long)(stime[step22] * 100.0 / m_pspeed);
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
		midi_stage = v_stage[v];
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
	long long time_stop = CGLib::time();
	CString st;
	st.Format("MIDI write %d (%d postponed) events: steps %d/%d - %d/%d (%lld to %lld ms) [to future %lld to %lld ms] (in %lld ms) playback is at %lld ms. Limit %lld. Last postponed %lld. Step22 stopped increasing at %.0f ms. Start time: %lld, current time: %lld",
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
	if (!amidi_export) {
		mutex_output.unlock();
	}
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
		long long searchElement = CGLib::time() - midi_start_time;
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

void CGMidi::AddNoteOn(long long timestamp, int data1, int data2)
{
	// Check if range valid
	if ((data1 < instr_nmin[instr[midi_voice]]) || (data1 > instr_nmax[instr[midi_voice]])) {
		if (warning_note_wrong[midi_voice] < 4) {
			CString st;
			st.Format("Blocked note %d/%d time %lld in voice %d instrument %d out of range %d-%d",
				data1, data2, timestamp, midi_voice, instr[midi_voice], 
				instr_nmin[instr[midi_voice]], instr_nmax[instr[midi_voice]]);
			WriteLog(1, st);
			warning_note_wrong[midi_voice] ++;
		}
		return;
	}
	AddMidiEvent(timestamp, MIDI_NOTEON + midi_channel, data1, data2);
}

void CGMidi::AddKsOn(long long timestamp, int data1, int data2)
{
	// Check if range valid
	if ((data1 >= instr_nmin[instr[midi_voice]]) && (data1 <= instr_nmax[instr[midi_voice]])) {
		if (warning_note_wrong[midi_voice] < 4) {
			CString st;
			st.Format("Blocked keyswitch %d/%d time %lld in voice %d instrument %d in note range %d-%d",
				data1, data2, timestamp, midi_voice, instr[midi_voice], 
				instr_nmin[instr[midi_voice]], instr_nmax[instr[midi_voice]]);
			WriteLog(5, st);
			warning_note_wrong[midi_voice] ++;
		}
		return;
	}
	AddMidiEvent(timestamp, MIDI_NOTEON + midi_channel, data1, data2);
}

void CGMidi::AddNoteOff(long long timestamp, int data1, int data2)
{
	// Check if range valid
	if ((data1 < instr_nmin[instr[midi_voice]]) || (data1 > instr_nmax[instr[midi_voice]])) {
		if (warning_note_wrong[midi_voice] < 4) {
			CString st;
			st.Format("Blocked note %d/%d time %lld in voice %d instrument %d out of range %d-%d",
				data1, data2, timestamp, midi_voice, instr[midi_voice], 
				instr_nmin[instr[midi_voice]], instr_nmax[instr[midi_voice]]);
			WriteLog(1, st);
			warning_note_wrong[midi_voice] ++;
		}
		return;
	}
	AddMidiEvent(timestamp, MIDI_NOTEOFF + midi_channel, data1, data2);
}

void CGMidi::AddKsOff(long long timestamp, int data1, int data2)
{
	// Check if range valid
	if ((data1 >= instr_nmin[instr[midi_voice]]) && (data1 <= instr_nmax[instr[midi_voice]])) {
		if (warning_note_wrong[midi_voice] < 4) {
			CString st;
			st.Format("Blocked keyswitch %d/%d time %lld in voice %d instrument %d in note range %d-%d",
				data1, data2, timestamp, midi_voice, instr[midi_voice], 
				instr_nmin[instr[midi_voice]], instr_nmax[instr[midi_voice]]);
			WriteLog(5, st);
			warning_note_wrong[midi_voice] ++;
		}
		return;
	}
	AddMidiEvent(timestamp, MIDI_NOTEOFF + midi_channel, data1, data2);
}

void CGMidi::AddCC(long long timestamp, int data1, int data2)
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
