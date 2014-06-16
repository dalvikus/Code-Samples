#include "CAccount.h"
#define MySQL_DAILY_USER	"bloomus"
#define MySQL_WORKER_HOST	"192.168.0.0/255.255.0.0"//"localhost"

using namespace Account;


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
Account::CAccount::eventOnLoad(
	System::Object		*sender,
	System::EventArgs	*e
)
{
	// Add tab...
	this->textBoxAddUsername->Text		= S"";
	this->textBoxPassword->Text		= S"";
	this->textBoxAddConfirm->Text		= S"";

	// Change tab...
	this->textBoxChangeUsername->Text	= S"";
	this->textBoxOldPassword->Text		= S"";
	this->textBoxNewPassword->Text		= S"";
	this->textBoxChangeConfirm->Text	= S"";

	// Log tab...
	this->richTextBoxMessage->Text		= S"";


	// Connect to databases...
	int	iStatus;
	__mysqlCAPI = new MySQLCAPI(&iStatus);
	if (!__mysqlCAPI || iStatus != 0) {
		// Error
		this->tabControlAccount->SelectedIndex	= 2;
		this->tabControlAccount->Refresh();

		this->richTextBoxMessage->Text = S"MySQLCAPI: Failure\n\tTerminate a Program and Consult...\n";
	}
}

