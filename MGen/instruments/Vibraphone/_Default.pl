# Main
library = "True Strike Cinematic Orchestral Percussion" # For which library algorithm is optimized
Type = 0 # Instrument type
poly = 100 # Maximum number of simultaneous voices

# Instrument parameters
n_min = "F3" # Lowest note
n_max = "F6" # Highest note
Volume_default = 50 # (direct CC volume, should not be changed by user)

# Controls
KswGroup = "C3: Staccato pedal up sound", "C#3: Longer version of staccato"

# Initial setup
InitCommand = "Staccato pedal up sound" 

Pedal_import = 1 # 0 - do not import pedal; 1 - import CC64 pedal from source midi file
