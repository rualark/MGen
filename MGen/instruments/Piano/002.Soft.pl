# Main
Library = Native Instruments Gentleman # For which library algorithm is optimized
Type = 0 # Instrument type

# These controls should be mapped manually
CC_Name = 2: "Color"
CC_Name = 3: "Dynamic range"
CC_Name = 4: "Resonances volume"
CC_Name = 5: "Low keys volume"
CC_Name = 6: "Overtones level"
CC_Name = 8: "Stereo image width"

# Initial setup
InitCommand = "Color: 45"
InitCommand = "Dynamic range: 0"
InitCommand = "Resonances volume: 80"
InitCommand = "Low keys volume: 63"
InitCommand = "Low keys volume: 63"
InitCommand = "Overtones level: 90"
InitCommand = "Stereo image width: 63"

# Instrument parameters
n_min = "A0" # Lowest note
n_max = "C8" # Highest note
t_min = 10 # Shortest note in ms
t_max = 0 # Longest melody withot pauses in ms (0 = no limit). Decreases with dynamics
poly = 100 # Maximum number of simultaneous voices
dyn_range = 0-100 # Dynamic range
#leap_t_min = 1 # Shortest note after leap
#dying = 5 # Seconds to fade out at mf

# Lengroup adaptor
lengroup2 = 30 # Percent of grouping notes into groups of 2
lengroup3 = 30 # Percent of grouping notes into groups of 3
lengroup4 = 30 # Percent of grouping notes into groups of 4
lengroup_edt1 = 70 # delta to alter note ending time of all notes in group except for the last (ms)
lengroup_edt2 = -100 # delta to alter note ending time of last note in group (ms)

# Randomization
rand_pos = 8-8 # Randomize note starts-ends not greater than percent of note length 
rand_pos_max = 70-70 # Maximum shift in ms (start-end)
rnd_vel = 8 # Randomize note velocity not greater than this percent

# Controls
Volume_default = 80 # default 127 (direct CC volume, should not be changed by user)

# Initial  setup
Pan = 50 # default 50 (maximum left, 50 - default position, 100 - maximum right)
Volume = 100 # default 100 (relative volume from 0 to 100)

# Pedal adaptor
Pedal_import = 1 # 0 - do not import pedal; 1 - import CC64 pedal from source midi file
