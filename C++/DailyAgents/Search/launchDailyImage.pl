#!/usr/bin/perl -w

use	strict;

if ( @ARGV != 6 ) {
	print STDERR <<USAGE;
Usage:
	$0 "UniqueFilename0" "Frame" bCloseWindow "FPS" "Viewer" "imageType"
USAGE
	exit 1;
}

my	$bVerbose = 0;

################################################################################
my	$gcProgPath = 'C:/Daily/Tools/';		# End with '/'!
my	$gcCygwinPath = 'C:/Daily/cygwin/';		# End with '/'!
my	$gcProgBinPath = $gcProgPath . 'bin/';		# End with '/'!
my	$gcProgTmpPath = $gcProgPath . 'tmp/';		# End with '/'!

my	$gcDailyImagePath = '//forest/DailyImages/Images/';		# End with '/'!

my	$gcshProg = $gcCygwinPath . 'bin/sh.exe';
my	$gcFrameCyclerProg = 'C:/Program Files/fc_pro_350sp2/bin/framecycler.exe';
my	$gciexploreProg = 'C:/Program Files/Internet Explorer/iexplore.exe';
my	$gcsleepProg = $gcProgBinPath . 'sleep.exe';
my	$gcgrToolsProg = $gcProgBinPath . 'grTools.exe';
my	$gcpskillProg = $gcProgBinPath . 'pskill.exe';
my	$gcfcheckProg = $gcProgBinPath . 'fcheck.exe';
my	$gcEND_OF_PROCESS_STRING = "\n" . '~!@345^&*90-+`12#$%678()_=' . "\n";
################################################################################


my	%ghVar = ();
if ( &checkArguments() == 0 ) {
	print STDERR 'Error: checkArguments(): Failure' . $/;
	exit 2;
}

if ( $ghVar{ 'bCloseWindow' } ) {
	my	@externalProg = ();

	if ( $ghVar{ 'imgType' } eq 'GIF' ) {
		push( @externalProg, 'grTools', 'iexplore' );
	} elsif ( $ghVar{ 'Viewer' } eq 'FrameCycler' ) {
		push( @externalProg, 'FrameCycler' );
	} else {	# fcheck
		push( @externalProg, 'grTools', 'fcheck' );
	}

	&killProcess( @externalProg );
}

# Launch Viewer.
if ( &launchViewer() == 0 ) {
	print STDERR 'Error: launchViewer(): Failure' . $/;
	exit 3;
}

exit 0;


