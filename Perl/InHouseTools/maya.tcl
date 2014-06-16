namespace eval CMaya {
     # output
     variable ID	      "mayaRender"   ;# id
     variable camName	      "perspShape"   ;# camera name
     variable frameRange      "1 - 1 / 1"    ;# frame range
     variable imgH	      640	     ;# image width
     variable imgW	      480	     ;# image height
     variable imgAR	      1.0	     ;# image Aspect Ratio
     variable fimgH	      320	     ;# x-resolution of final image
     variable fimgW	      240	     ;# y-resolution of final image
     variable xLeftPixelBound  0	     ;# x sub-region left pixel boundary
     variable xRightPixelBound 0	     ;# x sub-region right pixel boundary
     variable yLowPixelBound   0	     ;# y sub-region low pixel boundary
     variable yHightPixelBound 0	     ;# y sub-regionn high pixel boundary
     variable imgFormat	      "tiff"	     ;# output Format
     variable startFrame      0
     variable endFrame	      0
     variable byFrame	      1

     # motion blur
     variable useMB		   0    ;# use motion blur?
     variable shutterAngle	   0.0  ;# shutter angle
     variable MBByFrame		   0.0  ;# motion blur by frame
     variable use2DMB		   0	;# use 2D motion blur
     variable 2DMBLength	   0.0  ;# 2D motion blur length
     variable 2DMBSharpness	   0.0	;# 2D motion blur sharpness
     variable 2DMBSmoothVal	   0	;# 2D motion blur smooth value
     variable 2DMBSmoothCol	   0	;# 2D motion blur smooth color
     variable keep2DMV		   0	;# keep 2D motion vector
     variable MBVisibilitySam	   0	;# num. motion blur visibility samples
     variable maxMBVisibilitySam   0	;# max motion blur visibility samples

     # ray trace
     variable enableRT		   0	     ;# enable ray tracing 
     variable maxRTReflectionLev   0	     ;# max ray-tracing reflection level
     variable maxRTRefractionLev   0	     ;# max ray-tracing refraction level
     variable maxRTShadowRayDep	   0	     ;# max ray-tracing shadow ray depth

     # quality
     variable MPFiltering	   0		  ;# multi-pixel filtering
     variable MPFilter		   "catmullrom"	  ;# multi-pixel filter
     variable SSamples		   1		  ;# shading samples per surface/pixel
     variable maxSSamples	   1		  ;# max adaptive shading samples/surface/pixel
     variable volumeSS		   1		  ;# global num. volume shading samples
     variable particleVS	   1		  ;# num. particle visibility samples
     variable antiAliasing	   "high"	  ;# anti-aliasing quality fo EAS

     # accel.
     variable TFCache		   0	;# use tessellation file cache?
     variable detectST		   0	;# detect similar tesselations?
     variable reuseRGD		   0	;# reuse render geom for depth maps?
     variable useDBB		   0	;# use displacement bounding box?
     variable abortMT		   0	;# abort on missing textures?

     # misc.
     variable verboseRender	   0	     ;# verbose rendering?
     variable gamma		   1.0	     ;# gamma
     variable createIPR		   0	     ;# create an IPR file?
     variable enableDM		   0	     ;# enable depth maps?
     variable onlyDM		   0	     ;# only generate shadow depth maps?
     variable rContrast		   0.0	     ;# red channel contrast threshold
     variable gContrast		   0.0	     ;# green channel contrast threshold
     variable bContrast		   0.0	     ;# blue channel contrast threshold
     variable pContrast		   0.125     ;# pixel coverage contrast threshold

     # Dooooooooooooooooooooooooo ###############################################
     # output
     #variable IDDo	      0   ;# id
     variable camNameDo	      0   ;# camera name
     variable frameRangeDo    0    ;# frame range
     variable imgHDo	      0	     ;# image width
     variable imgWDo	      0	     ;# image height
     variable imgARDo	      0	     ;# image Aspect Ratio
     variable fimgHDo	      0	     ;# x-resolution of final image
     variable fimgWDo	      0	     ;# y-resolution of final image
     variable xLeftPixelBoundDo  0	     ;# x sub-region left pixel boundary
     variable xRightPixelBoundDo 0	     ;# x sub-region right pixel boundary
     variable yLowPixelBoundDo   0	     ;# y sub-region low pixel boundary
     variable yHightPixelBoundDo 0	     ;# y sub-regionn high pixel boundary
     variable imgFormatDo	 0	     ;# output Format
     variable startFrameDo       0
     variable endFrameDo	 0
     variable byFrameDo	         0

     # motion blur
     variable useMBDo		   0    ;# use motion blur?
     variable shutterAngleDo	   0  ;# shutter angle
     variable MBByFrameDo	   0  ;# motion blur by frame
     variable use2DMBDo		   0	;# use 2D motion blur
     variable 2DMBLengthDo	   0  ;# 2D motion blur length
     variable 2DMBSharpnessDo	   0	;# 2D motion blur sharpness
     variable 2DMBSmoothValDo	   0	;# 2D motion blur smooth value
     variable 2DMBSmoothColDo	   0	;# 2D motion blur smooth color
     variable keep2DMVDo	     0	;# keep 2D motion vector
     variable MBVisibilitySamDo	     0	;# num. motion blur visibility samples
     variable maxMBVisibilitySamDo   0	;# max motion blur visibility samples

     # ray trace
     variable enableRTDo	     0	     ;# enable ray tracing 
     variable maxRTReflectionLevDo   0	     ;# max ray-tracing reflection level
     variable maxRTRefractionLevDo   0	     ;# max ray-tracing refraction level
     variable maxRTShadowRayDepDo    0	     ;# max ray-tracing shadow ray depth

     # quality
     variable MPFilteringDo	   0		  ;# multi-pixel filtering
     variable MPFilterDo	   0	  ;# multi-pixel filter
     variable SSamplesDo	   0		  ;# shading samples per surface/pixel
     variable maxSSamplesDo	   0		  ;# max adaptive shading samples/surface/pixel
     variable volumeSSDo	   0		  ;# global num. volume shading samples
     variable particleVSDo	   0		  ;# num. particle visibility samples
     variable antiAliasingDo	   0	  ;# anti-aliasing quality fo EAS

     # accel.
     variable TFCacheDo		   0	;# use tessellation file cache?
     variable detectSTDo	   0	;# detect similar tesselations?
     variable reuseRGDDo	   0	;# reuse render geom for depth maps?
     variable useDBBDo		   0	;# use displacement bounding box?
     variable abortMTDo		   0	;# abort on missing textures?

     # misc.
     variable verboseRenderDo	   0	     ;# verbose rendering?
     variable gammaDo		   0	     ;# gamma
     variable createIPRDo	   0	     ;# create an IPR file?
     variable enableDMDo	   0	     ;# enable depth maps?
     variable onlyDMDo		   0	     ;# only generate shadow depth maps?
     variable rContrastDo	   0	     ;# red channel contrast threshold
     variable gContrastDo	   0	     ;# green channel contrast threshold
     variable bContrastDo	   0	     ;# blue channel contrast threshold
     variable pContrastDo	   0     ;# pixel coverage contrast threshold
}

