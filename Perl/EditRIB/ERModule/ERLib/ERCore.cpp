#include "ERConstant.h"
#include "ERCore.h"
#include <stdio.h>	// printf, fprintf
#include <string.h>	// strlen
#ifdef NDEBUG
#define RESTORE_NDEBUG
#undef NDEBUG
#endif	// RESTORE_NDEBUG
#include <assert.h>
#ifdef RESTORE_NDEBUG
#define NDEBUG
#endif	// RESTORE_NDEBUG


static char	evalSubExpr[] = ""
"sub ERCoreSub_Open"
"{"
"	my	( $fileHandle, $fileExpr ) = @_;"
""
"	return -1 unless $fileHandle;"
"	return -2 unless $fileExpr;"
""
"	return -3 unless open( $fileHandle, $fileExpr );"
"	return 0;"
"}"
""
"sub ERCoreSub_Close"
"{"
"	my	$fileHandle = shift;"
"	return -1 unless $fileHandle;"
"	return -2 unless close( $fileHandle );"
"	return 0;"
"}"
""
"sub ERCoreSub_print"
"{"
"	my	( $fileHandleExpr, $aref ) = @_;"
"	return unless $fileHandleExpr || $aref;"
"	print $fileHandleExpr join( '', @{ $aref } );"
"}"
""
"sub ERCoreSub_join"
"{"
"	my	$aref = shift;"
""
"	my	$s = join( '', @{ $aref } );"
"	return $s;"
"}"
""
""
"1";

ERCore::ERCore(
	int	*piReturnVal	// [OUT]
)
{
	SV	*svResult = eval_pv(evalSubExpr, TRUE);
	if (!svResult || SvOK(svResult) || SvIV(svResult) == 0) {
		*piReturnVal = -1;
	}

	// %HoHChunk
	HV	*HoHChunk = get_hv("ERModule::HoHChunk", 0);
	if (!HoHChunk)
		croak("XXX: Error: HoHChunk: undef\n");
	m__HoHChunk = HoHChunk;
	// %ghVar
	HV	*ghVar = get_hv("ERModule::ghVar", 0);
	if (!ghVar)
		croak("XXX: Error: ghVar: undef\n");
	// $hGlobalVariableRef
	SV	**hGlobalVariableRef = hv_fetch(ghVar, "global variable", 15, 0);
	if (!hGlobalVariableRef || !SvOK(*hGlobalVariableRef) || SvTYPE(SvRV(*hGlobalVariableRef)) != SVt_PVHV)
		croak("XXX: hv_fetch($ghVar{ 'global variable' }): Invalid\n");
	m__hGlobalVariable = (HV *) SvRV(*hGlobalVariableRef);
	// $hCommandLineOptionRef
	SV	**hCommandLineOptionRef = hv_fetch(ghVar, "command line option", 19, 0);
	if (!hCommandLineOptionRef || !SvOK(*hCommandLineOptionRef) || SvTYPE(SvRV(*hCommandLineOptionRef)) != SVt_PVHV)
		croak("XXX: hv_fetch($ghVar{ 'global variable' }): Invalid\n");
	m__hCommandLineOption = (HV *) SvRV(*hCommandLineOptionRef);

	m__strDeclare = 0;

	*piReturnVal = 0;
}


ERCore::~ERCore()
{
	if (m__strDeclare) {
		assert(SvREFCNT(m__strDeclare));
		SvREFCNT_dec(m__strDeclare);
		m__strDeclare = 0;
	}
}


