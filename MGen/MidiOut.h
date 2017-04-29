#pragma once

#include "portmidi/portmidi.h"
#include "portmidi/porttime.h"
#include "rtmidi/RtMidi.h"
#include "readerwriterqueue/readerwriterqueue.h"
#include "readerwriterqueue/atomicops.h"

#define TIME_PROC ((int32_t (*)(void *)) Pt_Time)
#define TIME_INFO NULL

#define WAIT_MS 50

using namespace moodycamel;

typedef vector<unsigned char> mMessage;

typedef struct {
	mMessage      message;
	PmTimestamp   timestamp;
} mEvent;

class CMidiOut
{
public:
	CMidiOut();
	~CMidiOut();

	int StartMidi();
	int StopMidi();
	int QueueEvent(mEvent event);

	int need_exit;

protected:
	static UINT MidiThread(LPVOID pParam);
	BlockingReaderWriterQueue<mEvent> q;
	RtMidiOut *rmo;
	CWinThread* m_MidiThread;

};

