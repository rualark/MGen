include "..\include\SampleModeling.pl"

# Main
library = Samplemodeling Trumpet v3 # For which library algorithm is optimized

# Instrument parameters
n_min = E3 # Lowest note
n_max = G6 # Highest note
t_min = 50 # Shortest note in ms
t_max = 12000 # Longest melody without pauses in ms (0 = no limit). Decreases with dynamics
#leap_t_min = 100 # Shortest note after leap in ms
