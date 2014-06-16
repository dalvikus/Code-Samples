#!/usr/local/tcltk-8.3.4/bin/wish83.exe

################################################################################
#
# 1.13
#	1. set w .job -> unset
#	2. chunk-1.12.tcl -> chunk-1.13.tcl
#	3. ".job" -> ""
#
#
# 1.14
#	1. Replace "OLD"
#
#		proc scene_cp {...} {......}
#
#	   with "NEW"
#
#		proc scene_cp {...} {......}
#
#	2. Remove unused procedures:
#
#		proc ::job::regex {...} {......}
#		proc ::job::gi {...} {......}
#
#	3. Display the pathname of a Maya scene file in
#
#		.msg
#
#	   widget and decorate it.
#
#	4. Destroy the associsted chunk windows after "Save".
#
#	5. Diable "Open" button.
#
#	6. Ask if overwriting "DPS" file if it exists.
#
#
# 1.15
#
#	1. Introduce NEW variable, eXtended CHUNK list
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
#	   So rewrite procedures
#
#
#		proc ::job::run {...} {......}
#		proc ::job::init {...} {......}
#		proc ::job::xedit {...} {......}
#
#
#	2. Modify
#
#		first.TIF
#
#	3. Introduce LAST chunk. So, e-mail to user and sys. admin
#
#	4. Restor FAILED frame by using
#
#		(OLD g_fi) NEW g_fi
#
# 1.16
#
#	1. Load and save the variables
#
#		::alfred::*
#		::job::*
#		::mkSRC::*
#
#
# 1.17
#
#	1. Implement "Render Ground Shadow"
#
################################################################################

option add *font {"Courier New" 12} userDefault
option add *font {"Times New Roman" 14} userDefault
option add *font {Arial 14} userDefault
wm title . "DigiScape Template"

label .msg -foreground #0066CC -justify left -text ""
pack .msg -side top

