@echo off
setlocal EnableDelayedExpansion
set Source=s:\app\MGen
set Target=n:\public\critic\app\MGen
rem robocopy /e /mir /sl "%Source%" "%Target%" * /XF "%Source%\MGen\server" /COPY:DATS
rem > backup-to-net.log

del "%Target%.rar"
"c:\program files\winrar\winrar" a -r -ma5 -ep1 "%Target%.rar" "%Source%" -x"%Source%\MGen\server" -x"%Source%\MGen\share"


