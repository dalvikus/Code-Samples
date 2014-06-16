#!/usr/local/tcltk-8.3.4/bin/tclsh83.exe

set VERSION 1.01
set FILE__configuration [file join $::env(HOME) .shotman-$VERSION.ini]

set x__USER {}
set x__GROUP {}
set x__SHOW {}
set x__SEQ {}
set x__SHOT {}

# for given, set the user informations(user, group)
#
# return:
#	-1	error
#	0	ok
proc userinfo {user} {
	# get GROUP ID
	set FILE_ID [open /etc/passwd r]
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
		puts stderr "unknown user = \"$user\""
		return -1
	}

	# get GROUP
	set FILE_ID [open /etc/group r]
	set group {}
	while {[gets $FILE_ID line] != -1} {
		set group_LIST [split $line :]
		if {$group_ID == [lindex $group_LIST 2]} {
			set group [lindex $group_LIST 0]
		}
	}
	close $FILE_ID
	if {$group == ""} {
		puts stderr "unknown group ID = $group_ID"
		return -1
	}

	set ::x__USER $user
	set ::x__GROUP $group
	return 0
}

# get and confirm variables
#
# return:
#	-1	error
#	0	ok
proc get_VARs {} {
	# get "USER" environment variable
	if [catch {set user $::env(USER)} msg] {
		puts stderr $msg
		return -1
	}

	# set GROUP with USER
	if {[userinfo $user] == -1} {
		return -1
	}
	puts stdout "USER = \"$::x__USER\", GROUP = \"$::x__GROUP\""
	puts -nonewline stdout "> Is it correct? (Y/N) "
	flush stdout
	gets stdin x
	if {[string toupper $x] != "Y"} {
		return -1
	}
	puts -nonewline stdout "> What's the name of SHOW, SHOT? "
	flush stdout
	gets stdin x
	if ![regexp -- {^\s*([[:alpha:]]+)\s*,\s*(([[:alpha:]]+)\d+\w*)\s*$} $x void ::x__SHOW ::x__SHOT ::x__SEQ] {
		puts stderr "\"$x\": NOT valid!"
		return -1
	}
	puts stdout "SHOW = \"$::x__SHOW\", SEQ = \"$::x__SEQ\", SHOT = \"$::x__SHOT\""
	puts -nonewline stdout "> Is it correct? (Y/N) "
	flush stdout
	gets stdin x
	if {[string toupper $x] != "Y"} {
		return -1
	}
}

