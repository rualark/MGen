#Main
Library = "Embertone Intimate Strings" # For which library algorithm is optimized
Type = 1 # Instrument type

# Instrument parameters
poly = 1 # Maximum number of simultaneous voices

# Controls
CC_Name = 1:  "Vibrato intensity"
CC_Name = 11: "Dynamics"
CC_Name = 14: "Vibrato speed"
CC_Name = 50: "Reverb level"
CC_Name = 51: "Bow noise reduction"
CC_Name = 52: "Transition attenuation"
CC_Name = 53: "Round-robin on"
CC_Name = 54: "Responsiveness"
CC_Name = 55: "Ensemble on"
CC_Name = 56: "Ensemble combine transitions"
CC_Name = 57: "Ensemble tightness"
CC_Name = 58: "Ensemble randomize"

# Map ensemble controls manually if you plan to use Ensemble mode
CC_Name = 3:  "Ensemble intonation"
CC_Name = 4:  "Ensemble L"
CC_Name = 19:  "Ensemble R"

# These controls should be mapped manually
CC_Name = 6:  "Transition speed lower"
CC_Name = 8:  "Transition speed upper"
CC_Name = 9:  "Solo intonation"
CC_Name = 13: "Shorts length control on"
CC_Name = 15: "Release samples on"
CC_Name = 17: "Portamento velocity threshold"
CC_Name = 18: "Dynamic KSW threshold"

KswGroup = "G2: Sustain", "A2: Staccato", "A#2: Pizzicato", "B2: Tremolo" # Style
