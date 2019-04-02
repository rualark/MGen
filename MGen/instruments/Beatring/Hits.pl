# Main
library = "True Strike Cinematic Orchestral Percussion" # For which library algorithm is optimized
Type = 0 # Instrument type
poly = 100 # Maximum number of simultaneous voices
single_stage = 1 # Collapse all tracks for this instrument into a single stage
Reverb_fixed = 1 # 0 - reverb can be changed; 1 - reverb cannot be changed (hard coded)

# Instrument parameters
ReplacePitch = "C5" # Replace all notes in track with this pitch
Volume_default = 80 # (direct CC volume, should not be changed by user)
Volume = 85
db_max = 0 # Maximum controlled loudness in db when volume CC is 127
db_coef = 3 # Slope of CC to db function (3 for Kontakt, 1 for SWAM)

# Tremolo adaptor
trem_maxlen = 100 # Maximum length of one note inside tremolo for detection
trem_min_repeats = 3 # Minimum number of note repeats to detect tremolo
trem_dyn_range = 0-100 # (0-100) Percent of tremolo dynamics compared to sustain notes
trem_replace = "E5" # Replace tremolo notes with this note
trem_maxint = 127 # Maximum interval between notes of a trill in semitones to be considered a tremolo
trem_end = 1 # 0 - normal ending, 1 - cutoff (need hit), 2 - ending hit
trem_len = 2000 # Tremolo maximum sample length in ms
