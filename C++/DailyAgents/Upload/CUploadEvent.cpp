#define PERL_EXE		S"C:/Daily/cygwin/bin/perl.exe"
#define MAKE_DAILY_IMAGE_PL	S"C:/Daily/Tools/bin/mkDailyImage.pl"

#include <Windows.h>
using namespace	System;

#include "CUpload.h"
using namespace	Upload;

#include <stdio.h>


System::Void
Upload::CUpload::eventOnLoad(
	System::Object		*sender,
	System::EventArgs	*e
)
{
	// Identity
	this->textBoxGroup->Text	= S"";
	this->textBoxUser->Text		= S"";

	// Project
	this->textBoxShow->Text		= S"";
	this->textBoxSeq->Text		= S"";
	this->textBoxShot->Text		= S"";

	// Image
	this->textBoxPathname->Text	= S"";
	this->textBoxFrame->Text	= S"";
	this->textBoxRevision->Text	= S"";

	// Note
	this->richTextBoxComment->Text	= S"";


	this->textBoxTdAnim->Text	= S"";
	this->textBoxTdTexture->Text	= S"";
	this->textBoxPixelSamples->Text	= S"";
	this->textBoxShadingRate->Text	= S"";

	this->textBoxNonTdAnim->Text	= S"";
	this->textBoxNonTdMdl->Text	= S"";
	this->textBoxNonTdTexture->Text	= S"";
	this->textBoxNonTdFx->Text	= S"";
	this->textBoxNonTdTd->Text	= S"";
	this->textBoxNonTdComp->Text	= S"";
	this->textBoxNonTdSetup->Text	= S"";
	this->textBoxNonTdConcept->Text	= S"";
	this->textBoxFocalLength->Text	= S"";
	this->textBoxFPS->Text		= S"";


	// Message
	this->richTextBoxMessage->Text	= S"";


	// Logon
	this->textBoxUsername->Text		= S"";
	this->textBoxPassword->Text		= S"";



	this->tabPageCommonInfo->Enabled	= false;
	this->tabPageSpecificInfo->Enabled	= false;
	this->tabControlUpload->SelectedIndex	= 3;
	this->tabControlUpload->Refresh();
}

System::Void
Upload::CUpload::eventOnClickOpen(
	System::Object		*sender,
	System::EventArgs	*e
)
{
	OpenFileDialog	*imageOpen = new OpenFileDialog();
	imageOpen->Filter = "Image (*.tif;*.iff;*.gif;*.bmp;*.tga)|*.tif;*.iff;*.gif;*.bmp;*.tga";
	imageOpen->Title = "Choose a Image File";

	if (imageOpen->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
		this->textBoxPathname->Text = imageOpen->FileName->Replace('\\', '/');
	}
}


System::Void
Upload::CUpload::eventOnSelectedIndexChanged(
	System::Object		*sender,
	System::EventArgs	*e
)
{
	if (String::Compare(this->textBoxGroup->Text, S"TD", true) == 0) {
		this->groupBoxTd->Visible	= true;
		this->groupBoxNonTd->Visible	= false;
	} else {
		this->groupBoxTd->Visible	= false;
		this->groupBoxNonTd->Visible	= true;
	}
}

System::Void
Upload::CUpload::eventOnClickNext(
	System::Object		*sender,
	System::EventArgs	*e
)
{
	this->tabControlUpload->SelectedIndex = 1;

	eventOnSelectedIndexChanged(sender, e);
}

