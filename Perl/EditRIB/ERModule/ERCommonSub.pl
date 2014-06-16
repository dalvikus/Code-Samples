use ERModule;

use strict;
no strict "refs";


# 모든 job chunk에서 변수와 값을 설정한다.
# $bGlobalVar값이 설정되면 전역변수도 설정한다.
sub setVar
{
	my	( $varName, $varValue, $varScope ) = @_;
	$varScope = 0 unless $varScope;		# 정의되지 않았다면 0으로 생각한다.

	if ( $varScope & ERModule::ER__VAR_SCOPE__GLOBAL ) {
		$ERModule::ghVar{ 'global variable' } -> { $varName } = $varValue;
	}

	if ( $varScope & ERModule::ER__VAR_SCOPE__JOB_CHUNK ) {
		for my $keyChunk ( keys( %ERModule::HoHChunk ) ) {
			$ERModule::HoHChunk{ $keyChunk } -> { $varName } = $varValue;
		}
	}

	return $varValue;
}


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

	splice( @{ $aRef }, $index, 1, [ ERModule::ER__CONTENT_STATE__DELETE, $aRef->[ $index ] ] );
}
# 2) i-번째 $newElement를 추가하는 경우
#	splice( @{ $RIBDirectiveContentRef }, $i, 0, [ $CONTENT_STATE__ADD, \$newElement ] );
sub addArrayRef
{
	my	( $aRef, $index, $addElementRef ) = @_;

	splice( @{ $aRef }, $index, 0, [ ERModule::ER__CONTENT_STATE__ADD, $$addElementRef ] );
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
			if ( $$lineRef->[ 0 ] == ERModule::ER__CONTENT_STATE__DELETE ) {	# 삭제하는 경우
				# '#'를 넣어 주석처리만 한다.
				print $FH '#' . $$lineRef->[ 1 ];
			} else {								# 추가하는 경우
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
			if ( $$lineRef->[ 0 ] == ERModule::ER__CONTENT_STATE__DELETE ) {	# 삭제하는 경우
				# '#'를 넣어 주석처리를 한 후 추가한다.
				push( @{ $aDestRef }, '#' . $$lineRef->[ 1 ] );
			} else {								# 추가하는 경우
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
		if ( $$lineRef->[ 0 ] == ERModule::ER__CONTENT_STATE__DELETE ) {	# 삭제된 경우
			# 원래대로 복구한다.
			$$lineRef = $$lineRef->[ 1 ];
		} else {								# 추가된 경우
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
#my	$gFrameNumber;		# 파일이름을 통해서 얻는다.
#my	$gElementName;		# 매 subframe마다, "Display"를 통해서 얻는다.
				# "렌더링 이미지"의 경우에는 ""이다.
				# "FrameEnd"에서 undef으로 초기화된다.
#my	$gObjectName;		# 다음과 같은 "Attribute"에서 얻는다.
				#	Attribute "identifier" "string name" ["..."]	# LIGHT
				#	Attribute "identifier" "name" ["..."]		# OBJECT (geometric primitive)
				# 위의 "Attribute"를 포함하는 "TransformEnd"(LIGHT의 경우)나
				# "AttributeEnd"에서 undef으로 초기화된다.
# $gObjectName을 얻은, "TransformBegin"이나 "AttributeBegin"의 스택 위치를 기억해서
# 대응하는 "TransformEnd"나 "AttributeEnd"를 만났을 때,
# 같은 스택 위치라면 undef으로 초기화한다.
#my	$giObjectNameStackIndex;
sub	evalRIBBoxExpr
{
	my	$myExpr = shift;

	my	$hGlobalVariableRef = $ERModule::ghVar{ 'global variable' };
	my	$ELEMENT = $hGlobalVariableRef -> { 'aElementName' };
	$ELEMENT = 'undef' unless defined( $ELEMENT );
	my	$FRAME = $hGlobalVariableRef -> { 'gFrameNumber' };
	$FRAME = 'undef' unless defined( $FRAME );
	my	$OBJECT = $hGlobalVariableRef -> { 'jcObjectName' };
	$OBJECT = 'undef' unless defined( $OBJECT );

	return defined( $myExpr ) ? eval( 'qq|' . $myExpr . '|' ) : undef;
}



# 2. 쉐이더가 속하는 즉 오브젝트의 이름을 말해주는
#	Attribute "identifier" "name" [ "..." ]
# 을 포함하고 있는 스택을 찾는다.
sub getStackName
{
	my	$AoHStackRef = shift;

	for ( my $iStack = $#{ $AoHStackRef }; $iStack > 0; --$iStack ) {	# "> 0": WorldBegin..WorldEnd안에서만 찾느다.
		my	$myhStackRef = $AoHStackRef->[ $iStack ];
		my	$myStackType = $myhStackRef->{ 'type' };
		my	$myStackName = $myhStackRef->{ 'name' };
		return ( $myStackName, $iStack ) if ( $myStackName );
	}
	return (undef, undef);
}



sub getRIBBoxExpr
{
	my	( $hAoARIBBoxRef, $enumRIBBox, $element, $object ) = @_;

	return undef unless $hAoARIBBoxRef;
	return undef if ( ref( $hAoARIBBoxRef ) ne 'HASH' );
	my	$val = $hAoARIBBoxRef->{ $enumRIBBox };
	return undef unless $val;
	my	$refType = ref( $val );

	if (
		$enumRIBBox == ERModule::ER__ENUM_RIB_BOX__DECLARE__JUST_BEFORE_FIRST_FRAME_BEGIN
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
		$enumRIBBox != ERModule::ER__ENUM_RIB_BOX__FRAME__JUST_BEFORE_DISPLAY &&
		$enumRIBBox != ERModule::ER__ENUM_RIB_BOX__FRAME__JUST_BEFORE_WORLD_BEGIN &&
		$enumRIBBox != ERModule::ER__ENUM_RIB_BOX__WORLD__JUST_BEFORE_FIRST_LIGHTSOURCE_BLOCK &&
		$enumRIBBox != ERModule::ER__ENUM_RIB_BOX__WORLD__JUST_BEFORE_FIRST_GEOMETRIC_PRIMITIVE_BLOCK &&
		$enumRIBBox != ERModule::ER__ENUM_RIB_BOX__WORLD__JUST_AFTER_LAST_OBJECT_BLOCK &&
		# CONTEXT에서 (OBJECT, ELEMENT)가 정의되어야 한다.
		#	하나만 정의되었을 때, 정의된 OBJECT가 모든 ELEMENT에서 적용된다고 가정한다.
		#	정의되지 않았다면, 모든 ELEMENT에서 모든 OBJECT에서 적용된다고 가정한다.
		$enumRIBBox != ERModule::ER__ENUM_RIB_BOX__LIGHT__JUST_AFTER_ATTRIBUTE &&
		$enumRIBBox != ERModule::ER__ENUM_RIB_BOX__OBJECT__JUST_AFTER_ATTRIBUTE &&
		$enumRIBBox != ERModule::ER__ENUM_RIB_BOX__OBJECT__JUST_BEFORE_LIGHT_SHADER &&
		$enumRIBBox != ERModule::ER__ENUM_RIB_BOX__OBJECT__JUST_BEFORE_SURFACE_SHADER &&
		$enumRIBBox != ERModule::ER__ENUM_RIB_BOX__OBJECT__JUST_BEFORE_GEOMETRIC_PRIMITIVE
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
			$enumRIBBox == ERModule::ER__ENUM_RIB_BOX__FRAME__JUST_BEFORE_DISPLAY ||
			$enumRIBBox == ERModule::ER__ENUM_RIB_BOX__FRAME__JUST_BEFORE_WORLD_BEGIN ||
			$enumRIBBox == ERModule::ER__ENUM_RIB_BOX__WORLD__JUST_BEFORE_FIRST_LIGHTSOURCE_BLOCK ||
			$enumRIBBox == ERModule::ER__ENUM_RIB_BOX__WORLD__JUST_BEFORE_FIRST_GEOMETRIC_PRIMITIVE_BLOCK ||
			$enumRIBBox == ERModule::ER__ENUM_RIB_BOX__WORLD__JUST_AFTER_LAST_OBJECT_BLOCK
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
	foreach my $Dir ( @eDir ) {
		$DirUNIXPath .= $Dir . '/';
#		print STDERR '{' . $DirUNIXPath . '}' . ': ' . ( -d $DirUNIXPath ? 'Yes' : 'No' ) . $/;
		next if -d "$DirUNIXPath";
		mkdir ( $DirUNIXPath ) || die '{mkdir ' . $DirUNIXPath . '}: Failure: ' . $!;
	}
	return $DirUNIXPath;
}



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



1;
