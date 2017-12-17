SetTitleMatchMode, RegEx

Log(fname, st) {
  TimeString = %A_YYYY%-%A_MM%-%A_DD% %A_Hour%:%A_Min%:%A_Sec%
  FileAppend, %TimeString% %st%`n, %fname%
}

FileOverWrite(fname, st) {
  TimeString = %A_YYYY%-%A_MM%-%A_DD% %A_Hour%:%A_Min%:%A_Sec%
  file := FileOpen(fname, "w")
  if IsObject(file) {
    file.Write(TimeString . " " . st . "`n")
    file.Close()
  }
}

Loop {
  Sleep, 500
   
  IfWinExist, Rendering to file\.\.\.
  {
    ControlGetText, Progress, Estimated Remaining, Rendering to file\.\.\.
    FileOverwrite("C:\ReaperBuf\progress.txt", Progress)
    Continue
  }

  IfWinExist, ODBC Data Source Configuration
  {
    WinGetTitle, Title
    ControlClick, OK, %Title%,,,, NA
    Log("C:\ReaperBuf\windows.log", Title)
  }

  IfWinExist, MIDI File Import
  {
    WinGetTitle, Title
    ControlClick, OK, %Title%,,,, NA
    Log("C:\ReaperBuf\windows.log", Title)
  }

  IfWinExist, Render Warning
  {
    WinGetTitle, Title
    ControlClick, Overwrite, %Title%,,,, NA
    Log("C:\ReaperBuf\windows.log", Title)
    Continue
  }
  
  IfWinExist, Finished in 0
  {
    WinGetTitle, Title
    ControlClick, Close, %Title%,,,, NA
    Log("C:\ReaperBuf\windows.log", Title)
    Continue
  }
  
  IfWinExist, Render to File
  {
    WinGetTitle, Title
    ControlClick, Button16, %Title%,,,, NA
    Log("C:\ReaperBuf\windows.log", Title)
  }

}
