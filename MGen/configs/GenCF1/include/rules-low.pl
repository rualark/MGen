# Rules: 0 - show only without violation, 1 - show with or without violation, 2 - show only violations
# ! means that this rule can produce results without other rules with c_len=9, interval 4-7
# Flag order in list below goes from green - less severe (top) to red - more severe (bottom)
# Groups:
# S - strict rules that can usually be violated
# U - universal for all melodies
# C - rules specifially for cantus firmus
# L - rules specifially for lower cantus firmus
# H - rules specifially for higher cantus firmus
# V - rules appropriate for vocal melody

Strict                     = 1 # No violated rules
3 letters in a row [V]     = X #SC 3 same letters (TDS) in possible chord names in a row (variants)
4 step miss [C]            = X #SC 4 steps missing one of 3 letters (TDS) in possible chord names in a row (constant)
4 letters in a row [V]     = X #SC 4 same letters (TDS) in possible chord names in a row (variants)
>4 letters in a row [V]    = X #SC More than 4 same letters (TDS) in possible chord names in a row (variants)
5 step miss [C]            = X #SC 5 steps missing one of 3 letters (TDS) in possible chord names in a row (constant)
>5 step miss [C]           = X #SC More than 5 steps missing one of 3 letters (TDS) in possible chord names in a row (constant)
"3rd to last is CEG"       = 1 #SC! Third to last note is not Subdominant (C E G forbidden)
Preleaped unfilled 3rd     = 0 #S 3rd is unfilled, but was prepared by leap before
Preleaped unresolved 3rd   = 0 #S! 3rd is unresolved, but was prepared by leap before
"Prefilled unresolved 3rd" = 0 #S 3rd is not resolved by backward movement, but it was filled before
"Prefilled unfilled 3rd"   = 0 #S 3rd is not filled, but it was filled before
Late <6th resolution       = 0 #S! Leap less than 6th is resolved after a second note
Leap back <7th             = 0 #S Leap returns to same note (in case leap is equal or less than 4th)
Last leap                  = 0 #S Last step is a leap
"Consecutive leaps"        = 0 #S More than cse_leaps consecutive leaps
Many leaps                 = 0 #S! Maximum allowed max_leaps during max_leap_steps
Two 3rds after 6/8         = 0 #S Chain of two thirds in one direction after 6th or octave
3 letters in a row [C]     = 0 #C! 3 same letters (TDS) in possible chord names in a row (constant)
Tritone resolved           = 0 #S Tritone is prepared by E and resolved to C
"Leap back overflow"       = 0 #S Leap back is going farther than starting note of a previous leap
"Prefilled unresolved 4th" = 0 #S 4th is not resolved by backward movement, but it was filled before
"Prefilled unfilled 4th"   = 0 #S 4th is not filled, but it was filled before
4 step miss [V]            = 0 #L 4 steps missing one of 3 letters (TDS) in possible chord names in a row (variants)
"First not C"              = 0 #C First note is not tonic
Minor seventh              = 0 #U Minor seventh interval leap
Outstanding repeat         = 0 #U Prohibit repeating of 3 notes closer than repeat_steps between first notes (if beats are same)
"Preleaped unfilled 4th"   = 0 #S 4th is not filled, but it has a greater leap as neighbour
"Preleaped unresolved 4th" = 0 #U 4th is not resolved by backward movement, but it has a greater leap as neighbour
Prefilled unfilled >4th    = 0 #S Leap longer than 4th is not filled, but it was filled before
Prefilled unresolved >4th  = 0 #U Leap longer than 4th is not resolved by backward movement, but it was filled before
Preleaped unfilled >4th    = 0 #S Leap longer than 4th is not filled, but it has a greater leap as neighbour
Early-late filled 3rd      = 0 #U 3rd is not filled or prefilled, but it is filled somewhere else in the melody
Early-late filled 4th      = 0 #U 4th is not filled or prefilled, but it is filled somewhere else in the melody
Early-late filled >4th     = 0 #S! Leap longer than 4th is not filled or prefilled, but it is filled somewhere else in the melody
"Unresolved 3rd"           = 0 #U 3rd is not resolved by backward movement
Unfilled 3rd               = 0 #U 3rd is not filled
Unfilled 4th               = 0 #U 4th is not filled
Unfilled leap >4th         = 0 #U! Leap is not filled
Late culmination           = 0 #L Culmination at last three steps
Long smooth                = 0 #U Too long smooth movement
Long line                  = 0 #U Too long smooth movement in one direction (linear)
First steps without C      = 0 #L First first_steps_tonic (see comment) steps do not have C note
First steps without E      = 0 #H First first_steps_tonic (see comment) steps do not have E note
"2nd to last not GBD"      = 0 #C Second to last note is not GBD (C E F A forbidden)
"2nd to last is G"         = 0 #L Second to last note is G (G forbidden)
4 letters in a row [C]     = 0 #C 4 same letters (TDS) in possible chord names in a row (constant)
5 step miss [V]            = 0 #C 5 steps missing one of 3 letters (TDS) in possible chord names in a row (variants)
"Unresolved 4th"           = 0 #U 4th is not resolved by backward movement
Late >5th resolution       = 0 #U Leap greater than 5th is resolved after a second note
Leap back >6th             = 0 #U Leap returns to same note (in case leap is larger than 4th)
Many leaps+                = 0 #U Maximum allowed max_leaps2 during max_leap_steps
Too wide range             = 0 #CV If range is greater than max_interval (diatonic interval)
Too tight range            = 0 #CV If range is lower than min_interval (diatonic interval)
Two 3rds                   = 0 #U Chain of two thirds in one direction
Close repeat               = 0 #U Two notes repeat in contact
Long repeat                = 0 #C 5 notes repeat 
Stagnation                 = 0 #U Stagnation on one note
Multiple culminations      = 0 #U Melody has more than one culmination with the same note
"3rd to last is leading"   = 0 #C Third to last note is leading (B forbidden)
"Start tonic unprepared"   = 0 #C If tonic is not the very first note, it has to be prepared with BD or GBD notes
"Last not C"               = 0 #C Last note is not tonic
"Consecutive leaps+"       = 0 #U More than cse_leaps2 consecutive leaps
>4 letters in a row [C]    = 0 #U More than 4 same letters (TDS) in possible chord names in a row (constant)
>5 step miss [V]           = 0 #U More than 5 steps missing one of 3 letters (TDS) in possible chord names in a row (variants)
Unfinished fill            = 0 #U Fill does not have last or prelast note (this flag is set only if not blocking)
Major seventh              = 0 #U Major seventh interval leap
Preleaped unresolved >4th  = 0 #U Leap longer than 4th is not resolved by backward movement, but it has a greater leap as neighbour
"G-C before cadence"       = 0 #L G note goes to C note before final cadence
Leap unresolved            = 0 #U Leap unresolved during two next steps (movement direction does not change)
Leap chain                 = 0 #U Chain of two leaps in one direction
Tritone unresolved         = 0 #U Tritone is not resolved correctly
Tritone culmination        = 0 #U Tritone is the highest leap

