include "include/CA2.pl"

# Analysis
Midi_file = "music\Counterpoint 2 voices\good\good-cp5.mid" # Path and file name of imported midi file

corrections = 0 # Number of corrections to show. Set to 0 to disable corrections. Does not work for swa.
confirm_mode = 2 # 0 - do not confirm expected flags; 1 - confirm global/local; 2 - confirm mistakes

AutoLegato = 1
AutoNonLegato = 1

#"Horn/Normal/1": Volume = 150

#show_min_severity = 49 # Minimum severity to highlight note
species = 0 # Do not override parameters in importing files

corrections = 1
max_correct_ms = 2000
