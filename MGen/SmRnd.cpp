#include "stdafx.h"
#include "SmRnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW 
#endif

CSmoothRandom::CSmoothRandom()
{
	InitRandom();
}

CSmoothRandom::~CSmoothRandom()
{
}

float CSmoothRandom::MakeNext()
{
	// Calculate gravitation a2 limits
	float a2_max = a2_range;
	float a2_min = -a2_range;
	if (sig > 0) a2_max = a2_range * (s_range - sig * a2_grav) / s_range;
	if (sig < 0) a2_min = -a2_range * (s_range + sig * a2_grav) / s_range;
	// Calculate gravitation a limits
	float a_max = a_range;
	float a_min = -a_range;
	if (sig > 0) a_max = a_range * (s_range - sig * a_grav) / s_range;
	if (sig < 0) a_min = -a_range * (s_range + sig * a_grav) / s_range;
	// Create second derivative of velocity
	acc2 = rand01() * (a2_max - a2_min) + a2_min;
	// Calculate acceleration
	acc += acc2;
	if (acc > a_max) acc = -acc / 4;
	if (acc < a_min) acc = -acc / 4;
	// Calculate velocity
	vel += acc;
	if (vel > v_range) {
		vel = v_range;
		if (acc > 0) acc = -acc / 4;
	}
	if (vel < -v_range) {
		vel = -v_range;
		if (acc < 0) acc = -acc / 4;
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
