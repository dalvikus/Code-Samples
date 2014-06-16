@ECHO OFF

SET DEBUG=1
:----------------------------------------------:
:-               argument check               -:
:----------------------------------------------:
: %1 => scene file name
: %2 => option file name
: %3 => frame number
: %4 => rib name
: %5 => light linking option 
: %6 => cmd files 

SET DO_MTOR=1
IF "%~1" EQU "" (
	SET DO_MTOR=0
)
SET SCENE_FILE=%~1

IF "%~2" EQU "" GOTO USAGE
SET OPTION_FILE=%~2

IF "%~3" EQU "" GOTO USAGE
SET FRAME_NUMBER=%~3

IF "%~4" EQU "" (
	SET DO_LL=0
	SET DO_R2R=0
) ELSE (
	SET RIB_NAME=%~4

	IF "%~5" EQU "" (
		SET DO_LL=0
	) ELSE (
		SET DO_LL=1
		SET LL_OPTION=%~5
	)
	IF "%~6	" EQU "" (
		SET DO_R2R=0
	) ELSE 	(
		SET DO_R2R=1
		SET CMD_FILES=%~6
	)
	IF "%~7" NEQ "" GOTO USAGE

)

GOTO NEXT

:USAGE
ECHO Usage :
	ECHO.
	ECHO %0 SCENE_FILE OPTION_FILE FRAME_NUMBER RIB_NAME "LL_OPTION" "R2R_OPTION"
	ECHO.
	ECHO NOTE : All files must be full path
	ECHO.
	EXIT 2

:NEXT
:----------------------------------------------:
:-	  DEBUG MODE				  -:
:----------------------------------------------:
IF "%DEBUG%" EQU "1" (
	ECHO scene 	= %SCENE_FILE%
	ECHO optionfile = %OPTION_FILE%
	ECHO frameNO. 	= %FRAME_NUMBER%
	ECHO rib_name	= %RIB_NAME%
	ECHO ll option	= %LL_OPTION%
	ECHO cmd files	= %CMD_FILES%
	ECHO do_ll	= %DO_LL%
	ECHO do_r2r	= %DO_R2R%
)

:---------------------------------------------:
:-		SCENE FILE CHECK              -:
:----------------------------------------------:
IF NOT EXIST %SCENE_FILE% (
	ECHO Can't find scene file... %SCENE_FILE%
	EXIT 2
)

:----------------------------------------------:
:-	OPTION FILE CHECK		  -:
:----------------------------------------------:
IF NOT EXIST %OPTION_FILE% (
	ECHO Can't find option file... %OPTION_FILE%
	EXIT 2
)

:-------------------------------------------------:
:-                    GEN RIB                    -:
:-------------------------------------------------:		
IF "%DO_MTOR%" EQU "1" (
	mtor_maya4 -scene %SCENE_FILE% -cmd source %OPTION_FILE% -cmd genRib %FRAME_NUMBER%	
)

IF "%DO_LL%" EQU "1" (
	y:/bin/perl y:/usr/local/bin/berm %RIB_NAME% %LL_OPTION% > %RIB_NAME%_
	y:/bin/mv %RIB_NAME%_ %RIB_NAME%
)


IF "%DO_R2R%" EQU "1" (
	y:/bin/perl y:/usr/local/bin/r2r %RIB_NAME% %CMD_FILES% > %RIB_NAME%_ 
	y:/bin/mv %RIB_NAME%_ %RIB_NAME%
)

EXIT 0
