namespace eval CChunk {
	variable renHeroNum 	""
	variable renGrndShdNum 	""	
	variable ezCompNum 		""
	variable selectedChunk		""
	variable frame
	
	namespace eval CRenHero {
	}
	
	namespace eval CRenGrndShd {
	}
	
	namespace eval CEzComp {
	}
	
	set renHeroChunk() ""
	set renGrndShdChunk() ""
	set ezCompChunk() ""
}

set CChunk::renHeroNum [expr $CChunk::renHeroNum+1]
set CChunk::renHeroChunk($CChunk::renHeroNum,1) 0 		# Do
set CChunk::renHeroChunk($CChunk::renHeroNum,2) ""		# Id
set CChunk::renHeroChunk($CChunk::renHeroNum,3) only 	# only | except
set CChunk::renHeroChunk($CChunk::renHeroNum,4) 		# only | except object

set CChunk::renGrndShdChunk($CChunk::renGrndShdNum,1) 	# Do
set CChunk::renGrndShdChunk($CChunk::renGrndShdNum,2) 	# light
set CChunk::renGrndShdChunk($CChunk::renGrndShdNum,3) 	# source
set CChunk::renGrndShdChunk($CChunk::renGrndShdNum,4) 	# target
set CChunk::renGrndShdChunk($CChunk::renGrndShdNum,5) 	# Id

namespace eval CDps { 
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

namespace eval CPrimeInf {
	variable scene 			""
    variable cmd 			""
	variable llOption		""
	
    variable show 			"" 
    variable seq 			""
    variable shot 			""
    variable dept 			""
    variable layer 			""
    variable revision 		""
    variable dspyServer 	""
    variable dspyMode 		""
    variable sendMail	 	""
    variable mailTo 		""
    variable crew 			""
    variable mailSubject 	""
    variable mailContent 	""
    variable render 		""
	variable bgName			""
}

namespace eval CRib {
	variable mtorDo 			""
	variable cameraName 		""
   	variable mtorID 			""
   	variable startFrame 		""
   	variable endFrame 			""
    variable frameInterval 		""

    variable renHeroDo 			""
    variable renHeroID 			""
    variable renHeroObj 		""
    variable renHeroOnlyExcept 	""
    
    variable renGrndShdDo 		""
    variable renGrndShdID 		""
    variable renGrndShdLight 	""
    variable renGrndShdSrc 		""
    variable renGrndShdTarget 	""
} 

namespace eval Global {
	variable patchLevel "3.00"
	variable jobMode 	"render only"
	variable scenePath 	""
	variable cmdPath 	""
}
