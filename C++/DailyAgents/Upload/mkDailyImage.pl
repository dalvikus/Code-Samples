#!/usr/bin/perl -w

use	strict;

if ( @ARGV < 9 || @ARGV != ( $ARGV[ 0 ] && uc( $ARGV[ 0 ] ) eq 'TD' ? 14 : 20 ) ) {
	print STDERR <<USAGE;
Usage:
	$0 "GroupName" "UserName" "ShowName" "SeqName" "ShotName" "PathName" "Frame" "Revision" "Note"
		"Anim" "Texture" "PixelSamples" "ShadingRate"					# Td specific info.
		"Anim" "Mdl" "Texture" "Fx" "Td" "Comp" "Setup" "Concept" "FocalLength" "FPS"	# Non-Td specific info.
		"Password"
USAGE
	exit 1;
}

my	$bVerbose = 0;

################################################################################
my	$gcProgPath = 'C:/Daily/Tools/';		# End with '/'!
my	$gcCygwinPath = 'C:/Daily/cygwin/';		# End with '/'!
my	$gcProgBinPath = $gcProgPath . 'bin/';		# End with '/'!
my	$gcProgTmpPath = $gcProgPath . 'tmp/';		# End with '/'!

my	$gcBloomusLogoImage = $gcProgBinPath . 'bloomusLogo.tif';
my	$gcMissingFrameImage = $gcProgBinPath . 'missingFrame.tif';

my	$gcDailyImagePath = '//forest/DailyImages/Images/';		# End with '/'!

my	$gcMySQLProg = $gcProgBinPath . 'mysql.exe';
my	$gcimgcvtProg = $gcProgBinPath . 'imgcvt.EXE';
my	$gcgrToolsProg = $gcProgBinPath . 'grTools.exe';
my	$gcXCACLSProg = $gcProgBinPath . 'XCACLS.vbs';

my	$gcHost = 'flowergarden';
my	@gcMySQL0 = (
	$gcMySQLProg,				# MySQL Client

#	'--user=******',			# User
	'--host=' . $gcHost,			# Host
#	'--password=******',			# Password

	# Options
	'--no-beep',
	'--show-warnings',
	'--skip-column-names',

	# Database
#	'--database=DBBloomUs',
);
################################################################################


my	%ghVar = ();
if ( &checkArguments() == 0 ) {
	print STDERR 'Error: checkArguments(): Failure' . $/;
	exit 2;
}

if ( &getUUID() == 0 ) {
	print STDERR 'Error: getUUID(): Failure' . $/;
	exit 3;
}

if ( &makeDailyImages() == 0 ) {
	print STDERR 'Error: makeDailyImages(): Failure' . $/;
	exit 4;
}

if ( &updateDBTable() == 0) {
	print STDERR 'Error: updateDBTable(): Failure' . $/;
	exit 5;
}
exit 0;


=head1
USE DBBloomUs;

