object MotorsControlForm: TMotorsControlForm
  Left = 687
  Top = 318
  BorderStyle = bsDialog
  Caption = 'Motors Control'
  ClientHeight = 384
  ClientWidth = 510
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PopupMenu = PopupMenu1
  OnClose = FormClose
  OnCreate = FormCreate
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Bevel1: TBevel
    Left = 7
    Top = 9
    Width = 313
    Height = 368
  end
  object Bevel2: TBevel
    Left = 335
    Top = 9
    Width = 168
    Height = 368
  end
  object Label5: TLabel
    Left = 28
    Top = 48
    Width = 3
    Height = 13
  end
  object LockDoorButton: TSpeedButton
    Left = 15
    Top = 305
    Width = 90
    Height = 25
    Action = LockDoorAction
    AllowAllUp = True
    Layout = blGlyphRight
  end
  object Label9: TLabel
    Left = 132
    Top = 351
    Width = 27
    Height = 13
    Caption = 'Units:'
  end
  object PageControl1: TPageControl
    Tag = -1
    Left = 15
    Top = 25
    Width = 289
    Height = 264
    ActivePage = TAxisTabSheet
    MultiLine = True
    TabIndex = 3
    TabOrder = 0
    object XAxisTabSheet: TTabSheet
      Caption = 'X'
      object Label1: TLabel
        Left = 29
        Top = 47
        Width = 37
        Height = 13
        Caption = 'Position'
      end
      object Label10: TLabel
        Left = 16
        Top = 187
        Width = 64
        Height = 13
        Caption = 'HW Left Limit'
      end
      object Label14: TLabel
        Left = 160
        Top = 187
        Width = 71
        Height = 13
        Caption = 'HW Right Limit'
      end
      object Image1: TImage
        Left = 91
        Top = 187
        Width = 17
        Height = 17
        Transparent = True
      end
      object Image2: TImage
        Left = 243
        Top = 187
        Width = 17
        Height = 17
        Transparent = True
      end
      object XEnableCheckBox: TCheckBox
        Left = 11
        Top = 16
        Width = 62
        Height = 17
        Caption = 'Enable'
        TabOrder = 0
        OnClick = EnableCheckBoxClick
      end
      object XPositionPanel: TPanel
        Tag = -1
        Left = 11
        Top = 63
        Width = 73
        Height = 25
        BevelOuter = bvLowered
        Caption = '???'
        TabOrder = 1
		ParentBackground = False
      end
      object XHomeButton: TButton
        Tag = -1
        Left = 11
        Top = 101
        Width = 75
        Height = 25
        Caption = 'Home'
        Enabled = False
        TabOrder = 2
        OnClick = HomeButtonClick
      end
      object XMoveEdit: TEdit
        Tag = -1
        Left = 96
        Top = 146
        Width = 75
        Height = 21
        Enabled = False
        TabOrder = 3
        Text = '0'
      end
      object XAbsValueCheckBox: TCheckBox
        Tag = -1
        Left = 183
        Top = 148
        Width = 94
        Height = 17
        Caption = 'Absolute value'
        Enabled = False
        TabOrder = 4
      end
      object XMoveButton: TButton
        Tag = -1
        Left = 11
        Top = 144
        Width = 75
        Height = 25
        Caption = 'Move'
        Default = True
        Enabled = False
        TabOrder = 5
        OnClick = XMoveButtonClick
      end
      object XKillMotionButton: TButton
        Tag = -1
        Left = 96
        Top = 101
        Width = 75
        Height = 25
        Caption = 'Kill Motion'
        Enabled = False
        TabOrder = 6
        OnClick = KillMotionButtonClick
      end
      object XLoopButton: TButton
        Tag = -1
        Left = 183
        Top = 101
        Width = 75
        Height = 25
        Caption = 'X Loop'
        Enabled = False
        TabOrder = 7
        OnClick = XLoopButtonClick
      end
      object XStopButton: TButton
        Tag = -1
        Left = 96
        Top = 63
        Width = 73
        Height = 25
        Caption = 'Stop'
        TabOrder = 8
        OnClick = StopButtonClick
      end
      object EncoderButton: TButton
        Left = 176
        Top = 8
        Width = 82
        Height = 20
        Caption = 'Read Encoder'
        TabOrder = 9
        OnClick = EncoderButtonClick
      end
      object EncoderPanel: TPanel
        Tag = -1
        Left = 176
        Top = 31
        Width = 82
        Height = 20
        BevelOuter = bvLowered
        TabOrder = 10
		ParentBackground = False
      end
    end
    object YAxisTabSheet: TTabSheet
      Tag = 1
      Caption = 'Y'
      ImageIndex = 1
      object Label6: TLabel
        Left = 29
        Top = 47
        Width = 37
        Height = 13
        Caption = 'Position'
      end
      object Label11: TLabel
        Left = 16
        Top = 187
        Width = 71
        Height = 13
        Caption = 'HW Back Limit'
      end
      object Image3: TImage
        Left = 91
        Top = 187
        Width = 17
        Height = 17
        Transparent = True
      end
      object Label15: TLabel
        Left = 160
        Top = 187
        Width = 70
        Height = 13
        Caption = 'HW Front Limit'
      end
      object Image4: TImage
        Left = 243
        Top = 187
        Width = 17
        Height = 17
        Transparent = True
      end
      object YEnableCheckBox: TCheckBox
        Tag = 1
        Left = 11
        Top = 16
        Width = 97
        Height = 17
        Caption = 'Enable'
        TabOrder = 0
        OnClick = EnableCheckBoxClick
      end
      object YPositionPanel: TPanel
        Tag = -1
        Left = 11
        Top = 63
        Width = 73
        Height = 25
        BevelOuter = bvLowered
        Caption = '???'
        TabOrder = 1
		ParentBackground = False
      end
      object YHomeButton: TButton
        Tag = -1
        Left = 11
        Top = 101
        Width = 75
        Height = 25
        Caption = 'Home'
        Enabled = False
        TabOrder = 2
        OnClick = HomeButtonClick
      end
      object YKillMotionButton: TButton
        Tag = -1
        Left = 96
        Top = 101
        Width = 75
        Height = 25
        Caption = 'Kill Motion'
        Enabled = False
        TabOrder = 3
        OnClick = KillMotionButtonClick
      end
      object YMoveButton: TButton
        Tag = -1
        Left = 11
        Top = 144
        Width = 75
        Height = 25
        Caption = 'Move'
        Default = True
        Enabled = False
        TabOrder = 4
        OnClick = YMoveButtonClick
      end
      object YMoveEdit: TEdit
        Tag = -1
        Left = 96
        Top = 146
        Width = 75
        Height = 21
        Enabled = False
        TabOrder = 5
        Text = '0'
      end
      object YAbsValueCheckBox: TCheckBox
        Tag = -1
        Left = 183
        Top = 148
        Width = 94
        Height = 17
        Caption = 'Absolute value'
        Enabled = False
        TabOrder = 6
      end
      object YStopButton: TButton
        Tag = -1
        Left = 96
        Top = 63
        Width = 73
        Height = 25
        Caption = 'Stop'
        TabOrder = 7
        OnClick = StopButtonClick
      end
    end
    object ZAxisTabSheet: TTabSheet
      Tag = 2
      Caption = 'Z'
      ImageIndex = 2
      object Label7: TLabel
        Left = 29
        Top = 47
        Width = 37
        Height = 13
        Caption = 'Position'
      end
      object Label12: TLabel
        Left = 16
        Top = 206
        Width = 75
        Height = 13
        Caption = 'HW Lower Limit'
      end
      object Label16: TLabel
        Left = 16
        Top = 183
        Width = 75
        Height = 13
        Caption = 'HW Upper Limit'
      end
      object Image5: TImage
        Left = 97
        Top = 182
        Width = 17
        Height = 17
        Transparent = True
      end
      object Image6: TImage
        Left = 97
        Top = 206
        Width = 17
        Height = 17
        Transparent = True
      end
      object ZEnableCheckBox: TCheckBox
        Tag = 2
        Left = 11
        Top = 16
        Width = 70
        Height = 17
        Caption = 'Enable'
        TabOrder = 0
        OnClick = EnableCheckBoxClick
      end
      object ZPositionPanel: TPanel
        Tag = -1
        Left = 11
        Top = 63
        Width = 73
        Height = 25
        BevelOuter = bvLowered
        Caption = '???'
        TabOrder = 1
		ParentBackground = False
      end
      object ZHomeButton: TButton
        Tag = -1
        Left = 11
        Top = 101
        Width = 75
        Height = 25
        Caption = 'Home'
        Enabled = False
        TabOrder = 2
        OnClick = HomeButtonClick
      end
      object ZKillMotionButton: TButton
        Tag = -1
        Left = 96
        Top = 101
        Width = 75
        Height = 25
        Caption = 'Kill Motion'
        Enabled = False
        TabOrder = 3
        OnClick = KillMotionButtonClick
      end
      object ZMoveButton: TButton
        Tag = -1
        Left = 11
        Top = 144
        Width = 75
        Height = 25
        Caption = 'Move'
        Default = True
        Enabled = False
        TabOrder = 4
        OnClick = ZMoveButtonClick
      end
      object ZMoveEdit: TEdit
        Tag = -1
        Left = 96
        Top = 146
        Width = 75
        Height = 21
        Enabled = False
        TabOrder = 5
        Text = '0'
      end
      object ZAbsValueCheckBox: TCheckBox
        Tag = -1
        Left = 183
        Top = 148
        Width = 94
        Height = 17
        Caption = 'Absolute value'
        Enabled = False
        TabOrder = 6
      end
      object TrayOutButton: TButton
        Tag = -1
        Left = 186
        Top = 63
        Width = 75
        Height = 25
        Caption = 'Go to Tray Out'
        Enabled = False
        TabOrder = 7
        OnClick = TrayOutButtonClick
      end
      object ZStopButton: TButton
        Tag = -1
        Left = 96
        Top = 63
        Width = 73
        Height = 25
        Caption = 'Stop'
        TabOrder = 8
        OnClick = StopButtonClick
      end
      object GoZDownButton: TButton
        Tag = -1
        Left = 186
        Top = 101
        Width = 75
        Height = 25
        Caption = 'Go Z Down'
        TabOrder = 9
        OnClick = GoZDownButtonClick
      end
      object ZStartGroupBox: TGroupBox
        Tag = -1
        Left = 96
        Top = 5
        Width = 165
        Height = 49
        Caption = 'ZStart'
        TabOrder = 10
      end
      object GoToZStartPositionButton1: TButton
        Tag = -1
        Left = 152
        Top = 24
        Width = 40
        Height = 20
        Caption = 'Goto'
        TabOrder = 11
        OnClick = ZStartButtonClick
      end
      object SaveZStartPositionButton: TButton
        Tag = -1
        Left = 104
        Top = 24
        Width = 40
        Height = 20
        Caption = 'Save'
        TabOrder = 12
        OnClick = SaveButtonClick
      end
      object ZSaveStartEdit: TEdit
        Tag = -1
        Left = 200
        Top = 24
        Width = 52
        Height = 21
        TabOrder = 13
      end
    end
    object TAxisTabSheet: TTabSheet
      Tag = 3
      Caption = 'T'
      ImageIndex = 3
      object Label8: TLabel
        Left = 29
        Top = 47
        Width = 37
        Height = 13
        Caption = 'Position'
      end
      object Label13: TLabel
        Left = 16
        Top = 187
        Width = 64
        Height = 13
        Caption = 'HW Left Limit'
      end
      object Image7: TImage
        Left = 91
        Top = 187
        Width = 17
        Height = 17
        Transparent = True
      end
      object Label17: TLabel
        Left = 160
        Top = 187
        Width = 71
        Height = 13
        Caption = 'HW Right Limit'
      end
      object Image8: TImage
        Left = 243
        Top = 187
        Width = 17
        Height = 17
        Transparent = True
      end
      object TEnableCheckBox: TCheckBox
        Tag = 3
        Left = 11
        Top = 16
        Width = 97
        Height = 17
        Caption = 'Enable'
        TabOrder = 0
        OnClick = EnableCheckBoxClick
      end
      object TPositionPanel: TPanel
        Tag = -1
        Left = 11
        Top = 63
        Width = 73
        Height = 25
        BevelOuter = bvLowered
        Caption = '???'
        TabOrder = 1
		ParentBackground = False
      end
      object THomeButton: TButton
        Tag = -1
        Left = 11
        Top = 101
        Width = 75
        Height = 25
        Caption = 'Home'
        Enabled = False
        TabOrder = 2
        OnClick = HomeButtonClick
      end
      object TKillMotionButton: TButton
        Tag = -1
        Left = 96
        Top = 101
        Width = 75
        Height = 25
        Caption = 'Kill Motion'
        Enabled = False
        TabOrder = 3
        OnClick = KillMotionButtonClick
      end
      object TMoveButton: TButton
        Tag = -1
        Left = 11
        Top = 144
        Width = 75
        Height = 25
        Caption = 'Move'
        Default = True
        Enabled = False
        TabOrder = 4
        OnClick = TMoveButtonClick
      end
      object TMoveEdit: TEdit
        Tag = -1
        Left = 96
        Top = 146
        Width = 75
        Height = 21
        Enabled = False
        TabOrder = 5
        Text = '0'
      end
      object TAbsValueCheckBox: TCheckBox
        Tag = -1
        Left = 183
        Top = 148
        Width = 94
        Height = 17
        Caption = 'Absolute value'
        Enabled = False
        TabOrder = 6
      end
      object TWipePositionButton: TButton
        Tag = -1
        Left = 184
        Top = 101
        Width = 75
        Height = 25
        Caption = 'Go to Wipe'
        Enabled = False
        TabOrder = 7
        OnClick = TWipePositionButtonClick
      end
      object TPurgePositionButton: TButton
        Tag = -1
        Left = 184
        Top = 63
        Width = 75
        Height = 26
        Caption = 'Go to Purge'
        Enabled = False
        TabOrder = 8
        OnClick = TPurgePositionButtonClick
      end
      object TStopButton: TButton
        Tag = -1
        Left = 96
        Top = 63
        Width = 73
        Height = 25
        Caption = 'Stop'
        TabOrder = 9
        OnClick = StopButtonClick
      end
    end
  end
  object CloseBitBtn: TBitBtn
    Left = 15
    Top = 345
    Width = 90
    Height = 25
    Cancel = True
    Caption = 'Close'
    TabOrder = 1
    OnClick = CloseBitBtnClick
    NumGlyphs = 2
  end
  object RemoteControlButton: TButton
    Left = 277
    Top = 345
    Width = 27
    Height = 25
    Action = ShowRemoteControlAction
    TabOrder = 2
  end
  object KillAllMotionButton: TButton
    Left = 113
    Top = 305
    Width = 90
    Height = 25
    Caption = 'Kill All Motion'
    TabOrder = 3
    OnClick = KillAllMotionButtonClick
  end
  object HomeAllButton: TButton
    Left = 214
    Top = 305
    Width = 90
    Height = 25
    Caption = 'Home All'
    TabOrder = 4
    OnClick = HomeAllButtonClick
  end
  object UnitsComboBox: TComboBox
    Left = 164
    Top = 347
    Width = 75
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    ItemIndex = 0
    TabOrder = 5
    Text = 'Steps'
    OnChange = UnitsComboBoxChange
    Items.Strings = (
      'Steps'
      'Millimeters'
      'Inches')
  end
  inline TZAxisFrame1: TZAxisFrame
    Left = 351
    Top = 218
    Width = 137
    Height = 125
    AutoSize = True
    TabOrder = 6
    inherited UpDownAxisFrame: TUpDownAxisFrame
      Height = 125
      inherited MainPanel: TPanel
        Height = 125
        inherited SlowModeCheckBox: TCheckBox
          Top = 219
          Visible = False
        end
      end
    end
  end
  inline TXYAxisFrame1: TXYAxisFrame
    Left = 351
    Top = 48
    Width = 137
    Height = 125
    AutoScroll = False
    AutoSize = True
    TabOrder = 7
    inherited Panel1: TPanel
      Height = 125
      inherited TYAxisFrame1: TYAxisFrame
        Top = 1
        Height = 123
        inherited UpDownAxisFrame: TUpDownAxisFrame
          Height = 123
          inherited MainPanel: TPanel
            Height = 123
          end
        end
      end
    end
    inherited SlowModeCheckBox: TCheckBox
      Top = 250
      Visible = False
    end
  end
  object SlowModeCheckBox: TCheckBox
    Left = 351
    Top = 25
    Width = 77
    Height = 17
    Caption = 'Slow Mode'
    TabOrder = 8
    OnClick = SlowModeCheckBoxClick
  end
  object ActionList1: TActionList
    Left = 257
    Top = 17
    object ShowRemoteControlAction: TAction
      Category = 'RemoteControl'
      Caption = '>>'
      OnExecute = ShowRemoteControlActionExecute
    end
    object HideRemoteControlAction: TAction
      Category = 'RemoteControl'
      Caption = '<<'
      OnExecute = HideRemoteControlActionExecute
    end
    object LockDoorAction: TAction
      Category = 'Door'
      Caption = 'Lock Door'
      ImageIndex = 0
      OnExecute = LockDoorActionExecute
    end
    object UnlockDoorAction: TAction
      Category = 'Door'
      Caption = 'Unlock Door'
      ImageIndex = 1
      OnExecute = UnlockDoorActionExecute
    end
    object AlwaysOnTopAction: TAction
      Caption = 'Always on top'
      OnExecute = AlwaysOnTopActionExecute
    end
    object CloseDialogAction: TAction
      Caption = 'Close'
      OnExecute = CloseDialogActionExecute
    end
  end
  object PopupMenu1: TPopupMenu
    Left = 329
    Top = 233
    object Alwaysontop1: TMenuItem
      Action = AlwaysOnTopAction
    end
    object N1: TMenuItem
      Caption = '-'
    end
    object Close1: TMenuItem
      Action = CloseDialogAction
    end
  end
  object Timer1: TTimer
    OnTimer = Timer1Timer
    Left = 224
    Top = 16
  end
end
