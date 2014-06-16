package ERModule;

use 5.008008;
use strict;
use warnings;
use Carp;

require Exporter;
use AutoLoader;

our @ISA = qw(Exporter);

# Items to export into callers namespace by default. Note: do not export
# names by default without a very good reason. Use EXPORT_OK instead.
# Do not simply export all your public functions/methods/constants.

# This allows declaration	use ERModule ':all';
# If you do not need this, moving things directly into @EXPORT or @EXPORT_OK
# will save memory.
our %EXPORT_TAGS = ( 'all' => [ qw(
	ER__CHUNK_TYPE__BAKE
	ER__CHUNK_TYPE__RS
	ER__CHUNK_TYPE__SHD
	ER__CONTENT_FATE__DELETE
	ER__CONTENT_FATE__PRINT
	ER__CONTENT_FATE__SAVE
	ER__CONTENT_STATE__ADD
	ER__CONTENT_STATE__DELETE
	ER__DECLARE__FRAME_BLOCK
	ER__DECLARE__FRAME_BLOCK_OUTSIDE
	ER__DECLARE__NO_FRAME
	ER__ENUM_RIB_BOX__DECLARE__JUST_BEFORE_FIRST_FRAME_BEGIN
	ER__ENUM_RIB_BOX__FRAME__JUST_BEFORE_DISPLAY
	ER__ENUM_RIB_BOX__FRAME__JUST_BEFORE_WORLD_BEGIN
	ER__ENUM_RIB_BOX__LIGHT__JUST_AFTER_ATTRIBUTE
	ER__ENUM_RIB_BOX__OBJECT__JUST_AFTER_ATTRIBUTE
	ER__ENUM_RIB_BOX__OBJECT__JUST_BEFORE_GEOMETRIC_PRIMITIVE
	ER__ENUM_RIB_BOX__OBJECT__JUST_BEFORE_LIGHT_SHADER
	ER__ENUM_RIB_BOX__OBJECT__JUST_BEFORE_SURFACE_SHADER
	ER__ENUM_RIB_BOX__WORLD__JUST_AFTER_LAST_OBJECT_BLOCK
	ER__ENUM_RIB_BOX__WORLD__JUST_BEFORE_FIRST_GEOMETRIC_PRIMITIVE_BLOCK
	ER__ENUM_RIB_BOX__WORLD__JUST_BEFORE_FIRST_LIGHTSOURCE_BLOCK
	ER__ENUM_RIB_DIRECTIVE__ATTRIBUTE
	ER__ENUM_RIB_DIRECTIVE__ATTRIBUTE_BEGIN
	ER__ENUM_RIB_DIRECTIVE__ATTRIBUTE_END
	ER__ENUM_RIB_DIRECTIVE__DISPLACEMENT
	ER__ENUM_RIB_DIRECTIVE__DISPLAY
	ER__ENUM_RIB_DIRECTIVE__DONT_CARE
	ER__ENUM_RIB_DIRECTIVE__FRAME_BEGIN
	ER__ENUM_RIB_DIRECTIVE__FRAME_END
	ER__ENUM_RIB_DIRECTIVE__GEOMETRIC_PRIMITIVE
	ER__ENUM_RIB_DIRECTIVE__LIGHTSOURCE
	ER__ENUM_RIB_DIRECTIVE__SURFACE
	ER__ENUM_RIB_DIRECTIVE__TRANSFORM_BEGIN
	ER__ENUM_RIB_DIRECTIVE__TRANSFORM_END
	ER__ENUM_RIB_DIRECTIVE__UNDEF
	ER__ENUM_RIB_DIRECTIVE__WORLD_BEGIN
	ER__ENUM_RIB_DIRECTIVE__WORLD_END
	ER__GET_RIB_DIRECTIVE__ERROR
	ER__GET_RIB_DIRECTIVE__EOF
	ER__GET_RIB_DIRECTIVE__EXCEPTION
	ER__GET_RIB_DIRECTIVE__UNKNOWN
	ER__GET_RIB_DIRECTIVE__VALID
	ER__RS__CAST_SHADOW
	ER__RS__DELETE
	ER__RS__MATTE
	ER__RS__RECV_SHADOW
	ER__RS__RT
	ER__RS__SSSC
	ER__RS__VISIBILITY
	ER__RS__VIS_REFL
	ER__RS__VIS_REFR
	ER__SHADER_TYPE__LIGHTSOURCE
	ER__SHADER_TYPE__SURFACE
	ER__STACK_TYPE__ATTRIBUTE
	ER__STACK_TYPE__FRAME
	ER__STACK_TYPE__TRANSFORM
	ER__VAR_SCOPE__ALL
	ER__VAR_SCOPE__GLOBAL
	ER__VAR_SCOPE__JOB_CHUNK
) ] );

