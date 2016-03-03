//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
	TButton *Button1;
	TEdit *Edit1;
	TCheckBox *CheckBox1;
	TRadioButton *RadioButton1;
	TRadioButton *RadioButton2;
	TListBox *ListBox1;
	TComboBox *ComboBox1;
	TTabControl *TabControl1;
	TPageControl *PageControl1;
	TTrackBar *TrackBar1;
	TUpDown *UpDown1;
	TScrollBar *ScrollBar1;
	TTabSheet *TabSheet1;
	TTabSheet *TabSheet2;
	TTabSheet *TabSheet3;
	TGroupBox *GroupBox1;
	TBitBtn *BitBtn1;
	TButton *Button2;
	TSpeedButton *SpeedButton1;
	TSpeedButton *SpeedButton3;
	TButton *Button3;
	TBitBtn *BitBtn2;
private:	// User declarations
public:		// User declarations
	__fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif

