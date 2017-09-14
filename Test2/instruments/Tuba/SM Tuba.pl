include "..\include\SampleModeling.pl"

# Main
library = Samplemodeling Tuba v3 # For which library algorithm is optimized

# Instrument parameters
n_min = C1 # Lowest note
n_max = F4 # Highest note
t_min = 150 # Shortest note in ms
t_max = 10000 # Longest note without pauses in ms (0 = no limit). Decreases with dynamics
#leap_t_min = 100 # Shortest note after leap in ms
ks1 = 0 # First keyswitch position

# Note ending adaptor
end_sfl_dur = 140 # Duration of short fall ending
end_sfl_freq = 10 # Frequency of short fall ending
end_pbd_dur = 640 # Duration of vibrato ending2
end_pbd_freq = 30 # Frequency of vibrato ending2
end_vib_dur = 640 # Duration of vibrato ending
end_vib_freq = 30 # Frequency of vibrato ending
end_vib2_dur = 0 # Disable
end_vib2_freq = 0 # Disable

# Legato adaptor
legato_ahead = 940,520 # Time in ms to stretch legato notes back to cope with legato delay [norm,chrom,penta,gliss]
splitpo_freq = 90 # Frequency of split portamento in percent
splitpo_pent_minint = 100 # Disable
gliss_freq = 0 # Disable
