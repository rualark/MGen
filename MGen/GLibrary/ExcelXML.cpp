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
	CString sst4 = "<Cell ss:StyleID=\"";
	CString sst5 = "<Data ss:Type=\"";
	CString sst6 = "><Comment";
	CString sst7 = "</Comment>";
	CString sst8 = "";
	CString sst9 = "";
	int pos = 0;
	int pos1, pos2, pos3, pos4, pos5, pos6;
	int slen;
	s.clear();
	CString st, st2, st3, sst, sname;
	for (int w = 0; w < INT_MAX; ++w) {
		if (GetTagSt(fst, sst, pos, sst1, sst2, pos1, pos2)) break;
	}
	return 0;
}