proc CMaya::init {} {
}

proc CMaya::create { w } {
	set _frame [$w insert end pgMaya -text "Maya"]
	set frame [frame $_frame.frame -relief groov -bd 1]

        NoteBook $frame.nbInf -font "arial 12 bold"

	createOutput	 $frame.nbInf
	createMB	 $frame.nbInf
	createRT	 $frame.nbInf
	createQuality    $frame.nbInf
	createAccel	 $frame.nbInf
	createMisc	 $frame.nbInf

	$frame.nbInf raise [$frame.nbInf page 0]

	pack $frame.nbInf -expand yes -fill both -padx 3 -pady 3
	pack $frame -fill both -expand yes -padx 3 -pady 3
}

proc CMaya::createOutput { w } {
     set _frOutput [$w insert end pgOutput -text "Output"]
     set frOutput [frame $_frOutput.frame -relief groov -bd 1]

     label $frOutput.laID -text "ID" -font "arial 10 bold"
     entry $frOutput.enID -width 15 -font "arial 10 bold" -textvariable CMaya::ID

     checkbutton $frOutput.laCamName -text "camera name" -font "arial 10 bold" -variable CMaya::camNameDo
     entry $frOutput.enCamName -width 15 -font "arial 10 bold" -textvariable CMaya::camName

     checkbutton $frOutput.laFrameRange -text "frame range" -font "arial 10 bold" -variable CMaya::frameRangeDo
     entry $frOutput.enFrameRange -width 15 -font "arial 10 bold" -textvariable CMaya::frameRange

     checkbutton $frOutput.laImgW -text "image Width" -font "arial 10 bold" -variable CMaya::imgWDo
     entry $frOutput.enImgW -width 10 -font "arial 10 bold" -textvariable CMaya::imgW

     checkbutton $frOutput.laImgH -text "image Height" -font "arial 10 bold" -variable CMaya::imgHDo
     entry $frOutput.enImgH -width 10 -font "arial 10 bold" -textvariable CMaya::imgH

     checkbutton $frOutput.laImgAR -text "image Aspect Ratio" -font "arial 10 bold" -variable CMaya::imgARDo
     entry $frOutput.enImgAR -width 10 -font "arial 10 bold" -textvariable CMaya::imgAR

     checkbutton $frOutput.laFImgW -text "X-resolution of final image" -font "arial 10 bold" -variable CMaya::fimgWDo
     entry $frOutput.enFImgW -width 10 -font "arial 10 bold" -textvariable CMaya::fimgW

     checkbutton $frOutput.laFImgH -text "Y-resolution of final image" -font "arial 10 bold" -variable CMaya::fimgHDo
     entry $frOutput.enFImgH -width 10 -font "arial 10 bold" -textvariable CMaya::fimgH

     checkbutton $frOutput.laXLeftPixelBound -text "X sub-region left pixel boundary" -font "arial 10 bold" -variable CMaya::xLeftPixelBoundDo
     entry $frOutput.enXLeftPixelBound -width 10 -font "arial 10 bold" -textvariable CMaya::xLeftPixelBound

     checkbutton $frOutput.laXRightPixelBound -text "X sub-region right pixel boundary" -font "arial 10 bold" -variable CMaya::xRightPixelBoundDo
     entry $frOutput.enXRightPixelBound -width 10 -font "arial 10 bold" -textvariable CMaya::xRightPixelBound

     checkbutton $frOutput.laYLowPixelBound -text "Y sub-region low pixel boundary" -font "arial 10 bold" -variable CMaya::yLowPixelBoundDo
     entry $frOutput.enYLowPixelBound -width 10 -font "arial 10 bold" -textvariable CMaya::yLowPixelBound

     checkbutton $frOutput.laYHightPixelBound -text "Y sub-region height pixel boundary" -font "arial 10 bold" -variable CMaya::yHightPixelBoundDo
     entry $frOutput.enYHightPixelBound -width 10 -font "arial 10 bold" -textvariable CMaya::yHightPixelBound

     label $frOutput.laImgFormat -text "output Format" -font "arial 10 bold"
     tk_optionMenu $frOutput.enImgFormat CMaya::imgFormat tiff iff
     $frOutput.enImgFormat configure -width 10 -font "arial 10 bold"

     #---- pack ----#
     grid columnconfigure $frOutput 0 -weight 1
     grid $frOutput.laID -row 0 -column 0 -padx 13 -pady 2 -sticky w
     grid $frOutput.enID -row 0 -column 1 -padx 10 -pady 2 -sticky e

     grid $frOutput.laCamName -row 1 -column 0 -padx 10 -pady 2 -sticky w 
     grid $frOutput.enCamName -row 1 -column 1 -padx 10 -pady 2  -sticky e

     grid $frOutput.laFrameRange -row 2 -column 0 -padx 10 -pady 2 -sticky w 
     grid $frOutput.enFrameRange -row 2 -column 1 -padx 10 -pady 2 -sticky e

     grid $frOutput.laImgW -row 3 -column 0 -padx 10 -pady 2 -sticky w 
     grid $frOutput.enImgW -row 3 -column 1 -padx 10 -pady 2  -sticky e

     grid $frOutput.laImgH -row 4 -column 0 -padx 10 -pady 2 -sticky w 
     grid $frOutput.enImgH -row 4 -column 1 -padx 10 -pady 2  -sticky e

     grid $frOutput.laImgAR -row 5 -column 0 -padx 10 -pady 2 -sticky w 
     grid $frOutput.enImgAR -row 5 -column 1 -padx 10 -pady 2  -sticky e

     grid $frOutput.laFImgW -row 6 -column 0 -padx 10 -pady 2 -sticky w 
     grid $frOutput.enFImgW -row 6 -column 1 -padx 10 -pady 2  -sticky e

     grid $frOutput.laFImgH -row 7 -column 0 -padx 10 -pady 2 -sticky w 
     grid $frOutput.enFImgH -row 7 -column 1 -padx 10 -pady 2  -sticky e

     grid $frOutput.laXLeftPixelBound -row 8 -column 0 -padx 10 -pady 2 -sticky w 
     grid $frOutput.enXLeftPixelBound -row 8 -column 1 -padx 10 -pady 2  -sticky e

     grid $frOutput.laXRightPixelBound -row 9 -column 0 -padx 10 -pady 2 -sticky w 
     grid $frOutput.enXRightPixelBound -row 9 -column 1 -padx 10 -pady 2  -sticky e

     grid $frOutput.laYLowPixelBound -row 10 -column 0 -padx 10 -pady 2 -sticky w 
     grid $frOutput.enYLowPixelBound -row 10 -column 1 -padx 10 -pady 2  -sticky e

     grid $frOutput.laYHightPixelBound -row 11 -column 0 -padx 10 -pady 2 -sticky w 
     grid $frOutput.enYHightPixelBound -row 11 -column 1 -padx 10 -pady 2  -sticky e

     grid $frOutput.laImgFormat -row 12 -column 0 -padx 13 -pady 2 -sticky w 
     grid $frOutput.enImgFormat -row 12 -column 1 -padx 8 -pady 2  -sticky e

     pack $frOutput -fill both -expand yes -padx 5 -pady 5
}

