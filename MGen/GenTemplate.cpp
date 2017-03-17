#include "stdafx.h"
#include "GenTemplate.h"


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
}


CGenTemplate::~CGenTemplate()
{
}

void CGenTemplate::Generate()
{
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

void CGenTemplate::Init()
{
	// Init rand
	srand((unsigned int)time(NULL));
	// ISAAC
	ub4 i;
	aa = bb = cc = (ub4)0;
	for (i = 0; i<256; ++i) mm[i] = randrsl[i] = rand()*rand();
	randinit(1);
	// Create vectors
	pause = vector<vector<unsigned char>>(t_allocated, vector<unsigned char>(v_cnt));
	note = vector<vector<unsigned char>>(t_allocated, vector<unsigned char>(v_cnt));
	len = vector<vector<unsigned char>>(t_allocated, vector<unsigned char>(v_cnt));
	coff = vector<vector<unsigned char>>(t_allocated, vector<unsigned char>(v_cnt));
	poff = vector<vector<unsigned char>>(t_allocated, vector<unsigned char>(v_cnt));
	noff = vector<vector<unsigned char>>(t_allocated, vector<unsigned char>(v_cnt));
	tempo = vector<vector<unsigned char>>(t_allocated, vector<unsigned char>(v_cnt));
	att = vector<vector<unsigned char>>(t_allocated, vector<unsigned char>(v_cnt));
}

void CGenTemplate::ResizeVectors(int size)
{
	if (!mutex_output.try_lock_for(chrono::milliseconds(1000))) {
		::PostMessage(m_hWnd, WM_WARN_MSG, 0, (LPARAM)new CString("ResizeVectors mutex timed out"));
	}
	CString* st = new CString;
	st->Format("ResizeVectors to %d", size);
	::PostMessage(m_hWnd, WM_DEBUG_MSG, 0, (LPARAM)st);
	pause.resize(size);
	note.resize(size);
	len.resize(size);
	coff.resize(size);
	poff.resize(size);
	noff.resize(size);
	tempo.resize(size);
	att.resize(size);
	for (int i = t_allocated; i < size; i++) {
		pause[i].resize(v_cnt);
		note[i].resize(v_cnt);
		len[i].resize(v_cnt);
		coff[i].resize(v_cnt);
		poff[i].resize(v_cnt);
		noff[i].resize(v_cnt);
		tempo[i].resize(v_cnt);
		att[i].resize(v_cnt);
	}
	t_allocated = size;
	mutex_output.unlock();
}

