# Main
Instrument = Violin
Type = 1 # Instrument type

# Instrument parameters
n_min = G3 # Lowest note
n_max = F7 # Highest note
t_min = 160 # Shortest note in ms
t_max = 0 # Longest melody withot pauses in ms (0 = no limit). Decreases with dynamics
poly = 1 # Maximum number of simultaneous voices
leap_t_min = 1 # Shortest note after leap

# Automation parameters
library = Friedlander Violin 1.5 # For which library algorithm is optimized
CC_dynamics = 11
CC_dyn_ma = 1 # If we should use moving average to smooth CC dynamics curve before sending
CC_retrigger = 64 # This CC will be set before note retriger and reset soon after
retrigger_freq = 50 # Percent to use CC retrigger instead of non-legato when repeating notes
retrigger_min_len = 600 # Minimum next note length in ms to use retrigger
retrigger_rand_max = 300 # Maximum length in ms to move note end to the left in case of nonlegato retrigger
retrigger_rand_end = 30 # Maximum percent of note length to move note end to the left in case of nonlegato retrigger
max_slur_count = 2 # Use slur for 2nd moves, but no longer than X moves
max_slur_interval = 2 # in semitones
slur_ks = 44 # Keyswitch to send to create slur
legato_ahead = 165 # Time in ms to stretch legato notes back to cope with legato delay
max_ahead_note = 12 # Maximum chromatic interval having ahead property
nonlegato_freq = 20 # Frequency (in percent) when legato can be replaced with non-legato by moving note end to the left
nonlegato_minlen = 400 # Minimum note length (in ms) allowed to convert to nonlegato
CC_steps = 9 # Number of CC steps in one note step (please use only odd numbers)
vel_harsh = 118 # Velocity equal or above this value triggers harsh sustain
vel_immediate = 84 # Velocity equal or above this value triggers immediate sustain
vel_normal = 50 # Velocity equal or above this value triggers normal sustain
vel_gliss = 18 # Velocity below this value triggers glissando transition
vel_normal_minlen = 600 # Minimum note length that can have a normal or lower sustain
gliss_minlen = 1000 # Minimum note length that can have a gliss transition
gliss_freq = 50 # Frequency of gliss articulation in percent
bell_start_mul = 0.2 # Multiply dynamics by this parameter at bell start
bell_end_mul = 0.2 # Multiply dynamics by this parameter at bell end
bell_start_len = 30 # Percent of notelength to use for slope at bell start
bell_end_len = 30 # Percent of notelength to use for slope at bell end


CC_vib_intens = 1
CC_vib_speed = 14
dying = 0 # Seconds to fade out at mf
