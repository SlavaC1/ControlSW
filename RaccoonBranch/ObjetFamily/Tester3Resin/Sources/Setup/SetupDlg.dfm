object SetupForm: TSetupForm
  Left = 2
  Top = 54
  Caption = 'System Setup'
  ClientHeight = 540
  ClientWidth = 990
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object GroupBox1: TGroupBox
    Left = 8
    Top = 16
    Width = 225
    Height = 401
    Caption = 'General Settings'
    TabOrder = 0
    object Date: TLabel
      Left = 16
      Top = 40
      Width = 23
      Height = 13
      Caption = 'Date'
    end
    object Time: TLabel
      Left = 16
      Top = 81
      Width = 23
      Height = 13
      Caption = 'Time'
    end
    object Label1: TLabel
      Left = 16
      Top = 122
      Width = 72
      Height = 13
      Caption = 'Operator Name'
    end
    object Label2: TLabel
      Left = 16
      Top = 163
      Width = 57
      Height = 13
      Caption = 'System S/N'
    end
    object Label3: TLabel
      Left = 16
      Top = 204
      Width = 73
      Height = 13
      Caption = 'Containers S/N'
    end
    object Label4: TLabel
      Left = 16
      Top = 245
      Width = 44
      Height = 13
      Caption = 'LCD S/N'
    end
    object Label5: TLabel
      Left = 16
      Top = 286
      Width = 46
      Height = 13
      Caption = 'MCB S/N'
    end
    object Label6: TLabel
      Left = 16
      Top = 327
      Width = 45
      Height = 13
      Caption = 'OCB S/N'
    end
    object Label7: TLabel
      Left = 16
      Top = 368
      Width = 54
      Height = 13
      Caption = 'OHDB S/N'
    end
    object DateMaskEdit: TMaskEdit
      Left = 104
      Top = 36
      Width = 105
      Height = 21
      EditMask = '!99/99/00;1;_'
      MaxLength = 8
      TabOrder = 0
      Text = '  /  /  '
    end
    object TimeMaskEdit: TMaskEdit
      Left = 104
      Top = 77
      Width = 105
      Height = 21
      EditMask = '!90:00;1;_'
      MaxLength = 5
      TabOrder = 1
      Text = '  :  '
    end
    object OperNameEdit: TEdit
      Left = 104
      Top = 118
      Width = 105
      Height = 21
      TabOrder = 2
    end
    object System_SNEdit: TEdit
      Left = 104
      Top = 159
      Width = 105
      Height = 21
      TabOrder = 3
    end
    object Containers_SNEdit: TEdit
      Left = 104
      Top = 200
      Width = 105
      Height = 21
      TabOrder = 4
    end
    object LCD_SNEdit: TEdit
      Left = 104
      Top = 241
      Width = 105
      Height = 21
      TabOrder = 5
    end
    object MCB_SNEdit: TEdit
      Left = 104
      Top = 282
      Width = 105
      Height = 21
      TabOrder = 6
    end
    object OCB_SNEdit: TEdit
      Left = 104
      Top = 323
      Width = 105
      Height = 21
      TabOrder = 7
    end
    object OHDB_SNEdit: TEdit
      Left = 104
      Top = 364
      Width = 105
      Height = 21
      TabOrder = 8
    end
  end
  object OKBitBtn: TBitBtn
    Left = 520
    Top = 480
    Width = 89
    Height = 33
    TabOrder = 1
    OnClick = OKBitBtnClick
    Kind = bkOK
  end
  object GroupBox7: TGroupBox
    Left = 496
    Top = 16
    Width = 233
    Height = 121
    Caption = 'Containers'
    TabOrder = 2
    object ContainersComboBox: TComboBox
      Left = 152
      Top = 48
      Width = 73
      Height = 21
      Style = csDropDownList
      ItemHeight = 0
      TabOrder = 0
      OnDropDown = ComComboBoxDropDown
    end
    object ContainersRadioGroup: TRadioGroup
      Left = 8
      Top = 16
      Width = 137
      Height = 81
      ItemIndex = 0
      Items.Strings = (
        'None'
        'Direct to'
        'Throogh Embedded')
      TabOrder = 1
      OnClick = ContainersRadioGroupClick
    end
  end
  object GroupBox2: TGroupBox
    Left = 496
    Top = 152
    Width = 233
    Height = 121
    Caption = 'LCD/Keyboard'
    TabOrder = 3
    object LCDComboBox: TComboBox
      Left = 152
      Top = 48
      Width = 73
      Height = 21
      Style = csDropDownList
      ItemHeight = 0
      TabOrder = 0
      OnDropDown = ComComboBoxDropDown
    end
    object LCDRadioGroup: TRadioGroup
      Left = 8
      Top = 16
      Width = 137
      Height = 81
      ItemIndex = 0
      Items.Strings = (
        'None'
        'Direct to'
        'Throogh Embedded')
      TabOrder = 1
      OnClick = LCDRadioGroupClick
    end
  end
  object GroupBox3: TGroupBox
    Left = 248
    Top = 152
    Width = 233
    Height = 121
    Caption = 'MCB'
    TabOrder = 4
    object MCBComboBox: TComboBox
      Left = 152
      Top = 48
      Width = 73
      Height = 21
      Style = csDropDownList
      ItemHeight = 0
      TabOrder = 0
      OnDropDown = ComComboBoxDropDown
    end
    object MCBRadioGroup: TRadioGroup
      Left = 8
      Top = 16
      Width = 137
      Height = 81
      ItemIndex = 0
      Items.Strings = (
        'None'
        'Direct to'
        'Throogh Embedded')
      TabOrder = 1
      OnClick = MCBRadioGroupClick
    end
  end
  object GroupBox4: TGroupBox
    Left = 744
    Top = 16
    Width = 233
    Height = 121
    Caption = 'OCB'
    TabOrder = 5
    object OCBComboBox: TComboBox
      Left = 152
      Top = 49
      Width = 73
      Height = 21
      Style = csDropDownList
      ItemHeight = 0
      TabOrder = 0
      OnDropDown = ComComboBoxDropDown
    end
    object OCBRadioGroup: TRadioGroup
      Left = 8
      Top = 16
      Width = 137
      Height = 81
      ItemIndex = 0
      Items.Strings = (
        'None'
        'Direct to'
        'Throogh Embedded')
      TabOrder = 1
      OnClick = OCBRadioGroupClick
    end
  end
  object GroupBox5: TGroupBox
    Left = 744
    Top = 152
    Width = 233
    Height = 121
    Caption = 'OHDB'
    TabOrder = 6
    object OHDBComboBox: TComboBox
      Left = 152
      Top = 48
      Width = 73
      Height = 21
      Style = csDropDownList
      ItemHeight = 0
      TabOrder = 0
      OnDropDown = ComComboBoxDropDown
    end
    object OHDBRadioGroup: TRadioGroup
      Left = 8
      Top = 16
      Width = 137
      Height = 97
      ItemIndex = 0
      Items.Strings = (
        'None'
        'Direct to'
        'Throogh Embedded'
        'Through OCB')
      TabOrder = 1
      OnClick = OHDBRadioGroupClick
    end
  end
  object GroupBox6: TGroupBox
    Left = 248
    Top = 16
    Width = 233
    Height = 121
    Caption = 'Embedded'
    TabOrder = 7
    object EmbeddedComboBox: TComboBox
      Left = 152
      Top = 68
      Width = 73
      Height = 21
      Style = csDropDownList
      ItemHeight = 0
      TabOrder = 0
      OnDropDown = ComComboBoxDropDown
    end
    object EmbeddedRadioGroup: TRadioGroup
      Left = 8
      Top = 16
      Width = 137
      Height = 81
      ItemIndex = 0
      Items.Strings = (
        'None'
        'Direct to')
      TabOrder = 1
      OnClick = EmbeddedRadioGroupClick
    end
  end
  object BitBtn1: TBitBtn
    Left = 400
    Top = 480
    Width = 89
    Height = 33
    TabOrder = 8
    Kind = bkCancel
  end
  object GroupBox8: TGroupBox
    Left = 248
    Top = 280
    Width = 233
    Height = 121
    Caption = 'OCB Simulator'
    TabOrder = 9
    object OCBSimComboBox: TComboBox
      Left = 152
      Top = 68
      Width = 73
      Height = 21
      Style = csDropDownList
      ItemHeight = 0
      TabOrder = 0
      OnDropDown = ComComboBoxDropDown
    end
    object OCBSimRadioGroup: TRadioGroup
      Left = 8
      Top = 16
      Width = 137
      Height = 81
      ItemIndex = 0
      Items.Strings = (
        'None'
        'Direct to')
      TabOrder = 1
      OnClick = OCBSimRadioGroupClick
    end
  end
  object GroupBox9: TGroupBox
    Left = 744
    Top = 288
    Width = 233
    Height = 89
    Caption = 'RFID'
    TabOrder = 10
    object RFIDRadioGroup: TRadioGroup
      Left = 8
      Top = 16
      Width = 137
      Height = 57
      ItemIndex = 0
      Items.Strings = (
        'None'
        'Direct to')
      TabOrder = 0
      OnClick = RFIDRadioGroupClick
    end
    object RFIDComboBox: TComboBox
      Left = 152
      Top = 48
      Width = 73
      Height = 21
      ItemHeight = 13
      TabOrder = 1
      OnDropDown = ComComboBoxDropDown
    end
  end
end
