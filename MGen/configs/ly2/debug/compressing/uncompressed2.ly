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
     
  \set Staff.instrumentName = \markup { \teeny \override #'(baseline-skip . 2.0) \center-column{ "Part"  ""  "[Soprano]" } }
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
     
  \set Staff.instrumentName = \markup { \teeny \override #'(baseline-skip . 2.0) \center-column{ "Part"  ""  "[Soprano]" } }
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
     
  \set Staff.instrumentName = \markup { \teeny \override #'(baseline-skip . 2.0) \center-column{ "Part"  ""  "[Alto]" } }
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
     
  \set Staff.instrumentName = \markup { \teeny \override #'(baseline-skip . 2.0) \center-column{ "Part"  ""  "[Alto]" } }
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
     
  \set Staff.instrumentName = \markup { \teeny \override #'(baseline-skip . 2.0) \center-column{ "Part"  ""  "[Tenor]" } }
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
     
  \set Staff.instrumentName = \markup { \teeny \override #'(baseline-skip . 2.0) \center-column{ "SOPRANO2"  ""  "[Tenor]" } }
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
     
  \set Staff.instrumentName = \markup { \teeny \override #'(baseline-skip . 2.0) \center-column{ "ALTO2"  ""  "[Bass]" } }
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
     
  \set Staff.instrumentName = \markup { \teeny \override #'(baseline-skip . 2.0) \center-column{ "TENOR2"  ""  "[Bass]" } }
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
     
  \set Staff.instrumentName = \markup { \teeny \override #'(baseline-skip . 2.0) \center-column{ "BASS2"  ""  "[Bass]" } }
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
\pageBreak
\markup \wordwrap \tiny \bold {
  Part [bar 1, beat 1] note D
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  1 "-" "Melody:" "Stagnation" "(5" "notes" "<m3)"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  2 "-" "Melody:" "Repetition" "-" "2" "notes" "are" "repeated:" "immediate" "(uniform)"

}
\markup \wordwrap \tiny \bold {
  Part [bar 2, beat 1] note E
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
  Part [bar 3, beat 1] note D
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
  Part [bar 4, beat 1] note E
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
  Part [bar 5, beat 1] note D
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  12 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "BASS2"

}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  13 "-" "Harmony:" "All" "voices" "move" "in" "same" "direction" "-" "with" "BASS2"

}
\markup \wordwrap \tiny \bold {
  Part (2) [bar 1, beat 1] note D
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  14 "-" "Melody:" "Stagnation" "(5" "notes" "<m3)"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  15 "-" "Melody:" "Repetition" "-" "2" "notes" "are" "repeated:" "immediate" "(uniform)"

}
\markup \wordwrap \tiny \bold {
  Part (2) [bar 2, beat 1] note E
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  16 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "Part"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  17 "-" "Melody:" "Repetition" "-" "2" "notes" "are" "repeated:" "immediate" "(uniform)"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  18 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "BASS2"

}
\markup \wordwrap \tiny \bold {
  Part (2) [bar 3, beat 1] note D
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  19 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "Part"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  20 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "BASS2"

}
\markup \wordwrap \tiny \bold {
  Part (2) [bar 4, beat 1] note E
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  21 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "Part"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  22 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "BASS2"

}
\markup \wordwrap \tiny \bold {
  Part (2) [bar 5, beat 1] note D
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  23 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "Part"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  24 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "BASS2"

}
\markup \wordwrap \tiny \bold {
  Part (3) [bar 1, beat 1] note D
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  25 "-" "Melody:" "Stagnation" "(5" "notes" "<m3)"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  26 "-" "Melody:" "Repetition" "-" "2" "notes" "are" "repeated:" "immediate" "(uniform)"

}
\markup \wordwrap \tiny \bold {
  Part (3) [bar 2, beat 1] note E
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  27 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "Part"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  28 "-" "Melody:" "Repetition" "-" "2" "notes" "are" "repeated:" "immediate" "(uniform)"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  29 "-" "Harmony:" "Voice" "overlapping" "(non-adjacent)" "-" "with" "Part"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  30 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "BASS2"

}
\markup \wordwrap \tiny \bold {
  Part (3) [bar 3, beat 1] note D
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  31 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "Part"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  32 "-" "Harmony:" "Voice" "overlapping" "(non-adjacent)" "-" "with" "Part"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  33 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "BASS2"

}
\markup \wordwrap \tiny \bold {
  Part (3) [bar 4, beat 1] note E
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  34 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "Part"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  35 "-" "Harmony:" "Voice" "overlapping" "(non-adjacent)" "-" "with" "Part"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  36 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" "-" "with" "BASS2"

}
\markup \wordwrap \tiny \bold {
  Part (3) [bar 5, beat 1] note D
}
