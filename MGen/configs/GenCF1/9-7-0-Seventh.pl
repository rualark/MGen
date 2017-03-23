# Main parameters
t_cnt = 1000 # Maximum number of canti generated
c_len = 9 # Number of measures in each cantus. Usually 9 to 11
max_interval = 7 # Maximum diatonic interval in cantus (7 = octave)
first_note = 72 # Starting note of each cantus
last_diatonic_int = 0 # Diatonic interval between first and last note
min_tempo = 100 # Minimum tempo
max_tempo = 130 # Maximum tempo

# Rules
accept = sPtjolcardgfm # Each upper case letter allows showing canti with specific properties:
# 0  S - Strict
# 1  p - Seventh
# 2  t - Tritone
# 3  j - Too many leaps
# 4  o - Too long smooth movement
# 5  l - Too long smooth movement in one direction (linear)
# 6  c - Chain of leaps in one direction
# 7  a - Leap is resolved after a second note
# 8  r - Leap returns to same note
# 9  d - Two notes repeat in contact
# 10 g - Stagnation on one note
# 11 f - Leap is not filled
# 12 m - Multiple culminations

max_unfill_steps = 7 # Number of steps to check filling
max_repeat_mul = 2 # Allow repeat of X notes after at least X*max_repeat_mul steps if beats are different
max_smooth_direct = 5 # Maximum linear movement in one direction allowed (in steps)
max_smooth = 7 # Maximum linear movement allowed (in steps)
max_leaps = 2 # Maximum allowed max_leaps during max_leap_steps
max_leap_steps = 7
stag_notes = 2 # Maximum allowed stag_notes (same notes) during stag_note_steps
stag_note_steps = 7

# Technical parameters
midifile_tpq_mul = 8 # All notes are made whole for midi export
