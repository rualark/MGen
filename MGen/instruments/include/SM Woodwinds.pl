# Samplemodeling Woodwinds

# Main
Type = 4 # Instrument type
poly = 1 # Maximum number of simultaneous voices

# Controls
Volume_default = 89 # default 127 (direct CC volume, should not be changed by user)
db_max = 0 # Maximum controlled loudness in db when volume CC is 127
db_coef = 1 # Slope of CC to db function (2 for Kontakt, 1 for SWAM)
CC_Name = 11: "Dynamics"
CC_Name = 1:  "Vibrato intensity"
CC_Name = 19: "Vibrato speed"
CC_Name = 5: "Portamento time"
CC_Name = 24: "Harmonic structure"
CC_Name = 20: "Subharmonics" # Clarinet, Bass Clarinet, Oboe, English horn, Bassoon, Contrabassoon, Sax
CC_Name = 27: "PipeSplit" # Piccolo, Flute, Alto flute, Bass flute
CC_Name = 21: "Growl"
CC_Name = 23: "Flutter tongue"
CC_Name = 26: "Formant"
CC_Name = 25: "Breath noise"
CC_Name = 9: "Key noise"
CC_Name = 28: "Alt. fingering" # Piccolo, Flute, Alto flute, Bass flute
CC_Name = 8: "Dynamic pitch" # Clarinet, Bass Clarinet, Oboe, English horn, Bassoon, Contrabassoon 
CC_Name = 64: "Overblow on"
CC_Name = 31: "Microtuning on"

# Controls (need to map manually)
CC_Name = 12: "Reverb mix"
CC_Name = 13: "Legato mode" # 0 - Gliss; 127 - Expr
#CC_Name = 7: "Volume"
#CC_Name = 10: "Pan"

# Techniques mapping to commands
Technique = "flutter; Flutter tongue: 100"
Technique = "ord; Flutter tongue: 0"

# Initial setup
Flutter tongue = 0
Harmonic structure = 64
Breath noise = 64
Key noise = 50
Reverb mix = 0
Legato mode = 127

# Automation parameters
CC_dynamics = 11
CC_steps = 30 # Number of CC steps in one second
CC_ma = 9 # Number of CC steps to moving average (please use only odd numbers)

# Legato adaptor
legato_ahead = 25 # Time in ms to stretch legato notes back to cope with legato delay
max_ahead_note = 64 # Maximum chromatic interval having ahead property
gliss_freq = 0 # Disable
legato_long_minlen = 300 # Minimum note duration that can have long legato transition from previous note
vel_legato_long = 64 # Long legato transition goes from gliss_leg_vel to this value

# Nonlegato adaptor
acc_range = 1-127 # (1-127) Map dynamics to specified accent range
all_ahead = 25 # Time in ms to stretch sutain notes (not legato) back to cope with slow attack
nonlegato_freq = 20 # Frequency (in percent) when legato can be replaced with non-legato by moving note end to the left
nonlegato_minlen = 400 # Minimum note length (in ms) allowed to convert to nonlegato
nonlegato_maxgap = 300 # Maximum gap between notes (in ms) introduced by automatic nonlegato 
nonlegato_mingap = 7 # Minimum distance between note ending and next note start to sound nonlegato
# For SM woodwinds real minimum distance for non-legato sample is 31 ms, but ahead is same for all and legato

# Retrigger adaptor
retrigger_freq = 50 # Percent to use non-legato when repeating notes
retrigger_min_len = 300 # Minimum next note length in ms to use retrigger
retrigger_rand_max = 300 # Maximum length in ms to move note end to the left in case of nonlegato retrigger
retrigger_rand_end = 40 # Maximum percent of note length to move note end to the left in case of nonlegato retrigger
retrigger_mingap = 30 # Minimum gap between notes to create during retrigger

# Automatic crescendo for long notes without attack
cresc_mindur = 800 # Minimum note duration (ms) that can have automatic crescendo
cresc_mul = 60 # Multiply dynamics by this percent at crescendo start (0.2 creates smooth slope)
cresc_len = 30 # Percent of note length to use for crescendo
cresc_vel = 80-100 # Set cresc note velocity to random between these percents of starting dynamics

# Automatic diminuendo for long notes
dim_mindur = 300 # Minimum note duration (ms) that can have automatic diminuendo
dim_mul = 20 # Multiply dynamics by this percent at diminuendo end (0.2 creates smooth slope)
dim_len = 30 # Percent of note length to use for diminuendo

