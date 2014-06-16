/*
 * ERModule.xs
 *	1. Clean up unused example codes.
 */
/*
	2007/06/28 (KST)
	김광호 (Gwang-Ho Kim, iccentric@gmail.com)

	HV	*h = newHV();
	AV	*a = newAV();
	// 어느 경우든 SvOK에는 실패한다.
	// 대신 hv_iterinit(h)나 av_len(a)의 결과는 각각 0, -1로서
	// 아직 초기화되지 않았음을 말해준다.

// New features;
//	Major:
//	1. getLineByRIBDirectiveSub.pl: getLineByRIBDirective function returns 4 values instead of 3 values;
//		1st: ER__GET_RIB_DIRECTIVE__XXX enumerator	(Instead of number)
//			ER__GET_RIB_DIRECTIVE__ERROR		-2 (NEW)
//			ER__GET_RIB_DIRECTIVE__UNKNOWN		-1
//			ER__GET_RIB_DIRECTIVE__EOF		0
//			ER__GET_RIB_DIRECTIVE__VALID		1
//			ER__GET_RIB_DIRECTIVE__EXCEPTION	2
//		2st: ER__ENUM_RIB_DIRECTIVE__XXX enumerator	(NEW!)
//		3rd: RIB Directive				(The same)
//		4th: referernce to RIB Directive's content	(The same)
//	2. ERCore.pl,ERBaseSub.pl: Seperate Pre-"Display" analyzing part, put it into ERBase.pl
//	with a name "ERBasePreDisplay".
//	3. Check variables with "use strict";
//	All the variables including constants are defined in ERModule module,
//	so ERConstant.pl is replaced by "ERLib/ERConstant.h".
//
//	Minor:
//	1. ERCore.pl: Does not treat "Displayment" RIB Directive any longer;
//	Use "Surface" RIB Directive instead of.
//	2. ERBaseSub.pl: ERBaseShader does not take an argument $shaderType any longer;
//	It can be set the global variable via $enumRIBDirective.
//	3. The enumerator for $fate is renamed;
//		CONTENT_DELETE		CONTENT_FATE__DELETE
//		CONTENT_PRINT		CONTENT_FATE__PRINT
//		CONTENT_SAVE		CONTENT_FATE__SAVE
// Fix bugs;
//	1. ERCore.pl: Doest not close the original RIB file.
//	2. ERBaseSub.pl: Cannot make a path for "Bake Pass" and "Final Pass" in case of SSSC.
//	3. editRIB.pl: Incorrect path when ROOT path with "-R" option is given without last '/'.
 */
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "ppport.h"

#include "ERLib/ERConstant.h"
#include "ERLib/ERCore.h"

#include "const-c.inc"

MODULE = ERModule		PACKAGE = ERModule		

INCLUDE: const-xs.inc

void
main()
INIT:
	int	iReturnVal;
	ERCore	Core(&iReturnVal);
	if (iReturnVal != 0) {
		(void) fprintf(stderr, "XSUB: ERCore::ERCore(): Failure\n");
		return;
	}
CODE:
	// 사용되는 변수들을 초기화한다.
	Core.mf__setVar("gKeyChunk", newSV(0), ER__VAR_SCOPE__GLOBAL);
	Core.mf__setVar("aElementName", newSV(0), ER__VAR_SCOPE__ALL);
	Core.mf__setVar("jcObjectName", newSV(0), ER__VAR_SCOPE__JOB_CHUNK);
	Core.mf__setVar("jcObjectNameStackIndex", newSV(0), ER__VAR_SCOPE__JOB_CHUNK);
	Core.mf__setVar("gDeclare", newSViv(ER__DECLARE__NO_FRAME), ER__VAR_SCOPE__GLOBAL);
	Core.mf__setgDeclare(ER__DECLARE__NO_FRAME);
	Core.mf__setstrDeclare(newSVpv("", 0));

	// 커맨드 라인상에서 설정된 변수를 읽어들인다.
	// 1. 플래그가 설정되어 있는 지 알아본다.
	bool	bAscii = SvIV(Core.mf__getCommandLineOption("bAscii"));
