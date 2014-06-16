#ifndef __ER_CORE_H__
#define __ER_CORE_H__


// 펄 헤더파일을 사용하기위하여...
#include "EXTERN.h"	// "perl.h"전에 포함해야 한다!
#include "perl.h"
#include "XSUB.h"	// "XSUB.h"도 포함해야 한다!

class ERCore
{
public:
	ERCore(int *piReturnVal);
	~ERCore();

public:
	int	mf__Read(
		SV	*fileHandle,
		int	&enumRIBDirective,
		SV	**RIBDirective,
		SV	**RIBDirectiveContentRef
	);
public:
	// ERCore.cpp의 evalSubExpr에서 정의된, 간단한 펄 함수(ERCoreSub_XXX)에 대한 Wrapper함수들...
	static int	mf__ERCoreSub_OpenWrapper(SV *fileHandle, SV *fileOpenExpr);
	static int	mf__ERCoreSub_CloseWrapper(SV *fileHandle);
	static SV	*mf__ERCoreSub_joinWrapper(SV *svArrayRef);
	static void	mf__ERCoreSub_printWrapper(SV *fileHandle, SV *svLine);


private:
	HV	*m__HoHChunk;
	HV	*m__hGlobalVariable;
	HV	*m__hCommandLineOption;
public:
	HV	*mf__getHoHChunk(void)			{return m__HoHChunk;}

private:
	int	m__gDeclare;
	SV	*m__strDeclare;		// free if non-zero
public:
	void	mf__setgDeclare(int gDeclare)		{m__gDeclare = gDeclare;}
	int	mf__getgDeclare(void)			{return m__gDeclare;}
	void	mf__setstrDeclare(SV *strDeclare)	{m__strDeclare = strDeclare;}
	SV	*mf__getstrDeclare(void)		{return m__strDeclare;}

private:
	SV	*m__path;
	SV	*m__dspyName;
	SV	*m__dspyName0;
public:
	void	mf__setpath(SV *path)			{m__path = path;}
	SV	*mf__getpath(void)			{return m__path;}
	void	mf__setdspyName(SV *dspyName)		{m__dspyName = dspyName;}
	SV	*mf__getdspyName(void)			{return m__dspyName;}
	void	mf__setdspyName0(SV *dspyName0)		{m__dspyName0 = dspyName0;}
	SV	*mf__getdspyName0(void)			{return m__dspyName0;}


public:
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
	int	mf__setVar(const char *varName, SV *varValue, int varScope);

	// $ERModule::ghVar{ 'command line option' }이 가리키는 해쉬에서
	// keyName에 해당하는 value를 얻는다.
	SV	*mf__getCommandLineOption(const char *keyName);

	// 주어진 문자열이 해당 정규식(regular expression)을 가지는 지 알아본다.
	// 괄호로 묶인 일치하는 것들에 대한 배열을 리턴한다.
	// 펄에서 해당 배열을 저장하였으므로 펄이 끝날 때 알아서 없앤다.
	static AV	*mf__re(const char *string, const char *pattern);

	static AV	*mf__copy(SV *srcArrayRef);
	static void	mf__push(SV *dstArrayRef, SV *srcArrayRef);

	// ERCommonSub에서 정의된 함수를 호출하는 Wrapper함수들...
public:
	static SV	*mf__mkdir2Wrapper(SV *oldUNIXPath);
	static void	mf__restoreArrayRefWrapper(SV *aref);
	static void	mf__printArrayRefWrapper(SV *fileHandle, SV *aref);
	static void	mf__appendArrayRefWrapper(SV *dstArrayRef, SV *srcArrayRef);
};


#endif	// !__ER_CORE_H__
