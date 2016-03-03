    
    prototype DisplayMaintenanceOptions();
    /* Upgrade of machine version during wizard is  Forbidden */
    prototype IsWithInWizard(string,string);  
    prototype STRING RemoveString(STRING, STRING);
    prototype DelShortcut();
   // Win32 APIs related constants and definitions   

/***********/
/* Defines */
/***********/ 
#define TH32CS_SNAPPROCESS  0x00000002  

typedef PROCESSENTRY32
begin
	LONG     dwSize;
	LONG     cntUsage;
	LONG     th32ProcessID;
	POINTER  th32DefaultHeapID;
	LONG     th32ModuleID;
	LONG     cntThreads;
	LONG     th32ParentProcessID;
	LONG     pcPriClassBase;
	LONG     dwFlags;
	STRING   szExeFile[MAX_PATH];
end;   

// Win32 API function prototypes  
prototype LONG KERNEL.CreateToolhelp32Snapshot(LONG, LONG);  
prototype BOOL KERNEL.Process32First( NUMBER, POINTER);
prototype BOOL KERNEL.Process32Next(NUMBER, POINTER);
prototype BOOL KERNEL.CloseHandle(HWND );

/****************/                     
/* Declerations */
/****************/ 

// our function for getting the process running state
prototype BOOL IsProcesRunning( STRING ); 

