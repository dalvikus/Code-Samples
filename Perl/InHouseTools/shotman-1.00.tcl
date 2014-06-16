#!/usr/local/tcltk-8.3.4/bin/tclsh83.exe

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

set g__CMD CMD
set g__XCACLS //chunji/share/ResourceKit/xcacls
set g__SERVER chunji
set g__ROOT showx
set g__GROUP_LIST {prog td anim}

array set args {}
#			long options	option	OPTION	REQ?	default	order
set args(SHOW)		{--show		-s	{}	2	{}	0}
set args(SHOT)		{--shot		-h	{}	2	{}	1}

set args(GROUP)		{--group	-g	{}	2	{}	10}
set args(USER)		{--user		-u	{}	2	{}	11}
set args(REV)		{--rev		-r	{}	2	{}	12}

set args(RMDIR)		{--remove	-e	{}	0	0	100}
set args(KEEP)		{--keep		-k	{}	0	0	101}
set args(INCLUDE)	{--include	-i	{}	1	{}	102}
set args(EXCLUDE)	{--exclude	-x	{}	1	{}	103}


proc sortCommand {a b} {
	set a4 [lindex $::args($a) 4]
	set b4 [lindex $::args($b) 4]
	if {$a4 < $b4} {
		return -1
	} elseif {$a4 > $b4} {
		return 1
	} else {
		return 0
	}
}

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
		}
		# nextToken is valid?
		if ![regexp -- {^[[:alpha:]](\w|\d)*$} $nextToken] {
			puts stderr "token(\"$nextToken\") NOT valid (\\w(\\w|\\d)*)"
			return
		}
	}

