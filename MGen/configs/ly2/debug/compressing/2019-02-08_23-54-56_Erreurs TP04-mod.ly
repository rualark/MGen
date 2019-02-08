\version "2.18.2"
\language "english"
\paper { 
  #(include-special-characters) 
  bottom-margin = 0.27\in
  #(define page-breaking ly:optimal-breaking)
}
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
  subtitle = "Multivoice Counterpoint Analysis: Erreurs TP04-mod (2019-02-08 23:54)"
  dedication = ""
}

\markup {
      \vspace #1

}
\markup {
      \vspace #1

}
\markup \wordwrap {
  \bold {
"#"1 (from Erreurs TP04-mod) Key: F major
}
\tiny { 
"Text:" "3" "paliers" "mélodiques" "distincts.
Ne" "peut" "être" "bon" "à" "une" "voix" "externe." "7e" "doublée" "Un" "exercice" "scolastique
débute" "" "toujours."
}
\tiny { 
"Lyrics:" "6."
}

}
<<
\new Staff = "staff3" {
     
  \set Staff.instrumentName = \markup { \teeny \override #'(baseline-skip . 2.0) \center-column{ "Part"  "[species 1]"  "[Soprano]" } }
  \clef "treble"
  \key f
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

  { a'1
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
bf'1
a'1
f''1
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
e''1
\staccato
\override Slur.color=#(rgb-color 0.706 0.706 0.000)
f''1
(
bf'1~
)
bf'1
a'1

  }
}
  \new Lyrics \with { alignAboveContext = "staff3" } {
    \lyricmode {
      \override StanzaNumber.font-size = #-2
      \set stanza = #" Flags:"
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000)  1
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 3
        \with-color #(rgb-color 1.000 0.000 0.000)  2
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8     }
  }
\new Staff = "staff2" {
     
  \set Staff.instrumentName = \markup { \teeny \override #'(baseline-skip . 2.0) \center-column{ "Part (2)"  "[species 3]"  "[Alto]" } }
  \clef "treble"
  \key f
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
c'4
\glissando
 \override NoteColumn.glissando-skip = ##t
f'4
\circle
e'4
f'4
\override NoteColumn.glissando-skip = ##f
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
g'4
\override HorizontalBracket.color=#(rgb-color 0.706 0.706 0.000)
\circle
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
a'4
-\tweak #'stencil #(label "m2 - unis." (rgb-color 0.706 0.706 0.000))\startGroup
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
bf'4
\stopGroup
g'4
f'4
\circle
e'4
d'4
\circle
e'4
f'4
\circle
g'4
a'4
f'4
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
c''4
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
bf'4
\circle
a'4
\override Slur.color=#(rgb-color 0.706 0.706 0.000)
\override Staff.PianoPedalBracket.color = #(rgb-color 0.706 0.706 0.000)
 \override Staff.SustainPedal #'stencil =
 #(lambda (grob) (grob-interpret-markup grob (markup
 #:with-color (rgb-color 0.706 0.706 0.000)
 #:lower 0.4
 (#:teeny "tri"))))
 \textSpannerDown
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
bf'4
\sustainOn
(
d''4
)
\sustainOff
\circle
c''4
\circle
bf'4
\circle
a'4
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
g'4
\circle
f'4
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
g'4
d'4
e'4
\staccato
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
c'4
\circle
d'4
e'4
\staccato
f'1

  }
}
  \new Lyrics \with { alignAboveContext = "staff2" } {
    \lyricmode {
      \override StanzaNumber.font-size = #-2
      \set stanza = #" Flags:"
      \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000)  4
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 5
      } } }8
 \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 7
        \with-color #(rgb-color 0.706 0.706 0.000)  6
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000)  8
      } } }8
 \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 9
      } } }8
 \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 12
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 11
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 10
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000)  13
      } } }8
 \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000)  14
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000)  15
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8     }
  }
\new Staff = "staff1" {
     
  \set Staff.instrumentName = \markup { \teeny \override #'(baseline-skip . 2.0) \center-column{ "Part (3)"  "[species 1]"  "[Tenor]" } }
  \clef "treble_8"
  \key f
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
f1
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
c'1
d'1
c'1
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
bf1
a1
bf1
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
c'1~
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
c'1

  }
}
  \new Lyrics \with { alignAboveContext = "staff1" } {
    \lyricmode {
      \override StanzaNumber.font-size = #-2
      \set stanza = #" Flags:"
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 16
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000)  17
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8     }
  }
