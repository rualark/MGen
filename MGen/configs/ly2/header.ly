\version "2.18.2"
\language "english"
\paper { 
  #(include-special-characters) 
  bottom-margin = 0.27\in
  #(define page-breaking ly:$PAGE_BREAKING$)
}

#(define (sign x)
  (if (= x 0)
      0
      (if (< x 0) -1 1)))
      
#(define (radians->degree radians)
  (/ (* radians 180) PI))

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%% Glissando with text
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%% Reads from Glissando.details
%%%%   - glissando-text
%%%%   - glissando-text-font-size
%%%%   - glissando-text-padding

#(define* ((gliss-plus-text #:optional always-add-text) grob)
  (let*  ((orig-grob (ly:grob-original grob))
          (broken-beams (ly:spanner-broken-into orig-grob))
          (stil (ly:line-spanner::print grob)))
    (if (or (null? broken-beams)
            (and (pair? broken-beams)
                 (or
                    always-add-text
                    (equal? grob (car broken-beams)))))
        (if (not (ly:stencil? stil))
            #f
            (let* ((layout (ly:grob-layout grob))
                   (line-thickness
                     (ly:output-def-lookup layout 'line-thickness))
                   (props
                     (ly:grob-alist-chain
                       grob
                       (ly:output-def-lookup layout 'text-font-defaults)))
                   (font
                     (ly:paper-get-font
                     layout
                     (cons
                       '((font-encoding . fetaMusic) (font-name . #f))
                       props)))
                   (trill-glyph
                     (ly:font-get-glyph font "scripts.trill_element"))
                   (trill-glyph-height
                     (interval-length (ly:stencil-extent trill-glyph Y)))
                   (zigzag-width
                     (ly:grob-property grob 'zigzag-width))
                   (thickness (ly:grob-property grob 'thickness))
                   (thick
                     (if (number? thickness)
                         (/ thickness 10)
                         line-thickness))
                   (style (ly:grob-property grob 'style))
                   (corr
                     (cond ((eq? style 'trill)
                             trill-glyph-height)
                           ((eq? style 'zigzag)
                             (+ thick zigzag-width))
                           (else 0)))
                   (stil-x-ext (ly:stencil-extent stil X))
                   (stil-y-ext (ly:stencil-extent stil Y))
                   (stil-x-length (- (cdr stil-x-ext) (car stil-x-ext)))
                   (stil-y-length (- (cdr stil-y-ext) (car stil-y-ext)))
                   (details (ly:grob-property grob 'details))
                   (gliss-text
                     (assoc-get 'glissando-text details "gliss."))
                   (gliss-text-font-size
                     (assoc-get 'glissando-text-font-size details -5))
                   (gliss-text-padding
                     (assoc-get 'glissando-text-padding details 0.5))
                   (left-bound-info (ly:grob-property grob 'left-bound-info))
                   (y-left (assoc-get 'Y left-bound-info))
                   (right-bound-info (ly:grob-property grob 'right-bound-info))
                   (y-right (assoc-get 'Y right-bound-info))
                   (slant (sign (- y-right y-left)))
                   (gradient
                     (/ (- stil-y-length corr) stil-x-length))
                   ;; `ly:stencil-rotate' needs an angle in degrees...
                   ;; TODO use ly:angle ?
                   (alpha
                     (radians->degree
                       (angle
                         (make-rectangular
                           stil-x-length
                           (- stil-y-length corr line-thickness)))))
                   (text-stencil
                     (grob-interpret-markup
                       grob
                         (make-halign-markup CENTER
                           (make-fontsize-markup
                             gliss-text-font-size
                             gliss-text))))
                   (text-stencil-height
                     (interval-length (ly:stencil-extent text-stencil Y)))
                   (y-move
                     (+
                       (/ text-stencil-height 2)
                       gliss-text-padding))
                   (rotated-text-stil
                     (ly:stencil-rotate text-stencil (* slant alpha) 0 0))
                   (text-center-X
                     (interval-center (ly:stencil-extent rotated-text-stil X)))
                   (translated-text-stencil
                     (ly:stencil-translate
                       rotated-text-stil
                       ;; Constuction-helpers
                       ;text-stencil
                       ;(make-cross-stencil '(0 . 0)) ;; not included atm
                       (cons
                         (+
                           (car stil-x-ext)
                           (/ stil-x-length 2)
                           (* slant -1 y-move gradient))
                         (+
                           (car stil-y-ext)
                           (/ stil-y-length 2)
                           y-move)))))
            (ly:stencil-add
              stil
              ;; Construction-helpers
              ;;
              ;(make-line-stencil
              ;  0.1
              ;  (+ (car stil-x-ext)(/ stil-x-length 2))
              ;  (+ (car stil-y-ext)(/ stil-y-length 2))
              ;  (+ (car stil-x-ext)(/ stil-x-length 2)(* slant -1 10 gradient))
              ;  10)
              ;;
              ;; (2) colors the text-part
              ;;
              ;(stencil-with-color translated-text-stencil red)
              translated-text-stencil
              )))
        stil)))

glissandoTextOn =
  \temporary \override Glissando.stencil =
    #(gliss-plus-text
      ;; change to true, if added text is wished for both parts of a broken
      ;; glissando
      ;; TODO added text at second parts needs to be improved, vertical
      ;; positioning is sometimes bad
      #f)

glissandoTextOff = \revert Glissando.stencil

circle =
\once \override NoteHead.stencil = #(lambda (grob)
    (let* ((note (ly:note-head::print grob))
           (combo-stencil (ly:stencil-add
               note
               (circle-stencil note 0.1 0.2))))
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

#(define-markup-command (left-bracket layout props) ()
"Draw left hand bracket"
(let* ((th 0.3) ;; todo: take from GROB
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
  (let* ((th .3);;todo: take from GROB
          (width (* 2.5 th)) ;; todo: take from GROB
          (ext '(-2.8 . 2.8))) ;; todo: take line-count into account
        (ly:bracket Y ext th (- width))))

rightBracket = {
  \once\override BreathingSign.text = #(make-right-bracket-markup)
  \once\override BreathingSign.Y-offset = ##f
  \breathe 
}

#(define (label text color)
  (lambda (grob)
    (ly:stencil-aligned-to
      (ly:stencil-combine-at-edge
        (ly:stencil-aligned-to (ly:horizontal-bracket::print grob) X LEFT)
        Y (ly:grob-property grob 'direction)
        (ly:stencil-aligned-to (grob-interpret-markup grob (markup 
                                                            #:with-color color
                                                            (#:teeny text))) X LEFT)
      0.2)
     X LEFT)))

myTS =
#(define-music-function (parser location st color) (string? color?)
  #{
    \override TextSpanner.style = #'dashed-line
      \override TextSpanner.dash-fraction = #0.3
  \override TextSpanner.dash-period = #1
    \override TextSpanner.font-size = #-3
    \override TextSpanner.bound-details.left.stencil-align-dir-y = #CENTER
    \override TextSpanner.bound-details.left.text = #st
    \override TextSpanner.color = #color
    \override TextSpanner.thickness = #2
  #})

\header {
  title = "$TITLE$"
  subtitle = "$SUBTITLE$"
  subsubtitle = "$SUBSUBTITLE$"
  dedication = "$DEDICATION$"
}
