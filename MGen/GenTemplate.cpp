#include "stdafx.h"
#include "GenTemplate.h"

#include "midifile/MidiFile.h"

int CGenTemplate::can_send_log = 1;
HWND CGenTemplate::m_hWnd = 0;
UINT CGenTemplate::WM_DEBUG_MSG = 0;

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

CGenTemplate::CGenTemplate()
{
	// Init constant length arrays
	play_transpose.resize(MAX_INSTR);
	play_transpose_old.resize(MAX_INSTR);
	instr.resize(MAX_INSTR);
	instr_type.resize(MAX_VOICE);
	instr_nmin.resize(MAX_VOICE);
	instr_nmax.resize(MAX_VOICE);
	instr_tmin.resize(MAX_VOICE);
	instr_tmax.resize(MAX_VOICE);
	CC_dynamics.resize(MAX_VOICE);
	max_slur_count.resize(MAX_VOICE);
	max_slur_interval.resize(MAX_VOICE);
	slur_ks.resize(MAX_VOICE);
	legato_ahead.resize(MAX_VOICE);
	nonlegato_freq.resize(MAX_VOICE);
	nonlegato_minlen.resize(MAX_VOICE);
	warning_note_range.resize(MAX_VOICE);
	warning_note_short.resize(MAX_VOICE);
	lengroup2.resize(MAX_INSTR);
	lengroup3.resize(MAX_INSTR);
	lengroup4.resize(MAX_INSTR);
	lengroup_edt1.resize(MAX_INSTR);
	lengroup_edt2.resize(MAX_INSTR);
	rand_start.resize(MAX_INSTR);
	// Set instrument
	instr[0] = 0;
}


CGenTemplate::~CGenTemplate()
{
	StopMIDI();
}

void CGenTemplate::InitVectors()
{
	// Create vectors
	pause = vector<vector<unsigned char>>(t_allocated, vector<unsigned char>(v_cnt));
	note = vector<vector<unsigned char>>(t_allocated, vector<unsigned char>(v_cnt));
	len = vector<vector<unsigned char>>(t_allocated, vector<unsigned char>(v_cnt));
	coff = vector<vector<unsigned char>>(t_allocated, vector<unsigned char>(v_cnt));
	poff = vector<vector<unsigned char>>(t_allocated, vector<unsigned char>(v_cnt));
	noff = vector<vector<unsigned char>>(t_allocated, vector<unsigned char>(v_cnt));
	dyn = vector<vector<unsigned char>>(t_allocated, vector<unsigned char>(v_cnt));
	vel = vector<vector<unsigned char>>(t_allocated, vector<unsigned char>(v_cnt));
	artic = vector<vector<unsigned char>>(t_allocated, vector<unsigned char>(v_cnt));
	lengroup = vector<vector<char>>(t_allocated, vector<char>(v_cnt));
	comment = vector<vector<CString>>(t_allocated, vector<CString>(v_cnt));
	color = vector<vector<Color>>(t_allocated, vector<Color>(v_cnt));
	tempo = vector<double>(t_allocated);
	stime = vector<double>(t_allocated);
	etime = vector<double>(t_allocated);
	dstime = vector<double>(t_allocated);
	detime = vector<double>(t_allocated);
	// Init ngv
	for (int v = 0; v < MAX_VOICE; v++) {
		ngv_min[v] = 1000;
		ngv_max[v] = 0;
	}
	// Init color
	for (int i = 0; i < t_allocated; i++) {
		for (int v = 0; v < v_cnt; v++) {
			color[i][v] = Color(0);
		}
	}
}

void CGenTemplate::ResizeVectors(int size)
{
	milliseconds time_start = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
	if (!mutex_output.try_lock_for(chrono::milliseconds(5000))) {
		WriteLog(1, new CString("Critical error: ResizeVectors mutex timed out"));
	}
	pause.resize(size);
	note.resize(size);
	len.resize(size);
	coff.resize(size);
	poff.resize(size);
	noff.resize(size);
	tempo.resize(size);
	stime.resize(size);
	etime.resize(size);
	dstime.resize(size);
	detime.resize(size);
	dyn.resize(size);
	vel.resize(size);
	artic.resize(size);
	lengroup.resize(size);
	comment.resize(size);
	color.resize(size);
	for (int i = t_allocated; i < size; i++) {
		pause[i].resize(v_cnt);
		note[i].resize(v_cnt);
		len[i].resize(v_cnt);
		coff[i].resize(v_cnt);
		poff[i].resize(v_cnt);
		noff[i].resize(v_cnt);
		dyn[i].resize(v_cnt);
		vel[i].resize(v_cnt);
		artic[i].resize(v_cnt);
		lengroup[i].resize(v_cnt);
		comment[i].resize(v_cnt);
		color[i].resize(v_cnt, Color(0));
	}
	// Count time
	milliseconds time_stop = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
	CString* st = new CString;
	st->Format("ResizeVectors from %d to %d (in %d ms)", t_allocated, size, time_stop - time_start);
	WriteLog(0, st);

	t_allocated = size;
	mutex_output.unlock();
}

