#!/bin/perl
$g__X = "amp_0";
$g__METHOD = '-';
$g__HOW_MUCH = 1.0;

$regex = "\"\\S+\\s+$g__X\"";
$regex = '"(\s*)(\S+)(\s+)' . $g__X . '(\s*)"(\s+)\[(\s*)(.+?)(\s*)\]';
print $regex . $/;

while (<>) {
	next if /^\s*#/;

	print $_ if s/$regex/"\"$1$2$3$g__X$4\"$5\[$6" . xxx( $2, $7 ) . "$8\]"/e;
#	print $_ if s/$regex/xxx( $7 )/e;
}

sub xxx
{
	# check type = $_[ 0 ]

	return $_[ 1 ] . ' -> ' . ($_[ 1 ] + $g__HOW_MUCH) unless ( $g__METHOD ne '+' );
	return $_[ 1 ] . ' -> ' . ($_[ 1 ] - $g__HOW_MUCH) unless ( $g__METHOD ne '-' );
	return "hello, world: type: $_[ 0 ], value = '$_[ 1 ]'";
}
