#!/usr/bin/perl -w

# 1. .sif나 .jcf에서 "집합"을 정의하는 부분을 .rbif처럼 다음과 같이 사용할 수 있도록 한다.
#
#	"a,b,...,-x,-y,..."
#
# 순서가 중요하다!
#
# 2. 펄 소스를 그대로 사용하지 않고 다음과 "bytecode"로 변환해서 사용한다.
#
#	perlcc -B -o editRIB editRIB.pl
#
# 여기서 사용하는 getLineByRIBDirectiveSub.pl도 "bytecode"로 변환한 후,
# 그것을 "require"를 통해 추가시키니 몇 개의 경고가 나타난다.
# 따라서 getLineByRIBDirectiveSub.pl(1.0.0-2)을 따루 두지 않고 여기에 포함시켰다.
#
#	2007/5/25 (EST)
#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)


# "RIB Box" 정보를 처리하는 방식를 달리한다.
# 지금까지는 각 "job chunk"를 정의한 $HoHChunk{ "JobChunkName" }의
# "rbref"라는 Key를 통해 "RIB Box" 정보를 얻었다.
# Sifc처럼 독립적인 파일(rbif: RIB Box Information File)에서 얻을 수도 있도록 한다.
#
# 전역변수 %HoARIBBoxInfo 라는 변수에 rbif에서 얻은 정보를 넣는다.
%HoARIBBoxInfo = ();
my	@aRBFile = ();
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


# 같은 $RIBDirectiveContentRef를 각 "job chunk"가 사용해야 한다!
# "job chunk"에서 필요에 따라 그 내용을 바꾸거나 삭제, 추가한다.
# 이렇게 되면 같은 $RIBDirectiveContentRef를 사용하므로,
# 다음 "job chunk"에서는 원본이 아닌 수정된 $RIBDirectiveContentRef를 사용한다.
# 이를 방지하기위해 특별한 $RIBDirective의 경우 즉 원본이 수정될 가능성이 있는
# $RIBDirective의 경우에는 그 내용을 미리 복사했다가
# 매 "job chunk"마다 복사본을 다시 원본으로 복사해서 사용한다.
# 대표적으로 "Display"와 "LightSource"나 "Surface"같은 쉐이더의 내용이 그러하다.
# 이런 경우는 원본의 크기가 별로 작지 않아서
# "원복 -> 복사 -> 다시 복사"의 과정을 거치더라도
# 크게 문제될 것은 없다.
#
# 문제는 "geometric primitive"에 해당하는 "RIB Directive"의 경우이다.
# (예를 들어, "geometric primitive" 바로 앞에 "RIB Box"을 넣는 경우)
# "Geometric primitive"에 해당하는 "RIB Directive"도 많은 것도 문제지만
# 그 크기도, 경우에 따라서 만만치 않다.
#
# 따라서 지금까지의 $RIBDirectiveContentRef를 처리하는 방식을 달리한다.
# 원본이 기리키는 배열의 각 성분의 형태는 SCALAR이다.
# 원본에서 수정되거나 추가, 삭제되는 부분에 해당하는 성분은
# 이와 달리 다음과 같은 두 성분을 가진 배열에 대한 참조라 한다.
#
#	[ CONTENT_STATE__XXX, CONTENT_SCALAR_REF ]
#
# 여기서 CONTENT_STATE__XXX는 다음과 같은 값을 가지고,
# 그 값에 따라 CONTENT_SCALAR_REF를 처리한다.
#
#	CONTENT_STATE__DELETE	원본에서 CONTENT_SCALAR부분이 삭제되는 경우
#	CONTENT_STATE__ADD	원본에 CONTENT_SCALAR부분이 추가되는 경우
#
$CONTENT_STATE__DELETE	= -1;
$CONTENT_STATE__ADD	= +1;
=head1 EXAMPLE
	# 2개의 성분을 가진 배열에 대한 참조를 만든다.
	my	$aref;
	push( @{ $aref }, "1\n" );
	push( @{ $aref }, "2\n" );
	# 같은 성분을 가진 배열에 대한 참조를 만든다.
	my	$bref;
	@{ $bref } = @{ $aref };
	print '-' x 80 . $/;
	# $aref가 가리키는 배열을 출력한다.
	printArrayRef( *STDOUT, $aref );
	print '-' x 80 . $/;
	# $aref가 참조하는 배열의 0번째 성분을 삭제하고
	deleteArrayRef( $aref, 0 );
	# 그 자리에 다른 내용을 넣는다.
	addArrayRef( $aref, 0, \"hello, world\n" );
	# 지금까지의 결과로 만들어진 배열을 출력한다.
	printArrayRef( *STDOUT, $aref );
	print '-' x 80 . $/;
	# $bref가 가리키는 배열에 $aref가 가리키는 배열을 추가한다. (APPEND)
	appendArrayRef( $bref, $aref );
	# $bref가 가리키는 배열을 출력한다.
	printArrayRef( *STDOUT, $bref );
	print '-' x 80 . $/;
	# $aref가 기리키는 배열을 원래대로 되돌리고
	restoreArrayRef( $aref );
	# 그 내용을 출력한 후 확인한다.
	printArrayRef( *STDOUT, $aref );
	print '-' x 80 . $/;
exit;
=cut
# 이렇게 새롭게 정의된 $RIBDirectiveContentRef에 대해서
# 다음 각 경우를 어떻게 할 것인지를 적어보면 다음과 같다.
#	1) 삭제하는 경우
#		my	$deleteElement = $RIBDirectiveContentRef->[ $i ];
#		splice( @$RIBDirectiveContentRef, $i, 1, '#' . $deleteElement );
#	2) 추가하는 경우
#		splice( @$RIBDirectiveContentRef, $i, 0, $addElement );
#	3) 출력하는 경우
#		print $FH @$RIBDirectiveContentRef;
#	4) 다른 배열의 뒤쪽에 추가하는 경우 (APPEND)
#		push( @{ $aDestRef }, @$RIBDirectiveContentRef );
# 내용을 고치는 경우는 "삭제"와 "추가"가 동시에 이루어진다.
# 1) i-번째 성분을 삭제하는 경우
#	my	$ithElementRef = \$RIBDirectiveContentRef->[ $i ];
#	splice( @{ $RIBDirectiveContentRef }, $i, 1, [ $CONTENT_STATE__DELETE, $ithElementRef ] );
sub deleteArrayRef
{
	my	( $aRef, $index ) = @_;

	splice( @{ $aRef }, $index, 1, [ $CONTENT_STATE__DELETE, $aRef->[ $index ] ] );
}
# 2) i-번째 $newElement를 추가하는 경우
#	splice( @{ $RIBDirectiveContentRef }, $i, 0, [ $CONTENT_STATE__ADD, \$newElement ] );
sub addArrayRef
{
	my	( $aRef, $index, $addElementRef ) = @_;

	splice( @{ $aRef }, $index, 0, [ $CONTENT_STATE__ADD, $$addElementRef ] );
}
# 3) 출력하는 경우
# 기존의 방식:
#	print $FH @$RIBDirectiveContentRef;
# 대신에 다음 함수를 이용한다.
sub printArrayRef
{
	my	( $FH, $aRef ) = @_;

	for ( my $i = 0; $i < @{ $aRef }; ++$i ) {
		# 문자열을 복사하는 대신에 "참조"를 통해 문자열을 처리한다.
		my	$lineRef = \$aRef->[ $i ];
		unless ( ref( $$lineRef ) ) {	# 유지된 경우
			# 그대로 출력한다.
			print $FH $$lineRef;
		} else {			# 원본이 바뀐 경우
			if ( $$lineRef->[ 0 ] == $CONTENT_STATE__DELETE ) {	# 삭제하는 경우
				# '#'를 넣어 주석처리만 한다.
				print $FH '#' . $$lineRef->[ 1 ];
			} else {						# 추가하는 경우
				# 추가된 내용을 출력한다.
				print $FH $$lineRef->[ 1 ];
			}
		}
	}
}
# 4) 다른 배열의 뒤쪽에 추가하는 경우 (APPEND)
sub appendArrayRef
{
	my	( $aDestRef, $aSrcRef ) = @_;

	for ( my $i = 0; $i < @{ $aSrcRef }; ++$i ) {
		# 문자열을 복사하는 대신에 "참조"를 통해 문자열을 처리한다.
		my	$lineRef = \$aSrcRef->[ $i ];
		unless ( ref( $$lineRef ) ) {		# 유지된 경우
			# 그대로 추가한다.
			push( @{ $aDestRef }, $$lineRef );
		} else {				# 원본이 바뀐 경우
			if ( $$lineRef->[ 0 ] == $CONTENT_STATE__DELETE ) {	# 삭제하는 경우
				# '#'를 넣어 주석처리를 한 후 추가한다.
				push( @{ $aDestRef }, '#' . $$lineRef->[ 1 ] );
			} else {						# 추가하는 경우
				# 추가된 내용을 추가한다.
				push( @{ $aDestRef }, $$lineRef->[ 1 ] );
			}
		}
	}
}
#
# 한편 매 "job chunk"마다 다음 함수를 사용하여 원본을 복구한다.
sub restoreArrayRef
{
	my	$aRef = shift;

	# 지우는 경우가 있으므로 끝에서부터 처리한다.
	for ( my $i = $#{ $aRef }; $i >= 0; --$i ) {
		# 문자열을 복사하는 대신에 "참조"를 통해 문자열을 처리한다.
		my	$lineRef = \$aRef->[ $i ];
		next unless ( ref( $$lineRef ) );			# 유지된 경우에는 그대로 둔다.
		# i-번째 성분은 추가되었거나 삭제되었다.
		if ( $$lineRef->[ 0 ] == $CONTENT_STATE__DELETE ) {	# 삭제된 경우
			# 원래대로 복구한다.
			$$lineRef = $$lineRef->[ 1 ];
		} else {						# 추가된 경우
			# 이 성분을 없앤다.
			splice( @{ $aRef }, $i, 1 );
		}
	}
}
#
# %hSaveRIBDirectiveContent에 Key로써 정의된 몇 개의 "RIB Directive"는 지금처럼
#
#	각 "job chunk" 루프 전에 원본 저장 -> 매 "job chunk"마다 복사해서 사용
#
# 의 방식을 그대로 사용하고, 문제가 됐던 "geometric primitive"에 해당하는 "RIB Directive"의 경우에만
# 매 "job chunk"마다 restoreArrayRef를 사용한다.
# 다른 함수(add, delete, print, append)를 쓸 곳도 많지 않다. (다섯 군데! ...ArrayRef()함수가 사용된 곳을 보라.)
# 즉 "geometric primitive"를 다룰 때에만 해당 함수를 사용한다.
#
#	2007/5/24 (EST)
#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)


# 1. "렌더링 이미지"에 해당하는 ELEMENT이름을 ""대신에 "Image"라 한다.
# gElementName의 값이 바뀌므로 "쉐도우 이미지"와 "렌더링 이미지"를 구별하기 위한 조건문에서
#	if ( $gElementName ... )
# 대신에
#	if ( $gElementName ne 'Image' ... )
# 라고 해야 한다.
#
# 2. RIB Box를 구현한다.
=head1
my	$hAoARIBBoxRef = {
	# [유형 1 ] "Declare"부분 즉 첫 번째 "FrameBegin" 바로 앞에 들어갈 내용이다.
	$ENUM_RIB_BOX__DECLARE__JUST_BEFORE_FIRST_FRAME_BEGIN	=>
		# 단순한 문자열이어야 한다!
		#	1) 여러 줄을 넣고 싶다면 "."을 이용해서 연결한다.
		#	2) 각 줄은 "\r\n"으로 끝나야 한다.

		# 다음이 실제로 들어갈 내용이다.
		# 여러 줄을 넣기 위하여 "."을 사용했음을 강조하기 위해
		# "↓"로 표시된 열에 맞추어서 "."을 넣었다.
		########################################################################↓
		'# RIB Box: Just before first FrameBegin' . "\r\n"			.

		# 렌더링에 사용된 정보를 나타낸다.
		'Option "statistics" "endofframe" [ 0 ]' . "\r\n"			.	# 0, 1, 2, 3
		# Release 13부터 0, 1만 지원하고 자세한 정보는 XML형태로 출력된다.
		# 다음 두 옵션은 "endofframe"값이 1로 설정되었을 때만 의미가 있다.
		'Option "statistics" "filename" [ "filename.txt" ]' . "\r\n"		.	# "", "stdout"
		# "catrib"에서 "xmlfilename"앞에 "string"이 없으면 다음 에러가 발생한다.
		#	A08003 RIBParameterList: Unknown parameter identifier: xmlfilename
		# "catrib"이 "Release 13"을 따라오지 못하는 것 같다. (?)
		'Option "statistics" "string xmlfilename" [ "filename.xml" ]' . "\r\n"		.	# "", "stdout", "usefilename"

		# micropolygon정보를 담은 cache를 디스크에 쓸지 메모리에 쓸지를 말해주는 다음 내용
		'Hider "hidden" "mpcache" [0]' . "\r\n"						# 0: 메모리, 1: 디스크
		########################################################################


		# 성분의 끝을 말해주기 위해 ","를 사용한다.
		,


	# [유형 2 ] 다음 5종류의 "RIB Box" 경우에는
	# 적용될 "ELEMENT"를 지정할 수 있다.
	#
	#	$ENUM_RIB_BOX__FRAME__JUST_BEFORE_DISPLAY
	#	$ENUM_RIB_BOX__FRAME__JUST_BEFORE_WORLD_BEGIN
	#	$ENUM_RIB_BOX__WORLD__JUST_BEFORE_FIRST_LIGHTSOURCE_BLOCK
	#	$ENUM_RIB_BOX__WORLD__JUST_BEFORE_FIRST_GEOMETRIC_PRIMITIVE_BLOCK
	#	$ENUM_RIB_BOX__WORLD__JUST_AFTER_LAST_OBJECT_BLOCK
	#
	# "ELEMENT"란 subframe에 해당하는 말로써,
	# "쉐도우 맵"을 만드는 subframe의 경우에는 해당 "라이트"의 이름이,
	# "렌더링 이미지"를 만드는 subframe의 경우에는 "Image"란 이름이 사용된다.
	# 생략된다면 모든 "ELEMENT"에 적용된다.
	#	0) 어느 부분에 적용되는 지는 이름을 보고 "명확히" 알 수 있다. (있기를 바란다.)
	#	1) 바깥의 "["과 "]"가 반드시 필요하다.
	#	2) 같은 종류에서 "ELEMENT"에 따라 다른 "RIB Box"를 사용하고자 하는 경우에는,
	#	다시 "[", "]"을 사용한다.
	#
	# "ELEMENT"를 지정하는 형식은 다음과 같다.
	#	"a,b,...,-x,-y,..."
	#	1) ","로 구분된 각 문자열은, 적용되는 문자열과 일치하는 지 확인할 때 사용된다.
	#	2) "-"로 시작하는 문자열은, 적용되는 문자열이 일치하는 않는 지 확인할 때 사용된다.
	#	위의 경우에는 "a", "b"와 같은 문자열은 포함되어 있지만
	#	"x", "y"와 같은 문자열이 포함되지 않을 때에만 적용된다.
	#	3) 모든 문자열은 ".*"와 같은 형식을 사용한다. (perl의 regular expression에 사용되는 모든 표현이 가능)
	#	4) 특별한 예로 정확한 문자열을 지정하려면 문자열 시작과 끝에 각각 "^", "$"를 사용한다.
	#	예를 들어, "^a$"라는 형식은 정확히 "a"라는 "ELEMENT"에만 적용된다.
	#	이와 달리 "a"는 "a"라는 문자열이 포함된 "ELEMENT"에 적용된다.

	$ENUM_RIB_BOX__FRAME__JUST_BEFORE_DISPLAY		=> [
		# "ELEMENT"에 따라 서로 다른 두 개의 "RIB Box"가 사용된다.
		#
		# 1. "쉐도우 맵"을 만드는 subframe에 적용된다.
		[
			# 다음 "RIB Box"는...
			########################################################################
			'# RIB Box: Just before "Display" in NOT-Final image pass' . "\r\n",
			########################################################################

			# "Image"라는 "ELEMENT"를 제외한 모든 "ELEMENT" 즉
			# "쉐도우 맵"을 만드는 subframe에 적용된다.
			'.*,-Image'
		],
		#
		# 2. "렌더링 이미지"를 만드는 subframe에 적용된다.
		[
			# 다음 "RIB Box"는...
			'# RIB Box: Just before "Display" in FINAL image pass' . "\r\n",

			# "Image"라는 "ELEMENT" 즉 "렌더링 이미지"를 만드는 subframe에 적용된다.
			'Image'
		],
	],

	$ENUM_RIB_BOX__FRAME__JUST_BEFORE_WORLD_BEGIN				=> [
		# 하나의 "RIB Box"가 있다면 안쪽의 "[", "]"는 생략될 수 있다.

		# 다음 "RIB Box"가...
		########################################################################
		'# RIB Box: Just before "WorldBegin"' . "\r\n"
		########################################################################

		# "ELEMENT"를 적는 부분이 생략되었으므로 모든 "ELEMENT"에 적용된다.
	],

	# 첫 번째 "LightSource" 블럭은 "쉐도우 맵"에서는 존재하지 않는다. (왜?)
	# 오직 "Image"라는 "ELEMENT" 즉 "렌더링 이미지"에서만 의미가 있다.
	# 따라서 "쉐도우 맵"에 해당하는 "ELEMENT"를 지정해도 의미가 없다.
	$ENUM_RIB_BOX__WORLD__JUST_BEFORE_FIRST_LIGHTSOURCE_BLOCK		=> [
		'# RIB Box: Just before FIRST "LightSource" block' . "\r\n"
	],

	$ENUM_RIB_BOX__WORLD__JUST_BEFORE_FIRST_GEOMETRIC_PRIMITIVE_BLOCK	=> [
		'# RIB Box: Just before FIRST geometric primitive block' . "\r\n"
	],

	$ENUM_RIB_BOX__WORLD__JUST_AFTER_LAST_OBJECT_BLOCK			=> [
		'# RIB Box: Just after LAST object block' . "\r\n"
	],


	# [유형 3 ] 다음 5종류의 "RIB Box" 경우에는 "ELEMENT"이외에 "OBJECT"를 지정할 수 있다.
	#
	#	$ENUM_RIB_BOX__LIGHT__JUST_AFTER_ATTRIBUTE
	#		"LightSource" 블럭 안에서 라이트의 이름을 말해주는 "Attribute"바로 후
	#	$ENUM_RIB_BOX__OBJECT__JUST_AFTER_ATTRIBUTE
	#		Geometric primitive 블럭 안에서 오브젝트의 이름을 말해주는 "Attribute" 바로 후
	#	$ENUM_RIB_BOX__OBJECT__JUST_BEFORE_LIGHT_SHADER
	#		"LightSource" 블럭 안에서 light shader 바로 전
	#	$ENUM_RIB_BOX__OBJECT__JUST_BEFORE_SURFACE_SHADER
	#		Geometric primitive 블럭 안에서 surface shader 바로 전
	#	$ENUM_RIB_BOX__OBJECT__JUST_BEFORE_GEOMETRIC_PRIMITIVE
	#		Geometric primitive 블럭 안에서 해당 geometric primitive 정의 바로 전
	#
	# "OBJECT"란 말 그대로 "sphere", "plane", "keyLight"와 같은 오브젝트의 이름이다.
	# 규칙은 [유형 2]와 동일하다.
	# "OBJECT"를 지정하는 형식도 "ELEMENT"의 그것과 동일하다.
	# 단지 "ELEMENT"와 "OBJECT"를 모두 지정하려면 ":"를 사용해서 구분한다.
	# 즉 "ELEMENT:OBJECT"와 같은 형식이다.
	# "ELEMENT"가 생략되면 해당하는 오브젝트가 정의된 모든 "ELEMENT"에 적용된다.
	# "OBJECT"마저 생략되면 모든 "ELEMENT"의 모든 "OBJECT"에 적용된다.

	# "LightSource"와 관련된 부분이므로 "ELEMENT"가 "Image"일 때에만 의미가 있다!
	$ENUM_RIB_BOX__LIGHT__JUST_AFTER_ATTRIBUTE		=> [
		'# RIB Box: Just after Attribute in "LIghtSource" block' . "\r\n"
	],

	$ENUM_RIB_BOX__OBJECT__JUST_AFTER_ATTRIBUTE		=> [
		'# RIB Box: Just after Attribute in geometric primitive block' . "\r\n"
	],

	$ENUM_RIB_BOX__OBJECT__JUST_BEFORE_LIGHT_SHADER	=> [
		'# RIB Box: Just before "Light" shader in "LightSouce" block' . "\r\n"
	],

	$ENUM_RIB_BOX__OBJECT__JUST_BEFORE_SURFACE_SHADER	=> [
		'# RIB Box: Just before "Surface" shader in geometric primitive block' . "\r\n"
	],

	$ENUM_RIB_BOX__OBJECT__JUST_BEFORE_GEOMETRIC_PRIMITIVE	=> [
		'# RIB Box: Just before geometric primitive definition in geometric primitive block' . "\r\n"
	],
};

$HoHChunk{ 'BakeEx1' } = {
	'type'	=> $CHUNK_TYPE__RS | $CHUNK_TYPE__BAKE,
	'rbref'	=> $hAoARIBBoxRef,
	......
};
=cut
#
#	2007/5/24 (EST)
#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)


# 1. 반복해서 다음 형태가 나오는 버그를 수정한다.
#	Attribute "grouping" "string membership" ["+RManReflSubset"]
#	Attribute "grouping" "string membership" ["+RManRefrSubset"]
# "Ray tracing"이 설정되어 있음을 뜻하는 라인이 반복해서 사용되기 때문이다.
# 각 "job chunk"마다 원본 라인을 사용해야 한다.
#
# 2. RIB형식은 "ascii"로 선택할 수 있도록 한다.
# 기본 형식은 "binary" & "gzip"이다.
#	"-ascii"
$bAscii = 0;
#
# 3. 대상을 지정할 때 다음 형식을 허용한다.
#
#	"a,b,...,-x,-y,..."
#
# 즉 예전으로 말하면 "집합 표현 2'인 다음과 동등하다.
#
#	[ [ "a", "b", ... ], [ "x", "y", ... ] ]
#
# RIB Box에서 "(OBJECT, ELEMENT)" 모두를 지정하는 경우,
# ":"를 써서 "ELEMENT:OBJECT"처럼 지정할 수도 있다.
# 다음과 같이 함께 쓸 수도 있다.
#
#	"A,B,...,-X,-Y,...:a,b,...,-x,-y"
#
# 즉 "A", "B"를 포함하지만 "X", "Y"는 포함하지 않는 ELEMENT에서
# "a", "b"를 포함하지만 "x", "y"를 포함하지 않는 OBJECT에 해당한다.
#
#	주) ","나 ":" 전후의 공백은 없어야한다.
#	공백이 필요한 문자열인 경우에는 포함해야 하지만.
#
#
sub isInSet
{
	my	$element = shift;
	return 0 unless $element;
	my	$setExpr = shift;
	return 1 unless $setExpr;	# 집합표현이 주어지지 않았다면 항상 속한다고 가정한다.

	# 순서가 중요하다!
	# 제일 먼저 일치하는 곳에서 결정난다.
	for my $exprElement ( split( ',', $setExpr ) ) {
		if ( $exprElement =~ /^-/ ) {	# 속하지 않는 지를 알아본다.
			return 0 if "-$element" =~ /$exprElement/i;
		} else {			# 속하는 지를 알아본다.
			return 1 if "$element" =~ /$exprElement/i;
		}
	}
	# 어떤 것과도 일치하지 않으면 "속하지 않는다."
	return 0;

	# "-"에 속하지 않으면서, "-"가 없는 것에 속하는 지 알아본다.
	my	$bInSet = 0;	# 기본값은 "속하지 않는다"이다.
	for my $exprElement ( split( ',', $setExpr ) ) {
		if ( $exprElement =~ /^-/ ) {	# 속하지 않는 지를 알아본다.
			if ( $bInSet ) {	# 속했을 때에만 알아본다.
				$bInSet = 0 if "-$element" =~ /$exprElement/i;
			}
		} else {			# 속하는 지를 알아본다.
			unless ( $bInSet ) {	# 속하지 않았을 때만 알아본다.
				$bInSet = 1 if "$element" =~ /$exprElement/i;
			}
		}
	}
	return $bInSet;
}
#
#
#	2007/5/23 (EST)
#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)
#
#
# "Ray tracing"이 설정되어 있는 지를 판단하는 데 여러가지 방법이 있다.
#	Attribute "visibility" "int camera" [1] "int trace" [1] "int photon" [1] "string transmission" ["opaque"]
#	Attribute "trace" ...
#	Attribute "visibility" "int diffuse" [1]
#	Attribute "visibility" "int specular" [1]
#	......
# 나중 2개는 12.5부터 도입된 부분이다.
# 지금까지는 두 번째를 사용했는 데, 여기서는 첫 번째를 사용한다.
# mtor이 어떻게 RIB을 뽑아내느냐에 따라 달라질 수 있다.
sub isRaytracingAttributeLine
{
	my	$line = shift;

	my	$bRaytracing;
       	$bRaytracing = $line =~ /^\s*Attribute\s*"visibility".*"int\s+[diffuse|specular]"\s*\[\s*1\s*\]/s;
	return 1 if $bRaytracing;
       	$bRaytracing = $line =~ /^\s*Attribute\s*"visibility".*"int\s+trace"\s*\[\s*1\s*\]/s;
	return 1 if $bRaytracing;
	return $line =~ /^\s*Attribute\s*"trace"/;
}


# 참고하는 모든 데이터는 다음과 같이 해쉬 형태를 가진다.
#
#	%hData = (
#		'Version'	=> '#.#',
#		'Data'		=> ...,
#	);
#
# 1) 기존의 데이터는 "Data"가 가리키므로 반드시 있어야 한다.
# 2) "Data"를 해석하는 방법을 명시할 때는 "Version"정보를 넣는다.
# 버젼에 따라 "Data"를 해석하는 방법이 달라질 수 있기 때문이다.
# 3) 데이터를 참고하는 곳의 필요에 따라 더 많은 "Key"를 사용할 수도 있다.
# 4) 따라서 "require"를 통해 읽혀지는 파일의 제일 마지막은
# 이러한 데이터 형태에 대한 참조가 되어야 한다. (\%hData)
# 5) 이러한 데이터 형이 아니라면 "초기" 버젼(0.9)가 사용된다.
# 이러한 경우 새로운 버젼을 사용하라는 경고메시지를 출력한다.
# 6) "Version"정보가 없다면 "최신" 버젼을 사용하고,
# 이에 대한 경고메시지를 출력한다. 물론 "Data"에 정보가 있어야 한다.
# 7) 어떻게 "Data"를 정의하고 사용할 것인지는 전적으로 프로그래머의 몫이다!
# 8) "Data"에 대한 참조가 다시 이런 형태일 수 있다!
#
# 예 1) Sif의 경우
# 	기존의 방법
# 		my	@AoHSIF1 = (
#			{
#				......
#			},
#			......
# 		);
#
#		\@AoHSIF1;
#
#	새로운 방법
#		my	@AoHSIF1 = (
#			{
#				......
#			},
#			......
#		);	// 데이터를 정의하는 부분은 같다.
#
#		{
#			'Version'	=> 1.0,
#			'Data'		=> \@AoHSIF1,
#		};	// 해쉬에 대한 참조를 리턴한다.
#
# 아래의 getDataRef함수를 이용해야 한다!
#
#	2007/5/22 (EST)
#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)
#
#
=head1 EXAMPLE
# 다음은 예제이다.
# "Data.pl"의 내용이 다음과 같다고 하자.
################################################################################
########################################
my	@A = (
	{
		'type'	=> 1,
		'content'	=> 'hello, world'
	},
	{
		'type'	=> 1,
		'content'	=> 'Hello, World',
	}
);	# "적절하게" 정보를 정의한다.
########################################

########################################
{
	'Version'	=> '1.0',
	'Data'		=> \@A,
	'Priority'	=> "hello, world",
};	# 해쉬에 대한 참조로 그 정보를 전달한다.
########################################
################################################################################
=cut
# 다음은 "Data.pl"에서 제공하는 정보를 얻는 예이다.
=head1 EXAMPLE
################################################################################
my	$unknownRef = require( $ARGV[ 0 ] );
my	$optionKey = 'Priority';
my	$refToPriorityData;
my	( $dataVer, $dataRef ) = getDataRef( $unknownRef,
	'Priority', \$refToPriorityData,		# "참조"에 대한 "참조"!
);
print $dataVer . $/;
print $dataRef . $/;
print 'Priority Data: ' . ( defined( $refToPriorityData ) ? 'Ref Type:= "' . ref( $refToPriorityData ) . '", Data:= ' . $refToPriorityData : 'undef' ) . $/;
################################################################################
=cut
#
#
# 주어지는 새로운 형태의 데이터에 대한 참조로부터
# (버젼정보, 실제 데이터에 대한 참조)를 얻는다.
#
#	my	( $dateVer, $dateRef ) = getDataRef( $unknownRef );
#
# 새로운 데이터의 형태안에, 얻고자 하는 다른 정보가 있다면,
# 그 정보의 이름과 그 정보에 대한 참조을 담을 곳을 추가로 적는다.
#
#	my	$refToPriorityData;
#	my	( $dateVer, $dateRef ) = getDataRef( $unknownRef,
#			'Priority', \$refToPriorityData		# "참조"에 대한 "참조"!
#	);
#
# 주 1) 새로운 데이터 형태인 해쉬 구조안에 "Priority"라는 Key에 대한 값(항상 참조이어야 한다!)을
# $refToPriorityData가 가리키도록 해야 한다. (참조에 대한 참조임을 기억하라!)
# 주 2) 위처럼 항상 쌍(추가 정보에 해당하는 Key, 추가 정보에 대한 참조를 담을 참조)으로 주어져야 한다.
sub getDataRef
{
	my	$unknownRef = shift;
	return (undef, undef) unless $unknownRef;	# 새로운 데이터에 대한 참조가 주어지지 않았다면
							# 아무것도 얻을 수 없다.

	# 추가적으로 얻고자 하는 정보가 있는 지 알아본다.
	my	%hOption;
	for ( my $i = 0; $i < @_; $i += 2 ) {
		my	$optionKey = $_[ $i ];
		# 참조가 아니어야 한다.
		if ( ref( $optionKey ) ) {
			# 얻고자 하는 추가정보에 대한 Key이어야 한다.
			print STDERR 'EDITRIB: Error: ' . ( 1 + $i ) . '번째: 추가정보에 대한 Key이어야한다: 이후는 모두 무시된다' . $/;
			last;
		}
		if ( $i + 1 == @_ ) {
			# 추가정보에 대한 Key만 존재한다면 그 Key는 무시되고 경고메시지를 출력한다.
			print STDERR 'EDITRIB: Error: Data: Option Key:= "' . $optionKey . '": 정보를 담을 참조가 주어져야 한다: 무시된다' . $/;
			last;
		}

		$hOption{ $optionKey } = $_[ $i + 1 ];
	}

	my	$dataVer;
	my	$dataRef;
	if ( ref( $unknownRef ) eq 'HASH' ) {
		$dataRef = $unknownRef -> { 'Data' };
		if ( defined( $dataRef ) ) {
			# 적어도 "Data"라는 키를 가지고 있으므로 새로운 데이터 형태이다.
			# 버젼을 확인한다.
			$dataVer = $unknownRef -> { 'Version' };
			$dataVer = '2.0' unless defined( $dataVer );
		} else {
			print STDERR 'EDITRIB: Warning: Data: 데이터를 정의하는 새로운 방법을 사용하십시요' . $/;
			# 기존의 데이터라고 가정한다.
			$dataVer = '0.9';
			$dataRef = $unknownRef;
		}

		# 추가정보를 설정한다.
		foreach my $optionKey ( keys( %hOption ) ) {
			my	$refToOptionData = $unknownRef -> { $optionKey };
			if ( defined( $refToOptionData ) ) {
				${ $hOption{ $optionKey } } = $refToOptionData;
			} else {
				# 해당 Key에 대해서 설정되지 않았다.
				${ $hOption{ $optionKey } } = undef;
			}
		}
	} else {
		# 기존의 데이터라고 가정한다.
		print STDERR 'EDITRIB: Warning: Data: 데이터를 정의하는 새로운 방법을 사용하십시요' . $/;
		$dataVer = '0.9';	# "초기" 버젼
		$dataRef = $unknownRef;

		# 추가정보를 얻고자 했을 때, 모든 Key에 대한 참조는 의미없는 값을 가리킨다.
		foreach my $optionKey ( keys( %hOption ) ) {
			${ $hOption{ $optionKey } } = undef;
		}
	}

	return ( $dataVer, $dataRef );
}


# 이제부터 사용하는 RIB원본이나 수정된 RIB은 모두 "binary" & "gzip" 형태임을 가정한다.
# 지금까지는 "ascii"를 사용했다.
# RIB파일의 크기는 약 1/5정도로 줄어든다.
# 읽어들일 때는 "< RIBFile"대신에 "catrib RIBFILE |"을,
# 쓸 때는 "> RIBFile"대신에 "| catrib -binary -gzip -o RIBFile"을 사용했다.
# 즉 원본을 이 프로그램에서 처리하기 전에 "catrib"을 써서 "ascii"형태로 바꾸고,
# 여기서 처리한 "ascii"형태를 "catrib"이 다시 받아 "binary" & "gzip"형태로 바꾼다.
# 예전과 달리 RIB을 한 번만 읽어 모든 처리를 하기 때문에 가능하다.
#
#	주) "binary" & "gzip"형태의 RIB을 만들기위해 mtor에 다음 옵션을 사용한다.
#		-cmd rg RIBFormat binarygzip
#
#	2007/5/20 (EST)
#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)


