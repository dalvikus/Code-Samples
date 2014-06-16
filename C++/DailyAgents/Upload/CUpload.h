#pragma once

namespace Upload
{
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary> 
	/// Summary for CUpload
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public __gc class CUpload : public System::Windows::Forms::Form
	{	
	public:
		CUpload(void)
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
	private: System::Windows::Forms::Label *  labelShow;
	private: System::Windows::Forms::TextBox *  textBoxShow;
	private: System::Windows::Forms::GroupBox *  groupBoxShow;










	private: System::Windows::Forms::Label *  labelSeq;
	private: System::Windows::Forms::TextBox *  textBoxSeq;
	private: System::Windows::Forms::Label *  labelShot;
	private: System::Windows::Forms::TextBox *  textBoxShot;





	private: System::Windows::Forms::GroupBox *  groupBoxProject;
	private: System::Windows::Forms::GroupBox *  groupBoxIdentity;
	private: System::Windows::Forms::Button *  buttonNext;
	private: System::Windows::Forms::ComboBox *  comboBoxSection;




	private: System::Windows::Forms::Button *  buttonOpen;


	private: System::Windows::Forms::Label *  labelRevision;
	private: System::Windows::Forms::TextBox *  textBoxRevision;

	private: System::Windows::Forms::RichTextBox *  richTextBoxComment;
	private: System::Windows::Forms::GroupBox *  groupBoxNote;
	private: System::Windows::Forms::Label *  labelPathname;

	private: System::Windows::Forms::GroupBox *  groupBoxImage;

	private: System::Windows::Forms::Label *  labelFrame;
	private: System::Windows::Forms::TextBox *  textBoxFrame;
	private: System::Windows::Forms::TextBox *  textBoxPathname;
	private: System::Windows::Forms::GroupBox *  groupBoxSeq;
	private: System::Windows::Forms::GroupBox *  groupBoxShot;
	private: System::Windows::Forms::GroupBox *  groupBoxGroup;
	private: System::Windows::Forms::Label *  labelGroup;
	private: System::Windows::Forms::TextBox *  textBoxGroup;
	private: System::Windows::Forms::GroupBox *  groupBoxPathname;
	private: System::Windows::Forms::GroupBox *  groupBoxRevision;
	private: System::Windows::Forms::GroupBox *  groupBoxFrame;
	private: System::Windows::Forms::GroupBox *  groupBoxUser;
	private: System::Windows::Forms::Label *  labelUser;
	private: System::Windows::Forms::TextBox *  textBoxUser;
	private: System::Windows::Forms::TabControl *  tabControlUpload;
	private: System::Windows::Forms::TabPage *  tabPageCommonInfo;
	private: System::Windows::Forms::TabPage *  tabPageLog;


	private: System::Windows::Forms::RichTextBox *  richTextBoxMessage;
	private: System::Windows::Forms::TabPage *  tabPageSpecificInfo;
	private: System::Windows::Forms::GroupBox *  groupBoxSpecificInfo;
	private: System::Windows::Forms::GroupBox *  groupBoxTd;
	private: System::Windows::Forms::GroupBox *  groupBoxNonTd;
	private: System::Windows::Forms::Label *  labelTdAnim;
	private: System::Windows::Forms::TextBox *  textBoxTdAnim;
	private: System::Windows::Forms::Label *  labelTdTexture;
	private: System::Windows::Forms::TextBox *  textBoxTdTexture;
	private: System::Windows::Forms::Label *  labelPixelSamples;
	private: System::Windows::Forms::TextBox *  textBoxPixelSamples;
	private: System::Windows::Forms::Label *  labelShadingRate;
	private: System::Windows::Forms::TextBox *  textBoxShadingRate;
	private: System::Windows::Forms::Label *  labelNonTdAnim;
	private: System::Windows::Forms::TextBox *  textBoxNonTdAnim;
	private: System::Windows::Forms::Label *  labelNonTdMdl;
	private: System::Windows::Forms::TextBox *  textBoxNonTdMdl;
	private: System::Windows::Forms::Label *  labelNonTdTexture;
	private: System::Windows::Forms::TextBox *  textBoxNonTdTexture;
	private: System::Windows::Forms::Label *  labelNonTdFx;
	private: System::Windows::Forms::TextBox *  textBoxNonTdFx;
	private: System::Windows::Forms::Label *  labelNonTdTd;
	private: System::Windows::Forms::TextBox *  textBoxNonTdTd;
	private: System::Windows::Forms::Label *  labelNonTdComp;
	private: System::Windows::Forms::TextBox *  textBoxNonTdComp;
	private: System::Windows::Forms::Label *  labelNonTdSetup;
	private: System::Windows::Forms::TextBox *  textBoxNonTdSetup;
	private: System::Windows::Forms::Label *  labelNonTdConcept;
	private: System::Windows::Forms::TextBox *  textBoxNonTdConcept;
	private: System::Windows::Forms::Label *  labelFocalLength;
	private: System::Windows::Forms::TextBox *  textBoxFocalLength;
	private: System::Windows::Forms::Label *  labelFPS;
	private: System::Windows::Forms::TextBox *  textBoxFPS;
	private: System::Windows::Forms::Button *  buttonUpload;
private: System::Windows::Forms::TabPage *  tabPageLogon;
private: System::Windows::Forms::GroupBox *  groupBoxLogon;
private: System::Windows::Forms::Button *  buttonOk;
private: System::Windows::Forms::TextBox *  textBoxPassword;
private: System::Windows::Forms::Label *  labelPassword;
private: System::Windows::Forms::TextBox *  textBoxUsername;
private: System::Windows::Forms::Label *  labelUsername;







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
			this->labelShow = new System::Windows::Forms::Label();
			this->textBoxShow = new System::Windows::Forms::TextBox();
			this->groupBoxShow = new System::Windows::Forms::GroupBox();
			this->groupBoxSeq = new System::Windows::Forms::GroupBox();
			this->labelSeq = new System::Windows::Forms::Label();
			this->textBoxSeq = new System::Windows::Forms::TextBox();
			this->groupBoxShot = new System::Windows::Forms::GroupBox();
			this->labelShot = new System::Windows::Forms::Label();
			this->textBoxShot = new System::Windows::Forms::TextBox();
			this->groupBoxUser = new System::Windows::Forms::GroupBox();
			this->labelUser = new System::Windows::Forms::Label();
			this->textBoxUser = new System::Windows::Forms::TextBox();
			this->groupBoxGroup = new System::Windows::Forms::GroupBox();
			this->labelGroup = new System::Windows::Forms::Label();
			this->textBoxGroup = new System::Windows::Forms::TextBox();
			this->groupBoxProject = new System::Windows::Forms::GroupBox();
			this->groupBoxIdentity = new System::Windows::Forms::GroupBox();
			this->buttonNext = new System::Windows::Forms::Button();
			this->comboBoxSection = new System::Windows::Forms::ComboBox();
			this->groupBoxPathname = new System::Windows::Forms::GroupBox();
			this->buttonOpen = new System::Windows::Forms::Button();
			this->labelPathname = new System::Windows::Forms::Label();
			this->textBoxPathname = new System::Windows::Forms::TextBox();
			this->groupBoxRevision = new System::Windows::Forms::GroupBox();
			this->labelRevision = new System::Windows::Forms::Label();
			this->textBoxRevision = new System::Windows::Forms::TextBox();
			this->groupBoxImage = new System::Windows::Forms::GroupBox();
			this->groupBoxFrame = new System::Windows::Forms::GroupBox();
			this->labelFrame = new System::Windows::Forms::Label();
			this->textBoxFrame = new System::Windows::Forms::TextBox();
			this->richTextBoxComment = new System::Windows::Forms::RichTextBox();
			this->groupBoxNote = new System::Windows::Forms::GroupBox();
			this->richTextBoxMessage = new System::Windows::Forms::RichTextBox();
			this->tabControlUpload = new System::Windows::Forms::TabControl();
			this->tabPageCommonInfo = new System::Windows::Forms::TabPage();
			this->tabPageSpecificInfo = new System::Windows::Forms::TabPage();
			this->buttonUpload = new System::Windows::Forms::Button();
			this->groupBoxSpecificInfo = new System::Windows::Forms::GroupBox();
			this->groupBoxTd = new System::Windows::Forms::GroupBox();
			this->textBoxShadingRate = new System::Windows::Forms::TextBox();
			this->labelShadingRate = new System::Windows::Forms::Label();
			this->textBoxPixelSamples = new System::Windows::Forms::TextBox();
			this->labelPixelSamples = new System::Windows::Forms::Label();
			this->textBoxTdTexture = new System::Windows::Forms::TextBox();
			this->labelTdTexture = new System::Windows::Forms::Label();
			this->textBoxTdAnim = new System::Windows::Forms::TextBox();
			this->labelTdAnim = new System::Windows::Forms::Label();
			this->groupBoxNonTd = new System::Windows::Forms::GroupBox();
			this->textBoxFPS = new System::Windows::Forms::TextBox();
			this->labelFPS = new System::Windows::Forms::Label();
			this->textBoxFocalLength = new System::Windows::Forms::TextBox();
			this->labelFocalLength = new System::Windows::Forms::Label();
			this->textBoxNonTdConcept = new System::Windows::Forms::TextBox();
			this->labelNonTdConcept = new System::Windows::Forms::Label();
			this->textBoxNonTdSetup = new System::Windows::Forms::TextBox();
			this->labelNonTdSetup = new System::Windows::Forms::Label();
			this->textBoxNonTdComp = new System::Windows::Forms::TextBox();
			this->labelNonTdComp = new System::Windows::Forms::Label();
			this->textBoxNonTdTd = new System::Windows::Forms::TextBox();
			this->labelNonTdTd = new System::Windows::Forms::Label();
			this->textBoxNonTdFx = new System::Windows::Forms::TextBox();
			this->labelNonTdFx = new System::Windows::Forms::Label();
			this->textBoxNonTdTexture = new System::Windows::Forms::TextBox();
			this->labelNonTdTexture = new System::Windows::Forms::Label();
			this->textBoxNonTdMdl = new System::Windows::Forms::TextBox();
			this->labelNonTdMdl = new System::Windows::Forms::Label();
			this->textBoxNonTdAnim = new System::Windows::Forms::TextBox();
			this->labelNonTdAnim = new System::Windows::Forms::Label();
			this->tabPageLog = new System::Windows::Forms::TabPage();
			this->tabPageLogon = new System::Windows::Forms::TabPage();
			this->groupBoxLogon = new System::Windows::Forms::GroupBox();
			this->buttonOk = new System::Windows::Forms::Button();
			this->textBoxPassword = new System::Windows::Forms::TextBox();
			this->labelPassword = new System::Windows::Forms::Label();
			this->textBoxUsername = new System::Windows::Forms::TextBox();
			this->labelUsername = new System::Windows::Forms::Label();
			this->groupBoxShow->SuspendLayout();
			this->groupBoxSeq->SuspendLayout();
			this->groupBoxShot->SuspendLayout();
			this->groupBoxUser->SuspendLayout();
			this->groupBoxGroup->SuspendLayout();
			this->groupBoxProject->SuspendLayout();
			this->groupBoxIdentity->SuspendLayout();
			this->groupBoxPathname->SuspendLayout();
			this->groupBoxRevision->SuspendLayout();
			this->groupBoxImage->SuspendLayout();
			this->groupBoxFrame->SuspendLayout();
			this->groupBoxNote->SuspendLayout();
			this->tabControlUpload->SuspendLayout();
			this->tabPageCommonInfo->SuspendLayout();
			this->tabPageSpecificInfo->SuspendLayout();
			this->groupBoxSpecificInfo->SuspendLayout();
			this->groupBoxTd->SuspendLayout();
			this->groupBoxNonTd->SuspendLayout();
			this->tabPageLog->SuspendLayout();
			this->tabPageLogon->SuspendLayout();
			this->groupBoxLogon->SuspendLayout();
			this->SuspendLayout();
			// 
			// labelShow
			// 
			this->labelShow->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->labelShow->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->labelShow->Location = System::Drawing::Point(0, 8);
			this->labelShow->Name = S"labelShow";
			this->labelShow->TabIndex = 10;
			this->labelShow->Text = S"Show";
			this->labelShow->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// textBoxShow
			// 
			this->textBoxShow->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->textBoxShow->Location = System::Drawing::Point(104, 8);
			this->textBoxShow->Name = S"textBoxShow";
			this->textBoxShow->Size = System::Drawing::Size(192, 24);
			this->textBoxShow->TabIndex = 11;
			this->textBoxShow->Text = S"textBoxShow";
			// 
			// groupBoxShow
			// 
			this->groupBoxShow->Controls->Add(this->labelShow);
			this->groupBoxShow->Controls->Add(this->textBoxShow);
			this->groupBoxShow->Location = System::Drawing::Point(8, 24);
			this->groupBoxShow->Name = S"groupBoxShow";
			this->groupBoxShow->Size = System::Drawing::Size(312, 40);
			this->groupBoxShow->TabIndex = 9;
			this->groupBoxShow->TabStop = false;
			// 
			// groupBoxSeq
			// 
			this->groupBoxSeq->Controls->Add(this->labelSeq);
			this->groupBoxSeq->Controls->Add(this->textBoxSeq);
			this->groupBoxSeq->Location = System::Drawing::Point(8, 64);
			this->groupBoxSeq->Name = S"groupBoxSeq";
			this->groupBoxSeq->Size = System::Drawing::Size(312, 40);
			this->groupBoxSeq->TabIndex = 12;
			this->groupBoxSeq->TabStop = false;
			// 
			// labelSeq
			// 
			this->labelSeq->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->labelSeq->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->labelSeq->Location = System::Drawing::Point(0, 8);
			this->labelSeq->Name = S"labelSeq";
			this->labelSeq->TabIndex = 13;
			this->labelSeq->Text = S"Seq";
			this->labelSeq->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// textBoxSeq
			// 
			this->textBoxSeq->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->textBoxSeq->Location = System::Drawing::Point(104, 8);
			this->textBoxSeq->Name = S"textBoxSeq";
			this->textBoxSeq->Size = System::Drawing::Size(192, 24);
			this->textBoxSeq->TabIndex = 14;
			this->textBoxSeq->Text = S"textBoxSeq";
			// 
			// groupBoxShot
			// 
			this->groupBoxShot->Controls->Add(this->labelShot);
			this->groupBoxShot->Controls->Add(this->textBoxShot);
			this->groupBoxShot->Location = System::Drawing::Point(8, 104);
			this->groupBoxShot->Name = S"groupBoxShot";
			this->groupBoxShot->Size = System::Drawing::Size(312, 40);
			this->groupBoxShot->TabIndex = 15;
			this->groupBoxShot->TabStop = false;
			// 
			// labelShot
			// 
			this->labelShot->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->labelShot->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->labelShot->Location = System::Drawing::Point(0, 8);
			this->labelShot->Name = S"labelShot";
			this->labelShot->TabIndex = 16;
			this->labelShot->Text = S"Shot";
			this->labelShot->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// textBoxShot
			// 
			this->textBoxShot->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->textBoxShot->Location = System::Drawing::Point(104, 8);
			this->textBoxShot->Name = S"textBoxShot";
			this->textBoxShot->Size = System::Drawing::Size(192, 24);
			this->textBoxShot->TabIndex = 17;
			this->textBoxShot->Text = S"textBoxShot";
			// 
			// groupBoxUser
			// 
			this->groupBoxUser->Controls->Add(this->labelUser);
			this->groupBoxUser->Controls->Add(this->textBoxUser);
			this->groupBoxUser->Location = System::Drawing::Point(8, 72);
			this->groupBoxUser->Name = S"groupBoxUser";
			this->groupBoxUser->Size = System::Drawing::Size(312, 40);
			this->groupBoxUser->TabIndex = 5;
			this->groupBoxUser->TabStop = false;
			// 
			// labelUser
			// 
			this->labelUser->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->labelUser->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->labelUser->Location = System::Drawing::Point(0, 8);
			this->labelUser->Name = S"labelUser";
			this->labelUser->TabIndex = 6;
			this->labelUser->Text = S"User";
			this->labelUser->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// textBoxUser
			// 
			this->textBoxUser->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->textBoxUser->Location = System::Drawing::Point(104, 8);
			this->textBoxUser->Name = S"textBoxUser";
			this->textBoxUser->Size = System::Drawing::Size(192, 24);
			this->textBoxUser->TabIndex = 7;
			this->textBoxUser->Text = S"textBoxUser";
			// 
			// groupBoxGroup
			// 
			this->groupBoxGroup->Controls->Add(this->labelGroup);
			this->groupBoxGroup->Controls->Add(this->textBoxGroup);
			this->groupBoxGroup->Location = System::Drawing::Point(8, 24);
			this->groupBoxGroup->Name = S"groupBoxGroup";
			this->groupBoxGroup->Size = System::Drawing::Size(312, 40);
			this->groupBoxGroup->TabIndex = 2;
			this->groupBoxGroup->TabStop = false;
			// 
			// labelGroup
			// 
			this->labelGroup->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->labelGroup->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->labelGroup->Location = System::Drawing::Point(0, 8);
			this->labelGroup->Name = S"labelGroup";
			this->labelGroup->TabIndex = 3;
			this->labelGroup->Text = S"Dept.";
			this->labelGroup->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// textBoxGroup
			// 
			this->textBoxGroup->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->textBoxGroup->Location = System::Drawing::Point(104, 8);
			this->textBoxGroup->Name = S"textBoxGroup";
			this->textBoxGroup->Size = System::Drawing::Size(192, 24);
			this->textBoxGroup->TabIndex = 4;
			this->textBoxGroup->Text = S"textBoxGroup";
			// 
			// groupBoxProject
			// 
			this->groupBoxProject->Controls->Add(this->groupBoxShot);
			this->groupBoxProject->Controls->Add(this->groupBoxSeq);
			this->groupBoxProject->Controls->Add(this->groupBoxShow);
			this->groupBoxProject->Font = new System::Drawing::Font(S"Tahoma", 11);
			this->groupBoxProject->Location = System::Drawing::Point(8, 136);
			this->groupBoxProject->Name = S"groupBoxProject";
			this->groupBoxProject->Size = System::Drawing::Size(328, 152);
			this->groupBoxProject->TabIndex = 8;
			this->groupBoxProject->TabStop = false;
			this->groupBoxProject->Text = S"Project";
			// 
			// groupBoxIdentity
			// 
			this->groupBoxIdentity->Controls->Add(this->groupBoxUser);
			this->groupBoxIdentity->Controls->Add(this->groupBoxGroup);
			this->groupBoxIdentity->Font = new System::Drawing::Font(S"Tahoma", 11);
			this->groupBoxIdentity->Location = System::Drawing::Point(8, 8);
			this->groupBoxIdentity->Name = S"groupBoxIdentity";
			this->groupBoxIdentity->Size = System::Drawing::Size(328, 120);
			this->groupBoxIdentity->TabIndex = 1;
			this->groupBoxIdentity->TabStop = false;
			this->groupBoxIdentity->Text = S"Identity";
			// 
			// buttonNext
			// 
			this->buttonNext->Font = new System::Drawing::Font(S"Tahoma", 11);
			this->buttonNext->Location = System::Drawing::Point(120, 576);
			this->buttonNext->Name = S"buttonNext";
			this->buttonNext->Size = System::Drawing::Size(176, 23);
			this->buttonNext->TabIndex = 31;
			this->buttonNext->Text = S"Next";
			this->buttonNext->Click += new System::EventHandler(this, eventOnClickNext);
			// 
			// comboBoxSection
			// 
			this->comboBoxSection->Location = System::Drawing::Point(0, 0);
			this->comboBoxSection->Name = S"comboBoxSection";
			this->comboBoxSection->TabIndex = 0;
			// 
			// groupBoxPathname
			// 
			this->groupBoxPathname->Controls->Add(this->buttonOpen);
			this->groupBoxPathname->Controls->Add(this->labelPathname);
			this->groupBoxPathname->Controls->Add(this->textBoxPathname);
			this->groupBoxPathname->Location = System::Drawing::Point(8, 24);
			this->groupBoxPathname->Name = S"groupBoxPathname";
			this->groupBoxPathname->Size = System::Drawing::Size(568, 40);
			this->groupBoxPathname->TabIndex = 19;
			this->groupBoxPathname->TabStop = false;
			// 
			// buttonOpen
			// 
			this->buttonOpen->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->buttonOpen->Location = System::Drawing::Point(480, 8);
			this->buttonOpen->Name = S"buttonOpen";
			this->buttonOpen->TabIndex = 22;
			this->buttonOpen->Text = S"Open";
			this->buttonOpen->Click += new System::EventHandler(this, eventOnClickOpen);
			// 
			// labelPathname
			// 
			this->labelPathname->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->labelPathname->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->labelPathname->Location = System::Drawing::Point(0, 8);
			this->labelPathname->Name = S"labelPathname";
			this->labelPathname->TabIndex = 20;
			this->labelPathname->Text = S"Pathname";
			this->labelPathname->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// textBoxPathname
			// 
			this->textBoxPathname->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->textBoxPathname->Location = System::Drawing::Point(104, 8);
			this->textBoxPathname->Name = S"textBoxPathname";
			this->textBoxPathname->Size = System::Drawing::Size(360, 24);
			this->textBoxPathname->TabIndex = 21;
			this->textBoxPathname->Text = S"textBoxPathname";
			// 
			// groupBoxRevision
			// 
			this->groupBoxRevision->Controls->Add(this->labelRevision);
			this->groupBoxRevision->Controls->Add(this->textBoxRevision);
			this->groupBoxRevision->Location = System::Drawing::Point(8, 104);
			this->groupBoxRevision->Name = S"groupBoxRevision";
			this->groupBoxRevision->Size = System::Drawing::Size(312, 40);
			this->groupBoxRevision->TabIndex = 26;
			this->groupBoxRevision->TabStop = false;
			// 
			// labelRevision
			// 
			this->labelRevision->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->labelRevision->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->labelRevision->Location = System::Drawing::Point(0, 8);
			this->labelRevision->Name = S"labelRevision";
			this->labelRevision->TabIndex = 27;
			this->labelRevision->Text = S"Revision";
			this->labelRevision->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// textBoxRevision
			// 
			this->textBoxRevision->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->textBoxRevision->Location = System::Drawing::Point(104, 8);
			this->textBoxRevision->Name = S"textBoxRevision";
			this->textBoxRevision->Size = System::Drawing::Size(192, 24);
			this->textBoxRevision->TabIndex = 28;
			this->textBoxRevision->Text = S"textBoxRevision";
			// 
			// groupBoxImage
			// 
			this->groupBoxImage->Controls->Add(this->groupBoxPathname);
			this->groupBoxImage->Controls->Add(this->groupBoxRevision);
			this->groupBoxImage->Controls->Add(this->groupBoxFrame);
			this->groupBoxImage->Font = new System::Drawing::Font(S"Tahoma", 11);
			this->groupBoxImage->Location = System::Drawing::Point(8, 304);
			this->groupBoxImage->Name = S"groupBoxImage";
			this->groupBoxImage->Size = System::Drawing::Size(592, 152);
			this->groupBoxImage->TabIndex = 18;
			this->groupBoxImage->TabStop = false;
			this->groupBoxImage->Text = S"Image";
			// 
			// groupBoxFrame
			// 
			this->groupBoxFrame->Controls->Add(this->labelFrame);
			this->groupBoxFrame->Controls->Add(this->textBoxFrame);
			this->groupBoxFrame->Location = System::Drawing::Point(8, 64);
			this->groupBoxFrame->Name = S"groupBoxFrame";
			this->groupBoxFrame->Size = System::Drawing::Size(312, 40);
			this->groupBoxFrame->TabIndex = 23;
			this->groupBoxFrame->TabStop = false;
			// 
			// labelFrame
			// 
			this->labelFrame->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->labelFrame->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->labelFrame->Location = System::Drawing::Point(0, 8);
			this->labelFrame->Name = S"labelFrame";
			this->labelFrame->TabIndex = 24;
			this->labelFrame->Text = S"Frame";
			this->labelFrame->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// textBoxFrame
			// 
			this->textBoxFrame->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->textBoxFrame->Location = System::Drawing::Point(104, 8);
			this->textBoxFrame->Name = S"textBoxFrame";
			this->textBoxFrame->Size = System::Drawing::Size(192, 24);
			this->textBoxFrame->TabIndex = 25;
			this->textBoxFrame->Text = S"textBoxFrame";
			// 
			// richTextBoxComment
			// 
			this->richTextBoxComment->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->richTextBoxComment->Location = System::Drawing::Point(8, 24);
			this->richTextBoxComment->Name = S"richTextBoxComment";
			this->richTextBoxComment->Size = System::Drawing::Size(488, 64);
			this->richTextBoxComment->TabIndex = 30;
			this->richTextBoxComment->Text = S"richTextBoxComment";
			this->richTextBoxComment->WordWrap = false;
			// 
			// groupBoxNote
			// 
			this->groupBoxNote->Controls->Add(this->richTextBoxComment);
			this->groupBoxNote->Font = new System::Drawing::Font(S"Tahoma", 11);
			this->groupBoxNote->Location = System::Drawing::Point(8, 472);
			this->groupBoxNote->Name = S"groupBoxNote";
			this->groupBoxNote->Size = System::Drawing::Size(504, 96);
			this->groupBoxNote->TabIndex = 29;
			this->groupBoxNote->TabStop = false;
			this->groupBoxNote->Text = S"Note";
			// 
			// richTextBoxMessage
			// 
			this->richTextBoxMessage->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->richTextBoxMessage->Location = System::Drawing::Point(8, 8);
			this->richTextBoxMessage->Name = S"richTextBoxMessage";
			this->richTextBoxMessage->Size = System::Drawing::Size(616, 432);
			this->richTextBoxMessage->TabIndex = 34;
			this->richTextBoxMessage->Text = S"richTextBoxMessage";
			this->richTextBoxMessage->WordWrap = false;
			// 
			// tabControlUpload
			// 
			this->tabControlUpload->Controls->Add(this->tabPageCommonInfo);
			this->tabControlUpload->Controls->Add(this->tabPageSpecificInfo);
			this->tabControlUpload->Controls->Add(this->tabPageLog);
			this->tabControlUpload->Controls->Add(this->tabPageLogon);
			this->tabControlUpload->Location = System::Drawing::Point(8, 8);
			this->tabControlUpload->Name = S"tabControlUpload";
			this->tabControlUpload->SelectedIndex = 0;
			this->tabControlUpload->Size = System::Drawing::Size(640, 640);
			this->tabControlUpload->TabIndex = 33;
			this->tabControlUpload->SelectedIndexChanged += new System::EventHandler(this, eventOnSelectedIndexChanged);
			// 
			// tabPageCommonInfo
			// 
			this->tabPageCommonInfo->AutoScroll = true;
			this->tabPageCommonInfo->Controls->Add(this->groupBoxIdentity);
			this->tabPageCommonInfo->Controls->Add(this->groupBoxProject);
			this->tabPageCommonInfo->Controls->Add(this->buttonNext);
			this->tabPageCommonInfo->Controls->Add(this->groupBoxImage);
			this->tabPageCommonInfo->Controls->Add(this->groupBoxNote);
			this->tabPageCommonInfo->Font = new System::Drawing::Font(S"Tahoma", 11);
			this->tabPageCommonInfo->Location = System::Drawing::Point(4, 28);
			this->tabPageCommonInfo->Name = S"tabPageCommonInfo";
			this->tabPageCommonInfo->Size = System::Drawing::Size(632, 608);
			this->tabPageCommonInfo->TabIndex = 0;
			this->tabPageCommonInfo->Text = S"Common Info";
			// 
			// tabPageSpecificInfo
			// 
			this->tabPageSpecificInfo->AutoScroll = true;
			this->tabPageSpecificInfo->Controls->Add(this->buttonUpload);
			this->tabPageSpecificInfo->Controls->Add(this->groupBoxSpecificInfo);
			this->tabPageSpecificInfo->Font = new System::Drawing::Font(S"Tahoma", 11);
			this->tabPageSpecificInfo->Location = System::Drawing::Point(4, 28);
			this->tabPageSpecificInfo->Name = S"tabPageSpecificInfo";
			this->tabPageSpecificInfo->Size = System::Drawing::Size(632, 608);
			this->tabPageSpecificInfo->TabIndex = 2;
			this->tabPageSpecificInfo->Text = S"Specific Info";
			// 
			// buttonUpload
			// 
			this->buttonUpload->Location = System::Drawing::Point(64, 504);
			this->buttonUpload->Name = S"buttonUpload";
			this->buttonUpload->Size = System::Drawing::Size(160, 23);
			this->buttonUpload->TabIndex = 1;
			this->buttonUpload->Text = S"Upload";
			this->buttonUpload->Click += new System::EventHandler(this, eventOnClickUpload);
			// 
			// groupBoxSpecificInfo
			// 
			this->groupBoxSpecificInfo->Controls->Add(this->groupBoxTd);
			this->groupBoxSpecificInfo->Controls->Add(this->groupBoxNonTd);
			this->groupBoxSpecificInfo->Location = System::Drawing::Point(8, 8);
			this->groupBoxSpecificInfo->Name = S"groupBoxSpecificInfo";
			this->groupBoxSpecificInfo->Size = System::Drawing::Size(568, 472);
			this->groupBoxSpecificInfo->TabIndex = 0;
			this->groupBoxSpecificInfo->TabStop = false;
			// 
			// groupBoxTd
			// 
			this->groupBoxTd->Controls->Add(this->textBoxShadingRate);
			this->groupBoxTd->Controls->Add(this->labelShadingRate);
			this->groupBoxTd->Controls->Add(this->textBoxPixelSamples);
			this->groupBoxTd->Controls->Add(this->labelPixelSamples);
			this->groupBoxTd->Controls->Add(this->textBoxTdTexture);
			this->groupBoxTd->Controls->Add(this->labelTdTexture);
			this->groupBoxTd->Controls->Add(this->textBoxTdAnim);
			this->groupBoxTd->Controls->Add(this->labelTdAnim);
			this->groupBoxTd->Location = System::Drawing::Point(4, 27);
			this->groupBoxTd->Name = S"groupBoxTd";
			this->groupBoxTd->Size = System::Drawing::Size(560, 269);
			this->groupBoxTd->TabIndex = 0;
			this->groupBoxTd->TabStop = false;
			this->groupBoxTd->Text = S"Td Specific Info";
			// 
			// textBoxShadingRate
			// 
			this->textBoxShadingRate->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->textBoxShadingRate->Location = System::Drawing::Point(160, 232);
			this->textBoxShadingRate->Name = S"textBoxShadingRate";
			this->textBoxShadingRate->Size = System::Drawing::Size(168, 24);
			this->textBoxShadingRate->TabIndex = 7;
			this->textBoxShadingRate->Text = S"textBoxShadingRate";
			this->textBoxShadingRate->WordWrap = false;
			// 
			// labelShadingRate
			// 
			this->labelShadingRate->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->labelShadingRate->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->labelShadingRate->Location = System::Drawing::Point(24, 232);
			this->labelShadingRate->Name = S"labelShadingRate";
			this->labelShadingRate->Size = System::Drawing::Size(128, 24);
			this->labelShadingRate->TabIndex = 6;
			this->labelShadingRate->Text = S"Shading Rate";
			this->labelShadingRate->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// textBoxPixelSamples
			// 
			this->textBoxPixelSamples->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->textBoxPixelSamples->Location = System::Drawing::Point(160, 192);
			this->textBoxPixelSamples->Name = S"textBoxPixelSamples";
			this->textBoxPixelSamples->Size = System::Drawing::Size(168, 24);
			this->textBoxPixelSamples->TabIndex = 5;
			this->textBoxPixelSamples->Text = S"textBoxPixelSamples";
			this->textBoxPixelSamples->WordWrap = false;
			// 
			// labelPixelSamples
			// 
			this->labelPixelSamples->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->labelPixelSamples->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->labelPixelSamples->Location = System::Drawing::Point(24, 192);
			this->labelPixelSamples->Name = S"labelPixelSamples";
			this->labelPixelSamples->Size = System::Drawing::Size(128, 24);
			this->labelPixelSamples->TabIndex = 4;
			this->labelPixelSamples->Text = S"Pixel Samples";
			this->labelPixelSamples->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// textBoxTdTexture
			// 
			this->textBoxTdTexture->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->textBoxTdTexture->Location = System::Drawing::Point(160, 104);
			this->textBoxTdTexture->Multiline = true;
			this->textBoxTdTexture->Name = S"textBoxTdTexture";
			this->textBoxTdTexture->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
			this->textBoxTdTexture->Size = System::Drawing::Size(304, 56);
			this->textBoxTdTexture->TabIndex = 3;
			this->textBoxTdTexture->Text = S"textBoxTdTexture";
			this->textBoxTdTexture->WordWrap = false;
			// 
			// labelTdTexture
			// 
			this->labelTdTexture->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->labelTdTexture->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->labelTdTexture->Location = System::Drawing::Point(24, 104);
			this->labelTdTexture->Name = S"labelTdTexture";
			this->labelTdTexture->Size = System::Drawing::Size(128, 24);
			this->labelTdTexture->TabIndex = 2;
			this->labelTdTexture->Text = S"Texture";
			this->labelTdTexture->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// textBoxTdAnim
			// 
			this->textBoxTdAnim->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->textBoxTdAnim->Location = System::Drawing::Point(160, 24);
			this->textBoxTdAnim->Multiline = true;
			this->textBoxTdAnim->Name = S"textBoxTdAnim";
			this->textBoxTdAnim->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
			this->textBoxTdAnim->Size = System::Drawing::Size(304, 56);
			this->textBoxTdAnim->TabIndex = 1;
			this->textBoxTdAnim->Text = S"textBoxTdAnim";
			this->textBoxTdAnim->WordWrap = false;
			// 
			// labelTdAnim
			// 
			this->labelTdAnim->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->labelTdAnim->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->labelTdAnim->Location = System::Drawing::Point(24, 24);
			this->labelTdAnim->Name = S"labelTdAnim";
			this->labelTdAnim->Size = System::Drawing::Size(128, 24);
			this->labelTdAnim->TabIndex = 0;
			this->labelTdAnim->Text = S"Anim";
			this->labelTdAnim->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// groupBoxNonTd
			// 
			this->groupBoxNonTd->Controls->Add(this->textBoxFPS);
			this->groupBoxNonTd->Controls->Add(this->labelFPS);
			this->groupBoxNonTd->Controls->Add(this->textBoxFocalLength);
			this->groupBoxNonTd->Controls->Add(this->labelFocalLength);
			this->groupBoxNonTd->Controls->Add(this->textBoxNonTdConcept);
			this->groupBoxNonTd->Controls->Add(this->labelNonTdConcept);
			this->groupBoxNonTd->Controls->Add(this->textBoxNonTdSetup);
			this->groupBoxNonTd->Controls->Add(this->labelNonTdSetup);
			this->groupBoxNonTd->Controls->Add(this->textBoxNonTdComp);
			this->groupBoxNonTd->Controls->Add(this->labelNonTdComp);
			this->groupBoxNonTd->Controls->Add(this->textBoxNonTdTd);
			this->groupBoxNonTd->Controls->Add(this->labelNonTdTd);
			this->groupBoxNonTd->Controls->Add(this->textBoxNonTdFx);
			this->groupBoxNonTd->Controls->Add(this->labelNonTdFx);
			this->groupBoxNonTd->Controls->Add(this->textBoxNonTdTexture);
			this->groupBoxNonTd->Controls->Add(this->labelNonTdTexture);
			this->groupBoxNonTd->Controls->Add(this->textBoxNonTdMdl);
			this->groupBoxNonTd->Controls->Add(this->labelNonTdMdl);
			this->groupBoxNonTd->Controls->Add(this->textBoxNonTdAnim);
			this->groupBoxNonTd->Controls->Add(this->labelNonTdAnim);
			this->groupBoxNonTd->Location = System::Drawing::Point(4, 27);
			this->groupBoxNonTd->Name = S"groupBoxNonTd";
			this->groupBoxNonTd->Size = System::Drawing::Size(560, 437);
			this->groupBoxNonTd->TabIndex = 1;
			this->groupBoxNonTd->TabStop = false;
			this->groupBoxNonTd->Text = S"Non-Td Specific Info";
			// 
			// textBoxFPS
			// 
			this->textBoxFPS->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->textBoxFPS->Location = System::Drawing::Point(160, 404);
			this->textBoxFPS->Name = S"textBoxFPS";
			this->textBoxFPS->Size = System::Drawing::Size(168, 24);
			this->textBoxFPS->TabIndex = 19;
			this->textBoxFPS->Text = S"textBoxFPS";
			this->textBoxFPS->WordWrap = false;
			// 
			// labelFPS
			// 
			this->labelFPS->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->labelFPS->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->labelFPS->Location = System::Drawing::Point(24, 404);
			this->labelFPS->Name = S"labelFPS";
			this->labelFPS->Size = System::Drawing::Size(128, 24);
			this->labelFPS->TabIndex = 18;
			this->labelFPS->Text = S"FPS";
			this->labelFPS->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// textBoxFocalLength
			// 
			this->textBoxFocalLength->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->textBoxFocalLength->Location = System::Drawing::Point(160, 364);
			this->textBoxFocalLength->Name = S"textBoxFocalLength";
			this->textBoxFocalLength->Size = System::Drawing::Size(168, 24);
			this->textBoxFocalLength->TabIndex = 17;
			this->textBoxFocalLength->Text = S"textBoxFocalLength";
			this->textBoxFocalLength->WordWrap = false;
			// 
			// labelFocalLength
			// 
			this->labelFocalLength->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->labelFocalLength->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->labelFocalLength->Location = System::Drawing::Point(24, 364);
			this->labelFocalLength->Name = S"labelFocalLength";
			this->labelFocalLength->Size = System::Drawing::Size(128, 24);
			this->labelFocalLength->TabIndex = 16;
			this->labelFocalLength->Text = S"Focal Length";
			this->labelFocalLength->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// textBoxNonTdConcept
			// 
			this->textBoxNonTdConcept->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->textBoxNonTdConcept->Location = System::Drawing::Point(160, 304);
			this->textBoxNonTdConcept->Name = S"textBoxNonTdConcept";
			this->textBoxNonTdConcept->Size = System::Drawing::Size(192, 24);
			this->textBoxNonTdConcept->TabIndex = 15;
			this->textBoxNonTdConcept->Text = S"textBoxNonTdConcept";
			this->textBoxNonTdConcept->WordWrap = false;
			// 
			// labelNonTdConcept
			// 
			this->labelNonTdConcept->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->labelNonTdConcept->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->labelNonTdConcept->Location = System::Drawing::Point(24, 304);
			this->labelNonTdConcept->Name = S"labelNonTdConcept";
			this->labelNonTdConcept->Size = System::Drawing::Size(128, 24);
			this->labelNonTdConcept->TabIndex = 14;
			this->labelNonTdConcept->Text = S"Concept";
			this->labelNonTdConcept->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// textBoxNonTdSetup
			// 
			this->textBoxNonTdSetup->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->textBoxNonTdSetup->Location = System::Drawing::Point(160, 264);
			this->textBoxNonTdSetup->Name = S"textBoxNonTdSetup";
			this->textBoxNonTdSetup->Size = System::Drawing::Size(192, 24);
			this->textBoxNonTdSetup->TabIndex = 13;
			this->textBoxNonTdSetup->Text = S"textBoxNonTdSetup";
			this->textBoxNonTdSetup->WordWrap = false;
			// 
			// labelNonTdSetup
			// 
			this->labelNonTdSetup->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->labelNonTdSetup->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->labelNonTdSetup->Location = System::Drawing::Point(24, 264);
			this->labelNonTdSetup->Name = S"labelNonTdSetup";
			this->labelNonTdSetup->Size = System::Drawing::Size(128, 24);
			this->labelNonTdSetup->TabIndex = 12;
			this->labelNonTdSetup->Text = S"Setup";
			this->labelNonTdSetup->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// textBoxNonTdComp
			// 
			this->textBoxNonTdComp->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->textBoxNonTdComp->Location = System::Drawing::Point(160, 224);
			this->textBoxNonTdComp->Name = S"textBoxNonTdComp";
			this->textBoxNonTdComp->Size = System::Drawing::Size(192, 24);
			this->textBoxNonTdComp->TabIndex = 11;
			this->textBoxNonTdComp->Text = S"textBoxNonTdComp";
			this->textBoxNonTdComp->WordWrap = false;
			// 
			// labelNonTdComp
			// 
			this->labelNonTdComp->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->labelNonTdComp->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->labelNonTdComp->Location = System::Drawing::Point(24, 224);
			this->labelNonTdComp->Name = S"labelNonTdComp";
			this->labelNonTdComp->Size = System::Drawing::Size(128, 24);
			this->labelNonTdComp->TabIndex = 10;
			this->labelNonTdComp->Text = S"Comp";
			this->labelNonTdComp->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// textBoxNonTdTd
			// 
			this->textBoxNonTdTd->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->textBoxNonTdTd->Location = System::Drawing::Point(160, 184);
			this->textBoxNonTdTd->Name = S"textBoxNonTdTd";
			this->textBoxNonTdTd->Size = System::Drawing::Size(192, 24);
			this->textBoxNonTdTd->TabIndex = 9;
			this->textBoxNonTdTd->Text = S"textBoxNonTdTd";
			this->textBoxNonTdTd->WordWrap = false;
			// 
			// labelNonTdTd
			// 
			this->labelNonTdTd->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->labelNonTdTd->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->labelNonTdTd->Location = System::Drawing::Point(24, 184);
			this->labelNonTdTd->Name = S"labelNonTdTd";
			this->labelNonTdTd->Size = System::Drawing::Size(128, 24);
			this->labelNonTdTd->TabIndex = 8;
			this->labelNonTdTd->Text = S"Td";
			this->labelNonTdTd->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// textBoxNonTdFx
			// 
			this->textBoxNonTdFx->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->textBoxNonTdFx->Location = System::Drawing::Point(160, 144);
			this->textBoxNonTdFx->Name = S"textBoxNonTdFx";
			this->textBoxNonTdFx->Size = System::Drawing::Size(192, 24);
			this->textBoxNonTdFx->TabIndex = 7;
			this->textBoxNonTdFx->Text = S"textBoxNonTdFx";
			this->textBoxNonTdFx->WordWrap = false;
			// 
			// labelNonTdFx
			// 
			this->labelNonTdFx->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->labelNonTdFx->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->labelNonTdFx->Location = System::Drawing::Point(24, 144);
			this->labelNonTdFx->Name = S"labelNonTdFx";
			this->labelNonTdFx->Size = System::Drawing::Size(128, 24);
			this->labelNonTdFx->TabIndex = 6;
			this->labelNonTdFx->Text = S"Fx";
			this->labelNonTdFx->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// textBoxNonTdTexture
			// 
			this->textBoxNonTdTexture->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->textBoxNonTdTexture->Location = System::Drawing::Point(160, 104);
			this->textBoxNonTdTexture->Name = S"textBoxNonTdTexture";
			this->textBoxNonTdTexture->Size = System::Drawing::Size(192, 24);
			this->textBoxNonTdTexture->TabIndex = 5;
			this->textBoxNonTdTexture->Text = S"textBoxNonTdTexture";
			this->textBoxNonTdTexture->WordWrap = false;
			// 
			// labelNonTdTexture
			// 
			this->labelNonTdTexture->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->labelNonTdTexture->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->labelNonTdTexture->Location = System::Drawing::Point(24, 104);
			this->labelNonTdTexture->Name = S"labelNonTdTexture";
			this->labelNonTdTexture->Size = System::Drawing::Size(128, 24);
			this->labelNonTdTexture->TabIndex = 4;
			this->labelNonTdTexture->Text = S"Texture";
			this->labelNonTdTexture->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// textBoxNonTdMdl
			// 
			this->textBoxNonTdMdl->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->textBoxNonTdMdl->Location = System::Drawing::Point(160, 64);
			this->textBoxNonTdMdl->Name = S"textBoxNonTdMdl";
			this->textBoxNonTdMdl->Size = System::Drawing::Size(192, 24);
			this->textBoxNonTdMdl->TabIndex = 3;
			this->textBoxNonTdMdl->Text = S"textBoxNonTdMdl";
			this->textBoxNonTdMdl->WordWrap = false;
			// 
			// labelNonTdMdl
			// 
			this->labelNonTdMdl->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->labelNonTdMdl->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->labelNonTdMdl->Location = System::Drawing::Point(24, 64);
			this->labelNonTdMdl->Name = S"labelNonTdMdl";
			this->labelNonTdMdl->Size = System::Drawing::Size(128, 24);
			this->labelNonTdMdl->TabIndex = 2;
			this->labelNonTdMdl->Text = S"Mdl";
			this->labelNonTdMdl->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// textBoxNonTdAnim
			// 
			this->textBoxNonTdAnim->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->textBoxNonTdAnim->Location = System::Drawing::Point(160, 24);
			this->textBoxNonTdAnim->Name = S"textBoxNonTdAnim";
			this->textBoxNonTdAnim->Size = System::Drawing::Size(192, 24);
			this->textBoxNonTdAnim->TabIndex = 1;
			this->textBoxNonTdAnim->Text = S"textBoxNonTdAnim";
			this->textBoxNonTdAnim->WordWrap = false;
			// 
			// labelNonTdAnim
			// 
			this->labelNonTdAnim->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->labelNonTdAnim->Font = new System::Drawing::Font(S"Tahoma", 10);
			this->labelNonTdAnim->Location = System::Drawing::Point(24, 24);
			this->labelNonTdAnim->Name = S"labelNonTdAnim";
			this->labelNonTdAnim->Size = System::Drawing::Size(128, 24);
			this->labelNonTdAnim->TabIndex = 0;
			this->labelNonTdAnim->Text = S"Anim";
			this->labelNonTdAnim->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// tabPageLog
			// 
			this->tabPageLog->AutoScroll = true;
			this->tabPageLog->Controls->Add(this->richTextBoxMessage);
			this->tabPageLog->Font = new System::Drawing::Font(S"Tahoma", 11);
			this->tabPageLog->Location = System::Drawing::Point(4, 28);
			this->tabPageLog->Name = S"tabPageLog";
			this->tabPageLog->Size = System::Drawing::Size(632, 608);
			this->tabPageLog->TabIndex = 1;
			this->tabPageLog->Text = S"Log";
			// 
			// tabPageLogon
			// 
			this->tabPageLogon->Controls->Add(this->groupBoxLogon);
			this->tabPageLogon->Font = new System::Drawing::Font(S"Tahoma", 11);
			this->tabPageLogon->Location = System::Drawing::Point(4, 28);
			this->tabPageLogon->Name = S"tabPageLogon";
			this->tabPageLogon->Size = System::Drawing::Size(632, 608);
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
			this->groupBoxLogon->TabIndex = 1;
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
			// CUpload
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(8, 20);
			this->AutoScroll = true;
			this->ClientSize = System::Drawing::Size(656, 656);
			this->Controls->Add(this->tabControlUpload);
			this->Cursor = System::Windows::Forms::Cursors::Default;
			this->Font = new System::Drawing::Font(S"Tahoma", 12);
			this->Name = S"CUpload";
			this->Text = S"Daily Upload";
			this->Load += new System::EventHandler(this, eventOnLoad);
			this->groupBoxShow->ResumeLayout(false);
			this->groupBoxSeq->ResumeLayout(false);
			this->groupBoxShot->ResumeLayout(false);
			this->groupBoxUser->ResumeLayout(false);
			this->groupBoxGroup->ResumeLayout(false);
			this->groupBoxProject->ResumeLayout(false);
			this->groupBoxIdentity->ResumeLayout(false);
			this->groupBoxPathname->ResumeLayout(false);
			this->groupBoxRevision->ResumeLayout(false);
			this->groupBoxImage->ResumeLayout(false);
			this->groupBoxFrame->ResumeLayout(false);
			this->groupBoxNote->ResumeLayout(false);
			this->tabControlUpload->ResumeLayout(false);
			this->tabPageCommonInfo->ResumeLayout(false);
			this->tabPageSpecificInfo->ResumeLayout(false);
			this->groupBoxSpecificInfo->ResumeLayout(false);
			this->groupBoxTd->ResumeLayout(false);
			this->groupBoxNonTd->ResumeLayout(false);
			this->tabPageLog->ResumeLayout(false);
			this->tabPageLogon->ResumeLayout(false);
			this->groupBoxLogon->ResumeLayout(false);
			this->ResumeLayout(false);

		}



private:
	System::Void	eventOnLoad(System::Object *sender, System::EventArgs *e);

	System::Void	eventOnClickOpen(System::Object *sender, System::EventArgs *e);
	System::Void	eventOnSelectedIndexChanged(System::Object *sender, System::EventArgs *e);
	System::Void	eventOnClickNext(System::Object *sender, System::EventArgs *e);
	System::Void	eventOnClickUpload(System::Object *sender, System::EventArgs *e);

	System::Void	eventOnClickOk(System::Object *sender, System::EventArgs *e);

};
}