CREATE TABLE IF NOT EXISTS TDaily
(
	DailyID		INT UNSIGNED NOT NULL AUTO_INCREMENT UNIQUE, 

	DateTime	DATETIME NOT NULL,

	GroupName	CHAR(8) NOT NULL,	-- GroupID
	UserName	CHAR(16) NOT NULL,	-- UserID

	ShowName	CHAR(16) NOT NULL,	-- ShowID	[A-Za-z]{2,16}
--	SeqName		CHAR(8) NOT NULL,	-- SeqID
	ShotName	CHAR(8) NOT NULL,	-- ShotID	[A-Za-z]{2,}[1-9]\d?

												/* linux/limits.h */
--	PathName	VARCHAR(4096) NOT NULL,	-- (PathID)	NFS Path...			PATH_MAX
--	GivenFileName	CHAR(255) NOT NULL,	--		Without \d{4}.FileType		NAME_MAX
	Revision	CHAR(3) NOT NULL,	--		r[1-9]\d?
	Frame		CHAR(32) NOT NULL,	--		1[-10[:2]][,...]
--	FileType	CHAR(4) NOT NULL,	-- (TypeID)	tiff, iff, ...,
	UniqueFileName0	CHAR(255) NOT NULL,	-- (FileID)	Date.ShowName.ShotName.GroupName.UserName.Revision.UUID
						--						NAME_MAX

	Comment		CHAR(255)
);
=cut
#	$0 "GrpName" "UserName" "ShowName" "SeqName" "ShotName" "PathName" "Frame" "Revision" "Note"
sub checkArguments
{
	print STDOUT
		'Trace: Check Arguments...' . $/ .
		'=' x 80 . $/ if $bVerbose;
	my	$returnVal = 1;		# 1: Success, 0: Failure


	# 1. Identity
	# $ARGV[ 0 ]: GroupName
	#	[A-Za-z]{2,8}
	print STDOUT 'Info: $ARGV[ 0 ]: GroupName:= |' . $ARGV[ 0 ] . '|' . $/ if $bVerbose;
	unless ( $ARGV[ 0 ] =~ /^[A-Za-z]{2,8}$/ ) {
		print STDERR
			'Error: Argument( "Dept." ):= |' . $ARGV[ 0 ] . '|' . $/ .
			"\t" . '적어도 두 개, 많아야 여덟 개의 알파벳문자로 이루어져야 한다' . $/;
		$returnVal = 0;
	}
	$ghVar{ 'GroupName' } = $ARGV[ 0 ];
	# $ARGV[ 1 ]: UserName
	#	[A-Za-z]{2,16}
	print STDOUT 'Info: $ARGV[ 1 ]: UserName:= |' . $ARGV[ 1 ] . '|' . $/ if $bVerbose;
	unless ( $ARGV[ 1 ] =~ /^[A-Za-z]{2,16}$/ ) {
		print STDERR
			'Error: Argument( "User" ):= |' . $ARGV[ 1 ] . '|' . $/ .
			"\t" . '적어도 두 개, 많아야 열여섯 개의 알파벳문자로 이루어져야 한다' . $/;
		$returnVal = 0;
	}
	$ghVar{ 'UserName' } = $ARGV[ 1 ];

	# 2. Project
	# $ARGV[ 2 ]: ShowName
	#	[A-Za-z]{2,16}
	print STDOUT 'Info: $ARGV[ 2 ]: ShowName:= |' . $ARGV[ 2 ] . '|' . $/ if $bVerbose;
	unless ( $ARGV[ 2 ] =~ /^[A-Za-z0-9]{2,16}$/ ) {
		print STDERR
			'Error: Argument( "Show" ):= |' . $ARGV[ 2 ] . '|' . $/ .
			"\t" . '적어도 두 개, 많아야 열 여섯 개의 알파벳 문자 또는 숫자의 조합으로 이루어져야 한다' . $/;
		$returnVal = 0;
	}
	$ghVar{ 'ShowName' } = $ARGV[ 2 ];
	# $ARGV[ 3 ]: SeqName
	if ( $ARGV[ 3 ] ) {
		unless ( $ARGV[ 3 ] =~ /^[A-Za-z0-9]+(\.[A-Za-z0-9]+)?$/ ) {
			print STDERR
				'Error: Argument( "Seq" ):= |' . $ARGV[ 3 ] . '|' . $/ .
				"\t" . '적어도 두 개, 많아야 여덟 개의 알파벳 문자 또는 숫자의 조합이, 많아야 두 번 점(.)으로 연결될 수 있다' . $/;
			$returnVal = 0;
		}
		$ghVar{ 'SeqName' } = $ARGV[ 3 ];
	} else {
		$ghVar{ 'SeqName' } = '';
	}
	# $ARGV[ 4 ]: ShotName
	#	[A-Za-z]\d{1,2}
	print STDOUT 'Info: $ARGV[ 4 ]: ShotName:= |' . $ARGV[ 4 ] . '|' . $/ if $bVerbose;
	unless ( $ARGV[ 4 ] =~ /^[A-Za-z]{1,2}\d{1,3}$/ && length( $ARGV[ 4 ] ) <= 8 ) {
		print STDERR
			'Error: Argument( "Shot" ):= |' . $ARGV[ 4 ] . '|' . $/ .
			"\t" . '최소 하나, 최대 두 개의 알파벳문자로 시작해야하고,' . $/ .
			"\t" . '최소 하나, 최대 세 개의 숫자로 끝나야 한다.' . $/ .
			"\t" . '전체 문자의 갯수는 여덟 개가 넘으면 안된다' . $/;
		$returnVal = 0;
	}
	$ghVar{ 'ShotName' } = $ARGV[ 4 ];

	# 3. Image
	# $ARGV[ 5 ]: PathName
	#	.+?[^\/]+?\.(\d{4})\.(tif|iff)
	print STDOUT 'Info: $ARGV[ 5 ]: Image:= |' . $ARGV[ 5 ] . '|' . $/ if $bVerbose;
	my	( $imgPath, $imgBareFilename, $dummyFrame, $imgType ) = $ARGV[ 5 ] =~ /^(.+?)([^\/]+?)\.(\d{4})\.(tif|iff|gif|bmp|tga)$/i;
	unless ( $imgPath ) {
		print STDERR
			'Error: Argument( "Pathname" ):= |' . $ARGV[ 5 ] . '|' . $/ .
			"\t" . '/로 시작하는 절대경로이어야 하고,' . $/ . 
			"\t" . '네 자리의 프레임 수와 이미지 형식(tif/iff/gif)을 담고 있어야 한다' . $/;
		$returnVal = 0;
	} else {
		$ghVar{ 'Image' } = {
			'imgPath'		=> $imgPath,
			'imgBareFilename'	=> $imgBareFilename,
			'imgType'		=> uc( $imgType ),
		};
		print STDOUT
			"\t" . 'Path:= |' . $imgPath . '|' . $/ .
			"\t" . 'BareFilename:= |' . $imgBareFilename . '|' . $/ .
			"\t" . 'Type:= |' . uc( $imgType ) . '|' . $/ if $bVerbose;
	}
	# $ARGV[ 6 ]: Frame
	#	\s*\d+[\s*-\s*\d+[\s*:\s*\d+\s*]\s*]\s*
	print STDOUT 'Info: $ARGV[ 6 ]: Frame:= |' . $ARGV[ 6 ] . '|' . $/ if $bVerbose;
	$ghVar{ 'Frame' } = [];
	for ( split( /\s*,\s*/, $ARGV[ 6 ] ) ) {
		my	( $from, $toInfo, $to, $byInfo, $by ) = /^\s*([1-9]\d*)(\s*-\s*([1-9]\d*)(\s*:\s*(-?[1-9]\d*)\s*)?\s*)?\s*$/;
		unless ( $from ) {
			print STDERR
				'Error: Argument( "Frame" ):= |' . $ARGV[ 6 ] . '|: <' . $_ . '>' . $/ .
				"\t" . '"숫자[-숫자[:숫자]]"의 형식이어야 한다' . $/;
			$returnVal = 0;
		} else {
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
			if ( checkFrame( $aref ) ) {
				print STDERR
					'Warning: Argument( "Frame" ):= |' . $ARGV[ 6 ] . '|: <' . $_ . '>' . $/ .
					"\t" . '다른 프레임과 겹치는 부분이 있다' . $/;
			}
			push( @{ $ghVar{ 'Frame' } }, $aref );
		}
	}
	# $ARGV[ 7 ]: Revision
	#	r[1-9]\d?
	print STDOUT 'Info: $ARGV[ 7 ]: Revision:= |' . $ARGV[ 7 ] . '|' . $/ if $bVerbose;
	unless ( $ARGV[ 7 ] =~ /^r\d{1,2}$/ ) {
		print STDERR
			'Error: Argument( "Revision" ):= |' . $ARGV[ 7 ] . '|' . $/ .
			"\t" . 'r로 시작하고, 최소 하나, 최대 두 개의 숫자로 끝나야 한다' . $/;
		$returnVal = 0;
	}
	$ghVar{ 'Revision' } = $ARGV[ 7 ];

	# 4. Note
	print STDOUT 'Info: $ARGV[ 8 ]: Comment:= |' . $ARGV[ 8 ] . '|' . $/ if $bVerbose;
	if ( $ARGV[ 8 ] =~ /^\s*$/ ) {
		print STDERR
			'Error: Argument( "Comment" ):= |' . $ARGV[ 8 ] . '|' . $/ .
			"\t" . 'Comment를 적어야 한다' . $/;
		$returnVal = 0;
	}
	if ( length( $ARGV[ 8 ] ) > 255 ) {
		print STDERR
			'Error: Argument( "Comment" ):= |' . $ARGV[ 8 ] . '|' . $/ .
			"\t" . '글자수 = ' . length( $ARGV[ 8 ] ) . ': 최대 255글자까지 허용된다' . $/;
		$returnVal = 0;
	}
	$ghVar{ 'Comment' } = $ARGV[ 8 ];


	# Td or Non-Td specific info.
	if ( uc( $ghVar{ 'GroupName' } ) eq 'TD' ) {	# Td specific info.
		$ghVar{ 'Anim' }		= $ARGV[ 9 ];
		$ghVar{ 'Texture' }		= $ARGV[ 10 ];

		$ghVar{ 'PixelSamples' }	= $ARGV[ 11 ];
		$ghVar{ 'ShadingRate' }		= $ARGV[ 12 ];
	} else {					# Non-Td specific info.
		$ghVar{ 'Anim' }		= $ARGV[ 9 ];
		$ghVar{ 'Mdl' }			= $ARGV[ 10 ];
		$ghVar{ 'Texture' }		= $ARGV[ 11 ];
		$ghVar{ 'Fx' }			= $ARGV[ 12 ];
		$ghVar{ 'Td' }			= $ARGV[ 13 ];
		$ghVar{ 'Comp' }		= $ARGV[ 14 ];
		$ghVar{ 'Setup' }		= $ARGV[ 15 ];
		$ghVar{ 'Concept' }		= $ARGV[ 16 ];

		$ghVar{ 'FocalLength' }		= $ARGV[ 17 ];
		$ghVar{ 'FPS' }			= $ARGV[ 18 ];
	}

	my	$Password = $ARGV[ uc( $ghVar{ 'GroupName' } ) eq 'TD' ? 13 : 19 ];
	# $ARGV[ $iPassword ]: Password
	#	\S{4,}
	if ( $Password =~ /\s/ || length( $Password ) < 4 ) {
		print STDERR
			'Error: Argument( "Password" ):' . $/ .
			"\t" . '공백문자(" ", "\t", "\n")를 포함하지 않고, 적어도 4글자이어야 한다' . $/;
		$returnVal = 0;
	}
	$ghVar{ 'Password' } = $Password;


	print STDOUT '=' x 80 . $/ if $bVerbose;
	return $returnVal;
}

