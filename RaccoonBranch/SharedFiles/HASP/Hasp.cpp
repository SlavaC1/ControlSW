/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: Q2RT.                                                   *
 * Module: HASP Interface                                           *
 * Module Description: Aladdin HASP application protection.         *
 *                                                                  *
 * Compilation: Standard C++ , BCB                                  *
 *                                                                  *
 * Author: Slava Chuhovich                                          *
 * Start date: 22/02/2010                                           *
 * Last upate:                                                      *
 ********************************************************************/

#pragma hdrstop
#include "Hasp.h"
#pragma package(smart_init)

#include <XMLDoc.hpp>
#include <MSXMLDOM.hpp>
#include <mbstring.h> // for '_mbscmp'
#include <ctime>
#include <utilcls.h> // for TInitOle

EHaspInterfaceException::EHaspInterfaceException(const QString& ErrMsg, const TQErrCode ErrCode) : EQException(ErrMsg, ErrCode){}

CHaspInterfaceWrapper *CHaspInterfaceWrapper::m_SingletonInstance = NULL;

CHaspInterface::CHaspInterface() : CQThread(true,"HASP"), m_Hasp(NULL) {}

CHaspInterface::CHaspInterface(ChaspFeature HaspFeature, 
                               TLogFileTag HaspLogTag, 
							   THandleHaspStatusCallback HandleHaspStatusCallback) 
							   
    : CQThread(true,"HASP")
{
	m_HaspStatus     = hsDisconnected;
	m_HaspPrevStatus = hsConnected;
	m_WriteToLog     = true;
	
	m_Hasp = new Chasp(HaspFeature);	
	
	m_HaspLogTag               = HaspLogTag;		
	m_HandleHaspStatusCallback = HandleHaspStatusCallback;
    m_DaysToExpiration         = -1;
    m_HaspFeatureNum           = HaspFeature;    
	
	// This check is for the developer - log file must be initialized in application before HaspInterface
	if(! CQLog::IsInitialized())
		throw EHaspInterfaceException("Log file not initialized");
}

CHaspInterface::~CHaspInterface()
{	
	Terminate();
	LogOut();	
	Q_SAFE_DELETE(m_Hasp);
}

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

// HaspInterface thread execution
void CHaspInterface::Execute()
{
	while(!Terminated)
	{
		switch (GetConnectionStatus())
		{
            case hsInvalid:
			case hsExpired:
			case hsDisconnected:
			{
				LogIn();
				
				if(m_HandleHaspStatusCallback)		
					(*m_HandleHaspStatusCallback)();
				
				PausePoint(TIME_BETWEEN_EACH_HASP_LOGIN_ATTEMPT);
				break;
			}
			
			case hsConnected:
			{				
				SetDaysToExpiration();
				
				CheckPlugValidity();
				
				if(m_HandleHaspStatusCallback)		
					(*m_HandleHaspStatusCallback)();
				
				PausePoint(TIME_BETWEEN_EACH_HASP_LICENSE_CHECK);
				break;
			}
				
			default:
			{
                WriteToLog("Incorrect HASP connection status", true);
				throw EHaspInterfaceException("Incorrect HASP connection status");				
			}	
		}					
	}
}


