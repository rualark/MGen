  \accidentalStyle modern-cautionary
  \time 4/4
  % \set Score.barNumberVisibility = #(every-nth-bar-number-visible 4)
  % \override Score.BarNumber.break-visibility = ##(#f #t #t)
  \new Voice \with {
    \remove "Note_heads_engraver"
    \consists "Completion_heads_engraver"
    \remove "Rest_engraver"
    \consists "Completion_rest_engraver"
    \consists "Horizontal_bracket_engraver"
    \override HorizontalBracket.thickness=3
    \override HorizontalBracket.#'bracket-flare = #'(0 . 0)
    \override HorizontalBracket.#'edge-height = #'(0.5 . 0.5)
    \override HorizontalBracket #'shorten-pair = #'(-0.3 . -0.3) 
    \override Glissando.thickness = #'3
  }
