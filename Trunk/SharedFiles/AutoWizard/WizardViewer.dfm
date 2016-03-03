object WizardViewerFrame: TWizardViewerFrame
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
  object Bevel1: TBevel
    Left = 6
    Top = 248
    Width = 503
    Height = 9
    Shape = bsTopLine
  end
  object PageImage: TImage
    Left = 11
    Top = 8
    Width = 120
    Height = 230
    ParentShowHint = False
    ShowHint = True
  end
  object HiddenTitleLabel: TLabel
    Left = 136
    Top = 16
    Width = 183
    Height = 26
    Caption = 'HiddenTitleLabel'
    Font.Charset = HEBREW_CHARSET
    Font.Color = clWindowText
    Font.Height = -24
    Font.Name = 'Times New Roman'
    Font.Style = [fsBold]
    ParentFont = False
    Visible = False
  end
  object HiddenSubTitleLabel: TLabel
    Left = 134
    Top = 61
    Width = 130
    Height = 16
    Caption = 'Hidden SubTitle label'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    Visible = False
  end
  object PleaseWaitLabel: TLabel
    Left = 11
    Top = 111
    Width = 493
    Height = 41
    Alignment = taCenter
    Anchors = [akLeft, akTop, akRight, akBottom]
    AutoSize = False
    Caption = 'Please wait while finishing ...'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -20
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    Transparent = True
    Layout = tlCenter
    Visible = False
  end
  object DoneButton: TBitBtn
    Left = 434
    Top = 260
    Width = 75
    Height = 25
    Caption = '&Done'
    Default = True
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    TabOrder = 4
    Visible = False
    OnClick = DoneButtonClick
    Glyph.Data = {
      DE010000424DDE01000000000000760000002800000024000000120000000100
      0400000000006801000000000000000000001000000000000000000000000000
      80000080000000808000800000008000800080800000C0C0C000808080000000
      FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333333333
      3333333333333333333333330000333333333333333333333333F33333333333
      00003333344333333333333333388F3333333333000033334224333333333333
      338338F3333333330000333422224333333333333833338F3333333300003342
      222224333333333383333338F3333333000034222A22224333333338F338F333
      8F33333300003222A3A2224333333338F3838F338F33333300003A2A333A2224
      33333338F83338F338F33333000033A33333A222433333338333338F338F3333
      0000333333333A222433333333333338F338F33300003333333333A222433333
      333333338F338F33000033333333333A222433333333333338F338F300003333
      33333333A222433333333333338F338F00003333333333333A22433333333333
      3338F38F000033333333333333A223333333333333338F830000333333333333
      333A333333333333333338330000333333333333333333333333333333333333
      0000}
    NumGlyphs = 2
  end
  object NextButton: TBitBtn
    Left = 434
    Top = 260
    Width = 75
    Height = 25
    Caption = '&Next'
    Default = True
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    TabOrder = 0
    OnClick = NextButtonClick
    Glyph.Data = {
      36060000424D3606000000000000360000002800000020000000100000000100
      18000000000000060000120B0000120B00000000000000000000FF00FFFF00FF
      FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00
      FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF
      00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF946F14896512
      FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00
      FFFF00FFFF00FFFF00FF9F9F9F9F9F9FFF00FFFF00FFFF00FFFF00FFFF00FFFF
      00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF9B7315
      8C68137D5D11FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00
      FFFF00FFFF00FFFF00FFFF00FF9F9F9F9F9F9F9F9F9FFF00FFFF00FFFF00FFFF
      00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFBC9C50
      B08F3E936E148562127A5B11FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00
      FFFF00FFFF00FFFF00FFFF00FF9F9F9FFF00FFFF00FF9F9F9F9F9F9FFF00FFFF
      00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF
      CBB275BEA15B9972158C6712816011795911FF00FFFF00FFFF00FFFF00FFFF00
      FFFF00FFFF00FFFF00FFFF00FFFF00FF9F9F9FFF00FFFF00FFFF00FF9F9F9F9F
      9F9FFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF
      FF00FFB69034AD85279D7516956E148966127F5F11775910FF00FFFF00FFFF00
      FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF9F9F9FFF00FFFF00FFFF00FFFF
      00FF9F9F9F9F9F9FFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF
      FF00FFAE8016AD8117A87D16A279159B7315936D138765127D5E11765810FF00
      FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF9F9F9FFF00FFFF00FFFF00FFFF
      00FFFF00FFFF00FF9F9F9F9F9F9FFF00FFFF00FFFF00FFFF00FFB98A18B08318
      B18418B58718B58718B08318AB8017A57B169F7716997215906C138564117C5D
      11765910FF00FFFF00FF9F9F9F9F9F9F9F9F9FFF00FFFF00FFFF00FFFF00FFFF
      00FFFF00FFFF00FFFF00FFFF00FF9F9F9F9F9F9FFF00FFFF00FFD29D1CD29F24
      CC9B23C79722C1901DB98A19B48618AE8117A87E16A279169C7515977115926C
      148B68128765128664129F9F9FFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF
      00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF9F9F9F9F9F9FEED290F2E0B5
      EFDAAAE2C379D0A43BC3911ABD8C19B68819B3871EB48D30BC9C50C4AA6DC2AA
      71B89C5EFF00FFFF00FF9F9F9F9F9F9F9F9F9FFF00FFFF00FFFF00FFFF00FFFF
      00FFFF00FFFF00FFFF00FFFF00FF9F9F9F9F9F9FFF00FFFF00FFFF00FFFF00FF
      FF00FFDFB654D5A636CB971CC79621C69B32CBA753D0B46FD0B678CBB275FF00
      FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF9F9F9FFF00FFFF00FFFF00FFFF
      00FFFF00FFFF00FF9F9F9F9F9F9FFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF
      FF00FFD7A01DD7A11DD8A836DAB354DDBD70DCBF7AD8BA76FF00FFFF00FFFF00
      FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF9F9F9FFF00FFFF00FFFF00FFFF
      00FF9F9F9F9F9F9FFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF
      D19B1CDDA51DE5BA4FE8C572E7C77BE3C478FF00FFFF00FFFF00FFFF00FFFF00
      FFFF00FFFF00FFFF00FFFF00FFFF00FF9F9F9FFF00FFFF00FFFF00FF9F9F9F9F
      9F9FFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFD8A21D
      E4AA1FEFCA6FF0CE80EDCB79FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00
      FFFF00FFFF00FFFF00FFFF00FF9F9F9FFF00FFFF00FF9F9F9F9F9F9FFF00FFFF
      00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFEBBD4D
      F3D896F1CF7EFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00
      FFFF00FFFF00FFFF00FFFF00FF9F9F9F9F9F9F9F9F9FFF00FFFF00FFFF00FFFF
      00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFEFC86DF5DFAA
      FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00
      FFFF00FFFF00FFFF00FF9F9F9F9F9F9FFF00FFFF00FFFF00FFFF00FFFF00FFFF
      00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF}
    Layout = blGlyphRight
    NumGlyphs = 2
  end
  object PrevButton: TBitBtn
    Left = 354
    Top = 260
    Width = 75
    Height = 25
    Caption = '&Previous'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    TabOrder = 1
    OnClick = PrevButtonClick
    Glyph.Data = {
      36060000424D3606000000000000360000002800000020000000100000000100
      18000000000000060000120B0000120B00000000000000000000FF00FFFF00FF
      FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00
      FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF
      00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF
      FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00
      FFFF00FF886512906C14FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF
      00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF9F9F9F9F9F9FFF00FFFF00FF
      FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF805F
      118C6813976F14FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF
      00FFFF00FFFF00FFFF00FFFF00FF9F9F9F9F9F9F9F9F9FFF00FFFF00FFFF00FF
      FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF7D5E11876412946F
      149E7615A17815FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF
      00FFFF00FFFF00FF9F9F9F9F9F9FFF00FFFF00FF9F9F9FFF00FFFF00FFFF00FF
      FF00FFFF00FFFF00FFFF00FFFF00FFFF00FF7D5C118462128E6913997215A780
      22A77C17FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF
      00FF9F9F9F9F9F9FFF00FFFF00FFFF00FF9F9F9FFF00FFFF00FFFF00FFFF00FF
      FF00FFFF00FFFF00FFFF00FF7A5B108261128B6813966F149D7616A57B16A77C
      15FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF9F9F9F9F
      9F9FFF00FFFF00FFFF00FFFF00FF9F9F9FFF00FFFF00FFFF00FFFF00FFFF00FF
      FF00FFFF00FF795A108060118A6613946D139B7415A27915A87D16AC8017A87C
      15FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF9F9F9F9F9F9FFF00FFFF
      00FFFF00FFFF00FFFF00FFFF00FF9F9F9FFF00FFFF00FFFF00FFFF00FFFF00FF
      7A5B117F5F11886512916C139973159F7716A57B16AB8017B08318B48618B285
      18AD8117AD8117B28517FF00FFFF00FF9F9F9F9F9F9FFF00FFFF00FFFF00FFFF
      00FFFF00FFFF00FFFF00FFFF00FFFF00FF9F9F9F9F9F9F9F9F9F8A67138C6913
      8F6B1398731C9C771D9F7719A27916A87E16AE8117B48618B98A19BF8D19C392
      1AC8961BCD991CC8961A9F9F9F9F9F9FFF00FFFF00FFFF00FFFF00FFFF00FFFF
      00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF9F9F9FFF00FFFF00FF
      D9CAA9DBCDABD5C398C6AB6BB9953EB58A23B68819BD8C19C3911ACB9C29D6AC
      46E5C578EACC85DFAB2FFF00FFFF00FF9F9F9F9F9F9FFF00FFFF00FFFF00FFFF
      00FFFF00FFFF00FFFF00FFFF00FFFF00FF9F9F9F9F9F9F9F9F9FFF00FFFF00FF
      FF00FFFF00FFE2D4B3E3D4B0DECA9AD3B56DCAA240C89826CB971CD09B1CD29D
      1DFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF9F9F9F9F9F9FFF00FFFF
      00FFFF00FFFF00FFFF00FFFF00FF9F9F9FFF00FFFF00FFFF00FFFF00FFFF00FF
      FF00FFFF00FFFF00FFFF00FFE9D9B3EAD9B1E7D09ADFBE6EDAAE43D5A01DCF9A
      1CFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF9F9F9F9F
      9F9FFF00FFFF00FFFF00FFFF00FF9F9F9FFF00FFFF00FFFF00FFFF00FFFF00FF
      FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFF0DEB4F1DEB2EFD69CE7BD57D8A2
      1DC9951BFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF
      00FF9F9F9F9F9F9FFF00FFFF00FFFF00FF9F9F9FFF00FFFF00FFFF00FFFF00FF
      FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFF5E2B5F6E1B1F1D0
      80DFA61ECF9A1CFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF
      00FFFF00FFFF00FF9F9F9F9F9F9FFF00FFFF00FF9F9F9FFF00FFFF00FFFF00FF
      FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFF7E3
      B5F5DDA2E4AB1FFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF
      00FFFF00FFFF00FFFF00FFFF00FF9F9F9F9F9F9F9F9F9FFF00FFFF00FFFF00FF
      FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00
      FFFF00FFF4D999E5AA1EFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF
      00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF9F9F9F9F9F9F}
    NumGlyphs = 2
  end
  object CancelButton: TBitBtn
    Left = 262
    Top = 260
    Width = 75
    Height = 25
    Cancel = True
    Caption = '&Cancel'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    TabOrder = 2
    OnClick = CancelButtonClick
    Glyph.Data = {
      DE010000424DDE01000000000000760000002800000024000000120000000100
      0400000000006801000000000000000000001000000000000000000000000000
      80000080000000808000800000008000800080800000C0C0C000808080000000
      FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333333333
      333333333333333333333333000033338833333333333333333F333333333333
      0000333911833333983333333388F333333F3333000033391118333911833333
      38F38F333F88F33300003339111183911118333338F338F3F8338F3300003333
      911118111118333338F3338F833338F3000033333911111111833333338F3338
      3333F8330000333333911111183333333338F333333F83330000333333311111
      8333333333338F3333383333000033333339111183333333333338F333833333
      00003333339111118333333333333833338F3333000033333911181118333333
      33338333338F333300003333911183911183333333383338F338F33300003333
      9118333911183333338F33838F338F33000033333913333391113333338FF833
      38F338F300003333333333333919333333388333338FFF830000333333333333
      3333333333333333333888330000333333333333333333333333333333333333
      0000}
    NumGlyphs = 2
  end
  object MainPanel: TPanel
    Left = 144
    Top = 8
    Width = 365
    Height = 231
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    TabOrder = 3
    object TitleLabel: TLabel
      Left = 6
      Top = 6
      Width = 353
      Height = 26
      Alignment = taCenter
      AutoSize = False
      Caption = 'Title'
      Font.Charset = HEBREW_CHARSET
      Font.Color = clWindowText
      Font.Height = -24
      Font.Name = 'Times New Roman'
      Font.Style = [fsBold]
      ParentFont = False
      WordWrap = True
    end
    object WizardPageControl: TPageControl
      Left = 1
      Top = 40
      Width = 363
      Height = 190
      ActivePage = StatusPage
      Align = alBottom
      Style = tsFlatButtons
      TabOrder = 0
      object MessagePage: TTabSheet
        Caption = 'MessagePage'
        TabVisible = False
        ExplicitLeft = 0
        ExplicitTop = 0
        ExplicitWidth = 0
        ExplicitHeight = 0
        object MessageLabel: TLabel
          Left = 8
          Top = 9
          Width = 339
          Height = 173
          AutoSize = False
          Caption = 'Message'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          WordWrap = True
        end
      end
      object StatusPage: TTabSheet
        Caption = 'StatusPage'
        ImageIndex = 1
        TabVisible = False
        object StatusPageSubTitle: TLabel
          Tag = 32
          Left = 14
          Top = 32
          Width = 331
          Height = 16
          AutoSize = False
          Caption = 'StatusPageSubTitle'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          WordWrap = True
        end
        object StatusPanel: TPanel
          Left = 4
          Top = 53
          Width = 347
          Height = 46
          BevelOuter = bvLowered
          Color = clWhite
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -16
          Font.Name = 'MS Sans Serif'
          Font.Style = [fsBold]
          ParentBackground = False
          ParentFont = False
          TabOrder = 0
          object StatusLabel: TLabel
            Tag = 32
            Left = 1
            Top = 1
            Width = 345
            Height = 44
            Align = alClient
            Alignment = taCenter
            Caption = 'Status'
            Color = clWhite
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clBlack
            Font.Height = -16
            Font.Name = 'MS Sans Serif'
            Font.Style = [fsBold]
            ParentColor = False
            ParentFont = False
            Layout = tlCenter
            WordWrap = True
            ExplicitWidth = 54
            ExplicitHeight = 20
          end
        end
      end
      object ProgressPage: TTabSheet
        Caption = 'ProgressPage'
        ImageIndex = 2
        TabVisible = False
        ExplicitLeft = 0
        ExplicitTop = 0
        ExplicitWidth = 0
        ExplicitHeight = 0
        object ProgressIndicator: TGauge
          Left = 13
          Top = 53
          Width = 328
          Height = 33
          ForeColor = clBlue
          Progress = 0
        end
        object ProgressPageSubTitle: TLabel
          Tag = 32
          Left = 14
          Top = 32
          Width = 323
          Height = 16
          AutoSize = False
          Caption = 'ProgressPageSubTitle'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          WordWrap = True
        end
      end
      object CheckboxesPage: TTabSheet
        Caption = 'CheckboxesPage'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ImageIndex = 3
        ParentFont = False
        TabVisible = False
        ExplicitLeft = 0
        ExplicitTop = 0
        ExplicitWidth = 0
        ExplicitHeight = 0
        object CheckBoxPageSubTitle: TLabel
          Left = 28
          Top = 0
          Width = 299
          Height = 16
          AutoSize = False
          Caption = 'CheckBoxPageSubTitle'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          WordWrap = True
        end
        object CheckBoxesContainer: TScrollBox
          Tag = 20
          Left = 28
          Top = 20
          Width = 300
          Height = 153
          HorzScrollBar.Tracking = True
          HorzScrollBar.Visible = False
          VertScrollBar.Tracking = True
          BevelInner = bvNone
          BevelOuter = bvNone
          BevelKind = bkFlat
          TabOrder = 0
        end
      end
      object RadioGroupPage: TTabSheet
        Caption = 'RadioGroupPage'
        ImageIndex = 4
        TabVisible = False
        ExplicitLeft = 0
        ExplicitTop = 0
        ExplicitWidth = 0
        ExplicitHeight = 0
        object RadioGroupPageSubTitle: TLabel
          Left = 38
          Top = 0
          Width = 299
          Height = 16
          AutoSize = False
          Caption = 'RadioGroupPageSubTitle'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          WordWrap = True
        end
        object RadioGroup: TRadioGroup
          Tag = 17
          Left = 37
          Top = 17
          Width = 280
          Height = 100
          TabOrder = 0
        end
      end
      object ProgressStatusTabSheet: TTabSheet
        Caption = 'ProgressStatusTabSheet'
        ImageIndex = 6
        TabVisible = False
        ExplicitLeft = 0
        ExplicitTop = 0
        ExplicitWidth = 0
        ExplicitHeight = 0
        object ProgressStatusProgressIndicator: TGauge
          Left = 13
          Top = 57
          Width = 328
          Height = 33
          ForeColor = clBlue
          Progress = 0
        end
        object ProgressStatusPageSubTitle: TLabel
          Tag = 32
          Left = 14
          Top = 32
          Width = 331
          Height = 16
          AutoSize = False
          Caption = 'ProgressStatusPageSubTitle '
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          WordWrap = True
        end
        object ProgressStatusStatusPanel: TPanel
          Left = 4
          Top = 53
          Width = 347
          Height = 41
          BevelOuter = bvLowered
          Caption = 'Status'
          Color = clWhite
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -16
          Font.Name = 'MS Sans Serif'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 0
        end
      end
      object CustomTabSheet: TTabSheet
        Caption = 'CustomTabSheet'
        ImageIndex = 5
        TabVisible = False
        ExplicitLeft = 0
        ExplicitTop = 0
        ExplicitWidth = 0
        ExplicitHeight = 0
      end
      object DataEntryPage: TTabSheet
        Caption = 'DataEntryPage'
        ImageIndex = 7
        TabVisible = False
        ExplicitLeft = 0
        ExplicitTop = 0
        ExplicitWidth = 0
        ExplicitHeight = 0
        object DataEntryPageSubTitle: TLabel
          Left = 38
          Top = 0
          Width = 299
          Height = 16
          AutoSize = False
          Caption = 'DataEntryPageSubTitle'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          WordWrap = True
        end
        object DataEntryFieldsContainer: TScrollBox
          Tag = 20
          Left = 38
          Top = 20
          Width = 280
          Height = 153
          HorzScrollBar.Tracking = True
          HorzScrollBar.Visible = False
          VertScrollBar.Tracking = True
          BevelInner = bvNone
          BevelOuter = bvNone
          BevelKind = bkFlat
          TabOrder = 0
        end
      end
    end
  end
  object HelpButton: TBitBtn
    Left = 16
    Top = 260
    Width = 75
    Height = 25
    Action = HelpAction
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    TabOrder = 5
    Kind = bkHelp
  end
  object UserButton2: TBitBtn
    Left = 98
    Top = 260
    Width = 75
    Height = 25
    Caption = 'UserButton2'
    Constraints.MaxWidth = 110
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    TabOrder = 6
    Visible = False
    OnClick = UserButton2Click
  end
  object UserButton1: TBitBtn
    Left = 180
    Top = 260
    Width = 75
    Height = 25
    Caption = 'UserButton1'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    TabOrder = 7
    Visible = False
    OnClick = UserButton1Click
  end
  object ActionList1: TActionList
    Left = 80
    Top = 140
    object HelpAction: TAction
      Caption = 'Help'
      ShortCut = 112
      OnExecute = HelpActionExecute
    end
    object ProgressiveDisclosureCollapseAction: TAction
      ImageIndex = 1
      OnExecute = ProgressiveDisclosureCollapseActionExecute
    end
    object ProgressiveDisclosureExpandAction: TAction
      ImageIndex = 0
      OnExecute = ProgressiveDisclosureExpandActionExecute
    end
  end
  object PageWizardTimer: TTimer
    Enabled = False
    Interval = 7200000
    OnTimer = PageWizardTimerTimer
    Left = 80
    Top = 80
  end
end
