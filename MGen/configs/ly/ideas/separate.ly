\version "2.18.2"
\language "english"
\paper { #(include-special-characters) }
circle =
\once \override NoteHead.stencil = #(lambda (grob)
    (let* ((note (ly:note-head::print grob))
           (combo-stencil (ly:stencil-add
               note
               (circle-stencil note 0.1 0.2))))
          (ly:make-stencil (ly:stencil-expr combo-stencil)
            (ly:stencil-extent note X)
            (ly:stencil-extent note Y))))
speakOn = {
  \override Stem.stencil =
    #(lambda (grob)
       (let* ((x-parent (ly:grob-parent grob X))
              (is-rest? (ly:grob? (ly:grob-object x-parent 'rest))))
         (if is-rest?
             empty-stencil
             (ly:stencil-combine-at-edge
              (ly:stem::print grob)
              Y
              (- (ly:grob-property grob 'direction))
              (grob-interpret-markup grob
                                     (markup #:center-align #:fontsize -4
                                             #:musicglyph "noteheads.s2cross"))
              -2.3))))
}

speakOff = {
  \revert Stem.stencil
  \revert Flag.stencil
}

#(define-markup-command (on-color layout props color arg) (color? markup?)
   (let* ((stencil (interpret-markup layout props arg))
          (X-ext (ly:stencil-extent stencil X))
          (Y-ext (ly:stencil-extent stencil Y)))
     (ly:stencil-add (ly:make-stencil
                      (list 'color color
                        (ly:stencil-expr (ly:round-filled-box X-ext Y-ext 0))
                        X-ext Y-ext)) stencil)))

#(define-markup-command (left-bracket layout props) ()
"Draw left hand bracket"
(let* ((th 0.3) ;; todo: take from GROB
	(width (* 2.5 th)) ;; todo: take from GROB
	(ext '(-2.8 . 2.8))) ;; todo: take line-count into account
	(ly:bracket Y ext th width)))

leftBracket = {
  \once\override BreathingSign.text = #(make-left-bracket-markup)
  \once\override BreathingSign.break-visibility = #end-of-line-invisible
  \once\override BreathingSign.Y-offset = ##f
  % Trick to print it after barlines and signatures:
  \once\override BreathingSign.break-align-symbol = #'custos
  \breathe 
}


#(define-markup-command (right-bracket layout props) ()
"Draw right hand bracket"
  (let* ((th .3);;todo: take from GROB
          (width (* 2.5 th)) ;; todo: take from GROB
          (ext '(-2.8 . 2.8))) ;; todo: take line-count into account
        (ly:bracket Y ext th (- width))))

rightBracket = {
  \once\override BreathingSign.text = #(make-right-bracket-markup)
  \once\override BreathingSign.Y-offset = ##f
  \breathe
}

#(define (label text color)
  (lambda (grob)
    (ly:stencil-aligned-to
      (ly:stencil-combine-at-edge
        (ly:stencil-aligned-to (ly:horizontal-bracket::print grob) X LEFT)
        Y (ly:grob-property grob 'direction)
        (ly:stencil-aligned-to (grob-interpret-markup grob (markup 
                                                            #:with-color color
                                                            (#:teeny text))) X LEFT)
      0.2)
     X LEFT)))

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

\header {
  title = ""
  subtitle = "Multivoice Counterpoint Analysis: test-pco (2018-07-31 09:38)"
  dedication = ""
}

\markup \wordwrap \bold {
      \vspace #3
"#"1 (from test-pco) Key: F lydian
}
<<
\new Staff = "staff3" {
     
  \set Staff.instrumentName = \markup { \tiny \override #'(baseline-skip . 2.0) \center-column{ "SOPRANO" "[species 1]" } }
  \clef "treble"
  \key c
  \major
  \override Score.VoltaBracketSpanner #'outside-staff-priority = 100
  \override Score.VoltaBracketSpanner #'direction = -1
  \override Score.VoltaBracket #'edge-height = #'(0.0 . 0.0)
  \override Score.VoltaBracket #'font-name = #"New Century Schoolbook" 
  \override Score.VoltaBracket #'font-shape = #'bold 
  \override Score.VoltaBracket.thickness = #3
  \override Staff.OttavaBracket.thickness = #2
  \override Staff.PianoPedalBracket.thickness = #2
  \set Staff.pedalSustainStyle = #'mixed
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
    \consists "Horizontal_bracket_engraver"
    \override HorizontalBracket.thickness=3
    \override HorizontalBracket.#'bracket-flare = #'(0 . 0)
    \override HorizontalBracket.#'edge-height = #'(0.5 . 0.5)
    \override HorizontalBracket #'shorten-pair = #'(-0.3 . -0.3) 
    \override TrillSpanner.bound-details.left.text = ##f
  }

  { f''1
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
d''1
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
e''1
f''1
c''1
b'1
a'1
b'1
c''1
a'1

  }
}
  \new Lyrics \with { alignAboveContext = "staff3" } {
    \lyricmode {
      \override StanzaNumber.font-size = #-2
      \set stanza = #" Flags:"
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000)  1
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8     }
  }
\new Staff = "staff2" {
     
  \set Staff.instrumentName = \markup { \tiny \override #'(baseline-skip . 2.0) \center-column{ "ALTO" "[species 5]" } }
  \clef "treble"
  \key c
  \major
  \override Score.VoltaBracketSpanner #'outside-staff-priority = 100
  \override Score.VoltaBracketSpanner #'direction = -1
  \override Score.VoltaBracket #'edge-height = #'(0.0 . 0.0)
  \override Score.VoltaBracket #'font-name = #"New Century Schoolbook" 
  \override Score.VoltaBracket #'font-shape = #'bold 
  \override Score.VoltaBracket.thickness = #3
  \override Staff.OttavaBracket.thickness = #2
  \override Staff.PianoPedalBracket.thickness = #2
  \set Staff.pedalSustainStyle = #'mixed
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
    \consists "Horizontal_bracket_engraver"
    \override HorizontalBracket.thickness=3
    \override HorizontalBracket.#'bracket-flare = #'(0 . 0)
    \override HorizontalBracket.#'edge-height = #'(0.5 . 0.5)
    \override HorizontalBracket #'shorten-pair = #'(-0.3 . -0.3) 
    \override TrillSpanner.bound-details.left.text = ##f
  }

  { c''1
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
b'4
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
c''4
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
b'4
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
g'4
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
a'1
\set Staff.ottavation = \markup { \teeny "Rhythm repeat" } \override Staff.OttavaBracket.color = #(rgb-color 0.706 0.706 0.000)
f'1
\unset Staff.ottavation
\set Staff.ottavation = \markup { \teeny "Rhythm repeat" } \override Staff.OttavaBracket.color = #(rgb-color 0.706 0.706 0.000)
a'1
\unset Staff.ottavation
\set Staff.ottavation = \markup { \teeny "Rhythm repeat" } \override Staff.OttavaBracket.color = #(rgb-color 0.706 0.706 0.000)
g'1
\unset Staff.ottavation
\set Staff.ottavation = \markup { \teeny "Rhythm repeat" } \override Staff.OttavaBracket.color = #(rgb-color 0.706 0.706 0.000)
f'1
\unset Staff.ottavation
\set Staff.ottavation = \markup { \teeny "Rhythm repeat" } \override Staff.OttavaBracket.color = #(rgb-color 0.706 0.706 0.000)
e'1
\unset Staff.ottavation
\set Staff.ottavation = \markup { \teeny "Rhythm repeat" } \override Staff.OttavaBracket.color = #(rgb-color 0.706 0.706 0.000)
g'1
\unset Staff.ottavation
\set Staff.ottavation = \markup { \teeny "Rhythm repeat" } \override Staff.OttavaBracket.color = #(rgb-color 0.706 0.706 0.000)
f'1
\unset Staff.ottavation

  }
}
  \new Lyrics \with { alignAboveContext = "staff2" } {
    \lyricmode {
      \override StanzaNumber.font-size = #-2
      \set stanza = #" Flags:"
      \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.000 0.706 0.000)  3
        \with-color #(rgb-color 0.706 0.706 0.000)  2
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
      } } }8
 \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.000 0.706 0.000)  4
      } } }8
 \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
      } } }8
 \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 5
      } } }8
 \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000)  6
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000)  8
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 7
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000)  10
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 9
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000)  12
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 11
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000)  14
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 13
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000)  16
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 15
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000)  18
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 17
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 19
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8     }
  }
