include "include/CA2.pl"

# Analysis
Midi_file = "music\Counterpoint 2 voices\good\good-cp1.mid" # Path and file name of imported midi file


max_correct_ms = 2000 # Maximum time in milliseconds to correct using window-scan (set to 0 to scan up to the end)
animate = 0 # Draw animation of preliminary result every X ms (0 to show each change, high to disable animation)
animate_delay = 0 # Delay in ms after each animation step
