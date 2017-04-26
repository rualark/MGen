#include "stdafx.h"
#include "GLib.h"

#ifdef _DEBUG
#define new DEBUG_NEW 
#endif

int CGLib::can_send_log = 1;
HWND CGLib::m_hWnd = 0;
int CGLib::debug_level = 1;
int CGLib::parameter_found = 0;
int CGLib::play_enabled = 1;
UINT CGLib::WM_DEBUG_MSG = 0;
vector<vector<CString>> CGLib::logs;

/* if (flag!=0), then use the contents of randrsl[] to initialize mm[]. */
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

CGLib::CGLib()
{
	logs.clear();
}


CGLib::~CGLib()
{
}

void CGLib::CheckVar(CString * sName, CString * sValue, char* sSearch, int * Dest, int vmin, int vmax)
{
	if (*sName == sSearch) {
		++parameter_found;
		*Dest = atoi(*sValue);
		if ((vmin != -1) && (vmax != -1)) {
			if (*Dest < vmin) *Dest = vmin;
			if (*Dest > vmax) *Dest = vmax;
		}
	}
}

void CGLib::LoadRange(CString * sName, CString * sValue, char* sSearch, int * vmin, int* vmax)
{
	if (*sName == sSearch) {
		++parameter_found;
		int pos = sValue->Find("-");
		if (pos == -1) {
			CString* est = new CString;
			est->Format("Error parsing range variable '%s'. Range format must be 'X-Y'. Not found '-' symbol in string '%s'.", *sName, *sValue);
			WriteLog(1, est);
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
			*vmin = atoi(st);
			*vmax = atoi(st2);
		}
	}
}

void CGLib::LoadRange(CString * sName, CString * sValue, char* sSearch, float * vmin, float * vmax)
{
	if (*sName == sSearch) {
		++parameter_found;
		int pos = sValue->Find("-");
		if (pos == -1) {
			CString* est = new CString;
			est->Format("Error parsing range variable '%s'. Range format must be 'X-Y'. Not found '-' symbol in string '%s'.", *sName, *sValue);
			WriteLog(1, est);
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
		}
	}
}

void CGLib::CheckVar(CString * sName, CString * sValue, char* sSearch, float * Dest)
{
	if (*sName == sSearch) {
		++parameter_found;
		*Dest = atof(*sValue);
	}
}

void CGLib::LoadVar(CString * sName, CString * sValue, char* sSearch, CString * Dest)
{
	if (*sName == sSearch) {
		++parameter_found;
		*Dest = *sValue;
	}
}

void CGLib::LoadVectorPar(CString * sName, CString * sValue, char* sSearch, vector<int> & Dest)
{
	if (*sName == sSearch) {
		++parameter_found;
		int pos = 0;
		CString st;
		for (int i = 0; i<1000; i++) {
			st = sValue->Tokenize(",", pos);
			st.Trim();
			if (st == "") break;
			Dest[i] = atoi(st);
		}
	}
}

int CGLib::CheckInclude(CString st, CString fname, CString &iname) {
	iname = "";
	if (st.Left(8) == "include ") {
		st = st.Mid(8);
		st.Replace("\"", "");
		st.Replace("/", "\\");
		st.Trim();
		char szDir[1024];
		char *fileExt;
		CString st4 = dir_from_path(fname) + "\\" + st;
		GetFullPathName(st4, 1024, szDir, &fileExt);
		iname = szDir;
		return 1;
	}
	return 0;
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

void CGLib::AppendLineToFile(CString fname, CString st)
{
	ofstream outfile;
	outfile.open(fname, ios_base::app);
	outfile << st;
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
		if (isdigit(st[1])) pos = 1;
		CString nname = st.Left(pos);
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
		if ((nid > -1) && (isdigit(st[pos]))) {
			int i = nid + (atoi(st.Mid(pos, 1))+1) * 12;
			//CString* est = new CString;
			//est->Format("Converted note name %s to %d", st, i);
			//WriteLog(0, est);
			return i;
		} else {
			CString* est = new CString;
			if (nid > -1)	est->Format("Error parsing note name %s: not found octave indicator after note. Correct format examples: C#2 or C3", st);
			else est->Format("Error parsing note name %s: note symbol not recognized. Correct format examples: C#2 or C3", st);
			WriteLog(1, est);
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
		//CString* est = new CString;
		//est->Format("Converted pitch class name %s to %d", st, nid);
		//WriteLog(0, est);
		return nid;
	}
	else {
		CString* est = new CString;
		est->Format("Error parsing pitch class name %s: note symbol not recognized. Correct format examples: C# or C", st);
		WriteLog(1, est);
	}
}

void CGLib::isaac()
{
	register ub4 i, x, y;

	cc = cc + 1;    /* cc just gets incremented once per 256 results */
	bb = bb + cc;   /* then combined with bb */

	for (i = 0; i<256; ++i)
	{
		x = mm[i];
		switch (i % 4)
		{
		case 0: aa = aa ^ (aa << 13); break;
		case 1: aa = aa ^ (aa >> 6); break;
		case 2: aa = aa ^ (aa << 2); break;
		case 3: aa = aa ^ (aa >> 16); break;
		}
		aa = mm[(i + 128) % 256] + aa;
		mm[i] = y = mm[(x >> 2) % 256] + aa + bb;
		randrsl[i] = bb = mm[(y >> 10) % 256] + x;

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
	aa = bb = cc = 0;
	a = b = c = d = e = f = g = h = 0x9e3779b9;  /* the golden ratio */

	for (i = 0; i<4; ++i)          /* scramble it */
	{
		mix(a, b, c, d, e, f, g, h);
	}

	for (i = 0; i<256; i += 8)   /* fill in mm[] with messy stuff */
	{
		if (flag)                  /* use all the information in the seed */
		{
			a += randrsl[i]; b += randrsl[i + 1]; c += randrsl[i + 2]; d += randrsl[i + 3];
			e += randrsl[i + 4]; f += randrsl[i + 5]; g += randrsl[i + 6]; h += randrsl[i + 7];
		}
		mix(a, b, c, d, e, f, g, h);
		mm[i] = a; mm[i + 1] = b; mm[i + 2] = c; mm[i + 3] = d;
		mm[i + 4] = e; mm[i + 5] = f; mm[i + 6] = g; mm[i + 7] = h;
	}

	if (flag)
	{        /* do a second pass to make all of the seed affect all of mm */
		for (i = 0; i<256; i += 8)
		{
			a += mm[i]; b += mm[i + 1]; c += mm[i + 2]; d += mm[i + 3];
			e += mm[i + 4]; f += mm[i + 5]; g += mm[i + 6]; h += mm[i + 7];
			mix(a, b, c, d, e, f, g, h);
			mm[i] = a; mm[i + 1] = b; mm[i + 2] = c; mm[i + 3] = d;
			mm[i + 4] = e; mm[i + 5] = f; mm[i + 6] = g; mm[i + 7] = h;
		}
	}

	isaac();            /* fill in the first set of results */
	randcnt = 256;        /* prepare to use the first set of results */
}

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
	unsigned seed = chrono::system_clock::now().time_since_epoch().count();
	srand(seed);
	//CString* est = new CString;
	//est->Format("Random test: %d", rand());
	//WriteLog(1, est);
	// Init ISAAC
	ub4 i;
	aa = bb = cc = (ub4)0;
	for (i = 0; i < 256; ++i) mm[i] = randrsl[i] = rand()*rand();
	randinit(1);
	//TestRandom();
}

void CGLib::TestRandom()
{
	milliseconds time_start = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
	int n_buckets = 30;
	int n_samples = 100000;
	int n_variants = 3;
	vector< vector <int> > test = vector<vector<int>>(n_buckets, vector<int>(n_variants+1));
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
	milliseconds time_stop = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
	CString* est = new CString;
	est->Format("TestRandom with %d buckets, %d samples, %d variants took %d ms", n_buckets, n_samples, n_variants, time_stop - time_start);
	WriteLog(0, est);
}

void CGLib::WriteLog(int i, CString* pST)
{
	// Add log to vector
	if (logs.size() < 2) logs.resize(10);
	logs[i].push_back(CTime::GetCurrentTime().Format("%H:%M:%S") + " " + *pST);
	if (can_send_log)	::PostMessage(m_hWnd, WM_DEBUG_MSG, i, (LPARAM)pST);
	else delete pST;
}

void CGLib::WriteLog(int i, CString st)
{
	CString *est = new CString;
	*est = st;
	WriteLog(i, est);
}

int CGLib::randbw(int n1, int n2)
{
	int re = (float)(n2 - n1 + 1) * rand2() / (float)RAND_MAX;
	re = re + n1;
	return re;
}

float CGLib::rand01()
{
	return (float) rand2() / (float)RAND_MAX;
}

