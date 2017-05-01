#pragma once

#include "GLib.h"

class CSmoothRandom :
	public CGLib
{
public:
	CSmoothRandom();
	~CSmoothRandom();

	float MakeNext();

	// Variables
	float acc2 = 0; // Second derivative of velocity
	float acc = 0; // First derivative of velocity (acceleration)
	float vel = 0; // Velocity
	float sig = 0; // Signal

	// Constants
	float a2_range = 0.1;
	float a_range = 0.5;
	float v_range = 2.5;
	float s_range = 25;

	// Gravitation
	float a2_grav = 2; // 2 = maximum gravity, 0 = no gravity
	float a_grav = 2; // 2 = maximum gravity, 0 = no gravity
};

