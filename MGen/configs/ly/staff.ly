  \accidentalStyle modern-cautionary
  \time 4/4
  % \set Score.barNumberVisibility = #(every-nth-bar-number-visible 4)
  % \override Score.BarNumber.break-visibility = ##(#f #t #t)
  \new Voice \with {
    \remove "Note_heads_engraver"
    \consists "Completion_heads_engraver"
    \remove "Rest_engraver"
    \consists "Completion_rest_engraver"
    \override Glissando.minimum-length = #5
    \override Glissando.springs-and-rods = #ly:spanner::set-spacing-rods
    \override Glissando.thickness = #'3
    \override Slur.line-thickness = #2
    \override PhrasingSlur.line-thickness = #2
    \phrasingSlurDown \phrasingSlurDashed
  }
