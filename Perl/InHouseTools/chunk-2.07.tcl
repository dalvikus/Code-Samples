# chunk.tcl --

################################################################################
#
# 1.13
#	1. ".job" -> ""
#
#
# 1.14
#	0. Nothing new
#
#
# 1.15
#
#	1. Rewrite entire module using NEW variable
#
#		::job::x_chunk
#
#	   instead of variables, for STATIC chunk,
#
#		::job::xjoblist
#		::job::xradiobutton_#
#
#	   and, for DYNAMIC chunk,
#
#		::job::hash
#		::job::list
#		::job::radiobutton_#
#
#	   The following procedures are modified:
#
#		proc ::chunk {...} {......}
#		proc ::xxx {...} {......}
#		proc ::nexti {...} {......}
#
################################################################################

option add *font {"Arial" 14} userDefault

set w .chunk
catch {destroy $w}
toplevel $w
wm title $w "job chunk"

namespace eval chunk {
}

label $w.msg \
	-justify left \
	-text "hello, world: " \
	-wraplength 4i
pack $w.msg -side top

########################################################################
set chunk_frame $w.chunk_frame
set chunk_listbox $chunk_frame.listbox
proc chunk {} {
	global w
	global chunk_frame
	global chunk_listbox

	frame $chunk_frame
#-borderwidth .5c
#
	scrollbar $chunk_frame.scroll -command "$chunk_listbox yview"
#
	listbox $chunk_listbox \
		-relief groove \
		-height 10 \
		-setgrid 1 \
		-yscroll "$chunk_frame.scroll set" \
		\
		-listvar ::chunk::basiclist
#
#
	pack $chunk_frame -side top -expand 1 -fill both -padx 32 -pady 20
	pack $chunk_frame.scroll -side right -fill y
	pack $chunk_listbox -side left -expand 1 -fill both
#

# set list variable
#
#	::chunk::basiclist
#
	set ::chunk::basiclist {}
	for {set i 0} {$i < [llength $::job::x_chunk]} {incr i} {
		# temporary LIST variable
		set x_LIST [lindex $::job::x_chunk $i]

		if {[lindex $x_LIST 2] == 0} {
			lappend ::chunk::basiclist "[lindex $x_LIST 0]: \"[file tail [lindex $x_LIST 1]]\""
		} else {
			lappend ::chunk::basiclist "\[ [lindex $x_LIST 3] \] [lindex $x_LIST 0]: \"[file tail [lindex $x_LIST 1]]\""
		}
	}
#
	$chunk_listbox itemconfigure 0 -foreground "#808080"
	$chunk_listbox itemconfigure 1 -foreground "#808080"
	$chunk_listbox itemconfigure [expr [llength $::chunk::basiclist] - 1] -foreground "#808080"
	$chunk_listbox selection set [expr [llength $::chunk::basiclist] - 1]
}
chunk
unset chunk_frame
########################################################################


########################################################################
proc plus {} {
	global w
	set buttons $w.buttons

	frame $buttons
#
	button $buttons.dismiss \
		-anchor w \
		-relief groove \
		-text Dismiss \
		\
		-command "destroy $w"
#
#
#
#	-relief {raised sunken flat ridge solid groove}
	button $buttons.log \
		-anchor w \
		-relief groove \
		-text "generate log" \
		\
		-command "printLog"
#
	entry $buttons.argv \
		-relief groove \
		-width 20
#
	button $buttons.genRIB \
		-anchor w \
		-relief groove \
		-text "generate RIB" \
		\
		-command "xxx genRIB"
#
	button $buttons.renHERO \
		-anchor w \
		-relief groove \
		-text "render Hero" \
		\
		-command "xxx renHERO"
#
	button $buttons.genShadow \
		-anchor w \
		-relief groove \
		-text "generate Shadow Map" \
		\
		-command "xxx genShadow"
#
	button $buttons.ren__GROUND_SHADOW \
		-anchor w \
		-relief groove \
		-text "render Ground Shadow" \
		\
		-command "xxx ren__GROUND_SHADOW"
#
	pack $buttons -side left -padx 32 -pady 20
	pack $buttons.dismiss -side top -fill x
	pack $buttons.log -side top -fill x
	pack $buttons.argv -side top
	pack $buttons.genRIB -side top -fill x
	pack $buttons.renHERO -side top -fill x
	pack $buttons.genShadow -side top -fill x
	pack $buttons.ren__GROUND_SHADOW -side top -fill x
}
plus
########################################################################


