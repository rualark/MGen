include "..\include\Embertone IS.pl"

# Main
library = "Blakus Cello 1.0" # For which library algorithm is optimized

# Instrument parameters
n_min = C2 # Lowest note
n_max = C6 # Highest note
t_min = 100 # Shortest note in ms
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
KswGroup = "A1: Legato", "A#1: Poly"
KswGroup = "C1: Sustain", "D1: Staccato", "D#1: Pizzicato", "E1: Tremolo" # Style

# Seems that vibrato styles cannot be selected with CC or keyswitch (bug?)
CC_Name = 2:  "Vibrato style"
KswGroup = "G0: Vibrato style - Default"
KswGroup = "G#0: Vibrato style - Progressive"
KswGroup = "A0: Vibrato style - Open string"
KswGroup = "A#0: Vibrato style - Gentle"
KswGroup = "B0: Vibrato style - Passionate"