// Log into the HL plug
void CHaspInterface::LogIn(bool CheckDirectly)
{
	haspStatus Status;
	
	try
	{
		Status = m_Hasp->login(HASP_OBJET_VENDOR_CODE, HASP_LOCAL_SCOPE);
		WriteToLog(QFormatStr("HASP: login, line: %d, status: %d", __LINE__, Status));
	}
	catch(...)
	{
		WriteToLog("HASP: Exception was thrown during login");
	}
	
	if (HASP_SUCCEEDED(Status))
	{		
		m_HaspPrevStatus = GetConnectionStatus();
		
		SetConnectionStatus(hsConnected);	
		
		try
		{
			SetPlugInfo();
		}
		catch(...)
		{
			WriteToLog("HASP: Error during plug information collection", true);
		}

		if(m_PlugInfo.UptodateDriver)
			WriteToLog(QFormatStr("HASP: Log in successful. Plug ID: %s, Feature: %s, License type: %s", m_PlugInfo.PlugID.c_str(), 
																										 m_PlugInfo.FeatureID.c_str(),
																										 m_PlugInfo.LicenseType.c_str()), true);
		if(CheckDirectly)
			throw hsConnected;
	}
	else
	{
		if (Status == HASP_FEATURE_EXPIRED)
		{
			m_HaspPrevStatus = GetConnectionStatus();
			
			SetConnectionStatus(hsExpired);			
			
			try
			{
				SetPlugInfo();
			}
			catch(...)
			{
				WriteToLog("HASP: Error during plug information collection", true);
			}
			
			WriteToLog("HASP: License expired", (GetConnectionStatus() != m_HaspPrevStatus));
			
			if(CheckDirectly)
				throw hsExpired;
		}
		else
		{
			LogOut();
			
			m_HaspPrevStatus = GetConnectionStatus();
			
			SetConnectionStatus(hsDisconnected);					
						
			// The feature we are trying to log into doesn't exist in HL plug
			if (Status == HASP_FEATURE_NOT_FOUND)
			{
				m_HaspPrevStatus = GetConnectionStatus();
				
				SetConnectionStatus(hsInvalid);						
				
				WriteToLog("HASP: Invalid product key", (GetConnectionStatus() != m_HaspPrevStatus));
				
				if(CheckDirectly)
					throw hsInvalid;
			}
			else
			{
				WriteToLog("HASP: Plug disconnected", (GetConnectionStatus() != m_HaspPrevStatus));
				
				if(CheckDirectly)
					throw hsDisconnected;
			}
		}			
	}	
}

// Log out from the HL plug
void CHaspInterface::LogOut()
{
	if(m_Hasp)
	{
		haspStatus Status;
		
		try
		{
			Status = m_Hasp->logout();
			WriteToLog(QFormatStr("HASP: logout, line: %d, status: %d", __LINE__, Status));
		}
		catch(...)
		{
			WriteToLog("HASP: Exception was thrown during logout");
		}

		if (! HASP_SUCCEEDED(Status))
			if(Status != HASP_ALREADY_LOGGED_OUT)
				throw EHaspInterfaceException("HASP logout failure");
	}
}

void CHaspInterface::CheckPlugValidityDirectly()
{
	try
	{
		CheckPlugValidity(true);
	}
	catch (THaspConnectionStatus status)
	{
		switch(status)
		{
			case hsConnected:
				break;
			
			case hsDisconnected:
				throw EHaspInterfaceException("HASP is not detected");				
				
			case hsExpired:
				throw EHaspInterfaceException("Your license expired. To activate it, contact your local distibuter or Objet Customer Support");				
				
			case hsInvalid:
				throw EHaspInterfaceException("Invalid product key");				
			
			default:
				break;
		}      
	}
}

// Checking the license status through current Session Information (this should be checked only in Connected state)
THaspConnectionStatus CHaspInterface::GetSessionStatus()
{	
	haspStatus Status;
	QString XMLOutput;
	TInitOle         ole; // Auto-managed object to work with COM instead of Coinitialize
	_di_IXMLDocument XMLDoc = NewXMLDocument();
	
	try
	{
		Status = m_Hasp->getSessionInfo(HASP_SESSION_FORMAT, XMLOutput);
		WriteToLog(QFormatStr("HASP: getSessionInfo, line: %d, status: %d", __LINE__, Status));
	}
	catch(...)
	{
		WriteToLog("HASP: Exception was thrown during getSessionInfo in GetSessionStatus function");		
	}
	
	// In case that the plug was disconnected
	if (Status == HASP_BROKEN_SESSION)
		return GetConnectionStatus();
	
	try
    {
	XMLDoc->LoadFromXML((AnsiString)XMLOutput.c_str());
    }
    catch(...)
    {
        return GetConnectionStatus();
    }
	
	// Collect 'feature' information
	_di_IXMLNode FeatureNode = XMLDoc->DocumentElement->ChildNodes->FindNode("feature");

	AnsiString ID = FeatureNode->Attributes["id"];
	
	// Looking for the cpecific feature we logged into
	while(m_HaspFeatureNum != StrToInt(ID))
    {
		FeatureNode = FeatureNode->NextSibling();
        ID = FeatureNode->Attributes["id"];
    }
	
	AnsiString Expired = FeatureNode->Attributes["expired"];
	
	if(Expired == "true")
		return hsExpired;
	else
		return GetConnectionStatus();	
}

