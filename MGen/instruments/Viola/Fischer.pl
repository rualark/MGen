include "..\include\Embertone IS.pl"

# Main
library = "Fischer Viola 1.0" # For which library algorithm is optimized

# Instrument parameters
n_min = C3 # Lowest note
n_max = G6 # Highest note
t_min = 100 # Shortest note in ms
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
KswGroup = "F2: Con sordino on", "F#2: Con sordino off"
KswGroup = "G2: Normal", "G#2: Sul ponticello", "A2: Sul tasto" # Bow position
KswGroup = "A#2: Legato", "B2: Poly"

# Seems that vibrato styles cannot be selected with CC or keyswitch (bug?)
CC_Name = 2:  "Vibrato style"
KswGroup = "D#1: Vibrato style - Default"
KswGroup = "E1: Vibrato style - Progressive"
KswGroup = "F1: Vibrato style - Open string"
KswGroup = "F#1: Vibrato style - Gentle"
KswGroup = "G1: Vibrato style - Passionate"

# Techniques mapping to commands
Technique = "con sord; Con sordino on"
Technique = "ord; Con sordino off + Normal + Sustain"
Technique = "nat; Con sordino off + Normal + Sustain"
Technique = "senza sord; Con sordino off"
Technique = "sul pont; Sul ponticello"
Technique = "sul tast; Sul tasto"
Technique = "stac; Staccato" # TODO: not implemented yet
Technique = "spic; Staccato" # TODO: not implemented yet
Technique = "martele; Staccato" # TODO: not implemented yet
Technique = "saltando; Staccato" # TODO: not implemented yet
Technique = "sautille; Staccato" # TODO: not implemented yet
Technique = "pizz; Pizzicato" # TODO: not implemented yet
Technique = "trem; Tremolo"

# Initial setup
InitCommand = "Ensemble intonation: 8"
InitCommand = "Legato"
