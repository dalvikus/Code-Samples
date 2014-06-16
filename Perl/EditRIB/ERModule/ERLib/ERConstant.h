/*
 * getLineByDirective함수의 리턴값
 */
#define ER__GET_RIB_DIRECTIVE__ERROR		(-2)
#define ER__GET_RIB_DIRECTIVE__UNKNOWN		(-1)	// 알려지지 않은 "RIB Directive"인 경우
#define ER__GET_RIB_DIRECTIVE__EOF		0	// EOF을 만났을 때
#define ER__GET_RIB_DIRECTIVE__VALID		1	// 알려진 "RIB Directive"인 경우
#define ER__GET_RIB_DIRECTIVE__EXCEPTION	2	// 주석이나 공백줄 같이 예외적인 경우


/*
 * setVar함수를 사용해서 정의하는 변수가
 * 어느 범위(scope)에 있는 변수를 설정할 지를 판단할 때 사용한다.
 */
#define ER__VAR_SCOPE__GLOBAL			(1 << 0)
#define ER__VAR_SCOPE__JOB_CHUNK		(1 << 1)
#define ER__VAR_SCOPE__ALL			(ER__VAR_SCOPE__GLOBAL | ER__VAR_SCOPE__JOB_CHUNK)


/*
 * 관심있는 "RIB Directive"를 "enum"형으로 분류한다.
 */
#define ER__ENUM_RIB_DIRECTIVE__UNDEF			(-2)
#define ER__ENUM_RIB_DIRECTIVE__DONT_CARE		(-1)
#define ER__ENUM_RIB_DIRECTIVE__FRAME_BEGIN		(0)
#define ER__ENUM_RIB_DIRECTIVE__FRAME_END		(1)
#define ER__ENUM_RIB_DIRECTIVE__DISPLAY			(2)
#define ER__ENUM_RIB_DIRECTIVE__WORLD_BEGIN		(3)
#define ER__ENUM_RIB_DIRECTIVE__WORLD_END		(4)
#define ER__ENUM_RIB_DIRECTIVE__ATTRIBUTE_BEGIN		(5)
#define ER__ENUM_RIB_DIRECTIVE__ATTRIBUTE_END		(6)
#define ER__ENUM_RIB_DIRECTIVE__TRANSFORM_BEGIN		(7)
#define ER__ENUM_RIB_DIRECTIVE__TRANSFORM_END		(8)
#define ER__ENUM_RIB_DIRECTIVE__LIGHTSOURCE		(9)
#define ER__ENUM_RIB_DIRECTIVE__SURFACE			(10)
#define ER__ENUM_RIB_DIRECTIVE__DISPLACEMENT		(11)
#define ER__ENUM_RIB_DIRECTIVE__ATTRIBUTE		(12)
#define ER__ENUM_RIB_DIRECTIVE__GEOMETRIC_PRIMITIVE	(13)


/*
 * Declare부분의 관점에서 현재 읽은 RIB Directive가 어디에 속하는 지를 말해준다.
 */
#define ER__DECLARE__NO_FRAME				(1)
#define ER__DECLARE__FRAME_BLOCK			(0)
#define ER__DECLARE__FRAME_BLOCK_OUTSIDE		(-1)


/*
 * RIB Directive에 해당하는 내용이 추가되었는 지 아니면 삭제될 예정인지를 말해준다.
 */
#define ER__CONTENT_STATE__DELETE		(-1)
#define ER__CONTENT_STATE__ADD			(1)


/*
 * RIB Box의 유형
 */
#define ER__ENUM_RIB_BOX__DECLARE__JUST_BEFORE_FIRST_FRAME_BEGIN		(0)
#define ER__ENUM_RIB_BOX__FRAME__JUST_BEFORE_DISPLAY				(1)
#define ER__ENUM_RIB_BOX__FRAME__JUST_BEFORE_WORLD_BEGIN			(2)
#define ER__ENUM_RIB_BOX__WORLD__JUST_BEFORE_FIRST_LIGHTSOURCE_BLOCK		(3)
#define ER__ENUM_RIB_BOX__WORLD__JUST_BEFORE_FIRST_GEOMETRIC_PRIMITIVE_BLOCK	(4)
#define ER__ENUM_RIB_BOX__WORLD__JUST_AFTER_LAST_OBJECT_BLOCK			(5)
#define ER__ENUM_RIB_BOX__LIGHT__JUST_AFTER_ATTRIBUTE				(6)
#define ER__ENUM_RIB_BOX__OBJECT__JUST_AFTER_ATTRIBUTE				(7)
#define ER__ENUM_RIB_BOX__OBJECT__JUST_BEFORE_LIGHT_SHADER			(8)
#define ER__ENUM_RIB_BOX__OBJECT__JUST_BEFORE_SURFACE_SHADER			(9)
#define ER__ENUM_RIB_BOX__OBJECT__JUST_BEFORE_GEOMETRIC_PRIMITIVE		(10)


/*
 * Job chunk의 유형
 */
#define ER__CHUNK_TYPE__SHD	(1)
#define ER__CHUNK_TYPE__RS	(2)
#define ER__CHUNK_TYPE__BAKE	(1 << 8)


/*
 * Render status를 결정하는 비트
 */
#define ER__RS__CAST_SHADOW	(1 << 0)
#define ER__RS__RECV_SHADOW	(1 << 1)
#define ER__RS__VISIBILITY	(1 << 2)
#define ER__RS__VIS_REFL	(1 << 3)
#define ER__RS__VIS_REFR	(1 << 4)
#define ER__RS__RT		(ER__RS__VIS_REFL | ER__RS__VIS_REFR)
#define ER__RS__SSSC		(1 << 29)
#define ER__RS__MATTE		(1 << 30)
#define ER__RS__DELETE		(1 << 31)


/*
 * 스택의 유형
 */
#define ER__STACK_TYPE__ATTRIBUTE	(0)
#define ER__STACK_TYPE__TRANSFORM	(1)
#define ER__STACK_TYPE__FRAME		(2)


/*
 * 스택에서 읽어들이는 RIB Directive의 내용을 처리하는 방법
 */
#define ER__CONTENT_FATE__UNDEF		(-2)
#define ER__CONTENT_FATE__DELETE	(-1)
#define ER__CONTENT_FATE__PRINT		(0)
#define ER__CONTENT_FATE__SAVE		(1)


/*
 * 쉐이더의 유형
 */
#define ER__SHADER_TYPE__LIGHTSOURCE	(0)
#define ER__SHADER_TYPE__SURFACE	(1)
