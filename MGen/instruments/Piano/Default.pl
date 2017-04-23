# Main
Library = Native Instruments Gentleman # For which library algorithm is optimized
Type = 0 # Instrument type

# Instrument parameters
n_min = 21 # Lowest note
n_max = 108 # Highest note
t_min = 20 # Shortest note in ms
t_max = 0 # Longest melody withot pauses in ms (0 = no limit). Decreases with dynamics
poly = 100 # Maximum number of simultaneous voices
leap_t_min = 1 # Shortest note after leap
#dying = 5 # Seconds to fade out at mf
lengroup2 = 30 # Percent of grouping notes into groups of 2
lengroup3 = 30 # Percent of grouping notes into groups of 3
lengroup4 = 30 # Percent of grouping notes into groups of 4
lengroup_edt1 = 70 # delta to alter note ending time of all notes in group except for the last (ms)
lengroup_edt2 = -100 # delta to alter note ending time of last note in group (ms)
rand_start = 8 # Randomize note starts not greater than percent of note length 
rand_end = 8 # Randomize note end not greater than percent of note length 
rand_start_max = 70 # Maximum shift in ms
rand_end_max = 70 # Maximum shift in ms
