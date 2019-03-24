include "..\include\LASS.pl"
Pan_default = 67

# Instrument parameters
n_min = C1 # Lowest note
n_max = C4 # Highest note
t_min = 1 # Shortest note in ms (note will not sound if shorter)
t_max = 0 # Longest melody withot pauses in ms (0 = no limit). Decreases with dynamics

all_ahead = 54 # Time in ms to stretch sutain notes (not legato) back to cope with slow attack
