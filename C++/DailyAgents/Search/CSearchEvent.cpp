#include <stdio.h>
#define PERL_EXE		"C:/Daily/cygwin/bin/perl.exe"
#define LAUNCH_DAILY_IMAGE_PL	"C:/Daily/Tools/bin/launchDailyImage.pl"
// UniqueFilename0:= Date.ShowName.ShotName.GroupName.UserName.Revision.UUID
const char	*SQLQuery0 = "SELECT DailyID, Frame, UniqueFilename0, ImageType, Comment FROM TDaily";
	// " WHERE DateTime >= CURDATE()"
	// " WHERE ShowName LIKE \"%...%\" AND ..."
	// " ORDER BY UniqueFilename0 DESC"
#define SQL_COLUMN_DAILY_ID_INDEX		0
#define SQL_COLUMN_FRAME_INDEX			1
#define SQL_COLUMN_UNIQUE_FILENAME0_INDEX	2
#define SQL_COLUMN_IMAGE_TYPE_INDEX		3
#define SQL_COLUMN_COMMENT_INDEX		4

#include "CSearch.h"
using namespace	Search;


char	*
ptrToSByteFromString(
	String	*strText
)
{
	int	length = strText->Length;

	// Convert String to char *.
	Char	charArray[] = strText->ToCharArray();
	char	*ptrToSByte = new char[length + 1];
	for (int i = 0; i < length; ++i) {
		ptrToSByte[i] = Convert::ToSByte(charArray[i]);
	}
	ptrToSByte[i] = '\0';

	return ptrToSByte;
}

String	*
GetMySQLEscapedString(
	MYSQL	*mysql,
	String	*Text
)
{
	int	length = Text->Length;

	char	*charGroupName = 0;
	char	*escapedGroupName = 0;
__try {
	// Convert String to char *.
	Char	charArrayGroupName[] = Text->ToCharArray();
	charGroupName = new char[length + 1];
	for (int i = 0; i < length; ++i) {
		charGroupName[i] = Convert::ToSByte(charArrayGroupName[i]);
	}
	charGroupName[i] = '\0';

	escapedGroupName = new char[2 * length + 1];
	(void) mysql_real_escape_string(mysql, escapedGroupName, charGroupName, (unsigned long) length);
	return new String(escapedGroupName);
} __finally {
	if (charGroupName) {
		delete[] charGroupName;
		charGroupName = 0;
	}
	if (escapedGroupName) {
		delete[] escapedGroupName;
		escapedGroupName = 0;
	}
}
}


System::Void
Search::CSearch::eventOnLoad(
	System::Object		*sender,
	System::EventArgs	*e
)
{
	// Identity
	this->textBoxGroup->Text		= S"%";
	this->textBoxGroup->Enabled		= false;
	this->textBoxUser->Text			= S"%";
	this->textBoxUser->Enabled		= false;

	// Project
	this->textBoxShow->Text			= S"%";
	this->textBoxShow->Enabled		= false;
	this->textBoxSeq->Text			= S"";
	this->textBoxSeq->Enabled		= false;
	this->textBoxShot->Text			= S"%";
	this->textBoxShot->Enabled		= false;

	// Date
//	MessageBox::Show(dateTimePickerFrom->Value.ToString());
//	DateTime	dateFrom = this->dateTimePickerFrom->Value;
//	MessageBox::Show(String::Format(S"{0}/{1}/{2}", dateFrom.Year.ToString(), dateFrom.Month.ToString(), dateFrom.Day.ToString()));
//	DateTime:	1753/1/1 - 9998/12/31
//	MySQL:		1000-1-1 - 9999-12-31
	this->dateTimePickerFrom->Value		= DateTime::Now;	// Value를 설정하면 Checked가 true로 되므로 먼저 설정해야 한다.
	this->dateTimePickerFrom->Checked	= false;
	this->dateTimePickerFrom->Enabled	= false;
	this->dateTimePickerTo->Value		= DateTime::Now;
	this->dateTimePickerTo->Checked		= false;
	this->dateTimePickerTo->Enabled		= false;
	this->checkBoxToday->Checked		= true;


	// Launch
	this->groupBoxLaunch->Enabled		= false;
	this->checkBoxCloseWindow->Checked	= true;
	this->comboBoxViewer->Text		= S"Select Viewer";
	this->comboBoxViewer->Items->Add(S"FrameCycler");
	this->comboBoxViewer->Items->Add(S"fcheck");
	this->comboBoxViewer->SelectedIndex	= 0;
	this->textBoxFPS->Text			= S"24";

	// Note
	this->richTextBoxComment->Text		= S"";


	// Log
	this->richTextBoxMessage->Text		= S"";


	// Logon
	this->textBoxUsername->Text		= S"";
	this->textBoxPassword->Text		= S"";



	__mysqlCAPI = 0;

	this->tabPageSearch->Enabled	= false;
	this->tabPageResult->Enabled	= false;
	this->tabControlSearch->SelectedIndex	= 3;
	this->tabControlSearch->Refresh();
}

