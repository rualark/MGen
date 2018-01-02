# Soundiron Voices of Rapture

#Main
Library = "Soundiron Voices of Rapture 1.0 - 2017-03-23" # For which library algorithm is optimized
Type = 3 # Instrument type
poly = 1 # Maximum number of simultaneous voices

# Automation parameters
CC_dynamics = 72
CC_ma = 9 # Number of CC steps to moving average (please use only odd numbers)
CC_steps = 30 # Number of CC steps in one note step (please use only odd numbers)

# Controls
CC_Name = 2: "Legato transition speed"
CC_Name = 3: "Legato on"
CC_Name = 4: "Chord auto-pan on"
CC_Name = 5: "Release samples on"
CC_Name = 72: "Dynamics"
CC_Name = 74: "Attack"
CC_Name = 76: "Release time"
CC_Name = 78: "Offset"
CC_Name = 80: "Vibrato intensity"
CC_Name = 90: "Release volume"

# Initial setup
InitInstrument = "Attack: 0"
InitInstrument = "Offset: 0"
InitInstrument = "Release time: 127"
InitInstrument = "Release volume: 127"
InitInstrument = "Legato transition speed: 0"
InitInstrument = "Legato on"
InitInstrument = "Chord auto-pan on: 0"
InitInstrument = "Release samples on"
Pan = 63 # Separate panning for each stage with comma. 0 is left, 63 is center, 127 is right
Volume = 127 # Separate volume for each stage with comma. 0 is minimum volume, 127 is maximum volume
