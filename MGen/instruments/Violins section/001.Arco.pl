include "_Default.pl"

# Slur adaptor
max_slur_count = 2 # Use slur for 2nd moves, but no longer than X moves
max_slur_interval = 2 # in semitones

# Legato adaptor
legato_ahead = 35 # Time in ms to stretch legato notes back to cope with legato delay
max_ahead_note = 12 # Maximum chromatic interval having ahead property

# Nonlegato adaptor
nonlegato_freq = 20 # Frequency (in percent) when legato can be replaced with non-legato by moving note end to the left
nonlegato_minlen = 600 # Minimum note length (in ms) allowed to convert to nonlegato
nonlegato_maxgap = 300 # Maximum gap between notes (in ms) introduced by automatic nonlegato 

# Staccato adaptor
stac_auto = 1 # Set to 0 to disable automatic staccato articulations
stac_maxlen = 400 # Maximum note length (ms) to be converted to staccato in case of non-legato on both sides
stac_ahead = -1 # Time in ms to stretch stac notes back to cope with slow attack. -1 falls back to all_ahead
stac_dyn_range = 1-50 # (1-100) Percent of staccato dynamics compared to sustain notes
spic_import = 1 # Allow to import spiccato technique from midi file into staccato articulation
stac_import = 1 # Set to 0 to ignore staccato in source MIDI file and play as normal notes. Set to 1 to import as staccato
stac_chan = 3 # Send stac to separate relative channel
stac_db = 0 # Relative db volume for stac articulation
marc_import = 1 # Set to 0 to ignore marcato in source MIDI file and play as normal notes. Set to 1 to import as staccato

# Pizzicato adaptor
pizz_import = 1 # Set to 0 to ignore pizzicato in source MIDI file and play as normal notes
pizz_ahead = -1 # Time in ms to stretch pizz notes back to cope with slow attack. -1 falls back to all_ahead
pizz_dyn_range = 1-60 # (1-100) Percent of pizz dynamics compared to sustain notes
pizz_chan = 4 # Send pizz to separate relative channel
pizz_db = -12 # Relative db volume for pizz articulation

# Tremolo adaptor
trem_maxlen = 100 # Maximum length of one note inside tremolo for detection
trem_min_repeats = 3 # Minimum number of note repeats to detect tremolo
trem_dyn_range = 0-80 # (0-100) Percent of tremolo dynamics compared to sustain notes
trem_import = 1 # Set to 0 to ignore tremolo keyswitches and program changes in source MIDI file and play as normal notes
trem_chan = 2 # Send trem to separate relative channel
trem_db = 0 # Relative db volume for trem articulation

# Accent velocity
acc_range = 1-90 # (1-127) Map dynamics to specified accent range
slow_acc_vel = 60 # Velocity below this value triggers slow attack sustain
slow_acc_minlen = 800 # Minimum note length (ms) that can have a slow attack sustain
harsh_acc_vel = 61 # Velocity equal or above this value triggers harsh attack sustain
harsh_acc_freq = 100 # Frequency of harsh sustain articulation in percent of all possible situations

