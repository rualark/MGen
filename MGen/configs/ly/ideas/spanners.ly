\version "2.18.2"

\relative c''
{
\sourcefileline 331
c4~( c8 d~\( d4 e) f c\) d, e

}

\relative {
  d''1\startTrillSpan
  d1 e \startTrillSpan
  f g 
  a b\stopTrillSpan
  c2\stopTrillSpan
  r2
}

stringNumberSpanner =
#(define-music-function (parser location StringNumber) (string?)
  #{
    \override TextSpanner.style = #'solid
    \override TextSpanner.font-size = #-5
    \override TextSpanner.bound-details.left.stencil-align-dir-y = #CENTER
    \override TextSpanner.bound-details.left.text = \markup { \circle \number #StringNumber }
  #})

stringNumberSpannerZ =
#(define-music-function (parser location StringNumber) (string?)
  #{
    \override TextSpanner.style = #'dashed-line
      \override TextSpanner.dash-fraction = #0.3
  \override TextSpanner.dash-period = #1
    \override TextSpanner.font-size = #-5
    \override TextSpanner.bound-details.left.stencil-align-dir-y = #CENTER
    \override TextSpanner.bound-details.left.text = \markup { " " }
  #})



\layout {
  \context {
    \Voice
    \consists "Horizontal_bracket_engraver"
  }
}

glissandoSkipOn = {
  \override NoteColumn.glissando-skip = ##t
  \hide NoteHead
  \override NoteHead.no-ledgers = ##t
}

glissandoSkipOff = {
  \revert NoteColumn.glissando-skip
  \undo \hide NoteHead
  \revert NoteHead.no-ledgers
}


\relative c {
  \override Score.VoltaBracket #'font-name = #"New Century Schoolbook" 
   \override Score.VoltaBracket #'font-shape = #'bold 
\clef "treble_8"
  \stringNumberSpanner "59"
  \textSpannerDown
  a8\startTextSpan
  b \startTrillSpan 
  \startGroup
  c (  
\set Score.repeatCommands = #`((volta ,#{ \markup "some text" #} )) 
d
  
  \(e 
\glissando
  \override NoteColumn.glissando-skip = ##t
  f
  \stopTextSpan
\stringNumberSpannerZ "4"
   \set Score.repeatCommands = #'((volta #f)) 

g)\startTextSpan a
  \stopGroup
  d4\) 
    \override NoteColumn.glissando-skip = ##f
  g, 
  
  \stopTrillSpan
  e2\stopTextSpan
}


\relative c {
  \override Score.VoltaBracket #'font-name = #"New Century Schoolbook" 
   \override Score.VoltaBracket #'font-shape = #'bold 
    \override Score.VoltaBracket #'edge-height = #'(0 . 0) 

\clef "treble_8"
  \stringNumberSpannerZ "59"
  \textSpannerDown
  a8\startTextSpan
  b \startTrillSpan 
  \startGroup
  c (  
\set Score.repeatCommands = #`((volta ,#{ \markup " " #} )) 
d
  
  \(e 
\glissando
  \override NoteColumn.glissando-skip = ##t
  f
  \stopTextSpan
\stringNumberSpannerZ "4"
   \set Score.repeatCommands = #'((volta #f)) 

g)\startTextSpan a
  \stopGroup
  d4\) 
    \override NoteColumn.glissando-skip = ##f
  g, 
  
  \stopTrillSpan
  e2\stopTextSpan
}


\relative c {
  \override Score.VoltaBracket #'font-name = #"New Century Schoolbook" 
   \override Score.VoltaBracket #'font-shape = #'bold 
    \override Score.VoltaBracket #'edge-height = #'(0 . 0) 

\clef "treble_8"
  \stringNumberSpannerZ "59"
  \textSpannerDown
  a8\startTextSpan
  b \startTrillSpan \stopTextSpan
  \startGroup
  c (  \stopTrillSpan \stopGroup \ottava #-1
\set Score.repeatCommands = #`((volta ,#{ \markup " " #} )) 
d 
  
  \(
  \ottava #0
  \sustainOn
  e 
  \sustainOff
\glissando
  \override NoteColumn.glissando-skip = ##t
  f
  
\stringNumberSpannerZ "4"
   \set Score.repeatCommands = #'((volta #f)) 

g)\startTextSpan a
  
  d4\) 
    \override NoteColumn.glissando-skip = ##f
  g, 
  
  
  e2\stopTextSpan
}

\relative c' { 
  \override Score.VoltaBracket #'font-name = #"New Century Schoolbook" 
   \override Score.VoltaBracket #'font-shape = #'bold 
   d'4. c8 b a g f 
   \set Score.repeatCommands = #`((volta ,#{ \markup  "some text" #} )) 
   e4 e' e r \bar "||" 
   \set Score.repeatCommands = #'((volta #f)) 
   c d e 
   \set Score.repeatCommands = #'((volta "Fine asdf.")) 
   e,4 
   \set Score.repeatCommands = #'((volta #f)) 
   c'4 c8 c c4 c8 c 
} 