#define	END_OF_PROCESS_STRING	"\n~!@345^&*90-+`12#$%678()_=\n"
// Thread's parameters:
static HANDLE	hChildStdoutWr;
static HANDLE	hEvent;
DWORD WINAPI
ThreadProc(
	LPVOID	lpParam
)
{
#define ERROR_BUFFER_SIZE	1024
	char	WriteErrorBuffer[ERROR_BUFFER_SIZE];		// 에러가 생겼을 때, 에러 내용을 적는 버퍼

	PROCESS_INFORMATION	pi;
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

	STARTUPINFO		si;
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
//	si.wShowWindow = SW_HIDE;
	si.hStdError = hChildStdoutWr;
	si.hStdOutput = hChildStdoutWr;
//	si.hStdInput = hChildStdinRd;
//	si.dwFlags |= STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.dwFlags |= STARTF_USESTDHANDLES;
	// Start the child process.
	if(!CreateProcess(
		NULL,		// No module name (use command line)
		(LPTSTR) lpParam,
		NULL,		// Process handle not inheritable
		NULL,		// Thread handle not inheritable
		TRUE,		// Set handle inheritance to FALSE
//CREATE_NO_WINDOW, DETACHED_PROCESS
		DETACHED_PROCESS,		// No creation flags
		NULL,		// Use parent's environment block
		NULL,		// Use parent's starting directory 
		&si,		// Pointer to STARTUPINFO structure
		&pi		// Pointer to PROCESS_INFORMATION structure
	)) {
		int	nNumberOfBytesToWrite;
		if ((nNumberOfBytesToWrite = sprintf(WriteErrorBuffer, "Error: CreateProcess(): Failure: 0x%08X\n", GetLastError())) == -1) {
			return (1 << 16) + (1 << 12) + 1;
		}
		DWORD	dwNumberOfBytesWritten;
		if (!WriteFile(hChildStdoutWr, WriteErrorBuffer, (DWORD) nNumberOfBytesToWrite, &dwNumberOfBytesWritten, 0)) {
			return (1 << 16) + (1 << 8) + 1;
		}
		return (1 << 16) + (1 << 4) + 1;
	}

	if (!SetEvent(hEvent)) {
		(void) fprintf(stderr, "Error: SetEvent(hEvent): Failure: 0x%08X\n", GetLastError());
		return (1 << 16) + 2;
	}

	// Wait until child process exits.
	if (WaitForSingleObject(pi.hProcess, INFINITE) == WAIT_FAILED) {
		(void) fprintf(stderr, "Error: WaitForSingleObject(pi.hProcess, INFINITE): Failure: 0x%08X\n", GetLastError());
		return (1 << 16) + 3;
	}

	DWORD	exitCode;
	if (!GetExitCodeProcess(pi.hProcess, &exitCode)) {
		(void) fprintf(stderr, "Error: GetExitCodeProcess(hThread): Failure: 0x%08X\n", GetLastError());
		return (1 << 16) + 4;
	}

	// Close process and thread handles.
	if (!CloseHandle(pi.hProcess)) {
		(void) fprintf(stderr, "Error: CloseHandle(pi.hProcess): Failure: 0x%08X\n", GetLastError());
		return (1 << 16) + 5;
	}
	if (!CloseHandle(pi.hThread)) {
		(void) fprintf(stderr, "Error: CloseHandle(pi.hThread): Failure: 0x%08X\n", GetLastError());
		return (1 << 16) + 6;
	}

	return exitCode;
}