\new Staff = "staff0" {
     
  \set Staff.instrumentName = \markup { \teeny \override #'(baseline-skip . 2.0) \center-column{ "Part (4)"  "[c.f.]"  "[Bass]" } }
  \clef "bass"
  \key f
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

  { \override TrillSpanner.bound-details.left.text = \markup{ \raise #0.6 \teeny "Many tonics" }
  \override TrillSpanner.color = #(rgb-color 0.706 0.706 0.000)
f1
\startTrillSpan
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
e1
\staccato
f1
a1
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
g1
f1
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
d1
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
g1
f1
\stopTrillSpan

  }
}
  \new Lyrics \with { alignAboveContext = "staff0" } {
    \lyricmode {
      \override StanzaNumber.font-size = #-2
      \set stanza = #" Flags:"
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 18
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 19
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000)  20
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 21
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 22
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 23
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
  \on-color #(rgb-color 0.941 0.941 0.000)   \pad-markup #0.4 
  \concat { 
    \general-align #Y #0.5 {D}
    \teeny
    \override #'(baseline-skip . 1.5) 
    \override #'(line-width . 100)  
    \center-column{ 6 5 } 
  }
}8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8   \markup{   \teeny 
  \pad-markup #0.4 
\concat { TSVI \raise #0.7 6 } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8   \markup{   \teeny 
  \pad-markup #0.4 
\concat { T \raise #0.7 6 } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8   \markup{   \teeny 
  \on-color #(rgb-color 0.941 0.941 0.000)   \pad-markup #0.4 
  \concat { 
    \general-align #Y #0.5 {D}
    \teeny
    \override #'(baseline-skip . 1.5) 
    \override #'(line-width . 100)  
    \center-column{ 4 3 } 
  }
}8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8   \markup{   \teeny 
  \pad-markup #0.4 
\concat { TSVI \raise #0.7 6 } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8   \markup{   \teeny 
  \on-color #(rgb-color 1.000 0.627 0.627)   \pad-markup #0.4 
  \concat { 
    \general-align #Y #0.5 {SII}
    \teeny
    \override #'(baseline-skip . 1.5) 
    \override #'(line-width . 100)  
    \center-column{ 6 4 } 
  }
}8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8   \markup{   \teeny 
  \on-color #(rgb-color 0.941 0.941 0.000)   \pad-markup #0.4 
  \concat { 
    \general-align #Y #0.5 {D}
    \teeny
    \override #'(baseline-skip . 1.5) 
    \override #'(line-width . 100)  
    \center-column{ 4 3 } 
  }
}8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8   \markup{   \teeny 
  \pad-markup #0.4 
\concat { T } }8
    }
  }
>>
\markup \wordwrap \tiny \bold {
  Part [bar 2, beat 1] note B\raise #0.3 \magnify #0.7 \flat 
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  1 "-" "Harmony:" "Harmonic" "tritone" "(with" "bass)" "-" "with" "Part" "(4)"

}
\markup \wordwrap \tiny \bold {
  Part [bar 6, beat 1] note F
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  2 "-" "Melody:" "Melodic" "climax" "is" "repeated" "(in" "upper" "part)"

}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  3 "-" "Melody:" "Uncompensated" "leap" "(5th)"

}
\markup \wordwrap \tiny \bold {
  Part (2) [bar 1, beat 1] note C
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  4 "-" "Rhythm:" "No" "starting" "rest"

}
\markup \wordwrap \tiny \bold {
  Part (2) [bar 2, beat 1] note G
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  5 "-" "Harmony:" "Doubling" "of" "a" "tritone" "note" "-" "with" "Part"

}
\markup \wordwrap \tiny \bold {
  Part (2) [bar 2, beat 3] note B\raise #0.3 \magnify #0.7 \flat 
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  6 "-" "Harmony:" "Harmonic" "tritone" "(with" "bass)" "-" "with" "Part" "(4)"

}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  7 "-" "Harmony:" "Unison" "(follows" "m2)" "-" "with" "Part"

}
\markup \wordwrap \tiny \bold {
  Part (2) [bar 5, beat 1] note C
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  8 "-" "Harmony:" "Harmonic" "4th" "(with" "bass)" "-" "with" "Part" "(4)"

}
\markup \wordwrap \tiny \bold {
  Part (2) [bar 5, beat 2] note B\raise #0.3 \magnify #0.7 \flat 
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  9 "-" "Harmony:" "Doubling" "of" "a" "tritone" "note" "-" "with" "Part" "(3)"

}
\markup \wordwrap \tiny \bold {
  Part (2) [bar 5, beat 4] note B\raise #0.3 \magnify #0.7 \flat 
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  10 "-" "Harmony:" "Unresolved" "harmonic" "tritone" "-" "with" "Part"

}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  11 "-" "Harmony:" "Doubling" "of" "a" "tritone" "note" "-" "with" "Part" "(3)"

}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  12 "-" "Melody:" "No" "change" "of" "direction" "before" "the" "leap" "between" "measures:" "1skip" "+" "close" "(3rd)"

}
\markup \wordwrap \tiny \bold {
  Part (2) [bar 7, beat 1] note G
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  13 "-" "Harmony:" "Harmonic" "4th" "(with" "bass)" "-" "with" "Part" "(4)"

}
\markup \wordwrap \tiny \bold {
  Part (2) [bar 7, beat 3] note G
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  14 "-" "Harmony:" "Harmonic" "4th" "(with" "bass)" "-" "with" "Part" "(4)"

}
\markup \wordwrap \tiny \bold {
  Part (2) [bar 8, beat 2] note C
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  15 "-" "Harmony:" "Harmonic" "4th" "(with" "bass)" "-" "with" "Part" "(4)"

}
\markup \wordwrap \tiny \bold {
  Part (3) [bar 2, beat 1] note C
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  16 "-" "Harmony:" "Parallel" "5ths:" "<" "measure" "apart" "(downbeat)" "-" "with" "Part" "(2)"

}
\markup \wordwrap \tiny \bold {
  Part (3) [bar 8, beat 1] note C
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  17 "-" "Harmony:" "Harmonic" "4th" "(with" "bass)" "-" "with" "Part" "(4)"

}
\markup \wordwrap \tiny \bold {
  Part (4) [bar 2, beat 1] note E
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  18 "-" "Harmony:" "7th" "chord"

}
\markup \wordwrap \tiny \bold {
  Part (4) [bar 5, beat 1] note G
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  19 "-" "Harmony:" "7th" "chord"

}
\markup \wordwrap \tiny \bold {
  Part (4) [bar 6, beat 1] note F
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  20 "-" "Melody:" "Frequent" "return" "to" "the" "tonic" "(3" "within" "6)"

}
\markup \wordwrap \tiny \bold {
  Part (4) [bar 7, beat 1] note D
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  21 "-" "Harmony:" "Six-four" "chord" "(real)"

}
\markup \wordwrap \tiny \bold {
  Part (4) [bar 8, beat 1] note G
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  22 "-" "Harmony:" "7th" "chord"

}
\markup \wordwrap \tiny \bold {
  Part (4) [bar 9, beat 1] note F
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  23 "-" "Melody:" "Frequent" "return" "to" "the" "tonic" "(4" "within" "12)"

}
\markup {
      \vspace #1

}
\markup {
      \vspace #1

}
\markup \wordwrap {
  \bold {
"#"2 (from Erreurs TP04-mod) Key: G dorian
}
\tiny { 
"Text:" "très" "dur" "peut" "passer."
}

}
<<
\new Staff = "staff3" {
     
  \set Staff.instrumentName = \markup { \teeny \override #'(baseline-skip . 2.0) \center-column{ "Part"  "[species 1]"  "[Soprano]" } }
  \clef "treble"
  \key f
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

  { \once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
bf'1
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
a'1
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
g'1
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
f'1
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
g'1

  }
}
\new Staff = "staff2" {
     
  \set Staff.instrumentName = \markup { \teeny \override #'(baseline-skip . 2.0) \center-column{ "Part (2)"  "[c.f.]"  "[Alto]" } }
  \clef "treble"
  \key f
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

  { \override TrillSpanner.bound-details.left.text = \markup{ \raise #0.6 \teeny "Parallel 4ths" }
  \override TrillSpanner.color = #(rgb-color 0.706 0.706 0.000)
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
f'1
\startTrillSpan
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
e'1
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
d'1
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
c'1
\stopTrillSpan
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
bf1

  }
}
  \new Lyrics \with { alignAboveContext = "staff2" } {
    \lyricmode {
      \override StanzaNumber.font-size = #-2
      \set stanza = #" Flags:"
      \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 1
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000)  2
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8     }
  }
\new Staff = "staff1" {
     
  \set Staff.instrumentName = \markup { \teeny \override #'(baseline-skip . 2.0) \center-column{ "Part (3)"  "[species 1]"  "[Tenor]" } }
  \clef "treble_8"
  \key f
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

  { \once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
f1~
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
f1
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
bf1
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
a1
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
g1

  }
}
  \new Lyrics \with { alignAboveContext = "staff1" } {
    \lyricmode {
      \override StanzaNumber.font-size = #-2
      \set stanza = #" Flags:"
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 3
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000)  4
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 5
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8     }
  }
\new Staff = "staff0" {
     
  \set Staff.instrumentName = \markup { \teeny \override #'(baseline-skip . 2.0) \center-column{ "Part (4)"  "[species 3]"  "[Bass]" } }
  \clef "bass"
  \key f
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

  { d4
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
f4
\circle
e4
\circle
d4
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
c4
\circle
d4
\circle
e4
\circle
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
f4
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
g4
bf,4
\circle
c4
\circle
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
d4
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
e4
f4
a4
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\circle
g4
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
f4
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
d4
\circle
e4
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
f4

  }
}
  \new Lyrics \with { alignAboveContext = "staff0" } {
    \lyricmode {
      \override StanzaNumber.font-size = #-2
      \set stanza = #" Flags:"
      \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000)  7
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 6
      } } }8
 \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000)  8
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000)  10
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 9
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000)  11
      } } }8
 \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 12
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 14
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 13
      } } }8
 \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000)  15
      } } }8
 \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000)  17
        \with-color #(rgb-color 0.706 0.706 0.000)  16
      } } }8
 \skip 8     }
  }
  \new Lyrics \with { alignBelowContext = "staff0" } {
    \lyricmode {
      \override StanzaNumber.font-size = #-2
      \set stanza = #" Harmony:"
      \override InstrumentName #'font-series = #'bold
      \override InstrumentName.font-size = #-2
      \set shortVocalName = "H:"
  \markup{   \teeny 
  \on-color #(rgb-color 0.941 0.941 0.000)   \pad-markup #0.4 
\concat { DTIII \raise #0.7 6 } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8   \markup{   \teeny 
  \on-color #(rgb-color 0.941 0.941 0.000)   \pad-markup #0.4 
  \concat { 
    \general-align #Y #0.5 {DVII}
    \teeny
    \override #'(baseline-skip . 1.5) 
    \override #'(line-width . 100)  
    \center-column{ 4 3 } 
  }
}8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8   \markup{   \teeny 
  \pad-markup #0.4 
\concat { T \raise #0.7 6 } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8   \markup{   \teeny 
  \on-color #(rgb-color 0.941 0.941 0.000)   \pad-markup #0.4 
  \concat { 
    \general-align #Y #0.5 {DVII}
    \teeny
    \override #'(baseline-skip . 1.5) 
    \override #'(line-width . 100)  
    \center-column{ 4 2 } 
  }
}8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8   \markup{   \teeny 
  \on-color #(rgb-color 0.941 0.941 0.000)   \pad-markup #0.4 
  \concat { 
    \general-align #Y #0.5 {T}
    \teeny
    \override #'(baseline-skip . 1.5) 
    \override #'(line-width . 100)  
    \center-column{ 4 3 } 
  }
}8
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
\myTS "overlap" #(rgb-color 0.706 0.706 0.000) \textSpannerDown
r8
\startTextSpan
r8
r8
\stopTextSpan
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

  }
>>
\markup \wordwrap \tiny \bold {
  Part (2) [bar 1, beat 1] note E"#"
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  1 "-" "Harmony:" "Too" "many" "consecutive" "4ths" "(>" "3)" "-" "with" "Part"

}
\markup \wordwrap \tiny \bold {
  Part (2) [bar 5, beat 1] note A"#"
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  2 "-" "Harmony:" "Harmonic" "4th" "(with" "bass)" "-" "with" "Part" "(4)"

}
\markup \wordwrap \tiny \bold {
  Part (3) [bar 3, beat 1] note A"#"
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  3 "-" "Harmony:" "Voice" "overlapping" "(adjacent" "voices)" "-" "with" "Part" "(4)"

}
\markup \wordwrap \tiny \bold {
  Part (3) [bar 4, beat 1] note A
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  4 "-" "Harmony:" "Harmonic" "4th" "(with" "bass)" "-" "with" "Part" "(4)"

}
\markup \wordwrap \tiny \bold {
  Part (3) [bar 5, beat 1] note G
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  5 "-" "Harmony:" "Voice" "overlapping" "(direct" "2nd)" "-" "with" "Part" "(4)"

}
\markup \wordwrap \tiny \bold {
  Part (4) [bar 1, beat 1] note D
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  6 "-" "Harmony:" "Non-permitted" "first" "harmony" "(not" "T)"

}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  7 "-" "Rhythm:" "No" "starting" "rest"

}
\markup \wordwrap \tiny \bold {
  Part (4) [bar 1, beat 2] note E"#"
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  8 "-" "Harmony:" "Harmonic" "4th" "(with" "bass)" "-" "with" "Part"

}
\markup \wordwrap \tiny \bold {
  Part (4) [bar 2, beat 1] note C
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  9 "-" "Harmony:" "7th" "chord"

}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  10 "-" "Harmony:" "Harmonic" "4th" "(with" "bass)" "-" "with" "Part" "(3)"

}
\markup \wordwrap \tiny \bold {
  Part (4) [bar 3, beat 4] note D
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  11 "-" "Harmony:" "Harmonic" "4th" "(with" "bass)" "-" "with" "Part"

}
\markup \wordwrap \tiny \bold {
  Part (4) [bar 4, beat 1] note E
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  12 "-" "Harmony:" "7th" "chord"

}
\markup \wordwrap \tiny \bold {
  Part (4) [bar 5, beat 1] note E"#"
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  13 "-" "Harmony:" "Non-permitted" "last" "harmony" "(not" "T)"

}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  14 "-" "Harmony:" "7th" "chord"

}
\markup \wordwrap \tiny \bold {
  Part (4) [bar 5, beat 2] note D
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  15 "-" "Harmony:" "Harmonic" "4th" "(with" "bass)" "-" "with" "Part" "(3)"

}
\markup \wordwrap \tiny \bold {
  Part (4) [bar 5, beat 4] note E"#"
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  16 "-" "Harmony:" "Harmonic" "4th" "(with" "bass)" "-" "with" "Part" "(2)"

}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  17 "-" "Rhythm:" "Last" "note" "is" "not" "a" "whole"

}
\markup {
      \vspace #1

}
\markup {
      \vspace #1

}
\markup \wordwrap {
  \bold {
"#"3 (from Erreurs TP04-mod) Key: F major
}
\tiny { 
"Text:" "Quelle" "est" "donc" "la" "règle" "générale" "concernant" "les" "renversements?

rép.:________________________________________________

" "" "" "" "" "" "" "________________________________________________" "impression" "de" "paliers,
dû" "au" "saut" "comp" "trop" "tard."
}

}
<<
\new Staff = "staff3" {
     
  \set Staff.instrumentName = \markup { \teeny \override #'(baseline-skip . 2.0) \center-column{ "Part"  "[species 3]"  "[Soprano]" } }
  \clef "treble"
  \key f
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

  { r4
a'4
\circle
g'4
a'4
bf'4
d''4
\circle
c''4
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\circle
bf'4
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
a'4
\circle
g'4
a'4
\circle
bf'4
c''4
f''4
\circle
g''4
a''4
g''4
e''4
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
f''4
\glissando
 \override NoteColumn.glissando-skip = ##t
\override Staff.PianoPedalBracket.color = #(rgb-color 0.706 0.706 0.000)
 \override Staff.SustainPedal #'stencil =
 #(lambda (grob) (grob-interpret-markup grob (markup
 #:with-color (rgb-color 0.706 0.706 0.000)
 #:lower 0.4
 (#:teeny "vocal range"))))
 \textSpannerDown
bf''4
\sustainOn
\override NoteColumn.glissando-skip = ##f
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
a''4
\sustainOff
\glissando
 \override NoteColumn.glissando-skip = ##t
\override TrillSpanner.bound-details.left.text = \markup{ \raise #0.6 \teeny "Stagnation" }
  \override TrillSpanner.color = #(rgb-color 0.706 0.706 0.000)
\circle
g''4
\startTrillSpan
\circle
f''4
\circle
e''4
\override NoteColumn.glissando-skip = ##f
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
d''4
\circle
e''4
\circle
f''4
g''4
\circle
f''4
e''4
\staccato
\circle
d''4
e''4
\staccato
f''1
\stopTrillSpan

  }
}
  \new Lyrics \with { alignAboveContext = "staff3" } {
    \lyricmode {
      \override StanzaNumber.font-size = #-2
      \set stanza = #" Flags:"
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 1
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 3
        \with-color #(rgb-color 0.706 0.706 0.000)  2
      } } }8
 \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 4
      } } }8
 \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 5
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000)  7
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 6
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000)  8
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8     }
  }
