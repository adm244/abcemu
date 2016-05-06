@echo off
IF [%1]==[setprjname] SET project=%2 && GOTO:EOF

SETLOCAL
SET libs=
SET args=/Od /Zi /Fe%project% /nologo %libs%
SET files=%source%\*.c*

SET edit=edit

IF [%1]==[%edit%] GOTO EditBuildFile
IF [%1]==[] GOTO Build
IF NOT [%1]==[] GOTO Error
GOTO:EOF

:Build
ECHO: Build started...

IF NOT EXIST "%bin%" MKDIR "%bin%"
PUSHD "%bin%"
CL %args% "%files%"
POPD

ECHO: Build finished.
GOTO:EOF

:EditBuildFile
%editor% "%tools%\%0.bat"
GOTO:EOF

:Error
ECHO: ERROR: wrong arguments passed!
GOTO:EOF
