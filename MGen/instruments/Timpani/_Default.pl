# Main
library = "True Strike Cinematic Orchestral Percussion" # For which library algorithm is optimized
Type = 0 # Instrument type
poly = 1 # Maximum number of simultaneous voices

# Instrument parameters
n_min = "C2" # Lowest note
n_max = "B5" # Highest note
Volume_default = 103 # (direct CC volume, should not be changed by user)
db_max = 0 # Maximum controlled loudness in db when volume CC is 127
db_coef = 2 # Slope of CC to db function (2 for Kontakt, 1 for SWAM)

# Controls
KswGroup = "C1: Long hits", "C#1: Dampened hits"
CC_dynamics = 1
CC_ma = 3 # Number of CC steps to moving average (please use only odd numbers)
CC_steps = 1 # Number of CC steps in one note step (please use only odd numbers)

# Tremolo adaptor
trem_maxlen = 100 # Maximum length of one note inside tremolo for detection
trem_min_repeats = 3 # Minimum number of note repeats to detect tremolo
trem_dyn_range = 0-100 # (0-100) Percent of tremolo dynamics compared to sustain notes
trem_chan = 2 # Send tremolo notes to separate channel specified
trem_maxint = 12 # Maximum interval between notes of a trill in semitones to be considered a tremolo
trem_maxint = 127 # Maximum interval between notes of a trill in semitones to be considered a tremolo
trem_end = 2 # 0 - normal ending, 1 - cutoff (need hit), 2 - ending hit

# Randomization
rnd_vel = 8 # Randomize note velocity not greater than this percent
rand_pos = 8-0 # Randomize note starts-ends not greater than percent of note length 
rand_pos_max = 40-0 # Maximum shift in ms (start-end)