our @EXPORT_OK = ( @{ $EXPORT_TAGS{'all'} } );

our @EXPORT = qw(
	ER__CHUNK_TYPE__BAKE
	ER__CHUNK_TYPE__RS
	ER__CHUNK_TYPE__SHD
	ER__CONTENT_FATE__DELETE
	ER__CONTENT_FATE__PRINT
	ER__CONTENT_FATE__SAVE
	ER__CONTENT_STATE__ADD
	ER__CONTENT_STATE__DELETE
	ER__DECLARE__FRAME_BLOCK
	ER__DECLARE__FRAME_BLOCK_OUTSIDE
	ER__DECLARE__NO_FRAME
	ER__ENUM_RIB_BOX__DECLARE__JUST_BEFORE_FIRST_FRAME_BEGIN
	ER__ENUM_RIB_BOX__FRAME__JUST_BEFORE_DISPLAY
	ER__ENUM_RIB_BOX__FRAME__JUST_BEFORE_WORLD_BEGIN
	ER__ENUM_RIB_BOX__LIGHT__JUST_AFTER_ATTRIBUTE
	ER__ENUM_RIB_BOX__OBJECT__JUST_AFTER_ATTRIBUTE
	ER__ENUM_RIB_BOX__OBJECT__JUST_BEFORE_GEOMETRIC_PRIMITIVE
	ER__ENUM_RIB_BOX__OBJECT__JUST_BEFORE_LIGHT_SHADER
	ER__ENUM_RIB_BOX__OBJECT__JUST_BEFORE_SURFACE_SHADER
	ER__ENUM_RIB_BOX__WORLD__JUST_AFTER_LAST_OBJECT_BLOCK
	ER__ENUM_RIB_BOX__WORLD__JUST_BEFORE_FIRST_GEOMETRIC_PRIMITIVE_BLOCK
	ER__ENUM_RIB_BOX__WORLD__JUST_BEFORE_FIRST_LIGHTSOURCE_BLOCK
	ER__ENUM_RIB_DIRECTIVE__ATTRIBUTE
	ER__ENUM_RIB_DIRECTIVE__ATTRIBUTE_BEGIN
	ER__ENUM_RIB_DIRECTIVE__ATTRIBUTE_END
	ER__ENUM_RIB_DIRECTIVE__DISPLACEMENT
	ER__ENUM_RIB_DIRECTIVE__DISPLAY
	ER__ENUM_RIB_DIRECTIVE__DONT_CARE
	ER__ENUM_RIB_DIRECTIVE__FRAME_BEGIN
	ER__ENUM_RIB_DIRECTIVE__FRAME_END
	ER__ENUM_RIB_DIRECTIVE__GEOMETRIC_PRIMITIVE
	ER__ENUM_RIB_DIRECTIVE__LIGHTSOURCE
	ER__ENUM_RIB_DIRECTIVE__SURFACE
	ER__ENUM_RIB_DIRECTIVE__TRANSFORM_BEGIN
	ER__ENUM_RIB_DIRECTIVE__TRANSFORM_END
	ER__ENUM_RIB_DIRECTIVE__UNDEF
	ER__ENUM_RIB_DIRECTIVE__WORLD_BEGIN
	ER__ENUM_RIB_DIRECTIVE__WORLD_END
	ER__GET_RIB_DIRECTIVE__ERROR
	ER__GET_RIB_DIRECTIVE__EOF
	ER__GET_RIB_DIRECTIVE__EXCEPTION
	ER__GET_RIB_DIRECTIVE__UNKNOWN
	ER__GET_RIB_DIRECTIVE__VALID
	ER__RS__CAST_SHADOW
	ER__RS__DELETE
	ER__RS__MATTE
	ER__RS__RECV_SHADOW
	ER__RS__RT
	ER__RS__SSSC
	ER__RS__VISIBILITY
	ER__RS__VIS_REFL
	ER__RS__VIS_REFR
	ER__SHADER_TYPE__LIGHTSOURCE
	ER__SHADER_TYPE__SURFACE
	ER__STACK_TYPE__ATTRIBUTE
	ER__STACK_TYPE__FRAME
	ER__STACK_TYPE__TRANSFORM
	ER__VAR_SCOPE__ALL
	ER__VAR_SCOPE__GLOBAL
	ER__VAR_SCOPE__JOB_CHUNK
);

