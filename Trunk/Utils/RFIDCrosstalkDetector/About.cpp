//---------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "About.h"
//---------------------------------------------------------------------
#pragma resource "*.dfm"
TAboutBox *AboutBox;

//---------------------------------------------------------------------
__fastcall TAboutBox::TAboutBox(TComponent* AOwner)	: TForm(AOwner)
{
	DWORD VersionHandle;
	DWORD VersionSize = GetFileVersionInfoSize(Application->ExeName.c_str(), &VersionHandle);

	if(VersionSize)
	{
		void *pBuffer = new char[VersionSize];
		
		if (GetFileVersionInfo(Application->ExeName.c_str(), VersionHandle, VersionSize, pBuffer))
		{
			char *b;
			UINT buflen;
			if (VerQueryValue(pBuffer, TEXT("\\StringFileInfo\\040904E4\\FileVersion"), (void** )&b, &buflen))
			{
				Version->Caption = "Version: " + AnsiString(b);
			}
		}
		delete[] pBuffer;
	}
}
//---------------------------------------------------------------------
