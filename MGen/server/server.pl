Server_id = 1
Share = server\share\

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

# Maximum time to send to process and timeout process in seconds
Job = CA2;60;80 
Job = CA2-cor;600;640 
Job = Render;600;600
Job = LY;600;600
