# This config is for testing new combinations of parameters
v_cnt = 2 # Number of voices. In this algorithm all voices will get notes
t_cnt = 400 # Number of steps to generate
min_tempo = 160 # Minimum tempo
max_tempo = 200 # Maximum tempo
min_len = 1 # Minimum note length
max_len = 5 # Maximum note length
min_note = 72 # Minimum note value
max_note = 83 # Maximum note value
note_step = 3 # Note step for random chain algorithm
sleep_ms = 3 # Artificial pause in ms after generating each step
min_dyn = 40 # Minimum dynamics
max_dyn = 125 # Maximum dynamics
pause_freq = 10 # Percent of notes that will be replaced with pauses
Instruments = Trumpet,Piano # Instruments separated with comma
dyn_step = 20 # Maximum dynamic change per step