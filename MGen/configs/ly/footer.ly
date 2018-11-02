\markup { 
  \vspace #5
  \bold Legend:
} 

\markup {
  \line {
  \vspace #1
    \column {
 \raise #0.3 \teeny \override #`(direction . ,UP) { \dir-column {
        \with-color #(rgb-color 0.2 0.995 0.000) \circle 7
 } }
    \hspace #5
      
 \raise #0.3 \teeny \override #`(direction . ,UP) { \dir-column {
        \with-color #(rgb-color 0.988 0.005 0.000) \underline \circle 8
 } }
    \hspace #5
      %\raise #0.9 \circle \note #"1" #1
      %\raise #0.9 \musicglyph #"noteheads.s2xcircle"
      \raise #0.5 .
      %\raise #0.9 \musicglyph #"noteheads.s0harmonic"
    }
    \hspace #5
    \column {
      "This note violates minor rule (green). If underlined, means that violation"
      "is shown in the score with shape of same color"
      "This note violates major rule (red). If underlined, means that violation"
      "is shown in the score with shape of same color"
      %"This note was corrected"
      %"This note is a dissonance"
      "Staccato note is considered non-harmonic (passing, auxiliary or other)"
    }
  }
} 