proc CMaya::createMB { w } {
     set _frMB [$w insert end pgMB -text "Motion Blur"]
     set frMB [frame $_frMB.frame -relief groov -bd 1]

     checkbutton $frMB.laUseMB -text "Use Motion Blur ?" -font "arial 10 bold" -variable CMaya::useMBDo
     checkbutton $frMB.enUseMB -width 10 -font "arial 10 bold" -variable CMaya::useMB -text "(on/off)"

     checkbutton $frMB.laShutterAngle -text "Shutter Angle (1-360)" -font "arial 10 bold" -variable CMaya::shutterAngleDo
     entry $frMB.enShutterAngle -width 10 -font "arial 10 bold" -textvariable CMaya::shutterAngle

     checkbutton $frMB.laMBByFrame -text "Motion Blur By Frame" -font "arial 10 bold" -variable CMaya::MBByFrameDo
     entry $frMB.enMBByFrame -width 10 -font "arial 10 bold" -textvariable CMaya::MBByFrame

     checkbutton $frMB.laUse2DMB -text "Use 2D Motion Blur ?" -font "arial 10 bold" -variable CMaya::use2DMBDo
     checkbutton $frMB.enUse2DMB -width 10 -font "arial 10 bold" -variable CMaya::use2DMB -text "(on/off)"

     checkbutton $frMB.la2DMBLength -text "2D Motion Blur Length" -font "arial 10 bold" -variable CMaya::2DMBLengthDo
     entry $frMB.en2DMBLength -width 10 -font "arial 10 bold" -textvariable CMaya::2DMBLength

     checkbutton $frMB.la2DMBSharpness -text "2D Motion Blur Sharpness" -font "arial 10 bold" -variable CMaya::2DMBSharpnessDo
     entry $frMB.en2DMBSharpness -width 10 -font "arial 10 bold" -textvariable CMaya::2DMBSharpness

     checkbutton $frMB.la2DMBSmoothVal -text "2D Motion Blur Smooth Value" -font "arial 10 bold" -variable CMaya::2DMBSmoothVal
     entry $frMB.en2DMBSmoothVal -width 10 -font "arial 10 bold" -textvariable CMaya::2DMBSmoothVal

     checkbutton $frMB.la2DMBSmoothCol -text "2D Motion Blur Smooth Color ?" -font "arial 10 bold" -variable CMaya::2DMBSmoothCol
     checkbutton $frMB.en2DMBSmoothCol -width 10 -font "arial 10 bold" -variable CMaya::2DMBSmoothCol -text "(on/off)"

     checkbutton $frMB.laKeep2DMV -text "Keep 2D Motion Vector ?" -font "arial 10 bold" -variable CMaya::keep2DMVDo
     checkbutton $frMB.enKeep2DMV -width 10 -font "arial 10 bold" -variable CMaya::keep2DMV -text "(on/off)"

     checkbutton $frMB.laMBVisibilitySam -text "Num. Motion Blur Visibility Samples" -font "arial 10 bold" -variable CMaya::MBVisibilitySamDo
     entry $frMB.enMBVisibilitySam -width 10 -font "arial 10 bold" -textvariable CMaya::MBVisibilitySam

     checkbutton $frMB.laMaxMBVisibilitySam -text "Max Motion Blur Visibility Samples" -font "arial 10 bold" -variable CMaya::maxMBVisibilitySamDo
     entry $frMB.enMaxMBVisibilitySam -width 10 -font "arial 10 bold" -textvariable CMaya::maxMBVisibilitySam

     #---- pack ----#
     grid columnconfigure $frMB 0 -weight 1
     grid $frMB.laUseMB -row 0 -column 0 -padx 10 -pady 2 -sticky w
     grid $frMB.enUseMB -row 0 -column 1 -padx 10 -pady 2

     grid $frMB.laShutterAngle -row 1 -column 0 -padx 10 -pady 2 -sticky w 
     grid $frMB.enShutterAngle -row 1 -column 1 -padx 10 -pady 2 

     grid $frMB.laMBByFrame -row 2 -column 0 -padx 10 -pady 2 -sticky w 
     grid $frMB.enMBByFrame -row 2 -column 1 -padx 10 -pady 2

     grid $frMB.laUse2DMB -row 3 -column 0 -padx 10 -pady 2 -sticky w 
     grid $frMB.enUse2DMB -row 3 -column 1 -padx 10 -pady 2 

     grid $frMB.la2DMBLength -row 4 -column 0 -padx 10 -pady 2 -sticky w 
     grid $frMB.en2DMBLength -row 4 -column 1 -padx 10 -pady 2 

     grid $frMB.la2DMBSharpness -row 5 -column 0 -padx 10 -pady 2 -sticky w 
     grid $frMB.en2DMBSharpness -row 5 -column 1 -padx 10 -pady 2 

     grid $frMB.la2DMBSmoothVal -row 6 -column 0 -padx 10 -pady 2 -sticky w 
     grid $frMB.en2DMBSmoothVal -row 6 -column 1 -padx 10 -pady 2 

     grid $frMB.la2DMBSmoothCol -row 7 -column 0 -padx 10 -pady 2 -sticky w 
     grid $frMB.en2DMBSmoothCol -row 7 -column 1 -padx 10 -pady 2 

     grid $frMB.laKeep2DMV -row 8 -column 0 -padx 10 -pady 2 -sticky w 
     grid $frMB.enKeep2DMV -row 8 -column 1 -padx 10 -pady 2 

     grid $frMB.laMBVisibilitySam -row 9 -column 0 -padx 10 -pady 2 -sticky w 
     grid $frMB.enMBVisibilitySam -row 9 -column 1 -padx 10 -pady 2 

     grid $frMB.laMaxMBVisibilitySam -row 10 -column 0 -padx 10 -pady 2 -sticky w 
     grid $frMB.enMaxMBVisibilitySam -row 10 -column 1 -padx 10 -pady 2 

     pack $frMB -fill both -expand yes -padx 5 -pady 5
}

