#!/bin/perl -w

$__DEBUG__ = 0x00;

# Is it unique?
$g__TMP_FILE = 'RS232Cx.binary.RIB';
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
%HASH = (
	'SubdivisionMesh'	=>	'',
	'PointsGeneralPolygons'	=>	'',
	'NuPatch'		=>	'',
);

$ascii = '';
$binary = '';
while ( $x = <> ) {
	if ( ( $word ) = ( $x =~ /^\s*([A-Z][[:alpha:]]+)/ ) ) {
		# if $ascii exists, pack it with 'catrib'
		if ( $ascii ne '' ) {
#			print $ascii;
			packing( \$ascii, \$binary );	
if ( $__DEBUG__ & 0x01 ) {
			$L_ascii = length( $ascii );
			$L_binary = length( $binary );
			$f_diff = -( $L_ascii - $L_binary ) / $L_ascii * 100;
			print STDERR "\t" . $L_ascii . ':' . $L_binary . ' [ ' . sprintf( "%.1f", $f_diff ) . '% ]' . $/;
}
			# append '\n'
			print '#' x 72 . $/ . $binary . $/ . '#' x 72 . $/;
			# reset
			$ascii = '';
		}

		unless ( exists( $HASH{ $word } ) ) {
			print $x;
		} else {
			$ascii = $x;
if ( $__DEBUG__ & 0x01 ) {
			print STDERR $word . ':' . $/;
}
		}
	} else {
		unless ( $ascii ne '' ) {
			print $x;
		} else {
			$ascii .= $x;
		}
	}
}
# remove temporary file
unlink $g__TMP_FILE;

sub	packing
{
	my	( $ascii_ref, $binary_ref ) = @_;

	# save binary RIB using temporary file
	open ( OUT, '| catrib -binary > ' . $g__TMP_FILE ) || die( 'OUT error' );
	print OUT ${ $ascii_ref };
	close ( OUT );

	# read it
	open ( IN, '< ' . $g__TMP_FILE ) || die( 'IN error' );
	${ $binary_ref } = '';
	while ( $m_x = <IN> ) {
		${ $binary_ref } .= $m_x;
	}
	close ( IN );
}
