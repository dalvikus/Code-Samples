@ECHO OFF

SETLOCAL
:LOOP
SET	args=%~1
IF "%~1" EQU "" GOTO END
SET /A ix=%ix% + 1
ECHO argv[%ix%] = '%args%'
SHIFT
GOTO LOOP
GOTO END
DPSMediaUtility %*

:END