proc CMaya::createRT { w } {
     set _frRT [$w insert end pgRT -text "Ray Trace"]
     set frRT [frame $_frRT.frame -relief groov -bd 1]

     checkbutton $frRT.laEnableRT -text "Enable Ray Tracing ?" -font "arial 10 bold" -variable CMaya::enableRTDo
     checkbutton $frRT.enEnableRT -width 10 -font "arial 10 bold" -variable CMaya::enableRT -text "(on/off)"

     checkbutton $frRT.laMaxRTReflectionLev -text "Max ray-tracing reflection level" -font "arial 10 bold" -variable CMaya::maxRTReflectionLevDo
     entry $frRT.enMaxRTReflectionLev -width 10 -font "arial 10 bold" -textvariable CMaya::maxRTReflectionLev

     checkbutton $frRT.laMaxRTRefractionLev -text "Max ray-tracing refraction level" -font "arial 10 bold" -variable CMaya::maxRTRefractionLevDo
     entry $frRT.enMaxRTRefractionLev -width 10 -font "arial 10 bold" -textvariable CMaya::maxRTRefractionLev

     checkbutton $frRT.laMaxRTShadowRayDep -text "Max ray-tracing shadow ray depth" -font "arial 10 bold" -variable CMaya::maxRTShadowRayDepDo
     entry $frRT.enMaxRTShadowRayDep -width 10 -font "arial 10 bold" -textvariable CMaya::maxRTShadowRayDep
     
     #----- pack -----#

     grid columnconfigure $frRT 0 -weight 1

     grid $frRT.laEnableRT -row 0 -column 0 -padx 10 -pady 2 -sticky w
     grid $frRT.enEnableRT -row 0 -column 1 -padx 10 -pady 2

     grid $frRT.laMaxRTReflectionLev -row 1 -column 0 -padx 10 -pady 2 -sticky w 
     grid $frRT.enMaxRTReflectionLev -row 1 -column 1 -padx 10 -pady 2 

     grid $frRT.laMaxRTRefractionLev -row 2 -column 0 -padx 10 -pady 2 -sticky w 
     grid $frRT.enMaxRTRefractionLev -row 2 -column 1 -padx 10 -pady 2

     grid $frRT.laMaxRTShadowRayDep -row 3 -column 0 -padx 10 -pady 2 -sticky w 
     grid $frRT.enMaxRTShadowRayDep -row 3 -column 1 -padx 10 -pady 2 

     pack $frRT -fill both -expand yes -padx 5 -pady 5
}

