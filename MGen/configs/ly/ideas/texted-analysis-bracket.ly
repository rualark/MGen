\version "2.14.1"
\paper { tagline = ##f }
#(define (label text color)
  (lambda (grob)
    (ly:stencil-aligned-to
      (ly:stencil-combine-at-edge
        (ly:stencil-aligned-to (ly:horizontal-bracket::print grob) X CENTER)
        Y (ly:grob-property grob 'direction)
        (ly:stencil-aligned-to (grob-interpret-markup grob (markup 
                                                            #:with-color color
                                                            (#:teeny text))) X CENTER)
      0.2)
     X LEFT)))

\layout {
  \context {
    \Voice
    \consists "Horizontal_bracket_engraver"
  }
}

\relative c'' {
  
  c4-\tweak #'stencil #(label "A" (rgb-color 0 1 0))\startGroup
    -\tweak #'stencil #(label "a" red)\startGroup
  d4\stopGroup
  %% following tweak seems to have no effect
  e4-\tweak #'stencil #(label "b" red)\startGroup
  d4\stopGroup\stopGroup
}