our	%ghVar = (
	'global variable'	=> {
		# 전역변수가 설정되는 범위(scope)는 두 종류가 있다.
		# 하나는 각 job chunk마다 설정되는 전역변수이고 (VAR_SCOPE__JOB_CHUNK),
		# 다른 하나는 말 그대로 프로그램 전체에서 선언되는 전역변수이다. (VAR_SCOPE__GLOBAL)
		# 물론 두 군데 모두에서 선언되는 전역변수도 있다. (VAR_SCOPE__ALL)
		# 아래의 변수에서 첫 번째에만 선언될 때 변수 앞에 "jc"를,
		# 두 번째에만 선언될 때에는 "g"를 쓰도록 한다.
		# 모두에서 선언될 때는 "a"를 사용한다.
		#
		# gKeyChunk		각 job chunk의 이름
		#			Job chunk의 루프의 시작에서 초기화되고 끝나면 undef으로 설정된다.
		#
		# gFrameNumber		RIB 파일로부터 얻는, 적용되는 frame #
		#			frame #가 없는 RIB 파일에는 이 프로그램이 적용되지 않는다!
		# aElementName		RIB 파일에 있는 각 subframe의 이름
		# 			Shadow pass의 경우에는 라이트의 이름으로,
		# 			final rendering pass의 경우에는 "Image"로 설정된다.
		#			초기값은 undef으로 Display를 만나면 이 값이 설정되고,
		#			FrameEnd에서 다시 undef으로 설정된다.
		#			단, Display를 만나더라도 해당 subframe이 살아남지 않는다면
		#			다시 undef이다. 따라서 해당 job chunk의 경우에는 undef이다.
		# jcObjectName		현 RIB Directive가 속한 Transform블럭/Attribute블럭의 이름
		#			(지금까지 관찰된 바에 따르면)
		#			라이트 오브젝트의 정의는 Transform블럭안의 다음 Attribute에서
		#				Attribute "identifier" "string name" ["..."]
		#			물체의 정의는 Attribute블럭안의 다음 Attribute에서
		#				Attribute "identifier" "name" ["..."]
		#			정해진다.
		#			undef인 초기값이 이 때부터 얻어진 이름으로 설정된다.
		#			해당 블럭이 끝나는 시점인 TransformEnd/AttributeEnd에서 undef으로 다시 설정된다.
		#			(아래의 jcObjectNameStackIndex가 해당 블럭의 끝을 알려준다.)
		#			만약 해당 오브젝트가 삭제된다면 역시 undef으로 설정된다.
		#			따라서 해당 job chunk의 경우에는 undef이다.
		# jcObjectNameStackIndex	jcObjectName을 얻은 스택의 위치를 기억한다.
		#
		# gDeclare		Declare입장에서 어느 부분인지를 말해준다.
		#			DECLARE__NO_FRAME: 첫 번째 FrameBegin을 만나기 전
		#			DECLARE__FRAME_BLOCK: Frame블럭 안
		#			DECLAER__FRAME_BLOCK_OUTSIDE: FrameEnd를 끝나고 FrameBegin만나기 전
		# gStrDeclare		첫 번째 FrameBegin전에 있는 Declare부분의 내용
		#			첫 번째 FrameBegin을 만났을 때 한 번 설정되고 끝난다.
		#
		# gRIBDirective		현재 읽은 RIB Directive에 해당하는 문자열
		# gRIBDirectiveContentRef	해당 RIB Directive를 포함하는 한 줄
		# gEnumRIBDirective	RIB Directive에 해당하는 enumerator
		#			RIB 파일에서 한 줄 읽을 때마다 재설정된다.
		#
		# ghDisplayRef		Display로부터 얻은 정보
		#			RIB Directive가 Display일 때에만 설정된다.
	},

	'command line option'	=> {
		#
		# bAscii		RIB Format; 0:= ASCII, 1:= binarygzip (default)
		# bShd			Reuse shadow map; 0:= NO (default), 1:= YES
		# bVerboseRIBBox
		# bVerbosePath
		# bVerbose
		# bVVerbose
		# bVVVerbose
		#
		# ribFile		원본 RIB 파일이름(경로 포함)
		# rootPathExpr		렌더맨 작업의 최상위 디렉토리 (mtor의 wsroot에 해당한다.)
		# addPathExpr		각 job chunk에 해당하는 디렉토리를 만들 때,
		#			각 job chunk이름앞에 추가되는 이름
		#
		# jcFile		Job chunk에 대한 정보를 가지는 파일
		#			정의되지 않을 수도 있다.
		# byArtKeyName		jcFile이 정의되지 않았을 때 사용되는 기본적으로 사용되는,
		#			job chunk의 이름 (기본값은 "byArt"이다.)
		# aRBFile		Job chunk의 정보를 담은 해쉬(%HoHChunk)의 "rbref"키가 가리키는,
		#			RIB Box을 정보를 담은 파일이름들로 이루어진 배열
		# aSIFile		Job chunk의 정보를 담은 해쉬(%HoHChunk)의 "siref"키가 가리키는,
		#			쉐이더 정보를 담은 파일이름들로 이루어진 배열
	},		# Read only!
);
our	%HoHChunk		= ();
our	%HoARIBBoxInfo		= ();
our	@AoHShaderInfoByArt	= ();
our	@AoHShaderInfoByTd	= ();
our	$SHADER_PATH;
our	$TEXTURE_PATH;