void CGenTemplate::copy_file(CString sName, CString dName) {
	std::ifstream  src(sName, std::ios::binary);
	std::ofstream  dst(dName, std::ios::binary);
	dst << src.rdbuf();
	src.close();
	dst.close();
}

void CGenTemplate::AppendLineToFile(CString fname, CString st)
{
	ofstream outfile;
	outfile.open(fname, ios_base::app);
	outfile << st;
	outfile.close();
}

void CGenTemplate::CheckVar(CString * sName, CString * sValue, char* sSearch, int * Dest, int vmin, int vmax)
{
	if (*sName == sSearch) {
		*Dest = atoi(*sValue);
		if ((vmin != -1) && (vmax != -1)) {
			if (*Dest < vmin) *Dest = vmin;
			if (*Dest > vmax) *Dest = vmax;
		}
	}
}

void CGenTemplate::CheckVar(CString * sName, CString * sValue, char* sSearch, double * Dest)
{
	if (*sName == sSearch) {
		*Dest = atof(*sValue);
	}
}

void CGenTemplate::LoadVar(CString * sName, CString * sValue, char* sSearch, CString * Dest)
{
	if (*sName == sSearch) {
		*Dest = *sValue;
	}
}

void CGenTemplate::LoadNote(CString * sName, CString * sValue, char* sSearch, int * Dest)
{
	if (*sName == sSearch) {
		*Dest = CGenTemplate::GetNoteI(*sValue);
	}
}

void CGenTemplate::LoadConfig(CString fname)
{
	CString st, st2, st3;
	ifstream fs;
	// Check file exists
	if (!CGenTemplate::fileExists(fname)) {
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
		if (pos != -1) st = st.Left(pos);
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
			double fdata = atof(st3);
			CheckVar(&st2, &st3, "v_cnt", &v_cnt);
			CheckVar(&st2, &st3, "t_cnt", &t_cnt);
			CheckVar(&st2, &st3, "t_allocated", &t_allocated);
			CheckVar(&st2, &st3, "t_send", &t_send);
			CheckVar(&st2, &st3, "midifile_tpq_mul", &midifile_tpq_mul);
			CheckVar(&st2, &st3, "sleep_ms", &sleep_ms);
			// Load algorithm-specific variables
			LoadConfigLine(&st2, &st3, idata, fdata);
		}
	}
	fs.close();
	CString* est = new CString;
	est->Format("LoadConfig loaded %d lines from %s", i, fname);
	WriteLog(0, est);
}

bool CGenTemplate::dirExists(CString dirName_in)
{
	DWORD ftyp = GetFileAttributesA(dirName_in);
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false;  //something is wrong with your path!

	if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
		return true;   // this is a directory!

	return false;    // this is not a directory!
}

bool CGenTemplate::fileExists(CString dirName_in)
{
	DWORD ftyp = GetFileAttributesA(dirName_in);
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false;  //something is wrong with your path!

	if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
		return false;   // this is a directory!

	return true;    // this is not a directory!
}

bool CGenTemplate::nodeExists(CString dirName_in)
{
	DWORD ftyp = GetFileAttributesA(dirName_in);
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false;  //something is wrong with your path!

	return true;    // file exists
}

CString CGenTemplate::fname_from_path(CString path)
{
	string::size_type pos = string(path).find_last_of("\\/");
	CString path2 = string(path).substr(pos + 1).c_str();
	return path2;
}

CString CGenTemplate::bname_from_path(CString path)
{
	string::size_type pos = string(path).find_last_of("\\/");
	string::size_type pos2 = string(path).find_last_of("./");
	CString path2 = string(path).substr(pos + 1, pos2 - pos - 1).c_str();
	return path2;
}

