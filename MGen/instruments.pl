# This file contains parameters of all supported instruments for MIDI port sending
# Instrument types:
# 0 - Melodic percussion
# 1 - Friedlander violin

Instrument = Piano
Type = 0 # Instrument type
n_min = 30 # Lowest note
n_max = 80 # Highest note
t_min = 20 # Shortest note in ms
t_max = 0 # Longest melody withot pauses in ms (0 = no limit). Decreases with dynamics
poly = 6 # Maximum number of simultaneous voices
leap_t_min = 1 # Shortest note after leap
dying = 5 # Seconds to fade out at mf
library = Any # For which library algorithm is optimized
lengroup2 = 30 # Percent of grouping notes into groups of 2
lengroup3 = 30 # Percent of grouping notes into groups of 3
lengroup4 = 30 # Percent of grouping notes into groups of 4
lengroup_edt1 = 50 # delta to alter note ending time of all notes in group except for the last (ms)
lengroup_edt2 = -50 # delta to alter note ending time of last note in group (ms)

Instrument = Violin
Type = 1 # Instrument type
n_min = G3 # Lowest note
n_max = F7 # Highest note
t_min = 220 # Shortest note in ms
t_max = 0 # Longest melody withot pauses in ms (0 = no limit). Decreases with dynamics
poly = 1 # Maximum number of simultaneous voices
leap_t_min = 1 # Shortest note after leap
dying = 0 # Seconds to fade out at mf
library = Friedlander Violin 1.5 # For which library algorithm is optimized
CC_dynamics = 11
CC_vib_intens = 1
CC_vib_speed = 14
max_slur_count = 2 # Use slur for 2nd moves, but no longer than X moves
max_slur_interval = 2 # in semitones
slur_ks = 44 # Keyswitch to send to create slur
legato_ahead = 165 # Time in ms to stretch legato notes back to cope with legato delay
nonlegato_freq = 20 # Frequency (in percent) when legato can be replaced with non-legato by moving note end to the left
nonlegato_minlen = 400 # Minimum note length (in ms) allowed to convert to nonlegato