# 1. CONTENT_SPOOL을 없앤다.
#
# fate에 대해서 다시 한 번 정리하자.
# 스택에 들어갈 때, "보모 스택"이 CONTENT_DELETE가 아니라면 CONTENT_SAVE로 바뀐다.
# (CONTENT_PRINT라 할지라도 Attribute "identifier"...를 만나 CONTENT_DELETE로 바뀔 수 있기 때문에)
# 스택을 나오면 "자식 스택"이 CONTENT_SAVE를 계속 유지해서 모은 내용이 있다면
# "부모 스택"의 "fate"에 따라 그 내용을 "부모 스택"에 추가할 지(fate = CONTENT_SAVE),
# 아니면 출력할 지(fate = CONTENT_PRINT)를 결정한다.
#
# "자식 스택"의 모든 내용을 모은다면(CONTENT_SAVE),
# 이 프로그램의 처리와 관련이 없는 "geometric primitives"정보 때문에(대부분의 내용을 차지한다)
# 사용하는 메모리의 양이 많다. 그래서 "자식 스택"의 도중에 가능하면 빨리
# CONTENT_PRINT로 바뀔 지 판단할 필요가 있었다. (굳이 할 필요는 없지만...)
# 그 때를 Attribute "identifier"...를 만났을 때로 결정했다.
# 여기서 오브젝트의 이름을 결정하고 "운명"이 CONTENT_PRINT나 CONTENT_DELETE로 바꿀 수 있다.
#
# 그런데 RIB Box를 구현하면서 위와 같이 CONTENT_PRINT로 바뀌면 문제가 있다.
# 첫 번째 LightSource 앞이나(ENUM_RIB_BOX__WORLD__JUST_BEFORE_FIRST_LIGHTSOURCE_BLOCK)
# 첫 번째 "geometric primitives"에 앞에(ENUM_RIB_BOX__WORLD__JUST_BEFORE_FIRST_GEOMETRIC_PRIMITIVE_BLOCK) 들어갈 내용은
# 더 이상 들어갈 자리가 없어진다. 이 내용은 해당 스택의 제일 앞부분에 들어가야 하기 때문이다.
#
# "운명"이 바뀔 지의 판단은 Attribute...에서 하고, 결정은 LightSource나 "geometric primitives"에서 하기 위해서
# CONTENT_SPOOL을 도입했다. 하지만 너무나 많은 곳에서 "지저분하게" 그 처리를 해야 하므로,
# Attribute...에서는 CONTENT_DELETE만 처리하고, CONTENT_PRINT로 바뀔 지는
# LightSource나 "geometric primitives"에서 결정하는 것으로 한다.
#
# 2. "Bake"에 사용되는 RIB의 이름을 각각 ShadowPass, BakePass, FinalPass라 한다.
#
# 3. 이 프로그램(editRIB)에서 바꾸는 부분에 다음과 같이 내용을 적는다.
# 	1) 한 줄 짜리
# 		## EDITRIB ## ......
# 	2) 여러 줄 짜리
# 		## EDITRIB ## <...> ####
# 		......
# 		## EDITRIB ## </...> ####
#
# 4. "Bake Pass"에서 나오는 이미지를 "null"로 만든다. 즉 "렌더링 이미지"를 만들지 않는다.
#	Display "null" "null" "z"
#
#	2007/5/6 (EST)
#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)


# 아래의 방법에 따라 "Subsurface Scattering"을 구현하는 rev. 1을 만든다.
# 1) 쉐이더는 예전 씬에 있는 "mushroom" 쉐이더를 변형해서 "Delux3"쉐이더로 만든다.
# 아래의 두 번째 방법을 사용해서 "Bake Pass"와 "Final Rendering Pass"를 구별해서
# 적절한(bake3d또는 texture3d) 일을 한다.
# "Bake Pass"의 "FrameEnd"다음에 "System" RIB Directive를 써서
# "ptfilter..."나 "brickmake..."를 실행한다.
# 2) 쉐이더 파라미터 중에는 "Final Rendering Pass"에서도 테스트할 수 있는 것들도 있지만,
# 많은 경우에는 "Bake Pass"부터 다시 해야한다.
# 하지만 "Shadow Pass"가 있다면 이는 생략해도 된다.
# (이를 하는 다른 방법을 생각해보아야 한다.)
# 1. "Bake"모드에서 원본 RIB을 다음과 같이 분리한다.
#	원본 RIB	= ("쉐도우 이미지 1" + "쉐도우 이미지 2" + ...") + ("렌더링 이미지")
#	Bake 모드 RIB	= "쉐도우 이미지 1" + "쉐도우 이미지 2" + ...
#			+ "Bake 이미지"
#			+ "렌더링 이미지"
# 3) CHUNK_TYPE__BAKE라는 플래그를 도입해서, "Bake"과정이 필요한 "job chunk"의
# 'type'에 '|'로 설정한다. 기존의 CHUNK_TYPE__SHD, CHUNK_TYPE__RS와는 별개의 플래그이다.
# 또한 CHUNK_TYPE__RS에서만 유효하다. (왜?)
# 시작 전에 CHUNK_TYPE__BAKE가 설정되어 있다면 이를 기억하고 곧 바로 해당 플래그를 떼어
# 나머지 코드에는 변함이 없도록 한다.
#
# 	2007/5/5 (EST)
# 	김광호 (Gwang-Ho Kim, iccentric@gmail.com)


# 즉 세 개의 파일로 분리한다.
# 주어진 "job chunk"가 "Bake"모드로 동작할 지는,
# "오브젝트"에 적어도 하나의, 유휴한 RS__SSSC플래그가 설정되어 있어야 한다.
# 불행히도 이는 RIB의 모든 내용을 읽기 전에는 판단할 수 없다.
# RIB의 렌더링 순서는 다음과 같다.
# 먼저 "쉐도우 이미지"를 얻는다.
# "Bake"모드가 "실제로" 설정되어 있다면 "Bake 이미지"를 만들면서
# 해당하는 오브젝트의 SSSC결과를 bake한다.
# "Bake"모드가 "실제로" 설정되지 않았다면 "Bake 이미지"에 해당하는 RIB을 렌더링하지 않는다.
# 어느 경우든 마지막에는 "렌더링 이미지"에 해당하는 RIB을 렌더링한다.
#
#	1) "Bake"모드에서 "쉐도우 이미지"에 해당하는 RIB만을 모으기 위해,
#		print $FH ...;
#	부분을
#		print $FH ... if $gElementName || !$bBake;
#	로 바꾼다.
#
#	2) 항상 subframe으로 나누지 않고, "Bake"노드에서 "렌더링 이미지"에 해당할 때만 따로 나눈다.
#		if ( $bBake && $gElementName eq '' ) {
#			......
#		}
#
#	3) "Bake"모드에서 "쉐도우 이미지", "Bake 이미지", "렌더링 이미지"에 사용되는 RIB의 이름은 각각
#	"Shadow", "Bake", "Image"라 한다. 즉 원본이 xxx.0001.rib이라면 각 RIB의 이름은
#	xxx.Shadow.0001.rib, xxx.Bake.0001.rib, xxx.Image.0001.rib이 된다.
#
#	4) "Bake 이미지"에서는 결과 이미지보다 쉐이더에서 만드는 "point cloud 파일"이 중요하므로,
#	"Bake 이미지"에서 "Display"는 "null"을 사용한다.
#
#		Display "null" "null" "z"
#
#	"Display"라는 "RIB Directive" 바로 다음에서 적절한 파일에 "Display"내용을 쓰므로,
#	"Bake 이미지"와 "렌더링 이미지"의 서로 다른 내용을 기억하고 있다고 쓰면 되었다.
#	하지만 정작 중요한 "쉐이더"의 파라미터로는 그렇게 할 수가 없다.
#	한참 뒤에 "쉐이더"의 내용을 쓰므로.
#
#	그래서...
#	두 가지 방법을 있다.
#	방법 1) 먼저, slim이 그렇게 하듯이, 바뀌는 부분을 보자.
#
#		i) 쉐이더 부분
#			Bake: Surface "......"
#				......
#				"string Context" [ "BAKE" ] "string PointCloudFile" [ ".../xxx.ptc" ]
#				"string BrickMapFile" [ "" ]		# 이 파라미터는 사용되지 않는다.
#			Image: Surface "......"
#				......
#				"string Context" [ "" ] "string PointCloudFile" [ "" ]	# 이 파라미터들은 사용되지 않는다.
#				"string BrickMapFile" [ ".../xxx.bkm" ]
#		ii) 속성 부분
#			......
#
#	@{ $RIBDirectiveContentRef }의 내용이 바뀌어야 한다면, 각각
#	$bakeSurfaceShaderRef, $bakeAttributeRef를 정의해서, 필요한 시점에서
#	@{ $RIBDirectiveContentRef }의 내용을 복사한 후 해당 내용만 바꾼다.
#	후에 $hStackRef->{ 'bake subframe content' }에 저장할 때,
#	$bake...Ref값이 정의되어 있다면 @{ $RIBDirectiveContentRef }대신에 @{ $bake...Ref }의 내용을 쓴다.
#
#		RS__RECV_SHADOW처럼 위와 같이 "쉐이더"에서 바뀔 부분만 적어준다.
#		이러한 인자를 적절하게 해석하는 것은 역시 쉐이더의 몫이다.
#
#	방법 2) "Bake 이미지"의 내용을 담은 쉐이더에서는 Point Cloud File을 만든다.
#	Point Cloud File을 만들 오브젝트에 있는 쉐이더에
#		"float bake" [ 1 ]
#	이라고 추가한다. "Bake 이미지"와 "렌더링 이미지"를 만드는 RIB의 "Declare"부분에
#	다음과 같이 각각 서로 다른 내용을 쓴다.
#		Option "user" "uniform string BakeContext" [ "BAKE" ]
#		Option "user" "uniform string BakeContext" [ "" ]	# 생략할 수 있다.
#	쉐이더에서는 bake라는 값이 설정되어 있을 경우,
#	"bakeContext"가 "BAKE"로 설정되어 있는 경우에는 bake3d함수를 호출하고,
#	그렇지 않은 경우는 texture3d함수를 호출한다.
#
#	if (bake) {
#		string	bakeContext;
#		if (option("user:BakeContext", bakeContext)) {
#			bake3d(...);
#		} else {
#			texture3d(...);
#		}
#	} else {
#		# "Bake"모드가 아니다.
#	}
#
#	문제는 어떻게 오브젝트마다 다른 "Point Cloud File"을 사용하느냐 인데,
#		1) 모든 오브젝트의 이름(objectName)은 유일하다고 가정하고
#		2) Point Clout File의 이름은 objectName.ptc이고,
#		3) Brick Map File의 이름은 objectName.bkm이라 가정하면
#	bake3d나 texture3d에서 적절한 파일을 사용할 수 있다.
#	쉐이더에서 오브젝트의 이름은 attribute()함수를 호출해서 얻는다.
#
#	string objectName;
#	if (attribute("identifier:name", objectName)) {
#	} else {
#		# 오브젝트의 이름이 지정되지 않은 경우이다.
#	}
#
#	마지막으로 해당 파일의 경로는, 역시, Option을 통해 지정한다.
#
#		Option "user" "uniform string BakePath" [ ".../" ]
#
#	이는 쉐이더에 넘겨주는 i)부분만을 고려하였지만,
#	ii)부분이더라도 "적절하게" 처리할 수 있다.
#	굳이 특정 속성(Attribute를 써서)을 설정할 필요가 없다.
#	쉐이더에서 해당 속성이 없는 것처럼 처리하면 된다.
#	예를 들어, "렌더링 이미지"에서는 ray tracing이 설정되어 있고,
#	"Bake 이미지"에서는 그렇지 않다. (왜?)
#		Attribute "visibility" "int camera" [1] "int trace" [1] "int photon" [1]
#	와 같은 내용을 RIB에서 삭제하지 않더라도 쉐이더에서 처리할 수 있다.
#
#	uniform float	rayDepth = 0;
#	if (rayinfo("depth", rayDepth) && rayDepth > 0) {
#		# Ray tracing에 의미있는 결과를 주지 않도록 한다.
#		Ci = 0;
#		return;
#	}
#
#	2007/5/3 (EST)
#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)
#$bBake = 1; 	# CHUNK_TYPE__BAKE를 통해서 얻는다.

# Subframe에 저장될 내용과 frame에 있는 subframe의 내용이 다르므로,
# 지금처럼 SUBFH에 $hStackRef->{ 'content' }을 그대로 쓰면 안 된다.
# 이를 위해 $hStackRef->{ 'subframe content' }를 추가한다.
# 만약 같은 내용이라면 $hStackRef->{ 'subframe content' } = $hStackRef->{ 'content' }로 해서 사용하면 된다.
#
#	2007/5/2 (EST)
#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)


# 렌더맨 설명서에 있는 "Application Notes"중에서 37번의
# subsurface scattering(SSSC)을 구현한다.
#
# 1. 내용을 요약하면 다음과 같다.
# SSSC를 구현하는 두 가지 방법이 있다.
# 하나는 "2 Multiple scattering (diffusion approximation)"에 있는 "ptfilter"프로그램을 사용하여
# diffusion simulation을 하는 것이고,
# 다른 하나는 "4 No scattering (absorption only)"에 있듯이,
# 물체의 표면 아래로 "직접" ray을 쏘아 ray tracing을 계산하는 것이다.
# 즉 2의 방법은 "ptfilter -ssdiffusion ..."처럼 "filter"를 "ssdiffusion"을 사용해서
# 만들어진 point cloud파일로부터 "ssdiffsusion"이 "대략적으로"(?) 계산된 또 다른 point cloud파일을 사용하는 것이고,
# 4의 방법은 "gather"함수를 이용하여 ray tracing을 이용하는 것이다.
# 4의 방법은 마지막에 언급되었든 렌더링하는 데 걸리는 시간이 훨씬(?) 길다. (40초와 2분 30분)
# 어느 경우든 먼저 point cloud파일을 만든 후 bake한다는 점에서는 같다.
# 2의 방법은
#	SHADER: bake3d xxx.ptc
#	PROGRAM: ptfilter -filter ssdiffusion xxx.ptc yyy.ptc
#	PROGRAM: brickmake yyy.ptc xxx.bkm
#	SHADER: texture3d xxx.bkm
# 4의 방법은
#	SHADER: bake3d xxx.ptc
#	PROGRAM: brickmake xxx.ptc xxx.bkm
#	SHADER: texture3d xxx.bkm + gather
#
# Q. 4방법의 마지막 이미지는 어떻게 얻은 것일까?
# A. 사용된 RIB이 없으나 설명을 이해한 것으로 추측하면,
# 마지막 쉐이더(subsurf_noscatter.sl)에서
# "direct" illumination은 계산하지 않고 extcoeff를 1보다 작은 값을 썼을 것이다.
#
# 2. 첫 번째 방법인 2의 방법을 사용할 것이고, 이는 Delux쉐이더를 통해 구현된다.
# 문제는 "렌더링"이미지를 RIB을 두 번 만든다는 것이다.
# 한 번은 bake3d를 통해서 또 한 번은 texture3d를 통해서.
# 이 외에도 RIB의 다른 차이가 있으나 우선은 두 종류의 RIB을 만드는 것을 구현한다.
#
# 원본 RIB을 xxx.000.rib이라 할 때 이 안에는 "렌더링 이미지"에 해당하는 subframe뿐만 아니라
# 여러 "쉐도우 이미지"에 해당하는 subframe이 있다.
#
#	xxx.0001.rib = (xxx.Light1.0001.rib + xxx.Light2.0001.rib + ...) + xxx.Image.0001.rib
#
# 여기서 xxx.Light#.0001.rib은 "쉐도우 이미지"를 만드는 subframe에 해당하는 RIB이고,
# xxx.Image.0001.rib은 "렌더링 이미지"를 만드는 subframe에 해당하는 RIB이다.
#
# SSSC를 구현하기 위해서는, 크게 본다면, 두 개의 "xxx.Image.0001.rib"을 만드는 것이다.
# 하나의 "쉐이더"에 bake3d가 있는 RIB이고, 다른 하나는 "쉐이더"에 texture3d가 있는 RIB이다.
# 각각을 xxx.bake3d.0001.rib, xxx.texture3d.0001.rib이라 부르면
# SSSC를 구현하가 위해서는 다음과 같이 해야 한다.
#
#	xxx.0001.rib = [(xxx.Light1.rib + xxx.Light2.rib + ...) + xxx.bake3d.0001.rib] + xxx.texture3d.0001.rib
#
# 결국 지금까지와 달리 "렌더링 이미지"에 해당하는 RIB을 추가적으로 만들어야 한다.
#
# 몇 가지 기록해야 할 사항들이다.
#	1) 예제로 사용했던 씬은 "rattutorials/mtor/scenes/subsurface_scattering/sssc.ma"이다.
#
#	2) 주어진 RIB을 subframe별로 쪼개는 것을 구현한 이유이기가 바로 SSSC를 위해서이다.
#	mtor에서 genRib명령어를 사용하지 않고 genWorklist를 사용하면 각 subframe별로 RIB이 만들어지므로,
#	이 경우에는 xxx.Image.0001.rib을 이 프로그램을 두 번 써서
#	xxx.bake3d.0001.rib, xxx.texture3d.0001.rib을 만들 수도 있다.
#	이 프로그램의 목적이 원본 RIB을 한 번만 읽는 것이므로,
#	역시 subframe별로 쪼개는 방법을 구현하는 것이 의미가 있다.
#	경우에 따라 원본 RIB에서 원하는 subframe에 해당하는 RIB만 뽑아내는 경우가 있을 수 있기 때문이다.
#	예를 들어, 이미 만들어진 point cloud 파일(.ptc)이나 brick map(.bkm)을 재사용하는 경우가 그러하다.
#	물론 "쉐도우 이미지"도 재사용하는 경우에.
#
#	3) xxx.bake3d.0001.rib과 xxx.texture3d.0001.rib의 차이는 기본적인 "쉐이더"의 차이외에도
#	몇 가지가 더 있다. 그 중에 하나가 "Applicatioin Note #37"에 있듯이,
#	다음 내용을 첫 번째 오브젝트에 해당하는 "AttributeBegin"전에 넣는 것이다.
#
#		Attribute "cull" "hidden" 0 "backfacing" 0
#		Attribute "dice" "rasterorient" 0
#
#	RIB Box를 구현하는 것 자체도 의미가 있지만, SSSC의 이 부분을 위해서이기도 필요했다.
#	예제로 사용했던 씬을 통해서도 확인된다!
#
# 3. "Job chunk"에 대한 정보에서, "Render Stats"에 SSSC에 해당하는 플래그가 있을 때,
# SSSC에 필요한 위의 작업을 한다.
# 다시 한 번 $RS__...내용을 적어본다.
#$RS__CAST_SHADOW	= 1 << 0;
#$RS__RECV_SHADOW	= 1 << 1;
#$RS__VISIBILITY		= 1 << 2;
#$RS__VIS_REFL		= 1 << 3;
#$RS__VIS_REFR		= 1 << 4;
#$RS__RT			= $RS__VIS_REFL | $RS__VIS_REFR;
#$RS__DELETE		= 1 << 31;	# 이 비트가 설정되어 있으면, 다른 비트는 무시된다.
#$RS__MATTE		= 1 << 30;	# RS__DELETE와 마찬가지로, 이 비트가 설정되어 있다면 다른 비트는 무시된다.
					# 하지만 RS__DELETE비트의 우선 순위가 더 높다.
$RS__SSSC		= 1 << 29;	# RS__DELETE와 마찬가지로, 이 비트가 설정되어 있다면 다른 비트는 무시된다.
					# 하지만 우선 순위는 RS__DELETE > RS__MATTE > RS__SSSC이다.
#
#	2007/5/1 (EST)
#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)


# 1. RIB Box안에서 몇 개의 변수를 참고할 수 있도록 한다.
#
#	$FRAME: "%04d"형식으로 된, 렌더링하는 프레임 수이다.
#	$ELEMENT: Subframe의 이름으로, "쉐도우 이미지"의 경우에는 해당 "라이트"의 이름이고,
#		"렌더링 이미지"의 경우에는 ""이다.
#	$OBJECT: "라이트 쉐이더"의 경우에는 "라이트"의 이름이,
#		"오브젝트"의 경우에는 "물체"의 이름이다.
#
# 주어진 "RIB Box"을 내용을 담은 "표현"($ribBoxExpr)은 evalExpr함수를 통해 해석된다.
# 내부적으로는 "eval( 'qq|' . $ribBoxExpr . '|' )"를 통해 해석된다.
#
#	$ribBoxExprEvaluated = evalExpr( . $ribBoxExpr . '|' );
#
# 다음 변수는, 위의 "RIB Box" 변수에 대응하는 전역변수이다.
# 설정되지 않았으면 "undef"값을 갖는다.
my	$gFrameNumber;		# 파일이름을 통해서 얻는다.
my	$gElementName;		# 매 subframe마다, "Display"를 통해서 얻는다.
				# "렌더링 이미지"의 경우에는 ""이다.
				# "FrameEnd"에서 undef으로 초기화된다.
my	$gObjectName;		# 다음과 같은 "Attribute"에서 얻는다.
				#	Attribute "identifier" "string name" ["..."]	# LIGHT
				#	Attribute "identifier" "name" ["..."]		# OBJECT (geometric primitive)
				# 위의 "Attribute"를 포함하는 "TransformEnd"(LIGHT의 경우)나
				# "AttributeEnd"에서 undef으로 초기화된다.
# $gObjectName을 얻은, "TransformBegin"이나 "AttributeBegin"의 스택 위치를 기억해서
# 대응하는 "TransformEnd"나 "AttributeEnd"를 만났을 때,
# 같은 스택 위치라면 undef으로 초기화한다.
my	$giObjectNameStackIndex;
sub	evalRIBBoxExpr
{
	my	$myExpr = shift;

	my	$ELEMENT = $gElementName;
	$ELEMENT = 'undef' unless defined( $ELEMENT );
	my	$FRAME = $gFrameNumber;
	$FRAME = 'undef' unless defined( $FRAME );
	my	$OBJECT = $gObjectName;
	$OBJECT = 'undef' unless defined( $OBJECT );

	return defined( $myExpr ) ? eval( 'qq|' . $myExpr . '|' ) : undef;
}
# 다음은 "frame # = $FRAME"이라는 내용의 RIB Box를 해석하는 예이다.
#	print STDERR evalRIBBoxExpr( 'frame # = $FRAME' ) . $/;
#
# 2. 다음과 같은 해쉬구조를 통해 RIB Box의 정보를 담는다.
# 기본적으로 "Key"에 RIB Box의 종류($EUNM_RIB_BOX___XXX),
# "Value"에 RIB Box의 내용을 담은 "RIB Box 표현"이다.
# 하지만 RIB Box의 종류에 따라 해당 "RIB Box 표현"이 어느 subframe(위의 $ELEMENT에 해당)에 적용될지,
# 또 어느 종류는 이 외에도 어느 object(위의 $OBJECT에 해당)에 적용될지를 말해주어야 한다.
# 이러한 "적용 지점"를 "CONTEXT"라 하자.
# 이 정보까지도 포함되어야 하므로 "Value"역시 또 하나의 구조를 가져야 한다.
# 배열 구조가 적당하다. 첫 번째 성분은 "RIB Box 표현"이고,
# 두 번째 성분은 "CONTEXT"이다.
#	1) $ENUM_RIB_BOX__DECLARE__JUST_BEFORE_FIRST_FRAME_BEGIN은 "CONTEXT"가 없으므로,
#	이 경우에는 "Value"가 단순한 "RIB Box 표현"이면 충분하다.
#	2) "CONTEXT"를 말할 때, ELEMENT는 반드시 지정해야한다.
#	"Categoray"가 "Light"와 "Object"의 경우에는 이 외에도 OBJECT도 지정해야 한다.
#	따라서 이 경우에는 두 번째 성분이 (ELEMENT, OBJECT)를 각 성분으로 하는 배열이어야 한다.
#	CONTEXT:= [ ELEMENT1, ELEMENT2, ... ] 또는
#	CONTEXT:= [ [ ELEMENT1, OBJECT1 ], [ ELEMENT2, OBJECT2], ... ]
#	3) "RIB Box 표현"이 "CONTEXT"가 둘 이상일 수 있다. 역시 배열이 되어야 한다.
#	즉 [ CONTEXT1, CONTEXT2, ...]
#	4) 한 RIB Box 종류에는 여러 개의 "RIB Box 표현"과 대응하는 "CONTEXT"정보가 있을 수 있다.
#	따라서 역시 배열이 되어야 한다.
#	[ [ EXPR1, [ CONTEXT11, CONTEXT12, ... ] ], [ EXPR2, [ CONTEXT21, CONTEXT22, ... ] ] ]
#
# 위의 규칙에 따라 RIB Box에 대한 정보는 아래의 getRIBBoxExpr함수를 이용한다.
#
#	2007/4/30 (EST)
#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)

# 1. RIB Box가 들어갈 위치를 알아본다.
# 명령줄에서 "-vRIBBox"옵션을 설정하면 그 위치에 대한 정보가 나온다.
# 물론 RIB에는 해당 위치에 다음과 같은 형식으로 RIB Box가 들어갈 수 있음을 표시한다.
# (ENUM_RIB_BOX__DECLARE__JUST_BEFORE_FIRST_FRAME_BEGIN의 경우에)
#
#	#### RIB Box: Just before frame begin
#
# 2. 쉐이더가 속하는 즉 오브젝트의 이름을 말해주는
#	Attribute "identifier" "name" [ "..." ]
# 을 포함하고 있는 스택을 찾는다.
sub getStackName
{
	my	$AoHStackRef = shift;

	print STDERR 'EDITRIB: Info: Seaching object...' . $/ if $bVerbose;
	for ( my $iStack = $#{ $AoHStackRef }; $iStack > 0; --$iStack ) {	# "> 0": WorldBegin..WorldEnd안에서만 찾느다.
		my	$myhStackRef = $AoHStackRef->[ $iStack ];
		my	$myStackType = $myhStackRef->{ 'type' };
		my	$myStackName = $myhStackRef->{ 'name' };
		print STDERR 'EDITRIB: Info: Stack[' . $iStack . ']: Type ID:= ' . $myStackType . ', Name:= ' . '{' . ( $myStackName ? $myStackName : 'undef' ) . '}' . $/ if $bVerbose;
		return ( $myStackName, $iStack ) if ( $myStackName );
	}
	return (undef, undef);
}
#	2007/4/26 (EST)
#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)
$bVerboseRIBBox = 0;

# RIB Box를 구현한다.
# RIB Box가 들어갈 위치는 다음과 같다.
#	1) "Declare" 부분 (Category: Declare)
#		"FrameBegin" 바로 전 (Place: JustBeforeFirstFrameBegin)
#			예) 렌더링에 사용된 정보를 나타내는 다음 내용
#				Option "statistics" "endofframe" [0]	# 0, 1, 2, 3
#			또는 micropolygon정보를 담은 cache를 디스크에 쓸지 메모리에 쓸지를 말해주는 다음 내용
#				Hider "hidden" "mpcache" [0]		# 0: 메모리, 1: 디스크
#	2) "Frame" 부분 (Category: Frame)
#		"Display" 바로 전 (Place: JustBeforeDisplay)
#			예) 이미지 크기("Format")나 "Display Channel" 정보
#		"WorldBegin" 바로 전 (Place: JustBeoreWorldBegin)
#	3) "World" 부분 (Category: World)
#		첫 번째 "LightSource" 정보를 담은 "TransformBegin" 바로 전 (Place: JustBeforeFirstLight)
#			예) Ray tracing과 관련된 다음과 같은 정보
#				Attribute "visibility" "int camera" [1] "int trace" [1] "int photon" [1] "string transmission" ["opaque"]
#				Attribute "trace" "int maxspeculardepth" [2]
#				Attribute "trace" "int maxdiffusedepth" [1]
#				Attribute "trace" "float bias" [0.05] "int displacements" [0] "int samplemotion" [0]
#				Attribute "photon" "string shadingmodel" ["matte"]
#				Attribute "photon" "int estimator" [50]
#		첫 번째 "geometric primitive" 정보를 담은 "AttributeBegin" 바로 전 (Place: JustBeforeFirstObject)
#			예) Subsurface scattering에 사용되는 brick map을 만들 때 필요한 다음과 같은 내용
#				Attribute "cull" "hidden" 0 "backfacing" 0
#				Attribute "dice" "rasterorient" 0
#		마지막 "geometric primitive" 정보를 담은 "AttributeEnd" 바로 후 (Place: JustAfterLastObject)
#		("WorldEnd"바로 직전)
#			예) RIB의 내용이 변하지 않는 정적인, 독립적인 "geometric primitive"에 관한 RIB
#	4) 특정 "LightSource" 부분 (Category: Light)
#		해당 "라이트 쉐이더"의 이름을 말하는 "Attribute" 바로 후 (Place: JustAfterAttribute)
#	5) 특정 "geometric primitive" 부분 (Category: Object 또는 생략)
#		해당 오브젝트의 이름을 말하는 "Attribute" 바로 후 (Place: JustAfterAttribute)
#			예) 오브젝트를 렌더링하는 데 필요한 정보(ray tracing, visibility 등)
#		"Surface" 바로 전/후 (Place: JustBeforeSurface/JustAfterSurface)
#			예) 쉐이더 파라미터나 또 다른 쉐이더
#		"Geometric primitive" 바로 전 (Place: JustBeforeGeometricPrimitive)
#			예) "Curves"와 같은 또 다른 "geometric primitive" 정보
# 다음은 "RIB Box"와 "enumerator" 상수들이다.
#	1) "Declare" 부분 (Category: Declare)
#		"FrameBegin" 바로 전 (Place: JustBeforeFirstFrameBegin)
$ENUM_RIB_BOX__DECLARE__JUST_BEFORE_FIRST_FRAME_BEGIN		= 0;
#	2) "Frame" 부분 (Category: Frame)
#		"Display" 바로 전 (Place: JustBeforeDisplay)
$ENUM_RIB_BOX__FRAME__JUST_BEFORE_DISPLAY			= 1;
#		"WorldBegin" 바로 전 (Place: JustBeoreWorldBegin)
$ENUM_RIB_BOX__FRAME__JUST_BEFORE_WORLD_BEGIN			= 2;
#	3) "World" 부분 (Category: World)
#		첫 번째 "LightSource" 정보를 담은 "TransformBegin" 바로 전 (Place: JustBeforeFirstLight)
$ENUM_RIB_BOX__WORLD__JUST_BEFORE_FIRST_LIGHTSOURCE_BLOCK	= 3;
#		첫 번째 "geometric primitive" 정보를 담은 "AttributeBegin" 바로 전 (Place: JustBeforeFirstObject)
$ENUM_RIB_BOX__WORLD__JUST_BEFORE_FIRST_GEOMETRIC_PRIMITIVE_BLOCK	= 4;
#		마지막 "geometric primitive" 정보를 담은 "AttributeEnd" 바로 후 (Place: JustAfterLastObject)
$ENUM_RIB_BOX__WORLD__JUST_AFTER_LAST_OBJECT_BLOCK		= 5;
#	4) 특정 "LightSource" 부분 (Category: Light)
#		해당 "라이트 쉐이더"의 이름을 말하는 "Attribute" 바로 후 (Place: JustAfterAttribute)
$ENUM_RIB_BOX__LIGHT__JUST_AFTER_ATTRIBUTE			= 6;
#	5) 특정 "geometric primitive" 부분 (Category: Object 또는 생략)
#		해당 오브젝트의 이름을 말하는 "Attribute" 바로 후 (Place: JustAfterAttribute)
$ENUM_RIB_BOX__OBJECT__JUST_AFTER_ATTRIBUTE			= 7;
#		"LightSource" 쉐이더 바로 전 (Place: JustBeforeLightShader)
$ENUM_RIB_BOX__OBJECT__JUST_BEFORE_LIGHT_SHADER			= 8;
#		"Surface" 쉐이더 바로 전 (Place: JustBeforeSurfaceShader)
$ENUM_RIB_BOX__OBJECT__JUST_BEFORE_SURFACE_SHADER		= 9;
#		"Geometric primitive" 바로 전 (Place: JustBeforeGeometricPrimitive)
$ENUM_RIB_BOX__OBJECT__JUST_BEFORE_GEOMETRIC_PRIMITIVE		= 10;
#
#	2007/4/25 (EST)
#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)

