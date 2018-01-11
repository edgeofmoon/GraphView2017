@echo off
IF "%1"=="" GOTO :ERROR
.\Release\GraphView.exe 0 %1 0
exit

:ERROR
ECHO Need Participant Index!