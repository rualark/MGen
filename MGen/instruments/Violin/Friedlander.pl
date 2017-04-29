# Main
library = Friedlander Violin 1.5 # For which library algorithm is optimized
Type = 1 # Instrument type

# Instrument parameters
n_min = G3 # Lowest note
n_max = F7 # Highest note
t_min = 100 # Shortest note in ms
t_max = 0 # Longest melody withot pauses in ms (0 = no limit). Decreases with dynamics
poly = 1 # Maximum number of simultaneous voices
#leap_t_min = 100 # Shortest note after leap in ms

# Automation parameters
CC_dynamics = 11
CC_dyn_ma = 1 # If we should use moving average to smooth CC dynamics curve before sending
CC_steps = 30 # Number of CC steps in one note step (please use only odd numbers)

# Retrigger adaptor
CC_retrigger = 64 # This CC will be set before note retriger and reset soon after
retrigger_freq = 50 # Percent to use CC retrigger instead of non-legato when repeating notes
retrigger_min_len = 600 # Minimum next note length in ms to use retrigger
retrigger_rand_max = 300 # Maximum length in ms to move note end to the left in case of nonlegato retrigger
retrigger_rand_end = 50 # Maximum percent of note length to move note end to the left in case of nonlegato retrigger

# Slur adaptor
max_slur_count = 2 # Use slur for 2nd moves, but no longer than X moves
max_slur_interval = 2 # in semitones
slur_ks = 44 # Keyswitch to send to create slur

# Legato adaptor
legato_ahead = 165 # Time in ms to stretch legato notes back to cope with legato delay
max_ahead_note = 12 # Maximum chromatic interval having ahead property

# Nonlegato adaptor
nonlegato_freq = 20 # Frequency (in percent) when legato can be replaced with non-legato by moving note end to the left
nonlegato_minlen = 400 # Minimum note length (in ms) allowed to convert to nonlegato

# Transition types
vel_harsh = 118 # Velocity equal or above this value triggers harsh sustain
vel_immediate = 84 # Velocity equal or above this value triggers immediate sustain
vel_normal = 50 # Velocity equal or above this value triggers normal sustain
vel_gliss = 18 # Velocity below this value triggers glissando transition
vel_normal_minlen = 600 # Minimum note length (ms) that can have a normal or lower sustain

# Gliss adaptor
gliss_minlen = 1000 # Minimum note length that can have a gliss transition
gliss_freq = 50 # Frequency of gliss articulation in percent

# Bell adaptor
bell_mindur = 300 # Minimum note duration (ms) that can have a bell
bell_mul = 0.2-0.2 # Multiply dynamics by this parameter at bell start-end
bell_len = 30-30 # Percent of notelength to use for slope at bell start-end

# Randomization
rnd_vel = 8 # Randomize note velocity not greater than this percent
rnd_dyn = 8 # Randomize step dynamics not greater than this percent
