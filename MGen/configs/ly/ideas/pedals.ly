sustainNotes = { c4\sustainOn d e\sustainOff\sustainOn f\sustainOff }

\relative c' {
  \sustainNotes 
  \set Staff.pedalSustainStrings = #'("P" "P-" "-")
  \sustainNotes 
  \set Staff.pedalSustainStrings = #'("d" "de" "e")
  \sustainNotes 
  \set Staff.pedalSustainStrings = #'("M" "M-" "-")
  \sustainNotes 
  \set Staff.pedalSustainStrings = #'("Ped" "*Ped" "*")
  \sustainNotes 
}