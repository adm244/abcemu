@ECHO OFF
IF [%1]==[compiler] GOTO RunCompiler

%bin%\%project%.exe %*
GOTO:EOF

:RunCompiler
%bin%\compiler\abccom.exe %*
GOTO:EOF
