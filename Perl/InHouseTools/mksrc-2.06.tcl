################################################################################
#
# 1.14 - 05/22/2002
#
#	1. Change a procedure name
#
#		proc cmd_load {...} {......}
#
#	   into
#
#		proc load_commandfile {...} {......}
#
#	2. Impliment a procedure
#
#		proc open_scene {...} {......}
#
#	   to open a Maya scene file.
#
#	3. Ensure a varialbe
#
#		::job::GH
#
#	   is set.
#
#	4. Add a variable in gh.py
#
#		m_DEPT
#
#
# 1.15 - 05/23/2002
#
#	1. Modify workspace environment variables
#	   1) Remove AddSearchPath for textures ($gh.g_TEXDIR)
#	   2) Add SetDir torShadows for shadows ($gh.g_SHDDIR)
#
#	2. Introduce JOB directory
#
#		m_pJOB	= m_pBASE + 'jobs/'
#
################################################################################
namespace eval mkSRC {	# global information

#	printf [::mkSRC::index {::mkSRC::gi(gh)} "g_LAYER"]
	proc index {var str} {
		upvar $var ref
		for {set i 0} {$i < [llength $ref]} {incr i} {
#			printf [lindex $ref $i]
			if {$str == [lindex [lindex $ref $i] 0]} {
				return $i
			}
		}
		return -1;
	}
	proc qVal {gKey qKey} {

		upvar ::mkSRC::gi($gKey) ref_gi
		return [lindex [lindex $ref_gi [::mkSRC::index ref_gi $qKey]] 1]
	}
}
################################################################################
proc ::mkSRC::displayText {w chunk text} {
	set bg_COLOR #A0B7CE
	set fg_COLOR #0066CC
	set hi_bg_COLOR #CC0066
	$w.text tag configure TAG -background $bg_COLOR
#	$w.text insert end $text

	set wordlist {}
	for {set i 0} {$i < [llength $::mkSRC::gi($chunk)]} {incr i} {
		set word [lindex [lindex $::mkSRC::gi($chunk) $i] 0]
		lappend wordlist $word

		set fw $w.text.${chunk}__${word}
		set lw $fw.label
		set ew $fw.entry

		frame $fw \
			-background #FFFFFF
		label $lw \
			-background $bg_COLOR \
			-justify left \
			-text ": " \
			-wraplength 4i
#		pack $lw -side left -fill both
		if {$chunk == "genRIB" && $word == "m_RIB_FORCED"} {
			checkbutton $fw.checkbutton \
				-activebackground #0066CC \
				-activeforeground #FFFFFF \
				-background #0066CC \
				-foreground #FFFFFF \
				-relief groove \
				-text "generate RIB always" \
				\
				-selectcolor #000000 \
				-variable ::alfred::gen_RIB::m__RIB_FORCED
			pack $fw.checkbutton -side left
		} else {
			entry $ew \
				-foreground $fg_COLOR \
				-highlightcolor #CC0066 \
				-highlightthickness 1 \
				-relief groove \
				\
			-width 20
			pack $ew
		}
		if {$chunk == "gh" && $word == "g_bg"} {
			$ew configure \
				-textvariable ::alfred::g_HEADER::g__bg \
				\
				-width 32
		}
		if {$chunk == "gh" && $word == "m_SCENE"} {
			set bw $fw.button
			button $bw \
				-background $hi_bg_COLOR \
				-text "Open Maya scene" \
				\
				-command "open_scene \"$w\" \"$fw\""
			$ew configure \
				-textvariable ::alfred::g_HEADER::m__loSCENE \
				\
				-width 48
#			pack $bw -side left -expand 1 -fill both
			pack $bw -anchor w
		}
		if {$chunk == "genRIB" && $word == "m_CMDFILEs"} {
			set bw $fw.button
			button $bw \
				-background $hi_bg_COLOR \
				-text "Load Command File" \
				\
				-command "load_commandfile \"$w\" \"$fw\""
			$ew configure \
				-textvariable ::alfred::gen_RIB::m__CMDFILEs \
				\
				-width 48
			pack $bw -anchor w
		}
		if {$chunk == "genRIB" && $word == "m_AUX_CMD"} {
			$ew configure \
				-textvariable ::alfred::gen_RIB::m__AUX_CMD \
				\
				-width 32
		}
		if {$chunk == "genRIB" && $word == "m_LL"} {
			$ew configure \
				-textvariable ::alfred::gen_RIB::m__LL \
				\
				-width 32
		}
	}
################################################################################
	upvar ::mkSRC::gi($chunk) ref
	if {$chunk == "gh"} {
		$w.text.${chunk}__g_fi.entry configure \
			-invalidcommand fi_invcmd \
			-validate focusout \
			-validatecommand fi_vcmd
		if [info exists g_fi] {
			upvar ::mkSRC::gi($chunk) ref
			set i [::mkSRC::index ::mkSRC::gi($chunk) g_fi]
			set x [lindex $ref $i]
			set x [lreplace $x 1 1 $::g_fi]
			set ref [lreplace $ref $i $i $x]
		}
	}
	if {$chunk == "first"} {
		set i [::mkSRC::index ::mkSRC::gi($chunk) g_DAILY_DATE]
		upvar ::mkSRC::gi($chunk) ref
		set x [lindex $ref $i]
		set x [lreplace $x 1 1 [clock format [clock scan "tomorrow"] -format "%Y%m%d"]]
		set ref [lreplace $ref $i $i $x]
	}
################################################################################
	set pre_text ""
	set word ""
	set post_text ""
	while {[
		regexp -- "^(.*?)<([join $wordlist "|"])>(.*)$" \
			$text x pre_text word post_text
	] == 1} {
		set text $post_text
		set ix [::mkSRC::index ::mkSRC::gi($chunk) $word]
		if {[lindex [lindex $ref $ix] 4] == 0} {
			$w.text insert end "$pre_text"
			set x [lindex [lindex $ref $ix] 1]
			if [lindex [lindex $ref $ix] 2] {
				set x "'$x'"
			}
			$w.text insert end "$x"
		} else {
			$w.text insert end "$pre_text"
#			$w.text insert end "<$word>" TAG
			$w.text window create end -window $w.text.${chunk}__${word}

			if {$chunk == "gh" && $word == "m_SCENE"} {
				# instead, use ::alfred::g_HEADER::m__loSCENE
			} elseif {$chunk == "gh" && $word == "g_bg"} {
				# instead, use ::alfred::g_HEADER::g__bg
			} elseif {$chunk == "genRIB" && $word == "m_CMDFILEs"} {
				# instead, use ::alfred::gen_RIB::m__CMDFILEs
			} elseif {$chunk == "genRIB" && $word == "m_LL"} {
				# instead, use ::alfred::gen_RIB::m__LL
			} elseif {$chunk == "genRIB" && $word == "m_RIB_FORCED"} {
				# instead, use ::alfred::gen_RIB::m__RIB_FORCED
				set ::alfred::gen_RIB::m__RIB_FORCED [lindex [lindex $ref $ix] 1]
			} elseif {$chunk == "genRIB" && $word == "m_AUX_CMD"} {
				# instead, use ::alfred::gen_RIB::m__AUX_CMD
			} else {
				$w.text.${chunk}__${word}.entry insert end \
					[lindex [lindex $ref $ix] 1]
			}

			if {[lindex [lindex $ref $ix] 3] == 1} {
				set x [$w.text index end]
				$w.text see $x
				$w.text.${chunk}__${word}.entry selection range 0 end
				focus $w.text.${chunk}__${word}.entry
			}
		}
	}
	$w.text insert end $text
	$w.text configure -state disabled
}
###############################################################################
proc fi_vcmd {} {
	if {[parse_fi [.agenda_gh.text.gh__g_fi.entry get] x__fi_list o__fi_list] == 1} {
		return 1
	} else {
		return 0
	}
	return [fi_isvalid [.agenda_gh.text.gh__g_fi.entry get]]
}
proc fi_invcmd {} {
	printf "Invalid g_fi: [.agenda_gh.text.gh__g_fi.entry get]"
}

