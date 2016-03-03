//---------------------------------------------------------------------------

#ifndef DoorDlgH
#define DoorDlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>

//---------------------------------------------------------------------------

#define ARRAY_SIZR 6 //Need to check the array size before using  "DoorArray"

enum {
	  PRINTER_COVER_IS_OPEN                  = 0,
	  SIDE_DOOR_COVER_IS_OPEN                = 1,
	  FRONT_RIGHT_DOOR_COVER_IS_OPEN         = 2,
	  FRONT_LEFT_DOOR_COVER_IS_OPEN          = 3,
	  REAR_EMERGENCY_STOP_BUTTON_IS_PRESSED  = 4,
	  FRONT_EMERGENCY_STOP_BUTTON_IS_PRESSED = 5,
	  SERVICE_DOOR_COVER_IS_OPEN             = 6,
	  };

static const char* DoorArray[ARRAY_SIZR] ={"Printer cover is open.",
								 "Service door is open.",
								 "Front right door is open.",
								 "Front left door is open.",
								 "Rear emergency stop button is pressed.",
								 "Front emergency stop button is pressed."};




static const char *ToContinueArray[] ={"To continue, close the door.",
                                      "To continue, release the emergency stop button."};

class TDoorCloseDlg : public TForm
{
__published:	// IDE-managed Components
	TLabel *Label1;
        TLabel *Label2;
        void __fastcall DoorCancelBitBtn1Click(TObject *Sender);
private:	// User declarations

public:		// User declarations
		__fastcall TDoorCloseDlg(TComponent* Owner);
		void __fastcall ChangeDoorsGuiLabels(int doorIndex);   
};
//---------------------------------------------------------------------------
extern PACKAGE TDoorCloseDlg *DoorCloseDlg;
//---------------------------------------------------------------------------
#endif