########################################################################
proc misc {} {
	global w
	set f_navi $w.f_navi
	frame $f_navi
#
	button $f_navi.expunge \
		-relief groove \
		-text "expunge" \
		-command {
			set i [$chunk_listbox curselection]
			if {$i == ""} {
				# ignore
				unset i
				return
			} elseif {$i < 2 || $i == [expr [$chunk_listbox index end] - 1]} {
				# ignore
				unset i
				return
			}
			$chunk_listbox delete $i
			$chunk_listbox selection set $i
#
if {1 == 0} {
	.html.text configure -state normal
	.html.text delete [expr $i + 1].0 [expr $i + 2].0
	.html.text configure -state disabled
}
#
			set ix [lindex [lindex $::job::x_chunk $i] 3]
			set u__NAME [lindex [lindex $::job::x_chunk $i] 1]
			if {$u__NAME != ""} {
				set xxx [lindex [lindex $::job::x_chunk $i] 0]
				if {$xxx == "ren__GROUND_SHADOW"} {
					array unset ::mkSRC::gi ren__GROUND_SHADOW___$u__NAME
					for {set iy 0} {$iy < [llength $::alfred::ren__GROUND_SHADOW::g__GROUND_SHADOWs]} {incr iy} {
						if {[lindex [lindex $::alfred::ren__GROUND_SHADOW::g__GROUND_SHADOWs $iy] 0] == $u__NAME} {
							set ::alfred::ren__GROUND_SHADOW::g__GROUND_SHADOWs [lreplace $::alfred::ren__GROUND_SHADOW::g__GROUND_SHADOWs $iy $iy]
						}
					}
				} elseif {$xxx == "renHERO"} {
					array unset ::mkSRC::gi renHERO___$u__NAME
					for {set iy 0} {$iy < [llength $::alfred::renHERO::g__HEROs]} {incr iy} {
						if {[lindex [lindex $::alfred::renHERO::g__HEROs $iy] 0] == $u__NAME} {
							set ::alfred::renHERO::g__HEROs [lreplace $::alfred::renHERO::g__HEROs $iy $iy]
						}
					}
				}
			}
			unset ::job::checkbutton_$ix
			destroy .items.item_$ix
			set ::job::x_chunk [lreplace $::job::x_chunk $i $i]
		}
#
	button $f_navi.move_up \
		-relief groove \
		-text "move up" \
		-command {
			set i [$chunk_listbox curselection]
			if {$i == ""} {
				# ignore
				unset i
				return
			} elseif {$i < 3 || $i == [expr [$chunk_listbox index end] - 1]} {
				# ignore
				unset i
				return
			}
			set x [$chunk_listbox get $i]
			$chunk_listbox delete $i
			$chunk_listbox insert [expr $i - 1] $x
			$chunk_listbox selection set [expr $i - 1]
			$chunk_listbox see [expr $i - 1]
#
if {1 == 0} {
	.html.text configure -state normal
	.html.text delete [expr $i + 1].0 [expr $i + 2].0
	set k $i
		set i [expr $i - 2 ]
		set ix [lindex $::job::list $i]

		set f .html.text.item_$ix
		frame $f
		set r $f.radiobutton_$ix
		set b $f.button
##
		checkbutton $r \
			-anchor w \
			-relief groove \
			-takefocus 0 \
			\
			-variable ::job::radiobutton_$ix
##
		button $b \
			-anchor w \
			-relief groove \
			-text "\[ [lindex $::job::list $i] \] $::job::hash(radiobutton_$ix)" \
			-width 50 \
			\
			-command "xedit $ix"
##
		pack $r -side left -fill both
		pack $b -side left -expand 1 -fill both
	#	.html.text window create [expr [lindex $::job::list [expr $idx - 2]] + 1].0 -window .html.text.item_$i
		.html.text window create [expr $i + 2 ].0 -window $f
		.html.text insert [expr $i + 2].1 "\n"
	set i $k
	.html.text configure -state disabled
	.html.text see $i.0
}
#
			set px [lindex $::job::x_chunk [expr $i - 1]]
			set  x [lindex $::job::x_chunk $i]
			set ipx [lindex $px 3]
			set  ix [lindex  $x 3]
			pack configure .items.item_$ix -before .items.item_$ipx
			set ::job::x_chunk [lreplace $::job::x_chunk [expr $i - 1] $i $x $px]
#
			unset x
			unset i
		}
#
	button $f_navi.move_down \
		-relief groove \
		-text "move down" \
		-command {
			set i [$chunk_listbox curselection]
			if {$i == ""} {
				# ignore
				unset i
				return
			} elseif {$i < 2 || $i >= [expr [$chunk_listbox index end] - 2]} {
				# ignore
				unset i
				return
			}
			set x [$chunk_listbox get $i]
			$chunk_listbox delete $i
			$chunk_listbox insert [expr $i + 1] $x
			$chunk_listbox selection set [expr $i + 1]
			$chunk_listbox see [expr $i + 1]
#
if {1 == 0} {
	.html.text configure -state normal
	.html.text delete [expr $i + 1].0 [expr $i + 2].0
	set k $i
		set i [expr $i - 2 ]
		set ix [lindex $::job::list $i]

		set f .html.text.item_$ix
		frame $f
		set r $f.radiobutton_$ix
		set b $f.button
##
		checkbutton $r \
			-anchor w \
			-relief groove \
			-takefocus 0 \
			\
			-variable ::job::radiobutton_$ix
##
		button $b \
			-anchor w \
			-relief groove \
			-text "\[ [lindex $::job::list $i] \] $::job::hash(radiobutton_$ix)" \
			-width 50 \
			\
			-command "xedit $ix"
##
		pack $r -side left -fill both
		pack $b -side left -expand 1 -fill both
	#	.html.text window create [expr [lindex $::job::list [expr $idx - 2]] + 1].0 -window .html.text.item_$i
		.html.text window create [expr $i + 4 ].0 -window $f
		.html.text insert [expr $i + 4].1 "\n"
	set i $k
	.html.text configure -state disabled
	.html.text see [expr $i + 2].0
}
#
			set  x [lindex $::job::x_chunk $i]
			set nx [lindex $::job::x_chunk [expr $i + 1]]
			set  ix [lindex  $x 3]
			set inx [lindex $nx 3]
			pack configure .items.item_$ix -after .items.item_$inx
			set ::job::x_chunk [lreplace $::job::x_chunk $i [expr $i + 1] $nx $x]
#
			unset x
			unset i
		}
#
	pack $f_navi -side right -padx 32 -pady 20 -fill y
	pack $f_navi.expunge -side top -fill x
	pack $f_navi.move_up -side top -fill x
	pack $f_navi.move_down -side top -fill x
}
misc
########################################################################

