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

\header {
  subtitle = "GenCA1: test-repeat (2017-10-27 18:43)"
}

\markup \wordwrap \bold {
      \vspace #2
"#"1 (from midi/GenCA1/test-repeat.mid), Rule penalty: 632  \char ##x27F9  0, Distance penalty: 14, Cantus: low, Key: C
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

  {  \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style e''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style f''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style f''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style g''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style f''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style f''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "S" }
 \speakOff \revert NoteHead.style e''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }

  }
}
\new Staff {
  \set Staff.instrumentName = #"1"
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

  {  \speakOff \revert NoteHead.style c''1 ^\markup{ \tiny \with-color #(rgb-color 0.592 0.269 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style  \circle c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style e''1 ^\markup{ \tiny \with-color #(rgb-color 0.592 0.269 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style f''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "S" }
 \speakOff \revert NoteHead.style  \circle e''1 ^\markup{ \tiny \with-color #(rgb-color 0.592 0.269 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style f''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style g''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style f''1 ^\markup{ \tiny \with-color #(rgb-color 0.592 0.269 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style  \circle e''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style f''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style e''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }

  }
}
>>
\markup \wordwrap \bold {
  \char ##x2460 NOTE 1 at 1:1 - C

}
\markup \wordwrap \with-color #(rgb-color 0.000 0.667 0.000) {
  - Range (voice <m6)

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - Adjacent repeat (2 notes)

}
\markup \wordwrap \with-color #(rgb-color 0.196 0.533 0.000) {
  - Notes repeat (2 sym. in 3) (Symmetric repeat of X notes within Y notes)

}
\markup \wordwrap \bold {
  \char ##x2460 NOTE 5 at 5:1 - E

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - Adjacent repeat (2 notes)

}
\markup \wordwrap \bold {
  \char ##x2460 NOTE 7 at 7:1 - E

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - CF Stepwise motion (>7 any direction) (Stepwise motion more than X notes in any direction)

}
\markup \wordwrap \bold {
  \char ##x2460 NOTE 10 at 10:1 - F

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - Adjacent repeat (2 notes)

}
\markup \wordwrap \bold {
      \vspace #2
"#"1 (from midi/GenCA1/test-repeat.mid), Rule penalty: 632  \char ##x27F9  0, Distance penalty: 14, Cantus: low, Key: C
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

  {  \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style e''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style f''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "S" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style f''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style g''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style f''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "S" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style f''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "S" }
 \speakOff \revert NoteHead.style e''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }

  }
}
>>
\markup \wordwrap \bold {
      \vspace #2
"#"2 (from midi/GenCA1/test-repeat.mid), Rule penalty: 957  \char ##x27F9  753, Distance penalty: 5, Cantus: low, Key: Am
}
<<
\new Staff {
  \set Staff.instrumentName = #"2"
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

  {  \speakOff \revert NoteHead.style a'1~  \speakOff \revert NoteHead.style a'1 ^\markup{ \tiny \with-color #(rgb-color 0.988 0.005 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "t" \tiny \with-color #(rgb-color 0.588 0.588 0.588) ", t" }
 \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "sII" }
 \speakOff \revert NoteHead.style e''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "d" }
 \speakOff \revert NoteHead.style f''1 ^\markup{ \tiny \with-color #(rgb-color 0.592 0.269 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style g''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "d" }
 \speakOff \revert NoteHead.style a''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "t" }
 \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "sII" }
 \speakOff \revert NoteHead.style c''1 ^\markup{ \tiny \with-color #(rgb-color 0.592 0.269 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "sII" }
 \speakOff \revert NoteHead.style e''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "d" }
 \speakOff \revert NoteHead.style f''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style g''1 ^\markup{ \tiny \with-color #(rgb-color 0.592 0.269 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "dVII" }
 \speakOff \revert NoteHead.style a''1 ^\markup{ \tiny \with-color #(rgb-color 0.000 0.667 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "t" }
 \speakOff \revert NoteHead.style c''1 ^\markup{ \tiny \with-color #(rgb-color 0.592 0.269 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "sII" }
 \speakOff \revert NoteHead.style e''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "d" }
 \speakOff \revert NoteHead.style f''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style g''1 ^\markup{ \tiny \with-color #(rgb-color 0.988 0.005 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "d" }
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "sII" }
 \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "t" }
 \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "s" }
 \speakOff \revert NoteHead.style e''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "d" }
 \speakOff \revert NoteHead.style f''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style g''1 ^\markup{ \tiny \with-color #(rgb-color 0.592 0.269 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "dVII" }
 \speakOff \revert NoteHead.style a''1 ^\markup{ \tiny \with-color #(rgb-color 0.000 0.667 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "t" }

  }
}
\new Staff {
  \set Staff.instrumentName = #"1"
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

  {  \speakOff \revert NoteHead.style a'1 ^\markup{ \tiny \with-color #(rgb-color 0.592 0.269 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "t" }
 \speakOff \revert NoteHead.style  \circle b'1 ^\markup{ \tiny \with-color #(rgb-color 0.592 0.269 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "sII" }
 \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style d''1 ^\markup{ \tiny \with-color #(rgb-color 0.592 0.269 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "s" }
 \speakOff \revert NoteHead.style e''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style f''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "s" }
 \speakOff \revert NoteHead.style g''1 ^\markup{ \tiny \with-color #(rgb-color 0.592 0.269 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "dVII" }
 \speakOff \revert NoteHead.style a''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "t" }
 \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "sII" }
 \speakOff \revert NoteHead.style c''1 ^\markup{ \tiny \with-color #(rgb-color 0.592 0.269 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "sII" }
 \speakOff \revert NoteHead.style e''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style f''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style g''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "dVII" }
 \speakOff \revert NoteHead.style a''1 ^\markup{ \tiny \with-color #(rgb-color 0.000 0.667 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "t" }
 \speakOff \revert NoteHead.style c''1 ^\markup{ \tiny \with-color #(rgb-color 0.592 0.269 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "s" }
 \speakOff \revert NoteHead.style e''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style f''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style g''1 ^\markup{ \tiny \with-color #(rgb-color 0.988 0.005 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "d" }
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "sII" }
 \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "t" }
 \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "s" }
 \speakOff \revert NoteHead.style e''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "d" }
 \speakOff \revert NoteHead.style f''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style g''1 ^\markup{ \tiny \with-color #(rgb-color 0.592 0.269 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "d" }
 \speakOff \revert NoteHead.style a''1 ^\markup{ \tiny \with-color #(rgb-color 0.000 0.667 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "t" }

  }
}
>>
\markup \wordwrap \bold {
  \char ##x2461 NOTE 1 at 1:1 - A

}
\markup \wordwrap \with-color #(rgb-color 0.988 0.005 0.000) {
  - Slurs (cantus)

}
\markup \wordwrap \bold {
  \char ##x2461 NOTE 5 at 6:1 - F

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - CF Stepwise motion (>4 one direction) (Stepwise motion more than X notes in one direction)

}
\markup \wordwrap \bold {
  \char ##x2461 NOTE 10 at 11:1 - C

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - Notes repeat (5 in 7) (Repeat of X notes within Y notes)

}
\markup \wordwrap \bold {
  \char ##x2461 NOTE 14 at 15:1 - G

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - CF Stepwise motion (>7 any direction) (Stepwise motion more than X notes in any direction)

}
\markup \wordwrap \bold {
  \char ##x2461 NOTE 15 at 16:1 - A

}
\markup \wordwrap \with-color #(rgb-color 0.000 0.667 0.000) {
  - Culmination (multi low) (Multiple culminations in lower voice)

}
\markup \wordwrap \bold {
  \char ##x2461 NOTE 16 at 17:1 - C

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - Notes repeat (5 in 7) (Repeat of X notes within Y notes)

}
\markup \wordwrap \bold {
  \char ##x2461 NOTE 20 at 21:1 - G

}
\markup \wordwrap \with-color #(rgb-color 0.988 0.005 0.000) {
  - Close + 1far MDC (>5th)

}
\markup \wordwrap \bold {
  \char ##x2461 NOTE 27 at 28:1 - G

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - Unaltered VII (before Im) (Next (or second to next in cadence))

}
\markup \wordwrap \bold {
  \char ##x2461 NOTE 28 at 29:1 - A

}
\markup \wordwrap \with-color #(rgb-color 0.000 0.667 0.000) {
  - Culmination (multi low) (Multiple culminations in lower voice)

}
\markup \wordwrap \bold {
  \char ##x2460 NOTE 1 at 1:1 - A

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - Notes repeat (7 in 10) (Repeat of X notes within Y notes)

}
\markup \wordwrap \bold {
  \char ##x2460 NOTE 2 at 2:1 - B

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - Notes repeat (7 in 10) (Repeat of X notes within Y notes)

}
\markup \wordwrap \bold {
  \char ##x2460 NOTE 4 at 4:1 - D

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - CF Stepwise motion (>4 one direction) (Stepwise motion more than X notes in one direction)

}
\markup \wordwrap \bold {
  \char ##x2460 NOTE 7 at 7:1 - G

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - CF Stepwise motion (>7 any direction) (Stepwise motion more than X notes in any direction)

}
\markup \wordwrap \bold {
  \char ##x2460 NOTE 11 at 11:1 - C

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - Notes repeat (5 in 7) (Repeat of X notes within Y notes)

}
\markup \wordwrap \bold {
  \char ##x2460 NOTE 16 at 16:1 - A

}
\markup \wordwrap \with-color #(rgb-color 0.000 0.667 0.000) {
  - Culmination (multi low) (Multiple culminations in lower voice)

}
\markup \wordwrap \bold {
  \char ##x2460 NOTE 17 at 17:1 - C

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - Notes repeat (5 in 7) (Repeat of X notes within Y notes)

}
\markup \wordwrap \bold {
  \char ##x2460 NOTE 21 at 21:1 - G

}
\markup \wordwrap \with-color #(rgb-color 0.988 0.005 0.000) {
  - Close + 1far MDC (>5th)

}
\markup \wordwrap \bold {
  \char ##x2460 NOTE 28 at 28:1 - G

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - Unaltered VII (before Im) (Next (or second to next in cadence))

}
\markup \wordwrap \bold {
  \char ##x2460 NOTE 29 at 29:1 - A

}
\markup \wordwrap \with-color #(rgb-color 0.000 0.667 0.000) {
  - Culmination (multi low) (Multiple culminations in lower voice)

}
\markup \wordwrap \bold {
      \vspace #2
"#"3 (from midi/GenCA1/test-repeat.mid), Rule penalty: 213  \char ##x27F9  1, Distance penalty: 8, Cantus: low, Key: F
}
<<
\new Staff {
  \set Staff.instrumentName = #"2"
  \clef "treble" \key f \major  \accidentalStyle modern-cautionary
  \time 4/4
  % \set Score.barNumberVisibility = #(every-nth-bar-number-visible 4)
  % \override Score.BarNumber.break-visibility = ##(#f #t #t)
  \new Voice \with {
  	\remove "Note_heads_engraver"
  	\consists "Completion_heads_engraver"
  	\remove "Rest_engraver"
  	\consists "Completion_rest_engraver"
  }

  {  \speakOff \revert NoteHead.style f''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style e''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style f''1 ^\markup{ \tiny \with-color #(rgb-color 0.000 0.667 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "S" }
 \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style g'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style f'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }

  }
}
\new Staff {
  \set Staff.instrumentName = #"1"
  \clef "treble" \key f \major  \accidentalStyle modern-cautionary
  \time 4/4
  % \set Score.barNumberVisibility = #(every-nth-bar-number-visible 4)
  % \override Score.BarNumber.break-visibility = ##(#f #t #t)
  \new Voice \with {
  	\remove "Note_heads_engraver"
  	\consists "Completion_heads_engraver"
  	\remove "Rest_engraver"
  	\consists "Completion_rest_engraver"
  }

  {  \speakOff \revert NoteHead.style  \circle c''1 ^\markup{ \tiny \with-color #(rgb-color 0.690 0.204 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "S" }
 \speakOff \revert NoteHead.style  \circle e''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style f''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "S" }
 \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style g'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style f'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }

  }
}
>>
\markup \wordwrap \bold {
  \char ##x2461 NOTE 4 at 4:1 - F

}
\markup \wordwrap \with-color #(rgb-color 0.000 0.667 0.000) {
  - Culmination (multi low) (Multiple culminations in lower voice)

}
\markup \wordwrap \bold {
  \char ##x2460 NOTE 1 at 1:1 - C

}
\markup \wordwrap \with-color #(rgb-color 0.690 0.204 0.000) {
  - Start steps tonic (no C)

}
\markup \wordwrap \with-color #(rgb-color 0.690 0.204 0.000) {
  - Start steps tonic (no E)

}
\markup \wordwrap \with-color #(rgb-color 0.690 0.204 0.000) {
  - Start steps tonic (unprepared)

}
\markup \wordwrap \bold {
      \vspace #2
"#"3 (from midi/GenCA1/test-repeat.mid), Rule penalty: 213  \char ##x27F9  0, Distance penalty: 14, Cantus: low, Key: F
}
<<
\new Staff {
  \set Staff.instrumentName = #"2"
  \clef "treble" \key f \major  \accidentalStyle modern-cautionary
  \time 4/4
  % \set Score.barNumberVisibility = #(every-nth-bar-number-visible 4)
  % \override Score.BarNumber.break-visibility = ##(#f #t #t)
  \new Voice \with {
  	\remove "Note_heads_engraver"
  	\consists "Completion_heads_engraver"
  	\remove "Rest_engraver"
  	\consists "Completion_rest_engraver"
  }

  {  \speakOff \revert NoteHead.style f''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "S" }
 \speakOff \revert NoteHead.style g''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style f''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "S" }
 \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style g'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style f'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }

  }
}
>>
\markup \wordwrap \bold {
      \vspace #2
"#"4 (from midi/GenCA1/test-repeat.mid), Rule penalty: 62  \char ##x27F9  61, Distance penalty: 4, Cantus: low, Key: C
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

  {  \speakOff \revert NoteHead.style c'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style d'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style e'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style c'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style d'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style e'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style g'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style e'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "S" }
 \speakOff \revert NoteHead.style g'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style f'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style e'1 ^\markup{ \tiny \with-color #(rgb-color 0.592 0.269 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style d'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style c'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }

  }
}
\new Staff {
  \set Staff.instrumentName = #"1"
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

  {  \speakOff \revert NoteHead.style c'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style d'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style e'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style c'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style d'1 ^\markup{ \tiny \with-color #(rgb-color 0.000 0.667 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style  \circle f'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "S" }
 \speakOff \revert NoteHead.style g'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style e'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "S" }
 \speakOff \revert NoteHead.style g'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style f'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style e'1 ^\markup{ \tiny \with-color #(rgb-color 0.592 0.269 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style  \circle d'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style c'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }

  }
}
>>
\markup \wordwrap \bold {
  \char ##x2461 NOTE 12 at 12:1 - E

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - CF Stepwise motion (>4 one direction) (Stepwise motion more than X notes in one direction)

}
\markup \wordwrap \bold {
  \char ##x2460 NOTE 5 at 5:1 - D

}
\markup \wordwrap \with-color #(rgb-color 0.000 0.667 0.000) {
  - Far + 1far MDC (3rd)

}
\markup \wordwrap \bold {
  \char ##x2460 NOTE 12 at 12:1 - E

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - CF Stepwise motion (>4 one direction) (Stepwise motion more than X notes in one direction)

}
\markup \wordwrap \bold {
      \vspace #2
"#"4 (from midi/GenCA1/test-repeat.mid), Rule penalty: 62  \char ##x27F9  0, Distance penalty: 10, Cantus: low, Key: C
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

  {  \speakOff \revert NoteHead.style c'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style d'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style e'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style c'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style d'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style e'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style g'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style e'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "S" }
 \speakOff \revert NoteHead.style g'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style f'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style e'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style b1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style c'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }

  }
}
>>
\markup \wordwrap \bold {
      \vspace #2
"#"5 (from midi/GenCA1/test-repeat.mid), Rule penalty: 4  \char ##x27F9  3, Distance penalty: 6, Cantus: low, Key: C
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

  {  \speakOff \revert NoteHead.style c'1 ^\markup{ \tiny \with-color #(rgb-color 0.000 0.667 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style d'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style e'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style f'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "S" }
 \speakOff \revert NoteHead.style e'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style c'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style d'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style f'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "S" }
 \speakOff \revert NoteHead.style e'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style g'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style c'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }

  }
}
\new Staff {
  \set Staff.instrumentName = #"1"
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

  {  \speakOff \revert NoteHead.style c'1 ^\markup{ \tiny \with-color #(rgb-color 0.000 0.667 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style d'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style e'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style f'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "S" }
 \speakOff \revert NoteHead.style  \circle g'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style c'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style d'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style f'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "S" }
 \speakOff \revert NoteHead.style e'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style g'1 ^\markup{ \tiny \with-color #(rgb-color 0.000 0.667 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style c'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }

  }
}
>>
\markup \wordwrap \bold {
  \char ##x2461 NOTE 1 at 1:1 - C

}
\markup \wordwrap \with-color #(rgb-color 0.000 0.667 0.000) {
  - Range (voice <m6)

}
\markup \wordwrap \with-color #(rgb-color 0.000 0.667 0.000) {
  - No melody development (10 notes <16) (Melody direction within X notes is less than Y decisemitones)

}
\markup \wordwrap \bold {
  \char ##x2460 NOTE 1 at 1:1 - C

}
\markup \wordwrap \with-color #(rgb-color 0.000 0.667 0.000) {
  - Range (voice <m6)

}
\markup \wordwrap \with-color #(rgb-color 0.000 0.667 0.000) {
  - No melody development (10 notes <16) (Melody direction within X notes is less than Y decisemitones)

}
\markup \wordwrap \bold {
  \char ##x2460 NOTE 10 at 10:1 - G

}
\markup \wordwrap \with-color #(rgb-color 0.000 0.667 0.000) {
  - Culmination (multi low) (Multiple culminations in lower voice)

}
\markup \wordwrap \bold {
      \vspace #2
"#"5 (from midi/GenCA1/test-repeat.mid), Rule penalty: 4  \char ##x27F9  0, Distance penalty: 11, Cantus: low, Key: C
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

  {  \speakOff \revert NoteHead.style c'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style d'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style e'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style f'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style b1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style c'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style d'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style f'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "S" }
 \speakOff \revert NoteHead.style e'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style g'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style c'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }

  }
}
>>
\markup \wordwrap \bold {
      \vspace #2
"#"6 (from midi/GenCA1/test-repeat.mid), Rule penalty: 288  \char ##x27F9  228, Distance penalty: 6, Cantus: low, Key: C
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

  {  \speakOff \revert NoteHead.style c'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style d'1 ^\markup{ \tiny \with-color #(rgb-color 0.000 0.667 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style g'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style f'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style g'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style c'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style d'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style f'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "S" }
 \speakOff \revert NoteHead.style e'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style g'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style a'1 ^\markup{ \tiny \with-color #(rgb-color 0.592 0.269 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style d'1 ^\markup{ \tiny \with-color #(rgb-color 0.988 0.005 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style b1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style c'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }

  }
}
\new Staff {
  \set Staff.instrumentName = #"1"
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

  {  \speakOff \revert NoteHead.style c'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style d'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style  \circle e'1 ^\markup{ \tiny \with-color #(rgb-color 0.000 0.667 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style f'1 ^\markup{ \tiny \with-color #(rgb-color 0.592 0.269 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "S" }
 \speakOff \revert NoteHead.style  \circle g'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style  \circle a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style c'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style d'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style f'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style e'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style g'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style  \circle a'1 ^\markup{ \tiny \with-color #(rgb-color 0.592 0.269 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style d'1 ^\markup{ \tiny \with-color #(rgb-color 0.988 0.005 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style b1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style c'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }

  }
}
>>
\markup \wordwrap \bold {
  \char ##x2461 NOTE 2 at 2:1 - D

}
\markup \wordwrap \with-color #(rgb-color 0.000 0.667 0.000) {
  - No melody development (10 notes <16) (Melody direction within X notes is less than Y decisemitones)

}
\markup \wordwrap \bold {
  \char ##x2461 NOTE 12 at 12:1 - A

}
\markup \wordwrap \with-color #(rgb-color 0.000 0.667 0.000) {
  - Culmination (multi low) (Multiple culminations in lower voice)

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - Uncompensated: precompensated (5th)

}
\markup \wordwrap \bold {
  \char ##x2461 NOTE 13 at 13:1 - D

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - Many leaps within 9 notes (>18 notes) (Total leaps length more than X notes:  3 is third, 4 is fourth and so on)

}
\markup \wordwrap \with-color #(rgb-color 0.988 0.005 0.000) {
  - Consecutive unidirectional leaps (Sum >6th)

}
\markup \wordwrap \bold {
  \char ##x2460 NOTE 3 at 3:1 - E

}
\markup \wordwrap \with-color #(rgb-color 0.000 0.667 0.000) {
  - No melody development (10 notes <16) (Melody direction within X notes is less than Y decisemitones)

}
\markup \wordwrap \bold {
  \char ##x2460 NOTE 4 at 4:1 - F

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - CF Stepwise motion (>4 one direction) (Stepwise motion more than X notes in one direction)

}
\markup \wordwrap \bold {
  \char ##x2460 NOTE 12 at 12:1 - A

}
\markup \wordwrap \with-color #(rgb-color 0.000 0.667 0.000) {
  - Culmination (multi low) (Multiple culminations in lower voice)

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - Uncompensated: precompensated (5th)

}
\markup \wordwrap \bold {
  \char ##x2460 NOTE 13 at 13:1 - D

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - Many leaps within 9 notes (>18 notes) (Total leaps length more than X notes:  3 is third, 4 is fourth and so on)

}
\markup \wordwrap \with-color #(rgb-color 0.988 0.005 0.000) {
  - Consecutive unidirectional leaps (Sum >6th)

}
\markup \wordwrap \bold {
      \vspace #2
"#"6 (from midi/GenCA1/test-repeat.mid), Rule penalty: 288  \char ##x27F9  0, Distance penalty: 18, Cantus: low, Key: C
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

  {  \speakOff \revert NoteHead.style c'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style d'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style e'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style f'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "S" }
 \speakOff \revert NoteHead.style g'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style c'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style d'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style f'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "S" }
 \speakOff \revert NoteHead.style e'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style g'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style e'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style d'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style b1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style c'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }

  }
}
>>
\markup \wordwrap \bold {
      \vspace #2
"#"7 (from midi/GenCA1/test-repeat.mid), Rule penalty: 3  \char ##x27F9  2, Distance penalty: 6, Cantus: low, Key: C
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

  {  \speakOff \revert NoteHead.style c''1 ^\markup{ \tiny \with-color #(rgb-color 0.000 0.667 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style f''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style e''1 ^\markup{ \tiny \with-color #(rgb-color 0.000 0.667 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style e''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }

  }
}
\new Staff {
  \set Staff.instrumentName = #"1"
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

  {  \speakOff \revert NoteHead.style c''1 ^\markup{ \tiny \with-color #(rgb-color 0.000 0.667 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style  \circle d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style  \circle e''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style e''1 ^\markup{ \tiny \with-color #(rgb-color 0.000 0.667 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }

  }
}
>>
\markup \wordwrap \bold {
  \char ##x2461 NOTE 1 at 1:1 - C

}
\markup \wordwrap \with-color #(rgb-color 0.000 0.667 0.000) {
  - Range (voice <m6)

}
\markup \wordwrap \bold {
  \char ##x2461 NOTE 3 at 3:1 - E

}
\markup \wordwrap \with-color #(rgb-color 0.000 0.667 0.000) {
  - Far + 1far MDC (3rd)

}
\markup \wordwrap \bold {
  \char ##x2460 NOTE 1 at 1:1 - C

}
\markup \wordwrap \with-color #(rgb-color 0.000 0.667 0.000) {
  - Range (voice <m6)

}
\markup \wordwrap \with-color #(rgb-color 0.000 0.667 0.000) {
  - No melody development (10 notes <16) (Melody direction within X notes is less than Y decisemitones)

}
\markup \wordwrap \bold {
  \char ##x2460 NOTE 7 at 7:1 - E

}
\markup \wordwrap \with-color #(rgb-color 0.000 0.667 0.000) {
  - Culmination (multi low) (Multiple culminations in lower voice)

}
\markup \wordwrap \bold {
      \vspace #2
"#"7 (from midi/GenCA1/test-repeat.mid), Rule penalty: 3  \char ##x27F9  0, Distance penalty: 6, Cantus: low, Key: C
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

  {  \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style g''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style e''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }

  }
}
>>
\markup \wordwrap \bold {
      \vspace #2
"#"8 (from midi/GenCA1/test-repeat.mid), Rule penalty: 196  \char ##x27F9  64, Distance penalty: 5, Cantus: low, Key: C
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

  {  \speakOff \revert NoteHead.style b'1 ^\markup{ \tiny \with-color #(rgb-color 0.592 0.269 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style e''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style e''1 ^\markup{ \tiny \with-color #(rgb-color 0.000 0.667 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }

  }
}
\new Staff {
  \set Staff.instrumentName = #"1"
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

  {  \speakOff \revert NoteHead.style  \circle b'1 ^\markup{ \tiny \with-color #(rgb-color 0.690 0.204 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style  \circle d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style  \circle e''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style e''1 ^\markup{ \tiny \with-color #(rgb-color 0.000 0.667 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }

  }
}
>>
\markup \wordwrap \bold {
  \char ##x2461 NOTE 1 at 1:1 - B

}
\markup \wordwrap \with-color #(rgb-color 0.000 0.667 0.000) {
  - Range (voice <m6)

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - First note (not GC)

}
\markup \wordwrap \with-color #(rgb-color 0.000 0.667 0.000) {
  - No melody development (10 notes <16) (Melody direction within X notes is less than Y decisemitones)

}
\markup \wordwrap \bold {
  \char ##x2461 NOTE 7 at 7:1 - E

}
\markup \wordwrap \with-color #(rgb-color 0.000 0.667 0.000) {
  - Culmination (multi low) (Multiple culminations in lower voice)

}
\markup \wordwrap \bold {
  \char ##x2460 NOTE 1 at 1:1 - B

}
\markup \wordwrap \with-color #(rgb-color 0.000 0.667 0.000) {
  - Range (voice <m6)

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - Notes repeat (3 sym. in 4) (Symmetric repeat of X notes within Y notes)

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - First note (not GC)

}
\markup \wordwrap \with-color #(rgb-color 0.690 0.204 0.000) {
  - Start steps tonic (no C)

}
\markup \wordwrap \with-color #(rgb-color 0.000 0.667 0.000) {
  - No melody development (10 notes <16) (Melody direction within X notes is less than Y decisemitones)

}
\markup \wordwrap \bold {
  \char ##x2460 NOTE 7 at 7:1 - E

}
\markup \wordwrap \with-color #(rgb-color 0.000 0.667 0.000) {
  - Culmination (multi low) (Multiple culminations in lower voice)

}
\markup \wordwrap \bold {
      \vspace #2
"#"8 (from midi/GenCA1/test-repeat.mid), Rule penalty: 196  \char ##x27F9  0, Distance penalty: 10, Cantus: low, Key: C
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

  {  \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style g''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style e''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }

  }
}
>>
\markup \wordwrap \bold {
      \vspace #2
"#"9 (from midi/GenCA1/test-repeat.mid), Rule penalty: 135  \char ##x27F9  132, Distance penalty: 6, Cantus: low, Key: C
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

  {  \speakOff \revert NoteHead.style e''1 ^\markup{ \tiny \with-color #(rgb-color 0.690 0.204 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style g''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style e''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }

  }
}
\new Staff {
  \set Staff.instrumentName = #"1"
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

  {  \speakOff \revert NoteHead.style  \circle e''1 ^\markup{ \tiny \with-color #(rgb-color 0.690 0.204 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style  \circle e''1 ^\markup{ \tiny \with-color #(rgb-color 0.000 0.667 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style e''1 ^\markup{ \tiny \with-color #(rgb-color 0.000 0.667 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }

  }
}
>>
\markup \wordwrap \bold {
  \char ##x2461 NOTE 1 at 1:1 - E

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - First note (not GC)

}
\markup \wordwrap \with-color #(rgb-color 0.690 0.204 0.000) {
  - Start steps tonic (no C)

}
\markup \wordwrap \bold {
  \char ##x2460 NOTE 1 at 1:1 - E

}
\markup \wordwrap \with-color #(rgb-color 0.000 0.667 0.000) {
  - Range (voice <m6)

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - First note (not GC)

}
\markup \wordwrap \with-color #(rgb-color 0.690 0.204 0.000) {
  - Start steps tonic (no C)

}
\markup \wordwrap \bold {
  \char ##x2460 NOTE 3 at 3:1 - E

}
\markup \wordwrap \with-color #(rgb-color 0.000 0.667 0.000) {
  - Culmination (multi low) (Multiple culminations in lower voice)

}
\markup \wordwrap \bold {
  \char ##x2460 NOTE 7 at 7:1 - E

}
\markup \wordwrap \with-color #(rgb-color 0.000 0.667 0.000) {
  - Culmination (multi low) (Multiple culminations in lower voice)

}
\markup \wordwrap \bold {
      \vspace #2
"#"9 (from midi/GenCA1/test-repeat.mid), Rule penalty: 135  \char ##x27F9  0, Distance penalty: 13, Cantus: low, Key: C
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

  {  \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style g''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style e''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }

  }
}
>>
\markup \wordwrap \bold {
      \vspace #2
"#"10 (from midi/GenCA1/test-repeat.mid), Rule penalty: 217  \char ##x27F9  133, Distance penalty: 4, Cantus: low, Key: C
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

  {  \speakOff \revert NoteHead.style b'1 ^\markup{ \tiny \with-color #(rgb-color 0.690 0.204 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style e''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style f''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "S" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style e''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }

  }
}
\new Staff {
  \set Staff.instrumentName = #"1"
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

  {  \speakOff \revert NoteHead.style  \circle b'1 ^\markup{ \tiny \with-color #(rgb-color 0.690 0.204 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style e''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style  \circle e''1 ^\markup{ \tiny \with-color #(rgb-color 0.592 0.269 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style e''1 ^\markup{ \tiny \with-color #(rgb-color 0.000 0.667 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }

  }
}
>>
\markup \wordwrap \bold {
  \char ##x2461 NOTE 1 at 1:1 - B

}
\markup \wordwrap \with-color #(rgb-color 0.000 0.667 0.000) {
  - Range (voice <m6)

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - First note (not GC)

}
\markup \wordwrap \with-color #(rgb-color 0.690 0.204 0.000) {
  - Start steps tonic (no C)

}
\markup \wordwrap \bold {
  \char ##x2460 NOTE 1 at 1:1 - B

}
\markup \wordwrap \with-color #(rgb-color 0.000 0.667 0.000) {
  - Range (voice <m6)

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - First note (not GC)

}
\markup \wordwrap \with-color #(rgb-color 0.690 0.204 0.000) {
  - Start steps tonic (no C)

}
\markup \wordwrap \bold {
  \char ##x2460 NOTE 5 at 5:1 - E

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - Adjacent repeat (2 notes)

}
\markup \wordwrap \with-color #(rgb-color 0.196 0.533 0.000) {
  - Notes repeat (2 sym. in 3) (Symmetric repeat of X notes within Y notes)

}
\markup \wordwrap \with-color #(rgb-color 0.000 0.667 0.000) {
  - Culmination (multi low) (Multiple culminations in lower voice)

}
\markup \wordwrap \bold {
  \char ##x2460 NOTE 7 at 7:1 - E

}
\markup \wordwrap \with-color #(rgb-color 0.000 0.667 0.000) {
  - Culmination (multi low) (Multiple culminations in lower voice)

}
\markup \wordwrap \bold {
      \vspace #2
"#"10 (from midi/GenCA1/test-repeat.mid), Rule penalty: 217  \char ##x27F9  0, Distance penalty: 10, Cantus: low, Key: C
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

  {  \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style e''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style g''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style e''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }

  }
}
>>
\markup \wordwrap \bold {
      \vspace #2
"#"11 (from midi/GenCA1/test-repeat.mid), Rule penalty: 62  \char ##x27F9  1, Distance penalty: 4, Cantus: low, Key: Am
}
<<
\new Staff {
  \set Staff.instrumentName = #"2"
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

  {  \speakOff \revert NoteHead.style a'1 ^\markup{ \tiny \with-color #(rgb-color 0.000 0.667 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "t" }
 \speakOff \revert NoteHead.style g'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "dVII" }
 \speakOff \revert NoteHead.style f'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style e'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "d" }
 \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "sII" }
 \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "t" }

  }
}
\new Staff {
  \set Staff.instrumentName = #"1"
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

  {  \speakOff \revert NoteHead.style a'1 ^\markup{ \tiny \with-color #(rgb-color 0.000 0.667 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "t" }
 \speakOff \revert NoteHead.style g'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "d" }
 \speakOff \revert NoteHead.style f'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style e'1 ^\markup{ \tiny \with-color #(rgb-color 0.592 0.269 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "d" }
 \speakOff \revert NoteHead.style  \circle gs'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "sII" }
 \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "t" }

  }
}
>>
\markup \wordwrap \bold {
  \char ##x2461 NOTE 1 at 1:1 - A

}
\markup \wordwrap \with-color #(rgb-color 0.000 0.667 0.000) {
  - Range (voice <m6)

}
\markup \wordwrap \bold {
  \char ##x2460 NOTE 1 at 1:1 - A

}
\markup \wordwrap \with-color #(rgb-color 0.000 0.667 0.000) {
  - Range (voice <m6)

}
\markup \wordwrap \bold {
  \char ##x2460 NOTE 4 at 4:1 - E

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - Two consecutive 3rds (plain)

}
\markup \wordwrap \bold {
      \vspace #2
"#"11 (from midi/GenCA1/test-repeat.mid), Rule penalty: 62  \char ##x27F9  0, Distance penalty: 7, Cantus: low, Key: Am
}
<<
\new Staff {
  \set Staff.instrumentName = #"2"
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

  {  \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "t" }
 \speakOff \revert NoteHead.style g'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "d" }
 \speakOff \revert NoteHead.style f'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "s" }
 \speakOff \revert NoteHead.style e'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "d" }
 \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "sII" }
 \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "t" }

  }
}
>>
\markup \wordwrap \bold {
      \vspace #2
"#"12 (from midi/GenCA1/test-repeat.mid), Rule penalty: 1  \char ##x27F9  0, Distance penalty: 5, Cantus: low, Key: Am
}
<<
\new Staff {
  \set Staff.instrumentName = #"2"
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

  {  \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "t" }
 \speakOff \revert NoteHead.style f'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "s" }
 \speakOff \revert NoteHead.style g'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "dVII" }
 \speakOff \revert NoteHead.style e'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "d" }
 \speakOff \revert NoteHead.style f'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style g'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "dVII" }
 \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "t" }
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "sII" }
 \speakOff \revert NoteHead.style e''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "d" }
 \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "sII" }
 \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "t" }

  }
}
\new Staff {
  \set Staff.instrumentName = #"1"
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

  {  \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "t" }
 \speakOff \revert NoteHead.style f'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style g'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "d" }
 \speakOff \revert NoteHead.style e'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style f'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "s" }
 \speakOff \revert NoteHead.style g'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "d" }
 \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "t" }
 \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "sII" }
 \speakOff \revert NoteHead.style e''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style  \circle d''1 ^\markup{ \tiny \with-color #(rgb-color 0.000 0.667 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "sII" }
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "dVII" }
 \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "t" }

  }
}
>>
\markup \wordwrap \bold {
  \char ##x2460 NOTE 11 at 11:1 - D

}
\markup \wordwrap \with-color #(rgb-color 0.000 0.667 0.000) {
  - Far + 1far MDC (3rd)

}
\markup \wordwrap \bold {
      \vspace #2
"#"12 (from midi/GenCA1/test-repeat.mid), Rule penalty: 1  \char ##x27F9  0, Distance penalty: 5, Cantus: low, Key: Am
}
<<
\new Staff {
  \set Staff.instrumentName = #"2"
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

  {  \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "t" }
 \speakOff \revert NoteHead.style f'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style g'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "d" }
 \speakOff \revert NoteHead.style e'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style f'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style g'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "d" }
 \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "t" }
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "sII" }
 \speakOff \revert NoteHead.style e''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "d" }
 \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "t" }
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "sII" }
 \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "t" }

  }
}
>>
\markup \wordwrap \bold {
      \vspace #2
"#"13 (from midi/GenCA1/test-repeat.mid), Rule penalty: 0, Cantus: low, Key: C
}
<<
\new Staff {
  \set Staff.instrumentName = #"1"
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

  {  \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "S" }
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style g'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style f'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "S" }
 \speakOff \revert NoteHead.style e'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style g'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }

  }
}
>>
\markup \wordwrap \bold {
      \vspace #2
"#"14 (from midi/GenCA1/test-repeat.mid), Rule penalty: 62  \char ##x27F9  61, Distance penalty: 5, Cantus: low, Key: Am
}
<<
\new Staff {
  \set Staff.instrumentName = #"2"
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

  {  \speakOff \revert NoteHead.style c''1 ^\markup{ \tiny \with-color #(rgb-color 0.592 0.269 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "t" }
 \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "sII" }
 \speakOff \revert NoteHead.style e''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "sII" }
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "dVII" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "s" }
 \speakOff \revert NoteHead.style e''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style f''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style e''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "d" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "s" }
 \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "sII" }
 \speakOff \revert NoteHead.style gs'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "t" }

  }
}
\new Staff {
  \set Staff.instrumentName = #"1"
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

  {  \speakOff \revert NoteHead.style  \circle c''1 ^\markup{ \tiny \with-color #(rgb-color 0.592 0.269 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "t" }
 \speakOff \revert NoteHead.style  \circle a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "sII" }
 \speakOff \revert NoteHead.style e''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "s" }
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "dVII" }
 \speakOff \revert NoteHead.style  \circle c''1 ^\markup{ \tiny \with-color #(rgb-color 0.000 0.667 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "t" }
 \speakOff \revert NoteHead.style e''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "d" }
 \speakOff \revert NoteHead.style f''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style e''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "d" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "sII" }
 \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "sII" }
 \speakOff \revert NoteHead.style gs'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "t" }

  }
}
>>
\markup \wordwrap \bold {
  \char ##x2461 NOTE 1 at 1:1 - C

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - First note (not GC)

}
\markup \wordwrap \bold {
  \char ##x2460 NOTE 1 at 1:1 - C

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - First note (not GC)

}
\markup \wordwrap \bold {
  \char ##x2460 NOTE 7 at 7:1 - C

}
\markup \wordwrap \with-color #(rgb-color 0.000 0.667 0.000) {
  - Far + 1far MDC (3rd)

}
\markup \wordwrap \bold {
      \vspace #2
"#"14 (from midi/GenCA1/test-repeat.mid), Rule penalty: 62  \char ##x27F9  0, Distance penalty: 17, Cantus: low, Key: Am
}
<<
\new Staff {
  \set Staff.instrumentName = #"2"
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

  {  \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "t" }
 \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "sII" }
 \speakOff \revert NoteHead.style e''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "s" }
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "sII" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "s" }
 \speakOff \revert NoteHead.style e''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style f''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "s" }
 \speakOff \revert NoteHead.style e''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "d" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "sII" }
 \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "sII" }
 \speakOff \revert NoteHead.style gs'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "t" }

  }
}
>>
\markup \wordwrap \bold {
      \vspace #2
"#"15 (from midi/GenCA1/test-repeat.mid), Rule penalty: 62  \char ##x27F9  61, Distance penalty: 5, Cantus: low, Key: C
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

  {  \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "S" }
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style e''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "S" }
 \speakOff \revert NoteHead.style g'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style f'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style g'1 ^\markup{ \tiny \with-color #(rgb-color 0.592 0.269 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }

  }
}
\new Staff {
  \set Staff.instrumentName = #"1"
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

  {  \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "S" }
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style  \circle d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style g'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style f'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "S" }
 \speakOff \revert NoteHead.style g'1 ^\markup{ \tiny \with-color #(rgb-color 0.592 0.269 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style  \circle b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style  \circle c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style d''1 ^\markup{ \tiny \with-color #(rgb-color 0.000 0.667 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }

  }
}
>>
\markup \wordwrap \bold {
  \char ##x2461 NOTE 11 at 11:1 - G

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - Compound tritone (unresolved)

}
\markup \wordwrap \bold {
  \char ##x2460 NOTE 11 at 11:1 - G

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - Compound tritone (unresolved)

}
\markup \wordwrap \bold {
  \char ##x2460 NOTE 16 at 16:1 - D

}
\markup \wordwrap \with-color #(rgb-color 0.000 0.667 0.000) {
  - Culmination (multi low) (Multiple culminations in lower voice)

}
\markup \wordwrap \bold {
      \vspace #2
"#"15 (from midi/GenCA1/test-repeat.mid), Rule penalty: 62  \char ##x27F9  0, Distance penalty: 11, Cantus: low, Key: C
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

  {  \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "S" }
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style g'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style f'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "S" }
 \speakOff \revert NoteHead.style g'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "S" }
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style e''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style d''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }

  }
}
>>
\header {tagline = "This file was created by MGen 2.7.83.g6594e.dirty and engraved by LilyPond"}
