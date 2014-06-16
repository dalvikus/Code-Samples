use 5.008008;
use ExtUtils::MakeMaker;
# See lib/ExtUtils/MakeMaker.pm for details of how to influence
# the contents of the Makefile that is written.
WriteMakefile(
	CC	=> 'g++',
	LD	=> 'g++',
	MYEXTLIB	=> 'ERLib/libERLib$(LIB_EXT)',

    NAME              => 'ERModule',
    VERSION_FROM      => 'lib/ERModule.pm', # finds $VERSION
    PREREQ_PM         => {}, # e.g., Module::Name => 1.1
    ($] >= 5.005 ?     ## Add these new keywords supported since 5.005
      (ABSTRACT_FROM  => 'lib/ERModule.pm', # retrieve abstract from module
       AUTHOR         => 'Gwang-Ho Kim <iccentric@gmail.com>') : ()),
    LIBS              => [''], # e.g., '-lm'
    DEFINE            => '', # e.g., '-DHAVE_SOMETHING'
    INC               => '-I.', # e.g., '-I. -I/usr/include/other'
	# Un-comment this if you add C files to link with later:
    # OBJECT            => '$(O_FILES)', # link all the C files too
);
if  (eval {require ExtUtils::Constant; 1}) {
  # If you edit these definitions to change the constants used by this module,
  # you will need to use the generated const-c.inc and const-xs.inc
  # files to replace their "fallback" counterparts before distributing your
  # changes.
  my @names = (qw(ER__CHUNK_TYPE__BAKE ER__CHUNK_TYPE__RS ER__CHUNK_TYPE__SHD
		 ER__CONTENT_FATE__UNDEF ER__CONTENT_FATE__DELETE ER__CONTENT_FATE__PRINT ER__CONTENT_FATE__SAVE
		 ER__CONTENT_STATE__ADD ER__CONTENT_STATE__DELETE
		 ER__DECLARE__FRAME_BLOCK ER__DECLARE__FRAME_BLOCK_OUTSIDE
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
		 ER__ENUM_RIB_DIRECTIVE__WORLD_END ER__GET_RIB_DIRECTIVE__ERROR ER__GET_RIB_DIRECTIVE__EOF
		 ER__GET_RIB_DIRECTIVE__EXCEPTION
		 ER__GET_RIB_DIRECTIVE__UNKNOWN ER__GET_RIB_DIRECTIVE__VALID
		 ER__RS__CAST_SHADOW ER__RS__DELETE ER__RS__MATTE
		 ER__RS__RECV_SHADOW ER__RS__RT ER__RS__SSSC ER__RS__VISIBILITY
		 ER__RS__VIS_REFL ER__RS__VIS_REFR ER__SHADER_TYPE__LIGHTSOURCE
		 ER__SHADER_TYPE__SURFACE ER__STACK_TYPE__ATTRIBUTE
		 ER__STACK_TYPE__FRAME ER__STACK_TYPE__TRANSFORM
		 ER__VAR_SCOPE__ALL ER__VAR_SCOPE__GLOBAL
		 ER__VAR_SCOPE__JOB_CHUNK));
  ExtUtils::Constant::WriteConstants(
                                     NAME         => 'ERModule',
                                     NAMES        => \@names,
                                     DEFAULT_TYPE => 'IV',
                                     C_FILE       => 'const-c.inc',
                                     XS_FILE      => 'const-xs.inc',
                                  );

}
else {
  use File::Copy;
  use File::Spec;
  foreach my $file ('const-c.inc', 'const-xs.inc') {
    my $fallback = File::Spec->catfile('fallback', $file);
    copy ($fallback, $file) or die "Can't copy $fallback to $file: $!";
  }
}


sub MY::postamble {
	'
$(MYEXTLIB): ERLib/Makefile
	cd ERLib && $(MAKE) $(PASSTHRU)
	';
}
