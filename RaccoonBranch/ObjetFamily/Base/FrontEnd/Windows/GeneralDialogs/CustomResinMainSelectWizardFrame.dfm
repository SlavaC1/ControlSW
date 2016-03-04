object CustomResinMainSelectWizardFrame: TCustomResinMainSelectWizardFrame
  Left = 0
  Top = 0
  Width = 574
  Height = 415
  Padding.Left = 5
  Padding.Top = 5
  Padding.Right = 5
  Padding.Bottom = 5
  TabOrder = 0
  object lblTitle: TLabel
    Left = 5
    Top = 5
    Width = 564
    Height = 25
    Align = alTop
    Alignment = taCenter
    AutoSize = False
    Caption = 'Material Replacement Options'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clBlack
    Font.Height = -16
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    ExplicitLeft = 0
    ExplicitTop = 7
    ExplicitWidth = 571
  end
  object grpPrintingMode: TGroupBox
    Left = 5
    Top = 30
    Width = 564
    Height = 81
    Align = alTop
    Caption = 'Printer Configuration'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 4
    object pnlPrintingMode: TPanel
      Left = 2
      Top = 15
      Width = 560
      Height = 64
      Align = alClient
      BevelOuter = bvNone
      TabOrder = 0
      object rbSingleMode: TRadioButton
        Left = 10
        Top = 1
        Width = 233
        Height = 25
        Caption = 'Single material (HQ, HS)'
        Checked = True
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Microsoft Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 0
        TabStop = True
        OnClick = OnClickSingleMode
      end
      object rbMultipleMode: TRadioButton
        Tag = 1
        Left = 10
        Top = 32
        Width = 233
        Height = 25
        Caption = 'Multiple materials (DM)'
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Microsoft Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 1
        TabStop = True
        OnClick = OnClickMultipleMode
      end
    end
  end
  object grpRequiredMaterials: TGroupBox
    AlignWithMargins = True
    Left = 5
    Top = 114
    Width = 564
    Height = 162
    Margins.Left = 0
    Margins.Top = 0
    Margins.Right = 0
    Margins.Bottom = 0
    Align = alCustom
    Caption = 'Required Model Materials'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 1
    object lblWarningAuto: TLabel
      Left = 11
      Top = 14
      Width = 401
      Height = 15
      Caption = 
        'Note: Cartidge positioning in material cabinet is determined aut' +
        'omatically.'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Microsoft Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object imgWarning: TImage
      Left = 500
      Top = 104
      Width = 48
      Height = 48
      Transparent = True
      Visible = False
    end
    object bvlResins12: TBevel
      Left = 183
      Top = 44
      Width = 2
      Height = 75
    end
    object bvlResins23: TBevel
      Left = 375
      Top = 44
      Width = 2
      Height = 75
    end
    object pnlResinBothCarts: TPanel
      Left = 8
      Top = 75
      Width = 540
      Height = 23
      BevelOuter = bvNone
      TabOrder = 2
      object chkBothCartResin1: TCheckBox
        Left = 4
        Top = 3
        Width = 105
        Height = 17
        Caption = 'Both cartridges'
        Checked = True
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Microsoft Sans Serif'
        Font.Style = []
        ParentFont = False
        State = cbChecked
        TabOrder = 3
      end
      object chkBothCartResin2: TCheckBox
        Tag = 1
        Left = 196
        Top = 3
        Width = 105
        Height = 17
        Caption = 'Both cartridges'
        Checked = True
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Microsoft Sans Serif'
        Font.Style = []
        ParentFont = False
        State = cbChecked
        TabOrder = 4
      end
      object chkBothCartResin3: TCheckBox
        Tag = 2
        Left = 388
        Top = 3
        Width = 105
        Height = 17
        Caption = 'Both cartridges'
        Checked = True
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Microsoft Sans Serif'
        Font.Style = []
        ParentFont = False
        State = cbChecked
        TabOrder = 5
      end
      object comboLeftRightCart1: TComboBox
        Left = 107
        Top = 0
        Width = 49
        Height = 23
        Style = csDropDownList
        DropDownCount = 2
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Microsoft Sans Serif'
        Font.Style = []
        ItemHeight = 15
        ItemIndex = 0
        ParentFont = False
        TabOrder = 0
        TabStop = False
        Text = 'L'
        Visible = False
        Items.Strings = (
          'L'
          'R')
      end
      object comboLeftRightCart2: TComboBox
        Tag = 1
        Left = 299
        Top = 0
        Width = 49
        Height = 23
        Style = csDropDownList
        DropDownCount = 2
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Microsoft Sans Serif'
        Font.Style = []
        ItemHeight = 15
        ItemIndex = 0
        ParentFont = False
        TabOrder = 1
        TabStop = False
        Text = 'L'
        Visible = False
        Items.Strings = (
          'L'
          'R')
      end
      object comboLeftRightCart3: TComboBox
        Tag = 2
        Left = 491
        Top = 0
        Width = 49
        Height = 23
        Style = csDropDownList
        DropDownCount = 2
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Microsoft Sans Serif'
        Font.Style = []
        ItemHeight = 15
        ItemIndex = 0
        ParentFont = False
        TabOrder = 2
        TabStop = False
        Text = 'L'
        Visible = False
        Items.Strings = (
          'L'
          'R')
      end
    end
    object pnlResinChooser: TPanel
      Left = 8
      Top = 46
      Width = 545
      Height = 23
      BevelOuter = bvNone
      TabOrder = 1
      object comboResins1: TComboBox
        Left = 4
        Top = 0
        Width = 152
        Height = 23
        Style = csDropDownList
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Microsoft Sans Serif'
        Font.Style = []
        ItemHeight = 15
        ParentFont = False
        TabOrder = 0
      end
      object comboResins2: TComboBox
        Tag = 1
        Left = 196
        Top = 0
        Width = 152
        Height = 23
        Style = csDropDownList
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Microsoft Sans Serif'
        Font.Style = []
        ItemHeight = 15
        ParentFont = False
        TabOrder = 1
      end
      object comboResins3: TComboBox
        Tag = 2
        Left = 388
        Top = 0
        Width = 152
        Height = 23
        Style = csDropDownList
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Microsoft Sans Serif'
        Font.Style = []
        ItemHeight = 15
        ParentFont = False
        TabOrder = 2
      end
    end
    object pnlResinChooserCaptions: TPanel
      Left = 10
      Top = 30
      Width = 505
      Height = 13
      BevelOuter = bvNone
      TabOrder = 0
      object lblResins1: TLabel
        Left = 1
        Top = 0
        Width = 56
        Height = 13
        Alignment = taCenter
        Caption = 'Material 1'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clMenuHighlight
        Font.Height = -11
        Font.Name = 'Microsoft Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object lblResins2: TLabel
        Tag = 1
        Left = 194
        Top = 0
        Width = 56
        Height = 13
        Alignment = taCenter
        Caption = 'Material 2'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clMenuHighlight
        Font.Height = -11
        Font.Name = 'Microsoft Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object lblResins3: TLabel
        Tag = 2
        Left = 386
        Top = 0
        Width = 56
        Height = 13
        Alignment = taCenter
        Caption = 'Material 3'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clMenuHighlight
        Font.Height = -11
        Font.Name = 'Microsoft Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
      end
    end
    object pnlResinForce: TPanel
      Left = 12
      Top = 104
      Width = 482
      Height = 17
      BevelOuter = bvNone
      TabOrder = 3
      object chkForceResin1: TCheckBox
        Left = 0
        Top = 0
        Width = 105
        Height = 17
        TabStop = False
        Caption = 'Flush again'
        Font.Charset = ANSI_CHARSET
        Font.Color = clMenuHighlight
        Font.Height = -12
        Font.Name = 'Microsoft Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 0
      end
      object chkForceResin2: TCheckBox
        Tag = 1
        Left = 192
        Top = 0
        Width = 97
        Height = 17
        TabStop = False
        Caption = 'Flush again'
        Font.Charset = ANSI_CHARSET
        Font.Color = clMenuHighlight
        Font.Height = -12
        Font.Name = 'Microsoft Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 1
      end
      object chkForceResin3: TCheckBox
        Tag = 2
        Left = 384
        Top = 0
        Width = 97
        Height = 17
        TabStop = False
        Caption = 'Flush again'
        Font.Charset = ANSI_CHARSET
        Font.Color = clMenuHighlight
        Font.Height = -12
        Font.Name = 'Microsoft Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 2
      end
    end
  end
  object grpFlushingCycle: TGroupBox
    AlignWithMargins = True
    Left = 5
    Top = 279
    Width = 564
    Height = 130
    Margins.Left = 0
    Margins.Top = 0
    Margins.Right = 0
    Margins.Bottom = 0
    Align = alCustom
    Caption = 'Flushing Cycle'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 2
    object lblNoteStep3: TLabel
      Left = 11
      Top = 89
      Width = 300
      Height = 30
      Caption = 
        #39'Full'#39' provides you with exact color immediately.'#13#10#39'Economy'#39' ena' +
        'bles a faster material replacement cycle.'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Microsoft Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object pnlResinReplCycle: TPanel
      Left = 3
      Top = 15
      Width = 545
      Height = 65
      BevelOuter = bvNone
      TabOrder = 0
      object bvlRepCycleResins12: TBevel
        Left = 180
        Top = -5
        Width = 2
        Height = 70
      end
      object bvlRepCycleResins23: TBevel
        Left = 372
        Top = -5
        Width = 2
        Height = 70
      end
      object pnlReplCycle1: TPanel
        Left = 1
        Top = 1
        Width = 145
        Height = 57
        BevelOuter = bvNone
        TabOrder = 0
        object rbFullResin1: TRadioButton
          Left = 8
          Top = 1
          Width = 89
          Height = 25
          Caption = 'Full'
          Checked = True
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Microsoft Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 0
          TabStop = True
        end
        object rbEconomyResin1: TRadioButton
          Tag = 1
          Left = 8
          Top = 32
          Width = 89
          Height = 25
          Caption = 'Economy'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Microsoft Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 1
          TabStop = True
        end
      end
      object pnlReplCycle2: TPanel
        Tag = 1
        Left = 192
        Top = 1
        Width = 145
        Height = 57
        BevelOuter = bvNone
        TabOrder = 1
        object rbFullResin2: TRadioButton
          Left = 8
          Top = 1
          Width = 89
          Height = 25
          Caption = 'Full'
          Checked = True
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Microsoft Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 0
          TabStop = True
        end
        object rbEconomyResin2: TRadioButton
          Tag = 1
          Left = 8
          Top = 32
          Width = 89
          Height = 25
          Caption = 'Economy'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Microsoft Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 1
          TabStop = True
        end
      end
      object pnlReplCycle3: TPanel
        Tag = 2
        Left = 385
        Top = 1
        Width = 145
        Height = 57
        BevelOuter = bvNone
        TabOrder = 2
        object rbFullResin3: TRadioButton
          Left = 8
          Top = 1
          Width = 89
          Height = 25
          Caption = 'Full'
          Checked = True
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Microsoft Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 0
          TabStop = True
        end
        object rbEconomyResin3: TRadioButton
          Tag = 1
          Left = 8
          Top = 32
          Width = 89
          Height = 25
          Caption = 'Economy'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Microsoft Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 1
          TabStop = True
        end
      end
    end
  end
  object grpRequiredSupportMaterials: TGroupBox
    AlignWithMargins = True
    Left = 582
    Top = 115
    Width = 185
    Height = 162
    Margins.Left = 0
    Margins.Right = 0
    Margins.Bottom = 0
    Caption = 'Required Support materials'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 3
    object lblSupportResin: TLabel
      Tag = 2
      Left = 16
      Top = 29
      Width = 44
      Height = 13
      Alignment = taCenter
      Caption = 'Support'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clMenuHighlight
      Font.Height = -11
      Font.Name = 'Microsoft Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
      Visible = False
    end
    object comboSupportResin: TComboBox
      Tag = 4
      Left = 17
      Top = 46
      Width = 152
      Height = 23
      Style = csDropDownList
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Microsoft Sans Serif'
      Font.Style = []
      ItemHeight = 15
      ParentFont = False
      TabOrder = 0
      OnChange = comboSupportResinChange
    end
    object comboLeftRightCartSupport: TComboBox
      Left = 120
      Top = 75
      Width = 49
      Height = 23
      Style = csDropDownList
      DropDownCount = 2
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Microsoft Sans Serif'
      Font.Style = []
      ItemHeight = 15
      ItemIndex = 0
      ParentFont = False
      TabOrder = 1
      TabStop = False
      Text = 'L'
      Visible = False
      Items.Strings = (
        'L'
        'R')
    end
    object chkBothCartSupport: TCheckBox
      Left = 16
      Top = 78
      Width = 104
      Height = 17
      Caption = 'Both cartridges'
      Checked = True
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Microsoft Sans Serif'
      Font.Style = []
      ParentFont = False
      State = cbChecked
      TabOrder = 2
    end
    object chkForceSupportResin: TCheckBox
      Tag = 2
      Left = 17
      Top = 101
      Width = 97
      Height = 17
      TabStop = False
      Caption = 'Flush again'
      Font.Charset = ANSI_CHARSET
      Font.Color = clMenuHighlight
      Font.Height = -12
      Font.Name = 'Microsoft Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 3
    end
  end
  object grpSupportFlushingCycle: TGroupBox
    Left = 582
    Top = 279
    Width = 185
    Height = 130
    Margins.Left = 0
    Margins.Top = 0
    Margins.Right = 0
    Margins.Bottom = 0
    Padding.Left = 2
    Padding.Top = 2
    Padding.Right = 2
    TabOrder = 0
  end
end
