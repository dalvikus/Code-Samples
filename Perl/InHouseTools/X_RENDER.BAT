@ECHO OFF

REM	\ NOT /
REM	NOT y:/bin/perl
SET UNIX_PERL=y:\bin\perl
SET PERL_RIBEDIT=y:\usr\local\bin\ribEdit

SET RIBFILE=%~1
IF "%RIBFILE%" EQU "" (
	ECHO Usage: %0 RIB [RIB_EDIT__ARGV...]
	EXIT 1
)
ECHO RIBFILE = "%RIBFILE%"
IF NOT EXIST "%RIBFILE%" (
	ECHO %RIBFILE% does not exist!
	EXIT 1
)

SET EDIT_ARGV=%~2
ECHO ARGV for RIBEDIT = "%EDIT_ARGV%"
IF "%EDIT_ARGV%" NEQ "" (
	%UNIX_PERL% %PERL_RIBEDIT% %RIBFILE% %EDIT_ARGV% | prman
) ELSE (
	prman -Progress %RIBFILE%
)
