// BurnerAppForm.cpp : main project file.

#include "stdafx.h"
#include "BurnerAppForm.h"

using namespace BurnerApp;

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	// Enabling Windows XP visual effects before any controls are created
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	// Create the main window and run it
	Application::Run(gcnew BurnerAppForm());
	return 0;
}
