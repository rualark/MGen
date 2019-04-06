# Main
library = "CinePerc 1.1" # For which library algorithm is optimized
Type = 0 # Instrument type
poly = 100 # Maximum number of simultaneous voices
Pan_default = 58
Reverb_fixed = 1 # 0 - reverb can be changed; 1 - reverb cannot be changed (hard coded)

# Instrument parameters
n_min = "A1" # Lowest note
n_max = "D7" # Highest note
Volume_default = 100 # (direct CC volume, should not be changed by user)
Volume = 85
db_max = 0 # Maximum controlled loudness in db when volume CC is 127
db_coef = 3 # Slope of CC to db function (3 for Kontakt, 1 for SWAM)
all_ahead = 73 # Time in ms to stretch all notes (sustains and legato) back to cope with slow attack

CC_dynamics = 1
CC_ma = 3 # Number of CC steps to moving average (please use only odd numbers)
CC_steps = 1 # Number of CC steps in one second

# Controls
KswGroup = "F#1: Snares on", "G#1: Snares off"

# Initial setup
Snares on = 101

# Randomization
rnd_vel = 8 # Randomize note velocity not greater than this percent
rand_pos = 0-0 # Randomize note starts-ends not greater than percent of note length 
rand_pos_max = 0-0 # Maximum shift in ms (start-end)
