object ParametersExposureLevelForm: TParametersExposureLevelForm
  Left = 556
  Top = 163
  BorderStyle = bsDialog
  Caption = 'Enter Password'
  ClientHeight = 101
  ClientWidth = 268
  Color = clBtnFace
  DefaultMonitor = dmDesktop
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsStayOnTop
  OldCreateOrder = False
  Position = poOwnerFormCenter
  OnClose = FormClose
  OnPaint = FormPaint
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 26
    Top = 60
    Width = 147
    Height = 26
    Caption = 'This configuration screen is for Technical Support use only'
    WordWrap = True
  end
  object IncorrectPass: TLabel
    Left = 38
    Top = 41
    Width = 90
    Height = 13
    Caption = 'Incorrect password'
    Color = clWindowText
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clRed
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentColor = False
    ParentFont = False
    Visible = False
  end
  object ChooseUserGroupBox: TGroupBox
    Left = 8
    Top = 104
    Width = 248
    Height = 177
    Caption = ' Choose user type '
    TabOrder = 0
    Visible = False
    object RadioButton1: TRadioButton
      Tag = 2
      Left = 12
      Top = 113
      Width = 113
      Height = 17
      Caption = 'R&&D'
      TabOrder = 3
    end
    object RadioButton2: TRadioButton
      Tag = 4
      Left = 12
      Top = 83
      Width = 113
      Height = 17
      Caption = 'QA'
      TabOrder = 2
    end
    object RadioButton3: TRadioButton
      Tag = 6
      Left = 12
      Top = 53
      Width = 97
      Height = 17
      Caption = 'Alpha'
      TabOrder = 1
    end
    object RadioButton4: TRadioButton
      Tag = 8
      Left = 12
      Top = 23
      Width = 113
      Height = 17
      Caption = 'Service'
      TabOrder = 0
    end
    object RadioButton5: TRadioButton
      Left = 12
      Top = 143
      Width = 93
      Height = 17
      Caption = 'Super User'
      TabOrder = 4
    end
  end
  object OkButton: TButton
    Left = 185
    Top = 18
    Width = 75
    Height = 25
    Caption = 'OK'
    Default = True
    TabOrder = 3
    OnClick = OkButtonClick
  end
  object CancelButton: TButton
    Left = 185
    Top = 49
    Width = 75
    Height = 25
    Caption = 'Cancel'
    TabOrder = 2
    OnClick = CancelButtonClick
  end
  object PasswordMaskEdit: TMaskEdit
    Left = 38
    Top = 20
    Width = 121
    Height = 21
    PasswordChar = '*'
    TabOrder = 1
  end
  object Timer1: TTimer
    Enabled = False
    Interval = 10
    OnTimer = Timer1Timer
    Left = 232
    Top = 240
  end
end
