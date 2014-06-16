#!/usr/bin/perl

$__DEBUG__ = 0x02;

	@g__MERB_LIST = ();
	%g__DISPLAY_TEXTURE = (
		'shd'	=>	'rmantex/shd',
		'env1'	=>	'rmantex/env',
		'env2'	=>	'rmantex/env',
		'env3'	=>	'rmantex/env',
		'env4'	=>	'rmantex/env',
		'env5'	=>	'rmantex/env',
		'env6'	=>	'rmantex/env',
		'ref'	=>	'rmantex/env',
	);
	@g__LIGHT_LIST = ();

# 1.01
#	1. object -> frame:object
#	2. Illuminate %LIGHT_ID# 0

	unless ( $#ARGV >= 0 ) {
		print STDERR <<USAGE;
Usage:
	$0 RibFile [-e[xpunge] Display:Object ...] [-r[ecover] Display:Object ...] [-m[atte] Display:Object ...] [-b[lind] Display:Object ...]
USAGE
		exit 1;
	}

	ProcessCommandArgument();
if ( $__DEBUG__ & 0x02 ) {
	foreach ( @g__MERB_LIST ) {
		print STDERR <<EOR;
[ @{ [ join( ', ', @{ $_ } ) ] } ]
EOR
	}
}

	unless ( $ARGV[0] ne '-' ) {
		$g_STDIN = 1;
	} else {
		$g_STDIN = 0;
		open( RIB, '< ' . $ARGV[ 0 ] ) || die( 'Cannot open ' . $ARGV[ 0 ] . $/ );
	}
	if ( ProcessRib( RIB ) != 0 ) {
		print STDERR 'ProcessRib Error...' . $/;
		exit 1;
	}
#	if ( ModifyAttribute( RIB ) != 0 ) {
#		print STDERR 'ModifyAttribute Error...' . $/;
#		exit 1;
#	}
	close( RIB ) unless ( $ARGV[ 0 ] ne '-' );
	exit 0;

sub ProcessCommandArgument
{
	my	$merb;
	my	$x;	# display:object
	for ( my $i = 1; $i <= $#ARGV; ++$i ) {
		unless ( $ARGV[ $i ] =~ /^-([merbx])/ ) {
			print STDERR <<WARN;
Warn: unknown option: $ARGV[ $i ]: ignored!
WARN
			next;
		}
		my $merb = $1;
		# get an object name
		if ( $i == $#ARGV ) {
			print STDERR <<WARN;
Warn: no object name for '$ARGV[ $i ]' option: ignored!
WARN
		} else {
			$x = $ARGV[ ++$i ];
			$x =~ s/([?+*])/\.$1/g;
			$display_ID = '.*';
			$object_ID = $x;
			if ( $x =~ /^([^:]+):([^:]+)$/ ) {
				$display_ID = $1;
				$object_ID = $2;
			}
			unless ( $merb ne 'b' ) {
				push( @g__MERB_LIST, [ '-', $object_ID, 'b', $display_ID ] );
				push( @g__MERB_LIST, [ $display_ID, $object_ID, 'e', '' ] );
			} else {
				push( @g__MERB_LIST, [ $display_ID, $object_ID, $merb, '' ] );
			}
		}
	}
}

