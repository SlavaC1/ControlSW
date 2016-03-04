//---------------------------------------------------------------------------

#ifndef MultiGridFormH
#define MultiGridFormH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>

#include "QTypes.h"

class TMultiGridEdit : public TEdit
{
private:
   int  m_TagsSize;

protected:

public:
//Members Variables
   int RowTag,ColTag,CellTag;
   int* Tags;
//Members Function
   __fastcall virtual TMultiGridEdit(TComponent* AOwner, int TagsSize=0);
   __fastcall virtual ~TMultiGridEdit();

__published:

};

//---------------------------------------------------------------------------
class TMultiGridForm : public TForm
{
__published:	// IDE-managed Components
        TPanel *MainPanel;
        TButton *RestoreButton;
        TButton *CancelButton;
        TButton *SaveButton;
    void __fastcall FormShow(TObject *Sender);
        void __fastcall RestoreButtonClick(TObject *Sender);
        void __fastcall SaveButtonClick(TObject *Sender);

private:	// User declarations
    int m_ColNum, m_RowNum, m_CellPartsNum;

    TLabel**  ColumnLabels;
    TLabel*** RowLabels;
    TMultiGridEdit**** Edits;
    QString*** OriginalValues;
    
public:		// User declarations

    __fastcall TMultiGridForm(TComponent* Owner, int ColNum, int RowNum, int CellPartsNum=1, int TagsSize=0);
    __fastcall ~TMultiGridForm();

    void SetRowLabelCaption(QString& Caption, int Cell=-1);
    void SetRowLabelWidth(int Width);
    void SetRowLabelHeight(int Height, int Cell=-1);
    
    void SetColumnLabelCaption(int col, QString Caption);
    void SetColumnWidth(int Width, int col=-1);
    void SetColumnLabelHeight(int Height, int col=-1);
    void SetColumnLabelColor(TColor Color, int col=-1);
    
    void RePosition(void);

//    void SetEditTextColor(TColor Color, int Row, int Cell, int Col);
    int GetEditTag(int TagIndex, int Row, int Cell, int Col);
    void SetEditTab(int Tab, int Row, int Cell, int Col);
    void SetEditTag(int Tag, int Row, int Cell, int Col);
    void SetEditTag(int Tag, int TagIndex, int Row, int Cell, int Col);
    void SetEditValue(QString Text, int Row, int Cell, int Col);
    void SetEditEnabled(bool Enabled, int Row, int Cell, int Col, TColor Color=clInactiveBorder);
    void SetKeyPressEvent(TKeyPressEvent pKeyPressEvent, int Cell=0);
    void SetEditExitEvent(TNotifyEvent EditExitEvent, int Cell=0);

    QString GetEditValue(int Row, int Cell, int Col);
    bool GetEditEnabled(int Row, int Cell, int Col);
    void Clear(void);

    void __fastcall OnChange(TObject *Sender);

};
//---------------------------------------------------------------------------
extern PACKAGE TMultiGridForm *MultiGridForm;
//---------------------------------------------------------------------------
#endif
