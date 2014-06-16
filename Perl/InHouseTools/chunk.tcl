namespace eval CChunk {
	variable renHeroNum 	""
	variable renGrndShdNum 	""	
	variable ezCompNum 		""
	variable selectedChunk		""
	variable id	      ""
	variable frame	""
	
	namespace eval CRenHero {
	}
	
	namespace eval CRenGrndShd {
	}
	
	namespace eval CEzComp {
	}
	
	set renHeroChunk()	   ""
	set renGrndShdChunk()	   ""
	set ezCompChunk()	   ""
}

proc CChunk::init {} {
	set CChunk::renHeroNum	      0
	set CChunk::renGrndShdNum     0	
	set CChunk::ezCompNum	      0
}

proc CChunk::create { w } {
	CChunk::init
	
	set CChunk::frame [$w insert end pgChunk -text "Chunk"]
	$CChunk::frame configure -relief groov -bd 1
	
	frame 	 $CChunk::frame.frTop 
	ComboBox $CChunk::frame.frTop.cmbChunk -insertontime 0 -selectbackground white -selectforeground black  -selectborderwidth 0 -editable 0 -font "arial 12 bold" -textvariable CChunk::selectedChunk \
		-width 15 -values {" --- select ---" "RenHero" "RenGrndShd"}
	entry    $CChunk::frame.frTop.enId   -textvariable CChunk::id -width 15 -font "arial 12 bold" -highlightcolor #00CC66 -highlightthickness 2
	button   $CChunk::frame.frTop.buAddChunk -text "Add" -bd 1 -font "arial 11 bold" -command {CChunk::addChunk}
	bind $CChunk::frame.frTop.enId <Return> {CChunk::addChunk} 

	pack $CChunk::frame.frTop.cmbChunk 	 -side left -padx 10 -pady 5
	pack $CChunk::frame.frTop.enId	-side left -padx 10 -pady 5
	pack $CChunk::frame.frTop.buAddChunk -side left -padx 10 -pady 5

	frame $CChunk::frame.frChunkList -relief sunken -bd 1
	ScrollableFrame $CChunk::frame.frChunkList.frChunks -yscrollcommand "$CChunk::frame.frChunkList.scChunkListY set"
	set ::frame_chunk [$CChunk::frame.frChunkList.frChunks getframe] ;# for access frame with top-level namespace
	
	scrollbar $CChunk::frame.frChunkList.scChunkListY -command "$CChunk::frame.frChunkList.frChunks yview"
	
	pack $CChunk::frame.frChunkList.frChunks -side left -expand yes -fill both
	pack $CChunk::frame.frChunkList.scChunkListY -side left -fill y
	
	pack $CChunk::frame.frTop
	pack $CChunk::frame.frChunkList -expand yes -fill both
}

proc CChunk::addChunk {} {
     if [string compare $CChunk::selectedChunk " --- select ---"] {
	  if {![string compare $CChunk::selectedChunk "RenHero"]} {
	       if {[regexp -- {^\s*$} $CChunk::id]} {
		    Msg "\nplease enter your chunk ID !\n"
	       } else {
		    CChunk::addRenHero
	       }
     
	  } elseif {![string compare $CChunk::selectedChunk "RenGrndShd"]} {
	       if {[regexp -- {^\s*$} $CChunk::id]} {
		    Msg "\nplease enter your chunk ID !\n"
	       } else {
		    CChunk::addRenGrndShd
	       }
     
	  } elseif {![string compare $CChunk::selectedChunk "EzComp"]} {
	       CChunk::addEzComp
     
	  }
     }
}

proc CChunk::addRenHero {} {
	set CChunk::renHeroNum [expr $CChunk::renHeroNum+1]
	set CChunk::renHeroChunk($CChunk::renHeroNum,1) 0 			;# Do
	set CChunk::renHeroChunk($CChunk::renHeroNum,2) $CChunk::id		;# Id
	set CChunk::renHeroChunk($CChunk::renHeroNum,3) ""			;# rib edit option
	
	frame $::frame_chunk.frRenHero$CChunk::renHeroNum -relief groov -bd 2
	checkbutton $::frame_chunk.frRenHero$CChunk::renHeroNum.cbDo -text "Ren - $CChunk::renHeroChunk($CChunk::renHeroNum,2)" -font "arial 12 bold"  -selectcolor #CC0066 -variable CChunk::renHeroChunk($CChunk::renHeroNum,1) -indicatoron false -bg black -fg white 
	entry $::frame_chunk.frRenHero$CChunk::renHeroNum.enRibEdit -width 63 -font "arial 10 bold" -textvariable CChunk::renHeroChunk($CChunk::renHeroNum,3) -highlightcolor #00CC66 -highlightthickness 2
	
	grid rowconfigure $::frame_chunk.frRenHero$CChunk::renHeroNum 0 -weight 1
	grid rowconfigure $::frame_chunk.frRenHero$CChunk::renHeroNum 1 -weight 0

	grid $::frame_chunk.frRenHero$CChunk::renHeroNum.cbDo -row 0 -column 0 -pady 5 -padx 5 
	grid $::frame_chunk.frRenHero$CChunk::renHeroNum.enRibEdit -row 1 -column 0 -sticky news -pady 5 -padx 5

	pack $::frame_chunk.frRenHero$CChunk::renHeroNum -fill x -padx 5 -pady 5 -expand yes -side top
}