sub ProcessRib
{
#
	unless ( $g_STDIN ) {
		open( OLD_STDIN, '<& STDIN' );
		open( STDIN, '<& ' . $_[ 0 ] );
	}
#

#
#	Display name type mode ...parameterlist...
#
#	MakeTexture picturename texturename swrap twrap filter swidth twidth ...parameterlist...
#	MakeLatLongEnvironment picturename texturename filter swidth twidth ...parameterlist...
#	MakeCubeFaceEnvironment px nx py ny pz nz texturename fov filter swidth twidth ...parameterlist...
#	MakeShadow picturename texturename ...parameterlist...
#
	my	$FRAME_BEGIN = '^\s*FrameBegin\s*(\d+)\s*$';
	my	$FRAME_END = '^\s*FrameEnd\s*$';

	my	$DISPLAY = '^\s*Display\s*"([^+]+?)"\s*"(.+?)"\s*"(.+?)"\s*(.*)\s*$';
	my	$display_name = '';
	my	$display_ID = '';
	my	$display_TEX = '';
	my	$display_type = '';
	my	$display_mode = '';

	my	$head = '';
	my	$in_head = 1;		# 0: no, 1: yes
	my	$frame_content = '';
	my	$in_frame = 0;		# 0: no, 1: yes
	my	$interim = '';		# between frames (FrameEnd .. FrameBegin)

	my	$ATTRIBUTE_BEGIN = '^(\s*)AttributeBegin\s*$';
	my	$ATTRIBUTE_IDENTIFIER = '^\s*Attribute\s+"identifier"\s+".+?"\s+\["(.+?)"\]\s*$'; 
	my	$ATTRIBUTE_END = '^(\s*)AttributeEnd\s*$';

	my	$attribute_content = '';
	my	$attribute_identifier = '';
	my	$attribute_level = '';
	my	$in_attribute = 0;	# 0: no, 1: yes

	my	$LIGHT_SHADER = '^\s*LightSource\s+".+?"\s+(.+?)\s+';
	my	$LIGHT_IDENTIFIER = '';
	while ( <STDIN> ) {
		if ( /$FRAME_BEGIN/ ) {
			# assert( $in_frame == 0 )
			$frame_content = $_;
			$in_frame = 1;
			if ( $in_head == 1 ) {
				print $head;
				# end of $head
				$in_head = 0;
			} elsif ( $interim ) {
				print $interim;
if ( $__DEBUG__ & 0x01 ) {
	print STDERR <<INTERIM;
@{ [ '-' x 72 ] }
$interim@{ [ '-' x 72 ] }
INTERIM
}
				$interim = '';
			}
		} elsif ( /$FRAME_END/ ) {
			# assert( $in_frame == 1 )
			# assert( $display_name != '' )
			$frame_content .= $_;

			# print $frame_content
#	preview.keyShape.shd.0001.tex
#	preview.0001
if ( $__DEBUG__ & 0x08 ) {
	print STDERR <<EOT
@{ [ '#' x 72 ] }
name = '$display_name'
ID = '$display_ID'
TEX = '$display_TEX'
type = '$display_type'
mode = '$display_mode'
@{ [ '#' x 72 ] }
EOT
}
			print $frame_content;

			# reset variables
			$display_name = '';
			$display_ID = '';
			$display_TEX = '';
			$display_type = '';
			$display_mode = '';
			$frame_content = '';
			$in_frame = 0;
		} else {
			if ( $in_head == 1 ) {
				$head .= $_;
			} else {
				if ( $in_frame == 1 ) {
					if ( /$DISPLAY/ ) {
						# assert( $display_name == '' )
						$display_name = $1;
						$display_type = $2;
						$display_mode = $3;
if ( $__DEBUG__ & 0x04 ) {
	print STDERR <<DISPLAY;
@{ [ '+' x 72 ] }
${_}name = '$display_name'
type = '$display_type'
mode = '$display_mode'
@{ [ '+' x 72 ] }
DISPLAY
}
						my      @LIST = split( '\.', $display_name );
if ( $__DEBUG__ & 0x04 ) {
	print STDERR $LIST[ 0 ] . $/;
	print STDERR $LIST[ 1 ] . $/;
	print STDERR $LIST[ 2 ] . $/;
	print STDERR $LIST[ 3 ] . $/;
}
						if ( defined( $LIST[ 2 ] ) && exists( $g__DISPLAY_TEXTURE{ $LIST[ 2 ] } ) ) {
							$display_ID = $LIST[ 1 ];
							$display_TEX = $LIST[ 2 ];
						} else {
							$display_ID = '-';
							$display_TEX = '-';
						}

						$frame_content .= $_;
					} else {
if (1) {
# AttributeBegin {...} AttributeEnd
		if ( /$ATTRIBUTE_BEGIN/ ) {
			if ( $in_attribute ) {
				# assert( $attribute_level < $1 )
				# level down
				$attribute_content .= $_;
			} else {
				$in_attribute = 1;
				$attribute_level = $1;
				$attribute_content = $_;
			}
		} elsif ( /$ATTRIBUTE_IDENTIFIER/ ) {
			# assert( $attribute_identifier == '' );
			if ( $in_attribute ) {
				$attribute_content .= $_;
				$attribute_identifier = $1;
			} else {
				# light (TransformBegin .. TransformEnd)
				unless ( $display_ID ne '-' ) {
					# assert( $LIGHT_IDENTIFIER = '' )
					$LIGHT_IDENTIFIER = $1
				}
				$frame_content .= $_;
			}
		} elsif ( /$ATTRIBUTE_END/ ) {
			# assert( $in_attribute == 1 )
			$attribute_content .= $_;
			unless ( $attribute_level ne $1 ) {
				if ( $attribute_identifier ne '' ) {
					#
					# do something!
					#
					do_something(
						$display_ID,
						$attribute_identifier,
						\$attribute_content
					);
				}
				$frame_content .= $attribute_content;

				# reset variables
				$attribute_content = '';
				$attribute_identifier = '';
				$attribute_level = '';
				$in_attribute = 0;	# 0: no, 1: yes
			} else {
				# assert( $attribute_level < $1 )
				# level up
			}
		} else {
			if ( $in_attribute ) {
				$attribute_content .= $_;
			} else {
				# LightSource ... with $light_attribute_identifier
				if ( $display_ID eq '-' && /$LIGHT_SHADER/ ) {
					# assert( $LIGHT_IDENTIFIER != '' )
					push( @g__LIGHT_LIST, [ $LIGHT_IDENTIFIER, $1 ] );
					$LIGHT_IDENTIFIER = '';
				}
				$frame_content .= $_;
			}
		}
# AttributeBegin {...} AttributeEnd
} else {
				$frame_content .= $_;
}
					}
				} else {
					$interim .= $_;
				}
			}
		}
	}
	# assert( $in_head == 0 )
if ( $__DEBUG__ & 0x01 ) {
	print STDERR <<HEAD;
@{ [ '=' x 72 ] }
$head@{ [ '=' x 72 ] }
HEAD
}

#
	open( STDIN, '<& OLD_STDIN' ) unless ( $g_STDIN );
#
	return 0;
}