CString CGenTemplate::dir_from_path(CString path)
{
	string::size_type pos = string(path).find_last_of("\\/");
	CString path2 = string(path).substr(0, pos).c_str();
	return path2;
}

int CGenTemplate::PmEvent_comparator(const void * v1, const void * v2)
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

CString CGenTemplate::GetNoteName(int n)
{
	CString st;
	st.Format("%d", n / 12 - 1);
	return NoteName[n % 12] + st;
}

int CGenTemplate::GetNoteI(CString st)
{
	if (isdigit(st[0])) {
		return atoi(st);
	}
	else {
		int nid = -1;
		int pos = 2;
		if (isdigit(st[1])) pos = 1;
		CString nname = st.Left(pos);
		for (int i = 0; i < 12; i++) if (NoteName[i] == nname) {
			nid = i;
			break;
		}
		if ((nid > -1) && (isdigit(st[pos]))) {
			int i = nid + (atoi(st.Mid(pos, 1))+1) * 12;
			CString* est = new CString;
			est->Format("Converted note name %s to %d", st, i);
			WriteLog(0, est);
			return i;
		} else {
			CString* est = new CString;
			if (nid > -1)	est->Format("Error parsing note name %s: not found octave indicator after note. Correct format examples: C#2 or C3", st);
			else est->Format("Error parsing note name %s: note symbol not recognized. Correct format examples: C#2 or C3", st);
			WriteLog(1, est);
		}
	}
}

void CGenTemplate::isaac()
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

void CGenTemplate::randinit(int flag)
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

unsigned int CGenTemplate::rand2() {
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

void CGenTemplate::InitRandom()
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

void CGenTemplate::TestRandom()
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

void CGenTemplate::LoadInstruments()
{
	// Load strings
	ifstream fs;
	fs.open("instruments.pl");
	CString st, st2, st3;
	char pch[2550];
	int pos = 0;
	int i = -1, x = 0;
	while (fs.good()) {
		x++;
		fs.getline(pch, 2550);
		st = pch;
		// Remove comments
		pos = st.Find("#");
		if (pos != -1) st = st.Left(pos);
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
			if (st2 == "instrument") {
				for (int x = 0; x<MAX_INSTR; x++) {
					if (InstName[x] == st3) i = x;
				}
			}
			if (i > -1) {
				LoadNote(&st2, &st3, "n_min", &instr_nmin[i]);
				LoadNote(&st2, &st3, "n_max", &instr_nmax[i]);
				CheckVar(&st2, &st3, "t_min", &instr_tmin[i]);
				CheckVar(&st2, &st3, "t_max", &instr_tmax[i]);
				CheckVar(&st2, &st3, "type", &instr_type[i]);
				CheckVar(&st2, &st3, "cc_dynamics", &CC_dynamics[i]);
				CheckVar(&st2, &st3, "max_slur_count", &max_slur_count[i]);
				CheckVar(&st2, &st3, "max_slur_interval", &max_slur_interval[i]);
				CheckVar(&st2, &st3, "slur_ks", &slur_ks[i]);
				CheckVar(&st2, &st3, "legato_ahead", &legato_ahead[i]);
				CheckVar(&st2, &st3, "nonlegato_minlen", &nonlegato_minlen[i]);
				CheckVar(&st2, &st3, "nonlegato_freq", &nonlegato_freq[i]);
				CheckVar(&st2, &st3, "lengroup2", &lengroup2[i]);
				CheckVar(&st2, &st3, "lengroup3", &lengroup3[i]);
				CheckVar(&st2, &st3, "lengroup4", &lengroup4[i]);
				CheckVar(&st2, &st3, "lengroup_edt1", &lengroup_edt1[i]);
				CheckVar(&st2, &st3, "lengroup_edt2", &lengroup_edt2[i]);
				CheckVar(&st2, &st3, "rand_start", &rand_start[i]);
				//CGenTemplate::LoadVar(&st2, &st3, "save_format_version", &save_format_version);
			}
		}
	}
	fs.close();
	CString* est = new CString;
	est->Format("LoadInstruments loaded %d lines from instruments.pl", x);
	WriteLog(0, est);
}

void CGenTemplate::SaveVector2C(ofstream & fs, vector< vector<unsigned char> > &v2D, int i) {
	const char* pointer = reinterpret_cast<const char*>(&v2D[i][0]);
	size_t bytes = v_cnt * sizeof(v2D[i][0]);
	fs.write(pointer, bytes);
}

