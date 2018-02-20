#pragma once
#include "GVar.h"

// Time to fade out smooth random cc (must be lower than MAX_MIDI_BUF_MSEC - MIN_MIDI_BUF_MSEC)
#define CC_FADEOUT 500
// Time to end fadeout before last cc is sent (must be lower than 1000 / CC_steps)
#define CC_FADEOUT_RESERVE 200

class CGAdapt :
	public CGVar
{
public:
	CGAdapt();
	~CGAdapt();

	void AdaptRndVel(int v, int x, int i, int ii, int ei, int pi, int pei);
	void Adapt(int step1, int step2);

	float adapt_pspeed = 100; // Playback speed used during adaptation

protected:
	void CalculateVoiceStages();
	void ExportVoiceStages();
	void SetPauseDyn(int v, int step1, int step2);
	void CheckInstrumentRange(int v, int ii);
	void CheckShortStep(int v, int x, int i, int ii, int ei, int pi, int pei);
	void CheckNoteBreath(int v, int x, int i, int ii, int ei, int pi, int pei);
	void AdaptLengroupStep(int v, int x, int i, int ii, int ei, int pi, int pei);
	void AdaptSlurStep(int v, int x, int i, int ii, int ei, int pi, int pei);
	void AdaptRetriggerRebowStep(int v, int x, int i, int ii, int ei, int pi, int pei);
	void AdaptRetriggerNonlegatoStep(int v, int x, int i, int ii, int ei, int pi, int pei);
	void AdaptAutoLegatoStep(int v, int x, int i, int ii, int ei, int pi, int pei);
	void AdaptNonlegatoStep(int v, int x, int i, int ii, int ei, int pi, int pei);
	inline int GetDrange(int src, int range1, int range2);
	void AdaptStaccatoStep(int v, int x, int i, int ii, int ei, int pi, int pei);
	void AdaptPizzStep(int v, int x, int i, int ii, int ei, int pi, int pei);
	void AdaptAheadStep(int v, int x, int i, int ii, int ei, int pi, int pei);
	void AdaptAllAheadStep(int v, int x, int i, int ii, int ei, int pi, int pei);
	void AdaptFlexAheadStep(int v, int x, int i, int ii, int ei, int pi, int pei);
	void FixOverlap(int v, int x, int i, int ii, int ei, int pi, int pei);
	void AdaptAttackStep(int v, int x, int i, int ii, int ei, int pi, int pei);
	void AdaptLongBell(int v, int x, int i, int ii, int ei, int pi, int pei, int ncount);
	void AdaptReverseBell(int v, int x, int i, int ii, int ei, int pi, int pei);
	void AdaptVibBell(int v, int x, int i, int ii, int ei, int pi, int pei);
	void AdaptNoteEndStep(int v, int x, int i, int ii, int ei, int pi, int pei, int ncount);
	void ApplyTrem(int & started, int step1, int step2, int v, int ii);
	void AdaptTrem(int step1, int step2, int v, int ii);
											 
  // Adapt local variables
	int slur_count = 0; // Number of successive slurs

	// MIDI play warnings for each voice show if warning was already fired to prevent repeated warnings
	vector<int> warning_note_range;
	vector<int> warning_note_short;
	vector<int> warning_note_long;
	vector<int> warning_note_wrong;

	vector<int> play_transpose; // If generated notes are not in instrument range, playback is automatically transposed (semitones)
};