SV	*
ERCore::mf__ERCoreSub_joinWrapper(
	SV	*svArrayRef
)
{
	if (!svArrayRef || !SvOK(svArrayRef) || !SvROK(svArrayRef) || SvTYPE(SvRV(svArrayRef)) != SVt_PVAV) {
		(void) fprintf(stderr, "XXX: Error: ERCore::mf__ERCoreSub_joinWrapper: svArrayRef(%p): Invalid\n", svArrayRef);
		return NULL;
	}
	SV	*svReturnVal;


	dSP;

	ENTER;
	SAVETMPS;

	PUSHMARK(SP);
	XPUSHs(svArrayRef);
	PUTBACK;

	int	count = call_pv("ERCoreSub_join", G_SCALAR);
	if (count != 1)
		croak("XXX: Error: ERCore::mf__ERCoreSub_joinWrapper(): count != 1: Failure\n");

	SPAGAIN;

	SV	*svLine = (SV *) POPs;
	if (SvOK(svLine)) {
		svReturnVal = newSVsv(svLine);
	} else {
		svReturnVal = NULL;
	}

	PUTBACK;

	FREETMPS;
	LEAVE;


	return svReturnVal;
}

void
ERCore::mf__ERCoreSub_printWrapper(
	SV	*fileHandle,
	SV	*svLine
)
{
	if (!fileHandle || !SvOK(fileHandle)) {
		(void) fprintf(stderr, "XXX: Error: ERCore::mf__ERCoreSub_printWrapper: fileHandle(%p): Invalid\n", fileHandle);
		return;
	}
	if (!svLine || !SvOK(svLine)) {
		(void) fprintf(stderr, "XXX: Error: ERCore::mf__ERCoreSub_printWrapper: svLine(%p): Invalid\n", svLine);
		return;
	}


	dSP;

	ENTER;
	SAVETMPS;

	PUSHMARK(SP);
	XPUSHs(fileHandle);
	XPUSHs(svLine);
	PUTBACK;

	call_pv("ERCoreSub_print", G_DISCARD);

	FREETMPS;
	LEAVE;
}

int
ERCore::mf__ERCoreSub_OpenWrapper(
	SV	*fileHandle,
	SV	*fileOpenExpr
)
{
	if (!fileHandle || !SvOK(fileHandle)) {		// if NOT VALID "Open" file expression
		(void) fprintf(stderr, "XXX: Error: ERCore::mf__ERCoreSub_OpenWrapper: fileHandle(%p): Invalid\n", fileHandle);
		return -1;
	}
	if (!fileOpenExpr || !SvOK(fileOpenExpr)) {		// if NOT VALID "Open" file expression
		(void) fprintf(stderr, "XXX: Error: ERCore::mf__ERCoreSub_OpenWrapper: fileOpenExpr(%p): Invalid\n", fileOpenExpr);
		return -1;
	}
	int	iReturnVal;	// return value


	dSP;

	ENTER;
	SAVETMPS;

	PUSHMARK(SP);
	XPUSHs(fileHandle);
	XPUSHs(fileOpenExpr);
	PUTBACK;

	int	count = call_pv("ERCoreSub_Open", G_SCALAR);
	if (count != 1) {
		croak("XXX: Error: ERCore::mf__ERCoreSub_OpenWrapper(): count != 1: Failure\n");
	}

	SPAGAIN;

	iReturnVal = POPi;

	PUTBACK;

	FREETMPS;
	LEAVE;


	return iReturnVal;
}

int
ERCore::mf__ERCoreSub_CloseWrapper(
	SV	*fileHandle
)
{
	if (!fileHandle || !SvOK(fileHandle)) {	// if NOT VALID file handle
		(void) fprintf(stderr, "XXX: Error: ERCore::mf__Close: fileHandle(%p): Invalid\n", fileHandle);
		return -1;
	}
	int	iReturnVal;	// return value


	dSP;

	ENTER;
	SAVETMPS;

	PUSHMARK(SP);
	XPUSHs(fileHandle);
	PUTBACK;

	int	count = call_pv("ERCoreSub_Close", G_SCALAR);
	if (count != 1)
		croak("XXX: Error: ERCore::mf__Close(): count != 1: Failure\n");

	SPAGAIN;

	SV	*svReturnVal = (SV *) POPs;
	if (SvOK(svReturnVal))
		iReturnVal = SvIV(svReturnVal);
	else {
		(void) fprintf(stderr, "XXX: Error: ERCore::mf__Close: Failure\n");
		iReturnVal = -1;
	}

	PUTBACK;

	FREETMPS;
	LEAVE;


	return iReturnVal;
}


