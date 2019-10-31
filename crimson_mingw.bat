@ECHO OFF

set CARD_BIN=bin

IF /I "%PROCESSOR_ARCHITECTURE%" == "amd64" (
    set CARD_BIN=bin64
)
IF /I "%PROCESSOR_ARCHITEW6432%" == "amd64" (
    set CARD_BIN=bin64
)

start %CARD_BIN%\cardboard_mingw.exe "-q$HOME\My Games\Project Crimson Alpha" -glog.txt %*
