#Main
Library = "Soundiron Voices of Rapture 1.0 - 2017-03-23" # For which library algorithm is optimized
Type = 3 # Instrument type
poly = 1 # Maximum number of simultaneous voices

# Automation parameters
CC_dynamics = 72
CC_ma = 9 # Number of CC steps to moving average (please use only odd numbers)
CC_steps = 30 # Number of CC steps in one note step (please use only odd numbers)

# Controls
Volume_default = 127 # default 127 (direct CC volume, should not be changed by user)
CC_Name = 72: "Dynamics"
CC_Name = 74: "Attack"
CC_Name = 76: "Release time"
CC_Name = 78: "Offset"
CC_Name = 80: "Vibrato intensity"
CC_Name = 90: "Release volume"

# Controls to map manually
CC_Name = 2: "Legato transition speed"
CC_Name = 3: "Legato on"
CC_Name = 4: "Chord auto-pan on"
CC_Name = 5: "Release samples on"

# Initial setup
InitCommand = "Attack: 0"
InitCommand = "Offset: 0"
InitCommand = "Release time: 127"
InitCommand = "Release volume: 127"
InitCommand = "Legato transition speed: 0"
InitCommand = "Legato on"
InitCommand = "Chord auto-pan on: 0"
InitCommand = "Release samples on"
Pan = 50 # default 50 (maximum left, 50 - default position, 100 - maximum right)
Volume = 100 # default 100 (relative volume from 0 to 100)

# Randomization
rand_pos = 8-0 # Randomize note starts-ends not greater than percent of note length 
rand_pos_max = 70-0 # Maximum shift in ms (start-end)
rnd_vel = 8 # Randomize note velocity not greater than this percent
rnd_dyn = 10 # Randomize step dynamics not greater than this percent
rnd_dyn_slow = 3 # Slow down random dynamics function by this value. Can be only integer: 1 and above

# Nonlegato adaptor
nonlegato_mingap = 100 # Minimum gap between notes (in ms) introduced by automatic nonlegato 

# Legato adaptor
all_ahead = 125 # Time in ms to stretch all notes (sustains and legato) back to cope with slow attack

# Nonlegato adaptor
nonlegato_freq = 20 # Frequency (in percent) when legato can be replaced with non-legato by moving note end to the left
nonlegato_minlen = 400 # Minimum note length (in ms) allowed to convert to nonlegato
nonlegato_maxgap = 300 # Maximum gap between notes (in ms) introduced by automatic nonlegato 

# Bell adaptor
bell_mindur = 700-200 # Minimum note duration (ms) that can have a bell
bell_mul = 0.2-0.2 # Multiply dynamics by this parameter at bell start-end
bell_len = 30-30 # Percent of notelength to use for slope at bell start-end

# Reverse bell adaptor
rbell_freq = 80 # Frequency to apply reverse bell when all conditions met
rbell_dur = 300-1000 # Minimum note duration (ms) that can have a reverse bell - that will have deepest reverse bell
rbell_mul = 0.8-0.2 # Multiply dynamics by this parameter at bell center with mindur - with longer dur
rbell_pos = 20-80 # Leftmost-rightmost minimum reverse bell position inside window (percent of window duration)

# Vibrato adaptor
CC_vib = 80 # CC number for vibrato intensity
vib_bell_top = 5-95 # Leftmost-rightmost maximum vibrato intensity in note (percent of note duration)
vib_bell_exp = 2 # Exponent to create non-linear bell shape
vib_bell_freq = 100 # Frequency to apply vibrato bell when all conditions met
vib_bell_dur = 600-1200 # Minimum note duration (ms) that can have a vibrato bell - that can have highest vibrato bell
vib_bell = 50-126 # Maximum vibrato intensity in vibrato bell (for minimum and highest duration)
rnd_vib = 10 # Randomize vibrato intensity not greater than this percent
rnd_vib_slow = 2 # Slow down random vibrato function by this value. Can be only integer: 1 and above