void CGenTemplate::SaveVector2Color(ofstream & fs, vector< vector<Color> > &v2D, int i) {
	size_t bytes = 4;
	for (int v = 0; v < v_cnt; v++) {
		DWORD color = v2D[i][v].GetValue();
		fs.write((char*)&color, bytes);
	}
}

void CGenTemplate::SaveVector2ST(ofstream & fs, vector< vector<CString> > &v2D, int i) {
	for (int v = 0; v < v_cnt; v++) {
		unsigned short len = v2D[i][v].GetLength();
		fs.write((char*)&len, sizeof(len));
		if (len != 0) fs.write(v2D[i][v].GetBuffer(), len);
	}
}

void CGenTemplate::SaveVectorD(ofstream &fs, vector<double> &v) {
	const char* pointer = reinterpret_cast<const char*>(&v[0]);
	size_t bytes = t_generated * sizeof(v[0]);
	fs.write(pointer, bytes);
}

void CGenTemplate::SaveResults(CString dir, CString fname)
{
	milliseconds time_start = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
	CreateDirectory(dir, NULL);
	ofstream fs;
	fs.open(dir + "\\" + fname + ".mgr", std::ofstream::binary);
	for (size_t i = 0; i < t_generated; i++)
	{
		SaveVector2C(fs, pause, i);
		SaveVector2C(fs, note, i);
		SaveVector2C(fs, len, i);
		SaveVector2C(fs, coff, i);
		SaveVector2C(fs, dyn, i);
		SaveVector2ST(fs, comment, i);
		SaveVector2Color(fs, color, i);
	}
	SaveVectorD(fs, tempo);
	fs.close();
	// Save strings
	CString st;
	fs.open(dir + "\\" + fname + ".txt");
	fs << "save_format_version = 1.2 # This is version of format used to save these files\n";
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
	// Count time
	milliseconds time_stop = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
	CString* est = new CString;
	est->Format("Saved results to file in %d ms", time_stop - time_start);
	WriteLog(0, est);
}

void CGenTemplate::LoadVector2C(ifstream& fs, vector< vector<unsigned char> > &v2D, int i) {
	v2D[i].resize(v_cnt);
	char* pointer = reinterpret_cast<char*>(&(v2D[i][0]));
	size_t bytes = v_cnt * sizeof(v2D[i][0]);
	fs.read(pointer, bytes);
}

void CGenTemplate::LoadVector2Color(ifstream & fs, vector< vector<Color> > &v2D, int i) {
	size_t bytes = 4;
	for (int v = 0; v < v_cnt; v++) {
		DWORD color;
		fs.read((char*)&color, bytes);
		v2D[i][v].SetValue(color);
	}
}

void CGenTemplate::LoadVector2ST(ifstream & fs, vector< vector<CString> > &v2D, int i) {
	for (int v = 0; v < v_cnt; v++) {
		unsigned short len;
		char buf[5000];
		fs.read((char*)&len, sizeof(len));
		if (len != 0) {
			fs.read((char*)&buf, len);
			v2D[i][v] = buf;
		}
	}
}

void CGenTemplate::LoadVectorD(ifstream &fs, vector<double> &v) {
	v.clear();
	v.resize(t_generated);
	char* pointer = reinterpret_cast<char*>(&v[0]);
	size_t bytes = t_generated * sizeof(v[0]);
	fs.read(pointer, bytes);
}