our $VERSION = '0.01';

sub AUTOLOAD {
    # This AUTOLOAD is used to 'autoload' constants from the constant()
    # XS function.

    my $constname;
    our $AUTOLOAD;
    ($constname = $AUTOLOAD) =~ s/.*:://;
    croak "&ERModule::constant not defined" if $constname eq 'constant';
    my ($error, $val) = constant($constname);
    if ($error) { croak $error; }
    {
	no strict 'refs';
	# Fixed between 5.005_53 and 5.005_61
#XXX	if ($] >= 5.00561) {
#XXX	    *$AUTOLOAD = sub () { $val };
#XXX	}
#XXX	else {
	    *$AUTOLOAD = sub { $val };
#XXX	}
    }
    goto &$AUTOLOAD;
}

require XSLoader;
XSLoader::load('ERModule', $VERSION);

# Preloaded methods go here.

# Autoload methods go after =cut, and are processed by the autosplit program.

1;
__END__
# Below is stub documentation for your module. You'd better edit it!

=head1 NAME

ERModule - Perl extension for blah blah blah

=head1 SYNOPSIS

  use ERModule;
  blah blah blah

=head1 DESCRIPTION

Stub documentation for ERModule, created by h2xs. It looks like the
author of the extension was negligent enough to leave the stub
unedited.

Blah blah blah.

=head2 EXPORT

None by default.

