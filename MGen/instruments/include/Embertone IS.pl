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

# Techniques mapping to commands
Technique = "solo; Ensemble on: 0"
Technique = "tutti; Ensemble on" # Tutti mode in Viola requires gliss_freq=0 due to bug in virtual instrument

# Initial setup
InitCommand = "Transition attenuation: 0"
InitCommand = "Bow noise reduction: 0"
InitCommand = "Reverb level: 0" # default: enabled
InitCommand = "Round-robin on"
InitTechnique = "solo"

# Automation parameters
CC_dynamics = 11
CC_ma = 9 # Number of CC steps to moving average (please use only odd numbers)
CC_steps = 30 # Number of CC steps in one note step (please use only odd numbers)

# Vibrato adaptor
CC_vib = 1 # CC number for vibrato intensity
CC_vibf = 14 # CC number for vibrato speed
