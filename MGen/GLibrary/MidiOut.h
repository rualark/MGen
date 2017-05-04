#pragma once

#include "../portmidi/portmidi.h"
#include "../portmidi/porttime.h"
#include "../rtmidi/RtMidi.h"
#include "../readerwriterqueue/readerwriterqueue.h"
#include "../readerwriterqueue/atomicops.h"

#define TIME_PROC ((int32_t (*)(void *)) Pt_Time)
#define TIME_INFO NULL

#define WAIT_MS 50

// MIDI PORT STATUS
#define MIDI_NOTEON 0x90
#define MIDI_NOTEOFF 0x90
#define MIDI_CC 0xB0
#define MIDI_ALLOFF 123

using namespace moodycamel;

typedef vector<unsigned char> mMessage;

class CMidiOut
{
public:
	CMidiOut();
	~CMidiOut();

	int StartMidi(int port);
	int StopMidi();
	int QueueEvent(PmEvent event);

	int need_exit = 0;
	CString m_error;
	int m_error_type;

protected:
	static UINT MidiThread(LPVOID pParam);
	BlockingReaderWriterQueue<PmEvent> q;
	RtMidiOut *rmo;
	CWinThread* m_MidiThread;

	// Which notes need to get OFF
	vector< vector <int> > note_on; // 1 = note is ON
};

