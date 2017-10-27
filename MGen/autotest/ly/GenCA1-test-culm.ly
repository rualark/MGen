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
  subtitle = "GenCA1: test-culm (2017-10-27 19:35)"
}

\markup \wordwrap \bold {
      \vspace #2
"#"1 (from midi/GenCA1/test-culm.mid), Rule penalty: 183  \char ##x27F9  0, Distance penalty: 6, Cantus: high, Key: C
}
<<
\new Sta ff {
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
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style g'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style f'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "S" }
 \speakOff \revert NoteHead.style g'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style d'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style c'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }

  }
}
\new Sta ff {
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
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "S" }
 \speakOff \revert NoteHead.style  \circle g'1 ^\markup{ \tiny \with-color #(rgb-color 0.592 0.269 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style f'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "S" }
 \speakOff \revert NoteHead.style  \circle e'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style d'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style c'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }

  }
}
>>
\markup \wordwrap \bold {
  \char ##x2460 NOTE 4 at 4:1 - G

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - CF Stepwise motion (>4 one direction) (Stepwise motion more than X notes in one direction)

}
\markup \wordwrap \bold {
      \vspace #2
"#"1 (from midi/GenCA1/test-culm.mid), Rule penalty: 183  \char ##x27F9  0, Distance penalty: 6, Cantus: high, Key: C
}
<<
\new Sta ff {
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
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "S" }
 \speakOff \revert NoteHead.style e'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style f'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style e'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style d'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style c'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }

  }
}
>>
\markup \wordwrap \bold {
      \vspace #2
"#"2 (from midi/GenCA1/test-culm.mid), Rule penalty: 396  \char ##x27F9  0, Distance penalty: 12, Cantus: high, Key: C
}
<<
\new Sta ff {
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
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style g'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style f'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style g'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style d'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style c'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }

  }
}
\new Sta ff {
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

  {  \speakOff \revert NoteHead.style  \circle a'1 ^\markup{ \tiny \with-color #(rgb-color 0.690 0.204 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "S" }
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style g'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style f'1 ^\markup{ \tiny \with-color #(rgb-color 0.592 0.269 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "S" }
 \speakOff \revert NoteHead.style  \circle e'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style d'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style c'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }

  }
}
>>
\markup \wordwrap \bold {
  \char ##x2460 NOTE 1 at 1:1 - A

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - First note (not GC)

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
  \char ##x2460 NOTE 5 at 5:1 - F

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - CF Stepwise motion (>4 one direction) (Stepwise motion more than X notes in one direction)

}
\markup \wordwrap \bold {
      \vspace #2
"#"2 (from midi/GenCA1/test-culm.mid), Rule penalty: 396  \char ##x27F9  0, Distance penalty: 12, Cantus: high, Key: C
}
<<
\new Sta ff {
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
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style g'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style f'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "S" }
 \speakOff \revert NoteHead.style g'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style d'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style c'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }

  }
}
>>
\markup \wordwrap \bold {
      \vspace #2
"#"3 (from midi/GenCA1/test-culm.mid), Rule penalty: 335  \char ##x27F9  0, Distance penalty: 14, Cantus: high, Key: C
}
<<
\new Sta ff {
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
 \speakOff \revert NoteHead.style g'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "S" }
 \speakOff \revert NoteHead.style g'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style f'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "S" }
 \speakOff \revert NoteHead.style e'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style b1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style c'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }

  }
}
\new Sta ff {
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

  {  \speakOff \revert NoteHead.style  \circle f'1 ^\markup{ \tiny \with-color #(rgb-color 0.690 0.204 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "S" }
 \speakOff \revert NoteHead.style g'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style g'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style  \circle f'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "S" }
 \speakOff \revert NoteHead.style e'1 ^\markup{ \tiny \with-color #(rgb-color 0.592 0.269 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style  \circle d'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style c'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }

  }
}
>>
\markup \wordwrap \bold {
  \char ##x2460 NOTE 1 at 1:1 - F

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - First note (not GC)

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
  \char ##x2460 NOTE 6 at 6:1 - E

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - CF Stepwise motion (>4 one direction) (Stepwise motion more than X notes in one direction)

}
\markup \wordwrap \bold {
      \vspace #2
"#"3 (from midi/GenCA1/test-culm.mid), Rule penalty: 335  \char ##x27F9  0, Distance penalty: 14, Cantus: high, Key: C
}
<<
\new Sta ff {
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
 \speakOff \revert NoteHead.style g'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style g'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style d'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style e'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style d'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style c'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }

  }
}
>>
\markup \wordwrap \bold {
      \vspace #2
"#"4 (from midi/GenCA1/test-culm.mid), Rule penalty: 133  \char ##x27F9  0, Distance penalty: 10, Cantus: high, Key: C
}
<<
\new Sta ff {
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
 \speakOff \revert NoteHead.style e'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style f'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style f'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style e'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style d'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style c'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }

  }
}
\new Sta ff {
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

  {  \speakOff \revert NoteHead.style  \circle d'1 ^\markup{ \tiny \with-color #(rgb-color 0.690 0.204 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style e'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style f'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "S" }
 \speakOff \revert NoteHead.style  \circle g'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style f'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style e'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style d'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style c'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }

  }
}
>>
\markup \wordwrap \bold {
  \char ##x2460 NOTE 1 at 1:1 - D

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
      \vspace #2
"#"4 (from midi/GenCA1/test-culm.mid), Rule penalty: 133  \char ##x27F9  0, Distance penalty: 10, Cantus: high, Key: C
}
<<
\new Sta ff {
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
 \speakOff \revert NoteHead.style e'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style f'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style f'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style e'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style d'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style c'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }

  }
}
>>
\markup \wordwrap \bold {
      \vspace #2
"#"5 (from midi/GenCA1/test-culm.mid), Rule penalty: 133  \char ##x27F9  0, Distance penalty: 10, Cantus: high, Key: C
}
<<
\new Sta ff {
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
 \speakOff \revert NoteHead.style a1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style d'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style e'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style f'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style e'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style d'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style c'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }

  }
}
\new Sta ff {
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

  {  \speakOff \revert NoteHead.style  \circle b1 ^\markup{ \tiny \with-color #(rgb-color 0.690 0.204 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style  \circle c'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style d'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style e'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style f'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style e'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style d'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style c'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }

  }
}
>>
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
  - Start steps tonic (no E)

}
\markup \wordwrap \bold {
      \vspace #2
"#"5 (from midi/GenCA1/test-culm.mid), Rule penalty: 133  \char ##x27F9  0, Distance penalty: 10, Cantus: high, Key: C
}
<<
\new Sta ff {
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
 \speakOff \revert NoteHead.style a1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style d'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style e'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style f'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "S" }
 \speakOff \revert NoteHead.style e'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style d'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style c'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }

  }
}
>>
\markup \wordwrap \bold {
      \vspace #2
"#"6 (from midi/GenCA1/test-culm.mid), Rule penalty: 224  \char ##x27F9  0, Distance penalty: 16, Cantus: high, Key: C
}
<<
\new Sta ff {
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
 \speakOff \revert NoteHead.style a1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style c'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style d'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style e'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style f'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style d'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style c'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }

  }
}
\new Sta ff {
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

  {  \speakOff \revert NoteHead.style  \circle a1 ^\markup{ \tiny \with-color #(rgb-color 0.690 0.204 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style  \circle b1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style c'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style d'1 ^\markup{ \tiny \with-color #(rgb-color 0.592 0.269 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style e'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style  \circle f'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "S" }
 \speakOff \revert NoteHead.style  \circle e'1 ^\markup{ \tiny \with-color #(rgb-color 0.294 0.468 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style c'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }

  }
}
>>
\markup \wordwrap \bold {
  \char ##x2460 NOTE 1 at 1:1 - A

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - First note (not GC)

}
\markup \wordwrap \with-color #(rgb-color 0.690 0.204 0.000) {
  - Start steps tonic (no E)

}
\markup \wordwrap \bold {
  \char ##x2460 NOTE 4 at 4:1 - D

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - CF Stepwise motion (>4 one direction) (Stepwise motion more than X notes in one direction)

}
\markup \wordwrap \bold {
  \char ##x2460 NOTE 7 at 7:1 - E

}
\markup \wordwrap \with-color #(rgb-color 0.294 0.468 0.000) {
  - 2nd to last note (not GBD)

}
\markup \wordwrap \bold {
      \vspace #2
"#"6 (from midi/GenCA1/test-culm.mid), Rule penalty: 224  \char ##x27F9  0, Distance penalty: 16, Cantus: high, Key: C
}
<<
\new Sta ff {
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
 \speakOff \revert NoteHead.style b1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style c'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style d'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style e'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style g'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style d'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style c'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }

  }
}
>>
\markup \wordwrap \bold {
      \vspace #2
"#"7 (from midi/GenCA1/test-culm.mid), Rule penalty: 285  \char ##x27F9  0, Distance penalty: 18, Cantus: high, Key: C
}
<<
\new Sta ff {
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
 \speakOff \revert NoteHead.style b1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style d'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style e'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style f'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "S" }
 \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style g'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style c'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }

  }
}
\new Sta ff {
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

  {  \speakOff \revert NoteHead.style  \circle b1 ^\markup{ \tiny \with-color #(rgb-color 0.690 0.204 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style  \circle c'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style d'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style e'1 ^\markup{ \tiny \with-color #(rgb-color 0.592 0.269 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style f'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style  \circle g'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style  \circle a'1 ^\markup{ \tiny \with-color #(rgb-color 0.294 0.468 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style c'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }

  }
}
>>
\markup \wordwrap \bold {
  \char ##x2460 NOTE 1 at 1:1 - B

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - First note (not GC)

}
\markup \wordwrap \with-color #(rgb-color 0.690 0.204 0.000) {
  - Start steps tonic (no E)

}
\markup \wordwrap \bold {
  \char ##x2460 NOTE 4 at 4:1 - E

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - CF Stepwise motion (>4 one direction) (Stepwise motion more than X notes in one direction)

}
\markup \wordwrap \bold {
  \char ##x2460 NOTE 7 at 7:1 - A

}
\markup \wordwrap \with-color #(rgb-color 0.294 0.468 0.000) {
  - 2nd to last note (not GBD)

}
\markup \wordwrap \bold {
      \vspace #2
"#"7 (from midi/GenCA1/test-culm.mid), Rule penalty: 285  \char ##x27F9  0, Distance penalty: 18, Cantus: high, Key: C
}
<<
\new Sta ff {
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
 \speakOff \revert NoteHead.style b1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style d'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style e'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style f'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "S" }
 \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style g'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style c'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }

  }
}
>>
\markup \wordwrap \bold {
      \vspace #2
"#"8 (from midi/GenCA1/test-culm.mid), Rule penalty: 285  \char ##x27F9  0, Distance penalty: 18, Cantus: high, Key: C
}
<<
\new Sta ff {
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
 \speakOff \revert NoteHead.style b1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style d'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style e'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style f'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "S" }
 \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style g'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }

  }
}
\new Sta ff {
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

  {  \speakOff \revert NoteHead.style  \circle b1 ^\markup{ \tiny \with-color #(rgb-color 0.690 0.204 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style  \circle c'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style d'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style e'1 ^\markup{ \tiny \with-color #(rgb-color 0.592 0.269 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style f'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "S" }
 \speakOff \revert NoteHead.style  \circle g'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style  \circle a'1 ^\markup{ \tiny \with-color #(rgb-color 0.294 0.468 0.000) \char ##x2716  }
_\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "S" }
 \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }

  }
}
>>
\markup \wordwrap \bold {
  \char ##x2460 NOTE 1 at 1:1 - B

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - First note (not GC)

}
\markup \wordwrap \with-color #(rgb-color 0.690 0.204 0.000) {
  - Start steps tonic (no E)

}
\markup \wordwrap \bold {
  \char ##x2460 NOTE 4 at 4:1 - E

}
\markup \wordwrap \with-color #(rgb-color 0.592 0.269 0.000) {
  - CF Stepwise motion (>4 one direction) (Stepwise motion more than X notes in one direction)

}
\markup \wordwrap \bold {
  \char ##x2460 NOTE 7 at 7:1 - A

}
\markup \wordwrap \with-color #(rgb-color 0.294 0.468 0.000) {
  - 2nd to last note (not GBD)

}
\markup \wordwrap \bold {
      \vspace #2
"#"8 (from midi/GenCA1/test-culm.mid), Rule penalty: 285  \char ##x27F9  0, Distance penalty: 18, Cantus: high, Key: C
}
<<
\new Sta ff {
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
 \speakOff \revert NoteHead.style b1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style d'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style e'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "TSVI" }
 \speakOff \revert NoteHead.style f'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "S" }
 \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style g'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style c''1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }

  }
}
>>
\markup \wordwrap \bold {
      \vspace #2
"#"9 (from midi/GenCA1/test-culm.mid), Rule penalty: 0, Cantus: high, Key: C
}
<<
\new Sta ff {
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
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DVII" }
 \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "S" }
 \speakOff \revert NoteHead.style g'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style f'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "S" }
 \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style g'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style b'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style a'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style g'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }
 \speakOff \revert NoteHead.style f'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "SII" }
 \speakOff \revert NoteHead.style e'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "DTIII" }
 \speakOff \revert NoteHead.style f'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "S" }
 \speakOff \revert NoteHead.style d'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "D" }
 \speakOff \revert NoteHead.style c'1 _\markup{ \tiny \with-color #(rgb-color 0.588 0.588 0.588) "T" }

  }
}
>>
\header {tagline = "This file was created by MGen 2.7.87.gaa39b.dirty and engraved by LilyPond"}
