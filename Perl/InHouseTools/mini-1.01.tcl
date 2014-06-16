#!/bin/sh
# the next line restarts using wish \
exec wish "$0" "$@"

option add *font {Arial 14} userDefault
set fixedFont {{Lucida Console} 14}
wm title . "mini DigiScape Template"


catch {uplevel #0 source [file join $::env(HOME) .mini.ini]}

namespace eval vars {
}
########################################################################
####                                                                ####
set ROOTPATH Y:/usr/local/bin/
set TCLSH Y:/usr/local/bin/TCLSH.EXE
set PASSWD Y:/etc/passwd
set GROUP Y:/etc/group
set prog_AT AT.tcl
set prog__wAT C:/WINNT/system32/AT.EXE
namespace eval gi {
}
uplevel #0 source [file join $::ROOTPATH utils.tcl]
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

set imageInfoLabelWidth 16
set imageLabelFont {Arial 14}
set imageLabelFgColor #0066CC
set slateInfoLabelWidth 12
set slateLabelFont {Arial 14}
set slateLabelFgColor #0066CC
set requiredFgColor #CC0066
set optionalFgColor #808080
set disabledBgColor #A0B7CE

proc chooseDirectory {w_entry} {
	set dir [tk_chooseDirectory \
		-initialdir . \
		-parent . \
		-title "select directory" \
		-mustexist true \
	]

	$w_entry delete 0 end
	$w_entry insert end $dir
}

frame .imageInfo -borderwidth 1 -relief solid
	frame .imageInfo.head -borderwidth 1 -relief solid
	frame .imageInfo.body
		frame .imageInfo.body.directory
		frame .imageInfo.body.filename
		frame .imageInfo.body.frames
		frame .imageInfo.body.minimumFrames
		frame .imageInfo.body.aT
frame .slateInfo -borderwidth 1 -relief solid
	frame .slateInfo.head -borderwidth 1 -relief solid
	frame .slateInfo.body
		frame .slateInfo.body.daily_date
		frame .slateInfo.body.show
		frame .slateInfo.body.shot
		frame .slateInfo.body.date
		frame .slateInfo.body.creator
		frame .slateInfo.body.mm
		frame .slateInfo.body.anim
		frame .slateInfo.body.fx
		frame .slateInfo.body.td
		frame .slateInfo.body.roto
		frame .slateInfo.body.comp
		frame .slateInfo.body.coord
		frame .slateInfo.body.bg
		frame .slateInfo.body.cameramove
		frame .slateInfo.body.pixs_srate
		frame .slateInfo.body.audio
		frame .slateInfo.body.filmio
		frame .slateInfo.body.cmd
		frame .slateInfo.body.note
frame .file


########################################################################
button .imageInfo.head.button \
	-anchor center \
	-relief groove \
	-text close \
	\
	-command {
		set mode [.imageInfo.head.button cget -text]
		if {$mode == "open"} {
			.imageInfo.head.button configure -text close
			pack configure .imageInfo.body -after .imageInfo.head
		} elseif {$mode == "close"} {
			.imageInfo.head.button configure -text open
			pack forget .imageInfo.body
		}
	}
label .imageInfo.head.label \
	-font {Arial 14} \
	-foreground $requiredFgColor \
	-text "Image Information (required)"
	#\
	#\
	#-width 40
pack .imageInfo.head.button -side left
pack .imageInfo.head.label -expand yes -fill x -side left
pack .imageInfo.head -fill x -padx 1 -pady 1 -side top
pack .imageInfo.body -side top
########################################################################

button .slateInfo.head.button \
	-anchor center \
	-relief groove \
	-text close \
	\
	-command {
		set mode [.slateInfo.head.button cget -text]
		if {$mode == "open"} {
			.slateInfo.head.button configure -text close
			pack configure .slateInfo.body -after .slateInfo.head
		} elseif {$mode == "close"} {
			.slateInfo.head.button configure -text open
			pack forget .slateInfo.body
		}
	}
label .slateInfo.head.label \
	-font {Arial 14} \
	-foreground $optionalFgColor \
	-text "Slate Information (optional)"
pack .slateInfo.head.button -side left
pack .slateInfo.head.label -expand yes -fill x -side left
pack .slateInfo.head -fill x -padx 1 -pady 1 -side top
pack .slateInfo.body -side top

# image information
button .imageInfo.body.directory.button \
	-anchor center \
	-relief groove \
	-text directory \
	\
	-command {
		chooseDirectory .imageInfo.body.directory.entry
	}
entry .imageInfo.body.directory.entry \
	-foreground #0066CC \
	-highlightcolor #CC0066 \
	-highlightthickness 1 \
	-relief groove \
	-textvariable ::vars::directory \
	\
	-width 32
pack .imageInfo.body.directory.button -padx 10 -side left
pack .imageInfo.body.directory.entry -side left
label .imageInfo.body.filename.label \
	-anchor e \
	-font $imageLabelFont \
	-foreground $imageLabelFgColor \
	-text "filename.####.extention: " \
	\
	-width $imageInfoLabelWidth
entry .imageInfo.body.filename.entry \
	-foreground #0066CC \
	-highlightcolor #CC0066 \
	-highlightthickness 1 \
	-relief groove \
	-textvariable ::vars::filename \
	\
	-width 24
label .imageInfo.body.filename.optionLabel \
	-text "####: number of digits"
pack .imageInfo.body.filename.label .imageInfo.body.filename.entry .imageInfo.body.filename.optionLabel -side left
label .imageInfo.body.frames.label \
	-anchor e \
	-font $imageLabelFont \
	-foreground $imageLabelFgColor \
	-text "frame range: " \
	\
	-width $imageInfoLabelWidth
entry .imageInfo.body.frames.entry \
	-foreground #0066CC \
	-highlightcolor #CC0066 \
	-highlightthickness 1 \
	-relief groove \
	-textvariable ::vars::frame_range \
	\
	-width 10
label .imageInfo.body.frames.descLabel \
	-text "\"#### - ####\""
pack .imageInfo.body.frames.label .imageInfo.body.frames.entry .imageInfo.body.frames.descLabel -side left
label .imageInfo.body.minimumFrames.label \
	-anchor e \
	-font $imageLabelFont \
	-foreground $imageLabelFgColor \
	-text "minimum frames: " \
	\
	-width $imageInfoLabelWidth
entry .imageInfo.body.minimumFrames.entry \
	-foreground #0066CC \
	-highlightcolor #CC0066 \
	-highlightthickness 1 \
	-relief groove \
	-textvariable ::vars::minimum_frames \
	\
	-width 4
label .imageInfo.body.minimumFrames.optionLabel \
	-foreground $optionalFgColor \
	-text (optional)
pack .imageInfo.body.minimumFrames.label .imageInfo.body.minimumFrames.entry .imageInfo.body.minimumFrames.optionLabel -side left
label .imageInfo.body.aT.label \
	-anchor e \
	-font $imageLabelFont \
	-foreground $imageLabelFgColor \
	-text "AT: " \
	\
	-width $imageInfoLabelWidth
button .imageInfo.body.aT.button \
	-anchor center \
	-foreground #CC0066 \
	-relief groove \
	-text "  AT  " \
	\
	-command {
		runAT
	}
entry .imageInfo.body.aT.entry \
	-foreground #0066CC \
	-highlightcolor #CC0066 \
	-highlightthickness 1 \
	-relief groove \
	-textvariable ::vars::aT \
	\
	-width 16
label .imageInfo.body.aT.description \
	-foreground #CC0066 \
	-text "# \"YYYY-MM-DD HH:MM\" or \"NOW\""
set ::vars::aT [clock format [clock scan tomorrow] -format "%Y-%m-%d 08:00"]
pack .imageInfo.body.aT.button -padx 10 -side left
pack .imageInfo.body.aT.entry -side left
pack .imageInfo.body.aT.description -side left

label .slateInfo.body.show.label \
	-anchor e \
	-font $slateLabelFont \
	-foreground $slateLabelFgColor \
	-text "show: " \
	\
	-width $slateInfoLabelWidth
entry .slateInfo.body.show.entry \
	-foreground #0066CC \
	-highlightcolor #CC0066 \
	-highlightthickness 1 \
	-relief groove \
	-textvariable ::vars::m_show \
	\
	-width 20
label .slateInfo.body.show.requiredLabel \
	-foreground $requiredFgColor \
	-text (required)
pack .slateInfo.body.show.label .slateInfo.body.show.entry .slateInfo.body.show.requiredLabel -side left
label .slateInfo.body.shot.label \
	-anchor e \
	-font $slateLabelFont \
	-foreground $slateLabelFgColor \
	-text "shot: " \
	\
	-width $slateInfoLabelWidth
entry .slateInfo.body.shot.entry \
	-foreground #0066CC \
	-highlightcolor #CC0066 \
	-highlightthickness 1 \
	-relief groove \
	-textvariable ::vars::m_shot \
	\
	-width 20
label .slateInfo.body.shot.requiredLabel \
	-foreground $requiredFgColor \
	-text (required)
pack .slateInfo.body.shot.label .slateInfo.body.shot.entry .slateInfo.body.shot.requiredLabel -side left
label .slateInfo.body.daily_date.label \
	-anchor e \
	-font $slateLabelFont \
	-foreground $slateLabelFgColor \
	-text "daily_date: " \
	\
	-width $slateInfoLabelWidth
entry .slateInfo.body.daily_date.entry \
	-foreground #0066CC \
	-highlightcolor #CC0066 \
	-highlightthickness 1 \
	-relief groove \
	-textvariable ::vars::daily_date \
	\
	-width 8
label .slateInfo.body.daily_date.requiredLabel \
	-foreground $requiredFgColor \
	-text "# tomorrow \"YYYYMMDD\" (required)"
set ::vars::daily_date [clock format [clock scan "tomorrow"] -format "%Y%m%d"]
pack .slateInfo.body.daily_date.label .slateInfo.body.daily_date.entry .slateInfo.body.daily_date.requiredLabel -side left
label .slateInfo.body.date.label \
	-anchor e \
	-font $slateLabelFont \
	-foreground $slateLabelFgColor \
	-text "date: " \
	\
	-width $slateInfoLabelWidth
entry .slateInfo.body.date.entry \
	-background $::disabledBgColor \
	-foreground #0066CC \
	-highlightcolor #CC0066 \
	-highlightthickness 1 \
	-relief groove \
	-textvariable ::vars::m_date \
	\
	-state disabled \
	-width 18
label .slateInfo.body.date.requiredLabel \
	-foreground $requiredFgColor \
	-text "# now"
set ::vars::m_date [clock format [clock scan now] -format "%a %b %d %H:%M %Y"]
pack .slateInfo.body.date.label .slateInfo.body.date.entry .slateInfo.body.date.requiredLabel -side left
label .slateInfo.body.creator.label \
	-anchor e \
	-font $slateLabelFont \
	-foreground $slateLabelFgColor \
	-text "creator: " \
	\
	-width $slateInfoLabelWidth
entry .slateInfo.body.creator.entry \
	-background $::disabledBgColor \
	-foreground #0066CC \
	-highlightcolor #CC0066 \
	-highlightthickness 1 \
	-relief groove \
	-textvariable ::vars::m_creator \
	\
	-state disabled \
	-width 20
pack .slateInfo.body.creator.label .slateInfo.body.creator.entry -side left
label .slateInfo.body.mm.label \
	-anchor e \
	-font $slateLabelFont \
	-foreground $slateLabelFgColor \
	-text "mm: " \
	\
	-width $slateInfoLabelWidth
entry .slateInfo.body.mm.entry \
	-foreground #0066CC \
	-highlightcolor #CC0066 \
	-highlightthickness 1 \
	-relief groove \
	-textvariable ::vars::m_mm \
	\
	-width 20
pack .slateInfo.body.mm.label .slateInfo.body.mm.entry -side left
label .slateInfo.body.anim.label \
	-anchor e \
	-font $slateLabelFont \
	-foreground $slateLabelFgColor \
	-text "anim: " \
	\
	-width $slateInfoLabelWidth
entry .slateInfo.body.anim.entry \
	-foreground #0066CC \
	-highlightcolor #CC0066 \
	-highlightthickness 1 \
	-relief groove \
	-textvariable ::vars::m_anim \
	\
	-width 20
pack .slateInfo.body.anim.label .slateInfo.body.anim.entry -side left
label .slateInfo.body.fx.label \
	-anchor e \
	-font $slateLabelFont \
	-foreground $slateLabelFgColor \
	-text "fx: " \
	\
	-width $slateInfoLabelWidth
entry .slateInfo.body.fx.entry \
	-foreground #0066CC \
	-highlightcolor #CC0066 \
	-highlightthickness 1 \
	-relief groove \
	-textvariable ::vars::m_fx \
	\
	-width 20
pack .slateInfo.body.fx.label .slateInfo.body.fx.entry -side left
label .slateInfo.body.td.label \
	-anchor e \
	-font $slateLabelFont \
	-foreground $slateLabelFgColor \
	-text "td: " \
	\
	-width $slateInfoLabelWidth
entry .slateInfo.body.td.entry \
	-foreground #0066CC \
	-highlightcolor #CC0066 \
	-highlightthickness 1 \
	-relief groove \
	-textvariable ::vars::m_td \
	\
	-width 20
pack .slateInfo.body.td.label .slateInfo.body.td.entry -side left
label .slateInfo.body.roto.label \
	-anchor e \
	-font $slateLabelFont \
	-foreground $slateLabelFgColor \
	-text "roto: " \
	\
	-width $slateInfoLabelWidth
entry .slateInfo.body.roto.entry \
	-foreground #0066CC \
	-highlightcolor #CC0066 \
	-highlightthickness 1 \
	-relief groove \
	-textvariable ::vars::m_roto \
	\
	-width 20
pack .slateInfo.body.roto.label .slateInfo.body.roto.entry -side left
label .slateInfo.body.comp.label \
	-anchor e \
	-font $slateLabelFont \
	-foreground $slateLabelFgColor \
	-text "comp: " \
	\
	-width $slateInfoLabelWidth
entry .slateInfo.body.comp.entry \
	-foreground #0066CC \
	-highlightcolor #CC0066 \
	-highlightthickness 1 \
	-relief groove \
	-textvariable ::vars::m_comp \
	\
	-width 20
pack .slateInfo.body.comp.label .slateInfo.body.comp.entry -side left
label .slateInfo.body.coord.label \
	-anchor e \
	-font $slateLabelFont \
	-foreground $slateLabelFgColor \
	-text "coord: " \
	\
	-width $slateInfoLabelWidth
entry .slateInfo.body.coord.entry \
	-foreground #0066CC \
	-highlightcolor #CC0066 \
	-highlightthickness 1 \
	-relief groove \
	-textvariable ::vars::m_coord \
	\
	-width 20
pack .slateInfo.body.coord.label .slateInfo.body.coord.entry -side left
label .slateInfo.body.bg.label \
	-anchor e \
	-font $slateLabelFont \
	-foreground $slateLabelFgColor \
	-text "background: " \
	\
	-width $slateInfoLabelWidth
entry .slateInfo.body.bg.entry \
	-foreground #0066CC \
	-highlightcolor #CC0066 \
	-highlightthickness 1 \
	-relief groove \
	-textvariable ::vars::m_background \
	\
	-width 20
pack .slateInfo.body.bg.label .slateInfo.body.bg.entry -side left
label .slateInfo.body.cameramove.label \
	-anchor e \
	-font $slateLabelFont \
	-foreground $slateLabelFgColor \
	-text "camera move: " \
	\
	-width $slateInfoLabelWidth
entry .slateInfo.body.cameramove.entry \
	-foreground #0066CC \
	-highlightcolor #CC0066 \
	-highlightthickness 1 \
	-relief groove \
	-textvariable ::vars::m_cameramove \
	\
	-width 20
pack .slateInfo.body.cameramove.label .slateInfo.body.cameramove.entry -side left
label .slateInfo.body.pixs_srate.label \
	-anchor e \
	-font $slateLabelFont \
	-foreground $slateLabelFgColor \
	-text "pixs srate: " \
	\
	-width $slateInfoLabelWidth
entry .slateInfo.body.pixs_srate.entry \
	-foreground #0066CC \
	-highlightcolor #CC0066 \
	-highlightthickness 1 \
	-relief groove \
	-textvariable ::vars::m_pixs_srate \
	\
	-width 20
pack .slateInfo.body.pixs_srate.label .slateInfo.body.pixs_srate.entry -side left
label .slateInfo.body.audio.label \
	-anchor e \
	-font $slateLabelFont \
	-foreground $slateLabelFgColor \
	-text "audio: " \
	\
	-width $slateInfoLabelWidth
entry .slateInfo.body.audio.entry \
	-foreground #0066CC \
	-highlightcolor #CC0066 \
	-highlightthickness 1 \
	-relief groove \
	-textvariable ::vars::m_audio \
	\
	-width 20
pack .slateInfo.body.audio.label .slateInfo.body.audio.entry -side left
label .slateInfo.body.filmio.label \
	-anchor e \
	-font $slateLabelFont \
	-foreground $slateLabelFgColor \
	-text "filmio: " \
	\
	-width $slateInfoLabelWidth
entry .slateInfo.body.filmio.entry \
	-foreground #0066CC \
	-highlightcolor #CC0066 \
	-highlightthickness 1 \
	-relief groove \
	-textvariable ::vars::m_filmio \
	\
	-width 20
pack .slateInfo.body.filmio.label .slateInfo.body.filmio.entry -side left
label .slateInfo.body.cmd.label \
	-anchor e \
	-font $slateLabelFont \
	-foreground $slateLabelFgColor \
	-text "cmd: " \
	\
	-width $slateInfoLabelWidth
entry .slateInfo.body.cmd.entry \
	-foreground #0066CC \
	-highlightcolor #CC0066 \
	-highlightthickness 1 \
	-relief groove \
	-textvariable ::vars::m_cmd \
	\
	-width 20
pack .slateInfo.body.cmd.label .slateInfo.body.cmd.entry -side left
label .slateInfo.body.note.label \
	-anchor e \
	-font $slateLabelFont \
	-foreground $slateLabelFgColor \
	-text "note: " \
	\
	-width $slateInfoLabelWidth
entry .slateInfo.body.note.entry \
	-foreground #0066CC \
	-highlightcolor #CC0066 \
	-highlightthickness 1 \
	-relief groove \
	-textvariable ::vars::m_notes \
	\
	-width 20
pack .slateInfo.body.note.label .slateInfo.body.note.entry -side left

button .file.openButton \
	-anchor center \
	-relief groove \
	-text open \
	\
	-command {
		fileDialog . open
	}
button .file.exitButton \
	-anchor center \
	-relief groove \
	-text exit \
	\
	-command {
		xIT
	}
button .file.saveButton \
	-anchor center \
	-relief groove \
	-text save \
	\
	-command {
		fileDialog . save
	}
#pack .file.openButton .file.exitButton .file.saveButton -expand 1 -padx 20 -pady 10 -side left
pack .file.exitButton -expand 1 -padx 20 -pady 10 -side left

pack .imageInfo.body.directory .imageInfo.body.filename .imageInfo.body.frames .imageInfo.body.minimumFrames .imageInfo.body.aT -anchor w -side top
pack .slateInfo.body.daily_date .slateInfo.body.show .slateInfo.body.shot .slateInfo.body.date .slateInfo.body.creator .slateInfo.body.mm .slateInfo.body.anim .slateInfo.body.fx .slateInfo.body.td .slateInfo.body.roto .slateInfo.body.comp .slateInfo.body.coord .slateInfo.body.bg .slateInfo.body.cameramove .slateInfo.body.pixs_srate .slateInfo.body.audio .slateInfo.body.filmio .slateInfo.body.cmd .slateInfo.body.note -anchor w -side top
pack .imageInfo .slateInfo .file -fill x -padx 10 -pady 10 -side top

proc fileDialog {w mode} {
	if {$mode == "open"} {
		set file [
			tk_getOpenFile \
				-parent $w \
				-title "tk_getOpenFile"
		]
		if {$file == ""} return
		tk_dialog .dialog {no title} $file {} 0 OK
	} elseif {$mode == "save"} {
		set file [
			tk_getSaveFile \
				-parent $w \
				-title "tk_getSaveFile"
		]
		if {$file == ""} return
		tk_dialog .dialog {no title} $file {} 0 OK
	} else {
		return
	}
}


proc validate_vars {} {
# directory
	set x $::vars::directory
	if ![file isdirectory $x] {
		printf "'$x' is NOT a directory"
		return 1
	}
	# change into nativename
	set x [file nativename $x]
	# replace '\' into '/'
	regsub -all -- {\\} $x {/} x
	if {[string range $x end end] != "/"} {
		append x /
	}
	# update
	set ::vars::directory $x

# filename.extensioin
	set x $::vars::filename
	if ![regexp -- {^\s*(\S+)\.([#]+)\.(\S+)\s*$} $x null image_NAME n_HASH image_EXT] {
		printf "filename.extension: '$x' is NOT valid"
		return 1
	}
	# update
	set ::vars::filename $image_NAME.$n_HASH.$image_EXT

# frame range
	set x $::vars::frame_range
	if ![regexp -- {^\s*(\d+)\s*-\s*(\d+)\s*$} $x null frame_START frame_END] {
		printf "frame range: '$x' is NOT valid"
		return 1
	}
	if {$frame_START > $frame_END} {
		printf "frame range: $frame_START <= $frame_END (???)"
		return 1
	}
	set ::vars::frame_range "$frame_START - $frame_END"

# minimum frames
	set x $::vars::minimum_frames
	set minimum {}
	if ![regexp -- {^\s*(\d+)\s*$} $x null minimum] {
		if {$x != ""} {
			printf "minimum frames: '$x' is NOT valid"
			return 1
		}
	}
	if {$x != "" && $minimum > [expr $frame_END - $frame_START + 1]} {
		printf "minimum frames: $minimum <= [$frame_END - $frame_START + 1] (???)"
		return 1
	}
	set ::vars::minimum_frames $minimum

# daily date
	set x $::vars::daily_date
	if [catch {eval set AT [clock scan $x]} msg] {
		printf "daily DATE: '$x': $msg"
		return 1
	}
	set ::vars::daily_date [clock format $AT -format "%Y%m%d"]

# SHOW
	set x $::vars::m_show
	if ![regexp -- {^\s*(\S+)\s*$} $x null m_SHOW] {
		printf "SHOW: '$x' should BE given"
		return 1
	}
	set ::vars::m_show $m_SHOW

# SHOT
	set x $::vars::m_shot
	if ![regexp -- {^\s*(\S+)\s*$} $x null m_SHOT] {
		printf "SHOT: '$x' should BE given"
		return 1
	}
	set ::vars::m_shot $m_SHOT

	return 0
}

proc save_FILE {p_FILE} {
	# validate variables
	if {[validate_vars] == 1} {
		return 1
	}

	# open
	set CHANNEL_ID [open $p_FILE w]

	# ::gi::*
	puts -nonewline $CHANNEL_ID "namespace eval gi \{\n"
	puts -nonewline $CHANNEL_ID "\}\n"
	set mode [.imageInfo.head.button cget -text]
	if {$mode == "open"} {
		set ::gi::g__imageInfo 0
	} elseif {$mode == "close"} {
		set ::gi::g__imageInfo 1
	}
	set mode [.slateInfo.head.button cget -text]
	if {$mode == "open"} {
		set ::gi::g__slateInfo 0
	} elseif {$mode == "close"} {
		set ::gi::g__slateInfo 1
	}
	foreach x [lsort [info vars ::gi::*]] {
		upvar $x ref_x
		puts -nonewline $CHANNEL_ID "set $x [list $ref_x]\n"
	}

	# ::vars::*
	puts -nonewline $CHANNEL_ID "namespace eval vars \{\n"
	puts -nonewline $CHANNEL_ID "\}\n"
	foreach x [lsort [info vars ::vars::*]] {
		upvar $x ref_x
		puts -nonewline $CHANNEL_ID "set $x [list $ref_x]\n"
	}
	puts -nonewline $CHANNEL_ID "\n\n\nreturn OK\n"

	# close
	close $CHANNEL_ID
}

proc xIT {} {
	# save variables
	if {[save_FILE $::env(HOME)/.mini.ini] == 1} {
		return
	}

	# 
	destroy .
}

proc runAT {} {
	set NOW [clock scan now]
	set u_FILENAME ".[file rootname [file tail $::argv0]]-[clock format $NOW -format "%Y%m%d%H%M%S"]"
	set u_PATHNAME "$::env(HOME)\\$u_FILENAME"
	# replace '\' into '/'
	regsub -all -- {\\} $u_PATHNAME {/} u_PATHNAME

	if {[string toupper [string trim $::vars::aT]] == "NOW"} {
		if {[save_FILE $u_PATHNAME] == 1} {
			return
		}
		if [catch {eval exec $::TCLSH [file join $::ROOTPATH $::prog_AT] $u_PATHNAME} msg] {
			printf $msg
		}
#		printf "process ID = $msg"
		printf $msg
		return
	}

	# using AT
	set NOW [clock scan now]
	if [catch {eval set AT [clock scan $::vars::aT]} msg] {
		printf $msg
		return
	}
	if {[expr $AT - $NOW] < 0} {
		printf "---- PAST(?) ----\nAT: [clock format $AT -format "%Y-%m-%d %H:%M"]\nNOW: [clock format $NOW -format "%Y-%m-%d %H:%M"]"
		return
	}
	if {[save_FILE $u_PATHNAME] == 1} {
		return
	}
	if [catch {eval exec [clock format $AT -format "$::prog__wAT %H:%M /NEXT:%d CMD /C $::TCLSH [file join $::ROOTPATH $::prog_AT] $u_PATHNAME"]} msg] {
		printf $msg
	}
	if ![regexp -- {ID = (\d+)\s*$} $msg x job__ID] {
		printf "xxx: $msg"
	}
	printf "JOB ID = $job__ID"
	return
}

if {[info exists ::gi::g__imageInfo] && $::gi::g__imageInfo == 0} {
	.imageInfo.head.button configure -text open
	pack forget .imageInfo.body
}
if {[info exists ::gi::g__slateInfo] && $::gi::g__slateInfo == 0} {
	.slateInfo.head.button configure -text open
	pack forget .slateInfo.body
}
