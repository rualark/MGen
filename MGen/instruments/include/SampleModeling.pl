# Main
Type = 2 # Instrument type
poly = 1 # Maximum number of simultaneous voices

# Automation parameters
CC_dynamics = 11
CC_dyn_ma = 1 # If we should use moving average to smooth CC dynamics curve before sending
CC_steps = 11 # Number of CC steps in one note step (please use only odd numbers)

# Legato adaptor
legato_ahead = 700 # Time in ms to stretch legato notes back to cope with legato delay
leg_pdur = 50 # Maximum percent of previous note duration, that legato transition can take
leg_cdur = 70 # Maximum percent of current note duration, that legato transition can take
legato_ahead_exp = 6 # Exponent to interpolate note movement ahead from note velocity
max_ahead_note = 16 # Maximum chromatic interval having ahead property
gliss_freq = 40 # Frequency of gliss articulation in percent
splitpo_freq = 40 # Frequency of split portamento in percent
splitpo_mindur = 50 # Minimum legato duration to use split portamento
gliss_mindur = 30 # Minimum legato duration to use gliss
splitpo_pent_minint = 3 # Minimum allowed interval in semitones for split portamento pentatonic

# Nonlegato adaptor
nonlegato_freq = 13 # Frequency (in percent) when legato can be replaced with non-legato by moving note end to the left
nonlegato_minlen = 200 # Minimum note length (in ms) allowed to convert to nonlegato

# Retrigger adaptor
retrigger_min_len = 600 # Minimum next note length in ms to use retrigger
retrigger_rand_max = 300 # Maximum length in ms to move note end to the left in case of nonlegato retrigger
retrigger_rand_end = 50 # Maximum percent of note length to move note end to the left in case of nonlegato retrigger

# Bell adaptor
bell_mindur = 300 # Minimum note duration (ms) that can have a bell
bell_mul = 0.2-0.2 # Multiply dynamics by this parameter at bell start-end
bell_len = 30-30 # Percent of notelength to use for slope at bell start-end

# Reverse bell adaptor
rbell_freq = 80 # Frequency to apply reverse bell when all conditions met
rbell_dur = 300-1000 # Minimum note duration (ms) that can have a reverse bell - that will have deepest reverse bell
rbell_mul = 0.8-0.2 # Multiply dynamics by this parameter at bell center with mindur - with longer dur

# Vibrato adaptor
CC_vib = 1 # CC number for vibrato intensity
CC_vibf = 19 # CC number for vibrato speed
vib_bell_top = 10-90 # Leftmost-rightmost maximum vibrato intensity in note (percent of note duration)
vibf_bell_top = 10-90 # Leftmost-rightmost maximum vibrato speed in note (percent of note duration)
vib_bell_exp = 2 # Exponent to create non-linear bell shape
vibf_bell_exp = 2 # Exponent to create non-linear bell shape
vib_bell_freq = 100 # Frequency to apply vibrato bell when all conditions met
vib_bell_dur = 600-1200 # Minimum note duration (ms) that can have a vibrato bell - that can have highest vibrato bell
vib_bell = 30-90 # Maximum vibrato intensity in vibrato bell (for minimum and highest duration)
vibf_bell = 20-80 # Max vibrato frequency in vibrato bell (for minimum and highest duration)
rnd_vib = 10 # Randomize vibrato intensity not greater than this percent
rnd_vibf = 10 # Randomize vibrato speed not greater than this percent

# Randomization
rnd_vel = 8 # Randomize note velocity not greater than this percent
rnd_vel_repeat = 70 # Randomize note velocity not greater than this percent for note retriggers
rnd_dyn = 8 # Randomize step dynamics not greater than this percent
