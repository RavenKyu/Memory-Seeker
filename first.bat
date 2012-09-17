@echo off
setlocal

set main=main.obj
set mornitor=mornitor.obj
set hex=hex_viewer.obj

del *.obj

ml /c /coff mornitor.asm
cl /c hex_viewer.c
cl /c main.c

IF EXIST %hex% (
IF EXIST %main% (
IF EXIST %mornitor% (
cl %main% %mornitor% %hex%
echo %main%.exe :: 컴파일 완료.
) ELSE (
echo %mornitor% 파일이 존재하지 않습니다.
)
) ELSE (
echo %main% 파일이 존재하지 않습니다.
)
) ELSE (
echo %hex% 파일이 존재하지 않습니다.
)
