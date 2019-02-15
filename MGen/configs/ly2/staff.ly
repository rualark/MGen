  \override Score.VoltaBracketSpanner #'outside-staff-priority = 100
  \override Score.VoltaBracketSpanner #'direction = -1
  \override Score.VoltaBracket #'edge-height = #'(0.0 . 0.0)
  \override Score.VoltaBracket #'font-name = #"New Century Schoolbook" 
  \override Score.VoltaBracket #'font-shape = #'bold 
  \override Score.VoltaBracket.thickness = #3
  \override Score.Glissando.breakable = ##t
  \override Score.Glissando.after-line-breaking = ##t
  \override Staff.OttavaBracket.thickness = #2
  \override Staff.PianoPedalBracket.thickness = #2
  \set Staff.pedalSustainStyle = #'mixed
  \accidentalStyle modern-cautionary
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
    \override Glissando.details.glissando-text-font-size = -3
    \override Glissando.details.glissando-text-padding = 0.1
    %\override Glissando.font-shape = #'italic
    \override Glissando.font-series = #'bold
    \override Glissando.bound-details.right.padding = 0.2
    \override Glissando.bound-details.right-broken.padding = 0.2
    \override Glissando.bound-details.left.padding = 0.2
    \override Glissando.bound-details.left-broken.padding = 0.2
    \override Slur.line-thickness = #2
    \slurDashed
    \slurDashPattern #0.5 #0.75
    \override PhrasingSlur.line-thickness = #2
    \phrasingSlurDown \phrasingSlurDashed
    \consists "Horizontal_bracket_engraver"
    \override HorizontalBracket.thickness=3
    \override HorizontalBracket.#'bracket-flare = #'(0 . 0)
    \override HorizontalBracket.#'edge-height = #'(0.5 . 0.5)
    \override HorizontalBracket #'shorten-pair = #'(-0.3 . -0.3) 
    \override TrillSpanner.bound-details.left.text = ##f
  }
