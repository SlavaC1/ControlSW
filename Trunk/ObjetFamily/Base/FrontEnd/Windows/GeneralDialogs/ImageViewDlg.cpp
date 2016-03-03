//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop


#include "ImageViewDlg.h"
#include "QUtils.h"
#include <algorithm>


//---------------------------------------------------------------------------
#pragma resource "*.dfm"

#define ZOOM_STEP   2
#define ZOOM_IN     0
#define ZOOM_OUT    1


__fastcall TImageViewForm :: TImageViewForm (TComponent* Owner)
    : TForm (Owner)
{
	m_LastBMPBuffer = NULL;
	m_VScreen = NULL;
	m_BMPRequestCallback = NULL;
	m_BMPRequestCallbackParam = 0;
}
//---------------------------------------------------------------------------
void TImageViewForm::SetCaption(const char* Caption)
{
   m_Caption = Caption;
}
//---------------------------------------------------------------------------
void __fastcall TImageViewForm::DisplayBMP(AnsiString          WindowCaption,
                                           int                 Chamber,
                                           TBMPRequestCallback BMPRequestCallback,
                                           TGenericCallback    BMPReleaseCallback,
                                           TGenericCockie      Cockie)
{
  m_Caption                 = WindowCaption;
  Caption                   = m_Caption;

  m_LastBMPBuffer           = NULL;

  m_BMPRequestCallback      = BMPRequestCallback;
  m_BMPRequestCallbackParam = Chamber;
  m_BMPReleaseCallback      = BMPReleaseCallback;
  m_Cockie                  = Cockie;

  int Width,Height;
  RefreshDisplay(Width,Height);
  RearrangeComponentsSize(Width,Height);

  // Save image (if we are in automatic save mode)
  if(ContinousSaveMenuItem->Checked)
  {
    BYTE *CurrentBMPBuffer;

    try
    {
      try
      {
        int Width,Height,SliceNumber;
        CurrentBMPBuffer = GetBMPSourceBuffer(Width,Height,SliceNumber);

        if(CurrentBMPBuffer != NULL)
           SaveBufferToBMPFile(CurrentBMPBuffer,Width,Height,InsertSliceNumber(SaveDialog1->FileName,SliceNumber));
      }
      catch(...)
        {
          ContinousSaveMenuItem->Checked = false;
          SaveToFileMenuItem->Enabled = true;
        }
    } __finally
      {
        if(CurrentBMPBuffer != NULL)
          (*m_BMPReleaseCallback)(m_Cockie);
      }
  }
}
//---------------------------------------------------------------------------
AnsiString TImageViewForm::InsertSliceNumber(const AnsiString& FileName, int SliceNumber)
{
  if (FileName == "")
      return "";
  QString   NewFileName = FileName.c_str();
  char      Extension[] = ".bmp";
  size_t    pos          = NewFileName.rfind(Extension);
  NewFileName.insert(pos, "[" + QIntToStr(SliceNumber) + "]");
  return NewFileName.c_str();
}
//---------------------------------------------------------------------------
void TImageViewForm::DrawBinaryImageOnDC(int Width,int Height,BYTE *BMPBuffer,TRect& ViewRect,HDC dc)
{
  BITMAP Bmp;

  Bmp.bmType = 0;
  Bmp.bmWidth = Width;
  Bmp.bmHeight = Height;
  Bmp.bmWidthBytes = Width / 8;
  Bmp.bmPlanes = 1;
  Bmp.bmBitsPixel = 1;
  Bmp.bmBits = BMPBuffer;

  HBITMAP BH = CreateBitmapIndirect(&Bmp);

  if(BH != NULL)
  {
    HDC ScreenDC = GetDC(0);
    HDC cdc = CreateCompatibleDC(ScreenDC);
    SelectObject(cdc,BH);

    if(StretchMenuItem->Checked)
      StretchBlt(dc,ViewRect.Left,ViewRect.Top,ViewRect.Width(),ViewRect.Height(),cdc,0,0,Width,Height,SRCCOPY);
    else
      BitBlt(dc,ViewRect.Left,ViewRect.Top,ViewRect.Width(),ViewRect.Height(),cdc,ViewRect.Left,ViewRect.Top,SRCCOPY);

    DeleteDC(cdc);
    DeleteDC(ScreenDC);
    DeleteObject(BH);
  }
}
//---------------------------------------------------------------------------
void TImageViewForm::RefreshDisplay(int& Width,int& Height)
{
  int   SliceNumber;
  BYTE* CurrentBMPBuffer = GetBMPSourceBuffer(Width,Height,SliceNumber);

  Caption = m_Caption;
  if (SliceNumber != -1)
      Caption += " [Slice: "+ IntToStr(SliceNumber) + "]";

  if(CurrentBMPBuffer != NULL)
  {
    int L = HorzImageScrollBar->Position;
    int T = VertImageScrollBar->Position;
    int W = std::min(GetDisplayClientWidth(),Width);
    int H = std::min(GetDisplayClientHeight(),Height);

    if(StretchMenuItem->Checked)
      CopyStretchBinaryImageToVScreen(CurrentBMPBuffer,Width,Height);
    else
    {
      TRect ViewRect;

      ViewRect.left   = (L / 8) * 8;
      ViewRect.top    = T;
      ViewRect.right  = ViewRect.left + W;
      ViewRect.bottom = ViewRect.top + H;

      CopyBinaryImageToVScreen(CurrentBMPBuffer,Width / 8,ViewRect);
    }

    if(m_LastBMPBuffer == NULL)
      (*m_BMPReleaseCallback)(m_Cockie);

    PaintBox1->Canvas->Draw(0,0,m_VScreen);
  }
}
//---------------------------------------------------------------------------
void TImageViewForm::AllocVScreen(void)
{
  if(m_VScreen != NULL)
    delete m_VScreen;

  m_VScreen = new Graphics::TBitmap;

  m_VScreen->PixelFormat = pf1bit;
  m_VScreen->Width = GetDisplayClientWidth();
  m_VScreen->Height = GetDisplayClientHeight();

  // Prepare two colors palette
  LOGPALETTE *Pal = (LOGPALETTE *)new BYTE[sizeof(LOGPALETTE) + sizeof(PALETTEENTRY)];

  Pal->palVersion = 0x300;
  Pal->palNumEntries = 2;
  Pal->palPalEntry[0].peRed   = 0;
  Pal->palPalEntry[0].peGreen = 0;
  Pal->palPalEntry[0].peBlue  = 0;
  Pal->palPalEntry[0].peFlags = 0;
  Pal->palPalEntry[1].peRed   = 0xff;
  Pal->palPalEntry[1].peGreen = 0xff;
  Pal->palPalEntry[1].peBlue  = 0;
  Pal->palPalEntry[1].peFlags = 0;

  m_VScreen->Palette = CreatePalette(Pal);

  delete []((BYTE *)Pal);
}
//---------------------------------------------------------------------------
void TImageViewForm::CopyBinaryImageToVScreen(BYTE *BMPBuffer,int Stride,TRect& ViewRect)
{
  BYTE *BufferPos = &BMPBuffer[ViewRect.Top * Stride + ViewRect.Left / 8];
  int BytesToCopy = (ViewRect.Width() + 7) / 8;

  for(int i = 0; i < ViewRect.Height(); i++)
  {
    memcpy(m_VScreen->ScanLine[i],BufferPos,BytesToCopy);
    BufferPos += Stride;
  }
}
//---------------------------------------------------------------------------
int TImageViewForm::GetDisplayClientWidth(void)
{
  return (ImagePanel->ClientWidth - VertImageScrollBar->Width);
}
//---------------------------------------------------------------------------
int TImageViewForm::GetDisplayClientHeight(void)
{
  return (ImagePanel->ClientHeight - HorzImageScrollBar->Height);
}
//---------------------------------------------------------------------------
void TImageViewForm::CopyStretchBinaryImageToVScreen(BYTE *BMPBuffer,int BufferWidth,int BufferHeight)
{
  float XRatio = (float)BufferWidth / (float)m_VScreen->Width;
  float YRatio = (float)BufferHeight / (float)m_VScreen->Height;


  BYTE *SrcPtr,*DstPtr,*SrcLineStartPtr;

  BYTE DstMask;
  BYTE SrcMask;
  int Stride = BufferWidth / 8;

  // Clear entire VScreen
  m_VScreen->Canvas->Brush->Color = clBlack;
  m_VScreen->Canvas->FillRect(Rect(0,0,m_VScreen->Width,m_VScreen->Height));
  float SrcXPos,SrcYPos = 0.0f;
  int SrcXPosInt;

  for(int y = 0; y < m_VScreen->Height; y++)
  {
    SrcXPos = 0.0f;
    SrcXPosInt = 0;

    SrcLineStartPtr = &BMPBuffer[((int)SrcYPos) * Stride];
    SrcPtr = SrcLineStartPtr;

    for(int x = 0; x < m_VScreen->Width; x++)
    {
      DstPtr = (BYTE *)m_VScreen->ScanLine[y] + (x >> 3);

      // Prepare pixel mask
      DstMask = 0x80 >> (x & 7);
      SrcMask = 0x80 >> (SrcXPosInt & 7);

      // Set relavent bit
      if(*SrcPtr & SrcMask)
        *DstPtr |= DstMask;

      SrcXPos += XRatio;
      SrcXPosInt = (int)SrcXPos;
      SrcPtr = SrcLineStartPtr + (SrcXPosInt >> 3);
    }

    SrcYPos += YRatio;
  }
}
//---------------------------------------------------------------------------
void TImageViewForm::RearrangeComponentsSize(int Width,int Height)
{
  if(StretchMenuItem->Checked)
  {
    PaintBox1->Width  = GetDisplayClientWidth();
    PaintBox1->Height = GetDisplayClientHeight();

    HorzImageScrollBar->Max = 0;
    VertImageScrollBar->Max = 0;
  } else
    {
      PaintBox1->Width  = std::min(GetDisplayClientWidth(),Width);
      PaintBox1->Height = std::min(GetDisplayClientHeight(),Height);

      HorzImageScrollBar->Max = Width - PaintBox1->Width;
      VertImageScrollBar->Max = Height - PaintBox1->Height;
    }
}
//---------------------------------------------------------------------------
void TImageViewForm::SaveBufferToBMPFile(BYTE *BMPBuffer,int BufferWidth,int BufferHeight,AnsiString FileName)
{
  // Prepare temporary bitmap
  Graphics::TBitmap *Bmp = new Graphics::TBitmap;

  int Stride = BufferWidth / 8;

  try
  {
    // Prepare two colors palette
    LOGPALETTE *Pal = (LOGPALETTE *)new BYTE[sizeof(LOGPALETTE) + sizeof(PALETTEENTRY)];

    Pal->palVersion = 0x300;
    Pal->palNumEntries = 2;
    Pal->palPalEntry[0].peRed   = 0;
    Pal->palPalEntry[0].peGreen = 0;
    Pal->palPalEntry[0].peBlue  = 0;
    Pal->palPalEntry[0].peFlags = 0;
    Pal->palPalEntry[1].peRed   = 0xff;
    Pal->palPalEntry[1].peGreen = 0xff;
    Pal->palPalEntry[1].peBlue  = 0;
    Pal->palPalEntry[1].peFlags = 0;

    Bmp->PixelFormat = pf1bit;
    Bmp->Width = BufferWidth;
    Bmp->Height = BufferHeight;
    Bmp->Palette = CreatePalette(Pal);

    delete []((BYTE *)Pal);

    // Copy data line by line
    for(int i = 0; i < BufferHeight; i++)
      memcpy(Bmp->ScanLine[i],&BMPBuffer[i * Stride],Stride);

    Bmp->SaveToFile(FileName);

  } __finally
    {
      delete Bmp;
    }
}
//---------------------------------------------------------------------------
// Return a pointer to the current active BMP source buffer
BYTE *TImageViewForm::GetBMPSourceBuffer(int& Width,int& Height,int& SliceNumber)
{
  BYTE* CurrentBMPBuffer;
  SliceNumber = -1;
  // Check if we need to use local buffer or to poll from the layer maker
  if(m_LastBMPBuffer == NULL)
  {
    if(m_BMPRequestCallback == NULL)
      return NULL;

    CurrentBMPBuffer = (*m_BMPRequestCallback)(m_BMPRequestCallbackParam,Width,Height,SliceNumber,m_Cockie);

    // If no buffer is available, unlock and leave
    if(CurrentBMPBuffer == NULL)
    {
      (*m_BMPReleaseCallback)(m_Cockie);
      return NULL;
    }

  } else
      CurrentBMPBuffer = m_LastBMPBuffer;

  return CurrentBMPBuffer;
}
//---------------------------------------------------------------------------



