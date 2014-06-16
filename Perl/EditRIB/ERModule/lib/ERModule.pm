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
		# ���������� �����Ǵ� ����(scope)�� �� ������ �ִ�.
		# �ϳ��� �� job chunk���� �����Ǵ� ���������̰� (VAR_SCOPE__JOB_CHUNK),
		# �ٸ� �ϳ��� �� �״�� ���α׷� ��ü���� ����Ǵ� ���������̴�. (VAR_SCOPE__GLOBAL)
		# ���� �� ���� ��ο��� ����Ǵ� ���������� �ִ�. (VAR_SCOPE__ALL)
		# �Ʒ��� �������� ù ��°���� ����� �� ���� �տ� "jc"��,
		# �� ��°���� ����� ������ "g"�� ������ �Ѵ�.
		# ��ο��� ����� ���� "a"�� ����Ѵ�.
		#
		# gKeyChunk		�� job chunk�� �̸�
		#			Job chunk�� ������ ���ۿ��� �ʱ�ȭ�ǰ� ������ undef���� �����ȴ�.
		#
		# gFrameNumber		RIB ���Ϸκ��� ���, ����Ǵ� frame #
		#			frame #�� ���� RIB ���Ͽ��� �� ���α׷��� ������� �ʴ´�!
		# aElementName		RIB ���Ͽ� �ִ� �� subframe�� �̸�
		# 			Shadow pass�� ��쿡�� ����Ʈ�� �̸�����,
		# 			final rendering pass�� ��쿡�� "Image"�� �����ȴ�.
		#			�ʱⰪ�� undef���� Display�� ������ �� ���� �����ǰ�,
		#			FrameEnd���� �ٽ� undef���� �����ȴ�.
		#			��, Display�� �������� �ش� subframe�� ��Ƴ��� �ʴ´ٸ�
		#			�ٽ� undef�̴�. ���� �ش� job chunk�� ��쿡�� undef�̴�.
		# jcObjectName		�� RIB Directive�� ���� Transform��/Attribute���� �̸�
		#			(���ݱ��� ������ �ٿ� ������)
		#			����Ʈ ������Ʈ�� ���Ǵ� Transform������ ���� Attribute����
		#				Attribute "identifier" "string name" ["..."]
		#			��ü�� ���Ǵ� Attribute������ ���� Attribute����
		#				Attribute "identifier" "name" ["..."]
		#			��������.
		#			undef�� �ʱⰪ�� �� ������ ����� �̸����� �����ȴ�.
		#			�ش� ���� ������ ������ TransformEnd/AttributeEnd���� undef���� �ٽ� �����ȴ�.
		#			(�Ʒ��� jcObjectNameStackIndex�� �ش� ���� ���� �˷��ش�.)
		#			���� �ش� ������Ʈ�� �����ȴٸ� ���� undef���� �����ȴ�.
		#			���� �ش� job chunk�� ��쿡�� undef�̴�.
		# jcObjectNameStackIndex	jcObjectName�� ���� ������ ��ġ�� ����Ѵ�.
		#
		# gDeclare		Declare���忡�� ��� �κ������� �����ش�.
		#			DECLARE__NO_FRAME: ù ��° FrameBegin�� ������ ��
		#			DECLARE__FRAME_BLOCK: Frame�� ��
		#			DECLAER__FRAME_BLOCK_OUTSIDE: FrameEnd�� ������ FrameBegin������ ��
		# gStrDeclare		ù ��° FrameBegin���� �ִ� Declare�κ��� ����
		#			ù ��° FrameBegin�� ������ �� �� �� �����ǰ� ������.
		#
		# gRIBDirective		���� ���� RIB Directive�� �ش��ϴ� ���ڿ�
		# gRIBDirectiveContentRef	�ش� RIB Directive�� �����ϴ� �� ��
		# gEnumRIBDirective	RIB Directive�� �ش��ϴ� enumerator
		#			RIB ���Ͽ��� �� �� ���� ������ �缳���ȴ�.
		#
		# ghDisplayRef		Display�κ��� ���� ����
		#			RIB Directive�� Display�� ������ �����ȴ�.
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
		# ribFile		���� RIB �����̸�(��� ����)
		# rootPathExpr		������ �۾��� �ֻ��� ���丮 (mtor�� wsroot�� �ش��Ѵ�.)
		# addPathExpr		�� job chunk�� �ش��ϴ� ���丮�� ���� ��,
		#			�� job chunk�̸��տ� �߰��Ǵ� �̸�
		#
		# jcFile		Job chunk�� ���� ������ ������ ����
		#			���ǵ��� ���� ���� �ִ�.
		# byArtKeyName		jcFile�� ���ǵ��� �ʾ��� �� ���Ǵ� �⺻������ ���Ǵ�,
		#			job chunk�� �̸� (�⺻���� "byArt"�̴�.)
		# aRBFile		Job chunk�� ������ ���� �ؽ�(%HoHChunk)�� "rbref"Ű�� ����Ű��,
		#			RIB Box�� ������ ���� �����̸���� �̷���� �迭
		# aSIFile		Job chunk�� ������ ���� �ؽ�(%HoHChunk)�� "siref"Ű�� ����Ű��,
		#			���̴� ������ ���� �����̸���� �̷���� �迭
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
