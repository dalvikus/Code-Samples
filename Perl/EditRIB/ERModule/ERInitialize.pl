use ERModule;

use strict;


# 1. RIB파일을 읽어 "파일 핸들"을 기억한다.
# 2. "jcf", "sif", "rbif" 파일을 읽어, 다음 4 전역변수를 설정한다.
# 궁극적으로는 "job chunk"에 정보를 설정하는 것이다.
#	$ghVar{ "command line option" }이 가리키는 해쉬구조의
#	"aRBFile"/"aSIFile"을 사용해서 %HoARIBBoxInfo/@AoHShaderInfoByArt를 설정한다.
#	%HoARIBBoxInfo, @AoHShaderInfoByArt/@AoHShaderInfoByTd를 이용하여,
#	%HoHChunk의 해당 키("rbref", "siref")를 설정한다.
#	몰론 해당 "job chunk"만의 키를, 이러한 전역변수를 이용하지 않고 따로 설정할 수도 있다.
#
#	2007/6/15 (KST)
#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)

%ERModule::HoARIBBoxInfo	= ();
@ERModule::AoHShaderInfoByArt	= ();
@ERmodule::AoHShaderInfoByTd	= ();
%ERModule::HoHChunk		= ();
sub Initialize
{
	my	$hCommandLineOptionRef = $ERModule::ghVar{ "command line option" };

	my	$aSIFileRef = $hCommandLineOptionRef -> { 'aSIFile' };
	if ( @{ $aSIFileRef } ) {
		# ART부서가 설정하는 쉐이더 정보를 담은 전역변수(@AoHShaderInfoByArt)를 설정한다.
		# 마지막에 설정된 파일이 우선한다. (LIFO)
		for ( my $i = $#{ $aSIFileRef }; $i >= 0; --$i ) {
			my	$SIFile = $aSIFileRef -> [ $i ];
			my	$aRef = require $SIFile;
			if ( ref( $aRef ) ne 'ARRAY' ) {
				print STDERR 'EDITRIB: Warning: ' . '{' . $SIFile . '}' . ': Not "SI" Format: Ignored' . $/;
				next;
			}
			for (my $iElement = 0; $iElement < @{ $aRef }; ++$iElement) {
				my	$hRef = ${ $aRef }[ $iElement ];
				if ( ref( $hRef ) ne 'HASH' ) {
					print STDERR 'EDITRIB: Warning: ' . '{' . $SIFile . '}' . ': ' . '[ ' . $iElement . ' ]' . ': Not HASH: Ignored' . $/;
					next;
				}

				my	$contentRef = \$hRef->{ 'content' };
				# print 'content => |' . $$contentRef . '|' . $/;
				$$contentRef =~ s/(Surface|Displacement|LightSource)\s*"\s*(.*?)\s*"/$1 . ' "' . ( $2 =~ m|^-(.*)| ? $1 : $ERModule::SHADER_PATH ? $ERModule::SHADER_PATH . '\/' . $2 : $2 ) . '"'/seg;
				# print 'content => |' . $$contentRef . '|' . $/;
				$$contentRef =~ s/"\s*([^"]*?tex)\s*"/'"' . ( $1 =~ m|^-(.*)| ? $1 : $ERModule::TEXTURE_PATH ? $ERModule::TEXTURE_PATH . '\/' . $1 : $1 ) . '"'/seg;
				# print 'content => |' . $$contentRef . '|' . $/;

				push( @ERModule::AoHShaderInfoByArt, $hRef );
			}
		}
	} else {
		# -sif가 주어지지 않았을 때는,
		# 어떤 쉐이더도 변경하지 않는다.
		# 즉 @AoHShaderInfoByArt = ();
	}

	my	$aRBFileRef = $hCommandLineOptionRef -> { 'aRBFile' };
	if ( @{ $aRBFileRef } ) {
		# 마지막에 설정된 파일이 우선한다. (LIFO)
		for ( my $i = $#{ $aRBFileRef }; $i >= 0; --$i ) {
			my	$rbFile = $aRBFileRef -> [ $i ];
			my	$hRef = require $rbFile;
			if ( ref( $hRef ) ne 'HASH' ) {
				print STDERR 'EDITRIB: Error: RIBBOX: ' . $rbFile . ': Not "RIB Box Info File" Format: Ignored' . $/;
				next;
			}
			foreach my $key ( keys( %{ $hRef } ) ) {
				my	$val = $hRef->{ $key };
				my	$valType = ref( $val );

				if ( $key == ERModule::ER__ENUM_RIB_BOX__DECLARE__JUST_BEFORE_FIRST_FRAME_BEGIN ) {
					if ( $valType ) {
						print STDERR 'EDITRIB: Error: RIBBOX: Key ID = ' . $key . ': SCALAR required: Ignored' . $/;
						next;
					}
					$ERModule::HoARIBBoxInfo{ $key } .= "\r\n" . $val;
					next;
				}
				if (
					# CONTEXT에서 ELEMENT가 정의되어야 한다.
					#	정의되지 않았다면 모든 ELEMENT에서 적용된다고 가정한다.
					$key != ERModule::ER__ENUM_RIB_BOX__FRAME__JUST_BEFORE_DISPLAY &&
					$key != ERModule::ER__ENUM_RIB_BOX__FRAME__JUST_BEFORE_WORLD_BEGIN &&
					$key != ERModule::ER__ENUM_RIB_BOX__WORLD__JUST_BEFORE_FIRST_LIGHTSOURCE_BLOCK &&
					$key != ERModule::ER__ENUM_RIB_BOX__WORLD__JUST_BEFORE_FIRST_GEOMETRIC_PRIMITIVE_BLOCK &&
					$key != ERModule::ER__ENUM_RIB_BOX__WORLD__JUST_AFTER_LAST_OBJECT_BLOCK &&
					# CONTEXT에서 (OBJECT, ELEMENT)가 정의되어야 한다.
					#	하나만 정의되었을 때, 정의된 OBJECT가 모든 ELEMENT에서 적용된다고 가정한다.
					#	정의되지 않았다면, 모든 ELEMENT에서 모든 OBJECT에서 적용된다고 가정한다.
					$key != ERModule::ER__ENUM_RIB_BOX__LIGHT__JUST_AFTER_ATTRIBUTE &&
					$key != ERModule::ER__ENUM_RIB_BOX__OBJECT__JUST_AFTER_ATTRIBUTE &&
					$key != ERModule::ER__ENUM_RIB_BOX__OBJECT__JUST_BEFORE_LIGHT_SHADER &&
					$key != ERModule::ER__ENUM_RIB_BOX__OBJECT__JUST_BEFORE_SURFACE_SHADER &&
					$key != ERModule::ER__ENUM_RIB_BOX__OBJECT__JUST_BEFORE_GEOMETRIC_PRIMITIVE
				) {
					print STDERR 'EDITRIB: Error: RIBBOX: Unknown Key ID = ' . $key . ': Ignored' . $/;
					next;
				}
				if ( $valType ne 'ARRAY' ) {
					print STDERR 'EDITRIB: Error: RIBBOX: Key ID = ' . $key . ': ARRAY required: Ignored' . $/;
					next;
				}
				if ( @{ $val } == 0 ) {
					print STDERR 'EDITRIB: Warning: RIBBOX: Key ID = ' . $key . ': # of element = 0: Skipped' . $/;
					next;
				}
				my	$firstElement = $val-> [ 0 ];
				my	$firstElementType = ref( $firstElement );
				if ( $firstElementType && $firstElementType ne 'ARRAY' ) {
					print STDERR 'EDITRIB: Error: RIBBOX: Key ID = ' . $key . ': ARRAY or ARRAY of ARRAY required: Ignored' . $/;
					next;
				}
				if ( $firstElementType eq'ARRAY' ) {
					push( @{ $ERModule::HoARIBBoxInfo{ $key } }, @{ $val } );
				} else {
					push( @{ $ERModule::HoARIBBoxInfo{ $key } }, $val );
				}
			}
		}
	}

