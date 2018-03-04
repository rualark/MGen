# Main
library = "True Strike Cinematic Orchestral Percussion" # For which library algorithm is optimized
Type = 0 # Instrument type
poly = 100 # Maximum number of simultaneous voices
n_min = D2

# Instrument parameters
ReplacePitch = "C4" # Replace all notes in track with this pitch
Volume_default = 50 # (direct CC volume, should not be changed by user)

# Controls
KswGroup = "C2: Snares on", "C#2: Snares off"

# Initial setup
InitCommand = "Snares off" 

# Tremolo adaptor
trem_maxlen = 100 # Maximum length of one note inside tremolo for detection
trem_min_repeats = 3 # Minimum number of note repeats to detect tremolo
trem_dyn_range = 0-100 # (0-100) Percent of tremolo dynamics compared to sustain notes
trem_replace = "F4" # Replace tremolo notes with this note
