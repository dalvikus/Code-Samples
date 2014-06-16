@ECHO OFF
:----- argument check -----

IF "%~1" EQU "" GOTO USAGE
SET RIB=%~1
SET BASE_RIB=%~n1
IF "%~2" EQU "" GOTO USAGE
SET LIGHT=%~2
IF "%~3" EQU "" GOTO USAGE
SET SRC=%~3
IF "%~4" EQU "" GOTO USAGE
SET DEST=%~4
IF "%~5" EQU "" GOTO USAGE
IF "%~6" EQU "" GOTO USAGE
IF "%~7" EQU "" GOTO NEXT

:USAGE
	ECHO Usage :
	ECHO.
	ECHO %0 RIB LIGHT SRC DEST IMAGE_DIR UNIQUE_ID
	ECHO.
	ECHO IMAGE_DIR: last '/' required!
	ECHO.
	GOTO END
	
:NEXT
	SET DEBUG=1
	SET UNIX_PERL=y:/bin/perl
	SET prog__RENDER=prman
	SET prog__SPLIT_RIB=y:/usr/local/bin/splitRib-1.04
	SET prog__R2R=y:/usr/local/bin/r2r
	SET prog__RIB_EDIT=y:/usr/local/bin/ribEdit

	SET CMD=%5%BASE_RIB%.%6.dscmd
	SET SHD=%5%BASE_RIB%.%6.shd.tex
	SET SHADOW=Display "%SHD%" "shadow" "z"

	IF "%~5" NEQ "-" (
		SET IMAGE=Display "%5%6" "tiff" "rgba"
	) ELSE (
		SET IMAGE=Display "%5%6" "it" "rgba"
	)
	
	IF %DEBUG% EQU 1 (
		ECHO RIB = %RIB%
		ECHO BASE_RIB = %BASE_RIB%
		ECHO LIGHT = %LIGHT%
		ECHO SRC = %SRC%
		ECHO DEST = %DEST%
		ECHO CMD = %CMD%
		ECHO SHD = %SHD%
		ECHO SHADOW = %SHADOW%
		ECHO IMAGE = %IMAGE%
	)
	
	%UNIX_PERL% %prog__SPLIT_RIB% %RIB% '%LIGHT%:-:%SHADOW%' > %5%6.x
	IF EXIST %5%6.x (
		%UNIX_PERL% %prog__RIB_EDIT% %5%6.x -e \* -r %SRC% > %5%6.y
		prman %5%6.y
	) ELSE (
		ECHO cannot find %5%6.x...
	)
	IF EXIST %CMD% (y:/bin/rm %CMD%)

	ECHO SHADER_PATH = "//chunji/share/shader/ds/standardShaders/" >> %CMD%
	ECHO.
	ECHO ======================================================================== >> %CMD%
	ECHO ObjectName	"%DEST%"	"Surface" >> %CMD%
	ECHO ------------------------------------------------------------------------ >> %CMD%
	ECHO Surface "shadowMaterial" "string shadowmap" [ "%SHD%" ] >> %CMD%
	ECHO ======================================================================== >> %CMD%

	%UNIX_PERL% %prog__SPLIT_RIB% %RIB% '-:-:%IMAGE%' > %5%6.x
	IF EXIST %5%6.x (
		%UNIX_PERL% %prog__RIB_EDIT% %5%6.x -e \* -r %DEST%  > %5%6.y
		%UNIX_PERL% %prog__R2R% %5%6.y %CMD%  > %5%6.x
		prman %5%6.x
	) ELSE (
		cannot find %5%6.x...
	)
	
	y:/bin/rm %5%6.x
	y:/bin/rm %5%6.y
	y:/bin/rm %CMD%
	y:/bin/rm %SHD%

	GOTO END
	
:END

	


