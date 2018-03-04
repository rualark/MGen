include "..\include\SI VoR.pl"

# Controls
KswGroup = "C0: Ah", "C#0: Oo" # Syllable

# Initial setup
InitCommand = "Ah"

# Instrument parameters
n_min = B3 # Lowest note
n_max = D6 # Highest note
t_min = 100 # Shortest note in ms
t_max = 12000 # Longest melody withot pauses in ms (0 = no limit). Decreases with dynamics
#leap_t_min = 100 # Shortest note after leap in ms

# Legato adaptor
all_ahead = 125 # Time in ms to stretch all notes (sustains and legato) back to cope with slow attack

# Nonlegato adaptor
nonlegato_freq = 20 # Frequency (in percent) when legato can be replaced with non-legato by moving note end to the left
nonlegato_minlen = 400 # Minimum note length (in ms) allowed to convert to nonlegato
nonlegato_maxgap = 500 # Maximum gap between notes (in ms) introduced by automatic nonlegato 

# Bell adaptor
bell_mindur = 700-200 # Minimum note duration (ms) that can have a bell
bell_mul = 0.2-0.2 # Multiply dynamics by this parameter at bell start-end
bell_len = 30-30 # Percent of notelength to use for slope at bell start-end

# Reverse bell adaptor
rbell_freq = 80 # Frequency to apply reverse bell when all conditions met
rbell_dur = 300-1000 # Minimum note duration (ms) that can have a reverse bell - that will have deepest reverse bell
rbell_mul = 0.8-0.2 # Multiply dynamics by this parameter at bell center with mindur - with longer dur
rbell_pos = 20-80 # Leftmost-rightmost minimum reverse bell position inside window (percent of window duration)

# Vibrato adaptor
CC_vib = 80 # CC number for vibrato intensity
vib_bell_top = 5-95 # Leftmost-rightmost maximum vibrato intensity in note (percent of note duration)
vib_bell_exp = 2 # Exponent to create non-linear bell shape
vib_bell_freq = 100 # Frequency to apply vibrato bell when all conditions met
vib_bell_dur = 600-1200 # Minimum note duration (ms) that can have a vibrato bell - that can have highest vibrato bell
vib_bell = 50-95 # Maximum vibrato intensity in vibrato bell (for minimum and highest duration)
rnd_vib = 10 # Randomize vibrato intensity not greater than this percent

# Randomization
rnd_vel = 8 # Randomize note velocity not greater than this percent
rnd_dyn = 8 # Randomize step dynamics not greater than this percent
