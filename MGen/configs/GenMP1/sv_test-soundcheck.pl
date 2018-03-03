# THIS CONFIG WAS AUTOMATICALLY CREATED FROM TEMPLATE CONFIG FILE
# YOU CAN EDIT THIS CONFIG FILE
# If you want to change template config file, that is copied for each new job, please go to server/share/configs folder

midifile_in_mul = 16 # How much you want to decrease minimum step length
AutoLegato = 0 # Enables auto legato for all instruments, converting touching notes into legato transitions (can be overridden for particular instruments). Usually can be used if you do not use slurs in your score and want all adjacent notes to be legato
AutoNonLegato = 0 # Enables automatic random replacement of legato transitions with nonlegato (frequency is controlled by nonlegato_freq in each instrument). Usually is used with AutoLegato.
unison_mute = 1 # 0 - do not mute unisons; 1 - mute unisons in voices with same instruments

Midi_file = "server\cache\test-soundcheck.mid"
MidiFileType = "Sibelius"
# New job = "17"
Instruments = "Piano/Hardest"
# Example result = "Dyn0"
"Piano/Hardest/1": dyn_range = "0-0"
