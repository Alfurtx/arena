@echo off

SETLOCAL

set CFLAGS=/W4 /std:c11 /Zi /nologo /Fearenatest.exe /wd5105 /wd4189

IF NOT EXIST "bin" mkdir bin

pushd "bin"
cl %CFLAGS% ../test.c
popd

ENDLOCAL