proc CMaya::createQuality { w } {
     set _frQuality [$w insert end pgQuality -text "Quality"]
     set frQuality [frame $_frQuality.frame -relief groov -bd 1]

     checkbutton $frQuality.laMPFiltering -text "Multi-Pixel Filtering ?" -font "arial 10 bold" -variable CMaya::MPFilteringDo
     checkbutton $frQuality.enMPFiltering -width 10 -font "arial 10 bold" -variable CMaya::MPFiltering -text "(on/off)"

     checkbutton $frQuality.laMPFilter -text "Multi-Pixel Filter" -font "arial 10 bold" -variable CMaya::MPFilterDo
     entry $frQuality.enMPFilter -width 15 -font "arial 10 bold" -textvariable CMaya::MPFilter

     checkbutton $frQuality.laSSamples -text "Shading Samples Per Surface/Pixel" -font "arial 10 bold" -variable CMaya::SSamplesDo
     entry $frQuality.enSSamples -width 10 -font "arial 10 bold" -textvariable CMaya::SSamples

     checkbutton $frQuality.laMaxSSamples -text "Max Shading Samples/Surface/Pixel" -font "arial 10 bold" -variable CMaya::maxSSamplesDo
     entry $frQuality.enMaxSSamples -width 10 -font "arial 10 bold" -textvariable CMaya::maxSSamples

     checkbutton $frQuality.laVolumeSS -text "Global Num. volume shading samples" -font "arial 10 bold" -variable CMaya::volumeSSDo
     entry $frQuality.enVolumeSS -width 10 -font "arial 10 bold" -textvariable CMaya::volumeSS

     checkbutton $frQuality.laParticleVS -text "Num. Particle Visibility Samples" -font "arial 10 bold" -variable CMaya::particleVSDo
     entry $frQuality.enParticleVS -width 10 -font "arial 10 bold" -textvariable CMaya::particleVS

     checkbutton $frQuality.laAntiAliasing -text "Anti-Aliasing quality of EAS" -font "arial 10 bold" -variable CMaya::antiAliasingDo
     entry $frQuality.enAntiAliasing -width 10 -font "arial 10 bold" -textvariable CMaya::antiAliasing

     #---- pack ----#
     grid columnconfigure $frQuality 0 -weight 1

     grid $frQuality.laMPFiltering -row 0 -column 0 -padx 10 -pady 2 -sticky w
     grid $frQuality.enMPFiltering -row 0 -column 1 -padx 10 -pady 2 -sticky e

     grid $frQuality.laMPFilter -row 1 -column 0 -padx 10 -pady 2 -sticky w 
     grid $frQuality.enMPFilter -row 1 -column 1 -padx 10 -pady 2  -sticky e

     grid $frQuality.laSSamples -row 2 -column 0 -padx 10 -pady 2 -sticky w 
     grid $frQuality.enSSamples -row 2 -column 1 -padx 10 -pady 2 -sticky e

     grid $frQuality.laMaxSSamples -row 3 -column 0 -padx 10 -pady 2 -sticky w 
     grid $frQuality.enMaxSSamples -row 3 -column 1 -padx 10 -pady 2  -sticky e

     grid $frQuality.laVolumeSS -row 4 -column 0 -padx 10 -pady 2 -sticky w 
     grid $frQuality.enVolumeSS -row 4 -column 1 -padx 10 -pady 2  -sticky e

     grid $frQuality.laParticleVS -row 5 -column 0 -padx 10 -pady 2 -sticky w 
     grid $frQuality.enParticleVS -row 5 -column 1 -padx 10 -pady 2  -sticky e

     grid $frQuality.laAntiAliasing -row 6 -column 0 -padx 10 -pady 2 -sticky w 
     grid $frQuality.enAntiAliasing -row 6 -column 1 -padx 10 -pady 2  -sticky e

     pack $frQuality -fill both -expand yes -padx 5 -pady 5
}

