%% http://lsr.di.unimi.it/LSR/Item?id=377
%% see also http://lilypond.org/doc/v2.18/Documentation/notation/graphic

% The below, invented by Mats Bengtsson, creates left and right brackets vertically 
% spanning an entire staff. It is useful for offseting optional passages of music,
% as shown in the example

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% LSR workaround:
#(set! paper-alist (cons '("snippet" . (cons (* 190 mm) (* 50 mm))) paper-alist))
\paper {
  #(set-paper-size "snippet")
  indent = 15
  tagline = ##f
}

\markup\vspace #.5
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%here starts the snippet:

% The number next to "th" in (th 0.2) controls thickness of the brackets. 
#(define-markup-command (left-bracket layout props) ()
"Draw left hand bracket"
(let* ((th 0.2) ;; todo: take from GROB
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
  (let* ((th .2);;todo: take from GROB
          (width (* 2.5 th)) ;; todo: take from GROB
          (ext '(-2.8 . 2.8))) ;; todo: take line-count into account
        (ly:bracket Y ext th (- width))))

rightBracket = {
  \once\override BreathingSign.text = #(make-right-bracket-markup)
  \once\override BreathingSign.Y-offset = ##f
  \breathe
}

%example of use:
\score { 
  \relative c'{ 
    \set Score.timing=##f \key f \major
    f8( g4 a2 a8-- g4 f8 g4 f g f8[ e] f4 e2 f4 e2  
    \leftBracket 
    g4. 
    \rightBracket 
    a8 bes2 a4 g a bes16[ a g8 a bes] c2 
    \bar""
    \break  
    bes8[ a] g4 bes a2 g8[ a bes c a] bes2 a4.\( bes8\) a4 g g8_-[ f16 g] a4 g f g2) 
    \rightBracket 
    g8([ bes a g a bes]) 
  }
  \layout{ 
    \context{ 
      \Staff 
      \remove "Time_signature_engraver" 
    } 
  }
}