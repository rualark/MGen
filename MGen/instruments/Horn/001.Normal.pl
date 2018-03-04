include "..\include\SM Brass.pl"

# Main
library = "Samplemodeling Horn v3" # For which library algorithm is optimized

# Techniques mapping to commands
Technique = "brass stop; Mute: 25"
Technique = "straight mute; Mute: 42"
Technique = "bell res 1; Mute: 60"
Technique = "bell res 2; Mute: 76"
Technique = "bell res 3; Mute: 104"

# Instrument parameters
n_min = A1 # Lowest note
n_max = G5 # Highest note
t_min = 20 # Shortest note in ms
t_max = 12000 # Longest note without pauses in ms (0 = no limit). Decreases with dynamics
#leap_t_min = 100 # Shortest note after leap in ms
ks1 = 12 # First keyswitch position

# Note ending adaptor
end_sfl_dur = 140 # Duration of short fall ending
end_sfl_freq = 0 # Frequency of short fall ending
end_pbd_dur = 640 # Duration of vibrato ending2
end_pbd_freq = 0 # Frequency of vibrato ending2
end_vib_dur = 640 # Duration of vibrato ending
end_vib_freq = 30 # Frequency of vibrato ending
end_vib2_dur = 0 # Disable
end_vib2_freq = 0 # Disable

# Vibrato adaptor
vib_bell = 20-60 # Maximum vibrato intensity in vibrato bell (for minimum and highest duration)

# Legato adaptor
splitpo_freq = 80 # Frequency of split portamento in percent
splitpo_pent_minint = 100 # Disable
gliss_freq = 0 # Disable
splitpo_mindur = 20 # Minimum legato duration to use split portamento
leg_pdur = 30 # Maximum percent of previous note duration, that legato transition can take
leg_cdur = 10 # Maximum percent of current note duration, that legato transition can take
legato_ahead = 80,640 # Time in ms to stretch legato notes back to cope with legato delay [norm,chrom,penta,gliss]
legato_ahead_exp = 2 # Exponent to interpolate note movement ahead from note velocity
# Time in ms to stretch legato notes back for chromatic split portamento (for each pitch interval)
ahead_chrom = 0,250,310,440,500,520,560,570,625,630,635,640,640

# Nonlegato adaptor
all_ahead = 60 # Time in ms to stretch sutain notes (not legato) back to cope with slow attack

# Mute adaptor
mute_import = 1 # Set to 0 to ignore mutes in source MIDI file and play as normal notes
mute_predelay = 300 # Delay between mute keyswitch and note in ms
mute_activate = "brass stop" # Choose mute that can be activated from MIDI file
mute_deactivate = "senza sord" # Choose mute that can be activated from MIDI file

# Accent types
acc_range = 1-110 # (1-127) Map dynamics to specified accent range
harsh_acc_vel = 100 # Velocity equal or above this value triggers harsh sustain
harsh_acc_freq = 100 # Frequency of harsh sustain articulation in percent of all possible situations
slow_acc_vel = 60 # Velocity equal or below this value triggers slow sustain
slow_acc_minlen = 600 # Minimum note length (ms) that can have a normal or lower sustain
