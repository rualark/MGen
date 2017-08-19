#pragma once

#include "../rtmidi/RtMidi.h"
#include "../readerwriterqueue/readerwriterqueue.h"
#include "../readerwriterqueue/atomicops.h"

typedef int32_t PmMessage;
typedef int32_t PmTimestamp;

typedef struct {
	PmMessage      message;
	PmTimestamp    timestamp;
} PmEvent;

/**
This is a test
Pm_Message() encodes a short Midi message into a 32-bit word. If data1
and/or data2 are not present, use zero.
 
Pm_MessageStatus(), Pm_MessageData1(), and
Pm_MessageData2() extract fields from a 32-bit midi message.
*/
#define Pm_Message(status, data1, data2) \
         ((((data2) << 16) & 0xFF0000) | \
          (((data1) << 8) & 0xFF00) | \
          ((status) & 0xFF))
#define Pm_MessageStatus(msg) ((msg) & 0xFF)
#define Pm_MessageData1(msg) (((msg) >> 8) & 0xFF)
#define Pm_MessageData2(msg) (((msg) >> 16) & 0xFF)

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
	int m_error_type = 0;

protected:
	static UINT MidiThread(LPVOID pParam);
	BlockingReaderWriterQueue<PmEvent> q;
	RtMidiOut *rmo;
	CWinThread* m_MidiThread = 0;

	// Which notes need to get OFF
	vector< vector <int> > note_on; // 1 = note is ON
};

