include "..\include\Embertone IS.pl"

# Main
library = "Blakus Cello 1.0" # For which library algorithm is optimized

# Instrument parameters
n_min = C2 # Lowest note
n_max = C6 # Highest note
t_min = 10 # Shortest note in ms
t_max = 0 # Longest melody withot pauses in ms (0 = no limit). Decreases with dynamics
#leap_t_min = 100 # Shortest note after leap in ms

# Controls
CC_Name = 59: "Transition speed"
#CC_Name = 66: "Bow position"
CC_Name = 67: "Slur"
CC_Name = 80: "Portamento speed" # Used only when enabled Portamento CC mode on.
#CC_Name = 87: "Rebow on"

# These controls should be mapped manually
#CC_Name = 20: "Retrigger on" # All except Cello
#CC_Name = 16: "Portamento CC mode on" # All except Cello

#KswGroup = "F2: Color on", "F#2: Color off"
KswGroup = "C#1: Slur while held"
KswGroup = "B1: Ghost note"
KswGroup = "F1: Normal", "F#1: Con sordino", "G1: Sul ponticello", "G#1: Sul ponticello con sordino"
KswGroup = "A1: Legato", "A#1: Polyphonic"
KswGroup = "C1: Sustain", "D1: Staccato", "D#1: Pizzicato", "E1: Tremolo" # Style

# Seems that vibrato styles cannot be selected with CC or keyswitch (bug?)
CC_Name = 2:  "Vibrato style"
KswGroup = "G0: Vibrato style - Default"
KswGroup = "G#0: Vibrato style - Progressive"
KswGroup = "A0: Vibrato style - Open string"
KswGroup = "A#0: Vibrato style - Gentle"
KswGroup = "B0: Vibrato style - Passionate"

# Techniques mapping to commands
Technique = "con sord; Con sordino"
Technique = "ord; Normal + Sustain"
Technique = "nat; Normal + Sustain"
Technique = "senza sord; Normal"
Technique = "sul pont; Sul ponticello"
#Technique = "sul tast; Sul tasto"
Technique = "stac; Staccato" # TODO: not implemented yet
Technique = "spic; Staccato" # TODO: not implemented yet
Technique = "martele; Staccato" # TODO: not implemented yet
Technique = "saltando; Staccato" # TODO: not implemented yet
Technique = "sautille; Staccato" # TODO: not implemented yet
Technique = "arco; Sustain" # TODO: not implemented yet
Technique = "pizz; Pizzicato" # TODO: not implemented yet
Technique = "trem; Tremolo" # TODO: not implemented yet

# Initial setup
Transition speed lower = 16
Transition speed upper = 16 # Default 49, but automatic legato speed is not supported yet because it decreases audio quality
Transition speed = 76
Responsiveness = 0 # default 0
Dynamic KSW threshold = 100
Shorts length control on = 101
Release samples on = 101
Portamento velocity threshold = 8
Vibrato style - Progressive = 101
Legato = 101
InitTechnique = "ord"

Solo intonation = 2
all_ahead = 28 # Time in ms to stretch sutain notes (not legato) back to cope with slow attack

# Sul tasto adaptor
tasto_import = 1 # Set to 0 to ignore sul tasto in source MIDI file and play as normal notes
bow_lock = 0 # 0 - do not lock bow, 1 - always sul tasto, 2 - always sul ponticello