// Check license validity and HL plug availability
void CHaspInterface::CheckPlugValidity(bool CheckDirectly)
{
	CQMutexHolder MutexHolder(&m_MutexCheckPlugValidity);	
	
	int StringSize = 10;
	QString RandomString = QGenerateRandomString(StringSize);
	
	unsigned char StringToEncrypt[11];
	RandomString.copy(StringToEncrypt, RandomString.size());
    StringToEncrypt[RandomString.size()]='\0';

	unsigned char StringToCompare[11];
	RandomString.copy(StringToCompare, RandomString.size());
    StringToCompare[RandomString.size()]='\0';	
	
	
		// Checking the connection status in case that the license is expired during current login session	
		if(GetSessionStatus() == hsExpired)
		{
			if(CheckDirectly)
			{
				m_HaspPrevStatus = GetConnectionStatus();
				SetConnectionStatus(hsExpired);
				WriteToLog("HASP: License expired", (GetConnectionStatus() != m_HaspPrevStatus));
				throw hsExpired;
			}
			else
			{
				m_HaspPrevStatus = GetConnectionStatus();
				SetConnectionStatus(hsExpired);
				WriteToLog("HASP: License expired", (GetConnectionStatus() != m_HaspPrevStatus));
				return;
			}			
		}		
		
		try
		{
			m_Hasp->encrypt(StringToEncrypt, 16);
		}
		catch(...)
		{
			WriteToLog("HASP: Exception was thrown during encrypt");
		}
	
		// The plug is disconnected or license expired if the StringToEncrypt value didn't change after encryption 
		if (_mbscmp(StringToEncrypt, StringToCompare) == 0)
		{
			// Attempt to LogIn to set the actual connection status
			LogIn(CheckDirectly);
			return;	
		}
	
		try
		{
			m_Hasp->decrypt(StringToEncrypt, 16);
		}
		catch(...)
		{
			WriteToLog("HASP: Exception was thrown during decrypt");
		}
	
		// The license is valid, if the string after encryption / decryption and the original string are the same
		if (_mbscmp(StringToEncrypt, StringToCompare) == 0)
		{
			m_HaspPrevStatus = GetConnectionStatus();
			
			SetConnectionStatus(hsConnected);			
			
			WriteToLog("HASP: License is valid");
		}
	}


// Get the current connection status to HL plug
THaspConnectionStatus CHaspInterface::GetConnectionStatus()
{
	CQMutexHolder MutexHolder(&m_MutexHaspConnectionStatus);
	THaspConnectionStatus status;
	status = m_HaspStatus;
	return status;
}

 void CHaspInterface::SetConnectionStatus(THaspConnectionStatus status)
{	
	CQMutexHolder MutexHolder(&m_MutexHaspConnectionStatus);
	m_HaspStatus = status;
}

// Get plug info
THaspPlugInfo CHaspInterface::GetPlugInfo()
{
	return m_PlugInfo;
}