# Reverse bell adaptor
rbell_freq = 0 # Frequency to apply reverse bell when all conditions met
rbell_dur = 1000-3000 # Minimum note duration (ms) that can have a reverse bell - that will have deepest reverse bell
rbell_mul = 20-90 # Multiply dynamics by this parameter at bell center with longer-shorter duration
rbell_pos = 20-80 # Leftmost-rightmost minimum reverse bell position inside window (percent of window duration)

# Vibrato adaptor
CC_vib = 1 # CC number for vibrato intensity
CC_vibf = 19 # CC number for vibrato speed
vib_bell_top = 40-90 # Leftmost-rightmost maximum vibrato intensity position in note (percent of note duration)
vibf_bell_top = 20-60 # Leftmost-rightmost maximum vibrato speed in note (percent of note duration)
vib_bell_exp = 2 # Exponent to create non-linear bell shape (left)
vibf_bell_exp = 2 # Exponent to create non-linear bell shape (left)
vib_bell_rexp = 2 # Exponent to create non-linear bell shape (right)
vibf_bell_rexp = 2 # Exponent to create non-linear bell shape (right)
vib_bell_freq = 100 # Frequency to apply vibrato bell when all conditions met
vib_sbell_freq = 100 # Frequency to apply vibrato bell when all conditions met
vib_bell_dur = 200-600 # Minimum note duration (ms) that can have a vibrato bell - that can have highest vibrato bell
vib_bell = 30-90 # Maximum vibrato intensity in vibrato bell (for minimum and highest dynamics)
vibf_bell = 10-60 # Max vibrato frequency in vibrato bell (for minimum and highest dynamics)
vib_dyn = 30-110 # Min-max dynamics. Below min dynamics there is no vibrato. Above max dynamics vibrato is max vibrato
rnd_vib = 10 # Randomize vibrato intensity not greater than this percent
rnd_vibf = 10 # Randomize vibrato speed not greater than this percent
rnd_vib_slow = 2 # Slow down random vibrato function by this value. Can be only integer: 1 and above
rnd_vibf_slow = 2 # Slow down random vibrato speed function by this value. Can be only integer: 1 and above

# Vibrato adaptor - short notes
vib_sbell_top = 0-0 # Leftmost-rightmost maximum vibrato intensity in note (percent of note duration)
vibf_sbell_top = 0-0 # Leftmost-rightmost maximum vibrato speed in note (percent of note duration)
vib_sbell_exp = 0.25 # Exponent to create non-linear bell shape (left)
vibf_sbell_exp = 0.25 # Exponent to create non-linear bell shape (left)
vib_sbell_rexp = 0.25 # Exponent to create non-linear bell shape (right)
vibf_sbell_rexp = 0.25 # Exponent to create non-linear bell shape (right)
vib_sbell_freq = 100 # Frequency to apply vibrato bell when all conditions met
vib_sdyn = 30-110 # Min-max dynamics. Below min dynamics there is no vibrato. Above max dynamics vibrato is max vibrato
vib_sbell = 0-0 # Maximum vibrato intensity in vibrato bell (for minimum and highest dynamics)
vibf_sbell = 0-0 # Max vibrato frequency in vibrato bell (for minimum and highest dynamics)

# Randomization
rnd_vel = 20 # Randomize note velocity not greater than this percent
rnd_vel_repeat = 50 # Randomize note velocity not greater than this percent for note retriggers
rnd_dyn = 15 # Randomize step dynamics not greater than this percent
rnd_dyn_slow = 3 # Slow down random dynamics function by this value. Can be only integer: 1 and above
rand_pos = 0-0 # Randomize note starts-ends not greater than percent of note length 
rand_pos_max = 0-0 # Maximum shift in ms (start-end)

# Flutter tongue adaptor
trem_maxlen = 100 # Maximum length of one note inside tremolo for detection
trem_min_repeats = 3 # Minimum number of note repeats to detect tremolo
trem_dyn_range = 0-100 # (0-100) Percent of tremolo dynamics compared to sustain notes
trem_activate = "flutter" # Choose tremolo technique that can be activated from MIDI file
trem_deactivate = "ord" # Choose tremolo technique that can be activated from MIDI file

# Unison adaptor
unison_dyn_mul = 1.3 # Multiply dynamics of staying unison note by this value
