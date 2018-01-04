include "include/CA2.pl"

# Analysis
Midi_file = "midi\GenCA2\test-vocal-play.mid" # Path and file name of imported midi file

corrections = 0 # Number of corrections to show. Set to 0 to disable corrections. Does not work for swa.

Instruments = Violin,Horn

Instrument = Tenor
InitCommand = "Ah f"

Instrument = Alto
InitCommand = "Ah"

Instrument = Violin
InitTechnique = "tutti"

Instrument = "Violin/Friedlander/1"
InitTechnique = trem
