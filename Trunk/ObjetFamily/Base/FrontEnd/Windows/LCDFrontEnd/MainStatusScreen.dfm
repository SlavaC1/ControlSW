inherited MainStatusFrame: TMainStatusFrame
  Width = 761
  Height = 600
  ExplicitWidth = 761
  ExplicitHeight = 600
  inherited Panel1: TPanel
    Width = 761
    Height = 600
    ExplicitWidth = 761
    ExplicitHeight = 600
    inherited PaintBox1: TPaintBox
      Width = 761
      Height = 600
      ExplicitWidth = 761
      ExplicitHeight = 600
    end
    inherited DateLabel: TLabel
      Left = 696
      Top = 390
      Width = 65
      ExplicitLeft = 696
      ExplicitTop = 390
      ExplicitWidth = 65
    end
    inherited TimeLabel: TLabel
      Left = 694
      Top = 358
      ExplicitLeft = 694
      ExplicitTop = 358
    end
    object CurrentStateLabel: TEnhancedLabel [3]
      Left = 30
      Top = 29
      Width = 0
      Height = 0
      CaptionXOffset = 0
      CaptionYOffset = 0
      Transparent = True
      TransparentMode = tmFixed
      TransparentColor = 9830550
      Alignment = taCenter
      MiddlePartsNum = 14
      Font.Charset = HEBREW_CHARSET
      Font.Color = clWindowText
      Font.Height = -16
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ActiveBitmap = 0
    end
    object PrintTimeLabel: TEnhancedLabel [4]
      Left = 30
      Top = 71
      Width = 0
      Height = 0
      CaptionXOffset = 4
      CaptionYOffset = 0
      Transparent = True
      TransparentMode = tmFixed
      TransparentColor = clFuchsia
      Alignment = taLeftJustify
      MiddlePartsNum = 231
      Font.Charset = HEBREW_CHARSET
      Font.Color = clWhite
      Font.Height = -16
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ActiveBitmap = 0
    end
    object SliceProgressLabel: TEnhancedLabel [5]
      Left = 30
      Top = 197
      Width = 0
      Height = 0
      CaptionXOffset = 4
      CaptionYOffset = 0
      Transparent = True
      TransparentMode = tmFixed
      TransparentColor = clFuchsia
      Alignment = taLeftJustify
      MiddlePartsNum = 231
      Font.Charset = HEBREW_CHARSET
      Font.Color = clWhite
      Font.Height = -16
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ActiveBitmap = 0
    end
    object HeightProgressLabel: TEnhancedLabel [6]
      Left = 30
      Top = 113
      Width = 0
      Height = 0
      CaptionXOffset = 4
      CaptionYOffset = 0
      Transparent = True
      TransparentMode = tmFixed
      TransparentColor = clFuchsia
      Alignment = taLeftJustify
      MiddlePartsNum = 231
      Font.Charset = HEBREW_CHARSET
      Font.Color = clWhite
      Font.Height = -16
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ActiveBitmap = 0
    end
    object PreviousJobLabel: TEnhancedLabel [7]
      Left = 30
      Top = 239
      Width = 0
      Height = 0
      CaptionXOffset = 4
      CaptionYOffset = 0
      Transparent = True
      TransparentMode = tmFixed
      TransparentColor = clFuchsia
      Alignment = taLeftJustify
      MiddlePartsNum = 231
      Font.Charset = HEBREW_CHARSET
      Font.Color = clWhite
      Font.Height = -16
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ActiveBitmap = 0
    end
    object SubStatusLabel: TEnhancedLabel [8]
      Left = 30
      Top = 155
      Width = 0
      Height = 0
      CaptionXOffset = 0
      CaptionYOffset = 0
      Transparent = True
      TransparentMode = tmFixed
      TransparentColor = 9830550
      Alignment = taCenter
      MiddlePartsNum = 14
      Font.Charset = HEBREW_CHARSET
      Font.Color = clWindowText
      Font.Height = -16
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ActiveBitmap = 0
    end
    object ServiceAlertLabel: TEnhancedLabel [9]
      Left = 30
      Top = 281
      Width = 0
      Height = 0
      CaptionXOffset = 0
      CaptionYOffset = 0
      Transparent = True
      TransparentMode = tmFixed
      TransparentColor = 9830550
      Alignment = taLeftJustify
      MiddlePartsNum = 111
      Font.Charset = HEBREW_CHARSET
      Font.Color = clMaroon
      Font.Height = -13
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ActiveBitmap = 0
      OnClick = ServiceAlertLabelClick
      Visible = False
    end
    object PrintProgressBar: TEnhancedProgressBar [10]
      Left = 565
      Top = 1
      Width = 30
      Height = 500
      TransparentMode = tmFixed
      TransparentColor = 9830550
      BodyBitmapTransparent = False
      ProgressBitmapTransparent = True
      Min = 0
      Max = 100
      Position = 0
      Orientation = pbVertical
      MaxProgressBitmaps = 65
      ReverseDirection = True
      XOffset = 1
      YOffset = -3
    end
    object CabinetAFrame: TEnhancedLabel [11]
      Left = 38
      Top = 247
      Width = 0
      Height = 0
      CaptionXOffset = 4
      CaptionYOffset = 0
      Transparent = True
      TransparentMode = tmFixed
      TransparentColor = clFuchsia
      Alignment = taLeftJustify
      MiddlePartsNum = 231
      Font.Charset = HEBREW_CHARSET
      Font.Color = clWhite
      Font.Height = -16
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ActiveBitmap = 0
    end
    object CabinetBFrame: TEnhancedLabel [12]
      Left = 46
      Top = 255
      Width = 0
      Height = 0
      CaptionXOffset = 4
      CaptionYOffset = 0
      Transparent = True
      TransparentMode = tmFixed
      TransparentColor = clFuchsia
      Alignment = taLeftJustify
      MiddlePartsNum = 231
      Font.Charset = HEBREW_CHARSET
      Font.Color = clWhite
      Font.Height = -16
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ActiveBitmap = 0
    end
    object CabinetALabel: TLabel [13]
      Left = 248
      Top = 137
      Width = 249
      Height = 36
      Alignment = taCenter
      AutoSize = False
      Font.Charset = HEBREW_CHARSET
      Font.Color = clWhite
      Font.Height = -15
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
      Transparent = True
    end
    object CabinetBLabel: TLabel [14]
      Left = 224
      Top = 155
      Width = 247
      Height = 30
      Alignment = taCenter
      AutoSize = False
      Font.Charset = HEBREW_CHARSET
      Font.Color = clWhite
      Font.Height = -15
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
      Transparent = True
    end
    inherited Panel2: TPanel
      Top = 550
      ExplicitTop = 550
      inherited SelectSoftButton: TEnhancedMenu
        Items.Strings = (
          'Menu')
        OnClick = SelectSoftButtonClick
      end
    end
  end
  object S2DrainTimer: TTimer
    Enabled = False
    Interval = 250
    OnTimer = DrainTimerTimer
    Left = 240
    Top = 313
  end
  object M1DrainTimer: TTimer
    Tag = 3
    Enabled = False
    Interval = 250
    OnTimer = DrainTimerTimer
    Left = 337
    Top = 313
  end
  object M2DrainTimer: TTimer
    Tag = 2
    Enabled = False
    Interval = 250
    OnTimer = DrainTimerTimer
    Left = 304
    Top = 313
  end
  object S1DrainTimer: TTimer
    Tag = 1
    Enabled = False
    Interval = 250
    OnTimer = DrainTimerTimer
    Left = 272
    Top = 313
  end
end
