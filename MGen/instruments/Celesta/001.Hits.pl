# Main
library = "True Strike Cinematic Orchestral Percussion" # For which library algorithm is optimized
Type = 0 # Instrument type
poly = 100 # Maximum number of simultaneous voices

# Map CC
CC_Name = 20: "Release"

# Init instrument
Release = 82

# Instrument parameters
n_min = "C4" # Lowest note
n_max = "C8" # Highest note
Volume_default = 107 # (direct CC volume, should not be changed by user)
db_max = 0 # Maximum controlled loudness in db when volume CC is 127
db_coef = 3 # Slope of CC to db function (3 for Kontakt, 1 for SWAM)

