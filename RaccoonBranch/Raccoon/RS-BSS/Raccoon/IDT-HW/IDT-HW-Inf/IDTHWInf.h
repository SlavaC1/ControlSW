#ifndef IDTHWInfH
#define IDTHWInfH

#if defined(WIN32) || defined(_WIN32)
#	ifdef _LIB
#		define IDTHWINF_LIB
#	elif IDTHWINF_EXPORTS
#       define IDTHWINF_LIB __declspec(dllexport)
#   elif defined(IDTHWINF_IMPORT)
#       define IDTHWINF_LIB __declspec(dllimport)
#   else
#       define IDTHWINF_LIB
#   endif
#else
#   define IDTHWINF_LIB
#endif

#ifdef __cplusplus
extern "C" {
#endif

	/*
	Initializes IDT HW.
	cartridgesCount: number of connected cartridges (output parameter).
	*/
	int IDTHWINF_LIB IdtInit(int *cartridgesCount);	
	/*
	Frees all resources.	
	*/
	int IDTHWINF_LIB IdtClose();
	/*
	Gets in-place status of all cartridges.
	status: a 32-bit integer representing status of all cartridges.
	*/
	int IDTHWINF_LIB IdtGetInPlaceStatus(int *status);
	/*
	Selects cartridge to send and receive data from.
	cartridgeNum: cartridge number.
	*/
	int IDTHWINF_LIB IdtSelectChannel(int cartridgeNum);
	/*
	Sends data to selected cartridge.
	bufferSize: buffer size.
	buffer: the data to send.
	*/
	int IDTHWINF_LIB IdtSendData(int bufferSize, char *buffer);
	/*
	Receive data from selected cartridge.
	bufferSize: buffer size.
	buffer: the received data.
	*/
	int IDTHWINF_LIB IdtReceiveData(int bufferSize, char *buffer);

#ifdef __cplusplus
}
#endif
#endif
