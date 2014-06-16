#include "CFile.h"

#include <string.h>	// strchar, strrchr

CFile::CFile(
	const FILE*	f,
	int		pipe,
	int		autoClose,
	CBuffer*	buffer
)
{
	m__f = const_cast<FILE*>(f);
	m__pipe = pipe;
	if (f && pipe == -1) {
#ifdef __TRACE__
		(void) fprintf(stderr, "__TRACE__: CFile::CFile(): f != NULL but pipe = -1: Ambiguous: Set pipe = 0: Correct?\n");
#endif	// __TRACE__
		m__pipe = 0;
	}
	m__autoClose = autoClose;
	if (!f && !autoClose) {
#ifdef __TRACE__
		(void) fprintf(stderr, "__TRACE__: CFile::CFile(): f = NULL but autoClose = False: Set autoClose = True: Correct?\n");
#endif	// __TRACE__
		autoClose = 0;
	}

	if (buffer) {
		// 외부에서 주어진 버퍼를 사용한다.
		m__autoFree = 0;
		m__buffer = buffer;
	} else {
		// 내부적으로 버퍼를 할당해서 사용한다.
		m__autoFree = 1;
		m__buffer = new CBuffer;
	}
}

CFile::~CFile()
{
	if (m__autoClose && m__f) {
#ifdef __TRACE__
		(void) fprintf(stderr, "__TRACE__: CFile::~CFile(): close...\n");
#endif	// __TRACE__
		// 어느 경우든 리턴값은 고려하지 않는다.
		// 예외처리를 통해서는 가능할까...
		// 아니면 CFile::closeFile()먼저 호출한다.
		if (m__pipe)
			(void) pclose(m__f);
		else
			(void) fclose(m__f);

		m__f = NULL;
		m__pipe = -1;
		m__autoClose = 0;
	}

	if (m__autoFree && m__buffer) {
		delete m__buffer;

		m__buffer = NULL;
	}
}


int
CFile::closeFile(
	void
)
{
	if (!m__f) {
		// 열린 파일스트림이 없다.
#ifdef __TRACE__
		(void) fprintf(stderr, "__TRACE__: CFile::closeFile(): 열린 파일스트림이 없다\n");
#endif	// __TRACE__

		return -2;
	}

	m__autoClose = 0;
#ifdef __TRACE__
	(void) fprintf(stderr, "__TRACE__: CFile::closeFile(): close...\n");
#endif	// __TRACE__
	if (m__pipe)
		return pclose(m__f);
	if (fclose(m__f)) {
#ifdef __TRACE__
		perror("__TRACE__: CFile::closeFile()");
#endif	// __TRACE__

		return -1;
	}
	m__f = NULL;

	return 0;
}

int
CFile::openFile(
	const char*	pathname_or_command,
	const char*	mode,
	int		pipe
)
{
	if (m__f) {
		// 이미 설정된 파일스트림이 있다.
#ifdef __TRACE__
		(void) fprintf(stderr, "__TRACE__: CFile::openFile(): 이미 설정된 파일스트림이 있다\n");
#endif	// __TRACE__
		m__pipe = -1;

		return -2;
	}

#ifdef __TRACE__
	(void) fprintf(stderr, "__TRACE__: CFile::openFile(): open...\n");
#endif	// __TRACE__
	if (pipe) {
		m__f = popen(pathname_or_command, mode);
		if (!m__f) {
#ifdef __TRACE__
			char	msg[128];
			(void) snprintf(msg, 128,
				"__TRACE__: popen(\"%s\", \"%s\")"
				,
				pathname_or_command, mode
			);
			perror(msg);
#endif	// __TRACE__
			m__pipe = -1;
			m__autoClose = 0;

			return -1;
		}
		m__pipe = 1;
	} else {
		m__f = fopen(pathname_or_command, mode);
		if (!m__f) {
#ifdef __TRACE__
			char	msg[128];
			(void) snprintf(msg, 128,
				"__TRACE__: fopen(\"%s\", \"%s\")"
				,
				pathname_or_command, mode
			);
			perror(msg);
#endif	// __TRACE__
			m__pipe = -1;
			m__autoClose = 0;

			return -1;
		}
		m__pipe = 0;
	}

	m__autoClose = 1;
	return 0;
}


