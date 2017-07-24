include "../../GenCF1/include/CF1.pl"

cf_rule_set = 1 # Specify which set of rules to use for cantus (see rules.xlsm)
cp_rule_set = 2 # Specify which set of rules to use for counterpoint (see rules.xlsm)

# Method parameters
#method = window-scan # This method can be faster in some cases
method = swa # This method is best for analysis and corrections in most cases

accept_reseed = 0 # After accepting first result reseed (if random_seed) and choose new range (if random_range)
random_seed = 1 # Seed melody with random numbers. This ensures giving different results if generation is very slow.
random_range = 0 # Limit scanning to one of possible fast-scan ranges (automatically enabled for RSWA)
s_len = 1 # Maximum number of measures to full scan. Higher values are slow to compute
cantus_id = 34 # Select particular cantus id. Set to 0 to select random
transpose_back = 0 # Should be set to 1 for CF1 generation algorithm only

shuffle = 0 # If you want to shuffle all canti after generation (can shuffle up to 32000 canti)

# Analysis
random_key = 0 # Select any of possible keys regardless of last note

# Main parameters
notes_per_measure = 2 # Number of notes per measure (1 = species1, 2 = species2, 4 = species3, 8 = species5)
start_pause = 1 # Length of starting pause at counterpoint voice start (length of pause is controlled by notes_per_measure)
tonic_window = 15 # Number of steps to search for adjacent tonic chords
rpenalty_accepted = 0 # Maximum accepted rule penalty for RandomSWA

# Technical parameters
midifile_in_mul = 0.5 # Multiply note length with this value when loading
t_cnt = 1000 # Maximum number of steps generated
midifile_export_marks = 1 # Set to export marks (harmony) to midifile lyrics
midifile_export_comments = 0 # Set to export comments (violations) to midifile lyrics
