use ERModule;

use strict;


# parseCommandLine함수에서 %ghVar의 "command line option"에 해당하는 옵션을 설정한다.
#
#	2007/6/15 (KST)
#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)


# "RIB Box" 정보를 처리하는 방식를 달리한다.
# 지금까지는 각 "job chunk"를 정의한 $HoHChunk{ "JobChunkName" }의
# "rbref"라는 Key를 통해 "RIB Box" 정보를 얻었다.
# Sifc처럼 독립적인 파일(rbif: RIB Box Information File)에서 얻을 수도 있도록 한다.
#
# 전역변수 %HoARIBBoxInfo 라는 변수에 rbif에서 얻은 정보를 넣는다.
#%HoARIBBoxInfo = ();
#my	@aRBFile = ();
# $HoHChunk{ "JobChunkName" }의 'rbref'라는 Key에 해당하는 값이
# 이 변수에 대한 참조가 되도록 한다.
#
#	$HoHChunk{ "JobChunkName" } = {
#		......
#		'rbref'		=> \%HoARIBBoxInfo,
#		......
#	};
#
# 1) 각 "job chunk" 고유의 "RIB Box" 정보를 사용하려면,
# 지금처럼 직접 정보를 넣을 수도 있다.
#
#	$HoHChunk{ "JobChunkName" } = {
#		......
#		'rbref'		=> {
#			$ENUM_RIB_BOX__DECLARE__JUST_BEFORE_FIRST_FRAME_BEGIN	=> "......",
#			......
#		},
#		......
#	};
#
# 2) 여러 개의 rbif가 있다면, sif처럼, 입력된 순서에 따라 각 rbif를 합한다. (LIFO)
# (.sif의 경우에도 마지막에 설정된 파일이 우선한다.)
# 예를 들어, rbFile1.pl, rbFile2.pl의 순서에 따라 .rbif가 주어졌을 때
# 첫 번째 파일(rbFile1.pl)에 다음과 같은 내용이 있다고 하자.
#
#	$ENUM_RIB_BOX__WORLD__JUST_BEFORE_FIRST_LIGHTSOURCE_BLOCK		=> [
#		'# RIB Box: Just before FIRST "LightSource" block' . "\r\n"
#	],
#
# 즉 모든 "ELEMENT"에 대해서 적용된다.
# 두 번째 파일(rbFile2.pl)에서 "Image"라는 "ELEMENT"즉 "렌더링 이미지"에 해당하는 subframe에서는
# 적용하지 않도록 하기 위해서는 다음과 같이 하면 된다.
#
#	$ENUM_RIB_BOX__WORLD__JUST_BEFORE_FIRST_LIGHTSOURCE_BLOCK		=> [
#		undef, 'Image'
#	],
#
# "undef"을 사용했음을 주목한다!
#
#	2007/5/25 (EST)
#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)



# 2. RIB형식은 "ascii"로 선택할 수 있도록 한다.
# 기본 형식은 "binary" & "gzip"이다.
#	"-ascii"
#my	$bAscii = 0;
#my	$bVerboseRIBBox = 0;

# 3. 이 프로그램에서 처리하는 "경로"에 대한 내용을 화면에 표시한다.
# 표시되는 "경로"는 다음과 같다.
#	1) 저장되는 RIB파일의 경로
#	2) 각 RIB파일에서 저장하는 이미지 파일의 경로
#	3) Subframe으로 나눌 때, subframe단위로 저장되는 RIB파일의 경로
# 다음 변수가, 명령줄에서 "-vPath"옵션으로, 설정되어 있다면 "경로"와 관련된 추가정보가 표시된다.
#my	$bVerbosePath = 0;
#	2007/4/19 (EST)
#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)

