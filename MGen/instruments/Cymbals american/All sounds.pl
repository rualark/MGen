# Main
library = "True Strike Cinematic Orchestral Percussion" # For which library algorithm is optimized
Type = 0 # Instrument type
poly = 100 # Maximum number of simultaneous voices
single_stage = 1 # Collapse all tracks for this instrument into a single stage
Pan_default = 53

# Instrument parameters
n_min = "C2" # Lowest note
n_max = "C5" # Highest note
Volume_default = 80 # (direct CC volume, should not be changed by user)
db_max = 0 # Maximum controlled loudness in db when volume CC is 127
db_coef = 3 # Slope of CC to db function (3 for Kontakt, 1 for SWAM)

# Tremolo adaptor
trem_maxlen = 100 # Maximum length of one note inside tremolo for detection
trem_min_repeats = 3 # Minimum number of note repeats to detect tremolo
trem_dyn_range = 0-100 # (0-100) Percent of tremolo dynamics compared to sustain notes
trem_replace = "E3" # Replace tremolo notes with this note
trem_maxint = 127 # Maximum interval between notes of a trill in semitones to be considered a tremolo
trem_end = 0 # 0 - normal ending, 1 - cutoff (need hit), 2 - ending hit
trem_len = 7000 # Tremolo maximum sample length in ms

# Randomization
rnd_vel = 8 # Randomize note velocity not greater than this percent
rand_pos = 0-0 # Randomize note starts-ends not greater than percent of note length 
rand_pos_max = 0-0 # Maximum shift in ms (start-end)

