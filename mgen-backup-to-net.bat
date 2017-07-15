@echo off
setlocal EnableDelayedExpansion
set Source=s:\app\MGen
set Target=n:\public\critic\app\MGen
robocopy /e /mir "%Source%" "%Target%" * > backup-to-net.log

