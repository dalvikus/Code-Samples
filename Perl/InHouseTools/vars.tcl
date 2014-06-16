namespace eval mkSRC {
}
#	{variable default type(0: number, 1: string) focus widget}
set ::mkSRC::gi(gh) {
	{"m_SCENE"		""			1 0 1}

	{"g_DEPT"		"td"			1 0 1}
	{"g_SHOW"		"hm"			1 0 1}
	{"m_SEQ"		"sd"			1 0 1}
	{"g_SHOT"		"sd3"			1 0 1}
	{"g_LAYER"		"desiree"		1 0 1}
	{"m_TAKE"		"r19"			1 0 1}

	{"m_ANIMTAKE"		""			1 0 1}
	{"m_VER"		""			1 0 1}
	{"g_fi"			"1"			1 0 1}
	{"g_SCENE_EXT"		".mb"			1 0 0}
	{"m_RENRES"		"1"			0 0 0}
	{"m_ELEM"		"hero"			1 0 1}
	{"g_display_SERVERNAME"	"tiff"			1 0 0}
	{"g_display_MODE"	"rgba"			1 0 0}
	{"g_IMG_EXT"		"tif"			1 0 0}
}
set ::mkSRC::gi(last) {
	{"m_FROM"		""			1 0 1}
	{"m_TO"			""			1 0 1}
	{"m_SUBJECT"		"[ daily ]"		1 0 0}
	{"m_CONTENT"		""			1 0 0}
}

set ::mkSRC::gi(first) {
	{"g_DAILY_DATE"		""			1 0 1}

	{"m_shot"		"m_shot"		1 0 1}
	{"m_show"		"m_show"		1 0 1}
	{"m_date"		"m_date"		1 0 1}
	{"m_creator"		"m_creator"		1 0 1}
	{"m_mm"			"m_mm"			1 0 1}
	{"m_anim"		"m_anim"		1 0 1}
	{"m_fx"			"m_fx"			1 0 1}
	{"m_td"			"m_td"			1 0 1}
	{"m_roto"		"m_roto"		1 0 1}
	{"m_comp"		"m_comp"		1 0 1}
	{"m_coord"		"m_coord"		1 0 1}
	{"m_background"		"m_background"		1 0 1}
	{"m_cameramove"		"m_cameramove"		1 0 1}
	{"m_pixs_srate"		"m_pixs_srate"		1 0 1}
	{"m_audio"		"m_audio"		1 0 1}
	{"m_filmio"		"m_filmio"		1 0 1}
	{"m_cmd"		"m_cmd"			1 0 1}
	{"m_notes"		"m_notes"		1 0 1}
}

set ::mkSRC::gi(genRIB) {
	{"m_FARM"		"1"			0 0 1}
	{"m_CMDFILEs"		""			1 0 1}
	{"m_AUX_CMD"		""			1 0 1}
}
set ::mkSRC::gi(renHERO) {
	{"m_FARM"		"1"			0 0 1}
	{"m_FILENAMEVAR"	""			1 0 1}
	{"m_EDIT_ARGS"		""			1 0 1}
}

################################################################################


namespace eval alfred {
	variable first 0
	variable gen_RIB 0
	variable ren_HERO {}

	namespace eval g_HEADER {
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
		variable g_RIBDIR ""
		variable g_RIB ""
		variable g_JOBDIR ""
		variable g_SHDDIR ""
		variable g_SHD ""
		variable g_CMDDIR ""
		variable g_RES ""
		variable g_IMGDIR ""
		variable g_IMG ""
	}

	namespace eval mk_SLATE {
		variable g_SLT 0

		variable g_SLATE ""

		proc _main {} {
		}
	}

	namespace eval send_MAIL {
		variable g_MAIL 0
	}

	namespace eval gen_RIB {
		variable g_RIB 0
	}

	namespace eval ren_HEROs {
		variable g_HEROs {}
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
