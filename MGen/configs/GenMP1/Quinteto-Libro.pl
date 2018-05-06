# THIS CONFIG WAS AUTOMATICALLY CREATED FROM DEFAULT CONFIG FILE TO OPEN A MIDI FILE
# YOU CAN EDIT THIS CONFIG FILE TO OPEN THIS FILE AGAIN
# If you want to change default config file, that is copied each time you open a MIDI file, please go to configs folder in your File browser
# Default configs are not accessible from MGen GUI, you will need to change it outside of the program

# This config was created from default config file configs\GenMP1.pl
# Created at 2018-05-06 19:41:08
Midi_file = midi\Music\private\Emili Gil\Quinteto-Libro.mid

midifile_in_mul = 16 # How much you want to decrease minimum step length
AutoLegato = 0 # Enables auto legato for all instruments, converting touching notes into legato transitions (can be overridden for particular instruments). Usually can be used if you do not use slurs in your score and want all adjacent notes to be legato
AutoNonLegato = 0 # Enables automatic random replacement of legato transitions with nonlegato (frequency is controlled by nonlegato_freq in each instrument). Usually is used with AutoLegato.
unison_mute = 1 # 0 - do not mute unisons; 1 - mute unisons in voices with same instruments
reverb_mix = 30 # Control reverb mix from 0 to 100% (0% is totally dry, 100% is totally wet)

Instruments = "Violin,Violin,Viola,Cello,Contrabass"
MidiFileType = "MuseScore"