proc xxx work {
	set i [nexti]
	if {$i == ""} {
		printf "no more space"
		return
	}



	global w
	global chunk_listbox
	set idx [$chunk_listbox curselection]
	if {$idx == ""} {
		if {[$chunk_listbox index end] != 0} {
			tk_messageBox -title "EVOID" -message "먼저 선택하세요!" -type ok
			return;
		}
		set idx end
	} elseif {$idx <= 1} {
		tk_messageBox -title "ERANGE" -message "$idx: >= 2" -type ok
		return;
	}
#	set ::basiclist [linsert $::basiclist $idx [concat "\[$i\]" $work: $x]]
#	printf $::basiclist

	set x [.chunk.buttons.argv get]

	if ![info exists ::job::list] {
		set ::job::list {}
	}
#	$chunk_listbox insert $idx [list $work: [$w.buttons.argv get]]
	$chunk_listbox insert $idx "\[ $i \] $work: \"$x\""
	$chunk_listbox see [expr $idx + 1]
########
	set f_items .items
	set f $f_items.item_$i
	set c $f.checkbutton_$i
	set b $f.button
	frame $f
##
	checkbutton $c \
		-anchor w \
		-relief groove \
		-takefocus 0 \
		\
		-command "::job::confirm $i" \
		-variable ::job::checkbutton_$i
##
	button $b \
		-anchor w \
		-relief groove \
		-text "\[ $i \] $work: \"$x\"" \
		\
		-command "xedit $i"
##
	pack $c -side left -fill both
	pack $b -side left -expand 1 -fill both
#	printf "$idx [llength $::job::x_chunk]"
	if {[expr $idx + 1] == [llength $::job::x_chunk]} {
		pack $f -before .items._item_2 -side top -fill x
	} else {
		pack $f -before .items.item_[lindex [lindex $::job::x_chunk $idx] 3] -side top -fill x
	}
	set ::job::x_chunk [linsert $::job::x_chunk $idx [list $work "" 1 $i]]
	return
######## <embedded widget> ########
	set xprintf .html.text
	set f $xprintf.item_$i
	set r $f.radiobutton_$i
	set b $f.button
	frame $f
##
	checkbutton $r \
		-anchor w \
		-relief groove \
		-takefocus 0 \
		\
		-variable ::job::radiobutton_$i
##
	button $b \
		-anchor w \
		-relief groove \
		-text [concat "\[ $i \]" $work: $x] \
		\
		-command "xedit $i"
##
	pack $r -side left -fill both
	pack $b -side left -expand 1 -fill both
	.html.text configure -state normal
	if {[expr $idx - 2] == [llength $::job::list]} {
		.html.text window create [expr $idx + 1].0 -window .html.text.item_$i
		.html.text insert [expr $idx + 1].1 "\n"
	} else {
		.html.text window create [expr $idx + 1].0 -window .html.text.item_$i
		.html.text insert [expr $idx + 1].1 "\n"
	}
	.html.text configure -state disabled
	.html.text see [expr $idx + 1].0
######## <embedded widget> ########
	set ::job::list [linsert $::job::list [expr $idx - 2] $i]
}

