#!/usr/local/tcltk-8.3.4/bin/wish83.exe

########################################################################
set VERSION 1.02
########################################################################
set ALFRED alfred
set SHAKE shake
regsub -all -- {\\} $::env(HOME) {/} HOME
########################################################################
option add *font {"Comic Sans MS" 16} userDefault
option add *font {"Arial Narrow" 16} userDefault
proc printf {msg} {
	tk_dialog .dialog {message} $msg {} 0 OK
}

if {1 == 0} {
	wm iconify .
	set w .ruler
	catch {destroy $w}
	toplevel $w
	wm title $w "Ez Comp."
	wm iconname $w "ezcomp"
	focus -force $w
	raise $w
} else {
	wm iconify .
	set w {}
}
################################################################################
namespace eval ezcomp {
#### CONSTANTs
	variable __lineColor grey40

	variable __Ux 10
	variable __Uy 10
	variable __Py 1
	variable __W  1
	variable __min_N 1
	# coordinate for the GRID
	variable __Lx 40
	variable __Ly 60
	variable __dy 0
	# __Nx will be set from g__f_START and g__f_END
	variable __Nx 0
	variable __Ny 10

	# gc__*
	variable ROTO yellow
	variable HERO blue
	variable GROUND_SHADOW red
	variable BACKGROUND green
#### VARIABLEs
	variable g__ID {}
	# default frame range (1 - 180)
	variable g__f_range {}
	variable g__f_START 1
	variable g__f_END 180
	variable g__fo {}

	# default directory for GUESS
	variable g__default_directory .

	# temporary variables for each layer
	variable m__directory {}
	variable m__filename {}
	variable m__ID {}
	# these two variables(m__f_START, m__f_END) are redundant,
	# but make life easy!
	variable m__f_START {}
	variable m__f_END {}
	variable m__f_range {}
	variable m__type BACKGROUND
	variable m__N {}
	variable m__i {}

	namespace eval layers {
	}

