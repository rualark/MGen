\version "2.18.2"
%% \gliss #'(0 . shift) #'Ypoint2 note\startTextSpan note\stopTextSpan
gliss = #(define-music-function (parser location shift pad) (pair? number?) #{
 	\once \override TextSpanner.style = #'line
    \once \override TextSpanner.bound-details.left.padding = #'1
    \once \override TextSpanner.bound-details.right.padding = #1
    \once \override TextSpanner.extra-offset = #shift 
    \override TextSpanner.bound-details.right.Y = #pad
    \once \override TextSpanner.bound-details.right.text = #'()
#})
% Short command for start and stop  TextSpan
#(define Stsp startTextSpan )
#(define Stpsp stopTextSpan )

\new Staff {
    \override Glissando.minimum-length = #5
    \override Glissando.springs-and-rods = #ly:spanner::set-spacing-rods
    \override Glissando.thickness = #'3
    \override Glissando.color=#(rgb-color 0.988 0.005 0.000)
   \gliss #'(0.0 . -5) #'-4  b'4\Stsp g''8 f'\Stpsp 
}

\relative c' {
    \override Glissando.minimum-length = #5
    \override Glissando.springs-and-rods = #ly:spanner::set-spacing-rods
    \override Glissando.thickness = #'3
    \override Glissando.color=#(rgb-color 0.988 0.005 0.000)
  a2 \glissando
  \override NoteColumn.glissando-skip = ##t
  c4 d e 
  \glissando
  c d e 
  \override NoteColumn.glissando-skip = ##f
  b c d e f g
}