# Check if given range of frame is overlapped with that of other frame.
#	1: yes
#	0: no
sub checkFrame
{
	my	( $FROM, $TO, $BY ) = @{ $_[ 0 ] };
	my	$LAST = $FROM + $BY * int( ( $TO - $FROM ) / $BY );

#	print STDOUT $from . ' - ' . $to . ': ' . $by . $/;
	for ( @{ $ghVar{ 'Frame' } } ) {
		my	( $from, $to, $by ) = @{ $_ };
		my	$last = $from + $by * int( ( $to - $from ) / $by );
		next if ( $FROM > $last || $LAST < $from );
		return 1;
	}

	return 0;
}

# Using mysql, get current date(YYYYMMDD) and a unique ID.
sub getUUID
{
#	my	$x = `C:/cygwin/usr/local/bin/mysql.exe --user=bloomus --host=192.168.2.30 --password=daily --no-beep --show-warnings --skip-column-names '--execute=SELECT REPLACE(CURDATE(), "-", ""), REPLACE(UUID(), "-", "")'`;
	my	$EscapedUsername = $ghVar{ 'UserName' };
	$EscapedUsername =~ s/\\/\\\\/g;		# \	=> \\
	$EscapedUsername =~ s/\"/\\\"/g;		# "	-> \"
	my	$EscapedPassword = $ghVar{ 'Password' };
	$EscapedPassword =~ s/\\/\\\\/g;		# \	=> \\
	$EscapedPassword =~ s/\"/\\\"/g;		# "	-> \"
	my	$SQLQuery = join( ' ', @gcMySQL0 ) .
		' --user=' . $EscapedUsername . ' --password=' . $EscapedPassword .
		' --execute="SELECT CURDATE(), UUID()"';
	print STDOUT 'Query:= |' . $SQLQuery . '|' . $/ if $bVerbose;
	my	$SQLResult = qx/$SQLQuery 2>&1/;

	# Check the status by examining the returned result.
	#	2007-08-07	de9d32ca-95da-102a-824c-000423d90c00
	my	( $Date, $UUID ) = $SQLResult =~ /^\s*(\d{4}-\d{2}-\d{2})\s+([0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12})\s*$/;
	unless ( $Date ) {
		print STDERR $SQLResult;
		return 0;
	}
	$Date =~ s/-//g;
	$UUID =~ s/-//g;
	$ghVar{ 'Date' } = $Date;
	$ghVar{ 'UUID' } = $UUID;
	print STDOUT 'Date: ' . $Date . ', UUID: ' . $UUID . $/ if $bVerbose;

	$ghVar{ 'Image' } -> { 'imgUniqueFilename0' } =
		$ghVar{ 'Date' } . '.' .
		$ghVar{ 'ShowName' } . ( $ghVar{ 'SeqName' } ? ( '.' . $ghVar{ 'SeqName' } ) : '' ) . '.' .
		$ghVar{ 'ShotName' } . '.' .
		$ghVar{ 'GroupName' } . '.' .
		$ghVar{ 'UserName' } . '.' .
		$ghVar{ 'Revision' } . '.' .
		$ghVar{ 'UUID' };

	return 1;
}

