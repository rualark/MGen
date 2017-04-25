# Main
library = Samplemodeling Trumpet v3 # For which library algorithm is optimized
Type = 2 # Instrument type

# Instrument parameters
n_min = E3 # Lowest note
n_max = G6 # Highest note
t_min = 50 # Shortest note in ms
t_max = 12000 # Longest melody without pauses in ms (0 = no limit). Decreases with dynamics
poly = 1 # Maximum number of simultaneous voices
leap_t_min = 100 # Shortest note after leap in ms

# Automation parameters
CC_dynamics = 11
CC_dyn_ma = 1 # If we should use moving average to smooth CC dynamics curve before sending
CC_steps = 7 # Number of CC steps in one note step (please use only odd numbers)

# Legato adaptor
legato_ahead = 700 # Time in ms to stretch legato notes back to cope with legato delay
max_ahead_note = 16 # Maximum chromatic interval having ahead property
gliss_freq = 40 # Frequency of gliss articulation in percent

# Note ending adaptor
end_sfl_dur = 140 # Duration of short fall ending
end_sfl_freq = 10 # Frequency of short fall ending
end_pbd_dur = 200 # Duration of pitch bend down ending
end_pbd_freq = 30 # Frequency of pitch bend down ending
end_vib2_dur = 240 # Duration of vibrato ending2
end_vib2_freq = 30 # Frequency of vibrato ending2
end_vib_dur = 445 # Duration of vibrato ending
end_vib_freq = 30 # Frequency of vibrato ending

# Nonlegato adaptor
nonlegato_freq = 13 # Frequency (in percent) when legato can be replaced with non-legato by moving note end to the left
nonlegato_minlen = 200 # Minimum note length (in ms) allowed to convert to nonlegato

# Bell adaptor
bell_mindur = 300 # Minimum note duration (ms) that can have a bell
bell_start_mul = 0.2 # Multiply dynamics by this parameter at bell start
bell_end_mul = 0.2 # Multiply dynamics by this parameter at bell end
bell_start_len = 30 # Percent of notelength to use for slope at bell start
bell_end_len = 30 # Percent of notelength to use for slope at bell end

# Reverse bell adaptor
rbell_freq = 80 # Frequency to apply reverse bell when all conditions met
rbell_dur = 300-1000 # Minimum note duration (ms) that can have a reverse bell - that will have deepest reverse bell
rbell_mul = 0.8-0.2 # Multiply dynamics by this parameter at bell center with mindur - with longer dur

# Vibrato adaptor
CC_vib = 1 # CC number for vibrato intensity
CC_vibf = 14 # CC number for vibrato speed
vib_bell_top = 10-90 # Leftmost-rightmost maximum vibrato intensity in note (percent of note duration)
vibf_bell_top = 10-90 # Leftmost-rightmost maximum vibrato speed in note (percent of note duration)
vib_bell_freq = 80 # Frequency to apply vibrato bell when all conditions met
vib_bell_mindur = 600 # Minimum note duration (ms) that can have a vibrato bell
vib_bell = 40 # Maximum vibrato intensity in vibrato bell
vibf_bell = 30 # Max vibrato frequency in vibrato bell
rnd_vib = 10 # Randomize vibrato intensity not greater than this percent
rnd_vibf = 10 # Randomize vibrato speed not greater than this percent

# Randomization
rnd_vel = 8 # Randomize note velocity not greater than this percent
rnd_dyn = 8 # Randomize step dynamics not greater than this percent
