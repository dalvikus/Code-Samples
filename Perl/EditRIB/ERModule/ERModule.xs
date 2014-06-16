/*
 * ERModule.xs
 *	1. Clean up unused example codes.
 */
/*
	2007/06/28 (KST)
	�豤ȣ (Gwang-Ho Kim, iccentric@gmail.com)

	HV	*h = newHV();
	AV	*a = newAV();
	// ��� ���� SvOK���� �����Ѵ�.
	// ��� hv_iterinit(h)�� av_len(a)�� ����� ���� 0, -1�μ�
	// ���� �ʱ�ȭ���� �ʾ����� �����ش�.

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
	// ���Ǵ� �������� �ʱ�ȭ�Ѵ�.
	Core.mf__setVar("gKeyChunk", newSV(0), ER__VAR_SCOPE__GLOBAL);
	Core.mf__setVar("aElementName", newSV(0), ER__VAR_SCOPE__ALL);
	Core.mf__setVar("jcObjectName", newSV(0), ER__VAR_SCOPE__JOB_CHUNK);
	Core.mf__setVar("jcObjectNameStackIndex", newSV(0), ER__VAR_SCOPE__JOB_CHUNK);
	Core.mf__setVar("gDeclare", newSViv(ER__DECLARE__NO_FRAME), ER__VAR_SCOPE__GLOBAL);
	Core.mf__setgDeclare(ER__DECLARE__NO_FRAME);
	Core.mf__setstrDeclare(newSVpv("", 0));

	// Ŀ�ǵ� ���λ󿡼� ������ ������ �о���δ�.
	// 1. �÷��װ� �����Ǿ� �ִ� �� �˾ƺ���.
	bool	bAscii = SvIV(Core.mf__getCommandLineOption("bAscii"));
//	bool	bVerbosePath = SvIV(Core.mf__getCommandLineOption("bVerbosePath"));
	// 2. RIB ���Ϸκ��� �ʿ��� ������ �����Ѵ�.
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
//			$path = './' unless $path;	# "-d"�ɼ����� �����θ� �������� �ʵ��� �ϱ� ���ؼ�
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

	// RIB ������ ����.
	SV	*ribFileHandle = newSVpv("RIBFH", 5);
	if (!ribFileHandle || !SvOK(ribFileHandle))
		croak("XSUB: newSVpv('RIBFH'): Failure\n");
	SV	*fileOpenExpr = newSVpvf("%s %s", bAscii ? "<" : "catrib", SvPV_nolen(ribFile));
	if (!fileOpenExpr || !SvOK(fileOpenExpr))
		croak("XSUB: newSVpvf( $ribFile ): Failure\n");
	if (Core.mf__ERCoreSub_OpenWrapper(ribFileHandle, fileOpenExpr) != 0)
		croak("XSUB: ERCore::mf__ERCoreSub_OpenWrapper(\"%s\"): Failure\n", SvPV_nolen(ribFile));
	SvREFCNT_dec(fileOpenExpr);

	// ���� �̸��� �����, �߰����� ��� ����(rootPathEvalExpr, addPathEvalExpr)�� ��´�.
	SV	*rootPathEvalExpr0 = Core.mf__getCommandLineOption("rootPathExpr");
	STRLEN	rootPathEvalExprLength = SvLEN(rootPathEvalExpr0);
	SV	*addPathEvalExpr0 = Core.mf__getCommandLineOption("addPathExpr");

	// �� "job chunk"�� ���ؼ� �ʿ��� ������ �����ϰ�,
	// �� "job chunk"�� ����� ������ RIB ������ ����.
	HV	*HoHChunk = Core.mf__getHoHChunk();
	(void) hv_iterinit(HoHChunk);		// # of keys ("job chunk")
	HE	*pHE;
	while ((pHE = hv_iternext(HoHChunk))) {
		// "job chunk"�� �̸��� ��´�.
		I32	keyLength;
		char	*keyChunk = hv_iterkey(pHE, &keyLength);
		if (!keyChunk)
			croak("XSUB: Error: hv_iterkey: Oops\n");
//		printf("Key:= |%s|(%d)\n", keyChunk, keyLength);
		// ���� ó���ϰ� �ִ� "job chunk"�� �̸��� ����Ѵ�.
		// ���⼭ ȣ���ϴ� � ������ ���������� ���� �� �� �ֵ���.
		Core.mf__setVar("gKeyChunk", newSVpvf("%s", keyChunk), ER__VAR_SCOPE__GLOBAL);

		// �� "job chunk"�� ������ ���� �ؽ������� ��´�.
		SV	*hChunkRef = hv_iterval(HoHChunk, pHE);
		if (!hChunkRef || !SvOK(hChunkRef) || SvTYPE(SvRV(hChunkRef)) != SVt_PVHV)
			croak("XSUB: Error: hv_iterval: Oops\n");
//		printf("Val:= |%s|\n", SvPV_nolen(hChunkRef));
		HV	*hChunk = (HV *) SvRV(hChunkRef);	// Hash

		// "type"�����κ��� Bake��尡 �����Ǿ��� �� �˾ƺ���.
		SV	**svTypeChunk = hv_fetch(hChunk, "type", 4, 0);
		if (!svTypeChunk)
			croak("XSUB: Error: hv_fetch: Oops\n");
		int	iTypeChunk = SvIV(*svTypeChunk);
		bool	bBake = false;
		if (iTypeChunk & ER__CHUNK_TYPE__BAKE)
			bBake = true;
{
		// Bake��� ������ �����Ѵ�.
		SV	*bakeValue = newSViv(bBake);
		SV	**svbBake = hv_store(hChunk, "bBake", 5, bakeValue, 0);
		if (!svbBake || !*svbBake || !SvOK(*svbBake))
			croak("XSUB: Error: hv_store: Oops\n");
}
		if (iTypeChunk & ER__CHUNK_TYPE__BAKE) {	// "type"�� Bake��� ������ �־��ٸ�
			// �̹� �� ������ ��������Ƿ� "type"���� Bake��� ������ ���ش�.
			iTypeChunk &= ~ER__CHUNK_TYPE__BAKE;
			SV	*svTypeChunkWithoutBake = newSViv(iTypeChunk);
			SV	**svTypeChunkTmp = hv_store(hChunk, "type", 4, svTypeChunkWithoutBake, 0);
			if (!svTypeChunkTmp || !*svTypeChunkTmp || !SvOK(*svTypeChunkTmp))
				croak("XSUB: Error: hv_store: Oops\n");
		}

		// ���ǥ���� �ؼ��� ���Ǵ� ����("$keyChunk")�� �����Ѵ�.
		SV	*evalExpr = newSVpvf("$keyChunk = \"%s\";", keyChunk);
		eval_sv(evalExpr, G_DISCARD | G_NOARGS);
		SvREFCNT_dec(evalExpr);
		// -R�� �־��� ��Ʈ ��θ� �ؼ��Ѵ�.
		SV	*rootPath = newSVpv("", 0);
		if (rootPathEvalExprLength) {
			SV	*rootPathEvalExpr = newSVpv("qq|", 3);
			sv_catpvf(rootPathEvalExpr, "%s/|", SvPV_nolen(rootPathEvalExpr0));
			rootPath = eval_pv(SvPV_nolen(rootPathEvalExpr), TRUE);
			sv_catpv(rootPath, "/rib/");
		}
		// -d�� �־��� �߰� ��θ� �ؼ��Ѵ�.
		SV	*addPathEvalExpr = newSVpv("qq|", 3);
		sv_catpvf(addPathEvalExpr, "%s|", SvPV_nolen(addPathEvalExpr0));
		SV	*addPath = eval_pv(SvPV_nolen(addPathEvalExpr), TRUE);
		SvREFCNT_dec(addPathEvalExpr);

		// �ش� "job chunk"�� ����� ������ �����̸��� �����Ѵ�.
		// �ʿ��ϸ� ��θ� �����.
		SV	*newUNIXPath = rootPathEvalExprLength ? rootPath : newSVsv(Core.mf__getpath());
		sv_catsv(newUNIXPath, addPath);
		SV	*jcPathname = ERCore::mf__mkdir2Wrapper(newUNIXPath);
//		(void) printf("RibPath: |%s|\n", SvPV_nolen(jcPathname));
		SvREFCNT_dec(newUNIXPath);
		sv_catpvf(jcPathname, "%s.%s%s%s", SvPV_nolen(Core.mf__getdspyName()), keyChunk, bBake ? ".ShadowPass" : "", SvPV_nolen(Core.mf__getdspyName0()));

		// "job chunk"�� ����� ������ RIB ������ ����.
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

		// �����ڵ鿡 ���� ������ �����ϰ�,
		// �� ����, �� "job chunk"������ ������ �ʱ�ȭ�Ѵ�.
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

	// ���� RIB�� RIB Directive������ �� �پ� �д´�.
	// ��� RIB Directive�� ��쿡�� �� "job chunk"���� �� ������ �ٲ�Ƿ�,
	// �ٸ� "job chunk"���� ����� �� �ֵ��� �� ������ ���� �����ؾ� �Ѵ�.
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
		// �� �پ� �д´�.
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

		// Declare�κ��� ���� �����Ѵ�.
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

		// �ٸ� �Լ����� ������ �� �ֵ��� (enumRIBDirective, RIBDirective, RIBDirectiveContentRef)�� �����Ѵ�.
		Core.mf__setVar("gEnumRIBDirective", newSViv(enumRIBDirective), ER__VAR_SCOPE__GLOBAL);
		Core.mf__setVar("gRIBDirective", newSVsv(RIBDirective), ER__VAR_SCOPE__GLOBAL);
		Core.mf__setVar("gRIBDirectiveContentRef", newSVsv(RIBDirectiveContentRef), ER__VAR_SCOPE__GLOBAL);

		// "Display"�� ���, �� ������ �ؼ��Ͽ� �ʿ��� ������ �����Ѵ�.
		if (enumRIBDirective == ER__ENUM_RIB_DIRECTIVE__DISPLAY) {
			eval_pv("ERBasePreDisplay()", TRUE);
		}

		// �� "job chunk"���� ���� ������ ������ ó���ϵ���
		// ���� ������ ���� �����Ѵ�.
		if (enumRIBDirective >= 0) {
			// iReadReturnVal = ER__GET_RIB_DIRECTIVE__EXCEPTION�� ���� �ִ�.
			// �� ���, RIB Directive�� ���ǵ��� �ʴ´�.
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

		// ���� ������ ������ �� "job chunk"���� ������ �۾��� �Ѵ�.
		HV	*HoHChunk = Core.mf__getHoHChunk();
		(void) hv_iterinit(HoHChunk);		// # of keys ("job chunk")
		HE	*pHE;
		while ((pHE = hv_iternext(HoHChunk))) {
			// "job chunk"�� �̸��� ��´�.
			I32	keyLength;
			char	*keyChunk = hv_iterkey(pHE, &keyLength);
			if (!keyChunk)
				croak("XSUB: Error: hv_iterkey: Oops\n");
//			printf("Key:= |%s|(%d)\n", keyChunk, keyLength);
			// �ٸ� �Լ����� ������ �� �ֵ��� "job chunk"�̸��� �����Ѵ�.
			Core.mf__setVar("gKeyChunk", newSVpvf("%s", keyChunk), ER__VAR_SCOPE__GLOBAL);

			// ��� RIB Directive�� ��� �׻� ������ ������ ������ ó���Ѵ�.
			if (enumRIBDirective >= 0) {
				// iReadReturnVal = ER__GET_RIB_DIRECTIVE__EXCEPTION�� ���� �ִ�.
				// �� ���, RIB Directive�� ���ǵ��� �ʴ´�.
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

			// �ʿ��� ������ �����Ѵ�.
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
			// ���� �������� �ʾ��� ���� �ִ�!
			SV	**BAKEFH = hv_fetch(hChunk, "bake subframe file handle", 25, 0);
			// ���� �������� �ʾ��� ���� �ִ�!
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
			// ���� "����"�� �������� �ʾ��� ���� �ִ�!
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
			bool	bPrintToFH = false;	// FH�� �� �� �ִ� ���� �����ش�.
{
			SV	**bBake = hv_fetch(hChunk, "bBake", 5, 0);
			if (!bBake)
				croak("XSUB: hv_fetch( $hChunkRef -> { 'bBake' } ): Failure\n");
			if (SvIV(*bBake)) {	// Bake��忡����
				// Shadow Pass�� ��쿡�� ����.
//	my	$aElementName = $hChunkRef -> { 'aElementName' };
				SV	**aElementNameRef = hv_fetch(hChunk, "aElementName", 12, 0);
				// ���� �������� �ʾ��� ���� �ִ�!
				if (aElementNameRef && SvOK(*aElementNameRef)) {	// �̸��� ������ ��쿡 ���ؼ�
					if (strcmp(SvPV_nolen(*aElementNameRef), "Image")) {
						// "Image"�� Final Rendering Pass�� �ƴ� ���
						bPrintToFH = true;
					}
				}
			} else {		// Bake��尡 �ƴ� ����
				// ������ ����.
				bPrintToFH = true;
			}
}
			// ���� �������� �ʾ��� ���� �ִ�!
			SV	**jcObjectNameStackIndex = hv_fetch(hChunk, "jcObjectNameStackIndex", 22, 0);

			// �������� ��쿡�� ���� �״�� ����.
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

			// FrameEnd�� FrameBegin���̿� �����ϴ� RIB�� �ִ� �״�� ����.
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

			// ������ ����� �������� �ϴ� ���̶�� ��� ������.
			if (fate == ER__CONTENT_FATE__DELETE) {
				continue;
			}

			// Display�����κ��� ��� ���� �����Ѵ�.
			//	1. �������� ����� �����ȴ�.
			//	2. Bake����� ��� BAKEFH("Bake Pass"), SUBFH("Final Pass")�� �����Ѵ�.
			if (enumRIBDirective == ER__ENUM_RIB_DIRECTIVE__DISPLAY) {
				eval_pv("ERBaseDisplay()", TRUE);
				continue;
			}

			// .sif���� �����κ��� ���̴��� �ٲ۴�.
			if (
				enumRIBDirective == ER__ENUM_RIB_DIRECTIVE__LIGHTSOURCE ||
				enumRIBDirective == ER__ENUM_RIB_DIRECTIVE__SURFACE
			) {
				eval_pv("ERBaseShader()", TRUE);
			}

			// �Ӽ�ó���� �Ѵ�.
			if (enumRIBDirective == ER__ENUM_RIB_DIRECTIVE__ATTRIBUTE) {
				eval_pv("ERBaseAttribute()", TRUE);

				SV	*svFate = eval_pv("ERBaseAttributeBlockTransformBlockAttribute()", TRUE);
				fate = SvIV(svFate);
			}

			// RIB Box�� �� ��ġ�̴�.
			if (enumRIBDirective == ER__ENUM_RIB_DIRECTIVE__WORLD_BEGIN) {
				eval_pv("ERRIBBoxFrameJustBeforeWorldBegin()", TRUE);
			}
			if (enumRIBDirective == ER__ENUM_RIB_DIRECTIVE__WORLD_END) {
				eval_pv("ERRIBBoxWorldJustAfterLastObjectBlock()", TRUE);
			}
			if (enumRIBDirective == ER__ENUM_RIB_DIRECTIVE__GEOMETRIC_PRIMITIVE) {
				eval_pv("ERRIBBoxObjectJustBeforeGeometricPrimitive()", TRUE);
			}

			// LightSource�� geometric primitive�� ���,
			// �� �ٸ� RIB Box�� �� ��ġ�̰��� ������
			// �߿��� ���� ���� Attribute/Transform���� �����
			// CONTENT_FATE__SAVE���� CONTENT_FATE__PRINT�� �ٲ� �� �ִ�.
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
				// stackIndex�� ���� jcObjectNameStackIndex�� ���� �ٸ��ٸ�
				// jcObjectNameStackIndex�� �ش��ϴ� ������ ������ ���� ����� �Ѵ�.
				// �׸��� �ش� ������ "���"�� CONTENT_PRINT�� �ٲ۴�.
				// jcObjectNameStackIndex�� �������� ���� ���� �ִ�.
				// �̷� ��쿡�� "���"�� CONTENT_SAVE�� ���� ������ ������ ���� ����.
				if (!jcObjectNameStackIndex) {
					// "���"�� CONTENT_SAVE�� �ֻ��� ������ ã�´�.
					// 1���� �����Ѵ�.
					// ��� Transform block�̳� Attribute block�ȿ� �־�� �ϱ� �����̴�.
					// �ݵ�� �����Ѵ�! (��?)
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
					// �� ���� ������ ����ϰ�,
					SV	**stackContentRef = hv_fetch((HV *) SvRV(*hStackRef), "content", 7, 0);
					if (!stackContentRef)
						croak("XSUB: hv_fetch( $AoHStackRef -> [ %d ] -> { 'content' } ): Failure\n", i);
					if (bPrintToFH)
						ERCore::mf__ERCoreSub_printWrapper(*FH, *stackContentRef);
					if (SUBFH)
						ERCore::mf__ERCoreSub_printWrapper(*SUBFH, *stackContentRef);
					if (BAKEFH)
						ERCore::mf__ERCoreSub_printWrapper(*BAKEFH, *stackContentRef);
					// ������ ��� ��
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

			// ���� RIB Directive�� ������ ����ϰų�,
			// ������ �ӽ÷� �����ϰų�
			// �ƴϸ� ������.
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

		// �� "job chunk"�� ���� �������� �����ش�.
		Core.mf__setVar("gKeyChunk", newSV(0), ER__VAR_SCOPE__GLOBAL);
		if (enumRIBDirective == ER__ENUM_RIB_DIRECTIVE__DISPLAY)
			Core.mf__setVar("ghDisplayRef", newSV(0), ER__VAR_SCOPE__GLOBAL);
		if (enumRIBDirective == ER__ENUM_RIB_DIRECTIVE__FRAME_END)
			Core.mf__setVar("aElementName", newSV(0), ER__VAR_SCOPE__ALL);
	}

	// ��� ���� �������Ƿ� �������۾��� ����.
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

	// ��� ������ �ݴ´�.
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
