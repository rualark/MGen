\version "2.18.2"
\language "english"
\paper { 
  #(include-special-characters) 
  bottom-margin = 0.27\in
}
myTS =
#(define-music-function (parser location st color) (string? color?)
  #{
    \override TextSpanner.style = #'dashed-line
      \override TextSpanner.dash-fraction = #0.3
  \override TextSpanner.dash-period = #1
    \override TextSpanner.font-size = #-3
    \override TextSpanner.bound-details.left.stencil-align-dir-y = #CENTER
    \override TextSpanner.bound-details.left.text = #st
    \override TextSpanner.color = #color
    \override TextSpanner.thickness = #2
  #})

\markup {
      \vspace #1

}
\markup {
      \vspace #1

}
\markup \wordwrap {
  \bold {
"#"1 (from test-vocal-range) Key: C major
}
\tiny { 
"Lyrics:" "sp011111133."
}

}
<<
\new Staff = "staff8" {
     
  \set Staff.instrumentName = \markup { \teeny \override #'(baseline-skip . 2.0) \center-column{ "Сопра"  "[species 3]"  "[Soprano]" } }
  \clef "treble"
  \key c
  \major
  \time 4/4
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

  { \myTS "Long stepwise" #(rgb-color 0.706 0.706 0.000) \textSpannerDown
d'4
\startTextSpan
e'4
f'4
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
g'4
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
a'4
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
b'4

\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
c''4

d''4
e''4

f''4
g''4
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)

a''4
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f

g''4
f''4

e''4
\stopTextSpan
\myTS "all similar" #(rgb-color 0.706 0.706 0.000) \textSpannerDown

d''4
\startTextSpan
c''1
\stopTextSpan

  }
}
  \new Lyrics \with { alignAboveContext = "staff8" } {
    \lyricmode {
      \override StanzaNumber.font-size = #-2
      \set stanza = #" Flags:"
      \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000)  1
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 2
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000)  3
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000)  4
      } } }8
 \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 5
      } } }8
 \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 6
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8     }
  }
\new Staff = "staff7" {
     
  \set Staff.instrumentName = \markup { \teeny \override #'(baseline-skip . 2.0) \center-column{ "Сопра"  "[species 3]"  "[Soprano]" } }
  \clef "treble"
  \key c
  \major
  \time 4/4
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

  { \myTS "Long stepwise" #(rgb-color 0.706 0.706 0.000) \textSpannerDown
a''4
\startTextSpan

g''4

f''4

e''4

d''4
\override Staff.PianoPedalBracket.color = #(rgb-color 0.706 0.706 0.000)
 \override Staff.SustainPedal #'stencil =
 #(lambda (grob) (grob-interpret-markup grob (markup
 #:with-color (rgb-color 0.706 0.706 0.000)
 #:lower 0.4
 (#:teeny "2 x 2nd"))))
 \textSpannerDown
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
c''4
\sustainOn

\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
b'4
\sustainOff
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)

a'4
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
g'4

f'4

e'4
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)

d'4
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
c'4

\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
d'4
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)

e'4
\stopTextSpan
\glissando
 \override NoteColumn.glissando-skip = ##t
f'4
\override NoteColumn.glissando-skip = ##f
d'1

  }
}
  \new Lyrics \with { alignAboveContext = "staff7" } {
    \lyricmode {
      \override StanzaNumber.font-size = #-2
      \set stanza = #" Flags:"
      \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000)  7
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000)  8
      } } }8
 \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 9
      } } }8
 \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000)  10
      } } }8
 \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 11
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 12
      } } }8
 \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 13
      } } }8
 \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 14
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8     }
  }
