# Main
library = "True Strike Cinematic Orchestral Percussion" # For which library algorithm is optimized
Type = 0 # Instrument type
poly = 1 # Maximum number of simultaneous voices

# Instrument parameters
n_min = "C2" # Lowest note
n_max = "B5" # Highest note
Volume_default = 65 # (direct CC volume, should not be changed by user)

# Controls
KswGroup = "C1: Long hits", "C#1: Dampened hits"
CC_dynamics = 1
CC_ma = 3 # Number of CC steps to moving average (please use only odd numbers)
CC_steps = 1 # Number of CC steps in one note step (please use only odd numbers)

# Tremolo adaptor
trem_maxlen = 100 # Maximum length of one note inside tremolo for detection
trem_min_repeats = 3 # Minimum number of note repeats to detect tremolo
trem_dyn_range = 0-100 # Percent of tremolo dynamics compared to sustain notes
trem_chan = 2 # Send tremolo notes to separate channel specified

