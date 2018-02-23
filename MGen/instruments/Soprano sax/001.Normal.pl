include "..\include\SM Woodwinds.pl"

# Main
library = "Samplemodeling Saxophones 2.8.0" # For which library algorithm is optimized

# Instrument parameters
n_min = "F#3" # Lowest note
n_max = "A6" # Highest note
t_min = 20 # Shortest note in ms
t_max = 8000 # Longest note without pauses in ms (0 = no limit). Decreases with dynamics
#leap_t_min = 100 # Shortest note after leap in ms

Breath noise = 64
Key noise = 64

# Transition types
harsh_acc_vel = 50 # Velocity equal or above this value triggers harsh sustain
harsh_acc_freq = 10 # Frequency of harsh sustain articulation in percent of all possible situations
slow_acc_vel = 40 # Velocity equal or above this value triggers normal sustain
slow_acc_minlen = 1000 # Minimum note length (ms) that can have a normal or lower sustain
