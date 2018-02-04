include "All sounds.pl"

ReplacePitch = "C4" # Replace all notes in track with this pitch
n_min = 0 # Lowest note
n_max = 127 # Highest note

# Tremolo adaptor
trem_maxlen = 100 # Maximum length of one note inside tremolo for detection
trem_min_repeats = 3 # Minimum number of note repeats to detect tremolo
trem_dynamics = 100 # Percent of tremolo dynamics compared to sustain notes
trem_replace = "G4" # Replace tremolo notes with this note
