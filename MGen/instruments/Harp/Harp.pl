# Main
library = "ProjectSAM: Concert Harp Exp" # For which library algorithm is optimized
Type = 0 # Instrument type
poly = 100 # Maximum number of simultaneous voices

# Instrument parameters
n_min = "C#1" # Lowest note
n_max = "F#7" # Highest note
Volume_default = 127 # (direct CC volume, should not be changed by user)

# Randomization
rnd_vel = 8 # Randomize note velocity not greater than this percent
rand_pos = 8-0 # Randomize note starts-ends not greater than percent of note length 
rand_pos_max = 40-0 # Maximum shift in ms (start-end)