#{
#	# 쉐이더 정보를 적을 때, 자주 쓰는 값들은 전역변수로 선언한 후,
#	# 필요할 때마다 그 변수를 사용한다.
#	# 경우에 따라 전역변수의 변수 이름을 추가, 삭제, 변경이 가능해야 한다.
#	# 이 기능을 위해서 펄의 man perlref에 있는 "Symbolic references"를 사용하였다.
#	#	1. 변수의 이름과 값을 저장하는 해쉬를 만든다.
#	#	해쉬 자체의 내용이야 원하는 대로 바꿀 수 있으므로,
#	#	곧 원하는 대로 변수의 이름이나 값을 바꿀 수 있음을 뜻한다.
#	#	2. 주어진 해쉬의 모든 키값에 해당하는 변수를 선언한다.
#	#	바로 "symbolic reference"의 기능이다.
#	#	예를 들어, $name = "foo"값이 저장되어 있을 때,
#	#	다음과 같이 하면 $foo라는 변수가 생성되고 동시에 그 변수에 값을 할당할 수 있다.
#	#		$$name = 3		# $foo = 3
#	my	%hGlobal = (
#		'allKd'		=> '1.0',	# $allKd = 1.0;
#		'Kd'		=> '0.8',	# $Kd = 0.8;
#	);
#	for $key (keys(%hGlobal)) {
#		${$key} = $hGlobal{ $key };
#	}
#	# 이는 다음과 같이 해당 변수를 "직접" 선언하는 것보다
#	# 외부에서 어떠한 전역변수가 있는 지,
#	# 그리고 때에 따라 전역변수를 추가, 삭제, 변경이 쉽도록 한다.
#	# 외부에서는 %hGlobal이라는 해쉬만을 참고해서
#	# 전역변수에 대한 모든 것을 할 수 있다.
##	my	$allKd	= 1.0;		# 모든 오브젝트는 이 값을 사용하고,
##	my	$Kd = 0.8;		# 특정한 오브젝트만 다른 값을 사용한다.
#
#	my	$hShaderInfoRef = {
#		'set'		=> ".*",			# 모든 오브젝트의
#		'type'		=> ERModule::ER__SHADER_TYPE__SURFACE,	# Surface쉐이더를
#		'content'	=> qq/Surface "mtorLambert" "float diffuseCoeff" [ $allKd ]/
#								# 기본값($allKd)을 사용하는 쉐이더(mtorLambert)로 바꾼다.
#	};
#	unshift(@AoHShaderInfoByArt, $hShaderInfoRef);		# 처음이므로 push(@AoHShaderInfoByArt, $hShaderInfoRef)와 동일하다.
#	$hShaderInfoRef = {
#		'set'		=> "sphere",			# "sphere"라는 이름을 가진 특정 오브젝트(들)의
#		'type'		=> ERModule::ER__SHADER_TYPE__SURFACE,	# Surface쉐이더를
#		'content'	=> qq/Surface "mtorBlinn" "float diffuseCoeff" [ $Kd ]/
#								# 특정 쉐이더(mtorBlinn)로 바꾼다.
#								# 그 쉐이더에서 특정한 값($Kd)을 사용한다.
#	};
#	unshift(@AoHShaderInfoByArt, $hShaderInfoRef);	# 제일 앞에 들어가야 한다.
#}

	# 참고할 쉐이더에 대한 정보를 담은 배열을 선택한다.
	# [작업 0], [작업 1], [작업 2]의 경우에는 ART부서가 정한 쉐이더 정보를 사용한다.
	# [작업 3]의 경우에는 특별히 만든 쉐이더를 사용한다.
	# 자세한 내용은 [작업 3]에 대한 아래의 설명을 보라.
