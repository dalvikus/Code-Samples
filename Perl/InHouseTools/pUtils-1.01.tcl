########################################################################
########################################################################
namespace eval utils {
	namespace export printf

	proc printf str {
		tk_dialog .dialog {no title} $str {} 0 OK
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
		set m_GROUP {}
		while {[gets $CHANNEL_ID line] != -1} {
			set x_LIST [split $line :]
			if {[llength $x_LIST] != 4} {
				continue
			}
			set group [lindex $x_LIST 0]
			if {$group_ID == [lindex $x_LIST 2]} {
				set m_GROUP $group
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
			set ::g__USER_LIST($group) [concat $x_user([lindex $x_LIST 2]) $y_LIST]
		}
		close $CHANNEL_ID
		if {$m_GROUP == ""} {
			printf "unknown group ID = $group_ID"
			return -1
		}
		# sort
		set ::g__GROUP_LIST [lsort $::g__GROUP_LIST]
		foreach x [array names ::g__USER_LIST] {
#			printf "\$::g__USER_LIST($x) = \[[join $::g__USER_LIST($x) ", "]\]: # = [llength $::g__USER_LIST($x)]"
			set ::g__USER_LIST($x) [lsort $::g__USER_LIST($x)]
		}

		set ::g__GROUP $m_GROUP
		set ::g__USER $m_USER

		return 0
	}
}

namespace import -force utils::printf utils::parse_fi

########################################################################
########################################################################
