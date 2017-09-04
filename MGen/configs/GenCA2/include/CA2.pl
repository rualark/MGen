include "../../GenCP1/include/CP1.pl"

rule_set = 1 # Specify which set of rules to use (see rules.xlsx)

# Method parameters
#method = window-scan # This method can be faster in some cases
method = swa # This method is best for analysis and corrections in most cases

accept_reseed = 0 # After accepting first result reseed (if random_seed) and choose new range (if random_range)
random_seed = 0 # Seed melody with random numbers. This ensures giving different results if generation is very slow.
random_range = 0 # Limit scanning to one of possible fast-scan ranges (automatically enabled for RSWA)
s_len = 1 # Maximum number of measures to full scan. Higher values are slow to compute
correct_inrange = 0 # Limit allowed range of corrected melody to range of source melody + specified number of chromatic steps. Set to high value to disable.
optimize_dpenalty = 0 # Saves only melodies closest to source melody. Decreases memory usage. Setting this flag may make corrections farther from source melody. Resetting allows for more close results when corrections>1

# Analysis
corrections = 0 # Number of corrections to show. Set to 0 to disable corrections. Does not work for swa.
pre_bad = 6 # How many notes to recalculate before rule violation
post_bad = 12 # How many notes to recalculate after rule violation
step_penalty = 3 # Penalty for adding one more changing step while correcting cantus
pitch_penalty = 1 # Penalty for changing note one more diatonic step while correcting cantus
show_transpose = 0,0,36,36 # Semitones to transpose each voice (separated with comma)
approximations = 500 # Maximum number of approximations to run if rpenalty decreases
swa_steps = 3 # Size of Sliding Window Approximation algorithm window in steps
correct_range = 14 # Maximum interval allowed between each source and corrected note (chromatic)
random_key = 0 # Select any of possible keys regardless of last note

# Technical parameters
midifile_in_mul = 1 # Multiply note length with this value when loading
t_cnt = 100000 # Maximum number of steps generated
midifile_export_marks = 0 # Set to export marks (harmony) to midifile lyrics
midifile_export_comments = 1 # Set to export comments (violations) to midifile lyrics