=head1 EXAMPLE
# "RIB Box"에 대한 정보를 담은 위와 같은 해쉬구조가 있을 때,
# 다음 함수를 통해 (Type[, ELEMENT[, OBJECT]])에 사용되는 "RIB Box 표현"에 대한 정보를 얻는다.
%hAoARIBBox = (
	$ENUM_RIB_BOX__DECLARE__JUST_BEFORE_FIRST_FRAME_BEGIN	=>
		# 렌더링에 사용된 정보를 나타낸다.
		'Option "statistics" "endofframe" [1]' . "\r\n" .	# 0, 1, 2, 3
		# micropolygon정보를 담은 cache를 디스크에 쓸지 메모리에 쓸지를 말해주는 다음 내용
		'Hider "hidden" "mpcache" [0]' . "\r\n"			# 0: 메모리, 1: 디스크
		,

	# 필요하면 적용될 ELEMENT를 정의한다.
	$ENUM_RIB_BOX__FRAME__JUST_BEFORE_DISPLAY		=> [
		'hello, world',

	],
	$ENUM_RIB_BOX__FRAME__JUST_BEFORE_WORLD_BEGIN		=> [
		'hello, world',
		'light'
	],
	$ENUM_RIB_BOX__WORLD__JUST_BEFORE_FIRST_LIGHTSOURCE_BLOCK	=> [
		'hello, world',
		'light1', 'light2',
	],
	$ENUM_RIB_BOX__WORLD__JUST_BEFORE_FIRST_GEOMETRIC_PRIMITIVE_BLOCK	=> [
		'hello, world',
		[ 'light1', 'light2' ]
	],
	$ENUM_RIB_BOX__WORLD__JUST_AFTER_LAST_OBJECT_BLOCK	=> [
		'hello, world',
		[ [ '.*' ], [ 'light' ] ]
	],

	# 필요하면 적용될 (OBJECT, ELEMENT)를 정의한다.
	$ENUM_RIB_BOX__LIGHT__JUST_AFTER_ATTRIBUTE		=> [
		'hello, world',
	],
	$ENUM_RIB_BOX__OBJECT__JUST_AFTER_ATTRIBUTE		=> [
		'hello, world',
		'.*',
	],
	$ENUM_RIB_BOX__OBJECT__JUST_BEFORE_SURFACE		=> [
		'hello, world',
		'.*',
		'light'
	],
	$ENUM_RIB_BOX__OBJECT__JUST_BEFORE_GEOMETRIC_PRIMITIVE	=> [
		'hello, world: $FRAME, $ELEMENT, $OBJECT',
		'j,ight$:sphere'
#		[ 'shere' ],
		# 'sphere'

#		'light1', 'light2',
#		[ 'light1', 'light2' ],
#		[ [ '.*' ], [ 'light' ] ],
	],
);
my	$RIBBoxExpr = getRIBBoxExpr(
	\%hAoARIBBox,
#	$ENUM_RIB_BOX__DECLARE__JUST_BEFORE_FIRST_FRAME_BEGIN,
	# 필요하면 적용될 ELEMENT를 정의한다.
	#$ENUM_RIB_BOX__FRAME__JUST_BEFORE_DISPLAY,
	#$ENUM_RIB_BOX__FRAME__JUST_BEFORE_WORLD_BEGIN,
	#$ENUM_RIB_BOX__WORLD__JUST_BEFORE_FIRST_LIGHTSOURCE_BLOCK,
	#$ENUM_RIB_BOX__WORLD__JUST_BEFORE_FIRST_GEOMETRIC_PRIMITIVE_BLOCK,
	#$ENUM_RIB_BOX__WORLD__JUST_AFTER_LAST_OBJECT_BLOCK,
	# 필요하면 적용될 (OBJECT, ELEMENT)를 정의한다.,
	#$ENUM_RIB_BOX__LIGHT__JUST_AFTER_ATTRIBUTE,
	#$ENUM_RIB_BOX__OBJECT__JUST_AFTER_ATTRIBUTE,
	#$ENUM_RIB_BOX__OBJECT__JUST_BEFORE_SURFACE,
	$ENUM_RIB_BOX__OBJECT__JUST_BEFORE_GEOMETRIC_PRIMITIVE,
	'iight',
	'sphere',
);
if (
	$RIBBoxExpr
) {
	print STDERR
		'=' x 40 . "\r\n" .
		$RIBBoxExpr . "\r\n" .
		'-' x 40 . "\r\n" .
		evalRIBBoxExpr( $RIBBoxExpr ) . "\r\n" .
		'=' x 40 . "\r\n"
	;
}
=cut
sub getRIBBoxExpr
{
	my	( $hAoARIBBoxRef, $enumRIBBox, $element, $object ) = @_;

	return undef unless $hAoARIBBoxRef;
	return undef if ( ref( $hAoARIBBoxRef ) ne 'HASH' );
	my	$val = $hAoARIBBoxRef->{ $enumRIBBox };
	return undef unless $val;
	my	$refType = ref( $val );

	if (
		$enumRIBBox == $ENUM_RIB_BOX__DECLARE__JUST_BEFORE_FIRST_FRAME_BEGIN
	) {
		# "..."이어야 한다.
		if ( $refType ) {
			print STDERR 'EDITRIB: Warning: RIBBOX: Expr: ENUM_RIB_BOX__XXX = ' . $enumRIBBox . ': Not a SCALAR: Ignored' . $/;
			return undef;
		}
		return $val;
	}
	if (
		# CONTEXT에서 ELEMENT가 정의되어야 한다.
		#	정의되지 않았다면 모든 ELEMENT에서 적용된다고 가정한다.
		$enumRIBBox != $ENUM_RIB_BOX__FRAME__JUST_BEFORE_DISPLAY &&
		$enumRIBBox != $ENUM_RIB_BOX__FRAME__JUST_BEFORE_WORLD_BEGIN &&
		$enumRIBBox != $ENUM_RIB_BOX__WORLD__JUST_BEFORE_FIRST_LIGHTSOURCE_BLOCK &&
		$enumRIBBox != $ENUM_RIB_BOX__WORLD__JUST_BEFORE_FIRST_GEOMETRIC_PRIMITIVE_BLOCK &&
		$enumRIBBox != $ENUM_RIB_BOX__WORLD__JUST_AFTER_LAST_OBJECT_BLOCK &&
		# CONTEXT에서 (OBJECT, ELEMENT)가 정의되어야 한다.
		#	하나만 정의되었을 때, 정의된 OBJECT가 모든 ELEMENT에서 적용된다고 가정한다.
		#	정의되지 않았다면, 모든 ELEMENT에서 모든 OBJECT에서 적용된다고 가정한다.
		$enumRIBBox != $ENUM_RIB_BOX__LIGHT__JUST_AFTER_ATTRIBUTE &&
		$enumRIBBox != $ENUM_RIB_BOX__OBJECT__JUST_AFTER_ATTRIBUTE &&
		$enumRIBBox != $ENUM_RIB_BOX__OBJECT__JUST_BEFORE_LIGHT_SHADER &&
		$enumRIBBox != $ENUM_RIB_BOX__OBJECT__JUST_BEFORE_SURFACE_SHADER &&
		$enumRIBBox != $ENUM_RIB_BOX__OBJECT__JUST_BEFORE_GEOMETRIC_PRIMITIVE
	) {
		print STDERR 'EDITRIB: Error: RIBBOX: Unknown ENUM_RIB_BOX__XXX = ' . $enumRIBBox . ': Ignored' . $/;
		return undef;
	}
	# 원래는
	#	[ [ EXPR, CONTEXT ] ], ... ]		- (1.1)
	# 와 같이 여러 개의 "RIB Box 표현"을 쓸 수 있으나
	# 다음과 같이 쓴다면
	#	[ EXPR, CONTEXT ]			- (1.2)
	# 성분이 하나만 있는 (1.1)로 생각한다.
	# "Value"자체는 "ARRAY"에 대한 "참조"이어야 한다.
	#	첫 번째 성분이 "SCALAR"라면 (1.2)을 가정하고,
	#	"ARRAY"에 대한 "참조"라면 (1.1)을 가정한다.
	#	즉 (1.2)는 성분이 하나인 (1.1)배열이라고 가정한다.
	if ( $refType ne 'ARRAY' ) {
		print STDERR 'EDITRIB: Error: RIBBOX: Expr: ENUM_RIB_BOX__XXX = ' . $enumRIBBox . ': Not a reference to ARRAY: Ignored' . $/;
		return undef;
	}
	if ( @{ $val } == 0 ) {
		print STDERR 'EDITRIB: Warning: RIBBOX: Expr: ENUM_RIB_BOX__XXX = ' . $enumRIBBox . ': # of element(s) = 0: Ignored' . $/;
		return undef;
	}
	my	$refType1 = ref( $val -> [ 0 ] );	# 첫 번째 성분을 조사한다.
	if (
		$refType1 &&		# (1.2)도 아니고
		$refType1 ne 'ARRAY'	# (1.1)도 아니다
	) {
		print STDERR 'EDITRIB: Error: RIBBOX: Expr: (1.1)도 아니고 (1.2)도 아니다: 무시한다' . $/;
		return undef;
	}
	for my $aRIBBoxExpr (
		$refType1 ? @{ $val } :
		@{ [ $val ] }		# (1.2)라면 성분이 하나인 (1.1)라고 가정
	) {
		# [ EXPR, CONTEXT ]의 형태이다.
		#
		# 첫 번째는 성분은  SCALAR로 "RIB Box 표현"이다.
		# 두 번째 성분 이후의 성분은 (있다면) CONTEXT를 정의한다.
		# 자세한 것은 아래의 각 경우에 대한 설명을 참고한다.
		if ( @{ $aRIBBoxExpr } == 0 ) {
			# 적어도 하나의 성분 즉 "RIB Box 표현"은 있어야 한다.
			print STDERR 'EDITRIB: Error: Expr: 적어도 하나의 성분("RIB Box 표현")은 있어야 한다: 무시한다' . $/;
			next;
		}
		# 적어도 하나의 성분은 있다.
		my	$RIBBoxExpr = $aRIBBoxExpr->[ 0 ];
		my	$refTypeExpr = ref( $RIBBoxExpr );
		if ( $refTypeExpr ) {
			# 첫 번째 성분은 "RIB Box 표현"인 SCALAR이어야 한다.
			print STDERR 'EDITRIB: Error: RIBBOX: Expr: 첫 번째 성분은 "RIB Box 표현"인 SCALAR이어야 한다: 무시한다' . $/;
			next;
		}
		if ( @{ $aRIBBoxExpr } == 1 ) {
			# CONTEXT가 생략되었으므로 모든 ELEMENT에 또는 모든 ELEMENT에 있는 모든 OBJECT에 적용된다.
			return $RIBBoxExpr;
		}
		# CONTEXT를 정의한 다른 성분이 있다.
		#
		# CONTEXT란 ELEMENT또는 (ELEMENT, OBJECT)를 정의한,
		# 이 프로그램의 다른 곳에서 많이 쓰이는 "집합 표현"을 말한다.
		if (
			$enumRIBBox == $ENUM_RIB_BOX__FRAME__JUST_BEFORE_DISPLAY ||
			$enumRIBBox == $ENUM_RIB_BOX__FRAME__JUST_BEFORE_WORLD_BEGIN ||
			$enumRIBBox == $ENUM_RIB_BOX__WORLD__JUST_BEFORE_FIRST_LIGHTSOURCE_BLOCK ||
			$enumRIBBox == $ENUM_RIB_BOX__WORLD__JUST_BEFORE_FIRST_GEOMETRIC_PRIMITIVE_BLOCK ||
			$enumRIBBox == $ENUM_RIB_BOX__WORLD__JUST_AFTER_LAST_OBJECT_BLOCK
		) {
			# ELEMENT가 필요한 CONTEXT이다.
			#
			# ELEMENT를 정의한 CONTEXT의 "집합 표현"은 다음과 같다.
			#	CONTEXT:= [ ELEMENT1, ... ]				- (집합 표현 1)
			#	CONTEXT:= [ [ ELEMENT1, ... ], [ ELEMENT2, ... ] ]	- (집합 표현 2)
			# "집합 표현 1"은 각 성분을 집합의 원소로 하는 일반적인 집합이고,
			# "집합 표현 2"는 첫 번째 성분인 "집합 표현 1"에서 두 번째 성분인 "집합 표현 1"을 뺀 차집합을 의미한다.
			# 여기서 ELEMENT, OBJECT는 모두 SCALAR이다.
			#	ELEMENT:= SCALAR
			#	OBJECT:= SCALAR
			#
			# "집합 표현 1"의 경우, '[', ']'는 생략될 수 있다.
			# 이 경우 모든 성분이 SCALAR이어야 한다!
			my	$refType2 = ref( $aRIBBoxExpr -> [ 1 ] );	# CONTEXT의 유형을 조사한다.
			if (
				$refType2 &&		# "집합 표현 1"에서 '[', ']'가 생략된 경우도 아니고,
				$refType2 ne 'ARRAY'	# "집합 표현"을 말하는 ARRAY에 대한 참조도 아니다.
			) {
				print STDERR 'EDITRIB: Error: RIBBOX: Expr: "집합 표현 1"에서 \'[\', \']\'가 생략된 경우도 아니고, "집합 표현"을 말하는 ARRAY에 대한 참조도 아니다: 무시한다' . $/;
				next;
			}
			if ( $refType2 eq 'ARRAY' ) {
				# (있다면) 세 번째 이 후의 성분은 무시된다.
				if ( @{ $aRIBBoxExpr } > 2 ) {
					print STDERR 'EDITRIB: Warning: RIBBOX: Expr: CONTEXT가 배열에 대한 참조의 경우, "RIB Box 표현"의 세 번째부터의 성분은 무시된다' . $/;
				}
				if ( isMatched( $element, $aRIBBoxExpr -> [ 1 ] ) ) {
					# ELEMENT가 일치한다.
					return $RIBBoxExpr;
				}
			} else {
				# "집합 표현 1"에서 '[', ']'가 생략된 경우이다.
				for ( my $i = 1; $i < @{ $aRIBBoxExpr }; ++$i ) {
					if ( ref( $aRIBBoxExpr->[ $i ] ) ) {
						print STDERR 'EDITRIB: Error: RIBBOX: Expr: Not a SCALAR: Ignored' . $/;
						next;
					}
#					return $RIBBoxExpr if $element =~ /$aRIBBoxExpr->[ $i ]/i;
					# "a,b,...,-x,-y,.."를 허용한다.
					return $RIBBoxExpr if isInSet( $element, $aRIBBoxExpr->[ $i ] );
				}
			}

			# 다음 [ EXPR, CONTEXT ]를 조사한다.
			next;
		}
		# (OBJECT, ELEMENT)가 필요한 CONTEXT이다.
		#
		# (OBJECT, ELEMENT)를 정의한 "집합 표현"은 다음과 같다.
		#	CONTEXT:= [ OBJECT1, ... ], [ ELEMENT1, ... ]		- "집합 표현 1"
		#	CONTEXT:= [ [ OBJECT1, ... ], [ OBJECT2, ... ] ], [ [ ELEMENT1, ... ], [ ELEMENT2, ... ] ]
		#								- (집합 표현 2)
		# 먼저 ELEMENT를 정의하는 집합에 속하는 지 알아본다.
		my	$bElement = 0;	# $element가 ELEMENT를 정의하는 "집합"에 속하는 지 말해준다.
		if ( @{ $aRIBBoxExpr } > 2 ) {
			# ELEMENT를 정의하는 "집합 표현"이 있다.
			# "집합 표현 1"의 경우, '[', ']'는 생략할 수 있다.
			my	$refType3 = ref( $aRIBBoxExpr->[ 2 ] );
			if (
				$refType3  &&		# "집합 표현 1"에서 '[', ']'가 생략된 경우도 아니고,
				$refType3 ne 'ARRAY'	# "집합 표현"을 말하는 ARRAY에 대한 참조도 아니다.
			) {
				print STDERR 'EDITRIB: Error: RIBBOX: Expr: "집합 표현 1"에서 \'[\', \']\'가 생략된 경우도 아니고, "집합 표현"을 말하는 ARRAY에 대한 참조도 아니다: 무시한다' . $/;
				next;
			}
			if ( $refType3 eq 'ARRAY' ) {
				# (있다면) 네 번째 이 후의 성분은 무시된다.
				if ( @{ $aRIBBoxExpr } > 3 ) {
					print STDERR 'EDITRIB: Warning: RIBBOX: Expr: CONTEXT가 배열에 대한 참조의 경우, "RIB Box 표현"의 네 번째부터의 성분은 무시된다' . $/;
				}
				$bElement = isMatched( $element, $aRIBBoxExpr -> [ 2 ] );
			} else {
				# "집합 표현 1"에서 '[', ']'가 생략된 경우이다.
				for ( my $i = 1; $i < @{ $aRIBBoxExpr }; ++$i ) {
					if ( ref( $aRIBBoxExpr->[ $i ] ) ) {
						print STDERR 'EDITRIB: Error: RIBBOX: Expr: Not a SCALAR: Ignored' . $/;
						next;
					}
#					if ( $element =~ /$aRIBBoxExpr->[ $i ]/i ) {
#						$bElement = 1;
#						last;
#					}
					# "a,b,...,-x,-y,.."를 허용한다.
					if ( isInSet( $element, $aRIBBoxExpr->[ $i ] ) ) {
						$bElement = 1;
						last;
					}
				}
			}
		} else {
			# 모든 ELEMENT에 해당한다고 가정한다.
			$bElement = 1;
		}
		next unless $bElement;	# ELEMENT가 해당하지 않으면 다음을 조사한다.
		# 적어도 ELEMENT는 해당한다.

		my	$refType = ref( $aRIBBoxExpr->[ 1 ] );
		if (
			$refType &&		# "집합 표현 1"에서 '[', ']'가 생략된 경우도 아니고,
						# OBJECT가 하나일 때는 생략할 수 있다.
			$refType ne 'ARRAY'	# "집합 표현"을 말하는 ARRAY에 대한 참조도 아니다.
		) {
			print STDERR 'EDITRIB: Error: RIBBOX: Expr: "집합 표현 1"에서 \'[\', \']\'가 생략된 경우도 아니고, "집합 표현"을 말하는 ARRAY에 대한 참조도 아니다: 무시한다' . $/;
			next;
		}
		if ( $refType eq 'ARRAY' ) {
			return $RIBBoxExpr if ( isMatched( $object, $aRIBBoxExpr->[ 1 ] ) );
		} else {
#			return $RIBBoxExpr if $object =~ /$aRIBBoxExpr->[ 1 ]/i;
			# "A,B,...,-X,-Y,...:a,b,...,-x,-y,..."을 허용한다.
			my	@elementObject = split( ':', $aRIBBoxExpr->[ 1 ] );
			if ( @elementObject == 0 ) {	# ""인 경우
							# ".*:.*"으로 가정한다.
				# "OBJECT"만 주어진 경우이다.
				return $RIBBoxExpr;
			} elsif ( @elementObject == 1 ) {	# "..."또는 "...:"인 경우
				if ( $aRIBBoxExpr->[ 1 ] =~ /:$/ ) {	# "...:"인 경우
									# "...:.*"으로 가정한다.
					# "ELEMENT"만 주어진 경우이다.
					# 모든 "OBJECT"에 해당한다.
					return $RIBBoxExpr if isInSet( $element, $elementObject[ 0 ] );
				} else {				# " ..."인 경우
					return $RIBBoxExpr if isInSet( $object, $elementObject[ 0 ] );
				}
			} else {
			       if ( @elementObject > 2 ) {
				       print STDERR 'EDITRIB: Error: Expr: ":"가 둘 이상 있다: 세 번째부터는 무시한다' . $/;
			       }
			       # $elementObject[ 0 ]은 "ELEMENT"을 지정하는 집합표현이고,
			       # $elementObject[ 1 ]은 "OBJECT"를 지정하는 집합표현이다.
			       return $RIBBoxExpr if ( isInSet( $element, $elementObject[ 0 ] ) && isInSet( $object, $elementObject[ 1 ] ) );
			}	       
		}
	}
}
#	2007/4/30 (EST)
#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)


# 1. "RIB Directive"를 "enum"형으로 분류한다.
my	$ENUM_RIB_DIRECTIVE__NONE		= -1;	# "RIB Directive"가 정의되지 않았거나,
							# 아래 중의 하나가 아닐 때
my	$ENUM_RIB_DIRECTIVE__FRAME_BEGIN	= 0;
my	$ENUM_RIB_DIRECTIVE__FRAME_END		= 1;
my	$ENUM_RIB_DIRECTIVE__DISPLAY		= 2;
my	$ENUM_RIB_DIRECTIVE__WORLD_BEGIN	= 3;
my	$ENUM_RIB_DIRECTIVE__WORLD_END		= 4;
my	$ENUM_RIB_DIRECTIVE__ATTRIBUTE_BEGIN	= 5;
my	$ENUM_RIB_DIRECTIVE__ATTRIBUTE_END	= 6;
my	$ENUM_RIB_DIRECTIVE__TRANSFORM_BEGIN	= 7;
my	$ENUM_RIB_DIRECTIVE__TRANSFORM_END	= 8;
my	$ENUM_RIB_DIRECTIVE__LIGHTSOURCE	= 9;
my	$ENUM_RIB_DIRECTIVE__SURFACE		= 10;
my	$ENUM_RIB_DIRECTIVE__DISPLACEMENT	= 11;
my	$ENUM_RIB_DIRECTIVE__ATTRIBUTE		= 12;
my	$ENUM_RIB_DIRECTIVE__GEOMETRIC_PRIMITIVE	= 13;
# "Geometric Primitive"에는 다음과 같은 것들이 있다.
# "The RenderMan Interface Specification"의,
# "The RenderMan Interface" 5장 "Geometric Primitives"에 있는 내용을 근거로 한다.
my	%hGeometricPrimitive = (
	# 5.1 Polygons
	'Polygon'	=> 1,
	'GeneralPolygon'	=> 1,
	'PointsPolygons'	=> 1,
	'PointsGeneralPolygons'	=> 1,
	# 5.2 Patches
	'Patch'			=> 1,
	'PatchMesh'		=> 1,
	'NuPatch'		=> 1,
	'TrimCurve'		=> 1,
	# 5.3 Subdivision Surfaces
	'SubdivisionMesh'	=> 1,
	# 5.4 Quadrics
	'Sphere'		=> 1,
	'Cone'			=> 1,
	'Cylinder'		=> 1,
	'Hyperboloid'		=> 1,
	'Paraboloid'		=> 1,
	'Disk'			=> 1,
	'Torus'			=> 1,
	# 5.5 Point and Curve Primitives
	'Points'		=> 1,
	'Curves'		=> 1,
	# 5.6 Blobby Implicit Surfaces
	'Blobby'		=> 1,
	# 5.7 Procedural Primitives
	'Procedural'		=> 1,
	# 5.8 Implementation-specific Geometric Primitives
	'Geometry'		=> 1,
	# 더 있으나 현재로서는...
	# 문제가 생기면 그 때 추가한다.

	# Application Note #41에서 "HierarchicalSubdivisionMesh"에 대한 설명이 있다.
	# 다음 예제 파일에 사용 예가 있다.
	#	rattutorials/mtor/scenes/subsurface_scattering/sssc.ma
	'HierarchicalSubdivisionMesh'	=> 1,
);
# 2. WorldBegin...WorldEnd안에서 "라이트 쉐이더"가 시작하는 위치와
# "오브젝트"가 시작하는 위치를 찾는다.
# RIB Box를 구현하기 위해서 필요하다.
# 특히 "오브젝트"의 시작위치는 "bake"할 때 필요하다.
#	1) "라이트 쉐이더"는 "렌더링 이미지"에서만 나타난다.
#	2) 모든 "라이트 쉐이더"가 나타난 후에 "오브젝트"가 나타난다.
#	3) AttributeBegin...AttributeEnd안에서,
#	"RIB Directive"가 위의 "geometric primitive"중의 하나일 때,
#	"오브젝트"에 대한 AttributeBegin...AttributeEnd이다.
#	4) "Attribute"라는 "RIB Directive"를 만나서 "스택"의 "운명"이
#	CONTENT_PRINT일 지라도 아직 "라이트 쉐이더"나 "오브젝트"를 만나지 않았다면
#	CONTENT_SPOOL이 된다. CONTENT_SAVE처럼 "RIB"의 내용을 모으지만
#	"라이트 쉐이더"나 "오브젝트" 정보를 만나면 CONTENT_PRINT로 바뀐다.
#
#	2007/4/24 (EST)
#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)

# 1. Subframe단위로 RIB을 저장한다.
# 모든 subframe에 해당하는 RIB을 만든다.
#
# 2. FrameBegin전에 나오는 RIB의 부분("declare"부분이라 한다)을 모아,
# subframe으로 나눌 때 사용한다.
my	$bDeclare	= 1;	# 첫 번째 FrameBegin을 만나면 0으로 바뀐다.
my	$strDeclare	= '';
# 3. 이 프로그램에서 처리하는 "경로"에 대한 내용을 화면에 표시한다.
# 표시되는 "경로"는 다음과 같다.
#	1) 저장되는 RIB파일의 경로
#	2) 각 RIB파일에서 저장하는 이미지 파일의 경로
#	3) Subframe으로 나눌 때, subframe단위로 저장되는 RIB파일의 경로
# 다음 변수가, 명령줄에서 "-vPath"옵션으로, 설정되어 있다면 "경로"와 관련된 추가정보가 표시된다.
$bVerbosePath = 0;
#	2007/4/19 (EST)
#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)

# Deep shadow의 경우에는 "쉐도우 이미지"단계에서도 쉐이더를 참고한다.
#	2007/4/18 (EST)
#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)

# 1. 쉐이더가 적용되는 오브젝트를 "끝까지" 찾는다.
#	지금까지는 현재 스택아니면 바로 위 스택까지만 찾았다.
#	하지만 경우에 따라서 더 위에 있을 수 있다.
#	예를 들어, 많은 TransformBegin...TransformEnd로 쉐이더가 싸여있을 수 있다.
#	그래도 없는 경우, 해당 쉐이더는 그대로 둔다.
# 2. Deep shadow의 경우를 처리한다.
#	1) Deep shadow의 이미지 유형은 "shadow"가 아닌 "deepshad"이다.
#	2) Deep shadow는 두 번째 "Display"의 정의되어야 한다. ('+'문자와 함께)
#	첫 번째 "Display"는 대개 다음과 같다.
#		Display "null" "null" "z"
# 3. 프로그램에서 보내는 메시지는 다음 형태를 취한다.
#	EDITRIB: (Error|Warning|Info): ...
# 4. 각 "job chunk"마다 공통으로 사용되는 "Display"에 대한 정보를 따로 저장한다.
#
#	2007/4/17 (EST)
#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)

# Shadow가 없는 light는 무시한다.
#	push( @aShadowTexture, $HoALightSourceRef->{ $light }->[ 2 ] );
# ->
#	my	$shadowName = $HoALightSourceRef->{ $light }->[ 2 ];
#	push( @aShadowTexture, $shadowName ) if $shadowName;
#	2007/3/27 (KST)
#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)

# RIB파일이름을 parsing할 때 버그 수정
# CMD창에서 사용법:
#	c:\cygwin\bin\perl x:/bin/editRIB.pl -sif x:/bin/siFile1.pl -sif x:/bin/siFile2.pl -jcf x:/bin/jcFile2.pl "X:/home/icentrik/My Documents/maya/projects/default/rib/xxx.0001.rib"
#	2007/2/26 (KST)
#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)

# 'matte'를 구현한다.
# "쉐도우 이미지"에서는 $RS__DELETE와 같고,
# "렌더링 이미지"에서는 "matte 1"이라는 내용을 적어주면 된다.
# 다시 한 번 $RS__...내용을 적어본다.
#$RS__CAST_SHADOW	= 1 << 0;
#$RS__RECV_SHADOW	= 1 << 1;
#$RS__VISIBILITY		= 1 << 2;
#$RS__VIS_REFL		= 1 << 3;
#$RS__VIS_REFR		= 1 << 4;
#$RS__RT			= $RS__VIS_REFL | $RS__VIS_REFR;
#$RS__DELETE		= 1 << 31;	# 이 비트가 설정되어 있으면, 다른 비트는 무시된다.
$RS__MATTE		= 1 << 30;	# RS__DELETE와 마찬가지로, 이 비트가 설정되어 있다면 다른 비트는 무시된다.
					# 하지만 RS__DELETE비트의 우선 순위가 더 높다.
#
#	2007/2/21 (KST)
#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)

# JCFile의 형식에서, 'jcref'가 가리키는 해쉬의 'secondary'키에 대응하는 @AoH...의 구조가 바뀌었다.
# 전(前):
#	[
#	{
#	'name'	=> 'specular',
#	'channels'	=> [ '_specular' ]
#	},
#	......
#	]
#
# 후(後):
#	{
#	'specular' => [ '_specular' ],
#	......
#	}
#
# 'name'에 대응하는 값('specular')으로 해당 파일을 만드므로,
# 값이 유일해야 하고, 순서도 중요하지 않으므로
# %HoA...구조로 만든다.
#
#	2007/2/16 (KST)
#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)


# 1. 여러 개의 SIFile을 읽어들일 수 있도록 한다.
#	1) 각 SIFile에서 @AoH...를 선언한 후, 그 참조를 리턴한다.
#	다음은 한 예이다.
#
#	my	@AoHShaderInfoByArtForObjectX = ();
#
#	# @AoH...를 구성한다.
#	......
#
#	# 마지막에 의미없는 "1"대신에 구성한 @AoH...에 대한 참조를 리턴한다.
#	\@AoHShaderInfoByArtForObjectX;
#
#	2) editRIB에서는 그 참조를 얻어서 쉐이더정보에 쓰이는 @AoHShaderInfoByArt를 구성한다.
#	다음은 한 예이다.
#
#	my	@AoHShaderInfoByArt = ();
#	# 각 SIFile에서 쉐이더 정보를 얻는다.
#	for my $SIFile ( @aSIFile ) {
#		my	$aRef = require $SIFile;
#		# $aRef가 ARRAY에 대한 참조인지 확인한다.
#		foreach my $hRef ( @{ $aRef } ) {
#			# 각 성분이 다시 HASH에 대한 참조인지 확인한다.
#			push( @AoHShaderInfoByArt, $hRef );
#		}
#	}
#
#	3) 여러 SIFile은 "-sif"옵션을 반복적으로 사용하거나 아니면
#	하나의 "-sif"옵션에서 ','로 구분되어 '"'로 묶어서 입력된다.
#
#		-sif SIFile0 -sif SIFile1
#	또는
#		-sif "SIFile0,SIFile1" -sif SIFile2
#
#
# 2. 각 SIFile에 사용하는 전역변수의 정의와 사용법이 달라졌다.
#	%hGlobal을 통해서 변수와 해당 값을 정의하는 대신에
#	에전처럼 다음과 같은 단순한 방법을 사용한다.
#
#	my	$allKd = 1.0;
#	my	$Kd = 0.8;
#
#	만약 모든 SIFile에서 사용되기를 원한다면 "my"를 생략한다.
#
#	$TEXTURE_PATH = "//FileServer/TexturePath/"
#
#	물론 여러 SIFile에서 중복된게 정의되지 않도록 조심해야 한다.
#
#
# 3. 집합의 각 성분을 정의할 때 다음 방법을 사용할 수 있다.
#	qw()안에 각 성분을 넣으면 된다.
#
#		[ [ qw(.* a b) ], [ qw(x y z) ] ]
#
#	각 성분을 작은 따옴표로 묶고 각 성분을 쉼표로 분리하는,
#	똑같은 집합을 정의하는 이전의 방법과의 차이를 주목한다.
#
#		[ [ '.*', 'a', 'b' ], [ 'x', 'y', 'z' ] ]
#
#	어느 것을 사용할 수도 있다. 첫번째 방법이 더 낫다는 의견이 있다.
#
#
# 4. SIFile의 각 정의를 보면 'set'에 해당하는 오브젝트에 대해서 'content'의 내용이 치환된다.
# "Surface"쉐이더는 항상 존재해서 원하는 대로 바꿀 수 있지만,
# "Displacement"쉐이더는 그렇지 않다. 따라서 편법으로 'content'안에
# 두 쉐이더에 대한 정보를 써준다. 예를  들어,
#
#	'content' => qq{
#		Displacement "..."	# Displacement 쉐이더
#			......		# 쉐이더 파라미터
#		Surface "..."		# Suface 쉐이더
#			......		# 쉐이더 파라미터
#	}
#
# 주 1) Displacement쉐이더만을 치환하고자 하는 경우,
# RIB에서 Displacement가 정의되어 있지 않을 때는 조금 복잡하다.
# 이런 경우는 취급하지 않는다.
# 주 2) Displacement는 항상 Surface앞에 둔다.
# 왜냐하면 $RS__RECV_SHADOW가 설정되어 있지 않았을 때,
# Surface쉐이더 뒤에 특별한 파라미터가 붙기 때문이다.
#	"string NotUsedShadowParameterName" [ "shadowname" ]	# <--- 이 부분이 써진다.
# (2007/1/26의 노트를 참고한다.)
# 주 3) 하지만 실제로는 Surface쉐이더와 Displacement쉐이더를 구분하지 않고,
# Surface쉐이더에서 Displacement까지 같이 처리하기 때문에
# Displacement쉐이더를 따로 생각할 필요가 없다.
#
#
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
my	@aSIFile = ();

