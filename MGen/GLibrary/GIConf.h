#pragma once
#include "GLib.h"
#include "MidiOut.h"

struct CaseInsensitiveCompare {
	bool operator() (const CString& lhs, const CString& rhs) const {
		return lhs.CompareNoCase(rhs) < 0;
	}
};

// Instrument config
struct IConf {
	CString name; // config name
	CString fname; // config file name
	CString group; // group name
	CString iclass; // class name
	int legato_disable = 0; // Set to 1 to disable legato adaptor
	int default_instr = 0; // Link to default instrument id
	int configs_count = 1; // Number of configs using this group (for default only)
	int port = 0; // If this instrument can receive messages through MIDI port in real time
	int track = 0; // Resulting track for this instrument
	int channel = 0; // Resulting MIDI channel for this instrument (both for MIDI file export and playback to MIDI port)
	int type = 0; 
	int ks1 = 0; // First keyswitch pitch
	//int used = 0; // For how many voices this instrument is already used
	int nmin = 0;
	int nmax = 0;
	int tmin = 0;
	int tmax = 0;
	int pan = 50; // 50 is center
	int vol = 100; // 0-100% of real cc value
	int vol_default = 127; // Real CC value
	CString lib = 0;
	int poly = 0; // Maximum number of simultaneous voices
	int splitpo_freq = 0; // Frequency of split portamento in percent
	int splitpo_mindur = 0; // Minimum legato duration to use split portamento
	int gliss_mindur = 0; // Minimum legato duration to use gliss
	int splitpo_mixed_minint = 0;
	int splitpo_pent_minint = 0;
	int art_freq = 0;
	int art_mindur = 0;
	int art_minint = 0;
	int art_maxint = 0;
	int vel_mul = 100; // Multiply incoming velocity of all midi notes by this percent (100 - do not change)
	int CC_vib = 0; // CC number for vibrato intensity
	int CC_vibf = 0; // CC number for vibrato speed
	float vib_bell_top1 = 0; // Leftmost - rightmost maximum vibrato intensity in note(percent of note duration)
	float vib_bell_top2 = 0; // Leftmost - rightmost maximum vibrato intensity in note(percent of note duration)
	float vibf_bell_top1 = 0; // Leftmost - rightmost maximum vibrato speed in note(percent of note duration)
	float vibf_bell_top2 = 0; // Leftmost - rightmost maximum vibrato speed in note(percent of note duration)
	float vib_bell_freq = 0; // Frequency to apply vibrato bell when all conditions met
	float vib_bell_mindur = 0; // Minimum note duration(ms) that can have a vibrato bell
	float vib_bell_dur = 0; // Minimum note duration(ms) that can have a highest vibrato bell
	float vib_bell1 = 0; // Maximum vibrato intensity in vibrato bell
	float vibf_bell1 = 0; // Max vibrato frequency in vibrato bell
	float vib_bell2 = 0; // Maximum vibrato intensity in vibrato bell
	float vibf_bell2 = 0; // Max vibrato frequency in vibrato bell
	float vib_bell_exp = 0; // Exponent to create non - linear bell shape
	float vibf_bell_exp = 0; // Exponent to create non - linear bell shape
	float rnd_vib = 0; // Randomize vibrato intensity not greater than this percent
	float rnd_vibf = 0; // Randomize vibrato speed not greater than this percent
	int replace_pitch = -1; // Single note that should be sent instead of all other notes
	int CC_steps = 0;
	int CC_ma = 0;
	int CC_dyn = 0;
	int CC_retrigger = 0;
	int retrigger_freq = 0;
	int max_slur_count = 0;
	int max_slur_interval = 0;
	int all_ahead = 0;
	int nonlegato_mingap = 0;
	int nonlegato_maxgap = 1;
	int auto_legato = 0;
	vector<int> legato_ahead;
	vector<int> ahead_chrom;
	vector<short> KswGroup;
	short ksw_group_count = 1;
	map<short, short> child;
	short parent = -1;
	map<char, CString> CCToName;
	map<CString, char, CaseInsensitiveCompare> NameToCC;
	map<char, CString> KswToName;
	map<CString, char, CaseInsensitiveCompare> NameToKsw;
	map<short, CString> TechToName;
	map<CString, short, CaseInsensitiveCompare> NameToTech;
	vector<list<PmMessage>> tech; // [tech][command]
	list<PmMessage> InitCommands; // [command]
	float legato_ahead_exp = 0; // Exponent to interpolate note movement ahead from note velocity
	float leg_pdur = 0; // Maximum percent of previous note duration, that legato transition can take
	float leg_cdur = 0; // Maximum percent of current note duration, that legato transition can take
	float nonlegato_freq = 0;
	int nonlegato_minlen = 0;
	int stac_maxlen = 0; // Maximum note length (ms) to be converted to staccato in case of non-legato on both sides
	int stac_dynamics = 0; // Percent of staccato dynamics compared to sustain notes
	int lengroup2 = 0;
	int lengroup3 = 0;
	int lengroup4 = 0;
	int lengroup_edt1 = 0;
	int lengroup_edt2 = 0;
	float rand_start = 0;
	float rand_end = 0;
	float rnd_vel = 0; // Randomize note velocity not greater than this percent
	float rnd_vel_repeat = 0; // Randomize note velocity not greater than this percent for note retriggers
	float rnd_dyn = 0; // Randomize step dynamics not greater than this percent
	int retrigger_min_len = 0; // Minimum next note length in ms to use retrigger
	int retrigger_rand_end = 0; // Maximum percent of note length to move note end to the left in case of nonlegato
	int retrigger_rand_max = 0; // Maximum length in ms to move note end to the left in case of nonlegato retrigger
	int harsh_freq = 0; // Frequency of harsh sustain articulation in percent of all possible situations
	int vel_harsh = 0; // Velocity equal or above this value triggers harsh sustain
	int vel_immediate = 0; // Velocity equal or above this value triggers immediate sustain
	int vel_normal = 0; // Velocity equal or above this value triggers normal sustain
	int vel_gliss = 0; // Velocity below this value triggers glissando transition
	int vel_normal_minlen = 0; // Minimum note length that can have a normal or lower sustain
	int bell_mindur = 0; // Minimum note duration (ms) that can have a bell
	int gliss_minlen = 0; // Minimum note length that can have a gliss transition
	int gliss_freq = 0; // Frequency of gliss articulation in percent
	int rand_start_max = 0; // Maximum shift in ms
	int rand_end_max = 0; // Maximum shift in ms
	int max_ahead_note = 0; // Maximum chromatic interval having ahead property
	float bell_start_mul = 0; // Multiply dynamics by this parameter at bell start
	float bell_end_mul = 0; // Multiply dynamics by this parameter at bell end
	float bell_start_len = 0; // Percent of notelength to use for slope at bell start
	float bell_end_len = 0; // Percent of notelength to use for slope at bell end
	float bell_start_vel = 0; // Set belled note velocity to random between these percents of starting dynamics
	float bell_end_vel = 0; // Set belled note velocity to random between these percents of starting dynamics
	float rbell_mindur = 0; // Minimum note duration(ms) that can have a reverse bell
	float rbell_dur = 0; // Minimum note duration(ms) that will have deepest reverse bell
	float rbell_mul = 0; // Multiply dynamics by this parameter at bell center with mindur
	float rbell_mul2 = 0; // Multiply dynamics by this parameter at bell center with dur
	float rbell_freq = 0; // Frequency to apply reverse bell when all conditions met
	float rbell_pos1 = 0; // Leftmost - rightmost minimum reverse bell position inside window(percent of window duration)
	float rbell_pos2 = 0; // Leftmost - rightmost minimum reverse bell position inside window(percent of window duration)
	float end_sfl_dur = 0; // Duration of short fall ending
	float end_sfl_freq = 0; // Frequency of short fall ending
	float end_pbd_dur = 0; // Duration of pitch bend down ending
	float end_pbd_freq = 0; // Frequency of pitch bend down ending
	float end_vib2_dur = 0; // Duration of vibrato ending2
	float end_vib2_freq = 0; // Frequency of vibrato ending2
	float end_vib_dur = 0; // Duration of vibrato ending
	float end_vib_freq = 0; // Frequency of vibrato ending
};