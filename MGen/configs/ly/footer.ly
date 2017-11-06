\markup { 
  \column {
    \vspace #2
    \line \bold { 
      Legend:
    } 
    \vspace #0.3
    \line { 
      \tiny \with-color #(rgb-color 0.000 0.767 0.000) \char ##x2716
      "    This note violates minor rule"
    } 
    \vspace #0.4
    \line { 
      \tiny \with-color #(rgb-color 0.867 0.0 0.000) \char ##x2716
      "    This note violates major rule"
    } 
    \vspace #0.4
    \line { 
      \circle \note #"1" #1
      \lower #0.6 "   This note was corrected"
    } 
    \vspace #0.3
    \line { 
      \musicglyph #"noteheads.s2xcircle"
      \lower #0.6 "    This note is a dissonance"
    } 
    \vspace #0.5
    \line { 
      \musicglyph #"noteheads.s0harmonic"
      \lower #0.6 "    This note has negative rpos (can be passing or auxiliary)"
    } 
    \vspace #0.5
    \line { 
      \char ##x00D0
      "    This letter is used for major Dominant chord to distinguish from D note"
    } 
    \vspace #0.3
    \line { 
      \char ##x0111
      "    This letter is used for minor Dominant chord to distinguish from D note"
    } 
    \vspace #0.3
  }
} 