proc CChunk::addRenGrndShd {} {
	set CChunk::renGrndShdNum [expr $CChunk::renGrndShdNum+1]
	set CChunk::renGrndShdChunk($CChunk::renGrndShdNum,1) 		0			    ;# Do
	set CChunk::renGrndShdChunk($CChunk::renGrndShdNum,2) 		""			    ;# light
	set CChunk::renGrndShdChunk($CChunk::renGrndShdNum,3) 		""			    ;# source
	set CChunk::renGrndShdChunk($CChunk::renGrndShdNum,4) 		""			    ;# target
	set CChunk::renGrndShdChunk($CChunk::renGrndShdNum,5) 		$CChunk::id		    ;# Id
	
	frame $::frame_chunk.frRenGrndShd$CChunk::renGrndShdNum -relief groov -bd 2
	
	checkbutton $::frame_chunk.frRenGrndShd$CChunk::renGrndShdNum.cbDo -text "RenGrndShd - $CChunk::renGrndShdChunk($CChunk::renGrndShdNum,5)" -font "arial 12 bold" -indicatoron false  -selectcolor #CC0066 -variable CChunk::renGrndShdChunk($CChunk::renGrndShdNum,1) -bg black -fg white

	LabelEntry $::frame_chunk.frRenGrndShd$CChunk::renGrndShdNum.leLight -label "Light" -width 13 -labelanchor center -labelwidth 6 -labelfont "arial 10 bold" -font "arial 10 bold" -textvariable CChunk::renGrndShdChunk($CChunk::renGrndShdNum,2) -highlightcolor #00CC66 -highlightthickness 2
	LabelEntry $::frame_chunk.frRenGrndShd$CChunk::renGrndShdNum.leSrc -label "Hero" -width 13 -labelanchor center -labelwidth 7 -labelfont "arial 10 bold" -font "arial 10 bold" -textvariable CChunk::renGrndShdChunk($CChunk::renGrndShdNum,3) -highlightcolor #00CC66 -highlightthickness 2
	LabelEntry $::frame_chunk.frRenGrndShd$CChunk::renGrndShdNum.leTarget -label "Grnd" -width 13 -labelanchor center -labelwidth 7 -labelfont "arial 10 bold" -font "arial 10 bold" -textvariable CChunk::renGrndShdChunk($CChunk::renGrndShdNum,4) -highlightcolor #00CC66 -highlightthickness 2

	grid columnconfigure $::frame_chunk.frRenGrndShd$CChunk::renGrndShdNum 0 -weight 1
	grid columnconfigure $::frame_chunk.frRenGrndShd$CChunk::renGrndShdNum 1 -weight 1
	grid columnconfigure $::frame_chunk.frRenGrndShd$CChunk::renGrndShdNum 2 -weight 1
	grid $::frame_chunk.frRenGrndShd$CChunk::renGrndShdNum.cbDo -row 0 -column 0 -columnspan 3 -pady 5 -padx 5
	grid $::frame_chunk.frRenGrndShd$CChunk::renGrndShdNum.leLight  -row 1 -column 0 -pady 5 -padx 3 -sticky news
	grid $::frame_chunk.frRenGrndShd$CChunk::renGrndShdNum.leSrc    -row 1 -column 1 -pady 5 -sticky news
	grid $::frame_chunk.frRenGrndShd$CChunk::renGrndShdNum.leTarget -row 1 -column 2 -pady 5 -padx 3 -sticky news
	
	pack $::frame_chunk.frRenGrndShd$CChunk::renGrndShdNum -padx 5 -pady 5 -expand yes -fill x -side top

}

proc CChunk::addEzComp {} {
}











