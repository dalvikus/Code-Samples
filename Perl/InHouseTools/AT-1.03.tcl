#!/usr/local/tcltk-8.3.4/bin/wish83.exe

set LOG_CHANNEL_ID [open "C:/XXX.LOG" w]

if {[llength $argv] != 1} {
	puts "$argv0 xxx.ini"
	puts $::LOG_CHANNEL_ID "$argv0 xxx.ini"
	close $::LOG_CHANNEL_ID
	return
}
if {[uplevel #0 source [lindex $argv 0]] != "OK"} {
	puts $::LOG_CHANNEL_ID "!OK"
	close $::LOG_CHANNEL_ID
	return
}

set ALFRED alfred
set SHAKE shake
set SENDMAIL notify-1.00
set SENDMAIL Y:/usr/local/DigiScape/slate/sendMail-1.00.py
set UNIX_BASH Y:/bin/bash
set UNIX_CP Y:/bin/cp
set UNIX_MV Y:/bin/mv
set wPYTHON //chunji/share/Python22/python.exe
set DPS_SERVER //dps1/daily/
set DPS_MEDIA_UTILITY //chunji/share/DPS/DPSMediaUtility
set MKSLATE_SRC Y:/usr/local/DigiScape/slate/
set MKSLATE mkslate-1.04.py


set g_DPS_PATHNAME {}
proc isReady {} {
	# set message list for ERROR
	set ::g_msg {}

	# with the filename information(barename.n_digits.extension)
	# Q: is it validated? A: assuming...
	regexp -- {^\s*(\S+)\.([#]+)\.(\S+)\s*$} $::vars::filename null file n_HASH ::g_image_EXT
	set ::g_image_DIGITs [string length $n_HASH]
	set ::image_SRC_FORMAT "${::vars::directory}$file.\[format %0${::g_image_DIGITs}d \$x__fi\].$::g_image_EXT"
#	puts $::image_SRC_FORMAT
	# get the file list
#	puts "${::vars::directory}/$file.[string repeat ? $::g_image_DIGITs].$::g_image_EXT"
	set filelist [lsort [glob -nocomplain "${::vars::directory}/$file.[string repeat ? $::g_image_DIGITs].$::g_image_EXT"]]
	# assuming VALID frame information
	regexp -- {^\s*(\d+)\s*-\s*(\d+)\s*$} $::vars::frame_range x ::g_FRAME_START ::g_FRAME_END
	# get image list
	set ::g_FRAMEs {}
	foreach file $filelist {
		regexp -- {\.([^\.]+)\.[^\.]+$} $file x x_fi
		set x_fi [string trimleft $x_fi 0]
		if {$x_fi >= $::g_FRAME_START && $x_fi <= $::g_FRAME_END} {
			lappend ::g_FRAMEs $x_fi
		}
	}
	# get missing image list
#	puts join($::g_FRAMEs)
	set ::g_missing_FRAMEs {}
	for {set i $::g_FRAME_START} {$i <= $::g_FRAME_END} {incr i} {
		if {[lsearch -exact $::g_FRAMEs $i] == -1} {
			lappend ::g_missing_FRAMEs $i
		}
	}
	if {[llength $::g_FRAMEs] == 0} {
		puts "no images"
		puts $::LOG_CHANNEL_ID "no images"
		lappend ::g_msg "no images"
		return 0
	}
	# apply MINIMUM # of images
	set x $::vars::minimum_frames
	if {$x == "" || $x == 0} {
		# OK
		return 1
	}
	# $x: non-zero integer
	if {[llength $::g_FRAMEs] < $x} {
		puts "not enough images([llength $::g_FRAMEs] < $x)"
		puts $::LOG_CHANNEL_ID "not enough images([llength $::g_FRAMEs] < $x)"
		lappend ::g_msg "not enough images([llength $::g_FRAMEs] < $x)"
		return 0
	}

	return 1
}

proc make_DPS {} {
# 1. make DPS file
	set m_DPS_PATH ${::DPS_SERVER}DPS/daily/${::vars::m_show}/
	# make DPS folder
	file mkdir $m_DPS_PATH
	set m_DPS_FILENAME $::vars::daily_date-$::gi::m_DEPT-$::gi::m_LAYER-$::vars::m_show.$::vars::m_shot
	# look for last sequence no. with the same SHOW and SHOT name
#	puts $m_DPS_PATH
#	puts $m_DPS_FILENAME

	set filelist [glob -nocomplain $m_DPS_PATH*-$::vars::m_show.$::vars::m_shot.???.dps]
#	puts $filelist
	set ix 0
	foreach file $filelist {
		if [regexp -- {\.(\d\d\d)\.dps$} $file x i] {
			# what if ix = "008" => invalid octal number ???
			# remove leading '0'
			set i [string trimleft $i 0]
			if {$i > $ix} {
				set ix $i
			}
		}
	}
#	puts $ix
	#!!! race condition?
	set ::g_DPS_FILENAME $m_DPS_FILENAME.[format %03d [expr $ix + 1]].dps
#	puts $::g_DPS_FILENAME
	set ::g_DPSIMG_PATHNAME ${::DPS_SERVER}[string toupper ${::g_image_EXT}]/daily/${::vars::m_show}/${::g_DPS_FILENAME}/[file rootname $::g_DPS_FILENAME]
	if {[string toupper $::g_image_EXT] == "PIC"} {
		set ::g_DPSIMG_PATHNAME $::g_DPSIMG_PATHNAME.
	}
#	puts $::g_DPSIMG_PATHNAME

	# execute DPSMediaUtility.EXE
	set ::g_DPS_PATHNAME $m_DPS_PATH$::g_DPS_FILENAME
#	puts $::g_DPS_PATHNAME
	if [catch {exec $::DPS_MEDIA_UTILITY -batch -d[expr 1 + [llength $::g_FRAMEs]] -f$::g_DPS_PATHNAME} msg] {
		set ::g_msg $msg
		puts $::LOG_CHANNEL_ID $msg
		return 0
	}

# 2. make SLATE and move to DPS
	set m_SLATE [lindex $::argv 0].TIF
	if [catch {eval set PIPE_ID [open "| $::wPYTHON ${::MKSLATE_SRC}${::MKSLATE} $m_SLATE" w]} msg] {
		set ::g_msg $msg
		puts $::LOG_CHANNEL_ID $msg
		return 0
	}
	foreach xxx [info vars ::vars::*] {
		if ![regexp -- {^::vars::(m_.*)$} $xxx x m_xxx] {
			continue
		}
		upvar ::vars::$m_xxx ref_m_xxx
		puts $PIPE_ID "$m_xxx: $ref_m_xxx"
	}
	if [catch {close $PIPE_ID} msg] {
		set ::g_msg $msg
		puts $::LOG_CHANNEL_ID $msg
		return 0
	}
	file rename -force $m_SLATE ${::g_DPSIMG_PATHNAME}0000.$::g_image_EXT

	return 1
}

proc copy_IMAGEs {} {
	set job_SCRIPT [lindex $::argv 0].job
# 1. open JOB file for writing
	if [catch {eval set FILE_ID [open $job_SCRIPT w]} msg] {
		set ::g_msg $msg
		return 0
	}

# 2. write ALFRED job script
	#### [ALFRED BEGIN.
	set m_ALFRED__BEGIN \
"##AlfredToDo 3.0

#set AlfJobPause 1

Job -title {$::g_DPS_PATHNAME} -crews {$::vars::m__crews} -comment {
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
	foreach x__fi $::g_FRAMEs {
		incr n__fi
		# without frame number
		set m_image_x [subst $::image_SRC_FORMAT]
		# temporary image
		set m_image_o $m_image_x-
		# without sequence number
		set m_image_in_DPS "$::g_DPSIMG_PATHNAME[format %04d $n__fi].[string toupper $::g_image_EXT]"

		set m_FRAME___HEAD \
"		Task -title {[format %04d $x__fi]} -subtasks \{
"
		puts -nonewline $FILE_ID [subst $m_FRAME___HEAD]

		# making TAB sign
		set m_bg $::vars::bg
		if {$m_bg != ""} {
			if [regexp -- {^(.*?)\.(#+)\.(.*?)$} $m_bg x ax mx px] {
				set m_bg "$ax.\[format %0[string length $mx]d \$x__fi\].$px"
				set m_bg [subst $m_bg]
			}
			set m_bg " -over $m_bg"
		}
		set m_make_TAB_sign \
{			# making TAB sign: [format %04d $x__fi]
			Task -title {[format %04d $x__fi]: making TAB sign} -subtasks {
				Instance {WORK - BODY -- preflight}
			} -cmds {
				RemoteCmd {
					$::SHAKE $m_image_x$m_bg -addtext \"$::vars::m_show[string toupper $::vars::m_shot 0 0][file extension [file rootname $::g_DPS_PATHNAME]] $::gi::m_LAYER $x__fi\" Arial 24 24 1 32 32 2 1 -setalpha 0 -fo $m_image_o tiff
				} -service {shake} -atmost {1}
			} -cleanup {
			} -chaser {
			}
}
		puts -nonewline $FILE_ID [subst $m_make_TAB_sign]

		# copying to DPS
		set m_copy_to_DPS \
{			# copying to DPS: [format %04d $n__fi]
			Task -title {[format %04d $n__fi]: copying to DPS} -subtasks {
				Task -title {[format %04d $x__fi]: copying to DPS - $n__fi} -subtasks {
					Instance {[format %04d $x__fi]: making TAB sign}
				} -cmds {
					Cmd {
						$::UNIX_MV $m_image_o $m_image_in_DPS
					} -tags {DPSCopy}
				} -cleanup {
				} -chaser {
				}
			} -cmds {
			} -cleanup {
			} -chaser {
			}
}
		puts -nonewline $FILE_ID [subst $m_copy_to_DPS]

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
	set m_MSG \
{FROM: $::gi::m_LAYER@digiscapefx.com
TO: $::vars::mail_TO
SUBJECT: \\\[ daily \\\] - $::vars::daily_date
DPS = $::g_DPS_PATHNAME (STATUS: OK)
# of frames = $::g_FRAME_START - $::g_FRAME_END \\\[[expr $::g_FRAME_END - $::g_FRAME_START + 1]\([expr $::g_FRAME_END - $::g_FRAME_START + 1 - [llength $::g_missing_FRAMEs]] + [llength $::g_missing_FRAMEs]\)\\\]
missing frames = \\\[[join $::g_missing_FRAMEs ", "]\\\]
.}
	set m_WORK__TAIL \
"#	<TAIL>
	Task -title {WORK - TAIL} -subtasks {
		Instance {WORK - BODY}
	} -cmds {
		Cmd {
			$::UNIX_BASH --login -c 'echo \"$m_MSG\" | $::SENDMAIL'
		}
	} -cleanup {
		Cmd {
			$::UNIX_BASH --login -c 'rm -f [lindex $::argv 0] $job_SCRIPT'
		}
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

	# when
	set NOW [clock scan now]
	if [catch {eval set AT [clock scan $::vars::aT]} msg] {
		printf $msg
		return
	}
	# run alfred job script
	if {$AT != "NOW" && [expr $AT - $NOW] > 0} {
		set AFTER "[clock format $AT -format "%m %d %H:%M"]"
		if [catch {eval exec -- \"$::ALFRED\" -after \"$AFTER\" $job_SCRIPT &} msg] {
			set ::g_msg $msg
		}
	} else {
		if [catch {eval exec -- \"$::ALFRED\" $job_SCRIPT &} msg] {
			set ::g_msg $msg
		}
	}
	return 1

	## using it, but...
# copy images
	# $x__fi
#	puts $::image_SRC_FORMAT
	# make IT combiner file
	set IT_CMB [lindex $::argv 0].CMB
#	puts $IT_CMB
	set IT__FILE_ID [open $IT_CMB w]
	set m_IT_FILE \
{
set image_SRC_FORMAT {$::image_SRC_FORMAT}
set FRAMEs {[join $::g_FRAMEs]}
set n__fi 0
foreach x__fi \$FRAMEs {
	set n__fi \[expr \$n__fi + 1\]

	# without frame number
	set m_IMG_o \[subst \$::image_SRC_FORMAT\]
	# temporary image
	set m_IMG__ \${m_IMG_o}-
	# without sequence number
	set m_IMG_in_DPS "$::g_DPSIMG_PATHNAME\[format %04d \$n__fi\].[string toupper $::g_image_EXT]"

	# combiner
	combiner setExpressionIndex 1
	combiner removeElement e0
	combiner addElement image "\$m_IMG_o" {e0}
	set x "title\\\[\\\"$::vars::m_shot[file extension [file rootname $::g_DPS_PATHNAME]] $::gi::m_LAYER \[format %04d \$x__fi\]\\\", 10, 440\] over \\\$e0 save\\\[\\\"\$m_IMG__\\\"\\\]"
	combiner setExpression 1 \$x
	combiner doCombine \$x__fi
	file rename -force \$m_IMG__ \$m_IMG_in_DPS
}
}
	# write CMB script
	puts -nonewline $IT__FILE_ID [subst $m_IT_FILE]

	close $IT__FILE_ID

#	puts $IT_CMB
	set rc [catch {eval exec it -combinerFile \"$IT_CMB\" -hide} msg]
	if {$rc != 0} {
		lappend ::msg $msg
	}
#	file delete -force $IT_CMB
}

proc send_MAIL__FAILED {} {
	set m_BODY \
{message: [join $::g_msg "\n\t"]
Ready? = $::g_isReady
DPS = $::g_DPS_PATHNAME
# of frames = $::g_FRAME_START - $::g_FRAME_END \[[expr $::g_FRAME_END - $::g_FRAME_START + 1]\([expr $::g_FRAME_END - $::g_FRAME_START + 1 - [llength $::g_missing_FRAMEs]] + [llength $::g_missing_FRAMEs]\)\]
missing frames = \[[join $::g_missing_FRAMEs ", "]\]
}
	if [catch {eval set FILE_ID [open "| $::wPYTHON $::SENDMAIL" w]} msg] {
		lappend ::g_msg $msg
		puts $::LOG_CHANNEL_ID $msg
		return
	}
	puts $FILE_ID "FROM: $::gi::m_LAYER@digiscapefx.com"
	puts $FILE_ID "TO: $::vars::mail_TO"
	puts $FILE_ID "SUBJECT: \[ daily \] - $::vars::daily_date"
	puts $FILE_ID "[subst $m_BODY]"
	if [catch {close $FILE_ID} msg] {
	}
	puts $::LOG_CHANNEL_ID $msg
}

set ::g_isReady [isReady]
if {$::g_isReady == 0} {
} else {
	if {[make_DPS] == 1} {
		if {[copy_IMAGEs] == 1} {
			# OK
		}
	}
}
#
#send_MAIL

puts "DPS = '$g_DPS_PATHNAME'"
close $::LOG_CHANNEL_ID
return
