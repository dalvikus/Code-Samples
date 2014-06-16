#!/bin/perl -w

$__DEBUG__ = 0x00;

@g__LIST = ();	# list of array (param, how, much, #|%, # of HITs, # of ERRORs)
#push( @g__LIST, [ 'amp_\d', '-', '11', '%', 0, 0 ] );

$is_CONTINUE = 1;
for ( $i = 0; $i <= $#ARGV; ++$i ) {
	unless ( $ARGV[ $i ] ne '-D' && $ARGV[ $i ] ne '--debug' ) {
		if ( $i == $#ARGV ) {
			$__DEBUG__ = 0;
			print STDERR '-D, --debug: no value: ignored' . $/;
			next;
		} else {
			$x = $ARGV[ $i + 1 ] + 0;
			if ( $x =~ /^\d+$/ ) {
				$__DEBUG__ = $x;
				++$i;
				next;
			} else {
				print STDERR '-D, --debug: $ARGV[ $i + 1 ]: invalid value: ignored' . $/;
			}
		}
	}

	unless ( scanArgs( $ARGV[ $i ] ) ) {
		print STDERR "$_: invalid format!\n";
		$is_CONTINUE = 0;
	}
}
exit 1 unless $is_CONTINUE;
if ( $__DEBUG__ & 0x01 ) {
	foreach ( @g__LIST ) {
		print STDERR <<EOT
+@{ [ '-' x 70 ] }+
|@{ [ ' ' x 70 ] }|
	PARAM = '@{ [ $_ -> [ 0 ] ] }'
	HOW = '@{ [ $_ -> [ 1 ] ] }'
	MUCH = '@{ [ $_ -> [ 2 ] ] }'
	#|% = '@{ [ $_ -> [ 3 ] ] }'
|@{ [ ' ' x 70 ] }|
+@{ [ '-' x 70 ] }+
EOT
	}
}

$g__regex = '"(\s*)(\S+)(\s+)(\S+)(\s*)"(\s+)\[(\s*)(.+?)(\s*)\]';
$g__LINENO = 0;
while ( <STDIN> ) {
	++$g__LINENO;
	if ( /^\s*#/ ) {
		print;
		next;
	}

	s/$g__regex/"\"$1$2$3$4$5\"$6\[$7" . modify( $g__LINENO, $2, $4, $8 ) . "$9\]"/e;
	print;
}
if ( $__DEBUG__ & 0x01 ) {
	foreach ( @g__LIST ) {
		print STDERR <<EOT
+@{ [ '-' x 70 ] }+
|@{ [ ' ' x 70 ] }|
	PARAM = '@{ [ $_ -> [ 0 ] ] }'
	HOW = '@{ [ $_ -> [ 1 ] ] }'
	MUCH = '@{ [ $_ -> [ 2 ] ] }'
	#|% = '@{ [ $_ -> [ 3 ] ] }'
	# of HITs = '@{ [ $_ -> [ 4 ] ] }'
	# of ERRORs = '@{ [ $_ -> [ 5 ] ] }'
|@{ [ ' ' x 70 ] }|
+@{ [ '-' x 70 ] }+
EOT
	}
}

sub scanArgs
{
	my	( $args ) = @_;
	my	$m_regex = '^(\S+)\s*([+|-])\s*(\d*\.?\d*)\s*(%?)\s*$';
	my	$AUX;

	if ( $args =~ /$m_regex/ && $3 ne '' && $3 ne '.' ) {
		$AUX = $4;
		$AUX = '#' unless $AUX;
		push ( @g__LIST, [ $1, $2, $3, $AUX, 0, 0 ] );

		return 1;
	}
	# INVALID
	return 0;
}

sub modify
{
	my	( $m_LINENO, $m_TYPE, $m_PARAM, $m_VALUE ) = @_;
	my	$index;
	my	$HOW;
	my	$MUCH;

	$index = isMatched( $m_PARAM );
	return $m_VALUE unless defined( $index );
	$HOW = $g__LIST[ $index ][ 1 ];
	$MUCH = $g__LIST[ $index ][ 2 ];
	$AUX = $g__LIST[ $index ][ 3 ];

	# check type = $_[ 0 ]
	# only for a number
	unless ( $m_VALUE =~ /^\-?(\d*)\.?(\d*)$/ ) {
		print STDERR 'LINE #: ' . $m_LINENO . $/;
		print STDERR "\t\"$m_TYPE $m_PARAM\" [ $m_VALUE ]\n";
		print STDERR "\t$m_VALUE" . '(/^\-?(\d*)\.?(\d*)$/)' . ': NOT a number: NOT modified' . $/;
		++$g__LIST[ $index ][ 5 ];
		return $m_VALUE;
	}
	++$g__LIST[ $index ][ 4 ];

	my	$x = abs( $MUCH );
	unless ( $HOW ne '+' ) {
if ( $__DEBUG__ & 0x02 ) {
		return $m_VALUE . ' ---(+' . $MUCH . '#)-> ' . ( $m_VALUE + $MUCH ) unless ( $AUX ne '#' );
} else {
		return $m_VALUE + $MUCH unless ( $AUX ne '#' );
}
		# AUX = '%'
if ( $__DEBUG__ & 0x02 ) {
		return $m_VALUE . ' ---(+' . $MUCH . '%)-> ' . ( $m_VALUE + $m_VALUE / 100. * $MUCH );
} else {
		return $m_VALUE + $m_VALUE / 100. * $MUCH;
}
	}
	# $HOW = '-'
if ( $__DEBUG__ & 0x02 ) {
	return $m_VALUE . ' ---(-' . $MUCH . '#)-> ' . ( $m_VALUE - $MUCH ) unless ( $AUX ne '#' );
} else {
	return $m_VALUE - $MUCH unless ( $AUX ne '#' );
}
	# AUX = '%'
if ( $__DEBUG__ & 0x02 ) {
	return $m_VALUE . ' ---(-' . $MUCH . '%)-> ' . ( $m_VALUE - $m_VALUE / 100. * $MUCH );
} else {
	return $m_VALUE - $m_VALUE / 100. * $MUCH;
}
}

sub isMatched
{
	my	$m_PARAM = $_[ 0 ];
	my	$i;
	for ($i = $#g__LIST; $i >= 0; --$i) {
		$m_PARAM =~ /^$g__LIST[ $i ][ 0 ]$/ && return $i;
	}

	return undef;
}