//	bool	bVerbosePath = SvIV(Core.mf__getCommandLineOption("bVerbosePath"));
	// 2. RIB 파일로부터 필요한 정보를 설정한다.
	SV	*ribFile = Core.mf__getCommandLineOption("ribFile");
{
	AV	*avResult = ERCore::mf__re(SvPV_nolen(ribFile), "(.*?)(([^\\/\\.]+)(\\..*?(\\d+)\\.rib))$");
//	printf("# of element(s) = %d\n", av_len(avResult));
	if (av_len(avResult) == -1)
		croak("XSUB: Cannot find frame # from \"%s\"\n", SvPV_nolen(ribFile));
	for (I32 i = 0; i <= av_len(avResult); ++i) {
		SV	**svElement = av_fetch(avResult, i, 0);
		if (!svElement || !SvOK(*svElement))
			croak("XSUB: av_fetch: Failure\n");
		if (i == 0) {
//			$path = './' unless $path;	# "-d"옵션으로 절대경로를 지정하지 않도록 하기 위해서
			STRLEN	pathLength;
			SvPV(*svElement, pathLength);
			if (!pathLength)
				sv_catpv(*svElement, "./");
//			(void) printf("Path: |%s|\n", SvPV_nolen(*svElement));
			Core.mf__setpath(*svElement);
		} else if (i == 2)
			Core.mf__setdspyName(*svElement);
		else if (i == 3)
			Core.mf__setdspyName0(*svElement);
		else if (i == 4)
			Core.mf__setVar("gFrameNumber", newSVsv(*svElement), ER__VAR_SCOPE__GLOBAL);
//		(void) printf("a[%d]:= |%s|\n", i, SvPV_nolen(*svElement));
	}
}

	// RIB 파일을 연다.
	SV	*ribFileHandle = newSVpv("RIBFH", 5);
	if (!ribFileHandle || !SvOK(ribFileHandle))
		croak("XSUB: newSVpv('RIBFH'): Failure\n");
	SV	*fileOpenExpr = newSVpvf("%s %s", bAscii ? "<" : "catrib", SvPV_nolen(ribFile));
	if (!fileOpenExpr || !SvOK(fileOpenExpr))
		croak("XSUB: newSVpvf( $ribFile ): Failure\n");
	if (Core.mf__ERCoreSub_OpenWrapper(ribFileHandle, fileOpenExpr) != 0)
		croak("XSUB: ERCore::mf__ERCoreSub_OpenWrapper(\"%s\"): Failure\n", SvPV_nolen(ribFile));
	SvREFCNT_dec(fileOpenExpr);

	// 파일 이름에 사용한, 추가적인 경로 정보(rootPathEvalExpr, addPathEvalExpr)을 얻는다.
	SV	*rootPathEvalExpr0 = Core.mf__getCommandLineOption("rootPathExpr");
	STRLEN	rootPathEvalExprLength = SvLEN(rootPathEvalExpr0);
	SV	*addPathEvalExpr0 = Core.mf__getCommandLineOption("addPathExpr");

	// 각 "job chunk"에 대해서 필요한 변수를 설정하고,
	// 각 "job chunk"의 결과를 저장할 RIB 파일을 연다.
	HV	*HoHChunk = Core.mf__getHoHChunk();
	(void) hv_iterinit(HoHChunk);		// # of keys ("job chunk")
	HE	*pHE;
	while ((pHE = hv_iternext(HoHChunk))) {
		// "job chunk"의 이름을 얻는다.
		I32	keyLength;
		char	*keyChunk = hv_iterkey(pHE, &keyLength);
		if (!keyChunk)
			croak("XSUB: Error: hv_iterkey: Oops\n");
//		printf("Key:= |%s|(%d)\n", keyChunk, keyLength);
		// 현재 처리하고 있는 "job chunk"의 이름을 기억한다.
		// 여기서 호출하는 어떤 곳에서 전역변수를 통해 알 수 있도록.
		Core.mf__setVar("gKeyChunk", newSVpvf("%s", keyChunk), ER__VAR_SCOPE__GLOBAL);

		// 각 "job chunk"의 정보를 담은 해쉬구조를 얻는다.
		SV	*hChunkRef = hv_iterval(HoHChunk, pHE);
		if (!hChunkRef || !SvOK(hChunkRef) || SvTYPE(SvRV(hChunkRef)) != SVt_PVHV)
			croak("XSUB: Error: hv_iterval: Oops\n");
//		printf("Val:= |%s|\n", SvPV_nolen(hChunkRef));
		HV	*hChunk = (HV *) SvRV(hChunkRef);	// Hash

		// "type"정보로부터 Bake모드가 설정되었는 지 알아본다.
		SV	**svTypeChunk = hv_fetch(hChunk, "type", 4, 0);
		if (!svTypeChunk)
			croak("XSUB: Error: hv_fetch: Oops\n");
		int	iTypeChunk = SvIV(*svTypeChunk);
		bool	bBake = false;
		if (iTypeChunk & ER__CHUNK_TYPE__BAKE)
			bBake = true;
{
		// Bake모드 정보를 저장한다.
		SV	*bakeValue = newSViv(bBake);
		SV	**svbBake = hv_store(hChunk, "bBake", 5, bakeValue, 0);
		if (!svbBake || !*svbBake || !SvOK(*svbBake))
			croak("XSUB: Error: hv_store: Oops\n");
}
		if (iTypeChunk & ER__CHUNK_TYPE__BAKE) {	// "type"에 Bake모드 정보가 있었다면
			// 이미 그 정보는 기억했으므로 "type"에서 Bake모드 정보를 없앤다.
			iTypeChunk &= ~ER__CHUNK_TYPE__BAKE;
			SV	*svTypeChunkWithoutBake = newSViv(iTypeChunk);
			SV	**svTypeChunkTmp = hv_store(hChunk, "type", 4, svTypeChunkWithoutBake, 0);
			if (!svTypeChunkTmp || !*svTypeChunkTmp || !SvOK(*svTypeChunkTmp))
				croak("XSUB: Error: hv_store: Oops\n");
		}

		// 경로표현의 해석에 사용되는 변수("$keyChunk")를 설정한다.
		SV	*evalExpr = newSVpvf("$keyChunk = \"%s\";", keyChunk);
		eval_sv(evalExpr, G_DISCARD | G_NOARGS);
		SvREFCNT_dec(evalExpr);
		// -R로 주어진 루트 경로를 해석한다.
		SV	*rootPath = newSVpv("", 0);
		if (rootPathEvalExprLength) {
			SV	*rootPathEvalExpr = newSVpv("qq|", 3);
			sv_catpvf(rootPathEvalExpr, "%s/|", SvPV_nolen(rootPathEvalExpr0));
			rootPath = eval_pv(SvPV_nolen(rootPathEvalExpr), TRUE);
			sv_catpv(rootPath, "/rib/");
		}
		// -d로 주어진 추가 경로를 해석한다.
		SV	*addPathEvalExpr = newSVpv("qq|", 3);
		sv_catpvf(addPathEvalExpr, "%s|", SvPV_nolen(addPathEvalExpr0));
		SV	*addPath = eval_pv(SvPV_nolen(addPathEvalExpr), TRUE);
		SvREFCNT_dec(addPathEvalExpr);

		// 해당 "job chunk"을 결과를 저정할 파일이름을 결정한다.
		// 필요하면 경로를 만든다.
		SV	*newUNIXPath = rootPathEvalExprLength ? rootPath : newSVsv(Core.mf__getpath());
		sv_catsv(newUNIXPath, addPath);
		SV	*jcPathname = ERCore::mf__mkdir2Wrapper(newUNIXPath);
//		(void) printf("RibPath: |%s|\n", SvPV_nolen(jcPathname));
		SvREFCNT_dec(newUNIXPath);
		sv_catpvf(jcPathname, "%s.%s%s%s", SvPV_nolen(Core.mf__getdspyName()), keyChunk, bBake ? ".ShadowPass" : "", SvPV_nolen(Core.mf__getdspyName0()));

		// "job chunk"의 결과를 저장할 RIB 파일을 연다.
		SV	*jcRibFileHandle = newSVpvf("fh%s", keyChunk);
		if (!jcRibFileHandle || !SvOK(jcRibFileHandle))
			croak("XSUB: newSVpv(\"%s\"): Failure\n", SvPV_nolen(jcRibFileHandle));
		SV	*jcRibFileOpenExpr = newSVpvf("%s %s", bAscii ? ">" : "| catrib -binary -gzip -o", SvPV_nolen(jcPathname));
		SvREFCNT_dec(jcPathname);
		if (!jcRibFileOpenExpr || !SvOK(jcRibFileOpenExpr))
			croak("XSUB: newSVpvf( $ribFile ): Failure\n");
		if (Core.mf__ERCoreSub_OpenWrapper(jcRibFileHandle, jcRibFileOpenExpr) != 0)
			croak("XSUB: ERCore::mf__ERCoreSub_OpenWrapper(\"%s\"): Failure\n", SvPV_nolen(jcPathname));
		SvREFCNT_dec(jcRibFileOpenExpr);

		// 파일핸들에 대한 정보를 저장하고,
		// 몇 개의, 각 "job chunk"마다의 변수를 초기화한다.
		SV	**hvStoreOk;
		hvStoreOk = hv_store(hChunk, "file handle", 11, jcRibFileHandle, 0);
		if (!hvStoreOk || !*hvStoreOk || !SvOK(*hvStoreOk))
			croak("XSUB: Error: hv_store($hChunk{ \"stack index\" }): Failure\n");
		hvStoreOk = hv_store(hChunk, "stack index", 11, newSViv(-1), 0);
		if (!hvStoreOk || !*hvStoreOk || !SvOK(*hvStoreOk))
			croak("XSUB: Error: hv_store($hChunk{ \"stack index\" }): Failure\n");
		hvStoreOk = hv_store(hChunk, "stack array", 11, newRV((SV *) newAV()), 0);
		if (!hvStoreOk || !*hvStoreOk || !SvOK(*hvStoreOk))
			croak("XSUB: Error: hv_store($hChunk{ \"stack array\" }): Failure\n");
		hvStoreOk = hv_store(hChunk, "declare written", 15, newSViv(0), 0);
		if (!hvStoreOk || !*hvStoreOk || !SvOK(*hvStoreOk))
			croak("XSUB: Error: hv_store($hChunk{ \"stack index\" }): Failure\n");
		hvStoreOk = hv_store(hChunk, "light source", 12, newRV((SV *) newHV()), 0);
		if (!hvStoreOk || !*hvStoreOk || !SvOK(*hvStoreOk))
			croak("XSUB: Error: hv_store($hChunk{ \"stack array\" }): Failure\n");
	}

	// 원본 RIB을 RIB Directive단위로 한 줄씩 읽는다.
	// 몇몇 RIB Directive의 경우에는 각 "job chunk"에서 그 내용이 바뀌므로,
	// 다른 "job chunk"에서 사용할 수 있도록 그 내용을 따로 저장해야 한다.
	// Create aSavedRIBDirectiveContent.
	AV	*aSavedRIBDirectiveContent = newAV();
	int	enumSavedRIBDirective[] = {
		ER__ENUM_RIB_DIRECTIVE__ATTRIBUTE,
		ER__ENUM_RIB_DIRECTIVE__WORLD_BEGIN,
		ER__ENUM_RIB_DIRECTIVE__WORLD_END,
		ER__ENUM_RIB_DIRECTIVE__DISPLAY,
		ER__ENUM_RIB_DIRECTIVE__LIGHTSOURCE,
		ER__ENUM_RIB_DIRECTIVE__SURFACE,
		ER__ENUM_RIB_DIRECTIVE__UNDEF
	};
	int	*iterSavedEnumRIBDirective = enumSavedRIBDirective;
	while (*iterSavedEnumRIBDirective != ER__ENUM_RIB_DIRECTIVE__UNDEF) {
		SV	**avStoreOk = av_store(aSavedRIBDirectiveContent, *iterSavedEnumRIBDirective, newRV((SV *) newAV()));
		if (!avStoreOk || !SvOK(*avStoreOk))
			croak("XSUB: av_store( $aSavedRIBDirectiveContent[ %d ] ): Failure\n", *iterSavedEnumRIBDirective);

		++iterSavedEnumRIBDirective;
	}