# 1. 지금까지는 CHUNK_TYPE__DEL인 경우와 CHUNK_TYPE__LL인 경우를 독립적으로 처리했다.
# Light Linking(CHUNK_TYPE__LL)을 한 후에,
# CHUNK_TYPE__DEL을 포함한, 오브젝트를 처리하는 "적절한" 일을 할 수 있도록 한다.
# 이러한 "job chunk"유형을 CHUNK_TYPE__RS라 부르기로 한다.
# 더 이상 CHUNK_TYPE__DEL, CHUNK_TYPE__LL은 사용되지 않는다!
$CHUNK_TYPE__SHD	= 1;
$CHUNK_TYPE__RS		= 2;
$CHUNK_TYPE__BAKE	= 1 << 8;
#
# 1.1. CHUNK_TYPE__SHD은 예전과 동일하다.
# $HoHChunk{ 'SHD' } = {
#	'type'	=> CHUNK_TYPE__SHD,
#	'siref'	=> \@AoHShaderInfoByTd,
#	'jcref'	=> [
#		{
#			'light set'	=> [ ... ],
#			'give set'	=> [ ... ],
#			'take set'	=> [ ... ]
#		},
#		......
#	]
# };
# 1.2. CHUNK_TYPE__RS의 예
# $HoHChunk{ 'RSExample' } = {
#	'type'	=> CHUNK_TYPE__RS,
#	'siref'	=> \@AoHShaderInfoByArt,
#	# CHUNK_TYPE__SHD와 달리, Light Linking에 관한 정보를 담은 키(llref)가 존재한다.
#	# 기본값은 '모든 라이트는 모든 오브젝트에 비춘다'이다.
#	'llref'	=> [
#		{
#			'light set'	=> [ ... ],
#			'object set'	=> [ ... ]
#		},
#		......
#	],
#	# CHUNK_TYPE__SHD와 달리, "[...]"이 아니라 "{...}"이다!
#	'jcref'	=> {
#		# Primary이미지를 얻을 때 사용되는 Render Stats에 관한 정보
#		# 여기서 설정되어있는 정보가 있을 때,
#		# 그 정보를 참고로 하여 오브젝트를 수정한다.
#		# 아무런 정보가 없다면 즉 "[]"라면 모든 오브젝트는 그대로 둔다.
#		'primary'	=> [
#			{
#				'object set'	=> [ ... ],
#				'Render Stats'	=> RS__XXX	# 아래 2.의 RS__XXX들의 '|'를 이용한 조합
#								# 단, RS__DELETE의 경우 다른 비트는 무시된다.
#			},
#			......
#		],
#		# Secondary이미지에 사용되는 channel에 관한 정보
#		# 여기서 설정된 정보에 따라 추가 이미지를 만든다.
#		# 추가 이미지를 원하지 않을 때는 "[]"로 설정한다.
#		'secondary'	=> [
#			{
#				'name'		=> 'diffuse',
#				'channels'	=> [ '_diffuse', ... ]	# 적어도 하나는 있어야 한다.
#			},
#			......
#		]
#	}
# };
#
#
# 2. 1.의 "적절한" 일이란 마야의 "Render Stats"에 나타난 것을 처리하는 것을 뜻한다.
# 그 중에서 다음 경우를 처리한다.
#
#	Casts Shadows (RS__CAST_SHADOW)
#	Receive Shadows (RS__RECV_SHADOW)
#	Primary Visibility (RS__VISIBILITY)
#	Visible in Reflections (RS__VIS_REFL)
#	Visible in Refractions (RS__VIS_REFR)
#
#	Delete (RS__DELETE)
$RS__CAST_SHADOW	= 1 << 0;
$RS__RECV_SHADOW	= 1 << 1;
$RS__VISIBILITY		= 1 << 2;
$RS__VIS_REFL		= 1 << 3;
$RS__VIS_REFR		= 1 << 4;
$RS__RT			= $RS__VIS_REFL | $RS__VIS_REFR;
$RS__DELETE		= 1 << 31;	# 이 비트가 설정되어 있으면, 다른 비트는 무시된다.
#
# 이 중에서 몇 가지(Receive Shadows, Visible in Refractions)는
# RIB자체의 처리만으로는 가능하지 않고,
# 오브젝트에서 사용되는 쉐이더와 "적절하게" 연동되어야 가능하다.
# (아래의 RS__RECV_SHADOW의 경우를 보라.)
#
# 2.1. RS__CAST_SHADOW
# RS__CAST_SHADOW = off의 경우, 쉐도우를 만들 때
# 해당 오브젝트를 정의하는 AttributeBegin...AttributeEnd를 삭제한다.
# 주) RS__CAST_SHADOW = on의 경우 쉐도우를 만들 때 아무 것도 하지 않는다.
#
# 2.2. RS__RECV_SHADOW의 경우,
# Surface쉐이더 안에서 illuminance를 통해 사용하는 light쉐이더에게
# 특정한 상황이니 쉐도우 맵이 있더라도 그 정보를 사용하지 않도록 해야 한다.
# [방법 1]
# Surface쉐이더:
#	surface SurfaceShader(
#		...,
#		float		UseShadow	= 1;		// RIB에서 적절하게 설정한다.
#		output float	_UseShadow	= 1;		// Light쉐이더에서 이 값을 참조하도록 한다.
#		...
#	)
#	{
#		...
#		_UseShadow = UseShadow;
#		...
#	}
# Light쉐이더:
#	light LightShader(
#		...
#		string ShadowMap = "";
#		...
#	)
#	{
#		...
#		uniform color	ShadowColor;
#		if (ShadowMap == "") {
#			ShadowColor = 0;
#		} else {
#			uniform float	UseShadow = 0;
#			surface("_UseShadow", UseShadow);
#			if (UseShadow == 1) {
#				ShadowColor = 0;
#			} else {
#				ShadowColor = color shadow(ShadowMap, ...);
#			}
#		}
#		...
#	}
# RIB: 쉐도우를 받지 않는 오브젝트에서
#	surface "..."
#		...
#		"float UseShadow" [ 0 ]		# 0: RECV_SHADOW = off, 1: RECV_SHADOW = on
#		...
#
# 주) Light쉐이더에서 설정한 "__nondiffuse", "__nonspecular"를,
# surface쉐이더에서 이용하는 것과 같은 방법을 사용하는 것이다.
#
# [방법 2] Surface쉐이더 안에서 illuminance를 호출할 때,
# light쉐이더에가 파라미터를 보낼 수 있다.
#
#	illuminance(
#		...
#		"send:light:ParameterName", ParameterValue,
#		...
#	);
#
# Light쉐이더가 사용하는 쉐도우 맵이 없다고 한다.
# 예를 들어, mtorSpotLight의 경우 쉐도우 맵에 사용하는 파라미터의 이름은 "shadowname"이므로,
# Surface쉐이더:
#	illuminance(
#		...
#		"send:light:shadowname", "",
#		...
#	);
# 마치 쉐도우 맵이 없는 것처럼 light쉐이더가 사용된다.
#
# 주) Surface쉐이더만 고치면 되므로 이 방법을 사용한다.
#
# 2.3. RS__VISIBILITY의 경우
# 렌더링 이미지를 만드는 FrameBegin...FrameEnd안의 해당 오브젝트에서
#
#	Attribute "visibility" "int camera" [ 0 ]	# 0: RS__VISIBILITY = off, 1: RS__VISIBILITY = on
#
# Ray traceing옵션이 꺼져있다면, RS__VISIBILITY가 off일 때,
# 오브젝트를 정의하는 AttributeBegin...AttributeEnd를 삭제한다.
# 그렇지 않으면 다음 경고 메시지가 나타난다.
#	Z10002 Primitive |nurbsCone1|nurbsConeShape1 is not visible to camera or traced rays. (WARNING)
# 필요없는 내용으로 RIB의 크기가 늘어난다는 것만 제외하고 무시해도 된다.
#
# 2.4. RS__VIS_REFL, RS__VIS_REFR의 경우
# Ray tracing의 subset을 이용한다.
# Surface쉐이더:
#	surface SurfaceShader(
#		...
#		string	subset;
#		...
#	)
#	{
#		...
#		gather(
#			...
#			"subset", subset,
#			...
#		);
#		...
#	}
# RIB:
#	Attribute "grouping" "string membership" [ "+ReflSet,RefrSet" ]	# RS__VIS_REFL = on, RS__VIS_REFR = on인 경우
#	...
#	Surface "SurfaceShader"
#		...
#		"string subset" [ "" ]
#		...
#
# 주 1) Ray tracing이 설정되어있을 때만 유효하다.
# 주 2) 렌더링 이미지를 만드는 FrameBegin...FrameEnd안의 해당 오브젝트에서
#
#	Attribute "visibility" "int trace" [ 1 ]
#
# "1"로 설정되어 있어야 한다.
# 주 3) RS__VIS_REFL = off, RS__VIS_REFR = off인 경우,
# 오브젝트를 정의하는 AttributeBegin...AttributeEnd를 삭제한다.
# 그렇지 않으면 VISIBILITY에서와 같은, 다음 경고 메시지가 나타난다.
#	Z10002 Primitive |nurbsCone1|nurbsConeShape1 is not visible to camera or traced rays. (WARNING)
# 역시 필요없는 내용으로 RIB의 크기가 늘어난다는 것만 제외하고 무시해도 된다.
# 주 4) subset에 대해서...
#	. subset이 ""이라면 모든 오브젝트가 고려된다.
#	. subset이 ""이 아니고 해당하는 subset이 정의되지 않았다면
#	쉐이더가 사용된 오브젝트만이 고려된다.
#	. 하나 이상의 subset은 ','로 연결한다.
#
# 2.6. RS__DELETE의 경우
# 어느 FrameBegin...FrameEnd든지
# 오브젝트를 정의하는 AttributeBegin...AttributeEnd를 삭제한다.
# Ray tracing이 켜져있는 경우에는
#	RS__CAST_SHADOW = off, RS__VISIBILITY = off, RS__VIS_REFL = off, RS__VIS_REFR = off
# 와 꺼져있는 경우는
#	RS__CAST_SHADOW = off, RS__VISIBILITY = off
# 와 동일하다.
# 주) 마야의 "Render Stats"에 있지 않은 옵션이다.
#
#
# 3. "RenderMan Global"에 있는,
# channel에 해당하는 secondary이미지를 만들 수 있도록 한다.
# 렌더링 이미지를 만드는 FrameBegin...FrameEnd안에서
# Display를 다음처럼 사용한다.
#
#	Display "+..." "tiff" "_diffuse,_specular"	# '+'가 있음에 주의한다.
#
# 주 1) 위 처럼 한 번에 여러 channel의 정보를 얻고 싶다면 좌우 공백없이 ','를 사용한다.
# 주 2) 물론 오브젝트에서 사용하는 쉐이더에서 channel에 해당하는 값을,
# output을 통해서 설정해야 한다. 위의 경우, 예를 들어, surface쉐이더에서,
#	surface SurfaceShader(
#		...
#		output color	_diffuse = 0;
#		output color	_specular = 0;
#		...
#	)
#	{
#		...
#		color	DiffuseColor = color ...;
#		color	SpecularColor = color ...;
#		...
#		_diffuse = DiffuseColor;
#		_specular = SpecularColor;
#		...
#	}
#
#	2007/1/26 (KST)
#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)
#

# RS__RECV_SHADOW, RS__VIS_REFL, RS__VIS_REFR은 쉐이더와 함께 사용되어야 한다.
# 따라서 쉐이더는 이를 처리할 수 있는 다음과 같은 파라미터와 기본값은 가져야 한다.
# 1. RS__RECV_SHADOW
#
#	"string NotUsedShadowParameterName" [ "" ]
#
# Light쉐이더마다 "쉐도우 맵"에 해당하는 파라미터이름이 다를 수 있다.
# (mtorSpotLight의 경우에 "shadowname"이다.)
# 이 값이 주어지지 않으면 기본값은 ""이므로,
# light쉐이더에 주어진 모든 "쉐도우 맵"을 그대로 사용한다.
# 만약 RS__RECV_SHADOW를 끈다면 surface쉐이더에
# light쉐이더에서 사용하는 "쉐도우 맵"에 해당하는 파라미터의 이름을 적어준다.
# 현재는 "mtorSpotLight"쉐이더를 사용한다고 가정하고,
# 항상 다음과 같이 surface쉐이더의 마지막에 쓸 것이다.
#	Surface "SurfaceShader"
#		......
#		"string NotUsedShadowParameterName" [ "shadowname" ]	# <--- 이 부분이 써진다.
# 나중에 여러 파라미터를 받아들이도록 수정할 것이다.
#
# 2. RS__VIS_REFL, RS__VIS_REFR
#
#	string	ReflSubset = "RManReflSubset";
#	string	RefrSubset = "RManRefrSubset";
#
# Ray tracing이 켜진 상태에서 모든 오브젝트는
# "RManReflSubset", "RManRefrSubset"에 속한다.
# "렌더링 이미지"를 처리하는 FrameBegin...FrameEnd안에서,
# 'stack index'가 0인 경우에 다음과 같은
#	Attribute "trace" "int maxspeculardepth" [2]
# "RIB Directive"를 만나면, 바로 앞에 다음과 같이
#	Attribute "grouping" "string membership" ["+RManReflSubset"]
#	Attribute "grouping" "string membership" ["+RManRefrSubset"]
# 모든 오브젝트가 두 "subset"에 속하게 한다.
# 이후, 각 오브젝트에 해당하는 AttributeBegin...AttributeEnd안에서
# RS__VIS_REFL이나 RS__VIS_REFR가 꺼진 상태일 때에만
#	Attribute "grouping" "string membership" ["-RManReflSubset"]
#	Attribute "grouping" "string membership" ["-RManRefrSubset"]
# 처럼, 해당 "subset"에서 제외시킨다.
# 쉐이더에서 사용하는 "string ReflSubset"이나 "string RefrSubset"의 기본값이
# "RManReflSubset"이나 "RManRefrSubset"이므로,
# 쉐이더를 바꾸지 않아도 원하는 결과를 얻는다.
#
#	2007/1/26 (KST)
#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)


$bVerbose = 0;
$bVVerbose = 0;
$bVVVerbose = 0;

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

my	($retVal, $jcFile, $ribFileHandle, $ribFile) = parseCommandLine();
unless ( $retVal ) {
	print STDERR 'Usage:' . $/;
	print STDERR "\t" . $0 . ' [-sif ".../siFile.pl"] [-jcf ".../jcFile.pl"] [-k byArt] [-d add/Path/Expr | -R /Root/Path/Expr] x.rib' . $/;
	exit 1;
}


$byArtKeyName	= 'byArt' unless $byArtKeyName;
$bShd		= 0 unless $bShd;
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

$addPathExpr	= '$keyChunk' unless $addPathExpr;
$rootPathExpr	= '' unless $rootPathExpr;
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
# 4. 최종적으로 해석된 경로가 존재하지 않을 수 있으므로,
# 아래의 "mkdir2"함수를 써서 중간에 없는 모든 경로를 만든다.
# 예를 들어, '...x/a/b/c/'라고 해석된 최종 경로에서, '...x/'만 존재한다면
# 그 중간에 있는 '...x/a/', '...x/a/b/', '...x/a/b/c/'를 차례로 만든다.
#
# 주어지는 모든 "UNIX" 경로를 만든다.
# 중간에 문제가 생기면 프로그램을 종료한다. ("die"를 써서)
# 성공적으로 모든 경로를 만들면 "정리된" 총 경로를 리턴한다.
# "정리된" 경로라 함은 처음에 있는 '//'를 제외한 중간에 있는 모든 '//+'은 '/'로 바뀌고,
# 제일 마지막에 '/'가 있다.
#
#	2007/1/10 (EST)
#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)
sub mkdir2
{
	my	@aDir = split( m|/|, $_[ 0 ] );
	my	@eDir = ();
	foreach my $eDir ( @aDir ) {
		push( @eDir, $eDir ) if $eDir;
	}
	# 제일 앞에 '/'(로컬 UNIX경로)나 '//'(네트워크 경로)가 있다면 그 경로는 유지한다.
	my	$DirUNIXPath = $_[ 0 ] =~ m/^(\/\/?)/ ? $1 : '';
	foreach $Dir ( @eDir ) {
		$DirUNIXPath .= $Dir . '/';
#		print STDERR '{' . $DirUNIXPath . '}' . ': ' . ( -d $DirUNIXPath ? 'Yes' : 'No' ) . $/;
		next if -d "$DirUNIXPath";
		mkdir ( $DirUNIXPath ) || die '{mkdir ' . $DirUNIXPath . '}: Failure: ' . $!;
	}
	return $DirUNIXPath;
}


# 명령어(editRIB.pl) 다음에 주어지는 인자(argument)를 사용하여,
# 네 값($retVal, $siFile, $jcFile, $ribFileHandlel)을 설정한다.
# $retVal는 처리상황(1: 성공, 0: 실패)이다.
# 실패일때는 나머지 값은 모두 undef이다.
sub parseCommandLine
{
	my	($jcFile, $ribFile);

	for (my $i = 0; $i < @ARGV; ++$i) {
		my	$argv = $ARGV[ $i ];

		if ( $argv eq '-ascii' ) {
			$bAscii = 1;
			next;
		}

		if ( $argv eq '-vRIBBox' ) {
			$bVerboseRIBBox = 1;
			next;
		}
		if ( $argv eq '-vPath' ) {
			$bVerbosePath = 1;
			next;
		}
		if ( $argv eq '-v' ) {
			$bVerbose = 1;
			next;
		}
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
				push( @aSIFile, $SIFileTmp);
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
				push( @aRBFile, $RBFileTmp);
			}

			# 다음 인자를 해석한다.
			next;
		}
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

			$byArtKeyNameTmp = $ARGV[ ++$i ];
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
		if ( $argv eq '-bShd' ) {
			$bShd = 1;
			next;
		}
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

	# "추가경로 표현"가 주어지지 않았다면 기본값은 '$keyChunk'이다.

	# RIB 파일을 지정해야 한다.
	unless ( $ribFile ) {
		print STDERR 'EDITRIB: Error: RIB 파일을 지정해야 한다' . $/;
		return 0;
	}
	$ribFile =~ s|\\|/|g;

	# $ribFile에 해당하는 파일을 읽기모드로 열고 "file handle"을 얻는다.
	my	$ribFileHandle = 'RIBFH';
#	unless (open( $ribFileHandle, '< ' . $ribFile )) {
#		print STDERR 'EDITRIB: Error: |' . $ribFile . '|: 읽기모드로 열 수 없다' . $/;
#	};
	if ( $bAscii ) {
		open( $ribFileHandle, '< ' . $ribFile ) ||
			die 'EDITRIB: Error: |' . $ribFile . '|: 읽기모드로 열 수 없다';
	} else {
		open( $ribFileHandle, 'catrib "' . $ribFile . '" |' ) ||
			die 'EDITRIB: Error: |' . $ribFile . '|: 읽기모드로 열 수 없다';
	}

	return (1, $jcFile, $ribFileHandle, $ribFile);
}


=head1 No More Use

# WINDOWS에서 사용될 수 있도록 포팅된
# 마야, 렌더맨, cygwin과 같은 UNIX용 프로그램처럼 UNIX경로를 사용한다.
# 예를 들어, CMD창에서
#	c:\cygwin\bin\perl.exe "T:/home/icentrik/My Documents/maya/projects/default/chkRIB.pl" < untitled.0001.rib > NUL
#print STDERR "Info: \$0: {$0}\n";
$0 =~ /(.*\/)/;
push(@INC, $1) if $1;

require "getLineByRIBDirectiveSub.pl";

=cut

# getLineByRIBDirectiveSub-1.0.0-2
{
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
	'HierarchicalSubdivisionMesh'	=> 1,

	'ArchiveBegin'	=> 1,
	'ArchiveEnd'	=> 1,
	'ArchiveRecord'	=> 1,
	'AreaLightSource'	=> 1,
	'Atmosphere'	=> 1,
	'Attribute'	=> 1,
	'AttributeBegin'	=> 1,
	'AttributeEnd'	=> 1,
	'Basis'	=> 1,
	'Begin'	=> 1,
	'Blobby'	=> 1,
	'Bound'	=> 1,
	'Clipping'	=> 1,
	'ClippingPlane'	=> 1,
	'Color'	=> 1,
	'ColorSamples'	=> 1,
	'ConcatTransform'	=> 1,
	'Cone'	=> 1,
	'Context'	=> 1,
	'CoordinateSystem'	=> 1,
	'CoordSysTransform'	=> 1,
	'CropWindow'	=> 1,
	'Curves'	=> 1,
	'Cylinder'	=> 1,
	'Declare'	=> 1,
	'DepthOfField'	=> 1,
	'Detail'	=> 1,
	'DetailRange'	=> 1,
	'Disk'	=> 1,
	'Displacement'	=> 1,
	'Display'	=> 1,
	'DisplayChannel'	=> 1,
	'Else'	=> 1,
	'ElseIf'	=> 1,
	'End'	=> 1,
	'ErrorHandler'	=> 1,
	'Exposure'	=> 1,
	'Exterior'	=> 1,
	'Format'	=> 1,
	'FrameAspectRatio'	=> 1,
	'FrameBegin'	=> 1,
	'FrameEnd'	=> 1,
	'GeneralPolygon'	=> 1,
	'GeometricApproximation'	=> 1,
	'Geometry'	=> 1,
	'GetContext'	=> 1,
	'Hider'	=> 1,
	'Hyperboloid'	=> 1,
	'Identity'	=> 1,
	'IfBegin'	=> 1,
	'IfEnd'	=> 1,
	'Illuminate'	=> 1,
	'Imager'	=> 1,
	'Interior'	=> 1,
	'LightSource'	=> 1,
	'MakeCubeFaceEnvironment'	=> 1,
	'MakeLatLongEnvironment'	=> 1,
	'MakeShadow'	=> 1,
	'MakeTexture'	=> 1,
	'Matte'	=> 1,
	'MotionBegin'	=> 1,
	'MotionEnd'	=> 1,
	'NuPatch'	=> 1,
	'ObjectBegin'	=> 1,
	'ObjectEnd'	=> 1,
	'ObjectInstance'	=> 1,
	'Opacity'	=> 1,
	'Option'	=> 1,
	'Orientation'	=> 1,
	'Paraboloid'	=> 1,
	'Patch'	=> 1,
	'PatchMesh'	=> 1,
	'Perspective'	=> 1,
	'PixelFilter'	=> 1,
	'PixelSamples'	=> 1,
	'PixelVariance'	=> 1,
	'Points'	=> 1,
	'PointsGeneralPolygons'	=> 1,
	'PointsPolygons'	=> 1,
	'Polygon'	=> 1,
	'Procedural'	=> 1,
	'Projection'	=> 1,
	'Quantize'	=> 1,
	'ReadArchive'	=> 1,
	'RelativeDetail'	=> 1,
	'Resource'	=> 1,
	'ResourceBegin'	=> 1,
	'ResourceEnd'	=> 1,
	'ReverseOrientation'	=> 1,
	'Rotate'	=> 1,
	'Scale'	=> 1,
	'ScopedCoordinateSystem'	=> 1,
	'ScreenWindow'	=> 1,
	'ShadingInterpolation'	=> 1,
	'ShadingRate'	=> 1,
	'Shutter'	=> 1,
	'Sides'	=> 1,
	'Skew'	=> 1,
	'SolidBegin'	=> 1,
	'SolidEnd'	=> 1,
	'Sphere'	=> 1,
	'SubdivisionMesh'	=> 1,
	'Surface'	=> 1,
	'System'	=> 1,
	'TextureCoordinates'	=> 1,
	'Torus'	=> 1,
	'Transform'	=> 1,
	'TransformBegin'	=> 1,
	'TransformEnd'	=> 1,
	'TransformPoints'	=> 1,
	'Translate'	=> 1,
	'TrimCurve'	=> 1,
	'WorldBegin'	=> 1,
	'WorldEnd'	=> 1,
);

my	$RIBDirective;		# 현재 진행 중인 "RIB Directive"이다.
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
	$RIBDirective	= '';
	@RIBDirectiveContent = ();

	# 최근에 읽어들인, 새로운 "RIB Directive"에 해당하는 줄이 있다면
	# 거기서부터 출발한다.
	if ($lastRIBDirective) {
		$RIBDirective = $lastRIBDirective;
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

		if (defined($Token) && defined($hRIBDirective{ $Token })) {
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
				$lastRIBDirectiveLine = $_;
				# 다음에 이 함수를 호출하면
				# 방금 읽어들인 새로운 "RIB Directive"에서부터 시작한다.
				return (1, $RIBDirective, \@RIBDirectiveContent);

			}

			# 현재 읽은 줄은 새로운 "RIB Directive"의 시작이 된다.
			push(@RIBDirectiveContent, $_);
			$RIBDirective = $Token;		# "RIB Directive"를 기억해야 한다!
							# 그렇지 않으면
							# 첫 번째 줄에 있는 유효한 "RIB Directive"를 잃는다!
							#	2007/5/3 (EST)
							#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)
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
					$lastRIBDirective = undef;
					$lastRIBDirectiveLine = undef;
					return (2, undef, [ $_ ]);
				} else {
					# [경우 2.2.2]: 알 수 없는 줄이다.
					# 더 이상 진행하면 안 된다.
					# 아니면 무시할 수도 있다.
					$lastRIBDirective = undef;
					$lastRIBDirectiveLine = undef;
					return (-1, undef, [ $_ ]);
				}
			}
		}
	}
	if (@RIBDirectiveContent) {	# 이전에 모은 "RIB Directive"내용이 있다면
		# 그것을 리턴한다.
		$lastRIBDirective = undef;
		$lastRIBDirectiveLine = undef;
		return (1, $RIBDirective, \@RIBDirectiveContent);

	}
	return (0, undef, undef);
}
}


#### FrameBegin과 FrameEnd사이에 있는 정보만을 생각한다.
#### 따라서 하나의 RIB파일 안에 여러 개의 FrameBegin과 FrameEnd를 가진 경우는
#### 적절하게 원하는 부분을 선택해서 여기서 만든 함수를 이용한다.
#### 아래의 '# x 100'까지는 모든 작업에 동일하다.
#### 즉 World안에서 처리하는 일이 작업마다 다르다.
####	2006/11/16 (EST)
####	김광호 (Gwang-Ho Kim, iccentric@gmail.com)
#my	$bFrame;	# FrameBegin과 FrameEnd사이에 있는 지를 말해준다.
			# FrameBegin, FrameEnd을 만났을 때 각각 1, 0으로 초기화된다.
#my	$bWorld;	# WorldBegin과 WorldEnd사이에 있는 지를 말해준다.
			# WorldBegin, WorldEnd을 만났을 때 각각 1, 0으로 초기화된다.
			# Frame안에 있을 때에만 의미가 있다.
# Frame도 하나의 "스택"으로 처리한다.
# $iStack의 값이 Frame의 안($iStack != -1)인지 밖인지($iStack = -1)를 말해준다.
# 따라서 $bFrame 변수가 필요없다.
# $bWorld변수는 특별히 사용될 것 같지 않다.
# Display라는 "RIB Directive"는 World밖에서 정의되어야 하지만,
# 그걸 굳이 확인할 필요가 없을 것 같다.
#	2006/11/28 (EST)
#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)

#### AttributeBeing...AttributeEnd나 TransformBegin...TransformEnd의 스택을 이용한 방식을 처리하기 위하여
#### 전체적인 구조를 새로 만든다.
####	2006/11/18 (EST)
####	김광호 (Gwang-Ho Kim, iccentric@gmail.com)
#### 데이터구조를 구조체(여기서는 해쉬)를 이용하여 새로 만든다.
####	2006/11/20 (EST)
####	김광호 (Gwang-Ho Kim, iccentric@gmail.com)
# 때에 따라 Frame자체를 사용하지 않을 수도 있다.
# 특정 라이트에 대한 쉐도우 이미지를 얻지 않은 경우가 그 예이다.
# 최상위 스택을 이를 위해 사용한다. ($STATCK_TYPE__FRAME)
# 해당 스택의 운명은 Display를 만나면 결정된다.
# 대신 없앨 것(CONTENT_DELETE)이냐 출력할 것(CONTENT_PRINT)이냐 두 가지만 존재한다.
#	2006/11/28 (EST)
#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)
# 각 "작업 chunk"마다 @AoHStack, $iStack정보를 보관해서,
# 각 "작업 chunk"마다 적절하게 RIB내용을 수정해서 출력한다.
#	2006/11/29 (EST)
#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)
#my	@AoHStack;	# AttributeBegin, AttributeEnd나 TransformBegin, TransformEnd가 그럿듯이
			# Attribute, Transform안의 내용을 관리하는 스택
			# 각 성분은 다음과 같은 정보를 담은 해쉬이다. (AoH: Array of Hashes)
			#	type: Attribute(0)또는 Transform(1)
my	$STACK_TYPE__ATTRIBUTE	= 0;	# 상수(constant)
my	$STACK_TYPE__TRANSFORM	= 1;	# 상수(constant)
my	$STACK_TYPE__FRAME	= 2;	# 상수(constant)	# 2006/11/28 (NEW)
			#	name: Attribute라는 "RIB Directive"를 만났을 때 정해지는 값
			#		Attribute안(type = 0)에 있을 때,
			#		오브젝트의 쉐이프(shape)노드의 이름을 말해준다.
			#		이 때부터 현재 스택의 "운명"이 결정된다.
			#		자세한 것은 아래의 fate에 대한 설명을 참고한다.
			#	content: Attribute나 Transform안의 내용
			#		즉 AttributeBegin...AttributeEnd또는 TransformBegin...TransformEnd안의 내용
			#		정확히는 fate값이 CONTENT_DELETE이 아닐 때에만 내용을 담고 있다.
			#		자세한 것은 아래의 fate에 대한 설명을 참고한다.
my	$CONTENT_DELETE	= -1;	# 상수(constant)
my	$CONTENT_PRINT	= 0;	# 상수(constant)
my	$CONTENT_SAVE	= 1;	# 상수(constant)
			#	fate: 현재($iStack번째) 스택에 RIB의 내용을 저장할 지를 결정
			#		CONTENT_DELETE: 현재 스택의 내용은 무시된다.
			#		CONTENT_PRINT: 현재 스택의 내용은 저장되지 않고 화면또는 파일에 출력된다.
			#			일단 저장하고 현재 스택이 닫힌 후(...End를 만났을 때)에
			#			그 운명을 결정할 수도 있으나, 오브젝트의 쉐이프(shape)정보가
			#			제일 큰 크기를 차지하는 데 이를 메모리에 저장하는 것은
			#			적절하지 못하기 때문에 곧바로 출력할 수도 있도록 하기위해서
			#			이 값을 도입했다.
			#		1(저장): 현재 스택의 내용이 content변수에 저장된다.
			#### 1. AttributBegin이나 TransformBegin을 만나 하위 단계의 스택을 시작할 때
			#	1) 상위 단계의 스택 즉 "부모(parent) 스택"의 fate값이 CONTENT_DELETE이면
			#	하위 단계의 스택 즉 "자식 스택"의 fate값도 CONTENT_DELETE이다.
			#	2) "부모 스택"의 fate값이 CONTENT_DELETE이 아니라면
			#	"자식 스택"의 초기값(...Begin을 만났을 때)은 CONTENT_SAVE이다.
			#	왜냐하면 "자식 스택"의 Attribute이름에 따라 "운명"이
			#	"부모 스택"과 달라질 수 있기 때문이다.
			#	이름이 지워져야할 오브젝트의 이름이라면 fate값이 CONTENT_DELETE로 바뀌고,
			#	지금까지 저장한 내용을 모두 버리고, 주어진 스택에서는 어떤 내용도 모으지 않는다.
			#	그렇지 않고 살아남는 오브젝트라면 "부모 스택"의 fate값을 따른다.
			#	즉 "부모 스택"의 fate값이 CONTENT_PRINT이였다면 지금까지 모은 내용을 모두 출력하고,
			#	주어진 스택에서는 모든 내용을 출력한다.
			#	"부모 스택"의 fate값이 CONTENT_SAVE이였다면 계속해서 내용을 모은다.
			#	대부분의 경우 Attribute이름은 제일 처음에 주어지므로
			#	"자식 스택"의 fate값의 변경도 곧바로 가능하게 된다.
			#	이후로 이 값은 고정이다.
			#### 2. AttributeEnd나 TransformEnd을 만나 현재 스택을 닫을 때
			#	1) "자식 스택"의 fate값이 CONTENT_SAVE이라면
			#	"자식 스택"에서 모은 내용은 "부모 스택"의 fate값(적어도 CONTENT_DELETE는 아니다.)에 따라 처리된다.
			#	CONTENT_PRINT이라면 모은 내용을 화면이나 파일에 출력하고,
			#	CONTENT_SAVE이라면 모은 내용을 "부모 스택"의 내용에 추가한다.(append)
			#### 3. 주어진 스택안에 쉐이더 정보가 있을 때
			#	1) fate값이 CONTENT_DELETE가 아니라면 (있다면) 새로운 쉐이더 정보로 바꾼다.
			#	왜냐하면 쉐이더 정보를 만나기 전에,
			#	어떤 형태로든 fate값은 고정이 되기 때문이다.
#my	$iStack = -1;	# 현재 관리되는 스택을 가리키는 인덱스
			# -1: 초기값으로 아직 스택을 처리 중이 아님을 뜻한다.
			# AttributeBegin이나 TransformBegin은 만나면 +1씩,
			# AttributeEnd나 TransformEnd를 만나면 -1씩 증가한다.
			## $bFrame을 삭제하고, Frame도 하나의 "스택"으로 처리했다.
			## 따라서 여기서 반드시 -1로 초기화해야 한다.
			##	2006/11/28 (EST)
			##	김광호 (Gwang-Ho Kim, iccentric@gmail.com)

