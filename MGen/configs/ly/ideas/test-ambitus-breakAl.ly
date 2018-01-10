\version "2.18.2"

\score {
  \relative {
    \override Score.BreakAlignment #'break-align-orders =
    #(make-vector 3 '(left-edge breathing-sign clef key-cancellation key-signature ambitus time-signature staff-bar cue-clef custos))
    \override Staff.KeySignature.space-alist =
    #'((ambitus extra-space . 1) (time-signature extra-space . 1.15))
    \override Staff.Ambitus.space-alist =
    #'((key-signature extra-space . 1)
       (time-signature extra-space . 1.15)
       (first-note fixed-space . 2.5))
    \time 3/8 \key c \minor
    es'8 g c
  }
  \layout {
    \context { \Staff
        \consists "Ambitus_engraver" }
  }
}