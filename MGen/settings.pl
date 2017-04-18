# Settings of MGen
# This file is loaded on MGen startup and automatically saved on every setting change

Algorithm = 101 # Id of the currently selected algorithm
Config = Test # Name of configuration file for selected algorithm
MIDI_OUT = loopMIDI Port, MMSystem # Name of MIDI device used for playing notes
Horizontal_zoom = 150 # Zoom of the piano roll. Can be from 80 to 500
playback_speed = 100 # Playback speed in percent

# The following settings cannot be changed in GUI. You can change them only in this file
View_timer = 100 # ms between each screen update during generation and playback. 100 ms is recommended. Increase for slower computers
Step_dyn = 1 # Show dynamics with note opacity for each step of note.Disable for slower computers.
Debug_level = 2 # Increase to show more debug logs
Playback_enabled = 1 # Disable playback to MIDI port by setting this to 0
