//---------------------------------------------------------------------------
#ifndef ImageViewDlgH
#define ImageViewDlgH
//---------------------------------------------------------------------------
#include <vcl\Classes.hpp>
#include <vcl\Controls.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
#include <vcl\ExtCtrls.hpp>
#include <vcl\Menus.hpp>
#include <vcl\ComCtrls.hpp>
#include <Dialogs.hpp>

#include "QTypes.h"

//---------------------------------------------------------------------------

typedef BYTE * (*TBMPRequestCallback)(int/*Chamber*/, int&/*Width*/,int&/*Height*/,int&/*SliceNumber*/,TGenericCockie /*Cockie*/);

class TImageViewForm : public TForm
{
__published:	// IDE-managed Components
	TStatusBar *StatusBar1;
    TPopupMenu *PopupMenu1;
    TMenuItem  *ClosePopMnu;
    TMenuItem  *N1;
        TMenuItem *StretchMenuItem;
        TPanel *ImagePanel;
        TPaintBox *PaintBox1;
        TScrollBar *HorzImageScrollBar;
        TScrollBar *VertImageScrollBar;
        TMenuItem *SaveToFileMenuItem;
        TMenuItem *N2;
        TMenuItem *ContinousSaveMenuItem;
        TSaveDialog *SaveDialog1;

	void __fastcall  Image1MouseMove     (TObject *Sender,
                                          TShiftState Shift,
                                          int X, int Y);

    void __fastcall  ClosePopMnuClick    (TObject *Sender);
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall PaintBox1Paint(TObject *Sender);
        void __fastcall FormDestroy(TObject *Sender);
        void __fastcall HorzImageScrollBarChange(TObject *Sender);
        void __fastcall FormResize(TObject *Sender);
        void __fastcall StretchMenuItemClick(TObject *Sender);
        void __fastcall SaveToFileMenuItemClick(TObject *Sender);
        void __fastcall ContinousSaveMenuItemClick(TObject *Sender);

private:
    BYTE *m_LastBMPBuffer;

    Graphics::TBitmap *m_VScreen;

    TBMPRequestCallback m_BMPRequestCallback;
    int                 m_BMPRequestCallbackParam;
    TGenericCallback    m_BMPReleaseCallback;
    TGenericCockie      m_Cockie;
    AnsiString          m_Caption;
    
    void AllocVScreen(void);

    int __fastcall BinaryToBMP(int StartLine, int NumLines, int BitWidth,BYTE *InBuffer,  Graphics::TBitmap *pBitmap);

    void DrawBinaryImageOnDC(int Width,int Height,BYTE *BMPBuffer,TRect& ViewRect,HDC dc);

    void CopyBinaryImageToVScreen(BYTE *BMPBuffer,int Stride,TRect& ViewRect);
    void CopyStretchBinaryImageToVScreen(BYTE *BMPBuffer,int BufferWidth,int BufferHeight);

    void RefreshDisplay(int& Width,int& Height);

    // Return a pointer to the current active BMP source buffer
    BYTE *GetBMPSourceBuffer(int& Width,int& Height,int& SliceNumber);

    void RearrangeComponentsSize(int Width,int Height);

    void SaveBufferToBMPFile(BYTE *BMPBuffer,int BufferWidth,int BufferHeight,AnsiString FileName);
    AnsiString InsertSliceNumber(const AnsiString& FileName, int SliceNumber);

    int GetDisplayClientWidth(void);
    int GetDisplayClientHeight(void);

public:
    void SetCaption(const char* Caption);

   __fastcall  TImageViewForm  (TComponent* Owner);

   void __fastcall DisplayBMP(AnsiString WindowCaption,int Chamber,TBMPRequestCallback BMPRequestCallback,
                              TGenericCallback BMPReleaseCallback,TGenericCockie Cockie);
};

#endif
