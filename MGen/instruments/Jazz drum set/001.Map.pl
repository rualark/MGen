include "All sounds.pl"


MapPitch = "C2, C1" # Kick
MapPitch = "D2, D#1, F#1" # Snare
MapPitch = "C#2, C#1" # Snare cross-stick
MapPitch = "F#2, A#1" # Hihat closed
MapPitch = "G#2, C2" # Hihat pedal
MapPitch = "A#2, F#2" # Hihat open
MapPitch = "F3, G#2" # Ride 1 bell
MapPitch = "D#3, A2" # Ride 1
MapPitch = "F2, F#3, G#3" # Lo tom
MapPitch = "G2, F#3, G#3" # Lo tom
MapPitch = "A2, F#3, G#3" # Lo tom
MapPitch = "B2, C#3, D#3" # Hi tom
MapPitch = "C3, C#3, D#3" # Hi tom

# Tremolo adaptor
trem_maxlen = 100 # Maximum length of one note inside tremolo for detection
trem_min_repeats = 3 # Minimum number of note repeats to detect tremolo
trem_dyn_range = 0-100 # (0-100) Percent of tremolo dynamics compared to sustain notes
