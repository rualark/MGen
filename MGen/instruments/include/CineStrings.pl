#Main
Library = "CineStrings CORE 1.0" # For which library algorithm is optimized
Type = 6 # Instrument type

# Velocity of legato note does not affect sound (speed and attack of legato cannot be controlled)

# Instrument parameters
poly = 1 # Maximum number of simultaneous voices

# Controls
Volume_default = 127 # default 127 (direct CC volume, should not be changed by user)
db_max = 0 # Maximum controlled loudness in db when volume CC is 127
db_coef = 3 # Slope of CC to db function (3 for Kontakt, 1 for SWAM)
CC_Name = 1:  "Dynamics"
CC_Name = 2:  "Vibrato intensity"

# These controls should be mapped manually

# These responsiveness / release CC controls do not work correctly - avoid initializing them!

# Initial setup

# Initial setup (can change)

# Automation parameters
CC_dynamics = 1
CC_ma = 9 # Number of CC steps to moving average (please use only odd numbers)
CC_steps = 30 # Number of CC steps in one second

# Nonlegato adaptor
nonlegato_mingap = 1 # Minimum distance between note ending and next note start to sound nonlegato

# Vibrato adaptor
CC_vib = 2 # CC number for vibrato intensity

# Automatic crescendo for long notes without attack
cresc_mindur = 700 # Minimum note duration (ms) that can have automatic crescendo
cresc_mul = 0.8 # Multiply dynamics by this coefficient at crescendo start (0.2 creates smooth slope)
cresc_len = 30 # Percent of note length to use for crescendo

# Automatic diminuendo for long notes
dim_mindur = 300 # Minimum note duration (ms) that can have automatic diminuendo
dim_mul = 0.4 # Multiply dynamics by this coefficient at diminuendo end (0.2 creates smooth slope)
dim_len = 30 # Percent of note length to use for diminuendo

# Reverse bell adaptor
rbell_freq = 0 # Frequency to apply reverse bell when all conditions met
rbell_dur = 300-1000 # Minimum note duration (ms) that can have a reverse bell - that will have deepest reverse bell
rbell_mul = 0.8-0.2 # Multiply dynamics by this parameter at bell center with mindur - with longer dur
rbell_pos = 20-80 # Leftmost-rightmost minimum reverse bell position inside window (percent of window duration)

# Vibrato adaptor
vib_bell_dur = 200-600 # Minimum note duration (ms) that can have a vibrato bell - division between short and long notes
vib_bell_top = 10-90 # Leftmost-rightmost maximum vibrato intensity in note (percent of note duration)
vib_bell_exp = 2 # Exponent to create non-linear bell shape (left)
vib_bell_rexp = 2 # Exponent to create non-linear bell shape (right)
vib_bell_freq = 100 # Frequency to apply vibrato bell when all conditions met
vib_dyn = 30-110 # Min-max dynamics. Below min dynamics there is no vibrato. Above max dynamics vibrato is max vibrato
vib_bell = 30-127 # Maximum vibrato intensity in vibrato bell (for minimum and highest dynamics)
rnd_vib = 10 # Randomize vibrato intensity not greater than this percent
rnd_vib_slow = 2 # Slow down random vibrato function by this value. Can be only integer: 1 and above

# Vibrato adaptor - short notes
vib_sbell_top = 10-90 # Leftmost-rightmost maximum vibrato intensity in note (percent of note duration)
vib_sbell_exp = 0.25 # Exponent to create non-linear bell shape (left)
vib_sbell_rexp = 0.25 # Exponent to create non-linear bell shape (right)
vib_sbell_freq = 100 # Frequency to apply vibrato bell when all conditions met
vib_sdyn = 30-110 # Min-max dynamics. Below min dynamics there is no vibrato. Above max dynamics vibrato is max vibrato
vib_sbell = 30-127 # Maximum vibrato intensity in vibrato bell (for minimum and highest dynamics)

# Randomization
rnd_vel = 8 # Randomize note velocity not greater than this percent
rand_pos = 0-0 # Randomize note starts-ends not greater than percent of note length 
rand_pos_max = 0-0 # Maximum shift in ms (start-end)
rnd_dyn = 10 # Randomize step dynamics not greater than this percent
rnd_dyn_slow = 3 # Slow down random dynamics function by this value. Can be only integer: 1 and above

# Unison adaptor
unison_dyn_mul = 1.3 # Multiply dynamics of staying unison note by this value
