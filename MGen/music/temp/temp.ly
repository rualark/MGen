\version "2.18.2"
\language "english"

\markup {
  "123.mid Cantus #1, counterpoint #2, species 3, CF=low"
}
\new Staff { \clef "treble" \key c \major }
\new Staff { \clef "treble" \key d \major }
\new Staff { \clef "treble" \key e \major }
\new Staff { \clef "treble" \key f \major }
\new Staff { \clef "treble" \key g \major }
\new Staff { \clef "treble" \key a \major }
\new Staff { \clef "treble" \key b \major }
\new Staff { \clef "treble" \key cs \major }
\new Staff { \clef "treble" \key ds \major }
\new Staff { \clef "treble" \key fs \major }
\new Staff { \clef "treble" \key gs \major }
\new Staff { \clef "treble" \key as \major }
\new Staff { \clef "treble" \key bf \major }
\new Staff { \clef "treble" \key ds \major }
\new Staff { \clef "treble" \key fs \major }
\new Staff { \clef "treble" \key gs \major }
\new Staff { \clef "treble" \key as \major }
<<
\new Staff { \clef "treble" \key d \major \time 4/4
  \set Score.barNumberVisibility = #all-bar-numbers-visible
  \override Score.BarNumber.break-visibility = ##(#f #t #t)
  \bar ""
  c' d' e' f' g' a' b' c' d' e' f' g' a' b'
  c' d' e' f' g' a' b' c' d' e' f' g' a' b'
\override NoteHead.color = #(rgb-color 1 0 1)
  c' d' e' f' g' a' b'
\override NoteHead.color = #(rgb-color 0 0 0)
  c' d' e' f' g' a' b'
}
\new Staff { \clef "bass"  \key d \major \time 4/4
  \set Score.barNumberVisibility = #all-bar-numbers-visible
  \override Score.BarNumber.break-visibility = ##(#f #t #t)
  \bar ""
  c d e f g a b c d e f g a b c d e f g a b c d e f g a b
  c d e f g a b c d e f g a b
}
\new Staff { \clef "treble" \key d \major \time 4/4
  \set Score.barNumberVisibility = #all-bar-numbers-visible
  \override Score.BarNumber.break-visibility = ##(#f #t #t)
  \bar ""
  c' d' e' f' g' a' b' c' d' e' f' g' a' b'
  c' d' e' f' g' a' b' c' d' e' f' g' a' b'
\override NoteHead.color = #(rgb-color 1 0 1)
  c' d' e' f' g' a' b'
\override NoteHead.color = #(rgb-color 0 0 0)
  c' d' e' f' g' a' b'
}
\new Staff { \clef "bass"  \key d \major \time 4/4
  \set Score.barNumberVisibility = #all-bar-numbers-visible
  \override Score.BarNumber.break-visibility = ##(#f #t #t)
  \bar ""
  c d e f g a b c d e f g a b c d e f g a b c d e f g a b
  c d e f g a b c d e f g a b
}
>>
\markup {
  \fill-line {
    \hspace #1
    \column {
      \line { O sacrum convivium }
      \line { in quo Christus sumitur, }
      \line { recolitur memoria passionis ejus, }
      \line { mens impletur gratia, }
\with-color #(rgb-color 1 0 1)
\line { futurae gloriae nobis pignus datur. }
      \line { Amen. }
    }
    \hspace #2
    \column \italic {
      \line { O sacred feast }
      \line { in which Christ is received, }
      \line { the memory of His Passion is renewed, }
      \line { the mind is filled with grace, }
      \line { and a pledge of future glory is given to us. }
      \line { Amen. }
    }
    \hspace #1
  }
}
\header { 
  tagline = "This file was created by MGen and engraved by LilyPond"  % removed 
} 