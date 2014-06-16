#!/usr/bin/perl -w
use strict;

require "getLineByRIBDirectiveSub.pl";	# "RIB Directive"단위로 한 줄씩 읽어들이는 함수가 정의되어있다.

require 'ERCommonSub.pl';		# 여러 함수들을 읽어들인다.

# %ghVar의 "command line option"을 설정한다.
# 프로그래머가 필요에의해 옵션을 설정할 수도 있다.
require 'ERParseCommandLine.pl';
unless ( parseCommandLine() ) {
	print STDERR 'Usage:' . $/;
	print STDERR "\t" . $0 . ' [-sif ".../siFile.pl"] [-jcf ".../jcFile.pl"] [-k byArt] [-d add/Path/Expr | -R /Root/Path/Expr] [...] x.rib' . $/;
	exit 1;
}

# 프로그램 동작에 필요한 초기화 작업을 한다.
# 프로그래머가 적절한 설정을 한다.
require "ERInitialize.pl";
Initialize();

require "ERBaseSub.pl";
require "ERRIBBoxSub.pl";


use ERModule;

ERModule::main();
