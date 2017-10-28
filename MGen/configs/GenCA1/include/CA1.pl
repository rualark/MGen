include "../../GenCF1/include/CF1.pl"

rule_set = 1 # Specify which set of rules to use (see rules.xlsx)

# Method parameters
method = window-scan # This method can be faster in some cases
#method = swa # This method is best for analysis and corrections in most cases

accept_reseed = 0 # After accepting first result reseed (if random_seed) and choose new range (if random_range)
random_seed = 0 # Seed melody with random numbers. This ensures giving different results if generation is very slow.
random_range = 0 # Limit scanning to one of possible fast-scan ranges (automatically enabled for RSWA)
s_len = 1 # Maximum number of measures to full scan. Higher values are slow to compute
correct_inrange = 10 # Limit allowed range of corrected melody to range of source melody + specified number of chromatic steps. Set to high value to disable.
optimize_dpenalty = 1 # Saves only melodies closest to source melody. Decreases memory usage. Setting this flag may make corrections farther from source melody. Resetting allows for more close results when corrections>1
max_correct_ms = 1000000 # Maximum time in milliseconds to correct using window-scan (set to 0 to scan up to the end)
transpose_back = 0 # Should be set to 1 for CF1 generation algorithm only

# Analysis
random_key = 0 # Select any of possible keys regardless of last note
show_transpose = 0,12 # Semitones to transpose each voice (separated with comma)

# Correction
corrections = 1 # Number of corrections to show. Set to 0 to disable corrections. Does not work for swa.
pre_bad = 60 # How many notes to recalculate before rule violation
post_bad = 120 # How many notes to recalculate after rule violation
step_penalty = 3 # Penalty for adding one more changing step while correcting cantus
pitch_penalty = 1 # Penalty for changing note one more diatonic step while correcting cantus
approximations = 500 # Maximum number of approximations to run if rpenalty decreases
swa_steps = 100 # Size of Sliding Window Approximation algorithm window in steps
correct_range = 12 # Maximum interval allowed between each source and corrected note (chromatic)
animate = 0 # Draw animation of preliminary result every X ms (0 to show each change, high to disable animation)
animate_delay = 0 # Delay in ms after each animation step

# Technical parameters
midifile_in_mul = 0.5 # Multiply note length with this value when loading
t_cnt = 100000 # Maximum number of steps generated
midifile_export_marks = 1 # Set to export marks (harmony) to midifile lyrics
midifile_export_comments = 0 # Set to export comments (violations) to midifile lyrics
emulate_sas = 0 # 0 = disable emulator, 1 = Enables SAS algorithm emulator
cor_ack = 1 # Acknowledge correction results by running both algorithms: SAS and ASWA
show_correct_hatch = 0 # Set to 1 to mark corrected notes with hatch
