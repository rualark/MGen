include "..\include\Embertone IS.pl"

# Main
library = "Fischer Viola 1.0" # For which library algorithm is optimized

# Instrument parameters
n_min = C3 # Lowest note
n_max = G6 # Highest note
t_min = 10 # Shortest note in ms
t_max = 0 # Longest melody withot pauses in ms (0 = no limit). Decreases with dynamics
#leap_t_min = 100 # Shortest note after leap in ms

# Controls
#CC_Name = 59: "Transition speed"
CC_Name = 66: "Bow position"
CC_Name = 64: "Slur"
CC_Name = 12: "Portamento speed" # Used only when enabled Portamento CC mode on
CC_Name = 5: "Portamento mode" # Used only when enabled Portamento CC mode on
CC_Name = 80: "Portamento speed2" # Used only when enabled Portamento CC mode on. Not mapped: mapping and then playing this CC results in crash.

# These controls should be mapped manually
CC_Name = 20: "Retrigger on" # All except Cello
CC_Name = 16: "Portamento CC mode on" # All except Cello
CC_Name = 21: "Speed control on"

KswGroup = "C#2: Slur while held"
KswGroup = "G#1: Ghost note"
KswGroup = "A1: Color on", "A#1: Color off"
KswGroup = "B1: Portamento"
KswGroup = "C2: Sustain", "D2: Staccato", "D#2: Pizzicato", "E2: Tremolo" # Style
KswGroup = "F2: Con sordino", "F#2: Senza sordino"
KswGroup = "G2: Normal", "G#2: Sul ponticello", "A2: Sul tasto" # Bow position
KswGroup = "A#2: Legato", "B2: Polyphonic"

# Seems that vibrato styles cannot be selected with CC or keyswitch (bug?)
CC_Name = 2:  "Vibrato style"
KswGroup = "D#1: Vibrato style - Default"
KswGroup = "E1: Vibrato style - Progressive"
KswGroup = "F1: Vibrato style - Open string"
KswGroup = "F#1: Vibrato style - Gentle"
KswGroup = "G1: Vibrato style - Passionate"

# Techniques mapping to commands
Technique = "con sord; Con sordino"
Technique = "ord; Senza sordino + Normal + Sustain"
Technique = "nat; Senza sordino + Normal + Sustain"
Technique = "senza sord; Senza sordino"
Technique = "sul pont; Sul ponticello"
Technique = "sul tast; Sul tasto"
Technique = "stac; Staccato" # TODO: not implemented yet
Technique = "spic; Staccato" # TODO: not implemented yet
Technique = "martele; Staccato" # TODO: not implemented yet
Technique = "saltando; Staccato" # TODO: not implemented yet
Technique = "sautille; Staccato" # TODO: not implemented yet
Technique = "arco; Sustain" # TODO: not implemented yet
Technique = "pizz; Pizzicato" # TODO: not implemented yet
Technique = "trem; Tremolo"

# Initial setup
InitCommand = "Ensemble intonation: 20" # default 8
InitCommand = "Ensemble L: 40" # default 0
InitCommand = "Ensemble R: 87" # default 127
InitCommand = "Ensemble tightness: 22" # default 22, alternative 65
InitCommand = "Ensemble randomize: 36" # default 36, alternative 100
InitCommand = "Ensemble combine transitions"
InitCommand = "Transition speed lower: 16"
InitCommand = "Transition speed upper: 16" # Default 54, but automatic legato speed is not supported yet because it decreases audio quality
InitCommand = "Solo intonation: 0"
InitCommand = "Retrigger on"
InitCommand = "Responsiveness: 44"
InitCommand = "Dynamic KSW threshold: 100"
InitCommand = "Shorts length control on: 0" # default 0
InitCommand = "Release samples on" # default 0
InitCommand = "Portamento CC mode on: 0"
InitCommand = "Portamento velocity threshold: 30"
InitCommand = "Vibrato style - Progressive"
InitCommand = "Legato"
InitCommand = "Color off"
InitTechnique = "ord"

# Retrigger adaptor
CC_retrigger = 64 # This CC will be set before note retriger and reset soon after
retrigger_freq = 50 # Percent to use CC retrigger instead of non-legato when repeating notes
retrigger_min_len = 600 # Minimum next note length in ms to use retrigger
retrigger_rand_max = 300 # Maximum length in ms to move note end to the left in case of nonlegato retrigger
retrigger_rand_end = 50 # Maximum percent of note length to move note end to the left in case of nonlegato retrigger

# Slur adaptor
max_slur_count = 2 # Use slur for 2nd moves, but no longer than X moves
max_slur_interval = 2 # in semitones

# Legato adaptor
legato_ahead = 165 # Time in ms to stretch legato notes back to cope with legato delay
max_ahead_note = 12 # Maximum chromatic interval having ahead property
all_ahead = 54 # Time in ms to stretch sutain notes (not legato) back to cope with slow attack

# Nonlegato adaptor
nonlegato_freq = 20 # Frequency (in percent) when legato can be replaced with non-legato by moving note end to the left
nonlegato_minlen = 400 # Minimum note length (in ms) allowed to convert to nonlegato
nonlegato_maxgap = 300 # Maximum gap between notes (in ms) introduced by automatic nonlegato 

# Staccato adaptor
stac_auto = 1 # Set to 0 to disable automatic staccato articulations
stac_ahead = -1 # Time in ms to stretch stac notes back to cope with slow attack. -1 falls back to all_ahead
stac_dyn_range = 15-55 # Percent of staccato dynamics compared to sustain notes
spic_import = 1 # Allow to import spiccato technique from midi file into staccato articulation
stac_import = 1 # Set to 0 to ignore staccato in source MIDI file and play as normal notes. Set to 1 to import as staccato
marc_import = 0 # Set to 0 to ignore marcato in source MIDI file and play as normal notes. Set to 1 to import as staccato

# Sul tasto adaptor
tasto_import = 1 # Set to 0 to ignore sul tasto in source MIDI file and play as normal notes
bow_lock = 0 # 0 - do not lock bow, 1 - always sul tasto, 2 - always sul ponticello

# Pizzicato adaptor
pizz_import = 1 # Set to 0 to ignore pizzicato in source MIDI file and play as normal notes
pizz_ahead = -1 # Time in ms to stretch pizz notes back to cope with slow attack. -1 falls back to all_ahead
pizz_dyn_range = 0-60 # Percent of pizz dynamics compared to sustain notes

# Tremolo adaptor
trem_import = 1 # Set to 0 to ignore tremolo keyswitches and program changes in source MIDI file and play as normal notes

# Accent types
acc_range = 0-100 # Map dynamics to specified accent range
harsh_acc_vel = 101 # Velocity equal or above this value triggers harsh sustain
slow_acc_vel = 40 # Velocity equal or below this value triggers slow sustain
slow_acc_minlen = 600 # Minimum note length (ms) that can have a normal or lower sustain

# In Viola, harsh sustains are too harsh, so I disable them
harsh_acc_freq = 100 # Frequency of harsh sustain articulation in percent of all possible situations

# Gliss adaptor
gliss_leg_vel = 30 # Velocity below this value triggers glissando transition
gliss_minlen = 1000 # Minimum note length that can have a gliss transition
gliss_freq = 0 # Frequency of gliss articulation in percent

