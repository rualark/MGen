# ============================================
#	ID	stohastic distribution function type
#	1 	uniform
#	2 	linear low priority
#	3 	linear high priority
#	4 	triangular
#	5 	linear low priority with depth
#	6 	linear high priority with depth
#	7 	triangular with depth
# ============================================

# [COMMON]
sleep_ms = 3 # Artificial pause in ms after generating each step
v_cnt = 2 # Number of voices. In this algorithm all voices will get notes
t_cnt = 400 # Number of steps to generate


# [PITCH]
min_pitch = 40 # Minimum note MIDI pitch
max_pitch = 72 # Maximum note MIDI pitch
distr_type_pitch = 7 # Type of stohastic distribution (see above)
distr_depth_pitch = 5 # Depth of stohastic distribution (if presented)


# [DYNAMIC]
min_dyn = 90
max_dyn = 120
distr_type_dyn = 6 # Type of stohastic distribution (see above)
distr_depth_dyn = 3 # Depth of stohastic distribution (if presented)


# [LENGTH]
min_len = 1 # Minimum note length
max_len = 5 # Maximum note length
note_step = 3 # Note step for random chain algorithm
distr_type_len = 5 # Type of stohastic distribution (see above)
distr_depth_len = 7 # Depth of stohastic distribution (if presented)


# [PAUSE]
pause_freq = 25 # Percent of notes that will be replaced with pauses


# [TEMPO]
min_tempo = 160 # Minimum tempo
max_tempo = 200 # Maximum tempo
distr_type_tempo = 2 # Type of stohastic distribution (see above)
distr_depth_tempo = 1 # Depth of stohastic distribution (if presented)

# [ORCH]
Instruments = Trumpet,Piano # Instruments separated with comma
