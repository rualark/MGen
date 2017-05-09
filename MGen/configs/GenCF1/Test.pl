include "include/default.pl"
include "include/hmean-low.pl"
include "include/rules-low.pl"
#include "include/hmean-high.pl"

c_len = 13 # Number of measures in each cantus. Usually 9 to 11
s_len = 7 # Maximum number of measures to full scan. 6-7 is recommended. Lower values can create less melodic results. Higher values are slow to compute
interval = 4-7 # Minimum-maximum diatonic interval in cantus (7 = octave)
key = Am # Music key to use for generation
first_note = A5 # Starting note of each cantus
last_note = A5 # Ending note of each cantus
random_seed = 0 # Seed melody with random numbers. This ensures giving different results if generation is very slow.
shuffle = 0 # If you want to shuffle all canti after generation (can shuffle up to 32000 canti)

# Technical parameters
calculate_correlation = 0 # Enables correlation calculation algorithm. Slows down generation. Outputs to cf1-cor.csv
calculate_blocking = 0 # Enables blocking flags calculation algorithm. Slows down generation.
calculate_stat = 0 # Enables flag statistics calculation algorithm. Slows down generation.