sub do_something
{
	my	( $display_ID, $attribute_ID, $ref__ATTRIBUTE_CONTENT) = @_;
	my	@indices;
	isMatched( \@indices, $display_ID, $attribute_ID );
	if ( $#indices == -1 ) {
		# do nothing with $ref__ATTRIBUTE_CONTENT
		return;
	}

	my	$ix = $indices[ 0 ];
	if ( $g__MERB_LIST[ $ix ][ 2 ] eq 'm' ) {	# Matte
#
#	AttributeBegin
#		Attribute "identifier" ....
#		Matte 1
#
		${ $ref__ATTRIBUTE_CONTENT } =~ s/(\n(\s*)Attribute "identifier" .*?\n)/$1$2Matte 1\n/s;
	} elsif ( $g__MERB_LIST[ $ix ][ 2 ] eq 'b' ) {	# blind
		unless ( $display_ID ne '-' ) {
			foreach $ix ( @indices ) {
				${ $ref__ATTRIBUTE_CONTENT } =~ s/(\n(\s*)Attribute "identifier" .*?\n)/ref_LIGHT( $1, $2, $g__MERB_LIST[ $ix ][ 3 ] )/e;
			}
		} else {
			# do nothing with $ref__ATTRIBUTE_CONTENT
		}
	} elsif ( $g__MERB_LIST[ $ix ][ 2 ] eq 'e' ) {	# expunge
		${ $ref__ATTRIBUTE_CONTENT } = '';
	} elsif ( $g__MERB_LIST[ $ix ][ 2 ] eq 'r' ) {	# recover
		# do nothing with $ref__ATTRIBUTE_CONTENT
	}

	return;
}

sub isMatched
{
	my	( $ref_indices, $display_ID, $attribute_ID ) = @_;
	my	@LIST = split( '\|', $attribute_ID );
	shift @LIST unless ( $LIST[ 0 ] ne '' );

	my	$i;
	my	$gi;
	for ( $gi = $#g__MERB_LIST; $gi >= 0; --$gi ) {
		for ( $i = $#LIST; $i >= 0; --$i ) {
			# 06/25/2002
			$LIST[ $i ] =~ s/_.*//g;
			$display_ID =~ /^$g__MERB_LIST[ $gi ][ 0 ]$/
				&&
			$LIST[ $i ] =~ /^$g__MERB_LIST[ $gi ][ 1 ]$/
				&&
			push( @{ $ref_indices }, $gi );
#			$display_ID =~ /^$g__MERB_LIST[ $gi ][ 0 ]$/
#				&&
#			$LIST[ $i ] =~ /^$g__MERB_LIST[ $gi ][ 1 ]$/
#				&&
#			return $gi;
		}
	}

#	return undef;
}

sub	ref_LIGHT
{
	my	( $ATTRIBUTE, $TAB, $LIGHT ) = @_;
	$LIGHT =~ s/_.*$//;
	my	@LIST;
	my	@m_ILLUMINATEs = ();
	for ( my $i = 0; $i <= $#g__LIGHT_LIST; ++$i ) {
		@LIST = split( '\|', $g__LIGHT_LIST[ $i ][ 0 ] );
		shift @LIST unless ( $LIST[ 0 ] ne '' );
		for ( my $ii = $#LIST; $ii >= 0; --$ii ) {
			if ( $LIST[ $ii ] =~ /^$LIGHT$/ ) {
				push( @m_ILLUMINATEs, $TAB . 'Illuminate ' . $g__LIGHT_LIST[ $i ][ 1 ] . ' 0' . $/ );
#				return $ATTRIBUTE
#					. $TAB . '#' x 72 . $/
#					. $TAB . 'Illuminate ' . $g__LIGHT_LIST[ $i ][ 1 ] . ' 0' . $/
#					. $TAB . '#' x 72 . $/;
			}
		}
	}
	if ( $#m_ILLUMINATEs == -1 ) {
		print STDERR 'ERROR: %' . $LIGHT . '#: no matching object(s)' . $/;
		exit 1;
	}

	return $ATTRIBUTE
		. $TAB . '#' x 72 . $/
		. join( '', @m_ILLUMINATEs )
		. $TAB . '#' x 72 . $/;
}
