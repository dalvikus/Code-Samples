#!/bin/perl -w

$__DEBUG__ = 0x00;

########################################################################

$g__stringMax = -1;
%g__stringHash = ();
%g__RIRequestHash = (
	PointsGeneralPolygons	=> '',
	NuPatch			=> '',

	SubdivisionMesh		=> '',
);
$g__RIRequestMax = 0177;	# magic # ?!~

########################################################################

$ascii_RIB = '';
$binary_RIB = '';
while ( $x = <> ) {
	if ( ( $word ) = ( $x =~ /^\s*([A-Z][[:alpha:]]+)/ ) ) {
		if ( $ascii_RIB ne '' ) {
			packing( \$ascii_RIB, \$binary_RIB );
if ( $__DEBUG__ & 0x04 ) {
			$L_ascii = length( $ascii_RIB );
			$L_binary = length( $binary_RIB );
			$f_diff = -( $L_ascii - $L_binary ) / $L_ascii * 100;
			print STDERR "\t" . $L_ascii . ':' . $L_binary . ' [ ' . sprintf( "%.1f", $f_diff ) . '% ]' . $/;
}
			# append '\n'
			print '#' x 72 . $/ . $binary_RIB . $/ . '#' x 72 . $/;
			# reset
			$ascii_RIB = '';
		}

		unless ( exists( $g__RIRequestHash{ $word } ) ) {
			print $x;
		} else {
			$ascii_RIB = $x;
if ( $__DEBUG__ & 0x04 ) {
			print STDERR $word . ':' . $/;
}
		}
	} else {
		unless ( $ascii_RIB ne '' ) {
			print $x;
		} else {
			$ascii_RIB .= $x;
		}
	}
}
if ( $ascii_RIB ne '' ) {
	packing( \$ascii_RIB, \$binary_RIB );
	# append '\n'
	print '#' x 72 . $/ . $binary_RIB . $/ . '#' x 72 . $/;
	# reset
	$ascii_RIB = '';
}

sub	packing
{
	my	( $ref__ascii_RIB, $ref__binary_RIB ) = @_;

	my	( $gprims ) = ( ${ $ref__ascii_RIB } =~ /^\s*(\S+)/ );

	# NuPatch
	$gprims =~ /^NuPatch$/
		&&
	NuPatch( $ref__ascii_RIB, $ref__binary_RIB );

	# PointsGeneralPolygons
	$gprims =~ /^PointsGeneralPolygons$/
		&&
	PointsGeneralPolygons( $ref__ascii_RIB, $ref__binary_RIB );

	# SubdivisionMesh
	$gprims =~ /^SubdivisionMesh$/
		&&
	SubdivisionMesh( $ref__ascii_RIB, $ref__binary_RIB );

	return;
}
exit;

########################################################################




