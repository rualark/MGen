SetTitleMatchMode, RegEx

Loop {
  WinWait, MIDI File Import|Render to File|Finished in 0|Render Warning|Rendering to file\.\.\.

  WinGetTitle, Title
  TimeString = %A_YYYY%-%A_MM%-%A_DD% %A_Hour%:%A_Min%:%A_Sec%
   
  IfWinExist, Rendering to file\.\.\.
  {
    ControlGetText, Progress, Estimated Remaining, Rendering to file\.\.\.
    ;MsgBox, Progress Is: %Progress%
    file := FileOpen("C:\ReaperBuf\progress.txt", "w")
    if IsObject(file) {
      file.Write(TimeString . " " . Progress . "`n")
      file.Close()
    }
    ;FileAppend, %TimeString% %Progress%`n, C:\ReaperBuf\progress.txt
    sleep, 500
  }
  Else
  {
    WinActivate
    sleep, 50
    Send , {Enter}
    WinWaitClose
    FileAppend, %TimeString% %Title%`n, C:\ReaperBuf\windows.log
  }
   
  ;WinMinimize, TeamViewer
  ;WinActivate, XBMC
}
