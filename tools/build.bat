@echo off
IF [%1]==[setprjname] SET project=%2 && GOTO:EOF

SETLOCAL
SET libs=
SET argsemu=/Od /Zi /Fe%project% /nologo %libs%
SET argscom=/Od /Zi /Feabccom /nologo %libs%
SET abcemu=%source%\main.c %source%\win32_conio.c
SET abccompiler=..\..\code\abccompiler.c

SET edit=edit

IF [%1]==[%edit%] GOTO EditBuildFile
IF [%1]==[] GOTO Build
IF NOT [%1]==[] GOTO Error
GOTO:EOF

:Build
ECHO: Build started...
ECHO:

ECHO: Compiling emulator...
IF NOT EXIST "%bin%" MKDIR "%bin%"
PUSHD "%bin%"
CL %argsemu% %abcemu%
POPD

ECHO:
ECHO: Compiling compiler...
IF NOT EXIST "%bin%\compiler" MKDIR "%bin%\compiler"
PUSHD "%bin%\compiler"
CL %argscom% %abccompiler%
POPD

ECHO:
ECHO: Build finished.
GOTO:EOF

:EditBuildFile
%editor% "%tools%\%0.bat"
GOTO:EOF

:Error
ECHO: ERROR: wrong arguments passed!
GOTO:EOF
