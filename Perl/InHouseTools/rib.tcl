namespace eval CRib {
     variable mtorDo	      1
     variable cameraName      ""
     variable mtorID	      "hero"
     variable startFrame      1
     variable endFrame	      1
     variable frameInterval   1
     variable frameInf	      ""
     variable dspyServer      ""
     variable dspyMode	      ""
     variable formatX	      ""
     variable foramtY	      ""
     variable pixelSampleX    ""
     variable pixelSampleY    ""
     variable shadingRate     ""
     variable cmd	      ""
     variable bgName	      ""
     variable ribEditOption   ""
     variable mtorCmd	      ""
     variable cmdFile {{{cmd file} {.dscmd}}}
} 

proc CRib::init {} {
}

proc CRib::getRibEditOption {} {
     set CRib::ribEditOption [$::tfRibEdit.frRibEdit.txRibEdit get 0.0 end]
}

proc CRib::setRibEditOption { str } {
     $::tfRibEdit.frRibEdit.txRibEdit insert 0.0 $CRib::ribEditOption
}

proc CRib::create { w } {
	set frame [$w insert end pgRib -text "RIB"]
	$frame configure -relief groov -bd 1
	TitleFrame $frame.tfMtor	   -text "Gen Rib"
	TitleFrame $frame.tfRibEdit	   -text "Rib Edit"

	set ::tfMtor [$frame.tfMtor getframe]
	set ::tfRibEdit	[$frame.tfRibEdit getframe]

	#---------- M t o r ----------#
	checkbutton $::tfMtor.cbDo -text "Do" -width 14 -font "arial 10 bold" -indicatoron false -variable CRib::mtorDo -selectcolor #0066CC -fg white -bg gray \
		-command {if $CRib::mtorDo {$::tfMtor.cbDo configure -text "Do" -fg white} else {$::tfMtor.cbDo configure -fg black}}
	LabelEntry $::tfMtor.enID -labelwidth 3 -label "ID" -labelfont "arial 10 bold" -font "arial 12 bold" -textvariable CRib::mtorID -width 15 -highlightcolor #00CC66 -highlightthickness 2
	
	label $::tfMtor.laCamName -text "Camera name" -font "arial 10 bold" -width 15 -relief groov
	entry $::tfMtor.enCamName -width 15 -font "arial 10 bold" -textvariable CRib::cameraName -highlightcolor #00CC66 -highlightthickness 2
	
	label $::tfMtor.laFrame -text "Frame" -font "arial 10 bold" -width 15 -relief groov
	entry $::tfMtor.enFrame -width 15 -font "arial 10 bold" -textvariable CRib::frameInf -highlightcolor #00CC66 -highlightthickness 2

	label $::tfMtor.laFormat -text "Format" -font "arial 10 bold" -width 15 -relief groov
	frame $::tfMtor.frFormat
	entry $::tfMtor.frFormat.enX -width 6 -font "arial 10 bold" -textvariable CRib::formatX -highlightcolor #00CC66 -highlightthickness 2
	entry $::tfMtor.frFormat.enY -width 6 -font "arial 10 bold" -textvariable CRib::formatY -highlightcolor #00CC66 -highlightthickness 2

	pack $::tfMtor.frFormat.enY -side right
	pack $::tfMtor.frFormat.enX -side right -padx 2
	
	label $::tfMtor.laPixelSample -text "Pixel sample" -font "arial 10 bold" -width 15 -relief groov
	frame $::tfMtor.frPixelSample
	entry $::tfMtor.frPixelSample.enX -width 6 -font "arial 10 bold" -textvariable CRib::pixelSampleX -highlightcolor #00CC66 -highlightthickness 2
	entry $::tfMtor.frPixelSample.enY -width 6 -font "arial 10 bold" -textvariable CRib::pixelSampleY -highlightcolor #00CC66 -highlightthickness 2

	pack $::tfMtor.frPixelSample.enY -side right
	pack $::tfMtor.frPixelSample.enX -side right -padx 2

	label $::tfMtor.laShadingRate -text "Shading rate" -font "arial 10 bold" -width 15 -relief groov
	entry $::tfMtor.enShadingRate -width 6 -font "arial 10 bold" -textvariable CRib::shadingRate -highlightcolor #00CC66 -highlightthickness 2
	
	label $::tfMtor.laDspyServer -text "dspyServer" -font "arial 10 bold" -width 15 -height 1 -relief groov
	#tk_optionMenu $::tfMtor.omDspyServer CRib::dspyServer targa sgif tiff mayaiff 
	tk_optionMenu $::tfMtor.omDspyServer CRib::dspyServer tiff mayaiff
	$::tfMtor.omDspyServer configure -font "arial 10 bold" -width 11
	
	label $::tfMtor.laDspyMode -text "dspyMode" -font "arial 10 bold" -width 15 -relief groov
	tk_optionMenu $::tfMtor.omDspyMode CRib::dspyMode rgb rgba rgbz rgbaz
	$::tfMtor.omDspyMode configure -font "arial 10 bold" -width 11
	
	
	entry $::tfMtor.enMtorCmd -font "arial 10 bold" -textvariable CRib::mtorCmd -highlightcolor #00CC66 -highlightthickness 2
	
	#------------------------------ RibEdit ---------------------------------#
	frame $::tfRibEdit.frCmdPath
	entry $::tfRibEdit.frCmdPath.enCmd -font "arial 10 bold" -textvariable CRib::cmd -highlightcolor #00CC66 -highlightthickness 2
	button $::tfRibEdit.frCmdPath.buFind -font "arial 10 bold" -text "cmd" -width 5 \
	       -command {set dir [tk_getOpenFile -initialdir $Global::cmdPath -filetypes $CRib::cmdFile]; regexp -- {.*\/} $dir Global::cmdPath; set CRib::cmd [concat $CRib::cmd $dir]}

	frame $::tfRibEdit.frBgPath
	entry $::tfRibEdit.frBgPath.enFileName -font "arial 10 bold" -textvariable CRib::bgName -highlightcolor #00CC66 -highlightthickness 2
	button $::tfRibEdit.frBgPath.buOpen -font "arial 10 bold" -width 5 -text "bg" \
	       -command {set CRib::bgName [tk_getOpenFile -initialdir $Global::bgPath]; regexp -- {.*\/} $CRib::bgName Global::bgPath}

     frame $::tfRibEdit.frRibEdit
     text  $::tfRibEdit.frRibEdit.txRibEdit -yscrollcommand "$::tfRibEdit.frRibEdit.scY set" -xscrollcommand "$::tfRibEdit.frRibEdit.scX set" -wrap none \
	  -relief sunken -bd 1 -setgrid 1 -height 5 -font "arial 10 bold"
     scrollbar $::tfRibEdit.frRibEdit.scY -command "$::tfRibEdit.frRibEdit.txRibEdit yview"
     scrollbar $::tfRibEdit.frRibEdit.scX -command "$::tfRibEdit.frRibEdit.txRibEdit xview" -orient horizontal

     grid columnconfigure $::tfRibEdit.frRibEdit 0 -weight 1
     grid $::tfRibEdit.frRibEdit.txRibEdit -row 0 -column 0 -sticky news
     grid $::tfRibEdit.frRibEdit.scY -row 0 -column 1 -sticky news
     grid $::tfRibEdit.frRibEdit.scX -row 1 -column 0 -columnspan 2 -sticky news
		
	#------------------------------------------------------------------------------------------#
	grid columnconfigure $::tfMtor 0 -weight 1
	grid columnconfigure $::tfMtor 1 -weight 1
	
	grid $::tfMtor.cbDo -row 0 -column 0 -sticky w -pady 3 -padx 5
	grid $::tfMtor.enID -row 0 -column 1 -sticky e -padx 5
	
	grid $::tfMtor.laCamName -row 1 -column 0 -sticky w -ipady 2 -pady 3 -padx 5
	grid $::tfMtor.enCamName -row 1 -column 1 -sticky e -padx 5
	
	grid $::tfMtor.laFrame  -row 2 -column 0 -sticky w -ipady 2 -pady 3 -padx 5
	grid $::tfMtor.enFrame	-row 2 -column 1 -sticky e -padx 5
	
	grid $::tfMtor.laFormat -row 3 -column 0 -sticky w -ipady 2 -pady 3 -padx 5
	grid $::tfMtor.frFormat -row 3 -column 1 -sticky e -padx 5

	grid $::tfMtor.laPixelSample -row 4 -column 0 -sticky w -ipady 2 -pady 3 -padx 5
	grid $::tfMtor.frPixelSample -row 4 -column 1 -sticky e -padx 5

	grid $::tfMtor.laShadingRate -row 5 -column 0 -sticky w -ipady 2 -pady 3 -padx 5
	grid $::tfMtor.enShadingRate -row 5 -column 1 -sticky e -padx 5

	grid $::tfMtor.laDspyServer -row 6 -column 0 -sticky w -ipady 2 -pady 3 -padx 5
	grid $::tfMtor.omDspyServer -row 6 -column 1 -sticky e -padx 5

	grid $::tfMtor.laDspyMode -row 7 -column 0 -sticky w -ipady 2 -pady 3 -padx 5
	grid $::tfMtor.omDspyMode -row 7 -column 1 -sticky e -padx 5


	grid $::tfMtor.enMtorCmd -row 8 -column 0 -columnspan 2 -sticky news -pady 3 -padx 5

	#----------------------- ribedit -----------------------------#
	grid columnconfigure $::tfRibEdit 0 -weight 1
	grid columnconfigure $::tfRibEdit 1 -weight 1
	grid columnconfigure $::tfRibEdit 2 -weight 1
	
	pack $::tfRibEdit.frCmdPath.buFind -side right
	pack $::tfRibEdit.frCmdPath.enCmd -side right -fill x -expand yes
	grid $::tfRibEdit.frCmdPath -row 1 -column 0 -columnspan 3 -padx 3 -sticky news
	
	
	pack $::tfRibEdit.frBgPath.buOpen -side right
	pack $::tfRibEdit.frBgPath.enFileName -side right -fill x -expand yes
	grid $::tfRibEdit.frBgPath -row 2 -column 0 -columnspan 3  -padx 3 -sticky news

	grid $::tfRibEdit.frRibEdit -row 3 -column 0 -columnspan 3 -padx 3 -pady 5

	pack $frame.tfMtor 	 -fill x -padx 5 -pady 3
	pack $frame.tfRibEdit	-fill x -padx 5 -pady 3

	if {![regexp -- {^\s*$} $CRib::ribEditOption]} {
	  CRib::setRibEditOption $CRib::ribEditOption
	}

}