=head2 Exportable constants

  ER__CHUNK_TYPE__BAKE
  ER__CHUNK_TYPE__RS
  ER__CHUNK_TYPE__SHD
  ER__CONTENT_FATE__DELETE
  ER__CONTENT_FATE__PRINT
  ER__CONTENT_FATE__SAVE
  ER__CONTENT_STATE__ADD
  ER__CONTENT_STATE__DELETE
  ER__DECLARE__FRAME_BLOCK
  ER__DECLARE__FRAME_BLOCK_OUTSIDE
  ER__DECLARE__NO_FRAME
  ER__ENUM_RIB_BOX__DECLARE__JUST_BEFORE_FIRST_FRAME_BEGIN
  ER__ENUM_RIB_BOX__FRAME__JUST_BEFORE_DISPLAY
  ER__ENUM_RIB_BOX__FRAME__JUST_BEFORE_WORLD_BEGIN
  ER__ENUM_RIB_BOX__LIGHT__JUST_AFTER_ATTRIBUTE
  ER__ENUM_RIB_BOX__OBJECT__JUST_AFTER_ATTRIBUTE
  ER__ENUM_RIB_BOX__OBJECT__JUST_BEFORE_GEOMETRIC_PRIMITIVE
  ER__ENUM_RIB_BOX__OBJECT__JUST_BEFORE_LIGHT_SHADER
  ER__ENUM_RIB_BOX__OBJECT__JUST_BEFORE_SURFACE_SHADER
  ER__ENUM_RIB_BOX__WORLD__JUST_AFTER_LAST_OBJECT_BLOCK
  ER__ENUM_RIB_BOX__WORLD__JUST_BEFORE_FIRST_GEOMETRIC_PRIMITIVE_BLOCK
  ER__ENUM_RIB_BOX__WORLD__JUST_BEFORE_FIRST_LIGHTSOURCE_BLOCK
  ER__ENUM_RIB_DIRECTIVE__ATTRIBUTE
  ER__ENUM_RIB_DIRECTIVE__ATTRIBUTE_BEGIN
  ER__ENUM_RIB_DIRECTIVE__ATTRIBUTE_END
  ER__ENUM_RIB_DIRECTIVE__DISPLACEMENT
  ER__ENUM_RIB_DIRECTIVE__DISPLAY
  ER__ENUM_RIB_DIRECTIVE__DONT_CARE
  ER__ENUM_RIB_DIRECTIVE__FRAME_BEGIN
  ER__ENUM_RIB_DIRECTIVE__FRAME_END
  ER__ENUM_RIB_DIRECTIVE__GEOMETRIC_PRIMITIVE
  ER__ENUM_RIB_DIRECTIVE__LIGHTSOURCE
  ER__ENUM_RIB_DIRECTIVE__SURFACE
  ER__ENUM_RIB_DIRECTIVE__TRANSFORM_BEGIN
  ER__ENUM_RIB_DIRECTIVE__TRANSFORM_END
  ER__ENUM_RIB_DIRECTIVE__UNDEF
  ER__ENUM_RIB_DIRECTIVE__WORLD_BEGIN
  ER__ENUM_RIB_DIRECTIVE__WORLD_END
  ER__GET_RIB_DIRECTIVE__ERROR
  ER__GET_RIB_DIRECTIVE__EOF
  ER__GET_RIB_DIRECTIVE__EXCEPTION
  ER__GET_RIB_DIRECTIVE__UNKNOWN
  ER__GET_RIB_DIRECTIVE__VALID
  ER__RS__CAST_SHADOW
  ER__RS__DELETE
  ER__RS__MATTE
  ER__RS__RECV_SHADOW
  ER__RS__RT
  ER__RS__SSSC
  ER__RS__VISIBILITY
  ER__RS__VIS_REFL
  ER__RS__VIS_REFR
  ER__SHADER_TYPE__LIGHTSOURCE
  ER__SHADER_TYPE__SURFACE
  ER__STACK_TYPE__ATTRIBUTE
  ER__STACK_TYPE__FRAME
  ER__STACK_TYPE__TRANSFORM
  ER__VAR_SCOPE__ALL
  ER__VAR_SCOPE__GLOBAL
  ER__VAR_SCOPE__JOB_CHUNK



=head1 SEE ALSO

Mention other useful documentation such as the documentation of
related modules or operating system documentation (such as man pages
in UNIX), or any relevant external documentation such as RFCs or
standards.

If you have a mailing list set up for your module, mention it here.

If you have a web site set up for your module, mention it here.

=head1 AUTHOR

Gwang-Ho Kim <lt>iccentric@gmail.com<gt>

=head1 COPYRIGHT AND LICENSE

Copyright (C) 2007 by Gwang-Ho Kim

This library is free software; you can redistribute it and/or modify
it under the same terms as Perl itself, either Perl version 5.8.8 or,
at your option, any later version of Perl 5 you may have available.


=cut