# proceed the configuration file
#
# return:
#	-1	error
#	0	ok
proc scan_VARs {pathname} {
	if [catch {set rs [uplevel #0 source $pathname]} msg] {
		puts stderr $msg
		return -1
	}
	if {$rs != "OK"} {
		puts stderr "return value: NOT OK!"
		return -1
	}

# Validate the variables
	# 1. user's informations
	# USER
	if {$::x__USER == ""} {
		puts stderr "NO USER"
		return -1
	}
	# GROUP
	if {$::x__GROUP == ""} {
		puts stderr "NO GROUP"
		return -1
	}
	# Contradict?
	set group $::x__GROUP
	# set GROUP with USER
	if {[userinfo $::x__USER] == -1} {
		return -1
	}
	if {$group != $::x__GROUP} {
		puts stderr "USER(\"$::x__USER\") does NOT match GROUP(\"$group\")!"
		return -1
	}

	# 2. show informations
	# SHOW
	if ![regexp -- {^[[:alpha:]]+$} $::x__SHOW] {
		puts stderr "SHOW(\"$::x__SHOW\"): NOT valid!"
		return -1
	}
	# SHOT
	if ![regexp -- {^([[:alpha:]]+)\d+.*$} $::x__SHOT void m__SEQ] {
		puts stderr "SHOT(\"$x\"): NOT valid!"
		return -1
	}
	# Contradict?
	if {$::x__SEQ != $m__SEQ} {
		puts stderr "SHOT(\"$::x__SHOT\") does NOT match SEQ(\"$::x__SEQ\")"
		return -1
	}

	return 0
}


if [file exists $::FILE__configuration] {
	if {[scan_VARs $::FILE__configuration] == -1} {
		puts stderr "Configuration file(\"$::FILE__configuration\") is NOT valid!"
		return
	}
} else {
	# get variables
	if {[get_VARs] == -1} {
		return
	}
	# save the variables
	if [catch {set CHANNEL_ID [open $::FILE__configuration w]} msg] {
		puts stderr "open: error message = \"$msg\""
		return
	}
	puts $CHANNEL_ID "set x__USER $::x__USER"
	puts $CHANNEL_ID "set x__GROUP $::x__GROUP"
	puts $CHANNEL_ID "set x__SHOW $::x__SHOW"
	puts $CHANNEL_ID "set x__SEQ $::x__SEQ"
	puts $CHANNEL_ID "set x__SHOT $::x__SHOT"
	puts $CHANNEL_ID "\nreturn OK"
	if [catch {close $CHANNEL_ID} msg] {
		puts stderr "close: error message = \"$msg\""
		return
	}
}
##puts "USER = \"$::x__USER\", GROUP = \"$::x__GROUP\""
##puts "SHOW = \"$::x__SHOW\", SEQ = \"$::x__SEQ\", SHOT = \"$::x__SHOT\""

# E:\
# E:\fs					: ADministrator
# E:\fs\sd				: ADministrator
# E:\fs\sd\sd1				: ADministrator
# E:\fs\sd\sd1\image			: ADministrator
# E:\fs\sd\sd1\scene			: ADministrator
# E:\fs\sd\sd1\image\prog		: prog
# E:\fs\sd\sd1\image\prog\im4u		: im4u
# E:\fs\sd\sd1\image\prog\im4u\r1	: im4u
# E:\fs\sd\sd1\scene\prog		: prog

set g__XCACLS //chunji/share/ResourceKit/xcacls
set g__SERVER chunji
set g__DOMAIN DIGISCAPEFX
set g__ROOT showx

set g__XCACLS xcacls
set g__DOMAIN WINDOWS2000S
set g__SERVER windows2000s
set g__ROOT show

set g__GROUP_LIST {prog td anim}

array set args {}
#			long options	option	OPTION	REQ?	default	order
set args(SHOW)		{--show		-s	{}	2	{}	0}
set args(SHOT)		{--shot		-h	{}	2	{}	1}

set g__USER $x__USER
set g__GROUP $x__GROUP
set args(REV)		{--rev		-r	{}	2	{}	10}

set ix_FILEs {}
set args(RMDIR)		{--remove	-e	{}	0	0	100}
set args(KEEP)		{--keep		-k	{}	1	0	101}
set args(INCLUDE)	{--include	-i	{}	1	{}	102}
set args(EXCLUDE)	{--exclude	-x	{}	1	{}	103}


proc whichOption {OPT} {
	set opt [string tolower $OPT]
	foreach x [array names ::args] {
		if {$opt == [lindex $::args($x) 0]} {
			# long option
			return $x
		}
		if {$opt == [lindex $::args($x) 1]} {
			# short option
			return $x
		}
	}

	# NOT option
	return ""
}

for {set i 0} {$i < [llength $argv]} {incr i} {
	set opt [lindex $argv $i]
	if {[string range $opt 0 0] != "-"} {
		puts stderr "warning...: \"$opt\""
		continue
	}

	set key [whichOption $opt]
	if {$key == ""} {
		puts stderr "unknown option: $opt"
		continue
	}
	set val [lreplace $::args($key) 2 2 $opt]
	set ::args($key) $val

	set nextToken {}
	if {[expr $i + 1] != [llength $argv]} {
		set nextToken [lindex $argv [expr $i + 1]]
		if {[string range $nextToken 0 0] == "-"} {
			set nextToken {}
		} else {
			# nextToken is valid?
			if ![regexp -- {^[[:alpha:]](\w|\d)*$} $nextToken] {
				puts stderr "$opt: token(\"$nextToken\") NOT valid (\\w(\\w|\\d)*)"
				return
			}
		}
	}

##	puts "option = $opt"
##	puts "nextToken = \"$nextToken\""
##	puts "key = $key"
##	puts "val = $val"
	switch -exact [lindex $val 3] {
		0	{
			set val [lreplace $val 4 4 1]
			set ::args($key) $val
			# continue
		}
		1	{
			if {$nextToken == ""} {
				puts stderr "option $opt: NO value!"
				return
			} else {
				if {$key == "INCLUDE" || $key == "EXCLUDE"} {
					set ::ix_FILEs [linsert $::ix_FILEs -1 [list $nextToken $key]]
				} else {
					set val [lreplace $val 4 4 $nextToken]
					set ::args($key) $val
				}
				incr i
			}
		}
		default	{
			if {$nextToken != ""} {
				set val [lreplace $val 4 4 $nextToken]
				set ::args($key) $val
				incr i
			}
		}
	}
}

########################################################################
########################################################################

proc usage {} {
	puts stdout "$::argv0 --Show SHOW \[--sHot SHOT\] \[--Group GROUP\] \[--User username\] \[--Revision revision\] \[--rEmove \[--Keep \[--Include WHICH --eXclude WHICH\]\]\]"
}




########################################################################
########                ADministrator Level                    #########
########################################################################

######## SHOW ########
# get option for SHOW
set x_OPT [lindex $::args(SHOW) 2]
if {$x_OPT != ""} {
	# set ::g__SHOW
	set ::g__SHOW [lindex $::args(SHOW) 4]
	if {$::g__SHOW == ""} {
		if ![regexp -- {-S} $x_OPT] {
			# get the shows
			set directory //$::g__SERVER/$::g__ROOT
			if [file isdirectory $directory] {
				puts stdout "ls $directory/*"
			} else {
				puts stderr "$directory: NOT a directory"
			}
			return
		}
	}
	# validate ::g__SHOW
	if ![regexp -- {^[[:alpha:]]+$} $::g__SHOW] {
		puts stderr "SHOW(\"$::g__SHOW\"): NOT valid!"
		return
	}
} else {
	# use OLD variable
	set ::g__SHOW $::x__SHOW
}
##puts "g__SHOW = \"$::g__SHOW\""
######################

######## SHOT ########
# get option for SHOT
set x_OPT [lindex $::args(SHOT) 2]
if {$x_OPT != ""} {
	# set ::g__SHOT
	set ::g__SHOT [lindex $::args(SHOT) 4]
	if {$::g__SHOT == ""} {
		# get the shots in $::g__SHOW/$::x__SEQ
		set directory //$::g__SERVER/$::g__ROOT/$::g__SHOW/$::x__SEQ
		if [file isdirectory $directory] {
			puts stdout "ls $directory/*"
		} else {
			puts stderr "$directory: NOT a directory"
		}
		return
	}
	# validate ::g__SHOT
	# guess ::g__SEQ from ::g__SHOT
	if ![regexp -- {^([[:alpha:]]+)\d+.*$} $::g__SHOT void ::g__SEQ] {
		# get the shots in ::g__SHOW
		# in this case, ::g__SHOT is ::g__SEQ
		set directory //$::g__SERVER/$::g__ROOT/$::g__SHOW/$::g__SHOT
		if [file isdirectory $directory] {
			puts stdout "ls $directory/*"
			return
		}
		puts stderr "SHOT(\"$::g__SHOT\"): cannot guess g__SEQ ((\w+\d+.*))"
		return
	}
} else {
	# use OLD variables
	set ::g__SEQ $::x__SEQ
	set ::g__SHOT $::x__SHOT
}
##puts "g__SEQ = \"$::g__SEQ\""
##puts "g__SHOT = \"$::g__SHOT\""
######################

######## REVISION ########
# get option for REVISION
set x_OPT [lindex $::args(REV) 2]
if {$x_OPT == ""} {
	puts stderr "Message: Only administrator can do."
##	puts "USER = \"$::x__USER\", GROUP = \"$::x__GROUP\""

	# make directory with g__SHOW, g__SEQ, g__SHOT
	set directory //$g__SERVER/$g__ROOT/$g__SHOW/$g__SEQ/$g__SHOT
	if [file isdirectory $directory] {
		puts stderr "$directory exists!"
		return
	}
	# make directory and proper subdirectories,
	# and set permission those
	puts stdout "$directory: make (sub)directories and set permission"

	if {[lsearch $g__GROUP_LIST $g__SEQ] != -1} {
		puts $::env(USER)
		puts stderr "\$g__SEQ(\"$g__SEQ\"): SHOULD NOT department"
		return
	}

	foreach directory [list //$g__SERVER/$g__ROOT/$g__SHOW //$g__SERVER/$g__ROOT/$g__SHOW/$g__SEQ //$g__SERVER/$g__ROOT/$g__SHOW/$g__SEQ/$g__SHOT //$g__SERVER/$g__ROOT/$g__SHOW/$g__SEQ/$g__SHOT/image //$g__SERVER/$g__ROOT/$g__SHOW/$g__SEQ/$g__SHOT/scene] {
		# make directory
		if [catch {file mkdir $directory} msg] {
			puts stderr "mkdir $directory: error message = \"$msg\""
			return
		}
		puts stdout "mkdir $directory: message = \"$msg\""

		# make directory for departments
		foreach m_DEPT $g__GROUP_LIST {
			# for given department
			set m_HOME $directory/$m_DEPT

			# if home already exists, skip!
			if [file isdirectory $m_HOME] {
				continue
			}

			# 1. make directories for home
			if [catch {file mkdir $m_HOME} msg] {
				puts stderr "mkdir $m_HOME: error message = \"$msg\""
				return
			}
			puts stdout "mkdir $m_HOME: message = \"$msg\""

			# 2. set permission(permit: group)
			# WINDOWS format since xcacls.exe is WINDOWS program
			regsub -all -- {/} $m_HOME {\\} m_HOME
			if [catch {exec $::g__XCACLS $m_HOME /E /P $g__DOMAIN\\$m_DEPT:F /Y} msg] {
				puts stderr "xcacls: $m_HOME: error message = \"$msg\""
				return
			}
			puts stdout "xcacls: $m_HOME: message = \"$msg\""
		}
	}
##	puts "USER = \"$::x__USER\", GROUP = \"$::x__GROUP\""
##	puts "SHOW = \"$::x__SHOW\", SEQ = \"$::x__SEQ\", SHOT = \"$::x__SHOT\""
	return
}

########################################################################
########                ADministrator Level                    #########
########################################################################

puts stderr "Message: Only users can do."
########################################################################
########                    User Level                         #########
########################################################################

# set g__REV
set g__REV [lindex $::args(REV) 4]
if {$g__REV == ""} {
	# get the revisions
	set directory //$g__SERVER/$g__ROOT/$g__SHOW/$g__SHOT/image/$g__GROUP/$g__USER
	if [file isdirectory $directory] {
		puts stdout "ls $directory/*"
	} else {
		puts stderr "$directory: NOT a directory"
	}
	return
}
if ![regexp -- {^r\d+$} $g__REV] {
	puts stderr "revision(\"$g__REV\"): invalid ((r\d+))"
	return
}
puts "g__REV = \"$g__REV\""
##########################

######## REMOVE ########
set x_OPT [lindex $::args(RMDIR) 2]
if {$x_OPT == ""} {
	# make directory for revision

	set directory //$g__SERVER/$g__ROOT/$g__SHOW/$g__SEQ/$g__SHOT/image/$g__GROUP/$g__USER/$g__REV
	if [file isdirectory $directory] {
		puts stderr "$directory exists!"
		return
	}

	# up to 3 revisions
	set directory //$g__SERVER/$g__ROOT/$g__SHOW/$g__SEQ/$g__SHOT/image/$g__GROUP/$g__USER
	set rev_LIST [glob -nocomplain -type d $directory/r*]
	puts stdout "rev. = ([join $rev_LIST ", "])"
	if {[llength $rev_LIST] >= 3} {
		puts stderr "# of revisions = [llength $rev_LIST]: at most 3 revisions allowed"
		return
	}


	# 1. common procedure
	# make home and therein pub directory
	foreach m_HOME [list //$g__SERVER/$g__ROOT/$g__SHOW/$g__GROUP/$g__USER //$g__SERVER/$g__ROOT/$g__SHOW/$g__SEQ/$g__GROUP/$g__USER //$g__SERVER/$g__ROOT/$g__SHOW/$g__SEQ/$g__SHOT/$g__GROUP/$g__USER] {
		if [file isdirectory $m_HOME] {
			continue
		}

		# 1. make home directory
		if [catch {file mkdir $m_HOME} msg] {
			puts stderr "mkdir $m_HOME: error message = \"$msg\""
			return
		}
		puts stdout "mkdir $m_HOME: message = \"$msg\""
		# set permission(user: permit, group: revoke)
		# WINDOWS format since xcacls.exe is WINDOWS program
		regsub -all -- {/} $m_HOME {\\} m_HOME
		if [catch {exec $::g__XCACLS $m_HOME /E /P $g__DOMAIN\\$g__USER:F /R $g__DOMAIN\\$g__GROUP /Y} msg] {
			puts stderr "xcacls: $m_HOME: error message = \"$msg\""
			return
		}
		puts stdout "xcacls: $m_HOME: message = \"$msg\""

		# 2. make pub directory in home directory
		set m_PUB $m_HOME/pub
		if [catch {file mkdir $m_PUB} msg] {
			puts stderr "mkdir $m_PUB: error message = \"$msg\""
			return
		}
		puts stdout "mkdir $m_PUB: message = \"$msg\""
	}

	# 2. specific procedure
	# make home directory in image directory
	set m_IMAGE //$g__SERVER/$g__ROOT/$g__SHOW/$g__SEQ/$g__SHOT/image/$g__GROUP/$g__USER
	if ![file isdirectory $m_IMAGE] {
		if [catch {file mkdir $m_IMAGE} msg] {
			puts stderr "mkdir $m_IMAGE: error message = \"$msg\""
			return
		}
		puts stdout "mkdir $m_IMAGE: message = \"$msg\""
		# set permission(user: permit, group: revoke)
		# WINDOWS format since xcacls.exe is WINDOWS program
		regsub -all -- {/} $m_IMAGE {\\} directory
		if [catch {exec $::g__XCACLS $directory /E /P $g__DOMAIN\\$g__USER:F /R $g__DOMAIN\\$g__GROUP /Y} msg] {
			puts stderr "xcacls: $m_IMAGE: error message = \"$msg\""
			return
		}
		puts stdout "xcacls: $m_IMAGE: message = \"$msg\""
	}


	# make revision directory
	set m_REV $m_IMAGE/$g__REV
	if [catch {file mkdir $m_REV} msg] {
		puts stderr "mkdir $m_REV: error message = \"$msg\""
		return
	}
	puts stdout "mkdir $m_REV: message = \"$msg\""
	return
}
# is something to save?
set x_OPT [lindex $::args(KEEP) 2]
if {$x_OPT != ""} {
	set dest_directory [lindex $::args(KEEP) 4]/${g__SHOW}_${g__SEQ}_${g__SHOT}__image_${g__GROUP}_${g__USER}_${g__REV}
	if [catch {file mkdir $dest_directory} msg] {
		puts stderr "mkdir $directory: error message = \"$msg\""
		return
	}
	set directory //$g__SERVER/$g__ROOT/$g__SHOW/$g__SEQ/$g__SHOT/image/$g__GROUP/$g__USER/$g__REV
	foreach dir [glob -nocomplain -type d $directory/*] {
		regsub -all -- {\\} $dir {/} dir
		foreach ix_FILE $::ix_FILEs {
			if [regexp -- [lindex $ix_FILE 0] [file tail $dir]] {
				if {[lindex $ix_FILE 1] == "INCLUDE"} {
					if [catch {file rename -force $dir $dest_directory} msg] {
						puts stderr "mv $dir $dest_directory: error message = \"$msg\""
						return
					}
					puts stderr "mv $dir $dest_directory: message = \"$msg\""
				} else {
					if [catch {file delete -force $directory} msg] {
						puts stderr "rm -rf $directory: error message = \"$msg\""
						return
					}
					puts stdout "rm -rf $directory: message = \"$msg\""
				}
			}
		}
	}
}
set directory //$g__SERVER/$g__ROOT/$g__SHOW/$g__SEQ/$g__SHOT/image/$g__GROUP/$g__USER/$g__REV
puts stdout "rm -rf $directory"
if [catch {file delete -force $directory} msg] {
	puts stderr "rm -rf $directory: error message = \"$msg\""
}
puts stdout "rm -rf $directory: message = \"$msg\""
return

########################################################################
########                    User Level                         #########
########################################################################
