include "..\include\SM Woodwinds.pl"

# Main
library = "Samplemodeling Flute 2.8.0" # For which library algorithm is optimized

# Instrument parameters
n_min = "A#3" # Lowest note
n_max = "C#7" # Highest note
t_min = 20 # Shortest note in ms
t_max = 7000 # Longest note without pauses in ms (0 = no limit). Decreases with dynamics
#leap_t_min = 100 # Shortest note after leap in ms

# Vibrato adaptor
vib_bell = 20-60 # Maximum vibrato intensity in vibrato bell (for minimum and highest duration)

# Legato adaptor
legato_ahead = 10 # Time in ms to stretch legato notes back to cope with legato delay
max_ahead_note = 12 # Maximum chromatic interval having ahead property

# Nonlegato adaptor
nonlegato_freq = 20 # Frequency (in percent) when legato can be replaced with non-legato by moving note end to the left
nonlegato_minlen = 600 # Minimum note length (in ms) allowed to convert to nonlegato
nonlegato_maxgap = 300 # Maximum gap between notes (in ms) introduced by automatic nonlegato 

# Transition types
vel_harsh = 50 # Velocity equal or above this value triggers harsh sustain
vel_immediate = 33 # Velocity equal or above this value triggers immediate sustain
vel_normal = 10 # Velocity equal or above this value triggers normal sustain
vel_normal_minlen = 400 # Minimum note length (ms) that can have a normal or lower sustain
harsh_freq = 10 # Frequency of harsh sustain articulation in percent of all possible situations
