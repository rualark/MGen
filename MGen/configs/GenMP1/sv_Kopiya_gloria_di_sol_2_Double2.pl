# THIS CONFIG WAS AUTOMATICALLY CREATED FROM TEMPLATE CONFIG FILE
# YOU CAN EDIT THIS CONFIG FILE
# If you want to change template config file, that is copied for each new job, please go to server/share/configs folder

midifile_in_mul = 16 # How much you want to decrease minimum step length
unison_mute = 1 # 0 - do not mute unisons; 1 - mute unisons in voices with same instruments
reverb_mix = 30 # Control reverb mix from 0 to 100% (0% is totally dry, 100% is totally wet)

Midi_file = "server\cache\Kopiya_gloria_di_sol_2_Double2.mid"
Instruments = "Flute,Oboe,Clarinet,Trumpet,Horn,Trombone,Tuba,Violin,Violin,Viola,Cello,Contrabass"
MidiFileType = "Sibelius"
AutoLegato = "1"
AutoNonLegato = "1"
"Oboe/Normal/2": dyn_range = "19-100"
"Oboe/Normal/2": reverb_mix = "10"

# New version after implementing volume analysis
"Flute/Normal/1": Volume = "87"
"Oboe/Normal/2": Volume = "130"
"Clarinet/Normal/3": Volume = "127"
"Trumpet/Normal/4": Volume = "130"
"Horn/Normal/5": Volume = "112"
"Trombone/Normal/6": Volume = "130"
"Tuba/Normal/7": Volume = "130"
"Violin/Arco/8": Volume = "91"
"Violin/Arco/9": Volume = "82"
"Viola/Arco/10": Volume = "118"
"Cello/Arco/11": Volume = "100"
"Contrabass/Arco/12": Volume = "80"

# Automatic volume