#	my	$lAoHShaderInfoRef = \@AoHShaderInfoByArt;

# 각 "작업 chunk"에 대한 정보를 따로 파일로 만들어서 사용한다.
# [작업 3]에서 필요한 정보이므로 그 파일에 넣어서 사용한다.
#	2006/11/30 (EST)
#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)
# TD가 사용하는 [작업 3]에 필요한 쉐이더이다.
#@AoHShaderInfoByTd = ();
	my	$hShaderInfoRef = {
		'set'		=> [ ".*" ],			# 모든 오브젝트의
		'type'		=> ERModule::ER__SHADER_TYPE__SURFACE,	# Surface쉐이더를
		'content'	=> qq/Surface "@{ [ $ERModule::SHADER_PATH ? $ERModule::SHADER_PATH . '\/' : 'rmanshader\/' ] }shaderShadowOfAByLOntoB"/
								# 특수한 쉐이더로 바꾼다.
								# 이 쉐이더의 유일한 파라미터는 "shadowTexture"이다.
								# 그러면 뒤에다 ' "string shadowTexture" [ "..." ]'라고 하면 될 것이다.
								# 단, '...'부분은 적당한 쉐도우 맵으로 치환한 후.
								# 그러나 이러한 맵이 여러 개 있을 수 있다.
								# 그것을 아래에서 결정한 후,
								# ' "string[#] shadowTexture" [ "..." "..." ... "..." ]'처럼 만들어 이 뒤에다 붙인다.
								# 여기서 '#'는 맵의 갯수이다.
	};
	push(@ERModule::AoHShaderInfoByTd, $hShaderInfoRef);


#%HoHChunk = ();
	my	$jcFile = $hCommandLineOptionRef -> { 'jcFile' };
	if ( $jcFile ) {
		require $jcFile;	# "작업 chunk"에 해당하는 전역변수(%HoHChunk)를 설정한다.
	} else {
		$ERModule::HoHChunk{ $hCommandLineOptionRef -> { 'byArtKeyName' } } = {
			'type'	=> ERModule::ER__CHUNK_TYPE__RS,
			'rbref'	=> \%ERModule::HoARIBBoxInfo,
			'siref'	=> \@ERModule::AoHShaderInfoByArt,
			'llref' => [
				{
					'light set'	=> [ '.*' ],
					'object set'	=> [ '.*' ]
				}
			],
			'jcref'	=> {
				'primary'	=> [],
				'secondary'	=> {}
			},
		};
	}
}


1;
