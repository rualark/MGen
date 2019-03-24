include "..\include\Embertone IS.pl"

# Main
library = "Friedlander Violin 1.5" # For which library algorithm is optimized
Pan_default = 35

# Instrument parameters
n_min = G3 # Lowest note
n_max = F7 # Highest note
t_min = 12 # Shortest note in ms
t_max = 0 # Longest melody withot pauses in ms (0 = no limit). Decreases with dynamics
#leap_t_min = 100 # Shortest note after leap in ms

# Controls
CC_Name = 59: "Transition speed"
CC_Name = 66: "Bow position"
CC_Name = 67: "Slur"
CC_Name = 80: "Portamento speed" # Used only when enabled Portamento CC mode on. Not working
CC_Name = 87: "Rebow on"

# These controls should be mapped manually
CC_Name = 20: "Retrigger on" # All except Cello
CC_Name = 16: "Portamento CC mode on" # All except Cello

KswGroup = "G#2: Slur while held"
KswGroup = "E2: Ghost note"
KswGroup = "F2: Color on", "F#2: Color off"
KswGroup = "C3: Con sordino", "C#3: Senza sordino"
KswGroup = "D3: Normal", "D#3: Sul ponticello", "E3: Sul tasto" # Bow position
KswGroup = "F3: Legato", "F#3: Polyphonic"
KswGroup = "G2: Sustain", "A2: Staccato", "A#2: Pizzicato", "B2: Tremolo" # Style

# Seems that vibrato styles cannot be selected with CC or keyswitch (bug?)
CC_Name = 2:  "Vibrato style"
KswGroup = "B0: Vibrato style - Default"
KswGroup = "C1: Vibrato style - Progressive"
KswGroup = "C#1: Vibrato style - Open string"
KswGroup = "D1: Vibrato style - Gentle"
KswGroup = "D#1: Vibrato style - Passionate"

# Techniques mapping to commands
Technique = "con sord; Con sordino"
Technique = "ord; Senza sordino + Normal + Sustain"
Technique = "nat; Senza sordino + Normal + Sustain"
Technique = "senza sord; Senza sordino"
Technique = "sul pont; Sul ponticello"
Technique = "sul tast; Sul tasto"
Technique = "stac; Staccato"
Technique = "spic; Staccato"
Technique = "martele; Staccato"
Technique = "saltando; Staccato"
Technique = "sautille; Staccato"
Technique = "arco; Sustain"
Technique = "pizz; Pizzicato"
Technique = "trem; Tremolo"

# Initial setup
Responsiveness = 0 # default for Violin: 0
Portamento speed = 11 # Use only when enabled Portamento CC mode on
Dynamic KSW threshold = 100
Shorts length control on = 0
Portamento CC mode on = 0
Portamento velocity threshold = 18
Vibrato style - Progressive = 101
Transition speed lower = 16
Transition speed upper = 16 # Default 90, but automatic legato speed is not supported yet because it decreases audio quality
Transition speed = 127
Retrigger on = 101 # default for Violin: 0
Legato = 101

# Initial setup (can change)
Ensemble on = 0
Color off = 90
Solo intonation = 2
Rebow on = 127 # default for Violin: 0
Release samples on = 127
InitTechnique = "ord"

all_ahead = 54 # Time in ms to stretch sutain notes (not legato) back to cope with slow attack

# Sul tasto adaptor
tasto_import = 1 # Set to 0 to ignore sul tasto in source MIDI file and play as normal notes
bow_lock = 0 # 0 - do not lock bow, 1 - always sul tasto, 2 - always sul ponticello