# 프로그램이 동작하는 방식에 필요한 정보를 정리하기 전에,
# 이 프로그램을 어떤 "작업"을 할 수 있는 지,
# 그리고 각 "작업"이 이루어지기 위해서는 알아야 하는 RIB구조에 대한 정리를 한다.
#	2006/11/22 - 2006/11/?? (EST)
#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)
#
# 이 프로그램을 써서 다음 4가지 "작업"을 할 수 있다.
#	ART/TD 툴
#		[작업 0] 쉐이더 바꿔치기
#			기존의 쉐이더(ART: Surface, Displacement, TD: LightSource) 대신에
#			ART/TD부서에서 지정한 쉐이더를 사용한다.
#			
#	TD 툴
#		[작업 1] 특정 오브젝트를 제외하기
#			말 그대로 씬에 놓여있는 모든 오브젝트를 렌더링하지 않고,
#			특정 오브젝트만을 렌더링한다.
#		[작업 2] Light Linking 처리하기
#			보통은 씬에 놓여진 빛은 씬에 있는 모든 오브젝트에 비친다.
#			때에 따라 특정 빛은 특정 오브젝트에만 비친 경우를 생각한다.
#		[작업 3] A가 B에 드리운 그림자 얻기
#			오브젝트 A(하나 또는 여러 개)가 오브젝트 B(하나 또는 여러 개)에 드리운 그림자만을 얻는다.
#			자세한 방법은 아래의 "A가 B에 드리운 그림자 얻기"에 대한 설명을 보라.
# 각 내용을 설명하기 전에, 먼저 간단히 RIB구조에 대해서 알아본다.
# 여기서는 위의 4가지 "작업"과 관련된 사항
# 즉 어떤 식으로 RIB안에 오브젝트 정보가 들어있는 지에
# 이야기의 촛점을 맞춘다.
#
# 마야 씬(.ma/.mb)을 mtor을 써서 RIB을 만들 때,
# 두 가지 명령어(-cmd다음에 주어지는)가 있다.
# 하나는 genRib이고 다른 하나는 genWorklistd이다.
# 렌더맨의 설명서(제목: MTOR Scripting)에 있듯이,
# genRib은 씬의 모든 정보가 하나의 RIB에 들어간다.
# genWorklist는 Render Global중의 jobChunkSize에 따라 그렇지 않을 수도 있다.
# jobChunkSize를 subframe으로 한 후, genWorklist명령을 사용해서 RIB을 만들면
# RIB안에는 오직 하나의 FrameBegin, FrameEnd쌍이 존재한다.
# 나중에 기회가 되면 jobChunkSize를 subframe으로 해서 모든 이미지와 관련된 RIB을
# 독립적으로 관리할 수도 있을 것이다. (DB를 통해서)
#
# 여기서는 모든 정보가 하나의 RIB에 들어있는 경우
# 즉 RIB을 만들 때 genRib을 사용하는 경우에 대해서 설명할 것이다.
# 이 경우 RIB의 구조는 다음과 같다.
# (실제로 몇 개의 오브젝트와 라이트를 포함한 씬으로부터 나온 RIB을 보고
# 얻은 경험을 쓴 내용이다.)
# 관심이 있는 것은 빛이 만드는 그림자 이미지(실제로는 텍스쳐)와
# 이를 사용한 실제 씬의 렌더링 이미지이다.
# 내용과 관련없는 사항은 "......"로 표시했고,
# 각 "RIB Directive"의 다음에 "<-"를 써서 필요한 설명을 적었다.
#------------------------------------------------------------------------------#
#	......
#	FrameBegin 1		<- 렌더링하는 이미지에 대한 정보가 여기서부터 시작한다.
#				<- FrameBegin다음에는 프레임 수가 있어,
#				<- 몇 번째 프레임에 해당하는 것이라는 정보가 들어간다고 되어있는 데,
#				<- mtor을 사용해서 그렇게 되어있는 경우는 접해보지 못했다.
#				<- 중요한 것은 아래의 Display에 적혀있는 이미지 파일이름이다.
#				<- 그냥 무시한다.
#	......
#	Display ".../xxx.spotLightShape1.shd.0001.tex" "shadow" "z"
#				<- 만들어지는 이미지의 이름과 특성을 말해준다.
#				<- "shadow"와 "z"(z-depth)를 보면 이미지는 그림자임을 알 수 있다.
#				<- 해당 라이트의 이름은 spotLightShape1이다.
#	......
#	WorldBegin		<- 이제부터 실제로 씬을 구성하는 오브젝트에 대한 정보가 들어있다.
#		......
#		AttributeBegin	<- 오브젝트에 대한 정보가 들어있다.
#			......
#			Attribute "identifier" "name" ["|nurbsPlane1|nurbsPlaneShape1"]
#				<- 그 오브젝트의 이름을 말해준다.
#			......
#		AttributeEnd	<- AttributeBegin과 쌍을 이루는 것으로,
#				<- 여기까지가 특정 오브젝트에 대한 정보의 끝이다.
#		......
#		......		<- 다른 오브젝트가 있다면 같은 구조로 반복된다.
#		......
#	WorldEnd		<- WorldBegin과 쌍을 이루는 것으로,
#				<- 여기까지가 이미지를 구성하는 오브젝트에 대한 정보의 끝이다.
#	FrameEnd		<- FrameBegin과 쌍을 이루는 것으로,
#				<- 여기까지가 이미지에 대한 정보의 끝이다.
#	......
#	......			<- 또 다른 라이트가 있다면 위와 똑같은 구조가 반복된다.
#				<- 라이트가 만드는 그림자를 사용하지 않는다면
#				<- (당연하겠지만) 여기까지 어떤 내용도 없다.
#	......
#	FrameBegin 1		<- 제일 마지막을 구성하고, 반드시 존재하는, 실제로 렌더링되는 이미지이다.
#				<- 오브젝트의 정보뿐만아니라 렌더링하는 데 필요한 쉐이더 정보가 들어있다.
#	......
#	Display ".../xxx.0001.tif" "tiff" "rgba" ...
#				<- "tiff"는 이미지의 형식을,
#				<- "rgba"는 이미지의 채널(r: Red, g: Green, b: Blue, a: Alpha)을 지정한다.
#				<- 저장될 이미지의 이름은 "xxx.0001.tif"이다.
#	......
#	WorldBegin		<- 그림자의 경우처럼 오브젝트에 대한 정보의 시작이다.
#		......
#		TransformBegin	<- 라이트 쉐이더를 지정한다.
#			......
#			Attribute "identifier" "string name" ["spotLightShape1"]
#				<- "spotLightShape1"이라는 라이트에 대한 쉐이더이다.
#			......
#			LightSource ...
#				<- 라이트 쉐이더에 대한 정보이다.
#			......
#		TransformEnd	<- 다른 쌍과 마찬가지로 TransformBegin과 쌍을 이루는 것으로,
#				<- 여기까지가 라이트 쉐이더에 정보의 끝이다.
#		......
#		......		<- 다른 라이트가 있다면 위와 같은 구조를 반복한다.
#		......
#		AttributeBegin	<- 쉐도우와 마찬가지로 씬을 구성하는 오브젝트에 대한 정보이다.
#				<- 쉐도우와 다른 점은 각 오브젝트가 사용하는 쉐이더 정보가 있다는 것이다.
#				<- 또한 각 오브젝트를 기술하기 전에 씬에 있는 모든 라이트 정보를 먼저 기술해야 한다.
#			......
#			Attribute "identifier" "name" ["|nurbsPlane1|nurbsPlaneShape1"]
#				<- 오브젝트의 이름이다.
#			......
#			Illuminate "spotLightShape1" 0
#				<- Light Linking의 예로서,
#				<- 특정 라이트(spotLightShape1)이 이 오브젝트에 비추지 않는다면
#				<- 이처럼 적어준다.
#			......
#			Surface ...
#				<- 오브젝트가 사용하는 쉐이더 정보이다.
#				<- 오브젝트의 빛에 따른 색깔을 결정하는 Surface 쉐이더 이전에,
#				<- 그 쉐이더에서 참고하는 라이트 쉐이더에 대한 정보가 위와 같이 있어야 한다.
#				<- 좀 더 자세히 설명하면
#				<- 라이트 쉐이더는 Illuminate를 통해서 빛에 대한 정보를 주고,
#				<- 그 빛을 사용하는 Surface 쉐이더에서는 Illuminance를 통해 빛에 대한 정보를 얻는다.
#				<- 구체적인 내용은 각 쉐이더에 대한 설명을 적은 책을 참고하라.
#			......
#		AttributeEnd	<- 마찬가지로, 오브젝트에 대한 정보의 끝이다.
#		......
#		......		<- 다른 오브젝트가 있다면 위와 같은 구조를 반복한다.
#		......
#	WorldEnd		<- 마찬가지로 이미지를 구성하는 모든 오브젝트들에 대한 정보의 끝이다.
#	FrameEnd		<- 마찬가지로 이미지를 구성하는 정보의 끝이다.
#------------------------------------------------------------------------------#
#
# 이제부터 위와 같은 구조를 통해 어떻게 4가지 "작업"이 가능한지 설명하겠다.
# Light쉐이더에서 사용하는 텍스쳐를 "그림자 이미지",
# 실제로 오브젝트에 쉐이더를 입혀서 렌더링된 이미지를 "렌더링 이미지"라 하겠다.
#
#
# [작업 0] 쉐이더 바꿔치기
#
#	쉐이더는 "렌더링 이미지"를 기술하는 FrameBegin...FrameEnd에 들어있다.
#	라이트 쉐이더(LightSource)는 오브젝트를 기술하기 전에
#	TransformBegin...TransformEnd에 들어있다.
#	나머지 쉐이더(Surface, Displacement)는 해당 오브젝트를 기술하는
#	AttributeBegin...AttributeEnd안에 들어있다.
#	Attribute "identifier" "name" ["..."]를 찾아 적절한 쉐이더롤 바꾸면 된다.
#
# 필요한 구조:
#	바꿔치기할 쉐이더에 대한 정보를 배열형태로 가지고 있다.
#	배열의 각 성분은
#		오브젝트 이름		<- 어떤 오브젝트에 대한
#		쉐이더의 종류		<- 어떤 종류(LightSource, Surface, Displacement)의 쉐이더를
#		쉐이더 자체의 내용	<- 어떤 정보로 바꿔치기 할 것인가?
#	를 담고 있어야 한다.
#	따라서 실제로 그 배열은 이러한 구조체(펄에서는 해쉬를 통해 저장된다)들의 배열이다.
#	주 1) 오브젝트의 이름은 정확한 이름이 아니라 해당 오브젝트를 찾을 수 있는 문자열이면 된다.
#		예를 들어, "*"와 같이 모든 오브젝트에 해당되는 것이라 할 수도 있다.
#		즉 여러 오브젝트가 같은 쉐이더를 공유할 수도 있다.
#	주 2) 따라서, 배열이 그렇듯이 첨자가 곧 순서를 말해주므로
#		한 오브젝트가 여러 성분의 오브젝트이 이름과 일치한다면
#		낮은 첨자의 정보가 우선한다. 즉 FIFO(First In First Out)의 배열이다.
#	주 3) 오브젝트의 이름은 대소문자를 구별하지 않는다. (ignore case)
#	주 4) 모든 오브젝트를 말할 때는 "*"대신에 ".*"를 사용한다. (쉘과 달리 perl의 regular expression의 특성상)
#	주 5) 쉐이더 내용이 그대로 바뀌치기 되므로 내용은 완전한 "RIB Directive"이어야 한다.
#		즉 "LightSource ...", "Surface ...", "Displacement ..."와 같아야 하고,
#		정확한 해당 "RIB Directive"의 문법을 지켜야 한다.
#		쉐이더 자체에 큰 따옴표가 있으므로, 쉐이더 내용을 묶을 때는 qq를 사용한다.
#	주 6) 쉐이더 이름이나 쉐이더의 내용은 큰 따옴표(")로 묶이므로,
#	자주 쓰는 변수이름을 써서 내용을 적을 수 있다.
#
#	예를 들면 다음과 같다.
#
#		my	$Kd = 0.8;
#		my	$shaderContent = qq/Surface "mtorLambert" "float diffuseCoeff" [ $Kd ]/;
#
#	하나의 완전한 예는 다음과 같다.
#	모든 오브젝트에는 동일한 쉐이더를 입히는 데,
#	특정 오브젝트에만 다른 쉐이더를 입히는 예는 다음과 같다.
#------------------------------------------------------------------------------#
	# 아래 내용을 따로 주어지는 파일에서 읽는다.
	# 해당 파일(sif: Shader Infomation File)은 다른 프로그램을 통해서 만들어질 수 있다.
	# 그 파일에서 4개의 $SHADER_TYPE__XXX라는 상수와 @AoHShaderInfoByArt라는 변수는
	# 여기서는 참고하므로 전역변수로 선언되어야 한다. (my를 이용해서 정의하면 안된다!)

$SHADER_TYPE__NONE		= -1;	# 상수(const)
$SHADER_TYPE__LIGHTSOURCE	= 0;	# 상수(const)
$SHADER_TYPE__SURFACE		= 1;	# 상수(const)
$SHADER_TYPE__DISPLACEMENT	= 2;	# 상수(const)

