// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../stdafx.h"
#include "GLib.h"
#include "SmRnd.h"
#include "VSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW 
#endif

char* CGLib::APP_VERSION = "Undefined";
long long CGLib::first_time = 0;
int CGLib::can_send_log = 1;
int CGLib::show_lining = 1;
HWND CGLib::m_hWnd = 0;
int CGLib::debug_level = 1;
int CGLib::m_ci = 1;
int CGLib::error = 0;
int CGLib::m_testing = 0;
int CGLib::m_test_sec = 5;
CString CGLib::m_cline2 = "";
int CGLib::exitcode = 0;
int CGLib::parameter_found = 0;
int CGLib::play_enabled = 1;
UINT CGLib::WM_DEBUG_MSG = 0;
UINT CGLib::WM_STATUS_MSG = 0;
timed_mutex CGLib::mutex_log;
vector<CString> CGLib::oinfo; // Strings of algorithm output status
vector<int> CGLib::oinfo_changed; // If string changed
vector<deque<CString>> CGLib::log_buffer;
vector<int> CGLib::warn_log_buffer;
int CGLib::warn_memory_usage = 0;
vector<int> CGLib::log_buffer_size;
vector<long long> CGLib::status_updates;
vector<long long> CGLib::logs_sent;

/* if (flag!=0), then use the contents of randrsl[] to initialize rmm[]. */
#define mix(a,b,c,d,e,f,g,h) \
{ \
	a ^= b << 11; d += a; b += c; \
	b ^= c >> 2;  e += b; c += d; \
	c ^= d << 8;  f += c; d += e; \
	d ^= e >> 16; g += d; e += f; \
	e ^= f << 10; h += e; f += g; \
	f ^= g >> 4;  a += f; g += h; \
	g ^= h << 8;  b += g; h += a; \
	h ^= a >> 9;  c += h; a += b; \
}

CGLib::CGLib() {
	error = 0;
	logs.clear();
	logs.resize(LOG_TABS);
	fill(begin(randrsl), end(randrsl), (ub4)0);
	fill(begin(rmm), end(rmm), (ub4)0);
}

CGLib::~CGLib() {
}

int CGLib::FileHasHeader(CString fname, CString header) {
	int retCode = 1;
	CString st;
	ifstream fs;
	// Check file exists
	if (!fileExists(fname)) {
		st.Format("Cannot open file to check header: %s", fname);
		WriteLog(5, st);
		return 0;
	}
	fs.open(fname);
	char pch[2550];
	// Load header
	fs.read(pch, header.GetLength());
	if (fs.gcount() < header.GetLength()) retCode = 0;
	fs.close();
	if (!retCode) return 0;
	pch[header.GetLength()] = '\0';
	st = pch;
	if (st != header) return 0;
	return 1;
}

// Run process with parameters, wait for delay (maximum), return exit code
DWORD CGLib::Run(CString fname, CString par, int delay) {
	DWORD ecode;
	SHELLEXECUTEINFO sei = { 0 };
	sei.cbSize = sizeof(SHELLEXECUTEINFO);
	sei.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI;
	sei.hwnd = NULL;
	sei.lpVerb = NULL;
	sei.lpFile = fname;
	sei.lpParameters = par;
	sei.lpDirectory = NULL;
	sei.nShow = SW_SHOWNORMAL;
	sei.hInstApp = NULL;
	ShellExecuteEx(&sei);
	if (delay) WaitForSingleObject(sei.hProcess, delay);
	if (!GetExitCodeProcess(sei.hProcess, &ecode)) ecode = 102;
	return ecode;
}

long long CGLib::time() {
	long long t = abstime();
	if (!first_time) first_time = t;
	return t - first_time;
}

