# Settings of MGen
# This file is loaded on MGen startup and automatically saved on every setting change

Algorithm = 101 # Id of the currently selected algorithm
Config = Test # Name of configuration file for selected algorithm
MIDI_OUT = [DISABLE PLAYBACK] # Name of MIDI device used for playing notes
Horizontal_zoom = 300 # Zoom of the piano roll. Can be from 1 to 500
playback_speed = 100 # Playback speed in percent
show_marks = 1 # Set to 1 to show note mark strings
show_comments = 1 # Set to 1 to surround notes which have comments with rectangles
show_lining = 1 # Set to 1 to show note lining
show_lines = 1 # Set to 1 to show line separators
show_vel = 1 # Set to 1 to show note velocity using alpha
show_curve = 1 # Set to 1 to show melody curves (MeloCurve)
show_tempo = 0 # Set to 1 to show tempo
show_notecolors = 1 # Set to 1 to show note colors instead of instrument colors

# The following settings cannot be changed in GUI. You can change them only in this file
View_timer = 100 # ms between each screen update during generation and playback. 100 ms is recommended. Increase for slower computers
Step_dyn = 1 # Show dynamics with note opacity for each step of note. Disable for slower computers.
Debug_level = 1 # Increase to show more debug logs