proc CMaya::createAccel { w } {
     set _frAccel [$w insert end pgAccel -text "Accel"]
     set frAccel [frame $_frAccel.frame -relief groov -bd 1]

     checkbutton $frAccel.laTFCache -text "Use Tessellation File Cache?" -font "arial 10 bold" -variable CMaya::TFCacheDo
     checkbutton $frAccel.enTFCache -width 10 -font "arial 10 bold" -variable CMaya::TFCache -text "(on/off)"

     checkbutton $frAccel.laDetectST -text "Detect Similar Tessellations?" -font "arial 10 bold" -variable CMaya::detectSTDo
     checkbutton $frAccel.enDetectST -width 10 -font "arial 10 bold" -variable CMaya::detectST -text "(on/off)"

     checkbutton $frAccel.laReuseRGD -text "Reuse Render Geom for Depth Maps?" -font "arial 10 bold" -variable CMaya::reuseRGDDo
     checkbutton $frAccel.enReuseRGD -width 10 -font "arial 10 bold" -variable CMaya::reuseRGD -text "(on/off)"

     checkbutton $frAccel.laUseDBB -text "Use Displacement Bounding Box?" -font "arial 10 bold" -variable CMaya::useDBBDo
     checkbutton $frAccel.enUseDBB -width 10 -font "arial 10 bold" -variable CMaya::useDBB -text "(on/off)"

     checkbutton $frAccel.laAbortMT -text "Abort on Missing Textures?" -font "arial 10 bold" -variable CMaya::abortMTDo
     checkbutton $frAccel.enAbortMT -width 10 -font "arial 10 bold" -variable CMaya::abortMT -text "(on/off)"

     #---- pack ----#
     grid columnconfigure $frAccel 0 -weight 1

     grid $frAccel.laTFCache -row 0 -column 0 -padx 10 -pady 2 -sticky w
     grid $frAccel.enTFCache -row 0 -column 1 -padx 10 -pady 2

     grid $frAccel.laDetectST -row 1 -column 0 -padx 10 -pady 2 -sticky w 
     grid $frAccel.enDetectST -row 1 -column 1 -padx 10 -pady 2 

     grid $frAccel.laReuseRGD -row 2 -column 0 -padx 10 -pady 2 -sticky w 
     grid $frAccel.enReuseRGD -row 2 -column 1 -padx 10 -pady 2

     grid $frAccel.laUseDBB -row 3 -column 0 -padx 10 -pady 2 -sticky w 
     grid $frAccel.enUseDBB -row 3 -column 1 -padx 10 -pady 2 

     grid $frAccel.laAbortMT -row 4 -column 0 -padx 10 -pady 2 -sticky w 
     grid $frAccel.enAbortMT -row 4 -column 1 -padx 10 -pady 2 

     pack $frAccel -fill both -expand yes -padx 5 -pady 5
}