\new Staff = "staff1" {
     
  \set Staff.instrumentName = \markup { \tiny \override #'(baseline-skip . 2.0) \center-column{ "TENOR" "[species 5]" } }
  \clef "bass"
  \key c
  \major
  \override Score.VoltaBracketSpanner #'outside-staff-priority = 100
  \override Score.VoltaBracketSpanner #'direction = -1
  \override Score.VoltaBracket #'edge-height = #'(0.0 . 0.0)
  \override Score.VoltaBracket #'font-name = #"New Century Schoolbook" 
  \override Score.VoltaBracket #'font-shape = #'bold 
  \override Score.VoltaBracket.thickness = #3
  \override Staff.OttavaBracket.thickness = #2
  \override Staff.PianoPedalBracket.thickness = #2
  \set Staff.pedalSustainStyle = #'mixed
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
    \consists "Horizontal_bracket_engraver"
    \override HorizontalBracket.thickness=3
    \override HorizontalBracket.#'bracket-flare = #'(0 . 0)
    \override HorizontalBracket.#'edge-height = #'(0.5 . 0.5)
    \override HorizontalBracket #'shorten-pair = #'(-0.3 . -0.3) 
    \override TrillSpanner.bound-details.left.text = ##f
  }

  { a1
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
b4
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
c'4
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
b4
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
g4
c'1
\set Staff.ottavation = \markup { \teeny "Rhythm repeat" } \override Staff.OttavaBracket.color = #(rgb-color 0.706 0.706 0.000)
b1
\unset Staff.ottavation
\set Staff.ottavation = \markup { \teeny "Rhythm repeat" } \override Staff.OttavaBracket.color = #(rgb-color 0.706 0.706 0.000)
a1
\unset Staff.ottavation
\set Staff.ottavation = \markup { \teeny "Rhythm repeat" } \override Staff.OttavaBracket.color = #(rgb-color 0.706 0.706 0.000)
d'1
\unset Staff.ottavation
\set Staff.ottavation = \markup { \teeny "Rhythm repeat" } \override Staff.OttavaBracket.color = #(rgb-color 0.706 0.706 0.000)
c'1
\unset Staff.ottavation
\override Slur.color=#(rgb-color 0.706 0.706 0.000)
\set Staff.ottavation = \markup { \teeny "Rhythm repeat" } \override Staff.OttavaBracket.color = #(rgb-color 0.706 0.706 0.000)
b1
(
\unset Staff.ottavation
\set Staff.ottavation = \markup { \teeny "Rhythm repeat" } \override Staff.OttavaBracket.color = #(rgb-color 0.706 0.706 0.000)
g1
)
\unset Staff.ottavation
\set Staff.ottavation = \markup { \teeny "Rhythm repeat" } \override Staff.OttavaBracket.color = #(rgb-color 0.706 0.706 0.000)
c'1
\unset Staff.ottavation

  }
}
  \new Lyrics \with { alignAboveContext = "staff1" } {
    \lyricmode {
      \override StanzaNumber.font-size = #-2
      \set stanza = #" Flags:"
      \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.000 0.706 0.000)  21
        \with-color #(rgb-color 0.706 0.706 0.000)  20
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 22
      } } }8
 \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 23
      } } }8
 \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 24
      } } }8
 \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000)  25
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000)  27
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 26
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000)  29
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 28
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000)  31
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 30
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000)  33
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 32
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 36
        \with-color #(rgb-color 0.706 0.706 0.000)  35
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 34
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000)  38
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 37
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 39
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8     }
  }