\new Staff = "staff6" {
     
  \set Staff.instrumentName = \markup { \teeny \override #'(baseline-skip . 2.0) \center-column{ "Сопра"  "[species 1]"  "[Alto]" } }
  \clef "treble"
  \key c
  \major
  \time 4/4
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

  { \override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\override BreathingSign.color = #(rgb-color 1.000 0.000 0.000)
 
\override TrillSpanner.bound-details.left.text = \markup{ \raise #0.6 \teeny "Stagnation" }
  \override TrillSpanner.color = #(rgb-color 1.000 0.000 0.000)
d'1
\startTrillSpan
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
e'1
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
d'1
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
e'1
\glissando
 \override NoteColumn.glissando-skip = ##t
\override BreathingSign.color = #(rgb-color 1.000 0.000 0.000) 
\override NoteColumn.glissando-skip = ##f
d'1
\stopTrillSpan

  }
}
  \new Lyrics \with { alignAboveContext = "staff6" } {
    \lyricmode {
      \override StanzaNumber.font-size = #-2
      \set stanza = #" Flags:"
      \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 16
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 15
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 18
        \with-color #(rgb-color 1.000 0.000 0.000)  17
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 19
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 20
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000)  23
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 22
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 21
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8     }
  }
\new Staff = "staff5" {
     
  \set Staff.instrumentName = \markup { \teeny \override #'(baseline-skip . 2.0) \center-column{ "Сопра"  "[species 1]"  "[Alto]" } }
  \clef "treble"
  \key c
  \major
  \time 4/4
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

  { \override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\override BreathingSign.color = #(rgb-color 1.000 0.000 0.000)
 
\override TrillSpanner.bound-details.left.text = \markup{ \raise #0.6 \teeny "Stagnation" }
  \override TrillSpanner.color = #(rgb-color 1.000 0.000 0.000)
d'1
\startTrillSpan
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
e'1
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
d'1
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
e'1
\glissando
 \override NoteColumn.glissando-skip = ##t
\override BreathingSign.color = #(rgb-color 1.000 0.000 0.000) 
\override NoteColumn.glissando-skip = ##f
d'1
\stopTrillSpan

  }
}
  \new Lyrics \with { alignAboveContext = "staff5" } {
    \lyricmode {
      \override StanzaNumber.font-size = #-2
      \set stanza = #" Flags:"
      \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 25
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 24
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 28
        \with-color #(rgb-color 1.000 0.000 0.000)  27
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 26
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 30
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 29
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 33
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 32
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 31
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
...
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 35
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 34
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8     }
  }
\new Staff = "staff4" {
     
  \set Staff.instrumentName = \markup { \teeny \override #'(baseline-skip . 2.0) \center-column{ "Сопра"  "[species 1]"  "[Tenor]" } }
  \clef "treble_8"
  \key c
  \major
  \time 4/4
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

  { \override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\override BreathingSign.color = #(rgb-color 1.000 0.000 0.000)
 
\override TrillSpanner.bound-details.left.text = \markup{ \raise #0.6 \teeny "Stagnation" }
  \override TrillSpanner.color = #(rgb-color 1.000 0.000 0.000)
\myTS "overlap" #(rgb-color 1.000 0.000 0.000) \textSpannerDown
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
g1
\startTrillSpan
\startTextSpan
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\myTS "overlap" #(rgb-color 1.000 0.000 0.000) \textSpannerDown
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
a1
\startTextSpan
\stopTextSpan
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\myTS "overlap" #(rgb-color 1.000 0.000 0.000) \textSpannerDown
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
g1
\startTextSpan
\stopTextSpan
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\myTS "overlap" #(rgb-color 1.000 0.000 0.000) \textSpannerDown
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
a1
\startTextSpan
\stopTextSpan
\glissando
 \override NoteColumn.glissando-skip = ##t
\override BreathingSign.color = #(rgb-color 1.000 0.000 0.000) 
\override NoteColumn.glissando-skip = ##f
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
g1
\stopTextSpan
\stopTrillSpan

  }
}
  \new Lyrics \with { alignAboveContext = "staff4" } {
    \lyricmode {
      \override StanzaNumber.font-size = #-2
      \set stanza = #" Flags:"
      \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 39
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 38
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
...
        \with-color #(rgb-color 1.000 0.000 0.000)  41
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 40
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 46
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 45
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 44
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 49
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 48
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 47
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
...
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 51
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 50
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8     }
  }
\new Staff = "staff3" {
     
  \set Staff.instrumentName = \markup { \teeny \override #'(baseline-skip . 2.0) \center-column{ "SOPRANO2"  "[species 1]"  "[Tenor]" } }
  \clef "treble_8"
  \key c
  \major
  \time 4/4
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

  { \override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\override BreathingSign.color = #(rgb-color 1.000 0.000 0.000)
 
\override TrillSpanner.bound-details.left.text = \markup{ \raise #0.6 \teeny "Stagnation" }
  \override TrillSpanner.color = #(rgb-color 1.000 0.000 0.000)
\myTS "overlap" #(rgb-color 1.000 0.000 0.000) \textSpannerDown
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
g1
\startTrillSpan
\startTextSpan
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\myTS "overlap" #(rgb-color 1.000 0.000 0.000) \textSpannerDown
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
a1
\startTextSpan
\stopTextSpan
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\myTS "overlap" #(rgb-color 1.000 0.000 0.000) \textSpannerDown
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
g1
\startTextSpan
\stopTextSpan
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\myTS "overlap" #(rgb-color 1.000 0.000 0.000) \textSpannerDown
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
a1
\startTextSpan
\stopTextSpan
\glissando
 \override NoteColumn.glissando-skip = ##t
\override BreathingSign.color = #(rgb-color 1.000 0.000 0.000) 
\override NoteColumn.glissando-skip = ##f
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
g1
\stopTextSpan
\stopTrillSpan

  }
}
  \new Lyrics \with { alignAboveContext = "staff3" } {
    \lyricmode {
      \override StanzaNumber.font-size = #-2
      \set stanza = #" Flags:"
      \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 55
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 54
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
...
        \with-color #(rgb-color 1.000 0.000 0.000)  57
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 56
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 62
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 61
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 60
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 65
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 64
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 63
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
...
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 67
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 66
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8     }
  }
\new Staff = "staff2" {
     
  \set Staff.instrumentName = \markup { \teeny \override #'(baseline-skip . 2.0) \center-column{ "ALTO2"  "[species 1]"  "[Bass]" } }
  \clef "bass"
  \key c
  \major
  \time 4/4
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

  { \override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\override BreathingSign.color = #(rgb-color 1.000 0.000 0.000)
 
\override TrillSpanner.bound-details.left.text = \markup{ \raise #0.6 \teeny "Stagnation" }
  \override TrillSpanner.color = #(rgb-color 1.000 0.000 0.000)
\myTS "overlap" #(rgb-color 1.000 0.000 0.000) \textSpannerDown
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
g1
\startTrillSpan
\startTextSpan
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\myTS "overlap" #(rgb-color 1.000 0.000 0.000) \textSpannerDown
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
a1
\startTextSpan
\stopTextSpan
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\myTS "overlap" #(rgb-color 1.000 0.000 0.000) \textSpannerDown
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
g1
\startTextSpan
\stopTextSpan
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\myTS "overlap" #(rgb-color 1.000 0.000 0.000) \textSpannerDown
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
a1
\startTextSpan
\stopTextSpan
\glissando
 \override NoteColumn.glissando-skip = ##t
\override BreathingSign.color = #(rgb-color 1.000 0.000 0.000) 
\override NoteColumn.glissando-skip = ##f
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
g1
\stopTextSpan
\stopTrillSpan

  }
}
  \new Lyrics \with { alignAboveContext = "staff2" } {
    \lyricmode {
      \override StanzaNumber.font-size = #-2
      \set stanza = #" Flags:"
      \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 71
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 70
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
...
        \with-color #(rgb-color 1.000 0.000 0.000)  73
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 72
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 78
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 77
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 76
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 81
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 80
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 79
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 84
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 83
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 82
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8     }
  }
\new Staff = "staff1" {
     
  \set Staff.instrumentName = \markup { \teeny \override #'(baseline-skip . 2.0) \center-column{ "TENOR2"  "[species 1]"  "[Bass]" } }
  \clef "bass"
  \key c
  \major
  \time 4/4
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

  { \override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\override BreathingSign.color = #(rgb-color 1.000 0.000 0.000)
 
\override TrillSpanner.bound-details.left.text = \markup{ \raise #0.6 \teeny "Stagnation" }
  \override TrillSpanner.color = #(rgb-color 1.000 0.000 0.000)
\myTS "overlap" #(rgb-color 1.000 0.000 0.000) \textSpannerDown
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
g1
\startTrillSpan
\startTextSpan
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\myTS "overlap" #(rgb-color 1.000 0.000 0.000) \textSpannerDown
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
a1
\startTextSpan
\stopTextSpan
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\myTS "overlap" #(rgb-color 1.000 0.000 0.000) \textSpannerDown
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
g1
\startTextSpan
\stopTextSpan
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\myTS "overlap" #(rgb-color 1.000 0.000 0.000) \textSpannerDown
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
a1
\startTextSpan
\stopTextSpan
\glissando
 \override NoteColumn.glissando-skip = ##t
\override BreathingSign.color = #(rgb-color 1.000 0.000 0.000) 
\override NoteColumn.glissando-skip = ##f
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
g1
\stopTextSpan
\stopTrillSpan

  }
}
  \new Lyrics \with { alignAboveContext = "staff1" } {
    \lyricmode {
      \override StanzaNumber.font-size = #-2
      \set stanza = #" Flags:"
      \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 86
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 85
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
...
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 88
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 87
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 92
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 91
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 95
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 94
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 93
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 98
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 97
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 96
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8     }
  }
\new Staff = "staff0" {
     
  \set Staff.instrumentName = \markup { \teeny \override #'(baseline-skip . 2.0) \center-column{ "BASS2"  "[c.f.]"  "[Bass]" } }
  \clef "bass"
  \key c
  \major
  \time 4/4
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

  { \override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\override BreathingSign.color = #(rgb-color 1.000 0.000 0.000)
 
\override TrillSpanner.bound-details.left.text = \markup{ \raise #0.6 \teeny "Stagnation" }
  \override TrillSpanner.color = #(rgb-color 1.000 0.000 0.000)
\myTS "overlap" #(rgb-color 1.000 0.000 0.000) \textSpannerDown
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
g1
\startTrillSpan
\startTextSpan
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\myTS "overlap" #(rgb-color 1.000 0.000 0.000) \textSpannerDown
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
a1
\startTextSpan
\stopTextSpan
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
g1
\stopTextSpan
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
a1
\glissando
 \override NoteColumn.glissando-skip = ##t
\override BreathingSign.color = #(rgb-color 1.000 0.000 0.000) 
\override NoteColumn.glissando-skip = ##f
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
g1
\stopTrillSpan

  }
}
  \new Lyrics \with { alignAboveContext = "staff0" } {
    \lyricmode {
      \override StanzaNumber.font-size = #-2
      \set stanza = #" Flags:"
      \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 101
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 100
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 99
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000)  104
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 103
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 102
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 107
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 106
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 105
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 110
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 109
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 108
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 113
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 112
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 111
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8     }
  }
  \new Lyrics \with { alignBelowContext = "staff0" } {
    \lyricmode {
      \override StanzaNumber.font-size = #-2
      \set stanza = #" Harmony:"
      \override InstrumentName #'font-series = #'bold
      \override InstrumentName.font-size = #-2
      \set shortVocalName = "H:"
  \markup{   \teeny 
   \pad-markup #0.4 
\concat { ? } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8   \markup{   \teeny 
  \pad-markup #0.4 
\concat { TSVI } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8   \markup{   \teeny 
     \pad-markup #0.4 
  \concat { 
    \general-align #Y #0.5 {DTIII}
    \teeny
    \override #'(baseline-skip . 1.5) 
    \override #'(line-width . 100)  
    \center-column{ 6 5 } 
  }
}8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8   \markup{   \teeny 
     \pad-markup #0.4 
  \concat { 
    \general-align #Y #0.5 {S}
    \teeny
    \override #'(baseline-skip . 1.5) 
    \override #'(line-width . 100)  
    \center-column{ 6 5 } 
  }
}8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8   \markup{   \teeny 
     \pad-markup #0.4 
\concat { ? } }8
    }
  }
\new Staff = "staffs" \with {
  \time 4/4
  \remove "Bar_engraver"
  \remove "Clef_engraver"
  \remove "Time_signature_engraver"
  \override Score.VoltaBracketSpanner #'outside-staff-priority = 100
  \override Score.VoltaBracketSpanner #'direction = -1
  \override Score.VoltaBracket #'edge-height = #'(0.0 . 0.0)
  \override Score.VoltaBracket #'font-name = #"New Century Schoolbook" 
  \override Score.VoltaBracket #'font-shape = #'bold 
  \override Score.VoltaBracket.thickness = #3
  \override OttavaBracket.thickness = #2
  \override PianoPedalBracket.thickness = #2
  \override StaffSymbol.line-count = #0
  }
  \new Voice \with {
    \remove "Rest_engraver"
    \remove "Beam_engraver"
    \remove "Note_heads_engraver"
    \override Glissando.minimum-length = #5
    \override Glissando.springs-and-rods = #ly:spanner::set-spacing-rods
    \override Glissando.thickness = #'3
    \override Slur.line-thickness = #2
    \override PhrasingSlur.line-thickness = #2
    \phrasingSlurDown \phrasingSlurDashed
    \consists "Horizontal_bracket_engraver"
    \override HorizontalBracket.thickness=3
    \override HorizontalBracket.#'bracket-flare = #'(0 . 0)
    \override HorizontalBracket.#'edge-height = #'(0.5 . 0.5)
    \override HorizontalBracket #'shorten-pair = #'(-0.3 . -0.3) 
    \override TrillSpanner.bound-details.left.text = ##f
  }

  { 
  \set Staff.pedalSustainStyle = #'mixed
r8
r8
r8
r8
r8
r8
r8
r8
r8
r8
r8
r8
r8
r8
r8
r8
r8
r8
r8
r8
r8
r8
r8
r8
\override Staff.PianoPedalBracket.color = #(rgb-color 1.000 0.000 0.000)
 \override Staff.SustainPedal #'stencil =
 #(lambda (grob) (grob-interpret-markup grob (markup
 #:with-color (rgb-color 1.000 0.000 0.000)
 #:lower 0.4
 (#:teeny "cross"))))
 \textSpannerDown
r8
\sustainOn
r8
r8
r8
\sustainOff
r8
r8
r8
r8
r8
r8
r8
r8
r8
r8
r8
r8

  }
>>
\markup \wordwrap \tiny \bold {
  Сопрано [bar 1, beat 1] note D
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  1 "-" "Rhythm:" "No" "starting" "rest"

}
\markup \wordwrap \tiny \bold {
  Сопрано [bar 2, beat 1] note A
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  2 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "BASS2"

}
\markup \wordwrap \tiny \bold {
  Сопрано [bar 2, beat 4] note D
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  3 "-" "Melody:" "Abuse" "of" "unidirectional" "stepwise" "motion" "(>7" "in" "one" "direction)"

}
\markup \wordwrap \tiny \bold {
  Сопрано [bar 4, beat 1] note G
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  4 "-" "Harmony:" "Passing" "dissonance" "on" "downbeat"

}
\markup \wordwrap \tiny \bold {
  Сопрано [bar 4, beat 3] note E
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  5 "-" "Melody:" "Abuse" "of" "stepwise" "motion" "(>14" "in" "any" "direction)"

}
\markup \wordwrap \tiny \bold {
  Сопрано [bar 5, beat 1] note C
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  6 "-" "Harmony:" "All" "voices" "move" "in" "same" "direction" "-" "with" "BASS2"

}
\markup \wordwrap \tiny \bold {
  Сопрано (2) [bar 1, beat 1] note A
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  7 "-" "Rhythm:" "No" "starting" "rest"

}
\markup \wordwrap \tiny \bold {
  Сопрано (2) [bar 2, beat 1] note D
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  8 "-" "Harmony:" "Passing" "dissonance" "on" "downbeat"

}
\markup \wordwrap \tiny \bold {
  Сопрано (2) [bar 2, beat 2] note C
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  9 "-" "Harmony:" "Wrong" "voice" "movement" "during" "voice" "crossing" "(2" "x" "2nd)" "-" "with" "Сопра"

}
\markup \wordwrap \tiny \bold {
  Сопрано (2) [bar 2, beat 4] note A
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  10 "-" "Melody:" "Abuse" "of" "unidirectional" "stepwise" "motion" "(>7" "in" "one" "direction)"

}
\markup \wordwrap \tiny \bold {
  Сопрано (2) [bar 3, beat 1] note G
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  11 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "BASS2"

}
\markup \wordwrap \tiny \bold {
  Сопрано (2) [bar 4, beat 1] note C
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  12 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "Сопра"

}
\markup \wordwrap \tiny \bold {
  Сопрано (2) [bar 4, beat 3] note E
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  13 "-" "Melody:" "Abuse" "of" "stepwise" "motion" "(>14" "in" "any" "direction)"

}
\markup \wordwrap \tiny \bold {
  Сопрано (2) [bar 5, beat 1] note D
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  14 "-" "Harmony:" "Parallel" "5ths:" "<" "measure" "apart" "(downbeat)" "-" "with" "BASS2"

}
\markup \wordwrap \tiny \bold {
  Сопрано (3) [bar 1, beat 1] note D
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  15 "-" "Melody:" "Stagnation" "(5" "notes" "<m3)"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  16 "-" "Melody:" "Repetition" "-" "2" "notes" "are" "repeated:" "immediate" "(uniform)"

}
\markup \wordwrap \tiny \bold {
  Сопрано (3) [bar 2, beat 1] note E
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  17 "-" "Melody:" "Repetition" "-" "2" "notes" "are" "repeated:" "immediate" "(uniform)"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  18 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "BASS2"

}
\markup \wordwrap \tiny \bold {
  Сопрано (3) [bar 3, beat 1] note D
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  19 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "BASS2"

}
\markup \wordwrap \tiny \bold {
  Сопрано (3) [bar 4, beat 1] note E
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  20 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "BASS2"

}
\markup \wordwrap \tiny \bold {
  Сопрано (3) [bar 5, beat 1] note D
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  21 "-" "Harmony:" "Parallel" "8ves" "/" "unisons:" "<" "measure" "apart" "(downbeat)" "-" "with" "Сопра"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  22 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "BASS2"

}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  23 "-" "Harmony:" "Similar" "movement" "to" "unison" "(leap" "cadence)" "-" "with" "Сопра"

}
\markup \wordwrap \tiny \bold {
  Сопрано (4) [bar 1, beat 1] note D
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  24 "-" "Melody:" "Stagnation" "(5" "notes" "<m3)"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  25 "-" "Melody:" "Repetition" "-" "2" "notes" "are" "repeated:" "immediate" "(uniform)"

}
\markup \wordwrap \tiny \bold {
  Сопрано (4) [bar 2, beat 1] note E
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  26 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "Сопра"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  27 "-" "Melody:" "Repetition" "-" "2" "notes" "are" "repeated:" "immediate" "(uniform)"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  28 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "BASS2"

}
\markup \wordwrap \tiny \bold {
  Сопрано (4) [bar 3, beat 1] note D
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  29 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "Сопра"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  30 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "BASS2"

}
\markup \wordwrap \tiny \bold {
  Сопрано (4) [bar 4, beat 1] note E
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  31 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "Сопра"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  32 "-" "Harmony:" "Voice" "crossing" "between" "non-adjacent" "voices" "-" "with" "Сопра"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  33 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "BASS2"

}
\markup \wordwrap \tiny \bold {
  Сопрано (4) [bar 5, beat 1] note D
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  34 "-" "Harmony:" "Parallel" "8ves" "/" "unisons:" "<" "measure" "apart" "(downbeat)" "-" "with" "Сопра"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  35 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "Сопра"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  36 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "BASS2"

}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  37 "-" "Harmony:" "Similar" "movement" "to" "unison" "(leap" "cadence)" "-" "with" "Сопра"

}
\markup \wordwrap \tiny \bold {
  Сопрано (5) [bar 1, beat 1] note G
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  38 "-" "Melody:" "Stagnation" "(5" "notes" "<m3)"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  39 "-" "Melody:" "Repetition" "-" "2" "notes" "are" "repeated:" "immediate" "(uniform)"

}
\markup \wordwrap \tiny \bold {
  Сопрано (5) [bar 2, beat 1] note A
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  40 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "Сопра"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  41 "-" "Melody:" "Repetition" "-" "2" "notes" "are" "repeated:" "immediate" "(uniform)"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  42 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "BASS2"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  43 "-" "Harmony:" "Voice" "overlapping" "(non-adjacent)" "-" "with" "BASS2"

}
\markup \wordwrap \tiny \bold {
  Сопрано (5) [bar 3, beat 1] note G
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  44 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "Сопра"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  45 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "BASS2"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  46 "-" "Harmony:" "Voice" "overlapping" "(non-adjacent)" "-" "with" "BASS2"

}
\markup \wordwrap \tiny \bold {
  Сопрано (5) [bar 4, beat 1] note A
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  47 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "Сопра"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  48 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "BASS2"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  49 "-" "Harmony:" "Voice" "overlapping" "(non-adjacent)" "-" "with" "BASS2"

}
\markup \wordwrap \tiny \bold {
  Сопрано (5) [bar 5, beat 1] note G
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  50 "-" "Harmony:" "Parallel" "5ths:" "<" "measure" "apart" "(downbeat)" "-" "with" "Сопра"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  51 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "Сопра"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  52 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "BASS2"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  53 "-" "Harmony:" "Voice" "overlapping" "(non-adjacent)" "-" "with" "BASS2"

}
\markup \wordwrap \tiny \bold {
  SOPRANO2 [bar 1, beat 1] note G
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  54 "-" "Melody:" "Stagnation" "(5" "notes" "<m3)"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  55 "-" "Melody:" "Repetition" "-" "2" "notes" "are" "repeated:" "immediate" "(uniform)"

}
\markup \wordwrap \tiny \bold {
  SOPRANO2 [bar 2, beat 1] note A
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  56 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "Сопра"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  57 "-" "Melody:" "Repetition" "-" "2" "notes" "are" "repeated:" "immediate" "(uniform)"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  58 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "BASS2"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  59 "-" "Harmony:" "Voice" "overlapping" "(non-adjacent)" "-" "with" "BASS2"

}
\markup \wordwrap \tiny \bold {
  SOPRANO2 [bar 3, beat 1] note G
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  60 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "Сопра"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  61 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "BASS2"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  62 "-" "Harmony:" "Voice" "overlapping" "(non-adjacent)" "-" "with" "BASS2"

}
\markup \wordwrap \tiny \bold {
  SOPRANO2 [bar 4, beat 1] note A
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  63 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "Сопра"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  64 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "BASS2"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  65 "-" "Harmony:" "Voice" "overlapping" "(non-adjacent)" "-" "with" "BASS2"

}
\markup \wordwrap \tiny \bold {
  SOPRANO2 [bar 5, beat 1] note G
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  66 "-" "Harmony:" "Parallel" "5ths:" "<" "measure" "apart" "(downbeat)" "-" "with" "Сопра"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  67 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "Сопра"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  68 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "BASS2"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  69 "-" "Harmony:" "Voice" "overlapping" "(non-adjacent)" "-" "with" "BASS2"

}
\markup \wordwrap \tiny \bold {
  ALTO2 [bar 1, beat 1] note G
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  70 "-" "Melody:" "Stagnation" "(5" "notes" "<m3)"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  71 "-" "Melody:" "Repetition" "-" "2" "notes" "are" "repeated:" "immediate" "(uniform)"

}
\markup \wordwrap \tiny \bold {
  ALTO2 [bar 2, beat 1] note A
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  72 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "Сопра"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  73 "-" "Melody:" "Repetition" "-" "2" "notes" "are" "repeated:" "immediate" "(uniform)"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  74 "-" "Harmony:" "Voice" "overlapping" "(non-adjacent)" "-" "with" "Сопра"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  75 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "BASS2"

}
\markup \wordwrap \tiny \bold {
  ALTO2 [bar 3, beat 1] note G
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  76 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "Сопра"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  77 "-" "Harmony:" "Voice" "overlapping" "(non-adjacent)" "-" "with" "Сопра"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  78 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "BASS2"

}
\markup \wordwrap \tiny \bold {
  ALTO2 [bar 4, beat 1] note A
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  79 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "Сопра"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  80 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "BASS2"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  81 "-" "Harmony:" "Voice" "overlapping" "(non-adjacent)" "-" "with" "BASS2"

}
\markup \wordwrap \tiny \bold {
  ALTO2 [bar 5, beat 1] note G
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  82 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "Сопра"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  83 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "BASS2"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  84 "-" "Harmony:" "Voice" "overlapping" "(non-adjacent)" "-" "with" "BASS2"

}
\markup \wordwrap \tiny \bold {
  TENOR2 [bar 1, beat 1] note G
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  85 "-" "Melody:" "Stagnation" "(5" "notes" "<m3)"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  86 "-" "Melody:" "Repetition" "-" "2" "notes" "are" "repeated:" "immediate" "(uniform)"

}
\markup \wordwrap \tiny \bold {
  TENOR2 [bar 2, beat 1] note A
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  87 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "Сопра"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  88 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "ALTO2"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  89 "-" "Melody:" "Repetition" "-" "2" "notes" "are" "repeated:" "immediate" "(uniform)"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  90 "-" "Harmony:" "Voice" "overlapping" "(non-adjacent)" "-" "with" "SOPRANO2"

}
\markup \wordwrap \tiny \bold {
  TENOR2 [bar 3, beat 1] note G
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  91 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "ALTO2"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  92 "-" "Harmony:" "Voice" "overlapping" "(non-adjacent)" "-" "with" "SOPRANO2"

}
\markup \wordwrap \tiny \bold {
  TENOR2 [bar 4, beat 1] note A
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  93 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "Сопра"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  94 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "ALTO2"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  95 "-" "Harmony:" "Voice" "overlapping" "(non-adjacent)" "-" "with" "SOPRANO2"

}
\markup \wordwrap \tiny \bold {
  TENOR2 [bar 5, beat 1] note G
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  96 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "Сопра"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  97 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "ALTO2"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  98 "-" "Harmony:" "Voice" "overlapping" "(non-adjacent)" "-" "with" "SOPRANO2"

}
\markup \wordwrap \tiny \bold {
  BASS2 [bar 1, beat 1] note G
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  99 "-" "Harmony:" "Convoluted" "harmony"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  100 "-" "Melody:" "Stagnation" "(5" "notes" "<m3)"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  101 "-" "Melody:" "Repetition" "-" "2" "notes" "are" "repeated:" "immediate" "(uniform)"

}
\markup \wordwrap \tiny \bold {
  BASS2 [bar 2, beat 1] note A
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  102 "-" "Harmony:" "Voice" "overlapping" "(non-adjacent)" "-" "with" "ALTO2"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  103 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "TENOR2"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  104 "-" "Melody:" "Repetition" "-" "2" "notes" "are" "repeated:" "immediate" "(uniform)"

}
\markup \wordwrap \tiny \bold {
  BASS2 [bar 3, beat 1] note G
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  105 "-" "Harmony:" "Voice" "overlapping" "(non-adjacent)" "-" "with" "ALTO2"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  106 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "TENOR2"

}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  107 "-" "Harmony:" "7th" "chord"

}
\markup \wordwrap \tiny \bold {
  BASS2 [bar 4, beat 1] note A
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  108 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "TENOR2"

}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  109 "-" "Harmony:" "Penultimate" "chord" "(no" "V" "or" "VII" "chord)"

}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  110 "-" "Harmony:" "7th" "chord"

}
\markup \wordwrap \tiny \bold {
  BASS2 [bar 5, beat 1] note G
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  111 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "TENOR2"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  112 "-" "Harmony:" "Convoluted" "harmony"

}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  113 "-" "Harmony:" "Non-permitted" "last" "harmony" "(not" "T)"

}
\markup {
      \vspace #1

}
\markup {
      \vspace #1

}
\markup \wordwrap {
  \bold {
"#"2 (from test-vocal-range) Key: G mixolydian
}
\tiny { 
"Lyrics:" "sp1."
}

}
<<
\new Staff = "staff8" {
     
  \set Staff.instrumentName = \markup { \teeny \override #'(baseline-skip . 2.0) \center-column{ "Сопра"  "[species 1]"  "[Soprano]" } }
  \clef "treble"
  \key c
  \major
  \time 4/4
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

  { \override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\override BreathingSign.color = #(rgb-color 1.000 0.000 0.000)
 
\override TrillSpanner.bound-details.left.text = \markup{ \raise #0.6 \teeny "Stagnation" }
  \override TrillSpanner.color = #(rgb-color 1.000 0.000 0.000)
\myTS "parallel" #(rgb-color 0.706 0.706 0.000) \textSpannerDown
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
d'1
\startTrillSpan
\startTextSpan
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
e'1
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\myTS "all similar" #(rgb-color 0.706 0.706 0.000) \textSpannerDown
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
d'1
\startTextSpan
\stopTextSpan
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\myTS "all similar" #(rgb-color 0.706 0.706 0.000) \textSpannerDown
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
e'1
\startTextSpan
\stopTextSpan
\glissando
 \override NoteColumn.glissando-skip = ##t
\override BreathingSign.color = #(rgb-color 1.000 0.000 0.000) 
\override NoteColumn.glissando-skip = ##f
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
d'1
\stopTextSpan
\stopTrillSpan

  }
}
  \new Lyrics \with { alignAboveContext = "staff8" } {
    \lyricmode {
      \override StanzaNumber.font-size = #-2
      \set stanza = #" Flags:"
      \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 2
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 1
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000)  5
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 4
        \with-color #(rgb-color 1.000 0.000 0.000)  3
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 8
        \with-color #(rgb-color 0.706 0.706 0.000)  7
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 6
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 11
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 10
        \with-color #(rgb-color 1.000 0.000 0.000)  9
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 13
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 12
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8     }
  }
\new Staff = "staff7" {
     
  \set Staff.instrumentName = \markup { \teeny \override #'(baseline-skip . 2.0) \center-column{ "Сопра"  "[species 1]"  "[Soprano]" } }
  \clef "treble"
  \key c
  \major
  \time 4/4
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

  { \override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\override BreathingSign.color = #(rgb-color 1.000 0.000 0.000)
 
\override TrillSpanner.bound-details.left.text = \markup{ \raise #0.6 \teeny "Stagnation" }
  \override TrillSpanner.color = #(rgb-color 1.000 0.000 0.000)
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
d'1
\startTrillSpan
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
e'1
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
d'1
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
e'1
\glissando
 \override NoteColumn.glissando-skip = ##t
\override BreathingSign.color = #(rgb-color 1.000 0.000 0.000) 
\override NoteColumn.glissando-skip = ##f
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
d'1
\stopTrillSpan

  }
}
  \new Lyrics \with { alignAboveContext = "staff7" } {
    \lyricmode {
      \override StanzaNumber.font-size = #-2
      \set stanza = #" Flags:"
      \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 15
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 14
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 18
        \with-color #(rgb-color 1.000 0.000 0.000)  17
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 16
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 20
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 19
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 22
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 21
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 24
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 23
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8     }
  }
\new Staff = "staff6" {
     
  \set Staff.instrumentName = \markup { \teeny \override #'(baseline-skip . 2.0) \center-column{ "Сопра"  "[species 1]"  "[Alto]" } }
  \clef "treble"
  \key c
  \major
  \time 4/4
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

  { \override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\override BreathingSign.color = #(rgb-color 1.000 0.000 0.000)
 
\override TrillSpanner.bound-details.left.text = \markup{ \raise #0.6 \teeny "Stagnation" }
  \override TrillSpanner.color = #(rgb-color 1.000 0.000 0.000)
\myTS "overlap" #(rgb-color 1.000 0.000 0.000) \textSpannerDown
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
d'1
\startTrillSpan
\startTextSpan
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\myTS "overlap" #(rgb-color 1.000 0.000 0.000) \textSpannerDown
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
e'1
\startTextSpan
\stopTextSpan
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\myTS "overlap" #(rgb-color 1.000 0.000 0.000) \textSpannerDown
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
d'1
\startTextSpan
\stopTextSpan
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\myTS "overlap" #(rgb-color 1.000 0.000 0.000) \textSpannerDown
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
e'1
\startTextSpan
\stopTextSpan
\glissando
 \override NoteColumn.glissando-skip = ##t
\override BreathingSign.color = #(rgb-color 1.000 0.000 0.000) 
\override NoteColumn.glissando-skip = ##f
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
d'1
\stopTextSpan
\stopTrillSpan

  }
}
  \new Lyrics \with { alignAboveContext = "staff6" } {
    \lyricmode {
      \override StanzaNumber.font-size = #-2
      \set stanza = #" Flags:"
      \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 26
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 25
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
...
        \with-color #(rgb-color 1.000 0.000 0.000)  28
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 27
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 33
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 32
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 31
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 36
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 35
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 34
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 39
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 38
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 37
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8     }
  }
\new Staff = "staff5" {
     
  \set Staff.instrumentName = \markup { \teeny \override #'(baseline-skip . 2.0) \center-column{ "Сопра"  "[species 1]"  "[Alto]" } }
  \clef "treble"
  \key c
  \major
  \time 4/4
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

  { \override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\override BreathingSign.color = #(rgb-color 1.000 0.000 0.000)
 
\override TrillSpanner.bound-details.left.text = \markup{ \raise #0.6 \teeny "Stagnation" }
  \override TrillSpanner.color = #(rgb-color 1.000 0.000 0.000)
\myTS "overlap" #(rgb-color 1.000 0.000 0.000) \textSpannerDown
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
d'1
\startTrillSpan
\startTextSpan
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\myTS "overlap" #(rgb-color 1.000 0.000 0.000) \textSpannerDown
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
e'1
\startTextSpan
\stopTextSpan
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\myTS "overlap" #(rgb-color 1.000 0.000 0.000) \textSpannerDown
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
d'1
\startTextSpan
\stopTextSpan
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\myTS "overlap" #(rgb-color 1.000 0.000 0.000) \textSpannerDown
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
e'1
\startTextSpan
\stopTextSpan
\glissando
 \override NoteColumn.glissando-skip = ##t
\override BreathingSign.color = #(rgb-color 1.000 0.000 0.000) 
\override NoteColumn.glissando-skip = ##f
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
d'1
\stopTextSpan
\stopTrillSpan

  }
}
  \new Lyrics \with { alignAboveContext = "staff5" } {
    \lyricmode {
      \override StanzaNumber.font-size = #-2
      \set stanza = #" Flags:"
      \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 41
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 40
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
...
        \with-color #(rgb-color 1.000 0.000 0.000)  43
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 42
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 48
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 47
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 46
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 51
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 50
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 49
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 54
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 53
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 52
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8     }
  }
\new Staff = "staff4" {
     
  \set Staff.instrumentName = \markup { \teeny \override #'(baseline-skip . 2.0) \center-column{ "Сопра"  "[species 1]"  "[Tenor]" } }
  \clef "treble_8"
  \key c
  \major
  \time 4/4
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

  { \override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\override BreathingSign.color = #(rgb-color 1.000 0.000 0.000)
 
\override TrillSpanner.bound-details.left.text = \markup{ \raise #0.6 \teeny "Stagnation" }
  \override TrillSpanner.color = #(rgb-color 1.000 0.000 0.000)
\myTS "overlap" #(rgb-color 1.000 0.000 0.000) \textSpannerDown
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
g1
\startTrillSpan
\startTextSpan
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\myTS "overlap" #(rgb-color 1.000 0.000 0.000) \textSpannerDown
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
a1
\startTextSpan
\stopTextSpan
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\myTS "overlap" #(rgb-color 1.000 0.000 0.000) \textSpannerDown
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
g1
\startTextSpan
\stopTextSpan
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\myTS "overlap" #(rgb-color 1.000 0.000 0.000) \textSpannerDown
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
a1
\startTextSpan
\stopTextSpan
\glissando
 \override NoteColumn.glissando-skip = ##t
\override BreathingSign.color = #(rgb-color 1.000 0.000 0.000) 
\override NoteColumn.glissando-skip = ##f
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
g1
\stopTextSpan
\stopTrillSpan

  }
}
  \new Lyrics \with { alignAboveContext = "staff4" } {
    \lyricmode {
      \override StanzaNumber.font-size = #-2
      \set stanza = #" Flags:"
      \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 56
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 55
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
...
        \with-color #(rgb-color 1.000 0.000 0.000)  58
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 57
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 63
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 62
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 61
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 66
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 65
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 64
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
...
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 68
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 67
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8     }
  }
\new Staff = "staff3" {
     
  \set Staff.instrumentName = \markup { \teeny \override #'(baseline-skip . 2.0) \center-column{ "SOPRANO2"  "[species 1]"  "[Tenor]" } }
  \clef "treble_8"
  \key c
  \major
  \time 4/4
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

  { \override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\override BreathingSign.color = #(rgb-color 1.000 0.000 0.000)
 
\override TrillSpanner.bound-details.left.text = \markup{ \raise #0.6 \teeny "Stagnation" }
  \override TrillSpanner.color = #(rgb-color 1.000 0.000 0.000)
\myTS "overlap" #(rgb-color 1.000 0.000 0.000) \textSpannerDown
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
g1
\startTrillSpan
\startTextSpan
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\myTS "overlap" #(rgb-color 1.000 0.000 0.000) \textSpannerDown
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
a1
\startTextSpan
\stopTextSpan
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\myTS "overlap" #(rgb-color 1.000 0.000 0.000) \textSpannerDown
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
g1
\startTextSpan
\stopTextSpan
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\myTS "overlap" #(rgb-color 1.000 0.000 0.000) \textSpannerDown
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
a1
\startTextSpan
\stopTextSpan
\glissando
 \override NoteColumn.glissando-skip = ##t
\override BreathingSign.color = #(rgb-color 1.000 0.000 0.000) 
\override NoteColumn.glissando-skip = ##f
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
g1
\stopTextSpan
\stopTrillSpan

  }
}
  \new Lyrics \with { alignAboveContext = "staff3" } {
    \lyricmode {
      \override StanzaNumber.font-size = #-2
      \set stanza = #" Flags:"
      \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 72
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 71
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
...
        \with-color #(rgb-color 1.000 0.000 0.000)  74
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 73
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 79
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 78
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 77
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 82
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 81
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 80
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
...
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 84
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 83
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8     }
  }
\new Staff = "staff2" {
     
  \set Staff.instrumentName = \markup { \teeny \override #'(baseline-skip . 2.0) \center-column{ "ALTO2"  "[species 1]"  "[Bass]" } }
  \clef "bass"
  \key c
  \major
  \time 4/4
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

  { \override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\override BreathingSign.color = #(rgb-color 1.000 0.000 0.000)
 
\override TrillSpanner.bound-details.left.text = \markup{ \raise #0.6 \teeny "Stagnation" }
  \override TrillSpanner.color = #(rgb-color 1.000 0.000 0.000)
\myTS "overlap" #(rgb-color 1.000 0.000 0.000) \textSpannerDown
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
g1
\startTrillSpan
\startTextSpan
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\myTS "overlap" #(rgb-color 1.000 0.000 0.000) \textSpannerDown
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
a1
\startTextSpan
\stopTextSpan
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\myTS "overlap" #(rgb-color 1.000 0.000 0.000) \textSpannerDown
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
g1
\startTextSpan
\stopTextSpan
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\myTS "overlap" #(rgb-color 1.000 0.000 0.000) \textSpannerDown
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
a1
\startTextSpan
\stopTextSpan
\glissando
 \override NoteColumn.glissando-skip = ##t
\override BreathingSign.color = #(rgb-color 1.000 0.000 0.000) 
\override NoteColumn.glissando-skip = ##f
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
g1
\stopTextSpan
\stopTrillSpan

  }
}
  \new Lyrics \with { alignAboveContext = "staff2" } {
    \lyricmode {
      \override StanzaNumber.font-size = #-2
      \set stanza = #" Flags:"
      \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 88
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 87
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
...
        \with-color #(rgb-color 1.000 0.000 0.000)  90
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 89
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 95
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 94
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 93
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 98
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 97
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 96
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
...
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 100
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 99
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8     }
  }
\new Staff = "staff1" {
     
  \set Staff.instrumentName = \markup { \teeny \override #'(baseline-skip . 2.0) \center-column{ "TENOR2"  "[species 1]"  "[Bass]" } }
  \clef "bass"
  \key c
  \major
  \time 4/4
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

  { \override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\override BreathingSign.color = #(rgb-color 1.000 0.000 0.000)
 
\override TrillSpanner.bound-details.left.text = \markup{ \raise #0.6 \teeny "Stagnation" }
  \override TrillSpanner.color = #(rgb-color 1.000 0.000 0.000)
\myTS "overlap" #(rgb-color 1.000 0.000 0.000) \textSpannerDown
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
g1
\startTrillSpan
\startTextSpan
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\myTS "overlap" #(rgb-color 1.000 0.000 0.000) \textSpannerDown
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
a1
\startTextSpan
\stopTextSpan
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\myTS "overlap" #(rgb-color 1.000 0.000 0.000) \textSpannerDown
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
g1
\startTextSpan
\stopTextSpan
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\myTS "overlap" #(rgb-color 1.000 0.000 0.000) \textSpannerDown
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
a1
\startTextSpan
\stopTextSpan
\glissando
 \override NoteColumn.glissando-skip = ##t
\override BreathingSign.color = #(rgb-color 1.000 0.000 0.000) 
\override NoteColumn.glissando-skip = ##f
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
g1
\stopTextSpan
\stopTrillSpan

  }
}
  \new Lyrics \with { alignAboveContext = "staff1" } {
    \lyricmode {
      \override StanzaNumber.font-size = #-2
      \set stanza = #" Flags:"
      \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 104
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 103
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
...
        \with-color #(rgb-color 1.000 0.000 0.000)  106
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 105
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 111
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 110
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 109
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 114
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 113
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 112
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
...
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 116
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 115
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8     }
  }
\new Staff = "staff0" {
     
  \set Staff.instrumentName = \markup { \teeny \override #'(baseline-skip . 2.0) \center-column{ "BASS2"  "[c.f.]"  "[Bass]" } }
  \clef "bass"
  \key c
  \major
  \time 4/4
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

  { \override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\override BreathingSign.color = #(rgb-color 1.000 0.000 0.000)
 
\override TrillSpanner.bound-details.left.text = \markup{ \raise #0.6 \teeny "Stagnation" }
  \override TrillSpanner.color = #(rgb-color 1.000 0.000 0.000)
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
g1
\startTrillSpan
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
a1
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
g1
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
a1
\glissando
 \override NoteColumn.glissando-skip = ##t
\override BreathingSign.color = #(rgb-color 1.000 0.000 0.000) 
\override NoteColumn.glissando-skip = ##f
\once \override NoteHead.color = #(rgb-color 1.000 0.000 0.000)
\once \override Stem.color = #(rgb-color 1.000 0.000 0.000)
g1
\stopTrillSpan

  }
}
  \new Lyrics \with { alignAboveContext = "staff0" } {
    \lyricmode {
      \override StanzaNumber.font-size = #-2
      \set stanza = #" Flags:"
      \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 120
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 119
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 123
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 122
        \with-color #(rgb-color 1.000 0.000 0.000)  121
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 126
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 125
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 124
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 129
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 128
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 127
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000)  132
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 131
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 130
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8     }
  }
  \new Lyrics \with { alignBelowContext = "staff0" } {
    \lyricmode {
      \override StanzaNumber.font-size = #-2
      \set stanza = #" Harmony:"
      \override InstrumentName #'font-series = #'bold
      \override InstrumentName.font-size = #-2
      \set shortVocalName = "H:"
  \markup{   \teeny 
    \pad-markup #0.4 
\concat { T } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8   \markup{   \teeny 
    \pad-markup #0.4 
\concat { SII } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8   \markup{   \teeny 
    \pad-markup #0.4 
\concat { T } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8   \markup{   \teeny 
    \pad-markup #0.4 
\concat { SII } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8   \markup{   \teeny 
    \pad-markup #0.4 
\concat { T } }8
    }
  }
>>
\markup \wordwrap \tiny \bold {
  Сопрано [bar 1, beat 1] note D
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  1 "-" "Melody:" "Stagnation" "(5" "notes" "<m3)"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  2 "-" "Melody:" "Repetition" "-" "2" "notes" "are" "repeated:" "immediate" "(uniform)"

}
\markup \wordwrap \tiny \bold {
  Сопрано [bar 2, beat 1] note E
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  3 "-" "Melody:" "Repetition" "-" "2" "notes" "are" "repeated:" "immediate" "(uniform)"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  4 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "BASS2"

}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  5 "-" "Harmony:" "All" "voices" "move" "in" "same" "direction" "-" "with" "BASS2"

}
\markup \wordwrap \tiny \bold {
  Сопрано [bar 3, beat 1] note D
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  6 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "BASS2"

}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  7 "-" "Harmony:" "All" "voices" "move" "in" "same" "direction" "-" "with" "BASS2"

}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  8 "-" "Harmony:" "All" "voices" "move" "in" "parallel" "(two" "times)" "-" "with" "BASS2"

}
\markup \wordwrap \tiny \bold {
  Сопрано [bar 4, beat 1] note E
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  9 "-" "Melody:" "Melodic" "climax" "is" "repeated" "(in" "upper" "part)"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  10 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "BASS2"

}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  11 "-" "Harmony:" "All" "voices" "move" "in" "same" "direction" "-" "with" "BASS2"

}
\markup \wordwrap \tiny \bold {
  Сопрано [bar 5, beat 1] note D
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  12 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "BASS2"

}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  13 "-" "Harmony:" "All" "voices" "move" "in" "same" "direction" "-" "with" "BASS2"

}
\markup \wordwrap \tiny \bold {
  Сопрано (2) [bar 1, beat 1] note D
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  14 "-" "Melody:" "Stagnation" "(5" "notes" "<m3)"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  15 "-" "Melody:" "Repetition" "-" "2" "notes" "are" "repeated:" "immediate" "(uniform)"

}
\markup \wordwrap \tiny \bold {
  Сопрано (2) [bar 2, beat 1] note E
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  16 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "Сопра"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  17 "-" "Melody:" "Repetition" "-" "2" "notes" "are" "repeated:" "immediate" "(uniform)"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  18 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "BASS2"

}
\markup \wordwrap \tiny \bold {
  Сопрано (2) [bar 3, beat 1] note D
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  19 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "Сопра"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  20 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "BASS2"

}
\markup \wordwrap \tiny \bold {
  Сопрано (2) [bar 4, beat 1] note E
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  21 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "Сопра"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  22 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "BASS2"

}
\markup \wordwrap \tiny \bold {
  Сопрано (2) [bar 5, beat 1] note D
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  23 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "Сопра"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  24 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "BASS2"

}
\markup \wordwrap \tiny \bold {
  Сопрано (3) [bar 1, beat 1] note D
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  25 "-" "Melody:" "Stagnation" "(5" "notes" "<m3)"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  26 "-" "Melody:" "Repetition" "-" "2" "notes" "are" "repeated:" "immediate" "(uniform)"

}
\markup \wordwrap \tiny \bold {
  Сопрано (3) [bar 2, beat 1] note E
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  27 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "Сопра"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  28 "-" "Melody:" "Repetition" "-" "2" "notes" "are" "repeated:" "immediate" "(uniform)"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  29 "-" "Harmony:" "Voice" "overlapping" "(non-adjacent)" "-" "with" "Сопра"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  30 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "BASS2"

}
\markup \wordwrap \tiny \bold {
  Сопрано (3) [bar 3, beat 1] note D
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  31 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "Сопра"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  32 "-" "Harmony:" "Voice" "overlapping" "(non-adjacent)" "-" "with" "Сопра"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  33 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "BASS2"

}
\markup \wordwrap \tiny \bold {
  Сопрано (3) [bar 4, beat 1] note E
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  34 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "Сопра"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  35 "-" "Harmony:" "Voice" "overlapping" "(non-adjacent)" "-" "with" "Сопра"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  36 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "BASS2"

}
\markup \wordwrap \tiny \bold {
  Сопрано (3) [bar 5, beat 1] note D
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  37 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "Сопра"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  38 "-" "Harmony:" "Voice" "overlapping" "(non-adjacent)" "-" "with" "Сопра"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  39 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "BASS2"

}
\markup \wordwrap \tiny \bold {
  Сопрано (4) [bar 1, beat 1] note D
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  40 "-" "Melody:" "Stagnation" "(5" "notes" "<m3)"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  41 "-" "Melody:" "Repetition" "-" "2" "notes" "are" "repeated:" "immediate" "(uniform)"

}
\markup \wordwrap \tiny \bold {
  Сопрано (4) [bar 2, beat 1] note E
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  42 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "Сопра"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  43 "-" "Melody:" "Repetition" "-" "2" "notes" "are" "repeated:" "immediate" "(uniform)"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  44 "-" "Harmony:" "Voice" "overlapping" "(non-adjacent)" "-" "with" "Сопра"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  45 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "BASS2"

}
\markup \wordwrap \tiny \bold {
  Сопрано (4) [bar 3, beat 1] note D
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  46 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "Сопра"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  47 "-" "Harmony:" "Voice" "overlapping" "(non-adjacent)" "-" "with" "Сопра"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  48 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "BASS2"

}
\markup \wordwrap \tiny \bold {
  Сопрано (4) [bar 4, beat 1] note E
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  49 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "Сопра"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  50 "-" "Harmony:" "Voice" "overlapping" "(non-adjacent)" "-" "with" "Сопра"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  51 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "BASS2"

}
\markup \wordwrap \tiny \bold {
  Сопрано (4) [bar 5, beat 1] note D
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  52 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "Сопра"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  53 "-" "Harmony:" "Voice" "overlapping" "(non-adjacent)" "-" "with" "Сопра"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  54 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "BASS2"

}
\markup \wordwrap \tiny \bold {
  Сопрано (5) [bar 1, beat 1] note G
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  55 "-" "Melody:" "Stagnation" "(5" "notes" "<m3)"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  56 "-" "Melody:" "Repetition" "-" "2" "notes" "are" "repeated:" "immediate" "(uniform)"

}
\markup \wordwrap \tiny \bold {
  Сопрано (5) [bar 2, beat 1] note A
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  57 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "Сопра"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  58 "-" "Melody:" "Repetition" "-" "2" "notes" "are" "repeated:" "immediate" "(uniform)"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  59 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "BASS2"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  60 "-" "Harmony:" "Voice" "overlapping" "(non-adjacent)" "-" "with" "BASS2"

}
\markup \wordwrap \tiny \bold {
  Сопрано (5) [bar 3, beat 1] note G
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  61 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "Сопра"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  62 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "BASS2"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  63 "-" "Harmony:" "Voice" "overlapping" "(non-adjacent)" "-" "with" "BASS2"

}
\markup \wordwrap \tiny \bold {
  Сопрано (5) [bar 4, beat 1] note A
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  64 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "Сопра"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  65 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "BASS2"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  66 "-" "Harmony:" "Voice" "overlapping" "(non-adjacent)" "-" "with" "BASS2"

}
\markup \wordwrap \tiny \bold {
  Сопрано (5) [bar 5, beat 1] note G
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  67 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "Сопра"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  68 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "BASS2"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  69 "-" "Harmony:" "Voice" "overlapping" "(non-adjacent)" "-" "with" "BASS2"

}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  70 "-" "Melody:" "Frequent" "return" "to" "the" "tonic" "(3" "within" "6)"

}
\markup \wordwrap \tiny \bold {
  SOPRANO2 [bar 1, beat 1] note G
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  71 "-" "Melody:" "Stagnation" "(5" "notes" "<m3)"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  72 "-" "Melody:" "Repetition" "-" "2" "notes" "are" "repeated:" "immediate" "(uniform)"

}
\markup \wordwrap \tiny \bold {
  SOPRANO2 [bar 2, beat 1] note A
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  73 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "Сопра"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  74 "-" "Melody:" "Repetition" "-" "2" "notes" "are" "repeated:" "immediate" "(uniform)"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  75 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "BASS2"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  76 "-" "Harmony:" "Voice" "overlapping" "(non-adjacent)" "-" "with" "BASS2"

}
\markup \wordwrap \tiny \bold {
  SOPRANO2 [bar 3, beat 1] note G
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  77 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "Сопра"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  78 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "BASS2"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  79 "-" "Harmony:" "Voice" "overlapping" "(non-adjacent)" "-" "with" "BASS2"

}
\markup \wordwrap \tiny \bold {
  SOPRANO2 [bar 4, beat 1] note A
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  80 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "Сопра"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  81 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "BASS2"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  82 "-" "Harmony:" "Voice" "overlapping" "(non-adjacent)" "-" "with" "BASS2"

}
\markup \wordwrap \tiny \bold {
  SOPRANO2 [bar 5, beat 1] note G
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  83 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "Сопра"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  84 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "BASS2"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  85 "-" "Harmony:" "Voice" "overlapping" "(non-adjacent)" "-" "with" "BASS2"

}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  86 "-" "Melody:" "Frequent" "return" "to" "the" "tonic" "(3" "within" "6)"

}
\markup \wordwrap \tiny \bold {
  ALTO2 [bar 1, beat 1] note G
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  87 "-" "Melody:" "Stagnation" "(5" "notes" "<m3)"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  88 "-" "Melody:" "Repetition" "-" "2" "notes" "are" "repeated:" "immediate" "(uniform)"

}
\markup \wordwrap \tiny \bold {
  ALTO2 [bar 2, beat 1] note A
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  89 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "Сопра"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  90 "-" "Melody:" "Repetition" "-" "2" "notes" "are" "repeated:" "immediate" "(uniform)"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  91 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "BASS2"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  92 "-" "Harmony:" "Voice" "overlapping" "(non-adjacent)" "-" "with" "BASS2"

}
\markup \wordwrap \tiny \bold {
  ALTO2 [bar 3, beat 1] note G
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  93 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "Сопра"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  94 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "BASS2"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  95 "-" "Harmony:" "Voice" "overlapping" "(non-adjacent)" "-" "with" "BASS2"

}
\markup \wordwrap \tiny \bold {
  ALTO2 [bar 4, beat 1] note A
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  96 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "Сопра"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  97 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "BASS2"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  98 "-" "Harmony:" "Voice" "overlapping" "(non-adjacent)" "-" "with" "BASS2"

}
\markup \wordwrap \tiny \bold {
  ALTO2 [bar 5, beat 1] note G
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  99 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "Сопра"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  100 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "BASS2"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  101 "-" "Harmony:" "Voice" "overlapping" "(non-adjacent)" "-" "with" "BASS2"

}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  102 "-" "Melody:" "Frequent" "return" "to" "the" "tonic" "(3" "within" "6)"

}
\markup \wordwrap \tiny \bold {
  TENOR2 [bar 1, beat 1] note G
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  103 "-" "Melody:" "Stagnation" "(5" "notes" "<m3)"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  104 "-" "Melody:" "Repetition" "-" "2" "notes" "are" "repeated:" "immediate" "(uniform)"

}
\markup \wordwrap \tiny \bold {
  TENOR2 [bar 2, beat 1] note A
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  105 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "Сопра"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  106 "-" "Melody:" "Repetition" "-" "2" "notes" "are" "repeated:" "immediate" "(uniform)"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  107 "-" "Harmony:" "Voice" "overlapping" "(non-adjacent)" "-" "with" "SOPRANO2"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  108 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "BASS2"

}
\markup \wordwrap \tiny \bold {
  TENOR2 [bar 3, beat 1] note G
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  109 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "Сопра"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  110 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "ALTO2"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  111 "-" "Harmony:" "Voice" "overlapping" "(non-adjacent)" "-" "with" "SOPRANO2"

}
\markup \wordwrap \tiny \bold {
  TENOR2 [bar 4, beat 1] note A
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  112 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "Сопра"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  113 "-" "Harmony:" "Voice" "overlapping" "(non-adjacent)" "-" "with" "SOPRANO2"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  114 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "BASS2"

}
\markup \wordwrap \tiny \bold {
  TENOR2 [bar 5, beat 1] note G
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  115 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "Сопра"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  116 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "ALTO2"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  117 "-" "Harmony:" "Voice" "overlapping" "(non-adjacent)" "-" "with" "SOPRANO2"

}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  118 "-" "Melody:" "Frequent" "return" "to" "the" "tonic" "(3" "within" "6)"

}
\markup \wordwrap \tiny \bold {
  BASS2 [bar 1, beat 1] note G
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  119 "-" "Melody:" "Stagnation" "(5" "notes" "<m3)"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  120 "-" "Melody:" "Repetition" "-" "2" "notes" "are" "repeated:" "immediate" "(uniform)"

}
\markup \wordwrap \tiny \bold {
  BASS2 [bar 2, beat 1] note A
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  121 "-" "Melody:" "Repetition" "-" "2" "notes" "are" "repeated:" "immediate" "(uniform)"

}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  122 "-" "Harmony:" "Two" "consecutive" "incomplete" "harmonies"

}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  123 "-" "Harmony:" "Incomplete" "harmony" "(any" "-" "no" "3rd)"

}
\markup \wordwrap \tiny \bold {
  BASS2 [bar 3, beat 1] note G
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  124 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "TENOR2"

}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  125 "-" "Harmony:" "Two" "consecutive" "incomplete" "harmonies"

}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  126 "-" "Harmony:" "Incomplete" "harmony" "(any" "-" "no" "3rd)"

}
\markup \wordwrap \tiny \bold {
  BASS2 [bar 4, beat 1] note A
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  127 "-" "Harmony:" "Two" "consecutive" "incomplete" "harmonies"

}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  128 "-" "Harmony:" "Incomplete" "harmony" "(any" "-" "no" "3rd)"

}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  129 "-" "Harmony:" "Penultimate" "chord" "(no" "V" "or" "VII" "chord)"

}
\markup \wordwrap \tiny \bold {
  BASS2 [bar 5, beat 1] note G
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  130 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "TENOR2"

}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  131 "-" "Harmony:" "Two" "consecutive" "incomplete" "harmonies"

}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  132 "-" "Melody:" "Frequent" "return" "to" "the" "tonic" "(3" "within" "6)"

}
\markup {
      \vspace #1

}