void CGenTemplate::LoadResults(CString dir, CString fname)
{
	milliseconds time_start = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
	// Load strings
	ifstream fs;
	fs.open(dir + "\\" + fname + ".txt");
	CString st, st2, st3;
	char pch[2550];
	int pos = 0;
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
			CGenTemplate::CheckVar(&st2, &st3, "t_cnt", &t_cnt);
			CGenTemplate::CheckVar(&st2, &st3, "m_algo_id", &m_algo_id);
			CGenTemplate::CheckVar(&st2, &st3, "v_cnt", &v_cnt);
			CGenTemplate::CheckVar(&st2, &st3, "t_generated", &t_generated);
			CGenTemplate::CheckVar(&st2, &st3, "t_sent", &t_sent);
			CGenTemplate::CheckVar(&st2, &st3, "t_send", &t_send);
			CGenTemplate::CheckVar(&st2, &st3, "need_exit", &need_exit);
			CGenTemplate::CheckVar(&st2, &st3, "time_started", &time_started);
			CGenTemplate::CheckVar(&st2, &st3, "time_stopped", &time_stopped);
			CGenTemplate::LoadVar(&st2, &st3, "m_config", &m_config);
			CGenTemplate::LoadVar(&st2, &st3, "save_format_version", &save_format_version);
		}
	}
	fs.close();
	// Allocate
	t_allocated = t_generated;
	InitVectors();
	// Load binary
	fs.open(dir + "\\" + fname + ".mgr", std::ofstream::binary);
	fs.unsetf(std::ios::skipws);
	if (t_generated != 0)
	for (size_t i = 0; i < t_generated; i++)
	{
		LoadVector2C(fs, pause, i);
		LoadVector2C(fs, note, i);
		LoadVector2C(fs, len, i);
		LoadVector2C(fs, coff, i);
		LoadVector2C(fs, dyn, i);
		LoadVector2ST(fs, comment, i);
		LoadVector2Color(fs, color, i);
	}
	LoadVectorD(fs, tempo);
	CountOff(0, t_generated - 1);
	CountTime(0, t_generated - 1);
	UpdateNoteMinMax(0, t_generated - 1);
	UpdateTempoMinMax(0, t_generated - 1);
	fs.close();
	// Count time
	milliseconds time_stop = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
	CString* est = new CString;
	est->Format("Loaded results from files in %d ms", time_stop - time_start);
	WriteLog(0, est);
}

