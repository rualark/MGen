include "..\include\SampleModeling.pl"

# Main
library = Samplemodeling Horn v3 # For which library algorithm is optimized

# Instrument parameters
n_min = A1 # Lowest note
n_max = G5 # Highest note
t_min = 100 # Shortest note in ms
t_max = 12000 # Longest melody without pauses in ms (0 = no limit). Decreases with dynamics
#leap_t_min = 100 # Shortest note after leap in ms
ks1 = 12 # First keyswitch position

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
splitpo_freq = 50 # Frequency of split portamento in percent
splitpo_pent_minint = 100 # Disable
gliss_freq = 0 # Disable

# Vibrato adaptor
vib_bell = 20-60 # Maximum vibrato intensity in vibrato bell (for minimum and highest duration)

# Legato adaptor
legato_ahead = 375,640 # Time in ms to stretch legato notes back to cope with legato delay [norm,chrom,penta,gliss]
# Time in ms to stretch legato notes back for chromatic split portamento (for each pitch interval)
ahead_chrom = 0,250,310,440,500,520,560,570,625,630,635,640,640
