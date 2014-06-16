#!/bin/sh
# the next line restarts using wish \
exec wish "$0" "$@"

set VERSION 1.01

#option clear
option add *font {Arial 14} userDefault
#option add *background {#808080} userDefault
#option add *foreground {Black} userDefault



catch {uplevel #0 source [file join $::env(HOME) .pubtool-$::VERSION.ini]}

namespace eval vars {
	variable file_open .
	variable file_save .
}
########################################################################
####                                                                ####
set g__VOID {>>> \ / : * ? " < > | <<<}

set g__SERVER windows2000s
set g__ROOT show

set g__SERVER chunji
set g__ROOT showx

#set g__GROUP_LIST {anim prog td}
#set g__USER_LIST(prog) {ghkim iamoh}
#set g__USER_LIST(td) {led kozz polygon}
#set g__USER_LIST(anim) {freechoi limlimmi ziwaza}

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
uplevel #0 source [file join $::ROOTPATH pUtils-$::VERSION.tcl]
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
if {[string toupper $::g__USER] == [string toupper ADministrator]} {
	printf "Message: Only users can do."
	exit 1
}
####                                                                ####
########################################################################
wm title . "PubTool-$::VERSION: $::g__USER\($::g__GROUP\)"
wm resizable . 0 0
focus -force .

########################################################################
####                    <USER LEVEL>                                ####
proc drawMain {} {
	# menu width
	set m_width 20
	# label width
	set l_width 8
	# listbox width
	set lb_width 32
	# listbox height
	set lb_height 5
	# pub width
	set p_width 48
	# file (button) width
	set fb_width 24
	# file (entry) width
	set fe_width 32



	####
	set g_f .g_f
	frame $g_f -borderwidth 2 -relief solid
	pack $g_f -anchor w -padx 10 -pady 10



	### METHOD
	set m_f $g_f.m_f
	frame $m_f -borderwidth 1 -relief solid
	pack $m_f -anchor w -padx 10 -pady 10


	##
	button $m_f.do \
		-foreground #cc0066 \
		-text Do: \
		\
		-command Run \
		-width 8
	pack $m_f.do -anchor w -padx 10 -pady 10 -side left

	frame $m_f.method -borderwidth 1 -relief solid
	pack $m_f.method -anchor w -padx 10 -pady 10

	# radio buttons
	radiobutton $m_f.method.push \
		-text put \
		\
		-command METHOD_selected \
		-value PUSH \
		-variable ::vars::m__METHOD
	pack $m_f.method.push -side left
	radiobutton $m_f.method.pop \
		-text get \
		\
		-command METHOD_selected \
		-value POP \
		-variable ::vars::m__METHOD
	pack $m_f.method.pop -side left
	set ::vars::m__METHOD PUSH



	###
	set b_f $g_f.b_f
	frame $b_f -borderwidth 1 -relief solid
	pack $b_f -anchor w -padx 10 -pady 10


	## SHOW, SEQ, SHOT frame
	set ::vars::f__wqt $b_f.wqt_f
	frame $::vars::f__wqt -borderwidth 1 -relief solid
	pack $::vars::f__wqt -anchor n -padx 10 -pady 10 -side left

	# SHOW
	frame $::vars::f__wqt.show -borderwidth 1 -relief solid
	pack $::vars::f__wqt.show -anchor w -padx 10 -pady 10
	label $::vars::f__wqt.show.l -anchor e -text "SHOW: " -width $l_width
	pack $::vars::f__wqt.show.l -side left
	# \ / : * ? " < > |
	set ::vars::menu__SHOW [tk_optionMenu $::vars::f__wqt.show.m ::vars::m__SHOW $::g__VOID]
	$::vars::menu__SHOW entryconfigure 0 -command "SHOW_selected 0"
	$::vars::f__wqt.show.m configure -width $m_width
	pack $::vars::f__wqt.show.m -side left

	# SEQ
	frame $::vars::f__wqt.seq -borderwidth 1 -relief solid
	pack $::vars::f__wqt.seq -anchor w -padx 10 -pady 10
	label $::vars::f__wqt.seq.l -anchor e -text "SEQ: " -width $l_width
	pack $::vars::f__wqt.seq.l -side left
	set ::vars::menu__SEQ [tk_optionMenu $::vars::f__wqt.seq.m ::vars::m__SEQ $::g__VOID]
	$::vars::menu__SEQ entryconfigure 0 -command "SEQ_selected 0"
	$::vars::f__wqt.seq.m configure -width $m_width
	pack $::vars::f__wqt.seq.m -side left

	# SHOT
	frame $::vars::f__wqt.shot -borderwidth 1 -relief solid
	pack $::vars::f__wqt.shot -anchor w -padx 10 -pady 10
	label $::vars::f__wqt.shot.l -anchor e -text "SHOT: " -width $l_width
	pack $::vars::f__wqt.shot.l -side left
	set ::vars::menu__SHOT [tk_optionMenu $::vars::f__wqt.shot.m ::vars::m__SHOT $::g__VOID]
	$::vars::menu__SHOT entryconfigure 0 -command "SHOT_selected 0"
	$::vars::f__wqt.shot.m configure -width $m_width
	pack $::vars::f__wqt.shot.m -side left


	## GROUP, USER frame
	set ::vars::f__gu $b_f.gu_f
	frame $::vars::f__gu -borderwidth 1 -relief solid
	pack $::vars::f__gu -anchor n -padx 10 -pady 10 -side left

	# GROUP
	frame $::vars::f__gu.group -borderwidth 1 -relief solid
	pack $::vars::f__gu.group -anchor w -padx 10 -pady 10
	label $::vars::f__gu.group.l -anchor e -text "GROUP: " -width $l_width
	pack $::vars::f__gu.group.l -side left
	# for PUSH
	label $::vars::f__gu.group.g -anchor w -foreground #0066CC -textvariable ::g__GROUP -width $m_width
	# in METHOD_selected procedure
#	pack $::vars::f__gu.group.g -side left
	# for POP
	# \ / : * ? " < > |
	set ::vars::menu__GROUP [tk_optionMenu $::vars::f__gu.group.m ::vars::m__GROUP $::g__VOID]
	$::vars::f__gu.group.m configure -width $m_width
	# in METHOD_selected procedure
#	pack $::vars::f__gu.group.m -side left

	# USER
	frame $::vars::f__gu.user -borderwidth 1 -relief solid
	pack $::vars::f__gu.user -anchor w -padx 10 -pady 10
	label $::vars::f__gu.user.l -anchor e -text "USER: " -width $l_width
	pack $::vars::f__gu.user.l -side left
	# for PUSH
	label $::vars::f__gu.user.u -anchor w -foreground #0066CC -textvariable ::g__USER -width $m_width
	# in METHOD_selected procedure
#	pack $::vars::f__gu.user.u -side left
	# for POP
	# \ / : * ? " < > |
	set ::vars::menu__USER [tk_optionMenu $::vars::f__gu.user.m ::vars::m__USER $::g__VOID]
	$::vars::f__gu.user.m configure -width $m_width
	# in METHOD_selected procedure
#	pack $::vars::f__gu.user.m -side left



	### pub directory frame
	set ::vars::f__pub $g_f.pub_f
	frame $::vars::f__pub -borderwidth 1 -relief solid
	pack $::vars::f__pub -anchor w -padx 10 -pady 10


	## files in PUB directory
	frame $::vars::f__pub.ls -borderwidth 1 -relief solid
	pack $::vars::f__pub.ls -anchor w -padx 10 -pady 10

	# head
	frame $::vars::f__pub.ls.head -borderwidth 1 -relief solid
	pack $::vars::f__pub.ls.head -fill x -padx 10 -pady 10
	set ::vars::l_ls "files in PUB directory: "
	label $::vars::f__pub.ls.head.l \
		-textvariable ::vars::l_ls
	pack $::vars::f__pub.ls.head.l -side left
	set ::vars::e_pub ""
	entry $::vars::f__pub.ls.head.e \
		-background grey80 \
		-takefocus {} \
		-textvariable ::vars::e_ls \
		\
		-state disabled \
		-width $p_width
	pack $::vars::f__pub.ls.head.e -side left

	# body
	frame $::vars::f__pub.ls.body -borderwidth 1 -relief solid
	pack $::vars::f__pub.ls.body -anchor w -padx 10 -pady 10
	frame $::vars::f__pub.ls.body.p_lb -borderwidth 1 -relief solid
	pack $::vars::f__pub.ls.body.p_lb -anchor w -padx 10 -pady 10
	listbox $::vars::f__pub.ls.body.p_lb.lb \
		-borderwidth 1 \
		-exportselection no \
		-relief solid \
		-yscrollcommand "$::vars::f__pub.ls.body.p_lb.sb set" \
		\
		-height $lb_height \
		-listvar ::vars::lb_ls \
		-selectmode extended \
		-width $lb_width
	pack $::vars::f__pub.ls.body.p_lb.lb -padx 4 -pady 4 -side left
	scrollbar $::vars::f__pub.ls.body.p_lb.sb \
		\
		-command "$::vars::f__pub.ls.body.p_lb.lb yview"
	pack $::vars::f__pub.ls.body.p_lb.sb -fill y -padx 4 -pady 4 -side left


	## file(s) to be published or directory to be retrieved
	frame $::vars::f__pub.file -borderwidth 1 -relief solid
	pack $::vars::f__pub.file -anchor w -padx 10 -pady 10
	# raised, sunken, flat, ridge, solid, groove
	button $::vars::f__pub.file.b \
		-textvariable ::vars::b_pub \
		\
		-width $fb_width
	pack $::vars::f__pub.file.b -padx 4 -pady 4 -side left
	entry $::vars::f__pub.file.e \
		-textvariable ::vars::e_file \
		\
		-width $fe_width
	pack $::vars::f__pub.file.e -padx 4 -pady 4 -side left
}

proc METHOD_selected {} {
	if {$::vars::m__METHOD == "PUSH"} {
		# PUSH
		pack forget $::vars::f__gu.group.m
		pack configure $::vars::f__gu.group.g -after $::vars::f__gu.group.l
		pack forget $::vars::f__gu.user.m
		pack configure $::vars::f__gu.user.u -after $::vars::f__gu.user.l

		# fixed!
		set ::vars::x__GROUP $::g__GROUP
		set ::vars::m__GROUP $::g__GROUP
		set ::vars::x__USER $::g__USER
		set ::vars::m__USER $::g__USER

		# files in PUB directory
		ls_PUB

		set ::vars::e_file {}
		bind $::vars::f__pub.ls.body.p_lb.lb <<ListboxSelect>> "$::vars::f__pub.ls.body.p_lb.lb selection clear 0 end"
		set ::vars::b_pub "select file(s) to be published"
		$::vars::f__pub.file.b configure -command file_publish
	} else {
		# POP
		pack forget $::vars::f__gu.group.g
		pack configure $::vars::f__gu.group.m -after $::vars::f__gu.group.l
		pack forget $::vars::f__gu.user.u
		pack configure $::vars::f__gu.user.m -after $::vars::f__gu.user.l

		set ::vars::x__GROUP $::g__VOID
		set ::vars::m__GROUP $::g__GROUP
		GROUP_selected
		set ::vars::x__USER $::g__USER
		set ::vars::m__USER $::g__USER

		set ::vars::e_file {}
		bind $::vars::f__pub.ls.body.p_lb.lb <<ListboxSelect>> ""
		set ::vars::b_pub "select directory to be retrieved"
		$::vars::f__pub.file.b configure -command file_retrieve
	}
}

proc activate_WQT {} {
	## 1. create the FIXED group menu
	$::vars::menu__GROUP delete 0 last
	for {set i 0} {$i < [llength $::g__GROUP_LIST]} {incr i} {
		# "tk_optionMenu" command creates a menu widget
		# of type "radiobutton".
		$::vars::menu__GROUP add radiobutton \
			-command "GROUP_selected" \
			-label [lindex $::g__GROUP_LIST $i] \
			-variable ::vars::m__GROUP
	}

	## 2. create SHOW, SEQ, SHOT menus
	set directory //$::g__SERVER/$::g__ROOT
	set x_LIST [glob -nocomplain -types d $directory/*]
	set o_LIST {}
	foreach x $x_LIST {
		set x [file tail $x]
		lappend o_LIST $x
	}

	for {set i 0} {$i < [llength $o_LIST]} {incr i} {
		# "tk_optionMenu" command creates a menu widget
		# of type "radiobutton".
		$::vars::menu__SHOW add radiobutton \
			-command "SHOW_selected [expr 1 + $i]" \
			-label [lindex $o_LIST $i] \
			-variable ::vars::m__SHOW
	}
	# SHOW
	set ::vars::x__SHOW ">>> Select <<<"
	set ::vars::m__SHOW $::vars::x__SHOW
	# both -label and -value SHOULD be specified!
	$::vars::menu__SHOW entryconfigure 0 \
		-label $::vars::x__SHOW \
		-value $::vars::m__SHOW
	# SEQ
	set ::vars::x__SEQ ">>> Select SHOW <<<"
	set ::vars::m__SEQ $::vars::x__SEQ
	# ditto
	$::vars::menu__SEQ entryconfigure 0 \
		-label $::vars::x__SEQ \
		-value $::vars::m__SEQ
	# SHOT
	set ::vars::x__SHOT $::g__VOID
	set ::vars::m__SHOT $::vars::x__SHOT
	# ditto
	$::vars::menu__SHOT entryconfigure 0 \
		-label $::vars::x__SHOT \
		-value $::vars::m__SHOT

	return
}

# Using ::vars::m__GROUP, create user menu, and
# select user through ::vars::m__USER.
# If user is not ::g__USER_LIST(::vars::m__GROUP),
# select the first user.
#
proc GROUP_selected {} {
	if {$::vars::m__GROUP == $::vars::x__GROUP} {
		return
	}

	# get the group
	set m_GROUP $::vars::m__GROUP

	# drop all the OLD users
	$::vars::menu__USER delete 0 last

	## 1. create user menu
	# void menu
	$::vars::menu__USER add radiobutton \
		-command "USER_selected" \
		-label $::g__VOID \
		-variable ::vars::m__USER
	upvar ::g__USER_LIST($m_GROUP) m_USER_LIST
	for {set i 0} {$i < [llength $m_USER_LIST]} {incr i} {
		# "tk_optionMenu" command creates a menu widget
		# of type "radiobutton".
		$::vars::menu__USER add radiobutton \
			-command "USER_selected" \
			-label [lindex $m_USER_LIST $i] \
			-variable ::vars::m__USER
	}
	# save last label
	set ::vars::x__GROUP $m_GROUP

	## 2. select user
	# get user
	set m_USER $::vars::m__USER
	if {[lsearch $m_USER_LIST $::vars::m__USER] == -1} {
#		set ::vars::x__USER [lindex $m_USER_LIST 0]
#		set ::vars::m__USER [lindex $m_USER_LIST 0]
		set ::vars::x__USER $::g__VOID
		set ::vars::m__USER $::g__VOID
	}

	# files in PUB directory
	ls_PUB

	return
}
proc USER_selected {} {
	if {$::vars::m__USER == $::vars::x__USER} {
		return
	}

	# save last label
	set ::vars::x__USER $::vars::m__USER

	# files in PUB directory
	ls_PUB

	return
}

proc SHOW_selected {index} {
	if {$::vars::m__SHOW == $::vars::x__SHOW} {
		return
	}

	# get the SHOW name
	set m_SHOW $::vars::m__SHOW
	if {$index == 0} {
		# SEQ
		if {[$::vars::menu__SEQ index last] >= 1} {
			$::vars::menu__SEQ delete 1 last
			if {[$::vars::menu__SHOT index last] >= 1} {
				$::vars::menu__SHOT delete 1 last
			}
		}
		set ::vars::x__SEQ ">>> Select SHOW <<<"
		set ::vars::m__SEQ $::vars::x__SEQ
		# both -label and -value SHOULD be specified!
		$::vars::menu__SEQ entryconfigure 0 \
			-label $::vars::x__SEQ \
			-value $::vars::m__SEQ

		# SHOT
		if {[$::vars::menu__SHOT index last] >= 1} {
			$::vars::menu__SHOT delete 1 last
		}
		set ::vars::x__SHOT $::g__VOID
		set ::vars::m__SHOT $::vars::x__SHOT
		# both -label and -value SHOULD be specified!
		$::vars::menu__SHOT entryconfigure 0 \
			-label $::vars::x__SHOT \
			-value $::vars::m__SHOT

		# CLEAR ls PUB directory
		set ::vars::lb_ls {}
		set ::vars::e_ls {}

		# save last label
		set ::vars::x__SHOW $m_SHOW

		return
	}

	# index != 0
	set directory //$::g__SERVER/$::g__ROOT/$m_SHOW
	set x_LIST [glob -nocomplain -types d $directory/*]
	set o_LIST {}
	foreach x $x_LIST {
		set x [file tail $x]
		if {[lsearch $::g__GROUP_LIST $x] != -1} {
			continue
		}
		lappend o_LIST $x
	}
	if {[$::vars::menu__SEQ index last] >= 1} {
		$::vars::menu__SEQ delete 1 last
		if {[$::vars::menu__SHOT index last] >= 1} {
			$::vars::menu__SHOT delete 1 last
		}
	}
	for {set i 0} {$i < [llength $o_LIST]} {incr i} {
		# "tk_optionMenu" command creates a menu widget
		# of type "radiobutton".
		$::vars::menu__SEQ add radiobutton \
			-command "SEQ_selected [expr 1 + $i]" \
			-label [lindex $o_LIST $i] \
			-variable ::vars::m__SEQ
	}
	set ::vars::x__SEQ ">>> Select <<<"
	set ::vars::m__SEQ $::vars::x__SEQ
	# both -label and -value SHOULD be specified!
	$::vars::menu__SEQ entryconfigure 0 \
		-label $::vars::x__SEQ \
		-value $::vars::m__SEQ

	set ::vars::x__SHOT ">>> Select SEQ <<<"
	set ::vars::m__SHOT $::vars::x__SHOT
	# ditto
	$::vars::menu__SHOT entryconfigure 0 \
		-label $::vars::x__SHOT \
		-value $::vars::m__SHOT

	# files in PUB directory
	ls_PUB

	# save last label
	set ::vars::x__SHOW $m_SHOW

	return
}
proc SEQ_selected {index} {
	if {$::vars::m__SEQ == $::vars::x__SEQ} {
		return
	}

	# get the SHOW name
	set m_SHOW $::vars::m__SHOW
	# get the SEQ name
	set m_SEQ $::vars::m__SEQ
	if {$index == 0} {
		if {[$::vars::menu__SHOT index last] >= 1} {
			$::vars::menu__SHOT delete 1 last
		}
		set ::vars::x__SHOT ">>> Select SEQ <<<"
		set ::vars::m__SHOT $::vars::x__SHOT
		# both -label and -value SHOULD be specified!
		$::vars::menu__SHOT entryconfigure 0 \
			-label $::vars::x__SHOT \
			-value $::vars::m__SHOT

		# files in PUB directory
		ls_PUB

		# save last label
		set ::vars::x__SEQ $m_SEQ

		return
	}

	# index != 0
	set directory //$::g__SERVER/$::g__ROOT/$m_SHOW/$m_SEQ
	set x_LIST [glob -nocomplain -types d $directory/*]
	set o_LIST {}
	foreach x $x_LIST {
		set x [file tail $x]
		if {[lsearch $::g__GROUP_LIST $x] != -1} {
			continue
		}
		lappend o_LIST $x
	}
	if {[$::vars::menu__SHOT index last] >= 1} {
		$::vars::menu__SHOT delete 1 last
	}
	for {set i 0} {$i < [llength $o_LIST]} {incr i} {
		# "tk_optionMenu" command creates a menu widget
		# of type "radiobutton".
		$::vars::menu__SHOT add radiobutton \
			-command "SHOT_selected [expr 1 + $i]" \
			-label [lindex $o_LIST $i] \
			-variable ::vars::m__SHOT
	}
	set ::vars::x__SHOT ">>> Select <<<"
	set ::vars::m__SHOT $::vars::x__SHOT
	# both -label and -value SHOULD be specified!
	$::vars::menu__SHOT entryconfigure 0 \
		-label $::vars::x__SHOT \
		-value $::vars::m__SHOT

	# files in PUB directory
	ls_PUB

	# save last label
	set ::vars::x__SEQ $m_SEQ

	return
}
proc SHOT_selected {index} {
	if {$::vars::m__SHOT == $::vars::x__SHOT} {
		return
	}

	# get the SHOW name
	set m_SHOW $::vars::m__SHOW
	# get the SEQ name
	set m_SEQ $::vars::m__SEQ
	# get the SHOT name
	set m_SHOT $::vars::m__SHOT
	if {$index == 0} {
		# files in PUB directory
		ls_PUB

		# save last label
		set ::vars::x__SHOT $m_SHOT

		return
	}

	# index != 0
	# files in PUB directory
	ls_PUB

	# save last label
	set ::vars::x__SHOT $m_SHOT

	return
}

# ::vars::m__GROUP, ::vars::m__USER
# ::vars::m__SHOW, ::vars::m__SEQ, ::vars::m__SHOT
#
proc ls_PUB {} {
	# GROUP, USER
	set m_GROUP $::vars::m__GROUP
	set m_USER $::vars::m__USER
#	if {[regexp -- {^>>>.*<<<$} $m_GROUP] || [regexp -- {^>>>.*<<<$} $m_USER]} {}
	if [regexp -- {^>>>.*<<<$} $m_GROUP] {
		set ::vars::lb_ls {}
		set ::vars::e_ls {}
		return
	}

	# SHOW, SEQ, SHOT
	set m_SHOW $::vars::m__SHOW
	set m_SEQ $::vars::m__SEQ
	set m_SHOT $::vars::m__SHOT

	if [regexp -- {^>>>.*<<<$} $m_SHOW] {
		set ::vars::lb_ls {}
		set ::vars::e_ls {}
		return
	}
	set m_directory //$::g__SERVER/$::g__ROOT/$m_SHOW
	if ![regexp -- {^>>>.*<<<$} $m_SEQ] {
		set m_directory $m_directory/$m_SEQ
		if ![regexp -- {^>>>.*<<<$} $m_SHOT] {
			set m_directory $m_directory/$m_SHOT
		}
	}

	if {$m_USER == $::g__VOID} {
		set m_directory $m_directory/$m_GROUP/pub
	} else {
		set m_directory $m_directory/$m_GROUP/$m_USER/pub
	}
	set x_LIST [glob -nocomplain -types f $m_directory/*]
	set o_LIST {}
	foreach x $x_LIST {
		set x [file tail $x]
		lappend o_LIST $x
	}

	set ::vars::lb_ls $o_LIST
	set ::vars::e_ls $m_directory/*
}

proc file_publish {} {
	set file [
		tk_getOpenFile \
			-initialdir $::vars::file_open \
			-parent . \
			-title "file(s) to be published" \
	]
	if {$file == ""} return

	set x [string trim $::vars::e_file]
	if {$x == ""} {
		set ::vars::e_file $file
		set ix end
	} else {
		set ::vars::e_file "$file, $x"
		set ix [expr [string length $file] + 2]
	}
	focus $::vars::f__pub.file.e
	$::vars::f__pub.file.e icursor $ix
	$::vars::f__pub.file.e selection range 0 $ix
}
proc file_retrieve {} {
	set file [
		tk_getSaveFile \
			-initialdir $::vars::file_save \
			-parent . \
			-title "directory to be retrieved" \
	]
	if {$file == ""} return

	set ::vars::e_file $file
	focus $::vars::f__pub.file.e
	$::vars::f__pub.file.e icursor end
}

proc Run {} {
	set m_SHOW $::vars::m__SHOW
	set m_SEQ $::vars::m__SEQ
	set m_SHOT $::vars::m__SHOT

	# WHERE from/to?
	if [regexp -- {^>>>.*<<<$} $m_SHOW] {
		printf "SHOW: SHOULD be selected!"
		return
	}
	set m_directory //$::g__SERVER/$::g__ROOT/$m_SHOW
	if ![regexp -- {^>>>.*<<<$} $m_SEQ] {
		set m_directory $m_directory/$m_SEQ
		if ![regexp -- {^>>>.*<<<$} $m_SHOT] {
			set m_directory $m_directory/$m_SHOT
		}
	}
	append m_directory /$::vars::m__GROUP/$::vars::m__USER/pub

	if {$::vars::m__METHOD == "PUSH"} {
		# WHICH one?
		set files [string trim $::vars::e_file]
		set ::vars::e_file $files
		if {$files == ""} {
			printf "With WHAT?"
			return
		}
		set file_LIST [split $files ,]
		if {[llength $file_LIST] == 0} {
			printf "FILE(s)(\"$files\"): NOT valid!"
			return
		}
		foreach file $file_LIST {
			set file [string trim $file]
			if ![file isfile $file] {
				printf "FILE(\"$file\"): NOT a plain file!"
				return
			}
			if [catch {file copy -force $file $m_directory/} msg] {
				printf $msg
				return
			}
			# update PUB directory
			ls_PUB
		}
	} else {
		# WHICH one(s)?
		set indices [$::vars::f__pub.ls.body.p_lb.lb curselection]
		if {$indices == ""} {
			printf "select FILE(s)!"
			return
		}

		# where TO
		set whereTO [string trim $::vars::e_file]
		set ::vars::e_file $whereTO
		if {$whereTO == ""} {
			printf "TO where?"
			return
		}
		if ![file isdirectory [file dirname $whereTO]] {
			printf "$whereTO\(\"[file dirname $whereTO]\"\): NOT valid!"
			return
		}
		if {[llength $indices] > 1 && [file tail $whereTO] != "*"} {
			set msg "[llength $indices] files selected, so ignore filename [file tail $whereTO]. Use * instead?"
			if [tk_dialog .dialog ...... $msg "" 0 okay cancel] {
				return
			}
			set ::vars::e_file [file dirname $whereTO]/*
		}
		if {[llength $indices] != 1} {
			foreach index $indices {
				if [catch {file copy -force $m_directory/[lindex $::vars::lb_ls $index] [file dirname $whereTO]} msg] {
					printf $msg
				}
			}
		} else {
			if [catch {file copy -force $m_directory/[lindex $::vars::lb_ls $indices] $whereTO} msg] {
				printf $msg
			}
		}
	}
}
####                    </USER LEVEL>                               ####
########################################################################

########################################################################
####                                                                ####
	drawMain
	activate_WQT
	METHOD_selected
####                                                                ####
########################################################################
