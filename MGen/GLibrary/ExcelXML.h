#pragma once

struct ExcelCell {
	CString val; // value
	CString com; // comment
	//unsigned short int mergeAcross = 0;
	//unsigned short int mergeDown = 0;
	
	// Style
	DWORD col = 0; // color
	DWORD bcol = 0; // background color
	unsigned short int size = 0; // font size
	bool bold : 1;
	bool italic : 1;
	bool strike : 1;
	bool underline : 1;
	ExcelCell() : bold(), italic(), strike(), underline() {}
};

class CExcelXML
{
public:
	CExcelXML();
	~CExcelXML();

	int LoadXML(CString fname);
	int ReportError(int pos, CString st);
	int GetTagSt(CString & st, CString & tst, int pos, CString tag1, CString tag2, int & pos1, int & pos2);
	int LoadStyles();
	int LoadCells();

public:
	map<CString, int> sheet_id; // Sheet id by name
	map<CString, ExcelCell> s; // Styles by name
	vector<vector<vector<ExcelCell>>> d; // [sheet][row][col] Resulting cells

	CString fst; // String read from file
	CString error_st;
};
