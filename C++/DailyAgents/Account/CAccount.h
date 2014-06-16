#pragma once

#include "MySQLCAPI.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace Account
{
	/// <summary> 
	/// Summary for CAccount
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public __gc class CAccount : public System::Windows::Forms::Form
	{
	public: 
		CAccount(void)
		{
			InitializeComponent();
		}
        
	protected: 
		void Dispose(Boolean disposing)
		{
			if (disposing && components)
			{
				components->Dispose();
			}
			__super::Dispose(disposing);
		}
	private: System::Windows::Forms::TabControl *  tabControlAccount;
	private: System::Windows::Forms::TabPage *  tabPageAddAcount;
	private: System::Windows::Forms::TabPage *  tabPageChangePassword;
	private: System::Windows::Forms::Label *  labelAddUsername;
	private: System::Windows::Forms::TextBox *  textBoxAddUsername;
	private: System::Windows::Forms::Label *  labelPassword;
	private: System::Windows::Forms::TextBox *  textBoxPassword;
	private: System::Windows::Forms::Label *  labelAddConfirm;
	private: System::Windows::Forms::TextBox *  textBoxAddConfirm;
	private: System::Windows::Forms::Button *  buttonAdd;
	private: System::Windows::Forms::Label *  labelChangeUsername;
	private: System::Windows::Forms::TextBox *  textBoxChangeUsername;
	private: System::Windows::Forms::Label *  labelOldPassword;
	private: System::Windows::Forms::TextBox *  textBoxOldPassword;
	private: System::Windows::Forms::Label *  labelNewPassword;
	private: System::Windows::Forms::TextBox *  textBoxNewPassword;
	private: System::Windows::Forms::Label *  labelChangeConfirm;
	private: System::Windows::Forms::TextBox *  textBoxChangeConfirm;
	private: System::Windows::Forms::Button *  buttonChange;
	private: System::Windows::Forms::TabPage *  tabPageLog;
	private: System::Windows::Forms::RichTextBox *  richTextBoxMessage;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container* components;

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->tabControlAccount = new System::Windows::Forms::TabControl();
			this->tabPageAddAcount = new System::Windows::Forms::TabPage();
			this->buttonAdd = new System::Windows::Forms::Button();
			this->textBoxAddConfirm = new System::Windows::Forms::TextBox();
			this->labelAddConfirm = new System::Windows::Forms::Label();
			this->textBoxPassword = new System::Windows::Forms::TextBox();
			this->labelPassword = new System::Windows::Forms::Label();
			this->textBoxAddUsername = new System::Windows::Forms::TextBox();
			this->labelAddUsername = new System::Windows::Forms::Label();
			this->tabPageChangePassword = new System::Windows::Forms::TabPage();
			this->buttonChange = new System::Windows::Forms::Button();
			this->textBoxChangeConfirm = new System::Windows::Forms::TextBox();
			this->labelChangeConfirm = new System::Windows::Forms::Label();
			this->textBoxNewPassword = new System::Windows::Forms::TextBox();
			this->labelNewPassword = new System::Windows::Forms::Label();
			this->textBoxOldPassword = new System::Windows::Forms::TextBox();
			this->labelOldPassword = new System::Windows::Forms::Label();
			this->textBoxChangeUsername = new System::Windows::Forms::TextBox();
			this->labelChangeUsername = new System::Windows::Forms::Label();
			this->tabPageLog = new System::Windows::Forms::TabPage();
			this->richTextBoxMessage = new System::Windows::Forms::RichTextBox();
			this->tabControlAccount->SuspendLayout();
			this->tabPageAddAcount->SuspendLayout();
			this->tabPageChangePassword->SuspendLayout();
			this->tabPageLog->SuspendLayout();
			this->SuspendLayout();
			// 
			// tabControlAccount
			// 
			this->tabControlAccount->Controls->Add(this->tabPageAddAcount);
			this->tabControlAccount->Controls->Add(this->tabPageChangePassword);
			this->tabControlAccount->Controls->Add(this->tabPageLog);
			this->tabControlAccount->Font = new System::Drawing::Font(S"Tahoma", 11);
			this->tabControlAccount->Location = System::Drawing::Point(8, 8);
			this->tabControlAccount->Name = S"tabControlAccount";
			this->tabControlAccount->SelectedIndex = 0;
			this->tabControlAccount->Size = System::Drawing::Size(400, 256);
			this->tabControlAccount->TabIndex = 0;
			// 
			// tabPageAddAcount
			// 
			this->tabPageAddAcount->Controls->Add(this->buttonAdd);
			this->tabPageAddAcount->Controls->Add(this->textBoxAddConfirm);
			this->tabPageAddAcount->Controls->Add(this->labelAddConfirm);
			this->tabPageAddAcount->Controls->Add(this->textBoxPassword);
			this->tabPageAddAcount->Controls->Add(this->labelPassword);
			this->tabPageAddAcount->Controls->Add(this->textBoxAddUsername);
			this->tabPageAddAcount->Controls->Add(this->labelAddUsername);
			this->tabPageAddAcount->Font = new System::Drawing::Font(S"Tahoma", 11);
			this->tabPageAddAcount->Location = System::Drawing::Point(4, 27);
			this->tabPageAddAcount->Name = S"tabPageAddAcount";
			this->tabPageAddAcount->Size = System::Drawing::Size(392, 225);
			this->tabPageAddAcount->TabIndex = 0;
			this->tabPageAddAcount->Text = S"Add Account";
			// 
			// buttonAdd
			// 
			this->buttonAdd->Location = System::Drawing::Point(144, 152);
			this->buttonAdd->Name = S"buttonAdd";
			this->buttonAdd->Size = System::Drawing::Size(96, 24);
			this->buttonAdd->TabIndex = 6;
			this->buttonAdd->Text = S"Add";
			this->buttonAdd->Click += new System::EventHandler(this, eventOnClickAdd);
			// 
			// textBoxAddConfirm
			// 
			this->textBoxAddConfirm->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->textBoxAddConfirm->Location = System::Drawing::Point(144, 104);
			this->textBoxAddConfirm->Name = S"textBoxAddConfirm";
			this->textBoxAddConfirm->PasswordChar = '*';
			this->textBoxAddConfirm->Size = System::Drawing::Size(176, 24);
			this->textBoxAddConfirm->TabIndex = 5;
			this->textBoxAddConfirm->Text = S"textBoxConfirm";
			// 
			// labelAddConfirm
			// 
			this->labelAddConfirm->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->labelAddConfirm->Location = System::Drawing::Point(32, 104);
			this->labelAddConfirm->Name = S"labelAddConfirm";
			this->labelAddConfirm->TabIndex = 4;
			this->labelAddConfirm->Text = S"Confirm";
			this->labelAddConfirm->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// textBoxPassword
			// 
			this->textBoxPassword->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->textBoxPassword->Location = System::Drawing::Point(144, 64);
			this->textBoxPassword->Name = S"textBoxPassword";
			this->textBoxPassword->PasswordChar = '*';
			this->textBoxPassword->Size = System::Drawing::Size(176, 24);
			this->textBoxPassword->TabIndex = 3;
			this->textBoxPassword->Text = S"textBoxPassword";
			// 
			// labelPassword
			// 
			this->labelPassword->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->labelPassword->Location = System::Drawing::Point(32, 64);
			this->labelPassword->Name = S"labelPassword";
			this->labelPassword->TabIndex = 2;
			this->labelPassword->Text = S"Password";
			this->labelPassword->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// textBoxAddUsername
			// 
			this->textBoxAddUsername->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->textBoxAddUsername->Location = System::Drawing::Point(144, 24);
			this->textBoxAddUsername->Name = S"textBoxAddUsername";
			this->textBoxAddUsername->Size = System::Drawing::Size(176, 24);
			this->textBoxAddUsername->TabIndex = 1;
			this->textBoxAddUsername->Text = S"textBoxUsername";
			// 
			// labelAddUsername
			// 
			this->labelAddUsername->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->labelAddUsername->Location = System::Drawing::Point(32, 24);
			this->labelAddUsername->Name = S"labelAddUsername";
			this->labelAddUsername->TabIndex = 0;
			this->labelAddUsername->Text = S"Username";
			this->labelAddUsername->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// tabPageChangePassword
			// 
			this->tabPageChangePassword->Controls->Add(this->buttonChange);
			this->tabPageChangePassword->Controls->Add(this->textBoxChangeConfirm);
			this->tabPageChangePassword->Controls->Add(this->labelChangeConfirm);
			this->tabPageChangePassword->Controls->Add(this->textBoxNewPassword);
			this->tabPageChangePassword->Controls->Add(this->labelNewPassword);
			this->tabPageChangePassword->Controls->Add(this->textBoxOldPassword);
			this->tabPageChangePassword->Controls->Add(this->labelOldPassword);
			this->tabPageChangePassword->Controls->Add(this->textBoxChangeUsername);
			this->tabPageChangePassword->Controls->Add(this->labelChangeUsername);
			this->tabPageChangePassword->Font = new System::Drawing::Font(S"Tahoma", 11);
			this->tabPageChangePassword->Location = System::Drawing::Point(4, 27);
			this->tabPageChangePassword->Name = S"tabPageChangePassword";
			this->tabPageChangePassword->Size = System::Drawing::Size(392, 225);
			this->tabPageChangePassword->TabIndex = 1;
			this->tabPageChangePassword->Text = S"Change Password";
			// 
			// buttonChange
			// 
			this->buttonChange->Location = System::Drawing::Point(128, 184);
			this->buttonChange->Name = S"buttonChange";
			this->buttonChange->Size = System::Drawing::Size(96, 24);
			this->buttonChange->TabIndex = 8;
			this->buttonChange->Text = S"Change";
			this->buttonChange->Click += new System::EventHandler(this, eventOnClickChange);
			// 
			// textBoxChangeConfirm
			// 
			this->textBoxChangeConfirm->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->textBoxChangeConfirm->Location = System::Drawing::Point(152, 136);
			this->textBoxChangeConfirm->Name = S"textBoxChangeConfirm";
			this->textBoxChangeConfirm->PasswordChar = '*';
			this->textBoxChangeConfirm->Size = System::Drawing::Size(176, 24);
			this->textBoxChangeConfirm->TabIndex = 7;
			this->textBoxChangeConfirm->Text = S"textBoxChangeConfirm";
			// 
			// labelChangeConfirm
			// 
			this->labelChangeConfirm->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->labelChangeConfirm->Location = System::Drawing::Point(40, 136);
			this->labelChangeConfirm->Name = S"labelChangeConfirm";
			this->labelChangeConfirm->TabIndex = 6;
			this->labelChangeConfirm->Text = S"Confirm";
			this->labelChangeConfirm->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// textBoxNewPassword
			// 
			this->textBoxNewPassword->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->textBoxNewPassword->Location = System::Drawing::Point(152, 96);
			this->textBoxNewPassword->Name = S"textBoxNewPassword";
			this->textBoxNewPassword->PasswordChar = '*';
			this->textBoxNewPassword->Size = System::Drawing::Size(176, 24);
			this->textBoxNewPassword->TabIndex = 5;
			this->textBoxNewPassword->Text = S"textBoxNewPassword";
			// 
			// labelNewPassword
			// 
			this->labelNewPassword->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->labelNewPassword->Location = System::Drawing::Point(40, 96);
			this->labelNewPassword->Name = S"labelNewPassword";
			this->labelNewPassword->TabIndex = 4;
			this->labelNewPassword->Text = S"New Password";
			this->labelNewPassword->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// textBoxOldPassword
			// 
			this->textBoxOldPassword->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->textBoxOldPassword->Location = System::Drawing::Point(152, 56);
			this->textBoxOldPassword->Name = S"textBoxOldPassword";
			this->textBoxOldPassword->PasswordChar = '*';
			this->textBoxOldPassword->Size = System::Drawing::Size(176, 24);
			this->textBoxOldPassword->TabIndex = 3;
			this->textBoxOldPassword->Text = S"textBoxOldPassword";
			// 
			// labelOldPassword
			// 
			this->labelOldPassword->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->labelOldPassword->Location = System::Drawing::Point(40, 56);
			this->labelOldPassword->Name = S"labelOldPassword";
			this->labelOldPassword->TabIndex = 2;
			this->labelOldPassword->Text = S"Old Password";
			this->labelOldPassword->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// textBoxChangeUsername
			// 
			this->textBoxChangeUsername->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->textBoxChangeUsername->Location = System::Drawing::Point(152, 16);
			this->textBoxChangeUsername->Name = S"textBoxChangeUsername";
			this->textBoxChangeUsername->Size = System::Drawing::Size(176, 24);
			this->textBoxChangeUsername->TabIndex = 1;
			this->textBoxChangeUsername->Text = S"textBoxUsername";
			// 
			// labelChangeUsername
			// 
			this->labelChangeUsername->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->labelChangeUsername->Location = System::Drawing::Point(40, 16);
			this->labelChangeUsername->Name = S"labelChangeUsername";
			this->labelChangeUsername->TabIndex = 0;
			this->labelChangeUsername->Text = S"Username";
			this->labelChangeUsername->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// tabPageLog
			// 
			this->tabPageLog->Controls->Add(this->richTextBoxMessage);
			this->tabPageLog->Location = System::Drawing::Point(4, 27);
			this->tabPageLog->Name = S"tabPageLog";
			this->tabPageLog->Size = System::Drawing::Size(392, 225);
			this->tabPageLog->TabIndex = 2;
			this->tabPageLog->Text = S"Log";
			// 
			// richTextBoxMessage
			// 
			this->richTextBoxMessage->Location = System::Drawing::Point(8, 8);
			this->richTextBoxMessage->Name = S"richTextBoxMessage";
			this->richTextBoxMessage->ReadOnly = true;
			this->richTextBoxMessage->Size = System::Drawing::Size(376, 208);
			this->richTextBoxMessage->TabIndex = 0;
			this->richTextBoxMessage->Text = S"richTextBoxMessage";
			this->richTextBoxMessage->WordWrap = false;
			// 
			// CAccount
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(8, 20);
			this->ClientSize = System::Drawing::Size(424, 274);
			this->Controls->Add(this->tabControlAccount);
			this->Font = new System::Drawing::Font(S"Tahoma", 12);
			this->Name = S"CAccount";
			this->Text = S"Account";
			this->Load += new System::EventHandler(this, eventOnLoad);
			this->tabControlAccount->ResumeLayout(false);
			this->tabPageAddAcount->ResumeLayout(false);
			this->tabPageChangePassword->ResumeLayout(false);
			this->tabPageLog->ResumeLayout(false);
			this->ResumeLayout(false);

		}		

private:
	MySQLCAPI	*__mysqlCAPI;
	System::Void eventOnLoad(System::Object *sender, System::EventArgs *e);
	System::Void eventOnClickAdd(System::Object *sender, System::EventArgs *e);
	System::Void eventOnClickChange(System::Object *sender, System::EventArgs *e);

};
}