namespace eval CDps { 
     variable date		[clock format [clock scan tomorrow] -format "%Y%m%d"]
     variable mm 		""
     variable anim 		""
     variable fx 		""
     variable td 		""
     variable roto 		""
     variable comp 		""
     variable coord 		""
     variable background ""
     variable cameraMove ""
     variable pixelRate 	""
     variable audio 		""
     variable filmIo 	""
     variable cmd 		""
     variable notes 		""
}     

proc CDps::init {} {
}

proc CDps::create { w } {
	set frame [$w insert end pgDps -text "DPS"]
	$frame configure -relief groov -bd 1
	
	ScrolledWindow $frame.swDps  -relief sunken -bd 1 -scrollbar vertical
	ScrollableFrame $frame.sfDps -constrainedwidth 1
	$frame.swDps setwidget $frame.sfDps
	set subFrame [$frame.sfDps getframe]

	label $subFrame.laDate -text "date" -font "arial 12 bold"
	entry $subFrame.enDate -width 20 -font "arial 12 bold" -textvariable CDps::date -highlightcolor #00CC66 -highlightthickness 3
	
	label $subFrame.laMm -text "mm" -font "arial 12 bold"
	entry $subFrame.enMm -width 20 -font "arial 12 bold" -textvariable CDps::mm -highlightcolor #00CC66 -highlightthickness 3

	label $subFrame.laAnim -text "anim" -font "arial 12 bold"
	entry $subFrame.enAnim -width 20 -font "arial 12 bold" -textvariable CDps::anim -highlightcolor #00CC66 -highlightthickness 3

	label $subFrame.laFx -text "fx" -font "arial 12 bold"
	entry $subFrame.enFx -width 20 -font "arial 12 bold" -textvariable CDps::fx -highlightcolor #00CC66 -highlightthickness 3

	label $subFrame.laTd -text "td" -font "arial 12 bold"
	entry $subFrame.enTd -width 20 -font "arial 12 bold" -textvariable CDps::td -highlightcolor #00CC66 -highlightthickness 3

	label $subFrame.laRoto -text "roto" -font "arial 12 bold"
	entry $subFrame.enRoto -width 20 -font "arial 12 bold" -textvariable CDps::roto -highlightcolor #00CC66 -highlightthickness 3

	label $subFrame.laComp -text "comp" -font "arial 12 bold"
	entry $subFrame.enComp -width 20 -font "arial 12 bold" -textvariable CDps::comp -highlightcolor #00CC66 -highlightthickness 3

	label $subFrame.laCoord -text "coord" -font "arial 12 bold"
	entry $subFrame.enCoord -width 20 -font "arial 12 bold" -textvariable CDps::coord -highlightcolor #00CC66 -highlightthickness 3

	label $subFrame.laBackground -text "background" -font "arial 12 bold"
	entry $subFrame.enBackground -width 20 -font "arial 12 bold" -textvariable CDps::background -highlightcolor #00CC66 -highlightthickness 3

	label $subFrame.laCameraMove -text "camera move" -font "arial 12 bold"
	entry $subFrame.enCameraMove -width 20 -font "arial 12 bold" -textvariable CDps::cameraMove -highlightcolor #00CC66 -highlightthickness 3

	label $subFrame.laPixsRate -text "pixel rate" -font "arial 12 bold"
	entry $subFrame.enPixsRate -width 20 -font "arial 12 bold" -textvariable CDps::pixelRate -highlightcolor #00CC66 -highlightthickness 3

	label $subFrame.laAudio -text "audio" -font "arial 12 bold"
	entry $subFrame.enAudio -width 20 -font "arial 12 bold" -textvariable CDps::audio -highlightcolor #00CC66 -highlightthickness 3

	label $subFrame.laFilmio -text "filmIo" -font "arial 12 bold"
	entry $subFrame.enFilmio -width 20 -font "arial 12 bold" -textvariable CDps::filmIo -highlightcolor #00CC66 -highlightthickness 3

	label $subFrame.laCmd -text "cmd" -font "arial 12 bold"
	entry $subFrame.enCmd -width 45 -font "arial 10 bold" -textvariable CDps::cmd -highlightcolor #00CC66 -highlightthickness 3

	label $subFrame.laNote -text "notes" -font "arial 12 bold"
	entry $subFrame.enNote -width 45 -font "arial 10 bold" -textvariable CDps::notes -highlightcolor #00CC66 -highlightthickness 3

	#-----------------------------------------------------------------------#	

	grid columnconfigure $subFrame 0 -weight 1
	grid columnconfigure $subFrame 1 -weight 1

	grid $subFrame.laDate -row 0 -column 0 -pady 2 -padx 7 -sticky w
	grid $subFrame.enDate -row 0 -column 1 -pady 2 -padx 5 -sticky e

	grid $subFrame.laMm -row 1 -column 0 -pady 2 -padx 7 -sticky w
	grid $subFrame.enMm -row 1 -column 1 -pady 2 -padx 5 -sticky e

	grid $subFrame.laAnim -row 2 -column 0 -pady 2 -padx 7 -sticky w
	grid $subFrame.enAnim -row 2 -column 1 -pady 2 -padx 5 -sticky e

	grid $subFrame.laFx -row 3 -column 0 -pady 2 -padx 7 -sticky w	
	grid $subFrame.enFx -row 3 -column 1 -pady 2 -padx 5 -sticky e

	grid $subFrame.laTd -row 4 -column 0 -pady 2 -padx 7 -sticky w
	grid $subFrame.enTd -row 4 -column 1 -pady 2 -padx 5 -sticky e

	grid $subFrame.laRoto -row 5 -column 0 -pady 2 -padx 7 -sticky w
	grid $subFrame.enRoto -row 5 -column 1 -pady 2 -padx 5 -sticky e

	grid $subFrame.laComp -row 6 -column 0 -pady 2 -padx 7 -sticky w
	grid $subFrame.enComp -row 6 -column 1 -pady 2 -padx 5 -sticky e

	grid $subFrame.laCoord -row 7 -column 0 -pady 2 -padx 7 -sticky w
	grid $subFrame.enCoord -row 7 -column 1 -pady 2 -padx 5 -sticky e

	grid $subFrame.laBackground -row 8 -column 0 -pady 2 -padx 7 -sticky w
	grid $subFrame.enBackground -row 8 -column 1 -pady 2 -padx 5 -sticky e

	grid $subFrame.laCameraMove -row 9 -column 0 -pady 2 -padx 7 -sticky w
	grid $subFrame.enCameraMove -row 9 -column 1 -pady 2 -padx 5 -sticky e

	grid $subFrame.laPixsRate -row 10 -column 0 -pady 2 -padx 7 -sticky w
	grid $subFrame.enPixsRate -row 10 -column 1 -pady 2 -padx 5 -sticky e

	grid $subFrame.laAudio -row 11 -column 0 -pady 2 -padx 7 -sticky w
	grid $subFrame.enAudio -row 11 -column 1 -pady 2 -padx 5 -sticky e

	grid $subFrame.laFilmio -row 12 -column 0 -pady 2 -padx 7 -sticky w
	grid $subFrame.enFilmio -row 12 -column 1 -pady 2 -padx 5 -sticky e

	grid $subFrame.laCmd -row 13 -column 0 -pady 2 -padx 7 -sticky w
	grid $subFrame.enCmd -row 13 -column 1 -pady 2 -padx 5 -sticky e

	grid $subFrame.laNote -row 14 -column 0 -pady 2 -padx 7 -sticky w
	grid $subFrame.enNote -row 14 -column 1 -pady 2 -padx 5 -sticky e

	pack $frame.sfDps -padx 2 -fill both -expand yes
	pack $frame.swDps -padx 2 -fill both -expand yes
}
