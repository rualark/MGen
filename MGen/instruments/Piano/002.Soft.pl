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
CC_Name = 12: "Space on"
CC_Name = 13: "EQ on"
CC_Name = 14: "Compressor on"
CC_Name = 15: "Tonal depth on"
CC_Name = 16: "Transients on"
CC_Name = 17: "EQ Bass"
CC_Name = 18: "EQ Body"
CC_Name = 19: "EQ Air"
CC_Name = 20: "Tonal depth amount"
CC_Name = 23: "TRANS Attack_Sustain"
CC_Name = 24: "OVERTONES_on"
CC_Name = 25: "STEREO_IMAGE Swap_on"
CC_Name = 26: "PEDAL Repedaling_on"
CC_Name = 27: "PEDAL Halfpedal_on"
CC_Name = 28: "RELEASE SAMPLES_on"
CC_Name = 29: "RELEASE SAMPLES_Vol"
CC_Name = 30: "NOISES Hammer_on"
CC_Name = 31: "NOISES Hammer_Vol"
CC_Name = 32: "NOISES Damper_on"
CC_Name = 33: "NOISES Damper_Vol"
CC_Name = 34: "NOISES Pedal_on"
CC_Name = 35: "NOISES Pedal_Vol"
CC_Name = 36: "NOISES String_on"
CC_Name = 37: "NOISES String_Vol"
CC_Name = 38: "TUNING Basic_Pitch"
CC_Name = 39: "KEYS Silent_key_on"

# Initial setup
Color = 45
Dynamic range = 64
Resonances volume = 80
Low keys volume = 63
Overtones level = 90
Stereo image width = 63
Space on = 0
EQ on = 100
Compressor on = 0
Tonal depth on = 100
Transients on = 0
EQ Bass = 64
EQ Body = 64
EQ Air = 64
Tonal depth amount = 89
#Add NSaranov
TRANS Attack_Sustain = 78
OVERTONES_on = 100
STEREO_IMAGE Swap_on = 100
PEDAL Repedaling_on = 0
PEDAL Halfpedal_on = 0
RELEASE SAMPLES_on = 100
RELEASE SAMPLES_Vol = 93
NOISES Hammer_on = 100
NOISES Hammer_Vol = 86
NOISES Damper_on = 100
NOISES Damper_Vol = 86
NOISES Pedal_on = 100
NOISES Pedal_Vol = 86
NOISES String_on = 100
NOISES String_Vol = 86
TUNING Basic_Pitch = 64
KEYS Silent_key_on = 0

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
db_max = 12 # Maximum controlled loudness in db when volume CC is 127
db_coef = 3 # Slope of CC to db function (3 for Kontakt, 1 for SWAM)

# Initial  setup
Pan = 50 # default 50 (maximum left, 50 - default position, 100 - maximum right)
Volume = 100 # default 100 (relative volume from 0 to 100)

# Pedal adaptor
Pedal_import = 1 # 0 - do not import pedal; 1 - import CC64 pedal from source midi file