int
ERCore::mf__Read(
	SV	*fileHandle,
	int	&enumRIBDirective,
	SV	**RIBDirective,
	SV	**RIBDirectiveContentRef
)
{
	if (!fileHandle || !SvOK(fileHandle)) {	// if NOT VALID file handle
		(void) fprintf(stderr, "XXX: Error: ERCore::mf__Read: fileHandle(%p): Invalid\n", fileHandle);
		return ER__GET_RIB_DIRECTIVE__ERROR;
	}
	int	iReturnVal;	// return value


	dSP;

	ENTER;
	SAVETMPS;

	PUSHMARK(SP);
//	XPUSHs(sv_2mortal(newSVsv(fileHandle)));
	XPUSHs(fileHandle);
	PUTBACK;

	int	count = call_pv("getLineByRIBDirective", G_ARRAY);
	if (count != 4)
		croak("XXX: Error: ERCore::mf__Read(): count != 4: Failure\n");

	SPAGAIN;

	SV	*sv4th = (SV *) POPs;
	*RIBDirectiveContentRef = newSVsv(sv4th);
	if (!SvOK(sv4th)) {
//		(void) fprintf(stderr, "XXX: Info: ERCore::mf__Read: RIB Directive Content Reference: undef\n");
	}
	SV	*sv3rd = (SV *) POPs;
	*RIBDirective = newSVsv(sv3rd);
	if (!SvOK(sv3rd)) {
//		(void) fprintf(stderr, "XXX: Info: ERCore::mf__Read: RIB Directive: undef\n");
	}
	SV	*sv2nd = (SV *) POPs;
	if (SvOK(sv2nd))
		enumRIBDirective = SvIV(sv2nd);
	else
		enumRIBDirective = ER__ENUM_RIB_DIRECTIVE__UNDEF;
	iReturnVal = POPi;

	PUTBACK;

	FREETMPS;
	LEAVE;


	return iReturnVal;
}


// 목적: (varName, varValue)쌍을 varScope에 따라 전역변수에 저장한다.
//	ER__VAR_SCOPE__GLOBAL:
//		ERModule::ghVar의 "global variable"가 키가 가리키는 해쉬구조에 저장한다.
//	ER__VAR_SCOPE__JOB_CHUNK:
//		ERModule::HoHChunk의 각 "key chunk"가 가리키는 해쉬구조에 저장한다.
//	ER__VAR_SCOPE__ALL:
//		두 곳에 모두 저장한다. 
// 주1. varValue는 호출하는 함수에서 newsV...를 통해서 만든다.
// 따라서 이 함수의 끝에서 SvREFCNT_dec를 호출하여 varValue를 없앤다.
// 주2. 어떤 문제가 있을 때 croak함수를 호출한다.
int
ERCore::mf__setVar(
	const char	*varName,
	SV		*varValue,	// 적절한 변수에 이 값을 설정한 후, 이 값 자체는 제거된다.
	int		varScope
)
{
	if (!varName)
		croak("XXX: Error: ERCore::mf__setVar: varName = NULL\n");
	I32	varNameLength = (I32) strlen(varName);	// size_t

	if (varScope & ER__VAR_SCOPE__GLOBAL ) {
		SV	**svValue = hv_store(m__hGlobalVariable, varName, varNameLength, newSVsv(varValue), 0);
		if (!svValue || !*svValue) {
			// undef가 설정될 수 있으므로 SvOK()로 테스트할 수 없다.
			croak("XXX: ERCore::mf__setVar: ER__VAR_SCOPE__GLOBAL: hv_store(): Invalid svValue(0x%p -> 0x%p)\n", svValue, svValue ? *svValue : 0);
		}
	}

	if (varScope & ER__VAR_SCOPE__JOB_CHUNK) {
		(void) hv_iterinit(m__HoHChunk);	// # of "job chunk"
		HE	*pHE;
		while ((pHE = hv_iternext(m__HoHChunk))) {
			I32	keyLength;
			char	*keyChunk = hv_iterkey(pHE, &keyLength);
			if (!keyChunk)
				croak("XXX: Error: hv_iterkey: key -> NULL\n");
			SV	*hChunkRef = hv_iterval(m__HoHChunk, pHE);
			if (!hChunkRef || !SvOK(hChunkRef) || SvTYPE(SvRV(hChunkRef)) != SVt_PVHV)
				croak("XXX: Error: hv_iterval( $HoHChunk{ '%s' } ): Invalid svValue(0x%p)\n", hChunkRef);
			HV	*hChunk = (HV *) SvRV(hChunkRef);

			SV	**svValue = hv_store(hChunk, varName, varNameLength, newSVsv(varValue), 0);
			if (!svValue || !*svValue) {
				// undef가 설정될 수 있으므로 SvOK()로 테스트할 수 없다.
				croak("XXX: ERCore::mf__setVar: ER__VAR_SCOPE__JOB_CHUNK: hv_store(): Invalid svValue(0x%p -> 0x%p)\n", svValue, svValue ? *svValue : 0);
			}
		}
	}

	// 더 이상 사용하지 않는 varValue을 없앤다.
	SvREFCNT_dec(varValue);

	return 0;
}

