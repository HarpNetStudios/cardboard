@ECHO OFF

set SAUER_BIN=bin

IF /I "%PROCESSOR_ARCHITECTURE%" == "amd64" (
    set SAUER_BIN=bin64
)
IF /I "%PROCESSOR_ARCHITEW6432%" == "amd64" (
    set SAUER_BIN=bin64
)

start %SAUER_BIN%\cardboard_msvc.exe "-q$HOME\My Games\Project Crimson Alpha" -gserver-log.txt -d %*
