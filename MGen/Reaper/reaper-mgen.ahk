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

ProcessExists(Name){
	Process,Exist,%Name%
	return Errorlevel
}

RestartServer() {
  Process, Close, MGenServer.exe
  Process, WaitClose, MGenServer.exe, 20
  if ErrorLevel ; The PID still exists.
    MsgBox The process MGenServer did not close within 20 seconds.
  Run, MGenServer.exe
}

Loop {
  Sleep, 500
   
  If !ProcessExists("MGenServer.exe")
  {
    Sleep, 5000
    If !ProcessExists("MGenServer.exe")
    {
      RestartServer()
    }
  }
  
  IfExist, server\status.txt
  {
    FileGetTime, myTime, server\status.txt
    var1 = %A_Now%
    EnvSub, var1, %myTime%, Seconds
    If var1 > 30
    {
      Sleep, 5000
      FileGetTime, myTime, server\status.txt
      var1 = %A_Now%
      EnvSub, var1, %myTime%, Seconds
      If var1 > 30
      {
        RestartServer()
      }
    }
  }
  else {
    Sleep, 5000
    IfNotExist, server\status.txt
    {
      RestartServer()
    }
  }
  
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