\new Staff = "staff2" {
     
  \set Staff.instrumentName = \markup { \teeny \override #'(baseline-skip . 2.0) \center-column{ "Part (2)"  "[species 1]"  "[Alto]" } }
  \clef "treble"
  \key f
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

  { f'1
g'1
f'1~
f'1
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
d''1
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
c''1
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
d''1
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
bf'1
a'1

  }
}
  \new Lyrics \with { alignAboveContext = "staff2" } {
    \lyricmode {
      \override StanzaNumber.font-size = #-2
      \set stanza = #" Flags:"
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 9
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8     }
  }
\new Staff = "staff1" {
     
  \set Staff.instrumentName = \markup { \teeny \override #'(baseline-skip . 2.0) \center-column{ "Part (3)"  "[species 1]"  "[Tenor]" } }
  \clef "treble_8"
  \key f
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

  { f1
c'1
d'1
c'1
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\override TrillSpanner.bound-details.left.text = \markup{ \raise #0.6 \teeny "Parallel 3rds" }
  \override TrillSpanner.color = #(rgb-color 0.706 0.706 0.000)
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
bf1
\startTrillSpan
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
a1
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
bf1
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
g1
\stopTrillSpan
f1

  }
}
  \new Lyrics \with { alignAboveContext = "staff1" } {
    \lyricmode {
      \override StanzaNumber.font-size = #-2
      \set stanza = #" Flags:"
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 10
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 11
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8     }
  }
\new Staff = "staff0" {
     
  \set Staff.instrumentName = \markup { \teeny \override #'(baseline-skip . 2.0) \center-column{ "Part (4)"  "[c.f.]"  "[Bass]" } }
  \clef "bass"
  \key f
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

  { f1
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
e1
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
d1
\override Glissando.color=#(rgb-color 0.706 0.706 0.000)
a,1
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
\myTS "Consecutive leaps" #(rgb-color 1.000 0.000 0.000) \textSpannerDown
bf,1
\startTextSpan
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
d1
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
g,1
c1
f,1
\stopTextSpan

  }
}
  \new Lyrics \with { alignAboveContext = "staff0" } {
    \lyricmode {
      \override StanzaNumber.font-size = #-2
      \set stanza = #" Flags:"
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 13
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 12
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 14
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 17
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 16
        \with-color #(rgb-color 0.706 0.706 0.000)  15
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 18
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 19
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
  \on-color #(rgb-color 0.941 0.941 0.000)   \pad-markup #0.4 
\concat { T } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8   \markup{   \teeny 
  \on-color #(rgb-color 1.000 0.627 0.627)   \pad-markup #0.4 
\concat { ? } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8   \markup{   \teeny 
  \pad-markup #0.4 
\concat { TSVI } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8   \markup{   \teeny 
  \pad-markup #0.4 
\concat { T \raise #0.7 6 } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8   \markup{   \teeny 
  \on-color #(rgb-color 1.000 0.627 0.627)   \pad-markup #0.4 
\concat { ? } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8   \markup{   \teeny 
  \on-color #(rgb-color 0.941 0.941 0.000)   \pad-markup #0.4 
\concat { TSVI \raise #0.7 7 } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8   \markup{   \teeny 
  \pad-markup #0.4 
\concat { SII } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8   \markup{   \teeny 
  \on-color #(rgb-color 0.941 0.941 0.000)   \pad-markup #0.4 
\concat { D \raise #0.7 7 } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8   \markup{   \teeny 
  \pad-markup #0.4 
\concat { T } }8
    }
  }
>>
\markup \wordwrap \tiny \bold {
  Part [bar 3, beat 1] note A
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  1 "-" "Harmony:" "Similar" "movement" "to" "5th" "in" "outer" "voices" "(downbeat," "not" "cadence)" "-" "with" "Part" "(4)"

}
\markup \wordwrap \tiny \bold {
  Part [bar 5, beat 4] note B\raise #0.3 \magnify #0.7 \flat 
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  2 "-" "Harmony:" "Parallel" "8ves" "/" "unisons:" "<" "measure" "apart" "(upbeat)" "-" "with" "Part" "(4)"

}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  3 "-" "Melody:" "Out" "of" "vocal" "range"

}
\markup \wordwrap \tiny \bold {
  Part [bar 6, beat 1] note A
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  4 "-" "Harmony:" "Parallel" "5ths:" "<" "measure" "apart" "(downbeat)" "-" "with" "Part" "(4)"

}
\markup \wordwrap \tiny \bold {
  Part [bar 6, beat 2] note G
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  5 "-" "Melody:" "Stagnation" "(12" "notes" "<5th)"

}
\markup \wordwrap \tiny \bold {
  Part [bar 7, beat 1] note D
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  6 "-" "Harmony:" "Parallel" "5ths:" "<" "measure" "apart" "(downbeat)" "-" "with" "Part" "(4)"

}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  7 "-" "Harmony:" "Similar" "movement" "to" "5th" "in" "outer" "voices" "(downbeat," "not" "cadence)" "-" "with" "Part" "(4)"

}
\markup \wordwrap \tiny \bold {
  Part [bar 8, beat 1] note F
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  8 "-" "Harmony:" "Passing" "dissonance" "on" "downbeat"

}
\markup \wordwrap \tiny \bold {
  Part (2) [bar 7, beat 1] note D
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  9 "-" "Harmony:" "Unison" "(on" "downbeat)" "-" "with" "Part"

}
\markup \wordwrap \tiny \bold {
  Part (3) [bar 5, beat 1] note B\raise #0.3 \magnify #0.7 \flat 
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  10 "-" "Harmony:" "Too" "many" "consecutive" "3rds" "(>" "3)" "-" "with" "Part" "(2)"

}
\markup \wordwrap \tiny \bold {
  Part (3) [bar 6, beat 1] note A
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  11 "-" "Harmony:" "Consecutive" "8ves" "or" "unisons" "(parallel)" "-" "with" "Part"

}
\markup \wordwrap \tiny \bold {
  Part (4) [bar 2, beat 1] note E
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  12 "-" "Harmony:" "Convoluted" "harmony"

}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  13 "-" "Harmony:" "Two" "consecutive" "incomplete" "harmonies"

}
\markup \wordwrap \tiny \bold {
  Part (4) [bar 5, beat 1] note B\raise #0.3 \magnify #0.7 \flat 
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  14 "-" "Harmony:" "Convoluted" "harmony"

}
\markup \wordwrap \tiny \bold {
  Part (4) [bar 6, beat 1] note D
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  15 "-" "Harmony:" "Two" "consecutive" "incomplete" "harmonies"

}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  16 "-" "Harmony:" "Incomplete" "harmony" "(any" "-" "no" "3rd)"

}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  17 "-" "Harmony:" "7th" "chord"

}
\markup \wordwrap \tiny \bold {
  Part (4) [bar 8, beat 1] note C
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  18 "-" "Harmony:" "7th" "chord"

}
\markup \wordwrap \tiny \bold {
  Part (4) [bar 9, beat 1] note F
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  19 "-" "Melody:" "Consecutive" "leaps" "(more" "than" "3)"

}
\markup {
      \vspace #1

}
\markup {
      \vspace #1

}
\markup \wordwrap {
  \bold {
"#"4 (from Erreurs TP04-mod) Key: D minor
}
\tiny { 
"Text:" "Quelle" "est" "donc" "la" "règle" "générale" "concernant" "les" "renversements?" "Mélodie" "très" "musicale!" "renv" "très
maladroit."
}

}
<<
\new Staff = "staff3" {
     
  \set Staff.instrumentName = \markup { \teeny \override #'(baseline-skip . 2.0) \center-column{ "Part"  "[species 1]"  "[Soprano]" } }
  \clef "treble"
  \key f
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
d'1
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
a'1
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
bf'1
a'1
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
g'1
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
f'1
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
g'1
e'1
d'1

  }
}
  \new Lyrics \with { alignAboveContext = "staff3" } {
    \lyricmode {
      \override StanzaNumber.font-size = #-2
      \set stanza = #" Flags:"
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 1.000 0.000 0.000) \underline 1
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8     }
  }
\new Staff = "staff2" {
     
  \set Staff.instrumentName = \markup { \teeny \override #'(baseline-skip . 2.0) \center-column{ "Part (2)"  "[species 1]"  "[Alto]" } }
  \clef "treble"
  \key f
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

  { a1
cs'1
\staccato
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
d'1
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
f'1
\override HorizontalBracket.color=#(rgb-color 0.706 0.706 0.000)
\override Staff.PianoPedalBracket.color = #(rgb-color 0.706 0.706 0.000)
 \override Staff.SustainPedal #'stencil =
 #(lambda (grob) (grob-interpret-markup grob (markup
 #:with-color (rgb-color 0.706 0.706 0.000)
 #:lower 0.4
 (#:teeny "tri"))))
 \textSpannerDown
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
e'1
\sustainOn
-\tweak #'stencil #(label "VII not desc." (rgb-color 0.706 0.706 0.000))\startGroup
c'1
\sustainOff
\override Glissando.color=#(rgb-color 0.706 0.706 0.000)
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
bf1
\stopGroup
\glissando
 \override NoteColumn.glissando-skip = ##t
\override NoteColumn.glissando-skip = ##f
a1~
a1

  }
}
  \new Lyrics \with { alignAboveContext = "staff2" } {
    \lyricmode {
      \override StanzaNumber.font-size = #-2
      \set stanza = #" Flags:"
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000)  4
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 3
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 2
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 5
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8     }
  }
\new Staff = "staff1" {
     
  \set Staff.instrumentName = \markup { \teeny \override #'(baseline-skip . 2.0) \center-column{ "Part (3)"  "[c.f.]"  "[Tenor]" } }
  \clef "treble_8"
  \key f
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

  { f1
g1
\myTS "overlap" #(rgb-color 0.706 0.706 0.000) \textSpannerDown
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
f1
\startTextSpan
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
d'1
\stopTextSpan
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
bf1
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
a1
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
e1
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
g1
f1

  }
}
  \new Lyrics \with { alignAboveContext = "staff1" } {
    \lyricmode {
      \override StanzaNumber.font-size = #-2
      \set stanza = #" Flags:"
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 7
        \with-color #(rgb-color 0.706 0.706 0.000)  6
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 9
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 8
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000)  10
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000)  11
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000)  12
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8     }
  }
  \new Lyrics \with { alignBelowContext = "staff1" } {
    \lyricmode {
      \override StanzaNumber.font-size = #-2
      \set stanza = #" Note:"
      \override InstrumentName #'font-series = #'bold
      \override InstrumentName.font-size = #-2
      \set shortVocalName = "N:"
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8 \markup{ \teeny  \on-color #(rgb-color 0.941 0.941 0.000)  \pad-markup #0.4 \concat { B\raise #0.3 \magnify #0.5 \flat  } }
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8 \markup{ \teeny  \on-color #(rgb-color 0.941 0.941 0.000)  \pad-markup #0.4 \concat { E } }
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8     }
  }
\new Staff = "staff0" {
     
  \set Staff.instrumentName = \markup { \teeny \override #'(baseline-skip . 2.0) \center-column{ "Part (4)"  "[species 3]"  "[Bass]" } }
  \clef "bass"
  \key f
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

  { r4
\override Glissando.color=#(rgb-color 1.000 0.000 0.000)
d4
\glissando
 \override NoteColumn.glissando-skip = ##t
\circle
c4
\circle
bf,4
\override NoteColumn.glissando-skip = ##f
\override Slur.color=#(rgb-color 0.706 0.706 0.000)
a,4
(
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
e4
)
\circle
f4
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
e4
d4
\override HorizontalBracket.color=#(rgb-color 0.706 0.706 0.000)
\circle
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
e4
-\tweak #'stencil #(label "m2 - unis." (rgb-color 0.706 0.706 0.000))\startGroup
\circle
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
f4
\stopGroup
\circle
g4
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
a4
f4
\circle
g4
\circle
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
a4
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
bf4
\circle
a4
g4
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
d4
\override HorizontalBracket.color=#(rgb-color 0.706 0.706 0.000)
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
e4
-\tweak #'stencil #(label "VII not desc." (rgb-color 0.706 0.706 0.000))\startGroup
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
c4
\circle
bf,4
\stopGroup
\circle
a,4
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
g,4
\circle
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
a,4
\override Glissando.color=#(rgb-color 0.706 0.706 0.000)
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
bf,4
\glissando
 \override NoteColumn.glissando-skip = ##t
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
d4
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
cs4
\staccato
\override NoteColumn.glissando-skip = ##f
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
a,4
\circle
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
b,4
\once \override NoteHead.color = #(rgb-color 0.706 0.706 0.000)
\once \override Stem.color = #(rgb-color 0.706 0.706 0.000)
cs4
\staccato
d1

  }
}
  \new Lyrics \with { alignAboveContext = "staff0" } {
    \lyricmode {
      \override StanzaNumber.font-size = #-2
      \set stanza = #" Flags:"
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 14
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 13
      } } }8
 \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000)  15
      } } }8
 \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000)  16
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 18
        \with-color #(rgb-color 0.706 0.706 0.000)  17
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000)  19
      } } }8
 \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 20
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000)  21
      } } }8
 \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 23
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 22
      } } }8
 \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000)  24
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 25
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000)  26
      } } }8
 \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 27
      } } }8
 \skip 8       \markup{ \teeny \override #`(direction . ,UP) \override #'(baseline-skip . 1.6) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000) \underline 28
      } } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8     }
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
\concat { t } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8   \markup{   \teeny 
  \on-color #(rgb-color 0.941 0.941 0.000)   \pad-markup #0.4 
\concat { D \raise #0.7 7 } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8   \markup{   \teeny 
  \pad-markup #0.4 
\concat { tsVI \raise #0.7 6 } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8   \markup{   \teeny 
  \pad-markup #0.4 
\concat { t \raise #0.7 6 } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8   \markup{   \teeny 
  \on-color #(rgb-color 0.941 0.941 0.000)   \pad-markup #0.4 
  \concat { 
    \general-align #Y #0.5 { \raise #0.3 \magnify #0.5 \flat 5sII}
    \teeny
    \override #'(baseline-skip . 1.5) 
    \override #'(line-width . 100)  
    \center-column{ 4 2 } 
  }
}8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8   \markup{   \teeny 
  \on-color #(rgb-color 0.941 0.941 0.000)   \pad-markup #0.4 
  \concat { 
    \general-align #Y #0.5 {dtIII}
    \teeny
    \override #'(baseline-skip . 1.5) 
    \override #'(line-width . 100)  
    \center-column{ 6 5 } 
  }
}8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8   \markup{   \teeny 
  \on-color #(rgb-color 0.941 0.941 0.000)   \pad-markup #0.4 
  \concat { 
    \general-align #Y #0.5 { \raise #0.3 \magnify #0.5 \flat 5sII}
    \teeny
    \override #'(baseline-skip . 1.5) 
    \override #'(line-width . 100)  
    \center-column{ 6 5 } 
  }
}8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8   \markup{   \teeny 
  \on-color #(rgb-color 0.941 0.941 0.000)   \pad-markup #0.4 
\concat { D \raise #0.7 7 } }8
 \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8  \skip 8   \markup{   \teeny 
  \pad-markup #0.4 
\concat { t } }8
    }
  }
>>
\markup \wordwrap \tiny \bold {
  Part [bar 2, beat 1] note A
}
\markup \smaller \wordwrap \with-color #(rgb-color 1.000 0.000 0.000) {
  1 "-" "Harmony:" "Parallel" "8ves" "/" "unisons:" "<" "measure" "apart" "(downbeat)" "-" "with" "Part" "(4)"

}
\markup \wordwrap \tiny \bold {
  Part (2) [bar 5, beat 1] note E
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  2 "-" "Melody:" "Note" "not" "stepwise" "descending" "(VII" "natural)"

}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  3 "-" "Harmony:" "Unresolved" "harmonic" "tritone" "-" "with" "Part" "(3)"

}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  4 "-" "Harmony:" "Harmonic" "tritone" "(with" "bass)" "-" "with" "Part" "(4)"

}
\markup \wordwrap \tiny \bold {
  Part (2) [bar 7, beat 1] note B\raise #0.3 \magnify #0.7 \flat 
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  5 "-" "Harmony:" "Doubling" "of" "a" "tritone" "note" "-" "with" "Part" "(4)"

}
\markup \wordwrap \tiny \bold {
  Part (3) [bar 4, beat 1] note D
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  6 "-" "Harmony:" "Harmonic" "4th" "(with" "bass)" "-" "with" "Part" "(4)"

}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  7 "-" "Harmony:" "Voice" "overlapping" "(adjacent" "voices)" "-" "with" "Part" "(2)"

}
\markup \wordwrap \tiny \bold {
  Part (3) [bar 5, beat 1] note B\raise #0.3 \magnify #0.7 \flat 
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  8 "-" "Harmony:" "Doubling" "of" "a" "tritone" "note" "-" "with" "Part" "(4)"

}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  9 "-" "Melody:" "Audible" "tritone:" "framed" "(unresolved)"

}
\markup \wordwrap \tiny \bold {
  Part (3) [bar 6, beat 1] note A
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  10 "-" "Harmony:" "Harmonic" "4th" "(with" "bass)" "-" "with" "Part" "(4)"

}
\markup \wordwrap \tiny \bold {
  Part (3) [bar 7, beat 1] note E
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  11 "-" "Harmony:" "Harmonic" "tritone" "(with" "bass)" "-" "with" "Part" "(4)"

}
\markup \wordwrap \tiny \bold {
  Part (3) [bar 8, beat 1] note G
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  12 "-" "Harmony:" "Harmonic" "tritone" "(with" "bass)" "-" "with" "Part" "(4)"

}
\markup \wordwrap \tiny \bold {
  Part (4) [bar 2, beat 1] note A
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  13 "-" "Harmony:" "7th" "chord"

}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  14 "-" "Melody:" "Uncompensated" "leap:" "precompensated" "(5th)"

}
\markup \wordwrap \tiny \bold {
  Part (4) [bar 2, beat 2] note E
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  15 "-" "Harmony:" "Harmonic" "4th" "(with" "bass)" "-" "with" "Part"

}
\markup \wordwrap \tiny \bold {
  Part (4) [bar 2, beat 4] note E
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  16 "-" "Harmony:" "Harmonic" "4th" "(with" "bass)" "-" "with" "Part"

}
\markup \wordwrap \tiny \bold {
  Part (4) [bar 3, beat 3] note F
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  17 "-" "Harmony:" "Harmonic" "4th" "(with" "bass)" "-" "with" "Part"

}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  18 "-" "Harmony:" "Unison" "(follows" "m2)" "-" "with" "Part" "(3)"

}
\markup \wordwrap \tiny \bold {
  Part (4) [bar 4, beat 4] note A
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  19 "-" "Harmony:" "Harmonic" "4th" "(with" "bass)" "-" "with" "Part" "(3)"

}
\markup \wordwrap \tiny \bold {
  Part (4) [bar 5, beat 1] note B\raise #0.3 \magnify #0.7 \flat 
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  20 "-" "Harmony:" "7th" "chord"

}
\markup \wordwrap \tiny \bold {
  Part (4) [bar 5, beat 4] note D
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  21 "-" "Harmony:" "Harmonic" "4th" "(with" "bass)" "-" "with" "Part"

}
\markup \wordwrap \tiny \bold {
  Part (4) [bar 6, beat 1] note E
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  22 "-" "Melody:" "Note" "not" "stepwise" "descending" "(VII" "natural)"

}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  23 "-" "Harmony:" "7th" "chord"

}
\markup \wordwrap \tiny \bold {
  Part (4) [bar 6, beat 2] note C
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  24 "-" "Harmony:" "Harmonic" "4th" "(with" "bass)" "-" "with" "Part"

}
\markup \wordwrap \tiny \bold {
  Part (4) [bar 7, beat 1] note G
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  25 "-" "Harmony:" "7th" "chord"

}
\markup \wordwrap \tiny \bold {
  Part (4) [bar 7, beat 4] note D
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  26 "-" "Harmony:" "Harmonic" "4th" "(with" "bass)" "-" "with" "Part"

}
\markup \wordwrap \tiny \bold {
  Part (4) [bar 8, beat 1] note C"#"
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  27 "-" "Harmony:" "7th" "chord"

}
\markup \wordwrap \tiny \bold {
  Part (4) [bar 8, beat 2] note A
}
\markup \smaller \wordwrap \with-color #(rgb-color 0.706 0.706 0.000) {
  28 "-" "Harmony:" "Parallel" "8ves" "/" "unisons:" "<" "measure" "apart" "(upbeat)" "-" "with" "Part" "(2)"

}
\markup {
      \vspace #1

}
\header {tagline = \markup \tiny \center-column {"This file was created by MGen CA3 v4.0-119-g9d69a40-dirty at 2019-02-08 23:54:59." #(string-append "Engraved by LilyPond " (lilypond-version) ". File: Erreurs TP04-mod")  " "}}
\markup { 
  \vspace #5
  \bold \tiny Legend:
} 
\noPageBreak 
\markup {
  \line {
  \vspace #1
    \column {
 \raise #0.3 \teeny \override #`(direction . ,UP) { \dir-column {
        \with-color #(rgb-color 0.706 0.706 0.000) 7
 } }
    \hspace #5
      
 \raise #0.3 \teeny \override #`(direction . ,UP) { \dir-column {
        \with-color #(rgb-color 0.988 0.005 0.000) \underline 8
 } }
    \hspace #5
      %\raise #0.9 \circle \note #"1" #1
      %\raise #0.9 \musicglyph #"noteheads.s2xcircle"
      \raise #0.9 \circle \note #"1" #1
      %\raise #0.9 \musicglyph #"noteheads.s0harmonic"
    }
    \hspace #5
    \tiny \column {
      "This note violates a minor rule (yellow). If underlined, means that violation"
      "is shown in the score with shape of same color"
      "This note violates a major rule (red). If underlined, means that violation"
      "is shown in the score with shape of same color"
      %"This note was corrected"
      %"This note is a dissonance"
      "Circled note is considered non-chord tone (passing, auxiliary or other)"
      "or a chord tone in a similar melodic form"
    }
  }
} 
\noPageBreak 

\markup \tiny { \vspace #1.5
 Harmonic notation: Russian notation (detailed) }