// 파일스트림(FILE*)으로부터 줄단위로 정보를 얻는다.
int
CFile::readline(
	void
)
{
	if (!m__f) {
		(void) fprintf(stderr,
			"__TRACE__: CFile::readline(): 아직 파일스트림이 설정되지 않았다: "
			"CFile::openFile()을 먼저 사용해야 한다\n"
		);

		return -3;
	}

	CBuffer&	buffer = *m__buffer;
	// 버퍼를 초기화한다.
	(void*) buffer.setDataSize(0);
	*(buffer.getBuf()) = '\0';
	do {
		// 적어도 80바이트의 공간을 확보한다.
		while (buffer.getBufSizeForNewData() < 80) {
			if (!buffer.incBuf()) {
				(void) fprintf(stderr, "CFile::readline: CBuffer::incBuf(): Failure\n");
				// 읽어들인 데이터가 있다면 모두 무효화한다.
				(void*) buffer.setDataSize(0);
				*(buffer.getBuf()) = '\0';

				return -2;
			}
		}

		char	*buf = buffer.getBufForNewData();
		if (!fgets(buf, buffer.getBufSizeForNewData(), m__f)) {
			// 파일스트림(FILE*)에 문제가 있거나 더 이상 읽은 내용이 없다.
			if (ferror(m__f)) {	// 파일스트림에 문제가 있다.
				perror("ferror()");
				// 읽어들인 데이터가 있다면 모두 무효화한다.
				(void*) buffer.setDataSize(0);
				*(buffer.getBuf()) = '\0';

				return -1;
			}

			break;
		}

		// 줄단위로 정보를 얻었는 지 확인한다. (버퍼의 크기가 충분했다면 이럴 수는 없지만 그건 모르는 일이니...)
		// 읽은 내용의 제일 마지막은 '\0'이므로,
		// 바로 앞의 문자가 '\n'인지 확인해서 한 줄을 읽었는 지 확인한다.
		// 단, EOF인 경우에는 마지막 글자가 '\n'이 아닐 수도 있다. feof를 통해 확인한다.
		char	*p = strchr(buf, '\0');
		size_t	len = p - buf;	// len은 적어도 0보다 크다.
		(void*) buffer.incDataSize(len);
		if (*(p - 1) == '\n' || feof(m__f)) {		// '\n'이거나 EOF을 만났다.
			if (*(p - 1) != '\n') {			// EOF인 경우 제일 마지막 줄이 '\n'로 끝나지 않았다.
				*p = '\n';			// '\n'을 추가하고,
				*(p + 1) = '\0';		// '\0'로 문자열을 마무리한다.
				(void*) buffer.incDataSize(1);	// 한 글자('\n')이 추가되었다.
			}
			// 현재까지 얻는 내용은 완전한 하나의 문장이다.
			// 즉 '\n'로 끝난다.
			break;
		}
		// 버퍼의 크기가 맞지 않아 완전한 한 줄을 읽지 못했다.
		// 이미 읽은 내용을 따로 저장하고 새로 읽는다.
	} while (1);

	return buffer.getDataSize();
}