sub NuPatch
{
if ( $__DEBUG__ & 0x01 ) {
	print STDERR '<NuPatch>' . $/;
}
	my	( $ref__ascii_RIB, $ref__binary_RIB ) = @_;
	my	$re__NuPatch = '\s*NuPatch\s+'
		. '\s*(\d+)\s+(\d+)\s+\[\s*(.+?)\s*\]\s+(\S+\s+\S+)\s+'
		. '\s*(\d+)\s+(\d+)\s+\[\s*(.+?)\s*\]\s+(\S+\s+\S+)\s+'
		. '(.*)';

	unless ( ${ $ref__ascii_RIB } =~ /^$re__NuPatch$/s ) {
		print STDERR 'NuPatch: Invalid' . $/;
		return 0;
	}
	my	$nu = $1;		# RtInt nu
	my	$uorder = $2;		# RtInt uorder
	my	$uknot_ = $3;		# RtFloat uknot[]
	my	$uminumax_ = $4;	# RtFloat umin, RtFloat umax
					# RtFloat umin, RtInt umax (???)
	my	$nv = $5;		# RtInt nv
	my	$vorder = $6;		# RtInt vorder
	my	$vknot_ = $7;		# RtFloat vknot[]
	my	$vminvmax_ = $8;	# RtFloat vmin, RtFloat vmax
					# RtFloat vmin, RtInt vmax (???)
	my	$parameterlist = $9;
if ( $__DEBUG__ & 0x01 ) {
	print STDERR 'nu = \'' . $nu . '\'' . $/;
	print STDERR 'uorder = \'' . $uorder . '\'' . $/;
	print STDERR 'uknot[] = [\'' . $uknot_ . '\']' . $/;
	print STDERR 'umin\s*uman = \'' . $uminumax_ . '\'' . $/;
	print STDERR 'nv = \'' . $nv . '\'' . $/;
	print STDERR 'vorder = \'' . $vorder . '\'' . $/;
	print STDERR 'vknot[] = [\'' . $vknot_ . '\']' . $/;
	print STDERR 'vmin\s*vman = \'' . $vminvmax_ . '\'' . $/;
}

	${ $ref__binary_RIB } = refRIRequest( 'NuPatch' );
	${ $ref__binary_RIB } .= binInt( $nu );
	${ $ref__binary_RIB } .= binInt( $uorder );
	${ $ref__binary_RIB } .= binArray( 'FLOAT', [ split( /\s+/, $uknot_ ) ] );
	$uminumax_ =~ /^(\S+)\s+(\S+)$/;
	${ $ref__binary_RIB } .= binFloat( $1 );
	${ $ref__binary_RIB } .= binFloat( $2 );

	${ $ref__binary_RIB } .= binInt( $nv );
	${ $ref__binary_RIB } .= binInt( $vorder );
	${ $ref__binary_RIB } .= binArray( 'FLOAT', [ split( /\s+/, $vknot_ ) ] );
	$vminvmax_ =~ /^(\S+)\s+(\S+)$/;
	${ $ref__binary_RIB } .= binFloat( $1 );
	${ $ref__binary_RIB } .= binFloat( $2 );
	getParameterList( $parameterlist, $ref__binary_RIB );

if ( $__DEBUG__ & 0x01 ) {
	print STDERR '</NuPatch>' . $/;
}
	return 1;
}

sub SubdivisionMesh
{
if ( $__DEBUG__ & 0x01 ) {
	print STDERR '<SubdivisionMesh>' . $/;
}
	my	( $ref__ascii_RIB, $ref__binary_RIB ) = @_;
	my	$re__SubdivisionMesh = '\s*SubdivisionMesh\s+'
		. '\s*"(.+?)"\s+'
		. '\s*\[\s*(.+?)\s*\]\s+'
		. '\s*\[\s*(.+?)\s*\]\s+'
		. '\s*\[\s*(.+?)\s*\]\s+'
		. '\s*\[\s*(.+?)\s*\]\s+'
		. '\s*\[\s*(.+?)\s*\]\s+'
		. '\s*\[\s*(.+?)\s*\]\s+'
		. '(.*)';

	unless ( ${ $ref__ascii_RIB } =~ /^$re__SubdivisionMesh$/s ) {
		print STDERR 'SubdivisionMesh: Invalid' . $/;
		return 0;
	}
	my	$scheme = $1;		# RtToken scheme
	my	$nvertices_ = $2;	# RtInt nvertices[]
	my	$vertices_ = $3;	# RtInt vertices[]
	my	$tags_ = $4;		# RtToken tags[]
	my	$nargs_ = $5;		# RtInt nargs[]
	my	$intargs_ = $6;		# RtInt intargs[]
	my	$floatargs_ = $7;	# RtFloat floatargs[]
	my	$parameterlist = $8;
if ( $__DEBUG__ & 0x01 ) {
	print STDERR 'scheme = \'' . $scheme . '\'' . $/;
	print STDERR 'nvertices = [\'' . $nvertices_ . '\']' . $/;
	print STDERR 'vertices = [\'' . $vertices_ . '\']' . $/;
	print STDERR 'tags = [\'' . $tags_ . '\']' . $/;
	print STDERR 'nargs = [\'' . $nargs_ . '\']' . $/;
	print STDERR 'intargs = [\'' . $intargs_ . '\']' . $/;
	print STDERR 'floatargs = [\'' . $floatargs_ . '\']' . $/;
}

	${ $ref__binary_RIB } = refRIRequest( 'SubdivisionMesh' );
	${ $ref__binary_RIB } .= refString( $scheme );
	${ $ref__binary_RIB } .= binArray( 'INT', [ split( /\s+/, $nvertices_ ) ] );
	${ $ref__binary_RIB } .= binArray( 'INT', [ split( /\s+/, $vertices_ ) ] );
	${ $ref__binary_RIB } .= binArray( 'STRING', [ split( /\s+/, $tags_ ) ] );
	${ $ref__binary_RIB } .= binArray( 'INT', [ split( /\s+/, $nargs_ ) ] );
	${ $ref__binary_RIB } .= binArray( 'INT', [ split( /\s+/, $intargs_ ) ] );
	${ $ref__binary_RIB } .= binArray( 'FLOAT', [ split( /\s+/, $floatargs_ ) ] );
	getParameterList( $parameterlist, $ref__binary_RIB );

if ( $__DEBUG__ & 0x01 ) {
	print STDERR '</SubdivisionMesh>' . $/;
}
	return 1;
}