#	puts "option = $opt"
#	puts "nextToken = \"$nextToken\""
#	puts "key = $key"
#	puts "val = $val"
	switch -exact [lindex $val 3] {
		0	{
			set val [lreplace $val 4 4 1]
			set ::args($key) $val
			# continue
		}
		1	{
			if {$nextToken == ""} {
				puts stderr "option $opt: NO value!"
			} else {
				set val [lreplace $val 4 4 $nextToken]
				set ::args($key) $val
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
set x__SHOW {}
set x__SHOT {}

set x__GROUP {}
set x__USER {}

set x__REV {}
########################################################################

proc usage {} {
	puts "$::argv0 --Show SHOW \[--sHot SHOT\] \[--Group GROUP\] \[--User username\] \[--Revision revision\] \[--rEmove \[--Keep \[--Include WHICH --eXclude WHICH\]\]\]"
}




########################################################################
########                ADministrator Level                    #########
########################################################################

######## SHOW ########
# get option for SHOW
set x_OPT [lindex $::args(SHOW) 2]
if {$x_OPT == ""} {
	usage
	return
}
# set g__SHOW
set g__SHOW [lindex $::args(SHOW) 4]
if {$g__SHOW == ""} {
	if ![regexp -- {-S} $x_OPT] {
		# get the shows
		set directory //$g__SERVER/$g__ROOT
		if [file isdirectory $directory] {
			puts "ls $directory/*"
		} else {
			puts stderr "$directory: NOT a directory"
		}
		return
	}
	# use x__SHOW
	if {$x__SHOW == ""} {
		puts "NO x__SHOW"
		return
	}
	set g__SHOW $x__SHOW
}
puts "g__SHOW = \"$g__SHOW\""
######################

######## SHOT ########
# get option for SHOT
set x_OPT [lindex $::args(SHOT) 2]
if {$x_OPT == ""} {
	# get the sequences in g__SHOW
	set directory //$g__SERVER/$g__ROOT/$g__SHOW
	if [file isdirectory $directory] {
		puts "ls $directory/*"
	} else {
		puts stderr "$directory: NOT a directory"
	}
	return
	usage
	return
}
# set g__SHOT
set g__SHOT [lindex $::args(SHOT) 4]
if {$g__SHOT == ""} {
	if ![regexp -- {-H} $x_OPT] {
		# get the sequences in g__SHOW
		set directory //$g__SERVER/$g__ROOT/$g__SHOW
		if [file isdirectory $directory] {
			puts "ls $directory/*"
		} else {
			puts stderr "$directory: NOT a directory"
		}
		return
	}
	# use x__SHOT
	if {$x__SHOT == ""} {
		puts stderr "NO x__SHOT"
		return
	}
	set g__SHOT $x__SHOT
}
# guess g__SEQ from g__SHOT
if ![regexp -- {^([[:alpha:]]+)\d+.*$} $g__SHOT void g__SEQ] {
	# get the shots in g__SHOW
	# in this case, g__SHOT is g__SEQ
	set directory //$g__SERVER/$g__ROOT/$g__SHOW/$g__SHOT
	if [file isdirectory $directory] {
		puts "ls $directory/*"
		return
	}
	puts stderr "shot(\"$g__SHOT\"): cannot guess g__SEQ ((\w+\d+.*))"
	return
}
puts "g__SEQ = \"$g__SEQ\""
puts "g__SHOT = \"$g__SHOT\""
######################

######## DEPARTMENT ########
# get option for DEPARTMENT
set x_OPT [lindex $::args(GROUP) 2]
if {$x_OPT == ""} {
	# make directory with g__SHOW, g__SEQ, g__SHOT
	set directory //$g__SERVER/$g__ROOT/$g__SHOW/$g__SEQ/$g__SHOT
	if [file isdirectory $directory] {
		puts stderr "$directory exists!"
		return
	}
	# make directory and proper subdirectories,
	# and set permission those
	puts "$directory: make (sub)directories and set permission"

	if {[lsearch $g__GROUP_LIST $g__SEQ] != -1} {
		puts $::env(USER)
		puts stder "\$g__SEQ(\"$g__SEQ\"): SHOULD NOT department"
		return
	}

	foreach directory [list //$g__SERVER/$g__ROOT/$g__SHOW //$g__SERVER/$g__ROOT/$g__SHOW/$g__SEQ //$g__SERVER/$g__ROOT/$g__SHOW/$g__SEQ/$g__SHOT //$g__SERVER/$g__ROOT/$g__SHOW/$g__SEQ/$g__SHOT/image //$g__SERVER/$g__ROOT/$g__SHOW/$g__SEQ/$g__SHOT/scene] {
		# make directory
		if [catch {file mkdir $directory} msg] {
			puts stderr "mkdir $directory: error message = \"$msg\""
			return
		}
		puts "mkdir $directory: message = \"$msg\""

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
			puts "mkdir $m_HOME: message = \"$msg\""

			# 2. set permission(permit: group)
			# WINDOWS format since xcacls.exe is WINDOWS program
			regsub -all -- {/} $m_HOME {\\} m_HOME
			if [catch {exec $::g__XCACLS $m_HOME /E /P DIGISCAPEFX\\$m_DEPT:F /Y} msg] {
				puts stderr "xcacls: $m_HOME: error message = \"$msg\""
				return
			}
			puts "xcacls: $m_HOME: message = \"$msg\""
		}
	}
	return
}

########################################################################
########                ADministrator Level                    #########
########################################################################

########################################################################
########                    User Level                         #########
########################################################################

# set g__GROUP
set g__GROUP [lindex $::args(GROUP) 4]
if {$g__GROUP == ""} {
	if ![regexp -- {-G} $x_OPT] {
		# get the departments
		set directory //$g__SERVER/$g__ROOT/$g__SHOW/$g__SHOT/image
		if [file isdirectory $directory] {
			puts "ls $directory/*"
		} else {
			puts stderr "$directory: NOT a directory"
		}
		return
	}
	# use x__GROUP
	if {$x__GROUP == ""} {
		puts stderr "NO x__GROUP"
		return
	}
	set g__GROUP $x__GROUP
}
puts "g__GROUP = \"$g__GROUP\""
############################

######## LAYER ########
# get option for LAYER
set x_OPT [lindex $::args(USER) 2]
if {$x_OPT == ""} {
	# get the users
	set directory //$g__SERVER/$g__ROOT/$g__SHOW/$g__SHOT/image/$g__GROUP
	if [file isdirectory $directory] {
		puts "ls $directory/*"
	} else {
		puts stderr "$directory: NOT a directory"
	}
	return
	usage
	return
}
# set g__USER
set g__USER [lindex $::args(USER) 4]
if {$g__USER == ""} {
	if ![regexp -- {-U} $x_OPT] {
		# get the users
		set directory //$g__SERVER/$g__ROOT/$g__SHOW/$g__SHOT/image/$g__GROUP
		if [file isdirectory $directory] {
			puts "ls $directory/*"
		} else {
			puts stderr "$directory: NOT a directory"
		}
		return
	}
	# use x__USER
	if {$x__USER == ""} {
		puts "NO x__USER"
		return
	}
	set g__USER $x__USER
}
puts "g__USER = \"$g__USER\""
#######################

######## REVISION ########
# get option for REVISION
set x_OPT [lindex $::args(REV) 2]
if {$x_OPT == ""} {
	# get the revisions
	set directory //$g__SERVER/$g__ROOT/$g__SHOW/$g__SHOT/image/$g__GROUP/$g__USER
	if [file isdirectory $directory] {
		puts "ls $directory/*"
	} else {
		puts stderr "$directory: NOT a directory"
	}
	return
	usage
	return
}
# set g__REV
set g__REV [lindex $::args(REV) 4]
if {$g__REV == ""} {
	if ![regexp -- {-R} $x_OPT] {
		# get the revisions
		set directory //$g__SERVER/$g__ROOT/$g__SHOW/$g__SHOT/image/$g__GROUP/$g__USER
		if [file isdirectory $directory] {
			puts "ls $directory/*"
		} else {
			puts stderr "$directory: NOT a directory"
		}
		return
	}
	# use x__REV
	if {$x__REV == ""} {
		puts "NO x__REV"
		return
	}
	set g__REV $x__REV
}
if ![regexp -- {^\w+\d+$} $g__REV] {
	puts stderr "revision(\"$g__REV\"): invalid ((\w+\d+))"
	return
}
puts "g__REV = \"$g__REV\""
##########################

######## REMOVE ########
set x_OPT [lindex $::args(RMDIR) 2]
if {$x_OPT == ""} {
	set directory //$g__SERVER/$g__ROOT/$g__SHOW/$g__SEQ/$g__SHOT/image/$g__GROUP/$g__USER/$g__REV
	if [file isdirectory $directory] {
		puts stderr "$directory exists!"
		return
	}

	# up to 3 revisions
	set directory //$g__SERVER/$g__ROOT/$g__SHOW/$g__SEQ/$g__SHOT/image/$g__GROUP/$g__USER
	set rev_LIST [glob -nocomplain -type d $directory/r*]
	puts "rev. = ([join $rev_LIST ", "])"
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
		puts "mkdir $m_HOME: message = \"$msg\""
		# set permission(user: permit, group: revoke)
		# WINDOWS format since xcacls.exe is WINDOWS program
		regsub -all -- {/} $m_HOME {\\} m_HOME
		if [catch {exec $::g__XCACLS $m_HOME /E /P DIGISCAPEFX\\$g__USER:F /R DIGISCAPEFX\\$g__GROUP /Y} msg] {
			puts stderr "xcacls: $m_HOME: error message = \"$msg\""
			return
		}
		puts "xcacls: $m_HOME: message = \"$msg\""

		# 2. make pub directory in home directory
		set m_PUB $m_HOME/pub
		if [catch {file mkdir $m_PUB} msg] {
			puts stderr "mkdir $m_PUB: error message = \"$msg\""
			return
		}
		puts "mkdir $m_PUB: message = \"$msg\""
	}

	# 2. specific procedure
	# make home directory in image directory
	set m_IMAGE //$g__SERVER/$g__ROOT/$g__SHOW/$g__SEQ/$g__SHOT/image/$g__GROUP/$g__USER
	if ![file isdirectory $m_IMAGE] {
		if [catch {file mkdir $m_IMAGE} msg] {
			puts stderr "mkdir $m_IMAGE: error message = \"$msg\""
			return
		}
		puts "mkdir $m_IMAGE: message = \"$msg\""
		# set permission(user: permit, group: revoke)
		# WINDOWS format since xcacls.exe is WINDOWS program
		regsub -all -- {/} $m_IMAGE {\\} directory
		if [catch {exec $::g__XCACLS $directory /E /P DIGISCAPEFX\\$g__USER:F /R DIGISCAPEFX\\$g__GROUP /Y} msg] {
			puts stderr "xcacls: $m_IMAGE: error message = \"$msg\""
			return
		}
		puts "xcacls: $m_IMAGE: message = \"$msg\""
	}


	# make revision directory
	set m_REV $m_IMAGE/$g__REV
	if [catch {file mkdir $m_REV} msg] {
		puts stderr "mkdir $m_REV: error message = \"$msg\""
		return
	}
	puts "mkdir $m_REV: message = \"$msg\""
	return
}
# is something to save?
set x_OPT [lindex $::args(KEEP) 2]
if {$x_OPT != ""} {
	# backup to where!
}
set directory //$g__SERVER/$g__ROOT/$g__SHOW/$g__SEQ/$g__SHOT/image/$g__GROUP/$g__USER/$g__REV
puts "rm -rf $directory"
if [catch {file delete -force $directory} msg] {
	puts "rm -rf $directory: error message = \"$msg\""
}
puts "rm -rf $directory: message = \"$msg\""
return

########################################################################
########                    User Level                         #########
########################################################################
