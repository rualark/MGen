# Main parameters
t_cnt = 1000000 # Maximum number of canti generated
c_len = 9 # Number of measures in each cantus. Usually 9 to 11
s_len = 7 # Maximum number of measures to full scan. 6-7 is recommended. Lower values can create less melodic results. Higher values are slow to compute
interval = 4-7 # Minimum-maximum diatonic interval in cantus (7 = octave)
key = C # Music key to use for generation
random_key = 0 # Allow generator to select random key
first_note = C5 # Starting note of each cantus
last_note = C5 # Ending note of each cantus
tempo = 100-130 # Tempo range
random_seed = 0 # Seed melody with random numbers. This ensures giving different results if generation is very slow.
shuffle = 0 # If you want to shuffle all canti after generation (can shuffle up to 32000 canti)

# Random SWA
fullscan_max = 11 # Maximum steps length to full scan. If melody is longer, use SWA
approximations = 50 # Maximum number of approximations to run if rpenalty decreases
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
calculate_stat = 0 # Enables flag statistics calculation algorithm. Slows down generation.
late_require = 0 # Allow not-last scan window to have no needed tags, but no blocked tags. This calculation usually requires much more time
