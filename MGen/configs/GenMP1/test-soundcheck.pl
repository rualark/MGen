midifile_in_mul = 16 # How much you want to decrease minimum step length
AutoLegato = 0 # Enables auto legato for all instruments, converting touching notes into legato transitions (can be overridden for particular instruments). Usually can be used if you do not use slurs in your score and want all adjacent notes to be legato
AutoNonLegato = 0 # Enables automatic random replacement of legato transitions with nonlegato (frequency is controlled by nonlegato_freq in each instrument). Usually is used with AutoLegato.
unison_mute = 1 # 0 - do not mute unisons; 1 - mute unisons in voices with same instruments

Midi_file = music\Music\test\test-soundcheck.mid
MidiFileType = "Sibelius"
Instruments = "Flute"
# Example result = "Dyn0"
#"Violin/Arco/1": dyn_range = "60-60"
"Violin/Arco/1": rnd_dyn = 70
"Violin/Arco/1": rnd_dyn_slow = 4
"Violin/Arco/1": rbell_freq = 0
"Flute/Normal/1": rnd_dyn = 70
"Flute/Normal/1": rnd_dyn_slow = 4
"Flute/Normal/1": rbell_freq = 0
"Piano/Hard": rand_pos = 1-70
"Piano/Hard": rand_pos_max = 1000-1000