System::Void
Upload::CUpload::eventOnClickUpload(
	System::Object		*sender,
	System::EventArgs	*e
)
{
	this->Cursor = System::Windows::Forms::Cursors::WaitCursor;
	this->tabControlUpload->SelectedIndex = 2;
	this->tabControlUpload->Refresh();

	// Create a pipe for the child process's STDOUT.
	// Set the bInheritHandle flag so pipe handles are inherited.
	HANDLE	hChildStdoutRd;
//	HANDLE	hChildStdoutWr;
	SECURITY_ATTRIBUTES	saAttr;
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.lpSecurityDescriptor = NULL;
	saAttr.bInheritHandle = TRUE;
	if (!CreatePipe(&hChildStdoutRd, &hChildStdoutWr, &saAttr, 0)) {
		(void) fprintf(stderr, "Error: CreatePipe() for the child process's STDOUT: Failure: 0x08X\n", GetLastError());
		exit(1);
	}
	// Ensure the read handle to the pipe for STDOUT is not inherited.
	if (!SetHandleInformation(hChildStdoutRd, HANDLE_FLAG_INHERIT, 0)) {
		(void) fprintf(stderr, "Error: SetHandleInformation(hChildStdoutRd): Failure: 0x08X\n", GetLastError());
		exit(1);
	}

	if (!(hEvent = CreateEvent(
		NULL,
		FALSE,
		FALSE,
		NULL
	))) {
		(void) fprintf(stderr, "Error: CreateEvent(): Failure: 0x%08X\n", GetLastError());
		exit(1);
	}

	String	*argvTdSpecific[] = {
		PERL_EXE,
		MAKE_DAILY_IMAGE_PL,
		this->textBoxGroup->Text,
		this->textBoxUser->Text,
		this->textBoxShow->Text,
		this->textBoxSeq->Text,
		this->textBoxShot->Text,
		this->textBoxPathname->Text,
		this->textBoxFrame->Text,
		this->textBoxRevision->Text,
		this->richTextBoxComment->Text->Replace(S"\\", S"\\\\")->Replace(S"\"", S"\\\""),
		// Td specific info.
		this->textBoxTdAnim->Text->Replace(S"\\", S"\\\\")->Replace(S"\"", S"\\\""),
		this->textBoxTdTexture->Text->Replace(S"\\", S"\\\\")->Replace(S"\"", S"\\\""),
		this->textBoxPixelSamples->Text,
		this->textBoxShadingRate->Text,
		// Logon info.
		this->textBoxPassword->Text
	};
	String	*argvNonTdSpecific[] = {
		PERL_EXE,
		MAKE_DAILY_IMAGE_PL,
		this->textBoxGroup->Text,
		this->textBoxUser->Text,
		this->textBoxShow->Text,
		this->textBoxSeq->Text,
		this->textBoxShot->Text,
		this->textBoxPathname->Text,
		this->textBoxFrame->Text,
		this->textBoxRevision->Text,
		this->richTextBoxComment->Text->Replace(S"\\", S"\\\\")->Replace(S"\"", S"\\\""),
		// Non-Td specific info.
		this->textBoxNonTdAnim->Text,
		this->textBoxNonTdMdl->Text,
		this->textBoxNonTdTexture->Text,
		this->textBoxNonTdFx->Text,
		this->textBoxNonTdTd->Text,
		this->textBoxNonTdComp->Text,
		this->textBoxNonTdSetup->Text,
		this->textBoxNonTdConcept->Text,
		this->textBoxFocalLength->Text,
		this->textBoxFPS->Text,
		// Logon info.
		this->textBoxPassword->Text
	};
	Char	charArray[];
	if (String::Compare(this->textBoxGroup->Text, S"TD", true) == 0) {
		charArray = String::Concat(String::Concat(S"\"", String::Join(S"\" \"", argvTdSpecific)), S"\"\n")->ToCharArray();
	} else {
		charArray = String::Concat(String::Concat(S"\"", String::Join(S"\" \"", argvNonTdSpecific)), S"\"\n")->ToCharArray();
	}
	TCHAR	CommandLine[4096];
	for (int i = 0; i < charArray->Count; ++i)
		CommandLine[i] = charArray[i];	// If UNICODE defined, TCHAR is equal to Char!
	CommandLine[i] = L'\0';
	HANDLE	hThread;
	if (!(hThread = CreateThread(
		NULL,
		0,
		ThreadProc,
		(LPVOID) CommandLine,
		0,
		NULL
	))) {
		(void) fprintf(stderr, "Error: CreateThread(): Failure: 0x%08X\n", GetLastError());
		exit(1);
	}
	HANDLE	Handles[2] = {hThread, hEvent};
	DWORD	dwWaitResult = WaitForMultipleObjects(2, Handles, FALSE, INFINITE);
	if (dwWaitResult != WAIT_OBJECT_0 + 0 && dwWaitResult != WAIT_OBJECT_0 + 1) {
		(void) fprintf(stderr, "Error: WaitForSingleObject(hEvent, INFINITE): Failure: 0x%08X\n", GetLastError());
		exit(1);
	}
	// WAIT_OBJECT_0 + 0: hThread
	// WAIT_OBJECT_0 + 1: hEvent
	//	어느 경우든 더 이상 hEvent는 사용되지 않는다.
	if (!CloseHandle(hEvent)) {
		(void) fprintf(stderr, "Error: CloseHandle(hEvent): Failure: 0x%08X\n", GetLastError());
		exit(1);
	}

#define BUFSIZE	4096
	DWORD	dwRead; 
	CHAR	chBuf[BUFSIZE]; 

	// Close the write end of the pipe before reading from the read end of the pipe.
	if (!CloseHandle(hChildStdoutWr)) {
		(void) fprintf(stderr, "Error: CloseHandle(hChildStdoutWr): Failure: 0x%08X\n", GetLastError());
		exit(1);
	}

	// Read output from the child process.
	unsigned	uCounter = 0;
	this->richTextBoxMessage->Text = S"";
	this->richTextBoxMessage->Refresh();
	for (;;) {
		if(!ReadFile(hChildStdoutRd, chBuf, BUFSIZE, &dwRead, NULL)) {
			DWORD	dwLastErrorCode = GetLastError();
			if (dwLastErrorCode == ERROR_BROKEN_PIPE) {
				break;
			}
			(void) fprintf(stderr, "Error: ReadFile(hChildStdoutRd): Failure: 0x%08X\n", dwLastErrorCode);
			exit(1);
		}
		if (dwRead == 0)
			break;
		String	*msg = new String(chBuf, 0, dwRead);
		this->richTextBoxMessage->Text = String::Concat(this->richTextBoxMessage->Text, msg);
		this->richTextBoxMessage->Refresh();
	}
	if (!CloseHandle(hChildStdoutRd)) {
		(void) fprintf(stderr, "Error: CloseHandle(hChildStdoutRd): Failure: 0x%08X\n", GetLastError());
		exit(1);
	}

	// Wait until child process exits.
	if (WaitForSingleObject(hThread, INFINITE) == WAIT_FAILED) {
		(void) fprintf(stderr, "Error: WaitForSingleObject(hThread, INFINITE): Failure: 0x%08X\n", GetLastError());
		exit(1);
	}

	DWORD	exitCodeThread;
	if (!GetExitCodeThread(hThread, &exitCodeThread)) {
		(void) fprintf(stderr, "Error: GetExitCodeThread(hThread): Failure: 0x%08X\n", GetLastError());
		exit(1);
	}

	if (!CloseHandle(hThread)) {
		(void) fprintf(stderr, "Error: CloseHandle(hThread): Failure: 0x%08X\n", GetLastError());
		exit(1);
	}

	this->richTextBoxMessage->Text = String::Format(
		S"{0}\nExit Code = 0x{1,8:x8} ({2})\n"
		,
		this->richTextBoxMessage->Text,
		__box(exitCodeThread),
		exitCodeThread ? new String("Failure") : new String("Success")
	);
	this->Cursor = System::Windows::Forms::Cursors::Default;
}


