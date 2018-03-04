include "..\include\SM Woodwinds.pl"

# Main
library = "Samplemodeling Bassoon 2.8.0" # For which library algorithm is optimized

# Instrument parameters
n_min = "A1" # Lowest note
n_max = "F#5" # Highest note
t_min = 20 # Shortest note in ms
t_max = 12000 # Longest note without pauses in ms (0 = no limit). Decreases with dynamics
#leap_t_min = 100 # Shortest note after leap in ms

Breath noise = 50
Key noise = 64

# Accent types
acc_range = 1-64 # (1-127) Map dynamics to specified accent range
harsh_acc_vel = 65 # Velocity equal or above this value triggers harsh sustain
harsh_acc_freq = 100 # Frequency of harsh sustain articulation in percent of all possible situations
slow_acc_vel = 30 # Velocity equal or below this value triggers slow sustain
slow_acc_minlen = 600 # Minimum note length (ms) that can have a normal or lower sustain