System::Void
Search::CSearch::eventOnClosed(
	System::Object		*sender,
	System::EventArgs	*e
)
{
	if (__mysqlCAPI) {
		delete __mysqlCAPI;

		__mysqlCAPI = NULL;
	}
}

System::Void
Search::CSearch::eventOnCheckedChangedToday(
	System::Object		*sender,
	System::EventArgs	*e
)
{
	this->textBoxGroup->Enabled		= !this->checkBoxToday->Checked;
	this->textBoxUser->Enabled		= !this->checkBoxToday->Checked;
	this->textBoxShow->Enabled		= !this->checkBoxToday->Checked;
	this->textBoxShot->Enabled		= !this->checkBoxToday->Checked;
	this->dateTimePickerFrom->Enabled	= !this->checkBoxToday->Checked;
	this->dateTimePickerTo->Enabled		= !this->checkBoxToday->Checked;
}

System::Void
Search::CSearch::eventOnClickSearch(
	System::Object		*sender,
	System::EventArgs	*e
)
{
	this->Cursor = System::Windows::Forms::Cursors::WaitCursor;
	this->richTextBoxMessage->Text = S"";
	this->tabControlSearch->SelectedIndex = 1;
	this->tabControlSearch->Refresh();
__try {
	MYSQL	*mysql = __mysqlCAPI->__getMySQL();
	String	*SQLQuery;
	if (this->checkBoxToday->Checked) {
		SQLQuery = String::Format(
			S"{0} WHERE Flag = 0 AND UserName LIKE '{1}' AND DateTime >= ADDTIME(CONCAT(CURDATE(), ' 00:00:00'), '-12:00:00') AND DateTime < ADDTIME(CONCAT(CURDATE(), ' 00:00:00'), '12:00:00') ORDER BY DateTime, ShowName, ShotName, GroupName, UserName, Revision"
			,
			new String(SQLQuery0),
			this->textBoxUser->Text
		);
	} else {
		DateTime	dateFrom = this->dateTimePickerFrom->Value;
		DateTime	dateTo = this->dateTimePickerTo->Value;
		String	*strArray[] = {
			new String(SQLQuery0),
			GetMySQLEscapedString(mysql, this->textBoxGroup->Text),
			GetMySQLEscapedString(mysql, this->textBoxUser->Text),
			GetMySQLEscapedString(mysql, this->textBoxShow->Text),
			GetMySQLEscapedString(mysql, this->textBoxShot->Text),
			this->dateTimePickerFrom->Checked ? String::Format(S"{0}-{1}-{2} 12:00:00", dateFrom.Year.ToString(), dateFrom.Month.ToString(), dateFrom.Day.ToString()) : "1000-1-1 00:00:00",
			this->dateTimePickerTo->Checked ? String::Format(S"{0}-{1}-{2} 11:59:59", dateTo.Year.ToString(), dateTo.Month.ToString(), dateTo.Day.ToString()) : "9999-12-31 23:59:59"
		};
		SQLQuery = String::Format(
			S"{0} WHERE Flag = 0 AND GroupName LIKE '{1}' AND UserName LIKE '{2}' AND ShowName LIKE '{3}' AND ShotName LIKE '{4}' AND DateTime >= '{5}' AND DateTime <= '{6}' ORDER BY DateTime, ShowName, ShotName, GroupName, UserName, Revision"
			,
			strArray
		);
	}
	char	*query = 0;
__try {
	query = ptrToSByteFromString(SQLQuery);
	if (mysql_query(mysql, query)) {
		this->groupBoxLaunch->Enabled		= false;
		this->tabControlSearch->SelectedIndex	= 2;
		this->tabControlSearch->Refresh();
		this->richTextBoxMessage->Text = String::Format(
			S"Error: mysql(\"{0}\"): Failure: ({1}) {2}\n"
			,
			Convert::ToString(SQLQuery),
			Convert::ToString(mysql_errno(mysql)),
			Convert::ToString(mysql_error(mysql))
		);

		return;
	}
} __finally {
	if (query) {
		delete[] query;
		query = 0;
	}
}
	MYSQL_RES	*result = mysql_store_result(mysql);
	if (result) {
		__mysqlCAPI->__setMySQLResult(result);

		this->listBoxResult->BeginUpdate();
		this->listBoxResult->Items->Clear();
		this->richTextBoxComment->Text = S"";
		// List <UniqueFilename0>
		unsigned	RowCount = 0;
		MYSQL_ROW	row;
		while ((row = mysql_fetch_row(result))) {
			String	*UniqueFilename0(row[SQL_COLUMN_UNIQUE_FILENAME0_INDEX]);
			int	lastIndexOfDot = UniqueFilename0->LastIndexOf(L'.');
			this->listBoxResult->Items->Add(UniqueFilename0->Substring(0, lastIndexOfDot));

			++RowCount;
		}
		__mysqlCAPI->__setRowCount(RowCount);
		if (RowCount) {
			this->listBoxResult->SetSelected(0, true);
		}
		this->groupBoxLaunch->Enabled		= RowCount ? true : false;
		if (RowCount == 0) {
			this->tabControlSearch->SelectedIndex = 2;
			this->tabControlSearch->Refresh();
			DateTime	dateFrom = this->dateTimePickerFrom->Value;
			DateTime	dateTo = this->dateTimePickerTo->Value;
			String	*strArray[] = {
				this->textBoxGroup->Text,
				this->textBoxUser->Text,
				this->textBoxShow->Text,
				this->textBoxShot->Text,
				this->dateTimePickerFrom->Checked ? String::Format(S"{0}/{1}/{2}", dateFrom.Year.ToString(), dateFrom.Month.ToString(), dateFrom.Day.ToString()) : "1000/1/1",
				this->dateTimePickerTo->Checked ? String::Format(S"{0}/{1}/{2}", dateTo.Year.ToString(), dateTo.Month.ToString(), dateTo.Day.ToString()) : "9999/12/31"
			};
			this->richTextBoxMessage->Text = String::Format(
				S"No Match:\n\tGroupName: |{0}|\n\tUserName: |{1}|\n\tShowName: |{2}|\n\tShotName: |{3}|\n\tFrom: |{4}|\n\tTo: |{5}|\n"
				,
				strArray
			);
		}
		this->listBoxResult->EndUpdate();
	} else {
		this->groupBoxLaunch->Enabled		= false;
		this->tabControlSearch->SelectedIndex	= 2;
		this->tabControlSearch->Refresh();
		this->richTextBoxMessage->Text = String::Format(
			S"Error: mysql_store_result(): Failure: ({0}) {1}\n"
			,
			Convert::ToString(mysql_errno(mysql)),
			Convert::ToString(mysql_error(mysql))
		);

		return;
	}
}
__finally {
	this->Cursor = System::Windows::Forms::Cursors::Default;
}
}

