object CustomResinMainSelectWizardFrame: TCustomResinMainSelectWizardFrame
  Left = 0
  Top = 0
  Width = 698
  Height = 455
  Padding.Left = 5
  Padding.Top = 5
  Padding.Right = 5
  Padding.Bottom = 5
  TabOrder = 0
  DesignSize = (
    698
    455)
  object lblTitle: TLabel
    Left = 5
    Top = 5
    Width = 688
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
    Width = 688
    Height = 81
    Align = alTop
    Caption = 'Printing mode'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 2
    object pnlPrintingMode: TPanel
      Left = 2
      Top = 15
      Width = 683
      Height = 64
      Align = alLeft
      BevelOuter = bvNone
      TabOrder = 0
      DesignSize = (
        683
        64)
      object imgWarning: TImage
        Left = 632
        Top = 12
        Width = 48
        Height = 48
        Anchors = [akTop, akRight]
        Transparent = True
        Visible = False
      end
      object lblHSWwarning: TLabel
        Left = 342
        Top = 7
        Width = 284
        Height = 39
        Caption = 
          'After replacing the Support material with a different type, you ' +
          'must run the Head Optimization Wizard before printing is enabled'
        Color = clRed
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clRed
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentColor = False
        ParentFont = False
        Visible = False
        WordWrap = True
      end
      object rbDM7Mode: TRadioButton
        Left = 10
        Top = 1
        Width = 233
        Height = 25
        Caption = '6 materials (High Mix / High Quality)'
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
      object rbDM3Mode: TRadioButton
        Tag = 1
        Left = 10
        Top = 32
        Width = 233
        Height = 25
        Caption = '3 materials (High Speed)'
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
  object grpRequiredMaterials: TGroupBox
    AlignWithMargins = True
    Left = 5
    Top = 112
    Width = 689
    Height = 289
    Margins.Left = 0
    Margins.Top = 0
    Margins.Right = 0
    Margins.Bottom = 0
    Align = alCustom
    Anchors = [akLeft, akTop, akRight]
    Caption = 'Required materials'
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
    object bvlResinsSupport: TBevel
      Left = 70
      Top = 250
      Width = 598
      Height = 1
    end
    object pnlResinBothCarts: TPanel
      Left = 249
      Top = 43
      Width = 160
      Height = 203
      BevelOuter = bvNone
      TabOrder = 2
      object chkBothCartResin7: TCheckBox
        Tag = 6
        Left = 2
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
      object chkBothCartResin6: TCheckBox
        Tag = 5
        Left = 2
        Top = 32
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
      object chkBothCartResin5: TCheckBox
        Tag = 4
        Left = 2
        Top = 61
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
      object chkBothCartResin4: TCheckBox
        Tag = 3
        Left = 2
        Top = 90
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
        TabOrder = 6
      end
      object chkBothCartResin3: TCheckBox
        Tag = 2
        Left = 2
        Top = 122
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
        TabOrder = 7
      end
      object chkBothCartResin2: TCheckBox
        Tag = 1
        Left = 2
        Top = 151
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
        TabOrder = 8
      end
      object chkBothCartResin1: TCheckBox
        Left = 2
        Top = 180
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
        TabOrder = 9
      end
      object comboLeftRightCart7: TComboBox
        Tag = 6
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
      object comboLeftRightCart6: TComboBox
        Tag = 5
        Left = 107
        Top = 29
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
        TabOrder = 10
        TabStop = False
        Text = 'L'
        Visible = False
        Items.Strings = (
          'L'
          'R')
      end
      object comboLeftRightCart5: TComboBox
        Tag = 4
        Left = 107
        Top = 58
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
      object comboLeftRightCart4: TComboBox
        Tag = 3
        Left = 107
        Top = 87
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
        TabOrder = 11
        TabStop = False
        Text = 'L'
        Visible = False
        Items.Strings = (
          'L'
          'R')
      end
      object comboLeftRightCart3: TComboBox
        Tag = 2
        Left = 107
        Top = 119
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
        TabOrder = 12
        TabStop = False
        Text = 'L'
        Visible = False
        Items.Strings = (
          'L'
          'R')
      end
      object comboLeftRightCart2: TComboBox
        Tag = 1
        Left = 107
        Top = 148
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
        TabOrder = 13
        TabStop = False
        Text = 'L'
        Visible = False
        Items.Strings = (
          'L'
          'R')
      end
      object comboLeftRightCart1: TComboBox
        Left = 107
        Top = 177
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
    end
    object pnlResinChooser: TPanel
      Left = 70
      Top = 42
      Width = 173
      Height = 207
      BevelOuter = bvNone
      TabOrder = 1
      object comboResins7: TComboBox
        Tag = 6
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
      object comboResins6: TComboBox
        Tag = 5
        Left = 4
        Top = 29
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
      object comboResins5: TComboBox
        Tag = 4
        Left = 4
        Top = 58
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
      object comboResins4: TComboBox
        Tag = 3
        Left = 4
        Top = 87
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
        TabOrder = 3
      end
      object comboResins3: TComboBox
        Tag = 2
        Left = 4
        Top = 119
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
        TabOrder = 4
      end
      object comboResins2: TComboBox
        Tag = 1
        Left = 4
        Top = 148
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
        TabOrder = 5
      end
      object comboResins1: TComboBox
        Left = 4
        Top = 177
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
        TabOrder = 6
      end
    end
    object pnlResinChooserCaptions: TPanel
      Left = 8
      Top = 35
      Width = 60
      Height = 214
      BevelOuter = bvNone
      TabOrder = 0
      object lblResins1: TLabel
        Left = 16
        Top = 12
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
      end
      object lblResins2: TLabel
        Tag = 1
        Left = 31
        Top = 42
        Width = 17
        Height = 13
        Alignment = taCenter
        Caption = 'M6'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clMenuHighlight
        Font.Height = -11
        Font.Name = 'Microsoft Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object lblResins3: TLabel
        Tag = 2
        Left = 31
        Top = 71
        Width = 17
        Height = 13
        Alignment = taCenter
        Caption = 'M5'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clMenuHighlight
        Font.Height = -11
        Font.Name = 'Microsoft Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object lblResins4: TLabel
        Tag = 3
        Left = 31
        Top = 100
        Width = 17
        Height = 13
        Alignment = taCenter
        Caption = 'M4'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clMenuHighlight
        Font.Height = -11
        Font.Name = 'Microsoft Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object lblResins5: TLabel
        Tag = 4
        Left = 31
        Top = 132
        Width = 17
        Height = 13
        Alignment = taCenter
        Caption = 'M3'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clMenuHighlight
        Font.Height = -11
        Font.Name = 'Microsoft Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object lblResins6: TLabel
        Tag = 5
        Left = 31
        Top = 161
        Width = 17
        Height = 13
        Alignment = taCenter
        Caption = 'M2'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clMenuHighlight
        Font.Height = -11
        Font.Name = 'Microsoft Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object lblResins7: TLabel
        Tag = 6
        Left = 31
        Top = 190
        Width = 17
        Height = 13
        Alignment = taCenter
        Caption = 'M1'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clMenuHighlight
        Font.Height = -11
        Font.Name = 'Microsoft Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object lblCabinetName: TLabel
        Left = 2
        Top = 56
        Width = 11
        Height = 20
        Caption = 'B'
        Font.Charset = ANSI_CHARSET
        Font.Color = clMaroon
        Font.Height = -16
        Font.Name = 'Microsoft Sans Serif'
        Font.Style = []
        ParentFont = False
        Layout = tlCenter
      end
      object lblCabinet2Name: TLabel
        Left = 2
        Top = 168
        Width = 11
        Height = 20
        Caption = 'A'
        Font.Charset = ANSI_CHARSET
        Font.Color = clMaroon
        Font.Height = -16
        Font.Name = 'Microsoft Sans Serif'
        Font.Style = []
        ParentFont = False
      end
    end
    object pnlResinForce: TPanel
      Left = 557
      Top = 44
      Width = 109
      Height = 197
      BevelOuter = bvNone
      TabOrder = 3
      object chkForceResin7: TCheckBox
        Tag = 6
        Left = 0
        Top = 3
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
      object chkForceResin6: TCheckBox
        Tag = 5
        Left = 0
        Top = 32
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
      object chkForceResin5: TCheckBox
        Tag = 4
        Left = 0
        Top = 61
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
      object chkForceResin4: TCheckBox
        Tag = 3
        Left = 0
        Top = 90
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
      object chkForceResin3: TCheckBox
        Tag = 2
        Left = 0
        Top = 122
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
        TabOrder = 4
      end
      object chkForceResin2: TCheckBox
        Tag = 1
        Left = 0
        Top = 152
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
        TabOrder = 5
      end
      object chkForceResin1: TCheckBox
        Left = 0
        Top = 180
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
        TabOrder = 6
      end
    end
    object pnlResinReplCycle: TPanel
      Left = 409
      Top = 31
      Width = 136
      Height = 218
      BevelOuter = bvNone
      TabOrder = 4
      object pnlReplCycle7: TPanel
        Tag = 6
        Left = 0
        Top = 11
        Width = 148
        Height = 24
        BevelOuter = bvNone
        TabOrder = 0
        Visible = False
        object rbFullResin7: TRadioButton
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
          Visible = False
        end
        object rbEconomyResin7: TRadioButton
          Tag = 1
          Left = 56
          Top = 1
          Width = 81
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
          Visible = False
        end
      end
      object pnlReplCycle6: TPanel
        Tag = 5
        Left = 0
        Top = 39
        Width = 148
        Height = 24
        BevelOuter = bvNone
        TabOrder = 1
        object rbFullResin6: TRadioButton
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
        object rbEconomyResin6: TRadioButton
          Tag = 1
          Left = 56
          Top = 1
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
      object pnlReplCycle5: TPanel
        Tag = 4
        Left = 0
        Top = 68
        Width = 148
        Height = 24
        BevelOuter = bvNone
        TabOrder = 4
        object rbFullResin5: TRadioButton
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
        object rbEconomyResin5: TRadioButton
          Tag = 1
          Left = 56
          Top = 1
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
      object pnlReplCycle4: TPanel
        Tag = 3
        Left = 0
        Top = 98
        Width = 148
        Height = 24
        BevelOuter = bvNone
        TabOrder = 3
        object rbFullResin4: TRadioButton
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
        object rbEconomyResin4: TRadioButton
          Tag = 1
          Left = 56
          Top = 1
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
        Left = 0
        Top = 130
        Width = 148
        Height = 24
        BevelOuter = bvNone
        TabOrder = 5
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
          Left = 56
          Top = 1
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
        Left = 0
        Top = 159
        Width = 148
        Height = 24
        BevelOuter = bvNone
        TabOrder = 6
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
          Left = 56
          Top = 1
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
      object pnlReplCycle1: TPanel
        Left = 0
        Top = 188
        Width = 148
        Height = 24
        BevelOuter = bvNone
        TabOrder = 2
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
          Left = 56
          Top = 1
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
    object pnlRequiredSupportMaterials: TPanel
      Left = 3
      Top = 252
      Width = 679
      Height = 29
      BevelOuter = bvNone
      TabOrder = 5
      object lblSupportResin: TLabel
        Left = 21
        Top = 8
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
      end
      object chkBothCartSupport: TCheckBox
        Left = 248
        Top = 6
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
        TabOrder = 1
      end
      object chkForceSupportResin: TCheckBox
        Left = 554
        Top = 6
        Width = 97
        Height = 17
        Caption = 'Flush again'
        Font.Charset = ANSI_CHARSET
        Font.Color = clMenuHighlight
        Font.Height = -12
        Font.Name = 'Microsoft Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 3
      end
      object comboLeftRightCartSupport: TComboBox
        Left = 353
        Top = 3
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
      object comboSupportResin: TComboBox
        Left = 71
        Top = 3
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
      object pnlSuppRedundancy: TPanel
        Left = 240
        Top = 3
        Width = 289
        Height = 25
        BevelOuter = bvNone
        TabOrder = 4
        Visible = False
        object lblCartridges: TLabel
          Left = 97
          Top = 4
          Width = 54
          Height = 15
          Caption = 'cartridges'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Microsoft Sans Serif'
          Font.Style = []
          ParentFont = False
        end
        object pnlTGroupBoxContainer: TPanel
          Left = 0
          Top = -11
          Width = 97
          Height = 33
          BevelOuter = bvNone
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Microsoft Sans Serif'
          Font.Style = []
          ParentFont = False
          TabOrder = 0
          object rbGroupRedund: TRadioGroup
            Left = -3
            Top = -3
            Width = 125
            Height = 40
            Margins.Left = 0
            Margins.Top = 0
            Margins.Right = 0
            Margins.Bottom = 0
            Columns = 4
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -12
            Font.Name = 'Microsoft Sans Serif'
            Font.Style = []
            Items.Strings = (
              '2'
              '3'
              '4')
            ParentFont = False
            TabOrder = 0
          end
        end
      end
    end
    object pnlBevelCabinets: TPanel
      Left = 12
      Top = 156
      Width = 656
      Height = 2
      TabOrder = 6
    end
    object pnlChamPairing: TPanel
      Left = 227
      Top = 40
      Width = 18
      Height = 205
      BevelOuter = bvNone
      TabOrder = 7
      object imgPair1: TImage
        Left = 1
        Top = 41
        Width = 16
        Height = 32
        Picture.Data = {
          07544269746D617036060000424D360600000000000036040000280000001000
          000020000000010008000000000000020000C40E0000C40E0000000100000001
          0000775E7700785E7800767076007B737B00787678007C747C007E767E007979
          79007A7A7A007B7A7B007C7A7C007D7B7D007C7C7C007D7D7D007E7E7E007F7F
          7F00807180008172810086778600807E8000887988008D7E8D008E7F8E00FF00
          FF00808080008180810083808300828282008480840085858500878487008686
          86008A868A008C848C0088888800898989008D8A8D008D8D8D008E8D8E008F8D
          8F008E8E8E0091829100938B9300908C9000918D9100968E96009D849D009090
          9000939193009393930094939400959395009595950097959700969696009796
          97009A949A009B959B00989898009B9B9B009C9C9C009D9D9D00AC8FAC00A69D
          A600AA95AA00AC9BAC00AC9CAC00A7A2A700A8A4A800A9A4A900AFACAF00B3AD
          B300B3AFB300B4AFB400B7AEB700B9A7B900BFA3BF00B9A8B900BDA8BD00CBCB
          CB00CCCCCC00D0D0D000D1D1D100D2D1D200D3D2D300D4D4D400D5D5D500D6D6
          D600D7D6D700D8D7D800DADADA00DBDBDB00DCDBDC00DCDCDC00DDDDDD00DEDD
          DE00DEDEDE00E1E1E100E2E2E200E6E6E6000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000171717174943220F1B084343171717171717174D4825281F22181B444217
          171717174E2D33565A5155504F0905401717174C4A325A585A515450505A0B3F
          3E17174C38595A2F271E20191F5153023E17172E385B6027211214061B555502
          0117172E385B5E2B161717101A5D60020017172E395C5F2B161717111C5D5F02
          0017172E395C5E352A152903135E60020017172E385B5D3630242C0A195E6002
          0017172E385B5D3B3B5E620D0F5555020117174C3858573A3D62630F0D515202
          3E17174C4A3057343C61630F0C570A3F3E1717174E4E26313C61630F08044040
          17171717174646343C61630F0843431717171717171747373D61630F0C451717
          17171717171747373D61630F0C44171717171717174B46343C61630F0C444117
          171717174E4E26313C61630F070440401717174C4A3057343C61630F0C570A3F
          3E17174C3858573A3D62630F0E5152023E17172E385B5D3B3B5E620D0F555502
          0117172E395B5D3630242C0A195E60020017172E395C5E372A152903135E6002
          0017172E395C5F2B161717111C5D5F020017172E385B5E2B161717101A5D6002
          0017172E385B6027211214061A5455020117174C38595A2F271E20191D525302
          3E17174C4A325A585A515450505A0B3F3E1717174E2D33565A5154504F090540
          17171717174D4825281F23181B4442171717171717174949220F1B0843431717
          1717}
        Transparent = True
      end
      object imgPair2: TImage
        Left = 1
        Top = 101
        Width = 16
        Height = 32
        Picture.Data = {
          07544269746D617036060000424D360600000000000036040000280000001000
          000020000000010008000000000000020000C40E0000C40E0000000100000001
          0000775E7700785E7800767076007B737B00787678007C747C007E767E007979
          79007A7A7A007B7A7B007C7A7C007D7B7D007C7C7C007D7D7D007E7E7E007F7F
          7F00807180008172810086778600807E8000887988008D7E8D008E7F8E00FF00
          FF00808080008180810083808300828282008480840085858500878487008686
          86008A868A008C848C0088888800898989008D8A8D008D8D8D008E8D8E008F8D
          8F008E8E8E0091829100938B9300908C9000918D9100968E96009D849D009090
          9000939193009393930094939400959395009595950097959700969696009796
          97009A949A009B959B00989898009B9B9B009C9C9C009D9D9D00AC8FAC00A69D
          A600AA95AA00AC9BAC00AC9CAC00A7A2A700A8A4A800A9A4A900AFACAF00B3AD
          B300B3AFB300B4AFB400B7AEB700B9A7B900BFA3BF00B9A8B900BDA8BD00CBCB
          CB00CCCCCC00D0D0D000D1D1D100D2D1D200D3D2D300D4D4D400D5D5D500D6D6
          D600D7D6D700D8D7D800DADADA00DBDBDB00DCDBDC00DCDCDC00DDDDDD00DEDD
          DE00DEDEDE00E1E1E100E2E2E200E6E6E6000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000171717174943220F1B084343171717171717174D4825281F22181B444217
          171717174E2D33565A5155504F0905401717174C4A325A585A515450505A0B3F
          3E17174C38595A2F271E20191F5153023E17172E385B6027211214061B555502
          0117172E385B5E2B161717101A5D60020017172E395C5F2B161717111C5D5F02
          0017172E395C5E352A152903135E60020017172E385B5D3630242C0A195E6002
          0017172E385B5D3B3B5E620D0F5555020117174C3858573A3D62630F0D515202
          3E17174C4A3057343C61630F0C570A3F3E1717174E4E26313C61630F08044040
          17171717174646343C61630F0843431717171717171747373D61630F0C451717
          17171717171747373D61630F0C44171717171717174B46343C61630F0C444117
          171717174E4E26313C61630F070440401717174C4A3057343C61630F0C570A3F
          3E17174C3858573A3D62630F0E5152023E17172E385B5D3B3B5E620D0F555502
          0117172E395B5D3630242C0A195E60020017172E395C5E372A152903135E6002
          0017172E395C5F2B161717111C5D5F020017172E385B5E2B161717101A5D6002
          0017172E385B6027211214061A5455020117174C38595A2F271E20191D525302
          3E17174C4A325A585A515450505A0B3F3E1717174E2D33565A5154504F090540
          17171717174D4825281F23181B4442171717171717174949220F1B0843431717
          1717}
        Transparent = True
      end
      object imgPair3: TImage
        Left = 1
        Top = 161
        Width = 16
        Height = 32
        Picture.Data = {
          07544269746D617036060000424D360600000000000036040000280000001000
          000020000000010008000000000000020000C40E0000C40E0000000100000001
          0000775E7700785E7800767076007B737B00787678007C747C007E767E007979
          79007A7A7A007B7A7B007C7A7C007D7B7D007C7C7C007D7D7D007E7E7E007F7F
          7F00807180008172810086778600807E8000887988008D7E8D008E7F8E00FF00
          FF00808080008180810083808300828282008480840085858500878487008686
          86008A868A008C848C0088888800898989008D8A8D008D8D8D008E8D8E008F8D
          8F008E8E8E0091829100938B9300908C9000918D9100968E96009D849D009090
          9000939193009393930094939400959395009595950097959700969696009796
          97009A949A009B959B00989898009B9B9B009C9C9C009D9D9D00AC8FAC00A69D
          A600AA95AA00AC9BAC00AC9CAC00A7A2A700A8A4A800A9A4A900AFACAF00B3AD
          B300B3AFB300B4AFB400B7AEB700B9A7B900BFA3BF00B9A8B900BDA8BD00CBCB
          CB00CCCCCC00D0D0D000D1D1D100D2D1D200D3D2D300D4D4D400D5D5D500D6D6
          D600D7D6D700D8D7D800DADADA00DBDBDB00DCDBDC00DCDCDC00DDDDDD00DEDD
          DE00DEDEDE00E1E1E100E2E2E200E6E6E6000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000171717174943220F1B084343171717171717174D4825281F22181B444217
          171717174E2D33565A5155504F0905401717174C4A325A585A515450505A0B3F
          3E17174C38595A2F271E20191F5153023E17172E385B6027211214061B555502
          0117172E385B5E2B161717101A5D60020017172E395C5F2B161717111C5D5F02
          0017172E395C5E352A152903135E60020017172E385B5D3630242C0A195E6002
          0017172E385B5D3B3B5E620D0F5555020117174C3858573A3D62630F0D515202
          3E17174C4A3057343C61630F0C570A3F3E1717174E4E26313C61630F08044040
          17171717174646343C61630F0843431717171717171747373D61630F0C451717
          17171717171747373D61630F0C44171717171717174B46343C61630F0C444117
          171717174E4E26313C61630F070440401717174C4A3057343C61630F0C570A3F
          3E17174C3858573A3D62630F0E5152023E17172E385B5D3B3B5E620D0F555502
          0117172E395B5D3630242C0A195E60020017172E395C5E372A152903135E6002
          0017172E395C5F2B161717111C5D5F020017172E385B5E2B161717101A5D6002
          0017172E385B6027211214061A5455020117174C38595A2F271E20191D525302
          3E17174C4A325A585A515450505A0B3F3E1717174E2D33565A5154504F090540
          17171717174D4825281F23181B4442171717171717174949220F1B0843431717
          1717}
        Transparent = True
      end
    end
  end
  object grpNotificationBar: TGroupBox
    Left = 5
    Top = 402
    Width = 689
    Height = 48
    Margins.Left = 0
    Margins.Top = 0
    Margins.Right = 0
    Margins.Bottom = 0
    Anchors = [akLeft, akTop, akRight]
    Padding.Left = 2
    Padding.Top = 2
    Padding.Right = 2
    TabOrder = 0
    object lblNoteFlushing: TLabel
      Left = 5
      Top = 8
      Width = 300
      Height = 30
      Margins.Top = 1
      Margins.Bottom = 1
      Align = alCustom
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
    object chkSilentMode: TCheckBox
      Left = 465
      Top = 16
      Width = 217
      Height = 25
      Caption = 'Replace materials without flushing'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Microsoft Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 0
      OnClick = OnClickSilentMode
    end
  end
end
