# Main parameters
t_cnt = 1000 # Maximum number of canti generated
c_len = 8 # Number of measures in each cantus. Usually 9 to 11
s_len = 18 # Maximum number of measures to full scan
min_interval = 4 # Minimum diatonic interval in cantus (7 = octave)
max_interval = 5 # Maximum diatonic interval in cantus (7 = octave)
first_note = C5 # Starting note of each cantus
last_diatonic_int = 0 # Diatonic interval between first and last note
tempo = 100-130 # Tempo range
shuffle = 1
calculate_correlation = 1 # Enables correlation calculation algorithm. Slows down generation. Outputs to cf1-cor.csv

# Rules: 0 - show only without violation, 1 - show with or without violation, 2 - show only violations
Strict                    = 1 # No violated rules
Seventh                   = 0 # Seventh interval leap
Tritone resolved          = 0 # Tritone is prepared by E and resolved to C
Tritone unresolved        = 0 # Tritone is not resolved correctly
Tritone culmination       = 0 # Tritone is the highest leap
Many leaps                = 0 # Maximum allowed max_leaps during max_leap_steps
Many leaps+               = 0 # Maximum allowed max_leaps2 during max_leap_steps
Long smooth               = 0 # Too long smooth movement
Long line                 = 0 # Too long smooth movement in one direction (linear)
Two 3rds                  = 0 # Chain of two thirds in one direction
Two 3rds after 6/8        = 0 # Chain of two thirds in one direction after 6th or octave
Late <6th resolution      = 0 # Leap less than 6th is resolved after a second note
Late >5th resolution      = 0 # Leap greater than 5th is resolved after a second note
Leap back <5th            = 0 # Leap returns to same note (in case leap is equal or less than 4th)
Leap back >4th            = 0 # Leap returns to same note (in case leap is larger than 4th)
Close repeat              = 0 # Two notes repeat in contact
Stagnation                = 0 # Stagnation on one note
Noncontiguous             = 0 # Melody has unfilled gaps
Multiple culminations     = 0 # Melody has more than one culmination with the same note
Last 3 step culmination   = 0 # If culmination is at the last three steps
2nd to last not D         = 0 # Second to last note is not Dominant (C E F A forbidden)
3rd to last is CEG        = 0 # Third to last note is not Subdominant (C E G forbidden)
3rd to last is leading    = 0 # Third to last note is leading (B forbidden)
3 letters in a row        = 0 # 3 same letters (TDS) in possible chord names in a row
4 letters in a row        = 0 # 4 same letters (TDS) in possible chord names in a row
>4 letters in a row       = 0 # More than 4 same letters (TDS) in possible chord names in a row
4 step miss               = 0 # 4 steps missing one of 3 letters (TDS) in possible chord names in a row
5 step miss               = 0 # 5 steps missing one of 3 letters (TDS) in possible chord names in a row
>5 step miss              = 0 # More than 5 steps missing one of 3 letters (TDS) in possible chord names in a row
Late culmination          = 0 # Culmination at last three steps
Last leap                 = 0 # Last step is a leap
Unfilled leap             = 0 # Leap is not filled
Leap unresolved           = 0 # Leap unresolved during two next steps (movement direction does not change)
Leap chain                = 0 # Chain of two leaps in one direction
Prepared unresolved 3rd   = 0 # 3rd is unresolved, but was prepared by filling or leap before
Leap to leap resolution   = 0 # Resolution of leap with another leap

fill_steps_mul = 2 # Multiply number of notes between leap notes to get steps for filling
max_smooth_direct = 5 # Maximum linear movement in one direction allowed (in steps)
max_smooth = 7 # Maximum linear movement allowed (in steps)
max_leaps = 2 # Maximum allowed max_leaps during max_leap_steps
max_leaps2 = 3 # Maximum allowed max_leaps2 during max_leap_steps for Many leaps+
max_leap_steps = 7
stag_notes = 2 # Maximum allowed stag_notes (same notes) during stag_note_steps
stag_note_steps = 7

# Technical parameters
midifile_tpq_mul = 8 # All notes are made whole for midi export

# Not implemented
max_repeat_mul = 2 # Allow repeat of X notes after at least X*max_repeat_mul steps if beats are different
