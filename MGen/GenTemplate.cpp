#include "stdafx.h"
#include "GenTemplate.h"

#include "midifile/MidiFile.h"

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

CGenTemplate::CGenTemplate()
{
	instr[0] = 1;
}


CGenTemplate::~CGenTemplate()
{
	StopMIDI();
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
	srand((unsigned int)time(NULL));
	// ISAAC
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

void CGenTemplate::InitVectors()
{
	// Create vectors
	pause = vector<vector<unsigned char>>(t_allocated, vector<unsigned char>(v_cnt));
	note = vector<vector<unsigned char>>(t_allocated, vector<unsigned char>(v_cnt));
	len = vector<vector<unsigned char>>(t_allocated, vector<unsigned char>(v_cnt));
	coff = vector<vector<unsigned char>>(t_allocated, vector<unsigned char>(v_cnt));
	poff = vector<vector<unsigned char>>(t_allocated, vector<unsigned char>(v_cnt));
	noff = vector<vector<unsigned char>>(t_allocated, vector<unsigned char>(v_cnt));
	att = vector<vector<unsigned char>>(t_allocated, vector<unsigned char>(v_cnt));
	comment = vector<vector<CString>>(t_allocated, vector<CString>(v_cnt));
	color = vector<vector<Color>>(t_allocated, vector<Color>(v_cnt));
	tempo = vector<double>(t_allocated);
	stime = vector<double>(t_allocated);
	etime = vector<double>(t_allocated);
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

void CGenTemplate::LoadInstruments()
{
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
	att.resize(size);
	comment.resize(size);
	color.resize(size);
	for (int i = t_allocated; i < size; i++) {
		pause[i].resize(v_cnt);
		note[i].resize(v_cnt);
		len[i].resize(v_cnt);
		coff[i].resize(v_cnt);
		poff[i].resize(v_cnt);
		noff[i].resize(v_cnt);
		att[i].resize(v_cnt);
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
		SaveVector2C(fs, att, i);
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
		LoadVector2C(fs, att, i);
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
		midifile.addTrackName(track, 0, "Melody");
		midifile.addPatchChange(track, 0, channel, 0); // 40=violin
		for (int i = 0; i < t_generated; i++) if (pause[i][v] == 0) {
			midifile.addNoteOn(track, (tpq*4)+ tpñ*i, channel, note[i][v], att[i][v]);
			midifile.addNoteOff(track, (tpq * 4) + tpñ*(i+len[i][v])-1, channel, note[i][v], 0);
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

void CGenTemplate::StartMIDI(int midi_device_i, int latency, int from)
{
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
	// Set playback start
	if (step1 == 0) midi_start_time = timestamp0;
	else if (midi_start_time == 0) midi_start_time = midi_sent_t - stime[step1];
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
		// Send notes
		PmEvent* buffer = new PmEvent[ncount * 2];
		i = step21;
		for (int x = 0; x < ncount; x++) {
			// Note ON
			timestamp = (stime[step21] - stime[step1]) * 100 / m_pspeed + timestamp0;
			buffer[x * 2].timestamp = timestamp;
			buffer[x * 2].message = Pm_Message(0x90, note[i][v], att[i][v]);
			// Note OFF
			timestamp = (etime[i + len[i][v] - 1] - stime[step1]) * 100 / m_pspeed + timestamp0;
			buffer[x * 2 + 1].timestamp = timestamp;
			buffer[x * 2 + 1].message = Pm_Message(0x90, note[i][v], 0);
			if (noff[i][v] == 0) break;
			i += noff[i][v];
		}
		Pm_Write(midi, buffer, ncount * 2);
		delete[] buffer;
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
