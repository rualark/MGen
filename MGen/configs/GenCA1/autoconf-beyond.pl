# THIS CONFIG WAS AUTOMATICALLY CREATED FROM TEMPLATE CONFIG FILE TO OPEN A MIDI FILE
# YOU CAN EDIT THIS CONFIG FILE TO OPEN THIS FILE AGAIN
# If you want to change template config file, that is copied each time you open a MIDI file, please go to configs folder in your File browser
# Template configs are not accessible from MGen GUI, you will need to change it outside of the program
# Template config was created by removing Midi_file parameter from source config

# Analysis
corrections = 1 # Number of corrections to show. Set to 0 to disable corrections
pre_bad = 6 # How many notes to recalculate before rule violation
post_bad = 8 # How many notes to recalculate after rule violation
step_penalty = 3 # Penalty for adding one more changing step while correcting cantus
pitch_penalty = 1 # Penalty for changing note one more diatonic step while correcting cantus
show_transpose = 0,12 # Semitones to transpose each voice (separated with comma)
fullscan_max = 5 # Maximum steps length to full scan. If melody is longer, use SWA
approximations = 10 # Maximum number of approximations to run if penalty decreases
swa_steps = 6 # Size of Sliding Window Approximation algorithm window in steps
correct_range = 4 # Maximum interval allowed between each source and corrected note
random_key = 0 # Select any of possible keys regardless of last note

# Main parameters
Instruments = Violin,Piano,Piano,Piano,Piano,Piano,Piano,Piano,Piano,Piano
s_len = 6 # Maximum number of measures to full scan. 6-7 is recommended. Lower values can create less melodic results. Higher values are slow to compute
min_interval = 4 # Minimum diatonic interval in cantus (7 = octave)
max_interval = 7 # Maximum diatonic interval in cantus (7 = octave)
tempo = 100-130 # Tempo range
random_seed = 0 # Seed melody with random numbers. This ensures giving different results if generation is very slow.
shuffle = 0 # If you want to shuffle all canti after generation (can shuffle up to 32000 canti)

# Rules: 0 - show only without violation, 1 - show with or without violation, 2 - show only violations
# ! means that this rule can produce results without other rules with c_len=9, interval 4-7
# Flag order in list below goes from green - less severe (top) to red - more severe (bottom)
Strict                    = 1 # No violated rules
Prepared unfilled 3rd     = 1 #S 3rd is unfilled, but was prepared by filling or leap before
Prepared unresolved 3rd   = 1 #S! 3rd is unresolved, but was prepared by filling or leap before
Two 3rds after 6/8        = 1 #S Chain of two thirds in one direction after 6th or octave
Late <6th resolution      = 1 #S! Leap less than 6th is resolved after a second note
Leap back <5th            = 1 #S Leap returns to same note (in case leap is equal or less than 4th)
Leap pre-late fill        = 1 #S! Melody has unfilled gaps
Many leaps                = 0 #S! Maximum allowed max_leaps during max_leap_steps
3rd to last is CEG        = 1 #SC! Third to last note is not Subdominant (C E G forbidden)
Last leap                 = 1 #S Last step is a leap
Tritone resolved          = 1 #S Tritone is prepared by E and resolved to C
3 letters in a row        = 1 #L! 3 same letters (TDS) in possible chord names in a row
Unfilled leap             = 0 #U! Leap is not filled
Leap to leap resolution   = 1 #S Resolution of leap with another leap
Two 3rds                  = 0 #U Chain of two thirds in one direction
4 step miss               = 0 #L 4 steps missing one of 3 letters (TDS) in possible chord names in a row
Minor seventh             = 1 #U Minor seventh interval leap
Outstanding repeat        = 0 #U Prohibit repeating of 3 notes closer than repeat_steps between first notes (if beats are same)
Long smooth               = 0 #U Too long smooth movement
Long line                 = 0 #U Too long smooth movement in one direction (linear)
First steps without C     = 0 #L First first_steps_tonic steps do not have C note
Close repeat              = 0 #U Two notes repeat in contact
Stagnation                = 0 #U Stagnation on one note
Multiple culminations     = 0 #U Melody has more than one culmination with the same note
Major seventh             = 0 #U Major seventh interval leap
2nd to last not GBD       = 0 #C Second to last note is not Dominant (C E F A forbidden)
4 letters in a row        = 0 #C 4 same letters (TDS) in possible chord names in a row
>4 letters in a row       = 0 #U More than 4 same letters (TDS) in possible chord names in a row
5 step miss               = 0 #C 5 steps missing one of 3 letters (TDS) in possible chord names in a row
>5 step miss              = 0 #U More than 5 steps missing one of 3 letters (TDS) in possible chord names in a row
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
Too wide range            = 1 #CV If range is greater than max_interval (diatonic interval)
Too tight range           = 0 #CV If range is lower than min_interval (diatonic interval)

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
first_steps_tonic = 3 # Number of first steps, which must contain tonic note

# Technical parameters
midifile_in_mul = 1 # Multiply note length with this value when loading
midifile_out_mul = 8 # Multiply note length with this value when saving
show_severity = 1 # =1 to show severity in square brackets in comments to notes (also when exporting to MIDI file)
calculate_correlation = 0 # Enables correlation calculation algorithm. Slows down generation. Outputs to cf1-cor.csv
calculate_blocking = 0 # Enables blocking flags calculation algorithm. Slows down generation.
calculate_stat = 0 # Enables flag statistics calculation algorithm. Slows down generation.
late_require = 0 # Allow not-last scan window to have no needed tags, but no blocked tags. This calculation usually requires much more time

# This config was created from default config file configs\GenCA1.pl
# Created at 2017-04-16 22-34-26
Midi_file = load\beyond.mid
