use ERModule;

use strict;


# 읽어들인 RIB Directive가 유효한 지를 알아볼 때, defined대신에 exists를 사용한다.
# RIB Directive가 %hRIBDirective의 키(key)로 정의되어있기만 하면 exists는 TRUE이다.
# 하지만 defined가 TRUE가 되기위해서는 값(value)도 정의되어 있어야 한다.
#
#	2007/6/22 (KST)
#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)


# %hRIBDirective에서 각 RIB Directive에 해당하는 값을 ENUM_RIB_DIRECTIVE__XXX로 변경한다.
#
#	2007/6/21 (KST)
#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)


# 첫 번째 줄에 유효한 "RIB Directive"가 올 때, 제대로 처리하지 못하는 버그를 수정한다.
#
#	2007/5/3 (EST)
#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)

# 유효한 "RIB Directive"에 "HierarchicalSubdivisionMesh"를 추가한다.
# Application Note #41에서 "HierarchicalSubdivisionMesh"에 대한 설명이 있다.
# 다음 예제파일에 사용 예가 있다.
#	rattutorials/mtor/scenes/subsurface_scattering/sssc.ma
#
#	2007/4/26 (EST)
#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)

#### RIB파일에서 "RIB Directive"단위로 정보를 읽는 함수이다.
#### 다음과 같은 가정을 한다.
####	1. 모든 정보는 줄단위로 들어있다.
####	2. "RIB Directive"의 내용은 여러 줄에 걸쳐 있을 수 있으나
####	3. 한 줄에 두 "RIB Directive"의 내용이 동시에 들어갈 수 없다.
#### 이는 Token에 따라 RIB을 분석하는 prman과는 다르다.


# 렌더맨 설명서에서
#	Pixar's RenderMan > Quick Reference > RI Procedures
# 의 "RPS(RenderMan Pro Server 12.5) Quick Reference: RIB Directives"에 있는
# RIB Directive들을 모든 가능한 것들로 받아들인다.
# IfBegin이 말해주듯 "The RenderMan Interface Specification DRAFT (Version 3.3)"을 기준으로 한다.
my %hRIBDirective = (
	# Application Note #41에서 "HierarchicalSubdivisionMesh"에 대한 설명이 있다.
	# 다음 예제 파일에 사용 예가 있다.
	#	rattutorials/mtor/scenes/subsurface_scattering/sssc.ma
	#
	#	2007/4/26 (EST)
	#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)
	'HierarchicalSubdivisionMesh'	=> ERModule::ER__ENUM_RIB_DIRECTIVE__GEOMETRIC_PRIMITIVE,

	'ArchiveBegin'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'ArchiveEnd'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'ArchiveRecord'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'AreaLightSource'		=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'Atmosphere'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'Attribute'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__ATTRIBUTE,
	'AttributeBegin'		=> ERModule::ER__ENUM_RIB_DIRECTIVE__ATTRIBUTE_BEGIN,
	'AttributeEnd'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__ATTRIBUTE_END,
	'Basis'				=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'Begin'				=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'Blobby'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__GEOMETRIC_PRIMITIVE,
	'Bound'				=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'Clipping'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'ClippingPlane'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'Color'				=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'ColorSamples'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'ConcatTransform'		=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'Cone'				=> ERModule::ER__ENUM_RIB_DIRECTIVE__GEOMETRIC_PRIMITIVE,
	'Context'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'CoordinateSystem'		=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'CoordSysTransform'		=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'CropWindow'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'Curves'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__GEOMETRIC_PRIMITIVE,
	'Cylinder'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__GEOMETRIC_PRIMITIVE,
	'Declare'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'DepthOfField'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'Detail'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'DetailRange'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'Disk'				=> ERModule::ER__ENUM_RIB_DIRECTIVE__GEOMETRIC_PRIMITIVE,
	'Displacement'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'Display'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__DISPLAY,
	'DisplayChannel'		=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'Else'				=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'ElseIf'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'End'				=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'ErrorHandler'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'Exposure'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'Exterior'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'Format'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'FrameAspectRatio'		=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'FrameBegin'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__FRAME_BEGIN,
	'FrameEnd'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__FRAME_END,
	'GeneralPolygon'		=> ERModule::ER__ENUM_RIB_DIRECTIVE__GEOMETRIC_PRIMITIVE,
	'GeometricApproximation'	=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'Geometry'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__GEOMETRIC_PRIMITIVE,
	'GetContext'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'Hider'				=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'Hyperboloid'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__GEOMETRIC_PRIMITIVE,
	'Identity'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'IfBegin'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'IfEnd'				=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'Illuminate'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'Imager'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'Interior'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'LightSource'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__LIGHTSOURCE,
	'MakeCubeFaceEnvironment'	=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'MakeLatLongEnvironment'	=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'MakeShadow'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'MakeTexture'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'Matte'				=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'MotionBegin'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'MotionEnd'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'NuPatch'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__GEOMETRIC_PRIMITIVE,
	'ObjectBegin'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'ObjectEnd'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'ObjectInstance'		=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'Opacity'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'Option'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'Orientation'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'Paraboloid'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__GEOMETRIC_PRIMITIVE,
	'Patch'				=> ERModule::ER__ENUM_RIB_DIRECTIVE__GEOMETRIC_PRIMITIVE,
	'PatchMesh'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__GEOMETRIC_PRIMITIVE,
	'Perspective'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'PixelFilter'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'PixelSamples'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'PixelVariance'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'Points'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__GEOMETRIC_PRIMITIVE,
	'PointsGeneralPolygons'		=> ERModule::ER__ENUM_RIB_DIRECTIVE__GEOMETRIC_PRIMITIVE,
	'PointsPolygons'		=> ERModule::ER__ENUM_RIB_DIRECTIVE__GEOMETRIC_PRIMITIVE,
	'Polygon'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__GEOMETRIC_PRIMITIVE,
	'Procedural'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__GEOMETRIC_PRIMITIVE,
	'Projection'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'Quantize'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'ReadArchive'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'RelativeDetail'		=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'Resource'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'ResourceBegin'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'ResourceEnd'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'ReverseOrientation'		=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'Rotate'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'Scale'				=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'ScopedCoordinateSystem'	=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'ScreenWindow'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'ShadingInterpolation'		=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'ShadingRate'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'Shutter'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'Sides'				=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'Skew'				=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'SolidBegin'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'SolidEnd'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'Sphere'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__GEOMETRIC_PRIMITIVE,
	'SubdivisionMesh'		=> ERModule::ER__ENUM_RIB_DIRECTIVE__GEOMETRIC_PRIMITIVE,
	'Surface'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__SURFACE,
	'System'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'TextureCoordinates'		=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'Torus'				=> ERModule::ER__ENUM_RIB_DIRECTIVE__GEOMETRIC_PRIMITIVE,
	'Transform'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'TransformBegin'		=> ERModule::ER__ENUM_RIB_DIRECTIVE__TRANSFORM_BEGIN,
	'TransformEnd'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__TRANSFORM_END,
	'TransformPoints'		=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'Translate'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE,
	'TrimCurve'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__GEOMETRIC_PRIMITIVE,
	'WorldBegin'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__WORLD_BEGIN,
	'WorldEnd'			=> ERModule::ER__ENUM_RIB_DIRECTIVE__WORLD_END,
);