	proc return__gi {w} {
		set w .gi
		set x [string trim $::ezcomp::g__ID]
		if {$x == ""} {
			printf "g__ID SHOULD be given!"
			focus -force $w.f.uID.entry
			$w.f.uID.entry icursor end
			return
		}
		# AND ......

		set x [string trim $::ezcomp::g__f_range]
		if {$x == ""} {
			printf "g__f_range SHOULD be given!"
			focus -force $w.f.f_range.entry
			$w.f.f_range.entry icursor end
			return
		}
		if ![regexp -- {^\s*(\d+)\s*-\s*(\d+)\s*$} $x void ::ezcomp::g__f_START ::ezcomp::g__f_END] {
			printf "g__f_range(\"$::ezcomp::g__f_range\") is NOT valid!"
			focus -force $w.f.f_range.entry
			$w.f.f_range.entry icursor end
			return
		}
		set ::ezcomp::g__f_range "$::ezcomp::g__f_START - $::ezcomp::g__f_END"

		set x [string trim $::ezcomp::g__fo]
		if {$x == ""} {
			printf "g__fo SHOULD be given!"
			focus -force $w.f.fo.entry
			$w.f.fo.entry icursor end
			return
		}
		if ![regexp -- {^\s*\S+\.(#+)\.(\S+)\s*$} $x void digits extension] {
			printf "g__fo(\"$::ezcomp::g__fo\") is NOT valid!"
			focus -force $w.f.fo.entry
			$w.f.fo.entry icursor end
			return
		}

		set ::x 1
	}

	proc init {w} {
		set w .$w
		catch {destroy $w}
		toplevel $w

	#	<HTML: GI>
		frame $w.f -borderwidth 0 -relief solid
	#	<BODY: ID>
		frame $w.f.uID -borderwidth 0 -relief solid
		label $w.f.uID.label \
			-anchor e \
			-text "g__ID: " \
			\
			-width 16
		entry $w.f.uID.entry \
			-foreground #0066CC \
			-highlightcolor #CC0066 \
			-highlightthickness 1 \
			-relief groove \
			-textvariable ::ezcomp::g__ID \
			\
			-width 12
		pack $w.f.uID.label -side left
		pack $w.f.uID.entry -side left
		pack $w.f.uID -fill x -padx 10 -pady 2 -side top
		bind $w.f.uID.entry <Return> \
			"
				printf \"what's up\"
			"
	#	</BODY: ID>
	#	<BODY: FRAME RANGE>
		frame $w.f.f_range -borderwidth 0 -relief solid
		label $w.f.f_range.label \
			-anchor e \
			-text "g__f_range: " \
			\
			-width 16
		entry $w.f.f_range.entry \
			-font {"Lucida Console" 14} \
			-foreground #0066CC \
			-highlightcolor #CC0066 \
			-highlightthickness 1 \
			-relief groove \
			-textvariable ::ezcomp::g__f_range \
			\
			-width 11
		pack $w.f.f_range.label -side left
		pack $w.f.f_range.entry -side left
		pack $w.f.f_range -fill x -padx 10 -pady 2 -side top
		bind $w.f.f_range.entry <Return> \
			"
				printf \"what's up\"
			"
	#	</BODY: FRAME RANGE>
	#	<BODY: OUTPUT>
		frame $w.f.fo -borderwidth 0 -relief solid
		label $w.f.fo.label \
			-anchor e \
			-text "g__fo: " \
			\
			-width 16
		entry $w.f.fo.entry \
			-foreground #0066CC \
			-highlightcolor #CC0066 \
			-highlightthickness 1 \
			-relief groove \
			-textvariable ::ezcomp::g__fo \
			\
			-width 60
		pack $w.f.fo.label -side left
		pack $w.f.fo.entry -side left
		pack $w.f.fo -fill x -padx 10 -pady 2 -side top
		bind $w.f.fo.entry <Return> \
			"
				printf \"what's up\"
			"
	#	</BODY: OUTPUT>
	#	<BODY: ACTION>
		frame $w.f.buttons -borderwidth 0 -relief solid
		button $w.f.buttons.oK \
			-anchor center \
			-font {"Arial Narrow" 16} \
			-relief groove \
			-text OK \
			\
			-command {
				::ezcomp::return__gi $w
			} \
			-width 8
		button $w.f.buttons.cANCEL \
			-anchor center \
			-font {"Arial Narrow" 16} \
			-relief groove \
			-text CANCEL \
			\
			-command {
				set ::x 0
			} \
			-width 8
		pack $w.f.buttons.oK -fill x -padx 10 -pady 10 -side left
		pack $w.f.buttons.cANCEL -fill x -padx 10 -pady 10 -side left
		pack $w.f.buttons -padx 10 -pady 10 -side top
	#	</BODY: ACTION>
		pack $w.f -fill x -padx 10 -pady 10
	#	</HTML: GI>

		bell
		focus -force $w.f.uID.entry
		grab $w
		tkwait variable ::x
		destroy $w
		set x $::x
		unset ::x
		return $x
	}

	proc load_LAYERs {w} {
		set m_ID $::ezcomp::m__ID
		set m_directory $::ezcomp::m__directory
		set m_filename $::ezcomp::m__filename
		set m_f_START $::ezcomp::m__f_START
		set m_f_END $::ezcomp::m__f_END
		set m_f_range $::ezcomp::m__f_range

		set m_ARRAYs {}
		foreach x [info vars ::ezcomp::layers::*] {
			if [array exists $x] {
				lappend m_ARRAYs $x
			}
		}
		foreach x [lsort -command compare $m_ARRAYs] {
			upvar $x x_ref
			set ::ezcomp::m__ID $x_ref(ID)
			set ::ezcomp::m__directory $x_ref(directory)
			set ::ezcomp::m__filename $x_ref(filename)
			set ::ezcomp::m__f_START $x_ref(f_START)
			set ::ezcomp::m__f_END $x_ref(f_END)
			set ::ezcomp::m__f_range "$x_ref(f_START) - $x_ref(f_END)"
			set ::ezcomp::m__type $x_ref(type)
			::ezcomp::append_LAYER 1 {}
		}

		set ::ezcomp::m__ID $m_ID
		set ::ezcomp::m__directory $m_directory
		set ::ezcomp::m__filename $m_filename
		set ::ezcomp::m__f_START $m_f_START
		set ::ezcomp::m__f_END $m_f_END
		set ::ezcomp::m__f_range $m_f_range
	}

	proc createPlane {canvas} {
		set ::ezcomp::__Nx \
			[expr \
				$::ezcomp::g__f_END - \
				$::ezcomp::g__f_START + \
				1 \
			]

		set max_x \
			[expr \
				$::ezcomp::__Lx + \
				$::ezcomp::__Nx * $::ezcomp::__Ux \
			]
		set max_y \
			[expr \
				$::ezcomp::__Ly + \
				$::ezcomp::__Ny * ( \
					$::ezcomp::__Py + \
					$::ezcomp::__Uy + \
					$::ezcomp::__W + \
					1 + \
					$::ezcomp::__Py \
				) \
			]
		set y_LOW [expr $::ezcomp::__Ly - 10]
		$canvas create line \
			[expr $::ezcomp::__Lx] $y_LOW $max_x $y_LOW \
			-fill $::ezcomp::__lineColor
		$canvas configure \
			-scrollregion \
				"0 0 [expr $max_x + 50] [expr $max_y + 50]"

		# make line mark
		for {set i 0} {$i <= $::ezcomp::__Nx} {incr i} {
			set n_FRAME [expr $::ezcomp::g__f_START + $i]
			set w 1
			set h 5
			if {[expr $n_FRAME % 5] == 0} {
				set w 2
				set h 10
			}
			if {[expr $n_FRAME % 10] == 0} {
				set w 2
				set h 15
			}
			set x [expr $::ezcomp::__Lx + $i * $::ezcomp::__Ux]
			set y_HIGH [expr $::ezcomp::__Ly - 10 - $h]
			$canvas create line \
				$x $y_HIGH $x $y_LOW \
				-fill $::ezcomp::__lineColor \
				-width $w
			if {[expr $n_FRAME % 5] == 0} {
				$canvas create text \
					[expr $x + 2] [expr $y_LOW - 10] \
					-anchor sw \
					-text $n_FRAME
			}
		}

		# horizontal line
		for {set i 0} {$i <= $::ezcomp::__Nx} {incr i} {
			$canvas create line \
				[expr $::ezcomp::__Lx + $i * $::ezcomp::__Ux] \
				$::ezcomp::__Ly \
				[expr $::ezcomp::__Lx + $i * $::ezcomp::__Ux] \
				$max_y \
				-fill $::ezcomp::__lineColor
		}

		# vertical line
		set ::ezcomp::__dy [expr \
			1 + \
			$::ezcomp::__Py + $::ezcomp::__Uy + \
			$::ezcomp::__W + \
			$::ezcomp::__Py \
		]
		set Dy $::ezcomp::__dy
		for {set i 0} {$i < $::ezcomp::__Ny} {incr i} {
			$canvas create rectangle \
				$::ezcomp::__Lx \
				[expr $::ezcomp::__Ly + $i * $Dy] \
				[expr \
					$::ezcomp::__Lx + \
					$::ezcomp::__Nx * $::ezcomp::__Ux \
				] \
				[expr \
					$::ezcomp::__Ly + \
					$i * $Dy + $::ezcomp::__Py \
				] \
				-fill $::ezcomp::__lineColor -outline $::ezcomp::__lineColor
			$canvas create rectangle \
				$::ezcomp::__Lx \
				[expr \
					$::ezcomp::__Ly + \
					$i * $Dy + \
					$::ezcomp::__Py + \
					$::ezcomp::__Uy + \
					$::ezcomp::__W + \
					1 \
				] \
				[expr \
					$::ezcomp::__Lx + \
					$::ezcomp::__Nx * $::ezcomp::__Ux \
				] \
				[expr \
					$::ezcomp::__Ly + \
					$i * $Dy + \
					$::ezcomp::__Py + \
					$::ezcomp::__Uy + \
					$::ezcomp::__W + \
					1 + \
					$::ezcomp::__Py \
				] \
				-fill $::ezcomp::__lineColor -outline $::ezcomp::__lineColor
		}

		# clear all layers
		foreach var [info vars ::ezcomp::layers::*] {
#			array unset $var
		}
		::ezcomp::load_LAYERs $w
	}

	# TYPE with '_' instead of blank(' ') if any, e.g., "GROUND_SHADOW"
	# return WIDGET to be focused
	proc createSlider {w ID TYPE x_ID} {
		set canvas $w.canvas

		if {$x_ID == ""} {
			set ROW [::ezcomp::nexti]
		} else {
			upvar ::ezcomp::layers::$x_ID x_ref
			set ROW $x_ref(index)
		}
		if {$ROW == -1} {
			printf "no available layer..."
			return $w.display.body.uID.entry
		}
		set COL [expr $::ezcomp::m__f_START - $::ezcomp::g__f_START]
		set N $::ezcomp::m__N
		upvar ::ezcomp::$TYPE x_ref
		set COLOR $x_ref

		if {$x_ID != ""} {
			$canvas delete box__$x_ID
		}
		set y_HIGH [expr $::ezcomp::__Ly + $ROW * ($::ezcomp::__Py + $::ezcomp::__Uy + $::ezcomp::__W + 1 + $::ezcomp::__Py) + $::ezcomp::__Py + 1]
		set y_LOW [expr $::ezcomp::__Ly + $ROW * ($::ezcomp::__Py + $::ezcomp::__Uy + $::ezcomp::__W + 1 + $::ezcomp::__Py) + $::ezcomp::__Uy + $::ezcomp::__Py + 1]
		$canvas create rectangle \
			[expr $::ezcomp::__Lx + [expr $COL - 1] * $::ezcomp::__Ux] \
			$y_HIGH \
			[expr $::ezcomp::__Lx + $COL * $::ezcomp::__Ux] \
			$y_LOW \
			-fill black \
			-outline #000000 \
			-width $::ezcomp::__W \
			-tag [subst {box__$ID box_L__$ID}]
		$canvas create rectangle \
			[expr $::ezcomp::__Lx + $COL * $::ezcomp::__Ux] \
			$y_HIGH \
			[expr $::ezcomp::__Lx + ($COL + $N) * $::ezcomp::__Ux] \
			$y_LOW \
			-fill $COLOR \
			-outline #000000 \
			-width $::ezcomp::__W \
			-tag [subst {box__$ID box_C__$ID}]
		$canvas create rectangle \
			[expr $::ezcomp::__Lx + ($COL + $N) * $::ezcomp::__Ux] \
			$y_HIGH \
			[expr $::ezcomp::__Lx + ($COL + $N + 1) * $::ezcomp::__Ux] \
			$y_LOW \
			-fill black \
			-outline #000000 \
			-width $::ezcomp::__W \
			-tag [subst {box__$ID box_R__$ID}]

	#	$canvas bind box__$ID <Enter> \
	#		"$canvas itemconfig current -fill red"
	#	$canvas bind box__$ID <Leave> \
	#		"$canvas itemconfig current -fill $COLOR"
		$canvas bind box__$ID <1> \
			"plotDown {} $ID %x %y"
		$canvas bind box_C__$ID <B1-Motion> \
			"C_plotMove {} $ID %x %y"
		$canvas bind box_L__$ID <B1-Motion> \
			"L_plotMove {} $ID %x %y"
		$canvas bind box_R__$ID <B1-Motion> \
			"R_plotMove {} $ID %x %y"

		upvar ::ezcomp::$TYPE x_ref
		$w.display.body.uID.type configure -background $x_ref

		if {$x_ID != ""} {
			array unset ::ezcomp::layers::$x_ID
		}
		# create array named $NAME in namespace ::ezcomp::layers
		array set ::ezcomp::layers::$ID {}
		upvar ::ezcomp::layers::$ID a_ref
		set a_ref(ID) $ID
		# static
		set a_ref(index) $ROW

		# for external use (static except for f_START and f_END)
		# copy from display information (::ezcomp::m__*)
		set a_ref(directory) $::ezcomp::m__directory
		set a_ref(filename) $::ezcomp::m__filename
		# dynamic
		set a_ref(f_START) $::ezcomp::m__f_START
		set a_ref(f_END) $::ezcomp::m__f_END
		set a_ref(type) $::ezcomp::m__type

		return $w.display.body.directory.entry
	}

	proc vcmd__directory {} {
		# with ::ezcomp::m__directory
		set x [string trim $::ezcomp::m__directory]

		if {$x == ""} {
			printf "\$::ezcomp::m__directory SHOULD be given!"
			return 0
		}
		# change into nativename
		set x [file nativename $x]
		# replace '\' into '/' as UNIX path
		regsub -all -- {\\} $x {/} x
		if {[string range $x 0 1] != "//"} {
			printf "NETWORK PATH(begining with \"//\") SHOULD be given!"
			return 0
		}
		if {[string range $x end end] != "/"} {
			append x /
		}

		set ::ezcomp::m__directory $x

		return 1
	}

	proc vcmd__filename {} {
		# with ::ezcomp::m__filename
		set x [string trim $::ezcomp::m__filename]

		if {$x == ""} {
			printf "\$::ezcomp::m__filename SHOULD be given!"
			return 0
		}
		if [regexp -- {^\s*(\S+)\.([#]+)\.(\S+)\s*$} $x null image_NAME HASHes image_EXT] {
			set x $image_NAME.$HASHes.$image_EXT
		} else {
			set x [string trim $x]
		}

		set ::ezcomp::m__filename $x

		return 1
	}

	proc vcmd__f_range {w} {
		# get the frame range in the ENTRY widget
		# with ::ezcomp::m__f_range
		set x [string trim $::ezcomp::m__f_range]

		if {$x == ""} {
			printf "\$::ezcomp::m__f_range SHOULD be given!"
			return 0
		}

		# is it VALID?
		if ![regexp -- {(\d+)\s*-\s*(\d+)\s*} $x void m__f_START m__f_END] {
			printf "\$::ezcomp::m__f_range: $::ezcomp::m__f_range is NOT valid"
			return 0
		}
		if {$m__f_START > $m__f_END} {
			printf "\$::ezcomp::m__f_range: $m__f_START <= $m__f_END (?)"
			return 0
		}
		if {$m__f_START < $::ezcomp::g__f_START || $m__f_END > $::ezcomp::g__f_END} {
			printf "\$::ezcomp::g__f_range: $::ezcomp::g__f_START - $::ezcomp::g__f_END, \$::ezcomp::m__f_range: $m__f_START - $m__f_END (?)"
			return 0
		}
		set ::ezcomp::m__f_START $m__f_START
		set ::ezcomp::m__f_END $m__f_END
		set ::ezcomp::m__f_range "$m__f_START - $m__f_END"

		set ::ezcomp::m__N [expr $m__f_END - $m__f_START + 1]
		set ::ezcomp::m__i 0

		return 1

		set canvas $w.canvas
		set ID $::ezcomp::m__ID
		upvar ::ezcomp::layers::$ID a_ref
		# update LAYER information
		set ::ezcomp::m__N [expr $a_ref(f_END) - $a_ref(f_START) + 1]
		set ::ezcomp::m__i 0
		set a_ref(f_START) $m__f_START
		set a_ref(f_END) $m__f_END

		# zero-based "relative" number
		incr m__f_START -$::ezcomp::g__f_START
		incr m__f_END -$::ezcomp::g__f_START

		# get the frame range in the SLIDE bar
		set RECT [$canvas coords box_C__$ID]
		set f_START [expr round(([lindex $RECT 0] - $::ezcomp::__Lx) / $::ezcomp::__Ux)]
		set f_END [expr round(([lindex $RECT 2] - $::ezcomp::__Lx) / $::ezcomp::__Ux) - 1]

		# move LEFT side
		set dx [expr $::ezcomp::__Ux * ($m__f_START - $f_START)]
		$canvas move box_L__$ID $dx 0
		incr ::ezcomp::m__N [expr -($m__f_START - $f_START)]
		set RECT [lreplace $RECT 0 0 [expr [lindex $RECT 0] + $dx]]
		$canvas coords box_C__$ID $RECT

		# move RIGHT side
		set dx [expr $::ezcomp::__Ux * ($m__f_END - $f_END)]
		$canvas move box_R__$ID $dx 0
		incr ::ezcomp::m__N [expr $m__f_END - $f_END]
		set RECT [lreplace $RECT 2 2 [expr [lindex $RECT 2] + $dx]]
		$canvas coords box_C__$ID $RECT

		return 1
	}

	proc return__f_range {w} {
		# get the frame range in the ENTRY widget
		if ![regexp -- {(\d+)\s*-\s*(\d+)\s*} $::ezcomp::m__f_range x m__f_START m__f_END] {
			printf "\$::ezcomp::m__f_range: $::ezcomp::m__f_range is NOT valid"
			return 0
		}
		# is it VALID?
		if {$m__f_START > $m__f_END} {
			printf "\$::ezcomp::m__f_range: $m__f_START <= $m__f_END (?)"
			return 0
		}
		# is it VALID?
		if {$m__f_START < $::ezcomp::g__f_START || $m__f_END > $::ezcomp::g__f_END} {
			printf "\$::ezcomp::g__f_range: $::ezcomp::g__f_START - $::ezcomp::g__f_END, \$::ezcomp::m__f_range: $m__f_START - $m__f_END (?)"
			return 0
		}
		# fit it! ("#### - ####")
		set ::ezcomp::m__f_START $m__f_START
		set ::ezcomp::m__f_END $m__f_END
		set ::ezcomp::m__f_range "$m__f_START - $m__f_END"

		set canvas $w.canvas
		set ID $::ezcomp::m__ID
		upvar ::ezcomp::layers::$ID a_ref
		# update LAYER information
		set ::ezcomp::m__N [expr $a_ref(f_END) - $a_ref(f_START) + 1]
		set ::ezcomp::m__i 0
		set a_ref(f_START) $m__f_START
		set a_ref(f_END) $m__f_END

		# zero-based "relative" number
		incr m__f_START -$::ezcomp::g__f_START
		incr m__f_END -$::ezcomp::g__f_START

		# get the frame range in the SLIDE bar
		set RECT [$canvas coords box_C__$ID]
		set f_START [expr round(([lindex $RECT 0] - $::ezcomp::__Lx) / $::ezcomp::__Ux)]
		set f_END [expr round(([lindex $RECT 2] - $::ezcomp::__Lx) / $::ezcomp::__Ux) - 1]

		# move LEFT side
		set dx [expr $::ezcomp::__Ux * ($m__f_START - $f_START)]
		$canvas move box_L__$ID $dx 0
		incr ::ezcomp::m__N [expr -($m__f_START - $f_START)]
		set RECT [lreplace $RECT 0 0 [expr [lindex $RECT 0] + $dx]]
		$canvas coords box_C__$ID $RECT

		# move RIGHT side
		set dx [expr $::ezcomp::__Ux * ($m__f_END - $f_END)]
		$canvas move box_R__$ID $dx 0
		incr ::ezcomp::m__N [expr $m__f_END - $f_END]
		set RECT [lreplace $RECT 2 2 [expr [lindex $RECT 2] + $dx]]
		$canvas coords box_C__$ID $RECT

		return 1
	}

	proc validation {w} {
		if {[vcmd__directory] == 0} {
			return $w.display.body.directory.entry
		}

		if {[vcmd__filename] == 0} {
			return $w.display.body.filename.entry
		}

		if {[vcmd__f_range $w] == 0} {
			return $w.display.body.f_range.entry
		}

		return ""
		upvar $xID a_ref

		# directory
		if {$a_ref(directory) == ""} {
			printf "LAYER $a_ref(index): directory SHOULD be given!"
			return ""
		}

		# filename
		set x $a_ref(filename)
		if {$x == ""} {
			printf "LAYER $a_ref(index): filename SHOULD be given!"
			return ""
		}
		# make image FORMAT
		if [regexp -- {^\s*(\S+)\.([#]+)\.(\S+)\s*$} $x void image_NAME HASHes image_EXT] {
			set n_DIGITs [string length $HASHes]
			set x "$image_NAME.\[format %0[string length $HASHes]d \$x__fi\].$image_EXT"
		}
		return $x
	}

	proc _validation {xID} {
		upvar $xID a_ref

		# directory
		if {$a_ref(directory) == ""} {
			printf "LAYER $a_ref(index): directory SHOULD be given!"
			return ""
		}

		# filename
		set x $a_ref(filename)
		if {$x == ""} {
			printf "LAYER $a_ref(index): filename SHOULD be given!"
			return ""
		}
		# make image FORMAT
		if [regexp -- {^\s*(\S+)\.([#]+)\.(\S+)\s*$} $x void image_NAME HASHes image_EXT] {
			set n_DIGITs [string length $HASHes]
			set x "$image_NAME.\[format %0[string length $HASHes]d \$x__fi\].$image_EXT"
		}
		return $x
	}

	proc comp {x y} {
		upvar ::ezcomp::layers::[lindex $x 0] x_ref
		upvar ::ezcomp::layers::[lindex $y 0] y_ref

		if {$x_ref(index) > $y_ref(index)} {
			return 1
		} elseif {$x_ref(index) == $y_ref(index)} {
			return 0
		} else {
			return -1
		}
	}

	proc nexti {} {
		# get index list
		set indices {}
		foreach name [info vars ::ezcomp::layers::*] {
			if {[array exists $name] == 0} {
				continue
			}
			upvar $name a_ref
			lappend indices $a_ref(index)
		}

		# look up available index
		for {set i 0} {$i < $::ezcomp::__Ny} {incr i} {
			if {[lsearch -exact $indices $i] == -1} {
				return $i
			}
		}

		return -1
	}

	proc find_ID {w index} {
		foreach name [info vars ::ezcomp::layers::*] {
			if {[array exists $name] == 0} {
				continue
			}
			upvar $name a_ref
			if {$a_ref(index) == $index} {
				# return "bare" ID name
				return [lindex [split $name ::] end]
			}
		}

		return ""
	}

	proc append_LAYER {i_FORCE w} {
		set ID $::ezcomp::m__ID

		if {$ID == ""} {
			printf "ID SHOULD be given!"
			focus -force $w.display.body.uID.entry
			return
		}
		# is it unique?
		set x_ID {}
		if {[array exists ::ezcomp::layers::$ID] == 1} {
			if {$i_FORCE == 0 && [tk_dialog .dialog "Overwrite?" "Overwrite $ID?" "" 0 "Ok" "Cancel"]} {
				printf "ID($ID) is NOT unique!"
				focus -force $w.display.body.uID.entry
				return
			}
			set x_ID $ID
		}

		set x [::ezcomp::validation $w]
		if {$x != ""} {
			focus -force $x
			return
		}

		set x $::ezcomp::m__type
		# substitute all blank(' ')s with '_'
		regsub -all -- { } $x _ x

		# append NEW slider
		set widget [::ezcomp::createSlider $w $ID $x $x_ID]
		focus -force $w.display.body.uID.entry
	}

	proc expunge {w} {
		set ID [string trim $::ezcomp::m__ID]
		if {$ID == ""} {
			return
		}

		if ![info exists ::ezcomp::layers::$ID] {
			set x [focus]
			printf "ID($ID) does NOT exist!"
			focus -force $x
			return
		}

		# delete items in CANVAS
		set canvas $w.canvas
		$canvas delete box__$ID
		# unset the associated variable
		unset ::ezcomp::layers::$ID

		return
	}

	proc up {w} {
		set ID [string trim $::ezcomp::m__ID]
		if {$ID == ""} {
			return
		}

		if ![info exists ::ezcomp::layers::$ID] {
			set x [focus]
			printf "ID($ID) does NOT exist!"
			focus -force $x
			return
		}

		# get the index
		upvar ::ezcomp::layers::$ID a_ref
		set index $a_ref(index)
		if {$index == 0} {
			return
		}

		set canvas $w.canvas
		# find ID just above
		set x_ID [::ezcomp::find_ID $w [expr $index - 1]]
		$canvas move box__$ID 0 -$::ezcomp::__dy
		# decrease index
		incr a_ref(index) -1
		if {$x_ID != ""} {
			$canvas move box__$x_ID 0 $::ezcomp::__dy
			upvar ::ezcomp::layers::$x_ID a_ref
			incr a_ref(index) 1
		}
	}

	proc down {w} {
		set ID [string trim $::ezcomp::m__ID]
		if {$ID == ""} {
			return
		}

		if ![info exists ::ezcomp::layers::$ID] {
			set x [focus]
			printf "ID($ID) does NOT exist!"
			focus -force $x
			return
		}

		# get the index
		upvar ::ezcomp::layers::$ID a_ref
		set index $a_ref(index)
		if {[expr $index + 1] == $::ezcomp::__Ny} {
			return
		}

		set canvas $w.canvas
		# find ID just above
		set x_ID [::ezcomp::find_ID $w [expr $index + 1]]
		$canvas move box__$ID 0 $::ezcomp::__dy
		# increase index
		incr a_ref(index)
		if {$x_ID != ""} {
			$canvas move box__$x_ID 0 -$::ezcomp::__dy
			upvar ::ezcomp::layers::$x_ID a_ref
			incr a_ref(index) -1
		}
	}
}
########################################################################

########################################################################
proc draw_WINDOW {w} {
	set c $w.canvas

########################################################################
####                                                                ####
	proc drawCrew {w} {
		set crew $w.crew
		frame $crew
		pack $crew -fill x -padx 10 -pady 10
		label $crew.l \
			-anchor e \
			-font {Arial 14} \
			-foreground #0066CC \
			-text "Partition: " \
			\
			-width 16
		pack $crew.l -padx 4 -side left
		entry $crew.e \
			-background #C0C0C0 \
			-foreground #CC0066 \
			-textvariable ::vars::m__crews \
			\
			-width 12
		pack $crew.e -side left
	}
#	drawCrew $w
####                                                                ####
########################################################################

# <HTML>
	frame $w.display -borderwidth 1 -relief solid
# <HEAD>
	frame $w.display.head -borderwidth 1 -relief solid
	button $w.display.head.button \
		-anchor center \
		-font {"Arial Narrow" 18} \
		-relief groove \
		-text close \
		\
		-command {
			set mode [$w.display.head.button cget -text]
			if {$mode == "open"} {
				$w.display.head.button configure -text close
				pack configure $w.display.body -after $w.display.head -padx 1 -pady 1 -anchor w -fill x -side top
			} elseif {$mode == "close"} {
				$w.display.head.button configure -text open
				pack forget $w.display.body
			}
		} \
		-width 8
	button $w.display.head.info \
		-anchor center \
		-font {"Arial Narrow" 18} \
		-relief flat \
		-text Information \
		\
		-command {
			set x [focus]
			printf "ID: $::ezcomp::g__ID\nframes: $::ezcomp::g__f_START - $::ezcomp::g__f_END\nfile out: $::ezcomp::g__fo"
			focus -force $x
			unset x
		}
	pack $w.display.head.button -side left
	pack $w.display.head.info -expand yes -fill x -side left
	pack $w.display.head -padx 1 -pady 1 -anchor w -fill x -side top
# </HEAD>
# <BODY>
	frame $w.display.body -borderwidth 1 -relief solid
#	<BODY: guess>
	frame $w.display.body.guess
	pack $w.display.body.guess -fill x
	button $w.display.body.guess.button \
		-anchor center \
		-foreground #CC0066 \
		-font {"Comic Sans MS" 16} \
		-relief groove \
		-text "guess" \
		\
		-command {guess_FILENAME $w}
	pack $w.display.body.guess.button -anchor center -expand yes -ipadx 10 -side left
#	</BODY: guess>
#	<BODY: ID>
	frame $w.display.body.uID
	label $w.display.body.uID.label \
		-anchor e \
		-text "ID: " \
		\
		-width 16
	entry $w.display.body.uID.entry \
		-foreground #0066CC \
		-highlightcolor #CC0066 \
		-highlightthickness 1 \
		-relief groove \
		-textvariable ::ezcomp::m__ID \
		\
		-width 12
	proc menu_SELECT {widget} {
		set menu [tk_optionMenu $widget \
				::ezcomp::m__type \
				ROTO HERO {GROUND SHADOW} BACKGROUND \
		]
		for {set i 0} {$i <= [$menu index last]} {incr i} {
			# substitute all blank(' ')s with '_'
			regsub -all -- { } [$menu entrycget $i -label] _ x
			upvar #0 ::ezcomp::$x x_ref
	#		$menu entryconfigure $i -activebackground $x_ref
			$menu entryconfigure $i -font {"Arial Narrow" 16}
	#		$menu entryconfigure $i -foreground $x_ref
			$menu entryconfigure $i -hidemargin 0
	#		$menu entryconfigure $i -selectcolor $x_ref
			$menu entryconfigure $i -command \
				"
					$widget configure -activebackground $x_ref
					$widget configure -background $x_ref
				"
		}
	#	$menu configure -background white
		set x $::ezcomp::m__type
		regsub -all -- { } $x _ x
		upvar #0 ::ezcomp::$x x_ref
		$widget configure -activebackground $x_ref
		$widget configure -background $x_ref
		$widget configure -borderwidth 1
		$widget configure -relief solid
		$widget configure -font {"Arial Narrow" 16}
		$widget configure -width 16
	}
	menu_SELECT $w.display.body.uID.type
	pack $w.display.body.uID.label -side left
	pack $w.display.body.uID.entry -side left
	pack $w.display.body.uID.type -side left
	pack $w.display.body.uID -fill x -side top
#	</BODY: ID>
#	<BODY: DIRECTORY>
	frame $w.display.body.directory
	label $w.display.body.directory.label \
		-anchor e \
		-text "directory: " \
		\
		-width 16
	entry $w.display.body.directory.entry \
		-foreground #0066CC \
		-highlightcolor #CC0066 \
		-highlightthickness 1 \
		-relief groove \
		-textvariable ::ezcomp::m__directory \
		\
		-width 32
	pack $w.display.body.directory.label -side left
	pack $w.display.body.directory.entry -side left
	pack $w.display.body.directory -fill x -side top
#	</BODY: DIRECTORY>
#	<BODY: FILENAME>
	frame $w.display.body.filename
	label $w.display.body.filename.label \
		-anchor e \
		-text "filename: " \
		\
		-width 16
	entry $w.display.body.filename.entry \
		-foreground #0066CC \
		-highlightcolor #CC0066 \
		-highlightthickness 1 \
		-relief groove \
		-textvariable ::ezcomp::m__filename \
		\
		-width 24
	pack $w.display.body.filename.label -side left
	pack $w.display.body.filename.entry -side left
	pack $w.display.body.filename -fill x -side top
#	</BODY: FILENAME>
#	<BODY: F_RANGE>
	frame $w.display.body.f_range
	label $w.display.body.f_range.label \
		-anchor e \
		-text "frame range: " \
		\
		-width 16
	entry $w.display.body.f_range.entry \
		-foreground #0066CC \
		-highlightcolor #CC0066 \
		-highlightthickness 1 \
		-relief groove \
		-textvariable ::ezcomp::m__f_range \
		\
		-width 11
	pack $w.display.body.f_range.label -side left
	pack $w.display.body.f_range.entry -side left
	pack $w.display.body.f_range -fill x -side top
	bind $w.display.body.f_range.entry <Return> \
		{ \
			::ezcomp::return__f_range $w; \
			focus -force $w.display.body.f_range.entry
		}
#	</BODY: F_RANGE>
#	<BODY: ACTION>
	frame $w.display.body.action
	button $w.display.body.action.append \
		-anchor center \
		-font {"Arial Narrow" 16} \
		-relief groove \
		-text "append" \
		\
		-command \
			{::ezcomp::append_LAYER 0 $w} \
		-width 8
	button $w.display.body.action.expunge \
		-anchor center \
		-font {"Arial Narrow" 16} \
		-relief groove \
		-text expunge \
		\
		-command {
			::ezcomp::expunge $w
		} \
		-width 8
	button $w.display.body.action.up \
		-anchor center \
		-font {"Arial Narrow" 16} \
		-relief groove \
		-text up \
		\
		-command {
			::ezcomp::up $w
		} \
		-width 8
	button $w.display.body.action.down \
		-anchor center \
		-font {"Arial Narrow" 16} \
		-relief groove \
		-text down \
		\
		-command {
			::ezcomp::down $w
		} \
		-width 8
	pack $w.display.body.action.up -expand yes -padx 10 -pady 10 -side left
	pack $w.display.body.action.append -expand no -padx 10 -pady 10 -side left
	pack $w.display.body.action.expunge -expand no -padx 10 -pady 10 -side left
	pack $w.display.body.action.down -expand yes -padx 10 -pady 10 -side left
	pack $w.display.body.action -fill x -side top
#	</BODY: ACTION>
	pack $w.display.body -padx 1 -pady 1 -anchor w -fill x -side top
# </BODY>
	pack $w.display -padx 20 -pady 20 -anchor w -fill x
# </HTML>
########################################################################

	frame $w.grid -borderwidth 1 -relief solid
	scrollbar $w.hscroll -borderwidth 1 -relief solid -orient horiz -command "$c xview"
	scrollbar $w.vscroll -command "$c yview"
	canvas $c -borderwidth 1 -relief solid -width 14.8c -height 7.5c \
		-scrollregion {0 0 300c 10c} \
		-xscrollcommand "$w.hscroll set" \
		-yscrollcommand "$w.vscroll set"
	pack $w.grid -expand yes -fill both -ipadx 10 -ipady 10 -padx 20 -pady 20
	grid rowconfig    $w.grid 0 -weight 1 -minsize 0
	grid columnconfig $w.grid 0 -weight 1 -minsize 0

	grid $c -padx 10 -in $w.grid -pady 10 \
    	-row 0 -column 0 -rowspan 1 -columnspan 1 -sticky news
	grid $w.vscroll -in $w.grid -padx 10 -pady 1 \
    	-row 0 -column 1 -rowspan 1 -columnspan 10 -sticky news
	grid $w.hscroll -in $w.grid -padx 10 -pady 10 \
    	-row 1 -column 0 -rowspan 1 -columnspan 1 -sticky news


	frame $w.buttons -borderwidth 1 -relief solid
	pack $w.buttons -side bottom -fill x -pady 2m
	button $w.buttons.dismiss -text EXiT -command "destroy ."
	button $w.buttons.code -text Run... -command "xprintf vprintf print_ALL"
	pack $w.buttons.dismiss $w.buttons.code -side left -expand 1
}


# plotDown --
# This procedure is invoked when the mouse is pressed over one of the
# data points.  It sets up state to allow the point to be dragged.
#
# Arguments:
# w -		The canvas window.
# x, y -	The coordinates of the mouse press.

proc plotDown {w ID x y} {
	set canvas $w.canvas

	upvar ::ezcomp::layers::$ID a_ref
	# internal variable
	upvar ::ezcomp::m__i i_ref

	$canvas dtag active

	# display slider information
	set ::ezcomp::m__directory $a_ref(directory)
	set ::ezcomp::m__filename $a_ref(filename)
	set ::ezcomp::m__ID $ID
	set ::ezcomp::m__N [expr $a_ref(f_END) - $a_ref(f_START) + 1]
	set f_START [expr $::ezcomp::g__f_START + round(([lindex [$canvas coords box_C__$ID] 0] - $::ezcomp::__Lx) / $::ezcomp::__Ux)]
	set ::ezcomp::m__f_START $f_START
	set ::ezcomp::m__f_END [expr $f_START + $::ezcomp::m__N - 1]
	set ::ezcomp::m__f_range "$::ezcomp::m__f_START - $::ezcomp::m__f_END"
	set type $a_ref(type)
	# substitute all ' 's with '_'
	regsub -all -- { } $type _ type
	upvar ::ezcomp::$type x_ref
	$w.display.body.uID.type configure -background $x_ref
	set ::ezcomp::m__type $a_ref(type)

	set i_ref [expr int(floor(([$canvas canvasx $x] - $::ezcomp::__Lx) / $::ezcomp::__Ux))]
	$canvas addtag active withtag current
	$canvas raise active
}

# update the following variables
#	a_ref(N)
#	a_ref(_i)
#	a_ref(f_range)
#	::ezcomp::m__f_START
#	::ezcomp::m__f_END
#	::ezcomp::m__f_range
proc L_plotMove {w ID x y} {
	set canvas $w.canvas

	upvar ::ezcomp::layers::$ID a_ref
	upvar ::ezcomp::m__i i_ref

	set i_ [expr int(floor(([$canvas canvasx $x] - $::ezcomp::__Lx) / $::ezcomp::__Ux))]
	if {$i_ < 0} {
		set i_ -1
	}
	set RECT [$canvas coords box_C__$ID]
	set iR [expr round(([lindex $RECT 2] - $::ezcomp::__Lx) / $::ezcomp::__Ux) - $::ezcomp::__min_N]
	if {$i_ >= $iR} {
		set i_ [expr $iR - 1]
	}
	set dx [expr $::ezcomp::__Ux * ($i_ - $i_ref)]
	$canvas move box_L__$ID $dx 0
	incr ::ezcomp::m__N [expr -($i_ - $i_ref)]
	set RECT [lreplace $RECT 0 0 [expr [lindex $RECT 0] + $dx]]
	$canvas coords box_C__$ID $RECT

	# update slider information (f_range)
	set f_START [expr $::ezcomp::g__f_START + $i_ + 1]
	set ::ezcomp::m__f_START $f_START
	set ::ezcomp::m__f_END [expr $f_START + $::ezcomp::m__N - 1]
	set ::ezcomp::m__f_range "$::ezcomp::m__f_START - $::ezcomp::m__f_END"
	set a_ref(f_START) $f_START
	set a_ref(f_END) [expr $f_START + $::ezcomp::m__N - 1]

	set i_ref $i_
}

# update the following variables
#	a_ref(_i)
#	a_ref(f_range)
#	::ezcomp::m__f_START
#	::ezcomp::m__f_END
#	::ezcomp::m__f_range
proc C_plotMove {w ID x y} {
	set canvas $w.canvas

	upvar ::ezcomp::layers::$ID a_ref
	upvar ::ezcomp::m__i i_ref

	set i_ [expr int(floor(([$canvas canvasx $x] - $::ezcomp::__Lx) / $::ezcomp::__Ux))]
	set RECT [$canvas coords box_C__$ID]

	set n [expr $i_ref - round(([lindex $RECT 0] - $::ezcomp::__Lx) / $::ezcomp::__Ux)]
	if {[expr $i_ - $n] < 0} {
		set i_ $n
	}

	set n [expr round(([lindex $RECT 2] - $::ezcomp::__Lx) / $::ezcomp::__Ux) - $i_ref]
	if {[expr $i_ + $n] > $::ezcomp::__Nx} {
		set i_ [expr $::ezcomp::__Nx - $n]
	}
	$canvas move box__$ID [expr $::ezcomp::__Ux * ($i_ - $i_ref)] 0

	# update slider information (f_range)
	set RECT [$canvas coords box_C__$ID]
	set f_START [expr $::ezcomp::g__f_START + round(([lindex $RECT 0] - $::ezcomp::__Lx) / $::ezcomp::__Ux)]
	set ::ezcomp::m__f_START $f_START
	set ::ezcomp::m__f_END [expr $f_START + $::ezcomp::m__N - 1]
	set ::ezcomp::m__f_range "$::ezcomp::m__f_START - $::ezcomp::m__f_END"
	set a_ref(f_START) $f_START
	set a_ref(f_END) [expr $f_START + $::ezcomp::m__N - 1]

	set i_ref $i_
}

# update the following variables
#	a_ref(N)
#	a_ref(_i)
#	a_ref(f_range)
#	::ezcomp::m__f_START
#	::ezcomp::m__f_END
#	::ezcomp::m__f_range
proc R_plotMove {w ID x y} {
	set canvas $w.canvas

	upvar ::ezcomp::layers::$ID a_ref
	upvar ::ezcomp::m__i i_ref

	set i_ [expr int(floor(([$canvas canvasx $x] - $::ezcomp::__Lx) / $::ezcomp::__Ux))]
	if {$i_ >= $::ezcomp::__Nx} {
		set i_ $::ezcomp::__Nx
	}

	set RECT [$canvas coords box_C__$ID]
	set iL [expr round(([lindex $RECT 0] - $::ezcomp::__Lx) / $::ezcomp::__Ux) + $::ezcomp::__min_N]
	if {$i_ < $iL} {
		set i_ $iL
	}

	set dx [expr $::ezcomp::__Ux * ($i_ - $i_ref)]
	$canvas move box_R__$ID $dx 0

	incr ::ezcomp::m__N [expr $i_ - $i_ref]
	set RECT [lreplace $RECT 2 2 [expr [lindex $RECT 2] + $dx]]
	$canvas coords box_C__$ID $RECT

	# update slider information (f_range)
	set f_END $i_
	set ::ezcomp::m__f_START [expr $::ezcomp::g__f_START + $f_END - $::ezcomp::m__N]
	set ::ezcomp::m__f_END [expr $::ezcomp::g__f_START + $f_END - 1]
	set ::ezcomp::m__f_range "$::ezcomp::m__f_START - $::ezcomp::m__f_END"
	set a_ref(f_START) [expr $::ezcomp::g__f_START + $f_END - $::ezcomp::m__N]
	set a_ref(f_END) [expr $::ezcomp::g__f_START + $f_END - 1]

	set i_ref $i_
}
################################################################################

################################################################################
proc xprintf {w proc} {
	set font {"Lucida Console" 14}
	if ![winfo exists .$w] {
		toplevel .$w
		frame .$w.buttons
		pack .$w.buttons -side bottom -fill x
		button .$w.buttons.dismiss \
			-font $font \
			-text Dismiss \
			-relief groove \
			-command "destroy .$w"
		button .$w.buttons.erase \
			-font $font \
			-relief groove \
			-text "Erase" \
			-command ".$w.text delete 1.0 end"
		pack .$w.buttons.dismiss .$w.buttons.erase -side left \
			-expand 1 -pady 2
		#
		frame .$w.frame
		pack .$w.frame -expand yes -fill both -padx 1 -pady 1
		text .$w.text \
			-background #FFFFFF \
			-foreground #0066CC \
			-font $font \
			-highlightthickness 0 \
			-padx 3 \
			-pady 2 \
			-setgrid 1 \
			-xscrollcommand ".$w.xscroll set" \
			-yscrollcommand ".$w.yscroll set" \
			\
			-height 20 \
			-width 80 \
			-wrap none
		scrollbar .$w.xscroll \
			-command ".$w.text xview" \
			-highlightthickness 0 \
			-orient horizontal
		scrollbar .$w.yscroll \
			-command ".$w.text yview" \
			-highlightthickness 0 \
			-orient vertical

		grid .$w.text -in .$w.frame -padx 1 -pady 1 \
			-row 0 -column 0 -rowspan 1 -columnspan 1 -sticky news
		grid .$w.yscroll -in .$w.frame -padx 1 -pady 1 \
			-row 0 -column 1 -rowspan 1 -columnspan 1 -sticky news
		grid .$w.xscroll -in .$w.frame -padx 1 -pady 1 \
			-row 1 -column 0 -rowspan 1 -columnspan 1 -sticky news
		grid rowconfig    .$w.frame 0 -weight 1 -minsize 0
		grid columnconfig .$w.frame 0 -weight 1 -minsize 0
	} else {
#		wm deiconify .$w
#		raise .$w
	}
#
	if {$proc != ""} {
		$proc $w
	}
#
}

proc print_ALL {w} {
	set CHANNEL_ID [open ${::env(HOME)}/.ezcomp-$::VERSION.ini w]
	puts -nonewline $CHANNEL_ID "### ezcomp-$::VERSION.ini\n\n\n"
	fprintf $CHANNEL_ID 0 "::ezcomp"
	puts -nonewline $CHANNEL_ID "return OK\n"
	close $CHANNEL_ID

	.$w.text insert end [string repeat = 72]\n
	descendant $w 0 .
	.$w.text insert end [string repeat = 72]\n

	.$w.text insert end [string repeat = 72]\n
	vprintf 0 "::ezcomp"
	.$w.text insert end [string repeat = 72]\n

########################################################################
	.$w.text insert end [string repeat = 72]\n
	# get the list for ID and image format
	# in doing, validate all the data
	set x_LIST {}
	foreach xID [info vars ::ezcomp::layers::*] {
		if {[array exists $xID] != 1} {
			# is it true?
			continue
		}
		upvar $xID a_ref
		set x $a_ref(filename)
		# make image FORMAT
		if [regexp -- {^\s*(\S+)\.([#]+)\.(\S+)\s*$} $x void image_NAME HASHes image_EXT] {
			set x "$a_ref(directory)$image_NAME.\[format %0[string length $HASHes]d \$x__fi\].$image_EXT"
		} else {
			set x $a_ref(directory)$x
		}
		lappend x_LIST [list [lindex [split $xID ::] end] $x]
	}
	set x__fo $::ezcomp::g__fo
	regexp -- {^\s*(\S+)\.([#]+)\.(\S+)\s*$} $x__fo void image_NAME HASHes image_EXT
	set x__fo "$image_NAME.\[format %0[string length $HASHes]d \$x__fi\].$image_EXT"
	.$w.text insert end "\[\n\t[join $x_LIST "\n\t"]\n\]\n"
	set x_LIST [lsort -command ::ezcomp::comp $x_LIST]
	.$w.text insert end "\[\n\t[join $x_LIST "\n\t"]\n\]\n"

	set NOW [clock scan now]
	set u_FILENAME ".[file rootname [file tail $::argv0]]-[clock format $NOW -format "%Y%m%d%H%M%S"]"
	set u_PATHNAME "$::env(HOME)/$u_FILENAME"
	set job_SCRIPT $::env(HOME)/.ezcomp-$::VERSION.job
	set job_SCRIPT $::env(HOME)/${u_FILENAME}.job
# 1. open JOB file for writing
	if [catch {eval set FILE_ID [open $job_SCRIPT w]} msg] {
		printf $msg
		return 0
	}

# 2. write ALFRED job script
	#### [ALFRED BEGIN.
	set m_ALFRED__BEGIN \
"##AlfredToDo 3.0

set AlfJobPause 1

Job -title {EZ-comp} -comment {
	#
} -envkey {} -dirmaps {} -service {} -tags {} -atleast 1 -atmost 1 -init {
} -subtasks \{
"
	puts -nonewline $FILE_ID [subst $m_ALFRED__BEGIN]
	#### .ALFRED BEGIN]
	####
	####
	#### [ALFRED MAIN.
	## [WORK HEAD.
	set m_WORK__HEAD \
"#	<HEAD>
	Task -title {WORK - HEAD} -subtasks {
	} -cmds {
	} -cleanup {
	} -chaser {
	}
#	</HEAD>
"
	puts -nonewline $FILE_ID [subst $m_WORK__HEAD]
	## .WORK HEAD]
	##
	## [WORK BODY BEGIN.
	set m_WORK__BODY___BEGIN \
"#	<BODY>
	Task -title {WORK - BODY} -subtasks \{
		Task -title {WORK - BODY -- preflight} -subtasks {
			Instance {WORK - HEAD}
		} -cmds {
		} -cleanup {
		} -chaser {
		}
"
	puts -nonewline $FILE_ID [subst $m_WORK__BODY___BEGIN]
	## .WORK BODY BEGIN]
	##
	set n__fi 0
	## [LOOP.
	for {set x__fi $::ezcomp::g__f_START} {$x__fi <= $::ezcomp::g__f_END} {incr x__fi} {
		# get LAYER list
		set LAYER_LIST {}

		set LABEL_LIST {}
		set OVER_LIST {}
		set SHADOW_LIST {}
		set ROTO_LIST {}
		set BACKGROUND_LIST {}
		for {set i 0} {$i < [llength $x_LIST]} {incr i} {
			set ID [lindex [lindex $x_LIST $i] 0]
			set image [subst [lindex [lindex $x_LIST $i] 1]]
			upvar ::ezcomp::layers::$ID a_ref
			if {$x__fi < $a_ref(f_START) || $a_ref(f_END) < $x__fi} {
				continue
			}
			lappend LAYER_LIST $ID

			# make COMP list
			if {$a_ref(type) == "ROTO"} {
				set ROTO_LIST [linsert $ROTO_LIST 0 "-isub $image"]
			}
			if {$a_ref(type) == "HERO"} {
				lappend OVER_LIST $image
			}
			if {$a_ref(type) == "GROUND SHADOW"} {
				lappend LABEL_LIST "$image -reorder 000r -label [string toupper $ID]"
				lappend SHADOW_LIST "-over [string toupper $ID]"
			}
			if {$a_ref(type) == "BACKGROUND"} {
				lappend BACKGROUND_LIST "-over $image"
			}
		}
####		.$w.text insert end "[format %3d $x__fi]: \[[join $LAYER_LIST ", "]\]\n"
		set COMP_ARGS {}
		if {[llength $LABEL_LIST] != 0} {
			lappend COMP_ARGS [join $LABEL_LIST]
		}
		if {[llength $OVER_LIST] != 0} {
			lappend COMP_ARGS [join $OVER_LIST]
		}
		if {[llength $SHADOW_LIST] != 0} {
			lappend COMP_ARGS [join $SHADOW_LIST]
		}
		if {[llength $ROTO_LIST] != 0} {
			lappend COMP_ARGS [join $ROTO_LIST]
		}
		if {[llength $BACKGROUND_LIST] != 0} {
			lappend COMP_ARGS [join $BACKGROUND_LIST]
		}
		lappend COMP_ARGS "-setalpha 0"
		lappend COMP_ARGS "-fo [subst $x__fo]"
		.$w.text insert end "[format %3d $x__fi]: \[[join $COMP_ARGS]\]\n"

########################################
		set m_FRAME___HEAD \
"		Task -title {[format %04d $x__fi]} -subtasks \{
"
		puts -nonewline $FILE_ID [subst $m_FRAME___HEAD]

		# do EZ-comp.
		set m_EZ_comp \
{			# do EZ-comp.: [format %04d $x__fi]
			Task -title {[format %04d $x__fi]: do EZ-comp.} -subtasks {
				Instance {WORK - BODY -- preflight}
			} -cmds {
				RemoteCmd {
					$::SHAKE [join $COMP_ARGS]
				} -service {shake} -atmost {1}
			} -cleanup {
			} -chaser {
				$::SHAKE [subst $x__fo]
			}
}
		puts -nonewline $FILE_ID [subst $m_EZ_comp]

		set m_FRAME___TAIL \
"		\} -cmds {
		} -cleanup {
		} -chaser {
		}
"
		puts -nonewline $FILE_ID [subst $m_FRAME___TAIL]
	}
	## .LOOP]
	##
	## [WORK BODY END.
	set m_WORK__BODY___END \
"	\} -cmds {
	} -cleanup {
	} -chaser {
	}
#	</BODY>
"
	puts -nonewline $FILE_ID [subst $m_WORK__BODY___END]
	## .WORK BODY END]
	##
	## [WORK TAIL.
	set m_WORK__TAIL \
"#	<TAIL>
	Task -title {WORK - TAIL} -subtasks {
		Instance {WORK - BODY}
	} -cmds {
	} -cleanup {
	} -chaser {
	}
#	</TAIL>
"
	puts -nonewline $FILE_ID [subst $m_WORK__TAIL]
	## .WORK TAIL]
	#### .ALFRED MAIN]
	####
	####
	#### [ALFRED END.
	set m_ALFRED___END \
"\} -cleanup {
}
"
	puts -nonewline $FILE_ID [subst $m_ALFRED___END]
	#### .ALFRED END]

# 3. close file
	if [catch {close $FILE_ID} msg] {
		set ::g_msg $msg
		return 0
	}

	if [catch {eval exec -- \"$::ALFRED\" $job_SCRIPT &} msg] {
		printf $msg
	}

	.$w.text insert end [string repeat = 72]\n
########################################################################
}

proc descendant {w LEVEL CHILD} {
	.$w.text insert end [format "%s%s\n" [string repeat "\t" $LEVEL] $CHILD]
	set list [winfo children $CHILD]
	if ![llength $list] "return"
	foreach x $list {
		descendant $w [expr $LEVEL + 1] $x
	}
}

proc vprintf {LEVEL CHILD} {
	set SPACEs [string repeat \t $LEVEL]

	if {$LEVEL == 0} {
		.vprintf.text insert end "## VAR: <$CHILD>\n"
		.vprintf.text insert end "if \{\[lsearch -exact \$::j_VARs $CHILD\] != -1\} \{\n"
	}
	.vprintf.text insert end "# NAMESPECE($CHILD)\n"
	.vprintf.text insert end "${SPACEs}namespace eval $CHILD \{\n"
	.vprintf.text insert end "${SPACEs}\}\n"

	foreach x [lsort [info vars "${CHILD}::*"]] {
		upvar $x ref_x
		if ![array exists $x] {
			# list
			.vprintf.text insert end \
				"${SPACEs}set $x [list $ref_x]\n"
			continue
		}

		# array
		foreach name [lsort [array names $x]] {
			.vprintf.text insert end \
				"${SPACEs}set $x\($name\) {$ref_x($name)}\n"
		}
	}

	set list [namespace children $CHILD]
	if ![llength $list] {
		if {$LEVEL == 0} {
			.vprintf.text insert end "\}\n"
			.vprintf.text insert end "## VAR: </$CHILD>\n\n\n"
		}
		return
	} else {
		.vprintf.text insert end "\n"
	}

	foreach x $list {
		vprintf [expr $LEVEL + 1] $x
	}
	if {$LEVEL == 0} {
		.vprintf.text insert end "\}\n"
		.vprintf.text insert end "## VAR: </$CHILD>\n\n\n"
	}
}

proc fprintf {CHANNEL_ID LEVEL CHILD} {
	set SPACEs [string repeat \t $LEVEL]

	if {$LEVEL == 0} {
#		puts -nonewline $CHANNEL_ID "## VAR: <$CHILD>\n"
#		puts -nonewline $CHANNEL_ID "if \{\[lsearch -exact \$::j_VARs $CHILD\] != -1\} \{\n"
	}
	puts -nonewline $CHANNEL_ID "# NAMESPACE($CHILD)\n"
	puts -nonewline $CHANNEL_ID "${SPACEs}namespace eval $CHILD \{\n"
	puts -nonewline $CHANNEL_ID "${SPACEs}\}\n"

	foreach x [lsort [info vars "${CHILD}::*"]] {
		upvar $x ref_x
		if ![array exists $x] {
			# list
			puts -nonewline $CHANNEL_ID \
				"${SPACEs}set $x [list $ref_x]\n"
			continue
		}

		# array
		foreach name [lsort [array names $x]] {
			puts -nonewline $CHANNEL_ID \
				"${SPACEs}set $x\($name\) {$ref_x($name)}\n"
		}
	}

	set list [namespace children $CHILD]
	if ![llength $list] {
		if {$LEVEL == 0} {
			puts -nonewline $CHANNEL_ID "\}\n"
			puts -nonewline $CHANNEL_ID "## VAR: </$CHILD>\n\n\n"
		}
		return
	} else {
		puts -nonewline $CHANNEL_ID "\n"
	}

	foreach x $list {
		fprintf $CHANNEL_ID [expr $LEVEL + 1] $x
	}
	if {$LEVEL == 0} {
#		puts -nonewline $CHANNEL_ID "\}\n"
#		puts -nonewline $CHANNEL_ID "## VAR: </$CHILD>\n\n\n"
	}
}

proc guess_FILENAME {w} {
	if {$w == ""} {
		set w .
	}

	set types {
		{"All images files" {.tif .iff} .iff}
		{"Configuration Files"	.ini}
		{"All Files"	*}
	}

	set file [
		tk_getOpenFile \
			-filetypes $types \
			-initialdir $::ezcomp::g__default_directory \
			-parent $w \
			-title "guess file information"
	]

	if {$file == ""} return
	set ::ezcomp::g__default_directory [file dirname $file]

	if ![regexp -- {\s*(.+)\.(\d+)\.(\S+)\s*$} $file void pre_file seq_num img_ext] {
#		printf "$file: NOT sequence file"
#		return
		set ::ezcomp::m__ID [file tail $file]
		set ::ezcomp::m__directory [file dirname $file]
		set ::ezcomp::m__filename [file tail $file]
		set ::ezcomp::m__f_START $::ezcomp::g__f_START
		set ::ezcomp::m__f_END $::ezcomp::g__f_END
		set ::ezcomp::m__f_range "$::ezcomp::m__f_START - $::ezcomp::m__f_END"
		return
	}
	set N [string length $seq_num]
	set seq_num [string trimleft $seq_num 0]

	set filelist [glob -nocomplain $pre_file.[string repeat \[0-9\] $N].$img_ext]
	set fi_list {}
	foreach file $filelist {
		regexp -- {\.(\d+)\.\S+\s*$} $file void fi
		set fi [string trimleft $fi 0]
		if {$fi < $seq_num || $fi < $::ezcomp::g__f_START || $fi > $::ezcomp::g__f_END} continue
		lappend fi_list $fi
	}

	if {[llength $fi_list] == 0} {
		printf "NO matching images"
		return
	}

	set fi_list [lsort -integer $fi_list]
	set ::ezcomp::m__ID [file tail $pre_file]
	set ::ezcomp::m__directory [file dirname $pre_file]
	set ::ezcomp::m__filename [file tail $pre_file].[string repeat # $N].$img_ext
	set ::ezcomp::m__f_START [lindex $fi_list 0]
	set ::ezcomp::m__f_END [lindex $fi_list end]
	set ::ezcomp::m__f_range "$::ezcomp::m__f_START - $::ezcomp::m__f_END"
	return
}

proc compare {a b} {
	upvar $a a_ref
	upvar $b b_ref
	if {$a_ref(index) > $b_ref(index)} {
		return 1
	}
	return -1
}

catch {uplevel #0 source ${::env(HOME)}/.ezcomp-$::VERSION.ini}
if {[::ezcomp::init gi] == 1} {
	draw_WINDOW $w
	wm deiconify .
	::ezcomp::createPlane $w.canvas
	focus -force $w.display.body.uID.entry
} else {
	if {$w == ""} {
		set w .
	}
	destroy $w
}
