include "../../GenCF1/include/CF1.pl"

rule_set = 1 # Specify which set of rules to use (see rules.xlsx)

# Method parameters
method = window-scan # This method can be faster in some cases
#method = swa # This method is best for analysis and corrections in most cases

accept_reseed = 0 # After accepting first result reseed (if random_seed) and choose new range (if random_range)
random_seed = 0 # Seed melody with random numbers. This ensures giving different results if generation is very slow.
random_range = 0 # Limit scanning to one of possible fast-scan ranges (automatically enabled for RSWA)
s_len = 7 # Maximum number of measures to full scan. Higher values are slow to compute

shuffle = 0 # If you want to shuffle all canti after generation (can shuffle up to 32000 canti)

# Analysis
random_key = 0 # Select any of possible keys regardless of last note

# Technical parameters
midifile_in_mul = 1 # Multiply note length with this value when loading
t_cnt = 1000 # Maximum number of steps generated
midifile_export_marks = 1 # Set to export marks (harmony) to midifile lyrics
midifile_export_comments = 0 # Set to export comments (violations) to midifile lyrics