# 5. 쉐이더 정보를 적을 때, 'content'안에 "쉐이더"와 '텍스쳐"를 적는 부분이 있다.
# 아무런 말이 없으면, 프로그램에서 자동적으로 쉐이더 앞에는 "$SHADER_PATH/"라는 문자열을,
# ".tex"로 끝나는 "텍스쳐"에는 "$TEXTURE_PATH/"라는 문자열을 넣고 이를 "확장"한다.
# 단, "$SHADER_PATH"나 "$TEXTURE_PATH"가 정의되어 있을 때에 한해서.
# 만약 이러한 "자동추가"를 원치않는다면
# 	$SHADER_PATH나 $TEXTURE_PATH를 정의하지 않거나
#	"쉐이더"나 "텍스쳐"의 앞에 '-'문자를 넣는다.
# 예를 들어,
#	Surface "Delux2"
#		"string shadowName" [ "xxx.tex" ]
# 인 경우에, $SHADER_PATH와 $TEXTURE_PATH가 정의되어 있다면
# 실제로는 다음과 같은 문자열이 사용되고 이를 "확장"한다.
#	Surface "$SHADER_PATH/Delux2"
#		"string shadowName" [ "$TEXTURE_PATH/xxx.tex" ]
# 만약 이것이 원하는 것이 아니라면, 예를 들어, $SHADER_EXTRA_PATH를 정의한 후
#	Surface "-rmanshader/Delux2"
#		"string shadowName" [ "xxx.tex" ]
# 라고 하면, 이는 다음과 같이 해석되고 "확장"된다.
# 	Surface "-rmanshader/Delux2"
# 		"string shadowName" [ "$TEXTURE_PATH/xxx.tex" ]
# 즉 "쉐이더"나 ".tex"로 끝나는 "텍스쳐"에 '-'로 시작하는 문자열이 있으면
# 그 "쉐이더"나 "텍스쳐"에는 "자동적"으로 "$SHADER_PATH"나 "$TEXTURE_PATH"가 추가되어 "확장"되지 않는다.
#
#	2007/2/14 (KST)
#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)
#my	@aSIFile = ();

#my	$bVerbose = 0;
#my	$bVVerbose = 0;
#my	$bVVVerbose = 0;

# 옵션을 사용할 수 있도록 한다.
# Usage:
#	$ edit.pl [ -sif '.../sifSub.pl' | -jcf '.../jcfSub.pl' ] x.rib
#
# -sif: 쉐이더 정보를 담은 파일
#	@AoHShaderInfoByArt를 설정한다.
# -jcf: "작업 chunk"를 정의하는 파일
#	%HoHChunk를 설정한다.
# x.rib: 원본 RIB
#	반드시 주어져야 한다.
#
# 주1) -jcf, -sif 둘 중의 적어도 하나는 있어야 한다.
#	1. 둘 다 있다면 -sif가 지정하는 쉐이더 정보를 사용하여,
#	-jcf가 지정한 "작업"을 하는 것이다.
#
#	2. -jcf가 없는 경우
#	-sif에 주어지는 파일을 참고하여, 쉐이더 정보만을 고치도록 하는 다음 %HoHChunk를 사용한다.
#
#		%HoHChunk = ();
#		$HoHChunk{ 'byArt.rib' } = {
#			'type'	=> $CHUNK_TYPE__DEL,
#			'jcref'	=> [],
#			'siref'	=> \@AoHShaderInfoByArt
#		};
#
#	새로운 RIB의 이름은 'byArt.rib'이다.
#
#	3. -sif가 없는 경우
#	-jcf 다음에 주어지는 파일을 참고하여 적절한 "작업"을 한다.
#	이 때, 쉐이더 정보를 담은 변수 @AoHShaderInfoByArt는 초기화한다.
#
#		@AoHShaderInfoByArt = ();
#
#
#	2006/11/28 (EST)
#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)

# 1. -jcf, -sif모두 없는 경우도 다룬다.
# 원본은 그대로 두고 RIB의 경로를 포함한 "쉐도우 맵 텍스쳐", "이미지"의 경로를 바꿀 때 사용한다.
# 2. -jcf가 없고 -sif만 있는 경우처럼, 특수한 "job chunk"를 사용한다.
# CHUNK_TYPE__DEL으로 모든 오브젝트를 건들지 않는 "job chunk"이다.
# -sif가 없으니 "Suface"나 "Displace"쉐이더도 고치지 않는다.
# 다만 경로가 바뀌므로 "쉐도우 맵 텍스쳐", "이미지"의 경로가 바뀐다.
# "쉐도우 맵 텍스쳐"를 참고하는 "LightSource"쉐이더는 바뀔 수 있다.
# 3. -jcf가 없고 -sif만 있는 경우의 사용하던 위의 "job chunk"의 키 이름을
# 기본값인 "byArt"대신에 다른 이름을 사용할 있도록 했다.
# "-k"옵션을 사용한다.
# 4. "-bShd"옵션은 기존에 있던 "쉐도우 맵(텍스쳐)"을 사용하게 한다.
# 기본값은 0으로 기존의 "쉐도우 맵"을 사용하지 않고 새로 만드는 것이다.
#	2007/1/11 (EST)
#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)
#

