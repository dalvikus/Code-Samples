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
#		g_JOBDIR	= m_OUTDIR + 'jobs/'
#
################################################################################
namespace eval mkSRC {	# global information

#	printf [::mkSRC::index {::mkSRC::gi(gh)} "m_LAYER"]
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

		frame $fw
		label $lw \
			-background $bg_COLOR \
			-justify left \
			-text ": " \
			-wraplength 4i
		entry $ew \
			-foreground $fg_COLOR \
			-highlightcolor #CC0066 \
			-highlightthickness 1 \
			-relief groove \
			\
			-width 20
#		bind $fw <Control-KeyRelease-q> "printf \"hello, world\""
		pack $lw -side left -fill both
		pack $ew -side left -expand 1 -fill both
		if {$chunk == "gh" && $word == "m_SCENE"} {
			set bw $fw.button
			button $bw \
				-background $hi_bg_COLOR \
				-relief groove \
				-text "Open Maya scene" \
				\
				-command "open_scene \"$w\" \"$fw\""
			pack $bw -side left -expand 1 -fill both
		}
		if {$chunk == "genRIB" && $word == "m_CMDFILEs"} {
			set bw $fw.button
			button $bw \
				-background $hi_bg_COLOR \
				-relief groove \
				-text "Load Command File" \
				\
				-command "load_commandfile \"$w\" \"$fw\""
			pack $bw -side left -expand 1 -fill both
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
			$w.text insert end "<$word>" TAG
			$w.text window create end -window $w.text.${chunk}__${word}
			$w.text.${chunk}__${word}.entry insert end \
				[lindex [lindex $ref $ix] 1]
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
	proc gh {w} {
		set text {#!/usr/bin/python

import string, os, sys
#	
######################## <SERVER INFORMATION> ########################
m_SERVER	= '$::SERVER';		# server name
m_DAILY		= 0;			# 0: NO, 1: YES
m_HOST		= 'jangbacksan';	# host name
if (m_DAILY == 1):
	m_WORKROOT = '/d/' + m_HOST + '/'
else:
	m_WORKROOT = '/show/'
m_WORKROOT	= '//' + m_SERVER + m_WORKROOT
######################## </SERVER INFORMATION> ########################


######################################################################
######################## <PRIME INFORMATION> ########################
m_SCENE		= <m_SCENE>	# Maya scene
# used for pathname
g_DEPT		= <g_DEPT>
g_SHOW		= <g_SHOW>
m_SEQ		= <m_SEQ>
g_SHOT		= <g_SHOT>
g_LAYER		= <g_LAYER>	# whoami
m_TAKE		= <m_TAKE>	# revision no.
# used for filename
m_ANIMTAKE	= <m_ANIMTAKE>	# which animation (g_SCENE)
m_VER		= <m_VER>	# mnemonic for use of animation(g_RIB, g_IMG)
######################## </PRIME INFORMATION> ########################
######################################################################

######################## <FRAME INFORMATION> ########################
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
# 'tiff', 'mayaiff', ...
g_display_SERVERNAME	= <g_display_SERVERNAME>
# (*generate RIB)
# 'rgb', 'rgba', 'rgbz', 'rgbaz'
g_display_MODE		= <g_display_MODE>
g_IMG_EXT		= <g_IMG_EXT>	# (*render HERO)
######################## </FRAME INFORMATION> ########################




######################################################################
######################## DIRECTORY STRUCTUREs ########################
######################################################################
#	Directories(texture, scene, command file, RIB, image)
#		g_TEXDIR	*
#		g_SRCDIR
#		g_RIBDIR
#		g_CMDDIR
#		g_IMGDIR
#	and files(scene, RIB, image)
#		g_SCENE
#		g_RIB
#		g_IMG
######################################################################

######################## <DIRECTORY: TEXTURE> ########################
g_TEXDIR	= m_WORKROOT + g_SHOW + '/Render Man Resources/'
######################## </DIRECTORY: TEXTURE> ########################

######################## <BASE DIRECTORY: WORK> ########################
m_WORKDIR	= m_WORKROOT + g_SHOW + '/' + m_SEQ + '/' + g_SHOT + '/'
######################## </BASE DIRECTORY: WORK> ########################

######################## <DIRECTORY: MAYA SCENE> ########################
x_TMP = g_SHOW			# temporary variable
if m_DAILY != 1:		# NOT daily
	x_TMP = string.upper(x_TMP)
g_SRCDIR	= m_WORKDIR + 'scene/' + g_DEPT + '/' \\
		+ x_TMP + '_' + g_SHOT + '_' + g_DEPT + '_' + g_LAYER + '_maya/scenes/'
g_SCENE		= g_SHOW + string.upper(g_DEPT) + '-' + g_SHOT + g_LAYER \\
		+ 'an' + m_ANIMTAKE + g_DEPT + m_TAKE
######################## </DIRECTORY: MAYA SCENE> ########################

######################## <BASE DIRECTORY: OUTPUT> ########################
m_OUTDIR	= m_WORKDIR + 'image/' + g_DEPT + '/' \\
		+ g_LAYER + '/' + m_TAKE + '/'
######################## </BASE DIRECTORY: OUTPUT> ########################

######################## <DIRECTORY: RIB> ########################
g_RIBDIR	= m_OUTDIR + 'ribs/'
g_RIB		= g_SHOT + g_LAYER + '_' + m_TAKE + m_VER + m_SUB
# without frame information, so filename = g_RIB + '.0001.rib'
# For shadow, if exists, g_RIB + '.key.0001.rib'
######################## </DIRECTORY: RIB> ########################
######################## <DIRECTORY: JOBs> ########################
g_JOBDIR	= m_OUTDIR + 'jobs/'
######################## </DIRECTORY: JOBs> ########################
######################## <DIRECTORY: RIB for Shadow> ########################
g_SHDDIR	= m_OUTDIR + 'shadowmaps/'
######################## </DIRECTORY: RIB for Shadow> ########################
# In tp jobfiles
#	g_SHD	= g_SHOT + g_LAYER + '_' + m_TAKE + m_SUB
# but here g_SHDRIB = g_RIB
g_SHD		= g_RIB
# without light and frame information, so filename = g_SHDRIB + '.key.shd.0001.tex'

######################## <DIRECTORY: COMMAND FILE> ########################
g_CMDDIR	= m_OUTDIR + 'cmds/'
######################## </DIRECTORY: COMMAND FILE> ########################

######################## <DIRECTORY: IMAGE> ########################
if m_RENRES == 1:
	g_RES	= 'hi'
else:
	g_RES	= 'tv'
g_IMGDIR 	= m_OUTDIR + 'renders/' + m_ELEM + '/' + g_RES + '/'
g_IMG		= g_SHOT + m_ELEM + '_' + m_TAKE + m_VER
######################## </DIRECTORY: IMAGE> ########################

######################## <MAKE DIRECTORIES> ########################
if not os.path.exists(g_SRCDIR):
	os.makedirs(g_SRCDIR)
if not os.path.exists(g_TEXDIR):
	os.makedirs(g_TEXDIR)
if not os.path.exists(g_RIBDIR):
	os.makedirs(g_RIBDIR)
if not os.path.exists(g_SHDDIR):
	os.makedirs(g_SHDDIR)
if not os.path.exists(g_JOBDIR):
	os.makedirs(g_JOBDIR)
if not os.path.exists(g_CMDDIR):
	os.makedirs(g_CMDDIR)
if not os.path.exists(g_IMGDIR):
	os.makedirs(g_IMGDIR)
######################## </MAKE DIRECTORIES> ########################

def _parse_fi(str):
	import string, re
	y = \[\]
	for x in string.split(str, ','):
		oRE = re.match('^\\s*(\\d+)\\s*(-?)\\s*(\\d*)\\s*(:?)\\s*(\\d*)\\s*$', x)
		if not oRE:
			print '%s: NOT MATCH' % (x)
			return
		f_S = oRE.group(1)
		f_R = oRE.group(2)
		f_E = oRE.group(3)
		f_B = oRE.group(4)
		f_T = oRE.group(5)

		if f_R == '-' and f_E == '':
			print '%s: NOT f_END' % (x)
			return
		if f_B == ':' and f_T == '':
			print '%s: NOT f_STEP' % (x)
			return
		if f_S != '' and f_E != '' and f_T != '':
			if f_R == '' or f_B == '':
				print '%s: NO -:' % (x)
				return

		if f_E != '' and f_T == '':
			f_T = 1

		if f_E == '':
			y.append(\[int(f_S), int(f_S), 1\])
		else:
			y.append(\[int(f_S), int(f_E), int(f_T)\])
	return y

def parse_fi(str):
	import re, string
	oRE = re.match('^\s*\\((.*?)\\)(.*)$', str)
	if not oRE:
		OLD_x = \[\]
		NEW_x = _parse_fi(str)
	else:
		OLD_x = _parse_fi(oRE.group(1))
		NEW_x = _parse_fi(oRE.group(2))

	return OLD_x, NEW_x

def make_SET(fi_list):
	import operator
	x = \[\]
	for x_fi in fi_list:
		x = operator.concat(x, range(x_fi\[0\], x_fi\[1\] + 1, x_fi\[2\]))

	return x

if __name__ == '__main__':
######################## <MAYA SCENE> ########################
	print g_SRCDIR + g_SCENE + g_SCENE_EXT
	print g_CMDDIR
	print g_JOBDIR
	if not os.path.isfile(g_SRCDIR + g_SCENE + g_SCENE_EXT):
# maya scene file does not exists
		sys.exit(1)
######################## </MAYA SCENE> ########################
#	print 'TEXTURE DIR:\t' + g_TEXDIR
#	print 'SOURCE:\t' + g_SRCDIR + g_SCENE + g_SCENE_EXT
#	print 'RIB DIR:\t' + g_RIBDIR
#	print 'RIB-:\t' + g_RIBDIR + g_RIB
#	print 'SHADOW DIR:\t' + g_SHDDIR
#	print 'SHD-:\t' + g_SHDDIR + g_RIB
#	print 'COMMAND DIR:\t' + g_CMDDIR
#	print 'IMAGE-:\t' + g_IMGDIR + g_IMG
}
	::mkSRC::displayText ".$w" "gh" [subst $text]
	}
######## </global header> ########

	proc first {w} {
		if {$::job::GH == ""} {
			destroy .$w
			printf "Visit \"global header\""
			return
		}
		set gh [file rootname [file tail $::job::GH]]
		set text {#!/usr/bin/python

import gh
import os, re, string, sys, time

g_DAILY_DATE	= <g_DAILY_DATE>	# tomorrow: YYYYMMDD (no space)

p_DPS_MEDIA_UTILITY	= '$::DPS_MEDIA_UTILITY'
m_DPS_SERVER	= '$::DPS_SERVER'
g_DPS_DIR	= 'daily/' + $gh.g_SHOW + '/'
# without '.dps'
g_DPS		= g_DAILY_DATE + '-' \\
		+ $gh.g_DEPT + '-' + $gh.g_LAYER + '-' \\
		+ $gh.g_SHOW + '.' + $gh.g_SHOT
# find next sequence number
m_DPS_LIST = os.listdir(m_DPS_SERVER + 'DPS/' + g_DPS_DIR)
ix = 0
for i in range(len(m_DPS_LIST)):
	oRE = re.match('^' + g_DPS + '.(\\d\\d\\d).dps$', m_DPS_LIST\[i\])
	if not oRE:
		continue
	if int(oRE.group(1)) > ix:
		ix = int(oRE.group(1))
ix = ix + 1

# make slate (eg. xxx0000.TIF)
p_PYTHON	= '$::wPYTHON'
m_MKSLATE_SRC	= '$::MKSLATE_SRC'
p_MKSLATE	= '$::MKSLATE'


m_shot		= $gh.g_SHOT
m_show		= $gh.g_SHOW
m_date		= time.asctime()	# time.strftime('%a %b %d %H:%M:%S %Y', time.localtime())
m_creator	= $gh.g_LAYER
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
m_filmio	= <m_filmio>
m_cmd		= <m_cmd>
m_notes		= <m_notes>


if __name__ == '__main__':
	print m_DPS_SERVER + 'DPS/' + g_DPS_DIR
	if not os.path.exists(m_DPS_SERVER + 'DPS/' + g_DPS_DIR):
		os.makedirs(m_DPS_SERVER + 'DPS/' + g_DPS_DIR)

	# 1. Ignore OLD fi list between '(' and ')'
	# 2. Only called ONCE!
	None, m_fi_list = $gh.parse_fi($gh.g_fi)
	if m_fi_list == None:
		print 'Parse Error'
		sys.exit(1)
	# 1: SLATE \[ eg. 0000.TIF \]
	x_FRAMEs = 1 + len($gh.make_SET(m_fi_list))

	# race condition?
	g_DPS += '.' + '%03d' % ix
	print 'DPS: \\\"' + m_DPS_SERVER + 'DPS/' + g_DPS_DIR + g_DPS + '.dps' + '\\\"'
#	if not os.path.exists(m_DPS_SERVER + 'DPS/' + g_DPS_DIR + g_DPS + '.dps'):
	os.system(
		p_DPS_MEDIA_UTILITY + \
			' -batch ' + \
			' -d' + str(x_FRAMEs) + \
			' -f' + m_DPS_SERVER + 'DPS/' + g_DPS_DIR + g_DPS + '.dps'
	)

#	os.system('cp ' + m_MKSLATE_SRC + p_MKSLATE + ' ' + p_MKSLATE)
#	os.system(p_PYTHON + ' ' + p_MKSLATE + ' first.' + $gh.g_IMG_EXT)
#p_PYTHON	= '$::wPYTHON'
#m_MKSLATE_SRC	= '$::MKSLATE_SRC'
#	os.system(p_PYTHON + ' ' + m_MKSLATE_SRC + p_MKSLATE + ' ' + sys.argv\[0\] + ' first.' + string.upper($gh.g_IMG_EXT))

	print 'mv [file dirname $::job::GH]/first.TIF' + $gh.g_IMG_EXT + ' ' + m_DPS_SERVER + string.upper($gh.g_IMG_EXT) + '/' + g_DPS_DIR + g_DPS + '.dps/' + g_DPS + '0000.' + string.upper($gh.g_IMG_EXT)
	x_xf = ''
	if string.upper($gh.g_IMG_EXT) == 'PIC':
		x_xf = '.'
	os.system('mv [file dirname $::job::GH]/first.TIF ' + m_DPS_SERVER + string.upper($gh.g_IMG_EXT) + '/' + g_DPS_DIR + g_DPS + '.dps/' + g_DPS + x_xf + '0000.' + string.upper($gh.g_IMG_EXT))
}
	::mkSRC::displayText ".$w" "first" [subst $text]
	}
######## <generate RIB> ########
	proc genRIB {w} {
		if {$::job::GH == ""} {
			destroy .$w
			printf "Visit \"global header\""
			return
		}
		set gh [file rootname [file tail $::job::GH]]
		set text {#!/usr/bin/python

print 'Generate RIB......'
import $gh
import getopt, os, re, string, sys

try:
	opts, args = getopt.getopt(sys.argv\[1:\], 'f:', \['frame='\])
except getopt.GetoptError, why:
	print "%s: getopt.GetoptError: %s" % (sys.argv\[0\], why)
	sys.exit(1)
# frame information
x_fi = ''
for x_o, x_a in opts:
	if x_o in ('-f', '-frame'):
		x_fi = x_a
if x_fi == '':
	x_fi = $gh.g_fi
# OLD fi list: m_fi_list_x
# NEW fi list: m_fi_list_o
m_fi_list_x, m_fi_list_o = $gh.parse_fi(x_fi)
if m_fi_list_x == None or m_fi_list_o == None:
	print 'Parse Error'
	sys.exit(1)

m_FARM		= <m_FARM>	# 0: No, 1: Yes

# Command file(s)
m_CMDFILEs	= <m_CMDFILEs>
x_CMDLIST = string.split(m_CMDFILEs, ',')
m_CMDLIST = \[\]
for x_CMD in x_CMDLIST:
	x_RE = re.match('^\s*$', x_CMD)
	if not x_RE:
		m_CMDLIST.append(x_CMD)
for i in range(len(m_CMDLIST)):
	if m_FARM:
		m_CMDLIST\[i\] = '\\'' + $gh.g_CMDDIR + string.strip(m_CMDLIST\[i\]) + '\\''
	else:
		m_CMDLIST\[i\] = '\"' + $gh.g_CMDDIR + string.strip(m_CMDLIST\[i\]) + '\"'

m_command = \['mtor_maya4'\]

# scene
m_command.append(
	'-scene '
		'' + $gh.g_SRCDIR + $gh.g_SCENE + $gh.g_SCENE_EXT
)
print m_command\[-1\]	# last element

#m_CMDLIST = string.split(m_CMDFILEs, ',')
# workspace environment
#m_command.append(
#	'-cmd workspace AddSearchPath \\\"' + $gh.g_TEXDIR + '\\\"'
#)
print m_command\[-1\]	# last element
# RIB
m_command.append(
	'-cmd workspace SetDir torRIBs \\\"' + $gh.g_RIBDIR + '\\\"'
)
print m_command\[-1\]	# last element
# image
m_command.append(
	'-cmd workspace SetDir torImgs \\\"' + $gh.g_IMGDIR + '\\\"'
)
print m_command\[-1\]	# last element
# shadow
m_command.append(
	'-cmd workspace SetDir torShadows \\\"' + $gh.g_SHDDIR + '\\\"'
)
print m_command\[-1\]	# last element

# basename
m_command.append(
	'-cmd rg dspyName '
		'' + $gh.g_RIB		# basename
)
print m_command\[-1\]	# last element

# display server name and mode
m_command.append(
	'-cmd rg dspyServer '
		'' + $gh.g_display_SERVERNAME
)
print m_command\[-1\]	# last element
m_command.append(
	'-cmd rg dspyServerMode '
		'' + $gh.g_display_MODE
)
print m_command\[-1\]	# last element

# 1. Specify camera name
#	-cmd rg camName perspShape
# 2. Add workspace environment variable, i.e. AddSearchPath
#	-cmd workspace AddSearchPath \\\\"//chunji/show/hm/Render Man Resources\\\\"
m_AUX_CMD	= <m_AUX_CMD>
if m_AUX_CMD != '':
	m_command.append(m_AUX_CMD)
	print m_command\[-1\]

if m_FARM:
	m_HEAD	= \\
		'##AlfredToDo 3.0\\n' \\
		'\\n' \\
		'#set AlfJobPause \{1\}\\n' \\
		'\\n' \\
		'Job -title \{Generating RIBs...\} -comment \{\\n' \\
		'	#\\n' \\
		'\} -envkey \{\} -dirmaps \{\} -service \{\} -tags \{\} -atleast 1 -atmost 1 -init \{\\n' \\
		'\} -subtasks \{\\n'
	m_BODY	= \[\]
	m_TAIL	= '\} -cleanup \{\}\\n'

i_command = string.join(m_command)
print 'command = <' + i_command + '...>'
if len(m_fi_list_x):
	m_fi_list = m_fi_list_x
	m_fi_list_o_SET = $gh.make_SET(m_fi_list_o)
else:
	m_fi_list = m_fi_list_o
for x_fi in m_fi_list:
#	print '#: %d, %s' % (len(x_fi), x_fi)
	m_fi_START = x_fi\[0\]
	m_fi_END = x_fi\[1\]
	m_fi_STEP = x_fi\[2\]
	if m_FARM:
		for i in range(m_fi_START, m_fi_END + 1, m_fi_STEP):
			if len(m_fi_list_x) and i not in m_fi_list_o_SET:
				print 'frame %d: PASS!' % (i)
				continue
			x_RIB	= $gh.g_RIB + '.' + '%04d' % (i)
#
			o_RIB	= $gh.g_RIBDIR + x_RIB + '-.rib'
			i_RIB	= $gh.g_RIBDIR + x_RIB + '.rib'
			if len(m_CMDLIST):
				x_CMD	= \\
					'		Cmd \{mv ' + i_RIB + ' ' + o_RIB + '\}\\n' \\
					'		Cmd \{sh -c \"r2r ' + o_RIB + ' ' + string.join(m_CMDLIST) + ' > ' + i_RIB + '\"\}\\n'
			else:
				x_CMD	= \\
					'\\n'
			m_BODY.append(
				'	Task -title \{genRIB-' + str(i) + '\} -subtasks \{\} -cmds \{\\n'
				'		RemoteCmd \{' + i_command + ' -cmd genRib ' + str(i) + '\} -service \{pixarNrm\} -tags \{prman\}\\n'
				'#		Cmd \{' + i_command + ' -cmd genRib ' + str(i) + '\} -service \{pixarNrm\} -tags \{prman\}\\n'
				'	\} -cleanup \{\\n'
				'' + x_CMD + ''
				'	\} -chaser \{\}\\n'
			)
	else:
		m_command.append(
			'-cmd \\''
				'for \{set i ' + str(m_fi_START) + '\} '
				'\{\$i <= ' + str(m_fi_END) + '\} '
				'\{\} \{'
					'genRib \$i; '
					'set i \[expr \$i + ' + str(m_fi_STEP) + ']'
				'\}'
			'\\''
		)
		print m_command\[-1\]	# last element
# run a command
if m_FARM:
	m_ALF	= '[file dirname $::job::GH]/genRIB.alf'
	m_of	= open(m_ALF, 'w')
	m_of.write(m_HEAD + string.join(m_BODY) + m_TAIL)
	m_of.close()
	#print m_HEAD
	#print string.join(m_BODY)
	#print m_TAIL
 	os.system('alfred ' + m_ALF)
	print 'ALFRED: \\\"' + m_ALF + '\\\"'
else:
	print 'm_command = <' + string.join(m_command) + '>'
	os.system(string.join(m_command))


if m_FARM or len(m_CMDLIST) == 0:
	sys.exit(0)

# len(m_CMDLIST) != 0
for x_fi in m_fi_list:
#	print '#: %d, %s' % (len(x_fi), x_fi)
	m_fi_START = x_fi\[0\]
	m_fi_END = x_fi\[1\]
	m_fi_STEP = x_fi\[2\]
#
	for i in range(m_fi_START, m_fi_END + 1, m_fi_STEP):
		if len(m_fi_list_x) and i not in m_fi_list_o:
			continue
		x_FRAME		= '%04d' % (i)
		x_RIB		= $gh.g_RIB + '.' + x_FRAME
#
		o_RIB		= $gh.g_RIBDIR + x_RIB + '-.rib'
		i_RIB		= $gh.g_RIBDIR + x_RIB + '.rib'
#		print '# of command files = ' + str(len(m_CMDLIST))
		os.system('mv ' + i_RIB + ' ' + o_RIB)
		os.system('r2r ' + o_RIB + ' ' + string.join(m_CMDLIST) + ' > ' + i_RIB)
}
	::mkSRC::displayText ".$w" "genRIB" [subst $text]
	}
######## </generate RIB> ########




######## <render Hero> ########
	proc renHERO {w} {
		if {$::job::GH == ""} {
			destroy .$w
			printf "Visit \"global header\""
			return
		}
		set gh [file rootname [file tail $::job::GH]]
		set first [file rootname [file tail $::job::FIRST]]
		if {$::alfred::mk_SLATE::g_SLT == 1} {
			set first "import [file rootname [file tail $::job::FIRST]]"
			set ff {}
		} else {
			set first {}
			set ff #
		}
		set text {#!/usr/bin/python

print 'Rendering Hero......'
import [file rootname [file tail $::job::GH]]
$first
import getopt, os, string, sys

try:
	opts, args = getopt.getopt(sys.argv\[1:\], 'f:', \['frame='\])
except getopt.GetoptError, why:
	print "%s: getopt.GetoptError: %s" % (sys.argv\[0\], why)
	sys.exit(1)
# frame information
x_fi = ''
for x_o, x_a in opts:
	if x_o in ('-f', '-frame'):
		x_fi = x_a
if x_fi == '':
	x_fi = $gh.g_fi
# OLD fi list: m_fi_list_x
# NEW fi list: m_fi_list_o
m_fi_list_x, m_fi_list_o = $gh.parse_fi(x_fi)
if m_fi_list_x == None or m_fi_list_o == None:
	print 'Parse Error'
	sys.exit(1)

m_FARM		= <m_FARM>	# 0: No, 1: Yes
m_FILENAMEVAR	= <m_FILENAMEVAR>	# distinct name appended to $gh.g_IMG
m_EDIT_ARGS	= <m_EDIT_ARGS>		# '-e \\* -r ballA'

if m_FARM:
	m_HEAD	= \\
		'##AlfredToDo 3.0\\n' \\
		'\\n' \\
		'#set AlfJobPause \{1\}\\n' \\
		'\\n' \\
		'Job -title \{\} -comment \{\\n' \\
		'	#\\n' \\
		'\} -envkey \{\} -dirmaps \{\} -service \{\} -tags \{\} -atleast 1 -atmost 1 -init \{\\n' \\
		'\} -subtasks \{\\n'
	m_BODY	= \[\]
	m_TAIL	= '\} -cleanup \{\}\\n'

m_DPS_IMG	= '${::alfred::mk_SLATE::g_SLATE}'
m_DPS_IMG	= string.replace(m_DPS_IMG, '/DPS/', '/' + string.upper($gh.g_IMG_EXT) + '/', 1)
m_DPS_IMG	= m_DPS_IMG + '/' + os.path.splitext(os.path.basename(m_DPS_IMG))\[0\]
x_xf = ''
if string.upper($gh.g_IMG_EXT) == 'PIC':
	x_xf = '.'
m_DPS_IMG	= m_DPS_IMG + x_xf


m_fi_list_x_SET = $gh.make_SET(m_fi_list_x)
m_fi_list_o_SET = $gh.make_SET(m_fi_list_o)
if len(m_fi_list_x_SET):
	m_fi_list = m_fi_list_x_SET
else:
	m_fi_list = m_fi_list_o_SET
for i in range(len(m_fi_list)):
	if len(m_fi_list_x_SET) and m_fi_list_x_SET\[i\] not in m_fi_list_o_SET:
		print 'm_fi_list\[%d\] = %d: PASS!' % (i, m_fi_list\[i\])
		continue
	x_FRAME		= '%04d' % (m_fi_list\[i\])
#
	x_RIB	= $gh.g_RIB + '.' + x_FRAME
	i_RIB	= $gh.g_RIBDIR + x_RIB + '.rib'
	o_RIB	= $gh.g_RIBDIR + x_RIB + '--.rib'
#	print i_RIB
#
	x_IMG	= $gh.g_IMG + m_FILENAMEVAR \\
		+ '.' + $gh.g_RES + '.' + x_FRAME
	i_IMG	= $gh.g_IMGDIR + x_IMG + '.' + $gh.g_IMG_EXT
#	print i_IMG

	x_f = i + 1
	i_DPS		= m_DPS_IMG + '%04d' % (x_f) + '.' + string.upper($gh.g_IMG_EXT)
#	print i_DPS
# render
##	if m_EDIT_ARGS == '':
##		os.system('prman ' + i_RIB)
##	else:
##		os.system('ribEdit ' + i_RIB + ' ' + m_EDIT_ARGS + ' | prman')
##	os.system('mv ' + $gh.g_IMGDIR + x_RIB + ' ' + i_IMG)
	x_xf = ''	# iff
	if $gh.g_IMG_EXT == 'tif':
		x_xf = '.tif'
	if m_EDIT_ARGS != '':
		if not os.path.isfile(o_RIB):
			os.system('mv ' + i_RIB + ' ' + o_RIB)
		os.system('ribEdit ' + o_RIB + ' ' + m_EDIT_ARGS + ' > ' + i_RIB)
	if m_FARM:
		m_BODY.append(
			'	Task -title \{renHERO-' + str(m_fi_list\[i\]) + '\} -subtasks \{\} -cmds \{\\n'
			'		RemoteCmd \{prman -Progress ' + i_RIB + '\} -service \{pixarNrm\} -tags \{prman\}\\n'
			'#		Cmd \{netrender -f -Progress %H ' + i_RIB + '\} -service \{pixarNrm\} -tags \{prman\}\\n'
			'	\} -cleanup \{\\n'
			'		Cmd \{mv ' + $gh.g_IMGDIR + x_RIB + x_xf + ' ' + i_IMG + '\}\\n'
			'$ff		Cmd \{cp ' + i_IMG + ' ' + i_DPS + '\}\\n'
			'	\} -chaser \{\}\\n'
		)
	else:
		os.system('prman ' + i_RIB)
		os.system('mv ' + $gh.g_IMGDIR + x_RIB + x_xf + ' ' + i_IMG)
		os.system('cp ' + i_IMG + ' ' + i_DPS)
if m_FARM:
	x_basename	= string.split(os.path.basename(sys.argv\[0\]), '.')
	x_basename\[0\]	= '[file dirname $::job::GH]/' \\
			+ x_basename\[0\]
	m_of	= open(x_basename\[0\] + '.alf', 'w')
	m_of.write(m_HEAD + string.join(m_BODY) + m_TAIL)
	m_of.close()

	os.system('alfred ' + x_basename\[0\] + '.alf')
	print 'ALFRED: \\\"' + x_basename\[0\] + '.alf\\\"'
}
#	.$w.text insert end [subst $text]
	::mkSRC::displayText ".$w" "renHERO" [subst $text]
	}
######## <render Hero> ########

	proc last {w} {
		if {$::job::FIRST == ""} {
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
		return
	}

	proc misc {w} {
		set text "#!/usr/bin/python

import gh

print 'Hmm......'"
		.$w.text insert end $text
	}
}

proc load_commandfile {w fw} {
	if {[info vars ::job::CMD_DIR] == ""} {
		return
	}
#
	if ![file isdirectory $::job::CMD_DIR] {
		return
	}
#
######## <types> ########
	set types {
		{"Command Files"	".cmd"}
		{"All Files"		"*"}
	}
######## </types> ########
	set file [
		tk_getOpenFile \
			-defaultextension .mb \
			-filetypes $types \
			-initialdir $::job::CMD_DIR \
			-parent $w \
			-title "tk_getOpenFile: Command File"
	]
	if {$file != ""} {
#		printf "$file, [file exists $file], [file isdirectory $file], [file isfile $file]"
		if [file isfile $file] {
#			printf "$::job::CMD_DIR, $file, [file dirname $file]"
			if {$::job::CMD_DIR != "[file dirname $file]/"} {
				file copy -force $file $::job::CMD_DIR
			}
			set x [file tail $file]
			if {[$fw.entry get] != ""} {
				set x ", $x"
			}
			$fw.entry insert end $x
		}
	} else {
		printf "No file(open) specified"
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
			-initialdir . \
			-parent $w \
			-title "tk_getOpenFile: Open Maya scene"
	]
	if {$file != ""} {
#		printf "$file, [file exists $file], [file isdirectory $file], [file isfile $file]"
		if [file isfile $file] {
			set ::job::SCENE_FILE $file
			$fw.entry insert end [file tail $file]
		} else {
			set ::job::SCENE_FILE ""
		}
	} else {
		printf "No file(open) specified"
	}
}


namespace eval alfred {
	namespace eval g_HEADER {
		proc _main {} {
			set m_WORKROOT "//${::SERVER}/show/"

			set m_SCENE	[::mkSRC::qVal "gh" "m_SCENE"]

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

			set m_OUTDIR	"${m_WORKDIR}image/${m_DEPT}/${m_LAYER}/${m_TAKE}/"
			set ::alfred::g_HEADER::g_RIBDIR \
					"${m_OUTDIR}ribs/"
			set ::alfred::g_HEADER::g_RIB \
					"${m_SHOT}${m_LAYER}_${m_TAKE}${m_VER}"
			set ::alfred::g_HEADER::g_JOBDIR \
					"${m_OUTDIR}jobs/"
			set ::alfred::g_HEADER::g_SHDDIR \
					"${m_OUTDIR}shadowmaps/"
			set ::alfred::g_HEADER::g_SHD \
					"$::alfred::g_HEADER::g_RIB"
			set ::alfred::g_HEADER::g_CMDDIR \
					"${m_OUTDIR}cmds/"

			if {$m_RENRES == 1} {
				set ::alfred::g_HEADER::g_RES "hi"
			} else {
				set ::alfred::g_HEADER::g_RES "tv"
			}
			set ::alfred::g_HEADER::g_IMGDIR \
					"${m_OUTDIR}renders/${m_ELEM}/${::alfred::g_HEADER::g_RES}/"
			set ::alfred::g_HEADER::g_IMG \
					"${m_SHOT}${m_ELEM}_${m_TAKE}${m_VER}"
		}
	}

	namespace eval mk_SLATE {
		proc _main {} {
		}
	}

	namespace eval send_MAIL {
		proc _main {} {
		}
	}

	namespace eval gen_RIB {
		proc _main {} {
			set m_FARM	[::mkSRC::qVal "genRIB" "m_FARM"]
			set m_CMDLIST {}
			foreach x_CMD [split [::mkSRC::qVal "genRIB" "m_CMDFILEs"] ","] {
				set x_CMD [string trim $x_CMD]
				if {$x_CMD != ""} {
					lappend m_CMDLIST $x_CMD
				}
			}
#			printf "\['[join $m_CMDLIST "', '"]'\]"
			set m_AUX_CMD	[::mkSRC::qVal "genRIB" "m_AUX_CMD"]

			# g_fi
			if {[parse_fi $::alfred::g_HEADER::g_fi x__fi_list o__fi_list] == 0} {
				printf "parse_fi: $fi: parse error"
			} else {
				if [info exists x__fi_list] {
					printf "x__fi_list = \[[join $x__fi_list ", "]\]"
				}
				if [info exists o__fi_list] {
					printf "o__fi_list = \[[join $o__fi_list ", "]\]"
				}
			}

			# set command for generating RIB
			set m_CMD {}
			lappend m_CMD "mtor_maya4"
			lappend m_CMD "-scene ${::alfred::g_HEADER::g_SRCDIR}${::alfred::g_HEADER::g_SCENE}${::alfred::g_HEADER::g_SCENE_EXT}"
			lappend m_CMD "-cmd workspace SetDir torRIBs \"${::alfred::g_HEADER::g_RIBDIR}\""
			lappend m_CMD "-cmd workspace SetDir torImgs \"${::alfred::g_HEADER::g_IMGDIR}\""
			lappend m_CMD "-cmd workspace SetDir torShadows \"${::alfred::g_HEADER::g_SHDDIR}\""
			lappend m_CMD "-cmd rg dspyName ${::alfred::g_HEADER::g_RIB}"
			lappend m_CMD "-cmd rg dspyServer ${::alfred::g_HEADER::g_display_SERVERNAME}"
			lappend m_CMD "-cmd rg dspyServerMode ${::alfred::g_HEADER::g_display_MODE}"
			if {$m_AUX_CMD != ""} {
				lappend m_CMD $m_AUX_CMD
			}
			printf [join $m_CMD]
		}
	}

	namespace eval ren_HEROs {
		proc _main {g_HERO} {
			printf "hi $g_HERO"
			set m_FARM	[::mkSRC::qVal "renHERO" "m_FARM"]
			set m_FILENAMEVAR \
					[::mkSRC::qVal "renHERO" "m_FILENAMEVAR"]
			set m_EDIT_ARGS	[::mkSRC::qVal "renHERO" "m_EDIT_ARGS"]

			# g_fi
			if {[parse_fi $::alfred::g_HEADER::g_fi x__fi_list o__fi_list] == 0} {
				printf "parse_fi: $fi: parse error"
			} else {
				if [info exists x__fi_list] {
					printf "x__fi_list = \[[join $x__fi_list ", "]\]"
				}
				if [info exists o__fi_list] {
					printf "o__fi_list = \[[join $o__fi_list ", "]\]"
				}
			}
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

			.alfred.text insert end "<mk_SLATE>\n"
			if {$::alfred::mk_SLATE::g_SLT == 1} {
				# set variables in ::alfred::mk_SLATE
				::alfred::mk_SLATE::_main

				foreach x [info vars ::alfred::mk_SLATE::*] {
					upvar $x ref__x
					.alfred.text insert end "$x = \"$ref__x\"\n"
				}
			}
			.alfred.text insert end "</mk_SLATE>\n"

			.alfred.text insert end "<gen_RIB>\n"
			if {$::alfred::gen_RIB::g_RIB != 0} {
				# set variables in ::alfred::gen_RIB
				::alfred::gen_RIB::_main

				foreach x [info vars ::alfred::gen_RIB::*] {
					upvar $x ref__x
					.alfred.text insert end "$x = \"$ref__x\"\n"
				}
			}
			.alfred.text insert end "</gen_RIB>\n"

			.alfred.text insert end "<ren_HEROs>\n"
			foreach g_HERO $::alfred::ren_HEROs::g_HEROs {
				.alfred.text insert end "HERO: $g_HERO\n"
				# set variables in ::alfred::ren_HEROs
				::alfred::ren_HEROs::_main $g_HERO
			}
			.alfred.text insert end "</ren_HEROs>\n"
		}
		.alfred.text insert end "</g_HEADER>\n"
	}
}
