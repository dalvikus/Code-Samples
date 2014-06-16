namespace eval vars {
	# default directory for CMD file(s)
	# will be changed just after loading CMD file
	variable m__pCMDFILEs .

	# ditto for SCENE file
	# will be changed just after open SCENE file
	variable m__pSCENE .
}

namespace eval ui {
}

namespace eval mkSRC {
}
#	{variable default type(0: number, 1: string) focus widget}
set ::mkSRC::gi(gh) {
	{"m_SCENE"		""			1 0 1}

	{"g_DEPT"		"td"			1 0 1}
	{"g_SHOW"		"fs"			1 0 1}
	{"m_SEQ"		"sd"			1 0 1}
	{"g_SHOT"		"sd1"			1 0 1}
	{"g_LAYER"		"whoami"		1 0 1}
	{"m_TAKE"		"r1"			1 0 1}

	{"m_ANIMTAKE"		""			1 0 1}
	{"m_VER"		""			1 0 1}
	{"g_fi"			"1"			1 0 1}
	{"g_SCENE_EXT"		".mb"			1 0 0}
	{"m_RENRES"		"0"			0 0 0}
	{"m_ELEM"		"hero"			1 0 1}
	{"g_display_SERVERNAME"	"tiff"			1 0 1}
	{"g_display_MODE"	"rgba"			1 0 1}
	{"g_IMG_EXT"		"tif"			1 0 1}
}
set ::mkSRC::gi(last) {
	{"m_FROM"		""			1 0 0}
	{"m_TO"			""			1 0 1}
	{"m_SUBJECT"		"[ daily ]"		1 0 0}
	{"m_CONTENT"		""			1 0 0}
}

set ::mkSRC::gi(first) {
	{"g_DAILY_DATE"		""		1 0 1}

	{"m_show"		""		1 0 1}
	{"m_shot"		""		1 0 1}
	{"m_date"		""		1 0 1}
	{"m_creator"		""		1 0 1}
	{"m_mm"			""		1 0 1}
	{"m_anim"		""		1 0 1}
	{"m_fx"			""		1 0 1}
	{"m_td"			""		1 0 1}
	{"m_roto"		""		1 0 1}
	{"m_comp"		""		1 0 1}
	{"m_coord"		""		1 0 1}
	{"m_background"		""		1 0 1}
	{"m_cameramove"		""		1 0 1}
	{"m_pixs_srate"		""		1 0 1}
	{"m_audio"		""		1 0 1}
	{"m_filmio"		""		1 0 1}
	{"m_cmd"		""		1 0 1}
	{"m_notes"		""		1 0 1}
}

set ::mkSRC::gi(genRIB) {
	{"m_FARM"		"1"			0 0 0}
	{"m_CMDFILEs"		""			1 0 1}
	{"m_AUX_CMD"		""			1 0 1}
}
set ::mkSRC::gi(renHERO) {
	{"m_FARM"		"1"			0 0 0}
	{"m_FILENAMEVAR"	""			1 0 1}
	{"m_EDIT_ARGS"		""			1 0 1}
}
set ::mkSRC::gi(ren__GROUND_SHADOW) {
	{"m_NAME"		""			1 0 1}
}
#	{"m_LIGHT"		""			1 0 1}
#	{"m_SRC"		""			1 0 1}
#	{"m_DEST"		""			1 0 1}
################################################################################


namespace eval alfred {
	variable first 0
	variable gen_RIB 0

	namespace eval g_HEADER {
		# local NEW file (lo)
		variable m__loSCENE {}
		# local OLD file (lx)
		variable m__lxSCENE {}
		# server NEW file (so)
		variable m__soSCENE {}
		# server OLD file (sx)
		variable m__sxSCENE {}

		variable g_HDR 0

		variable g_DEPT ""
		variable g_LAYER ""
		variable g_SHOW ""
		variable g_SHOT ""

		variable g_fi ""
		variable g_display_SERVERNAME ""
		variable g_display_MODE ""
		variable g_IMG_EXT ""
		variable g_SRCDIR ""
		variable g_SCENE ""
		variable g_SCENE_EXT ""
		variable g_RIB ""
		variable g_SHD ""
		variable g_RES ""
		variable g_IMG ""

		# directories
		#	base
		variable m__pBASE {}
		#	rib
		variable m__pRIB {}
		#	image
		variable m__pIMAGE {}
		#	shadow
		variable m__pSHADOW {}
		#	job
		variable m__pJOB {}
	}

	namespace eval mk_DPS {
		variable g__DPS 0

		variable g__DPS_PATHNAME ""
		variable g__DPSIMG_PATHNAME ""

		proc _main {} {
		}
	}

	namespace eval send_MAIL {
		variable g__MAIL 0
	}

	namespace eval gen_RIB {
		variable m__CMDFILEs {}

		variable g__RIB 0
		variable g__RIB_DONE 0
		variable g_CMD {}
		variable g_CMDs {}
	}

	namespace eval renHERO {
		variable g__HEROs {}
	}

	namespace eval ren__GROUND_SHADOW {
		variable g__GROUND_SHADOWs {}
	}
}

namespace eval job {
	variable GH ""
	variable FIRST ""
	variable SCENE_FILE ""
	variable SRC_DIR ""
	variable CMD_DIR ""
	variable JOB_DIR ""
	variable DPS_DIR ""
	variable DPS ""

	# eXtenden CHUNK list
	#	NAME	FILENAME	TYPE	SEQ.
	variable x_chunk {
		{"global header"	""	0	0}
		{"first"		""	0	1}
		{"genRIB"		""	1	0}
		{"renHERO"		""	1	1}
		{"last"			""	0	2}
	}
}

return 1
