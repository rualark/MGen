Server_id = 1
Share = server\share\
ReaperBuf = C:\ReaperBuf\
daw_wait = 450 # How many second to wait after starting DAW to allow virtual instruments to load

DB_Driver = MySQL ODBC 5.3 Unicode Driver
DB_Server = 192.168.9.4
DB_Port = 3306
DB_Login = mgen
DB_Pass = Congress1
DB_Name = mgen

ChildProcess = Reaper.exe
ChildRestart = 1
ChildPath = C:\Windows\

ChildProcess = AutoHotkey.exe
ChildRestart = 1
ChildPath = C:\Program Files\AutoHotkey\
ChildParams = Reaper\reaper-mgen.ahk

ChildProcess = MGen.exe

ChildProcess = lilypond-windows.exe
ChildPath = X:\Program Files (x86)\LilyPond\usr\bin\

ChildProcess = RestartOnCrash.exe
ChildRestart = 1
ChildPath = server\roc\
