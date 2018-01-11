@echo off
IF "%1"=="" GOTO :ERROR
.\Release\GraphView.exe 0 %1 1
exit

:ERROR
ECHO Need Participant Index!