#my	$addPathExpr	= '$keyChunk';
#my	$rootPathExpr	= '';
# 1. 원본 RIB파일의 이름을 유지한다.
# 무엇보다도 프레임수를 유지한다.
# Display에는 그렇게 했는 데 새로 만드는 RIB에서는 그렇게 하지 못한 것을 수정했다.
# 따라서 모든 파일이름($Path$filename)은 "$PATH$keyChunk/$filename"으로 바뀐다.
# 2. 여기에서 "-d"옵션을 써서 "추가경로 표현"를 넣을 수 있다.
# 파일의 경로에 "추가경로 표현"이 해석된 경로가 추가된다.
#	1) "추가경로 표현"의 기본값은 '$keyChunk'이다.
#	2) '$keyChunk'가 말해주듯(작은 따옴표!), 이 "추가경로 표현"은 "qq"와 "eval"를 통해 해석된다.
# 다음은 한 예이다.
#{
#	my	$a = 'A';
#	my	$aExpr = 'qq|$a x 10/a|';
#	my	$aEval = eval( $aExpr );
#	print STDERR $aEval . $/;	# "A x 10/a"
#}
# 따라서 몇 개의 이미 정의되어 있는 변수를 사용할 수 있다.
#
#	$keyChunk	"job chunk"의 key값이다.
#	$typeChunk	"job chunk"의 유형이다. ($CHUNK_TYPE__DEL, $CHUNK_TYPE__LL, $CHUNK_TYPE__SHD)중의 하나이다.
#
# "qq"와 "eval"로 해석되므로 유효한 어떠한 펄의 문법이 사용될 수 있다.
# 예를 들어, $typeChunk값은 숫자이다. ($CHUNK_TYPE__DEL, $CHUNK_TYPE__LL, $CHUNK_TYPE__SHD) = (1, 2, 3)
# 숫자대신에 각각 'DEL', 'LL', 'SHD'라는 문자열을 넣고 싶다면, "추가경로 표현"을 다음과 같이 적는다.
#
#	"@{[$typeChunk == $CHUNK_TYPE__DEL ? 'DEL' : $typeChunk == $CHUNK_TYPE__LL ? 'LL' : 'SHD']}"
#
# 라고 하면 된다.
# 3. "-R"옵션을 사용하여 "루트경로 표현"을 만들어 기본 "루트경로"를 대신한다.
# (유효한 지는 "mkdir2"에서 발혀질 것이다.)
# "루트경로"라 함은 마야의 "프로젝트 경로"를 뜻한다.
# 기본값으로 렌더맨은 "루트경로"아래에 있는
#	"rib/" (RIB)
#	"rmantex/" (텍스쳐), "rmantex/shd/" (쉐도우 텍스쳐), "rmantex/env/" (환경 텍스쳐)
#	"rmanpix/" (이미지)
# 를 적절히 사용한다. (RAT.ini에 설정되어 있다)
#	1) "-d"옵션처럼 나중에 "qq"와 "eval"롤 써서 해석된다.
#	2) 기본값은 ''이다.
#
# 다음은 "-d"옵션(기본값 '$keyChunk')과 "-R"옵션이 사용될 때 해석되는 경로표현이다.
#
#	"-d"옵션(기본값 '$keyChunk')	"-R"옵션(기본값 '')	"루트경로"		"추가 경로"
#	-				-			기본경로		"-d"옵션의 기본값인 '$keyChunk'
#	-				o			"-R"로 주어지는 표현	"-d"옵션의 기본값인 '$keyChunk'
#	o				-			기본경로		"-d"로 주어지는 표현
#	o				o			"-R"로 주어지는 표현	"-d"로 주어지는 표현
#
#my	$byArtKeyName	= 'byArt';
#my	$bShd		= 0;