// Set the information about currently connected HL plug
void CHaspInterface::SetPlugInfo()
{
	if (GetConnectionStatus() == hsDisconnected)
		throw EHaspInterfaceException("HASP plug is disconnected. Can't collect information");	
	
	TInitOle         ole; // Auto-managed object to work with COM instead of Coinitialize
	QString          XMLOutput;
	_di_IXMLDocument XMLDoc = NewXMLDocument();
	
	haspStatus Status;

	try
	{
		Status = m_Hasp->getInfo(HASP_LOCAL_SCOPE, HASP_FORMAT, HASP_OBJET_VENDOR_CODE, XMLOutput);
		WriteToLog(QFormatStr("HASP: getInfo, line: %d, status: %d", __LINE__, Status));
	}
	catch(...)
	{
		WriteToLog("HASP: Exception was thrown during getInfo in SetPlugInfo function");
		throw;
	}
	
	if (HASP_SUCCEEDED(Status))
	{
		m_PlugInfo.UptodateDriver = true;
	}
	else
	{
		m_PlugInfo.UptodateDriver = false;
		return;
	}

	XMLDoc->LoadFromXML((AnsiString)XMLOutput.c_str());

	// Collect 'hasp' information
	_di_IXMLNode HaspNode = XMLDoc->DocumentElement->ChildNodes->FindNode("hasp");

    m_PlugInfo.PlugID         = HaspNode->Attributes["id"];
    m_PlugInfo.PlugModel      = HaspNode->Attributes["key_model"];
    m_PlugInfo.ProductionDate = ConvertStringToDate(HaspNode->Attributes["production_date"]);
	
	// Collect 'license_manager' information
	_di_IXMLNode LicenseManagerNode = XMLDoc->DocumentElement->ChildNodes->FindNode("license_manager");
	
	_di_IXMLNode Node    = LicenseManagerNode->ChildNodes->FindNode("hostname");
	m_PlugInfo.HostName  = Node->Text;	
	
	// Collect 'feature' information
	_di_IXMLNode FeatureNode = XMLDoc->DocumentElement->ChildNodes->FindNode("feature");

	AnsiString ID = FeatureNode->Attributes["id"];
	
	// Looking for the cpecific feature we logged into
	while(m_HaspFeatureNum != StrToInt(ID))
    {
		FeatureNode = FeatureNode->NextSibling();
        ID = FeatureNode->Attributes["id"];
    }
	
	m_PlugInfo.FeatureID       = FeatureNode->Attributes["id"];
	m_PlugInfo.IsFeatureUsable = FeatureNode->Attributes["usable"];

    _di_IXMLNode LicenseNode = FeatureNode->ChildNodes->FindNode("license");
	Node                     = LicenseNode->ChildNodes->FindNode("license_type");	
	m_PlugInfo.LicenseType   = Node->Text;
	
	if(m_PlugInfo.LicenseType == "expiration")
	{		
		Node                      = LicenseNode->ChildNodes->FindNode("exp_date");    
		m_PlugInfo.ExpirationDate = ConvertStringToDate(Node->Text);			
	}
	
	if(m_PlugInfo.LicenseType == "trial")
	{
		Node                      = LicenseNode->ChildNodes->FindNode("time_start");
		AnsiString StartTime      = Node->Text;		
		Node                      = LicenseNode->ChildNodes->FindNode("total_time");
		AnsiString TimeToRun      = Node->Text;		
		m_PlugInfo.ExpirationDate = ConvertStringToDate(IntToStr(StrToInt(StartTime) + StrToInt(TimeToRun)));	
	}
}