proc fi_isvalid {str} {
	return 1
}

namespace eval mkSRC {
######## <global header> ########
	proc gh {w p_FILE} {
		set _text {
######################################################################
######################## <PRIME INFORMATION: REQUIRED> ########################
m_SCENE		= <m_SCENE>	# Maya scene
# used for pathname
g_DEPT		= <g_DEPT>
g_SHOW		= <g_SHOW>
m_SEQ		= <m_SEQ>
g_SHOT		= <g_SHOT>
g_LAYER		= <g_LAYER>	# whoami
m_TAKE		= <m_TAKE>	# revision no.
######################## </PRIME INFORMATION: REQUIRED> ########################
######################################################################
######################################################################
######################## <PRIME INFORMATION: OPTIONAL> ########################
# used for filename
m_ANIMTAKE	= <m_ANIMTAKE>	# which animation (g_SCENE)
m_VER		= <m_VER>	# mnemonic for use of animation(g_RIB, g_IMG)
######################## </PRIME INFORMATION: OPTIONAL> ########################
######################################################################

######################## <FRAME INFORMATION> ########################
g_bg		= <g_bg>
g_fi		= <g_fi>
######################## </FRAME INFORMATION> ########################

######################## <MISC. INFORMATION> ########################
g_SCENE_EXT	= <g_SCENE_EXT>		# extension of maya scene file(g_SCENE)
m_SUB		= 'all'			# only for rib filename(g_RIB)
m_RENRES	= <m_RENRES>		# 0: low, 1: high(<DIRECTORY: IMAGE>)
m_ELEM		= <m_ELEM>		# hero name(<DIRECTORY: IMAGE>)
# Q: Why these are here?
# A: Just for convenience!
# (*generate RIB)
# 'targa(tga)', 'sgif', 'tiff(tif)', 'mayaiff(iff)', ...
g_display_SERVERNAME	= <g_display_SERVERNAME>
# (*generate RIB)
# 'rgb', 'rgba', 'rgbz', 'rgbaz'
g_display_MODE		= <g_display_MODE>
g_IMG_EXT		= <g_IMG_EXT>	# (*render HERO)
######################## </MISC. INFORMATION> ########################
}
		set text \
{######################## <REQUIRED> ########################
m_SCENE:	<m_SCENE>

g_DEPT:		<g_DEPT>
g_SHOW:		<g_SHOW>
m_SEQ:		<m_SEQ>
g_SHOT:		<g_SHOT>
g_LAYER:	<g_LAYER>			# whoami
m_TAKE:		<m_TAKE>	# revision no.

g_fi:		<g_fi>	# frame range ( #### - #### )
m_ELEM:		<m_ELEM>	# hero name
############################################################

######################## <OPTIONAL> ########################
Animation rev. no.:		<m_ANIMTAKE>
m_VER:			<m_VER>

# background image used in DPS
bg:			<g_bg>

# 'targa(tga)', 'sgif(sgi)', 'tiff(tif)', 'mayaiff(iff)', ...
Display server:		<g_display_SERVERNAME>
Image extension:		<g_IMG_EXT>

# 'rgb', 'rgba', 'rgbz', 'rgbaz'
Display mode:		<g_display_MODE>
############################################################}

	::mkSRC::displayText ".$w" "gh" [subst $text]
	}
######## </global header> ########

	proc first {w p_FILE} {
		if {${::alfred::g_HEADER::g_HDR} != 1} {
			destroy .$w
			printf "Visit \"global header\""
			return
		}
		set text {
g_DAILY_DATE	= <g_DAILY_DATE>	# tomorrow: YYYYMMDD (no space)

m_mm		= <m_mm>
m_anim		= <m_anim>
m_fx		= <m_fx>
m_td		= <m_td>
m_roto		= <m_roto>
m_comp		= <m_comp>
m_coord		= <m_coord>
m_background	= <m_background>
m_cameramove	= <m_cameramove>
m_pixs_srate	= <m_pixs_srate>
m_audio		= <m_audio>
m_filmio		= <m_filmio>
m_cmd		= <m_cmd>
m_notes		= <m_notes>
}
	::mkSRC::displayText ".$w" "first" [subst $text]
	}
######## <generate RIB> ########
	proc genRIB {w p_FILE} {
		if {${::alfred::g_HEADER::g_HDR} != 1} {
			destroy .$w
			printf "Visit \"global header\""
			return
		}
		set _text {
m_FARM		= <m_FARM>	# 0: No, 1: Yes

# Command file(s)
m_CMDFILEs	= <m_CMDFILEs>

m_LightLinking	= <m_LL>

# 1. Specify camera name
#	-cmd rg camName perspShape
# 2. Add workspace environment variable, i.e. AddSearchPath
#	-cmd workspace AddSearchPath \\\\"//chunji/show/hm/Render Man Resources\\\\"
m_AUX_CMD	= <m_AUX_CMD>

m_RIB_FORCED	= <m_RIB_FORCED>
}
		set text \
{######################## <OPTIONAL> ########################
Command files:		<m_CMDFILEs>
Light linking:		<m_LL>
Auxiliary command:		<m_AUX_CMD>
			<m_RIB_FORCED>
######################## <OPTIONAL> ########################}
	::mkSRC::displayText ".$w" "genRIB" [subst $text]
	}
######## </generate RIB> ########




######## <render Hero> ########
	proc renHERO {w u_NAME} {
		if {${::alfred::g_HEADER::g_HDR} != 1} {
			destroy .$w
			printf "Visit \"global header\""
			return
		}

		set _text {
m_FARM		= <m_FARM>	# 0: No, 1: Yes
m_FILENAMEVAR	= <m_FILENAMEVAR>	# unique name appended to g_HEADER::g_IMG
m_EDIT_ARGS	= <m_EDIT_ARGS>		# '-e \\\\\\\\* -r ballA'
}
		set text \
{Unique ID:		<m_FILENAMEVAR>	# REQUIRED!

# e.g., "-e \\\\\\\\* -r ballA" without double quote(")
RIB edit argument(s):	<m_EDIT_ARGS>	# OPTIONAL}
#	.$w.text insert end [subst $text]
#	::mkSRC::displayText ".$w" "renHERO" [subst $text]
		if {$u_NAME == ""} {
			set m_renHERO renHERO
		} else {
			set m_renHERO renHERO___$u_NAME
		}
#		printf $m_renHERO
		::mkSRC::displayText \
			.$w \
			$m_renHERO \
			[subst $text]
	}

	proc ren__GROUND_SHADOW {w u_NAME} {
		if {${::alfred::g_HEADER::g_HDR} != 1} {
			destroy .$w
			printf "Visit \"global header\""
			return
		}

		set text {
#                 LIGHT.SRC.DEST: no space( ), no dot(.)
#		  keyShape.ball.wall
m_NAME		= <m_NAME>
}
		if {$u_NAME == ""} {
			set m_ren__GROUND_SHADOW ren__GROUND_SHADOW
		} else {
			set m_ren__GROUND_SHADOW ren__GROUND_SHADOW___$u_NAME
		}
#		printf $m_ren__GROUND_SHADOW
		::mkSRC::displayText \
			.$w \
			$m_ren__GROUND_SHADOW \
			[subst $text]
	}
######## <render Hero> ########

	proc last {w u_NAME} {
		if {$::alfred::mk_DPS::g__DPS != 1} {
			destroy .$w
			printf "Visit \"first\""
			return
		}
		set text {#!/usr/bin/python

m_FROM		= <m_FROM>
m_TO		= <m_TO>
m_SUBJECT	= <m_SUBJECT>
m_CONTENT	= <m_CONTENT>
}
		::mkSRC::displayText ".$w" "last" [subst $text]
	}
}

proc load_commandfile {w fw} {
######## <types> ########
	set types {
		{"Command Files"	".dscmd"}
		{"All Files"		"*"}
	}
######## </types> ########
	set file [
		tk_getOpenFile \
			-filetypes $types \
			-initialdir $::vars::m__pCMDFILEs \
			-parent $w \
			-title "Load Command File"
	]
	if {$file != ""} {
		if [file isfile $file] {
			set m_CMDFILEs [string trim $::alfred::gen_RIB::m__CMDFILEs]
			if {$m_CMDFILEs == ""} {
				set m_CMDFILEs $file
			} else {
				if {[string first $file $m_CMDFILEs] != -1} {
					# duplicate!
					return
				}
				append m_CMDFILEs " $file"
			}
			set ::alfred::gen_RIB::m__CMDFILEs $m_CMDFILEs
			set ::vars::m__pCMDFILEs [file dirname $file]
		}
	}
}

proc open_scene {w fw} {
######## <types> ########
	set types {
		{"Maya Files"	".mb"}
		{"All Files"		"*"}
	}
######## </types> ########
	set file [
		tk_getOpenFile \
			-defaultextension .mb \
			-filetypes $types \
			-initialdir $::vars::m__pSCENE \
			-parent $w \
			-title "tk_getOpenFile: Open Maya scene"
	]
	if {$file != ""} {
		set ::alfred::g_HEADER::m__loSCENE $file
	}
}


namespace eval alfred {
	namespace eval g_HEADER {
		proc _init {} {
			set m_WORKROOT "//${::SERVER}/show/"

			set m_DEPT	[::mkSRC::qVal "gh" "g_DEPT"]
			set ::alfred::g_HEADER::g_DEPT \
					$m_DEPT
			set m_SHOW	[::mkSRC::qVal "gh" "g_SHOW"]
			set ::alfred::g_HEADER::g_SHOW \
					$m_SHOW
			set m_SEQ	[::mkSRC::qVal "gh" "m_SEQ"]
			set m_SHOT	[::mkSRC::qVal "gh" "g_SHOT"]
			set ::alfred::g_HEADER::g_SHOT \
					$m_SHOT
			set m_LAYER	[::mkSRC::qVal "gh" "g_LAYER"]
			set ::alfred::g_HEADER::g_LAYER \
					$m_LAYER
			set m_TAKE	[::mkSRC::qVal "gh" "m_TAKE"]

			set m_ANIMTAKE	[::mkSRC::qVal "gh" "m_ANIMTAKE"]
			set m_VER	[::mkSRC::qVal "gh" "m_VER"]

			set ::alfred::g_HEADER::g_fi \
					[::mkSRC::qVal "gh" "g_fi"]

			set m_RENRES	[::mkSRC::qVal "gh" "m_RENRES"]
			set m_ELEM	[::mkSRC::qVal "gh" "m_ELEM"]

			set ::alfred::g_HEADER::g_display_SERVERNAME \
					[::mkSRC::qVal "gh" "g_display_SERVERNAME"]
			set ::alfred::g_HEADER::g_display_MODE \
					[::mkSRC::qVal "gh" "g_display_MODE"]
			set ::alfred::g_HEADER::g_IMG_EXT \
					[::mkSRC::qVal "gh" "g_IMG_EXT"]

			set m_WORKDIR	"${m_WORKROOT}${m_SHOW}/${m_SEQ}/${m_SHOT}/"
			set x_TMP [string toupper $m_SHOW]

			set ::alfred::g_HEADER::g_SRCDIR \
					"${m_WORKDIR}scene/${m_DEPT}/${x_TMP}_${m_SHOT}_${m_DEPT}_${m_LAYER}_maya/scenes/"
			set ::alfred::g_HEADER::g_SCENE \
					"${m_SHOW}[string toupper ${m_DEPT}]-${m_SHOT}${m_LAYER}an${m_ANIMTAKE}${m_DEPT}${m_TAKE}"
			set ::alfred::g_HEADER::g_SCENE_EXT \
					[::mkSRC::qVal "gh" "g_SCENE_EXT"]
			set ::alfred::g_HEADER::m__soSCENE ${::alfred::g_HEADER::g_SRCDIR}${::alfred::g_HEADER::g_SCENE}${::alfred::g_HEADER::g_SCENE_EXT}

			set m_pBASE	"${m_WORKDIR}image/${m_DEPT}/${m_LAYER}/${m_TAKE}/"
			set ::alfred::g_HEADER::m__pBASE $m_pBASE
			set ::alfred::g_HEADER::m__pRIB \
					"${m_pBASE}ribs/"
			set ::alfred::g_HEADER::g_RIB \
					"${m_SHOT}${m_LAYER}_${m_TAKE}${m_VER}"
			set ::alfred::g_HEADER::m__pJOB \
					"${m_pBASE}jobs/"
			set ::alfred::g_HEADER::m__pSHADOW \
					"${m_pBASE}shadowmaps/"
			set ::alfred::g_HEADER::g_SHD \
					"$::alfred::g_HEADER::g_RIB"

			if {$m_RENRES == 1} {
				set ::alfred::g_HEADER::g_RES "hi"
			} else {
				set ::alfred::g_HEADER::g_RES "tv"
			}
			set ::alfred::g_HEADER::m__pIMAGE \
					"${m_pBASE}renders/${m_ELEM}/${::alfred::g_HEADER::g_RES}/"
			set ::alfred::g_HEADER::g_IMG \
					"${m_SHOT}${m_ELEM}_${m_TAKE}${m_VER}"

			set ::alfred::g_HEADER::g_HDR 1
		}
	}

	namespace eval mk_DPS {
		proc _main {} {
		}
	}

	namespace eval send_MAIL {
		proc _main {} {
		}
	}

	namespace eval gen_RIB {
		proc _init {} {
			set m_FARM	[::mkSRC::qVal "genRIB" "m_FARM"]
			set m_AUX_CMD	[::mkSRC::qVal "genRIB" "m_AUX_CMD"]

			# set command for generating RIB
			set m_CMD {}
			set ::alfred::gen_RIB::g_CMDs {}
			lappend m_CMD "mtor_maya4"
			lappend m_CMD "-scene $::alfred::g_HEADER::m__soSCENE"
			lappend m_CMD "-cmd workspace SetDir torRIBs \"${::alfred::g_HEADER::m__pRIB}\""
			lappend m_CMD "-cmd workspace SetDir torImgs \"${::alfred::g_HEADER::m__pIMAGE}\""
			lappend m_CMD "-cmd workspace SetDir torShadows \"${::alfred::g_HEADER::m__pSHADOW}\""
			lappend m_CMD "-cmd rg dspyName ${::alfred::g_HEADER::g_RIB}"
			lappend m_CMD "-cmd rg dspyServer ${::alfred::g_HEADER::g_display_SERVERNAME}"
			lappend m_CMD "-cmd rg dspyServerMode ${::alfred::g_HEADER::g_display_MODE}"
			lappend ::alfred::gen_RIB::g_CMDs \
		       		"workspace SetDir torRIBs \"${::alfred::g_HEADER::m__pRIB}\""
			lappend ::alfred::gen_RIB::g_CMDs \
		       		"workspace SetDir torImgs \"${::alfred::g_HEADER::m__pIMAGE}\""
			lappend ::alfred::gen_RIB::g_CMDs \
		       		"workspace SetDir torShadows \"${::alfred::g_HEADER::m__pSHADOW}\""
			lappend ::alfred::gen_RIB::g_CMDs \
		       		"rg dspyName ${::alfred::g_HEADER::g_RIB}"
			lappend ::alfred::gen_RIB::g_CMDs \
		       		"rg dspyServer ${::alfred::g_HEADER::g_display_SERVERNAME}"
			lappend ::alfred::gen_RIB::g_CMDs \
		       		"rg dspyServerMode ${::alfred::g_HEADER::g_display_MODE}"
			if {$m_AUX_CMD != ""} {
				lappend m_CMD $m_AUX_CMD
				lappend ::alfred::gen_RIB::g_CMDs $m_AUX_CMD
			}
			set ::alfred::gen_RIB::g_CMD [join $m_CMD]
		}
		proc _main {} {
			set m_FARM	[::mkSRC::qVal "genRIB" "m_FARM"]
			set m_AUX_CMD	[::mkSRC::qVal "genRIB" "m_AUX_CMD"]

			# set command for generating RIB
			set m_CMD {}
			set ::alfred::gen_RIB::g_CMDs {}
			lappend m_CMD "mtor_maya4"
			lappend m_CMD "-scene $::alfred::g_HEADER::m__soSCENE"
			lappend m_CMD "-cmd workspace SetDir torRIBs \"${::alfred::g_HEADER::m__pRIB}\""
			lappend m_CMD "-cmd workspace SetDir torImgs \"${::alfred::g_HEADER::m__pIMAGE}\""
			lappend m_CMD "-cmd workspace SetDir torShadows \"${::alfred::g_HEADER::m__pSHADOW}\""
			lappend m_CMD "-cmd rg dspyName ${::alfred::g_HEADER::g_RIB}"
			lappend m_CMD "-cmd rg dspyServer fjdsasjfd${::alfred::g_HEADER::g_display_SERVERNAME}"
			lappend m_CMD "-cmd rg dspyServerMode ${::alfred::g_HEADER::g_display_MODE}"
			lappend ::alfred::gen_RIB::g_CMDs \
		       		"workspace SetDir torRIBs \"${::alfred::g_HEADER::m__pRIB}\""
			lappend ::alfred::gen_RIB::g_CMDs \
		       		"workspace SetDir torImgs \"${::alfred::g_HEADER::m__pIMAGE}\""
			lappend ::alfred::gen_RIB::g_CMDs \
		       		"workspace SetDir torShadows \"${::alfred::g_HEADER::m__pSHADOW}\""
			lappend ::alfred::gen_RIB::g_CMDs \
		       		"rg dspyName ${::alfred::g_HEADER::g_RIB}"
			lappend ::alfred::gen_RIB::g_CMDs \
		       		"rg dspyServer ${::alfred::g_HEADER::g_display_SERVERNAME}"
			lappend ::alfred::gen_RIB::g_CMDs \
		       		"rg dspyServerMode ${::alfred::g_HEADER::g_display_MODE}"
			if {$m_AUX_CMD != ""} {
				lappend m_CMD $m_AUX_CMD
				lappend ::alfred::gen_RIB::g_CMDs $m_AUX_CMD
			}
			set ::alfred::gen_RIB::g_CMD [join $m_CMD]
		}
	}

	namespace eval ren__GROUND_SHADOW {
		proc _init {p_NAME} {
#			printf $::alfred::ren__GROUND_SHADOW::g__GROUND_SHADOWs
			set ix ""
			for {set i 0} {$i < [llength $::alfred::ren__GROUND_SHADOW::g__GROUND_SHADOWs]} {incr i} {
				if {[lindex [lindex $::alfred::ren__GROUND_SHADOW::g__GROUND_SHADOWs $i] 0] == $p_NAME} {
					set ix $i
				}
			}
			if {$ix == ""} {
				lappend ::alfred::ren__GROUND_SHADOW::g__GROUND_SHADOWs [list $p_NAME 0]
			}
#			printf $::alfred::ren__GROUND_SHADOW::g__GROUND_SHADOWs
		}
	}

	namespace eval renHERO {
		proc _init {p_NAME} {
#			printf $::alfred::renHERO::g__HEROs
			set ix ""
			for {set i 0} {$i < [llength $::alfred::renHERO::g__HEROs]} {incr i} {
				if {[lindex [lindex $::alfred::renHERO::g__HEROs $i] 0] == $p_NAME} {
					set ix $i
				}
			}
			if {$ix == ""} {
				lappend ::alfred::renHERO::g__HEROs [list $p_NAME 0]
			}
#			printf $::alfred::renHERO::g__HEROs
		}
		proc _main {g_HERO} {
#			printf "hi $g_HERO"
			set m_FARM	[::mkSRC::qVal "renHERO" "m_FARM"]
			set m_FILENAMEVAR \
					[::mkSRC::qVal "renHERO" "m_FILENAMEVAR"]
			set m_EDIT_ARGS	[::mkSRC::qVal "renHERO" "m_EDIT_ARGS"]
		}
	}

	proc print_script {} {
		.alfred.text insert end "======== global header ========\n"
		.alfred.text insert end "<g_HEADER>\n"
		if {$::alfred::g_HEADER::g_HDR == 1} {
			# set variables in ::alfred::g_HEADER
			::alfred::g_HEADER::_main

			foreach x [info vars ::alfred::g_HEADER::*] {
				upvar $x ref__x
				.alfred.text insert end "$x = \"$ref__x\"\n"
			}

			.alfred.text insert end "<mk_DPS>\n"
			if {$::alfred::mk_DPS::g__DPS == 1} {
				# set variables in ::alfred::mk_DPS
				::alfred::mk_DPS::_main

				foreach x [info vars ::alfred::mk_DPS::*] {
					upvar $x ref__x
					.alfred.text insert end "$x = \"$ref__x\"\n"
				}
			}
			.alfred.text insert end "</mk_DPS>\n"

			.alfred.text insert end "<gen_RIB>\n"
			if {$::alfred::gen_RIB::g__RIB != 0} {
				# set variables in ::alfred::gen_RIB
				::alfred::gen_RIB::_main

				foreach x [info vars ::alfred::gen_RIB::*] {
					upvar $x ref__x
					.alfred.text insert end "$x = \"$ref__x\"\n"
				}
			}
			.alfred.text insert end "</gen_RIB>\n"

			.alfred.text insert end "<renHERO>\n"
			foreach g_HERO $::alfred::renHERO::g__HEROs {
				.alfred.text insert end "HERO: $g_HERO\n"
				# set variables in ::alfred::renHERO
				::alfred::renHERO::_main $g_HERO
			}
			.alfred.text insert end "</renHERO>\n"
		}
		.alfred.text insert end "</g_HEADER>\n"
	}
}

########################################################################
#
# purpose
#	generate alfred job script
#
# return value
#	0:	OK
#	1:	ERROR
#
########################################################################
proc ::alfred::main_TEXT {file n_genRIB n_renHEROs n_ren__GROUND_SHADOWs} {
# 1. open file
	if [catch {eval set FILE_ID [open $file w]} msg] {
		printf $msg
		return 1
	}

# 2. write ALFRED job script
	# 2-1. get the frame information
	set x_fi $::alfred::g_HEADER::g_fi
	if {[parse_fi $x_fi x__fi_list o__fi_list] == 0} {
		printf "parse_fi: $fi: parse error"
		return 1
	}
	if [info exists x__fi_list] {
		set m__fi_list $x__fi_list
	} else {
		set m__fi_list $o__fi_list
	}

	# 2-2. write ALFRED job script

	#### [ALFRED BEGIN.
	set m_ALFRED___BEGIN \
"##AlfredToDo 3.0

#set AlfJobPause 1

Job -title {preview (mtor_maya4 job)} -crews {$::job::m__crews} -comment {
	#
} -envkey {} -dirmaps {} -service {} -tags {} -atleast 1 -atmost 1 -init {
	Assign output_directory $::alfred::g_HEADER::m__pBASE
	Assign rib_directory $::alfred::g_HEADER::m__pRIB
	Assign image_directory $::alfred::g_HEADER::m__pIMAGE
	Assign shadow_directory $::alfred::g_HEADER::m__pSHADOW
} -subtasks \{
"
	puts -nonewline $FILE_ID [subst $m_ALFRED___BEGIN]
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
	foreach x__fi $m__fi_list {
		set n__fi [expr $n__fi + 1]
		if {[info exists x__fi_list] == 1} {
			if {[lsearch $o__fi_list $x__fi] == -1} {
				puts -nonewline $FILE_ID "\t\t[format #%04d $x__fi]: PASS\n"
				continue
			} else {
				puts -nonewline $FILE_ID "\t\t[format #%04d $x__fi]: REDO\n"
			}
		} else {
			puts -nonewline $FILE_ID "\t\t[format #%04d $x__fi]: WORK\n"
		}

		set m_FRAME___HEAD \
"		Task -title {[format %04d $x__fi]} -subtasks \{
"
		puts -nonewline $FILE_ID [subst $m_FRAME___HEAD]

		# generating RIB...
		# NEW RIB file
		set m_RIB_NEW $::alfred::g_HEADER::m__pRIB$::alfred::g_HEADER::g_RIB.[format %04d $x__fi].rib
	if {$n_genRIB == 1} {
		set m_gen__RIB \
{			# generating RIB: [format %04d $x__fi]
			Task -title {[format %04d $x__fi]: generating RIBs} -subtasks {
				Instance {WORK - BODY -- preflight}
			} -cmds {
				# mtor_maya4
				RemoteCmd {
					$::alfred::gen_RIB::g_CMD -cmd genRib $x__fi
				} -service {pixarNrm} -atmost 1 -tags {netrender} -if {[expr $::alfred::gen_RIB::m__RIB_FORCED == 1 || ![file exists $m_RIB_NEW]]}
####				Cmd {
####					$::alfred::gen_RIB::g_CMD -cmd genRib $x__fi
####				} -service {pixarNrm} -atmost 1 -tags {netrender}
			} -cleanup {
				$m_CLEANUP
			} -chaser {
				$m_CHASER
			}
}
		set m_LL $::alfred::gen_RIB::m__LL
		if {$m_LL != ""} {
			# OLD x RIB FILE
			set m_RIB_xOLD $::alfred::g_HEADER::m__pRIB$::alfred::g_HEADER::g_RIB--.[format %04d $x__fi].rib

			set m_CLEANUP "# save original and apply command file
			Cmd {$::UNIX_MV $m_RIB_NEW $m_RIB_xOLD}
			RemoteCmd {$::WINDOWS_CMD /C \"y:/bin/perl.exe y:/usr/local/bin/berm-1.1.2-1.pl $m_RIB_xOLD $::alfred::gen_RIB::m__LL > $m_RIB_NEW\" } -service {pixarNrm} -atmost 1 -tags {netrender}
			Cmd {Alfred} -msg {File delete \"$m_RIB_xOLD\"}
			"
		} else {
			set m_CLEANUP "#..."
		}
		if {$::alfred::gen_RIB::m__CMDFILEs != ""} {
			# OLD RIB file
			set m_RIB_OLD $::alfred::g_HEADER::m__pRIB$::alfred::g_HEADER::g_RIB-.[format %04d $x__fi].rib

			set m_CLEANUP "$m_CLEANUP
			# save original and apply command file
			Cmd {$::UNIX_MV $m_RIB_NEW $m_RIB_OLD}
			RemoteCmd {$::WINDOWS_CMD /C \"y:/bin/perl.exe y:/usr/local/bin/r2r-1.07 $m_RIB_OLD $::alfred::gen_RIB::m__CMDFILEs > $m_RIB_NEW\"} -service {pixarNrm} -atmost 1 -tags {netrender}
			Cmd {Alfred} -msg {File delete \"$m_RIB_OLD\"}
			"
		}
		set m_CHASER "NOTEPAD $m_RIB_NEW"
		puts -nonewline $FILE_ID [subst $m_gen__RIB]
	# $n_genRIB == 1
	}
		# generating RIB...done!

		# rendering HEROs...
	if {$n_renHEROs != 0} {
		# HEAD
		set m_ren__HERO___HEAD \
"			# rendering HEROs...
			Task -title {[format %04d $x__fi]: rendering HEROs} -subtasks \{
"
		# BODY
		set m_ren__HERO___BODY \
{				Task -title {[format %04d $x__fi]: rendering HERO - $m_SET} -subtasks {
					$m_INSTANCE
				} -cmds {
					# render (prman or netrender)
					RemoteCmd {
						Y:/usr/local/bin/X_RENDER.BAT \${rib_directory}${::alfred::g_HEADER::g_RIB}.[format %04d $x__fi].rib "$m_EDIT_ARGS"
					} -service {pixarNrm} -atmost 1 -tags {netrender}
				} -cleanup {
					Cmd {$::UNIX_MV $m_IMG_x $m_IMG_o}
				} -chaser {
					sho $m_IMG_o
				}
}
		# TAIL
		set m_ren__HERO___TAIL \
"			\} -cmds {
			} -cleanup {
			} -chaser {
			}
"
		puts -nonewline $FILE_ID [subst $m_ren__HERO___HEAD]
		# which HEROs?
		set m_HEROs {}
		foreach m_HERO $::alfred::renHERO::g__HEROs {
			if {[lindex $m_HERO 1] == 1} {
				lappend m_HEROs $m_HERO
			}
		}
		# LOOP
		set x_xf {}
		if {$::alfred::g_HEADER::g_IMG_EXT != "iff"} {
			set x_xf .$::alfred::g_HEADER::g_IMG_EXT
		}
		for {set i 0} {$i < [llength $m_HEROs]} {incr i} {
			set m_SET [lindex [lindex $m_HEROs $i] 0]

			set m_EDIT_ARGS [::mkSRC::qVal renHERO___${m_SET} m_EDIT_ARGS]
			set m_IMG_x $::alfred::g_HEADER::m__pIMAGE$::alfred::g_HEADER::g_RIB.[format %04d $x__fi]$x_xf
			set m_IMG_o $::alfred::g_HEADER::m__pIMAGE$::alfred::g_HEADER::g_IMG[::mkSRC::qVal renHERO___${m_SET} m_FILENAMEVAR].$::alfred::g_HEADER::g_RES.[format %04d $x__fi].$::alfred::g_HEADER::g_IMG_EXT
			set m_IMG_in_DPS $::alfred::mk_DPS::g__DPSIMG_PATHNAME[format %04d $n__fi].[string toupper $::alfred::g_HEADER::g_IMG_EXT]
			if {$i == 0} {
				if {$n_genRIB == 1} {
					set m_INSTANCE "Instance {[format %04d $x__fi]: generating RIBs}"
				} else {
					set m_INSTANCE "Instance {WORK - BODY -- preflight}"
				}
			} else {
				set m_INSTANCE "Instance {[format %04d $x__fi]: rendering HERO - [lindex [lindex $m_HEROs [expr $i - 1]] 0]}"
			}
			puts -nonewline $FILE_ID [subst $m_ren__HERO___BODY]
		}
		puts -nonewline $FILE_ID [subst $m_ren__HERO___TAIL]
	# $n_renHEROs != 0
	}
		# rendering HEROs...done!

		# rendering GROUND-SHADOWs...
	if {$n_ren__GROUND_SHADOWs != 0} {
		set m_INSTANCE "Instance {[format %04d $x__fi]: generating RIBs}"
		if {$n_genRIB == 0} {
			set m_INSTANCE "Instance {WORK - BODY -- preflight}"
		}

		# HEAD
		set m_ren__GROUND_SHADOW___HEAD \
"			# rendering GROUND SHADOWs...
			Task -title {[format %04d $x__fi]: rendering GROUND SHADOWs} -subtasks \{
"
		# BODY
		set m_ren__GROUND_SHADOW___BODY \
{				Task -title {[format %04d $x__fi]: rendering GROUND SHADOW - $m_SET} -subtasks {
					$m_INSTANCE
				} -cmds {
					RemoteCmd {
						Y:/usr/local/bin/X_RENGRNDSHD.BAT ${::alfred::g_HEADER::m__pRIB}${::alfred::g_HEADER::g_RIB}.[format %04d $x__fi].rib $m_LIGHT $m_SRC $m_DEST ${::alfred::g_HEADER::m__pSHADOW} $m_SET.[format %04d $x__fi]$x_xf
					} -service {pixarNrm} -atmost 1 -tags {netrender}
####					RemoteCmd {
####						$::UNIX_BASH --login -c \"renGrndShd ${::alfred::g_HEADER::m__pRIB}${::alfred::g_HEADER::g_RIB}.[format %04d $x__fi].rib $m_LIGHT $m_SRC $m_DEST ${::alfred::g_HEADER::m__pSHADOW} $m_SET.[format %04d $x__fi]$x_xf\"
####					} -service {pixarNrm} -atmost 1 -tags {netrender}
####					Cmd {
####						$::UNIX_BASH --login -c \"renGrndShd ${::alfred::g_HEADER::m__pRIB}${::alfred::g_HEADER::g_RIB}.[format %04d $x__fi].rib $m_LIGHT $m_SRC $m_DEST ${::alfred::g_HEADER::m__pSHADOW} $m_SET.[format %04d $x__fi]$x_xf\"
####					} -service {pixarNrm} -atmost 1 -tags {netrender}
####				} -cleanup {
#					$::UNIX_BASH --login -c \"renGrndShd-1.01 ${::alfred::g_HEADER::m__pRIB}${::alfred::g_HEADER::g_RIB}.[format %04d $x__fi].rib $m_LIGHT $m_SRC $m_DEST ${::alfred::g_HEADER::m__pSHADOW} $m_SET.[format %04d $x__fi]$x_xf\"
#					$::UNIX_BASH --login -c \"renGrndShd-1.01 ${::alfred::g_HEADER::m__pRIB}${::alfred::g_HEADER::g_RIB}.[format %04d $x__fi].rib $m_LIGHT $m_SRC $m_DEST //chunji/temp/shadow/ $m_SET.[format %04d $x__fi]$x_xf\"
#					CMD /C \"NET USE & $::UNIX_BASH --login -c 'echo \\\$PATH; mount; echo \\\$USER; ls /bin/; ls /usr/local/bin/*'\"
#					CMD /C \"NET USE\"
#					CMD /C \"DIR\"
				} -chaser {
					sho ${::alfred::g_HEADER::m__pSHADOW}$m_SET.[format %04d $x__fi]$x_xf
				}
}
		# TAIL
		set m_ren__GROUND_SHADOW___TAIL \
"			\} -cmds {
			} -cleanup {
			} -chaser {
			}
"
		puts -nonewline $FILE_ID [subst $m_ren__GROUND_SHADOW___HEAD]
		set x_xf {}
		if {$::alfred::g_HEADER::g_IMG_EXT != "iff"} {
			set x_xf .$::alfred::g_HEADER::g_IMG_EXT
		}
		foreach m_SET $::alfred::ren__GROUND_SHADOW::g__GROUND_SHADOWs {
			# only for CHECKED chunk!
			if {[lindex $m_SET 1] != 1} {
				continue
			}
			set m_SET [lindex $m_SET 0]
			set x [split $m_SET _]
			set m_LIGHT [lindex $x 0]
			set m_SRC [lindex $x 1]
			set m_DEST [lindex $x 2]
			puts -nonewline $FILE_ID [subst $m_ren__GROUND_SHADOW___BODY]
		}
		puts -nonewline $FILE_ID $m_ren__GROUND_SHADOW___TAIL
	# $n_ren__GROUND_SHADOWs != 0
	}
		# rendering GROUND-SHADOWs...done!

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
	if {${::alfred::mk_DPS::g__DPS} == 2} {
		# which HERO?
		set m_HEROs {}
		foreach m_HERO $::alfred::renHERO::g__HEROs {
			if {[lindex $m_HERO 1] == 1} {
				lappend m_HEROs $m_HERO
				break
			}
		}
		# with $m_HEROs[0] (only 1 element)
		set m_SET [lindex [lindex $m_HEROs 0] 0]

		set x_xf {}
		if {$::alfred::g_HEADER::g_IMG_EXT != "iff"} {
			set x_xf .$::alfred::g_HEADER::g_IMG_EXT
		}

		# open file for it-combinerFile
		set IT_CMB $::alfred::g_HEADER::m__pJOB$::alfred::g_HEADER::g_RIB--IT-$::VERSION.cmb
		set IT__FILE_ID [open $IT_CMB w]

		# without frame number
		set m_IMG_o {$::alfred::g_HEADER::m__pIMAGE$::alfred::g_HEADER::g_IMG[::mkSRC::qVal renHERO___${m_SET} m_FILENAMEVAR].$::alfred::g_HEADER::g_RES.\\\[format %%04d \\\$x__fi\\\].$::alfred::g_HEADER::g_IMG_EXT}
		# without sequence number
		set m_IMG_in_DPS {$::alfred::mk_DPS::g__DPSIMG_PATHNAME\\\[format %%04d \\\$n__fi\\\].[string toupper $::alfred::g_HEADER::g_IMG_EXT]}

		set m_IT_FILE \
{
source \[file join $::ROOTPATH utils-$::VERSION.tcl\]

# get the frame information
set x_fi "$::alfred::g_HEADER::g_fi"
if {\[parse_fi \$x_fi x__fi_list o__fi_list\] == 0} {
	printf "parse_fi: \$fi: parse error"
	return 1
}
if \[info exists x__fi_list\] {
	set m__fi_list \$x__fi_list
} else {
	set m__fi_list \$o__fi_list
}

set n__fi 0
foreach x__fi \$m__fi_list {
	set n__fi \[expr \$n__fi + 1\]
	if {\[info exists x__fi_list\] == 1} {
		if {\[lsearch \$o__fi_list \$x__fi\] == -1} {
			# SKIP
			continue
		} else {
			# REDO
		}
	} else {
		# WORK
	}

	# without frame number
	set m_IMG_o "$::alfred::g_HEADER::m__pIMAGE$::alfred::g_HEADER::g_IMG[::mkSRC::qVal renHERO___${m_SET} m_FILENAMEVAR].$::alfred::g_HEADER::g_RES.\[format %04d \$x__fi\].$::alfred::g_HEADER::g_IMG_EXT"
	# temporary image
	set m_IMG__ "$::alfred::g_HEADER::m__pIMAGE$::alfred::g_HEADER::g_IMG[::mkSRC::qVal renHERO___${m_SET} m_FILENAMEVAR].$::alfred::g_HEADER::g_RES-.\[format %04d \$x__fi\].$::alfred::g_HEADER::g_IMG_EXT"
	# without sequence number
	set m_IMG_in_DPS "$::alfred::mk_DPS::g__DPSIMG_PATHNAME\[format %04d \$n__fi\].[string toupper $::alfred::g_HEADER::g_IMG_EXT]"

	# combiner
	combiner setExpressionIndex 1
	combiner removeElement e0
	combiner addElement image "\$m_IMG_o" {e0}
	set x "title\\\[\\\"${::alfred::g_HEADER::g_SHOT}[file extension [file rootname ${::alfred::mk_DPS::g__DPS_PATHNAME}]] ${::alfred::g_HEADER::g_LAYER} \[format %04d \$x__fi\]\\\", 10, 440\] over \\\$e0 save\\\[\\\"\$m_IMG__\\\"\\\]"
	combiner setExpression 1 \$x
	combiner doCombine \$x__fi
	set msg {}
	catch {file rename -force \$m_IMG__ \$m_IMG_in_DPS} msg
	if {\$msg != ""} {
		puts \$msg
	}
}
}
		# write CMB script
		puts -nonewline $IT__FILE_ID [subst $m_IT_FILE]

		# close CMB
		close $IT__FILE_ID

		set m_afterWORK	"#Cmd {$::UNIX_BASH --login -c \"afterWORK '$::alfred::g_HEADER::g_fi' '$m_IMG_o' '$m_IMG_in_DPS'\"}"
		set m_afterWORK	"Cmd {it -combinerFile \"$IT_CMB\" -hide} -tags {DPSCopy}"
	} else {
#		set m_afterWORK "# do NOTHING"
	}
	if {${::alfred::mk_DPS::g__DPS} == 0} {
		set m_WORK__TAIL \
"#	<TAIL>
	Task -title {WORK - TAIL} -subtasks {
		Instance {WORK - BODY}
	} -cmds {
		#
	} -cleanup {
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
	#
}
"
		puts -nonewline $FILE_ID [subst $m_ALFRED___END]
		#### .ALFRED END]

# 3. close file
		if [catch {close $FILE_ID} msg] {
			printf $msg
			return 1
		}

		# OK
		return 0
	}

########
	## [WORK TAIL BEGIN.
	set m_WORK__TAIL___BEGIN \
"#	<TAIL>
	Task -title {WORK - TAIL} -subtasks \{
		Task -title {WORK - DPS -- preflight} -subtasks {
			Instance {WORK - BODY}
		} -cmds {
		} -cleanup {
		} -chaser {
		}
"
	puts -nonewline $FILE_ID [subst $m_WORK__TAIL___BEGIN]
	## .WORK TAIL BEGIN]
	##
	set n__fi 0
	## [LOOP.
	set m_SET [lindex [lindex $m_HEROs 0] 0]
	set image_SRC_FORMAT "$::alfred::g_HEADER::m__pIMAGE$::alfred::g_HEADER::g_IMG[::mkSRC::qVal renHERO___${m_SET} m_FILENAMEVAR].$::alfred::g_HEADER::g_RES.\[format %04d \$x__fi\].$::alfred::g_HEADER::g_IMG_EXT"
	foreach x__fi $m__fi_list {
		incr n__fi
		# without frame number
		set m_image_x "$::alfred::g_HEADER::m__pIMAGE$::alfred::g_HEADER::g_IMG[::mkSRC::qVal renHERO___${m_SET} m_FILENAMEVAR].$::alfred::g_HEADER::g_RES.[format %04d $x__fi].$::alfred::g_HEADER::g_IMG_EXT"
		# temporary image
		set m_image_o "$::alfred::g_HEADER::m__pIMAGE$::alfred::g_HEADER::g_IMG[::mkSRC::qVal renHERO___${m_SET} m_FILENAMEVAR].$::alfred::g_HEADER::g_RES-.[format %04d $x__fi].$::alfred::g_HEADER::g_IMG_EXT"
		# without sequence number
		set m_image_in_DPS "$::alfred::mk_DPS::g__DPSIMG_PATHNAME[format %04d $n__fi].[string toupper $::alfred::g_HEADER::g_IMG_EXT]"

		set m_FRAME___HEAD \
"		Task -title {[format %04d $x__fi]} -subtasks \{
"
		puts -nonewline $FILE_ID [subst $m_FRAME___HEAD]

		# making TAB sign
		set m_bg $::alfred::g_HEADER::g__bg
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
				Instance {WORK - DPS -- preflight}
			} -cmds {
				RemoteCmd {
					$::SHAKE $m_image_x$m_bg -addtext \"$::alfred::g_HEADER::g_SHOW[string toupper $::alfred::g_HEADER::g_SHOT 0 0][file extension [file rootname $::alfred::mk_DPS::g__DPS_PATHNAME]] $::alfred::g_HEADER::g_LAYER $x__fi\" Arial 24 24 1 32 32 2 1 -setalpha 0 -fo $m_image_o
				} -service {shake} -atmost {1}
			} -cleanup {
#				Cmd {Alfred} -msg {File Delete "$m_image_o"}
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
	## [WORK TAIL END.
	set m_WORK__TAIL___END \
"	\} -cmds {
		Cmd {
			$::UNIX_BASH --login -c \"$::SENDMAIL $::alfred::g_HEADER::m__pJOB$::alfred::g_HEADER::g_RIB.MAIL\"
		}
	} -cleanup {
	} -chaser {
	}
#	</TAIL>
"
	puts -nonewline $FILE_ID [subst $m_WORK__TAIL___END]
	## .WORK TAIL END]
	#### .ALFRED MAIN]
	####
	####
	#### [ALFRED END.
	set m_ALFRED___END \
"\} -cleanup {
	#
}
"
	puts -nonewline $FILE_ID [subst $m_ALFRED___END]
	#### .ALFRED END]

# 3. close file
	if [catch {close $FILE_ID} msg] {
		printf $msg
		return 1
	}

	# OK
	return 0
}
