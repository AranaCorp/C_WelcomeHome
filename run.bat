@echo off
REM ======================================================
REM  Compile and run a C file with GCC
REM  Usage: run file.c
REM ======================================================

if "%1"=="" (
    echo Usage: %0 file.c
    exit /b 1
)

set FILE=%~n1
set EXT=%~x1

REM Check for .c extension
if /I NOT "%EXT%"==".c" (
    echo Error: Input must be a .c file
    exit /b 1
)

echo Compiling %1 ...
REM gcc "%1" -o "%FILE%.exe"
gcc "%1" src\*.c -I include -o %FILE%.exe

if %ERRORLEVEL% NEQ 0 (
    echo Compilation failed!
    exit /b 1
)

echo Running %FILE%.exe ...
echo ======================================================
"%FILE%.exe"
