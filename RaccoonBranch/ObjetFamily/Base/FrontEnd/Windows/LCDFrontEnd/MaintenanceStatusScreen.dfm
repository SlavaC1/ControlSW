inherited MaintenanceStatusFrame: TMaintenanceStatusFrame
  Width = 650
  Height = 689
  ExplicitWidth = 650
  ExplicitHeight = 689
  inherited Panel1: TPanel
    Width = 650
    Height = 689
    ExplicitWidth = 650
    ExplicitHeight = 689
    inherited PaintBox1: TPaintBox
      Width = 650
      Height = 689
      ExplicitWidth = 650
      ExplicitHeight = 689
    end
    inherited DateLabel: TLabel
      Left = 58
      Top = 502
      ExplicitLeft = 58
      ExplicitTop = 502
    end
    inherited TimeLabel: TLabel
      Left = 166
      Top = 502
      ExplicitLeft = 166
      ExplicitTop = 502
    end
    object HeadsLabel: TEnhancedLabel [3]
      Left = 5
      Top = 29
      Width = 0
      Height = 0
      CaptionXOffset = 2
      CaptionYOffset = 0
      Transparent = True
      TransparentMode = tmFixed
      TransparentColor = clFuchsia
      Alignment = taLeftJustify
      MiddlePartsNum = 135
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clLime
      Font.Height = -17
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ActiveBitmap = 0
      Caption = 'Heads ('#176'C)'
    end
    object TrayLabel: TEnhancedLabel [4]
      Left = 5
      Top = 341
      Width = 0
      Height = 0
      CaptionXOffset = 2
      CaptionYOffset = 0
      Transparent = True
      TransparentMode = tmFixed
      TransparentColor = clFuchsia
      Alignment = taLeftJustify
      MiddlePartsNum = 60
      Font.Charset = HEBREW_CHARSET
      Font.Color = clLime
      Font.Height = -19
      Font.Name = 'Arial'
      Font.Style = []
      ActiveBitmap = 0
      Caption = 'Tray ('#176'C)'
    end
    object AmbientLabel: TEnhancedLabel [5]
      Left = 5
      Top = 265
      Width = 0
      Height = 0
      CaptionXOffset = 2
      CaptionYOffset = 0
      Transparent = True
      TransparentMode = tmFixed
      TransparentColor = clFuchsia
      Alignment = taLeftJustify
      MiddlePartsNum = 90
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clLime
      Font.Height = -17
      Font.Name = 'Arial'
      Font.Style = []
      ActiveBitmap = 0
      Caption = 'Ambient ('#176'C)'
    end
    object UVLampsLabel: TEnhancedLabel [6]
      Left = 5
      Top = 303
      Width = 0
      Height = 0
      CaptionXOffset = 2
      CaptionYOffset = 0
      Transparent = True
      TransparentMode = tmFixed
      TransparentColor = clFuchsia
      Alignment = taLeftJustify
      MiddlePartsNum = 60
      Font.Charset = HEBREW_CHARSET
      Font.Color = clLime
      Font.Height = -16
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ActiveBitmap = 0
      Caption = 'UV Lamps'
    end
    object HeadsLiquidLabel: TEnhancedLabel [7]
      Left = 309
      Top = 227
      Width = 0
      Height = 0
      CaptionXOffset = -4
      CaptionYOffset = 0
      Transparent = True
      TransparentMode = tmFixed
      TransparentColor = clFuchsia
      Alignment = taLeftJustify
      MiddlePartsNum = 60
      Font.Charset = HEBREW_CHARSET
      Font.Color = clLime
      Font.Height = -17
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ActiveBitmap = 0
      Caption = 'Heads Liquid'
    end
    object TrayTemperatureLabel: TEnhancedLabel [8]
      Left = 158
      Top = 341
      Width = 0
      Height = 0
      CaptionXOffset = 0
      CaptionYOffset = 0
      Transparent = True
      TransparentMode = tmFixed
      TransparentColor = 9830550
      Alignment = taCenter
      MiddlePartsNum = 5
      Font.Charset = HEBREW_CHARSET
      Font.Color = clBlack
      Font.Height = -16
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ActiveBitmap = 0
      Caption = '???'
    end
    object AmbientTemperatureLabel: TEnhancedLabel [9]
      Left = 158
      Top = 265
      Width = 0
      Height = 0
      CaptionXOffset = 0
      CaptionYOffset = 0
      Transparent = True
      TransparentMode = tmFixed
      TransparentColor = clFuchsia
      Alignment = taCenter
      MiddlePartsNum = 5
      Font.Charset = HEBREW_CHARSET
      Font.Color = clLime
      Font.Height = -17
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ActiveBitmap = 0
      Caption = '???'
      Visible = False
    end
    object HeadsVacuumTemperatureLabel: TEnhancedLabel [10]
      Left = 462
      Top = 265
      Width = 0
      Height = 0
      CaptionXOffset = 0
      CaptionYOffset = 0
      Transparent = True
      TransparentMode = tmFixed
      TransparentColor = clFuchsia
      Alignment = taCenter
      MiddlePartsNum = 5
      Font.Charset = HEBREW_CHARSET
      Font.Color = clLime
      Font.Height = -17
      Font.Name = 'Verdana'
      Font.Style = [fsBold]
      ActiveBitmap = 0
      Caption = '???'
      Visible = False
    end
    object HeadsVacuumLabel: TEnhancedLabel [11]
      Left = 309
      Top = 265
      Width = 0
      Height = 0
      CaptionXOffset = -4
      CaptionYOffset = 0
      Transparent = True
      TransparentMode = tmFixed
      TransparentColor = clFuchsia
      Alignment = taLeftJustify
      MiddlePartsNum = 60
      Font.Charset = HEBREW_CHARSET
      Font.Color = clLime
      Font.Height = -17
      Font.Name = 'Verdana'
      Font.Style = [fsBold]
      ActiveBitmap = 0
      Caption = 'Heads Vacuum'
    end
    object PreHeaterFrontLabel: TEnhancedLabel [12]
      Left = 309
      Top = 303
      Width = 0
      Height = 0
      CaptionXOffset = -4
      CaptionYOffset = 0
      Transparent = True
      TransparentMode = tmFixed
      TransparentColor = clFuchsia
      Alignment = taLeftJustify
      MiddlePartsNum = 60
      Font.Charset = HEBREW_CHARSET
      Font.Color = clLime
      Font.Height = -17
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ActiveBitmap = 0
      Caption = 'Pre Heater Front'
    end
    object WasteEnhancedLabel: TEnhancedLabel [13]
      Left = 5
      Top = 227
      Width = 0
      Height = 0
      CaptionXOffset = 2
      CaptionYOffset = 0
      Transparent = True
      TransparentMode = tmFixed
      TransparentColor = clFuchsia
      Alignment = taLeftJustify
      MiddlePartsNum = 60
      Font.Charset = HEBREW_CHARSET
      Font.Color = clLime
      Font.Height = -17
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ActiveBitmap = 0
      Caption = 'Waste (gr)'
    end
    object WasteWeightEnhancedLabel: TEnhancedLabel [14]
      Left = 158
      Top = 227
      Width = 0
      Height = 0
      CaptionXOffset = 0
      CaptionYOffset = 0
      Transparent = True
      TransparentMode = tmFixed
      TransparentColor = 9830550
      Alignment = taCenter
      MiddlePartsNum = 5
      Font.Charset = HEBREW_CHARSET
      Font.Color = clBlack
      Font.Height = -17
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ActiveBitmap = 0
    end
    object SupportHeaterLabel: TEnhancedLabel [15]
      Left = 238
      Top = 113
      Width = 0
      Height = 0
      CaptionXOffset = 0
      CaptionYOffset = 0
      Transparent = True
      TransparentMode = tmFixed
      TransparentColor = clFuchsia
      Alignment = taCenter
      MiddlePartsNum = 10
      Font.Charset = HEBREW_CHARSET
      Font.Color = clLime
      Font.Height = -19
      Font.Name = 'Arial'
      Font.Style = []
      ActiveBitmap = 0
      Caption = 'S'
    end
    object ModelHeaterLabel: TEnhancedLabel [16]
      Left = 542
      Top = 113
      Width = 0
      Height = 0
      CaptionXOffset = 0
      CaptionYOffset = 0
      Transparent = True
      TransparentMode = tmFixed
      TransparentColor = clFuchsia
      Alignment = taCenter
      MiddlePartsNum = 10
      Font.Charset = HEBREW_CHARSET
      Font.Color = clLime
      Font.Height = -19
      Font.Name = 'Arial'
      Font.Style = []
      ActiveBitmap = 0
      Caption = 'M'
    end
    object PreHeaterRearLabel: TEnhancedLabel [17]
      Left = 317
      Top = 311
      Width = 0
      Height = 0
      CaptionXOffset = -4
      CaptionYOffset = 0
      Transparent = True
      TransparentMode = tmFixed
      TransparentColor = clFuchsia
      Alignment = taLeftJustify
      MiddlePartsNum = 60
      Font.Charset = HEBREW_CHARSET
      Font.Color = clLime
      Font.Height = -17
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ActiveBitmap = 0
      Caption = 'Pre Heater Rear'
    end
    inherited Panel2: TPanel
      Top = 510
      ExplicitTop = 510
      inherited SelectSoftButton: TEnhancedMenu
        Items.Strings = (
          'Back')
        OnClick = SelectSoftButtonClick
      end
    end
  end
end
