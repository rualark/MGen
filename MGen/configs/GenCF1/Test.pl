# Main parameters
t_cnt = 1000000 # Maximum number of canti generated
c_len = 9 # Number of measures in each cantus. Usually 9 to 11
s_len = 7 # Maximum number of measures to full scan. 6-7 is recommended. Lower values can create less melodic results. Higher values are slow to compute
min_interval = 4 # Minimum diatonic interval in cantus (7 = octave)
max_interval = 7 # Maximum diatonic interval in cantus (7 = octave)
key = C # Music key to use for generation
random_key = 0 # Allow generator to select random key
first_note = C5 # Starting note of each cantus
last_note = C5 # Ending note of each cantus
tempo = 100-130 # Tempo range
random_seed = 0 # Seed melody with random numbers. This ensures giving different results if generation is very slow.
shuffle = 0 # If you want to shuffle all canti after generation (can shuffle up to 32000 canti)

# Rules: 0 - show only without violation, 1 - show with or without violation, 2 - show only violations
# ! means that this rule can produce results without other rules with c_len=9, interval 4-7
# Flag order in list below goes from green - less severe (top) to red - more severe (bottom)
Strict                    = 1 # No violated rules
Prepared unfilled 3rd     = 0 #S 3rd is unfilled, but was prepared by filling or leap before
Prepared unresolved 3rd   = 0 #S! 3rd is unresolved, but was prepared by filling or leap before
Two 3rds after 6/8        = 0 #S Chain of two thirds in one direction after 6th or octave
Late <6th resolution      = 0 #S! Leap less than 6th is resolved after a second note
Leap back <5th            = 0 #S Leap returns to same note (in case leap is equal or less than 4th)
Leap pre-late fill        = 0 #S! Melody has unfilled gaps
Many leaps                = 0 #S! Maximum allowed max_leaps during max_leap_steps
3rd to last is CEG        = 0 #SC! Third to last note is not Subdominant (C E G forbidden)
Last leap                 = 0 #S Last step is a leap
Tritone resolved          = 0 #S Tritone is prepared by E and resolved to C
3 letters in a row [C]    = 0 #L! 3 same letters (TDS) in possible chord names in a row (constant)
Unfilled leap             = 0 #U! Leap is not filled
Leap to leap resolution   = 0 #S Resolution of leap with another leap
Two 3rds                  = 0 #U Chain of two thirds in one direction
4 step miss [V]           = 0 #L 4 steps missing one of 3 letters (TDS) in possible chord names in a row (variants)
Minor seventh             = 0 #U Minor seventh interval leap
Outstanding repeat        = 0 #U Prohibit repeating of 3 notes closer than repeat_steps between first notes (if beats are same)
Long smooth               = 0 #U Too long smooth movement
Long line                 = 0 #U Too long smooth movement in one direction (linear)
First steps without C     = 0 #L First first_steps_tonic steps do not have C note
Close repeat              = 0 #U Two notes repeat in contact
Stagnation                = 0 #U Stagnation on one note
Multiple culminations     = 0 #U Melody has more than one culmination with the same note
Major seventh             = 0 #U Major seventh interval leap
2nd to last not GBD       = 0 #C Second to last note is not Dominant (C E F A forbidden)
4 letters in a row [C]    = 0 #C 4 same letters (TDS) in possible chord names in a row (constant)
>4 letters in a row [C]   = 0 #U More than 4 same letters (TDS) in possible chord names in a row (constant)
5 step miss [V]           = 0 #C 5 steps missing one of 3 letters (TDS) in possible chord names in a row (variants)
>5 step miss [V]          = 0 #U More than 5 steps missing one of 3 letters (TDS) in possible chord names in a row (variants)
First steps without CEG   = 0 #U First first_steps_tonic steps do not have CEG notes
Late culmination          = 0 #L Culmination at last three steps
Leap back >4th            = 0 #U Leap returns to same note (in case leap is larger than 4th)
Many leaps+               = 0 #U Maximum allowed max_leaps2 during max_leap_steps
Leap unresolved           = 0 #U Leap unresolved during two next steps (movement direction does not change)
Leap chain                = 0 #U Chain of two leaps in one direction
Late >5th resolution      = 0 #U Leap greater than 5th is resolved after a second note
Tritone unresolved        = 0 #U Tritone is not resolved correctly
Tritone culmination       = 0 #U Tritone is the highest leap
3rd to last is leading    = 0 #C Third to last note is leading (B forbidden)
Too wide range            = 0 #CV If range is greater than max_interval (diatonic interval)
Too tight range           = 0 #CV If range is lower than min_interval (diatonic interval)
3 letters in a row [V]    = X # 3 same letters (TDS) in possible chord names in a row (variants)
4 step miss [C]           = X # 4 steps missing one of 3 letters (TDS) in possible chord names in a row (constant)
4 letters in a row [V]    = X # 4 same letters (TDS) in possible chord names in a row (variants)
>4 letters in a row [V]   = X # More than 4 same letters (TDS) in possible chord names in a row (variants)
5 step miss [C]           = X # 5 steps missing one of 3 letters (TDS) in possible chord names in a row (constant)
>5 step miss [C]          = X # More than 5 steps missing one of 3 letters (TDS) in possible chord names in a row (constant)

# Rule parameters
# Lower voice
harm_var = TS,DS,DT,S,DT,TS,D # Variants of possible harmonic meaning of each note
harm_const = T,DS,T,S,D,S,D # Constant harmonic meaning that is always possible for particular note (must be after harm_var)
# Higher voice
#harm_var = TS,DS,DST,S,DT,TS,DT # Variants of possible harmonic meaning of each note
#harm_const = TS,DS,T,S,DT,S,D # Constant harmonic meaning that is always possible for particular note (must be after harm_var)

fill_steps_mul = 2 # Multiply number of notes between leap notes to get steps for filling
max_smooth_direct = 5 # Maximum linear movement in one direction allowed (in steps)
max_smooth = 7 # Maximum linear movement allowed (in steps)
max_leaps = 2 # Maximum allowed max_leaps during max_leap_steps
max_leaps2 = 3 # Maximum allowed max_leaps2 during max_leap_steps for Many leaps+
max_leap_steps = 7
stag_notes = 2 # Maximum allowed stag_notes (same notes) during stag_note_steps
stag_note_steps = 7
repeat_steps = 8 # Prohibit repeating of 3 notes closer than repeat_steps between first notes (if beats are same)
first_steps_tonic = 3 # Number of first steps, which must contain tonic note

# Technical parameters
midifile_out_mul = 8 # All notes are made whole for midi export
show_severity = 1 # =1 to show severity in square brackets in comments to notes (also when exporting to MIDI file)
calculate_correlation = 0 # Enables correlation calculation algorithm. Slows down generation. Outputs to cf1-cor.csv
calculate_blocking = 0 # Enables blocking flags calculation algorithm. Slows down generation.
calculate_stat = 0 # Enables flag statistics calculation algorithm. Slows down generation.
late_require = 0 # Allow not-last scan window to have no needed tags, but no blocked tags. This calculation usually requires much more time