#	$0 "UniqueFileName0" "Frame" bCloseWindow FPS Viewer imageType
sub checkArguments
{
	print STDOUT
		'Trace: Check Arguments...' . $/ .
		'=' x 80 . $/ if $bVerbose;
	my	$returnVal = 1;		# 1: Success, 0: Failure

	# $ARGV[ 0 ]: UniqueFilename0
	print STDOUT 'Info: $ARGV[ 0 ]: UniqueFilename0:= |' . $ARGV[ 0 ] . '|' . $/ if $bVerbose;
	unless ( $ARGV[ 0 ] ) {
		print STDERR
			'Error: Argument( "UniqueFilename0" ):= |' . ( defined( $ARGV[ 0 ] ) ? $ARGV[ 0 ] : 'undef' ) . '|' . $/ .
			"\t" . '의미있는 문자열이어야 한다' . $/;
		$returnVal = 0;
	}
	$ghVar{ 'UniqueFilename0' } = $ARGV[ 0 ];

	# $ARGV[ 1 ]: Frame
	#	\s*\d+[\s*-\s*\d+[\s*:\s*\d+\s*]\s*]\s*
	print STDOUT 'Info: $ARGV[ 1 ]: Frame:= |' . $ARGV[ 1 ] . '|' . $/ if $bVerbose;
	$ghVar{ 'Frame' } = [];
	for ( split( /\s*,\s*/, $ARGV[ 1 ] ) ) {
		my	( $from, $toInfo, $to, $byInfo, $by ) = /^\s*([1-9]\d*)(\s*-\s*([1-9]\d*)(\s*:\s*(-?[1-9]\d*)\s*)?\s*)?\s*$/;
		unless ( $from ) {
			print STDERR
				'Error: Argument( "Frame" ):= |' . $ARGV[ 1 ] . '|: <' . $_ . '>' . $/ .
				"\t" . '"숫자[-숫자[:숫자]]"의 형식이어야 한다' . $/;
			$returnVal = 0;

			next;
		}
		my	( $FROM, $TO, $BY );	# True FROM, TO, BY
		if ( $toInfo ) {
			if ( $from > $to ) {	# Reverse
				$FROM = $to;
				$TO = $from;
			} else {
				$FROM = $from;
				$TO = $to;
			}

			if ( $byInfo ) {
				$BY = $by > 0 ? $by : -$by;
				print STDOUT "\t" . '<' . $_ . '>: ' . $FROM . ' to ' . $TO . ' by ' . $BY . $/ if $bVerbose;
			} else {
				$BY = 1;
				print STDOUT "\t" . '<' . $_ . '>: ' . $FROM . ' to ' . $TO . $/ if $bVerbose;
			}
		} else {
			$FROM = $TO = $from;
			$BY = 1;

			print STDOUT "\t" . '<' . $_ . '>: ' . $from . $/ if $bVerbose;
		}

		my	$aref = [ 0 + $FROM, 0 + $TO, 0 + $BY ];	# Make a number
		push( @{ $ghVar{ 'Frame' } }, $aref );
	}

	# $ARGV[ 2 ]: bCloseWindow
	print STDOUT 'Info: $ARGV[ 2 ]: bCloseWindow:= |' . $ARGV[ 2 ] . '|' . $/ if $bVerbose;
	$ghVar{ 'bCloseWindow' } = $ARGV[ 2 ] ? 1 : 0;

	# $ARGV[ 3 ]: FPS
	print STDOUT 'Info: $ARGV[ 3 ]: FPS:= |' . $ARGV[ 3 ] . '|' . $/ if $bVerbose;
	my	( $FPS ) = $ARGV[ 3 ] =~ /^\s*(\d+)\s*$/;
	unless ( $FPS ) {
		print STDERR
			'Warning: Argument( "FPS" ):= |' . $ARGV[ 3 ] . '|' . $/ .
			"\t" . '숫자이어야 한다: 기본값인 24을 사용한다' . $/;
		$FPS = 24;
	}
	$ghVar{ 'FPS' } = $FPS;

	# $ARGV[ 4 ]: Viewer
	#	FrameCycler, fcheck
	if ( $ARGV[ 4 ] ne 'FrameCycler' && $ARGV[ 4 ] ne 'fcheck' ) {
		print STDERR
			'Error: Argument( "Viewer" ):= |' . $ARGV[ 4 ] . '|' . $/ .
			"\t" . '"FrameCycler" 또는 "fcheck"이어야 한다' . $/;
		$returnVal = 0;
	}
	$ghVar{ 'Viewer' } = $ARGV[ 4 ];

	# $ARGV[ 5 ]: imageType
	# 	TIFF, GIF
	if ( $ARGV[ 5 ] ne 'TIFF' && $ARGV[ 5 ] ne 'GIF' ) {
		print STDERR
			'Error: Argument( "imgageType" ):= |' . $ARGV[ 5 ] . '|' . $/ .
			"\t" . '"TIFF" 또는 "GIF"이어야 한다' . $/;
		$returnVal = 0;
	}
	if ( $ARGV[ 5 ] eq 'GIF' ) {
		print STDERR 'Warning: imageType:= |GIF|: Viewer is ignored' . $/;
	}
	$ghVar{ 'imgType' } = $ARGV[ 5 ];

	print STDOUT '=' x 80 . $/ if $bVerbose;
	return $returnVal;
}

