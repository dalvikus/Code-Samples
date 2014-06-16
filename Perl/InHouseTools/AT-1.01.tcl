#!/bin/sh
# the next line restarts using wish \
exec tclsh "$0" "$@"

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

set SENDMAIL "notify-1.00"
set SENDMAIL Y:/usr/local/DigiScape/slate/sendMail-1.00.py
set UNIX_BASH "Y:/bin/bash"
set wPYTHON "//chunji/share/Python22/python.exe"
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
	puts $::image_SRC_FORMAT
	# get the file list
	puts "${::vars::directory}/$file.[string repeat ? $::g_image_DIGITs].$::g_image_EXT"
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
	puts join($::g_FRAMEs)
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

proc send_MAIL {} {
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
	puts $FILE_ID "TO: $::gi::m_LAYER"
	puts $FILE_ID "SUBJECT: \[ daily \] - $::vars::daily_date"
	puts $FILE_ID "[subst $m_BODY]"
	if [catch {close $FILE_ID} msg] {
	}
	puts $::LOG_CHANNEL_ID $msg
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
	file rename -force $m_SLATE ${::g_DPSIMG_PATHNAME}0000.TIF

	return 1
}

proc copy_IMAGEs {} {
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

set ::g_isReady [isReady]
if {$::g_isReady == 0} {
} else {
	if {[make_DPS] == 1} {
		copy_IMAGEs
	}
}

send_MAIL

close $::LOG_CHANNEL_ID

return