proc nexti {} {
	for {set i 0} {$i < 100} {incr i} {
		if ![info exists "::job::checkbutton_$i"] {
			return $i
		}
	}
	return ""
}

proc printLog {} {
	set font {"Arial" 10}
	if ![winfo exists .log] {
		toplevel .log
		frame .log.buttons
		pack .log.buttons -side bottom -fill x
		button .log.buttons.dismiss \
			-font $font \
			-text Dismiss \
			-relief groove \
			-command "destroy .log"
		button .log.buttons.erase \
			-font $font \
			-relief groove \
			-text "Erase" \
			-command ".log.text delete 1.0 end"
		pack .log.buttons.dismiss .log.buttons.erase -side left \
			-expand 1 -pady 2
		#
		frame .log.frame
		pack .log.frame -expand yes -fill both -padx 1 -pady 1
		text .log.text \
			-font $font \
			-height 20 -width 80 \
			-wrap word \
			-xscrollcommand ".log.xscroll set" \
			-yscrollcommand ".log.yscroll set" \
			-setgrid 1 \
			-highlightthickness 0 \
			-pady 2 \
			-padx 3
		scrollbar .log.xscroll \
			-command ".log.text xview" \
			-highlightthickness 0 \
			-orient horizontal
		scrollbar .log.yscroll \
			-command ".log.text yview" \
			-highlightthickness 0 \
			-orient vertical

		grid .log.text -in .log.frame -padx 1 -pady 1 \
			-row 0 -column 0 -rowspan 1 -columnspan 1 -sticky news
		grid .log.yscroll -in .log.frame -padx 1 -pady 1 \
			-row 0 -column 1 -rowspan 1 -columnspan 1 -sticky news
#		grid .log.xscroll -in .log.frame -padx 1 -pady 1 \
#			-row 1 -column 0 -rowspan 1 -columnspan 1 -sticky news
		grid rowconfig    .log.frame 0 -weight 1 -minsize 0
		grid columnconfig .log.frame 0 -weight 1 -minsize 0
	} else {
#		wm deiconify .log
#		raise .log
	}
#
#
	global chunk_listbox
	wm title .log "xxx ......"
	wm iconname .log "xxx"
#
	.log.text insert end \
		[format "%s\n" [string repeat "=" 64]]
	set list [$chunk_listbox get 0 end]
	set i 0
	foreach x [$chunk_listbox get 0 end] {
		.log.text insert end \
			[format "foreach: %02d: '%s'\n" $i $x]
		incr i
	}
#	for {set i 0} {$i < [llength $list]} {incr i} {
#		.log.text insert end \
#			[format "for: %02d: '%s'\n" $i [lindex $list $i]]
#	}
	.log.text insert end \
		[format "%s\n" [string repeat "=" 64]]
}
