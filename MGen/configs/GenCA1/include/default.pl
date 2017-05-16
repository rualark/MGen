include "../../GenCF1/include/default.pl"

# Analysis
corrections = 1 # Number of corrections to show. Set to 0 to disable corrections
pre_bad = 6 # How many notes to recalculate before rule violation
post_bad = 8 # How many notes to recalculate after rule violation
step_penalty = 3 # Penalty for adding one more changing step while correcting cantus
pitch_penalty = 1 # Penalty for changing note one more diatonic step while correcting cantus
show_transpose = 0,12 # Semitones to transpose each voice (separated with comma)
fullscan_max = 100 # Maximum steps length to full scan. If melody is longer, use SWA
approximations = 50 # Maximum number of approximations to run if rpenalty decreases
swa_steps = 6 # Size of Sliding Window Approximation algorithm window in steps
correct_range = 6 # Maximum interval allowed between each source and corrected note
random_key = 0 # Select any of possible keys regardless of last note

# Main parameters
s_len = 1 # Maximum number of measures to full scan. Higher values are slow to compute
random_seed = 0 # Seed melody with random numbers. This ensures giving different results if generation is very slow.
random_range = 0 # Limit scanning to one of possible fast-scan ranges (automatically enabled for RSWA)
shuffle = 0 # If you want to shuffle all canti after generation (can shuffle up to 32000 canti)

# Technical parameters
midifile_in_mul = 1 # Multiply note length with this value when loading
