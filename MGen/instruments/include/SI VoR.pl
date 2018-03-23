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
rnd_dyn = 8 # Randomize step dynamics not greater than this percent
rand_pos = 8-0 # Randomize note starts-ends not greater than percent of note length 
rand_pos_max = 70-0 # Maximum shift in ms (start-end)

# Nonlegato adaptor
nonlegato_mingap = 100 # Minimum gap between notes (in ms) introduced by automatic nonlegato 
