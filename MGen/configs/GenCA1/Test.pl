# Analysis
Midi_file = load\known-cf-maj.mid # Path and file name of imported midi file
corrections = 1 # Number of corrections to show. Set to 0 to disable corrections
pre_bad = 6 # How many notes to recalculate before rule violation
post_bad = 8 # How many notes to recalculate after rule violation
step_penalty = 3 # Penalty for adding one more changing step while correcting cantus
pitch_penalty = 1 # Penalty for changing note one more diatonic step while correcting cantus
show_transpose = 0,12 # Semitones to transpose each voice (separated with comma)
fullscan_max = 5 # Maximum steps length to full scan. If melody is longer, use SWA
approximations = 10 # Maximum number of approximations to run if penalty decreases
swa_steps = 5 # Size of Sliding Window Approximation algorithm window in steps

# Main parameters
Instruments = Violin,Piano,Piano,Piano,Piano,Piano,Piano,Piano,Piano,Piano
s_len = 7 # Maximum number of measures to full scan. 6-7 is recommended. Lower values can create less melodic results. Higher values are slow to compute
min_interval = 4 # Minimum diatonic interval in cantus (7 = octave)
max_interval = 7 # Maximum diatonic interval in cantus (7 = octave)
first_note = C5 # Starting note of each cantus
last_diatonic_int = 0 # Diatonic interval between first and last note
tempo = 100-130 # Tempo range
random_seed = 0 # Seed melody with random numbers. This ensures giving different results if generation is very slow.
shuffle = 0 # If you want to shuffle all canti after generation (can shuffle up to 32000 canti)

# Rules: 0 - show only without violation, 1 - show with or without violation, 2 - show only violations
# ! means that this rule can produce results without other rules with c_len=9, interval 4-7
Strict                    = 1 # No violated rules
Prepared unfilled 3rd     = 1 # 3rd is unfilled, but was prepared by filling or leap before
Prepared unresolved 3rd   = 1 #! 3rd is unresolved, but was prepared by filling or leap before
Two 3rds after 6/8        = 1 # Chain of two thirds in one direction after 6th or octave
Late <6th resolution      = 1 #! Leap less than 6th is resolved after a second note
Leap back <5th            = 1 # Leap returns to same note (in case leap is equal or less than 4th)
Seventh                   = 1 # Seventh interval leap
Leap pre-late fill        = 0 #! Melody has unfilled gaps
Many leaps                = 0 #! Maximum allowed max_leaps during max_leap_steps
3rd to last is CEG        = 1 #! Third to last note is not Subdominant (C E G forbidden)
Last leap                 = 1 # Last step is a leap
Tritone resolved          = 1 # Tritone is prepared by E and resolved to C
3 letters in a row        = 1 #! 3 same letters (TDS) in possible chord names in a row
Unfilled leap             = 0 #! Leap is not filled
Leap to leap resolution   = 1 # Resolution of leap with another leap
Two 3rds                  = 0 # Chain of two thirds in one direction
4 step miss               = 0 # 4 steps missing one of 3 letters (TDS) in possible chord names in a row
Outstanding repeat        = 0 # Prohibit repeating of 3 notes closer than repeat_steps between first notes (if beats are same)
Long smooth               = 0 # Too long smooth movement
Long line                 = 0 # Too long smooth movement in one direction (linear)
Close repeat              = 0 # Two notes repeat in contact
Stagnation                = 0 # Stagnation on one note
Multiple culminations     = 0 # Melody has more than one culmination with the same note
2nd to last not D         = 0 # Second to last note is not Dominant (C E F A forbidden)
4 letters in a row        = 0 # 4 same letters (TDS) in possible chord names in a row
>4 letters in a row       = 0 # More than 4 same letters (TDS) in possible chord names in a row
5 step miss               = 0 # 5 steps missing one of 3 letters (TDS) in possible chord names in a row
>5 step miss              = 0 # More than 5 steps missing one of 3 letters (TDS) in possible chord names in a row
Late culmination          = 0 # Culmination at last three steps
Leap back >4th            = 0 # Leap returns to same note (in case leap is larger than 4th)
Many leaps+               = 0 # Maximum allowed max_leaps2 during max_leap_steps
Leap unresolved           = 0 # Leap unresolved during two next steps (movement direction does not change)
Leap chain                = 0 # Chain of two leaps in one direction
Late >5th resolution      = 0 # Leap greater than 5th is resolved after a second note
Tritone unresolved        = 0 # Tritone is not resolved correctly
Tritone culmination       = 0 # Tritone is the highest leap
3rd to last is leading    = 0 # Third to last note is leading (B forbidden)
Too wide range            = 0 # If range is greater than max_interval (diatonic interval)
Too tight range           = 0 # If range is lower than min_interval (diatonic interval)

#Rule parameters
fill_steps_mul = 2 # Multiply number of notes between leap notes to get steps for filling
max_smooth_direct = 5 # Maximum linear movement in one direction allowed (in steps)
max_smooth = 7 # Maximum linear movement allowed (in steps)
max_leaps = 2 # Maximum allowed max_leaps during max_leap_steps
max_leaps2 = 3 # Maximum allowed max_leaps2 during max_leap_steps for Many leaps+
max_leap_steps = 7
stag_notes = 2 # Maximum allowed stag_notes (same notes) during stag_note_steps
stag_note_steps = 7
repeat_steps = 8 # Prohibit repeating of 3 notes closer than repeat_steps between first notes (if beats are same)

# Technical parameters
midifile_tpq_mul = 8 # All notes are made whole for midi export
show_severity = 1 # =1 to show severity in square brackets in comments to notes (also when exporting to MIDI file)
calculate_correlation = 0 # Enables correlation calculation algorithm. Slows down generation. Outputs to cf1-cor.csv
calculate_blocking = 0 # Enables blocking flags calculation algorithm. Slows down generation.
calculate_stat = 0 # Enables flag statistics calculation algorithm. Slows down generation.
late_require = 0 # Allow not-last scan window to have no needed tags, but no blocked tags. This calculation usually requires much more time