# 명령어(editRIB.pl) 다음에 주어지는 인자(argument)를 사용하여,
# 네 값($retVal, $siFile, $jcFile, $ribFileHandlel)을 설정한다.
# $retVal는 처리상황(1: 성공, 0: 실패)이다.
# 실패일때는 나머지 값은 모두 undef이다.
sub parseCommandLine
{
	my	$hCommandLineOptionRef = $ERModule::ghVar{ 'command line option' };

	#### 명령줄에서 입력받을 수있는 옵션의 기본값을 설정한다.
	# 참이 아닌 값들은 굳이 설정할 필요가 없다.
	# 어떤 변수들이 있는 지를 보여주기 위해서 거짓(0, '', undef, (), ...)으로 설정한다!
	# 1. 플래그 옵션
	$hCommandLineOptionRef -> { 'bAscii' }		= 0;
	$hCommandLineOptionRef -> { 'bShd' }		= 0;
	$hCommandLineOptionRef -> { 'bVerboseRIBBox' }	= 0;
	$hCommandLineOptionRef -> { 'bVerbosePath' }	= 0;
	$hCommandLineOptionRef -> { 'bVerbose' }	= 0;
	$hCommandLineOptionRef -> { 'bVVerbose' }	= 0;
	$hCommandLineOptionRef -> { 'bVVVerbose' }	= 0;
	# 2. "jcf", "sif", "rbif"와 같은 파일과 관련된 옵션
	# "jcf"를 제외하고 여러 개의 파일이 올 수 있다.
	$hCommandLineOptionRef -> { 'jcFile' }		= undef;
	@{ $hCommandLineOptionRef -> { 'aRBFile' } }	= ();
	@{ $hCommandLineOptionRef -> { 'aSIFile' } }	= ();
	# 3. 추가경로 설정과 관련된 옵션
	$hCommandLineOptionRef -> { 'addPathExpr' }	= '$keyChunk';
	$hCommandLineOptionRef -> { 'rootPathExpr' }	= '';
	$hCommandLineOptionRef -> { 'byArtKeyName' }	= 'byArt';


	my	$jcFile;		# "jcf"파일이 생략될 수도 있다.
	my	$ribFile;		# 적어도 하나의 RIB 파일을 지정해야 한다.

	# 다음 변수들이 설정되면 기본값을 대신한다.
	my	$addPathExpr;
	my	$rootPathExpr;
	my	$byArtKeyName;

	for (my $i = 0; $i < @ARGV; ++$i) {
		my	$argv = $ARGV[ $i ];

		##### '-'로 시작하는 옵션을 알아본다.
		# 1. 플래그 옵션
		if ( $argv eq '-ascii' ) {
			$hCommandLineOptionRef -> { 'bAscii' } = 1;
			next;
		}
		if ( $argv eq '-vRIBBox' ) {
			$hCommandLineOptionRef -> { 'bVerboseRIBBox' } = 1;
			next;
		}
		if ( $argv eq '-vPath' ) {
			$hCommandLineOptionRef -> { 'bVerbosePath' } = 1;
			next;
		}
		if ( $argv eq '-v' ) {
			$hCommandLineOptionRef -> { 'bVerbose' } = 1;
			next;
		}
		if ( $argv eq '-vv' ) {
			$hCommandLineOptionRef -> { 'bVVerbose' } = 1;
			next;
		}
		if ( $argv eq '-vvv' ) {
			$hCommandLineOptionRef -> { 'bVVVerbose' } = 1;
			next;
		}
		if ( $argv eq '-bShd' ) {
			$hCommandLineOptionRef -> { 'bShd' } = 1;
			next;
		}

		# 2. 'jcf', 'sif', 'rbif'와 같은 파일 옵션을 알아본다.
		if ($argv eq '-jcf') {
			# 다음 인자는 %HoHChunk에 대한 정의가 들어있는 파일이어야 한다.
			if (
				$i == $#ARGV ||			# 마지막 인자이거나 (즉 파일의 이름이 없다)
				$ARGV[ $i + 1 ] =~ /^-/		# 역시 옵션을 뜻할 때는
			) {
				# 적절하지 않은 인자이다.
				print STDERR 'EDITRIB: Error: ' . $argv . ' 다음에는 파일이 주어져야 한다' . $/;

				return 0;
			}

			my	$jcFileTmp = $ARGV[ ++$i ];	# 다음 인자를 해석한다.
			unless ( -f $jcFileTmp ) {
				print STDERR 'EDITRIB: Error: ' . $argv . ': |' . $jcFileTmp . '|: 파일이 아니다(!-f)' . $/;
				return 0;
			}
			if ( $jcFile ) {
				print STDERR 'EDITRIB: Warning: ' . $argv . ': 전에 주어진 파일(|' . $jcFile . '|)은 무시된다' . $/;
			}
			$jcFile = $jcFileTmp;

			# 다음 인자를 해석한다.
			next;
		}
		if ($argv eq '-sif') {
			# 다음 인자는 @AoAShaderInfoByArt에 대한 정의가 들어있는 파일이어야 한다.
			if (
				$i == $#ARGV ||			# 마지막 인자이거나 (즉 파일의 이름이 없다)
				$ARGV[ $i + 1 ] =~ /^-/		# 역시 옵션을 뜻할 때는
			) {
				# 적절하지 않은 인자이다.
				print STDERR 'EDITRIB: Error: ' . $argv . ' 다음에는 파일이 주어져야 한다' . $/;

				return 0;
			}

			foreach my $SIFileTmp ( split( /\s*,\s*/, $ARGV[ ++$i ] ) ) {
				unless ( -f $SIFileTmp ) {
					print STDERR 'EDITRIB: Warning: ' . $argv . ': |' . $SIFileTmp . '|: 파일이 아니다(!-f): 무시한다' . $/;
					next;
				}
				push( @{ $hCommandLineOptionRef -> { 'aSIFile' } }, $SIFileTmp);
			}

			# 다음 인자를 해석한다.
			next;
		}
		if ($argv eq '-rbif') {
			# 다음 인자는 %HoARIBBoxInfo에 대한 정의가 들어있는 파일이어야 한다.
			if (
				$i == $#ARGV ||			# 마지막 인자이거나 (즉 파일의 이름이 없다)
				$ARGV[ $i + 1 ] =~ /^-/		# 역시 옵션을 뜻할 때는
			) {
				# 적절하지 않은 인자이다.
				print STDERR 'EDITRIB: Error: ' . $argv . ' 다음에는 파일이 주어져야 한다' . $/;

				return 0;
			}

			foreach my $RBFileTmp ( split( /\s*,\s*/, $ARGV[ ++$i ] ) ) {
				unless ( -f $RBFileTmp ) {
					print STDERR 'EDITRIB: Warning: ' . $argv . ': |' . $RBFileTmp . '|: 파일이 아니다(!-f): 무시한다' . $/;
					next;
				}
				push( @{ $hCommandLineOptionRef -> { 'aRBFile' } }, $RBFileTmp);
			}

			# 다음 인자를 해석한다.
			next;
		}

		# 3. 추가경로 설정과 관련된 옵션을 알아본다.
		if ( $argv eq '-d' ) {
			# 다음 인자는 '/'로 구분된 "추가경로 표현"이어야 한다.
			if (
				$i == $#ARGV ||			# 마지막 인자이거나 (즉 "추가경로 표현"이 없다)
				$ARGV[ $i + 1 ] =~ /^-/		# 역시 옵션을 뜻할 때는
			) {
				# 적절하지 않은 인자이다.
				print STDERR 'EDITRIB: Error: ' . $argv . ' 다음에는 "/"로 구분된 "추가경로 표현"이 주어져야 한다' . $/;

				return 0;
			}

			if ( $addPathExpr ) {
				print STDERR 'EDITRIB: Warning: 전에 주어진 "추가경로 표현"(|' . $addPathExpr . '|)은 무시된다' . $/;
			}

			$addPathExpr = $ARGV[ ++$i ];
			$addPathExpr =~ s|\\|/|g;
			next;
		}
		if ( $argv eq '-R' ) {
			# 다음 인자는 '/'로 구분된 "루트경로 표현"이어야 한다.
			if (
				$i == $#ARGV ||			# 마지막 인자이거나 (즉 "루트경로 표현"이 없다)
				$ARGV[ $i + 1 ] =~ /^-/		# 역시 옵션을 뜻할 때는
			) {
				# 적절하지 않은 인자이다.
				print STDERR 'EDITRIB: Error: ' . $argv . ' 다음에는 "/"로 구분된 "루트경로 표현"이 주어져야 한다' . $/;

				return 0;
			}

			if ( $rootPathExpr ) {
				print STDERR 'EDITRIB: Warning: 전에 주어진 "루트경로 표현"(|' . $rootPathExpr . '|)은 무시된다' . $/;
			}

			$rootPathExpr = $ARGV[ ++$i ];
			$rootPathExpr =~ s|\\|/|g;
			next;
		}
		if ( $argv eq '-k' ) {
			# $siFile, $jcFile 모두 주어지지 않을 때 사용할,
			# 특수한 "job chunk"의 이름이 주어져야 한다.
			if (
				$i == $#ARGV ||			# 마지막 인자이거나
				$ARGV[ $i + 1 ] =~ /^-/		# 다음 인자 역시 옵션을 뜻할 때는
			) {
				# 적절하지 않은 인자이다.
				print STDERR 'EDITRIB: Error: ' . $argv . ' 다음에는 특수한 "job chunk"의 이름이 주어져야 한다' . $/;

				return 0;
			}

			my	$byArtKeyNameTmp = $ARGV[ ++$i ];
			# 첫 글자는 알파벳이어야 하고,
			# 나머지는 숫자를 포함한 알파벳이어야 한다.
			unless ( $byArtKeyNameTmp =~ /^[[:alpha:]][[:alnum:]]*$/ ) {
				print STDERR 'EDITRIB: Error: "-k": ' . '{' . $byArtKeyNameTmp . '}' . ': 첫 글자는 알파벳이어야 하고, 나머지는 숫자를 포함한 알파벳이여야 한다' . $/;
				return 0;
			}
			if ( $byArtKeyName ) {
				print STDERR 'EDITRIB: Info: "-k": ' . '{' . $byArtKeyName . '}' . ': 전에 주어진 값은 무시된다' . $/;
			}
			$byArtKeyName = $byArtKeyNameTmp;
			next;
		}

		# 이 외의 옵션을 무시된다.
		if ( $argv =~ /^-/ ) {
			print STDERR 'EDITRIB: Warning: |' . $argv . '|: 적절하지 않은 옵션이다: 무시한다' . $/;

			# 다음 인자를 해석한다.
			next;
		}


		# 원본 RIB의 이름이 주어져야 한다.
		unless ( -f $argv ) {
			print STDERR 'EDITRIB: Error: ' . $argv . ': |' . $argv . '|: 파일이 아니다(!-f)' . $/;
			return 0;
		}
		if ( $ribFile ) {
			print STDERR 'EDITRIB: Warning: RIB: 전에 주어진 파일(|' . $ribFile . '|)은 무시된다' . $/;
		}
		$ribFile = $argv;
	}

	# RIB 파일을 지정해야 한다.
	unless ( $ribFile ) {
		print STDERR 'EDITRIB: Error: RIB 파일을 지정해야 한다' . $/;
		return 0;
	}
	$ribFile =~ s|\\|/|g;	# (있다면) WINDOWS경로를 UNIX경로로 바꾼다.
	$hCommandLineOptionRef -> { 'ribFile' } = $ribFile;

	# "jcf"가 주어졌다면 이를 설정한다.
	$hCommandLineOptionRef -> { 'jcFile' } = $jcFile if $jcFile;

	# 추가경로 표현이 주어졌다면 기본값을 대신한다.
	$hCommandLineOptionRef -> { 'addPathExpr' } = $addPathExpr if $addPathExpr;
	$hCommandLineOptionRef -> { 'rootPathExpr' } = $rootPathExpr if $rootPathExpr;
	$hCommandLineOptionRef -> { 'byArtKeyName' } = $byArtKeyName if $byArtKeyName;

	return 1;
}



1;
