#pragma once
#include "GVar.h"
class CGAdapt :
	public CGVar
{
public:
	CGAdapt();
	~CGAdapt();

	void Adapt(int step1, int step2);

	double adapt_pspeed = 100; // Playback speed used during adaptation

protected:
	void CheckRange(int v, int ii);
	void CheckShortStep(int v, int x, int i, int ii, int ei, int pi, int pei);
	void AdaptLengroupStep(int v, int x, int i, int ii, int ei, int pi, int pei);
	void AdaptSlurStep(int v, int x, int i, int ii, int ei, int pi, int pei);
	void AdaptRetriggerStep(int v, int x, int i, int ii, int ei, int pi, int pei);
	void AdaptNonlegatoStep(int v, int x, int i, int ii, int ei, int pi, int pei);
	void AdaptAheadStep(int v, int x, int i, int ii, int ei, int pi, int pei);

	void FixOverlap(int v, int x, int i, int ii, int ei, int pi, int pei);
												 
  // Adapt local variables
	int slur_count; // Number of successive slurs

	// MIDI play warnings for each voice show if warning was already fired to prevent repeated warnings
	vector<int> warning_note_range;
	vector<int> warning_note_short;
	vector<int> warning_note_wrong;

	vector<int> play_transpose; // If generated notes are not in instrument range, playback is automatically transposed (semitones)
};

