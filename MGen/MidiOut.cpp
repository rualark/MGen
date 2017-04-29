#include "stdafx.h"
#include "MidiOut.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CMidiOut::CMidiOut()
{
}

CMidiOut::~CMidiOut()
{
}

int CMidiOut::StartMidi()
{
	// Open RtMidi
	rmo = new RtMidiOut();
	rmo->openPort(0);
	// Begin MIDI playback thread
	AfxBeginThread(MidiThread, this, THREAD_PRIORITY_TIME_CRITICAL);
	return 0;
}

int CMidiOut::StopMidi()
{
	delete rmo;
	return 0;
}

int CMidiOut::QueueEvent(mEvent event)
{
	q.enqueue(event);
	return 0;
}

UINT CMidiOut::MidiThread(LPVOID pParam)
{
	// Need to get pointer to this object, because thread function is static
	CMidiOut* pMO = (CMidiOut*)pParam;
	// if Object is not valid  
	if (pMO == NULL) return 1;
	mEvent event;
	PmTimestamp timestamp_current;
	int wait_time;
	while (true) {
		// Check if we need to exit
		if (pMO->need_exit) break;
		// Wait for next event with timeout
		if (pMO->q.wait_dequeue_timed(event, milliseconds(50))) {
			// Get current timestamp
			timestamp_current = TIME_PROC(TIME_INFO);
			wait_time = event.timestamp - timestamp_current;
			// Wait for event timestamp
			if (wait_time > 0) {
				Sleep(wait_time);
			}
			// Send event
			pMO->rmo->sendMessage(&event.message);
		}
	}
	return 0;
}