// Calculate time to expiration in seconds
int CHaspInterface::CalculateTimeToExpiration()
{
	int ret = 0;
	int ExpirationTime = 0;
	
	THaspConnectionStatus ConnectionStatus = GetConnectionStatus();
	
	if (ConnectionStatus == hsDisconnected || ConnectionStatus == hsExpired || ConnectionStatus == hsInvalid)
		return ret;			
	
	TInitOle         ole; // Auto-managed object to work with COM instead of Coinitialize
	QString          XMLOutput;
	_di_IXMLDocument XMLDoc = NewXMLDocument();

	try
	{
		m_Hasp->getInfo(HASP_LOCAL_SCOPE, HASP_FORMAT, HASP_OBJET_VENDOR_CODE, XMLOutput);
	}
	catch(...)
	{
		WriteToLog("HASP: Exception was thrown during getInfo in SetPlugInfo function");		
	}

	try
    {
	XMLDoc->LoadFromXML((AnsiString)XMLOutput.c_str());	
    }
    catch(...)
    {
        return -1;
    }
	
	// Collect 'feature' information
	_di_IXMLNode FeatureNode = XMLDoc->DocumentElement->ChildNodes->FindNode("feature");

	AnsiString ID = FeatureNode->Attributes["id"];
	
	// Looking for the cpecific feature we logged into
	while(m_HaspFeatureNum != StrToInt(ID))
    {
		FeatureNode = FeatureNode->NextSibling();
        ID = FeatureNode->Attributes["id"];
    }
	
    _di_IXMLNode LicenseNode = FeatureNode->ChildNodes->FindNode("license");
	_di_IXMLNode Node        = LicenseNode->ChildNodes->FindNode("license_type");	
	AnsiString LicenseType   = Node->Text;
	
	if(LicenseType == "expiration")
	{		
		Node           = LicenseNode->ChildNodes->FindNode("exp_date");    
		AnsiString str = Node->Text;
		ExpirationTime = StrToInt(str);			
	}	
	else if(LicenseType == "trial")
	{
		Node                 = LicenseNode->ChildNodes->FindNode("time_start");
		AnsiString StartTime = Node->Text;		
		Node                 = LicenseNode->ChildNodes->FindNode("total_time");
		AnsiString TimeToRun = Node->Text;		
		ExpirationTime       = StrToInt(StartTime) + StrToInt(TimeToRun);	
	}
	else
	{
		// In case that the license type is "perpetual" 
		return -1;
	}
	
	ChaspTime haspTime;	

	if (HASP_SUCCEEDED(m_Hasp->getRtc(haspTime)))
	{
		ret = ExpirationTime - haspTime;
	}	
	
	return ret;
}

// Hasp time (in AnsiString format) to date conversion
THaspDate CHaspInterface::ConvertStringToDate(AnsiString HaspTime)
{
    ChaspTime haspTime(StrToInt64(HaspTime));

    THaspDate date;
	
	date.Minute = haspTime.minute();
	date.Hour   = haspTime.hour();
    date.Day    = haspTime.day();    
    date.Month  = haspTime.month();
    date.Year   = haspTime.year();

    return date;    
}

// Get the HASP driver API version. Only available when plug is connected
QString CHaspInterface::GetAPIVersion()
{		
	if (GetConnectionStatus() != hsConnected)
		return "0.0";
	
	TInitOle         ole; // Auto-managed object to work with COM instead of Coinitialize	
	QString          XMLOutput;
	_di_IXMLDocument XMLDoc = NewXMLDocument();
	

	try
	{
		m_Hasp->getInfo(HASP_LOCAL_SCOPE, HASP_API_VER_FORMAT, HASP_OBJET_VENDOR_CODE, XMLOutput);
	}
	catch(...)
	{
		WriteToLog("HASP: Exception was thrown during getInfo in GetAPIVersion function");
	}
	
	try
	{
	XMLDoc->LoadFromXML((AnsiString)XMLOutput.c_str());	
	}
	catch(...)
	{
		return "0.0";
	}
	_di_IXMLNode SessionNode = XMLDoc->DocumentElement->ChildNodes->FindNode("session");

    AnsiString str;
	
	if(SessionNode)
		str = SessionNode->Attributes["apiversion"];
	else
		str = "0.0";
	
	return str.c_str();	
}

// Get the current date from the HL plug
THaspDate CHaspInterface::GetCurrentDate()
{
	THaspDate CurrentDate;
	ChaspTime haspTime;	

	try
	{	
		if (HASP_SUCCEEDED(m_Hasp->getRtc(haspTime)))	
		{
			CurrentDate.Day   = haspTime.day();    
			CurrentDate.Month = haspTime.month();
			CurrentDate.Year  = haspTime.year();
		}
	}
	catch(...)
	{
		WriteToLog("HASP: Exception was thrown during getRtc");
	}
	
	return CurrentDate;
}