// $ERModule::ghVar{ 'command line option' }이 가리키는 해쉬에서
// keyName에 해당하는 value를 얻는다.
SV	*
ERCore::mf__getCommandLineOption(
	const char	*keyName
)
{
	I32	keyNameLength = (I32) strlen(keyName);	// size_t
	SV	**value = hv_fetch(m__hCommandLineOption, keyName, keyNameLength, 0);
	if (!value || !*value || !SvOK(*value))
		croak("XSUB: hv_fetch( $hCommandLineOption{ '%s' } ): Failure\n", keyName);
	return *value;
}

// 주어진 문자열이 해당 정규식(regular expression)을 가지는 지 알아본다.
// 괄호로 묶인 일치하는 것들에 대한 배열을 리턴한다.
// 펄에서 해당 배열을 저장하였으므로 펄이 끝날 때 알아서 없앤다.
AV	*
ERCore::mf__re(
	const char	*string,
	const char	*pattern
)
{
	SV	*svEvalExpr = newSVpvf("my $a = \"%s\"; my $bref = [ $a =~ /%s/ ]; $bref;", string, pattern);
	SV	*avResultRef = eval_pv(SvPV_nolen(svEvalExpr), TRUE);
	SvREFCNT_dec(svEvalExpr);
	if (!avResultRef || !SvOK(avResultRef) || !SvROK(avResultRef))
		croak("XSUB: eval_pv(\"%s\" =~ /%s/): Failure\n", string, pattern);
	return (AV *) SvRV(avResultRef);
}


// restoreArrayRef, printArrayRef, appendArrayRef를 호출하는 Wrapper함수들...
void
ERCore::mf__restoreArrayRefWrapper(
	SV	*aref
)
{
	dSP;

	ENTER;
	SAVETMPS;

	PUSHMARK(SP);
	XPUSHs(aref);
	PUTBACK;

	call_pv("restoreArrayRef", G_DISCARD);

	FREETMPS;
	LEAVE;
}
void
ERCore::mf__printArrayRefWrapper(
	SV	*fileHandle,
	SV	*aref
)
{
	dSP;

	ENTER;
	SAVETMPS;

	PUSHMARK(SP);
	XPUSHs(fileHandle);
	XPUSHs(aref);
	PUTBACK;

	call_pv("printArrayRef", G_DISCARD);

	FREETMPS;
	LEAVE;
}
void
ERCore::mf__appendArrayRefWrapper(
	SV	*dstArrayRef,
	SV	*srcArrayRef
)
{
	dSP;

	ENTER;
	SAVETMPS;

	PUSHMARK(SP);
	XPUSHs(dstArrayRef);
	XPUSHs(srcArrayRef);
	PUTBACK;

	call_pv("appendArrayRef", G_DISCARD);

	FREETMPS;
	LEAVE;
}