@AoHShaderInfoByArt = ();
if ( @aSIFile ) {
	# ART부서가 설정하는 쉐이더 정보를 담은 전역변수(@AoHShaderInfoByArt)를 설정한다.
	# 마지막에 설정된 파일이 우선한다. (LIFO)
	for ( my $i = $#aSIFile; $i >= 0; --$i ) {
		my	$SIFile = $aSIFile[ $i ];
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
			$$contentRef =~ s/(Surface|Displacement|LightSource)\s*"\s*(.*?)\s*"/$1 . ' "' . ( $2 =~ m|^-(.*)| ? $1 : $SHADER_PATH ? $SHADER_PATH . '\/' . $2 : $2 ) . '"'/seg;
			# print 'content => |' . $$contentRef . '|' . $/;
			$$contentRef =~ s/"\s*([^"]*?tex)\s*"/'"' . ( $1 =~ m|^-(.*)| ? $1 : $TEXTURE_PATH ? $TEXTURE_PATH . '\/' . $1 : $1 ) . '"'/seg;
			# print 'content => |' . $$contentRef . '|' . $/;

			push( @AoHShaderInfoByArt, $hRef );
		}
	}
} else {
	# -sif가 주어지지 않았을 때는,
	# 어떤 쉐이더도 변경하지 않는다.
	# 즉 @AoHShaderInfoByArt = ();
}
if ( @aRBFile ) {
	# 마지막에 설정된 파일이 우선한다. (LIFO)
	for ( my $i = $#aRBFile; $i >= 0; --$i ) {
		my	$rbFile = $aRBFile[ $i ];
		my	$hRef = require $rbFile;
		if ( ref( $hRef ) ne 'HASH' ) {
			print STDERR 'EDITRIB: Error: RIBBOX: ' . $rbFile . ': Not "RIB Box Info File" Format: Ignored' . $/;
			next;
		}
		foreach my $key ( keys( %{ $hRef } ) ) {
			my	$val = $hRef->{ $key };
			my	$valType = ref( $val );

			if ( $key == $ENUM_RIB_BOX__DECLARE__JUST_BEFORE_FIRST_FRAME_BEGIN ) {
				if ( $valType ) {
					print STDERR 'EDITRIB: Error: RIBBOX: Key ID = ' . $key . ': SCALAR required: Ignored' . $/;
					next;
				}
				$HoARIBBoxInfo{ $key } .= "\r\n" . $val;
				next;
			}
			if (
				# CONTEXT에서 ELEMENT가 정의되어야 한다.
				#	정의되지 않았다면 모든 ELEMENT에서 적용된다고 가정한다.
				$key != $ENUM_RIB_BOX__FRAME__JUST_BEFORE_DISPLAY &&
				$key != $ENUM_RIB_BOX__FRAME__JUST_BEFORE_WORLD_BEGIN &&
				$key != $ENUM_RIB_BOX__WORLD__JUST_BEFORE_FIRST_LIGHTSOURCE_BLOCK &&
				$key != $ENUM_RIB_BOX__WORLD__JUST_BEFORE_FIRST_GEOMETRIC_PRIMITIVE_BLOCK &&
				$key != $ENUM_RIB_BOX__WORLD__JUST_AFTER_LAST_OBJECT_BLOCK &&
				# CONTEXT에서 (OBJECT, ELEMENT)가 정의되어야 한다.
				#	하나만 정의되었을 때, 정의된 OBJECT가 모든 ELEMENT에서 적용된다고 가정한다.
				#	정의되지 않았다면, 모든 ELEMENT에서 모든 OBJECT에서 적용된다고 가정한다.
				$key != $ENUM_RIB_BOX__LIGHT__JUST_AFTER_ATTRIBUTE &&
				$key != $ENUM_RIB_BOX__OBJECT__JUST_AFTER_ATTRIBUTE &&
				$key != $ENUM_RIB_BOX__OBJECT__JUST_BEFORE_LIGHT_SHADER &&
				$key != $ENUM_RIB_BOX__OBJECT__JUST_BEFORE_SURFACE_SHADER &&
				$key != $ENUM_RIB_BOX__OBJECT__JUST_BEFORE_GEOMETRIC_PRIMITIVE
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
				push( @{ $HoARIBBoxInfo{ $key } }, @{ $val } );
			} else {
				push( @{ $HoARIBBoxInfo{ $key } }, $val );
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
#		'type'		=> $SHADER_TYPE__SURFACE,	# Surface쉐이더를
#		'content'	=> qq/Surface "mtorLambert" "float diffuseCoeff" [ $allKd ]/
#								# 기본값($allKd)을 사용하는 쉐이더(mtorLambert)로 바꾼다.
#	};
#	unshift(@AoHShaderInfoByArt, $hShaderInfoRef);		# 처음이므로 push(@AoHShaderInfoByArt, $hShaderInfoRef)와 동일하다.
#	$hShaderInfoRef = {
#		'set'		=> "sphere",			# "sphere"라는 이름을 가진 특정 오브젝트(들)의
#		'type'		=> $SHADER_TYPE__SURFACE,	# Surface쉐이더를
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
@AoHShaderInfoByTd = ();
{
	my	$hShaderInfoRef = {
		'set'		=> [ ".*" ],			# 모든 오브젝트의
		'type'		=> $SHADER_TYPE__SURFACE,	# Surface쉐이더를
		'content'	=> qq/Surface "@{ [ $SHADER_PATH ? $SHADER_PATH . '\/' : 'rmanshader\/' ] }shaderShadowOfAByLOntoB"/
								# 특수한 쉐이더로 바꾼다.
								# 이 쉐이더의 유일한 파라미터는 "shadowTexture"이다.
								# 그러면 뒤에다 ' "string shadowTexture" [ "..." ]'라고 하면 될 것이다.
								# 단, '...'부분은 적당한 쉐도우 맵으로 치환한 후.
								# 그러나 이러한 맵이 여러 개 있을 수 있다.
								# 그것을 아래에서 결정한 후,
								# ' "string[#] shadowTexture" [ "..." "..." ... "..." ]'처럼 만들어 이 뒤에다 붙인다.
								# 여기서 '#'는 맵의 갯수이다.
	};
	push(@AoHShaderInfoByTd, $hShaderInfoRef);
}
#------------------------------------------------------------------------------#
#
# 오늘은 여기까지...
#	2006/11/22 (EST)
#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)
#
#
# [작업 1] 특정 오브젝트를 제외하기
#
#	쉐이더 이미지와 렌더링 이미지를 만들 때,
#	오브젝트를 기술하는 AttributeBegin...AttributeEnd블럭을 지운다.
#
# 필요한 구조:
#	제거해야 할 오브젝트를 정의하는 두 가지 방법이 있다.
#	1. 선택한 오브젝트만을 제거하는 방법
#		제거해야할 오브젝트가 몇 개 되지 않는 경우에 사용한다.
#		제거해야 할 오브젝트를 배열로 설정하면 된다.
#		[ 'objA', 'objB', ..., 'objZ' ]
#	2. 선택한 오브젝트 중에서 몇 가지를 제외하고 제거하는 방법
#		몇 개만을 제외하고 대부분을 제거할 때 사용한다.
#		먼저 선택된 오브젝트들을 배열로 설정하고,
#		그 중에서 제거할 오브젝트들을 다시 배열로 설정한다.
#		즉 두 개의 배열이 필요하다.
#		[ [ 'obj.*' ], [ 'objX' ] ]
#		첫 번째 배열에 들어있으나 두 번째 배열에 들어있지 않은 오브젝트들만을 제거한다.
#		첫 번째 배열을 집합 A라 하고, 두 번째 배열을 집합 B라고 할 때,
#		차집합(A - B)에 속하는 오브젝트만을 제거하는 것이다.
#		1.의 집합 B가 공집합(empty set)인, 2.의 특별한 경우이다.
#
#	@AoAObjectDeleted = [
#		[ ... ],	# 집합 A: 반드시 지정해야 한다.
#		[ ... ]		# 집합 B: 없을 경우, 생략하거나 공집합([])으로 써도 된다.
#	];	# 두 개의 배열을 가진 배열이다.
#	# 제거해야할 오브젝트는 차집합(A - B)에 속한 오브젝트들이다.
#	
#	실제로 이와같은 차집합은 여러 번 지정할 수 있다.
#	즉 실제로 사용하는 배열은 위와 같은 변수(배열에 대한 배열)의 배열이다.
#
#	@AoAoAObjectDeleted = [
#		[
#			[ ... ],
#			[ ... ]
#		],
#		[
#			[ ... ],
#			[ ... ]
#		],
#		......
#	];
#
#	주 1) 배열의 특성상 순서가 있다.
#		어떤 오브젝트가 나중의 성분에 따르면 제거되지 않더라도,
#		그 보다 앞선 성분에서 제거된다면 제거된다.
# 각 "작업 chunk"에 대한 정보를 따로 파일로 만들어서 사용한다.
#	2006/11/30 (EST)
#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)
#==============================================================================#
#	my	@AoHObjectDeleted = ();
#	my	$hObjectDeletedRef;
#
#	#----------------------------------------------------------------#
#	$hObjectDeletedRef = {
#		'object set'	=> [ 'plane' ],		# 'plane'이라는 문자열이 들어있는 오브젝트만을
#		'delete'	=> 1			# 제거한다.
#	};
#	push(@AoHObjectDeleted, $hObjectDeletedRef);
#	#----------------------------------------------------------------#
#
#	#----------------------------------------------------------------#
#	$hObjectDeletedRef = {
#		'object set'	=> [ [ 'nurbs' ], [ 'sphere' ] ],
#							# 'nurbs'라는 문자열이 들어있는 오브젝트들은 선택한다.
#							# 단, 'sphere'라는 문자열이 들어있는 오브젝트를 제외한다.
#		'delete'	=> 1			# 제거한다.
#	};
#	push(@AoHObjectDeleted, $hObjectDeletedRef);
#	#----------------------------------------------------------------#
#==============================================================================#
#
# 오늘은 여기까지...
# 가장 기본적인 두 가지를 다루었다.
# 나머지는 이 둘의 적절한 조합이다.
#	2006/11/23 (EST)
#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)
#
#
# [작업 2] Light Linking 처리하기
#
#
# [작업 3] A가 B에 드리운 그림자 얻기
#
#
####
# 각 작업에 필요한 구조체를 정리한다.
#	2006/11/27 (EST)
#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)
# [작업 1]
#	1) 특정 오브젝트의 이름과 "일치"하는 이름을 가진 원소가 속한 집합
#		[ ... ]			A: 원소는 집합 A에 속한다.
#		[ [ ... ], [ ... ] ]	A - B: 원소는 집합 A에 속하지만 집합 B에는 속하지 않는다.
#	2) 지울 것인가 아니면 남길 것인가를 말하는 정보
#		새로 추가된 것으로 앞에서 고려한 구조(@AoAObjectDeleted)가 수정되었다.
#
#	my	$href = {
#		'object set'		=> AREF,
#		'delete'		=> BOOL
#	};
#
#	위와 같은 구조체에 대한 "순서"(FIFO: First In First Out)가 있는 배열
#
# [작업 2]
#	1) 특정 오브젝트의 이름과 "일치"하는 이름을 가진 원소가 속한 집합
#	[ ... ]			A: 원소는 집합 A에 속한다.
#	[ [ ... ], [ ... ] ]	A - B: 원소는 집합 A에 속하지만 집합 B에는 속하지 않는다.
#	2) 특정 라이트 이름과 "일치"하는 이름을 가진 원소가 속한 집합
#	[ ... ]			A: 원소는 집합 A에 속한다.
#	[ [ ... ], [ ... ] ]	A - B: 원소는 집합 A에 속하지만 집합 B에는 속하지 않는다.
#	3) 비출 것인가 아니면 비추지 않을 것인가를 말하는 정보
#
#	my	$href = {
#		'light set'	=> AREF,
#		'object set'	=> AREF,
#		'illuminate'	=> BOOL
#	};
#
#	위와 같은 구조체에 대한 "순서"(FIFO: First In First Out)가 있는 배열
# 각 "작업 chunk"에 대한 정보를 따로 파일로 만들어서 사용한다.
#	2006/11/30 (EST)
#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)
#==============================================================================#
#	my	@AoHLightLinking = ();
#	my	$hLightLinkingRef;
#
#	#----------------------------------------------------------------#
#	# Light-Centric Light Linking
#	$hLightLinkingRef = {
#		'light set'	=> [ "spotLightShape1" ],
#				# "spotLightShape1"이라는 문자열을 가진 라이트는
#		'object set'	=> [ "sphere" ],
#				# "sphere"라는 문자열을 포함하는 오브젝트에게
#		'illuminate'	=> 1
#				# 비춘다.
#	};
#	push( @AoHLightLinking, $hLightLinkingRef );
#	#----------------------------------------------------------------#
#
#	#----------------------------------------------------------------#
#	# Object-Centric Light Linking
#	$hLightLinkingRef = {
#		'light set'	=> [ [ ".*" ], [ "spotLightShape1" ] ],
#				# "spotLightShape1"라는 문자열을 포함하는 라이트를 제외한 모든 라이트는
#		'object set'	=> [ "cone" ],
#				# "cone"라는 문자열을 포함하는 오브젝트에
#		'illuminate'	=> 1
#				# 비춘다.
#	};
#	push( @AoHLightLinking, $hLightLinkingRef );
#	#----------------------------------------------------------------#
#==============================================================================#
#
# [작업 3]
#	1) 그림자를 만드는 빛 이름과 "일치"하는 이름을 가진 원소가 속한 집합
#	[ ... ]			A: 원소는 집합 A에 속한다.
#	[ [ ... ], [ ... ] ]	A - B: 원소는 집합 A에 속하지만 집합 B에는 속하지 않는다.
#	2) 그림자를 드리우는 오브젝트의 이름과 "일치"하는 이름을 가진 원소가 속한 집합
#	[ ... ]			A: 원소는 집합 A에 속한다.
#	[ [ ... ], [ ... ] ]	A - B: 원소는 집합 A에 속하지만 집합 B에는 속하지 않는다.
#	3) 그림자를 받는 오브젝트의 이름과 "일치"하는 이름을 가진 원소가 속한 집합
#	[ ... ]			A: 원소는 집합 A에 속한다.
#	[ [ ... ], [ ... ] ]	A - B: 원소는 집합 A에 속하지만 집합 B에는 속하지 않는다.
#
#	my	$href = {
#		'light set'	=> AREF,
#		'give set'	=> AREF,
#		'take set'	=> AREF
#	};
#
#	위와 같은 구조체에 대한 "순서"(FIFO: First In First Out)가 있는 배열
# 각 "작업 chunk"에 대한 정보를 따로 파일로 만들어서 사용한다.
#	2006/11/30 (EST)
#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)
#==============================================================================#
#	my	@AoHShadowOfAByLOntoB = ();
#	my	$hShadowOfAByLOntoBRef;
#
#	#----------------------------------------------------------------#
#	$hShadowOfAByLOntoBRef = {
#		'light set'	=> [ "spotLightShape1" ],
#			# "spotLightShape1"이라는 문자열을 포함하는 이름을 가진 라이트에 의해
#		'give set'	=> [ "sphere" ],
#			# "sphere"라는 문자열을 포함하는 이름을 가진 모든 오브젝트들이 만드는 그림자가
#		'take set'	=> [ "plane" ]
#			# "plane"이라는 문자열을 포함하는 이름을 가진 오브젝트에 비추는
#
#			# "특수" 처리된 그림자 이미지를 얻는다.
#	};
#	unshift( @AoHShadowOfAByLOntoB, $hShadowOfAByLOntoBRef );
#	#----------------------------------------------------------------#
#
#	#----------------------------------------------------------------#
#	$hShadowOfAByLOntoBRef = {
#		'light set'	=> [ "spotLightShape2" ],
#			# "spotLightShape2"이라는 문자열을 포함하는 이름을 가진 라이트에 의해
#		'give set'	=> [ "cone" ],
#			# "cone"이라는 문자열을 포함하는 이름을 가진 모든 오브젝트들이 만드는 그림자가
#		'take set'	=> [ "plane" ]
#			# "plane"이라는 문자열을 포함하는 이름을 가진 오브젝트에 비추는
#
#			# "특수" 처리된 그림자 이미지를 얻는다.
#	};
#	unshift( @AoHShadowOfAByLOntoB, $hShadowOfAByLOntoBRef );
#	#----------------------------------------------------------------#
#==============================================================================#

#	my	$light;		# 현재 처리하는 프레임이 어떤 라이트에 대한 것인지를 말해준다.
				# Display를 통해 알 수 있다.
				#	Display ".../untitled.spotLightShape1.shd.0001.tex" "shadow" "z"
				#	=> "spotLightShape1"
				#	Display "..." "tiff" "rgba" ...
				#	=> ""
				# ""이라면 "렌더링 이미지"에 해당하는 Frame(FrameBegin...FrameEnd)안에 있음을 뜻하고,
				# 그렇지 않을 때는 "쉐도우 이미지"에 해당하는 Frame안에 있음을 뜻한다.
				# "렌더링 이미지"를 처리할 때(이 값이 ""일 때)에만 쉐이더 정보를 사용한다.
# 각 "작업 chunk"의 최상위 "스택"의 이름을 통해 알 수 있다.
#	my	$hChunkRef;
#	my	$hStackRef = $hChunkRef->{ 'stack' }->[ 0 ];
#	my	$light = $hStackRef->{ 'name' };
#	2006/11/29 (EST)
#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)

#	my	@lightSet;	# 모든 라이트에 대한 정보를 담고 있다.
				# "렌더링 이미지"에 해당하는 Frame은 제일 마지막에 위치하므로(왜?),
				# 그 전에 "쉐두우 이미지"에 대한 Frame을 처리할 때마다 하나씩 추가한다.
				# (""이 아닌 앞의 $light변수값을 얻을 때마다 추기한다.)
				# "Light Linking"을 위해서 필요한 정보이다.
#	my	%shadowTexture;	# 작업("chunk")에 따라 서로 다른 이름의 쉐도우 텍스쳐 이름을 사용해야 한다.
				# 그러므로 "렌더링 이미지"의 라이트 쉐이더에서 사용되는 텍스쳐 이름도 적절하게 바꿔줘야 한다.
				# Key에는 원래 쉐도우 텍스쳐 이름이, Value에는 "chunk"에 따라 달라지는 쉐도우 텍스쳐 이름이 들어간다.
#	my	%hLightShadowTexture;
# Key에는 light의 이름이, Value에는 해당 라이트가 만드는 원본 쉐도우 텍스쳐의 이름이 들어간다.
# 정확히 1:1이다.
# 1) 다음과 같이 어떤 light가 있는 지 안다.
#	my	@lightSet = ();
#	foreach my $light (keys( %hLightShadowTexture )) {
#		push( @lightSet, $light );
#	}
#
# 2) 다음과 같이 주어진 쉐도우 텍스쳐에 만드는 light의 이름을 알 수 있다.
#	my	$givenShadowTexture;
#	my	$whichLight;
#	foreach my $light (keys( %hLightShadowTexture )) {
#		if ($givenShadowTexture eq $hLightShadowTexture{ $light }) {
#			$whichLight = $light;
#			last;
#		}
#	}
#	if ($whichLight) {
#		# $givenShadowTexture를 만드는 light은 $whichLight이다.
#	}
#	2006/11/29 (EST)
#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)


# 지금까지 "쉐도우 텍스쳐"이름을 기억할 때,
# 각 "job chunk"에 상관없는 원본 RIB에 있는 이름만 기억했다.
# "쉐도우 텍스쳐"를 참고하는 곳에서는 기껏해야 알려진 "job chunk"의 key만을 추가하여
# 각 "job chunk"마다 서로 다른 이름을 사용하게 했다.
# 하지만, 각 "job chunk"마다 완전히 다른 이름을 사용하게 할 때에는 적절하지 못하다.
# 따라서 라이트에 해당하는 "쉐도우 텍스쳐"의 이름을 각 "job chunk"마다 따로 기억한다.
#	2007/1/10 (EST)
#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)
#
# "LightSource"라는 "RIB Directive"를 보면 주어진 light를 RIB에 참고할 때 사용되는 handle이 있다.
#	LightSource name handle ...parameterlist...
# 지금까지의 테스트에서는 name과 handle이 같았으므로 문제가 없었으나,
# 새로운 예를 보니 일반적으로 같지 않다. 따라서 주어진 light에 대한 handle이 어는 것인지
# "LIghtSource"를 만났을 때 따로 기억해서, light를 참고할 때 그 handle을 적어주어야 한다.
# 따라서 light에 대한 정보를 가지는 %hLightShadowTexture는 더 이상 "Shadow Texture"에 대한 정보만을 가지지 않으므로
# 이름도 바꾸고 구조도 단순한 해쉬에서 %HoA나 %HoH로 바꿔어야 한다.
#	2007/1/5 (EST)
#	김광호 (Gwang-Ho Kim, iccentriic@gmail.com)
#my	%HoALightSource = ();	# Key는 light의 이름이고, Value는 두 성분으로 이루어진 배열에 대한 참조이다.
				# 배열의 첫 번째 성분은 light에 대한 handle이고,
				# 두 번째 성분은 light가 만드는 shadow texture의 이름이다.
# %HoALightSource = (
#	'spotLight1'	=> [ '...handle...', '...shadow texture...' ],
#	'spotLight2'	=> [ '...handle...', '...shadow texture...' ],
#	......
# );


#	my	@AoHObjectByL;	# 현재 처리 중인 "쉐도우 이미지"에 해당하는 Frame안에서
				# 고려해야 하는 오브젝트들에 대한 정보를 담은 참조롤 이루어진 배열이다.
				# 처리하는 상황에 따라, @AoHLightLinking또는 @AoHShadowOfAByLOntoB의 일부가 된다.
				# "쉐도우 이미지"에 해당하는 Frame안에서,
				# 각 오브젝트를 만날 때마다 그 오브젝트를 고려해야 할 지 아닐 지를 판단하는 데 있어서
				# 모든 라이트에 해당하는 정보를 담은
				# @AoHLightLinking이나 @AoHShadowOfAByLOntoB를 고려하는 것은
				# 중복된 부분이 많기 때문이다.
				# Frame안에서 World를 시작하기 전에,
				# Display를 통해 라이트 정보($light에 저장된 값)를 얻은 후에
				# 전체의 @AoHLightLinking이나 @AoHShadowOfByLOntoB에서
				# 해당 라이트에 관련된 성분만을 모아서 사용한다.
# 각 "작업 chunk"마다 이러한 정보를 저장해야 한다.
#	2006/11/29 (EST)
#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)
# 각 "작업 chunk"에 대한 정보를 따로 파일로 만들어서 사용한다.
#	2006/11/30 (EST)
#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)

#$CHUNK_TYPE__DEL	= 1;	# 상수(const): [작업 1]
#$CHUNK_TYPE__LL		= 2;	# 상수(const): [작업 2]
#$CHUNK_TYPE__SHD	= 3;	# 상수(const): [작업 3]

%HoHChunk = ();
if ( $jcFile ) {
	require $jcFile;	# "작업 chunk"에 해당하는 전역변수(%HoHChunk)를 설정한다.
} else {
	$HoHChunk{ $byArtKeyName } = {
		'type'	=> $CHUNK_TYPE__RS,
		'rbref'	=> \%HoARIBBoxInfo,
		'siref'	=> \@AoHShaderInfoByArt,
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

#==============================================================================#
#	my	%HoHChunk = ();
#	my	$keyChunk;
#	my	$hChunkRef;
#
#	#----------------------------------------------------------------#
#	$keyChunk = 'A';
#	$hChunkRef = {
#		'type'	=> $CHUNK_TYPE__DEL,
#		'jcref'	=> \@AoHObjectDeleted,
#		'siref'	=> \@AoHShaderInfoByArt
#	};
#	$HoHChunk{ $keyChunk } = $hChunkRef;
#	#----------------------------------------------------------------#
#
#	#----------------------------------------------------------------#
#	$keyChunk = 'B';
#	$hChunkRef = {
#		'type'	=> $CHUNK_TYPE__LL,
#		'jcref'	=> \@AoHLightLinking,
#		'siref'	=> \@AoHShaderInfoByArt
#	};
#	$HoHChunk{ $keyChunk } = $hChunkRef;
#	#----------------------------------------------------------------#
#
#	#----------------------------------------------------------------#
#	$keyChunk = 'C';
#	$hChunkRef = {
#		'type'	=> $CHUNK_TYPE__SHD,
#		'jcref'	=> \@AoHShadowOfAByLOntoB,
#		'siref'	=> \@AoHShaderInfoByTd
#	};
#	$HoHChunk{ $keyChunk } = $hChunkRef;
#	#----------------------------------------------------------------#
#==============================================================================#


# 주어진 오브젝트의 이름이,
# 배열로 표시되는 집합의 원소 중의 하나와 일치되는(match) 지를 알아본다.
#	리턴값: 일치하면 1, 그렇지 않으면 0
#	집합이 배열에 대한 참조의 형태로 주어지지 않았거나
#	적어도 하나의 성분이 없다면 -1
#	이 함수를 호출한 곳에서는 리턴값이 1(일치)할 때에만 원하는 일을 해야 한다.
#
#	주 1) 일치하는 지는 펄의 "=~"를 사용한다.
#	주 2) 집합은 배열로 표시된다.
#		배열이 이 함수의 인자로 사용될 때는 배열에 대한 참조(reference)로 주어져야 한다.
#		예) isMatch("ax", ["a", "b", "c"]) = YES(1)
#		"ax"라는 문자열이 세 원소("a", "b", "c")로 이루어진 집합의 어떤 원소와 "일치"하는가?
#	주 4) 집합이 두 집합의 차집합으로 주어질 수도 있다.
#		두 배열에 대한 참조를 두 성분으로 하는 배열에 대한 참조의 형태로 주어져야 한다.
#		예) isMatch("ax", [["a", "b", "c"], ["a"]]) = NO(0)
#		"ax"라는 문자열이 세 원소("a", "b", "c")로 이루어진 집합의 원소와는 "일치"하지만
#		한 원소("a")로 이루어진 집합의 원소와는 일치하지 않는가?
sub isMatched {
	my	($name, $arefSet) = @_;

	unless ( ref( $arefSet ) ) {
		# "a,b,...,-x,-y,..."의 형태로 주어졌다.
		return isInSet( $name, $arefSet );
	}
	if (ref($arefSet) ne 'ARRAY') {
		print STDERR 'EDITRIB: Warning: 배열에 대한 참조(reference)로 주어져야 한다' . $/;
		return -1;
	}

	if (@{ $arefSet } == 0) {
		print STDERR 'EDITRIB: Warning: 적어도 하나의 성분은 있어야 한다' . $/;
		return -1;
	}

	# Type 0: [ ... ]
	# Type 1: [ [ ... ], [ ... ] ]
	my	$type = ref($arefSet->[0]) ne 'ARRAY' ? 0 : 1;
	my	$arefSetA = $type == 0 ? $arefSet : $arefSet->[ 0 ];

	# 집합 A에서 "일치"하는 원소가 있는 지 알아본다.
	my	$bInSetA = 0;
	foreach my $elementA ( @{ $arefSetA } ) {
		if (ref($elementA)) {
			print STDERR 'EDITRIB: Warning: {' . $elementA . '}: 순수한 문자열이여야 한다: 무시한다' . $/;
			next;
		}
		if ($name =~ /$elementA/i) {
			$bInSetA = 1;
			last;
		}
	}

	# "Type 0"이라면 집합 A에 속하는 지 아닌 지가 곧 리턴값이 된다.
	if ($type == 0) {	# Type 0: [ ... ]
		# 집합 A에 속하는 지 아닌 지가 곧 리턴값이 된다.
		return $bInSetA;
	}

	# "Type 1"이라도 집합 A에 속하지 않으면 리턴값은 0(불일치)이다.
	return 0 unless $bInSetA;

	# "Type 1"인 경우로서 일단 집합 A에 속한다.
	# 다시, 집합 B에서 "일치"하는 원소가 있는 지 알아본다.
	# 집합 B의 어떤 원소와도 일치하지 않으면 리턴값은 1(일치)이고,
	# 그렇지 않으면 리턴값은 0(불일치)이다.

	if (@{ $arefSet } != 2) {
		print STDERR 'EDITRIB: Warning: "Type 1: [ [ ... ], [ ... ] ]"의 경우, 두 개의 성분을 가져야 한다' . $/;
		return -1;
	}
	my	$arefSetB = $arefSet->[ 1 ];
	if (ref($arefSetB) ne 'ARRAY') {
		print STDERR 'EDITRIB: Warning: "Type 1: [ [ ... ], [ ... ] ]"의 경우, 두번째 성분도 배열에 대한 참조여야 한다' . $/;
		return -1;
	}

	# 집합 B에서 "일치"하는 원소가 없는 지 알아본다.
	my	$bNotInSetB = 1;
	foreach my $elementB ( @{ $arefSetB } ) {
		if (ref($elementB)) {
			print STDERR 'EDITRIB: Warning: {' . $elementB . '}: 순수한 문자열이여야 한다: 무시한다' . $/;
			next;
		}
		if ($name =~ /$elementB/i) {
			$bNotInSetB = 0;
			last;
		}
	}
	return $bNotInSetB;
}

# 몇 개의 "RIB Directive"의 경우,
# 해당하는 내용이 각 "작업 chunk"에서 수정될 수 있다.
# 같은 내용을 각 "작업 chunk"에서 동일하게 참고하게 하기 위해
# 몇 "RIB Directive"의 경우, 원본을 여기서 복사하고,
# 각 "작업 chunk"의 시작에서 원본을 다시 읽어도로록 한다.
#	2006/11/30 (EST)
#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)
	my	%hSaveRIBDirectiveContent = (
		'Attribute'	=> [],		# "Ray tracing"이 설정된 경우에,
						# 다음 두 라인이 추가된다.
						#	Attribute "grouping" "string membership" ["+RManReflSubset"]
						#	Attribute "grouping" "string membership" ["+RManRefrSubset"]
						#	2007/5/23 (EST)
						#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)

		'WorldBegin'	=> [],		# ENUM_RIB_BOX__FRAME__JUST_BEFORE_WORLD_BEGIN "RIB Box"가 있는 경우에
						# 내용이 바뀔 수 있다.
		'WorldEnd'	=> [],		# ENUM_RIB_BOX__WORLD__JUST_AFTER_LAST_OBJECT_BLOCK의 경우에...

		'Display'	=> [],
		'LightSource'	=> [],
		'Surface'	=> [],
		'Displacement'	=> [],
	);

# 준비:
#	1) 각 [작업 chunk]에 쓰일 "file handle"을 연다.
#	2) "스택"을 초기화한다.
foreach my $keyChunk (keys( %HoHChunk )) {
	my	$hChunkRef = $HoHChunk{ $keyChunk };
	my	$typeChunk = $hChunkRef->{ 'type' };
	my	$bBake = 0;
	if (
		!( $hChunkRef->{ 'type' } & $CHUNK_TYPE__SHD ) &&
		( $hChunkRef->{ 'type' } & $CHUNK_TYPE__BAKE )
	) {
		$bBake = 1;
		$hChunkRef->{ 'bBake' } = 1;
		$hChunkRef->{ 'type' } &= ~$CHUNK_TYPE__BAKE;
	}

#( $path, $filename, $dspyName, $dspyName0, $light, $shadowType, $frame ) = $name =~ /(.*?)(([^\/]+?)(\.([^\.]+?)\.(shd|dsh)\.(\d+)\.tex))$/;
#( $path, $filename, $dspyName, $dspyName0, $frame, $imageFormatExtension ) = $name =~ /(.*?)(([^\/]+?)(\.(\d+)(.*)))$/;
	my	( $path, $filename, $dspyName, $dspyName0, $frame ) = $ribFile =~ /(.*?)(([^\/\.]+)(\..*?(\d+)\.rib))$/;
	die 'EDITRIB: Error: Cannot find frame #: ' . $ribFile . $/ unless $frame;
	# RIB Box에서 사용할 $gFrameNumber를 설정한다.
	$gFrameNumber = $frame;
	print STDERR "EDITRIB: Info: PATH.RIB: Path:= {$path}, filename:= {$filename}, dspyName:= {$dspyName}, dspyName0:= {$dspyName0}, frame:= {$frame}\n" if $bVerbosePath;
	$path = './' unless $path;	# "-d"옵션으로 절대경로를 지정하지 않도록 하기 위해서
	print STDERR 'EDITRIB: Info: PATH.RIB: rootPathExpr:= ' . '{' . $rootPathExpr . '}' . ', addPathExpr:= ' . '{' . $addPathExpr . '}' . $/ if $bVerbosePath;
	my	$newUNIXPath = ( $rootPathExpr ? eval( 'qq|' . $rootPathExpr . '|' ) . '/rib/' : $path ) . eval( 'qq|' . $addPathExpr . '|' ) . '/';
	print STDERR 'EDITRIB: Info: PATH.RIB: New Path:= ' . '{' . $newUNIXPath . '}' . $/ if $bVerbosePath;
	$newUNIXPath = mkdir2( $newUNIXPath );
	print STDERR 'EDITRIB: Info: PATH.RIB: New Path2:= ' . '{' . $newUNIXPath . '}' . $/ if $bVerbosePath;
	my	$pathname = $newUNIXPath . $dspyName . '.' . $keyChunk . ( $bBake ? '.ShadowPass' : '' ) . $dspyName0;
	print STDERR 'EDITRIB: Info: ' . ( $bBake ? 'BAKE: Shadow Pass ' : '' ) . 'RIB: PATHNAME:= ' . '{' . $pathname . '}' . $/;

	my	$fileHandle = 'fh' . $keyChunk;
#	unless (open($fileHandle, '> ' . $pathname)) {
#		print STDERR 'EDITRIB: Error: open for "chunk"(' . $keyChunk . '): Failed: Ignored' . $/;
#		next;
#	}
	if ( $bAscii ) {
		open($fileHandle, '> ' . $pathname) ||
			die 'EDITRIB: Error: open for "chunk"(' . $keyChunk . '): Failed';
	} else {
		open( $fileHandle, '| catrib -binary -gzip -o "' . $pathname . '"' ) ||
			die 'EDITRIB: Error: open for "chunk"(' . $keyChunk . '): Failed';
	}
	$hChunkRef->{ 'file handle' } = $fileHandle;

	$hChunkRef->{ 'stack index' } = -1;
	$hChunkRef->{ 'declare written' } = 0;	# "declare"부분이 RIB에 쓰여졌는 지를 말해준다.
						# "Display"를 통해, 유효한 "Frame"에 관한 RIB을 쓸 때,
						# 이 값을 참고한다.
	$hChunkRef->{ 'stack array' } = [];
	$hChunkRef->{ 'light source' } = {};	# %HoALightSource를 각 "job chunk"마다 기억하게 한다.
}
while (1) {
	my	($returnVal, $RIBDirective, $RIBDirectiveContentRef) = getLineByRIBDirective($ribFileHandle);

	if ($returnVal == 0) {
		# 더 이상 읽어들일 줄이 없다.
		last;
	}
	# $returnVal = 1인 경우에는 getLineByRIBDirective.pl에서 선언된
	# @RIBDirectiveContent변수의 주소와 같다.
	# print STDERR $RIBDirectiveContentRef . $/;
	if ($returnVal == -1) {
		# 의미없는 줄이다.
		print STDERR "EDITRIB: Warning: Unknown: {@$RIBDirectiveContentRef}\n" if $bVVVerbose;
		# 무시하고 다음 줄을 읽는다.
		next;
	}

	# "RIB Directive"를 "enum"형으로 분류한다.
	my	$enumRIBDirective;
	unless ( defined( $RIBDirective ) ) {
		$enumRIBDirective = $ENUM_RIB_DIRECTIVE__NONE;
	} else {
		if ( $RIBDirective eq 'FrameBegin' ) {
			$enumRIBDirective = $ENUM_RIB_DIRECTIVE__FRAME_BEGIN;
		} elsif ( $RIBDirective eq 'FrameEnd' ) {
			$enumRIBDirective = $ENUM_RIB_DIRECTIVE__FRAME_END;
		} elsif ( $RIBDirective eq 'Display' ) {
			$enumRIBDirective = $ENUM_RIB_DIRECTIVE__DISPLAY;
		} elsif ( $RIBDirective eq 'WorldBegin' ) {
			$enumRIBDirective = $ENUM_RIB_DIRECTIVE__WORLD_BEGIN;
		} elsif ( $RIBDirective eq 'WorldEnd' ) {
			$enumRIBDirective = $ENUM_RIB_DIRECTIVE__WORLD_END;
		} elsif ( $RIBDirective eq 'AttributeBegin' ) {
			$enumRIBDirective = $ENUM_RIB_DIRECTIVE__ATTRIBUTE_BEGIN;
		} elsif ( $RIBDirective eq 'AttributeEnd' ) {
			$enumRIBDirective = $ENUM_RIB_DIRECTIVE__ATTRIBUTE_END;
		} elsif ( $RIBDirective eq 'TransformBegin' ) {
			$enumRIBDirective = $ENUM_RIB_DIRECTIVE__TRANSFORM_BEGIN;
		} elsif ( $RIBDirective eq 'TransformEnd' ) {
			$enumRIBDirective = $ENUM_RIB_DIRECTIVE__TRANSFORM_END;
		} elsif ( $RIBDirective eq 'LightSource' ) {
			$enumRIBDirective = $ENUM_RIB_DIRECTIVE__LIGHTSOURCE;
		} elsif ( $RIBDirective eq 'Surface' ) {
			$enumRIBDirective = $ENUM_RIB_DIRECTIVE__SURFACE;
		} elsif ( $RIBDirective eq 'Displacement' ) {
			$enumRIBDirective = $ENUM_RIB_DIRECTIVE__DISPLACEMENT;
		} elsif ( $RIBDirective eq 'Attribute' ) {
			$enumRIBDirective = $ENUM_RIB_DIRECTIVE__ATTRIBUTE;
		} elsif ( defined( $hGeometricPrimitive{ $RIBDirective } ) ) {
			$enumRIBDirective = $ENUM_RIB_DIRECTIVE__GEOMETRIC_PRIMITIVE;
		} else {
			$enumRIBDirective = $ENUM_RIB_DIRECTIVE__NONE;
		}
	}

	# 첫 번째 FrameBegin을 만나기 전까지 모은다.
	if ( $enumRIBDirective == $ENUM_RIB_DIRECTIVE__FRAME_BEGIN ) {
		if ( $bDeclare ) {
			# RIB Box가 들어갈 위치이다. (ENUM_RIB_BOX__DECLARE__JUST_BEFORE_FIRST_FRAME_BEGIN)
			print STDERR 'EDITRIB: Info: RIBBOX.DECLARE: Just before first "FrameBegin"' . $/ if $bVerboseRIBBox;
			$strDeclare .= '## EDITRIB ## RIB Box: Declare: Just before first "FrameBegin"' . "\r\n" if $bVerboseRIBBox;
		}

		$bDeclare = 0;
	} else {
		if ( $bDeclare ) {
			$strDeclare .= join( '', @$RIBDirectiveContentRef );

			# 다음 줄을 읽는다.
			next;
		}
	}

	# 원본을 저장할 필요가 있을 땐 저장한다.
	if (defined( $RIBDirective ) && defined( $hSaveRIBDirectiveContent{ $RIBDirective } )) {
		@{ $hSaveRIBDirectiveContent{ $RIBDirective } } = @$RIBDirectiveContentRef;
	}

	# 각 "job chunk"마다 공통으로 사용되는, Display에 대한 정보를 저장한다.
	my	%hDisplay;
	if ( $enumRIBDirective == $ENUM_RIB_DIRECTIVE__DISPLAY ) {
		my	$display = join( '', @$RIBDirectiveContentRef );
		print STDERR '+' . '=' x 100 . '+' . $/ if $bVerbosePath;
		print STDERR '|' . ' ' x 100 . '|' . $/ if $bVerbosePath;
		print STDERR 'EDITRIB: Info: PATH.DISPLAY: ' . $display if $bVerbosePath;
		#	Display name type mode ...parameterlist...
		#
		#	Display ".../rmantex/shd/untitled.spotLightShape1.shd.0001.tex" "shadow" "z" 
		#	Display ".../rmantex/shd/untitled.spotLightShape2.shd.0001.tex" "shadow" "z" 
		#	Display ".../rmanpix/untitled.0001" "it" "rgba" ...

		# Deep shadow의 경우, 두 번째 "Display"의 의미있는 정보가 있다.
		# 현재(2007/4/17 EST)까지 관찰된 바로는 다음과 같다.
		#	Display "null" "null" "z"
		# 따라서 이와 같은 경우에는 "있는 그대로" 써준다.
		#	2007/4/17 (EST)
		#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)

while (1) {
		# "Deep shadow"의 경우에, 두 번째 "Display"에 필요한 정보가 있다.
		# 대부분 첫 번째 "Display"는 다음과 같은 형태가 된다.
		#	Display "null" "null" "z"
		# 하지만 렌더맨 설명서에 따르면 다른 형태가 올 수도 있다.
		# 이런 경우에만 에러로 처리한다.
		if ( $display =~ /^\s*Display\s*"null"\s*"null"\s*"z"/ ) {
			print STDERR 'EDITRIB: Info: Deep shadow' . $/ if $bVerbose;
			$hDisplay{ 'name' } = undef;	# 아직까지 제대로된 "Display"정보를 얻지 못했다.

			last;
		}

		my	($plusSign, $name, $type, $mode, $dummy) = $display =~ /^\s*Display\s*"(\+?)(.+?)"\s*"(.+?)"\s*"(.+?)"(.*)$/s;
		unless ( $name ) {
			die "EDITRIB: Error: Display: Invalid Format: $display";
		}
		print STDERR "EDITRIB: Info: PATH.DISPLAY: plusSign:= {$plusSign}, name:= {$name}, type:= {$type}, mode:= {$mode}, dummy:= {$dummy}\n" if $bVerbosePath;
		$hDisplay{ '+' } = $plusSign;
		$hDisplay{ 'name' } = $name;
		$hDisplay{ 'type' } = $type;
		$hDisplay{ 'mode' } = $mode;
		$hDisplay{ 'dummy' } = $dummy;

		my	$path;
		my	$filename;
		my	$dspyName;
		my	$dspyName0;	# $filename에서 "$dspyName"을 제외한 나머지 부분
		my	$frame;
		my	$light = 'Image';	# "쉐도우 이미지"에서는 "라이트"의 이름, "렌더링 이미지"에서는 "Image"
		if ( $type eq "shadow" || $type eq 'deepshad' ) {	# "쉐도우 이미지"
			my	$shadowType;
			( $path, $filename, $dspyName, $dspyName0, $light, $shadowType, $frame ) = $name =~ /(.*?)(([^\/]+?)(\.([^\.]+?)\.(shd|dsh)\.(\d+)\.tex))$/;
			unless ( $filename ) {
				die "EDITRIB: Error: Display: Invalid Format: $display";
			}
			print STDERR "EDITRIB: Info: PATH.DISPLAY: path:= {$path}, filename:= {$filename}, dspyName:={$dspyName}, dspyName0:={$dspyName0}, light:= {$light}, shadowType:={$shadowType}, frame:={$frame}\n" if $bVerbosePath;

			$hDisplay{ 'light' } = $light;
			$hDisplay{ 'shadowType' } = $shadowType;
		} else {
			my	$imageFormatExtension;
			( $path, $filename, $dspyName, $dspyName0, $frame, $imageFormatExtension ) = $name =~ /(.*?)(([^\/]+?)(\.(\d+)(.*)))$/;
			unless ( $filename ) {
				die "EDITRIB: Error: Display: Invalid Format: $display";
			}
			print STDERR "EDITRIB: Info: PATH.DISPLAY: path:= {$path}, filename:= {$filename}, dspyName:={$dspyName}, dspyName0:={$dspyName0}, frame:={$frame}, imageFormatExtension:= {$imageFormatExtension}\n" if $bVerbosePath;

			$hDisplay{ 'imageFormatExtension' } = $imageFormatExtension;
		}
		# RIB Box에서 사용할 $gElementName을 설정한다.
		$gElementName = $light;
		$hDisplay{ 'path' } = $path;
		$hDisplay{ 'filename' } = $filename;
		$hDisplay{ 'dspyName' } = $dspyName;
		$hDisplay{ 'dspyName0' } = $dspyName0;
		$hDisplay{ 'frame' } = $frame;

		last;	# 루프를 끝낸다.
}
	}

foreach my $keyChunk (keys( %HoHChunk )) {
	# 저장된 원본을 사용해야할 땐 사용한다.
	if ( defined( $RIBDirective ) && defined( $hSaveRIBDirectiveContent{ $RIBDirective } ) ) {
		@$RIBDirectiveContentRef = @{ $hSaveRIBDirectiveContent{ $RIBDirective } };
	}
	if ( $enumRIBDirective == $ENUM_RIB_DIRECTIVE__GEOMETRIC_PRIMITIVE ) {
		restoreArrayRef( $RIBDirectiveContentRef );
	}

	my	$hChunkRef = $HoHChunk{ $keyChunk };
	my	$typeChunk = $hChunkRef -> { 'type' };
	my	$bBake = $hChunkRef->{ 'bBake' };
	my	$FH = $hChunkRef -> { 'file handle' };
#	next unless $FH;	# 파일 핸들이 지정되지 않았다면 건너뛴다. (open함수에서 실패한 경우)
	my	$SUBFH = $hChunkRef -> { 'subframe file handle' };
				# "Display"를 만나면 subframe을 저장한 RIB파일을 열고,
				# 그 핸들을 저장한다.
				# "FrameEnd"를 만났을 때, 이 값은 유효하면,
				# subframe을 저장한 RIB파일을 닫는다.
	my	$BAKEFH = $hChunkRef -> { 'bake subframe file handle' };
				# "Bake"에서 만든 point cloud 파일(.ptc)이나 brick map 파일(.bkm)을 사용할
				# "렌더링 이미지"에 필요한 subframe을 담는 RIB파일에 대한 "파일 핸들"
	my	$AoHStackRef = $hChunkRef -> { 'stack array' };
	my	$iStackRef = \$hChunkRef -> { 'stack index' };
	my	$HoALightSourceRef = $hChunkRef -> { 'light source' };

	# 현재 "스택"의 fate값에 따라 내용을 처리한다.
	my	$hStackRef = $AoHStackRef->[ ${ $iStackRef } ];
	my	$fate = $hStackRef->{ 'fate' };

	if ($returnVal == 2) {
		print STDERR "EDITRIB: Info: Exception: {@$RIBDirectiveContentRef}\n" if $bVVVerbose;
		# 특별한 줄(빈줄, 주석줄, version정보를 담은 줄)이다.
		print $FH @$RIBDirectiveContentRef if $gElementName ne 'Image' || !$bBake;
		print $SUBFH @$RIBDirectiveContentRef if $SUBFH;
		print $BAKEFH @$RIBDirectiveContentRef if $BAKEFH;

		# 다음 줄을 읽는다.
		next;
	}
	# 이제부터 "RIB Directive"에 따라 그 내용을 적절히 처리한다.


	if ( $enumRIBDirective == $ENUM_RIB_DIRECTIVE__FRAME_BEGIN ) {
		# Frame의 시작이다.
#		$bFrame = 1;
		$bDeclare = 0;

		# 최상위 "스택"의 시작이다.
		# 최상위 "스택"을 초기화한다.
		# 최상위 "스택"의 "운명"의 초기값은 저장(CONTENT_SAVE)이다.
		# Display에 해당하는 "RIB Directive"를 만나면
		# 출력(CONTENT_PRINT)아니면 삭제(CONTENT_DELETE)로 바뀐다.
		# 그 때, "이름"(초기값: undef)도 함께 등록된다.
		# "쉐도우 이미지"를 만드는 Frame이라면 해당 라이트(light)의 쉐이프 노드 이름을,
		# "렌더링 이미지"를 만드는 Frame이라면 ""값이 저장된다.

		# "스택"에 대한 정보를 담을 구조체(해쉬)를 초기화한다.
		my	$hStackRef = {
			'type'		=> $STACK_TYPE__FRAME,
			'name'		=> undef,
			'content'	=> [],
			'fate'		=> $CONTENT_SAVE
		};
		# Subframe과 "Bake" 과정에 사용되는 subframe의 내용을 담을 공간을 확보한다.
		# "단순" subframe의 경우에는 공유한다.
		$hStackRef -> { 'bake subframe content' } = [] if $BAKEFH;

		# FrameBegin에 해당하는 내용을 저장한다.
		push( @{ $hStackRef ->{ 'content'} }, @$RIBDirectiveContentRef);
		push( @{ $hStackRef ->{ 'bake subframe content'} }, @$RIBDirectiveContentRef) if $BAKEFH;

		# 최상위 "스택"을 등록한다.
		${ $iStackRef } = 0;
		$AoHStackRef->[ 0 ] = $hStackRef;

		# "작업 chunk"가 "쉐도우 이미지"를 처리하는 경우,
		# 해당 light가 고려해야 하는 오브젝트들의 집합에 대한 참조를 따로 가지고 있는다.
		# 왜냐하면 "작업 chunk"는 전체적으로 라이트 집합과 대응하는 오브젝트 집합을 가지고 있다.
		# ([작업 2], [작업 3]의 경우)
		# 모든 작업은 Frame단위로 이루어진다. 즉 라이트 단위로 이루어진다. (물론 "쉐도우 이미지"의 경우)
		# 그런데 사용되지 않는 라이트에 해당하는 정보까지 사용한다면
		# 약간의 시간 손실이 있어, 전체에서 해당 라이트에 해당하는 오브젝트들의 집합을 생각한다.
		$hChunkRef->{ 'object array' } = [];
		$hChunkRef->{ 'bRayTracing' } = 0;	# Ray tracing옵션이 켜있는 지 말해준다.
				# "렌더링 이미지"를 처리하는 FrameBegin...FrameEnd안에서,
				# 'stack index'가 0인 경우에, 다음 "RIB Directive"를 만나면
				#	Attribute "trace" "int maxspeculardepth" [2]
				# 1로 바뀐다.
				# 이 값에 따라 ray tracing과 관련된 RS__VIS_REFL, RS__VIS_REFR옵션이
				# 사용될 지를 결정한다.
		$hChunkRef->{ 'found light source' } = 0;		# "라이트 쉐이더를 만났는 지를 말해준다.
		$hChunkRef->{ 'found geometric primitive' } = 0;	# "Geometric primitive"를 만났는 지를 말해준다.

		# 다음 "RIB Directive"내용을 읽는다.
		next;
	} elsif ( $enumRIBDirective == $ENUM_RIB_DIRECTIVE__FRAME_END ) {
		# Frame의 끝이다.
#		$bFrame = 0;
		$bDeclare = -1;

		# 최상위 "스택"의 끝이다.
		my	$hStackRef = $AoHStackRef->[ 0 ];
		# 최상위 "스택"의 "운명"에 따라 FrameEnd에 해당하는 내용을 처리한다.
		# 최상위 "스택" 내용의 "운명"은 출력(CONTENT_PRINT)아니면 삭제(CONTENT_DELETE)이다.
		if ($hStackRef->{ 'fate' } == $CONTENT_PRINT) {
			print $FH @$RIBDirectiveContentRef if $gElementName ne 'Image' || !$bBake;
			print $SUBFH @$RIBDirectiveContentRef if $SUBFH;
			print $BAKEFH @$RIBDirectiveContentRef if $BAKEFH;
		} else {	# $CONTENT_DELETE
			# 삭제하는 경우에는 아무것도 하지 않는다.
		}

		# FrameBegin...FrameEnd안에 있는 "Display"를 통해 얻은
		# $gElementName값은 이후로 의미가 없다.
		# 모든 "chunk"를 처리한 후에 이 값을 undef으로 설정해야 한다.
		#	2007/5/24 (EST)
		#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)
#		$gElementName = undef;

		# 모든 "스택"을 해제한다.
		${ $iStackRef } = -1;
		@{ $AoHStackRef } = ();

		# Frame에 따라 임시로 만든,
		# 주어진 라이트("쉐도우 이미지"의 경우)가 고려해야할 집합에 대한 정보를 초기화한다.
		$hChunkRef->{ 'object array' } = [];
		$hChunkRef->{ 'bRayTracing' } = 0;
		$hChunkRef->{ 'found light source' } = 0;
		$hChunkRef->{ 'found geometric primitive' } = 0;

		# Subframe을 저장한 RIB파일이 있다면 이를 닫는다.
		if ( $SUBFH ) {
			close( $SUBFH ) || die 'EDITRIB: Error: close for "subframe"(' . $hChunkRef->{ 'subframe pathname' } . ') of "chunk"(' . $keyChunk . '): Failed';
			delete $hChunkRef -> { 'subframe file handle' };
		}
		# "Bake"에 사용된 RIB파일이 있다면 이를 닫는다.
		if ( $BAKEFH ) {
			my	$bakedPath0 = $hChunkRef->{ 'TexturePathPlusDisplayNamePlusKeyChunk' };
			my	$NBakedObject = @{ $hChunkRef->{ 'SubsurfaceScattering_BakedObjects' } };

			print $BAKEFH <<BAKETEXT if $NBakedObject;
## EDITRIB ## <BAKE> ####\r
System "echo Bake Pass: Step 2/3: Running ptfilter -filter ssdifusion..."\r
BAKETEXT
			for my $bakedObject ( @{ $hChunkRef->{ 'SubsurfaceScattering_BakedObjects' } } ) {
				my	$bakedObject1 = $bakedPath0 . $bakedObject . '.' . $gFrameNumber;
				print $BAKEFH <<BAKETEXT;
System "echo OBJECT:= $bakedObject"\r
System "ptfilter -filter ssdiffusion -albedo fromfile -diffusemeanfreepath fromfile  -newer 1 -maxsolidangle 1.000000 \\\"$bakedObject1.ptc\\\" \\\"$bakedObject1.ssdf\\\""\r
BAKETEXT
			}
			print $BAKEFH <<BAKETEXT if $NBakedObject;
System "echo Bake Pass: Step 3/3: Running brickmake..."\r
BAKETEXT
			for my $bakedObject ( @{ $hChunkRef->{ 'SubsurfaceScattering_BakedObjects' } } ) {
				my	$bakedObject1 = $bakedPath0 . $bakedObject . '.' . $gFrameNumber;
				print $BAKEFH <<BAKETEXT;
System "echo OBJECT:= $bakedObject"\r
System "brickmake -newer 1 \\\"$bakedObject1.ssdf\\\" \\\"$bakedObject1.bkm\\\""\r
BAKETEXT
			}
			print $BAKEFH <<BAKETEXT if $NBakedObject;
## EDITRIB ## </BAKE> ####\r
BAKETEXT
			close( $BAKEFH ) || die 'EDITRIB: Error: close for "Bake" "subframe"(' . $hChunkRef->{ 'bake subframe pathname' } . ') of "chunk"(' . $keyChunk . '): Failed';
			delete $hChunkRef -> { 'bake subframe file handle' };
		}

		# 다음 "RIB Directive"내용을 읽는다.
		next;
	}


#### AttributeBeing...AttributeEnd나 TransformBegin...TransformEnd의 스택을 이용한 방식을 처리하기 위하여
#### 전체적인 구조를 새로 만든다.
####	2006/11/18 (EST)
####	김광호 (Gwang-Ho Kim, iccentric@gmail.com)
#### 데이터구조를 구조체(여기서는 해쉬)를 이용하여 새로 만든다.
####	2006/11/20 (EST)
####	김광호 (Gwang-Ho Kim, iccentric@gmail.com)
#### 이를 구현한다.
####	2006/11/21 (EST)
####	김광호 (Gwang-Ho Kim, iccentric@gmail.com)
	if (
		$enumRIBDirective == $ENUM_RIB_DIRECTIVE__ATTRIBUTE_BEGIN ||
		$enumRIBDirective == $ENUM_RIB_DIRECTIVE__TRANSFORM_BEGIN
	) {
#		print STDERR "\t" x (1 + $iStack) . '[' . $/;
		# "자식 스택"의 시작이다.
		# "자식 스택"의 fate값을 결정한다.
		# 자세한 것은 @AoHStack에 대한 설명에서 "fate"에 대한 1.부분을 보라.
		my	$fate = $CONTENT_SAVE;	# 기본값은 CONTENT_SAVE이다.
		if (
#			$iStack > -1 &&	# "부모 스택"이 있고,
# Frame을 최상위 스택으로 설정했고
# AttributeBegin이나 TransformBegin은 Frame안에 있으므로,
# $iStack의 값은 항상 -1보다 크다.
#	2006/11/28 (EST)
#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)
			$AoHStackRef->[ ${ $iStackRef } ]->{ 'fate' } == $CONTENT_DELETE	# "부모 스택"의 fate값이 CONTENT_DELETE라면
		) {
			# "자식 스택"의 fate값은 CONTENT_DELETE이다.
			$fate = $CONTENT_DELETE;
		}

		# "자식 스택"에 대한 정보를 담을 구조체(해쉬)를 초기화한다.
		my	$hStackRef = {
			'type'		=> ( $enumRIBDirective == $ENUM_RIB_DIRECTIVE__ATTRIBUTE_BEGIN ? $STACK_TYPE__ATTRIBUTE : $STACK_TYPE__TRANSFORM),
			'name'		=> '',	# Deep shadow의 경우처럼,
						# '+'로 시작하는 두 번째 "Display"에 필요한 정보가 있을 수 있다.
						# 이런 경우에는 이 값을 "undef"으로 설정해서, 적절하게 처리하도록 한다.
						# (아래의 "Display"를 처리하는 부분에서 '+'인 경우를 처리할 때를 보라.)
			'content'	=> [],
			'fate'		=> $fate
		};
		# Subframe과 "Bake" 과정에 사용되는 subframe의 내용을 담을 공간을 확보한다.
		# "단순" subframe의 경우에는 공유한다.
		$hStackRef -> { 'bake subframe content' } = [] if $BAKEFH;
		# fate값에 따라 AttributeBegin이나 TransformBegin에 해당하는 내용을 처리한다.
		if ( $fate == $CONTENT_SAVE ) {
			push(@{ $hStackRef ->{ 'content'} }, @$RIBDirectiveContentRef);
			push(@{ $hStackRef ->{ 'bake subframe content'} }, @$RIBDirectiveContentRef) if $BAKEFH;
		} else {	# $CONTENT_DELETE
			# 삭제하는 경우에는 아무것도 하지 않는다.
		}

		# "자식 스택"으로 등록한다.
		$AoHStackRef->[ ++${ $iStackRef } ] = $hStackRef;

		# 다음 "RIB Directive"내용을 읽는다.
		next;
	} elsif (
		$enumRIBDirective == $ENUM_RIB_DIRECTIVE__ATTRIBUTE_END ||
		$enumRIBDirective == $ENUM_RIB_DIRECTIVE__TRANSFORM_END
	) {
#		print STDERR "\t" x $iStack . ']' . $/;
		# "자식 스택"의 끝이다.
		my	$hStackRef = $AoHStackRef->[ ${ $iStackRef } ];
		my	$fate = $hStackRef->{ 'fate' };
		# 먼저, "자식 스택"의 fate값에 따라 AttributeEnd이나 TransformEnd에 해당하는 내용을 처리한다.
		if ( $fate == $CONTENT_SAVE ) {
			push(@{ $hStackRef->{ 'content'} }, @$RIBDirectiveContentRef);
			push(@{ $hStackRef->{ 'bake subframe content'} }, @$RIBDirectiveContentRef) if $BAKEFH;
		} elsif ($fate == $CONTENT_PRINT) {
			print $FH @$RIBDirectiveContentRef if $gElementName ne 'Image' || !$bBake;
			print $SUBFH @$RIBDirectiveContentRef if $SUBFH;
			print $BAKEFH @$RIBDirectiveContentRef if $BAKEFH;
		} else {	# $CONTENT_DELETE
			# 삭제하는 경우에는 아무것도 하지 않는다.
		}
		# 이 "TransformEnd"나 "AttributeEnd"에서 $gObjectName을 설정되었다면
		# 여기서 undef으로 초기화한다.
		if ( defined( $giObjectNameStackIndex ) && $$iStackRef == $giObjectNameStackIndex ) {
			$gObjectName = undef;
			$giObjectNameStackIndex = undef;
		}
		# 여기까지가 $hStackRef를 통해 참고하는, "자식 스택"에 대한 모든 정보이다.
		--${ $iStackRef };	# "부모 스택"을 접근한다.
		# "보모 스택"의 fate값에 따라 "자식 스택"의 내용을 처리한다.
		# 자세한 것은 @AoHStack에 대한 설명에서 "fate"에 대한 2.부분을 보라.
		if (
#			$iStack > -1 &&		# "부모 스택"이 있고
# Frame을 최상위 스택으로 설정했고
# AttributeEnd이나 TransformEnd은 Frame안에 있으므로,
# $iStack의 값은 항상 -1보다 크다.
#	2006/11/28 (EST)
#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)
			$AoHStackRef->[ ${ $iStackRef } ] -> { 'fate' } == $CONTENT_SAVE	# fate값이 CONTENT_SAVE이라면
		) {
			# "부모 스택"의 내용에 "자식 스택"의 내용을 추가한다. (append)
			push(@{ $AoHStackRef->[ ${ $iStackRef } ]->{ 'content' } }, @{ $hStackRef->{ 'content' } });
			push(@{ $AoHStackRef->[ ${ $iStackRef } ]->{ 'bake subframe content' } }, @{ $hStackRef->{ 'bake subframe content' } }) if $BAKEFH;
		} else {
			# 방금 처리가 끝난 스택이 최상위 스택이거나 ("부모 스택"이 없거나)
			# "부모 스택"의 fate값이 CONTENT_PRINT인 경우이다.
			print $FH @{ $hStackRef->{ 'content' } } if $gElementName ne 'Image' || !$bBake;
			print $SUBFH @{ $hStackRef->{ 'content' } } if $SUBFH;
			print $BAKEFH @{ $hStackRef->{ 'bake subframe content' } } if $BAKEFH;
		}

		# "자식 스택"을 해제한다.
		--$#{ $AoHStackRef };

		# 다음 "RIB Directive"내용을 읽는다.
		next;
	}

	if ( $bDeclare == -1 ) {
		# FrameEnd와 FrameBegin사이에 존재하는 "RIB Directive"가 있다.
		# 예를 들어, "System"...
		# 있는 그대로 쓴다.
		print $FH @$RIBDirectiveContentRef if $gElementName ne 'Image' || !$bBake;
		print $SUBFH @$RIBDirectiveContentRef if $SUBFH;
		print $BAKEFH @$RIBDirectiveContentRef if $BAKEFH;

		# 다음 "RIB Directive"내용을 읽는다.
		next;
	}

	if ( defined( $fate ) && $fate == $CONTENT_DELETE) {
		# 삭제될 내용이므로 읽은 내용은 버린다.
		#
		# 다음 "RIB Directive"내용을 읽는다.
		next;
	}

	# fate값은 CONTENT_PRINT 또는 CONTENT_SAVE이다.

	# 출력(CONTENT_PRINT)하거나 저장(CONTENT_SAVE)하기 전에
	# Display에 해당하는 내용은 "작업 chunk"에 따라 바뀌어야 한다.
	# 물론 삭제(CONTENT_DELETE)될 Frame이라면 바꿀 필요도 없지만...
	if ( $enumRIBDirective == $ENUM_RIB_DIRECTIVE__DISPLAY ) {
		print STDERR "EDITRIB: Info: Display: {@$RIBDirectiveContentRef}\n" if $bVVerbose;
		# 현재는 최상위 "스택"이다. ($iStack = -1)
		# "운명"도 기본값인 CONTENT_SAVE이다.
		# Display문자열을 해석해서
		# Frame의 운명을 결정한다. (CONTENT_PRINT냐 CONTENT_DELETE냐...)

		unless ( defined( $hDisplay{ 'name' } ) ) {
			$hStackRef->{ 'name' } = undef;	# 아직까지 제대로된 "Display"정보를 얻지 못했다.
			# 나중에 출력할 때 이 정보 즉
			#	Display "null" "null" "z"
			# 을 쓰도록 한다.
#			push( @{ $hStackRef->{ 'content' }}, @$RIBDirectiveContentRef);

			# 다음 "RIB Directive"내용을 읽는다.
			next;
		}

		# $type, $mode를 통해서 "운명"을 결정한다.
		my	$hStackRef = $AoHStackRef->[ 0 ];
		my	$newFate = $CONTENT_PRINT;	# "쉐도우 이미지"를 처리 중이면서,
							# 라이트가
							# [작업 3]에서 사용되지 않는 라이트라면
							# CONTENT_DELETE로 바뀐다.

		my	$plusSign = $hDisplay{ '+' };
		# 여러 개의 "Display" Directive가 있을 수 있다.
		if ( $plusSign ) {
			my	$displayName = $hStackRef->{ 'name' };
			if ( defined( $displayName) && !$displayName ) {
				# "렌더링 이미지"의 경우에...
				# $hStackRef->{ 'fate' }는 항상 $CONTENT_PRINT이다.
				print $FH '#' . join( '#', @$RIBDirectiveContentRef ) if $gElementName ne 'Image' || !$bBake;
				print $SUBFH '#' . join( '#', @$RIBDirectiveContentRef ) if $SUBFH;
				print $BAKEFH '#' . join( '#', @$RIBDirectiveContentRef ) if $BAKEFH;

				# 다음 "RIB Directive"내용을 읽는다.
				next;
			}
		}

		my	$name = $hDisplay{ 'name' };
		my	$type = $hDisplay{ 'type' };
		my	$mode = $hDisplay{ 'mode' };
		my	$dummy = $hDisplay{ 'dummy' };
		my	$path = $hDisplay{ 'path' };
		my	$filename = $hDisplay{ 'filename' };
		my	$dspyName = $hDisplay{ 'dspyName' };
		my	$dspyName0 = $hDisplay{ 'dspyName0' };	# $filename에서 "$dspyName"을 제외한 나머지 부분
		my	$frame = $hDisplay{ 'frame' };

		my	$light = $hDisplay{ 'light' };
		if ( $light ) {		# "쉐도우 이미지"
					# ($type eq "shadow" || $type eq 'deepshad' )와 동일
			my	$shadowType = $hDisplay{ 'shadowType ' };

			$hStackRef->{ 'name' } = $light;
			$HoALightSourceRef->{ $light } = [ $light, $name, $filename ];
			# 아래에서 "총 경로"를 구한 뒤, "총 경로"를 기억한다.
			if ($typeChunk == $CHUNK_TYPE__SHD) {	# [작업 3]
				my	$AoHObjectSHDRef = $hChunkRef->{ 'object array' };
				my	$AoHShadowOfAByLOntoBRef = $hChunkRef->{ 'jcref' };
				foreach my $hShadowOfAByLOntoBRef ( @{ $AoHShadowOfAByLOntoBRef } ) {
					if (isMatched($light, $hShadowOfAByLOntoBRef->{ 'light set' }) == 1) {
						push( @{ $AoHObjectSHDRef }, $hShadowOfAByLOntoBRef );
					}
				}
				if ( @{ $AoHObjectSHDRef } == 0 ) {	# 이 작업에 사용되는 라이트가 하나도 없다.
					$newFate = $CONTENT_DELETE;
				}
			} else {	# $CHUNK_TYPE__RS
				# 전체 배열중에서 이 라이트가 관련된 일부만의 배열을 생각한다.
				my	$AoHObjectLLRef = $hChunkRef->{ 'object array' };
				my	$AoHLightLinkingRef = $hChunkRef->{ 'llref' };
				foreach my $hLightLinkingRef ( @{ $AoHLightLinkingRef } ) {
					if (isMatched($light, $hLightLinkingRef->{ 'light set' }) == 1) {
						push( @{ $AoHObjectLLRef }, $hLightLinkingRef );
					}
				}
				if ( @{ $AoHObjectLLRef } == 0 ) {	# 이 작업에 사용되는 라이트가 하나도 없다.
					$newFate = $CONTENT_DELETE;
				}
			}
		} else {		# "렌더링 이미지"
			my	$imageFormatExtension = $hDisplay{ 'imageFormatExtension' };

			$hStackRef->{ 'name' } = '';
			$newFate = $CONTENT_PRINT;
		}

		my	$newDisplayLine;	# "Display ..."에 대신해서 들어갈 내용
		my	$newBakeDisplayLine;	# "Bake"모드에서 "Display..."에 대신에 들어갈 내용
		my	$newUNIXPath;
		my	$subframeRIBPathname;	# Subframe으로 쪼개졌을 때의 이름
	if ( $newFate == $CONTENT_PRINT ) {
#		print STDERR 'Path:= ' . '{' . $path . '}' . ', filename:= ' . '{' . $filename . '}' . $/;
#		print STDERR 'rootPathExpr:= ' . '{' . $rootPathExpr . '}' . ', addPathExpr:= ' . '{' . $addPathExpr . '}' . $/;
		$newUNIXPath = ( $rootPathExpr ? eval( 'qq|' . $rootPathExpr . '|' ) . '/' . ( $type eq 'shadow' || $type eq 'deepshad' ? 'rmantex/shd' : 'rmanpix' ) . '/' : $path ) . eval( 'qq|' . $addPathExpr . '|' ) . '/';
		print STDERR 'EDITRIB: Info: PATH.IMAGE: New Path:= ' . '{' . $newUNIXPath . '}' . $/ if $bVerbosePath;
		$newUNIXPath = mkdir2( $newUNIXPath );
		print STDERR 'EDITRIB: Info: PATH.IMAGE: New Path2:= ' . '{' . $newUNIXPath . '}' . $/ if $bVerbosePath;
		my	$subframeImagePathname = $newUNIXPath . $dspyName . '.' . $keyChunk . $dspyName0;
		print STDERR 'EDITRIB: Info: SUBFRAME IMAGE: TYPE:= ' . '{' . ($type) . '}' . ': PATHNAME:= ' . '{' . $subframeImagePathname . '}' . $/;

		if ( $light ) {	# $type eq 'shadow'와 동일
			$HoALightSourceRef->{ $light }->[ 2 ] = $subframeImagePathname;	# "총 경로"를 기억한다.
			if ( $newFate != $CONTENT_DELETE ) {
				# "쉐도우 맵(텍스쳐)"의 경우에,
				# 기존에 있던 텍스쳐를 사용한다면,
				# 해당하는 FrameBegin...FrameEnd는 필요없다.
				my	$bExist = -f $subframeImagePathname;
				print STDERR 'EDITRIB: Info: Shadow Map Texture: ' . '{' . $subframeImagePathname . '}' . ': Exists? ' . ( $bExist ? 'Yes' : 'No' ) . $/ if $bVerbose;
				$newFate = $CONTENT_DELETE if ( $bExist && $bShd );
			}
		}

		# Display의 name부분을 "작업 chunk"에 따라 적절하게 바꾸어 준다.
		$newDisplayLine = '#' . join( '#', @$RIBDirectiveContentRef );	# 현재 내용은 모두 주석처리한다.
		$newBakeDisplayLine = $newDisplayLine . 'Display "null" "null" "z"' if $bBake;
#		my	$bakeImagePathname = $subframeImagePathname;
#		$bakeImagePathname =~ s/(\d+)/NULL.$1/;
#		$newBakeDisplayLine = $newDisplayLine . qq/Display "$bakeImagePathname" "$type" "$mode"$dummy/ if $bBake;
		$newDisplayLine .= qq/Display "$plusSign$subframeImagePathname" "$type" "$mode"$dummy/;
		if ( $hStackRef->{ 'name' } eq '' && $typeChunk == $CHUNK_TYPE__RS ) {
			my	$hSecondaryRef = $hChunkRef->{ 'jcref' }->{ 'secondary' };
			foreach my $channelName ( keys( %{ $hSecondaryRef } ) ) {
				$newDisplayLine .= 'Display "+' . $newUNIXPath . $dspyName . '.' . $keyChunk . '.' . $channelName . '.' . $frame . '.tif" "tiff" "' . join( ',', @{ $hSecondaryRef->{ $channelName } } ) . '"' . "\r\n";
			}
		}

		if ( $light && $hDisplay{ 'shadowType' } eq 'dsh' ) {
			# Deep shadow의 경우이다.
			# 먼저 다음 정보를 쓴다.
			#	Display "null" "null" "z"
			$newDisplayLine = 'Display "null" "null" "z"' . "\r\n" . $newDisplayLine;
		}

		# RIB Box가 들어갈 위치이다. (ENUM_RIB_BOX__FRAME__JUST_BEFORE_DISPLAY)
		if ( $bVerboseRIBBox ) {
			my	$frameName = $light ? 'Shadow: ' . $light : 'Image';
			print STDERR 'EDITRIB: Info: RIBBOX.FRAME(' . $frameName . '): Just before display' . $/;
			$newDisplayLine = '## EDITRIB ## RIB Box: Frame(' . $frameName . '): Just before display' . "\r\n" . $newDisplayLine;
			$newBakeDisplayLine = '## EDITRIB ## RIB Box: Frame(' . $frameName . '): Just before display' . "\r\n" . $newBakeDisplayLine if $bBake;
		}

		# 들어갈 "RIB Box"가 있는 지 알아본다.
		my	$RIBBoxExpr = getRIBBoxExpr(
			$hChunkRef->{ 'rbref' },			
			$ENUM_RIB_BOX__FRAME__JUST_BEFORE_DISPLAY,
			$gElementName
		);
		if ( $RIBBoxExpr) {
			my	$RIBBoxContent = '## EDITRIB ## <RIB Box> ####' . "\r\n"
				. evalRIBBoxExpr( $RIBBoxExpr ) . "\r\n"
				. '## EDITRIB ## </RIB Box> ####' . "\r\n";
			$newDisplayLine = $RIBBoxContent . $newDisplayLine;
			$newBakeDisplayLine = $RIBBoxContent . $newBakeDisplayLine if $bBake;
		}
		$newBakeDisplayLine = <<BAKETEXT if $bBake;
## EDITRIB ## <BAKE> ####\r
#### Additional display channels for bake3d from subsurface scattering in Delux shader\r
DisplayChannel "float _area" "int[4] quantize" [0 0 0 0] "float dither" [0]\r
DisplayChannel "color _radiance_t" "int[4] quantize" [0 0 0 0] "float dither" [0]\r
DisplayChannel "color _albedo" "int[4] quantize" [0 0 0 0] "float dither" [0]\r
DisplayChannel "color _diffusemeanfreepath" "int[4] quantize" [0 0 0 0] "float dither" [0]\r
## EDITRIB ## </BAKE> ####\r
$newBakeDisplayLine\r
BAKETEXT

		# $hStackRef -> { 'contetn' }의 내용을 쓴 후, $newDisplayLine또는 $newBakeDisplayLine을 쓴다.
#		push( @{ $hStackRef->{ 'content' }}, $newDisplayLine );
	}

		if ($newFate == $CONTENT_DELETE) {
			# Subframe에 해당하는 핸들을 무효로 해서,
			# "FrameEnd"안에서 아무 일도 하지 않도록 한다.
			$hChunkRef->{ 'subframe file handle' } = undef;

			# 지금까지 모은 내용을 모두 버리고
			$hStackRef->{ 'content' } = [];
			$hStackRef->{ 'fate' } = $CONTENT_DELETE;

			# 다음 "RIB Directive"내용을 읽는다.
			next;
		}
		# $CONTENT_PRINT
		if ( $light ) {
			#	( $path, $filename, $dspyName, $dspyName0, $light, $shadowType, $frame ) =
			#		$name =~ /(.*?)(([^\/]+?)(\.([^\.]+?)\.(shd|dsh)\.(\d+)\.tex))$/;
			# $newUNIXPath에는 "rmantex/shd"처럼 "텍스쳐 경로"가 들어있다.
			my	$newUNIXRIBPath = $newUNIXPath;
			$newUNIXRIBPath =~ s|rmantex/shd|rib|;
			$subframeRIBPathname = $newUNIXRIBPath . $dspyName . '.' . $keyChunk . '.' . ($light . '.' . $hDisplay{ 'shadowType' } . '.' . $frame . '.' . 'rib');
		} else {
			# $newUNIXPath에는 "rmanpix"처럼 "이미지 경로"가 들어있다.
			my	$newUNIXRIBPath = $newUNIXPath;
			$newUNIXRIBPath =~ s|rmanpix|rib|;
			#	( $path, $filename, $dspyName, $dspyName0, $frame, $imageFormatExtension ) =
			#		$name =~ /(.*?)(([^\/]+?)(\.(\d+)(.*)))$/;
			$subframeRIBPathname = $newUNIXRIBPath . $dspyName . '.' . $keyChunk . '.' . 'FinalPass' . '.' . ( $frame . '.' . 'rib' );

	if ( $bBake ) {
			# "Bake"과정에서 사용될 subframe을 따로 저장한다.
			my	$bakeSubframeRIBPathname = $newUNIXRIBPath . $dspyName . '.' . $keyChunk . '.' . 'BakePass' . '.' . ( $frame . '.' . 'rib' );
			print STDERR 'EDITRIB: Info: BAKE: Bake Pass RIB: PATHNAME:= ' . '{' . $bakeSubframeRIBPathname . '}' . $/;
			$hChunkRef->{ 'bake subframe pathname' } = $bakeSubframeRIBPathname;
			my	$bakeSubframeFileHandle = 'bakeSubFh' . $keyChunk;
			if ( $bAscii ) {
				open( $bakeSubframeFileHandle, '> ' . $bakeSubframeRIBPathname ) ||
					die 'EDITRIB: Error: open for "Bake" "subframe"(' . $bakeSubframeRIBPathname . ') of "chunk"(' . $keyChunk . '): Failed';
			} else {
				open( $bakeSubframeFileHandle, '| catrib -binary -gzip -o "' . $bakeSubframeRIBPathname . '"' ) ||
					die 'EDITRIB: Error: open for "Bake" "subframe"(' . $bakeSubframeRIBPathname . ') of "chunk"(' . $keyChunk . '): Failed';
			}
			$BAKEFH = $bakeSubframeFileHandle;
			$hChunkRef->{ 'bake subframe file handle' } = $BAKEFH;

			print $BAKEFH <<BAKETEXT;
## EDITRIB ## <BAKE> ####\r
System "echo Bake Pass: Step 1/3: Making Point Cloud Files..."\r
## EDITRIB ## </BAKE> ####\r
BAKETEXT
			print $BAKEFH $strDeclare;	# 항상 "declare"부분을 먼저 쓴다.
			my	$RIBBoxExpr = getRIBBoxExpr(
				$hChunkRef->{ 'rbref' },			
				$ENUM_RIB_BOX__DECLARE__JUST_BEFORE_FIRST_FRAME_BEGIN
			);
			if ( $RIBBoxExpr) {
				print $BAKEFH '## EDITRIB ## <RIB Box> ####' . "\r\n"
					. evalRIBBoxExpr( $RIBBoxExpr ) . "\r\n"
					. '## EDITRIB ## </RIB Box> ####' . "\r\n";
			}
			my	$newUNIXTexturePath = $newUNIXPath;
			$newUNIXTexturePath =~ s|/rmanpix/|/rmantex/shd/|;
			$hChunkRef -> { 'TexturePathPlusDisplayNamePlusKeyChunk' } = "$newUNIXTexturePath$dspyName.$keyChunk.Bake.";
			print $BAKEFH <<BAKETEXT;
## EDITRIB ## <BAKE> ####\r
#### User options which the shader use to know "Bake Context"\r
Option "user" "uniform string SubsurfaceScattering_CurrentContext" [ "BAKECONTEXT" ]\r
Option "user" "uniform string TexturePathPlusDisplayNamePlusKeyChunk" [ "$hChunkRef->{ 'TexturePathPlusDisplayNamePlusKeyChunk' }" ]	# Append "\$OBJECT.\$FRAME.[ptc|bkm\" later.\r
## EDITRIB ## </BAKE> ####\r
BAKETEXT
			print $BAKEFH @{ $hStackRef->{ 'content' } };
			print $BAKEFH $newBakeDisplayLine;
	}
		}
		print STDERR 'EDITRIB: Info: SUBFRAME RIB: PATHNAME:= ' . '{' . $subframeRIBPathname . '}' . $/ if $bVerbosePath;

	if ( $bBake && $gElementName eq 'Image' ) {	# "Bake"모드에서 "렌더링 이미지"에 대한 부분은 따로 뽑는다.
# <Subframe을 따로 저장한다>
		# $subframeRIBPathname에 해당하는 파일을 열어,
		# subframe을 담은 RIB을 만든다.
		# "FrameEnd"에서 $SUBFH이 유효하면 파일을 닫는다.
		$hChunkRef-> { 'subframe pathname' } = $subframeRIBPathname;
		print STDERR 'EDITRIB: Info: BAKE: Final Rendering RIB: PATHNAME:= ' . '{' . $subframeRIBPathname . '}' . $/;
		my	$subframeFileHandle = 'subFh' . $keyChunk;
		if ( $bAscii ) {
			open( $subframeFileHandle, '> ' . $subframeRIBPathname ) ||
				die 'EDITRIB: Error: open for "subframe"(' . $subframeRIBPathname . ') of "chunk"(' . $keyChunk . '): Failed';
		} else {
			open( $subframeFileHandle, '| catrib -binary -gzip -o "' . $subframeRIBPathname . '"' ) ||
				die 'EDITRIB: Error: open for "subframe"(' . $subframeRIBPathname . ') of "chunk"(' . $keyChunk . '): Failed';
		}
		$SUBFH = $subframeFileHandle;
		$hChunkRef->{ 'subframe file handle' } = $SUBFH;

		print $SUBFH $strDeclare;	# 항상 "declare"부분을 먼저 쓴다.
		my	$RIBBoxExpr = getRIBBoxExpr(
			$hChunkRef->{ 'rbref' },			
			$ENUM_RIB_BOX__DECLARE__JUST_BEFORE_FIRST_FRAME_BEGIN
		);
		if ( $RIBBoxExpr) {
			print $SUBFH '## EDITRIB ## <RIB Box> ####' . "\r\n"
				. evalRIBBoxExpr( $RIBBoxExpr ) . "\r\n"
				. '## EDITRIB ## </RIB Box> ####' . "\r\n";
		}
		print $SUBFH <<BAKETEXT;
## EDITRIB ## <BAKE> ####\r
#### User options which the shader use to know "Bake Context"\r
Option "user" "uniform string SubsurfaceScattering_CurrentContext" [ "FINALIMAGECONTEXT" ]\r
Option "user" "uniform string TexturePathPlusDisplayNamePlusKeyChunk" [ "$hChunkRef->{ 'TexturePathPlusDisplayNamePlusKeyChunk' }" ]	# Append "\$objectName.\$FRAME.bkm" later.\r
## EDITRIB ## </BAKE> ####\r
BAKETEXT
		print $SUBFH @{ $hStackRef->{ 'content' } };
		print $SUBFH $newDisplayLine;
# </Subframe을 따로 저장한다>
	}

		# 아직까지 "declare"부분을 쓰지 않았다면 먼저 이 부분을 쓴다.
		unless ( $hChunkRef->{ 'declare written' } ) {
			$hChunkRef-> { 'declare written' } = 1;
			print $FH $strDeclare if $gElementName ne 'Image' || !$bBake;
			my	$RIBBoxExpr = getRIBBoxExpr(
				$hChunkRef->{ 'rbref' },			
				$ENUM_RIB_BOX__DECLARE__JUST_BEFORE_FIRST_FRAME_BEGIN
			);
			if ( $RIBBoxExpr) {
				print $FH '## EDITRIB ## <RIB Box> ####' . "\r\n"
					. evalRIBBoxExpr( $RIBBoxExpr ) . "\r\n"
					. '## EDITRIB ## </RIB Box> ####' . "\r\n" if $gElementName ne 'Image' || !$bBake;
			}
		}
		# 지금까지 모은 내용을 출력하고
		print $FH @{ $hStackRef->{ 'content' } } if $gElementName ne 'Image' || !$bBake;
		print $FH $newDisplayLine if $gElementName ne 'Image' || !$bBake;
		$hStackRef->{ 'content' } = [];
		$hStackRef->{ 'bake subframe content' } = [] if $BAKEFH;
		# fate값을 CONTENT_PRINT으로 바꾼다.
		$hStackRef->{ 'fate' } = $CONTENT_PRINT;

		# 다음 "RIB Directive"내용을 읽는다.
		next;
	}


	# RIB Box가 들어갈 위치를 정한다.
{
	my	$light = $AoHStackRef -> [ 0 ] -> { 'name' };
	my	$frameName = $light ? 'Shadow: ' . $light : 'Image';

	if ( $enumRIBDirective == $ENUM_RIB_DIRECTIVE__WORLD_BEGIN ) {
		# RIB Box가 들어갈 위치이다. (ENUM_RIB_BOX__FRAME__JUST_BEFORE_WORLD_BEGIN)
		print STDERR 'EDITRIB: Info: RIBBOX.FRAME' . $frameName . '): Just before "WorldBegin"' . $/ if $bVerboseRIBBox;
		unshift( @$RIBDirectiveContentRef, '## EDITRIB ## RIB Box: Frame(' . $frameName . '): Just before "WorldBegin"' . "\r\n" ) if $bVerboseRIBBox;

		# 들어갈 "RIB Box"가 있는 지 알아본다.
		my	$RIBBoxExpr = getRIBBoxExpr(
			$hChunkRef->{ 'rbref' },			
			$ENUM_RIB_BOX__FRAME__JUST_BEFORE_WORLD_BEGIN,
			$gElementName
		);
		if ( $RIBBoxExpr) {
			my	$RIBBoxContent = '## EDITRIB ## <RIB Box> ####' . "\r\n"
				. evalRIBBoxExpr( $RIBBoxExpr ) . "\r\n"
				. '## EDITRIB ## </RIB Box> ####' . "\r\n";
			unshift( @$RIBDirectiveContentRef, $RIBBoxContent );
		}
	}
	if ( $enumRIBDirective == $ENUM_RIB_DIRECTIVE__WORLD_END ) {
		# RIB Box가 들어갈 위치이다. (ENUM_RIB_BOX__WORLD__JUST_AFTER_LAST_OBJECT_BLOCK)
		print STDERR 'EDITRIB: Info: RIBBOX.FRAME(' . $frameName . ').WORLD: Just after LAST object block' . $/ if $bVerboseRIBBox;
		unshift( @$RIBDirectiveContentRef, '## EDITRIB ## RIB Box: Frame(' . $frameName . ').World: Just after LAST object block' . "\r\n" ) if $bVerboseRIBBox;

		# 들어갈 "RIB Box"가 있는 지 알아본다.
		my	$RIBBoxExpr = getRIBBoxExpr(
			$hChunkRef->{ 'rbref' },			
			$ENUM_RIB_BOX__WORLD__JUST_AFTER_LAST_OBJECT_BLOCK,
			$gElementName
		);
		if ( $RIBBoxExpr) {
			my	$RIBBoxContent = '## EDITRIB ## <RIB Box> ####' . "\r\n"
				. evalRIBBoxExpr( $RIBBoxExpr ) . "\r\n"
				. '## EDITRIB ## </RIB Box> ####' . "\r\n";
			unshift( @$RIBDirectiveContentRef, $RIBBoxContent );
		}
	}
	if ( $enumRIBDirective == $ENUM_RIB_DIRECTIVE__GEOMETRIC_PRIMITIVE ) {
		# RIB Box가 들어갈 위치이다. (ENUM_RIB_BOX__OBJECT__JUST_BEFORE_GEOMETRIC_PRIMITIVE)
		my	( $objectName, $iStack ) = getStackName( $AoHStackRef );
		print STDERR 'EDITRIB: Info: RIBBOX.FRAME(' . $frameName . ').OBJECT(' . $objectName . '): Just before geometric primitive' . $/ if $bVerboseRIBBox;
#		unshift( @$RIBDirectiveContentRef, '## EDITRIB ## RIB Box: Frame(' . $frameName . ').Object(' . $objectName . '): Just before geometric primitive' . "\r\n" ) if $bVerboseRIBBox;
		addArrayRef( $RIBDirectiveContentRef, 0, \"## EDITRIB ## RIB Box: Frame($frameName).Object($objectName): Just before geometric primitive\r\n" ) if $bVerboseRIBBox;

		# 들어갈 "RIB Box"가 있는 지 알아본다.
		my	$RIBBoxExpr = getRIBBoxExpr(
			$hChunkRef->{ 'rbref' },			
			$ENUM_RIB_BOX__OBJECT__JUST_BEFORE_GEOMETRIC_PRIMITIVE,
			$gElementName
		);
		if ( $RIBBoxExpr) {
			my	$RIBBoxContent = '## EDITRIB ## <RIB Box> ####' . "\r\n"
				. evalRIBBoxExpr( $RIBBoxExpr ) . "\r\n"
				. '## EDITRIB ## </RIB Box> ####' . "\r\n";
#			unshift( @$RIBDirectiveContentRef, $RIBBoxContent );
			addArrayRef( $RIBDirectiveContentRef, 0, \$RIBBoxContent );
		}
	}

	# 다음 두 RIB Box가 들어갈 수 있는 지 알아본다.
	#	ENUM_RIB_BOX__WORLD__JUST_BEFORE_FIRST_LIGHTSOURCE_BLOCK
	#	ENUM_RIB_BOX__WORLD__JUST_BEFORE_FIRST_GEOMETRIC_PRIMITIVE_BLOCK
	# RIB Box가 들어갈 수 있다면,
	# CONTENT_SAVE의 경우에 (CONTENT_DELETE가 아니므로) CONTENT_PRINT로 바뀐다.
	# 이미 이러한 RIB Box의 위치를 결정했다면($hChunkRef->{ 'found geometric primitive' }이 설정되었다면)
	# CONTENT_SAVE가 CONTENT_PRINT로 바뀔 수 있는 지는
	# Attribute "identifier" "name"...에서 결정한다.
	#
	#	2007/5/6 (EST)
	#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)
	my	$bPrint;
	if (
		$enumRIBDirective == $ENUM_RIB_DIRECTIVE__LIGHTSOURCE &&
		! $hChunkRef->{ 'found light source' }		# 아직 첫 번째 LightSource를 만나지 않았다면
	) {
		$hChunkRef->{ 'found light source' } = 1;
		$bPrint = 1 if $fate == $CONTENT_SAVE;

		# RIB Box가 들어갈 위치이다. (ENUM_RIB_BOX__WORLD__JUST_BEFORE_FIRST_LIGHTSOURCE_BLOCK)
		my	$light = $AoHStackRef->[ 0 ]->{ 'name' };
		my	$frameName = $light ? 'Shadow: ' . $light : 'Image';
		print STDERR 'EDITRIB: Info: RIBBOX.FRAME(' . $frameName . ').WORLD: Just before FIRST LightSource block' . $/ if $bVerboseRIBBox;
		my	$RIBBox = '## EDITRIB ## RIB Box: Frame(' . $frameName . ').World: Just before FIRST LightSource block' . "\r\n";
		print $FH $RIBBox if $bVerboseRIBBox && ($gElementName ne 'Image' || !$bBake);
		print $SUBFH $RIBBox if $SUBFH && $bVerboseRIBBox;
		print $BAKEFH $RIBBox if $BAKEFH && $bVerboseRIBBox;

		# 들어갈 "RIB Box"가 있는 지 알아본다.
		my	$RIBBoxExpr = getRIBBoxExpr(
			$hChunkRef->{ 'rbref' },			
			$ENUM_RIB_BOX__WORLD__JUST_BEFORE_FIRST_LIGHTSOURCE_BLOCK,
			$gElementName
		);
		if ( $RIBBoxExpr) {
			my	$RIBBoxContent = '## EDITRIB ## <RIB Box> ####' . "\r\n"
				. evalRIBBoxExpr( $RIBBoxExpr ) . "\r\n"
				. '## EDITRIB ## </RIB Box> ####' . "\r\n";
			print $FH $RIBBoxContent if $gElementName ne 'Image' || !$bBake;
			print $SUBFH $RIBBoxContent if $SUBFH;
			print $BAKEFH $RIBBoxContent if $BAKEFH;
		}
	} elsif (
		$enumRIBDirective == $ENUM_RIB_DIRECTIVE__GEOMETRIC_PRIMITIVE &&
		! $hChunkRef->{ 'found geometric primitive' }	# 아직 첫 번째 "geometric primitive"를 만나지 않았다면
	) {
		$hChunkRef->{ 'found geometric primitive' } = 1;
		$bPrint = 1 if $fate == $CONTENT_SAVE;

		# RIB Box가 들어갈 위치이다. (ENUM_RIB_BOX__WORLD__JUST_BEFORE_FIRST_GEOMETRIC_PRIMITIVE_BLOCK)
		my	$light = $AoHStackRef->[ 0 ]->{ 'name' };
		my	$frameName = $light ? 'Shadow: ' . $light : 'Image';
		print STDERR 'EDITRIB: Info: RIBBOX.FRAME(' . $frameName . ').WORLD: Just before FIRST geometric primitive block' . $/ if $bVerboseRIBBox;
		my	$RIBBox = '## EDITRIB ## RIB Box: Frame(' . $frameName . ').World: Just before FIRST geometric primitive block' . "\r\n";
		print $FH $RIBBox if $bVerboseRIBBox && ($gElementName ne 'Image' || !$bBake);
		print $SUBFH $RIBBox if $SUBFH && $bVerboseRIBBox;
		print $BAKEFH $RIBBox if $BAKEFH && $bVerboseRIBBox;
		print $BAKEFH <<BAKETEXT if $BAKEFH;
## EDITRIB ## <BAKE> ####\r
	Attribute "cull" "hidden" 0		# don't cull hidden surfaces\r
	Attribute "cull" "backfacing" 0		# don't cull backfacing surfaces\r
	Attribute "dice" "rasterorient" 0	# turn viewdependent gridding off\r
## EDITRIB ## </BAKE> ####\r
BAKETEXT

		# 들어갈 "RIB Box"가 있는 지 알아본다.
		my	$RIBBoxExpr = getRIBBoxExpr(
			$hChunkRef->{ 'rbref' },			
			$ENUM_RIB_BOX__WORLD__JUST_BEFORE_FIRST_GEOMETRIC_PRIMITIVE_BLOCK,
			$gElementName
		);
		if ( $RIBBoxExpr) {
			my	$RIBBoxContent = '## EDITRIB ## <RIB Box> ####' . "\r\n"
				. evalRIBBoxExpr( $RIBBoxExpr ) . "\r\n"
				. '## EDITRIB ## </RIB Box> ####' . "\r\n";
			print $FH $RIBBoxContent if $gElementName ne 'Image' || !$bBake;
			print $SUBFH $RIBBoxContent if $SUBFH;
			print $BAKEFH $RIBBoxContent if $BAKEFH;
		}
	}
	if ( $bPrint ) {
		# $$iStackRef의 값과 $giObjectNameStackIndex의 값이 다르다면
		# $giObjectNameStackIndex에 해당하는 스택의 내용을 먼저 비워야 한다.
		# 그리고 해당 스택의 "운명"을 CONTENT_PRINT로 바꾼다.
		# $giObjectNameStackIndex가 설정되지 않을 수도 있다.
		# 이런 경우에는 "운명"이 CONTENT_SAVE인 상위 스택의 내용을 먼저 비운다.
		my	$iPrintableStackIndex = $giObjectNameStackIndex;
		unless ( $iPrintableStackIndex ) {	# $giObjectNameStackIndex가 설정되지 않았다면
			# "운명"이 CONTENT_SAVE인 최상위 스택을 찾는다.
			# 1부터 시작한다.
			# 적어도 Transform block이나 Attribute block안에 있어야 하기 때문이다.
			# 반드시 존재한다! (왜?)
			for ( my $i = 1; $i <= $$iStackRef; ++$i ) {
				if ( $AoHStackRef->[ $i ]->{ 'fate' } == $CONTENT_SAVE ) {
					$iPrintableStackIndex = $i;
					last;
				}
			}
		}
		for ( my $i = $iPrintableStackIndex; $i <= $$iStackRef; ++$i ) {
			my	$myhStackRef = $AoHStackRef->[ $i ];

			# 그 스택 내용을 출력하고,
			print $FH @{ $myhStackRef-> { 'content' } } if $gElementName ne 'Image' || !$bBake;
			print $SUBFH @{ $myhStackRef-> { 'content' } } if $SUBFH;
			print $BAKEFH @{ $myhStackRef-> { 'bake subframe content' } } if $BAKEFH;
			# 스택을 비운 후
			$myhStackRef->{ 'content' } = [];
			$myhStackRef->{ 'bake subframe content' } = [] if $BAKEFH;
			# 해당 스택의 "운명"을 CONTENT_PRINT로 바꾼다.
			$myhStackRef->{ 'fate' } = $CONTENT_PRINT;
		}

		# "운명"이 바뀌었다!
		$fate = $CONTENT_PRINT;
	}
}


	# 출력(CONTENT_PRINT)하거나 저장(CONTENT_SAVE)하기 전에
	# 쉐이더 정보를 바꿀 필요가 있다면 바꾼다.
	my	$shaderType =
		$enumRIBDirective == $ENUM_RIB_DIRECTIVE__LIGHTSOURCE ? $SHADER_TYPE__LIGHTSOURCE :
		$enumRIBDirective == $ENUM_RIB_DIRECTIVE__SURFACE ? $SHADER_TYPE__SURFACE :
		$enumRIBDirective == $ENUM_RIB_DIRECTIVE__DISPLACEMENT ? $SHADER_TYPE__DISPLACEMENT :
		$SHADER_TYPE__NONE;
	if (
		${ $iStackRef } > 0 &&	# 적어도 AttributeBegin...AttributeEnd안이거나
					# TransformBegin...TransformEnd안이여야 한다.
					# 불행히도(?) 최상위 스택에도
					# Surface라는 "RIB Directive"가 있다.
#		$AoHStackRef->[ 0 ]->{ 'name' } eq '' &&
					# "렌더링 이미지"를 만드는 단계에서만 쉐이더를 바꾼다.
		# Deep shadow의 경우에는 "쉐도우 이미지"에서도 쉐이더를 참고한다.
		$shaderType != $SHADER_TYPE__NONE
	) {
		# "RIB Directive"가 쉐이더이다.
while (1) {	# if...else를 사용하지 않고, 필요할 때마다 빠져나오기 위해서...
		# 이 쉐이더가 적용되는 오브젝트를 찾는다.
		my	( $name, $iStack ) = getStackName( $AoHStackRef );
		unless ( $name ) {
			print STDERR 'EDITRIB: Info: ...Failed' . $/ if $bVerbose;
			print STDERR 'EDITRIB: Warning: Cannot find object for shader: ' . join( '', @$RIBDirectiveContentRef );
			last;	# 루프를 빠져나간다.
		}
		print STDERR 'EDITRIB: Info: ...Ok' . $/ if $bVerbose;
	       	print STDERR 'EDITRIB: Info: Object:= ' . '{' . $name . '}' . $/ if $bVerbose;

		#### 새로운 쉐이더를 사용한다면 그것으로 대체한다.
		my	$AoHShaderInfoRef = $hChunkRef->{ 'siref' };
		my	$hShaderInfoRef;
		#================================================#
		foreach my $hSIRef ( @{ $AoHShaderInfoRef} ) {
			next if ( $shaderType ne $hSIRef->{ 'type' } );
			if (isMatched( $name, $hSIRef->{ 'set' } )) {
				$hShaderInfoRef = $hSIRef;
				last;
			}
		}
		#================================================#
		if ($hShaderInfoRef) {
			my	$newShaderInfo;	# 여기에다 새로운 쉐이더 정보를 담는다.
						# 1) CHUNK_TYPE__DEL: 주어진 것의 단순한 저장
						# 2) CHUNK_TYPE__LL: 주어진 것의 단순한 저장
						# 3) CHUNK_TYPE__SHD: 주어진 정보 중에서,
						# 쉐도우 맵에 관한 정보를 넣는다.
			$newShaderInfo = $hShaderInfoRef->{ 'content' };
			if ( $hChunkRef->{ 'type' } == $CHUNK_TYPE__SHD ) {
				# 주어진 쉐이더 정보($AoHShaderInfoByTd[0]->{ 'content' })에다가
				# 쉐이더의 파라미터인 "shadowTextureName"에 사용될 쉐도우 맵에 대한 정보를 얻는다.

				my	$AoHShadowOfAByLOntoBRef = $hChunkRef->{ 'jcref' };
				my	@aShadowTexture;	# 이 오브젝트가 받는 라이트들이 만드는 쉐도우 텍스쳐들로 이루어진 배열
				foreach my $light (keys( %{ $HoALightSourceRef } )) {
					# 주어진 라이트가,
					# 이 오브젝트에 비추는 지 알아본다.
					foreach my $hShadowOfAByLOntoBRef ( @{ $AoHShadowOfAByLOntoBRef } ) {
						# 주어진 라이트와 관련이 없는 정보는 무시한다.
						next unless (isMatched( $light, $hShadowOfAByLOntoBRef->{ 'light set' } ) == 1);
						# 이 오브젝트와 관련되지 않은 정보도 무시한다.
						next unless (isMatched( $name, $hShadowOfAByLOntoBRef->{ 'take set' } ) == 1);
						# {주어진 라이트, 현재 오브젝트}와 관련된 정보가 있다.
#						push( @aShadowTexture, $HoALightSourceRef->{ $light }->[ 2 ] );
						my	$shadowName = $HoALightSourceRef->{ $light }->[ 2 ];
						push( @aShadowTexture, $shadowName ) if $shadowName;
						last;	# 일치하는 첫 번째 정보가 우선권이 있다.
					}
				}
				$newShaderInfo .= ' "string[' . @aShadowTexture . '] shadowTexture" [ "' . join( '" "', @aShadowTexture ) . '" ]';
			}
			# 새로운 쉐이더로 바꾼다.
			# 원래 쉐이더는 없애거나 주석처리한다.
			@{ $RIBDirectiveContentRef }[ 0 ] = '#' . join('#', @$RIBDirectiveContentRef);
			#### 새로운 쉐이더를 추가한다.
			@{ $RIBDirectiveContentRef }[ 1 ] = $newShaderInfo . "\r\n";
		}

		# RIB Box가 들어갈 위치이다. (ENUM_RIB_BOX__OBJECT__JUST_BEFORE_SURFACE_SHADER/ENUM_RIB_BOX__OBJECT__JUST_BEFORE_LISHT_SHADER)
		my	$light = $AoHStackRef -> [ 0 ] -> { 'name' };
		my	$frameName = $light ? 'Shadow: ' . $light : 'Image';
		print STDERR 'EDITRIB: Info: RIBBOX.FRAME(' . $frameName . ').OBJECT(' . $name . '): Just before "' . $RIBDirective . '" shader' . $/ if $bVerboseRIBBox;
		unshift( @$RIBDirectiveContentRef, '## EDITRIB ## RIB Box: Frame(' . $frameName . ').Object(' . ( $name ) . '): Just before "' . $RIBDirective . '" shader' . "\r\n" ) if $bVerboseRIBBox;

		# 들어갈 "RIB Box"가 있는 지 알아본다.
	if (
		$enumRIBDirective == $ENUM_RIB_DIRECTIVE__LIGHTSOURCE ||
		$enumRIBDirective == $ENUM_RIB_DIRECTIVE__SURFACE
	) {
		my	$RIBBoxExpr = getRIBBoxExpr(
			$hChunkRef->{ 'rbref' },			
			$enumRIBDirective == $ENUM_RIB_DIRECTIVE__LIGHTSOURCE ? $ENUM_RIB_BOX__OBJECT__JUST_BEFORE_LIGHT_SHADER : $ENUM_RIB_BOX__OBJECT__JUST_BEFORE_SURFACE_SHADER,
			$gElementName
		);
		if ( $RIBBoxExpr) {
			my	$RIBBoxContent = '## EDITRIB ## <RIB Box> ####' . "\r\n"
				. evalRIBBoxExpr( $RIBBoxExpr ) . "\r\n"
				. '## EDITRIB ## </RIB Box> ####' . "\r\n";
			unshift( @$RIBDirectiveContentRef, $RIBBoxContent );
		}
	}

		# 쉐이더 종류가 "Surface"일 때,
		# $CHUNK_TYPE__LL인 경우에는 새로운 쉐이더를 사용여부에 상관없이
		# 쉐이더 바로 위에 "illuminate"정보가 써질 수 있다.
		#	2007/1/5 (EST)
		#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)
		if (
			$shaderType == $SHADER_TYPE__SURFACE &&
			$hChunkRef->{ 'type' } == $CHUNK_TYPE__RS
		) {
			# 이 오브젝트에서 처리되어야 할 "Render Stats"정보가 있는 지 알아본다.
			my	$RenderStats;
			my	$AoHRenderStatRef = $hChunkRef->{ 'jcref' }->{ 'primary' };
			foreach my $hRenderStatRef ( @{ $AoHRenderStatRef } ) {
				if (isMatched($name, $hRenderStatRef->{ 'object set' }) == 1) {
					$RenderStats = $hRenderStatRef->{ 'Render Stats' };
					last;
				}
			}

		if ( $RenderStats && ( $RenderStats & $RS__MATTE ) ) {
			unshift( @{ $RIBDirectiveContentRef }, 'matte 1' . "\r\n");
		} else {
			# 이 오브젝트에 비추지 않는 라이트는
			# 'illuminate "..." 0'으로 처리한다.
			# 'illuminate'가 1로 명시되지 "않은" 설정된 라이트만을 모은다.
			foreach my $light (keys( %{ $HoALightSourceRef } )) {
				# Light Linking 정보를 읽으면서
				my	$AoHLightLinkingRef = $hChunkRef->{ 'llref' };
				# 현재 오브젝트와 관련된 정보가 있는 지 확인한다.
				my	$illuminate = 0;	# 'illuminate'가 1로 명시된 라이트만이
								# 이 오브젝트에 비춘다.
				foreach my $hLightLinkingRef ( @{ $AoHLightLinkingRef } ) {
					# 주어진 라이트와 관련이 없는 정보는 무시한다.
					next unless (isMatched( $light, $hLightLinkingRef->{ 'light set' } ) == 1);
					# 이 오브젝트와 관련되지 않은 정보도 무시한다.
					next unless (isMatched( $name, $hLightLinkingRef->{ 'object set' } ) == 1);
					# {주어진 라이트, 현재 오브젝트}와 관련된 정보가 있다.
					$illuminate = 1;
					last;	# 일치하는 첫 번째 정보가 우선권이 있다.
				}
				# 이 오브젝트와 관련된 어떠한 라이트 정보(illuminate)가 얻을 수도 없다.
				# 기본값인 0을 사용한다.
				unless ( $illuminate ) {
					unshift( @{ $RIBDirectiveContentRef }, 'illuminate "' . $HoALightSourceRef->{ $light }->[ 0 ] . '" 0' . "\r\n");
				}
			}

			if ( $RenderStats && !( $RenderStats & $RS__RECV_SHADOW ) ) {
				push( @$RIBDirectiveContentRef, "\r\n" . '"string NotUsedShadowParameterName" [ "shadowname" ]' . "\r\n" );
			}

			if ( $RenderStats && ( $RenderStats & $RS__SSSC ) ) {
				if ( $bBake ) {
					my	$objectName = $name;
					$objectName =~ s/\|//g;	# Remove all '|'s.
					push( @{ $hChunkRef->{ 'SubsurfaceScattering_BakedObjects' } }, $objectName );
					my	$bakeText = <<BAKETEXT;
## EDITRIB ## <BAKE> ####\r
#### Additional shader parameters:\r
	"float SubsurfaceScattering_BakeMode"	[ 1 ]			# 0:= Off, 1:= On\r
	"string SubsurfaceScattering_PtcFile"	[ "$objectName.$gFrameNumber.ptc" ]	# Bake DATA into this file in "Bake Pass"\r
	"string SubsurfaceScattering_BakedFile"	[ "$objectName.$gFrameNumber.bkm" ]	# Read DATA from this file in "Final Rendering Pass"\r
## EDITRIB ## </BAKE> ####\r
BAKETEXT
					push( @{ $RIBDirectiveContentRef }, $bakeText );
				} else {
					print STDERR 'EDITRIB: Warning: BAKE: RS__SSSC가 설정되었으나 CHUNK_TYPE__BAKE가 설정되지 않았다: 무시된다' . $/;
				}
			}
		}
		}

		# 라이트 쉐이더에서 참고하는 쉐도우가 있다면,
		# "작업 chunk"에서 만든 쉐도우이름으로 적절하게 바꾸어 준다.
		if ( $enumRIBDirective == $ENUM_RIB_DIRECTIVE__LIGHTSOURCE ) {
			# 주어진 light의 handle에 대한 정보를 얻는다.
			# "LIghtSource"라는 "RIB Directive"를 통해 얻는다.
			#	LightSource name handle ...parameterlist...
			# 한 줄에 해당 정보가 없다면...ㅠㅠ
			#	2007/1/5 (EST)
			#	김광호 (Gwang-Ho Kim, iccentriic@gmail.com)
			if ( join( '', @{ $RIBDirectiveContentRef } ) =~ /LightSource\s+".*?"\s*"(.*?)"/s ) {
				# 이 LightSource가 어느 light에 대한 정보인지 알아야 한다.
				# 문제는 LightSource는 이름을 알 수 있는,
				# AttributeBegin...AttributeEnd사이에 있지 않다는 것이다.
				# 어쩔 수 없이 스택의 내용을 검사해서 알아야 한다.
				# 보통은 같은 스택에 있지만 한 단계 위에 있을 수도 있다.
				# 문제를 어렵게 만든 이 경우가 그렇다.
				my	$lightHandle = $1;
				$HoALightSourceRef->{ $name }->[ 0 ] = $lightHandle;
			}
			for (my $i = 0; $i < @{ $RIBDirectiveContentRef }; ++$i) {
				my	$lineRef = \$RIBDirectiveContentRef->[ $i ];
				my	$line = $$lineRef;
				next if ($line =~ /^\s*#/);	# 주석줄은 제외
				foreach my $light (keys( %{ $HoALightSourceRef } )) {
					# ["..."]안에 들어있는 것이, 라이트가 만드는 원래의 "쉐도우 텍스쳐"의 일부라면,
					# 새로 만든 "쉐도우 텍스쳐"의 경로로 바꾼다.

					# "쉐도우 텍스쳐"를 만들지 않는 라이트는 고려하지 않는다.
					next unless ( @{ $HoALightSourceRef->{ $light } } == 3 );
					$line =~ s/\[\s*"(.*?)"\s*\]/$HoALightSourceRef->{ $light }->[ 1 ] =~ m|$1| ? '[ "' . $HoALightSourceRef->{ $light }->[ 2 ] . '" ]' : '[ "' . $1 . '" ]'/eg;
				}
				$$lineRef = '#' . $$lineRef . $line if ( $$lineRef ne $line );
			}
		}

		last;	# 루프를 빠져나간다.
}
	}

	# 여기에서 속성처리를 한다.
	if ( $enumRIBDirective == $ENUM_RIB_DIRECTIVE__ATTRIBUTE ) {
		my	$line = join( '', @$RIBDirectiveContentRef );
		if ( ${ $iStackRef } > 0 ) {		# 최상위 스택이 아닌 곳에서
			# 이전에 설정된 RS__VIS_REFL, RS__VIS_REFR과 관련된 "subset"을 무효로한다.
			# 이전에 설정된 RS__VISIBILITY와 관련된 부분을 무효로한다.
			if (
				$line =~ /"visibility"\s*"int\s+camera"/s ||
				$line =~ /"grouping" "string membership" \["[+-]?RManRef[lr]Subset"\]/s
			) {
				$RIBDirectiveContentRef->[ 0 ] = '#' . join( '#', @$RIBDirectiveContentRef );
				$#{ $RIBDirectiveContentRef } = 0;
			}
		} else {				# 최상위 스택에서
			# Ray Tracing이 설정되어 있는 지 알아본다.
#	Attribute "visibility" "int camera" [1] "int trace" [1] "int photon" [1] "string transmission" ["opaque"]
			if (
				$hChunkRef->{ 'bRayTracing' } == 0 &&		# 한 번만 한다.
				isRaytracingAttributeLine( $line )
#				$line =~ /^\s*Attribute\s*"visibility".*"int\s+trace"\s*\[\s*1\s*\]/s
#				$line =~ /"trace"/s
			) {
				$hChunkRef->{ 'bRayTracing' } = 1;
				push( @{ $RIBDirectiveContentRef },
					'Attribute "grouping" "string membership" ["+RManReflSubset"]' . "\r\n",
					'Attribute "grouping" "string membership" ["+RManRefrSubset"]' . "\r\n"
				);
			}
		}
	}


	if (
		$hStackRef->{ 'type' } == $STACK_TYPE__TRANSFORM &&
					# 오브젝트의 정보는 적어도 AttributeBEgin...AttributeEnd안에 들어있다.
					# 당연히 ${ $iStackRef } > 0보다 크다.
		$enumRIBDirective == $ENUM_RIB_DIRECTIVE__ATTRIBUTE
	) {
		#	Attribute "identifier" "string name" ["..."]
		if (join('', @$RIBDirectiveContentRef) =~ /"identifier"\s*"string\s+name"\s*\[\s*"(\S+?)"\s*\]/s) {
			# "라이트 쉐이더"
			my	$lightName = $1;
			$hStackRef->{ 'name' } = $lightName;
			# RIB Box에서 사용할 $gObjectName을 설정한다.
			$gObjectName = $lightName;
			# 현재 스택 위치를 기억한다.
			$giObjectNameStackIndex = $$iStackRef;

			# RIB Box가 들어갈 위치이다. (ENUM_RIB_BOX__LIGHT__JUST_AFTER_ATTRIBUTE)
			my	$light = $AoHStackRef -> [ 0 ] -> { 'name' };
			my	$frameName = $light ? 'Shadow: ' . $light : 'Image';
			print STDERR 'EDITRIB: Info: RIBBOX.FRAME(' . $frameName . ').LIGHT(' . $lightName . '): Just after LIGHT attribute' . $/ if $bVerboseRIBBox;
			push( @{ $RIBDirectiveContentRef }, '## EDITRIB ## RIB Box: Frame(' . $frameName . ').Light(' . $lightName . '): Just after LIGHT attribute' . "\r\n" ) if $bVerboseRIBBox;

			# 들어갈 "RIB Box"가 있는 지 알아본다.
			my	$RIBBoxExpr = getRIBBoxExpr(
				$hChunkRef->{ 'rbref' },			
				$ENUM_RIB_BOX__LIGHT__JUST_AFTER_ATTRIBUTE,
				$gElementName
			);
			if ( $RIBBoxExpr) {
				my	$RIBBoxContent = '## EDITRIB ## <RIB Box> ####' . "\r\n"
					. evalRIBBoxExpr( $RIBBoxExpr ) . "\r\n"
					. '## EDITRIB ## </RIB Box> ####' . "\r\n";
				push( @{ $RIBDirectiveContentRef }, $RIBBoxContent );
			}
		}

	}
	# Attribute에 해당하는 "RIB Directive"를 만나,
	# 현재 스택이 어떤 오브젝트를 기술하는 것인 지를 안다면
	# fate값이 바뀔 수도 있다.
	# 자세한 것은 @AoHStack에 대한 설명에서 "fate"에 대한 1.부분을 보라.
	# 특히 2)의 설명 부분을 보라.
	if (
		$hStackRef->{ 'type' } == $STACK_TYPE__ATTRIBUTE &&
					# 오브젝트의 정보는 적어도 AttributeBEgin...AttributeEnd안에 들어있다.
					# 당연히 ${ $iStackRef } > 0보다 크다.
		$enumRIBDirective == $ENUM_RIB_DIRECTIVE__ATTRIBUTE
	) {
		#	Attribute "identifier" "name" ["..."]
		if (join('', @$RIBDirectiveContentRef) =~ /"identifier"\s*"name"\s*\[\s*"(\S+?)"\s*\]/s) {
			# "오브젝트"
			my	$name = $1;
			$hStackRef->{ 'name' } = $name;
			# RIB Box에서 사용할 $gObjectName을 설정한다.
			$gObjectName = $name;
			# 현재 스택 위치를 기억한다.
			$giObjectNameStackIndex = $$iStackRef;
			# 지워져야할 오브젝트라면 fate값이 바뀐다.

			#### 오브젝트의 운명을 결정한다.
			#================================================#
			my	$bDelete;	# 삭제여부를 결정한다.
						# 1: 삭제된다, 0: 살아남는다

			if ( $typeChunk == $CHUNK_TYPE__SHD ) {
				$bDelete = 1;	# 원래는 삭제하는데,
						# 살아남을 목록에 있다면 살아남는다.
						# "쉐도우 이미지"에서는 "give set"에 있을 때,
						# "렌더링 이미지"에서는 "take set"에 있을 때
				my	$light = $AoHStackRef->[0]->{ 'name' };
				my	$AoHObjectSHDRef = $light ? $hChunkRef->{ 'object array' } : $hChunkRef->{ 'jcref' };
				foreach my $hObjectSHDRef ( @{ $AoHObjectSHDRef } ) {
					if (isMatched( $name, $hObjectSHDRef->{ $light ? 'give set' : 'take set' } ) == 1) {
						$bDelete = 0;
						last;
					}
				}
#				print STDERR "SHD: $name, $light, $bDelete\n";
			} elsif ( $typeChunk == $CHUNK_TYPE__RS ) {
				# [단계 1] Light Linking에 의해서 오브젝트가 놓인 "상황"을 결정한다.
				#	1) "쉐도우 이미지"단계라면 오브젝트의 삭제여부가 판단된다.
				#	2) "렌더링 이미지"단계라면 오브젝트가 빛을 받는 라이트가 있는 지 알 수 있다.
				#	어떤 라이트로부터도 빛을 받지 않는다 하더라도,
				#	Light Linking에서 오브젝트를 삭제할 수 없다.
				#	[단계 2]의 "Render Stats"에 의해서 삭제여부가 결정된다.
				#	(자세한 것은 [단계 2]의 내용을 참고한다.)
				#	2007/1/27 (KST)
				#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)
				#
				my	$illuminance;	# "렌더링 이미지"에서 빛을 받는 라이트가 있는 지 판단한다.
							# 적어도 하나의 빛을 주는 라이트가 있다면 1,
							# 어떠한 라이트도 없다면 0
				# Light Linking은 특정 오브젝트에 빛을 비출 것이냐 아니냐를 결정하는 것이다.
				# "쉐도우 이미지"를 만드는 단계라면,
				# 빛을 받는 오브젝트만을 고려해서 그림자를 만들어야 한다.
				# "렌더링 이미지"를 만드는 단계라면
				# 오브젝트 자체는 건들지 않고,
				# 쉐이더 단계에서 빛을 받을 것인지를 판단한다.
				my	$light = $AoHStackRef->[0]->{ 'name' };
				if ($light) {	# "쉐도우 이미지"
					my	$AoHObjectLLRef = $hChunkRef->{ 'object array' };
					$bDelete = 1;	# 이 오브젝트가 빛을 받는다고 명시된 경우에만
							# 오브젝트가 살아남는다.
							# 아무런 말이 없다면 이 오브젝트는 삭제된다.
					foreach my $hObjectLLRef ( @{ $AoHObjectLLRef } ) {
						if (isMatched($name, $hObjectLLRef->{ 'object set' }) == 1) {
							$bDelete = 0;
							last;
						}
					}
				} else {	# "렌더링 이미지"
					# 오브젝트 자체는 무조건 살아남는다.
					# 오브젝트에 라이트를 받지 않는다면,
					# 쉐이더에서 'illuminate "..." 0'으로 처리한다.
					# 어떠한 라이트도 받지 않는다면 오브젝트 자체를 없애야 한다.
					#	2007/1/7 (EST)
					#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)
					# 이 오브젝트에 비추지 않는 라이트는
					# 'illuminate "..." 0'으로 처리한다.
					# $bDelete = 1으로 설정한 후,
					# 'illuminate "..." 0'이 아닌 경우를 만나면
					# $bDelete = 0으로 설정한다.
					# 적어도 하나의 라이트는 이 오브젝트를 비추므로,
					# 오브젝트는 살아남아야 한다.
#					$bDelete = 1;
					# 여기서 삭제여부를 결정하지 않고, 빛을 받고 있는 지만 판단한다.
					$illuminance = 0;
					foreach my $light (keys( %{ $HoALightSourceRef } )) {
						# Light Linking 정보를 읽으면서
						my	$AoHLightLinkingRef = $hChunkRef->{ 'llref' };
						# 현재 오브젝트와 관련된 정보가 있는 지 확인한다.
						my	$illuminate = 0;	# 'illuminate'가 1로 명시된 라이트만이
										# 이 오브젝트에 비춘다.
						foreach my $hLightLinkingRef ( @{ $AoHLightLinkingRef } ) {
							# 주어진 라이트와 관련이 없는 정보는 무시한다.
							next unless (isMatched( $light, $hLightLinkingRef->{ 'light set' } ) == 1);
							# 이 오브젝트와 관련되지 않은 정보도 무시한다.
							next unless (isMatched( $hStackRef->{ 'name' }, $hLightLinkingRef->{ 'object set' } ) == 1);
							# {주어진 라이트, 현재 오브젝트}와 관련된 정보가 있다.
							$illuminate = 1;
							last;	# 일치하는 첫 번째 정보가 우선권이 있다.
						}
						# 이 오브젝트와 관련된 어떠한 라이트 정보(illuminate)가 없을 수도 있다.
						# 그 때는 기본값인 0을 사용한다.
						if ( $illuminate ) {	# 이 오브젝트를 비추는 라이트가 있다.
							# 살아남아야 한다.
							#$bDelete = 0;
							# 여기서 삭제여부를 결정하지 않고, 빛을 받고 있는 지만 판단한다.
							$illuminance = 1;
							last;
						}
					}
				}

				# [단계 2] "Render Stats"에 의해서 오브젝트의 "상황"을 결정한다.
				unless ( $bDelete ) {	# 아직까지는 살아남는다.
							# "쉐도우 이미지"단계에서 살아남거나,
							# "렌더링 이미지"단계이다.
	#================================================#
	# 이 오브젝트에서 처리되어야 할 "Render Stats"정보가 있는 지 알아본다.
	my	$RenderStats;
	my	$AoHRenderStatRef = $hChunkRef->{ 'jcref' }->{ 'primary' };
	foreach my $hRenderStatRef ( @{ $AoHRenderStatRef } ) {
		if (isMatched($name, $hRenderStatRef->{ 'object set' }) == 1) {
			$RenderStats = $hRenderStatRef->{ 'Render Stats' };
			last;
		}
	}
	if ( $RenderStats ) {
		if ( $RenderStats & $RS__DELETE ) {
			$bDelete = 1;
		} else {
			if ( $light ) {	# "쉐도우 이미지"단계에서 살아남은 경우
				if ( $RenderStats & $RS__MATTE ) {
					$bDelete = 1;
				}
				unless ( $RenderStats & $RS__CAST_SHADOW ) {
					# "Casts Shadows"옵션이 꺼져있다면
					$bDelete = 1;
				}
			} else {	# "렌더링 이미지"단계
				if ( $hChunkRef->{ 'bRayTracing' } ) {
					unless ( $RenderStats & ( $RS__VISIBILITY | $RS__RT ) ) {
						$bDelete = 1;
					} else {
						$bDelete = 0;
						unless ( $RenderStats & $RS__VISIBILITY ) {
							push( @{ $RIBDirectiveContentRef }, 'Attribute "visibility" "int camera" [ 0 ]' . "\r\n" );
						}
						unless ( $RenderStats & $RS__VIS_REFL ) {
							push( @{ $RIBDirectiveContentRef }, 'Attribute "grouping" "string membership" ["-RManReflSubset"]' . "\r\n" );
						}
						unless ( $RenderStats & $RS__VIS_REFR ) {
							push( @{ $RIBDirectiveContentRef }, 'Attribute "grouping" "string membership" ["-RManRefrSubset"]' . "\r\n" );
						}
					}
				} else {
					if ( $RenderStats & $RS__RT ) {
						print STDERR 'EDITRIB: Info: ' . '{' . $name . '}' . ': Ray tracing is OFF but "Visible in Reflections" or "Visible in Refractions" is ON: Ignored' . $/;
					}
					unless ( $illuminance ) {	# 이 오브젝트를 비추는 빛이 없다면
						# "Primary Visibility"옵션을 끈 것과 같다.
						$RenderStats &= ~$RS__VISIBILITY;
					}
					$bDelete = $RenderStats & $RS__VISIBILITY ? 0 : 1;
				}
				$bDelete = 0 if ( $RenderStats & $RS__MATTE );
			}
		}
	}
	#================================================#
				}
			} else {
				print STDERR 'EDITRIB: Warning: Unknow CHUNK type(' . $typeChunk . '): Ignored' . $/;
			}
			#================================================#

			if ( $bDelete ) {	# 지워져야할 오브젝트라면
				# 지금까지 모은 내용을 모두 버리고
				$hStackRef->{ 'content' } = [];
				$hStackRef->{ 'bake subframe content' } = [] if $BAKEFH;
				# fate값을 CONTENT_DELETE로 바꾼다.
				$hStackRef->{ 'fate' } = $CONTENT_DELETE;
				$fate = $CONTENT_DELETE;
			} else {
				# 살아남는 오브젝트이다.

########
			# RIB Box가 들어갈 위치이다. (ENUM_RIB_BOX__OBJECT__JUST_AFTER_ATTRIBUTE)
			my	$light = $AoHStackRef -> [ 0 ] -> { 'name' };
			my	$frameName = $light ? 'Shadow: ' . $light : 'Image';
			print STDERR 'EDITRIB: Info: RIBBOX.FRAME(' . $frameName . ').OBJECT(' . $name . '): Just after OBJECT attribute' . $/ if $bVerboseRIBBox;
			push(@{ $RIBDirectiveContentRef }, '## EDITRIB ## RIB Box: Frame(' . $frameName . ').Object(' . $name . '): Just after OBJECT attribute' . "\r\n" ) if $bVerboseRIBBox;

			# 들어갈 "RIB Box"가 있는 지 알아본다.
			my	$RIBBoxExpr = getRIBBoxExpr(
				$hChunkRef->{ 'rbref' },			
				$ENUM_RIB_BOX__OBJECT__JUST_AFTER_ATTRIBUTE,
				$gElementName
			);
			if ( $RIBBoxExpr) {
				my	$RIBBoxContent = '## EDITRIB ## <RIB Box> ####' . "\r\n"
					. evalRIBBoxExpr( $RIBBoxExpr ) . "\r\n"
					. '## EDITRIB ## </RIB Box> ####' . "\r\n";
				push(@{ $RIBDirectiveContentRef }, $RIBBoxContent );
			}
########
			}
		}
	}

	if ($fate == $CONTENT_PRINT) {		# 읽은 내용을 출력한다.
		if ( $enumRIBDirective == $ENUM_RIB_DIRECTIVE__GEOMETRIC_PRIMITIVE ) {
			printArrayRef( $FH, $RIBDirectiveContentRef ) if $gElementName ne 'Image' || !$bBake;
			printArrayRef( $SUBFH, $RIBDirectiveContentRef ) if $SUBFH;
			printArrayRef( $BAKEFH, $RIBDirectiveContentRef ) if $BAKEFH;
		} else {
			print $FH @$RIBDirectiveContentRef if $gElementName ne 'Image' || !$bBake;
			print $SUBFH @$RIBDirectiveContentRef if $SUBFH;
			print $BAKEFH @$RIBDirectiveContentRef if $BAKEFH;
		}
	} elsif ( $fate == $CONTENT_SAVE ) {				# $fate = $CONTENT_SAVE
		if ( $enumRIBDirective == $ENUM_RIB_DIRECTIVE__GEOMETRIC_PRIMITIVE ) {
			appendArrayRef( $hStackRef->{ 'content' }, $RIBDirectiveContentRef);
			appendArrayRef( $hStackRef->{ 'bake subframe content' }, $RIBDirectiveContentRef) if $BAKEFH;
		} else {
			push(@{ $hStackRef->{ 'content' }}, @$RIBDirectiveContentRef);
			push(@{ $hStackRef->{ 'bake subframe content' }}, @$RIBDirectiveContentRef) if $BAKEFH;
		}
	}
}

	if ( $enumRIBDirective == $ENUM_RIB_DIRECTIVE__FRAME_END ) {
		$gElementName = undef;
	}

	if ( $bVerbosePath && $enumRIBDirective == $ENUM_RIB_DIRECTIVE__DISPLAY ) {
		print STDERR '|' . ' ' x 100 . '|' . $/;
		print STDERR '+' . '=' x 100 . '+' . $/;
	}

}
# 마무리:
#	. 각 [작업 chunk]에 쓰인 "file handle"을 닫는다.
foreach my $keyChunk (keys( %HoHChunk )) {
	my	$fileHandle = $HoHChunk{ $keyChunk } -> { 'file handle' };
#	next unless $fileHandle;	# open함수에서 실패한 경우에는 'file handle'값이 설정되지 않았다.
#	unless (close( $fileHandle )) {
#		print STDERR 'EDITRIB: Error: close for "chunk"(' . $keyChunk . '): Failed: Ignored' . $/;
#		next;
#	}
	close( $fileHandle ) || die 'EDITRIB: Error: close for "chunk"(' . $keyChunk . '): Failed';
}