sub killProcess
{
	foreach ( @_ ) {
		my	@pskill = (
			$gcpskillProg,
			$_
		);
		print STDOUT '|' . join( '| |', @pskill ) . '|' . $/ if $bVerbose;

		my	@sh = (
			$gcshProg,
			'-c',
			'\'' . join( '\' \'', @pskill ) . '\' > /dev/null 2>&1'
		);
		print STDOUT '|' . join( '| |', @sh ) . '|' . $/ if $bVerbose;
		if ( &RunProgram( 'sh -c "pskill ..."', \@sh ) != 0 ) {
#			print STDERR 'Error: sh -c "pskill ...": $? != 0: Ignored' . $/;
#			return 0;
		}
	}
}

sub launchViewer
{
if ( $ghVar{ 'imgType' } eq 'GIF' ) {
	unless ( -f $gciexploreProg ) {
		print STDERR 'Error: Not a plain file: |' . $gciexploreProg . '|' . $/;
		return 0;
	}

	my	@grTools = (
		$gcgrToolsProg,
		$gcDailyImagePath . $ghVar{ 'UniqueFilename0' } . '.0.0000.tif'
	);
	print STDOUT '|' . join( '| |', @grTools ) . '|' . $/ if $bVerbose;
	my	@sh = (
		$gcshProg,
		'-c',
		'\'' . join( '\' \'', @grTools ) . '\' &'		# &: run as background...
	);
	print STDOUT '|' . join( '| |', @sh ) . '|' . $/ if $bVerbose;
	if ( &RunProgram( 'sh -c "grTools ... &"', \@sh ) != 0 ) {
		print STDERR 'Error: sh -c "grTools ... &": $? != 0: Ignored' . $/;
		return 0;
	}

	for ( my $iQueue = 0; $iQueue < @{ $ghVar{ 'Frame' } }; ++$iQueue ) {
		my	( $from, $to, $by ) = @{ $ghVar{ 'Frame' } -> [ $iQueue ] };
		my	$NFrame = $by == 1 ? ( 1 + $to - $from ) : ( 1 + int( ( $to - $from ) / $by ) );
		for ( my $iFrame = 1; $iFrame <= $NFrame; ++$iFrame ) {
			my	$dailyImageWinPath = $gcDailyImagePath;
			$dailyImageWinPath =~ s| |%20|g;
			my	@iexplore = (
				$gciexploreProg,

				'file://' . $dailyImageWinPath . $ghVar{ 'UniqueFilename0' } . '.' . ( 1 + $iQueue ) . '.' . sprintf( '%04d', $iFrame ) . '.gif'
			);
			print STDOUT '|' . join( '| |', @iexplore ) . '|' . $/ if $bVerbose;
			my	@sh = (
				$gcshProg,
				'-c',
				'\'' . join( '\' \'', @iexplore ) . '\' &'		# &: run as background...
			);
			print STDOUT '|' . join( '| |', @sh ) . '|' . $/ if $bVerbose;
			if ( &RunProgram( 'sh -c "iexplore ... &"', \@sh ) != 0 ) {
				print STDERR 'Error: sh -c "iexplore ... &": $? != 0: Ignored' . $/;
				return 0;
			}
		}
	}
} elsif ( $ghVar{ 'Viewer' } eq 'FrameCycler' ) {
	unless ( -f $gcFrameCyclerProg ) {
		print STDERR 'Error: Not a plain file: |' . $gcFrameCyclerProg . '|' . $/;
		return 0;
	}

	my	@sequence = ();
	for ( my $iQueue = 0; $iQueue < @{ $ghVar{ 'Frame' } }; ++$iQueue ) {
		my	( $from, $to, $by ) = @{ $ghVar{ 'Frame' } -> [ $iQueue ] };
		push( @sequence, 'Q[', $gcDailyImagePath . $ghVar{ 'UniqueFilename0' } . '.' . (1 + $iQueue ) . '.#.tif', '1-' . ( $by == 1 ? ( 1 + $to - $from ) : ( 1 + int( ( $to - $from ) / $by ) ) ), ']Q' );
	}

	my	@framecycler = (
		$gcFrameCyclerProg,

		# Slate image
		'Q[',
		$gcDailyImagePath . $ghVar{ 'UniqueFilename0' } . '.0.0000.tif',
		']Q',

		# Image sequences...
		@sequence,

		# FPS
		'-f',
		$ghVar{ 'FPS' }
	);
	print STDOUT '|' . join( '| |', @framecycler ) . '|' . $/ if $bVerbose;
	my	@sh = (
		$gcshProg,
		'-c',
		'\'' . join( '\' \'', @framecycler ) . '\' &'		# &: run as background...
	);
	print STDOUT '|' . join( '| |', @sh ) . '|' . $/ if $bVerbose;
	if ( &RunProgram( 'sh -c "framecycler ... &"', \@sh ) != 0 ) {
		print STDERR 'Error: sh -c "framecycler ... &": $? != 0: Ignored' . $/;
		return 0;
	}
} else {	# fcheck
	my	@grTools = (
		$gcgrToolsProg,
		$gcDailyImagePath . $ghVar{ 'UniqueFilename0' } . '.0.0000.tif'
	);
	print STDOUT '|' . join( '| |', @grTools ) . '|' . $/ if $bVerbose;
	my	@sh = (
		$gcshProg,
		'-c',
		'\'' . join( '\' \'', @grTools ) . '\' &'		# &: run as background...
	);
	print STDOUT '|' . join( '| |', @sh ) . '|' . $/ if $bVerbose;
	if ( &RunProgram( 'sh -c "grTools ... &"', \@sh ) != 0 ) {
		print STDERR 'Error: sh -c "grTools ... &": $? != 0: Ignored' . $/;
		return 0;
	}

	for ( my $iQueue = 0; $iQueue < @{ $ghVar{ 'Frame' } }; ++$iQueue ) {
		my	( $from, $to, $by ) = @{ $ghVar{ 'Frame' } -> [ $iQueue ] };

		my	@fcheck = (
			$gcfcheckProg,

			'-r', $ghVar{ 'FPS' },

			'-n',		# aNimate...
			'1',		# From
			$by == 1 ? ( 1 + $to - $from ) : ( 1 + int( ( $to - $from ) / $by ) ),
			'1',		# By

			$gcDailyImagePath . $ghVar{ 'UniqueFilename0' } . '.' . (1 + $iQueue ) . '.#.tif'
		);
		print STDOUT '|' . join( '| |', @fcheck ) . '|' . $/ if $bVerbose;

		my	@sh = (
			$gcshProg,
			'-c',
			'\'' . join( '\' \'', @fcheck ) . '\' > /dev/null 2>&1 &'	# &: run as background...
								# Since fcheck needs STDOUT/STDERR,
								# redirect them to /dev/null!
		);
		print STDOUT '|' . join( '| |', @sh ) . '|' . $/ if $bVerbose;
		if ( &RunProgram( 'sh -c "fcheck ... > /dev/null 2>&1 &"', \@sh ) != 0 ) {
			print STDERR 'Error: sh -c "fcheck ... > /dev/null 2>&1 &": $? != 0: Ignored' . $/;
			return 0;
		}
	}
}

	print STDERR $gcEND_OF_PROCESS_STRING;

	return 1;
}

sub RunProgram
{
	my	( $ProgName, $aSystemRef ) = @_;

	system @{ $aSystemRef };
	if ($? == -1) {
		print STDERR $ProgName . ': Failed to execute: $!' . $/;
		return -1;
	} elsif ($? & 127) {
		printf STDERR $ProgName . ': Child died with signal %d %s coredump' . $/,
			($? & 127), ($? & 128) ? 'with' : 'without';
		return -2;
	} else {
		return $? >> 8;
	}
}