System::Void
Account::CAccount::eventOnClickAdd(
	System::Object		*sender,
	System::EventArgs	*e
)
{
	this->richTextBoxMessage->Text	= S"";

	// Check (Username, Password)...
	bool	bParameterOk = true;
	// 1. Username
	//	[A-Za-z]{2,16}
	int	length = this->textBoxAddUsername->Text->Length;
	if (length < 2 || length > 16) {
		bParameterOk = false;
	} else {
		Char	Username[] = this->textBoxAddUsername->Text->ToCharArray();
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
			this->textBoxAddUsername->Text
		);
	}
	if (!String::Compare(this->textBoxAddUsername->Text, new String(MySQL_DAILY_USER), true)) {
		this->richTextBoxMessage->Text = String::Format(
			S"{0}Username(\"{1}\"): Not allowed\n"
			,
			this->richTextBoxMessage->Text,
			new String(MySQL_DAILY_USER)
		);
		bParameterOk = false;
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
	// 3. Password = Confirm
	if (!this->textBoxPassword->Text->Equals(this->textBoxAddConfirm->Text)) {
		this->richTextBoxMessage->Text = String::Concat(
			this->richTextBoxMessage->Text,
			S"Password != Confirm\n"
		);
		bParameterOk = false;
	}
	if (!bParameterOk) {
		this->tabControlAccount->SelectedIndex = 2;
		this->tabControlAccount->Refresh();

		return;
	}


	// Insert (Username, Password)...
	// GRANT SELECT, INSERT, UPDATE, DELETE
	//	ON DBBloomUs.*
	//	TO 'Username'@'192.168.0.0/255.255.0.0'
	//	IDENTIFIED BY 'Password'
	MYSQL	*mysql = __mysqlCAPI->__getMySQL();
	String	*SQLQuery = String::Format(
		S"GRANT SELECT, INSERT, UPDATE, DELETE "
		S"ON DBBloomUs.* "
		S"TO '{0}'@'{1}' "
		S"IDENTIFIED BY '{2}'"
		,
		GetMySQLEscapedString(mysql, this->textBoxAddUsername->Text),
		new String(MySQL_WORKER_HOST),
		GetMySQLEscapedString(mysql, this->textBoxPassword->Text)
	);
	char	*query = 0;
__try {
	query = ptrToSByteFromString(SQLQuery);
	if (mysql_query(mysql, query)) {
		this->tabControlAccount->SelectedIndex	= 2;
		this->tabControlAccount->Refresh();
		this->richTextBoxMessage->Text = String::Format(
			S"Error: mysql(\"...\"): Failure: ({0}) {1}\n"
			,
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
Account::CAccount::eventOnClickChange(
	System::Object		*sender,
	System::EventArgs	*e
)
{
	this->richTextBoxMessage->Text	= S"";

	// Check (Username, Password)...
	bool	bParameterOk = true;
	// 1. Username
	//	[A-Za-z]{2,16}
	int	length = this->textBoxChangeUsername->Text->Length;
	if (length < 2 || length > 16) {
		bParameterOk = false;
	} else {
		Char	Username[] = this->textBoxChangeUsername->Text->ToCharArray();
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
			this->textBoxChangeUsername->Text
		);
	}
	if (!String::Compare(this->textBoxChangeUsername->Text, new String(MySQL_DAILY_USER), true)) {
		this->richTextBoxMessage->Text = String::Format(
			S"{0}Username(\"{1}\"): Not allowed\n"
			,
			this->richTextBoxMessage->Text,
			new String(MySQL_DAILY_USER)
		);
		bParameterOk = false;
	}
	// 2. Old Password
	//	\S{4,}
	Char	Space[] = {' ', '\t', '\n'};
	if (this->textBoxOldPassword->Text->IndexOfAny(Space) != -1) {
		this->richTextBoxMessage->Text = String::Format(
			S"{0}Old Password: Space(\" \\t\\n\") not allowed\n"
			,
			this->richTextBoxMessage->Text
		);
		bParameterOk = false;
	}
	if (this->textBoxOldPassword->Text->Length < 4) {
		this->richTextBoxMessage->Text = String::Format(
			S"{0}Old Password: # of characters >= 4\n"
			,
			this->richTextBoxMessage->Text
		);
		bParameterOk = false;
	}
	// 3. New Password
	//	\S{4,}
	if (this->textBoxNewPassword->Text->IndexOfAny(Space) != -1) {
		this->richTextBoxMessage->Text = String::Format(
			S"{0}New Password: Space(\" \\t\\n\") not allowed\n"
			,
			this->richTextBoxMessage->Text
		);
		bParameterOk = false;
	}
	if (this->textBoxNewPassword->Text->Length < 4) {
		this->richTextBoxMessage->Text = String::Format(
			S"{0}New Password: # of characters >= 4\n"
			,
			this->richTextBoxMessage->Text
		);
		bParameterOk = false;
	}
	// 4. New Password = Confirm
	if (!this->textBoxNewPassword->Text->Equals(this->textBoxChangeConfirm->Text)) {
		this->richTextBoxMessage->Text = String::Concat(
			this->richTextBoxMessage->Text,
			S"New Password != Confirm\n"
		);
		bParameterOk = false;
	}

	if (!bParameterOk) {
		this->tabControlAccount->SelectedIndex = 2;
		this->tabControlAccount->Refresh();

		return;
	}


	// Update password...
	// 1. One query but cannot confirm 'OldPassword'
	//	SET PASSWORD FOR 'Username'@'192.168.0.0/255.255.0.0' = PASSWORD('NewPassword');
	// 2. Can confirm 'OldPassword' but two queries
	//	UPDATE mysql.user
	//		SET Password = PASSWORD('NewPassword')
	//		WHERE User = 'Username' AND Host = '192.168.0.0/255.255.0.0' AND Password = PASSWORD('OldPassword');
	//	FLUSH PRIVILEGES;
	MYSQL	*mysql = __mysqlCAPI->__getMySQL();
	String	*strArray[] = {
		GetMySQLEscapedString(mysql, this->textBoxNewPassword->Text),
		GetMySQLEscapedString(mysql, this->textBoxChangeUsername->Text),
		new String(MySQL_WORKER_HOST),
		GetMySQLEscapedString(mysql, this->textBoxOldPassword->Text)
	};
	String	*SQLQuery = String::Format(
		S"UPDATE mysql.user "
		S"SET Password = PASSWORD('{0}') "
		S"WHERE User = '{1}' AND Host = '{2}' AND Password = PASSWORD('{3}')"
		,
		strArray
	);
	char	*query = 0;
__try {
	query = ptrToSByteFromString(SQLQuery);
	if (mysql_query(mysql, query)) {
		this->tabControlAccount->SelectedIndex	= 2;
		this->tabControlAccount->Refresh();
		this->richTextBoxMessage->Text = String::Format(
			S"Error: mysql(\"...\"): Failure: ({1}) {2}\n"
			,
			Convert::ToString(mysql_errno(mysql)),
			Convert::ToString(mysql_error(mysql))
		);

		return;
	}
	if (mysql_affected_rows(mysql) != 1) {
		this->tabControlAccount->SelectedIndex	= 2;
		this->tabControlAccount->Refresh();
		this->richTextBoxMessage->Text = S"Error: No update: Check (Username, OldPassword)\n";

		return;
	}
} __finally {
	if (query) {
		delete[] query;
		query = 0;
	}
}
	//	FLUSH PRIVILEGES;
	(void) mysql_query(mysql, "FLUSH PRIVILEGES");
}