=head1
grTools bloomusLogo.tif -Label foregroundImage \
	-Color 1024 768 PixelFormat24bppRGB 0 0 0 \
	-AddText Show Arial 30 FontStyleRegular 100 120 100 40 255 255 255 \
	-AddText Guyana Arial 28 FontStyleRegular 200 120 '$WIDTH - 200' 40 200 200 200 \
	-AddText Shot Arial 30 FontStyleRegular 100 170 100 40 255 255 255 \
	-AddText hq13 Arial 28 FontStyleRegular 200 170 '$WIDTH - 200' 40 200 200 200 \
	-AddText Dept. Arial 30 FontStyleRegular 100 220 100 40 255 255 255 \
	-AddText Prog Arial 28 FontStyleRegular 200 220 '$WIDTH - 200' 40 200 200 200 \
	-AddText User Arial 30 FontStyleRegular 100 270 100 40 255 255 255 \
	-AddText icentrik Arial 28 FontStyleRegular 200 270 '$WIDTH - 200' 40 200 200 200 \
	-AddText Rev. Arial 30 FontStyleRegular 100 320 100 40 255 255 255 \
	-AddText r3 Arial 28 FontStyleRegular 200 320 '$WIDTH - 200' 40 200 200 200 \
	-AddText Comment Arial 30 FontStyleRegular 100 370 200 40 255 255 255 \
	-AddText 'hello, world: 1\nhello, world: 2\n' 'Arial' 28 FontStyleRegular 200 420 '$WIDTH - 50' 200 200 200 200 \
	-Label backgroundImage \
	foregroundImage -Over backgroundImage 100 30