########################################################################
set VERSION 2.04
########################################################################
set g_main("NAMESPACEs") [namespace children "::"]
set g_main("COMMANDs") [info commands "::*"]
set g_main("PROCs") [info procs "::*"]
set g_main("VARs") [info vars "::*"]
########
set wPYTHON "//chunji/share/Python22/python.exe"
if {$env(USERDOMAIN) == "WINDOWS2000S"} {
set PYTHON "F:/CyGWin/usr/bin/python2.2.exe"
set SERVER "WINDOWS2000S"
set ROOTPATH "F:/CyGWin/usr/local/bin/"
set WINDOWS_CMD "CMD"
set UNIX_BASH "F:/CyGWin/bin/bash"
set UNIX_CP "F:/CyGWin/usr/bin/cp"
set UNIX_MV "F:/CyGWin/usr/bin/mv"
set ALFRED "H:/Program Files/Pixar/RenderMan Artist Tools 4.5/bin/alfred.exe"
set ALFRED alfred
set PASSWD "F:/CyGWin/etc/passwd"
set GROUP "F:/CyGWin/etc/group"
} elseif {$env(USERDOMAIN) == "DIGISCAPEFX"} {
set PYTHON "Y:/bin/python2.2.exe"
set SERVER "chunji"
set ROOTPATH "Y:/usr/local/bin/"
set WINDOWS_CMD "CMD"
set UNIX_BASH "Y:/bin/bash"
set UNIX_CP "Y:/bin/cp"
set UNIX_MV "Y:/bin/mv"
set ALFRED "C:/Program Files/Pixar/RenderMan Artist Tools 4.5/bin/alfred.exe"
set ALFRED alfred
set PASSWD "Y:/etc/passwd"
set GROUP "Y:/etc/group"
set SHAKE shake
} else {
	printf "Where are you?"
	destroy .
	return
}
########################################################################
set DPS_MEDIA_UTILITY "//chunji/share/DPS/DPSMediaUtility"
set MKSLATE_SRC "Y:/usr/local/DigiScape/slate/"
set DPS_SERVER "//dps1/daily/"
set MKSLATE "mkslate-1.04.py"
########################################################################
set MAIL_HOST "digiscapefx.com"
set MAILTO "daily"
set SENDMAIL "notify-1.00"
########################################################################
uplevel #0 source [file join $::ROOTPATH menu-$::VERSION.tcl]
uplevel #0 source [file join $::ROOTPATH utils-$::VERSION.tcl]
uplevel #0 source [file join $::ROOTPATH mksrc-$::VERSION.tcl]
########################################################################
# load the default variables
proc load_VARs {} {
	global j_VARs
	set j_VARs {::ui ::mkSRC ::alfred ::job ::vars}

	if {[lindex $::argv 0] != ""} {
		# use external default variables
		uplevel #0 source [lindex $::argv 0]
	} elseif [file isfile $::env(HOME)/.j_VARs-$::VERSION.ini] {
		# use user default variables
		uplevel #0 source [file join $::env(HOME) .j_VARs-$::VERSION.ini]
	} else {
		# use system default variables
		uplevel #0 source [file join $::ROOTPATH vars-$::VERSION.tcl]
	}
}
load_VARs
########################################################################
if {1 == 0} {
if {$env(USER) != "ghkim" && $env(USER) != "im4u"} {
	printf "USER($env(USER)) != \"x-man\""
	destroy .
	return
}
}
if {![info exists ::ui::g__passUserInfo] || $::ui::g__passUserInfo == 0} {
	if {[::utils::userinfo] != 0} {
		printf "Call me(ghkim) - ^_^"
		destroy .
		return
	}
} else {
	focus -force .
	raise .
}
########################################################################
######## </menu> ########
########################################################################
######## <radiobutton> ########
#
# -value	Specifies value to store in the button's
#		associated variable whenever this button is selected.
# -variable	Specifies name of global variable to set
#		whenever this button is selected.
#		Changes in this variable also cause the button
#		to select or deselect itself.
#		Defaults to the value selectedButton.
#
######## </radiobutton> ########
namespace eval job {
	proc run {} {
		if {${::alfred::send_MAIL::g__MAIL} == 0 && ${::alfred::mk_DPS::g__DPS} == 1} {
			printf "Save \"last\""
			return
		}

		# how many chunks?

		# in genRIB chunk
		set n_genRIB 0
		# was RIB already made? or is RIB chunk activated?
		if {$::alfred::gen_RIB::g__RIB_DONE != 1} {
			set i_genRIB {}
			for {set i 0} {$i < [llength $::job::x_chunk]} {incr i} {
				# temporary LIST variable
				set x_LIST [lindex $::job::x_chunk $i]

				# only for TYPE = 1
				if {[lindex $x_LIST 2] != 1} {
					continue
				}

				if {[lindex $x_LIST 0] == "genRIB"} {
					upvar ::job::checkbutton_[lindex $x_LIST 3] i_ref
					if {$i_ref != 1} {
						printf "genRIB chunk NOT activated!"
						return
					}
					set i_genRIB $i
					set n_genRIB 1
					break
				}
			}
			if {$i_genRIB == ""} {
				printf "NO genRIB chunk!"
				return
			}
		}

		# in renHERO chunk
		set n_renHEROs 0
		for {set i 0} {$i < [llength $::alfred::renHERO::g__HEROs]} {incr i} {
			if {[lindex [lindex $::alfred::renHERO::g__HEROs $i] 1] == 1} {
				incr n_renHEROs
			}
		}
		# in ren__GROUND_SHADOW chunk
		set n_ren__GROUND_SHADOWs 0
		for {set i 0} {$i < [llength $::alfred::ren__GROUND_SHADOW::g__GROUND_SHADOWs]} {incr i} {
			if {[lindex [lindex $::alfred::ren__GROUND_SHADOW::g__GROUND_SHADOWs $i] 1] == 1} {
				incr n_ren__GROUND_SHADOWs
			}
		}
		
		if {[expr $n_genRIB + $n_renHEROs + $n_ren__GROUND_SHADOWs] == 0} {
			printf "NO work to do!"
			return
		}

		# save VARs
		set var_INI $::alfred::g_HEADER::m__pJOB$::alfred::g_HEADER::g_RIB--VARs-$::VERSION.ini
		set CHANNEL_ID [open $var_INI w]
		puts -nonewline $CHANNEL_ID "### j_VARs-$::VERSION\n\n\n"
		foreach var $::j_VARs {
			fprintf $CHANNEL_ID 0 $var
		}
		puts -nonewline $CHANNEL_ID "return OK\n"
		close $CHANNEL_ID

		# save JOB script
		set job_SCRIPT $::alfred::g_HEADER::m__pJOB$::alfred::g_HEADER::g_RIB--ALFRED-$::VERSION.job
		::alfred::main_TEXT $job_SCRIPT $n_genRIB $n_renHEROs $n_ren__GROUND_SHADOWs

		# execute alfred with job script in background
		set rc [catch {eval exec \"$::ALFRED\" $job_SCRIPT &} msg]
		if {$rc != 0} {
			printf "Exceptional Return Code = $rc"
			printf $msg
		}

		# reset DPS part
		set ::alfred::mk_DPS::g__DPS 0
		set ::alfred::send_MAIL::g__MAIL 0

		# reset command file(s) list
		set ::alfred::gen_RIB::m__CMDFILEs {}

		return

		# reset all(?) variables
		set ::alfred::g_HEADER::g_HDR 0

		# walk though "x_chunk" list
		for {set i 0} {$i < [llength $::job::x_chunk]} {incr i} {
			# temporary LIST variable
			set x_LIST [lindex $::job::x_chunk $i]

			# set index
			set ix [lindex $x_LIST 3]
			if {[lindex $x_LIST 2] == 0} {
				.items._item_$ix.button configure -text "[lindex $x_LIST 0]: \"\""
			} else {
				.items.item_$ix.button configure -text "\[ $ix \] [lindex $x_LIST 0]: \"\""
				set ::job::checkbutton_$ix 0
			}
			set x_LIST [lreplace $x_LIST 1 1 ""]
			set ::job::x_chunk [lreplace $::job::x_chunk $i $i $x_LIST]
		}

		set ::alfred::mk_DPS::g__DPS 0
		set ::alfred::send_MAIL::g__MAIL 0
#2.04#		.msg configure -text ""

		# in renHERO chunk
		foreach HERO $::alfred::renHERO::g__HEROs {
			array unset ::mkSRC::gi renHERO___$HERO
		}
		set ::alfred::renHERO::g__HEROs {}

		foreach SHADOW $::alfred::ren__GROUND_SHADOW::g__GROUND_SHADOWs {
			array unset ::mkSRC::gi ren__GROUND_SHADOW___$SHADOW
		}
		set ::alfred::ren__GROUND_SHADOW::g__HEROs {}

		return



		set f_RIB -1
		set proc_list {}
#		printf $::g_fi
		for {set i 0} {$i < [llength $::job::x_chunk]} {incr i} {
			# temporary LIST variable
			set x_LIST [lindex $::job::x_chunk $i]

			# only for TYPE = 1
			if {[lindex $x_LIST 2] != 1} {
				continue
			}

			upvar ::job::checkbutton_[lindex $x_LIST 3] ref_x
			# only for CHECKED button(s)
			if {$ref_x != 1} {
				continue
			}

			if {[lindex $x_LIST 0] == "genRIB"} {
				set f_RIB $i
			}

			set file [lindex $x_LIST 1]
			if {[lindex $x_LIST 0] == "genRIB"} {
				set f_RIB $i
				set proc_list [linsert $proc_list 0 $file]
			} else {
				set proc_list [lappend proc_list $file]
			}
		}
		if ![llength $proc_list] {
			printf "no work..."
			return
		}
		if {$f_RIB == -1} {
			printf "make RIB chunk..."
			return
		}

# Let's work
		set log ""
		xprintf log {}

		if {$::job::FIRST != ""} {
		}
		printf [join $proc_list ", "]

if {1 == 1} {
		set ALF_LIST {}
		foreach prog $proc_list {
			.log.text insert end "$prog begin......\n"

			set ix [catch {exec $::PYTHON $prog} msg]
			.log.text insert end "$msg\n"
			if [regexp -- \
				{ALFRED: \"(.*?)\"} \
				$msg x ALF] {
#				printf "x = '$x'"
#				printf "ALF = '$ALF'"
#				lappend ALF_LIST "$::ALFRED $ALF"
				lappend ALF_LIST "alfred $ALF"
			}

			.log.text insert end "$prog ......end\n"
#			set log "$log\n$msg\nexit status = $ix\n$prog ......end"
		}
}
		if {$::alfred::send_MAIL::g__MAIL == 1} {
#			set FILE_ID [open "| $::PYTHON $::SENDMAIL" w]
			if [catch {eval set FILE_ID [open "| $::PYTHON $::SENDMAIL" w]} msg] {
				printf $msg
				return
			}
			puts $FILE_ID "FROM: [::mkSRC::qVal last m_FROM]"
			puts $FILE_ID "TO: [::mkSRC::qVal last m_TO]"
			puts $FILE_ID "SUBJECT: [::mkSRC::qVal last m_SUBJECT]"
			puts $FILE_ID "[::mkSRC::qVal last m_CONTENT]\n"
			puts $FILE_ID "======== JOB STATUS ========"
			puts $FILE_ID "        unknown"
			puts $FILE_ID "======== ... ...... ========"
			if [catch {close $FILE_ID} msg] {
				printf $msg
			}
		}



#		printf [concat "sh" "-c" "\"[join $ALF_LIST "; "]\"" "&"]
#		catch {eval exec "sh" "-c" "\"[join $ALF_LIST "; sleep 5; "]\"" "&"} msg
#		.log.text insert end "$msg\n"
#		eval exec "\"$::ALFRED\"" "genRIB.alf" "&"
#		eval exec "\"$::ALFRED\"" "renHERO.alf" "&"
#		printf $log
	}
}
################################################################################
if {1 == 0} {
if ![info exists jobid] {
	set jobid "!@#$%^&*()"
	set g_fi "1"
	set shotid "sd3"
	set servers "jangbacksan 1"
	set logfile "xxx.log"
}
}

proc ::job::gi {} {
	set listvar {
		{"Job Name" jobid}
		{"Frames" g_fi}
		{"Shot Name" shotid}
		{"Partition" servers}
		{"Log File" logfile}
	}
	set f .gi
	frame $f
	for {set i 0} {$i < [llength $listvar]} {incr i} {
	########
		set if $f.if_$i
		set l $if.label
		set e $if.entry
		########
		frame $if
		label $l \
			-anchor e \
			-justify left \
			-text [lindex [lindex $listvar $i] 0] \
			-wraplength 4i \
			\
			-width 10
		set x [lindex [lindex $listvar $i] 1]
		upvar $x ref_x
#		printf "x = '$x', ref_x = '$ref_x'"
		entry $e \
			-relief groove \
			-textvariable $x
		pack $l -side left
		pack $e -side left -expand 1 -fill both
		pack $if -side top -fill x
		########
	########
	}
	pack $f -side top -padx 32 -pady 20 -fill x
}
#::job::gi
################################################################################
proc ::job::j_LOAD {} {
	set j_VARs [xfileDialog . open]
	if {$j_VARs == ""} return

	uplevel #0 source $j_VARs
	::job::init
}

proc ::job::j_SAVE {} {
	xprintf iprintf ::job::_j_SAVE

	set CHANNEL_ID [open ${::env(HOME)}/.j_VARs-$::VERSION.ini w]
	puts -nonewline $CHANNEL_ID "### j_VARs-$::VERSION.ini\n\n\n"
	fprintf $CHANNEL_ID 0 "::alfred"
	fprintf $CHANNEL_ID 0 "::job"
	fprintf $CHANNEL_ID 0 "::mkSRC"
	puts -nonewline $CHANNEL_ID "return OK\n"
	close $CHANNEL_ID
}
proc ::job::_j_SAVE {} {
	.iprintf.text insert end "### j_VARs-$::VERSION.ini\n\n\n"
	iprintf 0 "::alfred"
	iprintf 0 "::job"
	iprintf 0 "::mkSRC"
	.iprintf.text insert end "return OK"
}
################################################################################

proc ::job::confirm {index} {
	set ix [::job::i_search $index]
	upvar ::job::checkbutton_$index xref
#	printf "\[ $ix \]: index = \"$index\", status = \"$xref\""

	# only when checking ON
	if {$xref == 1} {
		if {[lindex [lindex $::job::x_chunk $ix] 1] == ""} {
			printf "Save?"
			set ::job::checkbutton_$index 0
			return
		}
	}

	# renHERO or ren__GROUND_SHADOW?
	set chunk [lindex [lindex $::job::x_chunk $ix] 0]
	set u_NAME [lindex [lindex $::job::x_chunk $ix] 1]
	if {$chunk == "renHERO"} {
		for {set i 0} {$i < [llength $::alfred::renHERO::g__HEROs]} {incr i} {
			set x [lindex $::alfred::renHERO::g__HEROs $i]
			if {$u_NAME == [lindex $x 0]} {
				set x [lreplace $x 1 1 $xref]
				set ::alfred::renHERO::g__HEROs [lreplace $::alfred::renHERO::g__HEROs $i $i $x]
				break
			}
		}
	}
	if {$chunk == "ren__GROUND_SHADOW"} {
		for {set i 0} {$i < [llength $::alfred::ren__GROUND_SHADOW::g__GROUND_SHADOWs]} {incr i} {
			set x [lindex $::alfred::ren__GROUND_SHADOW::g__GROUND_SHADOWs $i]
			if {$u_NAME == [lindex $x 0]} {
				set x [lreplace $x 1 1 $xref]
				set ::alfred::ren__GROUND_SHADOW::g__GROUND_SHADOWs [lreplace $::alfred::ren__GROUND_SHADOW::g__GROUND_SHADOWs $i $i $x]
				break
			}
		}
	}
}

proc ::job::i_search {index} {
	for {set i 0} {$i < [llength $::job::x_chunk]} {incr i} {
		# temporary LIST variable
		set x_LIST [lindex $::job::x_chunk $i]

		# only for TYPE = 1
		if {[lindex $x_LIST 2] != 1} {
			continue
		}
		if {[lindex $x_LIST 3] == $index} {
			return $i
			break
		}
	}
	return -1
}

proc ::job::init {} {
	# set a name of frame widget
	set f_items .items

	# create frame widget
	# destroy it first if it exists already
	if [winfo exists $f_items] {
		destroy $f_items
	}
	frame $f_items

	# pack frame widget
	#
	#         $f_items
	#
	# into ROOT(.) widget
	if [winfo exists .buttons] {
		pack $f_items -before .buttons -side top -padx 32 -pady 20 -fill x
	} else {
		pack $f_items -side top -padx 32 -pady 20 -fill x
	}

	# for TYPE = 0 elements
	for {set i 0} {$i < [llength $::job::x_chunk]} {incr i} {
		# temporary LIST variable
		set x_LIST [lindex $::job::x_chunk $i]

		# only for TYPE = 0
		if {[lindex $x_LIST 2] != 0} {
			continue
		}
		# set index
		set ix [lindex $x_LIST 3]
		# set a name of frame widget, which contains
		#     checkbutton and button widgets, and
		# names of checkbutton and button widgets
		set f $f_items._item_$ix
		set c $f.checkbutton
		set b $f.button

		# create frame widget
		frame $f

		# create checkbutton widget
		checkbutton $c \
			-anchor w \
			-relief groove \
			-takefocus 0 \
			\
			-state disabled \
			-variable ::job::_checkbutton_$ix
		# [lindex $x_LIST 4] = 1!
		$c select

		# set text variable for button and command
		# create button widget
		button $b \
			-anchor w \
			-relief groove \
			-text "[lindex $x_LIST 0]: \"[file tail [lindex $x_LIST 1]]\"" \
			\
			-command "xedit \"[lindex $x_LIST 0]\""

		# pack widgets
		#
		#     checkbutton $c, button $b
		#
		# into frame widget $f
		pack $c -side left -fill both
		pack $b -side left -expand 1 -fill both
		# , and frame widget $f into frame widget $f_items
		pack $f -side top -fill x
	}

	# for TYPE = 1 elements
	for {set i 0} {$i < [llength $::job::x_chunk]} {incr i} {
		# temporary LIST variable
		set x_LIST [lindex $::job::x_chunk $i]

		# only for TYPE = 1
		if {[lindex $x_LIST 2] != 1} {
			continue
		}
		# set index
		set ix [lindex $x_LIST 3]
		# set a name of frame widget, which contains
		#     checkbutton and button widgets, and
		# names of checkbutton and button widgets
		set f $f_items.item_$ix
		set c $f.checkbutton
		set b $f.button

		# create frame widget
		frame $f

		# create checkbutton widget
		checkbutton $c \
			-anchor w \
			-relief groove \
			-takefocus 0 \
			\
			-command "::job::confirm $ix" \
			-variable ::job::checkbutton_$ix
		# [lindex $x_LIST 4] = 1!
		if {[lindex $x_LIST 4] == 1} {
			$c select
		}

		# set text variable for button and command
		# create button widget
		if {[lindex $x_LIST 1] == ""} {
			set TEXT "\[ $ix \] [lindex $x_LIST 0]: \"\""
		} else {
			set TEXT "[lindex $x_LIST 0]: \"[file tail [lindex $x_LIST 1]]\""
		}
		button $b \
			-anchor w \
			-relief groove \
			-text $TEXT \
			\
			-command "xedit $ix"

		# pack widgets
		#
		#     checkbutton $c, button $b
		#
		# into frame widget $f
		pack $c -side left -fill both
		pack $b -side left -expand 1 -fill both
		# , and frame widget $f into frame widget $f_items
		# but before frame widget $f_items._item_2
		pack $f -before $f_items._item_2 -side top -fill x
	}
}
::job::init

#########################################################################
proc misc {} {
#
	set buttons .buttons
	frame $buttons
#	entry $buttons.argv \
#		-relief groove \
#		-textvariable argv \
#		-width 20
#
#
#
	button $buttons.chunk \
		-anchor w \
		-relief groove \
		-text "chunk" \
		-command {
			uplevel #0 source [file join $::ROOTPATH chunk-$::VERSION.tcl]
		}

######## <checkbutton> ########
#
# -variable	Specifies name of global variable to set
#		to indicate whether or not this button is selected.
#		Defaults to the name of the button within its parent
#		(i.e. the last element of the button window's path name).
#
######## </checkbutton> ########
	button $buttons.html \
		-relief groove \
		-anchor w \
		-text "html" \
		-command {
			html
		}

	button $buttons.run \
		-relief groove \
		-anchor w \
		-text "run" \
		-command {
			::job::run
		}

#	pack $buttons.html -side top -fill x
#	pack $buttons.chunk -side top -fill x
#	pack $buttons.run -side top -fill x
#	pack $buttons -side top -padx 32 -pady 20 -fill x
	pack $buttons -side top -padx 32 -pady 20
}
misc
########################################################################
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
		button .$w.buttons.save \
			-font $font \
			-relief groove \
			-text "save" \
			-command "xfileDialog .$w save"
		button .$w.buttons.erase \
			-font $font \
			-relief groove \
			-text "Erase" \
			-command ".$w.text delete 1.0 end"
		pack .$w.buttons.dismiss .$w.buttons.save .$w.buttons.erase -side left \
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
		$proc
	}
#
}
proc xfileDialog {w mode} {
	set types {
		{"Configuration Files"	.ini}
		{"All Files"	*}
	}
	set INIT_DIR "."
	if {$::job::GH != ""} {
		set INIT_DIR [file dirname $::job::GH]
	}

if {$mode == "open"} {
	set file [
		tk_getOpenFile \
			-defaultextension .ini \
			-filetypes $types \
			-initialdir $INIT_DIR \
			-initialfile j_VARs \
			-parent $w \
			-title "tk_getOpenFile: Configuration"
	]
	return $file
} elseif {$mode == "save"} {
	set file [
		tk_getSaveFile \
			-defaultextension .ini \
			-filetypes $types \
			-initialdir $INIT_DIR \
			-initialfile j_VARs \
			-parent $w \
			-title "tk_getSaveFile: Configuration"
	]
	if {$file != ""} {
		set CHANNEL_ID [open $file w]
		puts -nonewline $CHANNEL_ID [$w.text get 1.0 end]
		close $CHANNEL_ID
	} else {
#		printf "No file(save) specified"
	}
}
}

proc html {} {
	set font {"Lucida Console" 14}
	if ![winfo exists .html] {
		toplevel .html
		frame .html.buttons
		pack .html.buttons -side bottom -fill x
		button .html.buttons.dismiss \
			-font $font \
			-text Dismiss \
			-relief groove \
			-command "destroy .html"
		button .html.buttons.erase \
			-font $font \
			-relief groove \
			-text "Erase" \
			-command ".html.text delete 1.0 end"
		pack .html.buttons.dismiss .html.buttons.erase -side left \
			-expand 1 -pady 2
		#
		frame .html.frame
		pack .html.frame -expand yes -fill both -padx 1 -pady 1
		text .html.text \
			-font $font \
			-height 20 -width 80 \
			-highlightthickness 0 \
			-padx 3 \
			-pady 2 \
			-setgrid 1 \
			-xscrollcommand ".html.xscroll set" \
			-yscrollcommand ".html.yscroll set" \
			-wrap word \
			\
			-state normal
		scrollbar .html.xscroll \
			-command ".html.text xview" \
			-highlightthickness 0 \
			-orient horizontal
		scrollbar .html.yscroll \
			-command ".html.text yview" \
			-highlightthickness 0 \
			-orient vertical

		grid .html.text -in .html.frame -padx 1 -pady 1 \
			-row 0 -column 0 -rowspan 1 -columnspan 1 -sticky news
		grid .html.yscroll -in .html.frame -padx 1 -pady 1 \
			-row 0 -column 1 -rowspan 1 -columnspan 1 -sticky news
#		grid .html.xscroll -in .html.frame -padx 1 -pady 1 \
#			-row 1 -column 0 -rowspan 1 -columnspan 1 -sticky news
		grid rowconfig    .html.frame 0 -weight 1 -minsize 0
#		grid columnconfig .html.frame 0 -weight 1 -minsize 0
######## <embedded widget> ########
	for {set i 0} {$i < [llength $::job::xjoblist]} {incr i} {
		set x [lindex $::job::xjoblist $i]

		set f .html.text._item_$i
		set r $f.radiobutton_$i"
		set b $f.button"

		frame $f
##
		checkbutton $r \
			-anchor w \
			-relief groove \
			-takefocus 0 \
			-state disabled \
			\
			-variable ::job::xradiobutton_$i
		$r select
##
		button $b \
			-anchor w \
			-relief groove \
			-text $x \
			\
			-command "printf \"$x\""
##
		pack $r -side left -fill both
		pack $b -side left -expand 1 -fill both
#		pack $f -side top -fill x
		.html.text window create end -window $f
		.html.text insert end "\n"
	}
######## <main> ########
#	frame .html.text.items
	for {set i 0} {$i < [llength $::job::list]} {incr i} {
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
		.html.text window create [expr $i + 3 ].0 -window $f
		.html.text insert [expr $i + 3].1 "\n"
	}
#	.html.text configure -state disabled
#	.html.text configure -width
#	.html.text window create end -window .html.text.items
######## <embedded widget> ########
	} else {
#		wm deiconify .html
#		raise .html
	}
#
#	$proc
#
}

proc ex_printf x {
	upvar $x val_x
	if [array exists $x] {
		.xprintf.text insert end [format "ex_printf: %s is array\n" $x]
		foreach ax [array names val_x] {
			.xprintf.text insert end [format "\t%s(%s) = \[ %s \]\n" $x $ax [join $val_x($ax) ", "] ]
		}
	} else {
		.xprintf.text insert end [format "ex_printf: %s = \[ %s \]\n" $x [join $val_x ", "]]
	}
}

proc printHash {} {
	.xprintf.text insert end "======== children(.*) ========\n"
	foreach x [winfo children .] {
		descendant 0 $x
	}

#	printf [info nameofexecutable]
	.xprintf.text insert end "======== namespaces(::*) ========\n"
	vprintf 0 "::"
}

proc fprintf {CHANNEL_ID LEVEL CHILD} {
	set SPACEs [string repeat \t $LEVEL]

	if {$LEVEL == 0} {
		puts -nonewline $CHANNEL_ID "## VAR: <$CHILD>\n"
		puts -nonewline $CHANNEL_ID "if \{\[lsearch -exact \$::j_VARs $CHILD\] != -1\} \{\n"
	}
	puts -nonewline $CHANNEL_ID "# NAMESPECE($CHILD)\n"
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
		foreach name [array names $x] {
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
		puts -nonewline $CHANNEL_ID "\}\n"
		puts -nonewline $CHANNEL_ID "## VAR: </$CHILD>\n\n\n"
	}
}

proc iprintf {LEVEL CHILD} {
	set SPACEs [string repeat \t $LEVEL]

	if {$LEVEL == 0} {
		.iprintf.text insert end "## VAR: <$CHILD>\n"
		.iprintf.text insert end "if \{\[lsearch -exact \$::j_VARs $CHILD\] != -1\} \{\n"
	}
	.iprintf.text insert end "# NAMESPECE($CHILD)\n"
	.iprintf.text insert end "${SPACEs}namespace eval $CHILD \{\n"
	.iprintf.text insert end "${SPACEs}\}\n"

	foreach x [lsort [info vars "${CHILD}::*"]] {
		upvar $x ref_x
		if ![array exists $x] {
			# list
			.iprintf.text insert end \
				"${SPACEs}set $x [list $ref_x]\n"
			continue
		}

		# array
		foreach name [array names $x] {
			.iprintf.text insert end \
				"${SPACEs}set $x\($name\) {$ref_x($name)}\n"
		}
	}

	set list [namespace children $CHILD]
	if ![llength $list] {
		if {$LEVEL == 0} {
			.iprintf.text insert end "\}\n"
			.iprintf.text insert end "## VAR: </$CHILD>\n\n\n"
		}
		return
	} else {
		.iprintf.text insert end "\n"
	}

	foreach x $list {
		iprintf [expr $LEVEL + 1] $x
	}
	if {$LEVEL == 0} {
		.iprintf.text insert end "\}\n"
		.iprintf.text insert end "## VAR: </$CHILD>\n\n\n"
	}
}

# verbose printf
proc vprintf {level child} {
	.xprintf.text insert end [format "%s%s\n" [string repeat "\t" $level] $child]
	set level [expr $level + 1]
	.xprintf.text insert end [format "%sCOMMANDs\n" [string repeat "\t" $level]]
	foreach x [lsort [info commands "${child}::*"]] {
		if {[lsearch -exact $::g_main("COMMANDs") $x] != -1} {
			continue
		}
		if [llength [info procs "$x"]] {
			continue
		}
		if [regexp -- {^::\.} $x] {
			continue
		}
		.xprintf.text insert end [format "%s%s(%s)\n" [string repeat "\t" $level] $x [llength [info procs "$x"]]]
	}

	.xprintf.text insert end [format "%sPROCs\n" [string repeat "\t" $level]]
	foreach x [lsort [info procs "${child}::*"]] {
		if {[lsearch -exact $::g_main("PROCs") $x] != -1} {
			continue
		}
		if [regexp -- {^::pkg} $x] {
			continue
		}
		if [regexp -- {^::tcl} $x] {
			continue
		}
		if [regexp -- {^::tk} $x] {
			continue
		}
#		.xprintf.text insert end [format "%s%s(%s) {%s}\n" [string repeat "\t" $level] $x [info args $x] [info body $x]]
		.xprintf.text insert end [format "%s%s(%s) {......}\n" [string repeat "\t" $level] $x [info args $x]]
	}

	.xprintf.text insert end [format "%sVARs\n" [string repeat "\t" $level]]
	foreach x [lsort [info vars "${child}::*"]] {
		if {[lsearch -exact $::g_main("VARs") $x] != -1} {
			continue
		}
		upvar $x ref_x
		if [array exists $x] {
			.xprintf.text insert end [format "%s%s = %s\n" [string repeat "\t" $level] $x [print_array $x]]
#			.xprintf.text insert end [format "%s%s = \[... array ...\]\n" [string repeat "\t" $level] $x]
		} elseif {[llength $x] > 1} {
			.xprintf.text insert end [format "%s%s = \[%s\]\n" [string repeat "\t" $level] $x [join $ref_x "', '"]]
		} else {
			.xprintf.text insert end [format "%s%s = \['%s'\]\n" [string repeat "\t" $level] $x $ref_x]
		}
	}

	set x_list {"::msgcat" "::dom" "::xml" "::pkg"}
	set list [namespace children $child]
	if ![llength $list] "return"
	foreach x $list {
		if {[lsearch -exact $::g_main("NAMESPACEs") $x] != -1} {
			continue
		}
		if {[lsearch -exact $x_list $x] != -1} {
			continue
		}
#		vprintf [expr $level + 1] $x
		vprintf $level $x
	}
}

proc print_array {var_array} {
	upvar $var_array ref_array
	set x ""
	foreach key [array names $var_array] {
		set val $ref_array($key)
#		.xprintf.text insert end [format "%s\n" $val]
		lappend x "'$key':'$val'"
	}
	return "\[[join $x ", "]\]"
}


proc descendant {level child} {
	.xprintf.text insert end [format "%s%s\n" [string repeat "\t" $level] $child]
	set list [winfo children $child]
	if ![llength $list] "return"
	foreach x $list {
		descendant [expr $level + 1] $x
	}
}

proc EditBox {argv} {
	set w [lindex $argv 0]
	set font {"Courier New" 14}
	set font {"Lucida Console" 14}

	catch {destroy .$w}
	if ![winfo exists .$w] {
		toplevel .$w
		frame .$w.buttons
		pack .$w.buttons -side bottom -fill x
		button .$w.buttons.ok \
			-font $font \
			-relief groove \
			-text "   OK   " \
			-command "fileDialog .$w OK"
		button .$w.buttons.cancel \
			-font $font \
			-text " CANCEL " \
			-relief groove \
			-command "destroy .$w"
		pack .$w.buttons.ok .$w.buttons.cancel -side left \
			-expand 1 -pady 2
		#
		frame .$w.frame
		pack .$w.frame -expand yes -fill both -padx 1 -pady 1
		text .$w.text \
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

		return 0
	} else {
		wm deiconify .$w
		raise .$w

		return 1
	}
}

proc fileDialog {w mode} {
	if ![regexp -- {(\d+)$} $w index] {
		if ![regexp -- {([^_]+)$} $w chunk] {
			printf "invalid \$chunk = \"$chunk\""
			return
		}
#		printf $w
#		printf $chunk
#		set chunk "gh"
		set index 999
	} else {
		set ix [::job::i_search $index]
		if {$ix == -1} {
			printf "invalid index = \"$index\""
			return
		}
		set chunk [lindex [lindex $::job::x_chunk $ix] 0]
	}
################################################################################
	if {$chunk == "renHERO"} {
		set i_index [::job::i_search $index]
		set u_NAME [lindex [lindex $::job::x_chunk $i_index] 1]
		if {$u_NAME != ""} {
			set chunk ${chunk}___${u_NAME}
		}

		# get unique name
		set u_NAME [$w.text.${chunk}__m_FILENAMEVAR.entry get]
		if {$u_NAME == ""} {
			printf "Specify a unique \"m_FILENAMEVAR\"!"
			return
		}

		# is it unique?
		for {set i 0} {$i < [llength $::alfred::renHERO::g__HEROs]} {incr i} {
			if {$u_NAME == [lindex [lindex $::alfred::renHERO::g__HEROs $i] 0]} {
				if [tk_dialog .dialog "Overwrite?" "Overwrite $u_NAME?" "" 0 "Ok" "Cancel"] {
					return
				}
			}
		}
	}
	if {$chunk == "ren__GROUND_SHADOW"} {
		set i_index [::job::i_search $index]
		set u_NAME [lindex [lindex $::job::x_chunk $i_index] 1]
		if {$u_NAME != ""} {
			set chunk ${chunk}___${u_NAME}
		}

		# get unique name
		if ![regexp -- {^([^\s\.]+)\.([^\s\.]+)\.([^\s\.]+)$} [$w.text.${chunk}__m_NAME.entry get] x m_LIGHT m_SRC m_DEST] {
			printf "NOT \"LIGHT.SRC.DEST\""
			return
		}
####		printf "$m_LIGHT, $m_SRC, $m_DEST"
		set u_NAME ${m_LIGHT}_${m_SRC}_${m_DEST}

		# is it unique?
		for {set i 0} {$i < [llength $::alfred::ren__GROUND_SHADOW::g__GROUND_SHADOWs]} {incr i} {
			if {$u_NAME == [lindex [lindex $::alfred::ren__GROUND_SHADOW::g__GROUND_SHADOWs $i] 0]} {
				if [tk_dialog .dialog "Overwrite?" "Overwrite $u_NAME?" "" 0 "Ok" "Cancel"] {
					return
				}
			}
		}
	}
################################################################################
################################################################################
################################################################################
####	printf "OLD chunk = \"$chunk\""
	upvar ::mkSRC::gi($chunk) ref
	set text [$w.text get 1.0 end]
	for {set i 0} {$i < [llength $ref]} {incr i} {
		# temperary variable x
		set x [lindex $ref $i]
		if ![lindex $x 4] {
			continue
		}
#		printf $x
		set word [lindex $x 0]
		set content [$w.text.${chunk}__${word}.entry get]
		# update list
		set x [lreplace $x 1 1 $content]
		# update frame variable in global information
		if {$chunk == "gh" && $word == "g_fi"} {
			set ::g_fi $content
		}
		# quote string variable with "'"s
		if {[lindex $x 2] == 1} {
			set content "'$content'"
		}
		# update hash
		set ref [lreplace $ref $i $i $x]
		# temperary variable x
		regsub -- "<$word>" $text "$content" x
		set text $x
	}
#
	if {$index != 999} {
		if {$chunk == "genRIB" } {
			::alfred::gen_RIB::_init



			set x_LIST [lindex $::job::x_chunk $ix]
			set x_LIST [lreplace $x_LIST 1 1 "---- DONE ----"]
			set ::job::x_chunk [lreplace $::job::x_chunk $ix $ix $x_LIST]
			.items.item_[lindex $x_LIST 3].button configure -text "[lindex $x_LIST 0]: \"---- DONE ----\""

			set ::alfred::gen_RIB::g__RIB 1
		}

		if [regexp -- {^renHERO} $chunk x] {
#			set u_NAME [::mkSRC::qVal $chunk m_FILENAMEVAR]
			set x_LIST [lindex $::job::x_chunk $ix]
			set x_LIST [lreplace $x_LIST 1 1 $u_NAME]
			set ::job::x_chunk [lreplace $::job::x_chunk $ix $ix $x_LIST]
			.items.item_[lindex $x_LIST 3].button configure -text "[lindex $x_LIST 0]: \"$u_NAME\""

####			printf "NEW chunk = \"renHERO___$u_NAME\""
			::alfred::renHERO::_init $u_NAME
			set ::mkSRC::gi(renHERO___$u_NAME) $::mkSRC::gi($chunk)

			# $chunk = "renHERO" or "renHERO___OLDxxx"
			if {$chunk != "renHERO" && $chunk != "renHERO___$u_NAME"} {
				# reset CHECKBUTTON
				set ix [::job::i_search $index]
				upvar ::job::checkbutton_$index xref
####				printf "\[ $ix \]: index = \"$index\", status = \"$xref\""
				set xref 0

				array unset ::mkSRC::gi $chunk
				for {set iy 0} {$iy < [llength $::alfred::renHERO::g__HEROs]} {incr iy} {
					if {"renHERO___[lindex [lindex $::alfred::renHERO::g__HEROs $iy] 0]" == $chunk} {
						set ::alfred::renHERO::g__HEROs [lreplace $::alfred::renHERO::g__HEROs $iy $iy]
					}
				}
			}

			set ::mkSRC::gi(renHERO) $::mkSRC::gi(renHERO___$u_NAME)
		}

		if [regexp -- {^ren__GROUND_SHADOW} $chunk x] {
#			set u_NAME [::mkSRC::qVal $chunk m_NAME]
			set x_LIST [lindex $::job::x_chunk $ix]
			set x_LIST [lreplace $x_LIST 1 1 $u_NAME]
			set ::job::x_chunk [lreplace $::job::x_chunk $ix $ix $x_LIST]
			.items.item_[lindex $x_LIST 3].button configure -text "[lindex $x_LIST 0]: \"$u_NAME\""

####			printf "NEW chunk = \"ren__GROUND_SHADOW___$u_NAME\""
			::alfred::ren__GROUND_SHADOW::_init $u_NAME
			set ::mkSRC::gi(ren__GROUND_SHADOW___$u_NAME) $::mkSRC::gi($chunk)

			# $chunk = "ren__GROUND_SHADOW" or "ren__GROUND_SHADOW___OLDxxx"
			if {$chunk != "ren__GROUND_SHADOW" && $chunk != "ren__GROUND_SHADOW___$u_NAME"} {
				# reset CHECKBUTTON
				set ix [::job::i_search $index]
				upvar ::job::checkbutton_$index xref
####				printf "\[ $ix \]: index = \"$index\", status = \"$xref\""
				set xref 0

				array unset ::mkSRC::gi $chunk
				for {set iy 0} {$iy < [llength $::alfred::ren__GROUND_SHADOW::g__GROUND_SHADOWs]} {incr iy} {
					if {"ren__GROUND_SHADOW___[lindex [lindex $::alfred::ren__GROUND_SHADOW::g__GROUND_SHADOWs $iy] 0]" == $chunk} {
						set ::alfred::ren__GROUND_SHADOW::g__GROUND_SHADOWs [lreplace $::alfred::ren__GROUND_SHADOW::g__GROUND_SHADOWs $iy $iy]
					}
				}
			}

			set ::mkSRC::gi(ren__GROUND_SHADOW) $::mkSRC::gi(ren__GROUND_SHADOW___$u_NAME)
		}
	} elseif {$chunk == "gh"} {
		# initialize global variables
		::alfred::g_HEADER::_init

		# make directories
		file mkdir \
			${::alfred::g_HEADER::g_SRCDIR} \
			${::alfred::g_HEADER::m__pRIB} \
			${::alfred::g_HEADER::m__pSHADOW} \
			${::alfred::g_HEADER::m__pJOB} \
			${::alfred::g_HEADER::m__pIMAGE}

		# if necessary, copy Maya scene file
		if ![scene_cp] {
			printf "Maya scene not specified!"
			return
		}



		# update "x_chunk" list
		set x_LIST [lindex $::job::x_chunk 0]
		set x_LIST [lreplace $x_LIST 1 1 "---- DONE ----"]
		set ::job::x_chunk [lreplace $::job::x_chunk 0 0 $x_LIST]
		.items._item_0.button configure -text "global header: \"---- DONE ----\""

		# close this window
		destroy $w
		return
	} elseif {$chunk == "first"} {
	# 1. make DPS file
		# 1) get the frames' information
		set x_fi $::alfred::g_HEADER::g_fi
		if {[parse_fi $x_fi x__fi_list o__fi_list] == 0} {
			printf "parse_fi: $fi: parse error"
			return
		} else {
			if [info exists x__fi_list] {
####				printf "x__fi_list = \[[join $x__fi_list ", "]\]\n"
			}
			if [info exists o__fi_list] {
####				printf "o__fi_list = \[[join $o__fi_list ", "]\]\n"
			}
		}
		if [info exists x__fi_list] {
			set m__fi_list $x__fi_list
		} else {
			set m__fi_list $o__fi_list
		}

####		printf "$m__fi_list\n"
		if [info exists o__fi_list_SET] {
####			printf "$o__fi_list\n"
		}

		# 2) make DPS file
		set m_DPS_PATH ${::DPS_SERVER}DPS/daily/${::alfred::g_HEADER::g_SHOW}/
		# make DPS folder
		file mkdir $m_DPS_PATH
		set g_DPS_FILENAME [::mkSRC::qVal first g_DAILY_DATE]-${::alfred::g_HEADER::g_DEPT}-${::alfred::g_HEADER::g_LAYER}-${::alfred::g_HEADER::g_SHOW}.${::alfred::g_HEADER::g_SHOT}
		# look for last sequence no. with the same SHOW and SHOT name
####		printf $m_DPS_PATH
####		printf $g_DPS_FILENAME

		set filelist [glob -nocomplain $m_DPS_PATH*-${::alfred::g_HEADER::g_SHOW}.${::alfred::g_HEADER::g_SHOT}.???.dps]
####		printf $filelist
		set ix 0
		foreach file $filelist {
			if [regexp -- {\.(\d\d\d)\.dps$} $file x i] {
				set i [string trimleft $i 0]
				if {$i > $ix} {
					set ix $i
				}
			}
		}
		# what if ix = "008" => invalid octal number ???
		# remove leading '0'
####		printf $ix
		#!!! race condition?
		set g_DPS_FILENAME $g_DPS_FILENAME.[format %03d [expr $ix + 1]].dps
####		printf $g_DPS_FILENAME
		set g_DPSIMG_PATHNAME ${::DPS_SERVER}[string toupper ${::alfred::g_HEADER::g_IMG_EXT}]/daily/${::alfred::g_HEADER::g_SHOW}/${g_DPS_FILENAME}/[file rootname $g_DPS_FILENAME]
		if {[string toupper $::alfred::g_HEADER::g_IMG_EXT] == "PIC"} {
			set g_DPSIMG_PATHNAME $g_DPSIMG_PATHNAME.
		}
####		printf $g_DPSIMG_PATHNAME

		# 3) execute DPSMediaUtility.EXE
		set g_DPS_PATHNAME $m_DPS_PATH$g_DPS_FILENAME
####		printf $g_DPS_PATHNAME
		if [catch {exec $::DPS_MEDIA_UTILITY -batch -d[expr 1 + [llength $m__fi_list]] -f$g_DPS_PATHNAME} msg] {
			printf $msg
			return
		}

	# 2. update variables
		set ::alfred::mk_DPS::g__DPS_PATHNAME $g_DPS_PATHNAME
		set ::alfred::mk_DPS::g__DPSIMG_PATHNAME $g_DPSIMG_PATHNAME
####		printf $::alfred::mk_DPS::g__DPS_PATHNAME
####		printf $::alfred::mk_DPS::g__DPSIMG_PATHNAME
		set ::alfred::mk_DPS::g__DPS 1

		# 1) use variables set in global header as g_HEADER::*
		upvar ::mkSRC::gi($chunk) ref

		# g_SHOW
		set i [::mkSRC::index ::mkSRC::gi($chunk) m_show]
		set x [lindex $ref $i]
		set x [lreplace $x 1 1 ${::alfred::g_HEADER::g_SHOW}]
		set ref [lreplace $ref $i $i $x]

		# g_SHOT
		set i [::mkSRC::index ::mkSRC::gi($chunk) m_shot]
		set x [lindex $ref $i]
		set x [lreplace $x 1 1 ${::alfred::g_HEADER::g_SHOT}]
		set ref [lreplace $ref $i $i $x]

		# g_LAYER
		set i [::mkSRC::index ::mkSRC::gi($chunk) m_creator]
		set x [lindex $ref $i]
		set x [lreplace $x 1 1 ${::alfred::g_HEADER::g_LAYER}]
		set ref [lreplace $ref $i $i $x]

		set i [::mkSRC::index ::mkSRC::gi($chunk) m_date]
		set x [lindex $ref $i]
		set x [lreplace $x 1 1 [clock format [clock scan now] -format "%a %b %d %H:%M %Y"]]
		set ref [lreplace $ref $i $i $x]

		# 2) update array variable ::mkSRC::gi(last)
		set lastchunk "last"
		upvar ::mkSRC::gi($lastchunk) ref

		set i [::mkSRC::index ::mkSRC::gi($lastchunk) m_FROM]
		set x [lindex $ref $i]
		# MAIL_HOST required!
		set x [lreplace $x 1 1 ${::alfred::g_HEADER::g_LAYER}@${::MAIL_HOST}]
		set ref [lreplace $ref $i $i $x]

		set i [::mkSRC::index ::mkSRC::gi($lastchunk) m_TO]
		set x [lindex $ref $i]
		# optional MAIL_HOST
		set x [lreplace $x 1 1 "${::MAILTO}, ${::alfred::g_HEADER::g_LAYER}, ${::alfred::g_HEADER::g_DEPT}"]
		set ref [lreplace $ref $i $i $x]

		set i [::mkSRC::index ::mkSRC::gi($lastchunk) m_SUBJECT]
		set x [lindex $ref $i]
		set x [lreplace $x 1 1 "\[ daily \] - [::mkSRC::qVal first g_DAILY_DATE]"]
		set ref [lreplace $ref $i $i $x]

		set i [::mkSRC::index ::mkSRC::gi($lastchunk) m_CONTENT]
		set x [lindex $ref $i]
		set x [lreplace $x 1 1 ${::alfred::g_HEADER::g_SHOW}/[file tail $g_DPS_PATHNAME]]
		set ref [lreplace $ref $i $i $x]

		# 3) make and move SLATE to DPS
		if [catch {eval set FILE_ID [open "| $::wPYTHON ${::MKSLATE_SRC}${::MKSLATE} ${::alfred::g_HEADER::m__pJOB}/first.TIF" w]} msg] {
			printf $msg
			return
		} else {
			foreach m_XXX $::mkSRC::gi(first) {
				if {[lindex $m_XXX 0] == "g_DAILY_DATE"} {
					continue
				}
				puts $FILE_ID "[lindex $m_XXX 0]: [lindex $m_XXX 1]"
			}
			close $FILE_ID
		}


		# update "x_chunk" list
		set x_LIST [lindex $::job::x_chunk 1]
		set x_LIST [lreplace $x_LIST 1 1 "---- DONE ----"]
		set ::job::x_chunk [lreplace $::job::x_chunk 1 1 $x_LIST]
		.items._item_1.button configure -text "first: \"---- DONE ----\""

		file rename -force ${::alfred::g_HEADER::m__pJOB}/first.TIF ${g_DPSIMG_PATHNAME}0000.TIF
	} elseif {$chunk == "last"} {
	# make MAIL header
		if [catch {eval set FILE_ID [open "$::alfred::g_HEADER::m__pJOB$::alfred::g_HEADER::g_RIB.MAIL" w]} msg] {
			printf $msg
			return
		}
		puts $FILE_ID "FROM: [::mkSRC::qVal last m_FROM]"
		puts $FILE_ID "TO: [::mkSRC::qVal last m_TO]"
		puts $FILE_ID "SUBJECT: [::mkSRC::qVal last m_SUBJECT]"
		puts $FILE_ID "[::mkSRC::qVal last m_CONTENT]\n."
		if [catch {close $FILE_ID} msg] {
			printf $msg
			return
		}



		set i [expr [llength $::job::x_chunk] - 1]
		set x_LIST [lindex $::job::x_chunk $i]
		set x_LIST [lreplace $x_LIST 1 1 "---- DONE ----"]
		set ::job::x_chunk [lreplace $::job::x_chunk $i $i $x_LIST]
		.items._item_2.button configure -text "last: \"---- DONE ----\""

		set ::alfred::send_MAIL::g__MAIL 1
	}
	destroy $w
################################################################################
}
###############################################################################
proc xedit keyindex {
	set xlist {
		{"global header" gh}
		{"first" first}
		{"last" last}
	}

	set NAME ""
	if [regexp -- {^(\d+)$} $keyindex] {		# TYPE = 1 chunk
		# search for CHUNK
		set index [::job::i_search $keyindex]
		if {$index == -1} {
			printf "invalid \$keyindex = \"$keyindex\""
			return
		} else {
			set CHUNK [lindex [lindex $::job::x_chunk $index] 0]
			set NAME [lindex [lindex $::job::x_chunk $index] 1]
		}
		set WIDGET_NAME agenda_$keyindex
	} elseif {$keyindex == "global header"} {	# TYPE = 0 chunk
		set CHUNK "gh"
		set WIDGET_NAME agenda_$CHUNK
	} elseif {$keyindex == "first"} {		# TYPE = 0 chunk
		set CHUNK "first"
		set WIDGET_NAME agenda_$CHUNK
	} elseif {$keyindex == "last"} {		# TYPE = 0 chunk
		set CHUNK "last"
		set WIDGET_NAME agenda_$CHUNK
	} else {
		printf "invalid \$keyindex = \"$keyindex\""
		return
	}

####	printf "CHUNK = \"$CHUNK\", NAME = \"$NAME\", WIDGET_NAME = \"$WIDGET_NAME\""
	if ![EditBox $WIDGET_NAME] {
		::mkSRC::$CHUNK $WIDGET_NAME $NAME
	}
}

proc scene_cp {} {
	# references to the variable for scenes(local, server; OLD, NEW)
	upvar ::alfred::g_HEADER::m__lxSCENE ref_lxSCENE
	upvar ::alfred::g_HEADER::m__loSCENE ref_loSCENE
	upvar ::alfred::g_HEADER::m__sxSCENE ref_sxSCENE
	upvar ::alfred::g_HEADER::m__soSCENE ref_soSCENE

	set ref_loSCENE [string trim $ref_loSCENE]
	if ![file isfile $ref_loSCENE] {
		# not a plain file
		printf "$ref_loSCENE: NOT a plain file!"
		return 0
	}

	# copy file
	if {![file exists $ref_soSCENE] || ![tk_dialog .dialog Replace... "Overwirte $ref_soSCENE?" {} 0 okay cancel]} {
		if [catch {file copy -force $ref_loSCENE $ref_soSCENE} msg] {
			# error message
			printf $msg
			return 0
		}
		if {$msg != ""} {
			# what's the message?
			printf $msg
		}
	}

	#
	if {$ref_soSCENE != $ref_sxSCENE} {
		# start NEW job...

		# set NEW variable to OLD one
		set ref_sxSCENE $ref_soSCENE

		# reset command file(s) list
		set ::alfred::gen_RIB::m__CMDFILEs {}
	}

	# set NEW variable to OLD one
	set ref_lxSCENE $ref_loSCENE

	# update OLD scene pathname
	set ::vars::m__pSCENE [file dirname $ref_loSCENE]

	return 1
}
