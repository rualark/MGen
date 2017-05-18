# Choose one of algorithms:
# rswa - Random Sliding Windows Approximation (random_seed and random_range are activated automatically). Shuffle not needed
# scan - full (or window) scan all possible combinations (can randomize starting vector with random_seed and random_range)

algorithm = rswa

# Main parameters
c_len = 151 # Number of measures in each cantus. Usually 9 to 11 for cantus
interval = 7-12 # Minimum-maximum chromatic interval in cantus (12 = octave)
key = C # Music key to use for generation
first_note = C5 # Starting note of each cantus
last_note = C5 # Ending note of each cantus

t_cnt = 1000000 # Maximum number of canti generated
s_len = 1 # Maximum number of measures to full scan. 6-7 is recommended. Lower values can create less melodic results. Higher values are slow to compute
random_key = 0 # Allow generator to select random key
tempo = 100-130 # Tempo range
accept_reseed = 1 # After accepting first result reseed (if random_seed) and choose new range (if random_range)
random_seed = 1 # Seed melody with random numbers. This ensures giving different results if generation is very slow.
random_range = 1 # Limit scanning to one of possible fast-scan ranges (automatically enabled for RSWA)
shuffle = 0 # If you want to shuffle all canti after generation (can shuffle up to 32000 canti)

# Random SWA
fullscan_max = 1001 # Maximum steps length to full scan. If melody is longer, use SWA
approximations = 500 # Maximum number of approximations to run if rpenalty decreases
swa_steps = 3 # Size of Sliding Window Approximation algorithm window in steps
correct_range = 7 # Maximum interval allowed between each source and corrected note

# Rule parameters
fill_steps_mul = 2 # Multiply number of notes between leap notes to get steps for filling (+2)
max_smooth_direct = 5 # Maximum linear movement in one direction allowed (in steps)
max_smooth = 7 # Maximum linear movement allowed (in steps)
max_leaps = 2 # Maximum allowed max_leaps during max_leap_steps
max_leaps2 = 3 # Maximum allowed max_leaps2 during max_leap_steps for Many leaps+
cse_leaps = 2 # Maximum allowed consecutive leaps for Consecutive leaps
cse_leaps2 = 3 # Maximum allowed consecutive leaps for Consecutive leaps+
max_leap_steps = 7
stag_notes = 2 # Maximum allowed stag_notes (same notes) during stag_note_steps
stag_note_steps = 7
repeat_steps = 8 # Prohibit repeating of 3 notes closer than repeat_steps between first notes (if beats are same)
repeat_steps2 = 11 # Prohibit repeating of 5 notes closer than repeat_steps2 between first notes
first_steps_tonic = 3 # Number of first steps, which must contain tonic note (will increase by one if cantus is longer than 10 notes)

# Technical parameters
midifile_out_mul = 8 # All notes are made whole for midi export
show_severity = 1 # =1 to show severity in square brackets in comments to notes (also when exporting to MIDI file)
calculate_correlation = 0 # Enables correlation calculation algorithm. Slows down generation. Outputs to cf1-cor.csv
calculate_blocking = 0 # Enables blocking flags calculation algorithm. Slows down generation.
calculate_stat = 1 # Enables flag statistics calculation algorithm. Slows down generation.
calculate_ssf = 1 # Enables SWA stuck flags statistics calculation algorithm.
late_require = 0 # Allow not-last scan window to have no needed tags, but no blocked tags. This calculation usually requires much more time
best_rejected = 5000 # Show best rejected results if rejecting more than X ms. Requires blocking or stat to work. Set to 0 to disable. Slows down generation