=cut
sub makeSlateImage
{
	my	$imgSlatePathname = $gcDailyImagePath . $ghVar{ 'Image' } -> { 'imgUniqueFilename0' } . '.0.0000.tif';
	print STDOUT 'SLATE:= |' . $imgSlatePathname . '|' . $/ if $bVerbose;
	my	$imgSlateWinPathname = $imgSlatePathname;
	$imgSlateWinPathname =~ s|/|\\|g;
	my	$imgBloomusLogWinPathname = $gcBloomusLogoImage;
	$imgBloomusLogWinPathname =~ s|/|\\|g;
	my	@grToolsSlate = ();
	push( @grToolsSlate,
		$gcgrToolsProg,

		'--hide',		# Disble WINDOWS

		# Logo
#		'-Color',
#		'200',
#		'40',
#		'PixelFormat32bppARGB',
		$imgBloomusLogWinPathname,
		'-Label',
		'foregroundImage',

		# Prepare slate.
		'-Color',
		'1024',
		'768',
		'PixelFormat24bppRGB',
		'0',
		'0',
		'0',
	);

	push( @grToolsSlate,
		# Carve texts...
		'-AddText',
		'Show',
		'Arial',
		'30',
		'FontStyleRegular',
		'100',
		'120',
		'100',
		'40',
		'255',
		'255',
		'255',

		'-AddText',
		$ghVar{ 'ShowName' },
		'Arial',
		'28',
		'FontStyleRegular',
		'200',
		'120',
		'$WIDTH - 200',
		'40',
		'200',
		'200',
		'200',

		'-AddText',
		'Shot',
		'Arial',
		'30',
		'FontStyleRegular',
		'100',
		'170',
		'100',
		'40',
		'255',
		'255',
		'255',

		'-AddText',
		$ghVar{ 'ShotName' },
		'Arial',
		'28',
		'FontStyleRegular',
		'200',
		'170',
		'$WIDTH - 200',
		'40',
		'200',
		'200',
		'200',

		'-AddText',
		'Dept.',
		'Arial',
		'30',
		'FontStyleRegular',
		'100',
		'220',
		'100',
		'40',
		'255',
		'255',
		'255',

		'-AddText',
		$ghVar{ 'GroupName' },
		'Arial',
		'28',
		'FontStyleRegular',
		'200',
		'220',
		'$WIDTH - 200',
		'40',
		'200',
		'200',
		'200',

		'-AddText',
		'User',
		'Arial',
		'30',
		'FontStyleRegular',
		'100',
		'270',
		'100',
		'40',
		'255',
		'255',
		'255',

		'-AddText',
		$ghVar{ 'UserName' },
		'Arial',
		'28',
		'FontStyleRegular',
		'200',
		'270',
		'$WIDTH - 200',
		'40',
		'200',
		'200',
		'200',

		'-AddText',
		'Rev.',
		'Arial',
		'30',
		'FontStyleRegular',
		'100',
		'320',
		'100',
		'40',
		'255',
		'255',
		'255',

		'-AddText',
		$ghVar{ 'Revision' },
		'Arial',
		'28',
		'FontStyleRegular',
		'200',
		'320',
		'$WIDTH - 200',
		'40',
		'200',
		'200',
		'200',

		'-AddText',
		'Comment',
		'Arial',
		'30',
		'FontStyleRegular',
		'100',
		'370',
		'200',
		'40',
		'255',
		'255',
		'255',

		'-AddText',
		$ghVar{ 'Comment' },
		'Arial',
		'28',
		'FontStyleRegular',
		'200',
		'420',
		'$WIDTH - 50',
		'200',
		'200',
		'200',
		'200',
	);
	if ( uc( $ghVar{ 'GroupName' } ) eq 'TD' ) {	# Td specific info.
		push( @grToolsSlate,
			# Anim
			'-AddText',
			'Anim',
			'Arial',
			'18',
			'FontStyleRegular',
			'550',
			'120',
			'100',
			'40',
			'255',
			'255',
			'255',

			'-AddText',
			$ghVar{ 'Anim' },
			'Arial',
			'18',
			'FontStyleRegular',
			'630',
			'120',
			'$WIDTH - 200',
			'80',			# 80 / 20 = 4: Up to 4-lines
			'200',
			'200',
			'200',

			# Texture
			'-AddText',
			'Texture',
			'Arial',
			'18',
			'FontStyleRegular',
			'550',
			'210',
			'100',
			'40',
			'255',
			'255',
			'255',

			'-AddText',
			$ghVar{ 'Texture' },
			'Arial',
			'18',
			'FontStyleRegular',
			'630',
			'210',
			'$WIDTH - 200',
			'100',			# 100 / 20 = 5: Up to 5-lines
			'200',
			'200',
			'200',

			# PixelSamples
			'-AddText',
			'Pixel Samples',
			'Arial',
			'18',
			'FontStyleRegular',
			'600',
			'320',
			'300',
			'40',
			'255',
			'255',
			'255',

			'-AddText',
			$ghVar{ 'PixelSamples' },
			'Arial',
			'18',
			'FontStyleRegular',
			'750',
			'320',
			'$WIDTH - 200',
			'40',			# 100 / 20 = 5: Up to 5-lines
			'200',
			'200',
			'200',

			# ShadingRate
			'-AddText',
			'Shading Rate',
			'Arial',
			'18',
			'FontStyleRegular',
			'600',
			'350',
			'300',
			'40',
			'255',
			'255',
			'255',

			'-AddText',
			$ghVar{ 'ShadingRate' },
			'Arial',
			'18',
			'FontStyleRegular',
			'750',
			'350',
			'$WIDTH - 200',
			'40',			# 100 / 20 = 5: Up to 5-lines
			'200',
			'200',
			'200',
		);
	} else {					# Non-Td specific info.
		push( @grToolsSlate,
			# Anim
			'-AddText',
			'Anim',
			'Arial',
			'18',
			'FontStyleRegular',
			'600',
			'120',
			'100',
			'40',
			'255',
			'255',
			'255',

			'-AddText',
			$ghVar{ 'Anim' },
			'Arial',
			'18',
			'FontStyleRegular',
			'680',
			'120',
			'$WIDTH - 200',
			'40',
			'200',
			'200',
			'200',

			# Mdl
			'-AddText',
			'Mdl',
			'Arial',
			'18',
			'FontStyleRegular',
			'820',
			'120',
			'100',
			'40',
			'255',
			'255',
			'255',

			'-AddText',
			$ghVar{ 'Mdl' },
			'Arial',
			'18',
			'FontStyleRegular',
			'900',
			'120',
			'$WIDTH - 200',
			'40',
			'200',
			'200',
			'200',

			# Texture
			'-AddText',
			'Texture',
			'Arial',
			'18',
			'FontStyleRegular',
			'600',
			'160',
			'100',
			'40',
			'255',
			'255',
			'255',

			'-AddText',
			$ghVar{ 'Texture' },
			'Arial',
			'18',
			'FontStyleRegular',
			'680',
			'160',
			'$WIDTH - 200',
			'40',
			'200',
			'200',
			'200',

			# Fx
			'-AddText',
			'Fx',
			'Arial',
			'18',
			'FontStyleRegular',
			'820',
			'160',
			'100',
			'40',
			'255',
			'255',
			'255',

			'-AddText',
			$ghVar{ 'Fx' },
			'Arial',
			'18',
			'FontStyleRegular',
			'900',
			'160',
			'$WIDTH - 200',
			'40',
			'200',
			'200',
			'200',

			# Td
			'-AddText',
			'Td',
			'Arial',
			'18',
			'FontStyleRegular',
			'600',
			'200',
			'100',
			'40',
			'255',
			'255',
			'255',

			'-AddText',
			$ghVar{ 'Td' },
			'Arial',
			'18',
			'FontStyleRegular',
			'680',
			'200',
			'$WIDTH - 200',
			'40',
			'200',
			'200',
			'200',

			# Comp
			'-AddText',
			'Comp',
			'Arial',
			'18',
			'FontStyleRegular',
			'820',
			'200',
			'100',
			'40',
			'255',
			'255',
			'255',

			'-AddText',
			$ghVar{ 'Comp' },
			'Arial',
			'18',
			'FontStyleRegular',
			'900',
			'200',
			'$WIDTH - 200',
			'40',
			'200',
			'200',
			'200',

			# Setup
			'-AddText',
			'Setup',
			'Arial',
			'18',
			'FontStyleRegular',
			'600',
			'240',
			'100',
			'40',
			'255',
			'255',
			'255',

			'-AddText',
			$ghVar{ 'Setup' },
			'Arial',
			'18',
			'FontStyleRegular',
			'680',
			'240',
			'$WIDTH - 200',
			'40',
			'200',
			'200',
			'200',

			# Concept
			'-AddText',
			'Concept',
			'Arial',
			'18',
			'FontStyleRegular',
			'820',
			'240',
			'100',
			'40',
			'255',
			'255',
			'255',

			'-AddText',
			$ghVar{ 'Concept' },
			'Arial',
			'18',
			'FontStyleRegular',
			'900',
			'240',
			'$WIDTH - 200',
			'40',
			'200',
			'200',
			'200',

			# FocalLength
			'-AddText',
			'Focal Length',
			'Arial',
			'18',
			'FontStyleRegular',
			'650',
			'300',
			'300',
			'40',
			'255',
			'255',
			'255',

			'-AddText',
			$ghVar{ 'FocalLength' },
			'Arial',
			'18',
			'FontStyleRegular',
			'800',
			'300',
			'$WIDTH - 200',
			'40',
			'200',
			'200',
			'200',

			# FPS
			'-AddText',
			'FPS',
			'Arial',
			'18',
			'FontStyleRegular',
			'650',
			'340',
			'300',
			'40',
			'255',
			'255',
			'255',

			'-AddText',
			$ghVar{ 'FPS' },
			'Arial',
			'18',
			'FontStyleRegular',
			'800',
			'340',
			'$WIDTH - 200',
			'40',
			'200',
			'200',
			'200',
		);
	}

	push( @grToolsSlate,
		'-Label',
		'backgroundImage',

		# Compose
		'foregroundImage',
		'-Over',
		'backgroundImage',
		'100',
		'30',

		# Save
		'-FileOut',
		$imgSlateWinPathname,
		'TIFF',
		'PixelFormat24bppRGB',
	);
	print STDOUT '|' . join( '| |', @grToolsSlate ) . '|' . $/ if $bVerbose;
	if ( &RunProgram( 'grTools', \@grToolsSlate ) != 0 ) {
		print STDERR 'grTools: Slate: $? != 0' . $/;
		return 0;
	}
	return 1;
}

