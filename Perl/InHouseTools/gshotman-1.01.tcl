#!/bin/sh
# the next line restarts using wish \
exec wish "$0" "$@"

set VERSION 1.01

#option clear
option add *font {Arial 14} userDefault
#option add *background {#808080} userDefault
#option add *foreground {Black} userDefault



catch {uplevel #0 source [file join $::env(HOME) .shotman-$::VERSION.ini]}

namespace eval vars {
}
########################################################################
####                                                                ####
# Closing " will be appended...

set g__SHOTMAN {x:/bin/bash --login -c "shotman-1.02.tcl}
set g__XCACLS xcacls
set g__DOMAIN WINDOWS2000S
set g__SERVER windows2000s
set g__ROOT show

set g__SHOTMAN {y:/bin/bash --login -c "shotman-1.02.tcl}
set g__XCACLS //chunji/share/ResourceKit/xcacls
set g__SERVER chunji
set g__DOMAIN DIGISCAPEFX
set g__ROOT showx

#set g__GROUP_LIST {prog td anim}

#set g__USER Administrator
#set g__GROUP admin
#set g__USER im4u
#set g__GROUP prog

set ROOTPATH X:/usr/local/bin/
set PASSWD X:/etc/passwd
set GROUP X:/etc/group
set ROOTPATH Y:/usr/local/bin/
set PASSWD Y:/etc/passwd
set GROUP Y:/etc/group
# global information
namespace eval gi {
}
uplevel #0 source [file join $::ROOTPATH sUtils-$::VERSION.tcl]
#printf [option get . font *]
#printf [option get . background *]
#printf [option get . foreground *]
if {![info exists ::gi::g__passUserInfo] || $::gi::g__passUserInfo == 0} {
	if {[::utils::userinfo] != 0} {
		printf "Call me(ghkim) - ^_^"
		destroy .
		return
	}
} else {
	focus -force .
	raise .
}
####                                                                ####
########################################################################
wm title . "gShotMan-$::VERSION: $::g__USER\($::g__GROUP\)"
wm resizable . 0 0
focus -force .
printf $::g__GROUP_LIST
printf $::g__USER_LIST(prog)

########################################################################
####                    <ADMINISTRATOR LEVEL>                       ####
proc a__drawMain {} {
	set b_width 8
	set e_width 8
	set lb_width 10
	set lb_height 3

	frame .f -borderwidth 1 -relief solid
	pack .f -anchor w -padx 10 -pady 10

	## show
	frame .f.show -borderwidth 1 -relief solid
	pack .f.show -padx 10 -pady 10 -side left
	# head
	frame .f.show.head -borderwidth 1 -relief solid
	pack .f.show.head -anchor w -padx 10 -pady 10
	set ::vars::l_show "SHOW: "
	label .f.show.head.l \
		-textvariable ::vars::l_show
	pack .f.show.head.l -side left
	entry .f.show.head.e \
		-textvariable ::vars::e_show \
		\
		-width $e_width
	pack .f.show.head.e -padx 4 -pady 4 -side left
	checkbutton .f.show.head.cb \
		-text type \
		\
		-command a__CMD_checkbutton \
		-variable ::vars::cb_show
	pack .f.show.head.cb -side left
	# body
	frame .f.show.body -borderwidth 1 -relief solid
	pack .f.show.body -anchor w -padx 10 -pady 10
	listbox .f.show.body.lb \
		-borderwidth 1 \
		-exportselection no \
		-relief solid \
		-yscrollcommand ".f.show.body.sb set" \
		\
		-height $lb_height \
		-listvar ::vars::lb_show \
		-width $lb_width
	pack .f.show.body.lb -padx 4 -pady 4 -side left
	scrollbar .f.show.body.sb \
		\
		-command ".f.show.body.lb yview"
	pack .f.show.body.sb -fill y -padx 4 -pady 4 -side left


	## seq
	frame .f.seq -borderwidth 1 -relief solid
	pack .f.seq -padx 10 -pady 10 -side left
	# head
	frame .f.seq.head -borderwidth 1 -relief solid
	pack .f.seq.head -anchor w -padx 10 -pady 10
	set ::vars::l_seq "SEQ: "
	label .f.seq.head.l \
		-textvariable ::vars::l_seq
	pack .f.seq.head.l -side left
	entry .f.seq.head.e \
		-textvariable ::vars::e_seq \
		\
		-width $e_width
	pack .f.seq.head.e -padx 4 -pady 4 -side left
	# body
	frame .f.seq.body -borderwidth 1 -relief solid
	pack .f.seq.body -anchor w -padx 10 -pady 10
	listbox .f.seq.body.lb \
		-borderwidth 1 \
		-exportselection no \
		-relief solid \
		-yscrollcommand ".f.seq.body.sb set" \
		\
		-height $lb_height \
		-listvar ::vars::lb_seq \
		-width $lb_width
	pack .f.seq.body.lb -padx 4 -pady 4 -side left
	scrollbar .f.seq.body.sb \
		\
		-command ".f.seq.body.lb yview"
	pack .f.seq.body.sb -fill y -padx 4 -pady 4 -side left


	## shot
	frame .f.shot -borderwidth 1 -relief solid
	pack .f.shot -padx 10 -pady 10 -side left
	# head
	frame .f.shot.head -borderwidth 1 -relief solid
	pack .f.shot.head -anchor w -padx 10 -pady 10
	set ::vars::l_shot "SHOT: "
	label .f.shot.head.l \
		-textvariable ::vars::l_shot
	pack .f.shot.head.l -side left
	entry .f.shot.head.e \
		-textvariable ::vars::e_shot \
		\
		-width $e_width
	pack .f.shot.head.e -padx 4 -pady 4 -side left
	# body
	frame .f.shot.body -borderwidth 1 -relief solid
	pack .f.shot.body -anchor w -padx 10 -pady 10
	listbox .f.shot.body.lb \
		-borderwidth 1 \
		-exportselection no \
		-relief solid \
		-yscrollcommand ".f.shot.body.sb set" \
		\
		-height $lb_height \
		-listvar ::vars::lb_shot \
		-width $lb_width
	pack .f.shot.body.lb -padx 4 -pady 4 -side left
	scrollbar .f.shot.body.sb \
		\
		-command ".f.shot.body.lb yview"
	pack .f.shot.body.sb -fill y -padx 4 -pady 4 -side left

	frame .go -borderwidth 1 -relief solid
	pack .go -padx 10 -pady 10
	button .go.b \
		-text shoot \
		\
		-command a__run \
		-width $b_width
	pack .go.b -side left

	return
}
proc a__initMain {} {
	set ::vars::bg_show [.f.show.head.e cget -background]
	.f.show.head.e configure -background grey80 -state disabled
	.f.seq.head.e configure -background grey80 -state disabled
	set ::vars::bg_shot [.f.shot.head.e cget -background]
	.f.shot.head.e configure -background grey80 -state disabled

	set directory //$::g__SERVER/$::g__ROOT
	set x_LIST [glob -nocomplain -types d $directory/*]
	set o_LIST {}
	foreach x $x_LIST {
		set x [file tail $x]
		lappend o_LIST $x
	}
	set ::vars::lb_show $o_LIST

	bind .f.show.body.lb <ButtonRelease-1> a__extend_SEQ
	bind .f.seq.body.lb <ButtonRelease-1> a__extend_SHOT
	bind .f.shot.body.lb <<ListboxSelect>> ".f.shot.body.lb selection clear 0 end"
}


proc a__extend_SEQ {} {
	.f.shot.head.e configure -background grey80 -state disabled
	set ::vars::e_seq {}
	set ::vars::lb_seq {}
	set ::vars::e_shot {}
	set ::vars::lb_shot {}

	set m_index [.f.show.body.lb curselection]
	if {$m_index == ""} return
	set m_SHOW [lindex $::vars::lb_show $m_index]
	set ::vars::e_show $m_SHOW

	set directory //$::g__SERVER/$::g__ROOT/$m_SHOW
	set x_LIST [glob -nocomplain -types d $directory/*]
	set o_LIST {}
	foreach x $x_LIST {
		set x [file tail $x]
		if {[lsearch -exact $::g__GROUP_LIST $x] != -1} {
			continue
		}
		lappend o_LIST $x
	}
	set ::vars::lb_seq $o_LIST

	.f.shot.head.e configure -background $::vars::bg_shot -state normal
	focus .f.shot.head.e
}
proc a__extend_SHOT {} {
	set m_index [.f.show.body.lb curselection]
	if {$m_index == ""} return
	set m_SHOW [lindex $::vars::lb_show $m_index]
	set m_index [.f.seq.body.lb curselection]
	if {$m_index == ""} return
	set m_SEQ [lindex $::vars::lb_seq $m_index]
	set ::vars::e_seq $m_SEQ

	set directory //$::g__SERVER/$::g__ROOT/$m_SHOW/$m_SEQ
	set x_LIST [glob -nocomplain -types d $directory/*]
	set o_LIST {}
	foreach x $x_LIST {
		set x [file tail $x]
		if {[lsearch -exact $::g__GROUP_LIST $x] != -1} {
			continue
		}
		lappend o_LIST $x
	}
	set ::vars::lb_shot $o_LIST

	.f.shot.head.e configure -background $::vars::bg_shot -state normal
	set ::vars::e_shot $m_SEQ
	.f.shot.head.e icursor end
	focus .f.shot.head.e
}

proc a__run {} {
	xprintf .log {}
	focus .log

	if {$::vars::cb_show == 1} {
		# show
		set m_SHOW [string trim $::vars::e_show]
		if {$m_SHOW == ""} {
			printf "SHOW: SHOULD be given!"
			return
		}
		if ![regexp -- {^[[:alpha:]]+$} $m_SHOW] {
			printf "SHOW(\"$m_SHOW\"): NOT valid!"
			return
		}
		if {[lsearch -exact $::vars::lb_show $m_SHOW] != -1} {
			printf "SHOW(\"$m_SHOW\"): already exists!"
			return
		}
		# shot with seq
		set m_SHOT [string trim $::vars::e_shot]
		if {$m_SHOT == ""} {
			printf "SHOT: SHOULD be given!"
			return
		}
		if ![regexp -- {^([[:alpha:]]+)\d+.*$} $m_SHOT void m_SEQ] {
			printf "SHOT(\"$m_SHOT\"): NOT valid!"
			return
		}
		if {[lsearch -exact $::g__GROUP_LIST $m_SEQ] != -1} {
			printf "SEQ(\"$m_SEQ\"): should NOT a group!"
			return
		}

		set ::vars::e_show $m_SHOW
		set ::vars::e_seq $m_SEQ
		set ::vars::e_shot $m_SHOT
	} else {
		# show
		set m_index [.f.show.body.lb curselection]
		if {$m_index == ""} {
			printf "SHOW: SHOULD be selected!"
			return
		}
		set m_SHOW [lindex $::vars::lb_show $m_index]
		# seq
		set m_index [.f.seq.body.lb curselection]
		set x_SEQ {}
		if {$m_index != ""} {
			set x_SEQ [lindex $::vars::lb_seq $m_index]
		}
		# shot
		set m_SHOT [string trim $::vars::e_shot]
		if {[lsearch -exact $::vars::lb_shot $m_SHOT] != -1} {
			printf "SHOT(\"$m_SHOT\"): already exists!"
			return
		}
		if {$m_SHOT == ""} {
			printf "SHOT: SHOULD be given!"
			return
		}
		if ![regexp -- {^([[:alpha:]]+)\d+.*$} $m_SHOT void m_SEQ] {
			printf "SHOT(\"$m_SHOT\"): NOT valid!"
			return
		}
		if {[lsearch -exact $::g__GROUP_LIST $m_SEQ] != -1} {
			printf "SEQ(\"$m_SEQ\"): should NOT a group!"
			return
		}
		if {$x_SEQ != "" && $x_SEQ != $m_SEQ} {
			printf "SEQs(\"$x_SEQ\", \"$m_SEQ\"): NOT consistant!"
			return
		}

		set ::vars::e_shot $m_SHOT
		set ::vars::e_seq $m_SEQ
	}
	.log.text insert end "SHOW, SEQ, SHOT Information:\n\tSHOW = \"$m_SHOW\"\n\tSEQ = \"$m_SEQ\"\n\tSHOT = \"$m_SHOT\"\n"
	# Closing " have appended.
	set m_EXEC "$::g__SHOTMAN -s $m_SHOW -h $m_SHOT\""
	.log.text insert end "SHOTMAN Command:\n\t$m_EXEC\n"
	if [catch {eval exec $m_EXEC} msg] {
		.log.text insert end "SHOTMAN Error Message:\n+[string repeat - 70]+\n|[string repeat " " 70]|\n"
	} else {
		.log.text insert end "SHOTMAN Message:\n+[string repeat - 70]+\n|[string repeat " " 70]|\n"

		# refresh
		set m_index [lsearch -exact $::vars::lb_show $m_SHOW]
		if {$::vars::lb_show == "" || $m_index != -1} {
			set ::vars::lb_show [lsort [linsert $::vars::lb_show end $m_SHOW]]
		}
		set m_index [lsearch -exact $::vars::lb_seq $m_SEQ]
		if {$::vars::lb_seq == "" || $m_index != -1} {
			set ::vars::lb_seq [lsort [linsert $::vars::lb_seq end $m_SEQ]]
		}
		set ::vars::lb_shot [lsort [linsert $::vars::lb_shot end $m_SHOT]]
	}
	.log.text insert end $msg
	.log.text insert end \n|[string repeat " " 70]|\n+[string repeat - 70]+\n
}

proc a__CMD_checkbutton {} {
	if {$::vars::cb_show == 1} {
		.f.show.head.e configure -background $::vars::bg_show -state normal
		.f.shot.head.e configure -background $::vars::bg_shot -state normal
		focus .f.show.head.e

		.f.show.body.lb selection clear 0 end
		set ::vars::lb_seq {}
		set ::vars::e_show {}
		set ::vars::e_seq {}
		set ::vars::e_shot {}
		set ::vars::lb_shot {}

		bind .f.show.body.lb <ButtonRelease-1> ""
		bind .f.show.body.lb <<ListboxSelect>> ".f.show.body.lb selection clear 0 end"
	} else {
		.f.show.head.e configure -background grey80 -state disabled
		.f.seq.head.e configure -background grey80 -state disabled
		.f.shot.head.e configure -background grey80 -state disabled
		set ::vars::e_show {}
		set ::vars::e_seq {}
		set ::vars::e_shot {}

		bind .f.show.body.lb <ButtonRelease-1> a__extend_SEQ
		bind .f.show.body.lb <<ListboxSelect>> ""
	}
}
####                    </ADMINISTRATOR LEVEL>                      ####
########################################################################

########################################################################
####                    <USER LEVEL>                                ####
proc u__drawMain {} {
	set b_width 8
	set e_width 8
	set lb_width 10
	set lb_height 3

	frame .f -borderwidth 1 -relief solid
	pack .f -anchor w -padx 10 -pady 10

	## show
	frame .f.show -borderwidth 1 -relief solid
	pack .f.show -padx 10 -pady 10 -side left
	# head
	frame .f.show.head -borderwidth 1 -relief solid
	pack .f.show.head -anchor w -padx 10 -pady 10
	set ::vars::l_show "SHOW: "
	label .f.show.head.l \
		-textvariable ::vars::l_show
	pack .f.show.head.l -side left
	entry .f.show.head.e \
		-textvariable ::vars::e_show \
		\
		-width $e_width
	pack .f.show.head.e -padx 4 -pady 4 -side left
	# body
	frame .f.show.body -borderwidth 1 -relief solid
	pack .f.show.body -anchor w -padx 10 -pady 10
	listbox .f.show.body.lb \
		-borderwidth 1 \
		-exportselection no \
		-relief solid \
		-yscrollcommand ".f.show.body.sb set" \
		\
		-height $lb_height \
		-listvar ::vars::lb_show \
		-width $lb_width
	pack .f.show.body.lb -padx 4 -pady 4 -side left
	scrollbar .f.show.body.sb \
		\
		-command ".f.show.body.lb yview"
	pack .f.show.body.sb -fill y -padx 4 -pady 4 -side left


	## seq
	frame .f.seq -borderwidth 1 -relief solid
	pack .f.seq -padx 10 -pady 10 -side left
	# head
	frame .f.seq.head -borderwidth 1 -relief solid
	pack .f.seq.head -anchor w -padx 10 -pady 10
	set ::vars::l_seq "SEQ: "
	label .f.seq.head.l \
		-textvariable ::vars::l_seq
	pack .f.seq.head.l -side left
	entry .f.seq.head.e \
		-textvariable ::vars::e_seq \
		\
		-width $e_width
	pack .f.seq.head.e -padx 4 -pady 4 -side left
	# body
	frame .f.seq.body -borderwidth 1 -relief solid
	pack .f.seq.body -anchor w -padx 10 -pady 10
	listbox .f.seq.body.lb \
		-borderwidth 1 \
		-exportselection no \
		-relief solid \
		-yscrollcommand ".f.seq.body.sb set" \
		\
		-height $lb_height \
		-listvar ::vars::lb_seq \
		-width $lb_width
	pack .f.seq.body.lb -padx 4 -pady 4 -side left
	scrollbar .f.seq.body.sb \
		\
		-command ".f.seq.body.lb yview"
	pack .f.seq.body.sb -fill y -padx 4 -pady 4 -side left


	## shot
	frame .f.shot -borderwidth 1 -relief solid
	pack .f.shot -padx 10 -pady 10 -side left
	# head
	frame .f.shot.head -borderwidth 1 -relief solid
	pack .f.shot.head -anchor w -padx 10 -pady 10
	set ::vars::l_shot "SHOT: "
	label .f.shot.head.l \
		-textvariable ::vars::l_shot
	pack .f.shot.head.l -side left
	entry .f.shot.head.e \
		-textvariable ::vars::e_shot \
		\
		-width $e_width
	pack .f.shot.head.e -padx 4 -pady 4 -side left
	# body
	frame .f.shot.body -borderwidth 1 -relief solid
	pack .f.shot.body -anchor w -padx 10 -pady 10
	listbox .f.shot.body.lb \
		-borderwidth 1 \
		-exportselection no \
		-relief solid \
		-yscrollcommand ".f.shot.body.sb set" \
		\
		-height $lb_height \
		-listvar ::vars::lb_shot \
		-width $lb_width
	pack .f.shot.body.lb -padx 4 -pady 4 -side left
	scrollbar .f.shot.body.sb \
		\
		-command ".f.shot.body.lb yview"
	pack .f.shot.body.sb -fill y -padx 4 -pady 4 -side left


	### main frame
	frame .mf -borderwidth 1 -relief solid
#	pack .mf -anchor w -padx 10 -pady 10
	## method
	frame .mf.method -borderwidth 1 -relief solid
	pack .mf.method -anchor w -padx 10 -pady 10
	radiobutton .mf.method.c \
		-text "create" \
		\
		-command u__CMD_method \
		-value c \
		-variable ::vars::method
	pack .mf.method.c -side left
	radiobutton .mf.method.r \
		-text "remove" \
		\
		-command u__CMD_method \
		-value r \
		-variable ::vars::method
	pack .mf.method.r -side left
	# checkbutton
	checkbutton .mf.method.keep \
		-text keep \
		\
		-command "u__CMD_keep" \
		-variable ::vars::cb_keep
#	pack .mf.method.keep -padx 10 -pady 10 -side left
	set ::vars::method c
	## revision
	frame .mf.f -borderwidth 1 -relief solid
	pack .mf.f -anchor w -padx 10 -pady 10
	frame .mf.f.rev -borderwidth 1 -relief solid
	pack .mf.f.rev -anchor n -padx 10 -pady 10 -side left
	# head
	frame .mf.f.rev.head -borderwidth 1 -relief solid
	pack .mf.f.rev.head -fill x -padx 10 -pady 10
	set ::vars::l_rev "REV: "
	label .mf.f.rev.head.l \
		-textvariable ::vars::l_rev
	pack .mf.f.rev.head.l -side left
	entry .mf.f.rev.head.e \
		-textvariable ::vars::e_rev \
		\
		-width $e_width
	pack .mf.f.rev.head.e -padx 4 -pady 4 -side left
	# body
	frame .mf.f.rev.body -borderwidth 1 -relief solid
	pack .mf.f.rev.body -anchor w -padx 10 -pady 10
	listbox .mf.f.rev.body.lb \
		-borderwidth 1 \
		-exportselection no \
		-relief solid \
		-yscrollcommand ".mf.f.rev.body.sb set" \
		\
		-height $lb_height \
		-listvar ::vars::lb_rev \
		-width $lb_width
	pack .mf.f.rev.body.lb -padx 4 -pady 4 -side left
	scrollbar .mf.f.rev.body.sb \
		\
		-command ".mf.f.rev.body.lb yview"
	pack .mf.f.rev.body.sb -fill y -padx 4 -pady 4 -side left
	# keep
	frame .mf.f.keep -borderwidth 1 -relief solid
#	pack .mf.f.keep -anchor n -padx 10 -pady 10 -side left
	set ::vars::lb_keep {cmds jobs ribs renders shadowmaps}
	listbox .mf.f.keep.directory \
		-borderwidth 1 \
		-exportselection no \
		-relief solid \
		\
		-height 5 \
		-listvar ::vars::lb_keep \
		-selectmode extended
	pack .mf.f.keep.directory -anchor w -padx 4 -pady 4
	frame .mf.f.keep.f -borderwidth 1 -relief solid
	pack .mf.f.keep.f -padx 4 -pady 4
	# raised, sunken, flat, ridge, solid, groove
	button .mf.f.keep.f.b \
		-relief ridge \
		-text "backup to..." \
		\
		-command {
			set ::vars::e_keep [ \
						tk_chooseDirectory \
							-initialdir . \
							-parent . \
							-title "Choose directory..." \
							-mustexist true \
			]
		}
	pack .mf.f.keep.f.b -padx 4 -pady 4 -side left
	entry .mf.f.keep.f.e \
		-textvariable ::vars::e_keep \
		\
		-width 32
	pack .mf.f.keep.f.e -padx 4 -pady 4 -side left




	###
	frame .go -borderwidth 1 -relief solid
	pack .go -padx 10 -pady 10
	button .go.b \
		-text shoot \
		\
		-command u__run \
		-width $b_width
	pack .go.b -side left
}
proc u__initMain {} {
	.f.show.head.e configure -background grey80 -state disabled
	.f.seq.head.e configure -background grey80 -state disabled
	.f.shot.head.e configure -background grey80 -state disabled
	set ::vars::bg_rev [.mf.f.rev.head.e cget -background]

	set directory //$::g__SERVER/$::g__ROOT
	set x_LIST [glob -nocomplain -types d $directory/*]
	set o_LIST {}
	foreach x $x_LIST {
		set x [file tail $x]
		lappend o_LIST $x
	}
	set ::vars::lb_show $o_LIST

	bind .f.show.body.lb <ButtonRelease-1> u__extend_SEQ
	bind .f.seq.body.lb <ButtonRelease-1> u__extend_SHOT
	bind .f.shot.body.lb <ButtonRelease-1> u__extend_REV
}

proc u__init_SEQ {} {
	.f.seq.body.lb selection clear 0 end
	set ::vars::e_seq {}

	u__init_SHOT
}
proc u__extend_SEQ {} {
	set m_index [.f.show.body.lb curselection]
	if {$m_index == ""} return
	set m_SHOW [lindex $::vars::lb_show $m_index]
	set ::vars::e_show $m_SHOW

	u__init_SEQ

	set directory //$::g__SERVER/$::g__ROOT/$m_SHOW
	set x_LIST [glob -nocomplain -types d $directory/*]
	set o_LIST {}
	foreach x $x_LIST {
		set x [file tail $x]
		if {[lsearch -exact $::g__GROUP_LIST $x] != -1} {
			continue
		}
		lappend o_LIST $x
	}
	set ::vars::lb_seq $o_LIST
}
proc u__init_SHOT {} {
	set ::vars::e_seq {}
	.f.shot.body.lb selection clear 0 end
	set ::vars::e_shot {}
	set ::vars::lb_shot {}

	pack forget .mf
}
proc u__extend_SHOT {} {
	set m_index [.f.show.body.lb curselection]
	if {$m_index == ""} return
	set m_SHOW [lindex $::vars::lb_show $m_index]
	set m_index [.f.seq.body.lb curselection]
	if {$m_index == ""} return
	set m_SEQ [lindex $::vars::lb_seq $m_index]

	u__init_SHOT
	set ::vars::e_seq $m_SEQ

	set directory //$::g__SERVER/$::g__ROOT/$m_SHOW/$m_SEQ
	set x_LIST [glob -nocomplain -types d $directory/*]
	set o_LIST {}
	foreach x $x_LIST {
		set x [file tail $x]
		if {[lsearch -exact $::g__GROUP_LIST $x] != -1} {
			continue
		}
		lappend o_LIST $x
	}
	set ::vars::lb_shot $o_LIST
}
proc u__init_REV {} {
	pack configure .mf \
		-after .f \
		-anchor w \
		-padx 10 -pady 10

	set ::vars::method c
	u__CMD_method
	set ::vars::cb_keep 0

	set ::vars::e_rev {}
	set ::vars::lb_rev {}
	bind .mf.f.rev.body.lb <<ListboxSelect>> ".mf.f.rev.body.lb selection clear 0 end"
}
proc u__extend_REV {} {
	set m_index [.f.show.body.lb curselection]
	if {$m_index == ""} return
	set m_SHOW [lindex $::vars::lb_show $m_index]
	set m_index [.f.seq.body.lb curselection]
	if {$m_index == ""} return
	set m_SEQ [lindex $::vars::lb_seq $m_index]
	set m_index [.f.shot.body.lb curselection]
	if {$m_index == ""} return
	set m_SHOT [lindex $::vars::lb_shot $m_index]
	set ::vars::e_shot $m_SHOT

	u__init_REV

	set directory //$::g__SERVER/$::g__ROOT/$m_SHOW/$m_SEQ/$m_SHOT/image/$::g__GROUP/$::g__USER
	set x_LIST [glob -nocomplain -types d $directory/*]
	set o_LIST {}
	foreach x $x_LIST {
		set x [file tail $x]
		if {[lsearch -exact $::g__GROUP_LIST $x] != -1} {
			continue
		}
		lappend o_LIST $x
	}
	set ::vars::lb_rev $o_LIST

	set ::vars::e_rev r
	.mf.f.rev.head.e icursor end
	focus .mf.f.rev.head.e
	bind .mf.f.rev.body.lb <<ListboxSelect>> ".mf.f.rev.body.lb selection clear 0 end"
}
proc u__mark_REV {} {
	set m_index [.mf.f.rev.body.lb curselection]
	if {$m_index == ""} return
	set m_REV [lindex $::vars::lb_rev $m_index]
	set ::vars::e_rev $m_REV
}

proc u__run {} {
	xprintf .log {}
	focus .log

	set m_SHOW $::vars::e_show
	set m_SEQ $::vars::e_seq
	set m_SHOT $::vars::e_shot

	if {$::vars::method == "c"} {
		# at most 3 revisions
		if {[llength $::vars::lb_rev] == 3} {
			printf "At most 3 revisions!"
			return
		}
		set ::vars::e_rev [string trim $::vars::e_rev]
		set m_REV $::vars::e_rev
		if ![regexp -- {^r\d+$} $m_REV] {
			printf "revision(\"$m_REV\"): NOT valid!"
			return
		}
		if {[lsearch -exact $::vars::lb_rev $m_REV] != -1} {
			printf "revision(\"$m_REV\"): already exists!"
			return
		}

		.log.text insert end "1. Global informations:\n\tSHOW = \"$m_SHOW\"\n\tSEQ = \"$m_SEQ\"\n\tSHOT = \"$m_SHOT\"\n2. Purpose: create revision = \"$m_REV\"\n"
		# Closing " have appended.
		set m_EXEC "$::g__SHOTMAN -s $m_SHOW -h $m_SHOT -r $m_REV\""
		.log.text insert end "3. SHOTMAN Command:\n\t$m_EXEC\n"
		if [catch {eval exec $m_EXEC} msg] {
			.log.text insert end "4. SHOTMAN Error Message:\n+[string repeat - 70]+\n|[string repeat " " 70]|\n"
		} else {
			.log.text insert end "4. SHOTMAN Message:\n+[string repeat - 70]+\n|[string repeat " " 70]|\n"

			# refresh listbox
			set ::vars::lb_rev [lsort [linsert $::vars::lb_rev end $m_REV]]
			set ::vars::e_rev r
		}
		.log.text insert end $msg
		.log.text insert end \n|[string repeat " " 70]|\n+[string repeat - 70]+\n

		return
	}
	# ::vars::method = r
	set m_index [.mf.f.rev.body.lb curselection]
	if {$m_index == ""} return
	set m_REV [lindex $::vars::lb_rev $m_index]
	.log.text insert end "1. Global informations:\n\tSHOW = \"$m_SHOW\"\n\tSEQ = \"$m_SEQ\"\n\tSHOT = \"$m_SHOT\"\n2. Purpose: remove revision = \"$m_REV\"\n"
	set m_ARGV "-s $m_SHOW -h $m_SHOT -r $m_REV -e"
	if {$::vars::cb_keep == 1} {
		set ::vars::e_keep [string trim $::vars::e_keep]
		set m_directory $::vars::e_keep
		if {$m_directory == ""} {
			printf "choose directory!"
			return
		}
		if ![file isdirectory $m_directory] {
			printf "directory(\"$m_directory\"): NOT a directory!"
			return
		}
		append m_ARGV " -k $m_directory"

		set index_LIST [.mf.f.keep.directory curselection]
		set include_LIST {}
		foreach m_index $index_LIST {
			lappend include_LIST [lindex $::vars::lb_keep $m_index]
			append m_ARGV " -i [lindex $::vars::lb_keep $m_index]"
		}
		if {[llength $include_LIST] == 0} {
			printf "choose directories to be included!"
			return
		}

		.log.text insert end "\t2.1. Backup the followings to $m_directory\n\t\t\[[join $include_LIST ", "]\]\n"
	}

	# Closing " have appended.
	set m_EXEC "$::g__SHOTMAN $m_ARGV\""
	.log.text insert end "3. SHOTMAN Command:\n\t$m_EXEC\n"
	if [catch {eval exec $m_EXEC} msg] {
		.log.text insert end "4. SHOTMAN Error Message:\n+[string repeat - 70]+\n|[string repeat " " 70]|\n"
	} else {
		.log.text insert end "4. SHOTMAN Message:\n+[string repeat - 70]+\n|[string repeat " " 70]|\n"

		# initialization
		set m_index [lsearch -exact $::vars::lb_rev $m_REV]
		set ::vars::lb_rev [lreplace $::vars::lb_rev $m_index $m_index]
		set ::vars::e_rev {}
		.mf.f.rev.body.lb selection clear 0 end
		set ::vars::cb_keep 0
		u__CMD_keep

		if {[llength $::vars::lb_rev] == 0} {
			set ::vars::method c
			u__CMD_method
		}
	}
	.log.text insert end $msg
	.log.text insert end \n|[string repeat " " 70]|\n+[string repeat - 70]+\n



	return
}

proc u__CMD_method {} {
	if {$::vars::method == "r"} {
		if {[llength $::vars::lb_rev] == 0} {
			set ::vars::method c
			return
		}
		set ::vars::e_rev {}
		.mf.f.rev.head.e configure -background grey80 -state disabled
		bind .mf.f.rev.body.lb <<ListboxSelect>> {}
		bind .mf.f.rev.body.lb <ButtonRelease-1> "u__mark_REV"

		pack configure .mf.method.keep \
			-after .mf.method.r \
			-padx 10 -pady 10
		u__CMD_keep
	} else {
		set ::vars::e_rev r
		.mf.f.rev.head.e icursor end
		focus .mf.f.rev.head.e
		.mf.f.rev.head.e configure -background $::vars::bg_rev -state normal
		.mf.f.rev.body.lb selection clear 0 end
		bind .mf.f.rev.body.lb <<ListboxSelect>> ".mf.f.rev.body.lb selection clear 0 end"

		pack forget .mf.method.keep
		pack forget .mf.f.keep
	}
}

proc u__CMD_keep {} {
	if {$::vars::cb_keep == 1} {
		pack configure .mf.f.keep \
			-after .mf.f.rev \
			-anchor n \
			-padx 10 -pady 10
	} else {
		pack forget .mf.f.keep
	}
}
####                    </USER LEVEL>                               ####
########################################################################

proc xprintf {w proc} {
	if ![winfo exists $w] {
		toplevel $w
		frame $w.buttons -borderwidth 1 -relief solid
		pack $w.buttons -side bottom -fill x
		button $w.buttons.dismiss \
			-text dismiss \
			-command "destroy $w"
		button $w.buttons.erase \
			-text erase \
			-command "$w.text delete 1.0 end"
		pack $w.buttons.dismiss $w.buttons.erase -expand yes -side left
		#
		frame $w.frame -borderwidth 1 -relief solid
		pack $w.frame -expand yes -fill both
		text $w.text \
			-background #FFFFFF \
			-foreground #0066CC \
			-font {"Courier New" 16} \
			-highlightthickness 0 \
			-padx 3 \
			-pady 2 \
			-setgrid 1 \
			-xscrollcommand "$w.xscroll set" \
			-yscrollcommand "$w.yscroll set" \
			\
			-height 20 \
			-width 80 \
			-wrap none
		scrollbar $w.xscroll \
			-command "$w.text xview" \
			-highlightthickness 0 \
			-orient horizontal
		scrollbar $w.yscroll \
			-command "$w.text yview" \
			-highlightthickness 0 \
			-orient vertical

		grid $w.text -in $w.frame -padx 1 -pady 1 \
			-row 0 -column 0 -rowspan 1 -columnspan 1 -sticky news
		grid $w.yscroll -in $w.frame -padx 1 -pady 1 \
			-row 0 -column 1 -rowspan 1 -columnspan 1 -sticky news
		grid $w.xscroll -in $w.frame -padx 1 -pady 1 \
			-row 1 -column 0 -rowspan 1 -columnspan 1 -sticky news
		grid rowconfig    $w.frame 0 -weight 1 -minsize 0
		grid columnconfig $w.frame 0 -weight 1 -minsize 0
	} else {
		wm deiconify $w
		raise $w
	}

	if {$proc != ""} {
		$proc
	}

	return
}

########################################################################
####                                                                ####
if {[string toupper $::g__USER] == [string toupper ADministrator] || [string toupper $::g__USER] == [string toupper RenderMan]} {
	a__drawMain
	a__initMain
} else {
	u__drawMain
	u__initMain
}
####                                                                ####
########################################################################