void CGenTemplate::SaveMidi(CString dir, CString fname)
{
	MidiFile midifile;
	midifile.addTracks(v_cnt);    // Add another two tracks to the MIDI file
	int tpq = 120;                // ticks per quarter note
	int tpñ = 60 * midifile_tpq_mul; // ticks per croch
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
		midifile.addTrackName(track, 0, st);
		midifile.addPatchChange(track, 0, channel, 0); // 40=violin
		for (int i = 0; i < t_generated; i++) if (pause[i][v] == 0) {
			midifile.addNoteOn(track, (tpq*4)+ tpñ*i, channel, note[i][v], dyn[i][v]);
			midifile.addNoteOff(track, (tpq * 4) + tpñ*(i+len[i][v])-1, channel, note[i][v], 0);
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

void CGenTemplate::CountOff(int step1, int step2)
{
	for (int i = step1; i <= step2; i++) {
		for (int v = 0; v < v_cnt; v++) {
			noff[i][v] = len[i][v] - coff[i][v];
			if (i - coff[i][v] - 1 >= 0) poff[i][v] = len[i - coff[i][v] - 1][v] - coff[i][v];
			else poff[i][v] = 0;
		}
	}
}

void CGenTemplate::CountTime(int step1, int step2)
{
	for (int i = step1; i <= step2; i++) {
		if (i > 0) stime[i] = stime[i - 1] + 30000.0 / tempo[i - 1];
		else stime[i] = 0;
		etime[i] = stime[i] + 30000.0 / tempo[i];
	}
}

void CGenTemplate::UpdateNoteMinMax(int step1, int step2)
{
	for (int i = step1; i <= step2; i++) {
		for (int v = 0; v < v_cnt; v++) if (pause[i][v] == 0) {
			if (ng_min > note[i][v]) ng_min = note[i][v];
			if (ng_max < note[i][v]) ng_max = note[i][v];
			if (ngv_min[v] > note[i][v]) ngv_min[v] = note[i][v];
			if (ngv_max[v] < note[i][v]) ngv_max[v] = note[i][v];
		}
	}
}

void CGenTemplate::UpdateTempoMinMax(int step1, int step2)
{
	for (int i = step1; i <= step2; i++) {
		if (tg_min > tempo[i]) tg_min = tempo[i];
		if (tg_max < tempo[i]) tg_max = tempo[i];
	}
}

void CGenTemplate::Adapt(int step1, int step2)
{
	int slur_count = 0;
	int ei; // ending step
	int pi; // previous note step
	int ndur; // note duration
	for (int v = 0; v < v_cnt; v++) {
		int ii = instr[v]; // Instrument id
		int ncount = 0;
		// Move to note start
		if (coff[step1][v] > 0) step1 = step1 - coff[step1][v];
		// Count notes
		for (int i = step1; i < step2; i++) {
			if (i + len[i][v] > step2) break;
			ncount++;
			if (noff[i][v] == 0) break;
			i += noff[i][v] - 1;
		}
		// Check if notes are in instrument range
		if ((ngv_min[v] + play_transpose[v] < instr_nmin[ii]) || (ngv_max[v] + play_transpose[v] > instr_nmax[ii])) {
			if (ngv_min[v] < instr_nmin[ii]) {
				play_transpose[v] = ((instr_nmin[ii] - ngv_min[v]) / 12) * 12 + 12;
			}
			if (ngv_max[v] > instr_nmax[ii]) {
				play_transpose[v] = -((ngv_max[v] - instr_nmax[ii]) / 12) * 12 - 12;
			}
			// Check if still have problem
			CString* st = new CString;
			if ((ngv_min[v] + play_transpose[v] < instr_nmin[ii]) || (ngv_max[v] + play_transpose[v] > instr_nmax[ii])) {
				if (!warning_note_range[v]) {
					st->Format("Generated notes range (%s - %s) is outside instrument range (%s - %s). Cannot transpose automatically: range too wide.",
						GetNoteName(ngv_min[v]), GetNoteName(ngv_max[v]), GetNoteName(instr_nmin[ii]), 
						GetNoteName(instr_nmax[ii]), play_transpose[v]);
					warning_note_range[v] = 1;
					WriteLog(1, st);
				}
			}
			else {
				st->Format("Generated notes range (%s - %s) is outside instrument range (%s - %s). Transposed automatically to %d semitones. Consider changing instrument or generation range.",
					GetNoteName(ngv_min[v]), GetNoteName(ngv_max[v]), GetNoteName(instr_nmin[ii]), 
					GetNoteName(instr_nmax[ii]), play_transpose[v]);
				WriteLog(1, st);
			}
		}
		// Calculate delta: dstime / detime
		int i = step1;
		for (int x = 0; x < ncount; x++) {
			ei = i + len[i][v] - 1;
			pi = i - poff[i][v];
			// Calculate lengroups
			if (!pause[i][v]) {
				if ((i == 0) || ((i > 0) && (lengroup[pi][v] < 2))) {
					// Start new lengroup if was no lengroup or lengroup ended
					int r = randbw(0, 100);
					if (r < lengroup2[ii]) lengroup[i][v] = 2;
					else if (r < lengroup2[ii] + lengroup3[ii]) lengroup[i][v] = 3;
					else if (r < lengroup2[ii] + lengroup3[ii] + lengroup4[ii]) lengroup[i][v] = 4;
					else lengroup[i][v] = 0;
				}
				else if (i > 0) {
					// Continue lengroup
					lengroup[i][v] = lengroup[pi][v] - 1;
				}
				// Apply lengroups
				if (instr_type[ii] == INSTR_PIANO) {
					if (lengroup[i][v] > 1) {
						if (lengroup_edt1[ii] < 0) {
							detime[ei] = -min(-lengroup_edt1[ii], (etime[ei] - stime[i]) * 100 / m_pspeed / 3);
							artic[i][v] = ARTIC_NONLEGATO;
						}
						else {
							detime[ei] = lengroup_edt1[ii];
							artic[i][v] = ARTIC_SLUR;
						}
					}
					if (lengroup[i][v] == 1) {
						if (lengroup_edt2[ii] < 0) {
							detime[ei] = -min(-lengroup_edt2[ii], (etime[ei] - stime[i]) * 100 / m_pspeed / 3);
							artic[i][v] = ARTIC_NONLEGATO;
						}
						else {
							detime[ei] = lengroup_edt2[ii];
							artic[i][v] = ARTIC_SLUR;
						}
					}
				}
				// Advance start for legato (not longer than previous note length)
				if ((i > 0) && (legato_ahead[ii] > 0) && (detime[i - 1] >= 0) && (!pause[pi][v])) {
					dstime[i] = -min(legato_ahead[ii], (etime[i - 1] - stime[pi]) * 100 / m_pspeed +
						detime[i - 1] - dstime[pi] - 1);
					detime[i - 1] = 0.9 * dstime[i];
				}
				// Add slurs
				if ((i > 0) && (instr_type[ii] == INSTR_VIOLIN) && (abs(note[pi][v] - note[i][v]) <= max_slur_interval[ii]) &&
					(slur_count <= max_slur_count[ii])) {
					artic[x][v] = ARTIC_SLUR;
					slur_count++;
				}
				else {
					slur_count = 0;
				}
				// Randomly make some notes non-legato if they have enough length
				if (((etime[ei] - stime[i]) * 100 / m_pspeed + detime[ei] - dstime[i] > nonlegato_minlen[ii]) &&
					(randbw(0, 100) < nonlegato_freq[ii])) {
					detime[ei] = -min(300, (etime[ei] - stime[ei]) * 100 / m_pspeed / 3);
					artic[i][v] = ARTIC_NONLEGATO;
				}
				// Check if note is too short
				ndur = (etime[ei] - stime[i]) * 100 / m_pspeed + detime[ei] - dstime[i];
				if (ndur < instr_tmin[ii]) {
					CString* st = new CString;
					if (warning_note_short[v] < 4) {
						st->Format("Recommended minimum note length for %s instrument is %d ms. In voice %d note length at step %d is %d ms. Try to change playback speed, instrument or algorithm config.",
							InstName[ii], instr_tmin[ii], v, i, ndur);
						warning_note_short[v] ++;
						WriteLog(1, st);
					}
				}
				// Randomize note starts
				if (rand_start[ii] > 0) dstime[i] += (rand01() - 0.5) * (etime[ei] - stime[i]) * 100 / m_pspeed * rand_start[ii] / 100;
			} // !pause
			if (noff[i][v] == 0) break;
			i += noff[i][v];
		} // for x
	} // for v
}

void CGenTemplate::StartMIDI(int midi_device_i, int latency, int from)
{
	// Clear warning flags
	for (int i = 0; i < v_cnt; i++) warning_note_short[i] = 0;
	// Clear error flag
	buffer_underrun = 0;
	midi_play_step = 0;
	midi_start_time = 0;
	if (from > 0) {
		midi_sent = from;
		midi_sent_t = TIME_PROC(TIME_INFO) + 500;
	}
	else {
		midi_sent_t = 0;
		midi_sent = 0;
	}
	TIME_START;
	Pm_OpenOutput(&midi, midi_device_i, NULL, OUTPUT_BUFFER_SIZE, TIME_PROC, NULL, latency);
	CString* st = new CString;
	st->Format("Pm_OpenOutput: buffer size %d, latency %d", OUTPUT_BUFFER_SIZE, latency);
	WriteLog(4, st);
}

void CGenTemplate::SendMIDI(int step1, int step2)
{
	if (step2 == step1) return;
	milliseconds time_start = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
	PmTimestamp timestamp_current = TIME_PROC(TIME_INFO);
	PmTimestamp timestamp;
	if (midi_sent_t != 0) timestamp = midi_sent_t;
	else timestamp = timestamp_current;
	// Check if we have buffer underrun
	if (timestamp < timestamp_current) {
		CString* st = new CString;
		st->Format("SendMIDI got buffer underrun in %d ms (steps %d - %d)", timestamp_current - timestamp, step1, step2);
		WriteLog(1, st);
		timestamp = timestamp_current;
		buffer_underrun = 1;
	}
	PmTimestamp timestamp0 = timestamp;
	// Set real time when playback started
	if (step1 == 0) midi_start_time = timestamp0;
	else if (midi_start_time == 0) midi_start_time = (midi_sent_t - stime[step1] / m_pspeed * 100);
	// Check if buffer is full
	if (midi_sent_t - timestamp_current > MIN_MIDI_BUFFER_MSEC) {
		CString* st = new CString;
		st->Format("SendMIDI: no need to send (full buffer = %d ms) (steps %d - %d) playback is at %d", 
			midi_sent_t - timestamp_current, step1, step2, timestamp_current - midi_start_time);
		WriteLog(4, st);
		return;
	}
	int i;
	if (!mutex_output.try_lock_for(chrono::milliseconds(3000))) {
		WriteLog(0, new CString("SendMIDI mutex timed out: will try later"));
	}
	int step21; // Voice-dependent first step
	int step22; // Voice-independent last step
	// Find last step not too far
	double time;
	for (i = step1; i <= step2; i++) {
		step22 = i;
		if (i == 0) time = stime[i] * 100 / m_pspeed;
		else time = etime[i - 1] * 100 / m_pspeed;
		if (time - stime[step1] * 100 / m_pspeed + timestamp - timestamp_current > MAX_MIDI_BUFFER_MSEC) break;
	}
	for (int v = 0; v < v_cnt; v++) {
		int ii = instr[v];
		int ncount = 0;
		// Move to note start
		if (coff[step1][v] > 0) step21 = step1 - coff[step1][v];
		else step21 = step1;
		// Count notes
		for (i = step21; i < step22; i++) {
			if (i + len[i][v] > step22) break;
			ncount++;
			if (noff[i][v] == 0) break;
			i += noff[i][v] - 1;
		}
		vector <PmEvent> buffer;
		PmEvent event;
		int ei;
		int ndur;
		// Send notes
		i = step21;
		for (int x = 0; x < ncount; x++) {
			// Fist send last note off, because it was blocked to protect legato ahead
			if ((x == 0) && (i > 0) && (!pause[i - 1][v])) {
				ei = i - 1;
				timestamp = (etime[ei] - stime[step1]) * 100 / m_pspeed + timestamp0 + detime[ei];
				event.timestamp = timestamp;
				event.message = Pm_Message(0x90, note[ei][v] + play_transpose_old[v], 0);
				buffer.push_back(event);
			}
			// Note ON
			timestamp = (stime[i] - stime[step1]) * 100 / m_pspeed + timestamp0 + dstime[i];
			event.timestamp = timestamp;
			event.message = Pm_Message(0x90, note[i][v] + play_transpose[v], dyn[i][v]);
			buffer.push_back(event);
			// Send slur
			if ((instr_type[ii] == INSTR_VIOLIN) && (artic[i][v] == ARTIC_SLUR)) {
				event.timestamp = timestamp - ((stime[i] - stime[i-1]) * 100 / m_pspeed + dstime[i] - dstime[i-1]) / 10;
				event.message = Pm_Message(0x90, slur_ks[ii], 10);
				buffer.push_back(event);
				// Note OFF
				event.timestamp = timestamp + ((stime[i] - stime[i - 1]) * 100 / m_pspeed + dstime[i] - dstime[i - 1]) / 10;
				event.message = Pm_Message(0x90, slur_ks[ii], 0);
				buffer.push_back(event);
			}
			ei = i + len[i][v] - 1;
			ndur = (etime[ei] - stime[i]) * 100 / m_pspeed + detime[ei] - dstime[i];
			// Note OFF
			// Do not send NoteOFF if this is last note, because this will block legato ahead (if needed) in next MidiSend
			if (i < step22 - 1) {
				timestamp = (etime[ei] - stime[step1]) * 100 / m_pspeed + timestamp0 + detime[ei];
				event.timestamp = timestamp;
				event.message = Pm_Message(0x90, note[ei][v] + play_transpose[v], 0);
				buffer.push_back(event);
			}
			if (noff[i][v] == 0) break;
			i += noff[i][v];
			// Save current transpose to be sure that we send correct note off for last note in next SendMIDI call
			play_transpose_old[v] = play_transpose[v];
		}
		// Sort by timestamp before sending
		qsort(buffer.data(), buffer.size(), sizeof(PmEvent), PmEvent_comparator);
		// Send
		Pm_Write(midi, buffer.data(), buffer.size());
	}
	// Save last sent position
	midi_sent = step22;
	midi_sent_t = timestamp0 + (etime[midi_sent-1] - stime[step1]) * 100 / m_pspeed;
	mutex_output.unlock();
	// Count time
	milliseconds time_stop = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
	CString* st = new CString;
	st->Format("Pm_Write steps %d/%d - %d/%d [to future %d to %d ms] (in %d ms) playback is at %d ms", 
		step21, step1, midi_sent, step2, timestamp0 - timestamp_current, midi_sent_t - timestamp_current, 
		time_stop - time_start, timestamp_current-midi_start_time);
	WriteLog(4, st);
}

void CGenTemplate::WriteLog(int i, CString* pST)
{
	if (can_send_log)	::PostMessage(m_hWnd, WM_DEBUG_MSG, i, (LPARAM)pST);
	else delete pST;
}

void CGenTemplate::StopMIDI()
{
	WriteLog(4, new CString("Pm_Close"));
	if (midi != 0) Pm_Close(midi);
	midi = 0;
}

int CGenTemplate::randbw(int n1, int n2)
{
	int re = (double)(n2 - n1 + 1) * rand2() / (double)RAND_MAX;
	re = re + n1;
	return re;
}

double CGenTemplate::rand01()
{
	return (double) rand2() / (double)RAND_MAX;
}

int CGenTemplate::GetPlayStep() {
	if (buffer_underrun == 1) {
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