sub makeDailyImages
{
	print STDOUT
		'Trace: Make Images for Daily...' . $/ .
		'=' x 80 . $/ if $bVerbose;

	my	$hImageRef = $ghVar{ 'Image' };
	my	( $imgPath, $imgBareFilename, $imgType, $imgUniqueFilename0 ) = ( $hImageRef -> { 'imgPath' }, $hImageRef -> { 'imgBareFilename' }, $hImageRef -> { 'imgType' }, $hImageRef -> { 'imgUniqueFilename0' } );

	if ( &makeSlateImage() == 0 ) {
		print STDERR 'Error: Cannot make a Slate image' . $/;
		return 0;
	}

	my	@DailyImage;	# It says if a daily image of the frame corresponding to index is made or not.
				#	'': Not yet,
				#	Otherwise: Done
	for ( my $i = 1; $i < 10000; ++$i) {
		$DailyImage[ $i ] = '';	# By default, Not yet.
	}
	my	$iReturnVal = 1;
	for ( my $iQueue = 0; $iQueue < @{ $ghVar{ 'Frame' } }; ++$iQueue ) {
		my	( $from, $to, $by ) = @{ $ghVar{ 'Frame' } -> [ $iQueue ] };
		my	$iSerial = 1;
		for ( my $iFrame = $from; $iFrame <= $to; $iFrame += $by, ++$iSerial ) {
##	SavedFileName	CHAR(255) NOT NULL,	-- (FileID)	Date.ShowName.ShotName.UserName.Revision.UUID.#
			my	$imgOutPathname = $gcDailyImagePath . $imgUniqueFilename0 . '.' . ( 1 + $iQueue ) . '.' . sprintf( '%04d', $iSerial ) . ( $imgType ne 'GIF' ? '.tif' : '.gif' );
			print STDOUT $imgOutPathname . $/ if $bVerbose;

			my	$imgTmpPathname;
			if ( $DailyImage[ $iFrame ] ) {			# Already exists...
				if ( &cp( $DailyImage[ $iFrame ], $imgOutPathname ) != 0 ) {
					print STDERR 'cp: $? != 0' . $/;
					$iReturnVal = 0;
				}
			} else {					# Not yet...
				# Make a daily image.
				my	$imgInPathname0 = $imgPath . $imgBareFilename . '.' . sprintf( '%04d', $iFrame );
				my	$bMissing = 0;
				unless ( -f ( $imgInPathname0 . '.' . $imgType ) ) {
					print STDERR 'Warning: Image does not exist: |' . $imgInPathname0 . '.' . $imgType . '|' . $/;
					$bMissing = 1;
				}

				my	$imgInPathname;
				print STDOUT $imgInPathname . $/ if $bVerbose;
				if ( !$bMissing && ( $imgType eq 'IFF' || $imgType eq 'TGA' ) ) {
					# First, convert iff to tif.
					#	imgcvt '$imgInPathname0.iff' '$imgInPathname
					$imgInPathname = $gcProgTmpPath . $ghVar{ 'UUID' } . '.' . sprintf( '%04d', $iFrame ) . '.' . $iQueue . '.bmp';
					my	@imgcvt = (
						$gcimgcvtProg,
						$imgInPathname0 . '.' . lc( $imgType ),
						$imgInPathname
					);
					print STDOUT '|' . join( '| |', @imgcvt ) . '|' . $/ if $bVerbose;
					if ( &RunProgram( 'imgcvt', \@imgcvt ) != 0 ) {
						print STDERR 'imgcvt: Frame # = ' . $iFrame . ': $? != 0: Ignored' . $/;
#						$iReturnVal = 0;
						$imgInPathname = $gcMissingFrameImage;
					}
				} else {
					if ( !$bMissing && $imgType eq 'GIF' ) {
						$imgInPathname = $imgInPathname0 . '.gif';
					} else {
						$imgInPathname = $bMissing ? $gcMissingFrameImage : ( $imgInPathname0 . '.' . $imgType );
					}
				}
			if ( $imgType eq 'GIF' ) {
				# Just copy.
				if ( &cp( $imgInPathname, $imgOutPathname ) != 0 ) {
					print STDERR 'cp: $? != 0' . $/;
					$iReturnVal = 0;
				}
			} else {
				# grTools '$imgInPathname' -AddText '...'
				my	$imgInWinPathname = $imgInPathname;
				$imgInWinPathname =~ s|/|\\|g;
				my	$imgOutWinPathname = $imgOutPathname;
				$imgOutWinPathname =~ s|/|\\|g;
				my	@grTools = (
					$gcgrToolsProg,
					$imgInWinPathname,

					'-AddText',
					$ghVar{ 'ShowName' } . '.' . $ghVar{ 'ShotName' } . '.' . $ghVar{ 'GroupName' } . '.' . $ghVar{ 'UserName' } . '.' . $ghVar{ 'Revision' } . '.' . sprintf( '%04d', $iFrame ),
					# 'Arial' 18 50 '$HEIGHT - 30' '$WIDTH / 2 - 20' 40 255 255 255
					'Arial',
					'18',
					'FontStyleRegular',
					'50',			# X
					'$HEIGHT - 30',		# Y
					'$WIDTH - 18',		# Width: $WIDTH - X
					'30',			# Height: $HEIGHT - Y
					'255', '255', '255',	# Font Color: White

					'-FileOut',
					$imgOutWinPathname,
					'TIFF',
					'PixelFormat24bppRGB',
				);
				print STDOUT  '|' . join( '| |', @grTools ) . '|' . $/ if $bVerbose;
				if ( &RunProgram( 'grTools', \@grTools ) != 0 ) {
					print STDERR 'grTools: $? != 0' . $/;
					$iReturnVal = 0;
				}
			}

				if ( !$bMissing && ( $imgType eq 'IFF' || $imgType eq 'TGA' ) ) {
					# Delete a temporary .BMP image.
					if ( $imgInPathname ne $gcMissingFrameImage && unlink( $imgInPathname ) == 0) {
						print STDERR 'unlink: |' . $imgInPathname . '|: Failure' . $/;
					}
				}

				$DailyImage[ $iFrame ] = $imgOutPathname;	# Set it is Done.
			}

			if ( $iReturnVal == 0 ) {
				# Remove all the temporary images if any.
				unlink <$gcProgTmpPath . $ghVar{ 'UUID' } . '.*'>;
				# Remove all the images made.
				unlink <$gcDailyImagePath . $imgUniqueFilename0 . '.*'>;

				# End of loop; no more processing.
				last;
			}
		}
		last if ( $iReturnVal == 0);	# If there is an error, no more processing.
	}

	print STDOUT '=' x 80 . $/ if $bVerbose;
	return $iReturnVal;
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

sub updateDBTable
{
#	INSERT INTO TDaily (DateTime, GroupName, UserName, ShowName, ShotName, Revision, Frame, UniqueFileName0, Comment) VALUES (NOW(), 'Prog', 'icentrik', 'swShow1', 'sqSeq1', 'stShot1', 'r3', '1-10:2,20-100,24', UUID(), 'hello, world');

	my	@aFrame = ();
	for ( my $iQueue = 0; $iQueue < @{ $ghVar{ 'Frame' } }; ++$iQueue ) {
		my	( $from, $to, $by ) = @{ $ghVar{ 'Frame' } -> [ $iQueue ] };
		push( @aFrame, $from == $to ? $from : ( $by == 1 ? ( $from . '-' . $to ) : ( $from . '-' . $to . ':' . $by ) ) );
	}

	my	$EscapedUsername = $ghVar{ 'UserName' };
	$EscapedUsername =~ s/\\/\\\\/g;		# \	=> \\
	$EscapedUsername =~ s/\"/\\\"/g;		# "	-> \"
	my	$EscapedPassword = $ghVar{ 'Password' };
	$EscapedPassword =~ s/\\/\\\\/g;		# \	=> \\
	$EscapedPassword =~ s/\"/\\\"/g;		# "	-> \"

	my	$EscapedComment = $ghVar{ 'Comment' };
	$EscapedComment =~ s/\\/\\\\/g;		# \	=> \\
	$EscapedComment =~ s/\"/\\\"/g;		# "	-> \"

	my	$SQLQuery0 =
		'--execute=INSERT INTO DBBloomUs.TDaily ' .
		'(DateTime, GroupName, UserName, ShowName, ShotName, Revision, Frame, UniqueFileName0, ImageType, Comment)' . 
		' VALUES (NOW(), ' . 
		'"' . $ghVar{ 'GroupName' } . '"' . ', ' .
		'"' . $ghVar{ 'UserName' } . '"' . ', ' .
		'"' . $ghVar{ 'ShowName' } . '"' . ', ' .
		'"' . $ghVar{ 'ShotName' } . '"' . ', ' .
		'"' . $ghVar{ 'Revision' } . '"' . ', ' .
		'"' . join( ',', @aFrame ) . '"' . ', ' .
		'"' . $ghVar{ 'Image' } -> { 'imgUniqueFilename0' } . '"' . ', ' .
		'"' . ( $ghVar{ 'Image' } -> { 'imgType' } ne 'GIF' ? 'TIFF' : 'GIF' ) . '"' . ', ' .
		'"' . $EscapedComment . '"' .
		')'
	;
	push( @gcMySQL0,
		'--user=' . $EscapedUsername,
		'--password=' . $EscapedPassword,

		$SQLQuery0
	);
	print STDOUT '|' . join( '| |', @gcMySQL0 ) . '|' . $/ if $bVerbose;
	my	$iReturnVal = &RunProgram( 'mysql', \@gcMySQL0 );
	pop( @gcMySQL0 );
	if ( $iReturnVal != 0 ) {
		print STDERR 'mysql: $? != 0' . $/;
		return 0;
	}

	return 1;
}

sub cp
{
	my	( $inFile, $outFile ) = @_;

	my	$inWinFile = $inFile;
	$inWinFile =~ s|/|\\|g;
	my	$outWinFile = $outFile;
	$outWinFile =~ s|/|\\|g;
	my	@copy = (
		'CMD', '/C',
		'COPY', $inWinFile, $outWinFile,
		'>', 'NUL', '2>&1'
	);
	print STDOUT  '|' . join( '| |', @copy ) . '|' . $/ if $bVerbose;

	if ( &RunProgram( 'COPY', \@copy ) != 0 ) {
		print STDERR 'COPY: |' . $inFile . '| -> |' . $outFile . '|: $? != 0' . $/;
		return 1;
	}
	return 0;


	unless ( open( IN, '< ' . $inFile ) ) {
		print STDERR 'open(|' . $inFile . '|): Failure: ' . $! . $/;
		return 1;
	}
	unless ( open( OUT, '> ' . $outFile ) ) {
		close( IN );

		print STDERR 'open(|' . $outFile . '|): Failure: ' . $! . $/;
		return 2;
	}

	while ( <IN> ) {
		print OUT;
	}

	my	$iReturnVal = 0;
	$iReturnVal = 3 unless close( OUT );
	$iReturnVal = 4 unless close( IN );

	return $iReturnVal;
}