sub PointsGeneralPolygons
{
if ( $__DEBUG__ & 0x01 ) {
	print STDERR '<PointsGeneralPolygons>' . $/;
}
	my	( $ref__ascii_RIB, $ref__binary_RIB ) = @_;
	my	$re__PointsGeneralPolygons = '\s*PointsGeneralPolygons\s+'
		. '\s*\[\s*(.+?)\s*\]\s+'
		. '\s*\[\s*(.+?)\s*\]\s+'
		. '\s*\[\s*(.+?)\s*\]\s+'
		. '(.*)';

	unless ( ${ $ref__ascii_RIB } =~ /^$re__PointsGeneralPolygons$/s ) {
		print STDERR 'PointsGeneralPolygons: Invalid' . $/;
		return 0;
	}
	my	$nloops_ = $1;		# RtInt nloops[]
	my	$nvertices_ = $2;	# RtInt nvertices[]
	my	$vertices_ = $3;	# RtInt vertices[]
	my	$parameterlist = $4;
if ( $__DEBUG__ & 0x01 ) {
	print STDERR 'nloops = [\'' . $1 . '\']' . $/;
	print STDERR 'nvertices = [\'' . $2 . '\']' . $/;
	print STDERR 'vertices = [\'' . $3 . '\']' . $/;
}

	${ $ref__binary_RIB } = refRIRequest( 'PointsGeneralPolygons' );
	${ $ref__binary_RIB } .= binArray( 'INT', [ split( /\s+/, $nloops_ ) ] );
	${ $ref__binary_RIB } .= binArray( 'INT', [ split( /\s+/, $nvertices_ ) ] );
	${ $ref__binary_RIB } .= binArray( 'INT', [ split( /\s+/, $vertices_ ) ] );
	getParameterList( $parameterlist, $ref__binary_RIB );

if ( $__DEBUG__ & 0x01 ) {
	print STDERR '</PointsGeneralPolygons>' . $/;
}
	return 1;
}


