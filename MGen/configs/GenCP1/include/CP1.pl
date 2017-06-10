include "../../GenCF1/include/CF1.pl"

rule_set = 1 # Specify which set of rules to use (see rules.xlsx)

# Method parameters
method = window-scan # This method can be faster in some cases
#method = swa # This method is best for analysis and corrections in most cases

accept_reseed = 0 # After accepting first result reseed (if random_seed) and choose new range (if random_range)
random_seed = 0 # Seed melody with random numbers. This ensures giving different results if generation is very slow.
random_range = 0 # Limit scanning to one of possible fast-scan ranges (automatically enabled for RSWA)
s_len = 1 # Maximum number of measures to full scan. Higher values are slow to compute
cantus_id = 4 # Select particular cantus id. Set to 0 to select random

shuffle = 0 # If you want to shuffle all canti after generation (can shuffle up to 32000 canti)

# Analysis
random_key = 0 # Select any of possible keys regardless of last note

# Main parameters
interval = 7-14 # Minimum-maximum chromatic interval of counterpoint (12 = octave)
sum_interval = 22 # Maximum total chromatic interval of counterpoint with cantus (12 = octave)
between = 0-7 # Minimum-maximum diatonic interval between voices (7 = octave)
burst_between = 11 # Maximum diatonic interval between voices for burst_steps steps
burst_steps = 3 # Maximum number of steps for which max_between can be exceeded

cantus_high = 1 # Set to 1 to generate counterpoint under cantus
slurs_window = 10 # Number of steps to check for slur limit

# Technical parameters
midifile_in_mul = 0.5 # Multiply note length with this value when loading
t_cnt = 1000 # Maximum number of steps generated
midifile_export_marks = 1 # Set to export marks (harmony) to midifile lyrics
midifile_export_comments = 0 # Set to export comments (violations) to midifile lyrics
