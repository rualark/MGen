%%%  BOILERPLATE SNIPPET BEGINS 
\version "2.19" 

pedalSim = 
\once \override Staff.PianoPedalBracket.stencil = 
  #(lambda (grob) 
   (let* (;; have we been split? 
         (orig (ly:grob-original grob)) 
         ;; if yes, get the split pieces (our siblings) 
         (siblings (if (ly:grob? orig) 
                       (ly:spanner-broken-into orig) 
                       '()))) 
   ;; Modify the unbroken grob-stencil or the first part of the broken 
   ;; grob-stencil. 
   (if (or (null? siblings) 
           (and (>= (length siblings) 2) 
                (eq? (car siblings) grob))) 
       (let* (;; Get the default-stencil and its x-dimension and x-length. 
              (stil (ly:piano-pedal-bracket::print grob)) 
              (stil-x-extent (ly:stencil-extent stil X)) 
              (stil-x-length (interval-length stil-x-extent)) 
              ;; Create a stencil to be added to the default-stencil. 
              ;; Gets its x-dimension and x-length. 
              (sim-stil 
                 (grob-interpret-markup grob 
                    (markup #:fontsize -1 #:hspace 0.75 #:raise -0.05 #:italic "sim."))) 
              (sim-stil-x-extent (ly:stencil-extent sim-stil X)) 
              (sim-stil-x-length (interval-length sim-stil-x-extent)) 
              (thickness (max (layout-line-thickness grob) 0.1)) 
              ;; The value to shorten the default-stencil from the right side. 
              ;; 0.8 will shorten about 80%. 
              ;; Hard-coded, could be turned into a music-function. 
              (amount 0.6)) 

         ;; Print a warning if the length of the default-stencil would not 
         ;; warrant a sufficient output. 
         (if (> sim-stil-x-length stil-x-length) 
             (ly:warning "PianoPedalBracket is too short")) 

         ;; Shorten the default-stencil. 
         (ly:grob-set-property! 
            grob 
            'shorten-pair 
            (cons 0 (* amount stil-x-length))) 

         ;; Calculate the final stencil. 
         (let* (;; Get the shortened (default-)stencil 
                ;; and its y-dimension. 
                (shortened-stil (ly:piano-pedal-bracket::print grob)) 
                (shortened-stil-y-ext (ly:stencil-extent shortened-stil Y)) 
                ;; Modify the sim-stil to gain a little gap to the left and 
                ;; enlarge it downwards a little (otherwise the stencil-whiteout 
                ;; will not work sufficient. 
                (new-sim-stil 
                  (ly:make-stencil 
                    (ly:stencil-expr sim-stil) 
                    (interval-widen sim-stil-x-extent (* 2 thickness)) 
                    (cons (- (car shortened-stil-y-ext) thickness) 
                          (cdr shortened-stil-y-ext))))) 

           (ly:stencil-add 
               shortened-stil 
               (ly:stencil-translate-axis 
                  (stencil-whiteout-box new-sim-stil) 
                  (* (- 1 amount) (- stil-x-length sim-stil-x-length)) 
                  X)))) 
       ;; TODO: 
       ;; Is there any need to return #f explicitly? 
       ;; Deleting it seems to make no difference. 
       ;#f 
       ))) 

pedaltesting = { 
    \set Staff.pedalSustainStyle = #'bracket 
    c''1\sustainOn \pedalSim c''\sustainOff\sustainOn c''1\sustainOff 
} 

\score { \pedaltesting } 
%%%  BOILERPLATE SNIPPET ENDS 