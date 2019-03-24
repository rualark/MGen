# Main
library = "True Strike Cinematic Orchestral Percussion" # For which library algorithm is optimized
Type = 0 # Instrument type
poly = 100 # Maximum number of simultaneous voices
Pan_default = 56

# Instrument parameters
n_min = "F3" # Lowest note
n_max = "F6" # Highest note
Volume_default = 80 # (direct CC volume, should not be changed by user)
db_max = 0 # Maximum controlled loudness in db when volume CC is 127
db_coef = 3 # Slope of CC to db function (3 for Kontakt, 1 for SWAM)

# Controls
KswGroup = "C3: Staccato pedal up sound", "C#3: Longer version of staccato"

# Initial setup
Staccato pedal up sound = 101

Pedal_import = 1 # 0 - do not import pedal; 1 - import CC64 pedal from source midi file

# Randomization
rnd_vel = 8 # Randomize note velocity not greater than this percent
rand_pos = 0-0 # Randomize note starts-ends not greater than percent of note length 
rand_pos_max = 0-0 # Maximum shift in ms (start-end)

