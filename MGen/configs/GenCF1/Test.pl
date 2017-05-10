include "include/default.pl"
include "include/hmean-low.pl"
include "include/rules-low.pl"
#include "include/hmean-high.pl"

c_len = 37 # Number of measures in each cantus. Usually 9 to 11
s_len = 7 # Maximum number of measures to full scan. 6-7 is recommended. Lower values can create less melodic results. Higher values are slow to compute
interval = 4-7 # Minimum-maximum diatonic interval in cantus (7 = octave)
key = C # Music key to use for generation
first_note = C5 # Starting note of each cantus
last_note = C5 # Ending note of each cantus
random_seed = 1 # Seed melody with random numbers. This ensures giving different results if generation is very slow.
shuffle = 0 # If you want to shuffle all canti after generation (can shuffle up to 32000 canti)

# Random SWA
fullscan_max = 5 # Maximum steps length to full scan. If melody is longer, use SWA
approximations = 500 # Maximum number of approximations to run if rpenalty decreases
swa_steps = 3 # Size of Sliding Window Approximation algorithm window in steps
correct_range = 7 # Maximum interval allowed between each source and corrected note

# Technical parameters
calculate_correlation = 0 # Enables correlation calculation algorithm. Slows down generation. Outputs to cf1-cor.csv
calculate_blocking = 0 # Enables blocking flags calculation algorithm. Slows down generation.
calculate_stat = 1 # Enables flag statistics calculation algorithm. Slows down generation.
best_rejected = 2000 # Show best rejected results if rejecting more than X ms. Set to 0 to disable. Slows down generation. Requires calculate_stat