sub	getParameterList
{
	my	( $parameterlist, $ref__binary ) = @_;
	my	$re__Parameter .= '\s*"(.+?)"\s+\[\s*(.+?)\s*\]\s*';

#	print STDERR '...parameterlist... = \'' . $parameterlist . '\'' . $/;
	my	$Name;
	my	$Values;
	my	@ValueList;
	while ( ( $Name, $Values ) = ( $parameterlist =~ /^$re__Parameter(.*)$/s ) ) {
		@ValueList = split( /\s+/, $Values );
		${ $ref__binary } .= refString( $Name );
		${ $ref__binary } .= binArray( 'FLOAT', \@ValueList );
if ($__DEBUG__ & 0x02) {
		print STDERR 'Name = \'' . $Name . '\'' . $/;
		print STDERR '# of value(s) = ' . ( 1 + $#ValueList ) . $/;
		@ValueList = grep {$_ = '\'' . $_ . '\''} @ValueList;
		print STDERR 'Value(s):' . $/
			. "\t" . join( $/ . "\t", @ValueList ) . $/;
#		print STDERR 'Value(s) = [\'' . $Values . '\']' . $/;
}

		$parameterlist = $3;
	}

	return;
}


sub	refString
{
	my	( $s ) = @_;

	my	$x_token = $g__stringHash{ $s };
	if ( defined( $x_token ) ) {
		return $x_token;
	}

	# define string
	++$g__stringMax;
	if ( $g__stringMax >= 256 ) {
		print STDERR 'binString: no more token space' . $/;
		exit 1;
	}
	my	$token;
	if ( $g__stringMax < 16 ) {
		$x_token = pack( 'cc', 0317 + 0, $g__stringMax );
		$token = pack( 'cc', 0315 + 0, $g__stringMax );
	} else {
		$x_token = pack( 'c', 0317 + 1 ) . pack( 'n', $g__stringMax );
		$token = pack( 'c', 0315 + 1 ) . pack( 'n', $g__stringMax );
	}
	$g__stringHash{ $s } = $x_token;

	return $token . binString( $s ) . $x_token;
}

sub	refRIRequest
{
	my	( $RI ) = @_;

	my	$x_token = $g__RIRequestHash{ $RI };
	return $x_token if ( $x_token ne '' );

	# define string
	++$g__RIRequestMax;
	if ( $g__RIRequestMax >= 256 ) {
		print STDERR 'binRIRequest: no more token space' . $/;
		exit 1;
	}
	$x_token = pack( 'cc', 0246, $g__RIRequestMax );
	$g__RIRequestHash{ $RI } = $x_token;
	return pack( 'cc', 0314, $g__RIRequestMax ) . binString( $RI ) . $x_token;
}


sub binArray
{
	my	( $m_TYPE, $ref_LIST ) = @_;

	unless ( $m_TYPE =~ /^(INT|FLOAT|STRING)$/ ) {
		print STDERR 'binArray: !(INT|FLOAT|STRING): ' . $m_TYPE . $/;
		return undef;
	}

	my	$binary;
	my	@x;

	# $m_TYPE = 'FLOAT'
	unless ( $m_TYPE ne 'FLOAT' ) {
		my	$n = $#{ $ref_LIST } + 1;
		if ( $n >= 65536 ) {
			print STDERR 'binArray: array too large: # of elements >= 65536' . $/;
			return undef;
		}

		# 256 is more compact than 128, but ......
		# (See 0x00000080)
		$binary = $n >= 128	# 256 is more compact than 128, but ......
			? pack( 'c', 0311 ) . pack( 'n', $n )
			: pack( 'cc', 0310, $n );
		foreach my $f ( @{ $ref_LIST } ) {
			@x = split( '', pack( 'f', $f ) );
			$binary .= $x[ 3 ] . $x[ 2 ] . $x[ 1 ] . $x[ 0 ];
		}
	}

	# $m_TYPE = 'INT'
	unless ( $m_TYPE ne 'INT' ) {
		$binary = '[';
		foreach my $i ( @{ $ref_LIST } ) {
			if ( abs( $i ) >= 8388608 ) {
				print STDERR 'binArray: abs( ' . $i . ' ) >= 8399608' . $/;
				return undef;
			}
			$binary .= binInt( $i );
		}
		$binary .= ']';
	}

	# $m_TYPE = 'STRING'
	unless ( $m_TYPE ne 'STRING' ) {
		$binary = '[';
		foreach my $s ( @{ $ref_LIST } ) {
			$s =~/^"(.+?)"$/;	# trim double quote(")
			$binary .= refString( $1 );
		}
		$binary .= ']';
	}

	return $binary;
}

#	0x80000001	2147483649	244 317 000 000 000
#	0x80000000	2147483648	244 317 000 000 000
#	0x7FFFFFFF	2147483647	244 117 000 000 000
#	0x01000000	16777216	244 113 200 000 000
#	0x00FFFFFF	16777215	244 113 177 377 377
#	0x00800000	8388608		244 113 000 000 000	!
#	0x007FFFFF	8388607		202 177 377 377
#	0x00010000	65536		202 001 000 000
#	0x0000FFFF	65535		202 000 377 377
#	0x00008000	32768		202 000 200 000		!
#	0x00007FFF	32767		201 177 377
#	0x00000100	256		201 001 000
#	0x000000FF	255		201 000 377
#	0x00000080	128		201 000 200		!
#	0x0000007F	127		200 177
#	0x00000001	1		200 001
#	0x00000000	0		244 000 000 000 000	!
#	0xFFFFFFFF	-1		200 377
#	0xFFFFFF81	-127		200 201
#	0xFFFFFF80	-128		201 377 200		!
#	0xFFFFFF01	-255		201 377 001
#	0xFFFFFF00	-256		201 377 000
#	0xFFFF8001	-32767		201 200 001
#	0xFFFF8000	-32768		202 377 200 000		!
#	0xFFFF0001	-65535		202 377 000 001
#	0xFFFF0000	-65536		202 377 000 000
#	0xFF800001	-8388607	202 200 000 001
#	0xFF800000	-8388608	244 313 000 000 000	!
#	0xFF000001	-16777215	244 313 177 377 377
#	0xFF000000	-16777216	244 313 200 000 000
#	0xFEFFFFFF	-16777217	244 313 200 000 000
#	0xF0000001	-268435454	244 315 200 000 000
#	0xF0000000	-268435455	244 315 200 000 000
#	0x80000001	-2147483647	244 317 000 000 000
#	0x80000000	-2147483648	244 317 000 000 000
#
#
#
#	0x00800000	8388608		244 113 000 000 000	!
#	0x007FFFFF	8388607		202 177 377 377
#	0x00008000	32768		202 000 200 000		!
#	0x00007FFF	32767		201 177 377
#	0x00000080	128		201 000 200		!
#	0x0000007F	127		200 177
#	0x00000001	1		200 001
#	0x00000000	0		244 000 000 000 000	!
#	0xFFFFFFFF	-1		200 377
#	0xFFFFFF81	-127		200 201
#	0xFFFFFF80	-128		201 377 200		!
#	0xFFFF8001	-32767		201 200 001
#	0xFFFF8000	-32768		202 377 200 000		!
#	0xFF800001	-8388607	202 200 000 001
#	0xFF800000	-8388608	244 313 000 000 000	!
sub	binInt
{
	my	( $i ) = @_;

	my	$w;
	if ( abs( $i ) >= 32768 ) {	# 0x00008000
		$w = 3;
	} elsif ( abs( $i ) >= 128 ) {	# 0x00000080
		$w = 2;
	} else {
		$w = 1;
	}
	my	@x = split( '', pack( 'N', $i ) );
	splice( @x, 0, 4 - $w );

	return pack( 'c', 0200 + ( $w - 1 ) ) . join( '', @x );
}

sub	binFloat
{
	my	( $f ) = @_;

	my	@x = split( '', pack( 'f', $f ) );
	return pack( 'c', 0244 ) . $x[ 3 ] . $x[ 2 ] . $x[ 1 ] . $x[ 0 ];
}

sub	binString
{
	my	( $s ) = @_;

	my	$len = length( $s );
	return pack( 'c', 0220 + $len ) . $s if ( $len < 16 );

	# $len >= 16
	my	$w = $len >= 32768 ? 3 : $len >= 128 ? 2 : 1;
	my	@x = split( '', pack( 'N', $len ) );
	splice( @x, 0, 4 - $w );
	return pack( 'c', 0240 + ( $w - 1 ) ) . join( '', @x ) . $s;
}

if ( 0 ) {
@x = (
#	8388608,	#		244 113 000 000 000	!
	8388607,	#		202 177 377 377
	32768,		#		202 000 200 000		!
	32767,		#		201 177 377
	128,		#		201 000 200		!
	127,		#		200 177
	1,		#		200 001
	0,		#		244 000 000 000 000	!
	-1,		#		200 377
	-127,		#		200 201
	-128,		#		201 377 200		!
	-32767,		#		201 200 001
	-32768,		#		202 377 200 000		!
	-8388607,	#		202 200 000 001
#	-8388608,	#		244 313 000 000 000	!
);
#@x = grep {
#	$_ = '\'' . $_ . '\'';
#} @x;
print binArray( 'INT', \@x ) . $/;
}
#!/bin/perl -w

$__DEBUG__ = 0x00;

# Polygons
#	Polygon
#	GeneralPolygon
#	PointsPolygons
#	PointsGeneralPolygons
# Patches
#	Basis
#	Patch
#	PatchMesh
#	NuPatch
#	TrimCurve
# Subdivision Surfaces
#	SubdivisionMesh
# Quadrics
#	Sphere
#	Cone
#	Cylinder
#	Hyperboloid
#	Paraboloid
#	Disk
#	Torus
# Point and Curve Primitives
#	Points
#	Curves
# Blobby Implicit Surfaces
#	Blobby
# Procedual Primitives
#	Procedual
#	ProcDelayedReadArchive
#	ProcRunProgram
#	ProcDynamicLoad
# Implementation-specific Geometric Primitives
#	Geometry
# Solid and Spatial Set Operations
#	SolidBegin, SolidEnd
# Retained Geometry
#	ObjectBegin, ObjectEnd
#	ObjectInstance
