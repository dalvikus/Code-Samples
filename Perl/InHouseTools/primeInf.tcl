namespace eval CPrimeInf {
	variable scene 			""
  
    variable show 			"" 
    variable seq 			""
    variable shot 			""
    variable dept 			""
    variable layer 			$env(USERNAME)
    variable revision 		""
    variable output	        ""
    variable renderer	        ""
    variable sendMail	 	0
    variable mailTo 		$env(USERNAME)
    variable crew 			""
    variable mailSubject 	""
    variable mailContent 	""
    variable render 		0
    variable delRib		0
    variable mayaFile {{{maya binary file} {.mb}} {{maya ascii file} {.ma}}}
}

proc CPrimeInf::init {} {
}

proc CPrimeInf::create { w } {
	set frame [$w insert end pgPrimeInf -text "PrimeInf"]
	$frame configure -relief groov -bd 1
	
	TitleFrame $frame.tfRequired -text "Required"
	TitleFrame $frame.tfOptional -text "Optional"

	set ::tfRequired 	 [$frame.tfRequired getframe]
	set ::tfOptional	 [$frame.tfOptional getframe]

	#---------------------------------- required -------------------------------------#
	frame $::tfRequired.frScenePath
	entry $::tfRequired.frScenePath.enScene -width 70  -font "arial 10 bold" -textvariable CPrimeInf::scene -highlightcolor #00CC66 -highlightthickness 2
	button $::tfRequired.frScenePath.buFind -text "scene" -font "arial 11 bold" -width 5 \
	       -command {set CPrimeInf::scene [tk_getOpenFile -initialdir $Global::scenePath -filetypes $CPrimeInf::mayaFile]; regexp -- {.*\/} $CPrimeInf::scene Global::scenePath}
	
	label $::tfRequired.laCrew -text "Partition" -font "arial 12 bold" -width 12 -relief groov
	entry $::tfRequired.enCrew -width 14 -font "arial 12 bold" -textvariable CPrimeInf::crew -highlightcolor #00CC66 -highlightthickness 2
	
	label $::tfRequired.laShow -text "Show" -font "arial 12 bold" -width 12 -relief groov
	entry $::tfRequired.enShow -textvariable CPrimeInf::show -font "arial 12 bold" -width 14  -highlightcolor #00CC66 -highlightthickness 2

	#label $::tfRequired.laSeq -text "Seq" -font "arial 12 bold"
	#tk_optionMenu $::tfRequired.omSeq CPrimeInf::seq seqtest
	#$::tfRequired.omSeq configure -font "arial 12 bold" -width 10
	
	label $::tfRequired.laShot -text "Shot" -font "arial 12 bold" -width 12 -relief groov
	entry $::tfRequired.enShot -width 14 -font "arial 12 bold" -textvariable CPrimeInf::shot -highlightcolor #00CC66 -highlightthickness 2

	label $::tfRequired.laRevision -text "Revision" -font "arial 12 bold" -width 12 -relief groov
	entry $::tfRequired.enRevision -width 14 -font "arial 12 bold" -textvariable CPrimeInf::revision -highlightcolor #00CC66 -highlightthickness 2

	
	label $::tfRequired.laDept -text "Dept" -font "arial 12 bold" -width 12 -relief groov
	entry $::tfRequired.enDept -width 14 -font "arial 12 bold" -textvariable CPrimeInf::dept -highlightcolor #00CC66 -highlightthickness 2

	label $::tfRequired.laLayer -text "User" -font "arial 12 bold" -width 12 -relief groov
	entry $::tfRequired.enLayer -width 14 -font "arial 12 bold" -textvariable CPrimeInf::layer -highlightcolor #00CC66 -highlightthickness 2

        label $::tfRequired.laOutput -text "Output" -font "arial 12 bold" -width 12 -relief groov
	tk_optionMenu $::tfRequired.omOutput CPrimeInf::output tv 2K 4K Custom 
	$::tfRequired.omOutput configure -font "arial 12 bold" -width 10

        label $::tfRequired.laRenderer -text "Renderer" -font "arial 12 bold" -width 12 -relief groov
	tk_optionMenu $::tfRequired.omRenderer CPrimeInf::renderer prman render(M)
	$::tfRequired.omRenderer configure -font "arial 12 bold" -width 10
	
	#---------------------------------- optional -------------------------------------#
	label $::tfOptional.laRender -text "Render" -font "arial 12 bold" -width 12 -relief groov
	checkbutton $::tfOptional.cbRender -variable CPrimeInf::render -text "netrender"  -selectcolor #CC6600 -indicatoron false -width 13 -bg gray -fg black -font "arial 12 bold" \
		-command {if $CPrimeInf::render {$::tfOptional.cbRender configure -text "local" -fg white} else {$::tfOptional.cbRender configure -text "netrender" -fg black}}
		
        checkbutton $::tfOptional.cbSendMail -text "Send mail" -width 11 -font "arial 12 bold" -indicatoron false -variable CPrimeInf::sendMail -selectcolor #0066CC -bg gray 
	entry $::tfOptional.enMailTo -width 14 -font "arial 12 bold" -textvariable CPrimeInf::mailTo -highlightcolor #00CC66 -highlightthickness 2

	label $::tfOptional.laCleanUp -text "Cleanup" -font "arial 12 bold" -width 12 -relief groov
	checkbutton $::tfOptional.cbCleanUp -variable CPrimeInf::delRib -text "keep rib"  -selectcolor #0066CC -indicatoron false -width 13 -bg gray -fg black -font "arial 12 bold" \
		-command {if $CPrimeInf::delRib {$::tfOptional.cbCleanUp configure -text "delete rib" -fg white} else {$::tfOptional.cbCleanUp configure -text "keep rib" -fg black}}
	

	#--------------------------- P     A     C     K --------------------------#
	
	#--------------------------------- required ------------------------------------#
	grid columnconfigure $::tfRequired 0 -weight 1
	grid columnconfigure $::tfRequired 1 -weight 1
	grid columnconfigure $::tfRequired 2 -weight 1
	
	pack $::tfRequired.frScenePath.buFind -side right -fill y 
	pack $::tfRequired.frScenePath.enScene -side right -fill x -padx 3
	grid $::tfRequired.frScenePath -row 0 -column 0 -columnspan 3 -pady 2 -padx 10
	
	grid $::tfRequired.laCrew -row 1 -column 0 -sticky w -pady 2 -padx 10 -ipady 3
	grid $::tfRequired.enCrew -row 1 -column 2 -sticky e -pady 2 -padx 10

	grid $::tfRequired.laShow -row 2 -column 0 -sticky w -pady 2 -padx 10 -ipady 3
	grid $::tfRequired.enShow -row 2 -column 2 -sticky e -pady 2 -padx 10

	#grid $::tfRequired.laSeq -row 3 -column 0 -sticky w -pady 3 -padx 10
	#grid $::tfRequired.omSeq -row 3 -column 2 -sticky e -pady 5 -padx 10

	grid $::tfRequired.laShot -row 4 -column 0 -sticky w -pady 2 -padx 10 -ipady 3
	grid $::tfRequired.enShot -row 4 -column 2 -sticky e -pady 2 -padx 10

	grid $::tfRequired.laRevision -row 5 -column 0 -columnspan 2 -sticky w -pady 2 -padx 10 -ipady 3
	grid $::tfRequired.enRevision -row 5 -column 2 -sticky e -pady 2 -padx 10

	grid $::tfRequired.laDept -row 6 -column 0 -columnspan 2 -sticky w -pady 2 -padx 10 -ipady 3
	grid $::tfRequired.enDept -row 6 -column 2 -sticky e -pady 2 -padx 10
	
	grid $::tfRequired.laLayer -row 7 -column 0 -columnspan 2 -sticky w -pady 2 -padx 10 -ipady 3
	grid $::tfRequired.enLayer -row 7 -column 2 -sticky e -pady 2 -padx 10
	
	grid $::tfRequired.laOutput -row 8 -column 0 -columnspan 2 -sticky w -pady 2 -padx 10 -ipady 3
	grid $::tfRequired.omOutput -row 8 -column 2 -sticky e -pady 2 -padx 10
	
	grid $::tfRequired.laRenderer -row 9 -column 0 -columnspan 2 -sticky w -pady 2 -padx 10 -ipady 3
	grid $::tfRequired.omRenderer -row 9 -column 2 -sticky e -pady 2 -padx 10

	#--------------------------------- optional --------------------------------------#
	grid columnconfigure $::tfOptional 0 -weight 1
	grid columnconfigure $::tfOptional 1 -weight 1
	grid columnconfigure $::tfOptional 2 -weight 1
	
	grid $::tfOptional.laRender -row 0 -column 0 -columnspan 2 -sticky w -pady 2 -padx 10 -ipady 3
	grid $::tfOptional.cbRender -row 0 -column 2 -sticky e -pady 2 -padx 10
	
	grid $::tfOptional.cbSendMail -row 1 -column 0 -columnspan 2 -sticky w -pady 2 -padx 10 -ipady 3
	grid $::tfOptional.enMailTo -row 1 -column 2 -sticky e -pady 2 -padx 10

	grid $::tfOptional.laCleanUp -row 3 -column 0 -columnspan 2 -sticky w -pady 2 -padx 10 -ipady 3
	grid $::tfOptional.cbCleanUp -row 3 -column 2 -sticky e -pady 2 -padx 10
	
	pack $frame.tfRequired -expand yes -fill x -pady 5 -padx 5
	pack $frame.tfOptional -expand yes -fill x -pady 5 -padx 5
}