// 파일스트림(FILE*)으로부터 모든 내용을 읽는다.
int
CFile::readlines(
	int	nLineToBeSkipped,
	int	nLineToBeTailed
)
{
	if (!m__f) {
		(void) fprintf(stderr,
			"__TRACE__: CFile::readline(): 아직 파일스트림이 설정되지 않았다: "
			"CFile::openFile()을 먼저 사용해야 한다\n"
		);

		return -3;
	}

	int	nSkipped = nLineToBeSkipped;
	// 버퍼를 초기화한다.
	CBuffer&	buffer = *m__buffer;
	(void*) buffer.setDataSize(0);
	*(buffer.getBuf()) = '\0';
	do {
		// 적어도 80바이트의 공간을 확보한다.
		while (buffer.getBufSizeForNewData() < 80) {
			if (!buffer.incBuf()) {
				(void) fprintf(stderr, "CFile::readlines: CBuffer::incBuf(): Failure\n");
				// 읽어들인 데이터가 있다면 모두 무효화한다.
				(void*) buffer.setDataSize(0);
				*(buffer.getBuf()) = '\0';

				return -2;
			}
		}

		char	*buf = buffer.getBufForNewData();
		char	*s = fgets(buf, buffer.getBufSizeForNewData(), m__f);
		char	*p = strchr(buf, '\0');
		size_t	len = p - buf;
		(void*) buffer.incDataSize(len);
		if (!s) {
			// 파일스트림(FILE*) 문제가 있거나 더 이상 읽은 내용이 없다.
			if (ferror(m__f)) {	// 파일스트림에 문제가 있다.
				perror("ferror()");
				// 읽어들인 데이터가 있다면 모두 무효화한다.
				(void*) buffer.setDataSize(0);
				*(buffer.getBuf()) = '\0';

				return -1;
			}

			if (feof(m__f)) {
				// 제일 마지막이 '\n'로 끝나게 한다.
				if (len && *(p - 1) != '\n') {
					*p = '\n';
					*(p + 1) = '\0';
					(void*) buffer.incDataSize(1);
				}
			}
			break;
		}
		// 계속해서 읽는다.
		if (nSkipped) {
			--nSkipped;

			(void*) buffer.setDataSize(0);
		}
	} while (1);

	// 이 때, 끝에서 nTailed개의 줄은 제외한다.
	int	nTailed = nLineToBeTailed;
	if (nTailed > 0) {
		const char*	buf = buffer.getBuf();
		do {
			char	*pLastNewline = strrchr(buf, '\n');
			if (!pLastNewline) {
				// 더 이상 데이터는 없다.
				break;
			}
			*pLastNewline = '\0';
			char	*pLastNewline2 = strrchr(buf, '\n');
			if (!pLastNewline2) {
				*(buffer.getBuf()) = '\0';
				// 더 이상 데이터는 없다.
				(void*) buffer.setDataSize(0);
				break;
			}
			*(pLastNewline2 + 1) = '\0';
			(void*) buffer.setDataSize(pLastNewline2 - buf + 1);
		} while (--nTailed);
	}

	return buffer.getDataSize();
}

int
CFile::readAll(
	void
)
{
	if (!m__f) {
		(void) fprintf(stderr,
			"CFile::readAll(): 아직 파일스트림이 설정되지 않았다: "
			"CFile::openFile()을 먼저 사용해야 한다\n"
		);

		return -3;
	}

	// 버퍼를 초기화한다.
	CBuffer&	buffer = *m__buffer;
	(void*) buffer.setDataSize(0);
	do {
		// 적어도 80바이트의 공간을 확보한다.
		while (buffer.getBufSizeForNewData() < 80) {
			if (!buffer.incBuf()) {
				(void) fprintf(stderr, "CFile::readAll: CBuffer::incBuf(): Failure\n");
				// 읽어들인 데이터가 있다면 모두 무효화한다.
				(void*) buffer.setDataSize(0);
				*(buffer.getBuf()) = '\0';

				return -2;
			}
		}
		char	*buf = buffer.getBufForNewData();
		size_t	len = buffer.getBufSizeForNewData();
		size_t	n_read = fread(buf, 1, len, m__f);
		(void*) buffer.incDataSize(n_read);
		if (n_read < len) {
			// eof이거나 문제가 있다.
			if (ferror(m__f)) {
				perror("CFile::readAll: ferror()");
				// 읽어들인 모든 데이터를 무효로 한다.
				(void*) buffer.setDataSize(0);

				return -1;
			}

			break;
		}
	} while (1);

	return buffer.getDataSize();
}