// mkdir2를 호출하는 Wrapper함수
SV	*
ERCore::mf__mkdir2Wrapper(
	SV	*oldUNIXPath
)
{
	SV	*svReturnVal;


	dSP;

	ENTER;
	SAVETMPS;

	PUSHMARK(SP);
	XPUSHs(oldUNIXPath);
	PUTBACK;

	int	count = call_pv("mkdir2", G_SCALAR);
	if (count != 1)
		croak("XXX: Error: ERCore::mf__mkdir2Wrapper(): count != 1: Failure\n");

	SPAGAIN;

	SV	*newUNIXPath = (SV *) POPs;
	if (SvOK(newUNIXPath)) {
		svReturnVal = newSVsv(newUNIXPath);
	} else {
		(void) fprintf(stderr, "XXX: Error: ERCore::mf__mkdir2Wrapper: undef: Failure\n");
		svReturnVal = NULL;
	}

	PUTBACK;

	FREETMPS;
	LEAVE;


	return svReturnVal;
}


AV	*
ERCore::mf__copy(
	SV	*srcArrayRef
)
{
	if (!srcArrayRef || !SvOK(srcArrayRef) || !SvROK(srcArrayRef) || SvTYPE(SvRV(srcArrayRef)) != SVt_PVAV)
		croak("XXX: ERCore::mf__copy: srcArrayRef = %p: Not a reference to array\n", srcArrayRef);

	AV	*srcArray = (AV *) SvRV(srcArrayRef);
	AV	*dstArray = newAV();
	I32	arrayLastIndex = av_len(srcArray);
	if (arrayLastIndex >= 0)
		av_extend(dstArray, arrayLastIndex);
	for (I32 i = 0; i <= arrayLastIndex; ++i) {
		SV	**element = av_fetch(srcArray, i, 0);
		if (!element || !*element)
			croak("XSUB: mf__copy: av_fetch(srcArray[%d]): Failure\n", i);
		SV	**avStoreOk = av_store(dstArray, i, newSVsv(*element));
		if (!avStoreOk || !*avStoreOk)
			croak("XSUB: mf__copy: av_store(dstArray[%d]): Failure\n", i);
	}
	return dstArray;
}

void
ERCore::mf__push(
	SV	*dstArrayRef,
	SV	*srcArrayRef
)
{
	if (!dstArrayRef || !SvOK(dstArrayRef) || !SvROK(dstArrayRef) || SvTYPE(SvRV(dstArrayRef)) != SVt_PVAV)
		croak("XXX: ERCore::mf__push: dstArrayRef = %p: Not a reference to array\n", dstArrayRef);
	if (!srcArrayRef || !SvOK(srcArrayRef) || !SvROK(srcArrayRef) || SvTYPE(SvRV(srcArrayRef)) != SVt_PVAV)
		croak("XXX: ERCore::mf__push: srcArrayRef = %p: Not a reference to array\n", srcArrayRef);

	AV	*dstArray = (AV *) SvRV(dstArrayRef);
	AV	*srcArray = (AV *) SvRV(srcArrayRef);
	I32	arrayLastIndex = av_len(srcArray);
	for (I32 i = 0; i <= arrayLastIndex; ++i) {
		SV	**element = av_fetch(srcArray, i, 0);
		if (!element || !*element)
			croak("XSUB: mf__push: av_fetch(srcArray[%d]): Failure\n", i);
		av_push(dstArray, newSVsv(*element));
	}
}
