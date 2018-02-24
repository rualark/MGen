include "..\include\SM Woodwinds.pl"

# Main
library = "Samplemodeling Flute 2.8.0" # For which library algorithm is optimized

# Instrument parameters
n_min = "A#3" # Lowest note
n_max = "C#7" # Highest note
t_min = 20 # Shortest note in ms
t_max = 8000 # Longest note without pauses in ms (0 = no limit). Decreases with dynamics
#leap_t_min = 100 # Shortest note after leap in ms

# Accent types
acc_range = 0-127 # Map dynamics to specified accent range
harsh_acc_vel = 90 # Velocity equal or above this value triggers harsh sustain
harsh_acc_freq = 100 # Frequency of harsh sustain articulation in percent of all possible situations
slow_acc_vel = 0 # Velocity equal or below this value triggers slow sustain
slow_acc_minlen = 400 # Minimum note length (ms) that can have a normal or lower sustain

