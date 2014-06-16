########################################################################
########################################################################
namespace eval utils {
	namespace export printf
	namespace export parse_fi

	proc printf str {
		tk_dialog .dialog {no title} $str {} 0 OK
	}

	proc parse_fi {str ref__x_fi_list ref__o_fi_list} {
		upvar $ref__x_fi_list r__x_fi_list
		upvar $ref__o_fi_list r__o_fi_list

		if ![regexp -- {^s*\((.*?)\)(.*)$} $str x OLD__g_fi NEW__g_fi] {
			set OLD__g_fi ""
			set NEW__g_fi $str
		}
#		printf "OLD__g_fi = '$OLD__g_fi'"
#		printf "NEW__g_fi = '$NEW__g_fi'"

		if {$OLD__g_fi != ""} {
			if {[_parse_fi $OLD__g_fi r__x_fi_list] == 0} {
#				printf "_parse_fi: $OLD__g_fi: parse error"
				return 0
			}
		}
		if {$NEW__g_fi != ""} {
			if {[_parse_fi $NEW__g_fi r__o_fi_list] == 0} {
#				printf "_parse_fi: $NEW__g_fi: parse error"
				return 0
			}
		}

		return 1
	}

	proc _parse_fi {str ref__fi_list} {
		upvar $ref__fi_list r__fi_list
		set if_list [split $str ","]
		set r__fi_list {}
		for {set i 0} {$i < [llength $if_list]} {incr i} {
			set x [lindex $if_list $i]
			set y ""
			set start ""
			set end ""
			set step ""
			set i_range ""
			set i_by ""
			set ix [
				regexp -- \
					{^\s*(\d+)\s*(-?)\s*(\d*)\s*(:?)\s*(\d*)\s*$} \
					$x y start i_range end i_by step
			]
#			printf "x = '$x'\nix = $ix\nS = '$start', R = '$i_range', E = '$end', B = '$i_by', T = '$step'"

			if {$ix == 0} {
#				printf "fi = '$str'\ninvalid '$x': NOT MATCHED"
				unset r__fi_list
				return 0
			}


			if {$start != "" && $i_range == "" && $end != ""} {
#				printf "fi = '$str'\ninvalid '$x': NOT -"
				unset r__fi_list
				return 0
			}
			if {$i_range == "-" && $end == ""} {
#				printf "fi = '$str'\ninvalid '$x': NOT f_END"
				unset r__fi_list
				return 0
			}
			if {$i_by == ":" && $step == ""} {
#				printf "fi = '$str'\ninvalid '$x': NOT f_STEP"
				unset r__fi_list
				return 0
			}
			if {$start != "" && $end != "" && $step != ""} {
				if {$i_range == "" || $i_by == ""} {
#					printf "fi = '$str'\ninvalid '$x': NO '-:'"
					unset r__fi_list
					return 0
				}
			}

			if {$end != "" && $step == ""} {
				set step 1;
			}

			if {$end == ""} {
#				lappend r__fi_list $start
				if {[lsearch $r__fi_list $start] == -1} {
					lappend r__fi_list $start
				}
			} else {
#				lappend r__fi_list "$start $end $step"
				for {set fi $start} {$fi <= $end} {set fi [expr $fi + $step]} {
					if {[lsearch -exact $r__fi_list $fi] == -1} {
						lappend r__fi_list $fi
					}
				}
			}
		}

		# sort
		set r__fi_list [lsort -integer $r__fi_list]

		return 1
	}

	proc userinfo {} {
		# get USER
		if [catch {set user $::env(USER)} msg] {
			printf $msg
			return -1
		}

		# get GROUP ID
		set FILE_ID [open $::PASSWD r]
		set group_ID {}
		while {[gets $FILE_ID line] != -1} {
			set passwd [split $line :]
			if {$user == [lindex $passwd 0]} {
				set group_ID [lindex $passwd 3]
				break
			}
		}
		close $FILE_ID
		if {$group_ID == ""} {
			printf "unknown user = \"$user\""
			return -1
		}

		# get GROUP
		set FILE_ID [open $::GROUP r]
		set group {}
		while {[gets $FILE_ID line] != -1} {
			set group_LIST [split $line :]
			if {$group_ID == [lindex $group_LIST 2]} {
				set group [lindex $group_LIST 0]
			}
		}
		close $FILE_ID
		if {$group == ""} {
			printf "unknown group ID = $group_ID"
			return -1
		}

		wm iconify .
		set userinfo userinfo
		toplevel .$userinfo
		wm title .$userinfo "user information"
		frame .$userinfo.user
		pack .$userinfo.user -side top
                label .$userinfo.user.key \
                        -anchor e \
                        -justify left \
                        -text user: \
                        -wraplength 4i \
                        \
                        -width 10
		pack .$userinfo.user.key -side left
                label .$userinfo.user.val \
                        -anchor w \
                        -justify left \
                        -text $user \
                        -wraplength 4i \
                        \
                        -width 10
		pack .$userinfo.user.val -side left
		frame .$userinfo.group
		pack .$userinfo.group -side top
                label .$userinfo.group.key \
                        -anchor e \
                        -justify left \
                        -text group: \
                        -wraplength 4i \
                        \
                        -width 10
		pack .$userinfo.group.key -side left
                label .$userinfo.group.val \
                        -anchor w \
                        -justify left \
                        -text $group \
                        -wraplength 4i \
                        \
                        -width 10
		pack .$userinfo.group.val -side left

		frame .$userinfo.status
		pack .$userinfo.status -side top
		checkbutton .$userinfo.status.checkbutton \
			-anchor e \
			\
			-text "remember me!" \
			-variable ::gi::g__passUserInfo
		pack .$userinfo.status.checkbutton -side left
		button .$userinfo.status.button \
			-anchor c \
			-relief ridge \
			-text "OK" \
			\
			-command "
				destroy .$userinfo
				wm deiconify .
				focus -force .
				raise .
			"
		pack .$userinfo.status.button -side left
		button .$userinfo.status.cancel \
			-anchor c \
			-relief ridge \
			-text "CANCEL" \
			\
			-command "
				destroy .
			"
		pack .$userinfo.status.cancel -side left

		set ::gi::m_LAYER $user
		set ::gi::m_DEPT $group
		set ::vars::m_creator $user

		return 0
	}
}

namespace import -force utils::printf utils::parse_fi

########################################################################
# Example
########################################################################
if {1 == 0} {
	set fi "(1 - 3, 2, 3, 4 - 100)1, 2 - 20"
	if {[parse_fi $fi x__fi_list o__fi_list] == 0} {
		printf "parse_fi: $fi: parse error"
	} else {
		if [info exists x__fi_list] {
			printf "x__fi_list = \[[join $x__fi_list ", "]\]"
		}
		if [info exists o__fi_list] {
			printf "o__fi_list = \[[join $o__fi_list ", "]\]"
		}
	}
	unset fi
}
########################################################################
########################################################################
