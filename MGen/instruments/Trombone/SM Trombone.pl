include "..\include\SampleModeling.pl"

# Main
library = Samplemodeling Trombone v3 # For which library algorithm is optimized

# Instrument parameters
n_min = E2 # Lowest note
n_max = F5 # Highest note
t_min = 100 # Shortest note in ms
t_max = 8000 # Longest note without pauses in ms (0 = no limit). Decreases with dynamics
#leap_t_min = 100 # Shortest note after leap in ms
ks1 = 24 # First keyswitch position

# Note ending adaptor
end_sfl_dur = 140 # Duration of short fall ending
end_sfl_freq = 10 # Frequency of short fall ending
end_pbd_dur = 600 # Duration of vibrato ending
end_pbd_freq = 30 # Frequency of vibrato ending
end_vib_dur = 510 # Duration of vibrato ending2
end_vib_freq = 30 # Frequency of vibrato ending2
end_vib2_dur = 266 # Duration of vibrato ending3
end_vib2_freq = 10 # Duration of vibrato ending3
