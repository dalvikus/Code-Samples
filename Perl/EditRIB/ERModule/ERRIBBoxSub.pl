use ERModule;

use strict;
no strict "refs";


# ENUM_RIB_DIRECTIVE__WORLD_BEGIN
sub ERRIBBoxFrameJustBeforeWorldBegin
{
	my	$hGlobalVariableRef = $ERModule::ghVar{ 'global variable' };
	my	$RIBDirectiveContentRef = $hGlobalVariableRef -> { 'gRIBDirectiveContentRef' };
	my	$keyChunk = $hGlobalVariableRef -> { 'gKeyChunk' };
	my	$hChunkRef = $ERModule::HoHChunk{ $keyChunk };
	my	$aElementName = $hChunkRef -> { 'aElementName' };


	# RIB Box가 들어갈 위치이다. (ENUM_RIB_BOX__FRAME__JUST_BEFORE_WORLD_BEGIN)
	# 들어갈 "RIB Box"가 있는 지 알아본다.
	my	$RIBBoxExpr = getRIBBoxExpr(
		$hChunkRef->{ 'rbref' },			
		ERModule::ER__ENUM_RIB_BOX__FRAME__JUST_BEFORE_WORLD_BEGIN,
		$aElementName
	);
	if ( $RIBBoxExpr) {
		my	$RIBBoxContent = '## EDITRIB ## <RIB Box> ####' . "\r\n"
			. evalRIBBoxExpr( $RIBBoxExpr ) . "\r\n"
			. '## EDITRIB ## </RIB Box> ####' . "\r\n";
		unshift( @$RIBDirectiveContentRef, $RIBBoxContent );
	}
}


# ENUM_RIB_DIRECTIVE__WORLD_END
sub ERRIBBoxWorldJustAfterLastObjectBlock
{
	my	$hGlobalVariableRef = $ERModule::ghVar{ 'global variable' };
	my	$RIBDirectiveContentRef = $hGlobalVariableRef -> { 'gRIBDirectiveContentRef' };
	my	$keyChunk = $hGlobalVariableRef -> { 'gKeyChunk' };
	my	$hChunkRef = $ERModule::HoHChunk{ $keyChunk };
	my	$aElementName = $hChunkRef -> { 'aElementName' };


	# RIB Box가 들어갈 위치이다. (ENUM_RIB_BOX__WORLD__JUST_AFTER_LAST_OBJECT_BLOCK)
	# 들어갈 "RIB Box"가 있는 지 알아본다.
	my	$RIBBoxExpr = getRIBBoxExpr(
		$hChunkRef->{ 'rbref' },			
		ERModule::ER__ENUM_RIB_BOX__WORLD__JUST_AFTER_LAST_OBJECT_BLOCK,
		$aElementName
	);
	if ( $RIBBoxExpr) {
		my	$RIBBoxContent = '## EDITRIB ## <RIB Box> ####' . "\r\n"
			. evalRIBBoxExpr( $RIBBoxExpr ) . "\r\n"
			. '## EDITRIB ## </RIB Box> ####' . "\r\n";
		unshift( @$RIBDirectiveContentRef, $RIBBoxContent );
	}
}


# ENUM_RIB_DIRECTIVE__GEOMETRIC_PRIMITIVE
sub ERRIBBoxObjectJustBeforeGeometricPrimitive
{
	my	$hGlobalVariableRef = $ERModule::ghVar{ 'global variable' };
	my	$RIBDirectiveContentRef = $hGlobalVariableRef -> { 'gRIBDirectiveContentRef' };
	my	$keyChunk = $hGlobalVariableRef -> { 'gKeyChunk' };
	my	$hChunkRef = $ERModule::HoHChunk{ $keyChunk };
	my	$aElementName = $hChunkRef -> { 'aElementName' };


	# RIB Box가 들어갈 위치이다. (ENUM_RIB_BOX__OBJECT__JUST_BEFORE_GEOMETRIC_PRIMITIVE)
	# 들어갈 "RIB Box"가 있는 지 알아본다.
	my	$RIBBoxExpr = getRIBBoxExpr(
		$hChunkRef->{ 'rbref' },			
		ERModule::ER__ENUM_RIB_BOX__OBJECT__JUST_BEFORE_GEOMETRIC_PRIMITIVE,
		$aElementName
	);
	if ( $RIBBoxExpr) {
		my	$RIBBoxContent = '## EDITRIB ## <RIB Box> ####' . "\r\n"
			. evalRIBBoxExpr( $RIBBoxExpr ) . "\r\n"
			. '## EDITRIB ## </RIB Box> ####' . "\r\n";
		addArrayRef( $RIBDirectiveContentRef, 0, \$RIBBoxContent );
	}
}