#if 0
	for (I32 i = 0; i <= av_len(aSavedRIBDirectiveContent); ++i) {
		SV	**aref = av_fetch(aSavedRIBDirectiveContent, i, 0);
		if (!aref || !*aref || !SvOK(*aref))
			printf("aSavedRIBDirectiveContent[%d]: undef\n", i);
		else
			printf("aSavedRIBDirectiveContent[%d]: (%p->%p)%s|%s|\n", i, aref, *aref, SvPV_nolen(*aref), SvPV_nolen(Core.mf__ERCoreSub_joinWrapper(*aref)));
	}
#endif
	int	enumRIBDirective	= ER__ENUM_RIB_DIRECTIVE__UNDEF;
	SV	*RIBDirective		= 0;
	SV	*RIBDirectiveContentRef	= 0;
	while (1) {
		// 한 줄씩 읽는다.
		int	iReadReturnVal = Core.mf__Read(ribFileHandle, enumRIBDirective, &RIBDirective, &RIBDirectiveContentRef);
		if (iReadReturnVal == ER__GET_RIB_DIRECTIVE__ERROR || iReadReturnVal == ER__GET_RIB_DIRECTIVE__EOF)
			break;

		if (iReadReturnVal == ER__GET_RIB_DIRECTIVE__UNKNOWN) {
			SV	*line = Core.mf__ERCoreSub_joinWrapper(RIBDirectiveContentRef);
			if (!line)
				croak("XSUB: Error: ERCore::mf__ERCoreSub_joinWrapper(): Failure\n");
			(void) fprintf(stderr, "ERITRIB: Warning: Unknown RIB Directive: Ignored;\n|%s|\n", SvPV_nolen(line));
			continue;
		}

		// Declare부분을 따로 저장한다.
		if (enumRIBDirective == ER__ENUM_RIB_DIRECTIVE__FRAME_BEGIN) {
			if (Core.mf__getgDeclare() == ER__DECLARE__NO_FRAME) {
				Core.mf__setVar("gStrDeclare", newSVsv(Core.mf__getstrDeclare()), ER__VAR_SCOPE__GLOBAL);
			}
			Core.mf__setVar("gDeclare", newSViv(ER__DECLARE__FRAME_BLOCK), ER__VAR_SCOPE__GLOBAL);
			Core.mf__setgDeclare(ER__DECLARE__FRAME_BLOCK);
		} else if (enumRIBDirective == ER__ENUM_RIB_DIRECTIVE__FRAME_END) {
			Core.mf__setVar("gDeclare", newSViv(ER__DECLARE__FRAME_BLOCK_OUTSIDE), ER__VAR_SCOPE__GLOBAL);
		} else {
			if (Core.mf__getgDeclare() == ER__DECLARE__NO_FRAME) {
				sv_catsv(Core.mf__getstrDeclare(), Core.mf__ERCoreSub_joinWrapper(RIBDirectiveContentRef));
				continue;
			}
		}

		// 다른 함수에서 참고할 수 있도록 (enumRIBDirective, RIBDirective, RIBDirectiveContentRef)를 저장한다.
		Core.mf__setVar("gEnumRIBDirective", newSViv(enumRIBDirective), ER__VAR_SCOPE__GLOBAL);
		Core.mf__setVar("gRIBDirective", newSVsv(RIBDirective), ER__VAR_SCOPE__GLOBAL);
		Core.mf__setVar("gRIBDirectiveContentRef", newSVsv(RIBDirectiveContentRef), ER__VAR_SCOPE__GLOBAL);

		// "Display"의 경우, 그 내용을 해석하여 필요한 정보를 설정한다.
		if (enumRIBDirective == ER__ENUM_RIB_DIRECTIVE__DISPLAY) {
			eval_pv("ERBasePreDisplay()", TRUE);
		}

		// 각 "job chunk"마다 같은 내용을 가지고 처리하도록
		// 읽은 내용을 따로 저장한다.
		if (enumRIBDirective >= 0) {
			// iReadReturnVal = ER__GET_RIB_DIRECTIVE__EXCEPTION일 수도 있다.
			// 이 경우, RIB Directive가 정의되지 않는다.
			SV	**oldArrayRef = av_fetch(aSavedRIBDirectiveContent, enumRIBDirective, 0);
			if (oldArrayRef) {
				AV	*newArray = Core.mf__copy(RIBDirectiveContentRef);
				SV	**avStoreOk = av_store(aSavedRIBDirectiveContent, enumRIBDirective, newRV((SV *) newArray));
				if (!avStoreOk || !*avStoreOk)
					croak("XSUB: av_store( aSavedRIBDirectiveContent[ %d ] ): Failure\n", enumRIBDirective);
/*
				for (I32 i = 0; i <= av_len(aSavedRIBDirectiveContent); ++i) {
					SV	**aref = av_fetch(aSavedRIBDirectiveContent, i, 0);
					if (!aref || !*aref || !SvOK(*aref))
						printf("aSavedRIBDirectiveContent[%d]: undef\n", i);
					else
						printf("aSavedRIBDirectiveContent[%d]: (%p->%p)%s|%s|\n", i, aref, *aref, SvPV_nolen(*aref), SvPV_nolen(Core.mf__ERCoreSub_joinWrapper(*aref)));
				}
 */
			}
		}

		// 읽은 내용을 가지고 각 "job chunk"마다 적절한 작업을 한다.
		HV	*HoHChunk = Core.mf__getHoHChunk();
		(void) hv_iterinit(HoHChunk);		// # of keys ("job chunk")
		HE	*pHE;
		while ((pHE = hv_iternext(HoHChunk))) {
			// "job chunk"을 이름을 얻는다.
			I32	keyLength;
			char	*keyChunk = hv_iterkey(pHE, &keyLength);
			if (!keyChunk)
				croak("XSUB: Error: hv_iterkey: Oops\n");
//			printf("Key:= |%s|(%d)\n", keyChunk, keyLength);
			// 다른 함수에서 참고할 수 있도록 "job chunk"이름을 저장한다.
			Core.mf__setVar("gKeyChunk", newSVpvf("%s", keyChunk), ER__VAR_SCOPE__GLOBAL);

			// 몇몇 RIB Directive의 경우 항상 원본의 내용을 가지고 처리한다.
			if (enumRIBDirective >= 0) {
				// iReadReturnVal = ER__GET_RIB_DIRECTIVE__EXCEPTION일 수도 있다.
				// 이 경우, RIB Directive가 정의되지 않는다.
				SV	**oldArrayRef = av_fetch(aSavedRIBDirectiveContent, enumRIBDirective, 0);
				if (oldArrayRef) {
					AV	*newArray = Core.mf__copy(*oldArrayRef);
					RIBDirectiveContentRef = newRV((SV *) newArray);
					Core.mf__setVar("gRIBDirectiveContentRef", newSVsv(RIBDirectiveContentRef), ER__VAR_SCOPE__GLOBAL);
				}

			}
			if (enumRIBDirective == ER__ENUM_RIB_DIRECTIVE__GEOMETRIC_PRIMITIVE) {
				ERCore::mf__restoreArrayRefWrapper(RIBDirectiveContentRef);
			}

			// 필요한 변수를 설정한다.
			//	hChunk
			//	FH, SUBFH, BAKEFH
			//	stackArray, stackIndex
			//	hStack
			//	fate
			//	bPrintToFH
			//	jcObjectNameStackIndex
			HV	*hChunk = 0;
{
			SV	*hChunkRef = hv_iterval(HoHChunk, pHE);
			if (!hChunkRef || !SvOK(hChunkRef) || SvTYPE(SvRV(hChunkRef)) != SVt_PVHV)
				croak("XSUB: Error: hv_iterval: Oops\n");
//			printf("Val:= |%s|\n", SvPV_nolen(hChunkRef));
			hChunk = (HV *) SvRV(hChunkRef);	// Hash
}
			SV	**FH = hv_fetch(hChunk, "file handle", 11, 0);
			if (!FH)
				croak("XSUB: hv_fetch( $hChunkRef -> { 'FH' } ): Failure\n");
			SV	**SUBFH = hv_fetch(hChunk, "subframe file handle", 20, 0);
			// 아직 설정되지 않았을 수도 있다!
			SV	**BAKEFH = hv_fetch(hChunk, "bake subframe file handle", 25, 0);
			// 아직 설정되지 않았을 수도 있다!
			AV	*stackArray = 0;
{
			SV	**AoHStackRef = hv_fetch(hChunk, "stack array", 11, 0);
			if (!AoHStackRef)
				croak("XSUB: hv_fetch( $hChunkRef -> { 'stack array' } ): Failure\n");
			stackArray = (AV *) SvRV(*AoHStackRef);
}
			int	stackIndex;
{
			SV	**svStackIndex = hv_fetch(hChunk, "stack index", 11, 0);
			if (!svStackIndex)
				croak("XSUB: hv_fetch( $hChunkRef -> { 'stack index' } ): Failure\n");
			stackIndex = SvIV(*svStackIndex);
}
			HV	*hStack = 0;
{
			SV	**hStackRef = av_fetch(stackArray, stackIndex, 0);
			// 아직 "스택"이 형성되지 않았을 수도 있다!
			if (hStackRef)
				hStack = (HV *) SvRV(*hStackRef);
}
			IV	fate = ER__CONTENT_FATE__UNDEF;
{
			if (hStack) {
				SV	**svFate = hv_fetch(hStack, "fate", 4, 0);
				if (!svFate)
					croak("XSUB: hv_fetch( $hStackRef -> { 'fate' } ): Failure\n");
				fate = SvIV(*svFate);
			}
}
			bool	bPrintToFH = false;	// FH에 쓸 수 있는 지를 말해준다.
{
			SV	**bBake = hv_fetch(hChunk, "bBake", 5, 0);
			if (!bBake)
				croak("XSUB: hv_fetch( $hChunkRef -> { 'bBake' } ): Failure\n");
			if (SvIV(*bBake)) {	// Bake모드에서는
				// Shadow Pass인 경우에만 쓴다.
//	my	$aElementName = $hChunkRef -> { 'aElementName' };
				SV	**aElementNameRef = hv_fetch(hChunk, "aElementName", 12, 0);
				// 아직 설정되지 않았을 수도 있다!
				if (aElementNameRef && SvOK(*aElementNameRef)) {	// 이름이 정해진 경우에 한해서
					if (strcmp(SvPV_nolen(*aElementNameRef), "Image")) {
						// "Image"즉 Final Rendering Pass가 아닌 경우
						bPrintToFH = true;
					}
				}
			} else {		// Bake모드가 아닐 때는
				// 무조건 쓴다.
				bPrintToFH = true;
			}
}
			// 아직 설정되지 않았을 수도 있다!
			SV	**jcObjectNameStackIndex = hv_fetch(hChunk, "jcObjectNameStackIndex", 22, 0);

			// 예외적인 경우에는 읽은 그대로 쓴다.
			if (iReadReturnVal == ER__GET_RIB_DIRECTIVE__EXCEPTION) {
				if (bPrintToFH)
					ERCore::mf__ERCoreSub_printWrapper(*FH, RIBDirectiveContentRef);
				if (SUBFH)
					ERCore::mf__ERCoreSub_printWrapper(*SUBFH, RIBDirectiveContentRef);
				if (BAKEFH)
					ERCore::mf__ERCoreSub_printWrapper(*BAKEFH, RIBDirectiveContentRef);
				continue;
			}

			// FrameBegin, FrameEnd
			if (enumRIBDirective == ER__ENUM_RIB_DIRECTIVE__FRAME_BEGIN) {
				eval_pv("ERBaseFrameBegin()", TRUE);
				continue;
			}
			if (enumRIBDirective == ER__ENUM_RIB_DIRECTIVE__FRAME_END) {
				eval_pv("ERBaseFrameEnd()", TRUE);
				continue;
			}

			// AttributeBegin/TransformBegin, AttributeEnd/TransformEnd
			if (
				enumRIBDirective == ER__ENUM_RIB_DIRECTIVE__ATTRIBUTE_BEGIN ||
				enumRIBDirective == ER__ENUM_RIB_DIRECTIVE__TRANSFORM_BEGIN
			) {
				eval_pv("ERBaseAttributeBeginTransformBegin()", TRUE);
				continue;
			}
			if (
				enumRIBDirective == ER__ENUM_RIB_DIRECTIVE__ATTRIBUTE_END ||
				enumRIBDirective == ER__ENUM_RIB_DIRECTIVE__TRANSFORM_END
			) {
				eval_pv("ERBaseAttributeEndTransformEnd()", TRUE);
				continue;
			}

			// FrameEnd와 FrameBegin사이에 존재하는 RIB도 있는 그대로 쓴다.
			// 
			if (Core.mf__getgDeclare() == ER__DECLARE__FRAME_BLOCK_OUTSIDE) {
				if (bPrintToFH)
					ERCore::mf__ERCoreSub_printWrapper(*FH, RIBDirectiveContentRef);
				if (SUBFH)
					ERCore::mf__ERCoreSub_printWrapper(*SUBFH, RIBDirectiveContentRef);
				if (BAKEFH)
					ERCore::mf__ERCoreSub_printWrapper(*BAKEFH, RIBDirectiveContentRef);
				continue;
			}

			// 스택의 운명이 지워져야 하는 것이라면 모두 버린다.
			if (fate == ER__CONTENT_FATE__DELETE) {
				continue;
			}

			// Display정보로부터 모든 일을 시작한다.
			//	1. 프레임의 운명이 결정된다.
			//	2. Bake모드의 경우 BAKEFH("Bake Pass"), SUBFH("Final Pass")를 결정한다.
			if (enumRIBDirective == ER__ENUM_RIB_DIRECTIVE__DISPLAY) {
				eval_pv("ERBaseDisplay()", TRUE);
				continue;
			}

			// .sif파일 정보로부터 쉐이더를 바꾼다.
			if (
				enumRIBDirective == ER__ENUM_RIB_DIRECTIVE__LIGHTSOURCE ||
				enumRIBDirective == ER__ENUM_RIB_DIRECTIVE__SURFACE
			) {
				eval_pv("ERBaseShader()", TRUE);
			}

			// 속성처리를 한다.
			if (enumRIBDirective == ER__ENUM_RIB_DIRECTIVE__ATTRIBUTE) {
				eval_pv("ERBaseAttribute()", TRUE);

				SV	*svFate = eval_pv("ERBaseAttributeBlockTransformBlockAttribute()", TRUE);
				fate = SvIV(svFate);
			}

			// RIB Box가 들어갈 위치이다.
			if (enumRIBDirective == ER__ENUM_RIB_DIRECTIVE__WORLD_BEGIN) {
				eval_pv("ERRIBBoxFrameJustBeforeWorldBegin()", TRUE);
			}
			if (enumRIBDirective == ER__ENUM_RIB_DIRECTIVE__WORLD_END) {
				eval_pv("ERRIBBoxWorldJustAfterLastObjectBlock()", TRUE);
			}
			if (enumRIBDirective == ER__ENUM_RIB_DIRECTIVE__GEOMETRIC_PRIMITIVE) {
				eval_pv("ERRIBBoxObjectJustBeforeGeometricPrimitive()", TRUE);
			}

			// LightSource나 geometric primitive의 경우,
			// 또 다른 RIB Box가 들어갈 위치이가도 하지만
			// 중요한 것은 현재 Attribute/Transform블럭의 운명이
			// CONTENT_FATE__SAVE에서 CONTENT_FATE__PRINT로 바뀔 수 있다.
			bool	bFateToPrint = false;
			if (enumRIBDirective == ER__ENUM_RIB_DIRECTIVE__LIGHTSOURCE) {
				bFateToPrint = true;
				eval_pv("ERRIBBoxWorldJustBeforeFirstLightSourceBlock()", TRUE);
			}
			if (enumRIBDirective == ER__ENUM_RIB_DIRECTIVE__GEOMETRIC_PRIMITIVE) {
				bFateToPrint = true;
				eval_pv("ERRIBBoxWorldJustBeforeFirstGeometricPrimitiveBlock()", TRUE);
			}
			if (bFateToPrint) {
				int	iPrintableStackIndex = -1;
				// stackIndex의 값과 jcObjectNameStackIndex의 값이 다르다면
				// jcObjectNameStackIndex에 해당하는 스택의 내용을 먼저 비워야 한다.
				// 그리고 해당 스택의 "운명"을 CONTENT_PRINT로 바꾼다.
				// jcObjectNameStackIndex가 설정되지 않을 수도 있다.
				// 이런 경우에는 "운명"이 CONTENT_SAVE인 상위 스택의 내용을 먼저 비운다.
				if (!jcObjectNameStackIndex) {
					// "운명"이 CONTENT_SAVE인 최상위 스택을 찾는다.
					// 1부터 시작한다.
					// 적어도 Transform block이나 Attribute block안에 있어야 하기 때문이다.
					// 반드시 존재한다! (왜?)
					for (int i = 1; i <= stackIndex; ++i) {
						SV	**hStackRef = av_fetch(stackArray, i, 0);
						if (!hStackRef)
							croak("XSUB: av_fetch( $AoHStackRef -> [ %d ] ): Failure\n", i);
						SV	**stackFateRef = hv_fetch((HV *) SvRV(*hStackRef), "fate", 4, 0);
						if (!stackFateRef)
							croak("XSUB: hv_fetch( $AoHStackRef -> [ %d ] -> { 'fate' } ): Failure\n", i);
						if (SvIV(*stackFateRef) == ER__CONTENT_FATE__SAVE) {
							iPrintableStackIndex = i;
							break;
						}
					}
				} else
					iPrintableStackIndex = SvIV(*jcObjectNameStackIndex);
				if (iPrintableStackIndex <= 0)
					croak("XSUB: iPrintableStackIndex = %d: <= 0\n", iPrintableStackIndex);

				for (int i = iPrintableStackIndex; i <= stackIndex; ++i) {
					SV	**hStackRef = av_fetch(stackArray, i, 0);
					if (!hStackRef)
						croak("XSUB: av_fetch( $AoHStackRef -> [ %d ] ): Failure\n", i);
					// 그 스택 내용을 출력하고,
					SV	**stackContentRef = hv_fetch((HV *) SvRV(*hStackRef), "content", 7, 0);
					if (!stackContentRef)
						croak("XSUB: hv_fetch( $AoHStackRef -> [ %d ] -> { 'content' } ): Failure\n", i);
					if (bPrintToFH)
						ERCore::mf__ERCoreSub_printWrapper(*FH, *stackContentRef);
					if (SUBFH)
						ERCore::mf__ERCoreSub_printWrapper(*SUBFH, *stackContentRef);
					if (BAKEFH)
						ERCore::mf__ERCoreSub_printWrapper(*BAKEFH, *stackContentRef);
					// 스택을 비운 후
					HV	*hThisStack = (HV *) SvRV(*hStackRef);
					SV	**hvStoreOk = hv_store(hThisStack, "content", 7, newRV((SV *) newAV()), 0);
					if (!hvStoreOk)
						croak("XSUB: hv_store( $AoHStackRef -> [ %d ] -> { 'content' }, [] ): Failure\n", i);
					if (BAKEFH) {
						hvStoreOk = hv_store(hThisStack, "bake subframe content", 21, newRV((SV *) newAV()), 0);
						if (!hvStoreOk)
							croak("XSUB: hv_store( $AoHStackRef -> [ %d ] -> { 'bake subframe content' }, [] ): Failure\n", i);
					}
					hvStoreOk = hv_store(hThisStack, "fate", 4, newSViv(ER__CONTENT_FATE__PRINT), 0);
					if (!hvStoreOk)
						croak("XSUB: hv_store( $AoHStackRef -> [ %d ] -> { 'fate' }, CONTENT_FATE__PRINT ): Failure\n", i);
				}

				fate = ER__CONTENT_FATE__PRINT;
			}

			// 현재 RIB Directive의 내용을 출력하거나,
			// 스택의 임시로 저장하거나
			// 아니면 버린다.
			if (fate == ER__CONTENT_FATE__PRINT) {
				if (enumRIBDirective == ER__ENUM_RIB_DIRECTIVE__GEOMETRIC_PRIMITIVE) {
					if (bPrintToFH)
						ERCore::mf__printArrayRefWrapper(*FH, RIBDirectiveContentRef);
					if (SUBFH)
						ERCore::mf__printArrayRefWrapper(*SUBFH, RIBDirectiveContentRef);
					if (BAKEFH)
						ERCore::mf__printArrayRefWrapper(*BAKEFH, RIBDirectiveContentRef);
				} else {
					if (bPrintToFH)
						ERCore::mf__ERCoreSub_printWrapper(*FH, RIBDirectiveContentRef);
					if (SUBFH)
						ERCore::mf__ERCoreSub_printWrapper(*SUBFH, RIBDirectiveContentRef);
					if (BAKEFH)
						ERCore::mf__ERCoreSub_printWrapper(*BAKEFH, RIBDirectiveContentRef);
				}
			} else if (fate == ER__CONTENT_FATE__SAVE) {
				SV	**stackContentRef = hv_fetch(hStack, "content", 7, 0);
				if (!stackContentRef)
					croak("XSUB: hv_fetch( $hStackRef -> { 'content' } ): Failure\n");
				SV	**stackBakeSubframeContentRef = 0;
				if (BAKEFH) {
					stackBakeSubframeContentRef = hv_fetch(hStack, "bake subframe content", 21, 0);
					if (!stackBakeSubframeContentRef)
						croak("XSUB: hv_fetch( $hStackRef -> { 'bake subframe content' } ): Failure\n");
				}

				if (enumRIBDirective == ER__ENUM_RIB_DIRECTIVE__GEOMETRIC_PRIMITIVE) {
					ERCore::mf__appendArrayRefWrapper(*stackContentRef, RIBDirectiveContentRef);
					if (BAKEFH)
						ERCore::mf__appendArrayRefWrapper(*stackBakeSubframeContentRef, RIBDirectiveContentRef);
				} else {
					ERCore::mf__push(*stackContentRef, RIBDirectiveContentRef);
					if (BAKEFH)
						ERCore::mf__push(*stackBakeSubframeContentRef, RIBDirectiveContentRef);
				}
			}
		}

		// 각 "job chunk"의 일을 끝났음을 말해준다.
		Core.mf__setVar("gKeyChunk", newSV(0), ER__VAR_SCOPE__GLOBAL);
		if (enumRIBDirective == ER__ENUM_RIB_DIRECTIVE__DISPLAY)
			Core.mf__setVar("ghDisplayRef", newSV(0), ER__VAR_SCOPE__GLOBAL);
		if (enumRIBDirective == ER__ENUM_RIB_DIRECTIVE__FRAME_END)
			Core.mf__setVar("aElementName", newSV(0), ER__VAR_SCOPE__ALL);
	}

	// 모든 일을 마쳤으므로 마무리작업에 들어간다.
