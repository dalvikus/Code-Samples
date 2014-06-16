########################################################################
########################################################################
namespace eval utils {
	namespace export printf

	proc printf str {
		tk_dialog .dialog {no title} $str {} 0 OK
	}

	# group list: $g_LIST # of element > 1
	proc SEL_group {g_LIST} {
		wm iconify .

		set s .sel
		toplevel $s
		wm title $s "Select group"
		set f $s.f
		frame $f
		pack $f -padx 16 -pady 16
		frame $f.lf
		label $f.lf.l -anchor w -text "Select group:" -width 20
		pack $f.lf.l
		pack $f.lf
		frame $f.rf
		foreach group $g_LIST {
			radiobutton $f.rf.x_$group -anchor w -text $group -value $group -variable ::utils::m__GROUP
			pack $f.rf.x_$group -fill x -padx 10
		}
		pack $f.rf -fill x
#		frame $f.bf -borderwidth 2 -relief solid
#		button $f.bf.b -text OK -width 8
#		pack $f.bf.b
#		pack $f.bf -padx 10 -pady 10

		set ::utils::m__GROUP {}

		tkwait variable ::utils::m__GROUP

		destroy $s
		wm deiconify .
		focus -force .
		raise .
		return
	}

	proc userinfo {} {
		# get USER
		if [catch {set m_USER $::env(USER)} msg] {
			printf $msg
			return -1
		}

		# get users and set array for those
		set CHANNEL_ID [open /etc/passwd r]
		array set x_user {}
		set group_ID {}
		while {[gets $CHANNEL_ID line] != -1} {
			if {[string range $line 0 0] == "#"} {
				continue
			}
			set x_LIST [split $line :]
			if {[llength $x_LIST] != 7} {
				continue
			}
			set user [lindex $x_LIST 0]
			set gid [lindex $x_LIST 3]
			if {$m_USER == $user} {
				set group_ID $gid
			}
			lappend x_user($gid) $user
		}
		close $CHANNEL_ID
		if {$group_ID == ""} {
			printf "unknown user = \"$m_USER\""
			return -1
		}

		# get groups, and set array whose key is group
		set CHANNEL_ID [open /etc/group r]
		array set ::g__USER_LIST {}
		set ::g__GROUP_LIST {}
		set ::utils::m__GROUP {}
		while {[gets $CHANNEL_ID line] != -1} {
			if {[string range $line 0 0] == "#"} {
				continue
			}

			set x_LIST [split $line :]
			if {[llength $x_LIST] != 4} {
				continue
			}
			set group [lindex $x_LIST 0]
			if {$group_ID == [lindex $x_LIST 2]} {
				set ::utils::m__GROUP $group
			}
			# ignore admin group
			if {[string toupper $group] == [string toupper admin]} {
				continue
			}
			lappend ::g__GROUP_LIST $group
			set y_LIST {}
			foreach y [split [lindex $x_LIST 3] ,] {
				set y [string trim $y]
				if {$y != ""} {
					lappend y_LIST $y
				}
			}
			if {[lsearch -exact [array names x_user] [lindex $x_LIST 2]] == -1} {
				if {[llength $y_LIST] == 0} {
					continue
				} else {
					set ::g__USER_LIST($group) $y_LIST
				}
			} else {
				set ::g__USER_LIST($group) [concat $x_user([lindex $x_LIST 2]) $y_LIST]
			}
		}
		close $CHANNEL_ID
		if {$::utils::m__GROUP == ""} {
			printf "unknown group ID = $group_ID"
			return -1
		}
		# sort
		set ::g__GROUP_LIST [lsort $::g__GROUP_LIST]
		foreach x [array names ::g__USER_LIST] {
#			printf "\$::g__USER_LIST($x) = \[[join $::g__USER_LIST($x) ", "]\]: # = [llength $::g__USER_LIST($x)]"
			set ::g__USER_LIST($x) [lsort $::g__USER_LIST($x)]
		}
		# multi group
		set multi_group {}
		foreach x [array names ::g__USER_LIST] {
			if {[lsearch -exact $::g__USER_LIST($x) $m_USER] != -1} {
				lappend multi_group $x
			}
		}
		if {[llength $multi_group] > 1} {
			::utils::SEL_group $multi_group
		}

		set ::g__GROUP $::utils::m__GROUP
		set ::g__USER $m_USER

		return 0
	}
}

namespace import -force utils::printf

########################################################################
########################################################################
