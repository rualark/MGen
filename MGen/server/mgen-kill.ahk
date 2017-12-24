SetTitleMatchMode, RegEx

Loop, 5 {
  IfWinExist, Rendering to file\.\.\.
  {
    WinGetTitle, Title
    ControlClick, Cancel, %Title%,,,, NA
  }

  IfWinExist, MIDI File Import
  {
    WinGetTitle, Title
    ControlClick, Cancel, %Title%,,,, NA
  }

  IfWinExist, Render Warning
  {
    WinGetTitle, Title
    ControlClick, Cancel, %Title%,,,, NA
  }
    
  IfWinExist, Render to File
  {
    WinGetTitle, Title
    ControlClick, Cancel, %Title%,,,, NA
  }
  Sleep, 100
}