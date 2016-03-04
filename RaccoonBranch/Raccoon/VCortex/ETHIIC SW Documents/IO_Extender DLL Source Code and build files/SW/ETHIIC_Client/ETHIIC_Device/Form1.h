#pragma once

namespace ETHIIC_Device {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Net;
	using namespace System::Net::Sockets;
	using namespace System::Text;
	using namespace System::Runtime::InteropServices;
	using namespace System::Threading;

#pragma pack(push, 1)
	class IO_Ext_Msg {
	public:
		char msgType;
		short buffLen;
		char buff[1024];
	} ;
#pragma pack(pop)

	enum class IO_Ext_Msgtype : char {
		INIT = 0,
	} ;

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
//			receiveThread = gcnew Thread(gcnew ThreadStart(this, &ETHIIC_Device::Form1::receiveThreadProc));

			// Start the thread
//			receiveThread->Start();
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
	private: System::Windows::Forms::Label^  label1;
	Socket^ sending_socket ;
	UdpClient^ listener;
	Thread^ receiveThread;

	protected: 
	private: System::Windows::Forms::TextBox^  deviceNameTextBox;
	private: System::Windows::Forms::Label^  logLabel;

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
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(Form1::typeid));
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->deviceNameTextBox = (gcnew System::Windows::Forms::TextBox());
			this->logLabel = (gcnew System::Windows::Forms::Label());
			this->SuspendLayout();
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(13, 76);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(65, 13);
			this->label1->TabIndex = 0;
			this->label1->Text = L"My Name is:";
			// 
			// deviceNameTextBox
			// 
			this->deviceNameTextBox->Location = System::Drawing::Point(85, 76);
			this->deviceNameTextBox->Name = L"deviceNameTextBox";
			this->deviceNameTextBox->Size = System::Drawing::Size(124, 20);
			this->deviceNameTextBox->TabIndex = 1;
			this->deviceNameTextBox->Text = L"Dummy";
			// 
			// logLabel
			// 
			this->logLabel->AutoSize = true;
			this->logLabel->Location = System::Drawing::Point(16, 154);
			this->logLabel->Name = L"logLabel";
			this->logLabel->Size = System::Drawing::Size(0, 13);
			this->logLabel->TabIndex = 2;
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(284, 261);
			this->Controls->Add(this->logLabel);
			this->Controls->Add(this->deviceNameTextBox);
			this->Controls->Add(this->label1);
			this->Icon = (cli::safe_cast<System::Drawing::Icon^  >(resources->GetObject(L"$this.Icon")));
			this->Name = L"Form1";
			this->Text = L"ETHIIC device";
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	delegate void setTextDelegate( String^ txt);
	private: System::Void setLogTxt(String^ text){
				 if (this->InvokeRequired) {
					 setTextDelegate^ d = gcnew setTextDelegate(this, &Form1::setLogTxt);
					 this->logLabel->BeginInvoke(d, gcnew array<Object^>/*[]*/ { text }); // 
				 } else
					 this->logLabel->Text = text;
			 }

		System::Void receiveThreadProc(){
			IPEndPoint^ listeningAddresss = gcnew IPEndPoint(IPAddress::Any, IN_PORT);
			IPEndPoint^ sendingAddress = gcnew IPEndPoint(IPAddress::Broadcast, OUT_PORT);

			setLogTxt("Waiting for message...");
			this->sending_socket = gcnew Socket(AddressFamily::InterNetwork, SocketType::Dgram, ProtocolType::Udp);
			this->listener = gcnew UdpClient(IN_PORT);

			while (true){
				// Wait for message with 50 mSec interval
				while (listener->Available == 0){
					Thread::Sleep(50);
				};

				array<Byte>^ receivedBytes = listener->Receive(listeningAddresss);

				IO_Ext_Msg ioExtOutMsg;
				Marshal::Copy(receivedBytes, 0, (IntPtr)&ioExtOutMsg, receivedBytes->Length);

				if (ioExtOutMsg.msgType == (char)IO_Ext_Msgtype::INIT){
					ioExtOutMsg.buff[ioExtOutMsg.buffLen] = 0;

					String^ deviceName = gcnew String(ioExtOutMsg.buff);

					// If I was addressed
					if (deviceName == this->deviceNameTextBox->Text){
						setLogTxt("Init was received");

						ioExtOutMsg.buffLen = 0;
						int msgLen = 3;

						// reply with same message, w/o device name
						array<Byte>^ sendBytes = gcnew array<Byte>(msgLen);
						for (int i=0; i<msgLen ; i++)
							sendBytes[i] = Marshal::ReadByte((IntPtr)&ioExtOutMsg, i);

						sendingAddress =  gcnew IPEndPoint(listeningAddresss->Address, OUT_PORT);

						String^ txt = listeningAddresss->Address->ToString();
						txt = sendingAddress->Address->ToString();

						try{
							sending_socket->SendTo(sendBytes, sendingAddress);
						}
						catch(Exception^ send_exception){
							String^ txt = listeningAddresss->Address->ToString();
							txt = sendingAddress->Address->ToString();
						}

					}
				} // INIT message

			} // While true
			
		}
	};
}