# ENUM_RIB_DIRECTIVE__LIGHTSOURCE
sub ERRIBBoxWorldJustBeforeFirstLightSourceBlock
{
	my	$hGlobalVariableRef = $ERModule::ghVar{ 'global variable' };
	my	$RIBDirectiveContentRef = $hGlobalVariableRef -> { 'gRIBDirectiveContentRef' };
	my	$keyChunk = $hGlobalVariableRef -> { 'gKeyChunk' };
	my	$hChunkRef = $ERModule::HoHChunk{ $keyChunk };
	my	$FH = $hChunkRef -> { 'file handle' };
	my	$SUBFH = $hChunkRef -> { 'subframe file handle' };
	my	$BAKEFH = $hChunkRef -> { 'bake subframe file handle' };
	my	$aElementName = $hChunkRef -> { 'aElementName' };
	my	$bBake = $hChunkRef->{ 'bBake' };


	return if $hChunkRef -> { 'found light source' };
	$hChunkRef->{ 'found light source' } = 1;

	# RIB Box가 들어갈 위치이다. (ENUM_RIB_BOX__WORLD__JUST_BEFORE_FIRST_LIGHTSOURCE_BLOCK)
	# 들어갈 "RIB Box"가 있는 지 알아본다.
	my	$RIBBoxExpr = getRIBBoxExpr(
		$hChunkRef->{ 'rbref' },			
		ERModule::ER__ENUM_RIB_BOX__WORLD__JUST_BEFORE_FIRST_LIGHTSOURCE_BLOCK,
		$aElementName
	);
	if ( $RIBBoxExpr) {
		my	$RIBBoxContent = '## EDITRIB ## <RIB Box> ####' . "\r\n"
			. evalRIBBoxExpr( $RIBBoxExpr ) . "\r\n"
			. '## EDITRIB ## </RIB Box> ####' . "\r\n";
		print $FH $RIBBoxContent if $aElementName ne 'Image' || !$bBake;
		print $SUBFH $RIBBoxContent if $SUBFH;
		print $BAKEFH $RIBBoxContent if $BAKEFH;
	}
}


# ENUM_RIB_DIRECTIVE__GEOMETRIC_PRIMITIVE
sub ERRIBBoxWorldJustBeforeFirstGeometricPrimitiveBlock
{
	my	$hGlobalVariableRef = $ERModule::ghVar{ 'global variable' };
	my	$RIBDirectiveContentRef = $hGlobalVariableRef -> { 'gRIBDirectiveContentRef' };
	my	$keyChunk = $hGlobalVariableRef -> { 'gKeyChunk' };
	my	$hChunkRef = $ERModule::HoHChunk{ $keyChunk };
	my	$FH = $hChunkRef -> { 'file handle' };
	my	$SUBFH = $hChunkRef -> { 'subframe file handle' };
	my	$BAKEFH = $hChunkRef -> { 'bake subframe file handle' };
	my	$aElementName = $hChunkRef -> { 'aElementName' };
	my	$bBake = $hChunkRef->{ 'bBake' };


	return if $hChunkRef -> { 'found geometric primitive' };
	$hChunkRef->{ 'found geometric primitive' } = 1;

	print $BAKEFH <<BAKETEXT if $BAKEFH;
## EDITRIB ## <BAKE> ####\r
	Attribute "cull" "hidden" 0		# don't cull hidden surfaces\r
	Attribute "cull" "backfacing" 0		# don't cull backfacing surfaces\r
	Attribute "dice" "rasterorient" 0	# turn viewdependent gridding off\r
## EDITRIB ## </BAKE> ####\r
BAKETEXT

	# RIB Box가 들어갈 위치이다. (ENUM_RIB_BOX__WORLD__JUST_BEFORE_FIRST_GEOMETRIC_PRIMITIVE_BLOCK)
	# 들어갈 "RIB Box"가 있는 지 알아본다.
	my	$RIBBoxExpr = getRIBBoxExpr(
		$hChunkRef->{ 'rbref' },			
		ERModule::ER__ENUM_RIB_BOX__WORLD__JUST_BEFORE_FIRST_GEOMETRIC_PRIMITIVE_BLOCK,
		$aElementName
	);
	if ( $RIBBoxExpr) {
		my	$RIBBoxContent = '## EDITRIB ## <RIB Box> ####' . "\r\n"
			. evalRIBBoxExpr( $RIBBoxExpr ) . "\r\n"
			. '## EDITRIB ## </RIB Box> ####' . "\r\n";
		print $FH $RIBBoxContent if $aElementName ne 'Image' || !$bBake;
		print $SUBFH $RIBBoxContent if $SUBFH;
		print $BAKEFH $RIBBoxContent if $BAKEFH;
	}
}



1;