System::Void
Search::CSearch::eventOnSelectedIndex(
	System::Object		*sender,
	System::EventArgs	*e
)
{
	int	itemIndex = this->listBoxResult->SelectedIndex;
	if (itemIndex == -1) {
		// No item is selected, i.e., since selected item is removed.
		return;
	}

	MYSQL_RES	*result = __mysqlCAPI->__getMySQLResult();
	int	dataIndex = __mysqlCAPI->__getDataIndex(itemIndex);
	if (dataIndex == -1) {
		*((int *) 0) = 0;	// Debug...
		return;
	}
	mysql_data_seek(result, (my_ulonglong) dataIndex);
	MYSQL_ROW	row;
	if (!(row = mysql_fetch_row(result))) {
		MYSQL		*mysql = __mysqlCAPI->__getMySQL();
		this->groupBoxLaunch->Enabled		= false;
		this->tabControlSearch->SelectedIndex	= 2;
		this->tabControlSearch->Refresh();
		this->richTextBoxMessage->Text = String::Format(
			S"Error: mysql_fetch_row(): Failure: ({0}) {1}\n"
			,
			Convert::ToString(mysql_errno(mysql)),
			Convert::ToString(mysql_error(mysql))
		);

		return;
	}
	this->groupBoxLaunch->Enabled = true;

	this->richTextBoxComment->Text = Convert::ToString(row[SQL_COLUMN_COMMENT_INDEX]);
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
Search::CSearch::eventOnClickOpen(
	System::Object		*sender,
	System::EventArgs	*e
)
{
	int	itemIndex = this->listBoxResult->SelectedIndex;

	MYSQL_RES	*result = __mysqlCAPI->__getMySQLResult();
	int	dataIndex = __mysqlCAPI->__getDataIndex(itemIndex);
	if (dataIndex == -1) {
		*((int *) 0) = 0;	// Debug...
		return;
	}
	mysql_data_seek(result, (my_ulonglong) dataIndex);
	MYSQL_ROW	row;
	if (!(row = mysql_fetch_row(result))) {
		MYSQL		*mysql = __mysqlCAPI->__getMySQL();
		this->tabControlSearch->SelectedIndex	= 2;
		this->tabControlSearch->Refresh();
		this->richTextBoxComment->Text = String::Format(
			S"Error: mysql_fetch_row(): Failure: ({0}) {1}\n"
			,
			Convert::ToString(mysql_errno(mysql)),
			Convert::ToString(mysql_error(mysql))
		);

		return;
	}

	this->richTextBoxMessage->Text = S"";

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

	char	CommandLine[4096];
	char	*Viewer = 0;
	char	*FPS = 0;
__try {
	Viewer = ptrToSByteFromString(this->comboBoxViewer->SelectedItem->ToString());
	FPS = ptrToSByteFromString(this->textBoxFPS->Text);
	(void) sprintf(
		CommandLine,
		"\"%s\" \"%s\" \"%s\" \"%s\" \"%d\" \"%s\" \"%s\" \"%s\""
		,
		PERL_EXE,
		LAUNCH_DAILY_IMAGE_PL,
		row[SQL_COLUMN_UNIQUE_FILENAME0_INDEX],
		row[SQL_COLUMN_FRAME_INDEX],
		this->checkBoxCloseWindow->Checked,
		FPS,
		Viewer,
		row[SQL_COLUMN_IMAGE_TYPE_INDEX]
	);
} __finally {
	if (Viewer) {
		delete[] Viewer;
		Viewer = 0;
	}
	if (FPS) {
		delete[] FPS;
		FPS = 0;
	}
}
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
		if (strstr(chBuf, END_OF_PROCESS_STRING)) {
			break;
		}
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
	if (exitCodeThread) {
		this->tabControlSearch->SelectedIndex = 2;
		this->tabControlSearch->Refresh();
	}
}

System::Void
Search::CSearch::eventOnClickDelete(
	System::Object		*sender,
	System::EventArgs	*e
)
{
	int	itemIndex = this->listBoxResult->SelectedIndex;

	// Get the DailyID of selected item.
	MYSQL_RES	*result = __mysqlCAPI->__getMySQLResult();
	int	dataIndex = __mysqlCAPI->__getDataIndex(itemIndex);
	if (dataIndex == -1) {
		*((int *) 0) = 0;	// Debug...
		return;
	}
	mysql_data_seek(result, (my_ulonglong) dataIndex);
	MYSQL_ROW	row;
	if (!(row = mysql_fetch_row(result))) {
		MYSQL		*mysql = __mysqlCAPI->__getMySQL();
		this->groupBoxLaunch->Enabled		= false;
		this->tabControlSearch->SelectedIndex	= 2;
		this->tabControlSearch->Refresh();
		this->richTextBoxMessage->Text = String::Format(
			S"Error: mysql_fetch_row(): Failure: ({0}) {1}\n"
			,
			Convert::ToString(mysql_errno(mysql)),
			Convert::ToString(mysql_error(mysql))
		);

		return;
	}
	this->groupBoxLaunch->Enabled = true;
//	int	DailyID = row[SQL_COLUMN_DAILY_ID_INDEX];


	this->listBoxResult->Items->RemoveAt(itemIndex);
	__mysqlCAPI->__setDataRemoved(itemIndex);
	if (itemIndex == this->listBoxResult->Items->Count)
		--itemIndex;
	this->listBoxResult->SelectedIndex = itemIndex;

	if (itemIndex == -1) {
		// There is no item.
		this->groupBoxLaunch->Enabled	= false;
		this->richTextBoxComment->Text	= S"";
	}

//	UPDATE TDaily SET Flag = 0, DelDateTime = NOW() WHERE DailyID = 76;
	String	*SQLQuery;
	SQLQuery = String::Format(
		S"UPDATE TDaily SET Flag = 1, DelDateTime = NOW() WHERE DailyID = {0}"
		,
		new String(row[SQL_COLUMN_DAILY_ID_INDEX])
	);
//	this->richTextBoxMessage->Text = SQLQuery;
	MYSQL	*mysql = __mysqlCAPI->__getMySQL();
	char	*query = 0;
__try {
	query = ptrToSByteFromString(SQLQuery);
	if (mysql_query(mysql, query)) {
		this->groupBoxLaunch->Enabled		= false;
		this->tabControlSearch->SelectedIndex	= 2;
		this->tabControlSearch->Refresh();
		this->richTextBoxMessage->Text = String::Format(
			S"Error: mysql(\"{0}\"): Failure: ({1}) {2}\n"
			,
			Convert::ToString(SQLQuery),
			Convert::ToString(mysql_errno(mysql)),
			Convert::ToString(mysql_error(mysql))
		);

		return;
	}
} __finally {
	if (query) {
		delete[] query;
		query = 0;
	}
}
}


System::Void
Search::CSearch::eventOnClickOk(
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
if (!String::Compare(this->textBoxUsername->Text, new String(MySQL_DAILY_USER), true)) {
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
		this->tabControlSearch->SelectedIndex = 2;
		this->tabControlSearch->Refresh();

		return;
	}
}


	int	iStatus;
	char	*username = 0;
	char	*password = 0;
	username = ptrToSByteFromString(this->textBoxUsername->Text);
	password = ptrToSByteFromString(this->textBoxPassword->Text);
	if (__mysqlCAPI) {
		delete __mysqlCAPI;
		__mysqlCAPI = 0;
	}
	__mysqlCAPI = new MySQLCAPI(username, password, &iStatus);
	if (!__mysqlCAPI || iStatus != 0) {
		// Error
		this->groupBoxLaunch->Enabled		= false;
		this->tabControlSearch->SelectedIndex	= 2;
		this->tabControlSearch->Refresh();

		this->richTextBoxMessage->Text = S"MySQLCAPI: Failure: Check (Username, Password)\n";
	} else {
		this->tabPageSearch->Enabled = true;
		this->tabPageResult->Enabled = true;
		if (!this->textBoxUsername->Text->Equals(new String(MySQL_DAILY_USER))) {
			this->textBoxUser->Text		= this->textBoxUsername->Text;
			this->groupBoxUser->Enabled	= false;
			this->buttonDelete->Visible	= true;
		} else {
			this->textBoxUser->Text		= S"%";
			this->textBoxPassword->Text	= S"Don't Care";
			this->groupBoxUser->Enabled	= true;
			this->buttonDelete->Visible	= false;
		}
		this->tabControlSearch->SelectedIndex = 0;
		this->tabControlSearch->Refresh();
	}
	if (username) {
		delete[] username;
		username = 0;
	}
	if (password) {
		delete[] password;
		password = 0;
	}
}
