include "..\include\SM Brass.pl"

# Main
library = Samplemodeling Trombone v3 # For which library algorithm is optimized
Pan_default = 60

# Techniques mapping to commands
Technique = "straight mute; Mute: 25"
Technique = "cup mute; Mute: 42"
Technique = "bucket mute; Mute: 60"
Technique = "harmon mute; Mute: 76"
Technique = "harmon+stem mute; Mute: 104"

# Instrument parameters
n_min = E2 # Lowest note
n_max = F5 # Highest note
t_min = 20 # Shortest note in ms
t_max = 12000 # Longest note without pauses in ms (0 = no limit). Decreases with dynamics
#leap_t_min = 100 # Shortest note after leap in ms
ks1 = 24 # First keyswitch position

# Note ending adaptor
end_sfl_dur = 140 # Duration of short fall ending
end_sfl_freq = 0 # Frequency of short fall ending
end_pbd_dur = 600 # Duration of vibrato ending
end_pbd_freq = 30 # Frequency of vibrato ending
end_vib_dur = 510 # Duration of vibrato ending2
end_vib_freq = 30 # Frequency of vibrato ending2
end_vib2_dur = 266 # Duration of vibrato ending3
end_vib2_freq = 10 # Duration of vibrato ending3

# Mute adaptor
mute_import = 1 # Set to 0 to ignore mutes in source MIDI file and play as normal notes
mute_predelay = 300 # Delay between mute keyswitch and note in ms
mute_activate = "harmon mute" # Choose mute that can be activated from MIDI file
mute_deactivate = "senza sord" # Choose mute that can be activated from MIDI file

# Accent types
acc_range = 1-127 # (1-127) Map dynamics to specified accent range
harsh_acc_vel = 127 # Velocity equal or above this value triggers harsh sustain
harsh_acc_freq = 100 # Frequency of harsh sustain articulation in percent of all possible situations
slow_acc_vel = 0 # Velocity equal or below this value triggers slow sustain
slow_acc_minlen = 600 # Minimum note length (ms) that can have a normal or lower sustain
