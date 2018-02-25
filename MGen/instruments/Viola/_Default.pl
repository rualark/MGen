include "..\include\Embertone IS.pl"

# Main
library = "Fischer Viola 1.0" # For which library algorithm is optimized

# Instrument parameters
n_min = C3 # Lowest note
n_max = G6 # Highest note
t_min = 10 # Shortest note in ms
t_max = 0 # Longest melody withot pauses in ms (0 = no limit). Decreases with dynamics
#leap_t_min = 100 # Shortest note after leap in ms

# Controls
#CC_Name = 59: "Transition speed"
CC_Name = 66: "Bow position"
CC_Name = 64: "Slur"
CC_Name = 12: "Portamento speed" # Used only when enabled Portamento CC mode on
CC_Name = 5: "Portamento mode" # Used only when enabled Portamento CC mode on
CC_Name = 80: "Portamento speed2" # Used only when enabled Portamento CC mode on. Not mapped: mapping and then playing this CC results in crash.

# These controls should be mapped manually
CC_Name = 20: "Retrigger on" # All except Cello
CC_Name = 16: "Portamento CC mode on" # All except Cello
CC_Name = 21: "Speed control on"

KswGroup = "C#2: Slur while held"
KswGroup = "G#1: Ghost note"
KswGroup = "A1: Color on", "A#1: Color off"
KswGroup = "B1: Portamento"
KswGroup = "C2: Sustain", "D2: Staccato", "D#2: Pizzicato", "E2: Tremolo" # Style
KswGroup = "F2: Con sordino", "F#2: Senza sordino"
KswGroup = "G2: Normal", "G#2: Sul ponticello", "A2: Sul tasto" # Bow position
KswGroup = "A#2: Legato", "B2: Polyphonic"

# Seems that vibrato styles cannot be selected with CC or keyswitch (bug?)
CC_Name = 2:  "Vibrato style"
KswGroup = "D#1: Vibrato style - Default"
KswGroup = "E1: Vibrato style - Progressive"
KswGroup = "F1: Vibrato style - Open string"
KswGroup = "F#1: Vibrato style - Gentle"
KswGroup = "G1: Vibrato style - Passionate"

# Techniques mapping to commands
Technique = "con sord; Con sordino"
Technique = "ord; Senza sordino + Normal + Sustain"
Technique = "nat; Senza sordino + Normal + Sustain"
Technique = "senza sord; Senza sordino"
Technique = "sul pont; Sul ponticello"
Technique = "sul tast; Sul tasto"
Technique = "stac; Staccato" # TODO: not implemented yet
Technique = "spic; Staccato" # TODO: not implemented yet
Technique = "martele; Staccato" # TODO: not implemented yet
Technique = "saltando; Staccato" # TODO: not implemented yet
Technique = "sautille; Staccato" # TODO: not implemented yet
Technique = "arco; Sustain" # TODO: not implemented yet
Technique = "pizz; Pizzicato" # TODO: not implemented yet
Technique = "trem; Tremolo"

# Initial setup
InitCommand = "Ensemble intonation: 20" # default 8
InitCommand = "Ensemble L: 40" # default 0
InitCommand = "Ensemble R: 87" # default 127
InitCommand = "Ensemble tightness: 22" # default 22, alternative 65
InitCommand = "Ensemble randomize: 36" # default 36, alternative 100
InitCommand = "Ensemble combine transitions"
InitCommand = "Transition speed lower: 16"
InitCommand = "Transition speed upper: 16" # Default 54, but automatic legato speed is not supported yet because it decreases audio quality
InitCommand = "Solo intonation: 0"
InitCommand = "Retrigger on"
InitCommand = "Responsiveness: 44"
InitCommand = "Dynamic KSW threshold: 100"
InitCommand = "Shorts length control on: 0" # default 0
InitCommand = "Release samples on" # default 0
InitCommand = "Portamento CC mode on: 0"
InitCommand = "Portamento velocity threshold: 30"
InitCommand = "Vibrato style - Progressive"
InitCommand = "Legato"
InitCommand = "Color off"
InitTechnique = "ord"

all_ahead = 54 # Time in ms to stretch sutain notes (not legato) back to cope with slow attack

# Sul tasto adaptor
tasto_import = 1 # Set to 0 to ignore sul tasto in source MIDI file and play as normal notes
bow_lock = 0 # 0 - do not lock bow, 1 - always sul tasto, 2 - always sul ponticello

