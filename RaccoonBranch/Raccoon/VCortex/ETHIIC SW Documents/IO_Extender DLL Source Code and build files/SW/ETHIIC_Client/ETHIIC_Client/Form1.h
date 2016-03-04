#pragma once

#include "../IO_Extender/IO_Extender.h"
#include <msclr\marshal_cppstd.h>
#include <string>
namespace ETHIIC_Client {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace VCortex_IO_Extender;
	using namespace System::Runtime::InteropServices;

	using namespace VCortex_IO_Extender;
	using namespace std;

	/// <summary>
	/// Summary for Form1
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{
	public:
		Form1(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Form1()
		{
			if (components)
			{
				delete components;
			}
		}

//	private: IO_Extender_dll ioExtender;
	private: IOX_Handle myDeviceHandle;
	private: System::Windows::Forms::Button^  initButton;
	protected: 
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::TextBox^  logicalNameTextBox;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::ListBox^  IICSpeedListBox;
	private: System::Windows::Forms::Panel^  panel1;
	private: System::Windows::Forms::TextBox^  sendTextBox;

	private: System::Windows::Forms::Panel^  panel2;
	private: System::Windows::Forms::Button^  sendButton;
	private: System::Windows::Forms::TextBox^  slaveAddressTextBox;

	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Panel^  panel3;
	private: System::Windows::Forms::TextBox^  receivedTextBox;
	private: System::Windows::Forms::TextBox^  slaveAddressTextBox2;
	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::Label^  label6;
	private: System::Windows::Forms::Button^  sendByteReceiveByteButton;
	private: System::Windows::Forms::TextBox^  sendTextBox2;
	private: System::Windows::Forms::Label^  label8;
	private: System::Windows::Forms::Label^  label7;
	private: System::Windows::Forms::Panel^  panel4;
	private: System::Windows::Forms::Label^  label11;
	private: System::Windows::Forms::TextBox^  slaveAddressTextBox3;

	private: System::Windows::Forms::TextBox^  receiveTextBox;
	private: System::Windows::Forms::Label^  label9;
	private: System::Windows::Forms::Label^  label10;
	private: System::Windows::Forms::Button^  receiveButton;
	private: System::Windows::Forms::TextBox^  numBytesToReadTextBox;
	private: System::Windows::Forms::Label^  label12;
	private: System::Windows::Forms::TextBox^  numBytesToSendTextBox;
	private: System::Windows::Forms::Button^  wakeupIICButton;
	private: System::Windows::Forms::Button^  testButton;
	private: System::Windows::Forms::Button^  epromTestButton;
	private: System::Windows::Forms::TextBox^  sentTextBox;
	private: System::Windows::Forms::TextBox^  receivedTtextBox;
	private: System::Windows::Forms::Label^  label13;
	private: System::Windows::Forms::Label^  label14;
	private: System::Windows::Forms::Label^  label15;
	private: System::Windows::Forms::TextBox^  numCommandsTextBox;
	private: System::Windows::Forms::Label^  label16;
	private: System::Windows::Forms::TextBox^  numretriesTextBox;








	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->initButton = (gcnew System::Windows::Forms::Button());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->logicalNameTextBox = (gcnew System::Windows::Forms::TextBox());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->IICSpeedListBox = (gcnew System::Windows::Forms::ListBox());
			this->panel1 = (gcnew System::Windows::Forms::Panel());
			this->sendTextBox = (gcnew System::Windows::Forms::TextBox());
			this->panel2 = (gcnew System::Windows::Forms::Panel());
			this->label12 = (gcnew System::Windows::Forms::Label());
			this->numBytesToSendTextBox = (gcnew System::Windows::Forms::TextBox());
			this->slaveAddressTextBox = (gcnew System::Windows::Forms::TextBox());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->sendButton = (gcnew System::Windows::Forms::Button());
			this->panel3 = (gcnew System::Windows::Forms::Panel());
			this->label8 = (gcnew System::Windows::Forms::Label());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->receivedTextBox = (gcnew System::Windows::Forms::TextBox());
			this->slaveAddressTextBox2 = (gcnew System::Windows::Forms::TextBox());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->sendByteReceiveByteButton = (gcnew System::Windows::Forms::Button());
			this->sendTextBox2 = (gcnew System::Windows::Forms::TextBox());
			this->panel4 = (gcnew System::Windows::Forms::Panel());
			this->label11 = (gcnew System::Windows::Forms::Label());
			this->slaveAddressTextBox3 = (gcnew System::Windows::Forms::TextBox());
			this->receiveTextBox = (gcnew System::Windows::Forms::TextBox());
			this->label9 = (gcnew System::Windows::Forms::Label());
			this->label10 = (gcnew System::Windows::Forms::Label());
			this->receiveButton = (gcnew System::Windows::Forms::Button());
			this->numBytesToReadTextBox = (gcnew System::Windows::Forms::TextBox());
			this->wakeupIICButton = (gcnew System::Windows::Forms::Button());
			this->testButton = (gcnew System::Windows::Forms::Button());
			this->epromTestButton = (gcnew System::Windows::Forms::Button());
			this->sentTextBox = (gcnew System::Windows::Forms::TextBox());
			this->receivedTtextBox = (gcnew System::Windows::Forms::TextBox());
			this->label13 = (gcnew System::Windows::Forms::Label());
			this->label14 = (gcnew System::Windows::Forms::Label());
			this->label15 = (gcnew System::Windows::Forms::Label());
			this->numCommandsTextBox = (gcnew System::Windows::Forms::TextBox());
			this->label16 = (gcnew System::Windows::Forms::Label());
			this->numretriesTextBox = (gcnew System::Windows::Forms::TextBox());
			this->panel1->SuspendLayout();
			this->panel2->SuspendLayout();
			this->panel3->SuspendLayout();
			this->panel4->SuspendLayout();
			this->SuspendLayout();
			// 
			// initButton
			// 
			this->initButton->Location = System::Drawing::Point(269, 47);
			this->initButton->Name = L"initButton";
			this->initButton->Size = System::Drawing::Size(75, 23);
			this->initButton->TabIndex = 0;
			this->initButton->Text = L"Init";
			this->initButton->UseVisualStyleBackColor = true;
			this->initButton->Click += gcnew System::EventHandler(this, &Form1::initButton_Click);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(11, 18);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(75, 13);
			this->label1->TabIndex = 1;
			this->label1->Text = L"Logical Name:";
			// 
			// logicalNameTextBox
			// 
			this->logicalNameTextBox->Location = System::Drawing::Point(93, 15);
			this->logicalNameTextBox->Name = L"logicalNameTextBox";
			this->logicalNameTextBox->Size = System::Drawing::Size(130, 20);
			this->logicalNameTextBox->TabIndex = 2;
			this->logicalNameTextBox->Text = L"ETHIIC-1";
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(11, 52);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(54, 13);
			this->label3->TabIndex = 5;
			this->label3->Text = L"IIC Speed";
			// 
			// IICSpeedListBox
			// 
			this->IICSpeedListBox->FormattingEnabled = true;
			this->IICSpeedListBox->Items->AddRange(gcnew cli::array< System::Object^  >(4) {L"100k Baud", L"400k Baud", L"1M Baud", L"3.4M Baud"});
			this->IICSpeedListBox->Location = System::Drawing::Point(91, 43);
			this->IICSpeedListBox->Name = L"IICSpeedListBox";
			this->IICSpeedListBox->Size = System::Drawing::Size(120, 56);
			this->IICSpeedListBox->TabIndex = 0;
			// 
			// panel1
			// 
			this->panel1->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->panel1->Controls->Add(this->IICSpeedListBox);
			this->panel1->Location = System::Drawing::Point(4, 4);
			this->panel1->Name = L"panel1";
			this->panel1->Size = System::Drawing::Size(354, 132);
			this->panel1->TabIndex = 6;
			// 
			// sendTextBox
			// 
			this->sendTextBox->Location = System::Drawing::Point(97, 64);
			this->sendTextBox->Name = L"sendTextBox";
			this->sendTextBox->Size = System::Drawing::Size(137, 20);
			this->sendTextBox->TabIndex = 7;
			// 
			// panel2
			// 
			this->panel2->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->panel2->Controls->Add(this->label12);
			this->panel2->Controls->Add(this->numBytesToSendTextBox);
			this->panel2->Controls->Add(this->slaveAddressTextBox);
			this->panel2->Controls->Add(this->label4);
			this->panel2->Controls->Add(this->label2);
			this->panel2->Controls->Add(this->sendButton);
			this->panel2->Controls->Add(this->sendTextBox);
			this->panel2->Location = System::Drawing::Point(4, 276);
			this->panel2->Name = L"panel2";
			this->panel2->Size = System::Drawing::Size(354, 97);
			this->panel2->TabIndex = 8;
			// 
			// label12
			// 
			this->label12->AutoSize = true;
			this->label12->Location = System::Drawing::Point(14, 39);
			this->label12->Name = L"label12";
			this->label12->Size = System::Drawing::Size(61, 13);
			this->label12->TabIndex = 13;
			this->label12->Text = L"Num Bytes:";
			// 
			// numBytesToSendTextBox
			// 
			this->numBytesToSendTextBox->Location = System::Drawing::Point(97, 39);
			this->numBytesToSendTextBox->Name = L"numBytesToSendTextBox";
			this->numBytesToSendTextBox->Size = System::Drawing::Size(31, 20);
			this->numBytesToSendTextBox->TabIndex = 12;
			// 
			// slaveAddressTextBox
			// 
			this->slaveAddressTextBox->Location = System::Drawing::Point(96, 15);
			this->slaveAddressTextBox->Name = L"slaveAddressTextBox";
			this->slaveAddressTextBox->Size = System::Drawing::Size(32, 20);
			this->slaveAddressTextBox->TabIndex = 11;
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(14, 10);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(75, 13);
			this->label4->TabIndex = 10;
			this->label4->Text = L"Slave Address";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(14, 64);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(33, 13);
			this->label2->TabIndex = 9;
			this->label2->Text = L"Data:";
			// 
			// sendButton
			// 
			this->sendButton->Location = System::Drawing::Point(231, 10);
			this->sendButton->Name = L"sendButton";
			this->sendButton->Size = System::Drawing::Size(75, 23);
			this->sendButton->TabIndex = 8;
			this->sendButton->Text = L"Send";
			this->sendButton->UseVisualStyleBackColor = true;
			this->sendButton->Click += gcnew System::EventHandler(this, &Form1::sendButton_Click);
			// 
			// panel3
			// 
			this->panel3->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->panel3->Controls->Add(this->label8);
			this->panel3->Controls->Add(this->label7);
			this->panel3->Controls->Add(this->receivedTextBox);
			this->panel3->Controls->Add(this->slaveAddressTextBox2);
			this->panel3->Controls->Add(this->label5);
			this->panel3->Controls->Add(this->label6);
			this->panel3->Controls->Add(this->sendByteReceiveByteButton);
			this->panel3->Controls->Add(this->sendTextBox2);
			this->panel3->Location = System::Drawing::Point(4, 171);
			this->panel3->Name = L"panel3";
			this->panel3->Size = System::Drawing::Size(354, 74);
			this->panel3->TabIndex = 9;
			// 
			// label8
			// 
			this->label8->AutoSize = true;
			this->label8->Location = System::Drawing::Point(164, 43);
			this->label8->Name = L"label8";
			this->label8->Size = System::Drawing::Size(18, 13);
			this->label8->TabIndex = 13;
			this->label8->Text = L"in:";
			// 
			// label7
			// 
			this->label7->AutoSize = true;
			this->label7->Location = System::Drawing::Point(85, 42);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(25, 13);
			this->label7->TabIndex = 12;
			this->label7->Text = L"out:";
			// 
			// receivedTextBox
			// 
			this->receivedTextBox->Location = System::Drawing::Point(188, 41);
			this->receivedTextBox->Name = L"receivedTextBox";
			this->receivedTextBox->ReadOnly = true;
			this->receivedTextBox->Size = System::Drawing::Size(37, 20);
			this->receivedTextBox->TabIndex = 10;
			// 
			// slaveAddressTextBox2
			// 
			this->slaveAddressTextBox2->Location = System::Drawing::Point(96, 15);
			this->slaveAddressTextBox2->Name = L"slaveAddressTextBox2";
			this->slaveAddressTextBox2->Size = System::Drawing::Size(32, 20);
			this->slaveAddressTextBox2->TabIndex = 11;
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(14, 15);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(75, 13);
			this->label5->TabIndex = 10;
			this->label5->Text = L"Slave Address";
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Location = System::Drawing::Point(14, 41);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(33, 13);
			this->label6->TabIndex = 9;
			this->label6->Text = L"Data:";
			// 
			// sendByteReceiveByteButton
			// 
			this->sendByteReceiveByteButton->Location = System::Drawing::Point(231, 10);
			this->sendByteReceiveByteButton->Name = L"sendByteReceiveByteButton";
			this->sendByteReceiveByteButton->Size = System::Drawing::Size(86, 51);
			this->sendByteReceiveByteButton->TabIndex = 8;
			this->sendByteReceiveByteButton->Text = L"Send Byte Receive Byte";
			this->sendByteReceiveByteButton->UseVisualStyleBackColor = true;
			this->sendByteReceiveByteButton->Click += gcnew System::EventHandler(this, &Form1::sendByteReceiveByteButton_Click);
			// 
			// sendTextBox2
			// 
			this->sendTextBox2->Location = System::Drawing::Point(110, 41);
			this->sendTextBox2->Name = L"sendTextBox2";
			this->sendTextBox2->Size = System::Drawing::Size(37, 20);
			this->sendTextBox2->TabIndex = 7;
			// 
			// panel4
			// 
			this->panel4->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->panel4->Controls->Add(this->label11);
			this->panel4->Controls->Add(this->slaveAddressTextBox3);
			this->panel4->Controls->Add(this->receiveTextBox);
			this->panel4->Controls->Add(this->label9);
			this->panel4->Controls->Add(this->label10);
			this->panel4->Controls->Add(this->receiveButton);
			this->panel4->Controls->Add(this->numBytesToReadTextBox);
			this->panel4->Location = System::Drawing::Point(4, 413);
			this->panel4->Name = L"panel4";
			this->panel4->Size = System::Drawing::Size(354, 74);
			this->panel4->TabIndex = 10;
			// 
			// label11
			// 
			this->label11->AutoSize = true;
			this->label11->Location = System::Drawing::Point(148, 41);
			this->label11->Name = L"label11";
			this->label11->Size = System::Drawing::Size(33, 13);
			this->label11->TabIndex = 12;
			this->label11->Text = L"Data:";
			// 
			// slaveAddressTextBox3
			// 
			this->slaveAddressTextBox3->Location = System::Drawing::Point(96, 15);
			this->slaveAddressTextBox3->Name = L"slaveAddressTextBox3";
			this->slaveAddressTextBox3->Size = System::Drawing::Size(32, 20);
			this->slaveAddressTextBox3->TabIndex = 11;
			// 
			// receiveTextBox
			// 
			this->receiveTextBox->Location = System::Drawing::Point(188, 38);
			this->receiveTextBox->Name = L"receiveTextBox";
			this->receiveTextBox->ReadOnly = true;
			this->receiveTextBox->Size = System::Drawing::Size(137, 20);
			this->receiveTextBox->TabIndex = 11;
			// 
			// label9
			// 
			this->label9->AutoSize = true;
			this->label9->Location = System::Drawing::Point(14, 15);
			this->label9->Name = L"label9";
			this->label9->Size = System::Drawing::Size(75, 13);
			this->label9->TabIndex = 10;
			this->label9->Text = L"Slave Address";
			// 
			// label10
			// 
			this->label10->AutoSize = true;
			this->label10->Location = System::Drawing::Point(14, 41);
			this->label10->Name = L"label10";
			this->label10->Size = System::Drawing::Size(61, 13);
			this->label10->TabIndex = 9;
			this->label10->Text = L"Num Bytes:";
			// 
			// receiveButton
			// 
			this->receiveButton->Location = System::Drawing::Point(231, 10);
			this->receiveButton->Name = L"receiveButton";
			this->receiveButton->Size = System::Drawing::Size(75, 23);
			this->receiveButton->TabIndex = 8;
			this->receiveButton->Text = L"Receive";
			this->receiveButton->UseVisualStyleBackColor = true;
			this->receiveButton->Click += gcnew System::EventHandler(this, &Form1::receiveButton_Click);
			// 
			// numBytesToReadTextBox
			// 
			this->numBytesToReadTextBox->Location = System::Drawing::Point(97, 41);
			this->numBytesToReadTextBox->Name = L"numBytesToReadTextBox";
			this->numBytesToReadTextBox->Size = System::Drawing::Size(31, 20);
			this->numBytesToReadTextBox->TabIndex = 7;
			// 
			// wakeupIICButton
			// 
			this->wakeupIICButton->Location = System::Drawing::Point(373, 13);
			this->wakeupIICButton->Name = L"wakeupIICButton";
			this->wakeupIICButton->Size = System::Drawing::Size(75, 52);
			this->wakeupIICButton->TabIndex = 11;
			this->wakeupIICButton->Text = L"Wakeup IIC";
			this->wakeupIICButton->UseVisualStyleBackColor = true;
			this->wakeupIICButton->Click += gcnew System::EventHandler(this, &Form1::wakepIICButton_Click);
			// 
			// testButton
			// 
			this->testButton->Location = System::Drawing::Point(373, 112);
			this->testButton->Name = L"testButton";
			this->testButton->Size = System::Drawing::Size(75, 68);
			this->testButton->TabIndex = 12;
			this->testButton->Text = L"Test";
			this->testButton->UseVisualStyleBackColor = true;
			this->testButton->Click += gcnew System::EventHandler(this, &Form1::testButton_Click);
			// 
			// epromTestButton
			// 
			this->epromTestButton->Location = System::Drawing::Point(373, 228);
			this->epromTestButton->Name = L"epromTestButton";
			this->epromTestButton->Size = System::Drawing::Size(75, 68);
			this->epromTestButton->TabIndex = 13;
			this->epromTestButton->Text = L"Eprom Test";
			this->epromTestButton->UseVisualStyleBackColor = true;
			this->epromTestButton->Click += gcnew System::EventHandler(this, &Form1::epromTestButton_Click);
			// 
			// sentTextBox
			// 
			this->sentTextBox->Location = System::Drawing::Point(373, 316);
			this->sentTextBox->Name = L"sentTextBox";
			this->sentTextBox->Size = System::Drawing::Size(100, 20);
			this->sentTextBox->TabIndex = 14;
			// 
			// receivedTtextBox
			// 
			this->receivedTtextBox->Location = System::Drawing::Point(373, 352);
			this->receivedTtextBox->Name = L"receivedTtextBox";
			this->receivedTtextBox->Size = System::Drawing::Size(100, 20);
			this->receivedTtextBox->TabIndex = 15;
			// 
			// label13
			// 
			this->label13->AutoSize = true;
			this->label13->Location = System::Drawing::Point(373, 299);
			this->label13->Name = L"label13";
			this->label13->Size = System::Drawing::Size(29, 13);
			this->label13->TabIndex = 16;
			this->label13->Text = L"Sent";
			// 
			// label14
			// 
			this->label14->AutoSize = true;
			this->label14->Location = System::Drawing::Point(376, 341);
			this->label14->Name = L"label14";
			this->label14->Size = System::Drawing::Size(53, 13);
			this->label14->TabIndex = 17;
			this->label14->Text = L"Received";
			// 
			// label15
			// 
			this->label15->AutoSize = true;
			this->label15->Location = System::Drawing::Point(373, 392);
			this->label15->Name = L"label15";
			this->label15->Size = System::Drawing::Size(84, 13);
			this->label15->TabIndex = 18;
			this->label15->Text = L"Num Commands";
			// 
			// numCommandsTextBox
			// 
			this->numCommandsTextBox->Location = System::Drawing::Point(373, 413);
			this->numCommandsTextBox->Name = L"numCommandsTextBox";
			this->numCommandsTextBox->Size = System::Drawing::Size(56, 20);
			this->numCommandsTextBox->TabIndex = 19;
			this->numCommandsTextBox->Text = L"0";
			// 
			// label16
			// 
			this->label16->AutoSize = true;
			this->label16->Location = System::Drawing::Point(373, 455);
			this->label16->Name = L"label16";
			this->label16->Size = System::Drawing::Size(65, 13);
			this->label16->TabIndex = 20;
			this->label16->Text = L"Num Retries";
			// 
			// numretriesTextBox
			// 
			this->numretriesTextBox->Location = System::Drawing::Point(373, 472);
			this->numretriesTextBox->Name = L"numretriesTextBox";
			this->numretriesTextBox->Size = System::Drawing::Size(56, 20);
			this->numretriesTextBox->TabIndex = 21;
			this->numretriesTextBox->Text = L"0";
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(490, 529);
			this->Controls->Add(this->numretriesTextBox);
			this->Controls->Add(this->label16);
			this->Controls->Add(this->numCommandsTextBox);
			this->Controls->Add(this->label15);
			this->Controls->Add(this->label14);
			this->Controls->Add(this->label13);
			this->Controls->Add(this->receivedTtextBox);
			this->Controls->Add(this->sentTextBox);
			this->Controls->Add(this->epromTestButton);
			this->Controls->Add(this->testButton);
			this->Controls->Add(this->wakeupIICButton);
			this->Controls->Add(this->panel4);
			this->Controls->Add(this->panel3);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->logicalNameTextBox);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->initButton);
			this->Controls->Add(this->panel1);
			this->Controls->Add(this->panel2);
			this->Name = L"Form1";
			this->Text = L"ETHIIC Client";
			this->Load += gcnew System::EventHandler(this, &Form1::Form1_Load);
			this->panel1->ResumeLayout(false);
			this->panel2->ResumeLayout(false);
			this->panel2->PerformLayout();
			this->panel3->ResumeLayout(false);
			this->panel3->PerformLayout();
			this->panel4->ResumeLayout(false);
			this->panel4->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion


	private: System::Void initButton_Click(System::Object^  sender, System::EventArgs^  e) {

				 if (this->logicalNameTextBox->Text == "")				 
					MessageBox::Show("Please indicate device logical name",
					 "ETHIIC Client", MessageBoxButtons::OK,
					 MessageBoxIcon::Error);
				 else if (this->IICSpeedListBox->SelectedIndex <0)
					 MessageBox::Show("Please select IIC speed",
					 "ETHIIC Client", MessageBoxButtons::OK,
					 MessageBoxIcon::Error);
				 else{
					 // send UDP message in 
					 msclr::interop::marshal_context context;
					 std::string standardString = context.marshal_as<std::string>(this->logicalNameTextBox->Text);
					 IIC_Speeds IICspeed = (IIC_Speeds)IICSpeedListBox->SelectedIndex;
					 IOX_Handle stam;
					 IO_Ext_Err io_extErr = IOX_initDevice(standardString, &stam);
					 myDeviceHandle = stam;
					 if (io_extErr != IO_S_OK)
						 MessageBox::Show("Init failure",
						 "ETHIIC Client", MessageBoxButtons::OK,
						 MessageBoxIcon::Error);
					 else if (io_extErr == E_DEVICE_NOT_FOUND)
						 MessageBox::Show("Device not found",
						 "ETHIIC Client", MessageBoxButtons::OK,
						 MessageBoxIcon::Error);
					 else
						 io_extErr = IOX_initIIC(stam, IICspeed);

					 int numCommands, numRetries;
					 IOX_getCommStat(myDeviceHandle, &numCommands, &numRetries);
					 this->numCommandsTextBox->Text = numCommands.ToString();
					 this->numretriesTextBox->Text = numRetries.ToString();

				 }
			 }
	private: System::Void Form1_Load(System::Object^  sender, System::EventArgs^  e) {
				 IOX_setLog(TRACE_LOG_LEVEL, "IOX_Log.log");
			 }
private: System::Void sendButton_Click(System::Object^  sender, System::EventArgs^  e) {
			 int slaveAddress = int::Parse(this->slaveAddressTextBox->Text);
			 int buffLen = int::Parse(this->numBytesToSendTextBox->Text);
			 array<Byte>^ sendBytes = gcnew array<Byte>(buffLen);
			 IntPtr p = Marshal::StringToHGlobalAnsi(this->sendTextBox->Text);
			 char *pNewCharStr = static_cast<char*>(p.ToPointer());

//			 char dummySend[4] = {0x46, 0, 0, 0x46};
			

			 IO_Ext_Err io_extErr = IOX_writeBytes(myDeviceHandle, slaveAddress, (BYTE*)pNewCharStr, buffLen, true);

			 if (io_extErr == E_SLAVE_NOT_READY)
				 MessageBox::Show("Slave not ready",
				 "ETHIIC Client", MessageBoxButtons::OK,
				 MessageBoxIcon::Error);
			 else if (io_extErr == E_READ_OP_FAILED)
				 MessageBox::Show("Slave read failed",
				 "ETHIIC Client", MessageBoxButtons::OK,
				 MessageBoxIcon::Error);
			 else if (io_extErr == E_WRITE_OP_FAILED)
				 MessageBox::Show("Slave write failed",
				 "ETHIIC Client", MessageBoxButtons::OK,
				 MessageBoxIcon::Error);

			 int numCommands, numRetries;
			 IOX_getCommStat(myDeviceHandle, &numCommands, &numRetries);
			 this->numCommandsTextBox->Text = numCommands.ToString();
			 this->numretriesTextBox->Text = numRetries.ToString();

		 }
private: System::Void sendByteReceiveByteButton_Click(System::Object^  sender, System::EventArgs^  e) {
			 int slaveAddress = int::Parse(this->slaveAddressTextBox2->Text);

			 Byte byteToSend = int::Parse(this->sendTextBox2->Text);
			 Byte receivedByte;
			 IO_Ext_Err io_extErr = IOX_writeAndReadByte(myDeviceHandle, slaveAddress, byteToSend, receivedByte);

			 if (io_extErr == E_SLAVE_NOT_READY)
				 MessageBox::Show("Slave not ready",
				 "ETHIIC Client", MessageBoxButtons::OK,
				 MessageBoxIcon::Error);
			 else if (io_extErr == E_READ_OP_FAILED)
				 MessageBox::Show("Slave read failed",
				 "ETHIIC Client", MessageBoxButtons::OK,
				 MessageBoxIcon::Error);
			 else if (io_extErr == E_WRITE_OP_FAILED)
				 MessageBox::Show("Slave write failed",
				 "ETHIIC Client", MessageBoxButtons::OK,
				 MessageBoxIcon::Error);
			 else
				this->receivedTextBox->Text = receivedByte.ToString();

			 int numCommands, numRetries;
			 IOX_getCommStat(myDeviceHandle, &numCommands, &numRetries);
			 this->numCommandsTextBox->Text = numCommands.ToString();
			 this->numretriesTextBox->Text = numRetries.ToString();

		 }
private: System::Void receiveButton_Click(System::Object^  sender, System::EventArgs^  e) {
			 int slaveAddress = int::Parse(this->slaveAddressTextBox3->Text);
			 int numBytesToRead = int::Parse(this->numBytesToReadTextBox->Text);

// 			 if (numBytesToRead>1024)
// 				 numBytesToRead = 1024;

			 Byte receivedBytes[1024];

 			 while (true)
 			 {

			 
			 IO_Ext_Err io_extErr = IOX_readBytes(myDeviceHandle, slaveAddress, receivedBytes, numBytesToRead);

			 if (io_extErr != E_SLAVE_NOT_READY)
				 MessageBox::Show("Slave not ready",
				 "ETHIIC Client", MessageBoxButtons::OK,
				 MessageBoxIcon::Error);
			 else if (io_extErr == E_READ_OP_FAILED)
				 MessageBox::Show("Slave read failed",
				 "ETHIIC Client", MessageBoxButtons::OK,
				 MessageBoxIcon::Error);
			 else if (io_extErr == E_WRITE_OP_FAILED)
				 MessageBox::Show("Slave write failed",
				 "ETHIIC Client", MessageBoxButtons::OK,
				 MessageBoxIcon::Error);
			 else
			 {
				 // Consider it a string, and display it
				 receivedBytes[numBytesToRead] = 0;

				 this->receiveTextBox->Text = gcnew String((char*)receivedBytes);
			}
			int numCommands, numRetries;
			IOX_getCommStat(myDeviceHandle, &numCommands, &numRetries);
			this->numCommandsTextBox->Text = numCommands.ToString();
			this->numretriesTextBox->Text = numRetries.ToString();
			}
	}
private: System::Void wakepIICButton_Click(System::Object^  sender, System::EventArgs^  e) 
		 {
			 IO_Ext_Err io_extErr = IOX_wakeupIIC(myDeviceHandle);

			 int numCommands, numRetries;
			 IOX_getCommStat(myDeviceHandle, &numCommands, &numRetries);
			 this->numCommandsTextBox->Text = numCommands.ToString();
			 this->numretriesTextBox->Text = numRetries.ToString();

			 if (io_extErr == IO_S_OK)
				 MessageBox::Show("Wakeup sequence done",
				 "ETHIIC Client", MessageBoxButtons::OK,
				 MessageBoxIcon::Information);


		 }

private: System::Void testButton_Click(System::Object^  sender, System::EventArgs^  e) {
			 int good=0, bad=0;

			 for (int i=0; i<100; i++)
			 {
				 IO_Ext_Err io_extErr;

				 // Send 1 bytes to address 112
				 char dummySend1[1] = {1};

				 io_extErr = IOX_writeBytes(myDeviceHandle, 112, /*(BYTE*)pNewCharStr*/(BYTE*)dummySend1, 1, true);

				 io_extErr = IOX_wakeupIIC(myDeviceHandle);

				 Byte receivedBytes[1024];

				 // Send 4 bytes to address 0x5f
				 char dummySend[4] = {0x46, 0, 0, 0x46};

				 io_extErr = IOX_writeBytes(myDeviceHandle, 0x5f, /*(BYTE*)pNewCharStr*/(BYTE*)dummySend, 4, true);

				 // Read 4 bytes from 0x5f
				 io_extErr = IOX_readBytes(myDeviceHandle, 0x5f, receivedBytes, 4);

				 bool isBad = false;
				 for (int ii=0; ii<4; ii++)
				 {
					 if (receivedBytes[ii] != (dummySend[ii]*47/46))
						 isBad = true;
				 }
				if (isBad)
					bad++;
				else
					good++;

				int numCommands, numRetries;
				IOX_getCommStat(myDeviceHandle, &numCommands, &numRetries);
				this->numCommandsTextBox->Text = numCommands.ToString();
				this->numretriesTextBox->Text = numRetries.ToString();

			}

			 Sleep(1);

		 }
private: System::Void epromTestButton_Click(System::Object^  sender, System::EventArgs^  e) {
			 int numErrors = 0;

			 for (int ii=0; ii<10000; ii++)
			 {
			  IO_Ext_Err io_extErr;

			  array<Byte>^ bytes1 = gcnew array<Byte>(100);
			  Random^ rnd1 = gcnew Random();
			  Random^ rnd2 = gcnew Random();
			  int msgLen;
			  msgLen = 4;
//			  rnd1->NextBytes(bytes1);
			  for (int i=0; i<msgLen; i++)
				  bytes1[i] = 41+i;
//			  msgLen = rnd2->Next(10, 64);
			Byte receivedBytes[1024];

			 unsigned char dummySend[102] ;
			 dummySend[0] = 0;
			 dummySend[0] = 0;

			 for (int i=0; i<msgLen; i++)
				 dummySend[i+2] = bytes1[i];

			 io_extErr = IOX_writeBytes(myDeviceHandle, 0x50, /*(BYTE*)pNewCharStr*/(BYTE*)dummySend, msgLen+2, true);
			
			 for (int i=0; i<1024; i++)
				 receivedBytes[i] = -1;
 
			 for (int j=0; j<10; j++)
			 {
				 io_extErr = IOX_writeBytes(myDeviceHandle, 0x50, /*(BYTE*)pNewCharStr*/(BYTE*)dummySend, 2, true); // only address
				 io_extErr = IOX_readBytes(myDeviceHandle, 0x50, receivedBytes, msgLen);
			 }

			 bool err = false;
			 for (int i=0; i<msgLen; i++)
			 {
				 if (receivedBytes[i] != bytes1[i])
					 err = true;
			 }
			 if (err)
			 {
				 numErrors++;
				 this->receiveTextBox->Text = receivedBytes[0].ToString() + " " + receivedBytes[1].ToString() + " " + receivedBytes[2].ToString() + " " + receivedBytes[3].ToString() ; 
				 this->sentTextBox->Text = dummySend[2].ToString() + " " + dummySend[3].ToString() + " " + dummySend[4].ToString() + " " + dummySend[5].ToString() ; 
				 this->Refresh();
				 break;
			 }

			 int numCommands, numRetries;
			 IOX_getCommStat(myDeviceHandle, &numCommands, &numRetries);
			 this->numCommandsTextBox->Text = numCommands.ToString();
			 this->numretriesTextBox->Text = numRetries.ToString();
			}

			 Sleep(10);

		 }
};
}

