package require smtp

namespace eval CAlfred {
     set outDir		      ""		 
     set ribDir		      ""	
     set shdDir		      ""	
     set jobDir		      ""
     set renDir		      ""     
     set sceneName	      ""
     set sceneFolder	      ""
     set sceneDir	      ""
     set jobScrName	      ""
     set jobTitle	      ""
     set mtorOption	      ""
     set mayaOption	      ""
     set ribEditOption	      "" ;# string that delete 'newline tab' in CRib::ribEditOption
     set bgName		      "" ;# sub #### => [format %04 $count]
     set chuckRibEditOption   ""
     set baseRibName	      ""
     set editRib	      0  ;# 0: render xxx_o.rib     1: render xxx.rib
     set copyScene	      1  ;# 0: do not copy scenefile because it already exists 1: copy
     set frameInf	      0  ;# 0: sequence frame  1: random frame

     set dpsFrameRange	      0
     set dpsDailyNum	      0
     set dpsFileName	      ""
     set dpsImgName	      ""
     set dpsMediaUtility      ""
     set dpsSlateName	      ""
     set viewer		      ""

     proc init {} {

	  set CAlfred::outDir		   ""		 
	  set CAlfred::ribDir		   ""	
	  set CAlfred::shdDir		   ""	
	  set CAlfred::jobDir		   ""
	  set CAlfred::renDir		   ""     
	  set CAlfred::sceneName	   ""
	  set CAlfred::sceneFolder	   ""
	  set CAlfred::sceneDir		   ""
	  set CAlfred::jobScrName	   ""
	  set CAlfred::jobTitle		   ""
	  set CAlfred::mtorOption	   ""
	  set CAlfred::mayaOption	   ""
	  set CAlfred::ribEditOption	   ""
	  set CAlfred::bgName		   ""
	  set CAlfred::chuckRibEditOption  ""
	  set CAlfred::baseRibName	   ""
	  set CAlfred::editRib		   0 
	  set CAlfred::copyScene	   1  

	  set CAlfred::dpsFrameRange	   0
	  set CAlfred::dpsFileName	   ""
	  set CAlfred::dpsImgName	   ""
	  set CAlfred::dpsMediaUtility	   ""
	  set CAlfred::dpsSlateName	   ""
	  set CAlfred::dpsDailyNum	   0

	  set CAlfred::outDir	     //Chunji/show/$CPrimeInf::show/$CPrimeInf::seq/$CPrimeInf::shot/image/$CPrimeInf::dept/$CPrimeInf::layer/$CPrimeInf::revision
	  set CAlfred::ribDir	     $CAlfred::outDir/ribs
	  set CAlfred::shdDir	     $CAlfred::outDir/shadowmaps
	  set CAlfred::jobDir	     $CAlfred::outDir/jobs
	  set CAlfred::renDir	     $CAlfred::outDir/renders

	  ################################################################################
	  # prman
	  ################################################################################
	  if {![string compare $CPrimeInf::renderer prman]} { 
	       set CAlfred::sceneName     "$CPrimeInf::show\_$CPrimeInf::shot\_$CPrimeInf::dept\_$CPrimeInf::layer\_$CRib::mtorID\_$CPrimeInf::revision.mb"
	       set CAlfred::baseRibName   "$CPrimeInf::shot\_$CPrimeInf::layer\_$CRib::mtorID\_$CPrimeInf::revision"
	  }

	  ################################################################################
	  # render (M)
	  ################################################################################
	  if {![string compare $CPrimeInf::renderer "render(M)"]} { 
	       set CAlfred::sceneName     "$CPrimeInf::show\_$CPrimeInf::shot\_$CPrimeInf::dept\_$CPrimeInf::layer\_$CMaya::ID\_$CPrimeInf::revision.mb"
	       set CAlfred::baseRibName   "$CPrimeInf::shot\_$CPrimeInf::layer\_$CMaya::ID\_$CPrimeInf::revision"
	  }

	  set CAlfred::sceneFolder   "$CPrimeInf::show\_$CPrimeInf::shot\_$CPrimeInf::dept\_$CPrimeInf::layer\_maya"
	  set CAlfred::sceneDir	     "//Chunji/show/$CPrimeInf::show/$CPrimeInf::seq/$CPrimeInf::shot/scene/$CPrimeInf::dept/$CAlfred::sceneFolder"
	  set CAlfred::jobScrName    "$CPrimeInf::shot\_$CPrimeInf::layer\_$CPrimeInf::revision\-$Global::patchLevel\.job"
	  set CAlfred::jobTitle	     "$CPrimeInf::shot\_$CPrimeInf::revision\_$CPrimeInf::show\_$CPrimeInf::dept\_$CPrimeInf::layer"

	  ################################################################################  
	  #
	  #    case prman
	  #
	  ################################################################################  
	  if {![string compare $CPrimeInf::renderer prman]} {

	       # set frame range
	       if [regexp -- {^\s*(\d+)\s*-\s*(\d+)\s*$} $CRib::frameInf xxx CRib::startFrame CRib::endFrame] {
		    set CRib::frameInterval 1
		    set CAlfred::frameInf 0
		    
	       } elseif {[regexp -- {^\s*(\d+)\s*-\s*(\d+)\s*\/\s*(\d+)\s*$} $CRib::frameInf xxx CRib::startFrame CRib::endFrame CRib::frameInterval]} {
		    set CAlfred::frameInf 0
     
	       } else {
		    set CAlfred::frameInf 1
	       }

	       # case local render
	       if $CPrimeInf::render {
		    regsub -all -line {^\s*} $CRib::ribEditOption "" CAlfred::ribEditOption 
		    regsub -all -line {\s*\n} $CAlfred::ribEditOption  " " CAlfred::ribEditOption
		    #regsub -all -line {"} $CAlfred::ribEditOption \\\\\\\\\" CAlfred::ribEditOption
		    regsub -all -line {\s\*} $CAlfred::ribEditOption " \\\\\\*" CAlfred::ribEditOption
     
	       # case net render
	       } else {
		    regsub -all -line {^\s*} $CRib::ribEditOption "" CAlfred::ribEditOption 
		    regsub -all -line {\s*\n} $CAlfred::ribEditOption  " " CAlfred::ribEditOption
		    regsub -all -line {"} $CAlfred::ribEditOption \\\\\\\\\" CAlfred::ribEditOption
		    regsub -all -line {\s\*} $CAlfred::ribEditOption " \\\\\\\\\\\\\\\\\*" CAlfred::ribEditOption
	       }

	       if {![string compare $CRib::dspyServer "mayaiff"]} {
		    set CAlfred::viewer fcheck
	       } else {
		    set CAlfred::viewer sho
	       }

	       set CAlfred::mtorOption [concat $CAlfred::mtorOption "-scene $CAlfred::sceneDir/scenes/$CAlfred::sceneName"]
	       set CAlfred::mtorOption [concat $CAlfred::mtorOption "-cmd rg dspyName $CAlfred::baseRibName"]
	       set CAlfred::mtorOption [concat $CAlfred::mtorOption "-cmd rg camName $CRib::cameraName"]
	  
	       set CAlfred::mtorOption [concat $CAlfred::mtorOption "-cmd rg dspyServerMode $CRib::dspyMode"]
	       set CAlfred::mtorOption [concat $CAlfred::mtorOption "-cmd rg dspyServer $CRib::dspyServer"]
	       set CAlfred::mtorOption [concat $CAlfred::mtorOption "-cmd rg dspyFormatX $CRib::formatX"]
	       set CAlfred::mtorOption [concat $CAlfred::mtorOption "-cmd rg dspyFormatY $CRib::formatY"]
	       set CAlfred::mtorOption [concat $CAlfred::mtorOption "-cmd rg pixelSamplesX $CRib::pixelSampleX"]
	       set CAlfred::mtorOption [concat $CAlfred::mtorOption "-cmd rg pixelSamplesY $CRib::pixelSampleY"]
	       set CAlfred::mtorOption [concat $CAlfred::mtorOption "-cmd rg shadingRate $CRib::shadingRate"]
	       set CAlfred::mtorOption [concat $CAlfred::mtorOption "-cmd workspace SetDir torRIBs $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output"]
	       set CAlfred::mtorOption [concat $CAlfred::mtorOption "-cmd workspace SetDir torImgs $CAlfred::renDir/$CRib::mtorID/$CPrimeInf::output"]
	       set CAlfred::mtorOption [concat $CAlfred::mtorOption "-cmd workspace SetDir torShadows $CAlfred::shdDir/$CRib::mtorID/$CPrimeInf::output"]
	       set CAlfred::mtorOption [concat $CAlfred::mtorOption $CRib::mtorCmd]

	  ################################################################################  
	  #
	  #    case render(M)
	  #
	  ################################################################################  
	  } else {

	       if [regexp -- {^\s*(\d+)\s*-\s*(\d+)\s*$} $CMaya::frameRange xxx CMaya::startFrame CMaya::endFrame] {
		    set CMaya::byFrame 1
		    set CAlfred::frameInf 0
     
	       } elseif {[regexp -- {^\s*(\d+)\s*-\s*(\d+)\s*\/\s*(\d+)\s*$} $CMaya::frameRange xxx CMaya::startFrame CMaya::endFrame CMaya::byFrame]} {
		    set CAlfred::frameInf 0
     
	       } else {
		    set CAlfred::frameInf 1
	       }

	       set CAlfred::viewer fcheck

	       # setting mayaOption excep 'frame range option' 'image'
	       if $CMaya::camNameDo { set CAlfred::mayaOption [concat $CAlfred::mayaOption "-cam $CMaya::camName"] }
	       if $CMaya::imgHDo { set CAlfred::mayaOption [concat $CAlfred::mayaOption "-ih $CMaya::imgH"] }
	       if $CMaya::imgWDo { set CAlfred::mayaOption [concat $CAlfred::mayaOption "-iw $CMaya::imgW"] }
	       if $CMaya::imgARDo { set CAlfred::mayaOption [concat $CAlfred::mayaOption "-ar $CMaya::imgAR"] }
	       if $CMaya::fimgWDo { set CAlfred::mayaOption [concat $CAlfred::mayaOption "-x $CMaya::fimgW"] }
	       if $CMaya::fimgHDo { set CAlfred::mayaOption [concat $CAlfred::mayaOption "-y $CMaya::fimgH"] }
	       if $CMaya::xLeftPixelBoundDo { set CAlfred::mayaOption [concat $CAlfred::mayaOption "-xl $CMaya::xLeftPixelBound"] }
	       if $CMaya::xRightPixelBoundDo { set CAlfred::mayaOption [concat $CAlfred::mayaOption "-xr $CMaya::xRightPixelBound"]}
	       if $CMaya::yLowPixelBoundDo { set CAlfred::mayaOption [concat $CAlfred::mayaOption "-yl $CMaya::yLowPixelBound"]}
	       if $CMaya::yHightPixelBoundDo { set CAlfred::mayaOption [concat $CAlfred::mayaOption "-yh $CMaya::yHightPixelBound"]}
	       if $CMaya::useMBDo { set CAlfred::mayaOption [concat $CAlfred::mayaOption "-mb $CMaya::useMB"]}
	       if $CMaya::shutterAngleDo { set CAlfred::mayaOption [concat $CAlfred::mayaOption "-sa $CMaya::shutterAngle"]}
	       if $CMaya::MBByFrameDo { set CAlfred::mayaOption [concat $CAlfred::mayaOption "-mbf $CMaya::MBByFrame"]}
	       if $CMaya::use2DMBDo { set CAlfred::mayaOption [concat $CAlfred::mayaOption "-mb2d $CMaya::use2DMB"]}
	       if $CMaya::2DMBLengthDo { set CAlfred::mayaOption [concat $CAlfred::mayaOption "-bll $CMaya::2DMBLength"]}
	       if $CMaya::2DMBSharpnessDo { set CAlfred::mayaOption [concat $CAlfred::mayaOption "-bls $CMaya::2DMBSharpness"]}
	       if $CMaya::2DMBSmoothValDo { set CAlfred::mayaOption [concat $CAlfred::mayaOption "-smv $CMaya::2DMBSmoothVal"]}
	       if $CMaya::2DMBSmoothColDo { set CAlfred::mayaOption [concat $CAlfred::mayaOption "-smc $CMaya::2DMBSmoothCol"]}
	       if $CMaya::keep2DMVDo { set CAlfred::mayaOption [concat $CAlfred::mayaOption "-kmv $CMaya::keep2DMV"]}
	       if $CMaya::MBVisibilitySamDo { set CAlfred::mayaOption [concat $CAlfred::mayaOption "-mvs $CMaya::MBVisibilitySam"]}
	       if $CMaya::maxMBVisibilitySamDo { set CAlfred::mayaOption [concat $CAlfred::mayaOption "-mvm $CMaya::maxMBVisibilitySam"]}
	       if $CMaya::enableRTDo { set CAlfred::mayaOption [concat $CAlfred::mayaOption "-ert $CMaya::enableRT"]}
	       if $CMaya::maxRTReflectionLevDo { set CAlfred::mayaOption [concat $CAlfred::mayaOption "-rfl $CMaya::maxRTReflectionLev"]}
	       if $CMaya::maxRTRefractionLevDo { set CAlfred::mayaOption [concat $CAlfred::mayaOption "-rfr $CMaya::maxRTRefractionLev"]}
	       if $CMaya::maxRTShadowRayDepDo { set CAlfred::mayaOption [concat $CAlfred::mayaOption "-sl $CMaya::maxRTShadowRayDep"]}
	       if $CMaya::MPFilteringDo { set CAlfred::mayaOption [concat $CAlfred::mayaOption "-ufil $CMaya::MPFiltering"]}
	       if $CMaya::MPFilterDo { set CAlfred::mayaOption [concat $CAlfred::mayaOption "-pft $CMaya::MPFilter"]}
	       if $CMaya::SSamplesDo { set CAlfred::mayaOption [concat $CAlfred::mayaOption "-ss $CMaya::SSamples"]}
	       if $CMaya::maxSSamplesDo { set CAlfred::mayaOption [concat $CAlfred::mayaOption "-mss $CMaya::maxSSamples"]}
	       if $CMaya::volumeSSDo { set CAlfred::mayaOption [concat $CAlfred::mayaOption "-vs $CMaya::volumeSS"]}
	       if $CMaya::particleVSDo { set CAlfred::mayaOption [concat $CAlfred::mayaOption "-pss $CMaya::particleVS"]}
	       if $CMaya::antiAliasingDo { set CAlfred::mayaOption [concat $CAlfred::mayaOption "-eaa $CMaya::antiAliasing"]}
	       if $CMaya::TFCacheDo { set CAlfred::mayaOption [concat $CAlfred::mayaOption "-uf $CMaya::TFCache"]}
	       if $CMaya::detectSTDo { set CAlfred::mayaOption [concat $CAlfred::mayaOption "-oi $CMaya::detectST"]}
	       if $CMaya::reuseRGDDo { set CAlfred::mayaOption [concat $CAlfred::mayaOption "-rut $CMaya::reuseRGD"]}
	       if $CMaya::useDBBDo { set CAlfred::mayaOption [concat $CAlfred::mayaOption "-udb $CMaya::useDBB"]}
	       if $CMaya::abortMTDo { set CAlfred::mayaOption [concat $CAlfred::mayaOption "-amt $CMaya::abortMT"] }
	       if $CMaya::verboseRenderDo { set CAlfred::mayaOption [concat $CAlfred::mayaOption "-verbose $CMaya::verboseRender"]}
	       if $CMaya::gammaDo { set CAlfred::mayaOption [concat $CAlfred::mayaOption "-g $CMaya::gamma"]}
	       if $CMaya::createIPRDo { set CAlfred::mayaOption [concat $CAlfred::mayaOption "-ipr $CMaya::createIPR"] }
	       if $CMaya::enableDMDo { set CAlfred::mayaOption [concat $CAlfred::mayaOption "-edm $CMaya::enableDM"]}
	       if $CMaya::onlyDMDo { set CAlfred::mayaOption [concat $CAlfred::mayaOption "-sp $CMaya::onlyDM"]}
	       if $CMaya::rContrastDo { set CAlfred::mayaOption [concat $CAlfred::mayaOption "-rct $CMaya::rContrast"]}
	       if $CMaya::gContrastDo { set CAlfred::mayaOption [concat $CAlfred::mayaOption "-gct $CMaya::gContrast"]}
	       if $CMaya::bContrastDo { set CAlfred::mayaOption [concat $CAlfred::mayaOption "-bct $CMaya::bContrast"]}
	       if $CMaya::pContrastDo { set CAlfred::mayaOption [concat $CAlfred::mayaOption "-cct $CMaya::pContrast"]}

	       set CAlfred::mayaOption [concat $CAlfred::mayaOption "-of $CMaya::imgFormat"]
	  }

	  if $Global::jobMode {
	       regexp -- {[0-9]+} $CPrimeInf::revision CAlfred::dpsDailyNum
	       set CAlfred::dpsDailyNum [format %03d $CAlfred::dpsDailyNum]

	       if {![string compare $CPrimeInf::renderer prman]} {
		    # case prman && random frame
		    if {$CRib::frameInterval > 0 && $CAlfred::frameInf} {
			 set CAlfred::dpsFrameRange [expr [llength $CRib::frameInf] + 1]

		    # case prman && seq frame
		    } elseif {$CRib::frameInterval > 0 && !$CAlfred::frameInf} {
			 set CAlfred::dpsFrameRange [expr ($CRib::endFrame - $CRib::startFrame)/$CRib::frameInterval + 2]
		    }
	       }

	       if {![string compare $CPrimeInf::renderer "render(M)"]} {

		    # case render && random frame
		    if {$CMaya::byFrame > 0 && $CAlfred::frameInf} {
			 set CAlfred::dpsFrameRange [expr [llength $CMaya::frameRange] + 1]
			 
		    # case render && seq frame
		    } elseif {$CMaya::byFrame > 0 && !$CAlfred::frameInf} {
			 set CAlfred::dpsFrameRange [expr ($CMaya::endFrame - $CMaya::startFrame)/$CMaya::byFrame + 2]
		    }
	       }

	       set CAlfred::dpsFileName "//Dps1/daily/DPS/daily/$CPrimeInf::show/$CDps::date-$CPrimeInf::dept-$CPrimeInf::layer-$CPrimeInf::show.$CPrimeInf::shot.$CAlfred::dpsDailyNum.dps"
	       set CAlfred::dpsImgName "//Dps1/daily/TIF/daily/$CPrimeInf::show/$CDps::date-$CPrimeInf::dept-$CPrimeInf::layer-$CPrimeInf::show.$CPrimeInf::shot.$CAlfred::dpsDailyNum.dps/$CDps::date-$CPrimeInf::dept-$CPrimeInf::layer-$CPrimeInf::show.$CPrimeInf::shot.$CAlfred::dpsDailyNum"
	       set CAlfred::dpsMediaUtility "//Chunji/share/DPS/DPSMediaUtility"
	       set CAlfred::dpsSlateName "//Dps1/daily/TIF/daily/$CPrimeInf::show/$CDps::date-$CPrimeInf::dept-$CPrimeInf::layer-$CPrimeInf::show.$CPrimeInf::shot.$CAlfred::dpsDailyNum.dps/$CDps::date-$CPrimeInf::dept-$CPrimeInf::layer-$CPrimeInf::show.$CPrimeInf::shot.$CAlfred::dpsDailyNum[format %04d 0].TIF"
	  }
     }

     proc makeDir {} {
	  # make basic directory 
	  if {![file exists $CAlfred::outDir]} {
	       file mkdir $CAlfred::outDir
	  }
	  if {![file exists $CAlfred::ribDir]} {
	       file mkdir $CAlfred::ribDir
	  }
	  if {![file exists $CAlfred::shdDir]} {
	       file mkdir $CAlfred::shdDir
	  }
	  if {![file exists $CAlfred::jobDir]} {
	       file mkdir $CAlfred::jobDir
	  }
	  if {![file exists $CAlfred::renDir]} {
	       file mkdir $CAlfred::renDir
	  }
	  if {![file exists $CAlfred::sceneDir]} {
	       file mkdir $CAlfred::sceneDir
	  }
	  if {![file exists $CAlfred::sceneDir/scenes]} {
	       file mkdir $CAlfred::sceneDir/scenes
	  }
     
	  ################################################################################  
	  #
	  #    case prman
	  #
	  ################################################################################  
	  if {![string compare $CPrimeInf::renderer prman]} {
	       if {![file exists $CAlfred::ribDir/$CRib::mtorID]} {
		    file mkdir $CAlfred::ribDir/$CRib::mtorID
	       }
	       if {![file exists $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output]} {
		    file mkdir $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output
	       }
	       if {![file exists $CAlfred::shdDir/$CRib::mtorID]} {
		    file mkdir $CAlfred::shdDir/$CRib::mtorID
	       }
	       if {![file exists $CAlfred::shdDir/$CRib::mtorID/$CPrimeInf::output]} {
		    file mkdir $CAlfred::shdDir/$CRib::mtorID/$CPrimeInf::output
	       }
	       if {![file exists $CAlfred::renDir/$CRib::mtorID]} {
		    file mkdir $CAlfred::renDir/$CRib::mtorID
	       }
	       if {![file exists $CAlfred::renDir/$CRib::mtorID/$CPrimeInf::output]} {
		    file mkdir $CAlfred::renDir/$CRib::mtorID/$CPrimeInf::output
	       }

	       # make chunk directory
	       # case renHero
	       for {set i 1} {$i <= $CChunk::renHeroNum} {incr i} {
		    if $CChunk::renHeroChunk($i,1) {
			 if {![file exists $CAlfred::renDir/$CRib::mtorID/$CPrimeInf::output/$CChunk::renHeroChunk($i,2)]} {
			      file mkdir $CAlfred::renDir/$CRib::mtorID/$CPrimeInf::output/$CChunk::renHeroChunk($i,2)
			 }
			 if {![file exists $CAlfred::shdDir/$CRib::mtorID/$CPrimeInf::output/$CChunk::renHeroChunk($i,2)]} {
			      file mkdir $CAlfred::shdDir/$CRib::mtorID/$CPrimeInf::output/$CChunk::renHeroChunk($i,2)
			 }
			 if {![file exists $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output/$CChunk::renHeroChunk($i,2)]} {
			      file mkdir $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output/$CChunk::renHeroChunk($i,2)
			 }
		    }
	       }
	   
	       for {set i 1} {$i <= $CChunk::renGrndShdNum} {incr i} {
		    if $CChunk::renGrndShdChunk($i,1) {
			 if {![file exists $CAlfred::renDir/$CRib::mtorID/$CPrimeInf::output/$CChunk::renGrndShdChunk($i,5)]} {
			      file mkdir $CAlfred::renDir/$CRib::mtorID/$CPrimeInf::output/$CChunk::renGrndShdChunk($i,5)
			 }
			 if {![file exists $CAlfred::shdDir/$CRib::mtorID/$CPrimeInf::output/$CChunk::renGrndShdChunk($i,5)]} {
			      file mkdir $CAlfred::shdDir/$CRib::mtorID/$CPrimeInf::output/$CChunk::renGrndShdChunk($i,5)
			 }
			 if {![file exists $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output/$CChunk::renGrndShdChunk($i,5)]} {
			      file mkdir $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output/$CChunk::renGrndShdChunk($i,5)
			 }
		    }
	       }

	  ################################################################################  
	  #
	  #    case render(M)
	  #
	  ################################################################################  
	  } else {
	       if {![file exists $CAlfred::ribDir/$CMaya::ID]} {
		    file mkdir $CAlfred::ribDir/$CMaya::ID
	       }
	       if {![file exists $CAlfred::ribDir/$CMaya::ID/$CPrimeInf::output]} {
		    file mkdir $CAlfred::ribDir/$CMaya::ID/$CPrimeInf::output
	       }
	       if {![file exists $CAlfred::shdDir/$CMaya::ID]} {
		    file mkdir $CAlfred::shdDir/$CMaya::ID
	       }
	       if {![file exists $CAlfred::shdDir/$CMaya::ID/$CPrimeInf::output]} {
		    file mkdir $CAlfred::shdDir/$CMaya::ID/$CPrimeInf::output
	       }
	       if {![file exists $CAlfred::renDir/$CMaya::ID]} {
		    file mkdir $CAlfred::renDir/$CMaya::ID
	       }
	       if {![file exists $CAlfred::renDir/$CMaya::ID/$CPrimeInf::output]} {
		    file mkdir $CAlfred::renDir/$CMaya::ID/$CPrimeInf::output
	       }
	  }
     }

     proc CheckSynError {} {
	  #1. check chunk entry
	  for {set i 1} {$i <= $CChunk::renHeroNum} {incr i} {
	       if $CChunk::renHeroChunk($i,1) {
		    if {[regexp -- {^\s*$} $CChunk::renHeroChunk($i,3)]} {
			 Msg "\nribEdit option entry is empty !\n\ncheck $CChunk::renHeroChunk($i,2) chunk...\n"
			 return 1
		    }
	       }
	  }
	   
	  for {set i 1} {$i <= $CChunk::renGrndShdNum} {incr i} {
	       if $CChunk::renGrndShdChunk($i,1) {
		    if {[regexp -- {^\s*$} $CChunk::renGrndShdChunk($i,2)]} {
			 Msg "\nrenGrndShd Light entry is empty !\n\ncheck $CChunk::renGrndShdChunk($i,5) chunk...\n"
			 return 1
		    }
		    if {[regexp -- {^\s*$} $CChunk::renGrndShdChunk($i,3)]} {
			 Msg "\nrenGrndShd Hero entry is empty !\n\ncheck $CChunk::renGrndShdChunk($i,5) chunk...\n"
			 return 1
		    }
		    if {[regexp -- {^\s*$} $CChunk::renGrndShdChunk($i,4)]} {
			 Msg "\nrenGrndShd Grnd entry is empty !\n\ncheck $CChunk::renGrndShdChunk($i,5) chunk...\n"
			 return 1
		    }
	       }
	  }
	  
	  if {[regexp -- {^\s*$} $CPrimeInf::scene]} {
	       Msg "\ncheck your Scene entry, it is empty !\n"
	       return 1
	  }

	  if {[regexp -- {^\s*$} $CPrimeInf::show]} {
	       Msg "\ncheck your Show entry, it is empty !\n"
	       return 1
	  }

	  if {[regexp -- {^\s*$} $CPrimeInf::shot]} {
	       Msg "\ncheck your Shot entry, it is empty !\n"
	       return 1
	  }

	  if {[regexp -- {^\s*$} $CPrimeInf::dept]} {
	       Msg "\ncheck your Dept entry, it is empty !\n"
	       return 1
	  }

	  if {[regexp -- {^\s*$} $CPrimeInf::layer]} {
	       Msg "\nWho are you? \ncheck your User entry, it is empty !\n"
	       return 1
	  }

	  if {[regexp -- {^\s*$} $CPrimeInf::revision]} {
	       Msg "\ncheck your revision entry, it is empty !\n"
	       return 1
	  }

	  if {[regexp -- {^\s*$} $CPrimeInf::crew]} {
	       Msg "\ncheck your Partition entry, it is empty !\n"
	       return 1
	  }

	  return 0

     }

     proc CheckLogicError {} {
	  if !$CRib::mtorDo {
	       #case random frame
	       if $CAlfred::frameInf {
		    foreach frameNum $CRib::frameInf {
			 if {![file exists $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.[format %04d $frameNum]_o.rib]} {
			      Msg "\noriginal rib file of frame #[format %04d $frameNum] is not exist!\n\ncheck your genRib option...\n"
			      return 1
			 }
		    } 

	       } else {
		    for {set i $CRib::startFrame} {$i <= $CRib::endFrame} {incr i $CRib::frameInterval} {
			 if {![file exists $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.[format %04d $i]_o.rib]} {
			      Msg "\noriginal rib file of frame #[format %04d $i] is not exist!\n\ncheck your genRib option...\n"
			      return 1
			 }
		    }
	       }
	  }

	  if {![regexp -- {^\s*$} $CRib::bgName]} {
	       for {set i $CRib::startFrame} {$i <= $CRib::endFrame} {incr i $CRib::frameInterval} {

		    regsub -all {\.####\.} $CRib::bgName "\.[format %04d $i]\." tmp

		    if {![file exists $tmp]} {
			 if {![string compare "no" [tk_messageBox -message "\n$tmp is not exist\n\nIgnore?\n" -type yesno]]} {
			      return 1
			 }
		    }
	       }
	  }


	  if $CRib::mtorDo {
	       if {[regexp -- {^\s*$} $CRib::cameraName]} {
		    Msg "\ncheck your Camera name entry, it is empty !\n"
		    return 1
	       }
	       if {[regexp -- {^\s*$} $CRib::mtorID]} {
		    Msg "\ncheck your MtorID entry, it is empty !\n"
		    return 1
	       }
	       if {[regexp -- {^\s*$} $CRib::startFrame]} {
		    Msg "\ncheck your Frame range(start frame), it is empty !\n"
		    return 1
	       }
	       if {[regexp -- {^\s*$} $CRib::endFrame]} {
		    Msg "\ncheck your Frame range(end frame), it is empty !\n"
		    return 1
	       }
	       if {[regexp -- {^\s*$} $CRib::frameInterval]} {
		    Msg "\ncheck your Frame range(frame interval), it is empty !\n"
		    return 1
	       }
	       if {[regexp -- {^\s*$} $CRib::formatX]} {
		    Msg "\ncheck your display format(width), it is empty !\n"
		    return 1
	       }
	       if {[regexp -- {^\s*$} $CRib::formatY]} {
		    Msg "\ncheck your display format(height), it is empty !\n"
		    return 1
	       }
	       if {[regexp -- {^\s*$} $CRib::pixelSampleX]} {
		    Msg "\ncheck your pixel sample(X), it is empty !\n"
		    return 1
	       }
	       if {[regexp -- {^\s*$} $CRib::pixelSampleY]} {
		    Msg "\ncheck your pixel sample(Y), it is empty !\n"
		    return 1
	       }
	       if {[regexp -- {^\s*$} $CRib::shadingRate]} {
		    Msg "\ncheck your shading rate , it is empty !\n"
		    return 1
	       }
	  }

	  if {$CRib::startFrame > $CRib::endFrame} {
	    	Msg "\nstart_frame is bigger than end_frame!\ncheck your frame range\n"
	    	return 1
	  }
	  if {$CRib::frameInterval < 1} {
	    	Msg "\nframe interval must be bigger than 0 !\ncheck your frame interval\n"
	    	return 1
	  }

	  if $Global::jobMode {
	       if [file exists $CAlfred::dpsFileName] {
		    Msg "\n$CAlfred::dpsFileName is already exist\n\nCheck your revision number!\n"
		    return 1
		}
	  }

	  if [file exists $CAlfred::sceneDir/scenes/$CAlfred::sceneName] {
	       if {![string compare "no" [tk_messageBox -message "\n$CAlfred::sceneName already exist\n\nOverwrite?\n" -type yesno]]} {
		    set CAlfred::copyScene 0;
	       }
	  }


	  return 0
     }

     proc SendMessage {recipient email_server subject body} {
	  package require smtp
	  package require mime
	  set token [mime::initialize -canonical text/plain -string $body]
	  mime::setheader $token Subject $subject
	  smtp::sendmessage $token -recipients $recipient -servers $email_server
	  mime::finalize $token
     } 

     proc CheckSynErrMaya {} {

	  if {[regexp -- {^\s*$} $CMaya::ID]} {
	       Msg "\ncheck your \"ID\", it is empty !\n"
	       return 1
	  }
	  if {$CMaya::camNameDo && [regexp -- {^\s*$} $CMaya::camName]} {
	       Msg "\ncheck your \"camera name\", it is empty !\n"
	       return 1
	  }
	  if {$CMaya::frameRangeDo && [regexp -- {^\s*$} $CMaya::frameRange]} {
	       Msg "\ncheck your \"frame range\", it is empty !\n"
	       return 1
	  }
	  if {$CMaya::imgHDo && [regexp -- {^\s*$} $CMaya::imgH]} {
	       Msg "\ncheck your \"image height\", it is empty !\n"
	       return 1
	  }
	  if {$CMaya::imgWDo && [regexp -- {^\s*$} $CMaya::imgW]} {
	       Msg "\ncheck your \"image width\", it is empty !\n"
	       return 1
	  }
	  if {$CMaya::imgARDo && [regexp -- {^\s*$} $CMaya::imgAR]} {
	       Msg "\ncheck your \"image aspect ratio\", it is empty !\n"
	       return 1
	  }
	  if {$CMaya::fimgHDo && [regexp -- {^\s*$} $CMaya::fimgH]} {
	       Msg "\ncheck your \"y-resolution final image\", it is empty !\n"
	       return 1
	  }
	  if {$CMaya::fimgWDo && [regexp -- {^\s*$} $CMaya::fimgW]} {
	       Msg "\ncheck your \"x-resolution final image\", it is empty !\n"
	       return 1
	  }
	  if {$CMaya::xLeftPixelBoundDo && [regexp -- {^\s*$} $CMaya::xLeftPixelBound]} {
	       Msg "\ncheck your \"x sub-region left pixel boundary\", it is empty !\n"
	       return 1
	  }
	  if {$CMaya::xRightPixelBoundDo && [regexp -- {^\s*$} $CMaya::xRightPixelBound]} {
	       Msg "\ncheck your \"x sub-region right pixel boundary\", it is empty !\n"
	       return 1
	  }
	  if {$CMaya::yLowPixelBoundDo && [regexp -- {^\s*$} $CMaya::yLowPixelBound]} {
	       Msg "\ncheck your \"y sub-region low pixel boundary\", it is empty !\n"
	       return 1
	  }
	  if {$CMaya::yHightPixelBoundDo && [regexp -- {^\s*$} $CMaya::yHightPixelBound]} {
	       Msg "\ncheck your \"y sub-region high pixel boundary\", it is empty !\n"
	       return 1
	  }
	  if {$CMaya::imgFormatDo && [regexp -- {^\s*$} $CMaya::imgFormat]} {
	       Msg "\ncheck your \"output format\", it is empty !\n"
	       return 1
	  }
	  if {$CMaya::shutterAngleDo && [regexp -- {^\s*$} $CMaya::shutterAngle]} {
	       Msg "\ncheck your \"shutter angle\", it is empty !\n"
	       return 1
	  }
	  if {$CMaya::MBByFrameDo && [regexp -- {^\s*$} $CMaya::MBByFrame]} {
	       Msg "\ncheck your \"motion blur by frame\", it is empty !\n"
	       return 1
	  }
	  if {$CMaya::2DMBLengthDo && [regexp -- {^\s*$} $CMaya::2DMBLength]} {
	       Msg "\ncheck your \"2D motion blur length\", it is empty !\n"
	       return 1
	  }
	  if {$CMaya::2DMBSharpnessDo && [regexp -- {^\s*$} $CMaya::2DMBSharpness]} {
	       Msg "\ncheck your \"2D motion blur sharpness\", it is empty !\n"
	       return 1
	  }
	  if {$CMaya::2DMBSmoothValDo && [regexp -- {^\s*$} $CMaya::2DMBSmoothVal]} {
	       Msg "\ncheck your \"2D motion blur smooth value\", it is empty !\n"
	       return 1
	  }
	  if {$CMaya::MBVisibilitySamDo && [regexp -- {^\s*$} $CMaya::MBVisibilitySam]} {
	       Msg "\ncheck your \"num. motion blur visibility samples\", it is empty !\n"
	       return 1
	  }
	  if {$CMaya::maxMBVisibilitySamDo && [regexp -- {^\s*$} $CMaya::maxMBVisibilitySam]} {
	       Msg "\ncheck your \"max motion blur visibility samples\", it is empty !\n"
	       return 1
	  }
	  if {$CMaya::maxRTReflectionLevDo && [regexp -- {^\s*$} $CMaya::maxRTReflectionLev]} {
	       Msg "\ncheck your \"max ray-tracing reflecton level\", it is empty !\n"
	       return 1
	  }
	  if {$CMaya::maxRTRefractionLevDo && [regexp -- {^\s*$} $CMaya::maxRTRefractionLev]} {
	       Msg "\ncheck your \"max ray-tracing refraction level\", it is empty !\n"
	       return 1
	  }
	  if {$CMaya::maxRTShadowRayDepDo && [regexp -- {^\s*$} $CMaya::maxRTShadowRayDep]} {
	       Msg "\ncheck your \"max ray-tracing shadow ray depth\", it is empty !\n"
	       return 1
	  }
	  if {$CMaya::MPFilteringDo && [regexp -- {^\s*$} $CMaya::MPFiltering]} {
	       Msg "\ncheck your \"multi-pixel filtering\", it is empty !\n"
	       return 1
	  }
	  if {$CMaya::SSamplesDo && [regexp -- {^\s*$} $CMaya::SSamples]} {
	       Msg "\ncheck your \"shading samples per surface/pixel\", it is empty !\n"
	       return 1
	  }
	  if {$CMaya::maxSSamplesDo && [regexp -- {^\s*$} $CMaya::maxSSamples]} {
	       Msg "\ncheck your \"max adaptive shading samples/surface/pixel\", it is empty !\n"
	       return 1
	  }
	  if {$CMaya::volumeSSDo && [regexp -- {^\s*$} $CMaya::volumeSS]} {
	       Msg "\ncheck your \"global num. volume shading samples\", it is empty !\n"
	       return 1
	  }
	  if {$CMaya::particleVSDo && [regexp -- {^\s*$} $CMaya::particleVS]} {
	       Msg "\ncheck your \"num.particle visibility samples\", it is empty !\n"
	       return 1
	  }
	  if {$CMaya::antiAliasingDo && [regexp -- {^\s*$} $CMaya::antiAliasing]} {
	       Msg "\ncheck your \"anti-aliasing quality fo EAS\", it is empty !\n"
	       return 1
	  }
	  if {$CMaya::gammaDo && [regexp -- {^\s*$} $CMaya::gamma]} {
	       Msg "\ncheck your \"gamma\", it is empty !\n"
	       return 1
	  }
	  if {$CMaya::rContrastDo && [regexp -- {^\s*$} $CMaya::rContrast]} {
	       Msg "\ncheck your \"red channel contrast threshold\", it is empty !\n"
	       return 1
	  }
	  if {$CMaya::gContrastDo && [regexp -- {^\s*$} $CMaya::gContrast]} {
	       Msg "\ncheck your \"green channel contrast threshold\", it is empty !\n"
	       return 1
	  }
	  if {$CMaya::bContrastDo && [regexp -- {^\s*$} $CMaya::bContrast]} {
	       Msg "\ncheck your \"blue channel contrast threshold\", it is empty !\n"
	       return 1
	  }
	  if {$CMaya::pContrastDo && [regexp -- {^\s*$} $CMaya::pContrast]} {
	       Msg "\ncheck your \"pixel coverage contrast threshold\", it is empty !\n"
	       return 1
	  }
	  return 0
     }
     
     proc CheckLogicErrMaya {} {
	  if {$CMaya::startFrame > $CMaya::endFrame} {
	    	Msg "\nstart_frame is bigger than end_frame!\ncheck your frame range\n"
	    	return 1
	  }
	  if {$CMaya::byFrame < 1} {
	    	Msg "\nframe interval must be bigger than 0 !\ncheck your frame interval\n"
	    	return 1
	  }

	  if $Global::jobMode {
	       if [file exists $CAlfred::dpsFileName] {
		    Msg "\n$CAlfred::dpsFileName is already exist\n\nCheck your revision number!\n"
		    return 1
		}
	  }

	  if [file exists $CAlfred::sceneDir/scenes/$CAlfred::sceneName] {
	       if {![string compare "no" [tk_messageBox -message "\n$CAlfred::sceneName already exist\n\nOverwrite?\n" -type yesno]]} {
		    set CAlfred::copyScene 0;
	       }
	  }
	  return 0
     }

}

proc Msg { msg } {
     tk_messageBox -message $msg
}

####################################################################################################
#											           #
#				 	    Remote maya					           #
#											           #
####################################################################################################

proc WrScript_remote_maya {} {
     if [CAlfred::CheckSynErrMaya] {
	  return 1
     }
     CAlfred::init
     CAlfred::makeDir

     if [CAlfred::CheckLogicErrMaya] {
	  return 1
     }

     set file [open $CAlfred::jobDir/$CAlfred::jobScrName w]

     #########################################################
     #
     # case random frame
     #
     #########################################################
     if $CAlfred::frameInf {

     #=========================================================================================#
	  puts $file \
"##AlfredToDo 3.0
#set AlfJobPause 1

Job -title {$CAlfred::jobTitle} -crews { $CPrimeInf::crew } -subtasks \{
     
     Task -title {copy scene file} -subtasks {

     } -cmds \{"
     #=========================================================================================#

     if $CAlfred::copyScene {

     #=========================================================================================#
	  puts $file \
"     
	  # copy maya scene 
	  Cmd {CMD /C CP $CPrimeInf::scene $CAlfred::sceneDir/scenes/$CAlfred::sceneName}"
     #=========================================================================================#
	  
     }

     #=========================================================================================#
     puts $file \
"
     \}

     Task -title {Render(maya) Process} -subtasks \{

	  Instance {copy scene file}"
     #=========================================================================================#
     
     foreach frameNum $CMaya::frameRange {

     #=========================================================================================#
	  puts $file \
"
	  Task -title {frame [format %04d $frameNum]} -subtasks {

	  } -cmds {

	       RemoteCmd {render $CAlfred::mayaOption -s $frameNum -e $frameNum -b 1 -rd $CAlfred::renDir/$CMaya::ID/$CPrimeInf::output -im $CAlfred::renDir/$CMaya::ID/$CPrimeInf::output/$CAlfred::baseRibName $CAlfred::sceneDir/scenes/$CAlfred::sceneName } -service {maya} -atmost 1 -tags {netrender}
	    
	  } -cleanup {

	       Cmd {y:/bin/mv $CAlfred::renDir/$CMaya::ID/$CPrimeInf::output/$CAlfred::baseRibName$Global::fileExt($CMaya::imgFormat).$frameNum $CAlfred::renDir/$CMaya::ID/$CPrimeInf::output/$CAlfred::baseRibName.[format %04d $frameNum]$Global::fileExt($CMaya::imgFormat)}

	  } -chaser {

	       $CAlfred::viewer $CAlfred::renDir/$CMaya::ID/$CPrimeInf::output/$CAlfred::baseRibName.[format %04d $frameNum]$Global::fileExt($CMaya::imgFormat)

	  }"
     #=========================================================================================#

     }

     #=========================================================================================#
     puts $file \
"
     \}"
     #=========================================================================================#

     #****************************************************************************************#
     #											      #
     # Make daily
     #											      #
     #****************************************************************************************#

     if $Global::jobMode {

	  exec $CAlfred::dpsMediaUtility -Batch -F$CAlfred::dpsFileName -D$CAlfred::dpsFrameRange
	  
     #=========================================================================================#
	  puts $file \
"
     Task -title {Dps Process} -subtasks \{

	  Task {make slate} -subtasks {

	       Instance {Render(maya) Process}

	  } -cmds {
	       
	       RemoteCmd {CMD /C SET NR_INCLUDE_PATH=//chunji/shakemacro/nreal/include& shake -digislatex \"$CPrimeInf::shot\" \"$CPrimeInf::show\" \"$CDps::date\" \"$CPrimeInf::layer\" \"$CMaya::startFrame - $CMaya::endFrame \/ $CMaya::byFrame\" \"\" \"$CDps::mm\" \"$CDps::anim\" \"$CDps::fx\" \"$CDps::td\" \"$CDps::roto\" \"$CDps::comp\" \"$CDps::coord\" \"$CDps::background\" \"$CDps::cameraMove\" \"$CDps::pixelRate\" \"$CDps::audio\" \"$CDps::cmd\" \"$CDps::notes\" -setalpha 0 -fo $CAlfred::renDir/$CMaya::ID/$CPrimeInf::output/$CAlfred::baseRibName.slate.tif} -service {shake} -atmost 1

	  } -cleanup {

		Cmd {CMD /C CP $CAlfred::renDir/$CMaya::ID/$CPrimeInf::output/$CAlfred::baseRibName.slate.tif $CAlfred::dpsImgName\[format %04d 0\].TIF} -tags {DPSCopy}

	  } -chaser {

		sho $CAlfred::dpsImgName\[format %04d 0\].TIF
	  }

	  Task {make image} -subtasks \{"
     #=========================================================================================#
     
	  set dpsCount 1;
	  foreach frameNum $CMaya::frameRange {

     #=========================================================================================#
	       puts $file \
"
	       Task {Tab sign [format %04d $frameNum]} -subtasks {

		    Instance {Render(maya) Process}

	       } -cmds {

		    # make tab sign
		    RemoteCmd {shake  $CAlfred::renDir/$CMaya::ID/$CPrimeInf::output/$CAlfred::baseRibName.[format %04d $frameNum]$Global::fileExt($CMaya::imgFormat) -addtext \"$CPrimeInf::show[string toupper $CPrimeInf::shot 0 0].$CAlfred::dpsDailyNum $CPrimeInf::layer $frameNum\" \"arial\" 24 24 1 32 32 2 1 -setalpha 0 -fo $CAlfred::renDir/$CMaya::ID/$CPrimeInf::output/$CAlfred::baseRibName.[format %04d $frameNum]\_dps$Global::fileExt($CMaya::imgFormat) } -service {shake} -atmost 1
		    # copy to dps
		    Cmd {CMD /C CP $CAlfred::renDir/$CMaya::ID/$CPrimeInf::output/$CAlfred::baseRibName.[format %04d $frameNum]\_dps$Global::fileExt($CMaya::imgFormat) $CAlfred::dpsImgName[format %04d $dpsCount].TIF} -tags {DPSCopy}

	       } -cleanup {

		    # delete tab sign image 
		    Cmd {Alfred} -msg {File delete $CAlfred::renDir/$CMaya::ID/$CPrimeInf::output/$CAlfred::baseRibName.[format %04d $frameNum]\_dps$Global::fileExt($CMaya::imgFormat)}

	       } -chaser {
     
		    $CAlfred::viewer $CAlfred::dpsImgName[format %04d $dpsCount].TIF
	       }"
     #=========================================================================================# 

	       set dpsCount [expr $dpsCount + 1] 

	  } ;# foreach

     #=========================================================================================# 
	  puts $file \
"
	  \}

     \}"
     #=========================================================================================# 

     } ;# if

     #=========================================================================================# 
     puts $file \
"
\} "
     #=========================================================================================# 

     #########################################################
     #
     # case seq frame
     #
     #########################################################
     } else {

     #=========================================================================================#
	  puts $file \
"##AlfredToDo 3.0
#set AlfJobPause 1

Job -title {$CAlfred::jobTitle} -crews { $CPrimeInf::crew } -subtasks \{
     
     Task -title {copy scene file} -subtasks {

     } -cmds \{"
     #=========================================================================================#

     if $CAlfred::copyScene {

     #=========================================================================================#
	  puts $file \
"     
	  # copy maya scene 
	  Cmd {CMD /C CP $CPrimeInf::scene $CAlfred::sceneDir/scenes/$CAlfred::sceneName}"
     #=========================================================================================#
	  
     }

     #=========================================================================================#
     puts $file \
"
     \}

     Task -title {Render(maya) Process} -subtasks {
	  
	  Iterate count -from $CMaya::startFrame -to $CMaya::endFrame -by $CMaya::byFrame -subtasks {

	       Instance {copy scene file} 

	  } -template {

	       Task -title {frame \[format %04d \$count\]} -subtasks {

	       } -cmds {

		    RemoteCmd {render $CAlfred::mayaOption -s \$count -e \$count -b 1 -rd $CAlfred::renDir/$CMaya::ID/$CPrimeInf::output -im $CAlfred::renDir/$CMaya::ID/$CPrimeInf::output/$CAlfred::baseRibName $CAlfred::sceneDir/scenes/$CAlfred::sceneName } -service {maya} -atmost 1 -tags {netrender}
		    
	       } -cleanup {

		    Cmd {y:/bin/mv $CAlfred::renDir/$CMaya::ID/$CPrimeInf::output/$CAlfred::baseRibName$Global::fileExt($CMaya::imgFormat).\$count $CAlfred::renDir/$CMaya::ID/$CPrimeInf::output/$CAlfred::baseRibName.\[format %04d \$count\]$Global::fileExt($CMaya::imgFormat)}

	       } -chaser {

		    $CAlfred::viewer $CAlfred::renDir/$CMaya::ID/$CPrimeInf::output/$CAlfred::baseRibName.\[format %04d \$count\]$Global::fileExt($CMaya::imgFormat)

	       }
	  }
     }"

     #****************************************************************************************#
     #											      #
     # Make daily
     #											      #
     #****************************************************************************************#

     if $Global::jobMode {

	  exec $CAlfred::dpsMediaUtility -Batch -F$CAlfred::dpsFileName -D$CAlfred::dpsFrameRange
	  
     ########################################################################################### 
	  puts $file \
"
     Task -title {Dps Process} -subtasks {

	  Task {make slate} -subtasks {

	       Instance {Render(maya) Process}

	  } -cmds {
	       
	       RemoteCmd {CMD /C SET NR_INCLUDE_PATH=//chunji/shakemacro/nreal/include& shake -digislatex \"$CPrimeInf::shot\" \"$CPrimeInf::show\" \"$CDps::date\" \"$CPrimeInf::layer\" \"$CMaya::startFrame - $CMaya::endFrame \/ $CMaya::byFrame\" \"\" \"$CDps::mm\" \"$CDps::anim\" \"$CDps::fx\" \"$CDps::td\" \"$CDps::roto\" \"$CDps::comp\" \"$CDps::coord\" \"$CDps::background\" \"$CDps::cameraMove\" \"$CDps::pixelRate\" \"$CDps::audio\" \"$CDps::cmd\" \"$CDps::notes\" -setalpha 0 -fo $CAlfred::renDir/$CMaya::ID/$CPrimeInf::output/$CAlfred::baseRibName.slate.tif} -service {shake} -atmost 1

	  } -cleanup {

		Cmd {CMD /C CP $CAlfred::renDir/$CMaya::ID/$CPrimeInf::output/$CAlfred::baseRibName.slate.tif $CAlfred::dpsImgName\[format %04d 0\].TIF} -tags {DPSCopy}

	  } -chaser {

		sho $CAlfred::dpsImgName\[format %04d 0\].TIF
	  }

	  Task {make image} -subtasks {

	       Iterate count -from 1 -to [expr $CAlfred::dpsFrameRange - 1] -by 1 -template {

		    Task {Tab sign \[format %04d \$count\]} -subtasks {

			 Instance {Render(maya) Process}

		    } -cmds {

			 # make tab sign
			 RemoteCmd {shake  $CAlfred::renDir/$CMaya::ID/$CPrimeInf::output/$CAlfred::baseRibName.\[format %04d \[expr (\$count-1)*$CMaya::byFrame+$CMaya::startFrame\]\]$Global::fileExt($CMaya::imgFormat) -addtext \"$CPrimeInf::show[string toupper $CPrimeInf::shot 0 0].$CAlfred::dpsDailyNum $CPrimeInf::layer \$count\" \"arial\" 24 24 1 32 32 2 1 -setalpha 0 -fo $CAlfred::renDir/$CMaya::ID/$CPrimeInf::output/$CAlfred::baseRibName.\[format %04d \[expr (\$count-1)*$CMaya::byFrame+$CMaya::startFrame\]\]\_dps$Global::fileExt($CMaya::imgFormat) } -service {shake} -atmost 1
			 # copy to dps
			 Cmd {CMD /C CP $CAlfred::renDir/$CMaya::ID/$CPrimeInf::output/$CAlfred::baseRibName.\[format %04d \[expr (\$count-1)*$CMaya::byFrame+$CMaya::startFrame\]\]\_dps$Global::fileExt($CMaya::imgFormat) $CAlfred::dpsImgName\[format %04d \$count\].TIF} -tags {DPSCopy}

		    } -cleanup {

			 # delete tab sign image 
			 Cmd {Alfred} -msg {File delete $CAlfred::renDir/$CMaya::ID/$CPrimeInf::output/$CAlfred::baseRibName.\[format %04d \[expr (\$count-1)*$CMaya::byFrame+$CMaya::startFrame\]\]\_dps$Global::fileExt($CMaya::imgFormat)}

		    } -chaser {

			 $CAlfred::viewer $CAlfred::dpsImgName\[format %04d \$count\].TIF
		    }

	       }

	  } 
     }"
     #=========================================================================================#

     }

     #=========================================================================================#
     puts $file \
"
\} "
     #=========================================================================================#

     } ;# else

     #=========================================================================================#

     close $file
     exec alfred $CAlfred::jobDir/$CAlfred::jobScrName &
}

proc WrScript_remote {} { 
     if [CAlfred::CheckSynError] {
	  return 1
     }

     CAlfred::init
     CAlfred::makeDir

     if [CAlfred::CheckLogicError] {
	  return 1
     }

     set file [open $CAlfred::jobDir/$CAlfred::jobScrName w]

     #########################################################
     #
     # case random frame
     #
     #########################################################
     if $CAlfred::frameInf {

     #=========================================================================================#
	  puts $file \
"##AlfredToDo 3.0
#set AlfJobPause 1

Job -title {$CAlfred::jobTitle} -crews { $CPrimeInf::crew } -subtasks \{
     
     Task -title {copy scene file} -subtasks {

     } -cmds \{"
     #=========================================================================================#

	  if $CAlfred::copyScene {

     #=========================================================================================#
	       puts $file \
"     
	  # copy maya scene 
	       Cmd {CMD /C CP $CPrimeInf::scene $CAlfred::sceneDir/scenes/$CAlfred::sceneName}"
     #=========================================================================================#
	  
	  }

     #=========================================================================================#
	  puts $file \
"
     \}

     Task -title {Render Process} -subtasks \{
	  
	  Instance {copy scene file}"
     #=========================================================================================#

	  foreach frameNum $CRib::frameInf {

     #=========================================================================================#
	       puts $file \
"
	  Task -title {frame [format %04d $frameNum]} -subtasks \{

	       Task -title {genRib [format %04d $frameNum]} -subtasks {

	       } -cmds \{"
     #=========================================================================================#
			 
     #=========================================================================================#
     #	
     # gen Rib
     #
     #=========================================================================================#

	       if $CRib::mtorDo {

     #=========================================================================================#
		    puts $file \
" 
		    RemoteCmd {mtor_maya4 $CAlfred::mtorOption -cmd genRib [format %04d $frameNum]} -service {pixarNrm} -atmost 1 -tags {netrender}
		    
	       \} -cleanup \{
	       
		    Cmd {y:/bin/mv $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.[format %04d $frameNum].rib $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.[format %04d $frameNum]_o.rib}"
     #=========================================================================================#

	       }
     
     #=========================================================================================#
	       puts $file \
"
	       \} -chaser {
			 
		    NOTEPAD $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.[format %04d $frameNum]_o.rib

	       }"
     #=========================================================================================#

	       if {![regexp -- {^\s*$} $CRib::cmd]} {
		    set CAlfred::editRib 1

     #****************************************************************************************#
     #											      #
     # R2R
     #											      #
     #****************************************************************************************#

     #=========================================================================================#

		    puts $file \
"
	       Task -title {r2r [format %04d $frameNum]} -subtasks {

		    Instance {genRib [format %04d $frameNum]} 

	       } -cmds {

		    RemoteCmd {CMD /C \"y:/bin/perl.exe y:/usr/local/bin/r2r-1.07 $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.[format %04d $frameNum]_o.rib $CRib::cmd > $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.[format %04d $frameNum].rib\"} -service {pixarNrm} -atmost 1 -tags {netrender}
	       }"

     #=========================================================================================#

	       }

     #=========================================================================================#
	       puts $file \
"		    
	       Task -title {ezRibEdit [format %04d $frameNum]} -subtasks \{"

     #=========================================================================================#

	       if {![regexp -- {^\s*$} $CAlfred::ribEditOption]} {
		    set CAlfred::editRib 1
		    # case didR2r
		    if {![regexp -- {^\s*$} $CRib::cmd]} {

     #****************************************************************************************#
     #											      #
     # RIBEDIT - main 
     #											      #
     #****************************************************************************************#

     #=========================================================================================#
			 puts $file \
" 
		    Task -title {chunk - main} -subtasks {

			 Instance {r2r [format %04d $frameNum]}

		    } -cmds {

			 # case didR2r
			 RemoteCmd {CMD /C \"y:/bin/perl.exe y:/usr/local/bin/ezRibEdit $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.[format %04d $frameNum].rib $CAlfred::ribEditOption > $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.[format %04d $frameNum]_ribedit.rib\"} -service {pixarNrm} -atmost 1 -tags {netrender}

		    } -cleanup {

			 Cmd {y:/bin/mv $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.[format %04d $frameNum]_ribedit.rib $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.[format %04d $frameNum].rib}

		    }"
     #=========================================================================================#

		    } else {
			 #case didNotR2r 

     #=========================================================================================#
			 puts $file \
"			 	    
		    Task -title {chunk - main} -subtasks {

			 Instance {genRib [format %04d $frameNum]}

		    } -cmds {
			      
			 # case didNotR2r
			 RemoteCmd {CMD /C \"y:/bin/perl.exe y:/usr/local/bin/ezRibEdit $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.[format %04d $frameNum]_o.rib $CAlfred::ribEditOption > $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.[format %04d $frameNum].rib\"} -service {pixarNrm} -atmost 1 -tags {netrender}

		    }"
     #=========================================================================================#

		    }

	       } else {

		    if {![regexp -- {^\s*$} $CRib::cmd]} {

     #=========================================================================================#
			 puts $file \
"			 
		    Instance {r2r [format %04d $frameNum]}"
     #=========================================================================================#

		    } else {
	       
     #=========================================================================================#
		    puts $file \
"			 
		    Instance {genRib [format %04d $frameNum]}"
     #=========================================================================================#

		    }
	       }

     #****************************************************************************************#
     #											      #
     # RIBEDIT - ren hero chunk 
     #											      #
     #****************************************************************************************#

	       for {set i 1} {$i <= $CChunk::renHeroNum} {incr i} {

		    if {$CChunk::renHeroChunk($i,1)} {
			 regsub -all -line {"} $CChunk::renHeroChunk($i,3) \\\\\\\\\" tmp 
			 regsub -all -line {\s\*} $tmp " \\\\\\\\\\\\\\\\\*" tmp 
     
			 set tmp [concat $tmp "-bind global -shddir $CAlfred::shdDir/$CRib::mtorID/$CPrimeInf::output/$CChunk::renHeroChunk($i,2)"]
			 set tmp [concat $tmp "-bind global -outName $CPrimeInf::shot\_$CPrimeInf::layer\_$CRib::mtorID\_$CChunk::renHeroChunk($i,2)\_$CPrimeInf::revision.[format %04d $frameNum]$Global::fileExt($CRib::dspyServer)"]
			 set CAlfred::chunkRibEditOption [concat $tmp "-bind global -outdir $CAlfred::renDir/$CRib::mtorID/$CPrimeInf::output/$CChunk::renHeroChunk($i,2)"]
	  
			 if {![regexp -- {^\s*$} $CRib::cmd]} {

     #=========================================================================================#
			      puts $file \
" 
		    Task -title {chunk - $CChunk::renHeroChunk($i,2)} -subtasks {

			 Instance {r2r [format %04d $frameNum]}

		    } -cmds {
			      
			 # case didR2r
			 RemoteCmd {CMD /C \"y:/bin/perl.exe y:/usr/local/bin/ezRibEdit $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.[format %04d $frameNum].rib $CAlfred::chunkRibEditOption > $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output/$CChunk::renHeroChunk($i,2)/$CAlfred::baseRibName.[format %04d $frameNum].rib\"} -service {pixarNrm} -atmost 1 -tags {netrender}

		    }"
     #=========================================================================================#

			 } else {
			      #case didNotR2r 

     #=========================================================================================#
			      puts $file \
"		   
		    Task -title {chunk - $CChunk::renHeroChunk($i,2)} -subtasks {

			 Instance {genRib [format %04d $frameNum]}

		    } -cmds {

			 # case didNotR2r
			 RemoteCmd {CMD /C \"y:/bin/perl.exe y:/usr/local/bin/ezRibEdit $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.[format %04d $frameNum]_o.rib $CAlfred::chunkRibEditOption > $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output/$CChunk::renHeroChunk($i,2)/$CAlfred::baseRibName.[format %04d $frameNum].rib\"} -service {pixarNrm} -atmost 1 -tags {netrender}
		      
		    }"
     #=========================================================================================#

			 }

		    }

	       } ;# for

     #=========================================================================================#

     #****************************************************************************************#
     #											      #
     # PRMAN - main 
     #											      #
     #****************************************************************************************#

     #=========================================================================================#
	       puts $file \
"
	       \} -cmds {
		    
	       }

	       Task -title {prman [format %04d $frameNum]} -subtasks \{

		    Task -title {prman main} -subtasks {

			 Instance {ezRibEdit [format %04d $frameNum]}

		    } -cmds \{"
     #=========================================================================================#

	       if $CAlfred::editRib {

     #=========================================================================================#
		    puts $file \
"			      
			 RemoteCmd { prman -Progress $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.[format %04d $frameNum].rib } -service {pixarNrm} -almost 1 -tags {netrender}"

     #=========================================================================================#

	       } else {
	  
     #=========================================================================================#
		    puts $file \
"			      
			 RemoteCmd { prman -Progress $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.[format %04d $frameNum]_o.rib } -service {pixarNrm} -almost 1 -tags {netrender}"
     #=========================================================================================#

	       }

     #****************************************************************************************#
     #											      #
     # PRMAN - background
     #											      #
     #****************************************************************************************#

	       if {![regexp -- {^\s*$} $CRib::bgName]} {

		    regsub -all {\.####\.} $CRib::bgName "\.[format %04d $frameNum ]\." CAlfred::bgName

     #=========================================================================================#
		    puts $file \
"
			 RemoteCmd { shake $CAlfred::renDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.[format %04d $frameNum]$Global::fileExt($CRib::dspyServer) -over $CAlfred::bgName -fo $CAlfred::renDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.[format %04d $frameNum]\_bg$Global::fileExt($CRib::dspyServer)} -service {shake} -almost 1
		 
		    \} -cleanup \{

			 Cmd {y:/bin/mv $CAlfred::renDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.[format %04d $frameNum]\_bg$Global::fileExt($CRib::dspyServer) $CAlfred::renDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.[format %04d $frameNum]$Global::fileExt($CRib::dspyServer)}"
     #=========================================================================================#

		    if {$CAlfred::editRib && $CPrimeInf::delRib} {

     #=========================================================================================#
			 puts $file \
"
			 Cmd {Alfred} -msg {File delete $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.[format %04d $frameNum].rib} "
     #=========================================================================================#

		    }

	       } else {
		    
		    if {$CAlfred::editRib && $CPrimeInf::delRib} {

     #=========================================================================================#
			 puts $file \
"
		    \} -cleanup \{

			 Cmd {Alfred} -msg {File delete $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.[format %04d $frameNum].rib} "
     #=========================================================================================#
		    }
	       }

     #=========================================================================================#
	       puts $file \
"			 
		    \} -chaser {
	      
			 $CAlfred::viewer $CAlfred::renDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.[format %04d $frameNum]$Global::fileExt($CRib::dspyServer)
			      
		    }"
     #=========================================================================================#

			 
	       for {set i 1} {$i <= $CChunk::renHeroNum} {incr i} {

		    if {$CChunk::renHeroChunk($i,1)} {

     #****************************************************************************************#
     #											      #
     # PRMAN - chunk
     #											      #
     #****************************************************************************************#
	       
     #=========================================================================================#
			 puts $file \
"
		    Task -title {prman - $CChunk::renHeroChunk($i,2)} -subtasks {

			 Instance {ezRibEdit [format %04d $frameNum]}

		    } -cmds {

			 RemoteCmd { prman -Progress $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output/$CChunk::renHeroChunk($i,2)/$CAlfred::baseRibName.[format %04d $frameNum].rib } -service {pixarNrm} -almost 1 -tags {netrender}

		    } -cleanup \{"
     #=========================================================================================#

			 if {$CPrimeInf::delRib} {

     #=========================================================================================#
			      puts $file \
"
			 Cmd {Alfred} -msg {File delete $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output/$CChunk::renHeroChunk($i,2)/$CAlfred::baseRibName.[format %04d $frameNum].rib}"
     #=========================================================================================#

			 }

     #=========================================================================================#
			 puts $file \
"
		    \} -chaser {

			 $CAlfred::viewer $CAlfred::renDir/$CRib::mtorID/$CPrimeInf::output/$CChunk::renHeroChunk($i,2)/$CPrimeInf::shot\_$CPrimeInf::layer\_$CRib::mtorID\_$CChunk::renHeroChunk($i,2)\_$CPrimeInf::revision.[format %04d $frameNum]$Global::fileExt($CRib::dspyServer)

		    }"
     #=========================================================================================#

		    }
     
	       }

     #=========================================================================================#
	       puts $file \
" 
	       \} -cmds {

		    # prman
	       }"
     #=========================================================================================#

	       if {$CChunk::renGrndShdNum > 0} {

     #=========================================================================================#
		    puts $file \
" 
	       Task {RenGrndShd} -subtasks \{"
     #=========================================================================================#

     #****************************************************************************************#
     #											      #
     # render ground shadow 
     #											      #
     #****************************************************************************************#

		    for {set i 1} {$i <= $CChunk::renGrndShdNum} {incr i} {

			 if {$CChunk::renGrndShdChunk($i,1)} {

     #=========================================================================================#
			 puts $file \
"
		    Task -title { rndGrndShd - [format %04d $frameNum] - $CChunk::renGrndShdChunk($i,5) } -subtasks {
     
			 Instance {genRib [format %04d $frameNum]}
     
		    } -cmds {
     
			 RemoteCmd {y:/usr/local/bin/X_RENGRNDSHD.BAT $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.[format %04d $frameNum]_o.rib $CChunk::renGrndShdChunk($i,2) $CChunk::renGrndShdChunk($i,3) $CChunk::renGrndShdChunk($i,4) $CAlfred::renDir/$CRib::mtorID/$CPrimeInf::output/$CChunk::renGrndShdChunk($i,5)/ renGrndShd_$CChunk::renGrndShdChunk($i,5)\_[format %04d $frameNum] } -service {pixarNrm} -almost 1 -tags {netrender}
	       
		    } -chaser {
     
			 $CAlfred::viewer $CAlfred::renDir/$CRib::mtorID/$CPrimeInf::output/$CChunk::renGrndShdChunk($i,5)/renGrndShd_$CChunk::renGrndShdChunk($i,5)\_[format %04d $frameNum]
     
		    }"
     #=========================================================================================#
			 
			 }
		    }

     #=========================================================================================#
	       puts $file \
"
	       \} -cmds {
		 
		    # RenGrndShd
	       }"
     #=========================================================================================#

	       }

     #=========================================================================================#
	  puts $file \
"
	  \} -cmds {

	  }"
     #=========================================================================================#

	  }

     #=========================================================================================#
     puts $file \
"     \} -cmds {

     }"
     #=========================================================================================#


     #****************************************************************************************#
     #											      #
     # Make daily
     #											      #
     #****************************************************************************************#

	  if $Global::jobMode {

	       exec $CAlfred::dpsMediaUtility -Batch -F$CAlfred::dpsFileName -D$CAlfred::dpsFrameRange
	  
     #=========================================================================================#
	       puts $file \
"
     Task -title {Dps Process} -subtasks \{

	  Task {make slate} -subtasks {

	       Instance {Render Process}

	  } -cmds {
	       
	       RemoteCmd {CMD /C SET NR_INCLUDE_PATH=//chunji/shakemacro/nreal/include& shake -digislatex \"$CPrimeInf::shot\" \"$CPrimeInf::show\" \"$CDps::date\" \"$CPrimeInf::layer\" \"$CRib::startFrame - $CRib::endFrame \/ $CRib::frameInterval\" \"\" \"$CDps::mm\" \"$CDps::anim\" \"$CDps::fx\" \"$CDps::td\" \"$CDps::roto\" \"$CDps::comp\" \"$CDps::coord\" \"$CDps::background\" \"$CDps::cameraMove\" \"$CDps::pixelRate\" \"$CDps::audio\" \"$CDps::cmd\" \"$CDps::notes\" -setalpha 0 -fo $CAlfred::renDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.slate.tif} -service {shake} -atmost 1

	  } -cleanup {

		Cmd {CMD /C CP $CAlfred::renDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.slate.tif $CAlfred::dpsImgName.0000.TIF} -tags {DPSCopy}

	  } -chaser {

		sho $CAlfred::dpsImgName.0000.TIF
	  }

	  Task {make image} -subtasks \{"
     #=========================================================================================#

	       set dpsCount 0

	       foreach frameNum $CRib::frameInf {

     #=========================================================================================#
		    puts $file \
" 
	       Task {Tab sign [format %04d $frameNum]} -subtasks {

		    Instance {Render Process}

	       } -cmds {

		    # make tab sign
		    RemoteCmd {shake  $CAlfred::renDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.[format %04d $frameNum]$Global::fileExt($CRib::dspyServer) -addtext \"$CPrimeInf::show[string toupper $CPrimeInf::shot 0 0].$CAlfred::dpsDailyNum $CPrimeInf::layer $frameNum\" \"arial\" 24 24 1 32 32 2 1 -setalpha 0 -fo $CAlfred::renDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.[format %04d $frameNum]\_dps$Global::fileExt($CRib::dspyServer) } -service {shake} -atmost 1
		    # copy to dps
		    Cmd {CMD /C CP $CAlfred::renDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.[format %04d $frameNum]\_dps$Global::fileExt($CRib::dspyServer) $CAlfred::dpsImgName[format %04d $dpsCount].TIF} -tags {DPSCopy}

	       } -cleanup {

		    # delete tab sign image 
		    Cmd {Alfred} -msg {File delete $CAlfred::renDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.[format %04d $frameNum]\_dps$Global::fileExt($CRib::dspyServer)}

	       } -chaser {
     
		    $CAlfred::viewer $CAlfred::dpsImgName[format %04d $dpsCount].TIF
	       }"
     #=========================================================================================#

		    set dpsCount [expr $dpsCount + 1]
	       }

     #=========================================================================================#
	       puts $file \
" 
	  \}
     \}" 
     #=========================================================================================#

	  } ;# if jobMode...

     #=========================================================================================#
     puts $file \
"
\} "
     #=========================================================================================#

     #########################################################
     #
     # case seq frame
     #
     #########################################################
     } else { 

     #=========================================================================================#
	  puts $file \
"##AlfredToDo 3.0
#set AlfJobPause 1

Job -title {$CAlfred::jobTitle} -crews { $CPrimeInf::crew } -subtasks \{
     
     Task -title {copy scene file} -subtasks {

     } -cmds \{"
     #=========================================================================================#

	  if $CAlfred::copyScene {

     #=========================================================================================#
	       puts $file \
"     
	  # copy maya scene 
	       Cmd {CMD /C CP $CPrimeInf::scene $CAlfred::sceneDir/scenes/$CAlfred::sceneName}"
     #=========================================================================================#
	  
	  }

     #=========================================================================================#
	  puts $file \
"
     \}

     Task -title {Render Process} -subtasks \{
	  
	  Iterate count -from $CRib::startFrame -to $CRib::endFrame -by $CRib::frameInterval -subtasks {

	       Instance {copy scene file} 

	  } -template \{

	       Task -title {frame \[format %04d \$count\]} -subtasks \{

		    Task -title {genRib \[format %04d \$count\]} -subtasks {

		    } -cmds \{"
     #=========================================================================================#
			 
     #=========================================================================================#
     #											      #
     # gen Rib
     #											      #
     #=========================================================================================#

	  if $CRib::mtorDo {

     #=========================================================================================#
	       puts $file \
" 
			 RemoteCmd {mtor_maya4 $CAlfred::mtorOption -cmd genRib \[format %04d \$count\]} -service {pixarNrm} -atmost 1 -tags {netrender}
		    
		    \} -cleanup \{
	       
			 Cmd {y:/bin/mv $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.\[format %04d \$count\].rib $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.\[format %04d \$count\]_o.rib}"
     #=========================================================================================#

	  }
     
     #=========================================================================================#
	  puts $file \
"
		    \} -chaser {
			 
			 NOTEPAD $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.\[format %04d \$count\]_o.rib

		    }"
     #=========================================================================================#

	  if {![regexp -- {^\s*$} $CRib::cmd]} {
	       set CAlfred::editRib 1

     #****************************************************************************************#
     #											      #
     # R2R
     #											      #
     #****************************************************************************************#

     #=========================================================================================#

	       puts $file \
"
		    Task -title {r2r \[format %04d \$count\]} -subtasks {

			 Instance {genRib \[format %04d \$count\]} 

		    } -cmds {

			 RemoteCmd {CMD /C \"y:/bin/perl.exe y:/usr/local/bin/r2r-1.07 $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.\[format %04d \$count\]_o.rib $CRib::cmd > $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.\[format %04d \$count\].rib\"} -service {pixarNrm} -atmost 1 -tags {netrender}
		    }"

     #=========================================================================================#

	  }

     #=========================================================================================#
	  puts $file \
"		    
		    Task -title {ezRibEdit \[format %04d \$count\]} -subtasks \{"

     #=========================================================================================#

	  if {![regexp -- {^\s*$} $CAlfred::ribEditOption]} {
	       set CAlfred::editRib 1
	       # case didR2r
	       if {![regexp -- {^\s*$} $CRib::cmd]} {

     #****************************************************************************************#
     #											      #
     # RIBEDIT - main 
     #											      #
     #****************************************************************************************#

     #=========================================================================================#
		    puts $file \
" 
			 Task -title {chunk - main} -subtasks {

			      Instance {r2r \[format %04d \$count\]}

			 } -cmds {

			      # case didR2r
			      RemoteCmd {CMD /C \"y:/bin/perl.exe y:/usr/local/bin/ezRibEdit $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.\[format %04d \$count\].rib $CAlfred::ribEditOption > $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.\[format %04d \$count\]_ribedit.rib\"} -service {pixarNrm} -atmost 1 -tags {netrender}

			 } -cleanup {

			      Cmd {y:/bin/mv $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.\[format %04d \$count\]_ribedit.rib $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.\[format %04d \$count\].rib}

			 }"
     #=========================================================================================#

	       } else {
		    #case didNotR2r 

     #=========================================================================================#
		    puts $file \
"			 	    
			 Task -title {chunk - main} -subtasks {

			      Instance {genRib \[format %04d \$count\]}

			 } -cmds {
			      
			      # case didNotR2r
			      RemoteCmd {CMD /C \"y:/bin/perl.exe y:/usr/local/bin/ezRibEdit $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.\[format %04d \$count\]_o.rib $CAlfred::ribEditOption > $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.\[format %04d \$count\].rib\"} -service {pixarNrm} -atmost 1 -tags {netrender}

			 }"
     #=========================================================================================#

	       }

	  } else {

	       if {![regexp -- {^\s*$} $CRib::cmd]} {

     #=========================================================================================#
		    puts $file \
"			 
			 Instance {r2r \[format %04d \$count\]}"
     #=========================================================================================#

	       } else {
	       
     #=========================================================================================#
		    puts $file \
"			 
			 Instance {genRib \[format %04d \$count\]}"
     #=========================================================================================#

	       }
	  }

     #****************************************************************************************#
     #											      #
     # RIBEDIT - ren hero chunk 
     #											      #
     #****************************************************************************************#

	  for {set i 1} {$i <= $CChunk::renHeroNum} {incr i} {

	       if {$CChunk::renHeroChunk($i,1)} {
		    regsub -all -line {"} $CChunk::renHeroChunk($i,3) \\\\\\\\\" tmp 
		    regsub -all -line {\s\*} $tmp " \\\\\\\\\\\\\\\\\*" tmp 

		    set tmp [concat $tmp "-bind global -shddir $CAlfred::shdDir/$CRib::mtorID/$CPrimeInf::output/$CChunk::renHeroChunk($i,2)"]
		    set tmp [concat $tmp "-bind global -outName $CPrimeInf::shot\_$CPrimeInf::layer\_$CRib::mtorID\_$CChunk::renHeroChunk($i,2)\_$CPrimeInf::revision.\[format %04d \$count\]$Global::fileExt($CRib::dspyServer)"]
		    set CAlfred::chunkRibEditOption [concat $tmp "-bind global -outdir $CAlfred::renDir/$CRib::mtorID/$CPrimeInf::output/$CChunk::renHeroChunk($i,2)"]
     
		    if {![regexp -- {^\s*$} $CRib::cmd]} {

     #=========================================================================================#
			 puts $file \
" 
			 Task -title {chunk - $CChunk::renHeroChunk($i,2)} -subtasks {

			      Instance {r2r \[format %04d \$count\]}

			 } -cmds {
			      
			      # case didR2r
			      RemoteCmd {CMD /C \"y:/bin/perl.exe y:/usr/local/bin/ezRibEdit $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.\[format %04d \$count\].rib $CAlfred::chunkRibEditOption > $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output/$CChunk::renHeroChunk($i,2)/$CAlfred::baseRibName.\[format %04d \$count\].rib\"} -service {pixarNrm} -atmost 1 -tags {netrender}

			 }"
     #=========================================================================================#

		    } else {
			 #case didNotR2r 

     #=========================================================================================#
		    puts $file \
"		    
			 Task -title {chunk - $CChunk::renHeroChunk($i,2)} -subtasks {

			      Instance {genRib \[format %04d \$count\]}

			 } -cmds {

			      # case didNotR2r
			      RemoteCmd {CMD /C \"y:/bin/perl.exe y:/usr/local/bin/ezRibEdit $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.\[format %04d \$count\]_o.rib $CAlfred::chunkRibEditOption > $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output/$CChunk::renHeroChunk($i,2)/$CAlfred::baseRibName.\[format %04d \$count\].rib\"} -service {pixarNrm} -atmost 1 -tags {netrender}
			      
			 }"
     #=========================================================================================#

		    }

	       }

	  } ;# for

     #=========================================================================================#

     #****************************************************************************************#
     #											      #
     # PRMAN - main 
     #											      #
     #****************************************************************************************#

     #=========================================================================================#
	  puts $file \
"
		    \} -cmds {
		    }

		    Task -title {prman \[format %04d \$count\]} -subtasks \{

			 Task -title {prman main} -subtasks {

			      Instance {ezRibEdit \[format %04d \$count\]}

			 } -cmds \{"
     #=========================================================================================#

	  if $CAlfred::editRib {

     #=========================================================================================#
	       puts $file \
"			      
			      RemoteCmd { prman -Progress $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.\[format %04d \$count\].rib } -service {pixarNrm} -almost 1 -tags {netrender}"

     #=========================================================================================#

	  } else {
	  
     #=========================================================================================#
	       puts $file \
"			      
			      RemoteCmd { prman -Progress $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.\[format %04d \$count\]_o.rib } -service {pixarNrm} -almost 1 -tags {netrender}"
     #=========================================================================================#

	  }

     #****************************************************************************************#
     #											      #
     # PRMAN - background
     #											      #
     #****************************************************************************************#

	  if {![regexp -- {^\s*$} $CRib::bgName]} {

	       regsub -all {\.####\.} $CRib::bgName "\.\\\[format %04d \\\$count \\]\." CAlfred::bgName

     #=========================================================================================#
	       puts $file \
"
			      RemoteCmd { shake $CAlfred::renDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.\[format %04d \$count\]$Global::fileExt($CRib::dspyServer) -over $CAlfred::bgName -fo $CAlfred::renDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.\[format %04d \$count\]\_bg$Global::fileExt($CRib::dspyServer)} -service {shake} -almost 1
			 
			 \} -cleanup \{

			      Cmd {y:/bin/mv $CAlfred::renDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.\[format %04d \$count\]\_bg$Global::fileExt($CRib::dspyServer) $CAlfred::renDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.\[format %04d \$count\]$Global::fileExt($CRib::dspyServer)}"
     #=========================================================================================#

	       if {$CAlfred::editRib && $CPrimeInf::delRib} {

     #=========================================================================================#
		    puts $file \
"
			      Cmd {Alfred} -msg {File delete $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.\[format %04d \$count\].rib} "
     #=========================================================================================#

	       }

	  } else {
	       if {$CAlfred::editRib && $CPrimeInf::delRib} {

     #=========================================================================================#
		    puts $file \
"
			 \} -cleanup \{

			      Cmd {Alfred} -msg {File delete $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.\[format %04d \$count\].rib} "
     #=========================================================================================#
	       }
	  }

     #=========================================================================================#
	  puts $file \
"			 
			 \} -chaser {
			      
			      $CAlfred::viewer $CAlfred::renDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.\[format %04d \$count\]$Global::fileExt($CRib::dspyServer)
			      
			 }"
     #=========================================================================================#

			 
	  for {set i 1} {$i <= $CChunk::renHeroNum} {incr i} {

	       if {$CChunk::renHeroChunk($i,1)} {

     #****************************************************************************************#
     #											      #
     # PRMAN - chunk
     #											      #
     #****************************************************************************************#
	       
     #=========================================================================================#
		    puts $file \
"
			 Task -title {prman - $CChunk::renHeroChunk($i,2)} -subtasks {

			      Instance {ezRibEdit \[format %04d \$count\]}

			 } -cmds {

			      RemoteCmd { prman -Progress $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output/$CChunk::renHeroChunk($i,2)/$CAlfred::baseRibName.\[format %04d \$count\].rib } -service {pixarNrm} -almost 1 -tags {netrender}

			 } -cleanup \{"
     #=========================================================================================#

		    if {$CPrimeInf::delRib} {

     #=========================================================================================#
			 puts $file \
"
			      Cmd {Alfred} -msg {File delete $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output/$CChunk::renHeroChunk($i,2)/$CAlfred::baseRibName.\[format %04d \$count\].rib}"
     #=========================================================================================#

		    }

     #=========================================================================================#
		    puts $file \
"
			 \} -chaser {

			       $CAlfred::viewer $CAlfred::renDir/$CRib::mtorID/$CPrimeInf::output/$CChunk::renHeroChunk($i,2)/$CPrimeInf::shot\_$CPrimeInf::layer\_$CRib::mtorID\_$CChunk::renHeroChunk($i,2)\_$CPrimeInf::revision.\[format %04d \$count\]$Global::fileExt($CRib::dspyServer)

			 }"
     #=========================================================================================#

	       }
     
	  }

     #=========================================================================================#
	  puts $file \
" 
		    \} -cmds {

			 # prman
		    }"
     #=========================================================================================#

	  if {$CChunk::renGrndShdNum > 0} {

     #=========================================================================================#
		    puts $file \
" 
		    Task {RenGrndShd} -subtasks \{"

     #=========================================================================================#

     #****************************************************************************************#
     #											      #
     # render ground shadow 
     #											      #
     #****************************************************************************************#

	       for {set i 1} {$i <= $CChunk::renGrndShdNum} {incr i} {

		    if {$CChunk::renGrndShdChunk($i,1)} {

     #=========================================================================================#
		    puts $file \
"
			 Task -title { rndGrndShd\[format %04d \$count\] - $CChunk::renGrndShdChunk($i,5) } -subtasks {
	  
			      Instance {genRib \[format %04d \$count\]}
	  
			 } -cmds {
	  
			      RemoteCmd {y:/usr/local/bin/X_RENGRNDSHD.BAT $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.\[format %04d \$count\]_o.rib $CChunk::renGrndShdChunk($i,2) $CChunk::renGrndShdChunk($i,3) $CChunk::renGrndShdChunk($i,4) $CAlfred::renDir/$CRib::mtorID/$CPrimeInf::output/$CChunk::renGrndShdChunk($i,5)/ renGrndShd_$CChunk::renGrndShdChunk($i,5)\_\[format %04d \$count\] } -service {pixarNrm} -almost 1 -tags {netrender}
		    
			 } -chaser {
	  
			      $CAlfred::viewer $CAlfred::renDir/$CRib::mtorID/$CPrimeInf::output/$CChunk::renGrndShdChunk($i,5)/renGrndShd_$CChunk::renGrndShdChunk($i,5)\_\[format %04d \$count\]
     
			 }"
     #=========================================================================================#
			 
		    }
	       }

     #=========================================================================================#
	  puts $file \
"
		    \} -cmds {
		 
			 # RenGrndShd
		    }"
     #=========================================================================================#

	  }

     #=========================================================================================#
	  puts $file \
"
	       \} -cmds {

	       }

	  \} -cmds {

	  }
	  
     \} -cmds {
     
     }"

     #****************************************************************************************#
     #											      #
     # Make daily
     #											      #
     #****************************************************************************************#

	  if $Global::jobMode {

	       exec $CAlfred::dpsMediaUtility -Batch -F$CAlfred::dpsFileName -D$CAlfred::dpsFrameRange
	  
     #=========================================================================================#
	       puts $file \
"
     Task -title {Dps Process} -subtasks {

	  Task {make slate} -subtasks {

	       Instance {Render Process}

	  } -cmds {
	       
	       RemoteCmd {CMD /C SET NR_INCLUDE_PATH=//chunji/shakemacro/nreal/include& shake -digislatex \"$CPrimeInf::shot\" \"$CPrimeInf::show\" \"$CDps::date\" \"$CPrimeInf::layer\" \"$CRib::startFrame - $CRib::endFrame \/ $CRib::frameInterval\" \"\" \"$CDps::mm\" \"$CDps::anim\" \"$CDps::fx\" \"$CDps::td\" \"$CDps::roto\" \"$CDps::comp\" \"$CDps::coord\" \"$CDps::background\" \"$CDps::cameraMove\" \"$CDps::pixelRate\" \"$CDps::audio\" \"$CDps::cmd\" \"$CDps::notes\" -setalpha 0 -fo $CAlfred::renDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.slate.tif} -service {shake} -atmost 1

	  } -cleanup {

		Cmd {CMD /C CP $CAlfred::renDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.slate.tif $CAlfred::dpsImgName\[format %04d 0\].TIF} -tags {DPSCopy}

	  } -chaser {

		sho $CAlfred::dpsImgName\[format %04d 0\].TIF
	  }

	  Task {make image} -subtasks {

	       Iterate count -from 1 -to [expr $CAlfred::dpsFrameRange - 1] -by 1 -template {

		    Task {Tab sign \[format %04d \$count\]} -subtasks {

			 Instance {Render Process}

		    } -cmds {

			 # make tab sign
			 RemoteCmd {shake  $CAlfred::renDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.\[format %04d \[expr (\$count-1)*$CRib::frameInterval+$CRib::startFrame\]\]$Global::fileExt($CRib::dspyServer) -addtext \"$CPrimeInf::show[string toupper $CPrimeInf::shot 0 0].$CAlfred::dpsDailyNum $CPrimeInf::layer \$count\" \"arial\" 24 24 1 32 32 2 1 -setalpha 0 -fo $CAlfred::renDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.\[format %04d \[expr (\$count-1)*$CRib::frameInterval+$CRib::startFrame\]\]\_dps$Global::fileExt($CRib::dspyServer) } -service {shake} -atmost 1
			 # copy to dps
			 Cmd {CMD /C CP $CAlfred::renDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.\[format %04d \[expr (\$count-1)*$CRib::frameInterval+$CRib::startFrame\]\]\_dps$Global::fileExt($CRib::dspyServer) $CAlfred::dpsImgName\[format %04d \$count\].TIF} -tags {DPSCopy}

		    } -cleanup {

			 # delete tab sign image 
			 Cmd {Alfred} -msg {File delete $CAlfred::renDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.\[format %04d \[expr (\$count-1)*$CRib::frameInterval+$CRib::startFrame\]\]\_dps$Global::fileExt($CRib::dspyServer)}

		    } -chaser {

			 $CAlfred::viewer $CAlfred::dpsImgName\[format %04d \$count\].TIF
		    }

	       }

	  } 
     }"
     #=========================================================================================#

	  }

     #=========================================================================================#
     puts $file \
"
\} "
     #=========================================================================================#

     }

     #=========================================================================================#

     close $file
     exec alfred $CAlfred::jobDir/$CAlfred::jobScrName &

     if {$CPrimeInf::sendMail && $Global::jobMode} {
	  set mailContent \
"
show:	  $CPrimeInf::show
seq :	  $CPrimeInf::seq
shot:	  $CPrimeInf::shot
rev :	  $CPrimeInf::revision
dept:	  $CPrimeInf::dept
user:	  $CPrimeInf::layer

$CAlfred::dpsFileName"

	  foreach to $CPrimeInf::mailTo {
	       Msg $to@digiscapefx.com
	       if [catch {CAlfred::SendMessage $to@digiscapefx.com "mail.digiscapefx.com" $CAlfred::jobScrName $mailContent} msg] {
		    Msg msg
	       }
	  }
     }

}

proc WrScript_local {} { 
     CAlfred::init
     CAlfred::makeDir

     if [CAlfred::CheckUsrError] {
	  return 0
     }

     #------------------------------------------------------------------------#
     #			          COPY scene file
     #------------------------------------------------------------------------#
     if $CAlfred::copyScene {
	  if [catch {file copy -force $CPrimeInf::scene $CAlfred::sceneDir/scenes/$CAlfred::sceneName} msg] {
	       Msg msg
	  }
     }

     #------------------------------------------------------------------------#
     #
     #			           Render Process 
     #
     #------------------------------------------------------------------------#
     for {set count $CRib::startFrame} {$count <= $CRib::endFrame} {incr count} {

	  #------------------------------------------------------------------------#
	  #			           Gen Rib
	  #------------------------------------------------------------------------#
	  if $CRib::mtorDo {

	       if [catch "exec mtor_maya4 $CAlfred::mtorOption -cmd genRib $count" msg] {
		    Msg msg
	       }
	       if [catch "file rename -force $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.[format %04d $count].rib $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.[format %04d $count]_o.rib" msg] {
		    Msg msg
	       }
	  }

	  #------------------------------------------------------------------------#
	  #			            R 2 R
	  #------------------------------------------------------------------------#
	  if {![regexp -- {^\s*$} $CRib::cmd]} {

	       set CAlfred::editRib 1 
	       catch "exec y:/bin/perl.exe y:/usr/local/bin/r2r-1.07 $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.[format %04d $count]_o.rib $CRib::cmd > $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.[format %04d $count].rib"

	  }

	  #------------------------------------------------------------------------#
	  #			           Rib Eidt
	  #------------------------------------------------------------------------#
	  if {![regexp -- {^\s*$} $CAlfred::ribEditOption]} {
	       set CAlfred::editRib 1

	       # case didR2r
	       if {![regexp -- {^\s*$} $CRib::cmd]} {

		    catch "exec y:/bin/perl.exe y:/usr/local/bin/ezRibEdit $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.[format %04d $count].rib $CAlfred::ribEditOption > $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.[format %04d $count]_ribedit.rib"
		    catch "file rename -force $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.[format %04d $count]_ribedit.rib $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.[format %04d $count].rib"

	       } else {

		    #case didNotR2r 
		    catch "exec y:/bin/perl.exe y:/usr/local/bin/ezRibEdit $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.[format %04d $count]_o.rib $CAlfred::ribEditOption > $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.[format %04d $count].rib"
	       }

	  }

	  #------------------------------------------------------------------------#
	  #			       Prman - Main
	  #------------------------------------------------------------------------#
	  if $CAlfred::editRib {

	       catch "exec prman $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.[format %04d $count].rib"

	  } else {

	       catch "exec prman $CAlfred::ribDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.[format %04d $count]_o.rib"
	       
	  }

	  if {![regexp -- {^\s*$} $CRib::bgName]} {

	       regsub -all {\.####\.} $CRib::bgName "\.[format %04d $count ]\." CAlfred::bgName

	       catch "exec shake $CAlfred::renDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.[format %04d $count]$Global::fileExt($CRib::dspyServer) -over $CAlfred::bgName -fo $CAlfred::renDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.[format %04d $count]_bg$Global::fileExt($CRib::dspyServer)"
	       catch "file rename -force $CAlfred::renDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.[format %04d $count]_bg$Global::fileExt($CRib::dspyServer) $CAlfred::renDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.[format %04d $count]$Global::fileExt($CRib::dspyServer)"

	  }

	  #------------------------------------------------------------------------#
	  #			       Prman - Main
	  #------------------------------------------------------------------------#
	  catch "exec $CAlfred::viewer $CAlfred::renDir/$CRib::mtorID/$CPrimeInf::output/$CAlfred::baseRibName.[format %04d $count]$Global::fileExt($CRib::dspyServer) &"
     }
}
