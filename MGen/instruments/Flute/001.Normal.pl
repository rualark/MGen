include "..\include\SM Woodwinds.pl"

# Main
library = "Samplemodeling Flute 2.8.0" # For which library algorithm is optimized

# Instrument parameters
n_min = "A#3" # Lowest note
n_max = "C#7" # Highest note
t_min = 20 # Shortest note in ms
t_max = 8000 # Longest note without pauses in ms (0 = no limit). Decreases with dynamics
#leap_t_min = 100 # Shortest note after leap in ms

# Transition types
vel_harsh = 50 # Velocity equal or above this value triggers harsh sustain
vel_immediate = 1 # Velocity equal or above this value triggers immediate sustain
vel_normal = 1 # Velocity equal or above this value triggers normal sustain
vel_normal_minlen = 400 # Minimum note length (ms) that can have a normal or lower sustain
harsh_freq = 10 # Frequency of harsh sustain articulation in percent of all possible situations

