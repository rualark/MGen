\version "2.18.2"
\score {
  <<
    \new Lyrics = "lyrics" \with {
      % lyrics above a staff should have this override
      \override VerticalAxisGroup.staff-affinity = #DOWN
    }
    \new Staff {
      \new Voice = "melody" {
        \relative { 
          c''4 
          c 
 \override Score.VoltaBracket.outside-staff-priority = #40
 \override Score.VoltaBracket.color = #(rgb-color 0.690 0.204 0.000)
 \set Score.repeatCommands = #'((volta "rng"))
          c 
 \set Score.repeatCommands = #'((volta #f))
          c }
      }
    }
    \context Lyrics = "lyrics" {
      \lyricsto "melody" {
        Here are the words
      }
    }
  >>
}