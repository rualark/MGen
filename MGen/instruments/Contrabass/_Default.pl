include "..\include\Embertone IS.pl"

# Main
library = "Leonid Bass 1.0" # For which library algorithm is optimized

# Instrument parameters
n_min = C1 # Lowest note
n_max = G5 # Highest note
t_min = 10 # Shortest note in ms
t_max = 0 # Longest melody withot pauses in ms (0 = no limit). Decreases with dynamics
#leap_t_min = 100 # Shortest note after leap in ms

# Controls
#CC_Name = 59: "Transition speed"
CC_Name = 12: "Bow position"
#CC_Name = 64: "Slur"
CC_Name = 5: "Portamento speed" # Used only when enabled Portamento CC mode on
CC_Name = 2: "Portamento breath controller" # Used only when enabled Portamento CC mode on
CC_Name = 80: "Portamento speed2" # Used only when enabled Portamento CC mode on. Not mapped: mapping and then playing this CC results in crash.
CC_Name = 67: "Alternate transition style" # Changes to slur when Bow change slur reverse is 0

# These controls should be mapped manually
CC_Name = 20: "Retrigger on" # All except Cello
CC_Name = 16: "Portamento CC mode on" # All except Cello
#CC_Name = 21: "Speed control on"
CC_Name = 22: "Dynamic range"
CC_Name = 23: "Bow change slur reverse"

KswGroup = "A0: Slur while held" 
KswGroup = "A5: Ghost note"
KswGroup = "A-1: Color on", "A#-1: Color off"
KswGroup = "E0: Portamento"
KswGroup = "G#0: Sustain", "G0: Staccato", "F#0: Pizzicato", "F0: Tremolo" # Style
KswGroup = "D0: Con sordino", "D#0: Senza sordino"
KswGroup = "B-1: Normal", "C0: Sul ponticello", "C#0: Harmonics" # Bow position
KswGroup = "A#0: Legato", "B0: Polyphonic"

# Techniques mapping to commands
Technique = "con sord; Con sordino"
Technique = "ord; Senza sordino + Normal + Sustain"
Technique = "nat; Senza sordino + Normal + Sustain"
Technique = "senza sord; Senza sordino"
Technique = "sul pont; Sul ponticello"
Technique = "harm; Harmonics"
Technique = "stac; Staccato" # TODO: not implemented yet
Technique = "spic; Staccato" # TODO: not implemented yet
Technique = "martele; Staccato" # TODO: not implemented yet
Technique = "saltando; Staccato" # TODO: not implemented yet
Technique = "sautille; Staccato" # TODO: not implemented yet
Technique = "arco; Sustain" # TODO: not implemented yet
Technique = "pizz; Pizzicato" # TODO: not implemented yet
Technique = "trem; Tremolo"

# Initial setup
InitCommand = "Dynamic range: 19" # default 0
InitCommand = "Bow change slur reverse: 0" # default 0
InitCommand = "Transition speed lower: 16"
InitCommand = "Transition speed upper: 16" # Default 71, but automatic legato speed is not supported yet because it decreases audio quality
InitCommand = "Responsiveness: 0"
InitCommand = "Retrigger on" # default 100
InitCommand = "Dynamic KSW threshold: 100"
InitCommand = "Shorts length control on: 0" # default 100
InitCommand = "Release samples on" # default 100
InitCommand = "Portamento CC mode on: 0"
InitCommand = "Portamento velocity threshold: 23"
InitCommand = "Legato"
InitCommand = "Color off"
InitTechnique = "ord"

Solo intonation = 0
all_ahead = 104 # Time in ms to stretch sutain notes (not legato) back to cope with slow attack

# Sul tasto adaptor
tasto_import = 1 # Set to 0 to ignore sul tasto in source MIDI file and play as normal notes
bow_lock = 0 # 0 - do not lock bow, 1 - always sul tasto, 2 - always sul ponticello

