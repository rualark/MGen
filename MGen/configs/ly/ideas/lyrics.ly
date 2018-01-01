\version "2.18.2"
\language "english"
\paper { #(include-special-characters) }
circle =
\once \override NoteHead.stencil = #(lambda (grob)
    (let* ((note (ly:note-head::print grob))
           (combo-stencil (ly:stencil-add
               note
               (circle-stencil note 0.1 0.4))))
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

\header {
  subtitle = "GenCA2: test-ly-viz (2018-01-01 13:25)"
}

\markup \wordwrap \bold {
      \vspace #2
"#"1 (from test-ly-viz), Rule penalty: 603, Cantus: low, Key: Am
}
<<
\new Staff {
  \set Staff.instrumentName = \markup { \char ##x2460 }
  \clef "treble" \key a \minor  \accidentalStyle modern-cautionary
  \time 4/4
  % \set Score.barNumberVisibility = #(every-nth-bar-number-visible 4)
  % \override Score.BarNumber.break-visibility = ##(#f #t #t)
  \new Voice = "one" \with {
    \remove "Note_heads_engraver"
    \consists "Completion_heads_engraver"
    \remove "Rest_engraver"
    \consists "Completion_rest_engraver"
    \override Glissando.minimum-length = #5
    \override Glissando.springs-and-rods = #ly:spanner::set-spacing-rods
    \override Glissando.thickness = #'3
  }
  {  \override Glissando.color=#(rgb-color 0.592 0.269 0.000)  \speakOff \revert NoteHead.style a'1
_\markup{ \teeny \on-color #(rgb-color 1 1 1) \pad-markup #0.4 \concat { t }  }
^\markup{ \tiny \with-color #(rgb-color 0.988 0.005 0.000) \char ##x2605  }
 \glissando  \override Glissando.color=#(rgb-color 0.988 0.005 0.000)  \speakOff \revert NoteHead.style d'1
_\markup{ \teeny \on-color #(rgb-color 1 1 1) \pad-markup #0.4 \concat { b5sII }  }
 \glissando  \override Glissando.color=#(rgb-color 0.592 0.269 0.000)  \speakOff \revert NoteHead.style a1
_\markup{ \teeny \on-color #(rgb-color 1 1 1) \pad-markup #0.4 \concat { t }  }
 \glissando  \override Glissando.color=#(rgb-color 0.988 0.005 0.000)  \speakOff \revert NoteHead.style d'1
_\markup{ \teeny \on-color #(rgb-color 1 1 1) \pad-markup #0.4 \concat { s }  }
^\markup{ \tiny \with-color #(rgb-color 0.294 0.468 0.000) \char ##x2605  }
 \glissando  \speakOff \revert NoteHead.style a'1
_\markup{ \teeny \on-color #(rgb-color 0.598 0.900 0.500) \pad-markup #0.4 \concat { t }  }
^\markup{ \tiny \with-color #(rgb-color 0.000 0.667 0.000) \char ##x2605  }

  }
  \addlyrics {
    \teeny Let flee \repeat unfold 2 { \skip 1 } the hills a -- dorn, __
  }    
}
>>
\markup \wordwrap \bold {
  NOTE 1 at 1:1 - A
}
\markup \wordwrap \with-color #(rgb-color 0.988 0.005 0.000) {
  - Skipped notes (>1 joined)

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - Uncompensated leap (5th). If no other uncompensated rules worked.

}
\markup \wordwrap \bold {
  NOTE 2 at 2:1 - D
}
\markup \wordwrap \with-color #(rgb-color 0.988 0.005 0.000) {
  - Consecutive unidirectional leaps (Sum >6th)

}
\markup \wordwrap \bold {
  NOTE 3 at 3:1 - A
}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - Consecutive leaps, 1x3rd ignored (3)

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - Consecutive leaps, 1x3rd ignored (3)

}
\markup \wordwrap \with-color #(rgb-color 0.000 0.667 0.000) {
  - Uncompensated leap (4th). If no other uncompensated rules worked. (Possible in cantus)

}
\markup \wordwrap \bold {
  NOTE 4 at 4:1 - D
}
\markup \wordwrap \with-color #(rgb-color 0.294 0.468 0.000) {
  - 2nd to last note (not V/VIII/II). CF

}
\markup \wordwrap \with-color #(rgb-color 0.000 0.667 0.000) {
  - Penultimate note resolution (IV not to III) (IV penultimate note resolves not to III note)

}
\markup \wordwrap \with-color #(rgb-color 0.988 0.005 0.000) {
  - Consecutive unidirectional leaps (Sum >6th)

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - Uncompensated leap (5th). If no other uncompensated rules worked.

}
\markup \wordwrap \bold {
  NOTE 5 at 5:1 - A
}
\markup \wordwrap \with-color #(rgb-color 0.000 0.667 0.000) {
  - Culmination (multi low) (Multiple culminations in lower voice)

}
\markup \wordwrap \with-color #(rgb-color 0.196 0.533 0.000) {
  - Missing letters in a row (>4)

}
\markup \wordwrap \bold {
      \vspace #2
"#"1 (from test-ly-viz), Cantus: low, Species: 3, Rule penalty: 2300, Key: Am
}
<<
\new Staff {
  \set Staff.instrumentName = \markup { \char ##x2461 }
  \clef "treble^8" \key a \minor  \accidentalStyle modern-cautionary
  \time 4/4
  % \set Score.barNumberVisibility = #(every-nth-bar-number-visible 4)
  % \override Score.BarNumber.break-visibility = ##(#f #t #t)
  \new Voice \with {
    \remove "Note_heads_engraver"
    \consists "Completion_heads_engraver"
    \remove "Rest_engraver"
    \consists "Completion_rest_engraver"
  }

  { 
        \override Glissando.minimum-length = #5
    \override Glissando.springs-and-rods = #ly:spanner::set-spacing-rods
    \override Glissando.thickness = #'3
r4
_\markup{ \teeny \on-color #(rgb-color 1 1 1) \pad-markup #0.4 \concat { t }  }
 \speakOff \revert NoteHead.style a''4
^\markup{ \tiny \with-color #(rgb-color 0.988 0.005 0.000) \char ##x2605  }
 \speakOff \revert NoteHead.style b''4
 \override Glissando.color=#(rgb-color 0.988 0.005 0.000)  \speakOff \revert NoteHead.style a''4
 \glissando  \override Glissando.color=#(rgb-color 0.988 0.005 0.000)  \speakOff \revert NoteHead.style b'4
_\markup{ \teeny \on-color #(rgb-color 1 1 1) \pad-markup #0.4 \concat { b5sII \raise #0.7 6 }  }
 \glissando  \speakOff \revert NoteHead.style a''4
_\markup{ \teeny \on-color #(rgb-color 0.647 0.850 0.500) \pad-markup #0.4 \concat { s }  }
 \override Glissando.color=#(rgb-color 0.000 0.667 0.000)  \speakOff \revert NoteHead.style b''4
_\markup{ \teeny \on-color #(rgb-color 0.845 0.653 0.500) \pad-markup #0.4 \concat { b5sII \raise #0.7 6 }  }
 \glissando  \override Glissando.color=#(rgb-color 0.988 0.005 0.000)  \speakOff \revert NoteHead.style d'''4~
^\markup{ \teeny  \on-color #(rgb-color 0.796 0.702 0.500)  \pad-markup #0.4 8 }
 \override Glissando.color=#(rgb-color 0.988 0.005 0.000)  \speakOff \revert NoteHead.style d'''4
_\markup{ \teeny \on-color #(rgb-color 0.598 0.900 0.500) \pad-markup #0.4 \concat { t }  }
^\markup{ \teeny  \on-color #(rgb-color 0.994 0.504 0.500)  \pad-markup #0.4 4 }
 \glissando  \override Glissando.color=#(rgb-color 0.988 0.005 0.000)  \speakOff \revert NoteHead.style g'''4
^\markup{ \teeny  \on-color #(rgb-color 0.845 0.653 0.500)  \pad-markup #0.4 m7 }
 \glissando  \speakOff \revert NoteHead.style b''4
^\markup{ \teeny  \on-color #(rgb-color 0.796 0.702 0.500)  \pad-markup #0.4 M2 }
 \override Glissando.color=#(rgb-color 0.592 0.269 0.000)  \speakOff \revert NoteHead.style a''4
^\markup{ \teeny  \on-color #(rgb-color 0.796 0.702 0.500)  \pad-markup #0.4 8 }
 \glissando  \speakOff \revert NoteHead.style d'''4
_\markup{ \teeny \on-color #(rgb-color 0.647 0.850 0.500) \pad-markup #0.4 \concat { \char ##x00D0 VII \raise #0.7 64 }  }
^\markup{ \teeny  \on-color #(rgb-color 0.994 0.504 0.500)  \pad-markup #0.4 8 }
 \override Glissando.color=#(rgb-color 0.592 0.269 0.000)  \speakOff \revert NoteHead.style gs''4
^\markup{ \teeny  \on-color #(rgb-color 0.994 0.504 0.500)  \pad-markup #0.4 tri }
 \glissando  \speakOff \revert NoteHead.style d'''4
^\markup{ \tiny \with-color #(rgb-color 0.592 0.269 0.000) \char ##x2605  }
 \speakOff \revert NoteHead.style b''4
 \speakOff \revert NoteHead.style a''1
_\markup{ \teeny \on-color #(rgb-color 1 1 1) \pad-markup #0.4 \concat { t }  }

  }
  \addlyrics {
    \override LyricText.self-alignment-X = #LEFT
    \markup{ \teeny \on-color #(rgb-color 1 1 1) \pad-markup #0.4 \concat { t }  }
    \repeat unfold 2 { \skip 1 }
    \markup{ \teeny \on-color #(rgb-color 1 1 1) \pad-markup #0.4 \concat { b5sII \raise #0.7 6 }  }
    \markup{ \teeny \on-color #(rgb-color 0.647 0.850 0.500) \pad-markup #0.4 \concat { s }  }
    \markup{ \teeny \on-color #(rgb-color 0.845 0.653 0.500) \pad-markup #0.4 \concat { b5sII \raise #0.7 6 }  }
    \repeat unfold 1 { \skip 1 }
    \markup{ \teeny \on-color #(rgb-color 0.598 0.900 0.500) \pad-markup #0.4 \concat { t }  }
    \repeat unfold 2 { \skip 1 }
    \markup{ \teeny \on-color #(rgb-color 0.647 0.850 0.500) \pad-markup #0.4 \concat { \char ##x00D0 VII \raise #0.7 64 }  }
    \repeat unfold 3 { \skip 1 }
    \markup{ \teeny \on-color #(rgb-color 1 1 1) \pad-markup #0.4 \concat { t }  }
  }    
}
\new Staff {
  \set Staff.instrumentName = \markup { \char ##x2460 }
  \clef "treble" \key a \minor  \accidentalStyle modern-cautionary
  \time 4/4
  % \set Score.barNumberVisibility = #(every-nth-bar-number-visible 4)
  % \override Score.BarNumber.break-visibility = ##(#f #t #t)
  \new Voice \with {
    \remove "Note_heads_engraver"
    \consists "Completion_heads_engraver"
    \remove "Rest_engraver"
    \consists "Completion_rest_engraver"
  }

  {  
        \override Glissando.minimum-length = #5
    \override Glissando.springs-and-rods = #ly:spanner::set-spacing-rods
    \override Glissando.thickness = #'3
\speakOff \revert NoteHead.style a'1
_\markup{ \teeny \on-color #(rgb-color 1 1 1) \pad-markup #0.4 \concat { t }  }
\speakOff \revert NoteHead.style d'1
 \override Glissando.color=#(rgb-color 0.592 0.269 0.000)  \speakOff \revert NoteHead.style a1
 \glissando  \speakOff \revert NoteHead.style d'1
 \speakOff \revert NoteHead.style a'1

  }
  \addlyrics {
    \override LyricText.self-alignment-X = #LEFT
    \markup{ \teeny \on-color #(rgb-color 1 1 1) \pad-markup #0.4 \concat { t }  }
    \repeat unfold 1 { \skip 1 }
    \markup{ \teeny \on-color #(rgb-color 0.598 0.900 0.500) \pad-markup #0.4 \concat { t }  }
    \repeat unfold 1 { \skip 1 }
  }    
}
>>
\markup \wordwrap \bold {
  \char ##x2461 NOTE 1 at 1:3 - A
}
\markup \wordwrap \with-color #(rgb-color 0.690 0.204 0.000) {
  - Range (cp voice >M12)

}
\markup \wordwrap \with-color #(rgb-color 0.988 0.005 0.000) {
  - Skipped notes (>1 joined)

}
\markup \wordwrap \bold {
  \char ##x2461 NOTE 3 at 1:7 - A
}
\markup \wordwrap \with-color #(rgb-color 0.294 0.468 0.000) {
  - Leap down size (m7)

}
\markup \wordwrap \with-color #(rgb-color 0.988 0.005 0.000) {
  - Far + close MDC (>5th)

}
\markup \wordwrap \bold {
  \char ##x2461 NOTE 4 at 2:1 - B
}
\markup \wordwrap \with-color #(rgb-color 0.294 0.468 0.000) {
  - Leap up size (m7)

}
\markup \wordwrap \with-color #(rgb-color 0.988 0.005 0.000) {
  - Uncompensated leap (>5th). If no other uncompensated rules worked.

}
\markup \wordwrap \with-color #(rgb-color 0.988 0.005 0.000) {
  - Close + no MDC (>5th)

}
\markup \wordwrap \bold {
  \char ##x2461 NOTE 5 at 2:3 - A
}
\markup \wordwrap \with-color #(rgb-color 0.294 0.468 0.000) {
  - Multiple harmonies in measure (inside) (In measures before penultimate)

}
\markup \wordwrap \bold {
  \char ##x2461 NOTE 6 at 2:5 - B
}
\markup \wordwrap \with-color #(rgb-color 0.000 0.667 0.000) {
  - No close MDC (3rd). Except far+1far

}
\markup \wordwrap \with-color #(rgb-color 0.690 0.204 0.000) {
  - Multiple harmonies in measure (>2) (More than 2 harmonies)

}
\markup \wordwrap \bold {
  \char ##x2461 NOTE 7 at 2:7 - D
}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - Max note length (sp3: >1/4). Except last note (Usually >1/4 or >2/4)

}
\markup \wordwrap \with-color #(rgb-color 0.000 0.667 0.000) {
  - Slurs within 10 notes (1). For counterpoint species 1-3

}
\markup \wordwrap \with-color #(rgb-color 0.000 0.667 0.000) {
  - Suspension (not last 3) (Suspension before N last measures {sp2,3})

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - Suspension rhythm (1/4 + ?)

}
\markup \wordwrap \with-color #(rgb-color 0.988 0.005 0.000) {
  - Consecutive unidirectional leaps (Sum >6th)

}
\markup \wordwrap \bold {
  \char ##x2461 NOTE 7 at 3:1 - D (slur)
}
\markup \wordwrap \with-color #(rgb-color 0.988 0.005 0.000) {
  - Suspension resolution (unresolved)

}
\markup \wordwrap \with-color #(rgb-color 0.196 0.533 0.000) {
  - Missing letters in a row (>4)

}
\markup \wordwrap \bold {
  \char ##x2461 NOTE 8 at 3:3 - G
}
\markup \wordwrap \with-color #(rgb-color 0.690 0.204 0.000) {
  - Range (int >15va) (Maximum burst interval is 16th during 3 notes  Perfect: int >8va (burst 12th during 3 notes) Good: int >12va (burst 15th during 3 notes))

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - Dissonance upbeat (leap)

}
\markup \wordwrap \with-color #(rgb-color 0.988 0.005 0.000) {
  - Uncompensated leap (>5th). If no other uncompensated rules worked.

}
\markup \wordwrap \with-color #(rgb-color 0.988 0.005 0.000) {
  - Close + 1far MDC (>5th)

}
\markup \wordwrap \with-color #(rgb-color 0.294 0.468 0.000) {
  - Multiple harmonies in measure (inside) (In measures before penultimate)

}
\markup \wordwrap \bold {
  \char ##x2461 NOTE 9 at 3:5 - B
}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - Dissonance upbeat (leap)

}
\markup \wordwrap \with-color #(rgb-color 0.690 0.204 0.000) {
  - Multiple harmonies in measure (>2) (More than 2 harmonies)

}
\markup \wordwrap \bold {
  \char ##x2461 NOTE 10 at 3:7 - A
}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - Close perfect consonances (parallel)

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - Perfect consonances < 4/4 apart (other). Two same non-close perfect consonances in neighboring measures less than 4 notes apart (If other rules in this row did not apply)

}
\markup \wordwrap \with-color #(rgb-color 0.690 0.204 0.000) {
  - Multiple harmonies in measure (>2) (More than 2 harmonies)

}
\markup \wordwrap \bold {
  \char ##x2461 NOTE 11 at 4:1 - D
}
\markup \wordwrap \with-color #(rgb-color 0.988 0.005 0.000) {
  - Adjacent tritone framed (unresolved). Melodic tritone within measure, framed by two melody direction changes

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - Many leaps within 9 notes (>6 leaps)

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - Many leaps within 9 notes (>22 notes) (Total leaps length more than X notes:  3 is third, 4 is fourth and so on)

}
\markup \wordwrap \with-color #(rgb-color 0.294 0.468 0.000) {
  - Prohibited progression (64 chord)

}
\markup \wordwrap \bold {
  \char ##x2461 NOTE 12 at 4:3 - G"#"
}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - Dissonance upbeat (leap)

}
\markup \wordwrap \with-color #(rgb-color 0.988 0.005 0.000) {
  - Adjacent tritone framed (unresolved). Melodic tritone within measure, framed by two melody direction changes

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - Consecutive leaps, 1x3rd ignored (3)

}
\markup \wordwrap \bold {
  \char ##x2461 NOTE 13 at 4:5 - D
}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - Many leaps within 15 notes (>8 leaps)

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - Many leaps within 15 notes (>32 notes) (Total leaps length more than X notes:  3 is third, 4 is fourth and so on)

}
\markup \wordwrap \bold {
      \vspace #2
"#"2 (from test-ly-viz), Rule penalty: 1, Cantus: low, Key: C
}
<<
\new Staff {
  \set Staff.instrumentName = \markup { \char ##x2460 }
  \clef "treble" \key c \major  \accidentalStyle modern-cautionary
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
  }

  {  \speakOff \revert NoteHead.style c'1
_\markup{ \teeny \on-color #(rgb-color 1 1 1) \pad-markup #0.4 \concat { T }  }
^\markup{ \tiny \with-color #(rgb-color 0.000 0.667 0.000) \char ##x2605  }
 \speakOff \revert NoteHead.style d'1
_\markup{ \teeny \on-color #(rgb-color 1 1 1) \pad-markup #0.4 \concat { SII }  }
 \speakOff \revert NoteHead.style c'1
_\markup{ \teeny \on-color #(rgb-color 1 1 1) \pad-markup #0.4 \concat { T }  }

  }
}
>>
\markup \wordwrap \bold {
  NOTE 1 at 1:1 - C
}
\markup \wordwrap \with-color #(rgb-color 0.000 0.667 0.000) {
  - Range (voice <m6) (Range below m6 is acceptable for cantus up to 10 notes)

}
\markup \wordwrap \bold {
      \vspace #2
"#"2 (from test-ly-viz), Cantus: low, Species: 5, Rule penalty: 530, Key: C
}
<<
\new Staff {
  \set Staff.instrumentName = \markup { \char ##x2461 }
  \clef "treble" \key c \major  \accidentalStyle modern-cautionary
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
  }

  { r4
_\markup{ \teeny \on-color #(rgb-color 1 1 1) \pad-markup #0.4 \concat { T }  }
 \speakOff \revert NoteHead.style c''2
^\markup{ \teeny  \on-color #(rgb-color 0.796 0.702 0.500)  \pad-markup #0.4 8 }
 \speakOff \revert NoteHead.style d''4
^\markup{ \tiny \with-color #(rgb-color 0.592 0.269 0.000) \char ##x2605  }
 \speakOff \revert NoteHead.style d''4
_\markup{ \teeny \on-color #(rgb-color 1 1 1) \pad-markup #0.4 \concat { \char ##x00D0 VII \raise #0.7 6 }  }
^\markup{ \teeny  \on-color #(rgb-color 0.796 0.702 0.500)  \pad-markup #0.4 8 }
 \speakOff \revert NoteHead.style c''4
 \speakOff \revert NoteHead.style b'4
 \override Glissando.color=#(rgb-color 0.592 0.269 0.000)  \speakOff \revert NoteHead.style d''4
^\markup{ \teeny  \on-color #(rgb-color 0.994 0.504 0.500)  \pad-markup #0.4 8 }
 \glissando  \speakOff \revert NoteHead.style c''1
_\markup{ \teeny \on-color #(rgb-color 1 1 1) \pad-markup #0.4 \concat { T }  }
^\markup{ \teeny  \pad-markup #0.4 8 }

  }
}
\new Staff {
  \set Staff.instrumentName = \markup { \char ##x2460 }
  \clef "treble" \key c \major  \accidentalStyle modern-cautionary
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
  }

  {  \speakOff \revert NoteHead.style c'1
 \override Glissando.color=#(rgb-color 0.592 0.269 0.000)  \speakOff \revert NoteHead.style d'1
 \glissando  \speakOff \revert NoteHead.style c'1

  }
}
>>
\markup \wordwrap \bold {
  \char ##x2461 NOTE 1 at 1:3 - C
}
\markup \wordwrap \with-color #(rgb-color 0.000 0.667 0.000) {
  - Range (voice <m6) (Range below m6 is acceptable for cantus up to 10 notes)

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - Rhythm (1/4 syncope) (1/4 on beat 2 or 4 is slurred to next note)

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - Rhythm (uneven start) (Starting measure contains notes of different length)

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - Rhythm (uneven pause) (Starting pause in first measure does not equal length of next note)

}
\markup \wordwrap \bold {
  \char ##x2461 NOTE 2 at 1:7 - D
}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - Rhythm (uneven start) (Starting measure contains notes of different length)

}
\markup \wordwrap \with-color #(rgb-color 0.000 0.667 0.000) {
  - Anticipation (any) (Prohibit this rule to prevent detection of any anticipation. By default allows stepwize anticipation in cadence with next note longer and previous note longer or equal)

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - Anticipation (inside) (Anticipation inside counterpoint - not in cadence)

}
\markup \wordwrap \bold {
  \char ##x2461 NOTE 3 at 2:1 - D
}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - Perfect consonances < 4/4 apart (anticipation). Two same non-close perfect consonances in neighboring measures less than 4 notes apart (Second interval is anticipation downbeat)

}
\markup \wordwrap \bold {
  \char ##x2461 NOTE 6 at 2:7 - D
}
\markup \wordwrap \with-color #(rgb-color 0.988 0.005 0.000) {
  - Culmination (multi high) (Multiple culminations in higher voice)

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - Close perfect consonances (parallel)

}
\header {tagline = "This file was created by MGen v3.0-2-g2632972-dirty"}
\markup { 
  \vspace #2
  \bold Legend:
} 

\markup {
  \line {
  \vspace #1
    \column {
      \char ##x2461
      \bold 4:3
      \raise #0.3 \tiny \with-color #(rgb-color 0.000 0.767 0.000) \char ##x2605
      \raise #0.3 \tiny \with-color #(rgb-color 0.867 0.0 0.000) \char ##x2605
      \raise #0.9 \circle \note #"1" #1
      \raise #0.9 \musicglyph #"noteheads.s2xcircle"
      \raise #0.9 \musicglyph #"noteheads.s0harmonic"
      \char ##x00D0
      \char ##x0111
    }
    \hspace #5
    \column {
      "Voice number (2) is shown before note"
      "For each note bar number (4) and croche beat (3) is shown"
      "This note violates minor rule"
      "This note violates major rule"
      "This note was corrected"
      "This note is a dissonance"
      "This note has negative rpos (can be passing or auxiliary)"
      "This letter is used for major Dominant chord to distinguish from D note"
      "This letter is used for minor Dominant chord to distinguish from D note"
    }
  }
} 

