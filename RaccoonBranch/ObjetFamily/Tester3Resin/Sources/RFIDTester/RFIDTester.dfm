object RFIDForm: TRFIDForm
  Left = 639
  Top = 105
  Caption = 'RFIDForm'
  ClientHeight = 578
  ClientWidth = 604
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  PixelsPerInch = 96
  TextHeight = 13
  object Label4: TLabel
    Left = 400
    Top = 148
    Width = 32
    Height = 13
    Caption = 'Label4'
  end
  object Button2: TButton
    Left = 112
    Top = 552
    Width = 99
    Height = 21
    Caption = 'Close'
    TabOrder = 0
    OnClick = Button2Click
  end
  object PageControl1: TPageControl
    Left = 3
    Top = 0
    Width = 593
    Height = 537
    ActivePage = TabSheet1
    TabOrder = 1
    object TabSheet1: TTabSheet
      Caption = 'Tag Init'
      object GroupBox1: TGroupBox
        Left = 8
        Top = 8
        Width = 281
        Height = 70
        Caption = 'Tag ID'
        TabOrder = 0
        object Label1: TLabel
          Left = 8
          Top = 18
          Width = 144
          Height = 13
          Caption = 'Set Channel && Get the Tag ID:'
        end
        object RFChannelBox: TComboBox
          Left = 8
          Top = 36
          Width = 145
          Height = 21
          ItemHeight = 13
          TabOrder = 0
          Text = 'Select Channel'
          OnChange = RFChannelBoxChange
          Items.Strings = (
            'Support 1'
            'Support 2'
            'Model 1 left'
            'Model 1 right'
            'model 2 left'
            'model 2 right'
            'model 3 left'
            'model 3 right')
        end
        object TagIDPanel: TPanel
          Left = 176
          Top = 12
          Width = 97
          Height = 21
          BevelOuter = bvLowered
          Caption = 'Tag ID'
          TabOrder = 1
        end
        object GetTagButton: TButton
          Left = 176
          Top = 36
          Width = 97
          Height = 21
          Caption = 'Get Tag'
          TabOrder = 2
          OnClick = GetTagButtonClick
        end
      end
      object GroupBox2: TGroupBox
        Left = 8
        Top = 80
        Width = 281
        Height = 70
        Caption = 'Material'
        TabOrder = 1
        object Label2: TLabel
          Left = 8
          Top = 20
          Width = 190
          Height = 13
          Caption = 'Setting the wanted Material Type to Tag'
        end
        object MaterialTypeBox: TComboBox
          Left = 8
          Top = 36
          Width = 145
          Height = 21
          ItemHeight = 13
          TabOrder = 0
          Text = 'Select Material Type'
          OnChange = MaterialTypeBoxChange
          Items.Strings = (
            'FullCure980'
            'FullCure930'
            'FullCure720'
            'FullCure630'
            'FullCure640'
            'FullCure660 (RoseClear)'
            'FullCure680 (SkinTone)'
            'FullCure430'
            'VeroGrey'
            'VeroBlack'
            'VeroBlue'
            'VeroWhite'
            'VeroClear'
            'VeroMgnt'
            'VeroCyan'
            'VeroYellow'
            'TangoBlack'
            'TangoGrey'
            'RGD515'
            'RGD525'
            'RGD535'
            'RGD531 (White RF)'
            'RGD545 (Epoxy)'
            'RGD546 (Acrylic)'
            'FullCure705'
            'MCleanser'
            'SCleanser')
        end
        object SetMaterialTypeButton: TButton
          Left = 176
          Top = 36
          Width = 97
          Height = 21
          Caption = 'Set Material Type'
          TabOrder = 1
          OnClick = SetMaterialTypeButtonClick
        end
        object MaterialIndex: TEdit
          Left = 216
          Top = 11
          Width = 57
          Height = 21
          TabOrder = 2
          OnChange = MaterialIndexChange
        end
      end
      object SecurityGroup: TGroupBox
        Left = 301
        Top = 80
        Width = 281
        Height = 70
        Caption = 'SecurityGroup'
        TabOrder = 2
        object Label6: TLabel
          Left = 8
          Top = 20
          Width = 89
          Height = 13
          Caption = 'Setting Pump Time'
        end
        object PumpTimeEdit: TEdit
          Left = 17
          Top = 36
          Width = 131
          Height = 21
          TabOrder = 0
          Text = 'Pump Time'
        end
        object SetPumpTimeButton: TButton
          Left = 176
          Top = 36
          Width = 97
          Height = 21
          Caption = 'Set Pump Time'
          TabOrder = 1
          OnClick = SetPumpTimeButtonClick
        end
        object ResetPumpTimeButton: TButton
          Left = 176
          Top = 12
          Width = 97
          Height = 21
          Caption = 'Reset Pump Time'
          TabOrder = 2
          OnClick = ResetPumpTimeButtonClick
        end
      end
      object GroupBox4: TGroupBox
        Left = 8
        Top = 224
        Width = 281
        Height = 70
        Caption = 'Expiration'
        TabOrder = 3
        object Label3: TLabel
          Left = 9
          Top = 20
          Width = 156
          Height = 13
          Caption = 'Setting the Resin Expiration Date'
        end
        object ExpirationDateEdit: TEdit
          Left = 9
          Top = 36
          Width = 143
          Height = 21
          TabOrder = 0
          Text = 'DD/MM/YY (e.g: 28/09/06)'
        end
        object SetExpirationButton: TButton
          Left = 177
          Top = 36
          Width = 97
          Height = 21
          Caption = 'Set Expiration Date'
          TabOrder = 1
          OnClick = SetExpirationButtonClick
        end
        object DatePlusTimeCheck: TCheckBox
          Left = 195
          Top = 16
          Width = 81
          Height = 17
          Caption = 'Date + Time'
          TabOrder = 2
        end
      end
      object GroupBox5: TGroupBox
        Left = 8
        Top = 300
        Width = 281
        Height = 193
        Caption = 'Read Tag'
        TabOrder = 4
        object ReadTagButton: TButton
          Left = 86
          Top = 163
          Width = 99
          Height = 21
          Caption = 'Read Tag'
          TabOrder = 0
          OnClick = ReadTagButtonClick
        end
        object TagDataMemo: TMemo
          Left = 8
          Top = 16
          Width = 265
          Height = 139
          Lines.Strings = (
            'TagDataMemo')
          ScrollBars = ssVertical
          TabOrder = 1
        end
      end
      object InitialWeightGroup: TGroupBox
        Left = 301
        Top = 300
        Width = 281
        Height = 70
        Caption = 'Initial Weight'
        TabOrder = 5
        object SettingWeightLabel: TLabel
          Left = 9
          Top = 20
          Width = 143
          Height = 13
          Caption = 'Setting Tank Initial Weight (gr)'
        end
        object SetWeightButton: TButton
          Left = 177
          Top = 37
          Width = 97
          Height = 21
          Caption = 'Set Weight'
          TabOrder = 0
          OnClick = SetWeightButtonClick
        end
        object InitialWeightEdit: TEdit
          Left = 17
          Top = 37
          Width = 131
          Height = 21
          TabOrder = 1
          Text = '3600'
        end
      end
      object CurrentWeightGroupBox: TGroupBox
        Left = 8
        Top = 152
        Width = 281
        Height = 70
        Caption = 'Current Weight'
        TabOrder = 6
        object Label9: TLabel
          Left = 8
          Top = 20
          Width = 152
          Height = 13
          Caption = 'Setting the tank'#39's current weight'
        end
        object SetCurrentWeightBtn: TButton
          Left = 176
          Top = 36
          Width = 98
          Height = 21
          Caption = 'Set Current Weight'
          TabOrder = 0
          OnClick = SetCurrentWeightBtnClick
        end
        object CurrentWeightEdit: TEdit
          Left = 9
          Top = 36
          Width = 143
          Height = 21
          TabOrder = 1
          Text = '3600'
        end
      end
      object GroupBox3: TGroupBox
        Left = 301
        Top = 8
        Width = 281
        Height = 70
        Caption = 'Password'
        Color = clBtnFace
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentColor = False
        ParentFont = False
        TabOrder = 7
        object IncorrectPass: TLabel
          Left = 22
          Top = 16
          Width = 90
          Height = 13
          AutoSize = False
          Caption = 'Incorrect password'
          Color = clBtnFace
          Constraints.MaxHeight = 100
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clRed
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentColor = False
          ParentFont = False
          Transparent = False
          Visible = False
        end
        object EnterPassword: TButton
          Left = 176
          Top = 36
          Width = 98
          Height = 21
          Caption = 'Enter password'
          TabOrder = 0
          OnClick = EnterPasswordClick
        end
      end
      object PasswordMaskEdit: TMaskEdit
        Left = 318
        Top = 44
        Width = 131
        Height = 21
        PasswordChar = '*'
        TabOrder = 8
      end
      object ErrorCodeGroup: TGroupBox
        Left = 301
        Top = 224
        Width = 281
        Height = 70
        Caption = 'Error code'
        TabOrder = 9
        object Label8: TLabel
          Left = 8
          Top = 18
          Width = 138
          Height = 13
          Caption = 'Set refill protection error code'
        end
        object ErrorCodeBtn: TButton
          Left = 176
          Top = 36
          Width = 97
          Height = 21
          Caption = 'Set Error Code'
          TabOrder = 0
          OnClick = ErrorCodeBtnClick
        end
        object ErrorCodeEdit: TEdit
          Left = 17
          Top = 36
          Width = 131
          Height = 21
          TabOrder = 1
          Text = '0'
        end
      end
      object ProtocolNumberGroup: TGroupBox
        Left = 301
        Top = 152
        Width = 281
        Height = 70
        Caption = 'Protocol number'
        TabOrder = 10
        object Label10: TLabel
          Left = 8
          Top = 18
          Width = 121
          Height = 13
          Caption = 'Set RF tag version (1,2...)'
        end
        object TagVersionBtn: TButton
          Left = 176
          Top = 36
          Width = 97
          Height = 21
          Caption = 'Set Tag Version'
          TabOrder = 0
          OnClick = TagVersionBtnClick
        end
        object TagVersionEdit: TEdit
          Left = 17
          Top = 36
          Width = 131
          Height = 21
          TabOrder = 1
          Text = '0'
        end
      end
    end
    object TabSheet2: TTabSheet
      Caption = 'Read Write'
      ImageIndex = 1
      object Label5: TLabel
        Left = 8
        Top = 56
        Width = 27
        Height = 13
        Caption = 'ASCII'
      end
      object Label7: TLabel
        Left = 184
        Top = 56
        Width = 22
        Height = 13
        Caption = 'HEX'
      end
      object TagDataAsciiMemo: TMemo
        Left = 8
        Top = 72
        Width = 169
        Height = 249
        Lines.Strings = (
          'TagDataAsciiMemo')
        TabOrder = 0
      end
      object TagDataHexMemo: TMemo
        Left = 184
        Top = 72
        Width = 185
        Height = 249
        Lines.Strings = (
          'TagDataHexMemo')
        TabOrder = 1
      end
      object ReadTag: TButton
        Left = 144
        Top = 344
        Width = 75
        Height = 25
        Caption = 'Read Tag'
        TabOrder = 2
        OnClick = ReadTagClick
      end
    end
  end
end