#if 0
	for (I32 i = 0; i <= av_len(aSavedRIBDirectiveContent); ++i) {
		SV	**aref = av_fetch(aSavedRIBDirectiveContent, i, 0);
		if (!aref || !*aref || !SvOK(*aref))
			printf("aSavedRIBDirectiveContent[%d]: undef\n", i);
		else
			printf("aSavedRIBDirectiveContent[%d]: (%p->%p)%s|%s|\n", i, aref, *aref, SvPV_nolen(*aref), SvPV_nolen(Core.mf__ERCoreSub_joinWrapper(*aref)));
	}
#endif
	// Destroy aSavedRIBDirectiveContent
	av_undef(aSavedRIBDirectiveContent);

	if (RIBDirective && SvOK(RIBDirective)) {
		SvREFCNT_dec(RIBDirective);
		RIBDirective = 0;
	}
	if (RIBDirectiveContentRef && SvOK(RIBDirectiveContentRef)) {
		SvREFCNT_dec(RIBDirectiveContentRef);
		RIBDirectiveContentRef = 0;
	}

	// 모든 파일을 닫는다.
{
	HV	*HoHChunk = Core.mf__getHoHChunk();
	(void) hv_iterinit(HoHChunk);		// # of keys
	HE	*pHE;
	while ((pHE = hv_iternext(HoHChunk))) {
		I32	keyLength;
		char	*keyChunk = hv_iterkey(pHE, &keyLength);
		if (!keyChunk)
			croak("XSUB: Error: hv_iterkey: Oops\n");
//		printf("Key:= |%s|(%d)\n", keyChunk, keyLength);
		Core.mf__setVar("gKeyChunk", newSVpvf("%s", keyChunk), ER__VAR_SCOPE__GLOBAL);
		SV	*jcRibFileHandle = newSVpvf("fh%s", keyChunk);
		if (Core.mf__ERCoreSub_CloseWrapper(jcRibFileHandle) != 0) {
			(void) fprintf(stderr, "XSUB: ERCore::mf__ERCoreSub_CloseWrapper(\"%s\"): Failure\n", SvPV_nolen(jcRibFileHandle));
		}
		SvREFCNT_dec(jcRibFileHandle);
	}
}
	if (Core.mf__ERCoreSub_CloseWrapper(ribFileHandle) != 0) {
		(void) fprintf(stderr, "XSUB: ERCore::mf__ERCoreSub_CloseWrapper(\"%s\"): Failure\n", SvPV_nolen(ribFileHandle));
	}
	if (ribFileHandle && SvOK(ribFileHandle)) {
		SvREFCNT_dec(ribFileHandle);
		ribFileHandle = 0;
	}
