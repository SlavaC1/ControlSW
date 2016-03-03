object WizardForm: TWizardForm
  Left = 274
  Top = 279
  AutoSize = True
  BorderIcons = []
  BorderStyle = bsDialog
  Caption = 'Wizard'
  ClientHeight = 294
  ClientWidth = 517
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnClose = FormClose
  OnCloseQuery = FormCloseQuery
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 13
  inline WizardViewerFrame: TWizardViewerFrame
    Left = 0
    Top = 0
    Width = 517
    Height = 294
    Font.Charset = HEBREW_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    TabOrder = 0
    inherited DoneButton: TBitBtn
      TabOrder = 7
    end
    inherited NextButton: TBitBtn
      OnClick = WizardViewerFrameNextButtonClick
    end
    inherited PrevButton: TBitBtn
      TabOrder = 5
      OnClick = WizardViewerFramePrevButtonClick
    end
    inherited CancelButton: TBitBtn
      Left = 254
      TabOrder = 4
      ExplicitLeft = 254
    end
    inherited MainPanel: TPanel
      TabOrder = 6
    end
    inherited HelpButton: TBitBtn
      Left = 11
      TabOrder = 1
      ExplicitLeft = 11
    end
    inherited UserButton2: TBitBtn
      Left = 92
      TabOrder = 2
      ExplicitLeft = 92
    end
    inherited UserButton1: TBitBtn
      Left = 173
      TabOrder = 3
      ExplicitLeft = 173
    end
  end
end