proc CMaya::createMisc { w } {
     set _frMisc [$w insert end pgMisc -text "Misc."]
     set frMisc [frame $_frMisc.frame -relief groov -bd 1]

     checkbutton $frMisc.laVerboseRender -text "Verbose Rendering ?" -font "arial 10 bold" -variable CMaya::verboseRenderDo
     checkbutton $frMisc.enVerboseRender -width 10 -font "arial 10 bold" -variable CMaya::verboseRender -text "(on/off)"

     checkbutton $frMisc.laGamma -text "Gamma" -font "arial 10 bold" -variable CMaya::gammaDo
     entry $frMisc.enGamma -width 10 -font "arial 10 bold" -textvariable CMaya::gamma

     checkbutton $frMisc.laCreateIPR -text "Create an IPR file ?" -font "arial 10 bold" -variable CMaya::createIPRDo
     checkbutton $frMisc.enCreateIPR -width 10 -font "arial 10 bold" -variable CMaya::createIPR -text "(on/off)"

     checkbutton $frMisc.laEnableDM -text "Enable Depth Maps ?" -font "arial 10 bold" -variable CMaya::enableDMDo
     checkbutton $frMisc.enEnableDM -width 10 -font "arial 10 bold" -variable CMaya::enableDM -text "(on/off)"

     checkbutton $frMisc.laOnlyDM -text "Only Generate Shadow Depth Maps ?" -font "arial 10 bold" -variable CMaya::onlyDMDo
     checkbutton $frMisc.enOnlyDM -width 10 -font "arial 10 bold" -variable CMaya::onlyDM -text "(on/off)"

     checkbutton $frMisc.laRContrast -text "red channel contrast threshold" -font "arial 10 bold" -variable CMaya::rContrastDo
     entry $frMisc.enRContrast -width 10 -font "arial 10 bold" -textvariable CMaya::rContrast

     checkbutton $frMisc.laGContrast -text "green channel contrast threshold" -font "arial 10 bold" -variable CMaya::gContrastDo
     entry $frMisc.enGContrast -width 10 -font "arial 10 bold" -textvariable CMaya::gContrast

     checkbutton $frMisc.laBContrast -text "blue channel contrast threshold" -font "arial 10 bold" -variable CMaya::bContrastDo
     entry $frMisc.enBContrast -width 10 -font "arial 10 bold" -textvariable CMaya::bContrast

     checkbutton $frMisc.laPContrast -text "pixel coverage contrast threshold" -font "arial 10 bold" -variable CMaya::pContrastDo
     entry $frMisc.enPContrast -width 10 -font "arial 10 bold" -textvariable CMaya::pContrast

     #---- pack ----#
     grid columnconfigure $frMisc 0 -weight 1

     grid $frMisc.laVerboseRender -row 0 -column 0 -padx 10 -pady 2 -sticky w
     grid $frMisc.enVerboseRender -row 0 -column 1 -padx 10 -pady 2

     grid $frMisc.laGamma -row 1 -column 0 -padx 10 -pady 2 -sticky w 
     grid $frMisc.enGamma -row 1 -column 1 -padx 10 -pady 2 

     grid $frMisc.laCreateIPR -row 2 -column 0 -padx 10 -pady 2 -sticky w 
     grid $frMisc.enCreateIPR -row 2 -column 1 -padx 10 -pady 2

     grid $frMisc.laEnableDM -row 3 -column 0 -padx 10 -pady 2 -sticky w 
     grid $frMisc.enEnableDM -row 3 -column 1 -padx 10 -pady 2 

     grid $frMisc.laOnlyDM -row 4 -column 0 -padx 10 -pady 2 -sticky w 
     grid $frMisc.enOnlyDM -row 4 -column 1 -padx 10 -pady 2 

     grid $frMisc.laRContrast -row 5 -column 0 -padx 10 -pady 2 -sticky w 
     grid $frMisc.enRContrast -row 5 -column 1 -padx 10 -pady 2 

     grid $frMisc.laGContrast -row 6 -column 0 -padx 10 -pady 2 -sticky w 
     grid $frMisc.enGContrast -row 6 -column 1 -padx 10 -pady 2 
     
     grid $frMisc.laBContrast -row 7 -column 0 -padx 10 -pady 2 -sticky w 
     grid $frMisc.enBContrast -row 7 -column 1 -padx 10 -pady 2 

     grid $frMisc.laPContrast -row 8 -column 0 -padx 10 -pady 2 -sticky w 
     grid $frMisc.enPContrast -row 8 -column 1 -padx 10 -pady 2 

     pack $frMisc -fill both -expand yes -padx 5 -pady 5
}