// Calculate and set the 'days to expiration' value in 'trial' and 'expiration' type licenses
void CHaspInterface::SetDaysToExpiration()
{
    if(m_PlugInfo.LicenseType == "trial" || m_PlugInfo.LicenseType == "expiration")
    {
		THaspDate CurrentDate = GetCurrentDate();
		
		if (m_PlugInfo.ExpirationDate.Day > 0)
		{
			struct std::tm a = {0, 0, 0, CurrentDate.Day, 
										 CurrentDate.Month, 
										 CurrentDate.Year - 1900};
			struct std::tm b = {0, 0, 0, m_PlugInfo.ExpirationDate.Day, 
										 m_PlugInfo.ExpirationDate.Month, 
										 m_PlugInfo.ExpirationDate.Year - 1900};
			std::time_t x = std::mktime(&a);
			std::time_t y = std::mktime(&b);
			if ( x != (std::time_t)(-1) && y != (std::time_t)(-1) )
				m_DaysToExpiration = (int)(std::difftime(y, x) / (60 * 60 * 24));	
		}
	}
	else
		m_DaysToExpiration = -1;
}

// Get the 'days to expiration' value
int CHaspInterface::GetDaysToExpiration()
{
	return m_DaysToExpiration;
}

// Write to an encrypted log (RF log file) and to general log
void CHaspInterface::WriteToLog(QString string, bool WriteToEncrypted)
{
	if(! m_WriteToLog)
		return;
		
	CQLog::Write(m_HaspLogTag, string);
	if(WriteToEncrypted)
		CQEncryptedLog::Write(m_HaspLogTag, string);
}

void CHaspInterface::SetWriteToLog(bool WriteToLog)
{
    m_WriteToLog = WriteToLog;
}

//-----------------------------------------------------
void CHaspInterfaceWrapper::Init(ChaspFeature HaspFeature, TLogFileTag HaspLogTag, THandleHaspStatusCallback HandleHaspStatusCallback)
{
	if(! m_SingletonInstance)
        m_SingletonInstance = new CHaspInterfaceWrapper(HaspFeature, HaspLogTag, HandleHaspStatusCallback);
}

void CHaspInterfaceWrapper::Init()
{
	if(! m_SingletonInstance)
        m_SingletonInstance = new CHaspInterfaceWrapper();
}

void CHaspInterfaceWrapper::DeInit()
{
	Q_SAFE_DELETE(m_SingletonInstance);
}

CHaspInterfaceWrapper::CHaspInterfaceWrapper(ChaspFeature HaspFeature, TLogFileTag HaspLogTag, THandleHaspStatusCallback HandleHaspStatusCallback)
{
	m_HaspInterfaceInstance = new CHaspInterface(HaspFeature, HaspLogTag, HandleHaspStatusCallback);
}

CHaspInterfaceWrapper::CHaspInterfaceWrapper()
{
	m_HaspInterfaceInstance = new CHaspInterfaceDummy();
}
		
CHaspInterfaceWrapper::~CHaspInterfaceWrapper()
{
	Q_SAFE_DELETE(m_HaspInterfaceInstance);
}

CHaspInterface *CHaspInterfaceWrapper::Instance()
{
	if(m_SingletonInstance)
		return m_SingletonInstance->m_HaspInterfaceInstance;
	else
		return NULL; 
}

//--------------------------------------

int CHaspInterfaceDummy::GetDaysToExpiration()
{
	return 1000;
}

int CHaspInterfaceDummy::CalculateTimeToExpiration()
{
	return -1;
}

THaspConnectionStatus CHaspInterfaceDummy::GetConnectionStatus()
{
	return hsConnected;
}

THaspPlugInfo CHaspInterfaceDummy::GetPlugInfo()
{
	THaspPlugInfo PlugInfo;	
	
	PlugInfo.HostName        = "Emulation";	
	PlugInfo.PlugID          = "0";
    PlugInfo.PlugModel       = "Emulation";	
	PlugInfo.FeatureID       = "0";
	PlugInfo.IsFeatureUsable = "true";
	PlugInfo.LicenseType     = "Emulation";

	return  PlugInfo;
}

QString CHaspInterfaceDummy::GetAPIVersion()
{
	return "0.0";
}
		

