#include "../stdafx.h"
#include "ExcelXML.h"
#include "GLib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CExcelXML::CExcelXML() {
}

CExcelXML::~CExcelXML() {
}

// Problems with manual parse solution:
// - Text inside quotes can interfere with detection
// - Case sensitive
// - Whitespace not allowed
// - Properties are attached to search tags directly
int CExcelXML::LoadXML(CString fname) {
	if (!CGLib::fileExists(fname)) {
		error_st.Format("LoadXML cannot find file: %s", fname);
		return 1;
	}
	CGLib::read_file_st(fname, fst);
	LoadStyles();
	LoadCells();
	return 0;
}

int CExcelXML::ReportError(int pos, CString st) {
	st.Replace("##", "%d: %s");
	error_st.Format(st, pos, fst.Mid(pos, 100));
	return 1;
}

// Find tag1 and tag2 in st starting from st and return found positions (pos1, pos2) and string in between (tst)
int CExcelXML::GetTagSt(CString &st, CString &tst, int pos, CString tag1, CString tag2, int &pos1, int &pos2) {
	// Find first tag
	pos1 = st.Find(tag1, pos);
	if (pos1 == -1) return 1;
	pos1 += tag1.GetLength();
	// Find second tag
	pos2 = st.Find(tag2, pos1);
	int pos3, pos4;
	pos3 = st.Find(">", pos1);
	pos4 = st.Find("/>", pos1);
	// If tag brief-closes before closing tag or there is no closing tag
	if (pos4 != -1 && (pos4 < pos2 || pos2 == -1)) {
		// Do not rewrite close if tag finishes before brief-close (foreign brief-close)
		if (pos4 < pos3) pos2 = pos4;
	}
	if (pos2 == -1) return 1;
	tst = st.Mid(pos1, pos2 - pos1);
	return 0;
}

int CExcelXML::LoadStyles() {
	CString sst1 = "<Style ss:ID=\"";
	CString sst2 = "</Style>";
	CString sst3 = " ss:Bold=\"1";
	CString sst4 = " ss:StrikeThrough=\"1";
	CString sst5 = " ss:Italic=\"1";
	CString sst6 = "<Interior ss:Color=\"#";
	CString sst7 = " ss:Size=\"";
	CString sst8 = "\" ss:Color=\"#";
	int pos = 0;
	int pos1, pos2, pos3, pos4, pos5, pos6;
	s.clear();
	CString st, st2, st3, sst, sname;
	for (int i = 0; i < INT_MAX; ++i) {
		ExcelCell style;
		if (GetTagSt(fst, sst, pos, sst1, sst2, pos1, pos2)) break;
		// Get style id
		pos4 = sst.Find("\"");
		sname = sst.Mid(0, pos4);
		// Get properties
		pos4 = sst.Find(sst3);
		if (pos4 > 0) style.bold = 1;
		pos4 = sst.Find(sst4);
		if (pos4 > 0) style.strike = 1;
		pos4 = sst.Find(sst5);
		if (pos4 > 0) style.italic = 1;
		// Color
		pos4 = sst.Find(sst8);
		if (pos4 > 0) {
			pos5 = pos4 + sst8.GetLength();
			pos6 = sst.Find("\"", pos5);
			st2 = sst.Mid(pos5, pos6 - pos5);
			style.col = strtol(st2, NULL, 16);
		}
		// Background color
		pos4 = sst.Find(sst6);
		if (pos4 > 0) {
			pos5 = pos4 + sst6.GetLength();
			pos6 = sst.Find("\"", pos5);
			st2 = sst.Mid(pos5, pos6 - pos5);
			style.bcol = strtol(st2, NULL, 16);
		}
		// Font size
		pos4 = sst.Find(sst7);
		if (pos4 > 0) {
			pos5 = pos4 + sst7.GetLength();
			pos6 = sst.Find("\"", pos5);
			style.size = atoi(sst.Mid(pos5, pos6 - pos5));
		}
		// Save style
		s.insert(pair<CString, ExcelCell>(sname, style));
		pos = pos2 + 1;
	}
	return 0;
}

int CExcelXML::LoadCells() {
	CString sst1 = "<Worksheet ss:Name=\"";
	CString sst2 = "</Worksheet>";
	CString sst3 = "<Row ";
	CString sst4 = "</Row>";
	CString sst5 = "<Cell";
	CString sst6 = "</Cell>";
	CString sst7 = "<Data ss:Type=\"";
	CString sst8 = "</Data>";
	CString sst9 = "><Comment";
	CString sst10 = "</Comment>";
	CString sst11 = " ss:Index=\"";
	CString sst12 = " ss:MergeAcross=\"";
	CString sst13 = " ss:StyleID=\"";
	int wpos = 0;
	int rpos, cpos, pos1, pos2, pos3, pos4, pos5, pos6, pos7, pos8, pos9, pos10, pos11;
	int pos12, pos13;
	int slen;
	d.clear();
	sheet_id.clear();
	CString st, st2, st3, wname;
	// Load worksheets
	for (int w = 0; w < INT_MAX; ++w) {
		CString wst;
		if (GetTagSt(fst, wst, wpos, sst1, sst2, pos1, pos2)) break;
		d.resize(w + 1);
		// Get sheet name
		pos11 = wst.Find("\"");
		if (pos11 > -1) {
			wname = wst.Left(pos11);
			sheet_id.insert(pair<CString, int>(wname, w));
		}
		// Load rows
		rpos = 0;
		for (int r = 0; r < INT_MAX; ++r) {
			CString rst;
			if (GetTagSt(wst, rst, rpos, sst3, sst4, pos3, pos4)) break;
			d[w].resize(r + 1);
			// Load cells
			cpos = 0;
			for (int c = 0; c < INT_MAX; ++c) {
				CString cst, dst, mst, ist, ist2;
				if (GetTagSt(rst, cst, cpos, sst5, sst6, pos5, pos6)) break;
				d[w][r].resize(c + 1);
				// Data
				GetTagSt(cst, dst, 0, sst7, sst8, pos7, pos8);
				// Index
				if (!GetTagSt(cst, ist, 0, sst11, "\"", pos12, pos13)) {
					int c2 = atoi(ist) - 1;
					d[w][r].resize(c2 + 1);
					c = c2;
				}
				// Style
				if (!GetTagSt(cst, ist, 0, sst13, "\"", pos12, pos13)) {
					if (s.find(ist) != s.end()) d[w][r][c] = s[ist];
				}
				// Remove first tag close
				pos11 = dst.Find(">");
				if (pos11 > -1) dst = dst.Mid(pos11 + 1);
				d[w][r][c].val = dst;
				// Comment
				if (!GetTagSt(cst, mst, 0, sst9, sst10, pos9, pos10)) {
					// Remove first tag close
					pos11 = mst.Find(">");
					if (pos11 > -1) mst = mst.Mid(pos11 + 1);
					d[w][r][c].com = CGLib::reg_replace(mst, "\<[^\>]*\>", "");
				}
				// Merge across
				if (!GetTagSt(cst, ist2, 0, sst12, "\"", pos12, pos13)) {
					int c2 = c + atoi(ist2);
					d[w][r].resize(c2 + 1);
					c = c2;
				}
				cpos = pos6 + 1;
			}
			rpos = pos4 + 1;
		}
		wpos = pos2 + 1;
	}
	return 0;
}
