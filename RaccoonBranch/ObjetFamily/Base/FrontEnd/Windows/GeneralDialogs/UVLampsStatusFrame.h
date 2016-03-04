//---------------------------------------------------------------------------


#ifndef UVLampsStatusFrameH
#define UVLampsStatusFrameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "WizardViewer.h"
#include "UVLampsStatusWizardPage.h"
#include "PythonWizardPages.h"
#include <ExtCtrls.hpp>
#include <ImgList.hpp>
#include "GlobalDefs.h"

const int USER_ENTRY_RESPONSE_TIMEOUT = 2 * 60;

const int HS_R_STATUS = 0;
const int HS_L_STATUS = 1;
const int HQ_R_STATUS = 2;
const int HQ_L_STATUS = 3;
const int HS_R_LABEL = 4;
const int HS_L_LABEL = 5;
const int HQ_R_LABEL = 6;
const int HQ_L_LABEL = 7;

#define LAMP_STATUS_OK "OK"


//---------------------------------------------------------------------------
class TUVLampStatusFrame : public TFrame
{
__published:	// IDE-managed Components
        TBevel *TopHorizontalBevel;
        TBevel *VerticalBevel;
private:	// User declarations
public:		// User declarations

        TLabel* m_UVLampLabelArray    [NUMBER_OF_UV_LAMPS];        
        TLabel* m_ModeLabelArray      [NUMBER_OF_QUALITY_MODES]
                                      [NUM_OF_OPERATION_MODES];
        TImage* m_ImagesArray         [NUMBER_OF_UV_LAMPS]
                                      [NUMBER_OF_QUALITY_MODES]
                                      [NUM_OF_OPERATION_MODES];
        TLabel* m_UVStatusLabelArray  [NUMBER_OF_UV_LAMPS]
                                      [NUMBER_OF_QUALITY_MODES]
                                      [NUM_OF_OPERATION_MODES];

        __fastcall TUVLampStatusFrame(TComponent* Owner);

};
//---------------------------------------------------------------------------
extern PACKAGE TUVLampStatusFrame *UVLampStatusFrame;
//---------------------------------------------------------------------------

class CUVLampsStatusPageViewer : public CCustomWizardPageViewer {
private:
    TUVLampStatusFrame *m_UVLampStatusFrame;

    void UpdateUVLampsStatus(CUVLampsStatusWizardPage *Page);
    void UpdateUVLampsStatusPy(CPythonGenericCustomWizardPage *Page);

public:
    void Prepare(TWinControl *PageWindow,CWizardPage *WizardPage);
    void Refresh(TWinControl *PageWindow,CWizardPage *WizardPage);
    void Leave(TWinControl *PageWindow,CWizardPage *WizardPage,TWizardPageLeaveReason LeaveReason);

};

#endif