/*******************************************************************************************************************/
#pragma warn -8057 // Disable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/
void __fastcall TImageViewForm :: Image1MouseMove
        (TObject *Sender, TShiftState Shift, int X, int Y)
{
  StatusBar1->SimpleText = IntToStr(X) + "," + IntToStr(Y);
}
//---------------------------------------------------------------------------
void __fastcall TImageViewForm :: ClosePopMnuClick (TObject *Sender)
{
  Close ();
}
//---------------------------------------------------------------------------
void __fastcall TImageViewForm::ContinousSaveMenuItemClick(TObject *Sender)
{
  ContinousSaveMenuItem->Checked = !ContinousSaveMenuItem->Checked;

  if(ContinousSaveMenuItem->Checked)
  {
    int Width,Height,SliceNumber;

    // Lock refresh whil showing save dialog
    BYTE *CurrentBMPBuffer = GetBMPSourceBuffer(Width,Height,SliceNumber);

    if(!SaveDialog1->Execute())
      ContinousSaveMenuItem->Checked = false;

    if(CurrentBMPBuffer != NULL)
      (*m_BMPReleaseCallback)(m_Cockie);
  }

  SaveToFileMenuItem->Enabled = !ContinousSaveMenuItem->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TImageViewForm::FormCreate(TObject *Sender)
{
  m_VScreen = NULL;
  m_LastBMPBuffer = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TImageViewForm::PaintBox1Paint(TObject *Sender)
{
  int Width,Height;
  RefreshDisplay(Width,Height);
}
//---------------------------------------------------------------------------
void __fastcall TImageViewForm::FormDestroy(TObject *Sender)
{
  if(m_VScreen != NULL)
    delete m_VScreen;
}
//---------------------------------------------------------------------------
void __fastcall TImageViewForm::HorzImageScrollBarChange(TObject *Sender)
{
  int Width, Height;
  RefreshDisplay(Width,Height);
}
//---------------------------------------------------------------------------
void __fastcall TImageViewForm::FormResize(TObject *Sender)
{
  AllocVScreen();
  RearrangeComponentsSize(0,0);
}
//---------------------------------------------------------------------------
void __fastcall TImageViewForm::StretchMenuItemClick(TObject *Sender)
{
  StretchMenuItem->Checked = !StretchMenuItem->Checked;

  int Width,Height;
  RefreshDisplay(Width,Height);
  RearrangeComponentsSize(Width,Height);
}
//---------------------------------------------------------------------------
void __fastcall TImageViewForm::SaveToFileMenuItemClick(TObject *Sender)
{
  int Width,Height,SliceNumber;
  BYTE *CurrentBMPBuffer = GetBMPSourceBuffer(Width,Height,SliceNumber);

  if(CurrentBMPBuffer != NULL)
  {
    if(SaveDialog1->Execute())
       SaveBufferToBMPFile(CurrentBMPBuffer,Width,Height,InsertSliceNumber(SaveDialog1->FileName,SliceNumber));

    (*m_BMPReleaseCallback)(m_Cockie);
  }
}
/*******************************************************************************************************************/
#pragma warn .8057 // Enable warning Number 8057 "Never use parameter"
/*******************************************************************************************************************/

