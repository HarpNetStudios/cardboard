@ECHO OFF

set CARD_BIN=bin

IF /I "%PROCESSOR_ARCHITECTURE%" == "x86" (
    set CARD_BIN=bin32
)
IF /I "%PROCESSOR_ARCHITEW6432%" == "x86" (
    set CARD_BIN=bin32
)

start %CARD_BIN%\cardboard_mingw.exe "-q$HOME\My Games\Carmine Impact Alpha" -gserver-log.txt -d %*
