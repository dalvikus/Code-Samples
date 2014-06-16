#include "CPacketList.h"
#include "CPacket.h"
#include "CZip.h"

#include <stdio.h>
#include <string.h>

CPacketList::CPacketList(
	void
)
{
#if 1
	m__indexPacket = -1;
	m__packetListSize = 0;
	m__packetList = NULL;
#else
	// 적어도 하나의 패킷을 넣은 공간은 마련한다.
	this->m__packetList = (CBuffer**) calloc(1, sizeof(CBuffer*));
	if (!this->m__packetList) {
		(void) fprintf(stderr, "__TRACE__: CPacket::CPacket: calloc(): Failure\n");
		m__NPacket = -1;
		m__packetListSize = 0;
	}

	CBuffer*	pBuffer = new CBuffer;
	if (!pBuffer) {
		(void) fprintf(stderr, "__TRACE__: CPacket::CPacket: new CBuffer: Failure\n");
		free(this->m__packetList);

		m__NPacket = -1;
		m__packetListSize = 0;
	}

	m__NPacket = 0;
	m__packetListSize = 1;
	this->m__packetList[0] = pBuffer;
#endif
}

CPacketList::~CPacketList()
{
	if (this->m__packetList) {
		for (size_t i = 0; i < m__packetListSize; ++i) {
			if (this->m__packetList[i]) {
				delete this->m__packetList[i];
				this->m__packetList[i] = NULL;
			}
		}

		free(this->m__packetList);
		m__packetList = NULL;
	}
}


#define PACKET_INC_UNIT	10
int
CPacketList::incPacket(
	void
)
{
	CBuffer**	newPacketList = (CBuffer**) realloc(this->m__packetList, sizeof(CBuffer*) * (this->m__packetListSize + PACKET_INC_UNIT));
	if (!newPacketList) {
		(void) fprintf(stderr, "__TRACE__: CPacketList::incPacket: realloc(): Failure\n");

		return -1;
	}
	this->m__packetList = newPacketList;
	// 먼저 NULL 로 초기화한다.
	for (size_t i = this->m__packetListSize; i < this->m__packetListSize + PACKET_INC_UNIT; ++i) {
		this->m__packetList[i] = NULL;
	}
	for (size_t i = this->m__packetListSize; i < this->m__packetListSize + PACKET_INC_UNIT; ++i) {
		this->m__packetList[i] = new CBuffer;
		if (this->m__packetList[i] == NULL) {
			(void) fprintf(stderr, "__TRACE__: CPacketList::incPacket: new CBuffer: Failure\n");

			return -2;
		}
	}
	this->m__packetListSize += PACKET_INC_UNIT;

	return 0;
}
#undef PACKET_INC_UNIT