\new Staff = "staff0" {
     
  \set Staff.instrumentName = \markup { \tiny \override #'(baseline-skip . 2.0) \center-column{ "BASS" "[c.f.]" } }
  \clef "bass"
  \key c
  \major
  \override Score.VoltaBracketSpanner #'outside-staff-priority = 100
  \override Score.VoltaBracketSpanner #'direction = -1
  \override Score.VoltaBracket #'edge-height = #'(0.0 . 0.0)
  \override Score.VoltaBracket #'font-name = #"New Century Schoolbook" 
  \override Score.VoltaBracket #'font-shape = #'bold 
  \override Score.VoltaBracket.thickness = #3
  \override Staff.OttavaBracket.thickness = #2
  \override Staff.PianoPedalBracket.thickness = #2
  \set Staff.pedalSustainStyle = #'mixed
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
    \consists "Horizontal_bracket_engraver"
    \override HorizontalBracket.thickness=3
    \override HorizontalBracket.#'bracket-flare = #'(0 . 0)
    \override HorizontalBracket.#'edge-height = #'(0.5 . 0.5)
    \override HorizontalBracket #'shorten-pair = #'(-0.3 . -0.3) 
    \override TrillSpanner.bound-details.left.text = ##f
  }

  { f1
g1
c1
d1
f1
g1
a1
g1
e1
f1

  }
}
  \new Staff = "staff7" \with {
     
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
  
    r8 r8 r8 r8 r8  r8 r8 r8 
\set Staff.ottavation = \markup { \teeny "Rhythm repeat" } 
\override Staff.OttavaBracket.color = #(rgb-color 0.706 0.706 0.000)
    r8 r8 r8 r8 r8 r8 r8 r8 
\unset Staff.ottavation
    r8 r8 r8 r8 r8 r8 r8 r8 
\myTS "2 x 3rds" #(rgb-color 0.706 0.706 0.000) \textSpannerDown
    r8 r8 r8 r8 r8 r8 r8 r8 
\startTextSpan
    r8 r8 r8 r8 r8 r8 r8 r8 
-\tweak #'stencil #(label "VII<->VII#" (rgb-color 1.000 0.000 0.000))\startGroup
\override Staff.PianoPedalBracket.color = #(rgb-color 0.706 0.706 0.000)
 \override Staff.SustainPedal #'stencil =
 #(lambda (grob) (grob-interpret-markup grob (markup
 #:with-color (rgb-color 0.706 0.706 0.000)
 #:lower 0.4
 (#:teeny "Rhythm repeat"))))
    r8 r8 r8 r8 r8 r8 r8 r8 
\stopTextSpan
\sustainOn

\override TrillSpanner.bound-details.left.text = \markup{ \raise #0.6 \teeny "Stagnation" }
  \override TrillSpanner.color = #(rgb-color 0.000 0.706 0.000)
    r8 r8 r8 r8 r8 r8 r8 r8 
\stopGroup
\startTrillSpan
    r8 r8 r8 r8 r8 r8 r8 r8 
\sustainOff
    
    r8 r8 r8 r8 r8 r8 r8 r8 
\stopTrillSpan
    r8 r8 r8 r8 r8 r8 r8 r8 
  }

\new Lyrics \with { alignAboveContext = "staff0" } {
    \lyricmode {
      \override StanzaNumber.font-size = #-2
      \set stanza = #" Flags:"
      \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000)  40
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8     }
  }
  \new Lyrics \with { alignBelowContext = "staff0" } {
    \lyricmode {
      \override StanzaNumber.font-size = #-2
      \set stanza = #" Harmony:"
      \override InstrumentName #'X-offset = #1
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
\concat { DTIII \raise #0.7 6 } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8   \markup{   \teeny 
  \pad-markup #0.4 
\concat { S \raise #0.7 6 } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8   \markup{   \teeny 
  \pad-markup #0.4 
\concat { T } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8   \markup{   \teeny 
  \pad-markup #0.4 
\concat { SII } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8   \markup{   \teeny 
  \pad-markup #0.4 
\concat { T \raise #0.7 6 } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8   \markup{   \teeny 
  \pad-markup #0.4 
\concat { DVII \raise #0.7 6 } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8   \markup{   \teeny 
  \pad-markup #0.4 
\concat { D \raise #0.7 6 } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8   \markup{   \teeny 
  \pad-markup #0.4 
\concat { T } }8
    }
  }
>>
\markup \wordwrap \tiny \bold {
  SOPRANO [bar 4, beat 1] note F
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  1 "-" "Melody:" "Melodic" "climax" "is" "repeated" "(in" "upper" "part)" ""

}
\markup \wordwrap \tiny \bold {
  ALTO [bar 1, beat 1] note C
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  2 "-" "Rhythm:" "Beginning" "without" "rest" "(without" "rest)" ""

}
\markup \smaller \wordwrap \with-color #(rgb-color 0.000 0.706 0.000) {
  3 "-" "Rhythm:" "Suspension" "(each" "3" "meas.)" ""

}
\markup \wordwrap \tiny \bold {
  ALTO [bar 2, beat 2] note C
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.000 0.706 0.000) {
  4 "-" "Melody:" "Melodic" "climax" "is" "repeated" "(in" "lower" "part)" ""

}
\markup \wordwrap \tiny \bold {
  ALTO [bar 2, beat 4] note G
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  5 "-" "Harmony:" "Consecutive" "5ths" "(parallel)" ""

}
\markup \wordwrap \tiny \bold {
  ALTO [bar 3, beat 1] note A
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  6 "-" "Rhythm:" "Non-permitted" "rhythm" "(whole" "inside)" ""

}
\markup \wordwrap \tiny \bold {
  ALTO [bar 4, beat 1] note F
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  7 "-" "Rhythm:" "Same" "rhythm" "in" "adjacent" "measures" "(same)" ""

}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  8 "-" "Rhythm:" "Non-permitted" "rhythm" "(whole" "inside)" ""

}
\markup \wordwrap \tiny \bold {
  ALTO [bar 5, beat 1] note A
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  9 "-" "Rhythm:" "Same" "rhythm" "in" "adjacent" "measures" "(same)" ""

}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  10 "-" "Rhythm:" "Non-permitted" "rhythm" "(whole" "inside)" ""

}
\markup \wordwrap \tiny \bold {
  ALTO [bar 6, beat 1] note G
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  11 "-" "Rhythm:" "Same" "rhythm" "in" "adjacent" "measures" "(same)" ""

}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  12 "-" "Rhythm:" "Non-permitted" "rhythm" "(whole" "inside)" ""

}
\markup \wordwrap \tiny \bold {
  ALTO [bar 7, beat 1] note F
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  13 "-" "Rhythm:" "Same" "rhythm" "in" "adjacent" "measures" "(same)" ""

}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  14 "-" "Rhythm:" "Non-permitted" "rhythm" "(whole" "inside)" ""

}
\markup \wordwrap \tiny \bold {
  ALTO [bar 8, beat 1] note E
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  15 "-" "Rhythm:" "Same" "rhythm" "in" "adjacent" "measures" "(same)" ""

}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  16 "-" "Rhythm:" "Non-permitted" "rhythm" "(whole" "inside)" ""

}
\markup \wordwrap \tiny \bold {
  ALTO [bar 9, beat 1] note G
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  17 "-" "Rhythm:" "Same" "rhythm" "in" "adjacent" "measures" "(same)" ""

}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  18 "-" "Rhythm:" "Non-permitted" "rhythm" "(whole" "inside)" ""

}
\markup \wordwrap \tiny \bold {
  ALTO [bar 10, beat 1] note F
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  19 "-" "Rhythm:" "Same" "rhythm" "in" "adjacent" "measures" "(same)" ""

}
\markup \wordwrap \tiny \bold {
  TENOR [bar 1, beat 1] note A
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  20 "-" "Rhythm:" "Beginning" "without" "rest" "(without" "rest)" ""

}
\markup \smaller \wordwrap \with-color #(rgb-color 0.000 0.706 0.000) {
  21 "-" "Rhythm:" "Suspension" "(each" "3" "meas.)" ""

}
\markup \wordwrap \tiny \bold {
  TENOR [bar 2, beat 1] note B
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  22 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" ""

}
\markup \wordwrap \tiny \bold {
  TENOR [bar 2, beat 2] note C
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  23 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" ""

}
\markup \wordwrap \tiny \bold {
  TENOR [bar 2, beat 3] note B
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  24 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" ""

}
\markup \wordwrap \tiny \bold {
  TENOR [bar 3, beat 1] note C
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  25 "-" "Rhythm:" "Non-permitted" "rhythm" "(whole" "inside)" ""

}
\markup \wordwrap \tiny \bold {
  TENOR [bar 4, beat 1] note B
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  26 "-" "Rhythm:" "Same" "rhythm" "in" "adjacent" "measures" "(same)" ""

}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  27 "-" "Rhythm:" "Non-permitted" "rhythm" "(whole" "inside)" ""

}
\markup \wordwrap \tiny \bold {
  TENOR [bar 5, beat 1] note A
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  28 "-" "Rhythm:" "Same" "rhythm" "in" "adjacent" "measures" "(same)" ""

}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  29 "-" "Rhythm:" "Non-permitted" "rhythm" "(whole" "inside)" ""

}
\markup \wordwrap \tiny \bold {
  TENOR [bar 6, beat 1] note D
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  30 "-" "Rhythm:" "Same" "rhythm" "in" "adjacent" "measures" "(same)" ""

}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  31 "-" "Rhythm:" "Non-permitted" "rhythm" "(whole" "inside)" ""

}
\markup \wordwrap \tiny \bold {
  TENOR [bar 7, beat 1] note C
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  32 "-" "Rhythm:" "Same" "rhythm" "in" "adjacent" "measures" "(same)" ""

}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  33 "-" "Rhythm:" "Non-permitted" "rhythm" "(whole" "inside)" ""

}
\markup \wordwrap \tiny \bold {
  TENOR [bar 8, beat 1] note B
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  34 "-" "Rhythm:" "Same" "rhythm" "in" "adjacent" "measures" "(same)" ""

}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  35 "-" "Rhythm:" "Non-permitted" "rhythm" "(whole" "inside)" ""

}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  36 "-" "Melody:" "No" "change" "of" "direction" "before" "the" "leap" "between" "measures:" ">1skip" "+" "close" "(3rd)" ""

}
\markup \wordwrap \tiny \bold {
  TENOR [bar 9, beat 1] note G
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  37 "-" "Rhythm:" "Same" "rhythm" "in" "adjacent" "measures" "(same)" ""

}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  38 "-" "Rhythm:" "Non-permitted" "rhythm" "(whole" "inside)" ""

}
\markup \wordwrap \tiny \bold {
  TENOR [bar 10, beat 1] note C
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  39 "-" "Rhythm:" "Same" "rhythm" "in" "adjacent" "measures" "(same)" ""

}
\markup \wordwrap \tiny \bold {
  BASS [bar 1, beat 1] note F
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  40 "-" "Harmony:" "Non-permitted" "first" "harmony" "(not" "T)" ""

}
\header {tagline = "This file was created by MGen CP2 v3.7-138-g7504d43-dirty at 2018-07-31 09:38:39"}
\markup { 
  \vspace #5
  \bold Legend:
} 

\markup {
  \line {
  \vspace #1
    \column {
      \char ##x2461
      \bold 4:3
 \raise #0.3 \teeny \override #`(direction . ,UP) { \dir-column {
        \with-color #(rgb-color 0.2 0.995 0.000) \circle 7
 } }
    \hspace #5
      
 \raise #0.3 \teeny \override #`(direction . ,UP) { \dir-column {
        \with-color #(rgb-color 0.988 0.005 0.000) \underline \circle 8
 } }
    \hspace #5
      \raise #0.9 \circle \note #"1" #1
      \raise #0.9 \musicglyph #"noteheads.s2xcircle"
      \raise #0.9 \musicglyph #"noteheads.s0harmonic"
    }
    \hspace #5
    \column {
      "Voice number (2) is shown before note"
      "For each note bar number (4) and croche beat (3) is shown"
      "This note violates minor rule (green). If underlined, means that violation"
      "is shown in the score with shape of same color"
      "This note violates major rule (red). If underlined, means that violation"
      "is shown in the score with shape of same color"
      "This note was corrected"
      "This note is a dissonance"
      "This note is considered passing or auxiliary"
    }
  }
} 


