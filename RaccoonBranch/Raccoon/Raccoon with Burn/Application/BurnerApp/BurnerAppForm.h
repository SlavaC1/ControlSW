#pragma once

#include "MaterialMonitor.h"
#include "Utils.h"
#include <time.h>
#include <vcclr.h>

namespace BurnerApp {

	using namespace std;	
	using namespace System::Windows::Forms;	
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::IO;
	using namespace System::Runtime::InteropServices;
	using namespace System::Threading;
	using namespace StratasysIDTSrv;
	using namespace RSCommon;

	void LogMessage(const char* message);

	public ref class BurnerAppForm : public System::Windows::Forms::Form
	{
	public:
		BurnerAppForm(void)
		{
			InitializeComponent();
			this->dtExpirationDate->Value = DateTime::Now.AddYears(2);

			String^ executionDirectory = Path::GetDirectoryName(
				Diagnostics::Process::GetCurrentProcess()->MainModule->FileName);

			Directory::SetCurrentDirectory(executionDirectory);
			openKeyFileDialog->InitialDirectory = executionDirectory;
			m_materialMonitor = new MaterialMonitor();

			SetLogMessageFunction(&LogMessage);
		}

	private: System::Void BurnerAppForm_Shown(System::Object^  sender, System::EventArgs^  e) {
				 bgInitHW->RunWorkerAsync();
				 this->Refresh();
			 }

	private: System::Void bgInitHW_DoWork(System::Object^  sender, System::ComponentModel::DoWorkEventArgs^  e) {
				 if (InvokeRequired)
				 {
					 BeginInvoke(
						 gcnew System::ComponentModel::DoWorkEventHandler(this, &BurnerAppForm::bgInitHW_DoWork),
						 sender, e);

					 return;
				 }

				 grpIdentificationData->Enabled = false;
				 grpBurning->Enabled = false;
				 this->Refresh();

				 lblStatus->Text = "Initializing...";
				 unsigned char cartridgesCount;
				 int returnValue = m_materialMonitor->InitHW(&cartridgesCount);
				 if (returnValue != IDTLIB_SUCCESS)
				 {
					 MessageBox::Show(gcnew String(GetError(returnValue)), gcnew String("Error initializing HW"));
				 }
				 else
				 {
					 GetInPlaceStatus();
					 grpIdentificationData->Enabled = true;
					 grpBurning->Enabled = true;
				 }

				 lblStatus->Text = String::Empty;
				 this->Refresh();
				 e->Result = gcnew Int32(returnValue);
			 }

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~BurnerAppForm()
		{
			if (components)
			{
				delete components;
			}
		}

	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::TextBox^  txtMaterialType;
	private: System::Windows::Forms::TextBox^  txtTagStructID;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::ComboBox^  cmbCartridgeNo;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::TextBox^  txtInitialWeight;
	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::TextBox^  txtBatchNo;
	private: System::Windows::Forms::Label^  label6;
	private: System::Windows::Forms::Label^  label7;
	private: System::Windows::Forms::DateTimePicker^  dtManufacturingDate;
	private: System::Windows::Forms::DateTimePicker^  dtExpirationDate;
	private: System::Windows::Forms::ListBox^  lstLog;
	private: System::Windows::Forms::OpenFileDialog^  openKeyFileDialog;
	private: System::Windows::Forms::TextBox^  txtPrivateKey;
	private: System::Windows::Forms::Label^  label8;
	private: System::Windows::Forms::TextBox^  txtPublicKey;
	private: System::Windows::Forms::Label^  label9;
	private: System::Windows::Forms::Button^  btnOpenPrivateKeyFile;
	private: System::Windows::Forms::Button^  btnOpenPublicKeyFile;
	private: System::Windows::Forms::GroupBox^  grpIdentificationData;
	private: System::Windows::Forms::GroupBox^  grpBurning;
	private: System::Windows::Forms::Button^  btnErase;
	private: System::Windows::Forms::Button^  btnModify;
	private: System::Windows::Forms::Button^  btnRead;

	private: System::ComponentModel::BackgroundWorker^  bgInitHW;
	private: System::Windows::Forms::Button^  btnRefresh;
	private: System::Windows::Forms::StatusStrip^  statusStrip1;
	private: System::Windows::Forms::ToolStripStatusLabel^  lblStatus;

	private: System::Windows::Forms::Button^  btnPrepare;

	public: void WriteMessage(const char* message)
			{
				lstLog->Items->Add(gcnew String(message));
				lstLog->Refresh();
			} 

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;
		MaterialMonitor* m_materialMonitor;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(BurnerAppForm::typeid));
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->txtMaterialType = (gcnew System::Windows::Forms::TextBox());
			this->txtTagStructID = (gcnew System::Windows::Forms::TextBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->cmbCartridgeNo = (gcnew System::Windows::Forms::ComboBox());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->txtInitialWeight = (gcnew System::Windows::Forms::TextBox());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->txtBatchNo = (gcnew System::Windows::Forms::TextBox());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->dtManufacturingDate = (gcnew System::Windows::Forms::DateTimePicker());
			this->dtExpirationDate = (gcnew System::Windows::Forms::DateTimePicker());
			this->lstLog = (gcnew System::Windows::Forms::ListBox());
			this->openKeyFileDialog = (gcnew System::Windows::Forms::OpenFileDialog());
			this->txtPrivateKey = (gcnew System::Windows::Forms::TextBox());
			this->label8 = (gcnew System::Windows::Forms::Label());
			this->txtPublicKey = (gcnew System::Windows::Forms::TextBox());
			this->label9 = (gcnew System::Windows::Forms::Label());
			this->btnOpenPrivateKeyFile = (gcnew System::Windows::Forms::Button());
			this->btnOpenPublicKeyFile = (gcnew System::Windows::Forms::Button());
			this->grpIdentificationData = (gcnew System::Windows::Forms::GroupBox());
			this->btnRefresh = (gcnew System::Windows::Forms::Button());
			this->grpBurning = (gcnew System::Windows::Forms::GroupBox());
			this->btnErase = (gcnew System::Windows::Forms::Button());
			this->btnModify = (gcnew System::Windows::Forms::Button());
			this->btnRead = (gcnew System::Windows::Forms::Button());
			this->btnPrepare = (gcnew System::Windows::Forms::Button());
			this->bgInitHW = (gcnew System::ComponentModel::BackgroundWorker());
			this->statusStrip1 = (gcnew System::Windows::Forms::StatusStrip());
			this->lblStatus = (gcnew System::Windows::Forms::ToolStripStatusLabel());
			this->grpIdentificationData->SuspendLayout();
			this->grpBurning->SuspendLayout();
			this->statusStrip1->SuspendLayout();
			this->SuspendLayout();
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(6, 74);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(70, 13);
			this->label1->TabIndex = 5;
			this->label1->Text = L"Material type:";
			// 
			// txtMaterialType
			// 
			this->txtMaterialType->Location = System::Drawing::Point(114, 71);
			this->txtMaterialType->Name = L"txtMaterialType";
			this->txtMaterialType->Size = System::Drawing::Size(210, 20);
			this->txtMaterialType->TabIndex = 6;
			// 
			// txtTagStructID
			// 
			this->txtTagStructID->Location = System::Drawing::Point(114, 45);
			this->txtTagStructID->Name = L"txtTagStructID";
			this->txtTagStructID->Size = System::Drawing::Size(210, 20);
			this->txtTagStructID->TabIndex = 4;
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(7, 48);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(72, 13);
			this->label2->TabIndex = 3;
			this->label2->Text = L"Tag struct ID:";
			// 
			// cmbCartridgeNo
			// 
			this->cmbCartridgeNo->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cmbCartridgeNo->Location = System::Drawing::Point(114, 19);
			this->cmbCartridgeNo->Name = L"cmbCartridgeNo";
			this->cmbCartridgeNo->Size = System::Drawing::Size(48, 21);
			this->cmbCartridgeNo->TabIndex = 1;
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(6, 22);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(90, 13);
			this->label3->TabIndex = 0;
			this->label3->Text = L"Cartridge number:";
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(6, 103);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(102, 13);
			this->label4->TabIndex = 7;
			this->label4->Text = L"Manufacturing date:";
			// 
			// txtInitialWeight
			// 
			this->txtInitialWeight->Location = System::Drawing::Point(114, 149);
			this->txtInitialWeight->Name = L"txtInitialWeight";
			this->txtInitialWeight->Size = System::Drawing::Size(210, 20);
			this->txtInitialWeight->TabIndex = 12;
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(6, 152);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(68, 13);
			this->label5->TabIndex = 11;
			this->label5->Text = L"Initial weight:";
			// 
			// txtBatchNo
			// 
			this->txtBatchNo->Location = System::Drawing::Point(114, 123);
			this->txtBatchNo->Name = L"txtBatchNo";
			this->txtBatchNo->Size = System::Drawing::Size(210, 20);
			this->txtBatchNo->TabIndex = 10;
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Location = System::Drawing::Point(6, 126);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(76, 13);
			this->label6->TabIndex = 9;
			this->label6->Text = L"Batch number:";
			// 
			// label7
			// 
			this->label7->AutoSize = true;
			this->label7->Location = System::Drawing::Point(6, 181);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(80, 13);
			this->label7->TabIndex = 13;
			this->label7->Text = L"Expiration date:";
			// 
			// dtManufacturingDate
			// 
			this->dtManufacturingDate->CustomFormat = L"dd-MMM-yyyy HH:mm::ss";
			this->dtManufacturingDate->Format = System::Windows::Forms::DateTimePickerFormat::Custom;
			this->dtManufacturingDate->Location = System::Drawing::Point(114, 97);
			this->dtManufacturingDate->Name = L"dtManufacturingDate";
			this->dtManufacturingDate->Size = System::Drawing::Size(210, 20);
			this->dtManufacturingDate->TabIndex = 8;
			// 
			// dtExpirationDate
			// 
			this->dtExpirationDate->CustomFormat = L"dd-MMM-yyyy HH:mm::ss";
			this->dtExpirationDate->Format = System::Windows::Forms::DateTimePickerFormat::Custom;
			this->dtExpirationDate->Location = System::Drawing::Point(114, 175);
			this->dtExpirationDate->Name = L"dtExpirationDate";
			this->dtExpirationDate->Size = System::Drawing::Size(210, 20);
			this->dtExpirationDate->TabIndex = 14;
			// 
			// lstLog
			// 
			this->lstLog->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->lstLog->FormattingEnabled = true;
			this->lstLog->HorizontalScrollbar = true;
			this->lstLog->Location = System::Drawing::Point(348, 22);
			this->lstLog->Name = L"lstLog";
			this->lstLog->SelectionMode = System::Windows::Forms::SelectionMode::None;
			this->lstLog->Size = System::Drawing::Size(411, 368);
			this->lstLog->TabIndex = 2;
			// 
			// openKeyFileDialog
			// 
			this->openKeyFileDialog->FileName = L"prvKS.key";
			// 
			// txtPrivateKey
			// 
			this->txtPrivateKey->Location = System::Drawing::Point(114, 201);
			this->txtPrivateKey->Name = L"txtPrivateKey";
			this->txtPrivateKey->Size = System::Drawing::Size(171, 20);
			this->txtPrivateKey->TabIndex = 16;
			this->txtPrivateKey->Text = L"Keys\\prvKS.key";
			// 
			// label8
			// 
			this->label8->AutoSize = true;
			this->label8->Location = System::Drawing::Point(6, 204);
			this->label8->Name = L"label8";
			this->label8->Size = System::Drawing::Size(63, 13);
			this->label8->TabIndex = 15;
			this->label8->Text = L"Private key:";
			// 
			// txtPublicKey
			// 
			this->txtPublicKey->Location = System::Drawing::Point(114, 227);
			this->txtPublicKey->Name = L"txtPublicKey";
			this->txtPublicKey->Size = System::Drawing::Size(171, 20);
			this->txtPublicKey->TabIndex = 19;
			this->txtPublicKey->Text = L"Keys\\pubKS.key";
			// 
			// label9
			// 
			this->label9->AutoSize = true;
			this->label9->Location = System::Drawing::Point(6, 230);
			this->label9->Name = L"label9";
			this->label9->Size = System::Drawing::Size(59, 13);
			this->label9->TabIndex = 18;
			this->label9->Text = L"Public key:";
			// 
			// btnOpenPrivateKeyFile
			// 
			this->btnOpenPrivateKeyFile->Location = System::Drawing::Point(291, 199);
			this->btnOpenPrivateKeyFile->Name = L"btnOpenPrivateKeyFile";
			this->btnOpenPrivateKeyFile->Size = System::Drawing::Size(33, 23);
			this->btnOpenPrivateKeyFile->TabIndex = 17;
			this->btnOpenPrivateKeyFile->Text = L"...";
			this->btnOpenPrivateKeyFile->UseVisualStyleBackColor = true;
			this->btnOpenPrivateKeyFile->Click += gcnew System::EventHandler(this, &BurnerAppForm::btnOpenPrivateKeyFile_Click);
			// 
			// btnOpenPublicKeyFile
			// 
			this->btnOpenPublicKeyFile->Location = System::Drawing::Point(291, 225);
			this->btnOpenPublicKeyFile->Name = L"btnOpenPublicKeyFile";
			this->btnOpenPublicKeyFile->Size = System::Drawing::Size(33, 23);
			this->btnOpenPublicKeyFile->TabIndex = 20;
			this->btnOpenPublicKeyFile->Text = L"...";
			this->btnOpenPublicKeyFile->UseVisualStyleBackColor = true;
			this->btnOpenPublicKeyFile->Click += gcnew System::EventHandler(this, &BurnerAppForm::btnOpenPublicKeyFile_Click);
			// 
			// grpIdentificationData
			// 
			this->grpIdentificationData->Controls->Add(this->btnRefresh);
			this->grpIdentificationData->Controls->Add(this->label2);
			this->grpIdentificationData->Controls->Add(this->btnOpenPublicKeyFile);
			this->grpIdentificationData->Controls->Add(this->btnOpenPrivateKeyFile);
			this->grpIdentificationData->Controls->Add(this->label1);
			this->grpIdentificationData->Controls->Add(this->txtPublicKey);
			this->grpIdentificationData->Controls->Add(this->txtTagStructID);
			this->grpIdentificationData->Controls->Add(this->txtMaterialType);
			this->grpIdentificationData->Controls->Add(this->label9);
			this->grpIdentificationData->Controls->Add(this->txtPrivateKey);
			this->grpIdentificationData->Controls->Add(this->label3);
			this->grpIdentificationData->Controls->Add(this->label8);
			this->grpIdentificationData->Controls->Add(this->cmbCartridgeNo);
			this->grpIdentificationData->Controls->Add(this->label4);
			this->grpIdentificationData->Controls->Add(this->label5);
			this->grpIdentificationData->Controls->Add(this->dtExpirationDate);
			this->grpIdentificationData->Controls->Add(this->txtInitialWeight);
			this->grpIdentificationData->Controls->Add(this->dtManufacturingDate);
			this->grpIdentificationData->Controls->Add(this->label6);
			this->grpIdentificationData->Controls->Add(this->label7);
			this->grpIdentificationData->Controls->Add(this->txtBatchNo);
			this->grpIdentificationData->Location = System::Drawing::Point(12, 12);
			this->grpIdentificationData->Name = L"grpIdentificationData";
			this->grpIdentificationData->Size = System::Drawing::Size(330, 317);
			this->grpIdentificationData->TabIndex = 0;
			this->grpIdentificationData->TabStop = false;
			this->grpIdentificationData->Text = L"Identification Data";
			// 
			// btnRefresh
			// 
			this->btnRefresh->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"btnRefresh.Image")));
			this->btnRefresh->Location = System::Drawing::Point(168, 17);
			this->btnRefresh->Name = L"btnRefresh";
			this->btnRefresh->Size = System::Drawing::Size(24, 24);
			this->btnRefresh->TabIndex = 2;
			this->btnRefresh->UseVisualStyleBackColor = true;
			this->btnRefresh->Click += gcnew System::EventHandler(this, &BurnerAppForm::btnRefresh_Click);
			// 
			// grpBurning
			// 
			this->grpBurning->Controls->Add(this->btnErase);
			this->grpBurning->Controls->Add(this->btnModify);
			this->grpBurning->Controls->Add(this->btnRead);
			this->grpBurning->Controls->Add(this->btnPrepare);
			this->grpBurning->Location = System::Drawing::Point(12, 335);
			this->grpBurning->Name = L"grpBurning";
			this->grpBurning->Size = System::Drawing::Size(330, 53);
			this->grpBurning->TabIndex = 1;
			this->grpBurning->TabStop = false;
			this->grpBurning->Text = L"Burning";
			// 
			// btnErase
			// 
			this->btnErase->Location = System::Drawing::Point(249, 19);
			this->btnErase->Name = L"btnErase";
			this->btnErase->Size = System::Drawing::Size(75, 23);
			this->btnErase->TabIndex = 3;
			this->btnErase->Text = L"Erase";
			this->btnErase->UseVisualStyleBackColor = true;
			this->btnErase->Click += gcnew System::EventHandler(this, &BurnerAppForm::btnErase_Click);
			// 
			// btnModify
			// 
			this->btnModify->Location = System::Drawing::Point(168, 19);
			this->btnModify->Name = L"btnModify";
			this->btnModify->Size = System::Drawing::Size(75, 23);
			this->btnModify->TabIndex = 2;
			this->btnModify->Text = L"Modify";
			this->btnModify->UseVisualStyleBackColor = true;
			this->btnModify->Click += gcnew System::EventHandler(this, &BurnerAppForm::btnModify_Click);
			// 
			// btnRead
			// 
			this->btnRead->Location = System::Drawing::Point(87, 19);
			this->btnRead->Name = L"btnRead";
			this->btnRead->Size = System::Drawing::Size(75, 23);
			this->btnRead->TabIndex = 1;
			this->btnRead->Text = L"Read";
			this->btnRead->UseVisualStyleBackColor = true;
			this->btnRead->Click += gcnew System::EventHandler(this, &BurnerAppForm::btnRead_Click);
			// 
			// btnPrepare
			// 
			this->btnPrepare->Location = System::Drawing::Point(6, 19);
			this->btnPrepare->Name = L"btnPrepare";
			this->btnPrepare->Size = System::Drawing::Size(75, 23);
			this->btnPrepare->TabIndex = 0;
			this->btnPrepare->Text = L"Prepare";
			this->btnPrepare->UseVisualStyleBackColor = true;
			this->btnPrepare->Click += gcnew System::EventHandler(this, &BurnerAppForm::btnPrepare_Click);
			// 
			// bgInitHW
			// 
			this->bgInitHW->DoWork += gcnew System::ComponentModel::DoWorkEventHandler(this, &BurnerAppForm::bgInitHW_DoWork);
			// 
			// statusStrip1
			// 
			this->statusStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->lblStatus});
			this->statusStrip1->Location = System::Drawing::Point(0, 391);
			this->statusStrip1->Name = L"statusStrip1";
			this->statusStrip1->Size = System::Drawing::Size(775, 22);
			this->statusStrip1->SizingGrip = false;
			this->statusStrip1->TabIndex = 3;
			this->statusStrip1->Text = L"statusStrip1";
			// 
			// lblStatus
			// 
			this->lblStatus->Name = L"lblStatus";
			this->lblStatus->Size = System::Drawing::Size(0, 17);
			this->lblStatus->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// BurnerAppForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(775, 413);
			this->Controls->Add(this->statusStrip1);
			this->Controls->Add(this->grpBurning);
			this->Controls->Add(this->grpIdentificationData);
			this->Controls->Add(this->lstLog);
			this->DoubleBuffered = true;
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->Icon = (cli::safe_cast<System::Drawing::Icon^  >(resources->GetObject(L"$this.Icon")));
			this->MaximizeBox = false;
			this->Name = L"BurnerAppForm";
			this->Text = L"Burner Demo";
			this->FormClosed += gcnew System::Windows::Forms::FormClosedEventHandler(this, &BurnerAppForm::BurnerAppForm_FormClosed);
			this->Shown += gcnew System::EventHandler(this, &BurnerAppForm::BurnerAppForm_Shown);
			this->grpIdentificationData->ResumeLayout(false);
			this->grpIdentificationData->PerformLayout();
			this->grpBurning->ResumeLayout(false);
			this->statusStrip1->ResumeLayout(false);
			this->statusStrip1->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

	private: System::Void BurnerAppForm_FormClosed(System::Object^  sender, System::Windows::Forms::FormClosedEventArgs^  e) {
				 delete m_materialMonitor;
			 }

	private: System::Void btnOpenPrivateKeyFile_Click(System::Object^  sender, System::EventArgs^  e) {
				 openKeyFileDialog->FileName = txtPrivateKey->Text;
				 if (openKeyFileDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK)
				 {
					 txtPrivateKey->Text = openKeyFileDialog->FileName;
				 }
			 }

	private: System::Void btnOpenPublicKeyFile_Click(System::Object^  sender, System::EventArgs^  e) {
				 openKeyFileDialog->FileName = txtPublicKey->Text;
				 if (openKeyFileDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK)
				 {
					 txtPublicKey->Text = openKeyFileDialog->FileName;
				 }
			 }

	private: System::Void btnPrepare_Click(System::Object^  sender, System::EventArgs^  e) {
				 Execute(gcnew Action<unsigned char>(this, &BurnerAppForm::Burn));
			 }

	private: System::Void btnRead_Click(System::Object^  sender, System::EventArgs^  e) {
				 Execute(gcnew Action<unsigned char>(this, &BurnerAppForm::Read));
			 }

	private: System::Void btnModify_Click(System::Object^  sender, System::EventArgs^  e) {
				 Execute(gcnew Action<unsigned char>(this, &BurnerAppForm::Modify));
			 }

	private: System::Void btnErase_Click(System::Object^  sender, System::EventArgs^  e) {
				 Execute(gcnew Action<unsigned char>(this, &BurnerAppForm::Erase));
			 }

	private: void GetInPlaceStatus()
			 {
				 cmbCartridgeNo->Items->Clear();

				 int status;
				 int returnValue = m_materialMonitor->GetInPlaceStatus(&status);

				 if ((returnValue == IDTLIB_SUCCESS) && (status != 0))
				 {
					 for (byte b = 0; b < 0x20; b++)
					 {
						 if ((status & (1 << b)) != 0)
						 {
							 cmbCartridgeNo->Items->Add(b);
						 }
					 }

					 cmbCartridgeNo->SelectedIndex = 0;
				 }
			 }


	private: void Execute(Action<unsigned char>^ function)
			 {
				 ClearLogs();
				 grpIdentificationData->Enabled = false;
				 grpBurning->Enabled = false;
				 this->Refresh();

				 unsigned char cartridgeNumber = 0;
				 bool ok = Byte::TryParse(cmbCartridgeNo->Text, cartridgeNumber);
				 if (ok)
				 {
					 try
					 {
						 function(cartridgeNumber);
					 }
					 catch (System::Exception^ ex)
					 {
						 lstLog->Items->Add(ex->Message);
						 lstLog->Refresh();
					 }
				 }
				 else
				 {
					 lblStatus->ForeColor = Color::FromKnownColor(KnownColor::Red);
					 lblStatus->Text = L"Invalid cartridge number.";
				 }

				 grpIdentificationData->Enabled = true;
				 grpBurning->Enabled = true;

				 this->Refresh();
			 }

	private: void ClearLogs()
			 {
				 lblStatus->Text = "";
				 lstLog->Items->Clear();
			 }

	private: bool ReadMaterialInfo(unsigned char* aucMaterialInfo, unsigned short* usMaterialInfoLength)
			 {
				 bool ok;

				 try
				 {
					 MaterialInformation materialInfo;
					 ok = UInt16::TryParse(txtTagStructID->Text, materialInfo.TagStructID);
					 if (!ok)
					 {
						 throw gcnew FormatException("Invalid tag struct ID.");
					 }

					 ok = UInt16::TryParse(txtMaterialType->Text, materialInfo.MaterialType);
					 if (!ok)
					 {
						 throw gcnew FormatException("Invalid material type.");
					 }

					 memset(materialInfo.BatchNo, 0, BATCH_NO_SIZE);
					 pin_ptr<const wchar_t> wch = PtrToStringChars(txtBatchNo->Text);

					 size_t convertedChars;
					 int length = 2 * Math::Min(txtBatchNo->Text->Length, BATCH_NO_SIZE);
					 if (length > 0)
					 {
						 errno_t err = wcstombs_s(&convertedChars, materialInfo.BatchNo, length, wch, length);
						 ok = (err == 0);
					 }
					 else
					 {
						 ok = false;
					 }

					 if (!ok)
					 {
						 throw gcnew FormatException("Invalid batch number.");
					 }

					 materialInfo.MfgDateTime = (TAG_TYPE__MFG_DATE_TIME)dtManufacturingDate->Value.Subtract(DateTime(1970, 1, 1)).TotalSeconds;
					 materialInfo.ExpirationDate = (TAG_TYPE__EXPIRATION_DATE)dtExpirationDate->Value.Subtract(DateTime(1970, 1, 1)).TotalSeconds;

					 ok = UInt16::TryParse(txtInitialWeight->Text, materialInfo.InitialWeight);
					 if (!ok)
					 {
						 throw gcnew FormatException("Invalid initial weight.");
					 }

					 *usMaterialInfoLength = (unsigned short)materialInfo.Encode(aucMaterialInfo);
				 }
				 catch (System::Exception^ ex)
				 {
					 lblStatus->ForeColor = Color::FromKnownColor(KnownColor::Red);
					 lblStatus->Text = ex->Message;
				 }

				 return ok;
			 }

	private: bool ReadKeys(unsigned char* aucPrvKS, unsigned char* aucPubKS)
			 {
				 if (aucPrvKS != NULL)
				 {
					 try
					 {
						 array<unsigned char>^ prvKS = System::IO::File::ReadAllBytes(txtPrivateKey->Text);
						 Marshal::Copy(prvKS, 0, IntPtr(aucPrvKS), prvKS->Length);
					 }
					 catch (...)
					 {
						 lblStatus->ForeColor = Color::FromKnownColor(KnownColor::Red);
						 lblStatus->Text = L"Invalid private key file.";
						 return false;
					 }
				 }

				 if (aucPubKS != NULL)
				 {
					 try
					 {
						 array<unsigned char>^ pubKS = System::IO::File::ReadAllBytes(txtPublicKey->Text);
						 Marshal::Copy(pubKS, 0, IntPtr(aucPubKS), pubKS->Length);
					 }
					 catch (...)
					 {
						 lblStatus->ForeColor = Color::FromKnownColor(KnownColor::Red);
						 lblStatus->Text = L"Invalid public key file.";
						 return false;
					 }
				 }

				 return true;
			 }

	private: void DisplayStatus(int returnValue)
			 {
				 if (returnValue == IDTLIB_SUCCESS)
				 {
					 lblStatus->ForeColor = Color::FromKnownColor(KnownColor::Green);
				 }
				 else
				 {
					 lblStatus->ForeColor = Color::FromKnownColor(KnownColor::Red);
				 }

				 int count = lstLog->Items->Count;
				 if (count > 0)
				 {
					 String^ lastLine = lstLog->Items[count - 1]->ToString();
					 String^ moduleName = gcnew String(MODULE_NAME);
					 lastLine = lastLine->Substring(lastLine->IndexOf(moduleName) + moduleName->Length);
					 lastLine = lastLine->Substring(lastLine->IndexOf(":") + 2);
					 lblStatus->Text = lastLine;
				 }
			 }

	private: void Burn(unsigned char cartridgeNum)
			 {
				 lblStatus->ForeColor = SystemColors::ControlText;
				 lblStatus->Text = "Preparing tag...";
				 Refresh();

				 unsigned char aucMaterialInfo[100];
				 unsigned short usMaterialInfoLength;
				 bool ok = ReadMaterialInfo(aucMaterialInfo, &usMaterialInfoLength);
				 if (!ok)
				 {
					 return;
				 }

				 unsigned char aucPrvKS[PRIVATE_KEY_SIZE], aucPubKS[PUBLIC_KEY_SIZE];
				 ok = ReadKeys(aucPrvKS, aucPubKS);
				 if (!ok)
				 {
					 return;
				 }

				 int returnValue = m_materialMonitor->BurnIDC(cartridgeNum, aucMaterialInfo, usMaterialInfoLength, aucPrvKS, aucPubKS);
				 DisplayStatus(returnValue);
			 }

	private: void Read(unsigned char cartridgeNum)
			 {
				 lblStatus->ForeColor = SystemColors::ControlText;
				 lblStatus->Text = "Reading tag...";
				 Refresh();

				 unsigned char aucMaterialInfo[100];
				 unsigned short usMaterialInfoLength;
				 unsigned int currentVolume;

				 unsigned char aucPubKS[PUBLIC_KEY_SIZE];
				 bool ok = ReadKeys(NULL, aucPubKS);
				 if (!ok)
				 {
					 return;
				 }

				 int returnValue = m_materialMonitor->AuthenticateCartridge(cartridgeNum, aucPubKS, aucMaterialInfo, &usMaterialInfoLength, &currentVolume);
				 if (returnValue == IDTLIB_SUCCESS)
				 {
					 MaterialInformation materialInfo;
					 materialInfo.Decode(aucMaterialInfo);
					 txtTagStructID->Text = materialInfo.TagStructID.ToString();
					 txtMaterialType->Text = materialInfo.MaterialType.ToString();
					 dtManufacturingDate->Value = DateTime(1970, 1, 1).Add(TimeSpan::FromSeconds(materialInfo.MfgDateTime));
					 txtInitialWeight->Text = materialInfo.InitialWeight.ToString();
					 txtBatchNo->Text = gcnew String(materialInfo.BatchNo, 0, BATCH_NO_SIZE);
					 dtExpirationDate->Value = DateTime(1970, 1, 1).Add(TimeSpan::FromSeconds(materialInfo.ExpirationDate));
				 }

				 DisplayStatus(returnValue);
			 }

	private: void Modify(unsigned char cartridgeNum)
			 {
				 lblStatus->ForeColor = SystemColors::ControlText;
				 lblStatus->Text = "Modifying tag...";
				 Refresh();

				 unsigned char aucMaterialInfo[100];
				 unsigned short usMaterialInfoLength;
				 bool ok = ReadMaterialInfo(aucMaterialInfo, &usMaterialInfoLength);
				 if (!ok)
				 {
					 return;
				 }

				 unsigned char aucPrvKS[PRIVATE_KEY_SIZE], aucPubKS[PUBLIC_KEY_SIZE];
				 ok = ReadKeys(aucPrvKS, aucPubKS);
				 if (!ok)
				 {
					 return;
				 }

				 int returnValue = m_materialMonitor->ModifyIDC(cartridgeNum, aucMaterialInfo, usMaterialInfoLength, aucPrvKS, aucPubKS);
				 DisplayStatus(returnValue);
			 }

	private: void Erase(unsigned char cartridgeNum)
			 {
				 lblStatus->ForeColor = SystemColors::ControlText;
				 lblStatus->Text = "Erasing tag...";
				 Refresh();

				 int returnValue = m_materialMonitor->EraseCartridge(cartridgeNum);
				 DisplayStatus(returnValue);
			 }

	private: System::Void btnRefresh_Click(System::Object^  sender, System::EventArgs^  e) {
				 GetInPlaceStatus();
			 }
	};

	void LogMessage(const char* message)
	{
		if (Application::OpenForms->Count > 0)
		{
			BurnerAppForm^ f = static_cast<BurnerAppForm^>(Application::OpenForms[0]);
			f->WriteMessage(message);
		}
	}
}

