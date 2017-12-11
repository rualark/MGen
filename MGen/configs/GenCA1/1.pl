# THIS CONFIG WAS AUTOMATICALLY CREATED FROM TEMPLATE CONFIG FILE
# YOU CAN EDIT THIS CONFIG FILE
# If you want to change template config file, that is copied for each new job, please go to server/share/configs folder

include "include/CA1.pl"

method = swa # This method is best for analysis and corrections in most cases
max_correct_ms = 10000 # Maximum time in milliseconds to correct using window-scan (set to 0 to scan up to the end)
corrections = 0 # Number of corrections to show. Set to 0 to disable corrections. Does not work for swa.

t_cnt = 100000 # Maximum number of steps generated
midifile_export_marks = 1 # Set to export marks (harmony) to midifile lyrics
midifile_export_comments = 0 # Set to export comments (violations) to midifile lyrics
show_correct_hatch = 1 # Set to 1 to mark corrected notes with hatch

Midi_file = server\midi\1.mid
corrections = 1 # Enable correcting score