System::Void
Upload::CUpload::eventOnClickOk(
	System::Object		*sender,
	System::EventArgs	*e
)
{
	this->richTextBoxMessage->Text	= S"";

	// Check (Username, Password)...
	bool	bParameterOk = true;
	// 1. Username
	//	[A-Za-z]{2,16}
	int	length = this->textBoxUsername->Text->Length;
	if (length < 2 || length > 16) {
		bParameterOk = false;
	} else {
		Char	Username[] = this->textBoxUsername->Text->ToCharArray();
		for (int i = 0; i < length; ++i) {
			if (!isalpha(Convert::ToSByte(Username[i]))) {
				bParameterOk = false;
				break;
			}
		}
	}
	if (!bParameterOk) {
		this->richTextBoxMessage->Text = String::Format(
			S"{0}Username:= |{1}|\n"
			S"\t2 <= # of alphabet characters <= 16\n"
			,
			this->richTextBoxMessage->Text,
			this->textBoxUsername->Text
		);
	}
	// 2. Password
	//	\S{4,}
	Char	Space[] = {' ', '\t', '\n'};
	if (this->textBoxPassword->Text->IndexOfAny(Space) != -1) {
		this->richTextBoxMessage->Text = String::Format(
			S"{0}Password: Space(\" \\t\\n\") not allowed\n"
			,
			this->richTextBoxMessage->Text
		);
		bParameterOk = false;
	}
	if (this->textBoxPassword->Text->Length < 4) {
		this->richTextBoxMessage->Text = String::Format(
			S"{0}Password: # of characters >= 4\n"
			,
			this->richTextBoxMessage->Text
		);
		bParameterOk = false;
	}
	if (!bParameterOk) {
		this->tabControlUpload->SelectedIndex = 2;
		this->tabControlUpload->Refresh();

		return;
	}


	this->tabPageCommonInfo->Enabled = true;
	this->tabPageSpecificInfo->Enabled = true;
	this->textBoxUser->Text = this->textBoxUsername->Text;
	this->groupBoxUser->Enabled = false;
	this->tabControlUpload->SelectedIndex = 0;
	this->tabControlUpload->Refresh();
}
