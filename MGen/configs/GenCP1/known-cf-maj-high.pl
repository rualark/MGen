include "include/CP1.pl"

# Analysis
Midi_file = midi\known-cf-maj.mid # Path and file name of imported midi file

cantus_id = 0 # Select particular cantus id. Set to 0 to select random

#method = window-scan # This method can be faster in some cases
method = swa # This method is best for analysis and corrections in most cases
notes_per_measure = 2 # Number of notes per measure (1 = species1, 2 = species2, 4 = species3, 8 = species5)
start_pause = 1 # Length of starting pause at counterpoint voice start (length of pause is controlled by notes_per_measure)
