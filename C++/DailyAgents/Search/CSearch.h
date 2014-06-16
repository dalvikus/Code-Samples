#pragma once

#include "MySQLCAPI.h"

namespace Search
{
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary> 
	/// Summary for CSearch
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public __gc class CSearch : public System::Windows::Forms::Form
	{	
	public:
		CSearch(void)
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
	private: System::Windows::Forms::GroupBox *  groupBoxIdentity;






	private: System::Windows::Forms::GroupBox *  groupBoxProject;

	private: System::Windows::Forms::Label *  labelShot;
	private: System::Windows::Forms::TextBox *  textBoxShot;



	private: System::Windows::Forms::GroupBox *  groupBoxShow;
	private: System::Windows::Forms::Label *  labelShow;
	private: System::Windows::Forms::TextBox *  textBoxShow;
	private: System::Windows::Forms::GroupBox *  groupBoxUser;
	private: System::Windows::Forms::Label *  labelUser;
	private: System::Windows::Forms::GroupBox *  groupBoxGroup;
	private: System::Windows::Forms::Label *  labelGroup;
	private: System::Windows::Forms::TextBox *  textBoxGroup;
	private: System::Windows::Forms::GroupBox *  groupBoxShot;








	private: System::Windows::Forms::GroupBox *  groupBoxDate;





	private: System::Windows::Forms::ListBox *  listBoxResult;
	private: System::Windows::Forms::GroupBox *  groupBoxNote;
	private: System::Windows::Forms::RichTextBox *  richTextBoxComment;

	private: System::Windows::Forms::Button *  buttonSearch;
	private: System::Windows::Forms::CheckBox *  checkBoxCloseWindow;
	private: System::Windows::Forms::Label *  labelSeq;
	private: System::Windows::Forms::TextBox *  textBoxSeq;
	private: System::Windows::Forms::GroupBox *  groupBoxSeq;
	private: System::Windows::Forms::CheckBox *  checkBoxToday;
	private: System::Windows::Forms::TabControl *  tabControlSearch;
	private: System::Windows::Forms::TabPage *  tabPageSearch;
	private: System::Windows::Forms::TabPage *  tabPageResult;
	private: System::Windows::Forms::GroupBox *  groupBoxLaunch;
	private: System::Windows::Forms::TextBox *  textBoxFPS;
	private: System::Windows::Forms::Label *  labelFPS;
	private: System::Windows::Forms::TextBox *  textBoxUser;
	private: System::Windows::Forms::TabPage *  tabPageLog;
	private: System::Windows::Forms::RichTextBox *  richTextBoxMessage;
	private: System::Windows::Forms::DateTimePicker *  dateTimePickerFrom;
	private: System::Windows::Forms::DateTimePicker *  dateTimePickerTo;
	private: System::Windows::Forms::Label *  labelTo;
	private: System::Windows::Forms::Label *  labelFrom;
	private: System::Windows::Forms::Button *  buttonOpen;
	private: System::Windows::Forms::Label *  labelViewer;
	private: System::Windows::Forms::ComboBox *  comboBoxViewer;
	private: System::Windows::Forms::TabPage *  tabPageLogon;
	private: System::Windows::Forms::GroupBox *  groupBoxLogon;


	private: System::Windows::Forms::Button *  buttonOk;
	private: System::Windows::Forms::Label *  labelUsername;
	private: System::Windows::Forms::TextBox *  textBoxUsername;
	private: System::Windows::Forms::TextBox *  textBoxPassword;
	private: System::Windows::Forms::Label *  labelPassword;
	private: System::Windows::Forms::Button *  buttonDelete;














	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container * components;

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->groupBoxIdentity = new System::Windows::Forms::GroupBox();
			this->groupBoxUser = new System::Windows::Forms::GroupBox();
			this->labelUser = new System::Windows::Forms::Label();
			this->textBoxUser = new System::Windows::Forms::TextBox();
			this->groupBoxGroup = new System::Windows::Forms::GroupBox();
			this->labelGroup = new System::Windows::Forms::Label();
			this->textBoxGroup = new System::Windows::Forms::TextBox();
			this->groupBoxProject = new System::Windows::Forms::GroupBox();
			this->groupBoxShot = new System::Windows::Forms::GroupBox();
			this->labelShot = new System::Windows::Forms::Label();
			this->textBoxShot = new System::Windows::Forms::TextBox();
			this->groupBoxSeq = new System::Windows::Forms::GroupBox();
			this->labelSeq = new System::Windows::Forms::Label();
			this->textBoxSeq = new System::Windows::Forms::TextBox();
			this->groupBoxShow = new System::Windows::Forms::GroupBox();
			this->labelShow = new System::Windows::Forms::Label();
			this->textBoxShow = new System::Windows::Forms::TextBox();
			this->groupBoxDate = new System::Windows::Forms::GroupBox();
			this->labelFrom = new System::Windows::Forms::Label();
			this->labelTo = new System::Windows::Forms::Label();
			this->checkBoxToday = new System::Windows::Forms::CheckBox();
			this->dateTimePickerFrom = new System::Windows::Forms::DateTimePicker();
			this->dateTimePickerTo = new System::Windows::Forms::DateTimePicker();
			this->checkBoxCloseWindow = new System::Windows::Forms::CheckBox();
			this->listBoxResult = new System::Windows::Forms::ListBox();
			this->buttonOpen = new System::Windows::Forms::Button();
			this->groupBoxNote = new System::Windows::Forms::GroupBox();
			this->richTextBoxComment = new System::Windows::Forms::RichTextBox();
			this->buttonSearch = new System::Windows::Forms::Button();
			this->tabControlSearch = new System::Windows::Forms::TabControl();
			this->tabPageSearch = new System::Windows::Forms::TabPage();
			this->tabPageResult = new System::Windows::Forms::TabPage();
			this->groupBoxLaunch = new System::Windows::Forms::GroupBox();
			this->buttonDelete = new System::Windows::Forms::Button();
			this->labelViewer = new System::Windows::Forms::Label();
			this->comboBoxViewer = new System::Windows::Forms::ComboBox();
			this->labelFPS = new System::Windows::Forms::Label();
			this->textBoxFPS = new System::Windows::Forms::TextBox();
			this->tabPageLog = new System::Windows::Forms::TabPage();
			this->richTextBoxMessage = new System::Windows::Forms::RichTextBox();
			this->tabPageLogon = new System::Windows::Forms::TabPage();
			this->groupBoxLogon = new System::Windows::Forms::GroupBox();
			this->buttonOk = new System::Windows::Forms::Button();
			this->textBoxPassword = new System::Windows::Forms::TextBox();
			this->labelPassword = new System::Windows::Forms::Label();
			this->textBoxUsername = new System::Windows::Forms::TextBox();
			this->labelUsername = new System::Windows::Forms::Label();
			this->groupBoxIdentity->SuspendLayout();
			this->groupBoxUser->SuspendLayout();
			this->groupBoxGroup->SuspendLayout();
			this->groupBoxProject->SuspendLayout();
			this->groupBoxShot->SuspendLayout();
			this->groupBoxSeq->SuspendLayout();
			this->groupBoxShow->SuspendLayout();
			this->groupBoxDate->SuspendLayout();
			this->groupBoxNote->SuspendLayout();
			this->tabControlSearch->SuspendLayout();
			this->tabPageSearch->SuspendLayout();
			this->tabPageResult->SuspendLayout();
			this->groupBoxLaunch->SuspendLayout();
			this->tabPageLog->SuspendLayout();
			this->tabPageLogon->SuspendLayout();
			this->groupBoxLogon->SuspendLayout();
			this->SuspendLayout();
			// 
			// groupBoxIdentity
			// 
			this->groupBoxIdentity->Controls->Add(this->groupBoxUser);
			this->groupBoxIdentity->Controls->Add(this->groupBoxGroup);
			this->groupBoxIdentity->Font = new System::Drawing::Font(S"Tahoma", 11);
			this->groupBoxIdentity->Location = System::Drawing::Point(8, 8);
			this->groupBoxIdentity->Name = S"groupBoxIdentity";
			this->groupBoxIdentity->Size = System::Drawing::Size(472, 120);
			this->groupBoxIdentity->TabIndex = 11;
			this->groupBoxIdentity->TabStop = false;
			this->groupBoxIdentity->Text = S"Identity";
			// 
			// groupBoxUser
			// 
			this->groupBoxUser->Controls->Add(this->labelUser);
			this->groupBoxUser->Controls->Add(this->textBoxUser);
			this->groupBoxUser->Location = System::Drawing::Point(8, 72);
			this->groupBoxUser->Name = S"groupBoxUser";
			this->groupBoxUser->Size = System::Drawing::Size(456, 40);
			this->groupBoxUser->TabIndex = 6;
			this->groupBoxUser->TabStop = false;
			// 
			// labelUser
			// 
			this->labelUser->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->labelUser->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->labelUser->Location = System::Drawing::Point(0, 8);
			this->labelUser->Name = S"labelUser";
			this->labelUser->TabIndex = 0;
			this->labelUser->Text = S"User";
			this->labelUser->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// textBoxUser
			// 
			this->textBoxUser->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->textBoxUser->Location = System::Drawing::Point(104, 8);
			this->textBoxUser->Name = S"textBoxUser";
			this->textBoxUser->Size = System::Drawing::Size(192, 24);
			this->textBoxUser->TabIndex = 1;
			this->textBoxUser->Text = S"textBoxUser";
			// 
			// groupBoxGroup
			// 
			this->groupBoxGroup->Controls->Add(this->labelGroup);
			this->groupBoxGroup->Controls->Add(this->textBoxGroup);
			this->groupBoxGroup->Location = System::Drawing::Point(8, 24);
			this->groupBoxGroup->Name = S"groupBoxGroup";
			this->groupBoxGroup->Size = System::Drawing::Size(456, 40);
			this->groupBoxGroup->TabIndex = 7;
			this->groupBoxGroup->TabStop = false;
			// 
			// labelGroup
			// 
			this->labelGroup->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->labelGroup->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->labelGroup->Location = System::Drawing::Point(0, 8);
			this->labelGroup->Name = S"labelGroup";
			this->labelGroup->TabIndex = 0;
			this->labelGroup->Text = S"Dept.";
			this->labelGroup->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// textBoxGroup
			// 
			this->textBoxGroup->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->textBoxGroup->Location = System::Drawing::Point(104, 8);
			this->textBoxGroup->Name = S"textBoxGroup";
			this->textBoxGroup->Size = System::Drawing::Size(192, 24);
			this->textBoxGroup->TabIndex = 1;
			this->textBoxGroup->Text = S"textBoxGroup";
			// 
			// groupBoxProject
			// 
			this->groupBoxProject->Controls->Add(this->groupBoxShot);
			this->groupBoxProject->Controls->Add(this->groupBoxSeq);
			this->groupBoxProject->Controls->Add(this->groupBoxShow);
			this->groupBoxProject->Font = new System::Drawing::Font(S"Tahoma", 11);
			this->groupBoxProject->Location = System::Drawing::Point(8, 144);
			this->groupBoxProject->Name = S"groupBoxProject";
			this->groupBoxProject->Size = System::Drawing::Size(472, 144);
			this->groupBoxProject->TabIndex = 10;
			this->groupBoxProject->TabStop = false;
			this->groupBoxProject->Text = S"Project";
			// 
			// groupBoxShot
			// 
			this->groupBoxShot->Controls->Add(this->labelShot);
			this->groupBoxShot->Controls->Add(this->textBoxShot);
			this->groupBoxShot->Location = System::Drawing::Point(8, 96);
			this->groupBoxShot->Name = S"groupBoxShot";
			this->groupBoxShot->Size = System::Drawing::Size(456, 40);
			this->groupBoxShot->TabIndex = 4;
			this->groupBoxShot->TabStop = false;
			// 
			// labelShot
			// 
			this->labelShot->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->labelShot->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->labelShot->Location = System::Drawing::Point(0, 8);
			this->labelShot->Name = S"labelShot";
			this->labelShot->TabIndex = 0;
			this->labelShot->Text = S"Shot";
			this->labelShot->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// textBoxShot
			// 
			this->textBoxShot->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->textBoxShot->Location = System::Drawing::Point(104, 8);
			this->textBoxShot->Name = S"textBoxShot";
			this->textBoxShot->Size = System::Drawing::Size(192, 24);
			this->textBoxShot->TabIndex = 1;
			this->textBoxShot->Text = S"textBoxShot";
			// 
			// groupBoxSeq
			// 
			this->groupBoxSeq->Controls->Add(this->labelSeq);
			this->groupBoxSeq->Controls->Add(this->textBoxSeq);
			this->groupBoxSeq->Font = new System::Drawing::Font(S"Tahoma", 11);
			this->groupBoxSeq->Location = System::Drawing::Point(8, 56);
			this->groupBoxSeq->Name = S"groupBoxSeq";
			this->groupBoxSeq->Size = System::Drawing::Size(456, 40);
			this->groupBoxSeq->TabIndex = 3;
			this->groupBoxSeq->TabStop = false;
			// 
			// labelSeq
			// 
			this->labelSeq->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->labelSeq->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->labelSeq->Location = System::Drawing::Point(0, 8);
			this->labelSeq->Name = S"labelSeq";
			this->labelSeq->TabIndex = 0;
			this->labelSeq->Text = S"Seq";
			this->labelSeq->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// textBoxSeq
			// 
			this->textBoxSeq->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->textBoxSeq->Location = System::Drawing::Point(104, 8);
			this->textBoxSeq->Name = S"textBoxSeq";
			this->textBoxSeq->Size = System::Drawing::Size(192, 24);
			this->textBoxSeq->TabIndex = 1;
			this->textBoxSeq->Text = S"textBoxSeq";
			// 
			// groupBoxShow
			// 
			this->groupBoxShow->Controls->Add(this->labelShow);
			this->groupBoxShow->Controls->Add(this->textBoxShow);
			this->groupBoxShow->Location = System::Drawing::Point(8, 16);
			this->groupBoxShow->Name = S"groupBoxShow";
			this->groupBoxShow->Size = System::Drawing::Size(456, 40);
			this->groupBoxShow->TabIndex = 2;
			this->groupBoxShow->TabStop = false;
			// 
			// labelShow
			// 
			this->labelShow->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->labelShow->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->labelShow->Location = System::Drawing::Point(0, 8);
			this->labelShow->Name = S"labelShow";
			this->labelShow->TabIndex = 0;
			this->labelShow->Text = S"Show";
			this->labelShow->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// textBoxShow
			// 
			this->textBoxShow->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->textBoxShow->Location = System::Drawing::Point(104, 8);
			this->textBoxShow->Name = S"textBoxShow";
			this->textBoxShow->Size = System::Drawing::Size(192, 24);
			this->textBoxShow->TabIndex = 1;
			this->textBoxShow->Text = S"textBoxShow";
			// 
			// groupBoxDate
			// 
			this->groupBoxDate->Controls->Add(this->labelFrom);
			this->groupBoxDate->Controls->Add(this->labelTo);
			this->groupBoxDate->Controls->Add(this->checkBoxToday);
			this->groupBoxDate->Controls->Add(this->dateTimePickerFrom);
			this->groupBoxDate->Controls->Add(this->dateTimePickerTo);
			this->groupBoxDate->Font = new System::Drawing::Font(S"Tahoma", 11);
			this->groupBoxDate->Location = System::Drawing::Point(8, 296);
			this->groupBoxDate->Name = S"groupBoxDate";
			this->groupBoxDate->Size = System::Drawing::Size(472, 72);
			this->groupBoxDate->TabIndex = 13;
			this->groupBoxDate->TabStop = false;
			this->groupBoxDate->Text = S"Date";
			// 
			// labelFrom
			// 
			this->labelFrom->AutoSize = true;
			this->labelFrom->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->labelFrom->Location = System::Drawing::Point(8, 32);
			this->labelFrom->Name = S"labelFrom";
			this->labelFrom->Size = System::Drawing::Size(41, 20);
			this->labelFrom->TabIndex = 18;
			this->labelFrom->Text = S"From:";
			// 
			// labelTo
			// 
			this->labelTo->AutoSize = true;
			this->labelTo->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->labelTo->Location = System::Drawing::Point(192, 32);
			this->labelTo->Name = S"labelTo";
			this->labelTo->Size = System::Drawing::Size(25, 20);
			this->labelTo->TabIndex = 17;
			this->labelTo->Text = S"To:";
			// 
			// checkBoxToday
			// 
			this->checkBoxToday->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->checkBoxToday->Location = System::Drawing::Point(400, 32);
			this->checkBoxToday->Name = S"checkBoxToday";
			this->checkBoxToday->Size = System::Drawing::Size(64, 24);
			this->checkBoxToday->TabIndex = 1;
			this->checkBoxToday->Text = S"Today";
			this->checkBoxToday->CheckedChanged += new System::EventHandler(this, eventOnCheckedChangedToday);
			// 
			// dateTimePickerFrom
			// 
			this->dateTimePickerFrom->CustomFormat = S"yyyy/MM/dd";
			this->dateTimePickerFrom->Format = System::Windows::Forms::DateTimePickerFormat::Custom;
			this->dateTimePickerFrom->Location = System::Drawing::Point(48, 32);
			this->dateTimePickerFrom->Name = S"dateTimePickerFrom";
			this->dateTimePickerFrom->ShowCheckBox = true;
			this->dateTimePickerFrom->Size = System::Drawing::Size(128, 25);
			this->dateTimePickerFrom->TabIndex = 15;
			this->dateTimePickerFrom->TabStop = false;
			this->dateTimePickerFrom->Value = System::DateTime(2007, 8, 13, 0, 0, 0, 0);
			// 
			// dateTimePickerTo
			// 
			this->dateTimePickerTo->CustomFormat = S"yyyy/MM/dd";
			this->dateTimePickerTo->Format = System::Windows::Forms::DateTimePickerFormat::Custom;
			this->dateTimePickerTo->Location = System::Drawing::Point(216, 32);
			this->dateTimePickerTo->Name = S"dateTimePickerTo";
			this->dateTimePickerTo->ShowCheckBox = true;
			this->dateTimePickerTo->Size = System::Drawing::Size(128, 25);
			this->dateTimePickerTo->TabIndex = 16;
			this->dateTimePickerTo->TabStop = false;
			this->dateTimePickerTo->Value = System::DateTime(2007, 8, 13, 0, 0, 0, 0);
			// 
			// checkBoxCloseWindow
			// 
			this->checkBoxCloseWindow->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->checkBoxCloseWindow->Location = System::Drawing::Point(104, 24);
			this->checkBoxCloseWindow->Name = S"checkBoxCloseWindow";
			this->checkBoxCloseWindow->Size = System::Drawing::Size(112, 24);
			this->checkBoxCloseWindow->TabIndex = 18;
			this->checkBoxCloseWindow->Text = S"Close Window";
			// 
			// listBoxResult
			// 
			this->listBoxResult->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->listBoxResult->ItemHeight = 16;
			this->listBoxResult->Location = System::Drawing::Point(8, 8);
			this->listBoxResult->Name = S"listBoxResult";
			this->listBoxResult->Size = System::Drawing::Size(464, 148);
			this->listBoxResult->TabIndex = 17;
			this->listBoxResult->SelectedIndexChanged += new System::EventHandler(this, eventOnSelectedIndex);
			// 
			// buttonOpen
			// 
			this->buttonOpen->Font = new System::Drawing::Font(S"Tahoma", 11);
			this->buttonOpen->Location = System::Drawing::Point(16, 24);
			this->buttonOpen->Name = S"buttonOpen";
			this->buttonOpen->TabIndex = 0;
			this->buttonOpen->Text = S"Open";
			this->buttonOpen->Click += new System::EventHandler(this, eventOnClickOpen);
			// 
			// groupBoxNote
			// 
			this->groupBoxNote->Controls->Add(this->richTextBoxComment);
			this->groupBoxNote->Font = new System::Drawing::Font(S"Tahoma", 11);
			this->groupBoxNote->Location = System::Drawing::Point(8, 296);
			this->groupBoxNote->Name = S"groupBoxNote";
			this->groupBoxNote->Size = System::Drawing::Size(472, 120);
			this->groupBoxNote->TabIndex = 16;
			this->groupBoxNote->TabStop = false;
			this->groupBoxNote->Text = S"Note";
			// 
			// richTextBoxComment
			// 
			this->richTextBoxComment->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->richTextBoxComment->Location = System::Drawing::Point(8, 24);
			this->richTextBoxComment->Name = S"richTextBoxComment";
			this->richTextBoxComment->ReadOnly = true;
			this->richTextBoxComment->Size = System::Drawing::Size(448, 88);
			this->richTextBoxComment->TabIndex = 14;
			this->richTextBoxComment->Text = S"richTextBoxComment";
			this->richTextBoxComment->WordWrap = false;
			// 
			// buttonSearch
			// 
			this->buttonSearch->Font = new System::Drawing::Font(S"Tahoma", 11);
			this->buttonSearch->Location = System::Drawing::Point(8, 384);
			this->buttonSearch->Name = S"buttonSearch";
			this->buttonSearch->TabIndex = 14;
			this->buttonSearch->Text = S"Search";
			this->buttonSearch->Click += new System::EventHandler(this, eventOnClickSearch);
			// 
			// tabControlSearch
			// 
			this->tabControlSearch->Controls->Add(this->tabPageSearch);
			this->tabControlSearch->Controls->Add(this->tabPageResult);
			this->tabControlSearch->Controls->Add(this->tabPageLog);
			this->tabControlSearch->Controls->Add(this->tabPageLogon);
			this->tabControlSearch->Font = new System::Drawing::Font(S"Tahoma", 11);
			this->tabControlSearch->Location = System::Drawing::Point(8, 8);
			this->tabControlSearch->Name = S"tabControlSearch";
			this->tabControlSearch->SelectedIndex = 0;
			this->tabControlSearch->Size = System::Drawing::Size(496, 456);
			this->tabControlSearch->TabIndex = 16;
			// 
			// tabPageSearch
			// 
			this->tabPageSearch->AutoScroll = true;
			this->tabPageSearch->Controls->Add(this->groupBoxIdentity);
			this->tabPageSearch->Controls->Add(this->groupBoxProject);
			this->tabPageSearch->Controls->Add(this->groupBoxDate);
			this->tabPageSearch->Controls->Add(this->buttonSearch);
			this->tabPageSearch->Font = new System::Drawing::Font(S"Tahoma", 11);
			this->tabPageSearch->Location = System::Drawing::Point(4, 27);
			this->tabPageSearch->Name = S"tabPageSearch";
			this->tabPageSearch->Size = System::Drawing::Size(488, 425);
			this->tabPageSearch->TabIndex = 0;
			this->tabPageSearch->Text = S"Search";
			// 
			// tabPageResult
			// 
			this->tabPageResult->AutoScroll = true;
			this->tabPageResult->Controls->Add(this->groupBoxLaunch);
			this->tabPageResult->Controls->Add(this->listBoxResult);
			this->tabPageResult->Controls->Add(this->groupBoxNote);
			this->tabPageResult->Font = new System::Drawing::Font(S"Tahoma", 11);
			this->tabPageResult->Location = System::Drawing::Point(4, 27);
			this->tabPageResult->Name = S"tabPageResult";
			this->tabPageResult->Size = System::Drawing::Size(488, 425);
			this->tabPageResult->TabIndex = 1;
			this->tabPageResult->Text = S"Result";
			// 
			// groupBoxLaunch
			// 
			this->groupBoxLaunch->Controls->Add(this->buttonDelete);
			this->groupBoxLaunch->Controls->Add(this->labelViewer);
			this->groupBoxLaunch->Controls->Add(this->comboBoxViewer);
			this->groupBoxLaunch->Controls->Add(this->labelFPS);
			this->groupBoxLaunch->Controls->Add(this->textBoxFPS);
			this->groupBoxLaunch->Controls->Add(this->buttonOpen);
			this->groupBoxLaunch->Controls->Add(this->checkBoxCloseWindow);
			this->groupBoxLaunch->Location = System::Drawing::Point(8, 168);
			this->groupBoxLaunch->Name = S"groupBoxLaunch";
			this->groupBoxLaunch->Size = System::Drawing::Size(464, 112);
			this->groupBoxLaunch->TabIndex = 19;
			this->groupBoxLaunch->TabStop = false;
			this->groupBoxLaunch->Text = S"Launch";
			// 
			// buttonDelete
			// 
			this->buttonDelete->Location = System::Drawing::Point(16, 72);
			this->buttonDelete->Name = S"buttonDelete";
			this->buttonDelete->TabIndex = 23;
			this->buttonDelete->Text = S"Delete";
			this->buttonDelete->Click += new System::EventHandler(this, eventOnClickDelete);
			// 
			// labelViewer
			// 
			this->labelViewer->AutoSize = true;
			this->labelViewer->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->labelViewer->Location = System::Drawing::Point(240, 24);
			this->labelViewer->Name = S"labelViewer";
			this->labelViewer->Size = System::Drawing::Size(46, 20);
			this->labelViewer->TabIndex = 22;
			this->labelViewer->Text = S"Viewer";
			this->labelViewer->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// comboBoxViewer
			// 
			this->comboBoxViewer->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->comboBoxViewer->ItemHeight = 16;
			this->comboBoxViewer->Location = System::Drawing::Point(288, 24);
			this->comboBoxViewer->Name = S"comboBoxViewer";
			this->comboBoxViewer->Size = System::Drawing::Size(152, 24);
			this->comboBoxViewer->TabIndex = 21;
			this->comboBoxViewer->Text = S"comboBoxViewer";
			// 
			// labelFPS
			// 
			this->labelFPS->AutoSize = true;
			this->labelFPS->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->labelFPS->Location = System::Drawing::Point(256, 64);
			this->labelFPS->Name = S"labelFPS";
			this->labelFPS->Size = System::Drawing::Size(27, 20);
			this->labelFPS->TabIndex = 20;
			this->labelFPS->Text = S"FPS";
			this->labelFPS->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// textBoxFPS
			// 
			this->textBoxFPS->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->textBoxFPS->Location = System::Drawing::Point(288, 64);
			this->textBoxFPS->Name = S"textBoxFPS";
			this->textBoxFPS->Size = System::Drawing::Size(80, 24);
			this->textBoxFPS->TabIndex = 19;
			this->textBoxFPS->Text = S"textBoxFPS";
			// 
			// tabPageLog
			// 
			this->tabPageLog->AutoScroll = true;
			this->tabPageLog->Controls->Add(this->richTextBoxMessage);
			this->tabPageLog->Font = new System::Drawing::Font(S"Tahoma", 11);
			this->tabPageLog->Location = System::Drawing::Point(4, 27);
			this->tabPageLog->Name = S"tabPageLog";
			this->tabPageLog->Size = System::Drawing::Size(488, 425);
			this->tabPageLog->TabIndex = 2;
			this->tabPageLog->Text = S"Log";
			// 
			// richTextBoxMessage
			// 
			this->richTextBoxMessage->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->richTextBoxMessage->Location = System::Drawing::Point(8, 8);
			this->richTextBoxMessage->Name = S"richTextBoxMessage";
			this->richTextBoxMessage->ReadOnly = true;
			this->richTextBoxMessage->Size = System::Drawing::Size(472, 320);
			this->richTextBoxMessage->TabIndex = 0;
			this->richTextBoxMessage->Text = S"richTextBoxMessage";
			this->richTextBoxMessage->WordWrap = false;
			// 
			// tabPageLogon
			// 
			this->tabPageLogon->AutoScroll = true;
			this->tabPageLogon->Controls->Add(this->groupBoxLogon);
			this->tabPageLogon->Location = System::Drawing::Point(4, 27);
			this->tabPageLogon->Name = S"tabPageLogon";
			this->tabPageLogon->Size = System::Drawing::Size(488, 425);
			this->tabPageLogon->TabIndex = 3;
			this->tabPageLogon->Text = S"Logon";
			// 
			// groupBoxLogon
			// 
			this->groupBoxLogon->Controls->Add(this->buttonOk);
			this->groupBoxLogon->Controls->Add(this->textBoxPassword);
			this->groupBoxLogon->Controls->Add(this->labelPassword);
			this->groupBoxLogon->Controls->Add(this->textBoxUsername);
			this->groupBoxLogon->Controls->Add(this->labelUsername);
			this->groupBoxLogon->Location = System::Drawing::Point(16, 8);
			this->groupBoxLogon->Name = S"groupBoxLogon";
			this->groupBoxLogon->Size = System::Drawing::Size(344, 176);
			this->groupBoxLogon->TabIndex = 0;
			this->groupBoxLogon->TabStop = false;
			// 
			// buttonOk
			// 
			this->buttonOk->Location = System::Drawing::Point(128, 120);
			this->buttonOk->Name = S"buttonOk";
			this->buttonOk->TabIndex = 4;
			this->buttonOk->Text = S"Ok";
			this->buttonOk->Click += new System::EventHandler(this, eventOnClickOk);
			// 
			// textBoxPassword
			// 
			this->textBoxPassword->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->textBoxPassword->Location = System::Drawing::Point(136, 80);
			this->textBoxPassword->Name = S"textBoxPassword";
			this->textBoxPassword->PasswordChar = '*';
			this->textBoxPassword->Size = System::Drawing::Size(184, 24);
			this->textBoxPassword->TabIndex = 3;
			this->textBoxPassword->Text = S"textBoxPassword";
			// 
			// labelPassword
			// 
			this->labelPassword->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->labelPassword->Font = new System::Drawing::Font(S"Tahoma", 11);
			this->labelPassword->Location = System::Drawing::Point(24, 80);
			this->labelPassword->Name = S"labelPassword";
			this->labelPassword->TabIndex = 1;
			this->labelPassword->Text = S"Password";
			this->labelPassword->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// textBoxUsername
			// 
			this->textBoxUsername->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->textBoxUsername->Location = System::Drawing::Point(136, 40);
			this->textBoxUsername->Name = S"textBoxUsername";
			this->textBoxUsername->Size = System::Drawing::Size(184, 24);
			this->textBoxUsername->TabIndex = 1;
			this->textBoxUsername->Text = S"textBoxUsername";
			// 
			// labelUsername
			// 
			this->labelUsername->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->labelUsername->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->labelUsername->Location = System::Drawing::Point(24, 40);
			this->labelUsername->Name = S"labelUsername";
			this->labelUsername->TabIndex = 0;
			this->labelUsername->Text = S"Username";
			this->labelUsername->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// CSearch
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(8, 20);
			this->AutoScroll = true;
			this->ClientSize = System::Drawing::Size(520, 482);
			this->Controls->Add(this->tabControlSearch);
			this->Font = new System::Drawing::Font(S"Tahoma", 12);
			this->Name = S"CSearch";
			this->Text = S"Daily Search";
			this->Load += new System::EventHandler(this, eventOnLoad);
			this->Closed += new System::EventHandler(this, eventOnClosed);
			this->groupBoxIdentity->ResumeLayout(false);
			this->groupBoxUser->ResumeLayout(false);
			this->groupBoxGroup->ResumeLayout(false);
			this->groupBoxProject->ResumeLayout(false);
			this->groupBoxShot->ResumeLayout(false);
			this->groupBoxSeq->ResumeLayout(false);
			this->groupBoxShow->ResumeLayout(false);
			this->groupBoxDate->ResumeLayout(false);
			this->groupBoxNote->ResumeLayout(false);
			this->tabControlSearch->ResumeLayout(false);
			this->tabPageSearch->ResumeLayout(false);
			this->tabPageResult->ResumeLayout(false);
			this->groupBoxLaunch->ResumeLayout(false);
			this->tabPageLog->ResumeLayout(false);
			this->tabPageLogon->ResumeLayout(false);
			this->groupBoxLogon->ResumeLayout(false);
			this->ResumeLayout(false);

		}	

	private:
		MySQLCAPI	*__mysqlCAPI;
private:
	System::Void	eventOnLoad(System::Object *sender, System::EventArgs *e);
	System::Void	eventOnClosed(System::Object *sender, System::EventArgs *e);

	System::Void	eventOnCheckedChangedToday(System::Object *sender, System::EventArgs *e);
	System::Void	eventOnClickSearch(System::Object *sender, System::EventArgs *e);
	System::Void	eventOnClickOpen(System::Object *sender, System::EventArgs *e);
	System::Void	eventOnClickDelete(System::Object *sender, System::EventArgs *e);

	System::Void	eventOnSelectedIndex(System::Object *sender, System::EventArgs *e);

	System::Void	eventOnClickOk(System::Object *sender, System::EventArgs *e);

};
}


