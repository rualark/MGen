# THIS CONFIG WAS AUTOMATICALLY CREATED FROM TEMPLATE CONFIG FILE TO OPEN A MIDI FILE
# YOU CAN EDIT THIS CONFIG FILE TO OPEN THIS FILE AGAIN
# If you want to change template config file, that is copied each time you open a MIDI file, please go to configs folder in your File browser
# Template configs are not accessible from MGen GUI, you will need to change it outside of the program
# Template config was created by removing Midi_file parameter from source config

include "include/CA2.pl"

# This config was created from default config file configs\GenCA2.pl
# Created at 2018-06-29 16:41:46
Midi_file = midi\Counterpoint\known\3voix-20180622-02-altbas.mid

corrections = 1

t_cnt = 100000 # Maximum number of steps generated
midifile_export_marks = 1 # Set to export marks (harmony) to midifile lyrics
midifile_export_comments = 0 # Set to export comments (violations) to midifile lyrics
show_correct_hatch = 1 # Set to 1 to mark corrected notes with hatch

AutoLegato = 0 # Enables auto legato for all instruments, converting touching notes into legato transitions (can be overridden for particular instruments). Usually can be used if you do not use slurs in your score and want all adjacent notes to be legato
AutoNonLegato = 0 # Enables automatic random replacement of legato transitions with nonlegato (frequency is controlled by nonlegato_freq in each instrument). Usually is used with AutoLegato.
unison_mute = 1 # 0 - do not mute unisons; 1 - mute unisons in voices with same instruments
reverb_mix = 30 # Control reverb mix from 0 to 100% (0% is totally dry, 100% is totally wet)

show_min_severity = 49 # Minimum severity to highlight note
species = 0 # Do not override parameters in importing files

harm_notation = 1 # Harmonic notation (see configs/harm/harm-notation.csv for details)
tempo_bell = 1.3 # Increase tempo in the middle of cantus / counterpoint. 0 - preserve source tempo
show_hatch = 0 # 0 - show no hatch, 1 = show dissonance hatch, 2 = show msh hatch
show_harmony_bass = 2 # 0 = do not show bass, 1 = Show harmony bass, 2 = Show chord type (6 or 64)
