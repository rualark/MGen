# Main
library = "CinePerc 1.1" # For which library algorithm is optimized
Type = 0 # Instrument type
poly = 100 # Maximum number of simultaneous voices

# Instrument parameters
n_min = "C2" # Lowest note
n_max = "Bb7" # Highest note
Volume_default = 100 # (direct CC volume, should not be changed by user)
Volume = 85
db_max = 0 # Maximum controlled loudness in db when volume CC is 127
db_coef = 3 # Slope of CC to db function (3 for Kontakt, 1 for SWAM)

CC_dynamics = 1
CC_ma = 3 # Number of CC steps to moving average (please use only odd numbers)
CC_steps = 1 # Number of CC steps in one note step (please use only odd numbers)

# Tremolo adaptor
trem_maxlen = 100 # Maximum length of one note inside tremolo for detection
trem_min_repeats = 3 # Minimum number of note repeats to detect tremolo
trem_dyn_range = 0-100 # (0-100) Percent of tremolo dynamics compared to sustain notes
trem_maxint = 127 # Maximum interval between notes of a trill in semitones to be considered a tremolo

# Randomization
rnd_vel = 8 # Randomize note velocity not greater than this percent
rand_pos = 8-0 # Randomize note starts-ends not greater than percent of note length 
rand_pos_max = 40-0 # Maximum shift in ms (start-end)

