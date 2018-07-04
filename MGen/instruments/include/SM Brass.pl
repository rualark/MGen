# Samplemodeling Brass

# Main
Type = 2 # Instrument type
poly = 1 # Maximum number of simultaneous voices
unison_dyn_mul = 1.02 # Unisons should not increase brass dynamics significantly, because brass dynamics is non-linear

# Controls
Volume_default = 127 # default 127 (direct CC volume, should not be changed by user)
CC_Name = 11: "Dynamics"
CC_Name = 1:  "Vibrato intensity"
CC_Name = 19: "Vibrato speed"
CC_Name = 26: "Attack time"
CC_Name = 27: "Release time"
CC_Name = 20: "Attack modulation"
CC_Name = 22: "Transition flutter"
CC_Name = 21: "Growl"
CC_Name = 23: "Flutter tongue"
CC_Name = 100: "Mute"
CC_Name = 24: "Dynamics to pitch"
CC_Name = 28: "Random detune"
CC_Name = 32: "Pitch fluctuation"
CC_Name = 25: "Velocity to dynamics"
CC_Name = 5: "Portamento time" # Only if selected for controlling portamento time. Usually velocity is used
CC_Name = 91: "Harm. Group1 gain"
CC_Name = 92: "Harm. Group2 gain"
CC_Name = 93: "Harm. Group3 gain"
CC_Name = 90: "Harm. Form. shift"
CC_Name = 29: "Early reflections"
CC_Name = 31: "Predelay"
CC_Name = 14: "Distance"

# Techniques mapping to commands
Technique = "senza sord; Mute: 0"
Technique = "flutter; Flutter tongue: 100"
Technique = "ord; Flutter tongue: 0"

# Initial setup
Early reflections = 0
Predelay = 0
Distance = 0
Harm. Group1 gain = 64
Harm. Group2 gain = 64
Harm. Group3 gain = 64
Harm. Form. shift = 64
InitTechnique = "senza sord"

# Automation parameters
CC_dynamics = 11
CC_steps = 30 # Number of CC steps in one note step (please use only odd numbers)
CC_ma = 9 # Number of CC steps to moving average (please use only odd numbers)

# Legato adaptor
legato_ahead = 700 # Time in ms to stretch legato notes back to cope with legato delay
leg_pdur = 30 # Maximum percent of previous note duration, that legato transition can take
leg_cdur = 30 # Maximum percent of current note duration, that legato transition can take
legato_ahead_exp = 6 # Exponent to interpolate note movement ahead from note velocity
max_ahead_note = 64 # Maximum chromatic interval having ahead property
gliss_freq = 40 # Frequency of gliss articulation in percent
splitpo_freq = 20 # Frequency of split portamento in percent
splitpo_mindur = 40 # Minimum legato duration to use split portamento
gliss_mindur = 30 # Minimum legato duration to use gliss
splitpo_pent_minint = 3 # Minimum allowed interval in semitones for split portamento pentatonic

# Nonlegato adaptor
acc_range = 1-127 # (1-127) Map dynamics to specified accent range
all_ahead = 25 # Time in ms to stretch sutain notes (not legato) back to cope with slow attack
nonlegato_freq = 13 # Frequency (in percent) when legato can be replaced with non-legato by moving note end to the left
nonlegato_minlen = 200 # Minimum note length (in ms) allowed to convert to nonlegato
nonlegato_maxgap = 300 # Maximum gap between notes (in ms) introduced by automatic nonlegato 
nonlegato_mingap = 7 # Minimum distance between note ending and next note start to sound nonlegato

# Retrigger adaptor
retrigger_freq = 50 # Percent to use non-legato when repeating notes
retrigger_min_len = 600 # Minimum next note length in ms to use retrigger
retrigger_rand_max = 300 # Maximum length in ms to move note end to the left in case of nonlegato retrigger
retrigger_rand_end = 70 # Maximum percent of note length to move note end to the left in case of nonlegato retrigger

# Bell adaptor
bell_mindur = 800-300 # Minimum note duration (ms) that can have a bell
bell_mul = 0.6-0.2 # Multiply dynamics by this parameter at bell start-end
bell_len = 30-30 # Percent of notelength to use for slope at bell start-end
bell_vel = 80-120 # Set belled note velocity to random between these percents of starting dynamics

# Reverse bell adaptor
rbell_freq = 80 # Frequency to apply reverse bell when all conditions met
rbell_dur = 1000-3000 # Minimum note duration (ms) that can have a reverse bell - that will have deepest reverse bell
rbell_mul = 0.9-0.2 # Multiply dynamics by this parameter at bell center with mindur - with longer dur
rbell_pos = 20-80 # Leftmost-rightmost minimum reverse bell position inside window (percent of window duration)

# Vibrato adaptor
CC_vib = 1 # CC number for vibrato intensity
CC_vibf = 19 # CC number for vibrato speed
vib_bell_top = 40-90 # Leftmost-rightmost maximum vibrato intensity position in note (percent of note duration)
vibf_bell_top = 30-90 # Leftmost-rightmost maximum vibrato speed in note (percent of note duration)
vib_bell_exp = 2 # Exponent to create non-linear bell shape (left)
vibf_bell_exp = 2 # Exponent to create non-linear bell shape (left)
vib_bell_rexp = 2 # Exponent to create non-linear bell shape (right)
vibf_bell_rexp = 2 # Exponent to create non-linear bell shape (right)
vib_bell_freq = 100 # Frequency to apply vibrato bell when all conditions met
vib_bell_dur = 200-600 # Minimum note duration (ms) that can have a vibrato bell - that can have highest vibrato bell
vib_bell = 30-60 # Maximum vibrato intensity in vibrato bell (for minimum and highest dynamics)
vibf_bell = 10-60 # Max vibrato frequency in vibrato bell (for minimum and highest dynamics)
vib_dyn = 30-110 # Min-max dynamics. Below min dynamics there is no vibrato. Above max dynamics vibrato is max vibrato
rnd_vib = 10 # Randomize vibrato intensity not greater than this percent
rnd_vibf = 10 # Randomize vibrato speed not greater than this percent
rnd_vib_slow = 2 # Slow down random vibrato function by this value. Can be only integer: 1 and above
rnd_vibf_slow = 2 # Slow down random vibrato speed function by this value. Can be only integer: 1 and above

# Randomization
rnd_vel = 8 # Randomize note velocity not greater than this percent
rnd_vel_repeat = 30 # Randomize note velocity not greater than this percent for note retriggers
rnd_dyn = 15 # Randomize step dynamics not greater than this percent
rnd_dyn_slow = 3 # Slow down random dynamics function by this value. Can be only integer: 1 and above
rand_pos = 8-0 # Randomize note starts-ends not greater than percent of note length 
rand_pos_max = 70-0 # Maximum shift in ms (start-end)

# Flutter tongue adaptor
trem_maxlen = 100 # Maximum length of one note inside tremolo for detection
trem_min_repeats = 3 # Minimum number of note repeats to detect tremolo
trem_dyn_range = 0-100 # (0-100) Percent of tremolo dynamics compared to sustain notes
trem_activate = "flutter" # Choose tremolo technique that can be activated from MIDI file
trem_deactivate = "ord" # Choose tremolo technique that can be activated from MIDI file
