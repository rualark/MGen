#include "stdafx.h"
#include "SmRnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW 
#endif

CSmoothRandom::CSmoothRandom()
{
}

CSmoothRandom::~CSmoothRandom()
{
}

float CSmoothRandom::MakeNext()
{
	// Create second derivative of velocity
	acc2 = rand01() * (a2_range - -a2_range) + -a2_range;
	// Calculate acceleration
	acc += acc2;
	if (acc > a_range) acc = a_range;
	if (acc < -a_range) acc = -a_range;
	// Calculate velocity
	vel += acc;
	if (vel > v_range) {
		vel = v_range;
		if (acc > 0) acc = -acc / 2;
	}
	if (vel < -v_range) {
		vel = -v_range;
		if (acc < 0) acc = -acc / 2;
	}
	// Calculate signal
	sig += vel;
	if (sig > s_range) {
		sig = s_range;
		if (vel > 0) vel = -vel / 4;
		if (acc > 0) acc = -acc / 4;
	}
	if (sig < -s_range) {
		sig = -s_range;
		if (vel < 0) vel = -vel / 4;
		if (acc < 0) acc = -acc / 4;
	}
	return sig;
}