// Get file last write time
FILETIME CGLib::fileTime(CString fname) {
	HANDLE hFile = CreateFile(fname, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	FILETIME creationTime, lpLastAccessTime, lastWriteTime;
	bool err = 
	GetFileTime(hFile, &creationTime, &lpLastAccessTime, &lastWriteTime);
	CloseHandle(hFile);
	// If file does not exist, garbage is returned
	return lastWriteTime;
}

void CGLib::start_time() {
	long long t = abstime();
	first_time = t;
}

long long CGLib::abstime() {
	milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
	return ms.count();
}

void CGLib::CheckVar(CString * sName, CString * sValue, char* sSearch, int * Dest, int lmin, int lmax)
{
	if (*sName == sSearch) {
		++parameter_found;
		*Dest = atoi(*sValue);
		CheckLimits(sName, Dest, lmin, lmax);
	}
}

void CGLib::LoadVar(CString * sName, CString * sValue, char* sSearch, long long * Dest)
{
	if (*sName == sSearch) {
		++parameter_found;
		*Dest = atoll(*sValue);
	}
}

void CGLib::CheckVar(CString * sName, CString * sValue, char* sSearch, float * Dest, float lmin, float lmax)
{
	if (*sName == sSearch) {
		++parameter_found;
		*Dest = atof(*sValue);
		CheckLimits(sName, Dest, lmin, lmax);
	}
}

void CGLib::LoadVar(CString * sName, CString * sValue, char* sSearch, CString * Dest)
{
	if (*sName == sSearch) {
		++parameter_found;
		*Dest = *sValue;
	}
}

void CGLib::StringToVector(CString *sValue, CString stDelim, vector<int> & Dest, int lmin, int lmax) {
	int pos = 0;
	CString st;
	for (int i = 0; i<1000; i++) {
		st = sValue->Tokenize(stDelim, pos);
		st.Trim();
		if (st.IsEmpty()) break;
		if (i >= Dest.size()) {
			CString est;
			est.Format("Cannot load more than %d values into vector. String: '%s'.", Dest.size(), *sValue);
			WriteLog(5, est);
			continue;
		}
		Dest[i] = atoi(st);
		CheckLimits(sValue, &(Dest[i]), lmin, lmax);
	}
}

void CGLib::LoadVectorPar(CString * sName, CString * sValue, char* sSearch, vector<int> & Dest, int lmin, int lmax)
{
	if (*sName == sSearch) {
		++parameter_found;
		int pos = 0;
		CString st;
		for (int i = 0; i<1000000; i++) {
			st = sValue->Tokenize(",", pos);
			st.Trim();
			if (st.IsEmpty()) break;
			if (!atoi(st) && st != "0") {
				WriteLog(5, "Wrong format of parameter value in config line: " + *sName + " = " + *sValue);
				error = 101;
			}
			if (i >= Dest.size()) {
				CString est;
				est.Format("Cannot load more than %d values into vector named '%s'. String: '%s'.", Dest.size(), *sName, *sValue);
				WriteLog(5, est);
				return;
			}
			Dest[i] = atoi(st);
			CheckLimits(sName, &(Dest[i]), lmin, lmax);
		}
	}
}

void CGLib::PushVectorPar(CString * sName, CString * sValue, char* sSearch, vector<int> & Dest, int lmin, int lmax)
{
	if (*sName == sSearch) {
		++parameter_found;
		int pos = 0;
		CString st;
		Dest.clear();
		for (int i = 0; i<1000000; i++) {
			st = sValue->Tokenize(",", pos);
			st.Trim();
			if (st.IsEmpty()) break;
			if (!atoi(st) && st != "0") {
				WriteLog(5, "Wrong format of parameter value in config line: " + *sName + " = " + *sValue);
				error = 101;
			}
			Dest.push_back(atoi(st));
			CheckLimits(sName, &(Dest[i]), lmin, lmax);
		}
	}
}

// Increment vector values with indexes specified in string, separated by commas
void CGLib::LoadVectorPar2(CString * sName, CString * sValue, char* sSearch, vector<int> & Dest, int value, int lmin, int lmax)
{
	int x;
	if (*sName == sSearch) {
		++parameter_found;
		int pos = 0;
		CString st;
		for (int i = 0; i<1000000; i++) {
			st = sValue->Tokenize(",", pos);
			st.Trim();
			if (st.IsEmpty()) break;
			x = atoi(st);
			if (x >= Dest.size()) {
				CString est;
				est.Format("Cannot increment value with index %d in vector with size %d named '%s'. String: '%s'.", x, Dest.size(), *sName, *sValue);
				WriteLog(5, est);
				return;
			}
			CheckLimits(sName, &x, lmin, lmax);
			Dest[x] += value;
		}
	}
}

void CGLib::CheckLimits(CString * sName, int * Dest, int lmin, int lmax)
{
	if (lmin != -1000000) {
		if (*Dest < lmin) {
			CString st;
			st.Format("Variable %s is %d (below minimum allowed value %d) - corrected to %d", *sName, *Dest, lmin, lmin);
			WriteLog(5, st);
			*Dest = lmin;
		}
	}
	if (lmax != -1000000) {
		if (*Dest > lmax) {
			CString st;
			st.Format("Variable %s is %d (above maximum allowed value %d) - corrected to %d", *sName, *Dest, lmax, lmax);
			WriteLog(5, st);
			*Dest = lmax;
		}
	}
}

void CGLib::CheckLimits(CString * sName, float * Dest, float lmin, float lmax)
{
	if (lmin != -1000000) {
		if (*Dest < lmin) {
			*Dest = lmin;
			CString st;
			st.Format("Variable %s is %f (below minimum allowed value %f) - corrected to %f", *sName, *Dest, lmin, lmin);
			WriteLog(5, st);
		}
	}
	if (lmax != -1000000) {
		if (*Dest > lmax) {
			*Dest = lmax;
			CString st;
			st.Format("Variable %s is %f (above maximum allowed value %f) - corrected to %f", *sName, *Dest, lmax, lmax);
			WriteLog(5, st);
		}
	}
}

void CGLib::CheckRange(CString * sName, CString * sValue, char* sSearch, int * vmin, int* vmax, int lmin, int lmax)
{
	if (*sName == sSearch) {
		++parameter_found;
		int pos = sValue->Find("-");
		if (pos == -1) {
			CString est;
			est.Format("Error parsing range variable '%s'. Range format must be 'X-Y'. Not found '-' symbol in string '%s'.", *sName, *sValue);
			WriteLog(5, est);
		}
		else {
			CString st, st2;
			// Get min and max values
			st = sValue->Left(pos);
			st2 = sValue->Mid(pos + 1);
			// Trim
			st.Trim();
			st2.Trim();
			// Load values
			*vmin = min(atoi(st), atoi(st2));
			*vmax = max(atoi(st), atoi(st2));
			CheckLimits(sName, vmin, lmin, lmax);
			CheckLimits(sName, vmax, lmin, lmax);
		}
	}
}

void CGLib::CheckRange(CString * sName, CString * sValue, char* sSearch, float * vmin, float * vmax, float lmin, float lmax)
{
	if (*sName == sSearch) {
		++parameter_found;
		int pos = sValue->Find("-");
		if (pos == -1) {
			CString est;
			est.Format("Error parsing range variable '%s'. Range format must be 'X-Y'. Not found '-' symbol in string '%s'.", *sName, *sValue);
			WriteLog(5, est);
		}
		else {
			CString st, st2;
			// Get min and max values
			st = sValue->Left(pos);
			st2 = sValue->Mid(pos + 1);
			// Trim
			st.Trim();
			st2.Trim();
			// Load values
			*vmin = atof(st);
			*vmax = atof(st2);
			CheckLimits(sName, vmin, lmin, lmax);
			CheckLimits(sName, vmax, lmin, lmax);
		}
	}
}

void CGLib::Tokenize(const CString& s, vector<CString>& tokens, const CString &delim)
{
	CString s2 = s;
	int pos;
	int end;
	tokens.clear();

	while (!s2.IsEmpty())
	{
		pos = s2.FindOneOf(delim);
		end = pos;
		if (end == -1) end = s2.GetLength();
		if (end) {
			tokens.push_back(s2.Left(end));
		}
		else {
			tokens.push_back("");
		}
		s2.Delete(0, end + 1);
		// If last token saved, but last character was delimiter, push empty token
		if (s2.IsEmpty() && pos != -1) tokens.push_back("");
	}
}

void CGLib::GetVint(const CString & st, vector<int>& res) {
	CString st2;
	int sign = 1;
	for (int i = 0; i < st.GetLength(); ++i) {
		if (isdigit(st[i])) {
			// Check minor
			if (st2.IsEmpty() && i > 0 && (st[i - 1] == 'm' || st[i-1] == 'b')) sign = -1;
			st2 += st[i];
		}
		else if (!st2.IsEmpty()) {
			res.push_back(atoi(st2));
			st2.Empty();
		}
	}
	if (st2 != "") {
		res.push_back(sign*atoi(st2));
	}
}

int CGLib::CheckInclude(CString st, CString fname, CString &iname) {
	iname.Empty();
	if (st.Left(8) == "include ") {
		st = st.Mid(8);
		iname = GetLinkedPath(st, fname);
		return 1;
	}
	return 0;
}

CString CGLib::GetLinkedPath(CString st, CString fname) {
	st.Replace("\"", "");
	st.Replace("/", "\\");
	st.Trim();
	char szDir[1024];
	char *fileExt;
	CString st4 = dir_from_path(fname) + "\\" + st;
	GetFullPathName(st4, 1024, szDir, &fileExt);
	return szDir;
}

void CGLib::LoadNote(CString * sName, CString * sValue, char* sSearch, int * Dest)
{
	if (*sName == sSearch) {
		++parameter_found;
		*Dest = CGLib::GetNoteI(*sValue);
		++parameter_found;
	}
}

CString CGLib::FormatTime(int sec)
{
	CString st;
	int hours = sec / 3600;
	int minutes = (sec - hours*3600) / 60;
	int seconds = sec % 60;
	if (hours > 0) st.Format("%02d:%02d:%02d", hours, minutes, seconds);
	else st.Format("%02d:%02d", minutes, seconds);
	return st;
}

void CGLib::copy_file(CString sName, CString dName) {
	std::ifstream  src(sName, std::ios::binary);
	std::ofstream  dst(dName, std::ios::binary);
	dst << src.rdbuf();
	src.close();
	dst.close();
}

void CGLib::write_file_sv(ofstream &fs, vector<CString> &sv) {
	for (int i = 0; i < sv.size(); ++i) {
		fs << sv[i] << "\n";
	}
}

void CGLib::read_file_sv(CString fname, vector<CString> &sv) {
	// Load file
	ifstream fs;
	if (!fileExists(fname)) {
		WriteLog(5, "Cannot find file " + fname);
		return;
	}
	fs.open(fname);
	char pch[2550];
	CString st2;
	sv.clear();
	while (fs.good()) {
		fs.getline(pch, 2550);
		st2 = pch;
		sv.push_back(st2);
	}
	fs.close();
}

void CGLib::AppendLineToFile(CString fname, CString st)
{
	ofstream outfile;
	outfile.open(fname, ios_base::app);
	outfile << st;
	outfile.close();
}

void CGLib::OverwriteFile(CString fname, CString st)
{
	ofstream outfile;
	outfile.open(fname);
	outfile << st;
	outfile.close();
}

// Appends line number 'line' in file with st
void CGLib::AppendLineInFile(CString fname, int line, CString st) {
	// Load file
	ifstream fs;
	if (!fileExists(fname)) {
		WriteLog(5, "Cannot find file " + fname);
		return;
	}
	fs.open(fname);
	char pch[2550];
	int pos = 0;
	int x = 0;
	CString st2;
	vector<CString> ast;
	while (fs.good()) {
		fs.getline(pch, 2550);
		st2 = pch;
		ast.push_back(st2);
	}
	fs.close();
	// Append string
	if (line > ast.size() - 1) {
		CString est;
		est.Format("There is no line %d in file '%s': cannot append (total %d line in file)",
			line, fname, ast.size());
		WriteLog(5, est);
		return;
	}
	ast[line] += st;
	// Write file
	ofstream outfile;
	outfile.open(fname, ios_base::trunc);
	for (int i = 0; i < ast.size(); ++i) {
		outfile << ast[i] << "\n";
	}
	outfile.close();
}

bool CGLib::dirExists(CString dirName_in)
{
	DWORD ftyp = GetFileAttributesA(dirName_in);
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false;  //something is wrong with your path!

	if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
		return true;   // this is a directory!

	return false;    // this is not a directory!
}

bool CGLib::fileExists(CString dirName_in)
{
	DWORD ftyp = GetFileAttributesA(dirName_in);
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false;  //something is wrong with your path!

	if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
		return false;   // this is a directory!

	return true;    // this is not a directory!
}

bool CGLib::nodeExists(CString dirName_in)
{
	DWORD ftyp = GetFileAttributesA(dirName_in);
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false;  //something is wrong with your path!

	return true;    // file exists
}

CString CGLib::ext_from_path(CString path)
{
	string::size_type pos2 = string(path).find_last_of("./");
	CString path2 = string(path).substr(pos2 + 1, path.GetLength()).c_str();
	return path2;
}

CString CGLib::noext_from_path(CString path)
{
	string::size_type pos2 = string(path).find_last_of("./");
	CString path2 = string(path).substr(0, pos2).c_str();
	return path2;
}

CString CGLib::fname_from_path(CString path)
{
	string::size_type pos = string(path).find_last_of("\\/");
	CString path2 = string(path).substr(pos + 1).c_str();
	return path2;
}

CString CGLib::bname_from_path(CString path)
{
	string::size_type pos = string(path).find_last_of("\\/");
	string::size_type pos2 = string(path).find_last_of("./");
	CString path2 = string(path).substr(pos + 1, pos2 - pos - 1).c_str();
	return path2;
}

CString CGLib::dir_from_path(CString path)
{
	string::size_type pos = string(path).find_last_of("\\/");
	CString path2 = string(path).substr(0, pos).c_str();
	return path2;
}

CString CGLib::GetNoteName(int n)
{
	CString st;
	st.Format("%d", n / 12 - 1);
	return NoteName[n % 12] + st;
}

int CGLib::GetNoteI(CString &st)
{
	st.Replace("'", "");
	if (isdigit(st[0])) {
		return atoi(st);
	}
	else {
		int nid = -1;
		int pos = st.GetLength()-1;
		while (pos > 0 && (isdigit(st[pos-1]) || st[pos-1] == '-')) --pos;
		//if (isdigit(st[1])) pos = 1;
		CString nname = st.Left(pos);
		int noct = atoi(st.Mid(pos));
		for (int i = 0; i < 12; i++) {
			if (NoteName[i] == nname) {
				nid = i;
				break;
			}
			if (NoteName2[i] == nname) {
				nid = i;
				break;
			}
		}
		if ((nid > -1) && (isdigit(st[st.GetLength() - 1]))) {
			int i = nid + (noct + 1) * 12;
			//CString est;
			//est.Format("Converted note name %s to %d", st, i);
			//WriteLog(1, est);
			return i;
		} else {
			CString est;
			if (nid > -1)	est.Format("Error parsing note name %s: not found octave indicator after note. Correct format examples: C#2 or C3", st);
			else est.Format("Error parsing note name %s: note symbol not recognized. Correct format examples: C#2 or C3", st);
			WriteLog(5, est);
			return -1;
		}
	}
}

// Get pith class index from note name
int CGLib::GetPC(CString &st)
{
	st.Replace("'", "");
	int nid = -1;
	for (int i = 0; i < 12; i++) {
		if (NoteName[i] == st) {
			nid = i;
			break;
		}
		if (NoteName2[i] == st) {
			nid = i;
			break;
		}
	}
	if (nid > -1) {
		//CString est;
		//est.Format("Converted pitch class name %s to %d", st, nid);
		//WriteLog(0, est);
		return nid;
	}
	else {
		CString est;
		est.Format("Error parsing pitch class name %s: note symbol not recognized. Correct format examples: C# or C", st);
		WriteLog(5, est);
		return 0;
	}
}

int CGLib::MatchVectors(vector<int>& v1, vector<int>& v2, int i1, int i2)
{
	if (v1.size() <= i2) return 0;
	if (v2.size() <= i2) return 0;
	for (int i = i1; i <= i2; ++i) {
		if (v1[i] != v2[i]) return 0;
	}
	return 1;
}

void CGLib::isaac()
{
	register ub4 i, x, y;

	rcc = rcc + 1;    /* rcc just gets incremented once per 256 results */
	rbb = rbb + rcc;   /* then combined with rbb */

	for (i = 0; i<256; ++i)
	{
		x = rmm[i];
		switch (i % 4)
		{
		case 0: raa = raa ^ (raa << 13); break;
		case 1: raa = raa ^ (raa >> 6); break;
		case 2: raa = raa ^ (raa << 2); break;
		case 3: raa = raa ^ (raa >> 16); break;
		}
		raa = rmm[(i + 128) % 256] + raa;
		rmm[i] = y = rmm[(x >> 2) % 256] + raa + rbb;
		randrsl[i] = rbb = rmm[(y >> 10) % 256] + x;

		/* Note that bits 2..9 are chosen from x but 10..17 are chosen
		from y.  The only important thing here is that 2..9 and 10..17
		don't overlap.  2..9 and 10..17 were then chosen for speed in
		the optimized version (rand.c) */
		/* See http://burtleburtle.net/bob/rand/isaac.html
		for further explanations and analysis. */
	}
}

void CGLib::randinit(int flag)
{
	int i;
	ub4 a, b, c, d, e, f, g, h;
	raa = rbb = rcc = 0;
	a = b = c = d = e = f = g = h = 0x9e3779b9;  /* the golden ratio */

	for (i = 0; i<4; ++i)          /* scramble it */
	{
		mix(a, b, c, d, e, f, g, h);
	}

	for (i = 0; i<256; i += 8)   /* fill in rmm[] with messy stuff */
	{
		if (flag)                  /* use all the information in the seed */
		{
			a += randrsl[i]; b += randrsl[i + 1]; c += randrsl[i + 2]; d += randrsl[i + 3];
			e += randrsl[i + 4]; f += randrsl[i + 5]; g += randrsl[i + 6]; h += randrsl[i + 7];
		}
		mix(a, b, c, d, e, f, g, h);
		rmm[i] = a; rmm[i + 1] = b; rmm[i + 2] = c; rmm[i + 3] = d;
		rmm[i + 4] = e; rmm[i + 5] = f; rmm[i + 6] = g; rmm[i + 7] = h;
	}

	if (flag)
	{        /* do a second pass to make all of the seed affect all of rmm */
		for (i = 0; i<256; i += 8)
		{
			a += rmm[i]; b += rmm[i + 1]; c += rmm[i + 2]; d += rmm[i + 3];
e += rmm[i + 4]; f += rmm[i + 5]; g += rmm[i + 6]; h += rmm[i + 7];
mix(a, b, c, d, e, f, g, h);
rmm[i] = a; rmm[i + 1] = b; rmm[i + 2] = c; rmm[i + 3] = d;
rmm[i + 4] = e; rmm[i + 5] = f; rmm[i + 6] = g; rmm[i + 7] = h;
		}
	}

	isaac();            /* fill in the first set of results */
	randcnt = 256;        /* prepare to use the first set of results */
}

// Return random value from 0 to RAND_MAX using randrsl
unsigned int CGLib::rand2() {
	cur_rand2++;
	if (cur_rand2 > 1) {
		cur_rand2 = 0;
		cur_rand++;
		if (cur_rand > 255) {
			cur_rand = 0;
			isaac();
		}
	}
	//return randrsl[cur_rand];
	return ((randrsl[cur_rand]) >> (cur_rand2 * 16)) % RAND_MAX;
}

void CGLib::InitRandom()
{
	// Init rand
	srand(GetTickCount());
	//CString est;
	//est.Format("Random test: %d", rand());
	//WriteLog(1, est);
	// Init ISAAC
	ub4 i;
	raa = rbb = rcc = (ub4)0;
	for (i = 0; i < 256; ++i) rmm[i] = randrsl[i] = rand()*rand();
	randinit(1);
	//TestRandom();
}

void CGLib::TestRandom()
{
	long long time_start = CGLib::time();
	int n_buckets = 30;
	int n_samples = 100000;
	int n_variants = 3;
	vector< vector <int> > test = vector<vector<int>>(n_buckets, vector<int>(n_variants + 1));
	// Fill test
	for (int v = 0; v < n_samples; v++) {
		for (int i = 0; i < n_buckets; i++) {
			test[i][randbw(0, n_variants - 1)] ++;
		}
	}
	// Show results
	ofstream fs;
	fs.open("test-random.csv");
	CString st;
	for (int i = 0; i < n_buckets; i++) {
		for (int v = 0; v <= n_variants; v++) {
			st.Format("%d;", test[i][v]);
			fs << st;
		}
		fs << "\n";
	}
	fs.close();
	// Count time
	long long time_stop = CGLib::time();
	CString est;
	est.Format("TestRandom with %d buckets, %d samples, %d variants took %lld ms", 
		n_buckets, n_samples, n_variants, time_stop - time_start);
	WriteLog(0, est);
	// In case of successful test, you will see approximately same numbers in each column in output file
}

void CGLib::TestSmoothRandom()
{
	long long time_start = CGLib::time();
	int n_samples = 1000;
	CSmoothRandom sr;
	// Show results
	ofstream fs;
	fs.open("test-smrandom.csv");
	CString st;
	fs << "Sig;Vel;Acc;Acc2;\n";
	for (int i = 0; i < n_samples; i++) {
		sr.MakeNext();
		st.Format("%.0f;%.0f;%.0f;%.0f;", sr.sig * 1000, sr.vel * 1000, sr.acc * 1000, sr.acc2 * 1000);
		fs << st;
		fs << "\n";
	}
	fs.close();
	// Count time
	long long time_stop = CGLib::time();
	CString est;
	est.Format("TestSmoothRandom with %d samples took %lld ms", n_samples, time_stop - time_start);
	WriteLog(0, est);
}

void CGLib::EscalateLog(CString st) {
	if (m_testing == 1 && m_ci) {
		AppendLineToFile("autotest\\buffer.log", st + "\n");
		// Send log to appveyor
		CString par = "AddMessage \"" + m_cline2 + ": " + st + "\" -Category Warning";
		SHELLEXECUTEINFO sei = { 0 };
		sei.cbSize = sizeof(SHELLEXECUTEINFO);
		sei.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI;
		sei.hwnd = NULL;
		sei.lpVerb = NULL;
		sei.lpFile = "appveyor";
		sei.lpParameters = par;
		sei.lpDirectory = NULL;
		sei.nShow = SW_SHOWNORMAL;
		sei.hInstApp = NULL;
		ShellExecuteEx(&sei);
		WaitForSingleObject(sei.hProcess, 1000);
	}
	exitcode = 10;
}

void CGLib::WriteLog(int i, CString st)
{
	if (i >= LOG_TABS) i = 0;
	if (can_send_log) {
		// Check error log
		if (i == 5) {
			st = "! " + st;
			EscalateLog(st);
			i = 1;
		}
		if (!mutex_log.try_lock_for(chrono::milliseconds(2000))) {
			return;
		}
		++logs_sent[i];
		if (log_buffer_size[i] >= MAX_LOG_BUFFER) {
			if (!warn_log_buffer[i]) {
				CString st2;
				st2.Format("WARNING: MAXIMUM LOGS FREQUENCY %d in %d MS EXCEEDED FOR LONG TIME IN LOG: SOME LOG ENTRIES LOST. Please check your algorithm or increase MAX_LOG_BUFFER or LOG_MAX_SEND", LOG_MAX_SEND, LOG_TIMER);
				log_buffer[i].push_back(st2);
				++log_buffer_size[i];
				warn_log_buffer[i] = 1;
			}
			// Exit to prevent buffer overflow
			mutex_log.unlock();
			return;
		}
		log_buffer[i].push_back(CTime::GetCurrentTime().Format("%H:%M:%S") + " " + st);
		++log_buffer_size[i];
		mutex_log.unlock();
	}
}

void CGLib::SetStatusText(int line, CString st)
{
	if (can_send_log) {
		if (!mutex_log.try_lock_for(chrono::milliseconds(2000))) {
			return;
		}
		if (!oinfo.size()) {
			oinfo.resize(STATUS_LINES);
			oinfo_changed.resize(STATUS_LINES);
			status_updates.resize(STATUS_LINES);
		}
		++status_updates[line];
		if (line < STATUS_LINES) {
			oinfo[line] = st;
			oinfo_changed[line] = 1;
		}
		mutex_log.unlock();
	}
}

int CGLib::randbw(int n1, int n2)
{
	int re = (float)(n2 - n1 + 1) * rand() / (float)RAND_MAX;
	re = re + n1;
	if (re < n1) re = n1;
	if (re > n2) re = n2;
	return re;
}

int CGLib::randbw2(int n1, int n2)
{
	int re = (float)(n2 - n1 + 1) * rand2() / (float)RAND_MAX;
	re = re + n1;
	if (re < n1) re = n1;
	if (re > n2) re = n2;
	return re;
}

float CGLib::rand01()
{
	return (float)rand() / (float)RAND_MAX;
}

float CGLib::rand01_2()
{
	return (float)rand2() / (float)RAND_MAX;
}

void CGLib::TestVSet()
{
	// Init rand
	CString st;
	long long seed = CGLib::time();
	srand(seed);
	VSet<int> vs;
	vector<int> v;
	int vsize = 3;
	int vmax = 4;
	int vs_size = 0;
	for (int i = 0; i < 64; ++i) {
		v.clear();
		for (int x = 0; x < vsize; ++x) {
			v.push_back(rand()*vmax/RAND_MAX);
		}
		if (vs.Insert(v)) {
			vs_size = vs.s.size();
		}
		// Duplicate
		else {
			//WriteLog(0, "Found duplicate");
		}
	}
	st.Format("Total elements: %d", vs.s.size());
	WriteLog(0, st);
}

// Fill vector with value
void CGLib::vfill(vector<int> &v, int value) {
	int x2 = v.size();
	for (int x = 0; x < x2; ++x) v[x] = value;
}

// Sum of vector
int CGLib::vsum(vector<int> &v) {
	int res = 0;
	int x2 = v.size();
	for (int x = 0; x < x2; ++x) res += v[x];
	return res;
}

CString CGLib::HumanFloat(float f) {
	CString st;
	if (f > 5) st.Format("%.0f", f);
	else if (f > 0.05) st.Format("%.2f", f);
	else if (f > 0.0005) st.Format("%.4f", f);
	else if (f > 0.000005) st.Format("%.6f", f);
	else if (f > 0.00000005) st.Format("%.8f", f);
	else st = "0";
	return st;
}

// Add more precision by showing additional digits
CString CGLib::HumanFloatPrecision(float f) {
	CString st;
	if (f > 5) st.Format("%.2f", f);
	else if (f > 0.05) st.Format("%.4f", f);
	else if (f > 0.0005) st.Format("%.6f", f);
	else if (f > 0.000005) st.Format("%.8f", f);
	else if (f > 0.00000005) st.Format("%.10f", f);
	else st = "0";
	return st;
}

// Take chromatic note, key and minor flag and convert to real note, octave and alteration
void CGLib::GetRealNote(int no, int key, int mi, int &no2, int &oct, int &alter) {
	oct = no / 12;
	if (mi) {
		// Get base chromatic note for current note
		no2 = note_base_m[key][no % 12];
		// If it is diatonic, just return it
		if (no2 > -1) {
			if (no2 > no % 12 + 6) --oct;
			if (no2 < no % 12 - 6) ++oct;
			alter = no % 12 - no2;
		}
		// If not, build needed note from next lower note
		else {
			no2 = note_base_m[key][(no + 11) % 12];
			if (no2 > no % 12 + 6) --oct;
			if (no2 < no % 12 - 6) ++oct;
			alter = no % 12 - no2;
		}
	}
	else {
		no2 = note_base[key][no % 12];
		if (no2 > -1) {
			if (no2 > no % 12 + 6) --oct;
			if (no2 < no % 12 - 6) ++oct;
			alter = no % 12 - no2;
		}
		else {
			no2 = note_base[key][(no + 11) % 12];
			if (no2 > no % 12 + 6) --oct;
			if (no2 < no % 12 - 6) ++oct;
			alter = no % 12 - no2;
		}
	}
	if (alter > 2) alter -= 12;
	if (alter < -2) alter += 12;
}

CString CGLib::GetAlterName(int alter) {
	if (alter == -1) return "b";
	else if (alter == -2) return "bb";
	else if (alter == 1) return "#";
	else if (alter == 2) return "##";
	return "";
}

CString CGLib::GetRealNoteName(int no, int key, int mi) {
	int no2, oct, alter;
	GetRealNote(no, key, mi, no2, oct, alter);
	return NoteName[no2] + GetAlterName(alter);
}

// A Dequeue(Double ended queue) based method for finding maixmum element of
// all subarrays of size k
void CGLib::GetMovingMax(vector<int> &arr, int k, vector<int> &lmax) {
	// Create a Double Ended Queue, Qi that will store indexes of array elements
	// The queue will store indexes of useful elements in every window and it will
	// maintain decreasing order of values from front to rear in Qi, i.e., 
	// arr[Qi.front[]] to arr[Qi.rear()] are sorted in decreasing order
	int n = arr.size();
	int k2 = min(k, n);
	deque<int> Qi(k2);

	/* Process first k (or first window) elements of array */
	int i;
	for (i = 0; i < k2; ++i) {
		// For very element, the previous smaller elements are useless so
		// remove them from Qi (remove from rear)
		while ((!Qi.empty()) && arr[i] >= arr[Qi.back()])
			Qi.pop_back();
		// Add new element at rear of queue
		Qi.push_back(i);
	}
	// Process rest of the elements, i.e., from arr[k2] to arr[n-1]
	for (; i < n; ++i) {
		// The element at the front of the queue is the largest element of
		// previous window, so print it
		lmax[i - k2 / 2] = arr[Qi.front()];
		// Remove the elements which are out of this window (remove from front of queue)
		while ((!Qi.empty()) && Qi.front() <= i - k2)
			Qi.pop_front();
		// Remove all elements smaller than the currently
		// being added element (remove useless elements)
		while ((!Qi.empty()) && arr[i] >= arr[Qi.back()])
			Qi.pop_back();
		// Add current element at the rear of Qi
		Qi.push_back(i);
	}
	// Print the maximum element of last window
	lmax[i - k2 / 2] = arr[Qi.front()];
	// Fill borders
	for (i = 0; i <= k2 / 2; ++i) {
		lmax[i] = lmax[k2 - k2 / 2];
		lmax[n - i - 1] = lmax[n - k2 / 2];
	}
}

__int64 CGLib::FileSize(CString fname)
{
	HANDLE hFile = CreateFile(fname, GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return -1; // error condition, could call GetLastError to find out more

	LARGE_INTEGER size;
	if (!GetFileSizeEx(hFile, &size))
	{
		CloseHandle(hFile);
		return -1; // error condition, could call GetLastError to find out more
	}

	CloseHandle(hFile);
	return size.QuadPart;
}

void CGLib::CleanFolder(CString Wildcard) {
	CFileFind finder;
	BOOL bWorking = finder.FindFile(Wildcard);
	while (bWorking) {
		bWorking = finder.FindNextFile();
		if (finder.IsDots()) continue;
		DeleteFile(finder.GetFilePath());
	}
	finder.Close();
}

void CGLib::CheckMemoryUsage() {
	if (warn_memory_usage) return;
	PROCESS_MEMORY_COUNTERS_EX pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
	SIZE_T virtualMemUsedByMe = pmc.PrivateUsage / 1024 / 1024;
	if (virtualMemUsedByMe > MAX_MEMORY_WARN) {
		CString st;
		st.Format("Memory usage is above limit %d Mb", (int)MAX_MEMORY_WARN);
		WriteLog(5, st);
		warn_memory_usage = 1;
	}
}

void CGLib::SetBit(unsigned char &ch, int bit) {
	ch |= 1UL << bit;
}

void CGLib::ClearBit(unsigned char &ch, int bit) {
	ch &= ~(1UL << bit);
}

int CGLib::GetBit(unsigned char ch, int bit) {
	return (ch >> bit) & 1U;
}
