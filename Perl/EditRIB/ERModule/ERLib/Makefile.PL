use ExtUtils::MakeMaker;
$Verbose = 1;
WriteMakefile(
	'CC'	=> 'g++',
	'LD'	=> 'g++',

	NAME   => 'ERModule::ERLib',
	SKIP   => [qw(all static static_lib dynamic dynamic_lib)],
	clean  => {'FILES' => 'libERLib$(LIB_EXT)'},
);

sub MY::top_targets {
	'
all :: static

pure_all :: static

static ::       libERLib$(LIB_EXT)

libERLib$(LIB_EXT): $(O_FILES)
	$(AR) cr libERLib$(LIB_EXT) $(O_FILES)
	$(RANLIB) libERLib$(LIB_EXT)
	';
}