int
CPacketList::pyPack(
	const PyObject*	dataList,
	size_t		packetSizeUpperBound
)
{
	int	iReturnVal = 0;
	PyGILState_STATE	gstate = PyGILState_Ensure();

do {
	this->m__indexPacket = -1;	// 첫 번째 패킷 공간에서부터 채워나간다.

	if (!dataList) {
		(void) fprintf(stderr, "__TRACE__: CPacketList::pyPack: dataList = NULL\n");
		iReturnVal = -1;

		break;
	}

	if (!PyList_Check(dataList)) {
		(void) fprintf(stderr, "__TRACE__: CPacketList::pyPack: dataList: Not a list: ");
		(void) PyObject_Print(const_cast<PyObject*>(dataList), stderr, 0);
		(void) fprintf(stderr, "\n");
		iReturnVal = -2;

		break;
	}

	for (Py_ssize_t index = 0; index < PyList_Size(const_cast<PyObject*>(dataList)); ++index) {
		PyObject*	objData = PyList_GetItem(const_cast<PyObject*>(dataList), index);	// Borrowed reference
		if (!objData) {
			(void) fprintf(stderr, "__TRACE__: CPacketList::pyPack: PyList_GetItem(index = %d): Failure\n", index);
			iReturnVal = -3;

			break;
		}

		// 역시 List 이어야한다.
		if (!PyList_Check(objData)) {
			(void) fprintf(stderr, "__TRACE__: CPacketList::pyPack: index = %d: Data: Not a list: ", index);
			(void) PyObject_Print(objData, stderr, 0);
			(void) fprintf(stderr, "\n");
			iReturnVal = -4;

			break;
		}
		Py_ssize_t	len = PyList_Size(objData);

		// 첫 번째값을 읽어 성능데이터인지 장애데이터인지 판단한다.
		PyObject*	objType = PyList_GetItem(objData, 0);
		if (!objType) {
			(void) fprintf(stderr, "__TRACE__: CPacketList::pyPack: index = %d: PyList_GetItem(objType): Failure\n", index);
			iReturnVal = -5;

			break;
		}
		// PACKET_TYPE__ERR 또는 PACKET_TYPE__PERF 이어야한다.
		int	iType = PyInt_AsLong(objType);
		if (
			iType == -1 ||
			(iType != CPacket::PACKET_TYPE__ERR && iType != CPacket::PACKET_TYPE__PERF)
		) {	// 문제가 있거나 값이 -1이다. 후자도 역시 문제가 있는 경우이다.
			(void) fprintf(stderr, "__TRACE__: CPacket::byte: index = %d: Type: Invalid type: ", index);
			(void) PyObject_Print(objType, stderr, 0);
			(void) fprintf(stderr, "\n");
			iReturnVal = -6;
			break;
		}

		// Job ID 를 얻는다.
		PyObject*	objJobID = PyList_GetItem(objData, 1);
		if (!objJobID) {
			(void) fprintf(stderr, "__TRACE__: CPacketList::pyPack: index = %d: PyList_GetItem(objJobID): Failure\n", index);
			iReturnVal = -7;

			break;
		}
		int	jobID = PyInt_AsLong(objJobID);
		if (jobID == -1) {	// 문제가 있거나 값이 -1이다. 값이 -1 즉 0xFFFFFFFF 인 JOB ID 는 없다.
			(void) fprintf(stderr, "__TRACE__: CPacket::byte: index = %d: Invalid Job ID = ", index);
			(void) PyObject_Print(objJobID, stderr, 0);
			(void) fprintf(stderr, "\n");
			iReturnVal = -8;
			break;
		}

		if (iType == CPacket::PACKET_TYPE__ERR) {	// 장애데이터
//			[PACKET_TYPE__ERR, JOB_ID, EVENT_ID, 심각도, "장애값", "상세설명"],
			// EVENT ID 를 얻는다.
			PyObject*	objEventID = PyList_GetItem(objData, 2);
			if (!objEventID) {
				(void) fprintf(stderr, "__TRACE__: CPacketList::pyPack: index = %d: PyList_GetItem(objEventID): Failure\n", index);
				iReturnVal = -9;

				break;
			}
			int	eventID = PyInt_AsLong(objEventID);
			if (eventID == -1) {	// 문제가 있거나 값이 -1이다. 값이 -1 즉 0xFFFFFFFF 인 EVENT ID 는 없다.
				(void) fprintf(stderr, "__TRACE__: CPacket::byte: index = %d: Invalid Event ID = ", index);
				(void) PyObject_Print(objEventID, stderr, 0);
				(void) fprintf(stderr, "\n");
				iReturnVal = -10;
				break;
			}

			// Severity 를 얻는다.
			PyObject*	objSeverity = PyList_GetItem(objData, 3);
			if (!objSeverity) {
				(void) fprintf(stderr, "__TRACE__: CPacketList::pyPack: index = %d: PyList_GetItem(objSeverity): Failure\n", index);
				iReturnVal = -11;

				break;
			}
			int	severity = PyInt_AsLong(objSeverity);
			if (
				severity == -1 ||	// 문제가 있거나 값이 -1이다.
							// 값이 -1 즉 0xFFFFFFFF 인 Severity 는 없다.
				(severity < CPacket::SEV__CLEAR || severity > CPacket::SEV__INFORMATION)
			) {
				(void) fprintf(stderr, "__TRACE__: CPacket::byte: index = %d: Invalid Severity = ", index);
				(void) PyObject_Print(objSeverity, stderr, 0);
				(void) fprintf(stderr, "\n");
				iReturnVal = -12;
				break;
			}

			const char*	strErrValue = "\0";
			const char*	strErrDetail = NULL;
			if (severity == CPacket::SEV__CLEAR && len > 4) {
				(void) fprintf(stderr, "__TRACE__: WARNING: CPacketList::pyPack: index = %d: PACKET_TYPE__ERR: SEV__CLEAR: Too many data: "
					,
					index
				);
				(void) PyObject_Print(objData, stderr, 0);
				(void) fprintf(stderr, ": Ignored\n");
			}
			if (severity != CPacket::SEV__CLEAR) {
				// 장애값에 해당하는 문자열을 얻는다.
				if (len < 5) {
					(void) fprintf(stderr,
						"__TRACE__: CPacketList::pyPack: index = %d: PACKET_TYPE__ERR: No Value\n"
						,
						index
					);
					iReturnVal = -13;

					break;
				}
				PyObject*	objErrValue = PyList_GetItem(objData, 4);
				if (!objErrValue) {
					(void) fprintf(stderr,
						"__TRACE__: CPacketList::pyPack: index = %d: PyList_GetItem(objErrValue): Failure\n"
						,
						index
					);
					iReturnVal = -14;

					break;
				}
				if (!(strErrValue = PyString_AsString(objErrValue))) {
					(void) fprintf(stderr,
						"__TRACE__: CPacketList::pyPack: index = %d: PyString_AsString(objErrValue): Failure\n"
						,
						index
					);
					iReturnVal = -15;

					break;
				}

				if (len > 5) {
					// 상세설명에 해당하는 문자열을 얻는다.
					PyObject*	objErrDetail = PyList_GetItem(objData, 5);
					if (!objErrDetail) {
						(void) fprintf(stderr, "__TRACE__: CPacketList::pyPack: index = %d: PyList_GetItem(objErrDetail): Failure\n", index);
						iReturnVal = -16;

						break;
					}
					if (!(strErrDetail = PyString_AsString(objErrDetail))) {
						(void) fprintf(stderr,
							"__TRACE__: CPacketList::pyPack: index = %d: PyString_AsString(objErrDetail): Failure\n"
							,
							index
						);
						iReturnVal = -17;

						break;
					}
				}

				if (len > 6) {
					(void) fprintf(stderr,
						"__TRACE__: WARNING: CPacketList::pyPack: index = %d: PACKET_TYPE__ERR: Too many data: "
						,
						index
					);
					(void) PyObject_Print(objData, stderr, 0);
					(void) fprintf(stderr, ": Ignored\n");
				}
			}

#if 1	// 읽어들인 값들은 확인한다.
{
		(void) printf(
			"PACKET_TYPE__ERR: Job ID = %d, Event ID = %d, Severity = %d, Value = |%s|, Detail = |%s|\n"
			,
			jobID, eventID, severity,
			strErrValue, strErrDetail ? strErrDetail : "N/A"
		);
}
#endif
			++this->m__indexPacket;
			if ((size_t) this->m__indexPacket == m__packetListSize) {
				if (this->incPacket() < 0) {
					(void) fprintf(stderr, "__TRACE__: CPacketList::pyPack: CPacket::incPack(): Failure\n");
					iReturnVal = -200;

					break;
				}
			}
			CBuffer&	packet = *this->m__packetList[this->m__indexPacket];
			if (CPacket::packErrorData(
				packet,

				jobID, eventID, severity, strErrValue, strErrDetail,

				packetSizeUpperBound
			) < 0) {
				(void) fprintf(stderr, "__TRACE__: CPacketList::pyPack: index = %d: CPacket::packErrorData(): Failure\n"
						,
					index
				);
				iReturnVal = -100;

				break;
			}
			(void) printf(
				"__TRACE__: CPacketList::pyPack: index = %d: # of byte(s) in packet = %d\n"
				,
				index, packet.getDataSize()
			);
#if 1	// 패킹을 풀어 원래 값과 비교한다.
{
	unsigned int	jobID;
	unsigned int	eventID;
	unsigned int	severity;
	CBuffer		strErrValue;
	int		isDetail;
	CBuffer		strErrDetail;
	if (CPacket::unpackErrorData(
		packet,
		jobID, eventID, severity, strErrValue, isDetail, strErrDetail
	) < 0) {
		(void) fprintf(stderr,
			"__TRACE__: CPacketList::pyPack: index = %d: CPacket::unpackErrorData(): Failure\n"
			,
			index
		);
		iReturnVal = -101;

		break;
	}
	(void) printf(
		"JOB_ID = %d, EVENT_ID = %d, "
		"Severity = %s, "
		"Error Value:= |%s|, "
		"Error Details:= |%s|\n"
		,
		jobID, eventID,
		severity == CPacket::SEV__CLEAR ? "SEV__CLEAR" : (
		severity == CPacket::SEV__FATAL ? "SEV__FATAL" : (
		severity == CPacket::SEV__CRITICAL ? "SEV__CRITICAL" : (
		severity == CPacket::SEV__WARNING ? "SEV__WARNING" : (
		severity == CPacket::SEV__ATTENTION ? "SEV__ATTENTION" : "SEV__INFORMATION")))),
		strErrValue.getBuf(),
		isDetail ? strErrDetail.getBuf() : "None"
	);
}
#endif

			continue;
		}
		// 성능데이터
//		[PACKET_TYPE__PERF, JOB_ID, 성능값, "추가내용"],

		// 성능값을 얻는다.
		if (len < 3) {
			(void) fprintf(stderr,
				"__TRACE__: ERROR: CPacketList::pyPack: index = %d: PACKET_TYPE__PERF: No Value: "
				,
				index
			);
			(void) PyObject_Print(objData, stderr, 0);
			(void) fprintf(stderr, "\n");
			iReturnVal = -18;

			break;
		}
		PyObject*	objPerfValue = PyList_GetItem(objData, 2);
		if (!objPerfValue) {
			(void) fprintf(stderr, "__TRACE__: CPacketList::pyPack: index = %d: PyList_GetItem(objPerfValue): Failure\n", index);
			iReturnVal = -19;

			break;
		}
		// 어떤 형인지 알아본다.
		CPacket::TYPE_VALUE_T	typeValue;
		// 정수형인가?
		if (PyInt_Check(objPerfValue)) {
			typeValue.type = CPacket::VAL_TYPE__INTERGER;
			typeValue.value.ival = PyInt_AsLong(objPerfValue);
			if (PyErr_Occurred()) {
				(void) fprintf(stderr,
					"__TRACE__: CPacketList::pyPack: index = %d: PACKET_TYPE__PERF: objPerlValue: "
					,
					index
				);
				(void) PyObject_Print(objPerfValue, stderr, 0);
				(void) fprintf(stderr, ": PyInt_AsLong(): Failure\n");
				iReturnVal = -20;

				break;
			}
		} else if (PyFloat_Check(objPerfValue)) {
			typeValue.type = CPacket::VAL_TYPE__FIXED_POINT;
			typeValue.value.fixedPoint.fval = PyFloat_AsDouble(objPerfValue);
			if (PyErr_Occurred()) {
				(void) fprintf(stderr,
					"__TRACE__: CPacketList::pyPack: index = %d: PACKET_TYPE__PERF: objPerlValue: "
					,
					index
				);
				(void) PyObject_Print(objPerfValue, stderr, 0);
				(void) fprintf(stderr, ": PyFloat_AsDouble(): Failure\n");
				iReturnVal = -21;

				break;
			}
			// 소수 몇 째자리인 지 알아본다.
////////////////////////////////////////////////////////////////////////
			float	f = typeValue.value.fixedPoint.fval;
			int	iFixed = -1;
			int	iPoint = -1;
			for (int i = 0; i < 4; ++i) {
				iFixed = (int) f;
//				(void) fprintf(stderr, "f = %f, iFixed = %d\n", f, iFixed);
				if (fabsf(iFixed - f) < 0.0000001) {
					iPoint = i;	// 소수 iPoint 번째 수이다.
					break;
				}
				f *= 10;
			}
			if (iPoint == -1) {
				iPoint = 3;
				// iPoint = 3인 경우에는 마지막 자리가 달라질 수 있다.
				//	0.005	=> (4, 3):= 4/1000 = 0.004
				// +/- 0.001 의 오차가 있을 수 있다.
			}
			typeValue.value.fixedPoint.iFixed = iFixed;
			typeValue.value.fixedPoint.iPoint = iPoint;
////////////////////////////////////////////////////////////////////////
		} else if (PyString_Check(objPerfValue)) {
			typeValue.type = CPacket::VAL_TYPE__STRING;
			if (!(typeValue.value.str = PyString_AsString(objPerfValue))) {
				(void) fprintf(stderr,
					"__TRACE__: CPacketList::pyPack: index = %d: PACKET_TYPE__PERF: objPerlValue: "
					,
					index
				);
				(void) PyObject_Print(objPerfValue, stderr, 0);
				(void) fprintf(stderr, ": PyString_AsString(): Failure\n");
				iReturnVal = -22;

				break;
			}
		} else {
			(void) fprintf(stderr,
				"__TRACE__: CPacketList::pyPack: index = %d: objPerValue: "
				,
				index
			);
			(void) PyObject_Print(objPerfValue, stderr, 0);
			(void) fprintf(stderr, ": Invalid Type\n");
			iReturnVal = -23;

			break;
		}

		const char*	strPerfExtra = NULL;
		if (len > 3) {
			// 추가내용을 얻는다.
			PyObject*	objPerfExtra = PyList_GetItem(objData, 3);
			if (!objPerfExtra) {
				(void) fprintf(stderr, "__TRACE__: CPacketList::pyPack: index = %d: PyList_GetItem(objPerfExtra): Failure\n", index);
				iReturnVal = -24;

				break;
			}
			if (!(strPerfExtra = PyString_AsString(objPerfExtra))) {
				(void) fprintf(stderr,
					"__TRACE__: CPacketList::pyPack: index = %d: PyString_AsString(objPerfExtra): Failure\n"
					,
					index
				);
			this->incPacket();
				iReturnVal = -26;

				break;
			}
		}

		if (len > 4) {
			(void) fprintf(stderr,
				"__TRACE__: WARNING: CPacketList::pyPack: index = %d: PACKET_TYPE__ERR: Too many data: "
				,
				index
			);
			(void) PyObject_Print(objData, stderr, 0);
			(void) fprintf(stderr, ": Ignored\n");
		}

#if 1	// 읽어들인 값들을 확인한다.
{
	char	strValue[32];
	if (typeValue.type == CPacket::VAL_TYPE__INTERGER) {
		(void) snprintf(strValue, 32, "%d<INT>", typeValue.value.ival);
	} else if (typeValue.type == CPacket::VAL_TYPE__FIXED_POINT) {
		(void) snprintf(strValue, 32, "%.3f<FIXED>(%d, %d)",
			typeValue.value.fixedPoint.fval,
			typeValue.value.fixedPoint.iFixed, typeValue.value.fixedPoint.iPoint
		);
	} else {	// VAL_TYPE__STRING
		(void) snprintf(strValue, 32, "\"%s\"<STR>", typeValue.value.str);
	}
	(void) printf(
		"PACKET_TYPE__PERF: Job ID = %d, Value = %s, Extra = |%s|\n"
		,
		jobID, strValue, strPerfExtra ? strPerfExtra : "N/A"
	);
}
#endif
		++this->m__indexPacket;
		if ((size_t) this->m__indexPacket == m__packetListSize) {
			if (this->incPacket() < 0) {
				(void) fprintf(stderr, "__TRACE__: CPacketList::pyPack: CPacket::incPack(): Failure\n");
				iReturnVal = -200;

				break;
			}
		}
		CBuffer&	packet = *this->m__packetList[this->m__indexPacket];
		if (CPacket::packPerfData(
				packet,

				jobID, typeValue, strPerfExtra,

				packetSizeUpperBound
		) < 0) {
			(void) fprintf(stderr,
				"__TRACE__: CPacketList::pyPack: index = %d: CPacket::packPerfData(): Failure\n"
				,
				index
			);
			iReturnVal = -101;

			break;
		}
		(void) printf("# of byte(s) in packet = %d\n", packet.getDataSize());
#if 1	// 패킹된 것을 풀어 원래 값과 비교한다.
{
	unsigned int	jobID;
	CPacket::TYPE_VALUE_T	typeValue;
	int		isExtra;
	CBuffer		strPerfExtra;
	if (CPacket::unpackPerfData(packet, jobID, typeValue, isExtra, strPerfExtra) < 0) {
		(void) fprintf(stderr, "__TRACE__: CPacket::unpackPerfData(): Failure\n");

		break;
	}
	char	strValue[64];
	if (typeValue.type == CPacket::VAL_TYPE__INTERGER) {
		(void) snprintf(strValue, 64, "%d<INT>", typeValue.value.ival);
	} else if (typeValue.type == CPacket::VAL_TYPE__FIXED_POINT) {
		(void) snprintf(strValue, 64, "%.3f<FIXED_POINT>(%d, %d)",
			typeValue.value.fixedPoint.fval,
			typeValue.value.fixedPoint.iFixed, typeValue.value.fixedPoint.iPoint
		);
	} else {	// VAL_TYPE__STRING
		(void) snprintf(strValue, 64, "\"%s\"<STR>", typeValue.buffer.getBuf());
	}
	const CPacket::VAL_TYPE&	type = typeValue.type;
	(void) printf(
		"JOB_ID = %d, Type:= %s, Value: %s, Extra:= |%s|\n"
		,
		jobID,
		type == CPacket::VAL_TYPE__INTERGER ? "VAL_TYPE__INTERGER" : (
		type == CPacket::VAL_TYPE__FIXED_POINT ? "VAL_TYPE__FIXED_POINT" : "VAL_TYPE__STRING"),
		strValue,
		isExtra ? strPerfExtra.getBuf() : "None"
	);
}
#endif
	}

} while (0);
	if (iReturnVal < 0) {
		if (PyErr_Occurred()) {
			PyErr_Print();
		}

		// (있다면) 지금까지의 모든 내용을 무효로한다.
		for (int i = 0; i < 1 + this->m__indexPacket; ++i) {
			(void*) this->m__packetList[i]->setDataSize(0);
		}
	} else {
		iReturnVal = 1 + this->m__indexPacket;	// 이 함수에서 만들어진 총 패킷의 갯수
	}

	PyGILState_Release(gstate);
	return iReturnVal;
}