int
CFile::writeAll(
	int	iFlush
)
{
	if (!m__f) {
		(void) fprintf(stderr,
			"CFile::writeAll(): 아직 파일스트림이 설정되지 않았다: "
			"CFile::openFile()을 먼저 사용해야 한다\n"
		);

		return -1;
	}

	CBuffer&	buffer = *m__buffer;
	size_t	n_to_be_written = buffer.getDataSize();
	if (fwrite(buffer.getBuf(), 1, n_to_be_written, m__f) != n_to_be_written) {
		perror("CFile::writeAll(): fwrite(): Failure");

		return -1;
	}
	if (iFlush) {
		if (fflush(this->m__f) < 0) {
			perror("CFile::writeAll: fflush");
			return -1;
		}
	}

	return 0;
}

#if 0
int
CFile::readPacket(
	CPacket::PACKET_TYPE_T&	packetType
)
{
	if (!m__f) {
		(void) fprintf(stderr,
			"ERROR: "
			"__FILE__:= |%s|, __LINE_NO__ = %d: "
			"CFile::readPacket: "
			"아직 파일스트림이 설정되지 않았다: "
			"CFile::openFile()을 먼저 사용해야 한다\n"
			,
			__FILE__, __LINE__
		);

		return -1;
	}

	// 버퍼를 초기화한다.
	CBuffer&	packet = *m__buffer;
	(void*) packet.setDataSize(0);

	// 적어도 5바이트는 있어야 한다. (패킷 크기:= 4바이트, 패킷 헤더:= 1바이트)
	if (packet.setBufSizeAtLeast(1024) < 0) {
		(void) fprintf(stderr,
			"ERROR: "
			"__FILE__:= |%s|, __LINE_NO__ = %d: "
			"CFile::readPacket: "
			"CBuffer::setBufSizeAtLeast(1024): Failure\n"
			,
			__FILE__, __LINE__
		);
		return -1;
	}
	// 먼저 5바이트만 읽어 패킷의 크기가 얼마나 되는 지 알아본다.
	size_t	n_read = fread(packet.getBuf(), 1, 5, this->m__f);
	(void*) packet.incDataSize(n_read);
	if (n_read < 5) {	// 파일의 끝이거나 문제가 있는 경우
		if (ferror(this->m__f)) {
			perror("CFile::readPacket: ferror: Failure");

			// 읽어들인 모든 데이터를 무효로 한다.
			(void*) packet.setDataSize(0);

			return -1;
		}
		packetType = CPacket::PACKET_TYPE__INVALID;
		return n_read;
	}
	unsigned int	packetSize;
	packetType = CPacket::whichType(packet, &packetSize);
	if (packetType == CPacket::PACKET_TYPE__INVALID) {
		(void) fprintf(stderr,
			"ERROR: "
			"__FILE__:= |%s|, __LINE_NO__ = %d: "
			"CFile::readPacket: "
			"CPacket::whichType: PACKET_TYPE__INVALID\n"
			,
			__FILE__, __LINE__
		);

		return -1;
	}
	// 최소한 (packetSize - 1) 바이트의 공간은 있어야 한다. (이미 헤더바이트는 읽었으므로 1 을 뺀다.)
	if (packet.setBufSizeAtLeast(packetSize - 1) < 0) {
		(void) fprintf(stderr,
			"ERROR: "
			"__FILE__:= |%s|, __LINE_NO__ = %d: "
			"CFile::readPacket: "
			"CBuffer::setBufSizeAtLeast(1024): Failure\n"
			,
			__FILE__, __LINE__
		);
		return -1;
	}
	// 패킷의 나머지를 읽는다.
	n_read = fread(packet.getBuf() + 5, 1, packetSize - 1, this->m__f);
	(void*) packet.incDataSize(n_read);
	if (n_read < packetSize - 1) {
		(void) fprintf(stderr,
			"ERROR: "
			"__FILE__:= |%s|, __LINE_NO__ = %d: "
			"CFile::readPacket: "
			"fread(): n_read < packetSize - 1\n"
			,
			__FILE__, __LINE__
		);

		return -1;
	}

	return packetSize;
}
#endif
