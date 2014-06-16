use ERModule;

use strict;
no strict "refs";


sub ERBasePreDisplay
{
	my	$hGlobalVariableRef = $ERModule::ghVar{ 'global variable' };
	my	$enumRIBDirective = $hGlobalVariableRef -> { 'gEnumRIBDirective' };
	my	$RIBDirective = $hGlobalVariableRef -> { 'gRIBDirective' };
	my	$RIBDirectiveContentRef = $hGlobalVariableRef -> { 'gRIBDirectiveContentRef' };
	my	$hCommandLineOptionRef = $ERModule::ghVar{ 'command line option' };
	my	$bVerbose = $hCommandLineOptionRef -> { 'bVerbose' };
	my	$bVerbosePath = $hCommandLineOptionRef -> { 'bVerbosePath' };


	my	%hDisplay;
	setVar( 'ghDisplayRef', \%hDisplay, ERModule::ER__VAR_SCOPE__GLOBAL );

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

	# "Deep shadow"의 경우에, 두 번째 "Display"에 필요한 정보가 있다.
	# 대부분 첫 번째 "Display"는 다음과 같은 형태가 된다.
	#	Display "null" "null" "z"
	# 하지만 렌더맨 설명서에 따르면 다른 형태가 올 수도 있다.
	# 이런 경우에만 에러로 처리한다.
	if ( $display =~ /^\s*Display\s*"null"\s*"null"\s*"z"/ ) {
		print STDERR 'EDITRIB: Info: Deep shadow' . $/ if $bVerbose;
		$hDisplay{ 'name' } = undef;	# 아직까지 제대로된 "Display"정보를 얻지 못했다.

		return;
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
	# RIB Box에서 사용할 $aElementName을 설정한다.
	setVar( 'aElementName', $light, ERModule::ER__VAR_SCOPE__ALL );
	$hDisplay{ 'path' } = $path;
	$hDisplay{ 'filename' } = $filename;
	$hDisplay{ 'dspyName' } = $dspyName;
	$hDisplay{ 'dspyName0' } = $dspyName0;
	$hDisplay{ 'frame' } = $frame;
}


sub ERBaseFrameBegin
{
	my	$hGlobalVariableRef = $ERModule::ghVar{ 'global variable' };
	my	$RIBDirectiveContentRef = $hGlobalVariableRef -> { 'gRIBDirectiveContentRef' };
	my	$keyChunk = $hGlobalVariableRef -> { 'gKeyChunk' };
	my	$hChunkRef = $ERModule::HoHChunk{ $keyChunk };
	my	$AoHStackRef = $hChunkRef -> { 'stack array' };
	my	$iStackRef = \$hChunkRef -> { 'stack index' };
	my	$BAKEFH = $hChunkRef -> { 'bake subframe file handle' };


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
		'type'		=> ERModule::ER__STACK_TYPE__FRAME,
		'name'		=> undef,
		'content'	=> [],
		'fate'		=> ERModule::ER__CONTENT_FATE__SAVE
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
}


sub ERBaseFrameEnd
{
	my	$hGlobalVariableRef = $ERModule::ghVar{ 'global variable' };
	my	$gFrameNumber = $hGlobalVariableRef -> { 'gFrameNumber' };
	my	$RIBDirectiveContentRef = $hGlobalVariableRef -> { 'gRIBDirectiveContentRef' };
	my	$keyChunk = $hGlobalVariableRef -> { 'gKeyChunk' };
	my	$hChunkRef = $ERModule::HoHChunk{ $keyChunk };
	my	$AoHStackRef = $hChunkRef -> { 'stack array' };
	my	$iStackRef = \$hChunkRef -> { 'stack index' };
	my	$FH = $hChunkRef -> { 'file handle' };
	my	$SUBFH = $hChunkRef -> { 'subframe file handle' };
	my	$BAKEFH = $hChunkRef -> { 'bake subframe file handle' };
	my	$aElementName = $hChunkRef -> { 'aElementName' };
	my	$bBake = $hChunkRef->{ 'bBake' };


	# 최상위 "스택"의 끝이다.
	my	$hStackRef = $AoHStackRef->[ 0 ];
	# 최상위 "스택"의 "운명"에 따라 FrameEnd에 해당하는 내용을 처리한다.
	# 최상위 "스택" 내용의 "운명"은 출력(CONTENT_PRINT)아니면 삭제(CONTENT_DELETE)이다.
	if ( $hStackRef->{ 'fate' } == ERModule::ER__CONTENT_FATE__PRINT ) {
		if ( $aElementName ) {
			# aElementName이 설정되었을 때 즉 의미있는 subframe인 경우에만...
			print $FH @$RIBDirectiveContentRef if $aElementName ne 'Image' || !$bBake;
			print $SUBFH @$RIBDirectiveContentRef if $SUBFH;
			print $BAKEFH @$RIBDirectiveContentRef if $BAKEFH;
		}
	} else {	# $CONTENT_DELETE
		# 삭제하는 경우에는 아무것도 하지 않는다.
	}

	# FrameBegin...FrameEnd안에 있는 "Display"를 통해 얻은
	# $aElementName값은 이후로 의미가 없다.
	# 모든 "chunk"를 처리한 후에 이 값을 undef으로 설정해야 한다.
	#	2007/5/24 (EST)
	#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)

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
		my	$NBakedObject = @{ $hChunkRef->{ 'SubsurfaceScattering_BakedObjects' } } if $hChunkRef->{ 'SubsurfaceScattering_BakedObjects' };

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
}


# ENUM_RIB_DIRECTIVE__ATTRIBUTE_BEGIN
# ENUM_RIB_DIRECTIVE__TRANSFORM_BEGIN
sub ERBaseAttributeBeginTransformBegin
{
	my	$hGlobalVariableRef = $ERModule::ghVar{ 'global variable' };
	my	$RIBDirectiveContentRef = $hGlobalVariableRef -> { 'gRIBDirectiveContentRef' };
	my	$enumRIBDirective = $hGlobalVariableRef -> { 'gEnumRIBDirective' };

	my	$keyChunk = $hGlobalVariableRef -> { 'gKeyChunk' };
	my	$hChunkRef = $ERModule::HoHChunk{ $keyChunk };
	my	$BAKEFH = $hChunkRef -> { 'bake subframe file handle' };
	my	$AoHStackRef = $hChunkRef -> { 'stack array' };
	my	$iStackRef = \$hChunkRef -> { 'stack index' };


	my	$fate = ERModule::ER__CONTENT_FATE__SAVE;	# 기본값은 CONTENT_SAVE이다.
	if ( $AoHStackRef->[ ${ $iStackRef } ]->{ 'fate' } == ERModule::ER__CONTENT_FATE__DELETE ) {	# "부모 스택"의 fate값이 CONTENT_DELETE라면
		# "자식 스택"의 fate값도 CONTENT_DELETE이다.
		$fate = ERModule::ER__CONTENT_FATE__DELETE;
	}

	# "자식 스택"에 대한 정보를 담을 구조체(해쉬)를 초기화한다.
	my	$hStackRef = {
		'type'		=> ( $enumRIBDirective == (ERModule::ER__ENUM_RIB_DIRECTIVE__ATTRIBUTE_BEGIN) ? ERModule::ER__STACK_TYPE__ATTRIBUTE : ERModule::ER__STACK_TYPE__TRANSFORM ),
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
	if ( $fate == ERModule::ER__CONTENT_FATE__SAVE ) {
		push(@{ $hStackRef ->{ 'content'} }, @$RIBDirectiveContentRef);
		push(@{ $hStackRef ->{ 'bake subframe content'} }, @$RIBDirectiveContentRef) if $BAKEFH;
	} else {	# $CONTENT_DELETE
		# 삭제하는 경우에는 아무것도 하지 않는다.
	}

	# "자식 스택"으로 등록한다.
	$AoHStackRef->[ ++${ $iStackRef } ] = $hStackRef;
}


sub ERBaseAttributeEndTransformEnd
{
	my	$hGlobalVariableRef = $ERModule::ghVar{ 'global variable' };
	my	$RIBDirectiveContentRef = $hGlobalVariableRef -> { 'gRIBDirectiveContentRef' };
	my	$enumRIBDirective = $hGlobalVariableRef -> { 'gEnumRIBDirective' };

	my	$keyChunk = $hGlobalVariableRef -> { 'gKeyChunk' };
	my	$hChunkRef = $ERModule::HoHChunk{ $keyChunk };
	my	$FH = $hChunkRef -> { 'file handle' };
	my	$SUBFH = $hChunkRef -> { 'subframe file handle' };
	my	$BAKEFH = $hChunkRef -> { 'bake subframe file handle' };
	my	$aElementName = $hChunkRef -> { 'aElementName' };
	my	$bBake = $hChunkRef->{ 'bBake' };
	my	$AoHStackRef = $hChunkRef -> { 'stack array' };
	my	$iStackRef = \$hChunkRef -> { 'stack index' };


	my	$hStackRef = $AoHStackRef->[ ${ $iStackRef } ];
	my	$fate = $hStackRef->{ 'fate' };
	# 먼저, "자식 스택"의 fate값에 따라 AttributeEnd이나 TransformEnd에 해당하는 내용을 처리한다.
	if ( $fate == ERModule::ER__CONTENT_FATE__SAVE ) {
		push(@{ $hStackRef->{ 'content'} }, @$RIBDirectiveContentRef);
		push(@{ $hStackRef->{ 'bake subframe content'} }, @$RIBDirectiveContentRef) if $BAKEFH;
	} elsif ($fate == ERModule::ER__CONTENT_FATE__PRINT) {
		print $FH @$RIBDirectiveContentRef if $aElementName ne 'Image' || !$bBake;
		print $SUBFH @$RIBDirectiveContentRef if $SUBFH;
		print $BAKEFH @$RIBDirectiveContentRef if $BAKEFH;
	} else {	# $CONTENT_DELETE
		# 삭제하는 경우에는 아무것도 하지 않는다.
	}
	my	$hParentStackRef = $AoHStackRef -> [ ${ $iStackRef } - 1 ];
	# "보모 스택"의 fate값에 따라 "자식 스택"의 내용을 처리한다.
	# 자세한 것은 @AoHStack에 대한 설명에서 "fate"에 대한 2.부분을 보라.
	if ( $hParentStackRef -> { 'fate' } == ERModule::ER__CONTENT_FATE__SAVE ) {	# fate값이 CONTENT_SAVE이라면
		# "부모 스택"의 내용에 "자식 스택"의 내용을 추가한다. (append)
		push(@{ $hParentStackRef->{ 'content' } }, @{ $hStackRef->{ 'content' } });
		push(@{ $hParentStackRef->{ 'bake subframe content' } }, @{ $hStackRef->{ 'bake subframe content' } }) if $BAKEFH;
	} else {
		# 방금 처리가 끝난 스택이 최상위 스택이거나 ("부모 스택"이 없거나)
		# "부모 스택"의 fate값이 CONTENT_PRINT인 경우이다.
		print $FH @{ $hStackRef->{ 'content' } } if $aElementName ne 'Image' || !$bBake;
		print $SUBFH @{ $hStackRef->{ 'content' } } if $SUBFH;
		print $BAKEFH @{ $hStackRef->{ 'bake subframe content' } } if $BAKEFH;
	}
	# "자식 스택"을 해제한다.
	--$#{ $AoHStackRef };

	# 이 "TransformEnd"나 "AttributeEnd"에서 $jcObjectName을 설정되었다면
	# 여기서 undef으로 초기화한다.
	my	$jcObjectNameStackIndex = $hChunkRef -> { 'jcObjectNameStackIndex' };
	if ( defined( $jcObjectNameStackIndex ) && ${ $iStackRef } == $jcObjectNameStackIndex ) {
		$hChunkRef -> { 'jcObjectName' } = undef;
		$hChunkRef -> { 'jcObjectNameStackIndex' } = undef;
	}
	# 여기까지가 $hStackRef를 통해 참고하는, "자식 스택"에 대한 모든 정보이다.
	--${ $iStackRef };	# "부모 스택"을 접근한다.
}


sub ERBaseDisplay
{
	my	$hCommandLineOptionRef = $ERModule::ghVar{ 'command line option' };
	my	$bVerbose = $hCommandLineOptionRef -> { 'bVerbose' };
	my	$bVerbosePath = $hCommandLineOptionRef -> { 'bVerbosePath' };
	my	$bVerboseRIBBox = $hCommandLineOptionRef -> { 'bVerboseRIBBox' };
	my	$bAscii = $hCommandLineOptionRef -> { 'bAscii' };
	my	$hGlobalVariableRef = $ERModule::ghVar{ 'global variable' };
	my	$RIBDirectiveContentRef = $hGlobalVariableRef -> { 'gRIBDirectiveContentRef' };

	my	$hDisplayRef = $hGlobalVariableRef -> { 'ghDisplayRef' };
	my	$strDeclare = $hGlobalVariableRef -> { 'gStrDeclare' };
	my	$keyChunk = $hGlobalVariableRef -> { 'gKeyChunk' };
	my	$hChunkRef = $ERModule::HoHChunk{ $keyChunk };
	my	$typeChunk = $hChunkRef -> { 'type' };
	my	$FH = $hChunkRef -> { 'file handle' };
	my	$SUBFH = $hChunkRef -> { 'subframe file handle' };
	my	$BAKEFH = $hChunkRef -> { 'bake subframe file handle' };
	my	$HoALightSourceRef = $hChunkRef -> { 'light source' };
	my	$aElementName = $hChunkRef -> { 'aElementName' };
	my	$bBake = $hChunkRef->{ 'bBake' };
	my	$hStackRef = $hChunkRef -> { 'stack array' } ->[ 0 ];


	unless ( defined( $hDisplayRef -> { 'name' } ) ) {
		$hStackRef->{ 'name' } = undef;	# 아직까지 제대로된 "Display"정보를 얻지 못했다.
		# 나중에 출력할 때 이 정보 즉
		#	Display "null" "null" "z"
		# 을 쓰도록 한다.

		return;
	}

	# $type, $mode를 통해서 "운명"을 결정한다.
	my	$newFate = ERModule::ER__CONTENT_FATE__PRINT;	# "쉐도우 이미지"를 처리 중이면서,
						# 라이트가
						# [작업 3]에서 사용되지 않는 라이트라면
						# CONTENT_DELETE로 바뀐다.

	my	$plusSign = $hDisplayRef -> { '+' };
	# 여러 개의 "Display" Directive가 있을 수 있다.
	if ( $plusSign ) {
		my	$displayName = $hStackRef->{ 'name' };
		if ( defined( $displayName) && !$displayName ) {
			# "렌더링 이미지"의 경우에...
			# $hStackRef->{ 'fate' }는 항상 $CONTENT_PRINT이다.
			print $FH '#' . join( '#', @$RIBDirectiveContentRef ) if $aElementName ne 'Image' || !$bBake;
			print $SUBFH '#' . join( '#', @$RIBDirectiveContentRef ) if $SUBFH;
			print $BAKEFH '#' . join( '#', @$RIBDirectiveContentRef ) if $BAKEFH;

			return;
		}
	}

	my	$name = $hDisplayRef -> { 'name' };
	my	$type = $hDisplayRef -> { 'type' };
	my	$mode = $hDisplayRef -> { 'mode' };
	my	$dummy = $hDisplayRef -> { 'dummy' };
	my	$path = $hDisplayRef -> { 'path' };
	my	$filename = $hDisplayRef -> { 'filename' };
	my	$dspyName = $hDisplayRef -> { 'dspyName' };
	my	$dspyName0 = $hDisplayRef -> { 'dspyName0' };	# $filename에서 "$dspyName"을 제외한 나머지 부분
	my	$frame = $hDisplayRef -> { 'frame' };

	my	$light = $hDisplayRef -> { 'light' };
	if ( $light ) {		# "쉐도우 이미지"
				# ($type eq "shadow" || $type eq 'deepshad' )와 동일
		my	$shadowType = $hDisplayRef -> { 'shadowType ' };

		$hStackRef->{ 'name' } = $light;
		$HoALightSourceRef->{ $light } = [ $light, $name, $filename ];
		# 아래에서 "총 경로"를 구한 뒤, "총 경로"를 기억한다.
		if ($typeChunk == ERModule::ER__CHUNK_TYPE__SHD) {	# [작업 3]
			my	$AoHObjectSHDRef = $hChunkRef->{ 'object array' };
			my	$AoHShadowOfAByLOntoBRef = $hChunkRef->{ 'jcref' };
			foreach my $hShadowOfAByLOntoBRef ( @{ $AoHShadowOfAByLOntoBRef } ) {
				if (isMatched($light, $hShadowOfAByLOntoBRef->{ 'light set' }) == 1) {
					push( @{ $AoHObjectSHDRef }, $hShadowOfAByLOntoBRef );
				}
			}
			if ( @{ $AoHObjectSHDRef } == 0 ) {	# 이 작업에 사용되는 라이트가 하나도 없다.
				$newFate = ERModule::ER__CONTENT_FATE__DELETE;
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
				$newFate = ERModule::ER__CONTENT_FATE__DELETE;
			}
		}
	} else {		# "렌더링 이미지"
		my	$imageFormatExtension = $hDisplayRef -> { 'imageFormatExtension' };

		$hStackRef->{ 'name' } = '';
		$newFate = ERModule::ER__CONTENT_FATE__PRINT;
	}

	my	$newDisplayLine;	# "Display ..."에 대신해서 들어갈 내용
	my	$newBakeDisplayLine;	# "Bake"모드에서 "Display..."에 대신에 들어갈 내용
	my	$newUNIXPath;
	my	$subframeRIBPathname;	# Subframe으로 쪼개졌을 때의 이름
	if ( $newFate == ERModule::ER__CONTENT_FATE__PRINT ) {
		my	$rootPathExpr = $hCommandLineOptionRef -> { 'rootPathExpr' };
		my	$addPathExpr = $hCommandLineOptionRef -> { 'addPathExpr' };
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
			if ( $newFate != ERModule::ER__CONTENT_FATE__DELETE ) {
				# "쉐도우 맵(텍스쳐)"의 경우에,
				# 기존에 있던 텍스쳐를 사용한다면,
				# 해당하는 FrameBegin...FrameEnd는 필요없다.
				my	$bExist = -f $subframeImagePathname;
				print STDERR 'EDITRIB: Info: Shadow Map Texture: ' . '{' . $subframeImagePathname . '}' . ': Exists? ' . ( $bExist ? 'Yes' : 'No' ) . $/ if $bVerbose;
				$newFate = ERModule::ER__CONTENT_FATE__DELETE if ( $bExist && $hCommandLineOptionRef->{ 'bShd' } );
			}
		}

		# Display의 name부분을 "작업 chunk"에 따라 적절하게 바꾸어 준다.
		$newDisplayLine = '#' . join( '#', @$RIBDirectiveContentRef );	# 현재 내용은 모두 주석처리한다.
		$newBakeDisplayLine = $newDisplayLine . 'Display "null" "null" "z"' if $bBake;
#		my	$bakeImagePathname = $subframeImagePathname;
#		$bakeImagePathname =~ s/(\d+)/NULL.$1/;
#		$newBakeDisplayLine = $newDisplayLine . qq/Display "$bakeImagePathname" "$type" "$mode"$dummy/ if $bBake;
		$newDisplayLine .= qq/Display "$plusSign$subframeImagePathname" "$type" "$mode"$dummy/;
		if ( $hStackRef->{ 'name' } eq '' && $typeChunk == ERModule::ER__CHUNK_TYPE__RS ) {
			my	$hSecondaryRef = $hChunkRef->{ 'jcref' }->{ 'secondary' };
			foreach my $channelName ( keys( %{ $hSecondaryRef } ) ) {
				$newDisplayLine .= 'Display "+' . $newUNIXPath . $dspyName . '.' . $keyChunk . '.' . $channelName . '.' . $frame . '.tif" "tiff" "' . join( ',', @{ $hSecondaryRef->{ $channelName } } ) . '"' . "\r\n";
			}
		}

		if ( $light && $hDisplayRef -> { 'shadowType' } eq 'dsh' ) {
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
			ERModule::ER__ENUM_RIB_BOX__FRAME__JUST_BEFORE_DISPLAY,
			$aElementName
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

	if ($newFate == ERModule::ER__CONTENT_FATE__DELETE) {
		$hChunkRef -> { 'aElementName' } = undef;

		# Subframe에 해당하는 핸들을 무효로 해서,
		# "FrameEnd"안에서 아무 일도 하지 않도록 한다.
		$hChunkRef -> { 'subframe file handle' } = undef;

		# 지금까지 모은 내용을 모두 버리고
		$hStackRef->{ 'content' } = [];
		$hStackRef->{ 'fate' } = ERModule::ER__CONTENT_FATE__DELETE;

		return;
	}
	# $CONTENT_PRINT
	if ( $light ) {
		#	( $path, $filename, $dspyName, $dspyName0, $light, $shadowType, $frame ) =
		#		$name =~ /(.*?)(([^\/]+?)(\.([^\.]+?)\.(shd|dsh)\.(\d+)\.tex))$/;
		# $newUNIXPath에는 "rmantex/shd"처럼 "텍스쳐 경로"가 들어있다.
		my	$newUNIXRIBPath = $newUNIXPath;
		$newUNIXRIBPath =~ s|rmantex/shd|rib|;
		$subframeRIBPathname = $newUNIXRIBPath . $dspyName . '.' . $keyChunk . '.' . ($light . '.' . $hDisplayRef -> { 'shadowType' } . '.' . $frame . '.' . 'rib');
	} else {
		# $newUNIXPath에는 "rmanpix"처럼 "이미지 경로"가 들어있다.
		my	$newUNIXRIBPath = $newUNIXPath;
		$newUNIXRIBPath =~ s|rmanpix|rib|;
		print STDERR 'EDITRIB: Info: PATH.RIB: New Path:= ' . '{' . $newUNIXRIBPath . '}' . $/ if $bVerbosePath;
		$newUNIXRIBPath = mkdir2( $newUNIXRIBPath );
		print STDERR 'EDITRIB: Info: PATH.RIB: New Path2:= ' . '{' . $newUNIXRIBPath . '}' . $/ if $bVerbosePath;
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
				ERModule::ER__ENUM_RIB_BOX__DECLARE__JUST_BEFORE_FIRST_FRAME_BEGIN
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

	if ( $bBake && $aElementName eq 'Image' ) {	# "Bake"모드에서 "렌더링 이미지"에 대한 부분은 따로 뽑는다.
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
			ERModule::ER__ENUM_RIB_BOX__DECLARE__JUST_BEFORE_FIRST_FRAME_BEGIN
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
		print $FH $strDeclare if $aElementName ne 'Image' || !$bBake;
		my	$RIBBoxExpr = getRIBBoxExpr(
			$hChunkRef->{ 'rbref' },			
			ERModule::ER__ENUM_RIB_BOX__DECLARE__JUST_BEFORE_FIRST_FRAME_BEGIN
		);
		if ( $RIBBoxExpr) {
			print $FH '## EDITRIB ## <RIB Box> ####' . "\r\n"
				. evalRIBBoxExpr( $RIBBoxExpr ) . "\r\n"
				. '## EDITRIB ## </RIB Box> ####' . "\r\n" if $aElementName ne 'Image' || !$bBake;
		}
	}
	# 지금까지 모은 내용을 출력하고
	print $FH @{ $hStackRef->{ 'content' } } if $aElementName ne 'Image' || !$bBake;
	print $FH $newDisplayLine if $aElementName ne 'Image' || !$bBake;
	$hStackRef->{ 'content' } = [];
	$hStackRef->{ 'bake subframe content' } = [] if $BAKEFH;
	# fate값을 CONTENT_PRINT으로 바꾼다.
	$hStackRef->{ 'fate' } = ERModule::ER__CONTENT_FATE__PRINT;
}


# ENUM_RIB_DIRECTIVE__LIGHTSOURCE
# ENUM_RIB_DIRECTIVE__SURFACE
sub ERBaseShader
{
	my	$hGlobalVariableRef = $ERModule::ghVar{ 'global variable' };
	my	$gFrameNumber = $hGlobalVariableRef -> { 'gFrameNumber' };
	my	$hChunkRef = $ERModule::HoHChunk{ $hGlobalVariableRef -> { 'gKeyChunk' } };
	my	$HoALightSourceRef = $hChunkRef -> { 'light source' };
	my	$aElementName = $hChunkRef -> { 'aElementName' };
	my	$bBake = $hChunkRef->{ 'bBake' };

	return unless $hChunkRef -> { 'stack index' } > 0;	# 적어도 AttributeBegin...AttributeEnd안이거나
								# TransformBegin...TransformEnd안이여야 한다.
								# 불행히도(?) 최상위 스택에도
								# Surface라는 "RIB Directive"가 있다.

	my	$RIBDirectiveContentRef = $hGlobalVariableRef -> { 'gRIBDirectiveContentRef' };
	my	$enumRIBDirective = $hGlobalVariableRef -> { 'gEnumRIBDirective' };
	my	$shaderType = $enumRIBDirective == (ERModule::ER__ENUM_RIB_DIRECTIVE__LIGHTSOURCE) ? ERModule::ER__SHADER_TYPE__LIGHTSOURCE : ERModule::ER__SHADER_TYPE__SURFACE;


	my	$hCommandLineOptionRef = $ERModule::ghVar{ 'command line option' };
	my	$bVerbose = $hCommandLineOptionRef -> { 'bVerbose' };


	# 이 쉐이더가 적용되는 오브젝트를 찾는다.
	my	( $name, $iStack ) = getStackName( $hChunkRef -> { 'stack array' } );
	unless ( $name ) {
		print STDERR 'EDITRIB: Info: ...Failed' . $/ if $bVerbose;
		print STDERR 'EDITRIB: Warning: Cannot find object for shader: ' . join( '', @$RIBDirectiveContentRef );

		return;		# 어느 오브젝트에 속하는 지도 모르는 쉐이더를 가지고 어떤 조작도 할 수 없다.
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
		if ( $hChunkRef->{ 'type' } == ERModule::ER__CHUNK_TYPE__SHD ) {
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
#					push( @aShadowTexture, $HoALightSourceRef->{ $light }->[ 2 ] );
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

	# 들어갈 "RIB Box"가 있는 지 알아본다.
	my	$RIBBoxExpr = getRIBBoxExpr(
		$hChunkRef->{ 'rbref' },			
		$enumRIBDirective == (ERModule::ER__ENUM_RIB_DIRECTIVE__LIGHTSOURCE) ? (ERModule::ER__ENUM_RIB_BOX__OBJECT__JUST_BEFORE_LIGHT_SHADER) : (ERModule::ER__ENUM_RIB_BOX__OBJECT__JUST_BEFORE_SURFACE_SHADER),
		$aElementName
	);
	if ( $RIBBoxExpr) {
		my	$RIBBoxContent = '## EDITRIB ## <RIB Box> ####' . "\r\n"
			. evalRIBBoxExpr( $RIBBoxExpr ) . "\r\n"
			. '## EDITRIB ## </RIB Box> ####' . "\r\n";
		unshift( @$RIBDirectiveContentRef, $RIBBoxContent );
	}

	# 쉐이더 종류가 "Surface"일 때,
	# $CHUNK_TYPE__LL인 경우에는 새로운 쉐이더를 사용여부에 상관없이
	# 쉐이더 바로 위에 "illuminate"정보가 써질 수 있다.
	#	2007/1/5 (EST)
	#	김광호 (Gwang-Ho Kim, iccentric@gmail.com)
	if (
		$shaderType == ERModule::ER__SHADER_TYPE__SURFACE &&
		$hChunkRef->{ 'type' } == ERModule::ER__CHUNK_TYPE__RS
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

	if ( $RenderStats && ( $RenderStats & ERModule::ER__RS__MATTE ) ) {
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

		if ( $RenderStats && !( $RenderStats & ERModule::ER__RS__RECV_SHADOW ) ) {
			push( @$RIBDirectiveContentRef, "\r\n" . '"string NotUsedShadowParameterName" [ "shadowname" ]' . "\r\n" );
		}

		if ( $RenderStats && ( $RenderStats & ERModule::ER__RS__SSSC ) ) {
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
	if ( $enumRIBDirective == ERModule::ER__ENUM_RIB_DIRECTIVE__LIGHTSOURCE ) {
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
}


sub ERBaseAttribute
{
	my	$hGlobalVariableRef = $ERModule::ghVar{ 'global variable' };

	my	$keyChunk = $hGlobalVariableRef -> { 'gKeyChunk' };
	my	$hChunkRef = $ERModule::HoHChunk{ $keyChunk };
#	my	$hChunkRef = $ERModule::HoHChunk{ $hGlobalVariableRef -> { 'gKeyChunk' } };
	my	$RIBDirectiveContentRef = $hGlobalVariableRef -> { 'gRIBDirectiveContentRef' };

	my	$line = join( '', @$RIBDirectiveContentRef );
	if ( $hChunkRef -> { 'stack index' } > 0 ) {		# 최상위 스택이 아닌 곳에서
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
#			$line =~ /^\s*Attribute\s*"visibility".*"int\s+trace"\s*\[\s*1\s*\]/s
#			$line =~ /"trace"/s
		) {
			$hChunkRef->{ 'bRayTracing' } = 1;
			push( @{ $RIBDirectiveContentRef },
				'Attribute "grouping" "string membership" ["+RManReflSubset"]' . "\r\n",
				'Attribute "grouping" "string membership" ["+RManRefrSubset"]' . "\r\n"
			);
		}
	}
}


sub ERBaseAttributeBlockTransformBlockAttribute
{
	my	$hGlobalVariableRef = $ERModule::ghVar{ 'global variable' };
	my	$RIBDirectiveContentRef = $hGlobalVariableRef -> { 'gRIBDirectiveContentRef' };
	my	$keyChunk = $hGlobalVariableRef -> { 'gKeyChunk' };
	my	$hChunkRef = $ERModule::HoHChunk{ $keyChunk };
#	my	$hChunkRef = $ERModule::HoHChunk{ $hGlobalVariableRef -> { 'gKeyChunk' } };
	my	$typeChunk = $hChunkRef -> { 'type' };
	my	$HoALightSourceRef = $hChunkRef -> { 'light source' };
	my	$aElementName = $hChunkRef -> { 'aElementName' };
	my	$iStack = $hChunkRef -> { 'stack index' };
	my	$hStackRef = $hChunkRef -> { 'stack array' } ->[ $iStack ];
	my	$fate = $hStackRef->{ 'fate' };


	# Transform블럭 안에서...
	my	$line = join( '', @$RIBDirectiveContentRef );
	if ( $hStackRef -> { 'type' } == ERModule::ER__STACK_TYPE__TRANSFORM ) {
		#	Attribute "identifier" "string name" ["..."]
		return $fate unless $line =~ /"identifier"\s*"string\s+name"\s*\[\s*"(\S+?)"\s*\]/s;

		# "라이트 쉐이더"
		my	$lightName = $1;
		$hStackRef->{ 'name' } = $lightName;

		# RIB Box에서 사용할 $jcObjectName을 설정한다.
		$hChunkRef -> { 'jcObjectName' } = $lightName;
		# 현재 스택 위치를 기억한다.
		$hChunkRef -> { 'jcObjectNameStackIndex' } = $iStack;

		# RIB Box가 들어갈 위치이다. (ENUM_RIB_BOX__LIGHT__JUST_AFTER_ATTRIBUTE)
		# 들어갈 "RIB Box"가 있는 지 알아본다.
		my	$RIBBoxExpr = getRIBBoxExpr(
			$hChunkRef->{ 'rbref' },			
			ERModule::ER__ENUM_RIB_BOX__LIGHT__JUST_AFTER_ATTRIBUTE,
			$aElementName
		);
		if ( $RIBBoxExpr) {
			my	$RIBBoxContent = '## EDITRIB ## <RIB Box> ####' . "\r\n"
				. evalRIBBoxExpr( $RIBBoxExpr ) . "\r\n"
				. '## EDITRIB ## </RIB Box> ####' . "\r\n";
			push( @{ $RIBDirectiveContentRef }, $RIBBoxContent );
		}

		return $fate;
	}
	return $fate unless $hStackRef -> { 'type' } == ERModule::ER__STACK_TYPE__ATTRIBUTE;

	# Attribute블럭 안에서...
	#	Attribute "identifier" "name" ["..."]
	return $fate unless $line =~ /"identifier"\s*"name"\s*\[\s*"(\S+?)"\s*\]/s;

	# "오브젝트"
	my	$name = $1;
	$hStackRef->{ 'name' } = $name;
	# RIB Box에서 사용할 $jcObjectName을 설정한다.
	$hChunkRef -> { 'jcObjectName' } = $name;
	# 현재 스택 위치를 기억한다.
	$hChunkRef -> { 'jcObjectNameStackIndex' } = $iStack;

	# 지워져야할 오브젝트라면 fate값이 바뀐다.
	#### 오브젝트의 운명을 결정한다.
	#========================================================================#
	my	$bDelete;	# 삭제여부를 결정한다.
				# 1: 삭제된다, 0: 살아남는다

	if ( $typeChunk == ERModule::ER__CHUNK_TYPE__SHD ) {
		$bDelete = 1;	# 원래는 삭제하는데,
				# 살아남을 목록에 있다면 살아남는다.
				# "쉐도우 이미지"에서는 "give set"에 있을 때,
				# "렌더링 이미지"에서는 "take set"에 있을 때
		my	$AoHObjectSHDRef = $aElementName ne 'Image' ? $hChunkRef->{ 'object array' } : $hChunkRef->{ 'jcref' };
		foreach my $hObjectSHDRef ( @{ $AoHObjectSHDRef } ) {
			if (isMatched( $name, $hObjectSHDRef->{ $aElementName ne 'Image' ? 'give set' : 'take set' } ) == 1) {
				$bDelete = 0;
				last;
			}
		}
#		print STDERR "SHD: $name, $light, $bDelete\n";
	} elsif ( $typeChunk == ERModule::ER__CHUNK_TYPE__RS ) {
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
		if ( $aElementName ne 'Image' ) {	# "쉐도우 이미지"
			my	$AoHObjectLLRef = $hChunkRef->{ 'object array' };
			$bDelete = 1;	# 이 오브젝트가 빛을 받는다고 명시된 경우에만
					# 오브젝트가 살아남는다.
					# 아무런 말이 없다면 이 오브젝트는 삭제된다.
			foreach my $hObjectLLRef ( @{ $AoHObjectLLRef } ) {
				if (isMatched( $name, $hObjectLLRef->{ 'object set' } ) == 1) {
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
#			$bDelete = 1;
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
					next unless (isMatched( $name, $hLightLinkingRef->{ 'object set' } ) == 1);
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
		if ( $RenderStats & ERModule::ER__RS__DELETE ) {
			$bDelete = 1;
		} else {
			if ( $aElementName ne 'Image' ) {	# "쉐도우 이미지"단계에서 살아남은 경우
				if ( $RenderStats & ERModule::ER__RS__MATTE ) {
					$bDelete = 1;
				}
				unless ( $RenderStats & ERModule::ER__RS__CAST_SHADOW ) {
					# "Casts Shadows"옵션이 꺼져있다면
					$bDelete = 1;
				}
			} else {	# "렌더링 이미지"단계
				if ( $hChunkRef->{ 'bRayTracing' } ) {
					unless ( $RenderStats & ( ERModule::ER__RS__VISIBILITY | ERModule::ER__RS__RT ) ) {
						$bDelete = 1;
					} else {
						$bDelete = 0;
						unless ( $RenderStats & ERModule::ER__RS__VISIBILITY ) {
							push( @{ $RIBDirectiveContentRef }, 'Attribute "visibility" "int camera" [ 0 ]' . "\r\n" );
						}
						unless ( $RenderStats & ERModule::ER__RS__VIS_REFL ) {
							push( @{ $RIBDirectiveContentRef }, 'Attribute "grouping" "string membership" ["-RManReflSubset"]' . "\r\n" );
						}
						unless ( $RenderStats & ERModule::ER__RS__VIS_REFR ) {
							push( @{ $RIBDirectiveContentRef }, 'Attribute "grouping" "string membership" ["-RManRefrSubset"]' . "\r\n" );
						}
					}
				} else {
					if ( $RenderStats & ERModule::ER__RS__RT ) {
						print STDERR 'EDITRIB: Info: ' . '{' . $name . '}' . ': Ray tracing is OFF but "Visible in Reflections" or "Visible in Refractions" is ON: Ignored' . $/;
					}
					unless ( $illuminance ) {	# 이 오브젝트를 비추는 빛이 없다면
						# "Primary Visibility"옵션을 끈 것과 같다.
						$RenderStats &= ~ERModule::ER__RS__VISIBILITY;
					}
					$bDelete = ( $RenderStats & ERModule::ER__RS__VISIBILITY ) ? 0 : 1;
				}
				$bDelete = 0 if ( $RenderStats & ERModule::ER__RS__MATTE );
			}
		}
	}
	#================================================#
		}
	} else {
		print STDERR 'EDITRIB: Warning: Unknow CHUNK type(' . $typeChunk . '): Ignored' . $/;
	}
	#========================================================================#

	if ( $bDelete ) {	# 지워져야할 오브젝트라면
		# 지금까지 모은 내용을 모두 버리고
		$hStackRef->{ 'content' } = [];
		my	$BAKEFH = $hChunkRef -> { 'bake subframe file handle' };
		$hStackRef->{ 'bake subframe content' } = [] if $BAKEFH;
		# fate값을 CONTENT_DELETE로 바꾼다.
		$hStackRef->{ 'fate' } = ERModule::ER__CONTENT_FATE__DELETE;
		$hChunkRef -> { 'jcObjectName' } = undef;
		$hChunkRef -> { 'jcObjectNameStackIndex' } = undef;

		return ERModule::ER__CONTENT_FATE__DELETE;
	}

	# 살아남는 오브젝트이다.
	# RIB Box가 들어갈 위치이다. (ENUM_RIB_BOX__OBJECT__JUST_AFTER_ATTRIBUTE)
	# 들어갈 "RIB Box"가 있는 지 알아본다.
	my	$RIBBoxExpr = getRIBBoxExpr(
		$hChunkRef->{ 'rbref' },			
		ERModule::ER__ENUM_RIB_BOX__OBJECT__JUST_AFTER_ATTRIBUTE,
		$aElementName
	);
	if ( $RIBBoxExpr) {
		my	$RIBBoxContent = '## EDITRIB ## <RIB Box> ####' . "\r\n"
			. evalRIBBoxExpr( $RIBBoxExpr ) . "\r\n"
			. '## EDITRIB ## </RIB Box> ####' . "\r\n";
		push( @{ $RIBDirectiveContentRef }, $RIBBoxContent );
	}
	return $fate;
}



1;
