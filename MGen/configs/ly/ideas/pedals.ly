\version "2.18.2"
sustainNotes = { c4\sustainOn d\sustainOff 
                 e\sustainOn 
                 f\sustainOff }

\relative c' {
  \sustainNotes 
  \override Staff.PianoPedalBracket.color = #red
  \override Staff.PianoPedalBracket.thickness = #2
  \override Staff.PianoPedalBracket.bound-details.left.text = "1"
  \set Staff.pedalSustainStrings = #'("P" "P-" "-")
  \set Staff.pedalSustainStyle = #'bracket
  \sustainNotes 
  \set Staff.pedalSustainStrings = #'("d" "de" "e")
  \sustainNotes 
  \set Staff.pedalSustainStrings = #'("M" "M-" "-")
  \sustainNotes 
  \set Staff.pedalSustainStrings = #'("Ped" "*Ped" "*")
  \sustainNotes 
}