my	$RIBDirective;		# 현재 진행 중인 "RIB Directive"이다.
my	$enumRIBDirective;	# RIB Directive에 해당하는 enum값
my	@RIBDirectiveContent;	# 해당하는 "RIB Directive"내용을 담는다.
				# 여러 줄에 걸쳐있을 수 있다.
#print STDERR \@RIBDirectiveContent . $/;	# @RIBDirectiveContent변수의 메모리 주소이다.

# 아래 함수는 완전한 하나의 "RIB Directive"내용을 알려준다.
# 이 함수가 리턴하는 다음 두 경우가 있다. (리턴값 = 1)
#	1. 새로운 "RIB Directive"에 해당하는 줄을 읽어들였을 때
#	2. 더 이상 읽어들일 줄이 없을 때
# 이 때, 이전에 모은 "RIB Directive"의 내용도 함께 리턴한다.
# 1.의 경우, 다음에 이 함수를 호출할 때
# 최근에 읽어들인 줄이 새로운 "RIB Directive"내용의 시작이 된다.
my	$lastRIBDirective;
my	$lastEnumRIBDirective;
my	$lastRIBDirectiveLine;	# 가장 최근에 읽어들인 줄
# 2.의 경우 다음 번 호출에서 리턴값이 0이다.
# 문제가 있다면 리턴값은 -1이다. 즉 RIB파일은 의미있는 것이 아니므로
# 이전에 사용했던 "RIB Directive"내용들은 모두 무효로 한다.
# 아니면 해당 줄은 무시할 수도 있다.
# 예외의 경우가 있다. (리턴값 = 2)
# 읽어들인 줄이 빈줄이거나 주석줄또는 version정보를 담은 줄인 경우이다. ([경우 2.2.1])
sub getLineByRIBDirective
{
	my	$ribFileHandle = shift;

	# "RIB Directive"의 내용을 초기화한다.
	$RIBDirective		= '';
	$enumRIBDirective	= ERModule::ER__ENUM_RIB_DIRECTIVE__DONT_CARE;
	@RIBDirectiveContent	= ();

	# 최근에 읽어들인, 새로운 "RIB Directive"에 해당하는 줄이 있다면
	# 거기서부터 출발한다.
	if ($lastRIBDirective) {
		$RIBDirective = $lastRIBDirective;
		$enumRIBDirective = $lastEnumRIBDirective;
		push(@RIBDirectiveContent, $lastRIBDirectiveLine);
	}

	# 한 줄씩 읽어들인다.
	while (<$ribFileHandle>) {
		# 한 줄을 읽어 공백이 아닌 문자열을 읽는다.
		/^\s*(\S+)/;
		# 크게 두 가지 경우가 있다.
		#	[경우 1] 공백이 아닌 문자열이 존재하고(즉 아무것도 없는 줄이 아닌 경우),
		#	그 문자열이 "RIB Directive"인 경우와
		#	[경우 2] 그렇지 않은 경우
		# 2.의 경우에는 다시 두 경우로 나뉜다.
		#	[경우 2.1] "RIB Directive"의 내용의 일부이거나
		#	[경우 2.2] 그렇지 않은 경우
		# 2.2.는 다시 두 경우로 나뉜다.
		#	[경우 2.2.1] 특별한 경우(빈 줄, 주석줄, version정보를 담은 줄)
		#	[경우 2.2.2] 처리할 수 없는 경우
		my	$Token = $1;

		if (defined($Token) && exists($hRIBDirective{ $Token })) {
			# [경우 1]: 새로운 "RIB Directive"의 시작이다.
			#
			# 따라서 새로운 "RIB Directive"내용을 모으기 시작한다.
			# 이전에 모은 "RIB Directive"내용이 있다면 이를 먼저 처리한다.
			# 즉 리턴한다.

			if (@RIBDirectiveContent) {	# 이전에 모은 "RIB Directive"내용이 있다면
				# 그것을 리턴한다.
				# 이 때, 방금 읽어들인 새로운 "RIB Directive"에 대한 정보는
				# 다음에 이 함수를 호출할 때 사용하도록 저장된다.
				$lastRIBDirective = $Token;
				$lastEnumRIBDirective = $hRIBDirective{ $Token };
				$lastRIBDirectiveLine = $_;
				# 다음에 이 함수를 호출하면
				# 방금 읽어들인 새로운 "RIB Directive"에서부터 시작한다.
				return (ERModule::ER__GET_RIB_DIRECTIVE__VALID, $enumRIBDirective, $RIBDirective, \@RIBDirectiveContent);

			}

			# 현재 읽은 줄은 새로운 "RIB Directive"의 시작이 된다.
			$RIBDirective = $Token;		# "RIB Directive"를 기억해야 한다!
							# 그렇지 않으면
							# 첫 번째 줄에 있는 유효한 "RIB Directive"를 잃는다!
							#	2007/5/3 (EST)
							#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)
			$enumRIBDirective = $hRIBDirective{ $Token };
			push(@RIBDirectiveContent, $_);
		} else {
			# [경우 2]: "RIB Directive"를 포함하는 줄이 아니다.
			#
			# "RIB Directive"내용의 일부이거나([경우 2.1])
			# 예외적인 경우([경우 2.2])이다.

			if (@RIBDirectiveContent) {
				# [경우 2.1]: "RIB Directive"내용의 일부이다.
				push(@RIBDirectiveContent, $_);
			} else {
				# [경우 2.2]: 현재 줄은 "RIB Directive"를 포함한 줄([경우 1])도 아니고,
				# "RIB Directive"내용의 일부([경우 2.1])도 아니다.
				#
				# 예외의 경우([경우 2.2.1]: 빈줄, 주석줄, version정보를 담은 줄)이거나
				# 처리할 수 없는 정보를 담은 경우([경우 2.2.2])이다.

				if (
					/^\s*$/ ||	# 빈줄
					/^\s*#/ ||	# 주석줄
					/^\s*version/	# version정보를 담은 줄
				) {
					# [경우 2.2.1]: 예외의 경우이다.
					# 즉 빈줄, 주석줄이나, version정보를 담은 줄은 있는 그대로 둔다.
					# "RIB Directive"내용은 아니지만
					# 계속 진행해도 무리는 없다.
					$lastRIBDirective	= undef;
					$lastEnumRIBDirective	= undef;
					$lastRIBDirectiveLine	= undef;
					return (ERModule::ER__GET_RIB_DIRECTIVE__EXCEPTION, undef, undef, [ $_ ]);
				} else {
					# [경우 2.2.2]: 알 수 없는 줄이다.
					# 더 이상 진행하면 안 된다.
					# 아니면 무시할 수도 있다.
					$lastRIBDirective	= undef;
					$lastEnumRIBDirective	= undef;
					$lastRIBDirectiveLine	= undef;
					return (ERModule::ER__GET_RIB_DIRECTIVE__UNKNOWN, undef, undef, [ $_ ]);
				}
			}
		}
	}
	if (@RIBDirectiveContent) {	# 이전에 모은 "RIB Directive"내용이 있다면
		# 그것을 리턴한다.
		$lastRIBDirective	= undef;
		$lastEnumRIBDirective	= undef;
		$lastRIBDirectiveLine	= undef;
		return (ERModule::ER__GET_RIB_DIRECTIVE__VALID, $enumRIBDirective, $RIBDirective, \@RIBDirectiveContent);

	}
	return (ERModule::ER__GET_RIB_DIRECTIVE__EOF, undef, undef, undef);
}

# The file must return true as the last statement to indicate successful execution of any ini-
# tialization code, so it's customary to end such a file with "1;" unless you're sure it'll
# return true otherwise.  But it's better just to put the "1;", in case you add more state-
# ments.
1;
