namespace eval gui {
	variable m__WM "window"
	variable m__VOID {>>> \ / : * ? " < > | <<<}

	proc init {} {
		toplevel .$::gui::m__WM
		wm title .$::gui::m__WM "-_-"

		::gui::FORM__gh .$::gui::m__WM
	}

	proc FORM__gh {w} {
		set f_gh $w.gh

		frame $f_gh -borderwidth 1 -relief solid
		pack $f_gh

			# head
			set f_head $f_gh.head
			frame $f_head -borderwidth 1 -relief solid
			pack $f_head
				button $f_head.b -text x
				pack $f_head.b -side left
				label $f_head.l -text "global header"
				pack $f_head.l -side left

			# body
			set f_body $f_gh.body
			frame $f_body -borderwidth 1 -relief solid
			pack $f_body
				## scene
				frame $f_body.f_scene -borderwidth 1 -relief solid
					button $f_body.f_scene.b -text scene -width 12
					pack $f_body.f_scene.b -side left
					entry $f_body.f_scene.e -textvariable ::alfred::g_HEADER::m__loSCENE -width 32
					pack $f_body.f_scene.e -side left
				pack $f_body.f_scene
				## show
				frame $f_body.f_show -borderwidth 1 -relief solid
					label $f_body.f_show.l -anchor e -text show -width 12
					pack $f_body.f_show.l -side left
					# \ / : * ? " < > |
					set ::gui::menu__SHOW [tk_optionMenu $f_body.f_show.m ::alfred::g_HEADER::m__SHOW $::gui::m__VOID]
					$::gui::menu__SHOW entryconfigure 0 -command "::gui::SHOW_selected 0"
					$f_body.f_show.m configure -width 16
					pack $f_body.f_show.m -side left
				pack $f_body.f_show
				## shot
				frame $f_body.f_shot -borderwidth 1 -relief solid
					label $f_body.f_shot.l -text shot -width 12
					pack $f_body.f_shot.l -side left
				pack $f_body.f_shot
	}
}

::gui::init
