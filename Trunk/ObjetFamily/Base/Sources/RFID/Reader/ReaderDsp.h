//ReaderDsp.h
//
#ifndef __CREADERDSP
#define __CREADERDSP

#include <string>
using namespace std ;
#include "Tag.h"
//#include "DLLVersion.h"

////////////////////////////////////////////////////////////////
class CReaderDsp
{
public:
	CReaderDsp();
	~CReaderDsp();
	int Connect(int iComPort = 1/*, int iBaudRate = 9600, int iParity = 0, int iDataBits = 8, int iStopBit = 1*/);
	int Disconnect();
	bool IsConnect();

	int SetChannel(int v_iChannel);//sets active channel
	int CheckTags();//Checks number of tags currently in the reader
	CTag* GetTags();//Gets an array of Tags - currently available tags

	int GetCurrentError();//Returns current error number

	//int GetDLLVersion();
protected:
    int SetInitParam();// initialization of parameters
	int CreateTags();//Creates an array with all availabe tags
private:

	CLogic*	  m_pLogic;
	CTag	  m_pTags[CHANNELS][CHANNELS];//an array of current tags
	int		  m_iChannel;
};


#endif