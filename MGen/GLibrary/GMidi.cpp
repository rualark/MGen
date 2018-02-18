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
	mo = 0;
	last_cc.resize(128);
	//TestKeyMatrix();
}

// One-time function
void CGMidi::TestKeyMatrix() {
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

void CGMidi::GetLyVcnt(int step1, int step2, vector<int> &vm_max) {
	ly_vm_cnt = 0;
	for (int v = v_cnt - 1; v >= 0; --v) {
		if (vm_max[v]) ++ly_vm_cnt;
	}
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

void CGMidi::SendLyViz(ofstream &fs, int pos, CString &ev, int le, int i, int v, int phase) {
	if (!lyi.size()) return;
	// Show flag finish
	for (int x = 0; x < lyi[ly_s2].shf.size(); ++x) {
		if (!lyi[ly_s2].shf[x]) continue;
		int sev = lyi[ly_s2 + lyi[ly_s2].shsl[x]].shse[x];
		if (x == vSlur) {
			if (phase == 10) fs << " ) ";
		}
		if (x == vGlis) {
			if (phase == 1) 
				fs << " \\override NoteColumn.glissando-skip = ##f\n ";
		}
		if (x == vPSlur) {
			if (phase == 10) fs << " \\) ";
		}
		if (x == vBracket) {
			if (phase == 10) {
				fs << " \\stopGroup\n ";
			}
		}
		if (x == vTS) {
			if (phase == 10) {
				fs << " \\stopTextSpan\n";
			}
		}
		if (x == vVBracket) {
			if (phase == 11) fs << " \\override BreathingSign.color = #(rgb-color "
				<< GetLyColor(flag_color[sev])
				<< ")\n \\rightBracket\n";
		}
		if (x == vVolta) {
			if (phase == 11)
				fs << " \\set Score.repeatCommands = #'((volta #f))\n";
		}
		if (x == vTrill) {
			if (phase == 10) {
				fs << " \\stopTrillSpan\n";
			}
		}
		if (x == vOttava) {
			if (phase == 11) {
				fs << " \\unset Staff.ottavation\n";
			}
		}
		if (x == vPedal) {
			if (phase == 10) {
				fs << " \\sustainOff\n";
			}
		}
	}
	// Show flag start
	for (int x = 0; x < lyi[ly_s2].shs.size(); ++x) {
		if (!lyi[ly_s2].shs[x]) continue;
		int sev = lyi[ly_s2].shse[x];
		if (x == vGlis) {
			if (phase == 1) {
				fs << " \\override Glissando.color=#(rgb-color "
					<< GetLyColor(flag_color[sev])
					<< ")\n";
			}
			if (phase == 10)
				fs << " \\glissando\n \\override NoteColumn.glissando-skip = ##t\n";
		}
		if (x == vSlur) {
			if (phase == 1) {
				fs << " \\override Slur.color=#(rgb-color "
					<< GetLyColor(flag_color[sev])
					<< ")\n";
			}
			if (phase == 10)
				fs << " ( ";
		}
		if (x == vPSlur) {
			if (phase == 1) {
				fs << " \\override PhrasingSlur.color=#(rgb-color "
					<< GetLyColor(flag_color[sev])
					<< ")\n";
			}
			if (phase == 10)
				fs << " \\( ";
		}
		if (x == vVBracket) {
			if (phase == 1)
				fs << " \\override BreathingSign.color = #(rgb-color "
				<< GetLyColor(flag_color[sev]) << ")\n \\leftBracket\n";
		}
		if (x == vVolta) {
			if (phase == 1)
				fs << " \\override Score.VoltaBracket.color = #(rgb-color "
				<< GetLyColor(flag_color[sev])
				<< ")\n \\set Score.repeatCommands = #'((volta \"" + 
				lyi[ly_s2].sht[x] + "\"))\n";
		}
		if (x == vBracket) {
			if (phase == 1) {
				fs << " \\override HorizontalBracket.color=#(rgb-color "
					<< GetLyColor(flag_color[sev]) << ")\n ";
			}
			if (phase == 10) {
				fs << " -\\tweak #'stencil #(label \"" + lyi[ly_s2].sht[x] + "\" (rgb-color "
					<< GetLyColor(flag_color[sev]) << "))\\startGroup\n";
			}
		}
		if (x == vTrill) {
			if (phase == 1) {
				fs << " \\override TrillSpanner.bound-details.left.text = ";
				if (lyi[ly_s2].sht[x].IsEmpty()) {
					fs << "#f\n ";
				}
				else {
					fs << "\\markup{ \\raise #0.6 \\teeny \"" + lyi[ly_s2].sht[x] + "\" }\n ";
				}
				fs << " \\override TrillSpanner.color = #(rgb-color "
					<< GetLyColor(flag_color[sev]) << ")\n";
			}
			if (phase == 9) {
				fs << " \\startTrillSpan\n";
			}
		}
		if (x == vOttava) {
			if (phase == 1) {
				fs << " \\set Staff.ottavation = \\markup { \\teeny \""
					+ lyi[ly_s2].sht[x] + "\" }\n ";
				fs << " \\override Staff.OttavaBracket.color = #(rgb-color "
					<< GetLyColor(flag_color[sev]) << ")\n";
			}
		}
		if (x == vTS) {
			if (phase == 1) {
				fs << " \\myTS \""
					+ lyi[ly_s2].sht[x] + "\" #(rgb-color " +
					GetLyColor(flag_color[sev]) << ")\n";
				fs << "\\textSpannerDown\n";
			}
			if (phase == 9) {
				fs << "\\startTextSpan\n";
			}
		}
		if (x == vPedal) {
			if (phase == 1) {
				fs << " \\override Staff.PianoPedalBracket.color = #(rgb-color " +
					GetLyColor(flag_color[sev]) << ")\n";
				fs << "  \\override Staff.SustainPedal #'stencil = \n" <<
					"  #(lambda (grob) (grob-interpret-markup grob (markup \n" <<
					"  #:with-color (rgb-color " + GetLyColor(flag_color[sev]) + ")\n" <<
					"  #:lower 0.4\n" <<
					"	 (#:teeny \"" + lyi[ly_s2].sht[x] + "\")))) \n";
				fs << "\\textSpannerDown\n";
			}
			if (phase == 9) {
				fs << "\\sustainOn\n";
			}
		}
	}
}

// Send note or pause
void CGMidi::SendLyEvent(ofstream &fs, int pos, CString ev, int le, int i, int v) {
	// Length array
	vector<int> la;
	SplitLyNote(pos, le, la);
	SplitLyNoteMeasure(pos, le, la);
	for (int lc = 0; lc < la.size(); ++lc) {
		ly_s = i;
		ly_s2 = ly_s - ly_step1;
		SaveLyComments(i, v, pos);
		SendLyViz(fs, pos, ev, le, i, v, 1);
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
		if (midifile_export_marks && !mark[i][v].IsEmpty()) {
			CString st = mark[i][v];
			st.Replace("\n", "");
			if (st == "PD" || st == "CA" || st == "DN") {
				if (ly_msh) {
					if (GetGreen(mark_color[i][v]) == GetRed(mark_color[i][v])) {
						fs << " \\staccato ";
					}
					else {
						fs << "  \\staccatissimo ";
					}
				}
			}
		}
		if (i > -1) { 
			i += la[lc] / midifile_out_mul[i];
			pos += la[lc];
		}
		SendLyViz(fs, pos, ev, le, i, v, 9);
		SendLyViz(fs, pos, ev, le, i, v, 10);
		SendLyViz(fs, pos, ev, le, i, v, 11);
		SendLyViz(fs, pos, ev, le, i, v, 12);
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
		Lighten(GetRed(col), 2) / 255.0,
		Lighten(GetGreen(col) * 1.5, 2) / 255.0,
		Lighten(GetBlue(col), 2) / 255.0);
	return st;
}

CString CGMidi::GetLyMarkColor2(DWORD col) {
	if (col == color_noflag) return "1 1 1";
	CString st;
	if (GetGreen(col) == GetRed(col) && GetRed(col) == GetBlue(col)) return "1 1 1";
	st.Format("%.3f %.3f %.3f",
		Lighten(GetRed(col), 2) / 255.0, 
		Lighten(GetGreen(col), 2) / 255.0, 
		Lighten(GetBlue(col), 2) / 255.0);
	return st;
}

void CGMidi::SendLyNoteColor(ofstream &fs, DWORD col) {
	fs << "\n    \\override NoteHead.color = #(rgb-color " << GetLyColor(col) << ") ";
	fs << "\n    \\override Stem.color = #(rgb-color " << GetLyColor(col) << ") ";
}

CString CGMidi::GetRealIntName(int s, int v1, int v2) {
	// Exact interval
	int in = abs(note[s][v2] - note[s][v1]);
	in = in ? ((in % 12) ? (in % 12) : 12) : 0;
	// Interval between base notes
	int no, oct, alter;
	int no2, oct2, alter2;
	GetRealNote(note[s][v1], tonic[s][v1], minor[s][v1], no, oct, alter);
	GetRealNote(note[s][v2], tonic[s][v2], minor[s][v2], no2, oct2, alter2);
	int fno = no + oct * 12;
	int fno2 = no2 + oct2 * 12;
	int bin = abs(fno - fno2);
	bin = bin ? ((bin % 12) ? (bin % 12) : 12) : 0;
	// Diatonic interval
	int din = CC_C(abs(note[s][v1] - note[s][v2]), 0, 0);
	// Base diatonic interval
	int bdin = CC_C(abs(fno - fno2), 0, 0);
	int bdin2 = bdin ? ((bdin % 7) ? (bdin % 7) : 7) : 0;
	// Build string
	// Diatonic did not change or triton / triton base
	if (din == bdin || in == 6 || bin == 6) {
		if (in == 0) return "1";
		else if (in == 1) return "m2";
		else if (in == 2) return "M2";
		else if (in == 3) return "m3";
		else if (in == 4) return "M3";
		else if (in == 5) return "4";
		else if (in == 6) return "tri";
		else if (in == 7) return "5";
		else if (in == 8) return "m6";
		else if (in == 9) return "M6";
		else if (in == 10) return "m7";
		else if (in == 11) return "M7";
		else return "8";
	}
	// Diatonic changed
	CString st;
	st.Format("%d", bdin2 + 1);
	if (din < bdin) st = "dim" + st;
	else st = "aug" + st;
	return st;
}

CString CGMidi::GetIntName(int iv) {
	if (iv == 0) return "1";
	else if (iv == 1) return "m2";
	else if (iv == 2) return "M2";
	else if (iv == 3) return "m3";
	else if (iv == 4) return "M3";
	else if (iv == 5) return "4";
	else if (iv == 6) return "tri";
	else if (iv == 7) return "5";
	else if (iv == 8) return "m6";
	else if (iv == 9) return "M6";
	else if (iv == 10) return "m7";
	else if (iv == 11) return "M7";
	else return "8";
}

void CGMidi::ParseNLinks(int i, int i2, int v, int foreign) {
	CString com;
	int x = 0;
	for (auto const& it : nlink[i][v]) {
		if (foreign && !rule_viz_v2[it.first]) continue;
		lyi[i2 - ly_step1].nflags.push_back(it.first);
		if (foreign) {
			lyi[i2 - ly_step1].nfl.push_back(i + it.second - coff[i + it.second][v] - i2);
		}
		else {
			lyi[i2 - ly_step1].nfl.push_back(it.second);
		}
		lyi[i2 - ly_step1].nfn.push_back(ly_flags + 1);
		lyi[i2 - ly_step1].nff.push_back(foreign);
		lyi[i2 - ly_step1].nfs.push_back(0);
		lyi[i2 - ly_step1].nfc.push_back(comment[i][v][x]);
		if (!foreign) ++ly_flags;
		++x;
	}
}

void CGMidi::SaveLyComments(int i, int v, int pos) {
	CString st, com, note_st;
	int pos1, pos2, found;
	if (!lyi.size()) return;
	if (lyi[ly_s2].nflags.size()) {
		note_st = "\\markup \\wordwrap \\bold {\n  ";
		// Show voice number if more than 1 voice
		if (ly_vm_cnt > 1) {
			st.Format("VOICE %d, ", v + 1);
			note_st += st;
		} 
		st.Format("NOTE %d at %d:%d - %s",
			ly_nnum, pos / 8 + 1, pos % 8 + 1, GetLyNoteVisual(i, v));
		if (coff[i][v]) 
			st += " (slur)";
		note_st += st + "\n}\n";
		found = 0;
		for (int c = 0; c < lyi[ly_s2].nflags.size(); ++c) {
			// Do not process foreign flags
			if (lyi[ly_s2].nff[c]) break;
			int fl = lyi[ly_s2].nflags[c];
			if (!accept[fl]) st = "- ";
			else if (accept[fl] == -1) st = "$ ";
			else st = "+ ";
			com = st + RuleName[rule_set][fl] + " (" + SubRuleName[rule_set][fl] + ")";
			if (!RuleComment[fl].IsEmpty()) com += ". " + RuleComment[fl];
			if (!SubRuleComment[rule_set][fl].IsEmpty()) com += " (" + SubRuleComment[rule_set][fl] + ")";
			// Send note number with first comment
			if (!found) {
				found = 1;
				ly_com_st += note_st;
			}
			ly_com_st += "\\markup \\wordwrap \\with-color #(rgb-color " +
				GetLyColor(flag_color[severity[fl]]) + ") {\n  ";
			com.Replace("\"", "\\\"");
			com.Replace(" ", "\" \"");
			st.Format("\\teeny \\raise #0.2 \\circle %d \"", lyi[ly_s2].nfn[c]);
			ly_com_st += st;
			ly_com_st += com + "\"\n";
			ly_com_st += "\n}\n";
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

void CGMidi::SetLyShape(int s1, int s2, int f, int fl, int vtype) {
	if (lyi[s1].shse[vtype] <= severity[fl]) {
		// Start
		lyi[s1].shs[vtype] = 1;
		// Finish
		lyi[s2].shf[vtype] = 1;
		// Link to start
		lyi[s2].shsl[vtype] = s1 - s2;
		lyi[s1].shse[vtype] = severity[fl];
		if (vtype == vInterval || vtype == vNoteName) {
			if (lyi[s2].shse[vtype] <= severity[fl]) {
				lyi[s2].shse[vtype] = severity[fl];
			}
			if (vtype == vNoteName) ++ly_notenames;
		}
		lyi[s1].sht[vtype] = rule_viz_t[fl];
		// Save flag shape (step depends if link is forward or backward)
		lyi[ly_s2].nfs[f] = vtype;
		lyi[s1].shflag[vtype] = f;
		lyi[s1].shfp[vtype] = ly_s2;
	}
}

void CGMidi::ClearLyShape(int s1, int s2, int vtype) {
	lyi[s1].shs[vtype] = 0;
	lyi[s2].shf[vtype] = 0;
	// Calculate maximum severity
	lyi[s1].shse[vtype] = -1;
	// Remove link
	lyi[lyi[s1].shfp[vtype]].nfs[lyi[s1].shflag[vtype]] = 0;
	lyi[s1].shflag[vtype] = -1;
	lyi[s1].shfp[vtype] = -1;
}

void CGMidi::ExportLyI() {
	ofstream fs;
	fs.open("log\\lyi-" + m_config + ".csv", ios_base::app);
	fs << "Step[" << ly_mel << "];";
	for (int x = 0; x < MAX_VIZ; ++x) {
		fs << "shs[" << x << "];";
		fs << "shf[" << x << "];";
		fs << "shsl[" << x << "];";
		fs << "shse[" << x << "];";
		fs << "sht[" << x << "];";
	}
	fs << "\n";
	for (ly_s = ly_step1; ly_s < ly_step2; ++ly_s) {
		ly_s2 = ly_s - ly_step1;
		fs << ly_s2 << ";";
		for (int x = 0; x < MAX_VIZ; ++x) {
			fs << lyi[ly_s2].shs[x] << ";";
			fs << lyi[ly_s2].shf[x] << ";";
			fs << lyi[ly_s2].shsl[x] << ";";
			fs << lyi[ly_s2].shse[x] << ";";
			fs << lyi[ly_s2].sht[x] << ";";
		}
		fs << "\n";
	}
	fs << "\n";
	fs.close();
}
void CGMidi::InitLyI() {
	if (ly_mel == -1) return;
	ly_v2 = ly_v;
	if (ly_vm_cnt > 1) ly_v2 = (ly_v / 2) * 2 + !(ly_v % 2);
	ly_flags = 0;
	ly_notenames = 0;
	if (m_algo_id == 111) {
		ly_vhigh = ly_v;
		ly_vlow = ly_v;
	}
	else { 
		ly_vhigh = max(ly_v, ly_v2);
		ly_vlow = min(ly_v, ly_v2);
	}
	lyi.clear();
	lyi.resize(ly_step2 - ly_step1);
	for (ly_s = ly_step1; ly_s < ly_step2; ++ly_s) {
		ly_s2 = ly_s - ly_step1;
		// Find current note position
		int cur_note_step = ly_s;
		if (ly_s2 > 0) {
			for (int x = ly_s; x >= ly_step1; --x) {
				if (note[x][ly_v] != note[ly_s][ly_v]) {
					cur_note_step = x + 1;
					break;
				}
				if (x == ly_step1) cur_note_step = x;
			}
		}
		// Init vectors
		lyi[ly_s2].shs.resize(MAX_VIZ);
		lyi[ly_s2].shsl.resize(MAX_VIZ);
		lyi[ly_s2].shf.resize(MAX_VIZ);
		lyi[ly_s2].shse.resize(MAX_VIZ, -1);
		lyi[ly_s2].shflag.resize(MAX_VIZ, -1);
		lyi[ly_s2].shfp.resize(MAX_VIZ, -1);
		lyi[ly_s2].sht.resize(MAX_VIZ);
		// Parse flags
		ParseNLinks(ly_s, ly_s, ly_v, 0);
		if (!lyi[ly_s2].nflags.size() && v_cnt > 1) {
			if (ly_v2 < v_cnt) {
				ParseNLinks(ly_s, cur_note_step, ly_v2, 1);
			}
		}
		else {
			// Detect flags that are not at note start and not at cantus note start
			for (int f = 0; f < lyi[ly_s2].nflags.size(); ++f) {
				int link = lyi[ly_s2].nfl[f];
				if ((!coff[ly_s][ly_v] || !coff[ly_s][ly_v2]) && 
					(!coff[ly_s + link][ly_v] || !coff[ly_s + link][ly_v2])) continue;
				int fl = lyi[ly_s2].nflags[f];
				CString est;
				est.Format("Detected flag at hidden position %d/%d: [%d] %s %s (%s)",
					ly_s, ly_s + link, fl, accept[fl] ? "+" : "-", 
					RuleName[rule_set][fl], SubRuleName[rule_set][fl]);
				WriteLog(1, est);
			}
		}
	}
	for (ly_s = ly_step1; ly_s < ly_step2; ++ly_s) {
		ly_s2 = ly_s - ly_step1;
		// Find next note position
		int next_note_step = ly_s + noff[ly_s][ly_v];
		// Find previous note position
		int prev_note_step = max(ly_step1, ly_s - poff[ly_s][ly_v]);
		// Parse flags
		for (int f = 0; f < lyi[ly_s2].nflags.size(); ++f) {
			int fl = lyi[ly_s2].nflags[f];
			int link = lyi[ly_s2].nfl[f];
			int vtype = rule_viz[fl];
			int sev = severity[fl];
			int skip_shape = 0;
			int prev_link_note = max(ly_step1, ly_s + link - poff[ly_s + link][ly_v]);
			// Find link note position
			int link_note_step = ly_s + link;
			if (ly_s2 > 0) {
				for (int x = ly_s + link; x >= ly_step1; --x) {
					if (note[x][ly_v] != note[ly_s + link][ly_v]) {
						link_note_step = x + 1;
						break;
					}
					if (x == ly_step1) link_note_step = x;
				}
			}
			// Get flag start/stop
			int s1 = min(ly_s2, ly_s2 + link);
			int s2 = max(ly_s2, ly_s2 + link);
			if (lyi[ly_s2].nff[f]) {
				s1 = min(ly_s2, link_note_step - ly_step1);
				s2 = max(ly_s2, link_note_step - ly_step1);
			}
			// If shape cannot highlight single note, but flag does not contain link, then link to next note
			if (!viz_singlenote[vtype] && s1 == s2) s2 = next_note_step - ly_step1;
			// Set interval
			if (rule_viz_int[fl]) {
				SetLyShape(s1, s2, f, fl, vInterval);
			}
			if (!viz_can_overlap[vtype]) {
				// Check that flag overlaps
				int overlap1 = -1;
				int overlap2 = -1;
				int overlap_limit = s1;
				// For groups check for collision between borders
				if (viz_type[vtype] == vtGroup || viz_type[vtype] == vtVolta)
					overlap_limit = s1 - 1;
				// For vbrackets check for collision between notes
				if (viz_type[vtype] == vtVBracket)
					overlap_limit = min(prev_note_step, prev_link_note) - ly_step1 - 1;
				for (int x = ly_step2 - ly_step1 - 1; x > overlap_limit; --x) {
					if (lyi[x].shf[vtype]) {
						overlap2 = x; 
						overlap1 = x + lyi[x].shsl[vtype];
						// Choose highest severity
						if (sev > lyi[overlap1].shse[vtype]) {
							ClearLyShape(overlap1, overlap2, vtype);
						}
						else {
							// Skip shape
							skip_shape = 1;
							break;
						}
					}
				}
				if (skip_shape) continue;
			}
			SetLyShape(s1, s2, f, fl, vtype);
		}
	}
	ExportLyI();
}

void CGMidi::SaveLySegment(ofstream &fs, CString st, CString st2, int step1, int step2) {
	vector<CString> sv;
	CString clef, key, key_visual;
	int pos, pos2, le, le2, pause_accum, pause_pos, pause_i;
	ly_com_st.Empty();
	ly_ly_st.Empty();
	// Voice melody min pitch
	vector<int> vm_min;
	// Voice melody max pitch
	vector<int> vm_max;
	// Calculate stats
	ly_step1 = step1;
	ly_step2 = step2;
	GetLyRange(step1, step2, vm_min, vm_max);
	GetLyVcnt(step1, step2, vm_max);
	ly_mul = midifile_out_mul[step1];
	//if (ly_vm_cnt == 1 && (m_algo_id == 121 || m_algo_id == 112)) mul = 8;
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
		ly_v = v;
		InitLyI();
		// Select best clef
		clef = DetectLyClef(vm_min[v], vm_max[v]);
		st.Format("\\new Staff = \"staff%d\" {\n", ly_v);
		fs << st;
		st.Format("  \\set Staff.instrumentName = \"Voice %d\"\n", v + 1);
		fs << st;
		fs << "  \\clef \"" << clef << "\" \\key " << key;
		fs << " \\" << (minor[step1][0] ? "minor" : "major") << "\n";
		read_file_sv("configs\\ly\\staff.ly", sv);
		write_file_sv(fs, sv);
		fs << "  { ";
		ly_nnum = 0;
		pause_accum = 0;
		pause_pos = -1;
		for (int i = step1; i < step2; i++) {
			pos = ly_mul * (i - step1);
			le = ly_mul * len[i][v];
			if (pause[i][v]) {
				pause_accum += le;
				if (pause_pos == -1) {
					pause_pos = pos;
					pause_i = i;
				}
			}
			else {
				++ly_nnum;
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
		SendLyMistakes();
		SendLyNoteNames();
		SendLyHarm();
		SendLyIntervals();
	}
	fs << ly_ly_st;
	fs << ">>\n";
	fs << ly_com_st;
	// Second info
	//st2.Replace("\n", "\n}\n\\markup \\wordwrap \\italic {\n  ");
	//st2.Replace("#", "\"#\"");
	//fs << "\\markup \\wordwrap \\italic {\n  \\vspace #2\n  " << st2 << "\n}\n";
}

void CGMidi::SendLySkips(int count) {
	for (int x = 0; x < count; ++x) {
		ly_ly_st += " \\skip 8 ";
	}
}

void CGMidi::SendLyMistakes() {
	CString st;
	if (!ly_flags) return;
	st.Format("  \\new Lyrics \\with { alignAboveContext = \"staff%d\" } {\n", ly_vhigh);
	ly_ly_st += st;
	ly_ly_st += "    \\lyricmode {\n";
	ly_ly_st += "      \\set stanza = #\" Flags:\"\n";
	for (ly_s = ly_step1; ly_s < ly_step2; ++ly_s) {
		ly_s2 = ly_s - ly_step1;
		if (!lyi[ly_s2].nflags.size()) {
			SendLySkips(ly_mul);
			continue;
		}
		ly_ly_st += "      \\markup{ \\teeny \\override #`(direction . ,UP) { \\dir-column {\n";
		int max_mist = lyi[ly_s2].nflags.size() - 1;
		// Do not show too many mistakes
		if (max_mist > 5) {
			max_mist = 4; 
			ly_ly_st += "...\n";
		}
		for (int f = max_mist; f >= 0 ; --f) {
			int fl = lyi[ly_s2].nflags[f];
			st.Format("        \\with-color #(rgb-color " +
				GetLyColor(flag_color[severity[fl]]) + ") %s \\circle %d\n",
				lyi[ly_s2].nfs[f]?"\\underline":"", lyi[ly_s2].nfn[f]);
			// \override #'(offset . 5) \override #'(thickness . 2) 
			ly_ly_st += st;
		}
		ly_ly_st += "      } } }8\n";
		SendLySkips(ly_mul - 1);
	}
	ly_ly_st += "    }\n";
	ly_ly_st += "  }\n";
}

void CGMidi::SendLyHarm() {
	CString st;
	if (!ly_flags) return;
	st.Format("  \\new Lyrics \\with { alignBelowContext = \"staff%d\" } {\n", ly_vlow);
	ly_ly_st += st;
	ly_ly_st += "    \\lyricmode {\n";
	ly_ly_st += "      \\set stanza = #\" Harmony:\"\n";
	for (ly_s = ly_step1; ly_s < ly_step2; ++ly_s) {
		ly_s2 = ly_s - ly_step1;
		CString st = mark[ly_s][ly_v2];
		st.Replace("\n", "");
		if (!st.IsEmpty() && st != "PD" && st != "CA" && st != "DN") {
			ly_ly_st += "  \\markup{ ";
			int found = 0;
			// Replace dominant symbol
			st.Replace("#", " \"#\" ");
			st.Replace("b", " \\flat ");
			if (ly_dominant_letter) {
				if (st[0] == 'D') {
					st = "\\concat { \\char ##x00D0 " + st.Right(st.GetLength() - 1) + " } ";
				}
				else if (st[0] == 'd') {
					st = "\\concat { \\char ##x0111 " + st.Right(st.GetLength() - 1) + " } ";
				}
				else st = "\\concat { " + st + " } ";
			}
			else st = "\\concat { " + st + " } ";
			st.Replace("6", " \\raise #0.7 6");
			//if (found) st = ", " + st;
			found = 1;
			ly_ly_st += "\\teeny \\on-color #(rgb-color ";
			ly_ly_st += GetLyMarkColor(mark_color[ly_s][ly_v2]);
			ly_ly_st += ") \\pad-markup #0.4 " + st + " ";
			ly_ly_st += "}8\n";
			SendLySkips(ly_mul - 1);
		}
		else {
			SendLySkips(ly_mul);
		}
	}
	ly_ly_st += "    }\n";
	ly_ly_st += "  }\n";
}

void CGMidi::SendLyIntervals() {
	CString st;
	if (!ly_flags) return;
	if (ly_vlow == ly_vhigh) return;
	st.Format("  \\new Lyrics \\with { alignBelowContext = \"staff%d\" } {\n", ly_vlow);
	ly_ly_st += st;
	ly_ly_st += "    \\lyricmode {\n";
	ly_ly_st += "      \\set stanza = #\" Interval:\"\n";
	for (ly_s = ly_step1; ly_s < ly_step2; ++ly_s) {
		ly_s2 = ly_s - ly_step1;
		if (!lyi[ly_s2].shs[vInterval] && !lyi[ly_s2].shf[vInterval]) {
			SendLySkips(ly_mul);
			continue;
		}
		//int in = note[ly_s][ly_vhigh] - note[ly_s][ly_vlow];
		//int in2 = in % 12;
		//in = in ? (in2 ? in2 : 12) : 0;
		//CString st = GetIntName(in);
		CString st = GetRealIntName(ly_s, ly_vhigh, ly_vlow);
		ly_ly_st += "\\markup{ ";
		ly_ly_st += "\\teeny ";
		if (lyi[ly_s2].shse[vInterval] > -1) {
			DWORD col = flag_color[lyi[ly_s2].shse[vInterval]];
			if (col && col != color_noflag)
				ly_ly_st += " \\on-color #(rgb-color " + GetLyMarkColor2(col) + ") ";
		}
		ly_ly_st += " \\pad-markup #0.4 \\concat { " + st + " ";
		ly_ly_st += "} }\n";
		SendLySkips(ly_mul - 1);
	}
	ly_ly_st += "    }\n";
	ly_ly_st += "  }\n";
}

void CGMidi::SendLyNoteNames() {
	CString st;
	if (!ly_notenames) return;
	st.Format("  \\new Lyrics \\with { alignBelowContext = \"staff%d\" } {\n", ly_v);
	ly_ly_st += st;
	ly_ly_st += "    \\lyricmode {\n";
	ly_ly_st += "      \\set stanza = #\" Note:\"\n";
	for (ly_s = ly_step1; ly_s < ly_step2; ++ly_s) {
		ly_s2 = ly_s - ly_step1;
		if (!lyi[ly_s2].shs[vNoteName] && !lyi[ly_s2].shf[vNoteName]) {
			SendLySkips(ly_mul);
			continue;
		}
		CString st = GetLyNoteVisual(ly_s, ly_v);
		ly_ly_st += "\\markup{ ";
		ly_ly_st += "\\teeny ";
		if (lyi[ly_s2].shse[vNoteName] > -1) {
			DWORD col = flag_color[lyi[ly_s2].shse[vNoteName]];
			if (col && col != color_noflag)
				ly_ly_st += " \\on-color #(rgb-color " + GetLyMarkColor2(col) + ") ";
		}
		ly_ly_st += " \\pad-markup #0.4 \\concat { " + st + " } ";
		ly_ly_st += "}\n";
		SendLySkips(ly_mul - 1);
	}
	ly_ly_st += "    }\n";
	ly_ly_st += "  }\n";
}

void CGMidi::SaveLy(CString dir, CString fname) {
	vector<CString> sv;
	CString title;
	DeleteFile("log\\lyi-" + m_config + ".csv");
	title = m_algo_folder + ": " + m_config + " (" +
		CTime::GetCurrentTime().Format("%Y-%m-%d %H:%M") + ")";
	ly_fs.open(dir + "\\" + fname + ".ly");
	read_file_sv("configs\\ly\\header.ly", sv);
	for (int i = 0; i < sv.size(); ++i) {
		sv[i].Replace("$TITLE$", title);
		ly_fs << sv[i] << "\n";
	}

	if (!mel_info.size()) {
		CString st;
		st = "Whole piece";
		ly_mel = -1;
		SaveLySegment(ly_fs, st, "", 0, t_generated);
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
			ly_mel = m;
			if (found) SaveLySegment(ly_fs, mel_info[m], mel_info2[m], first_step, last_step);
			//if (m < mel_info.size() - 1) fs << "\\pageBreak\n";
		}
	}
	ly_fs << "\\header {tagline = \"This file was created by MGen ";
	ly_fs << APP_VERSION << "\"}\n";
	read_file_sv("configs\\ly\\footer.ly", sv);
	write_file_sv(ly_fs, sv);
	ly_fs.close();
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
		for (int tr = MAX_VOICE - 1; tr >= 0; --tr) {
			if (midifile_buf[sta][tr].size()) {
				if (!stracks_cnt[sta]) stracks_cnt[sta] = tr + 1;
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
		smidifile[sta].addTrackName(0, 0, st);
		smidifile[sta].addTempo(track, 0, 60 / spq);
	}
	midifile.addTracks(tracks_cnt + 1);
	midifile.setTicksPerQuarterNote(tpq);
	midifile.addTrackName(0, 0, st);
	midifile.addTempo(track, 0, 60 / spq);
	track = 0;
	for (int sta = 0; sta < stage_max; ++sta) {
		for (int tr = 0; tr < MAX_VOICE; ++tr) {
			if (!midifile_buf[sta][tr].size()) continue;
			// Convert DAW track to midi file track number
			strack = tr + 1;
			++track;
			// Send instrument name
			CString st;
			if (sta == 0) st = icf[t_instr[tr]].group;
			else st.Format("%s %d", icf[t_instr[tr]].group, sta);
			string st2 = st;
			midifile.addTrackName(track, 0, st2);
			smidifile[sta].addTrackName(strack, 0, st2);
			//midifile.addPatchChange(track, 0, channel, 0); // 0=piano, 40=violin, 70=bassoon
			//smidifile[sta].addPatchChange(strack, 0, channel, 0); // 0=piano, 40=violin, 70=bassoon
			for (int i = 0; i < midifile_buf[sta][tr].size(); i++) {
				tick = midifile_buf[sta][tr][i].timestamp / 1000.0 / spq * tpq;
				type = Pm_MessageStatus(midifile_buf[sta][tr][i].message) & 0xF0;
				data1 = Pm_MessageData1(midifile_buf[sta][tr][i].message);
				data2 = Pm_MessageData2(midifile_buf[sta][tr][i].message);
				channel = Pm_MessageStatus(midifile_buf[sta][tr][i].message) & 0x0F;
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
		string st = icf[instr[v]].group;
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
	in_ppq = midifile.getTicksPerQuarterNote();
	int tpc = (float)in_ppq / (float)2 / (float)midifile_in_mul; // ticks per croche
	vector<int> vlast_step(MAX_VOICE);
	vector<int> vlast_pitch(MAX_VOICE);
	vector<int> voverlap(MAX_VOICE);
	vector<int> vdist(MAX_VOICE);
	CString st, tnames = "", inames = "";
	// Convert track instrument ids to voice instrument ids
	vector<int> instr2 = instr;

	// Autolegato with gaps, if grownotes was not set
	if (midi_file_type == mftFIN && grow_notes == 0 && auto_legato == 1) {
		grow_notes = in_ppq * 0.09;
	}
	if (midi_file_type == mftMUS && grow_notes == 0 && auto_legato == 1) {
		grow_notes = in_ppq * 0.09;
	}
	midifile_loaded = 1;
	int last_step = 0;
	// If there is no tempo in file, set default
	tempo[0] = 120; 
	int first_track = -1;
	vector<int> track_firstchan;
	track_firstchan.resize(midifile.getTrackCount(), -1);
	// Load tempo
	for (int track = 0; track < midifile.getTrackCount(); track++) {
		for (int i = 0; i < midifile[track].size(); i++) {
			MidiEvent* mev = &midifile[track][i];
			if (mev->isNoteOn() || mev->isController()) {
				if (track_firstchan[track] == -1)
					track_firstchan[track] = mev->getChannel();
			}
			if (mev->isTempo()) {
				int pos = round(mev->tick / (float)tpc);
				if (pos >= t_allocated) ResizeVectors(max(t_allocated * 2, pos + 1));
				tempo[pos] = mev->getTempoBPM() * midifile_in_mul;
				if (pos > last_step) last_step = pos;
			}
			if (mev->isNoteOn() && first_track == -1) first_track = track;
		}
	}
	if (first_track == -1) first_track = 0;
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

	vector<int> mute_active;
	vector<int> marc_active;
	vector<int> pizz_active;
	vector<int> trem_active;
	vector<int> spic_active;
	vector<int> stac_active;
	vector<int> tasto_active;
	mute_active.resize(16);
	marc_active.resize(16);
	pizz_active.resize(16);
	trem_active.resize(16);
	spic_active.resize(16);
	stac_active.resize(16);
	tasto_active.resize(16);

	for (int track = first_track; track < midifile.getTrackCount(); track++) {
		if (need_exit) break;
		int last_cc1_step = -1;
		vfill(mute_active, 0);
		vfill(marc_active, 0);
		vfill(pizz_active, 0);
		vfill(trem_active, 0);
		vfill(spic_active, 0);
		vfill(stac_active, 0);
		vfill(tasto_active, 0);
		if (track > first_track) {
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
		track_id[v] = track - first_track + 1;
		track_vid[v] = 0;
		// Convert track instrument to voice instrument
		instr[v] = instr2[track_id[v]-1];
		for (int i = 0; i<midifile[track].size(); i++) {
			if (need_exit) break;
			MidiEvent* mev = &midifile[track][i];
			int chan = mev->getChannel();
			// Get track names
			if (mev->isMetaMessage()) {
				if (mev->getMetaType() == 0x03 && track_name[v].IsEmpty()) {
					track_name[v].Empty();
					for (int x = 0; x < mev->size(); x++) {
						track_name[v] += mev->data()[x];
					}
					// Remove first data items
					track_name[v] = track_name[v].Mid(3);
					st.Format("%d", v);
					tnames += " \n" + st + "=" + track_name[v];
					// Map track name to instrument name
					for (int i = 0; i < icf.size(); i++) {
						// Exact match
						//if (InstName[i] == track_name[v]) instr[v] = i;
						// Search inside track name
						//else if (track_name[v].Find(InstName[i], 0) != -1) instr[v] = i;
					}
				}
			}
			if (mev->isPatchChange()) {
				// Get program changes for MuseScore files
				if (track_name[v] == "") {
					track_name[v] = midi_iname[mev->data()[1]];
				}
				// Get program changes for Sibelius files
				int patch = mev->data()[1];
				if (patch == 59) mute_active[chan] = 1;
				else if (patch == 45) pizz_active[chan] = 1;
				else {
					mute_active[chan] = 0;
					marc_active[chan] = 0;
					pizz_active[chan] = 0;
					trem_active[chan] = 0;
					spic_active[chan] = 0;
					stac_active[chan] = 0;
					tasto_active[chan] = 0;
				}
			}
			if (mev->isController()) {
				int pos = round(mev->tick / (float)tpc);
				int cc = mev->data()[1];
				int val = mev->data()[2];
				if (pos + 1 >= t_allocated) ResizeVectors(max(pos + 1, t_allocated * 2));
				if (cc == 1) {
					// Fill cc1
					if (last_cc1_step > -1) {
						for (int z = last_cc1_step + 1; z < pos; ++z) {
							dyn[z][v1] = dyn[z - 1][v1];
						}
					}
					dyn[pos][v1] = val;
					last_cc1_step = pos;
				}
				if (cc == 64 && icf[instr[v]].pedal_import) {
					if (val > 63) {
						SetBit(filter[pos][v], fPEDAL);
					}
					else {
						SetBit(filter[pos][v], fUNPEDAL);
					}
				}
			}
			if (mev->isNoteOn()) {
				int pos = round(mev->tick / (float)tpc);
				int pitch = mev->getKeyNumber();
				int myvel = mev->getVelocity();
				int tick_dur = mev->getTickDuration();
				if (grow_notes > -1) tick_dur += grow_notes;
				int nlen = round((mev->tick + tick_dur) / (float)tpc) - pos;
				// Parse keyswitch
				if (pitch < icf[instr[v]].import_min || pitch > icf[instr[v]].import_max) {
					if (pitch < 12) {
						vfill(mute_active, 0);
						vfill(marc_active, 0);
						vfill(pizz_active, 0);
						vfill(trem_active, 0);
						vfill(spic_active, 0);
						vfill(stac_active, 0);
						vfill(tasto_active, 0);
					}
					if (pitch == 2) vfill(mute_active, 1);
					if (pitch == 3) vfill(marc_active, 1);
					if (pitch == 5) vfill(pizz_active, 1);
					if (pitch == 7) vfill(trem_active, 1);
					if (pitch == 9) vfill(spic_active, 1);
					if (pitch == 10) vfill(stac_active, 1);
					if (pitch == 11) vfill(tasto_active, 1);
				}
				// Parse normal note
				else {
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
					if (!tempo[pos + nlen]) {
						for (int z = last_step_tempo + 1; z < pos + nlen + 1; ++z) {
							if (!tempo[z]) tempo[z] = tempo[z - 1];
						}
						// Count new time
						CountTime(last_step_tempo + 1, pos + nlen - 1);
						// Set last step that has tempo
						last_step_tempo = pos + nlen - 1;
					}
					// Fallback
					if (!tempo[pos]) tempo[pos] = 100;
					float delta = (float)(mev->tick - pos*tpc) / (float)tpc * 30000.0 / (float)tempo[pos];
					float delta2 = (float)(mev->tick + tick_dur - (pos + nlen)*tpc) /
						(float)tpc * 30000.0 / (float)tempo[pos + nlen];
					// Find overlaps and distance
					if (icf[instr[v]].poly > 1) {
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
							track_id[v] = track - first_track + 1;
							track_vid[v] = v - v1;
							track_name[v] = track_name[v1];
						}
					} // if (instr_poly[instr[v]] > 1)
					else {
						// Check if overwriting long overlap
						if (!pause[pos][v] && noff[pos][v]) {
							// Update previous note ending
							if (pos) {
								setime[pos - 1][v] = stime[pos] + delta;
								smet[pos - 1][v] = smet[pos - 1 + noff[pos - 1][v] - 1][v];
							}
							// Using stime/etime here, because this check is approximate
							float ndur = etime[pos + nlen - 1] - stime[pos];
							float ndur2 = etime[pos + noff[pos][v] - 1] - stime[pos - coff[pos][v]];
							// Calculate overlap (abs is protection from bugs)
							float ov = abs(etime[pos + noff[pos][v] - 1] - stime[pos]);
							// Is overlap long?
							if (ov > ndur * MAX_OVERLAP_MONO || ov > ndur2 * MAX_OVERLAP_MONO) if (warning_loadmidi_overlap < MAX_WARN_MIDI_OVERLAP) {
								CString st;
								st.Format("Error: too long overlap (voice %d) %.0f ms at step %d (note lengths %.0f, %.0f ms) in monophonic instrument %s/%s. Probably sending polyphonic instrument to monophonic.",
									v, ov, pos, ndur, ndur2, icf[instr[v]].group, icf[instr[v]].name);
								WriteLog(0, st);
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
								pause[z][v] = 1;
								vel[z][v] = 0;
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
						vel[pos + z][v] = myvel;
						midi_ch[pos + z][v] = chan;
						pause[pos + z][v] = 0;
						coff[pos + z][v] = z;
						if (trem_active[chan] && icf[instr[v]].trem_import) artic[pos + z][v] = aTREM;
						if (pizz_active[chan] && icf[instr[v]].pizz_import)
							artic[pos + z][v] = aPIZZ;
						if (spic_active[chan] && icf[instr[v]].spic_import) artic[pos + z][v] = aSTAC;
						if (stac_active[chan] && icf[instr[v]].stac_import) artic[pos + z][v] = aSTAC;
						if (marc_active[chan] && icf[instr[v]].marc_import) artic[pos + z][v] = aSTAC;
						if (mute_active[chan] && icf[instr[v]].mute_import) SetBit(filter[pos + z][v], fMUTE);
						if (tasto_active[chan] && icf[instr[v]].tasto_import) SetBit(filter[pos + z][v], fTASTO);
						// Load MuseScore articulations
						if (midi_file_type == mftMUS) {
							int dchan = (chan - track_firstchan[track] + 16) % 16;
							if (dchan == 1 && icf[instr[v]].mute_import) SetBit(filter[pos + z][v], fMUTE);
							if (dchan == 1 && icf[instr[v]].pizz_import) {
								artic[pos + z][v] = aPIZZ;
							}
							if (dchan == 2 && icf[instr[v]].trem_import) artic[pos + z][v] = aTREM;
						}
						// Lock mute
						if (icf[instr[v]].mute_lock) SetBit(filter[pos + z][v], fMUTE);
						// Lock bow
						if (icf[instr[v]].bow_lock == 1) {
							SetBit(filter[pos + z][v], fTASTO);
							ClearBit(filter[pos + z][v], fPONT);
						}
						if (icf[instr[v]].bow_lock == 2) {
							SetBit(filter[pos + z][v], fPONT);
							ClearBit(filter[pos + z][v], fTASTO);
						}
					}
					// Set midi ticks
					smst[pos][v] = mev->tick;
					smet[pos + nlen - 1][v] = mev->tick + tick_dur;
					// Set midi delta only to first step of note, because in in-note steps you can get different calculations for different tempo
					//midi_delta[pos][v] = delta;
					sstime[pos][v] = stime[pos] + delta;
					setime[pos + nlen - 1][v] = etime[pos + nlen - 1] + delta2;
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
		}
		// If track is empty, create a single pause
		if (!note[0][v] && !pause[0][v] && !len[0][v]) {
			FillPause(0, 1, v);
		}
		// Fill cc1 in first voice
		if (last_cc1_step > -1) {
			for (int z = last_cc1_step + 1; z <= last_step; ++z) {
				dyn[z][v1] = dyn[z - 1][v1];
			}
			last_cc1_step = last_step;
		} 
		// Copy cc1 to all voices of current track
		for (int v = v1 + 1; v <= v2; ++v) {
			for (int z = 0; z <= last_step; ++z) {
				dyn[z][v] = dyn[z][v1];
			}
		}
		// Overwrite dynamics with vel where dynamics does not make sense
		for (int v = v1; v <= v2; ++v) {
			for (int z = 0; z <= last_step; ++z) {
				if (artic[z][v] == aPIZZ || !dyn[z][v]) dyn[z][v] = vel[z][v];
			}
		}
	} // for track
	if (need_exit) return;
	// Add closing pauses
	if (last_step + TAIL_STEPS + 1 >= t_allocated) 
		ResizeVectors(max(last_step + TAIL_STEPS + 1, t_allocated * 2));
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
	UnisonMute(0, last_step);
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

void CGMidi::UnisonMute(int step1, int step2) {
	for (int v = 0; v < v_cnt; v++) {
		// Do not check if instrument does not support unison muting
		if (!icf[instr[v]].unis_mute) continue;
		for (int v2 = v+1; v2 < v_cnt; v2++) {
			if (icf[instr[v]].group != icf[instr[v2]].group) continue;
			if (icf[instr[v]].name != icf[instr[v2]].name) continue;
			for (int i = step1; i <= step2; ++i) {
				if (coff[i][v]) continue;
				if (coff[i][v2]) continue;
				if (note_muted[i][v]) continue;
				if (pause[i][v]) continue;
				if (pause[i][v2]) continue;
				if (note[i][v] != note[i][v2]) continue;
				if (len[i][v] != len[i][v2]) continue;
				note_muted[i][v2] = 1;
				// Increase dynamics of staying note
				for (int x = i; x < i + len[i][v]; ++x) {
					dyn[x][v] = min(127, dyn[x][v] * icf[instr[v]].unis_dyn_mul);
				}
				if (warning_unison_mute < MAX_WARN_UNISON_MUTE) {
					++warning_unison_mute;
					CString est;
					est.Format("Muted note at step %d voice %d because it is unison with voice %d",
						i, v, v2);
					WriteLog(0, est);
				}
			}
		}
	}
}

void CGMidi::MergeSmallOverlaps(int step1, int step2) {
	// Merge small overlaps
	for (int i = step1; i <= step2; ++i) {
		// Cycle through steps to ensure that moved note is checked later
		for (int v = 0; v < v_cnt; ++v) if (icf[instr[v]].poly > 1) {
			// Look for note start
			if (!coff[i][v] && !pause[i][v]) {
				// Do not include dstime/detime in time calculation, because it can change result
				// Do not use playback speed in time calculation, because all calculations are relative in this algorithm
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

	in_ppq = midifile.getTicksPerQuarterNote();
	// ticks per croche
	int tpc = (float)in_ppq / (float)2 / (float)midifile_in_mul;

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
				int tick_dur = mev->getTickDuration();
				float nlen2 = tick_dur;
				int nlen = round((mev->tick + tick_dur) / (float)tpc) - pos;
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
					int nlen = round((mev->tick + tick_dur) / (float)tpc) - pos;
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

	in_ppq = midifile.getTicksPerQuarterNote();
	// ticks per croche
	int tpc = (float)in_ppq / (float)2 / (float)midifile_in_mul;

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
				int tick_dur = mev->getTickDuration();
				float nlen2 = tick_dur;
				int nlen = round((mev->tick + tick_dur) / (float)tpc) - pos;
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
	for (int i = 0; i < icf.size(); i++) {
		st.Format("%d ", icf[i].channel);
		st2 += st;
	}
	est.Format("%s", st2);
	WriteLog(4, est);
}

void CGMidi::AddMidiEvent(long long timestamp, int mm_type, int data1, int data2)
{
	long long real_timestamp = timestamp + midi_start_time + midi_prepause;
	PmEvent event;
	event.timestamp = real_timestamp;
	event.message = Pm_Message(mm_type, data1, data2);
	if (amidi_export) {
		midifile_buf[midi_stage][midi_track].push_back(event);
		return;
	}
	// Check if event is in future
	if (real_timestamp >= midi_sent_t) {
		// If it is not the last SendMIDI, postpone future events
		if ((!midi_last_run) && (real_timestamp > midi_buf_lim)) {
			if (!v_stage[midi_voice] && icf[instr[midi_voice]].port) 
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
			if (!v_stage[midi_voice] && icf[instr[midi_voice]].port) midi_buf.push_back(event);
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
		((sstime[i][v] - sstime[pi][v]) * 100 / m_pspeed + dstime[i][v] - dstime[pi][v]) / 10), ks, 10);
	AddKsOff(stimestamp + min(MAX_TRANS_DELAY,
		((setime[ei][v] - sstime[i][v]) * 100 / m_pspeed + detime[ei][v] - dstime[i][v]) / 10), ks, 0);
}

void CGMidi::AddKs(long long stimestamp, int ks) {
	AddKsOn(stimestamp, ks, 101);
	AddKsOff(stimestamp + 5, ks, 0);
}

void CGMidi::AddTransitionCC(int i, long long stimestamp, int CC, int value1, int value2) {
	int v = midi_voice;
	int pi = i - poff[i][v];
	int ei = i + len[i][v] - 1;
	AddCC(stimestamp - min(MAX_TRANS_DELAY, 
		((sstime[i][v] - sstime[pi][v]) * 100 / m_pspeed + dstime[i][v] - dstime[pi][v]) / 10), CC, value1);
	AddCC(stimestamp + min(MAX_TRANS_DELAY, 
		((setime[ei][v] - sstime[i][v]) * 100 / m_pspeed + detime[ei][v] - dstime[i][v]) / 10), CC, value2);
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

void CGMidi::GetMidiPrePause() {
	midi_prepause = 0;
	long long stimestamp;
  // Check first steps of each voice
	for (int v = 0; v < v_cnt; ++v) {
		for (int i = 0; i < t_sent; ++i) {
			if (!pause[i][v]) {
				stimestamp = sstime[i][v] * 100 / m_pspeed + dstime[i][v];
				midi_prepause = max(midi_prepause, -stimestamp);
				break;
			}
		}
		midi_prepause = max(midi_prepause, icf[instr[v]].mute_predelay);
	}
	midi_prepause += INIT_PRESTEPS * INIT_PRESTEP;
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
	if ((step1 == 0) || (!midi_sent_t) || (!midi_start_time)) {
		midi_first_run = 1;
		GetMidiPrePause();
	}
	else midi_first_run = 0;
	if (midi_first_run) LogInstruments();
	// Set real time when playback started (approximate, because SStime can vary)
	if (!midi_start_time) midi_start_time = timestamp_current + MIDI_BUF_PROTECT - 
		(long long)(stime[step1] / m_pspeed * 100);
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
	// Find last step not too far (approximate, because SStime can vary)
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
	// Calculate midi right limit (approximate, because SStime can vary)
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
			// Subtract prepause, because it is already added in buffer
			AddMidiEvent(mbn[i].timestamp - midi_start_time - midi_prepause, 
				Pm_MessageStatus(mbn[i].message),
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
		// Clear last cc
		last_cc.clear();
		last_cc.resize(128, -1);
		// Initialize voice
		int ei;
		int ncount = 0;
		int ii = instr[v];
		midi_channel = icf[ii].channel;
		midi_channel_saved = midi_channel;
		midi_track = icf[ii].track;
		midi_stage = v_stage[v];
		midi_voice = v;
		midi_current_step = 0;
		// Send initialization commands
		if (midi_first_run) {
			for (auto const& it : icf[ii].InitCommands) {
				AddMidiEvent(midi_sent_t - midi_start_time - midi_prepause, 
					Pm_MessageStatus(it) + midi_channel, 
					Pm_MessageData1(it), Pm_MessageData2(it));
				if (Pm_MessageStatus(it) == MIDI_NOTEON) {
					AddKsOff(midi_sent_t - midi_start_time - midi_prepause + 1,
						Pm_MessageData1(it), 0);
				}
			}
			// Send pan
			AddCC(midi_sent_t - midi_start_time - midi_prepause, 10, 
				(icf[ii].pan * 127) / 100);
			// Send vol
			AddCC(midi_sent_t - midi_start_time - midi_prepause, 7, 
				(icf[ii].vol * icf[ii].vol_default * master_vol) / 10000);
			if (icf[ii].trem_chan > -1) {
				midi_channel = icf[ii].trem_chan;
				// Send pan
				AddCC(midi_sent_t - midi_start_time - midi_prepause, 10, 
					(icf[ii].pan * 127) / 100);
				// Send vol
				AddCC(midi_sent_t - midi_start_time - midi_prepause, 7, 
					(icf[ii].vol * icf[ii].vol_default * master_vol) / 10000);
				midi_channel = midi_channel_saved;
			}
		}
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
				int my_note;
				// Replace note
				if (icf[ii].replace_pitch > -1) my_note = icf[ii].replace_pitch;
				else if (icf[ii].map_pitch.find(note[i][v]) != icf[ii].map_pitch.end()) {
					my_note = icf[ii].map_pitch[note[i][v]];
				}
				else my_note = note[i][v] + play_transpose[v];
				if (artic[i][v] == aTREM) {
					if (icf[ii].trem_replace > -1) {
						my_note = icf[ii].trem_replace;
					}
					if (icf[ii].trem_transpose) {
						my_note += icf[ii].trem_transpose;
					}
					if (icf[ii].map_tremolo.find(note[i][v]) != icf[ii].map_tremolo.end()) {
						my_note = icf[ii].map_tremolo[note[i][v]];
					}
				}
				// Note ON if it is not blocked and was not yet sent
				if (artic[i][v] == aTREM && icf[ii].trem_chan > -1) midi_channel = icf[ii].trem_chan;
				stimestamp = sstime[i][v] * 100 / m_pspeed + dstime[i][v];
				CheckDstime(i, v);
				if ((stimestamp + midi_start_time + midi_prepause >= midi_sent_t) && (i >= midi_sent)) {
					if (!note_muted[i][v]) {
						AddNoteOn(stimestamp, my_note, vel[i][v]);
					}
					if (icf[ii].type == itEIS) {
						// Send bow
						if (GetBit(filter[i][v], fTASTO)) {
							if (icf[ii].NameToKsw.find("Sul tasto") != icf[ii].NameToKsw.end())
								AddKs(stimestamp - 3, icf[ii].NameToKsw["Sul tasto"]);
							if (icf[ii].NameToKsw.find("Harmonics") != icf[ii].NameToKsw.end())
								AddKs(stimestamp - 3, icf[ii].NameToKsw["Harmonics"]);
						}
						else if (GetBit(filter[i][v], fPONT)) {
							AddKs(stimestamp - 3, icf[ii].NameToKsw["Sul ponticello"]);
						}
						else {
							AddKs(stimestamp - 3, icf[ii].NameToKsw["Normal"]);
						}
						// Send slur
						if (artic[i][v] == aSLUR) {
							AddTransitionKs(i, stimestamp, icf[ii].NameToKsw["Slur while held"]);
						}
						// Send staccato
						if (artic[i][v] == aSTAC) {
							AddKs(stimestamp - 3, icf[ii].NameToKsw["Staccato"]);
						}
						else if (artic[i][v] == aPIZZ) {
							AddKs(stimestamp - 3, icf[ii].NameToKsw["Pizzicato"]);
						}
						else if (artic[i][v] == aTREM) {
							AddKs(stimestamp - 3, icf[ii].NameToKsw["Tremolo"]);
						}
						else {
							AddKs(stimestamp - 3, icf[ii].NameToKsw["Sustain"]);
						}
						// Send rebow retrigger
						if (artic[i][v] == aREBOW) {
							AddTransitionCC(i, stimestamp, icf[ii].CC_retrigger, 100, 0);
						}
					}
					// Send transition ks
					if (icf[ii].type == itSMB || icf[ii].type == itSMW) {
						// Frullato
						if (artic[i][v] == aTREM && icf[ii].trem_activate > -1) {
							for (auto const& it : icf[ii].tech[icf[ii].trem_activate]) {
								AddMidiEvent(stimestamp - 3,
									Pm_MessageStatus(it) + midi_channel,
									Pm_MessageData1(it), Pm_MessageData2(it));
								if (Pm_MessageStatus(it) == MIDI_NOTEON) {
									AddKsOff(stimestamp + 3,
										Pm_MessageData1(it), 0);
								}
							}
						}
						// Open
						if (artic[i][v] != aTREM && icf[ii].trem_deactivate > -1) {
							for (auto const& it : icf[ii].tech[icf[ii].trem_deactivate]) {
								AddMidiEvent(stimestamp - 3,
									Pm_MessageStatus(it) + midi_channel,
									Pm_MessageData1(it), Pm_MessageData2(it));
								if (Pm_MessageStatus(it) == MIDI_NOTEON) {
									AddKsOff(stimestamp + 3,
										Pm_MessageData1(it), 0);
								}
							}
						}
					}
					if (icf[ii].type == itSMB) {
						// Mute
						if (GetBit(filter[i][v], fMUTE) && icf[ii].mute_activate > -1) {
							for (auto const& it : icf[ii].tech[icf[ii].mute_activate]) {
								AddMidiEvent(stimestamp - icf[ii].mute_predelay,
									Pm_MessageStatus(it) + midi_channel,
									Pm_MessageData1(it), Pm_MessageData2(it));
								if (Pm_MessageStatus(it) == MIDI_NOTEON) {
									AddKsOff(stimestamp - icf[ii].mute_predelay + 1,
										Pm_MessageData1(it), 0);
								}
							}
						}
						// Open
						if (!GetBit(filter[i][v], fMUTE) && icf[ii].mute_deactivate > -1) {
							for (auto const& it : icf[ii].tech[icf[ii].mute_deactivate]) {
								AddMidiEvent(stimestamp - icf[ii].mute_predelay,
									Pm_MessageStatus(it) + midi_channel,
									Pm_MessageData1(it), Pm_MessageData2(it));
								if (Pm_MessageStatus(it) == MIDI_NOTEON) {
									AddKsOff(stimestamp - icf[ii].mute_predelay + 1,
										Pm_MessageData1(it), 0);
								}
							}
						}
						if (artic[i][v] == aSPLITPO_CHROM) {
							AddTransitionKs(i, stimestamp, icf[ii].ks1 + 12);
							AddTransitionKs(i, stimestamp, icf[ii].ks1 + 0);
						}
						if (artic[i][v] == aSPLITPO_MIX) {
							AddTransitionKs(i, stimestamp, icf[ii].ks1 + 12);
							AddTransitionKs(i, stimestamp, icf[ii].ks1 + 1);
						}
						if (artic[i][v] == aSPLITPO_ARAB) {
							AddTransitionKs(i, stimestamp, icf[ii].ks1 + 12);
							AddTransitionKs(i, stimestamp, icf[ii].ks1 + 3);
						}
						if (artic[i][v] == aSPLITPO_PENT) {
							AddTransitionKs(i, stimestamp, icf[ii].ks1 + 12);
							AddTransitionKs(i, stimestamp, icf[ii].ks1 + 4);
						}
						if (artic[i][v] == aGLISS) {
							AddTransitionKs(i, stimestamp, icf[ii].ks1 + 12);
							AddTransitionKs(i, stimestamp, icf[ii].ks1 + 2);
						}
						if (artic[i][v] == aGLISS2) {
							AddTransitionKs(i, stimestamp, icf[ii].ks1 + 12);
							AddTransitionKs(i, stimestamp, icf[ii].ks1 + 5);
						}
					}
				}
				// Note OFF if it is in window
				if (ei <= step22) {
					// Note OFF
					// ndur = (etime[ei] - stime[i]) * 100 / m_pspeed + detime[ei][v] - dstime[i][v];
					etimestamp = setime[ei][v] * 100 / m_pspeed + detime[ei][v];
					AddNoteOff(etimestamp, my_note, 0);
					// Send note ending ks
					if (icf[ii].type == itSMB) {
						if (artic[ei][v] == aEND_SFL) {
							AddKs(etimestamp - icf[ii].end_sfl_dur, icf[ii].ks1 + 11);
						}
						if (artic[ei][v] == aEND_PBD) {
							AddKs(etimestamp - icf[ii].end_pbd_dur, icf[ii].ks1 + 4);
						}
						if (artic[ei][v] == aEND_VIB2) {
							AddKs(etimestamp - icf[ii].end_vib2_dur, icf[ii].ks1 + 6);
						}
						if (artic[ei][v] == aEND_VIB) {
							AddKs(etimestamp - icf[ii].end_vib_dur, icf[ii].ks1 + 5);
						}
					}
				}
				midi_channel = midi_channel_saved;
			}
			// Go to next note
			if (noff[i][v] == 0) break;
			i += noff[i][v];
		}
		// Send CC
		if (icf[ii].trem_chan > -1) midi_channel = icf[ii].trem_chan;
		InterpolateCC(icf[ii].CC_dyn, icf[ii].rnd_dyn, step1, step22, dyn, ii, v);
		InterpolateCC(icf[ii].CC_vib, icf[ii].rnd_vib, step1, step22, vib, ii, v);
		InterpolateCC(icf[ii].CC_vibf, icf[ii].rnd_vibf, step1, step22, vibf, ii, v);
		SendPedalCC(step1, step22, ii, v);
		midi_channel = midi_channel_saved;
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
	st.Format("MIDI write %d (%d postponed) events: steps %d/%d - %d/%d (%lld to %lld ms) [to future %lld to %lld ms] (in %lld ms) playback is at %lld ms. Limit %lld. Last postponed %lld. Step22 stopped increasing at %.0f ms. Start time: %lld, current time: %lld. Prepause %.0f ms",
		midi_buf.size(), midi_buf_next.size(), step21, step1, step22, step2, 
		midi_sent_t-midi_start_time, midi_sent_t2 - midi_start_time, 
		midi_sent_t - timestamp_current, midi_sent_t2 - timestamp_current,
		time_stop - time_start, timestamp_current - midi_start_time, midi_buf_lim - midi_start_time, 
		midi_sent_t3 - midi_start_time,	time, midi_start_time, timestamp_current, midi_prepause);
	WriteLog(4, st);
	// Save last sent position
	midi_sent = step22;
	midi_sent_t = midi_sent_t2;
	midi_sent_msg = midi_sent_msg2;
	if (!amidi_export) {
		mutex_output.unlock();
	}
}

void CGMidi::SendPedalCC(int step1, int step2, int ii, int v) {
	for (int i = step1; i < step2; i++) {
		if (GetBit(filter[i][v], fPEDAL)) {
			long long stimestamp = sstime[i][v] * 100 / m_pspeed + dstime[i][v];
			AddCC(stimestamp, 64, 127);
		}
		if (GetBit(filter[i][v], fUNPEDAL)) {
			long long stimestamp = sstime[i][v] * 100 / m_pspeed + dstime[i][v];
			AddCC(stimestamp, 64, 0);
		}
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
		fsteps = (float)(icf[ii].CC_steps) / 1000.0 * (etime[i] - stime[i]);
		steps = max(1, fsteps);
		pre_cc += steps;
		if (pre_cc > icf[ii].CC_ma) {
			first_step = i;
			break;
		}
	}
	for (int i = first_step; i < step2; i++) {
		if (i < 0) continue;
		midi_current_step = i;
		// Get CC steps count
		fsteps = (float)(icf[ii].CC_steps) / 1000.0 * (etime[i] - stime[i]);
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
	int CC_ma2 = icf[ii].CC_ma / 2;
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
		cc_ma[c] = cc_ma[c - 1] + (cc_lin[c + CC_ma2] - cc_lin[c  - CC_ma2 - 1]) / (float)(icf[ii].CC_ma);
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
	int is_first_cc = 1;
	// Send ma CC
	for (int c = first_cc; c <= last_cc; c++) {
		float t = cc_time[c];
		if (t >= midi_sent_t - midi_start_time + midi_prepause) {
			if (is_first_cc) {
				if (midi_first_run) AddCC(midi_sent_t - midi_start_time - midi_prepause,
					CC, cc_ma[c]);
				is_first_cc = 0;
			}
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
		long long searchElement = CGLib::time() - midi_start_time - midi_prepause;
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
	if ((data1 < icf[instr[midi_voice]].nmin) || (data1 > icf[instr[midi_voice]].nmax)) {
		if (warning_note_wrong[midi_voice] < 4) {
			CString st;
			st.Format("Blocked note %d/%d step %d time %lld in voice %d instrument %d out of range %d-%d",
				data1, data2, midi_current_step, timestamp, midi_voice, instr[midi_voice], 
				icf[instr[midi_voice]].nmin, icf[instr[midi_voice]].nmax);
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
	if ((data1 >= icf[instr[midi_voice]].nmin) && (data1 <= icf[instr[midi_voice]].nmax)) {
		if (warning_note_wrong[midi_voice] < 4) {
			CString st;
			st.Format("Blocked keyswitch %d/%d time %lld in voice %d instrument %d %s in note range %d-%d",
				data1, data2, timestamp, midi_voice, instr[midi_voice], icf[instr[midi_voice]].group, 
				icf[instr[midi_voice]].nmin, icf[instr[midi_voice]].nmax);
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
	if ((data1 < icf[instr[midi_voice]].nmin) || (data1 > icf[instr[midi_voice]].nmax)) {
		if (warning_note_wrong[midi_voice] < 4) {
			CString st;
			st.Format("Blocked note %d/%d time %lld in voice %d instrument %d out of range %d-%d",
				data1, data2, timestamp, midi_voice, instr[midi_voice], 
				icf[instr[midi_voice]].nmin, icf[instr[midi_voice]].nmax);
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
	if ((data1 >= icf[instr[midi_voice]].nmin) && (data1 <= icf[instr[midi_voice]].nmax)) {
		if (warning_note_wrong[midi_voice] < 4) {
			CString st;
			st.Format("Blocked keyswitch %d/%d time %lld in voice %d instrument %d %s in note range %d-%d",
				data1, data2, timestamp, midi_voice, instr[midi_voice], icf[instr[midi_voice]].group,
				icf[instr[midi_voice]].nmin, icf[instr[midi_voice]].nmax);
			WriteLog(5, st);
			warning_note_wrong[midi_voice] ++;
		}
		return;
	}
	AddMidiEvent(timestamp, MIDI_NOTEOFF + midi_channel, data1, data2);
}

void CGMidi::AddCC(long long timestamp, int data1, int data2) {
	if (last_cc[data1] == data2) return;
	last_cc[data1] = data2;
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
