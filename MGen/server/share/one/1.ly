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
  subtitle = "GenCA2: 1 (2017-12-09 11:38)"
}

\markup \wordwrap \bold {
      \vspace #2
"#"1 (from server/share/one/1.mid), Rule penalty: 477, Cantus: low, Key: C
}
<<
\new Staff {
  \set Staff.instrumentName = #"1"
  \clef "bass_8" \key c \major  \accidentalStyle modern-cautionary
  \time 4/4
  % \set Score.barNumberVisibility = #(every-nth-bar-number-visible 4)
  % \override Score.BarNumber.break-visibility = ##(#f #t #t)
  \new Voice \with {
  	\remove "Note_heads_engraver"
  	\consists "Completion_heads_engraver"
  	\remove "Rest_engraver"
  	\consists "Completion_rest_engraver"
  }

  {  \speakOff \revert NoteHead.style c1
_\markup{ \teeny \on-color #(rgb-color 1 1 1) \pad-markup #0.4 \concat { T }  }
^\markup{ \tiny \with-color #(rgb-color 0.988 0.005 0.000) \char ##x2716  }
 \speakOff \revert NoteHead.style f,1
_\markup{ \teeny \on-color #(rgb-color 1 1 1) \pad-markup #0.4 \concat { S }  }
^\markup{ \tiny \with-color #(rgb-color 0.988 0.005 0.000) \char ##x2716  }
 \speakOff \revert NoteHead.style b,,1
_\markup{ \teeny \on-color #(rgb-color 1 1 1) \pad-markup #0.4 \concat { \char ##x00D0  }  }
^\markup{ \tiny \with-color #(rgb-color 0.592 0.269 0.000) \char ##x2716  }
 \speakOff \revert NoteHead.style e,1
_\markup{ \teeny \on-color #(rgb-color 1 1 1) \pad-markup #0.4 \concat { \char ##x00D0 TIII }  }
^\markup{ \tiny \with-color #(rgb-color 0.294 0.468 0.000) \char ##x2716  }
 \speakOff \revert NoteHead.style c,1
_\markup{ \teeny \on-color #(rgb-color 1 1 1) \pad-markup #0.4 \concat { T }  }

  }
}
>>
\markup \wordwrap \bold {
  NOTE 1 at 1:1 - C

}
\markup \wordwrap \with-color #(rgb-color 0.988 0.005 0.000) {
  - Skipped notes (>1 joined)

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - Uncompensated leap (5th). If no other uncompensated rules worked.

}
\markup \wordwrap \bold {
  NOTE 2 at 2:1 - F

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - Adjacent tritone (unresolved). Melodic tritone within measure, framed by two melody direction changes

}
\markup \wordwrap \with-color #(rgb-color 0.988 0.005 0.000) {
  - Consecutive unidirectional leaps (Sum >6th)

}
\markup \wordwrap \bold {
  NOTE 3 at 3:1 - B

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - Consecutive leaps, 1x3rd ignored (3)

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - Consecutive leaps, 1x3rd ignored (3)

}
\markup \wordwrap \bold {
  NOTE 4 at 4:1 - E

}
\markup \wordwrap \with-color #(rgb-color 0.294 0.468 0.000) {
  - 2nd to last note (not V/VIII/II). CF

}
\markup \wordwrap \bold {
      \vspace #2
"#"1 (from 1), Cantus: low, Species: 5, Rule penalty: 288, Key: C
}
<<
\new Staff {
  \set Staff.instrumentName = #"2"
  \clef "treble" \key c \major  \accidentalStyle modern-cautionary
  \time 4/4
  % \set Score.barNumberVisibility = #(every-nth-bar-number-visible 4)
  % \override Score.BarNumber.break-visibility = ##(#f #t #t)
  \new Voice \with {
  	\remove "Note_heads_engraver"
  	\consists "Completion_heads_engraver"
  	\remove "Rest_engraver"
  	\consists "Completion_rest_engraver"
  }

  { r4
_\markup{ \teeny \on-color #(rgb-color 1 1 1) \pad-markup #0.4 \concat { T }  }
 \speakOff \revert NoteHead.style c'4
^\markup{ \tiny \with-color #(rgb-color 0.000 0.667 0.000) \char ##x2716  }
 \speakOff \override NoteHead.style = #'petrucci d'4
 \speakOff \override NoteHead.style = #'petrucci e'4
 \speakOff \revert NoteHead.style f'4
_\markup{ \teeny \on-color #(rgb-color 1 1 1) \pad-markup #0.4 \concat { S }  }
^\markup{ \tiny \with-color #(rgb-color 0.592 0.269 0.000) \char ##x2716  }
 \speakOff \override NoteHead.style = #'petrucci e'4
 \speakOff \override NoteHead.style = #'petrucci d'4
^\markup{ \tiny \with-color #(rgb-color 0.000 0.667 0.000) \char ##x2716  }
 \speakOff \override NoteHead.style = #'petrucci c'4
 \speakOff \revert NoteHead.style b2
_\markup{ \teeny \on-color #(rgb-color 1 1 1) \pad-markup #0.4 \concat { \char ##x00D0 VII }  }
^\markup{ \tiny \with-color #(rgb-color 0.592 0.269 0.000) \char ##x2716  }
 \speakOff \revert NoteHead.style d'2
^\markup{ \tiny \with-color #(rgb-color 0.690 0.204 0.000) \char ##x2716  }
 \speakOff \revert NoteHead.style b2
_\markup{ \teeny \on-color #(rgb-color 0.824 0.925 0.750) \pad-markup #0.4 \concat { \char ##x00D0 TIII }  }
 \speakOff \override NoteHead.style = #'petrucci c'4
 \speakOff \override NoteHead.style = #'petrucci d'4
 \speakOff \revert NoteHead.style c'1
_\markup{ \teeny \on-color #(rgb-color 1 1 1) \pad-markup #0.4 \concat { T }  }

  }
}
\new Staff {
  \set Staff.instrumentName = #"1"
  \clef "bass_8" \key c \major  \accidentalStyle modern-cautionary
  \time 4/4
  % \set Score.barNumberVisibility = #(every-nth-bar-number-visible 4)
  % \override Score.BarNumber.break-visibility = ##(#f #t #t)
  \new Voice \with {
  	\remove "Note_heads_engraver"
  	\consists "Completion_heads_engraver"
  	\remove "Rest_engraver"
  	\consists "Completion_rest_engraver"
  }

  {  \speakOff \revert NoteHead.style c1
 \speakOff \revert NoteHead.style f,1
 \speakOff \revert NoteHead.style b,,1
 \speakOff \revert NoteHead.style e,1
 \speakOff \revert NoteHead.style c,1

  }
}
>>
\markup \wordwrap \bold {
  \char ##x2461 NOTE 3 at 1:3 - C

}
\markup \wordwrap \with-color #(rgb-color 0.000 0.667 0.000) {
  - Range (voice <m6) (Range below m6 is acceptable for cantus up to 10 notes)

}
\markup \wordwrap \with-color #(rgb-color 0.000 0.667 0.000) {
  - Suspension (each 3 meas.) (At least one suspension is needed each X measures {sp5})

}
\markup \wordwrap \bold {
  \char ##x2461 NOTE 6 at 2:1 - F

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - Perfect consonances < 4/4 apart (downbeat). Two same non-close perfect consonances in neighboring measures less than 4 notes apart (Second interval is downbeat)

}
\markup \wordwrap \bold {
  \char ##x2461 NOTE 8 at 2:5 - D

}
\markup \wordwrap \with-color #(rgb-color 0.000 0.667 0.000) {
  - Constrained local range (8 notes <4th) (X consecutive notes have total range less than Y)

}
\markup \wordwrap \bold {
  \char ##x2461 NOTE 10 at 3:1 - B

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - Perfect consonance (LT 8va) (Leading tone harmonic octave)

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - Perfect consonances < 4/4 apart (downbeat). Two same non-close perfect consonances in neighboring measures less than 4 notes apart (Second interval is downbeat)

}
\markup \wordwrap \bold {
  \char ##x2461 NOTE 11 at 3:5 - D

}
\markup \wordwrap \with-color #(rgb-color 0.690 0.204 0.000) {
  - Range (int >15va) (Maximum burst interval is 16th during 3 notes  Perfect: int >8va (burst 12th during 3 notes) Good: int >12va (burst 15th during 3 notes))

}
\markup \wordwrap \bold {
  \char ##x2461 NOTE 12 at 4:1 - B

}
\markup \wordwrap \with-color #(rgb-color 0.294 0.468 0.000) {
  - Penultimate measure (not D/DVII) (Penultimate harmony is not D / DVII)

}
\markup \wordwrap \bold {
      \vspace #2
"#"2 (from server/share/one/1.mid), Rule penalty: 135, Cantus: low, Key: C
}
<<
\new Staff {
  \set Staff.instrumentName = #"1"
  \clef "bass_8" \key c \major  \accidentalStyle modern-cautionary
  \time 4/4
  % \set Score.barNumberVisibility = #(every-nth-bar-number-visible 4)
  % \override Score.BarNumber.break-visibility = ##(#f #t #t)
  \new Voice \with {
  	\remove "Note_heads_engraver"
  	\consists "Completion_heads_engraver"
  	\remove "Rest_engraver"
  	\consists "Completion_rest_engraver"
  }

  {  \speakOff \revert NoteHead.style c,1
_\markup{ \teeny \on-color #(rgb-color 1 1 1) \pad-markup #0.4 \concat { T }  }
^\markup{ \tiny \with-color #(rgb-color 0.988 0.005 0.000) \char ##x2716  }
 \speakOff \revert NoteHead.style f,1
_\markup{ \teeny \on-color #(rgb-color 1 1 1) \pad-markup #0.4 \concat { SII }  }
^\markup{ \tiny \with-color #(rgb-color 0.294 0.468 0.000) \char ##x2716  }
 \speakOff \revert NoteHead.style c,1
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
\markup \wordwrap \with-color #(rgb-color 0.988 0.005 0.000) {
  - Skipped notes (>1 joined)

}
\markup \wordwrap \bold {
  NOTE 2 at 2:1 - F

}
\markup \wordwrap \with-color #(rgb-color 0.294 0.468 0.000) {
  - 2nd to last note (not V/VIII/II). CF

}
\markup \wordwrap \with-color #(rgb-color 0.000 0.667 0.000) {
  - Penultimate note resolution (IV not to III) (IV penultimate note resolves not to III note)

}
\markup \wordwrap \with-color #(rgb-color 0.000 0.667 0.000) {
  - Uncompensated leap (4th). If no other uncompensated rules worked. (Possible in cantus)

}
\markup \wordwrap \bold {
      \vspace #2
"#"2 (from 1), Cantus: low, Species: 3, Rule penalty: 287, Key: C
}
<<
\new Staff {
  \set Staff.instrumentName = #"2"
  \clef "bass" \key c \major  \accidentalStyle modern-cautionary
  \time 4/4
  % \set Score.barNumberVisibility = #(every-nth-bar-number-visible 4)
  % \override Score.BarNumber.break-visibility = ##(#f #t #t)
  \new Voice \with {
  	\remove "Note_heads_engraver"
  	\consists "Completion_heads_engraver"
  	\remove "Rest_engraver"
  	\consists "Completion_rest_engraver"
  }

  { r4
_\markup{ \teeny \on-color #(rgb-color 1 1 1) \pad-markup #0.4 \concat { T }  }
 \speakOff \revert NoteHead.style c4
^\markup{ \tiny \with-color #(rgb-color 0.000 0.667 0.000) \char ##x2716  }
 \speakOff \override NoteHead.style = #'petrucci d4
^\markup{ \tiny \with-color #(rgb-color 0.592 0.269 0.000) \char ##x2716  }
 \speakOff \override NoteHead.style = #'petrucci e4
 \speakOff \revert NoteHead.style d4
_\markup{ \teeny \on-color #(rgb-color 1 1 1) \pad-markup #0.4 \concat { SII \raise #0.7 6 }  }
 \speakOff \override NoteHead.style = #'petrucci e4
 \speakOff \override NoteHead.style = #'petrucci f4
 \speakOff \revert NoteHead.style e4
_\markup{ \teeny \on-color #(rgb-color 0.824 0.925 0.750) \pad-markup #0.4 \concat { S }  }
^\markup{ \tiny \with-color #(rgb-color 0.592 0.269 0.000) \char ##x2716  }
 \speakOff \revert NoteHead.style c1
_\markup{ \teeny \on-color #(rgb-color 1 1 1) \pad-markup #0.4 \concat { T }  }
^\markup{ \tiny \with-color #(rgb-color 0.592 0.269 0.000) \char ##x2716  }

  }
}
\new Staff {
  \set Staff.instrumentName = #"1"
  \clef "bass_8" \key c \major  \accidentalStyle modern-cautionary
  \time 4/4
  % \set Score.barNumberVisibility = #(every-nth-bar-number-visible 4)
  % \override Score.BarNumber.break-visibility = ##(#f #t #t)
  \new Voice \with {
  	\remove "Note_heads_engraver"
  	\consists "Completion_heads_engraver"
  	\remove "Rest_engraver"
  	\consists "Completion_rest_engraver"
  }

  {  \speakOff \revert NoteHead.style c,1
 \speakOff \revert NoteHead.style f,1
 \speakOff \revert NoteHead.style c,1

  }
}
>>
\markup \wordwrap \bold {
  \char ##x2461 NOTE 2 at 1:3 - C

}
\markup \wordwrap \with-color #(rgb-color 0.000 0.667 0.000) {
  - Range (voice <m6) (Range below m6 is acceptable for cantus up to 10 notes)

}
\markup \wordwrap \bold {
  \char ##x2461 NOTE 3 at 1:5 - D

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - Adjacent repeat 2 notes (uniform) (Length of first three notes is the same)

}
\markup \wordwrap \bold {
  \char ##x2461 NOTE 8 at 2:7 - E

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - Dissonance upbeat (leap)

}
\markup \wordwrap \with-color #(rgb-color 0.294 0.468 0.000) {
  - Penultimate measure (no leading tone) (no leading tone is allowed if last bass notes are G-C)

}
\markup \wordwrap \with-color #(rgb-color 0.294 0.468 0.000) {
  - Penultimate measure (not D/DVII) (Penultimate harmony is not D / DVII)

}
\markup \wordwrap \bold {
  \char ##x2461 NOTE 9 at 3:1 - C

}
\markup \wordwrap \with-color #(rgb-color 0.392 0.403 0.000) {
  - Direct leaping movement (8 cadence). Direct movement to interval with leaping motion in higher voice

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - Perfect consonances < 4/4 apart (downbeat). Two same non-close perfect consonances in neighboring measures less than 4 notes apart (Second interval is downbeat)

}
\header {tagline = "This file was created by MGen v2.9-65-gbcda035-dirty and engraved by LilyPond"}
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
      \raise #0.3 \tiny \with-color #(rgb-color 0.000 0.767 0.000) \char ##x2716
      \raise #0.3 \tiny \with-color #(rgb-color 0.867 0.0 0.000) \char ##x2716
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

