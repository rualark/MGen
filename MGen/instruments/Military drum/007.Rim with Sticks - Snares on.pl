include "_Default.pl"

ReplacePitch = "E4" # Replace all notes in track with this pitch

# Initial setup
Snares on = 101

# Tremolo adaptor
trem_maxlen = 100 # Maximum length of one note inside tremolo for detection
trem_min_repeats = 3 # Minimum number of note repeats to detect tremolo
trem_dyn_range = 0-100 # (0-100) Percent of tremolo dynamics compared to sustain notes
trem_replace = "B3" # Replace tremolo notes with this note
trem_maxint = 127 # Maximum interval between notes of a trill in semitones to be considered a tremolo
trem_end = 2 # 0 - normal ending, 1 - cutoff (need hit), 2 - ending hit
