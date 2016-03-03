object SetupForm: TSetupForm
  Left = 2
  Top = 54
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSingle
  Caption = 'System Setup'
  ClientHeight = 431
  ClientWidth = 968
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poMainFormCenter
  OnCreate = FormCreate
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object OKBitBtn: TBitBtn
    Left = 487
    Top = 389
    Width = 89
    Height = 33
    Caption = 'OK'
    Default = True
    ModalResult = 1
    TabOrder = 0
    OnClick = OKBitBtnClick
    Glyph.Data = {
      36030000424D3603000000000000360000002800000010000000100000000100
      1800000000000003000000000000000000000000000000000000FF00FFFF00FF
      FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00
      FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFA9A0915D6634B3A89BFF00FFFF
      00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF
      FF00FF69784940C35B527E3DC1B8ACFF00FFFF00FFFF00FFFF00FFFF00FFFF00
      FFFF00FFFF00FFFF00FFFF00FFFF00FFB0AE9C4B994659E9914DD275547936CC
      CBBFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF
      748C5C5CCF7B7DE8AA6EE8A252CE75567C38D7D9CFFF00FFFF00FFFF00FFFF00
      FFFF00FFFF00FFFF00FFFF00FFC6CFBB43913C88EEB495F3C489EBB77AECAE51
      CC73578038E2E6DDFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF81A270
      6DCE81A0FAD15DCD7779E6A494F3C87DF2B750C86E5D8744FF00FFFF00FFFF00
      FFFF00FFFF00FFFF00FFDEE6D843913897FCC95EC573407C254095366DE39790
      F9CC7DF7C04CC2656A9453FF00FFFF00FFFF00FFFF00FFFF00FF8EB6845BD07B
      60D5805D984CFF00FFC4D4BD438F3052CB6E81F9C57BFECA45BF5E709F5CFF00
      FFFF00FFFF00FFFF00FF419E3B45D063579E4BFF00FFFF00FFFF00FFDBE6D869
      A2593BB3476FF2B273FFD040BA5481B074FF00FFFF00FFFF00FF2C96234CA241
      FF00FFFF00FFFF00FFFF00FFFF00FFFF00FF9DC2942D9C255AE8956AFFD239B7
      4A88BA81FF00FFFF00FFDFEDDEFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF
      00FFFF00FFCCE1C83E9F3443D46F66FCBD35B6479BC998FF00FFFF00FFFF00FF
      FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF6CB36530BD
      485CF5A32CB03BB2D8B0FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF
      00FFFF00FFFF00FFFF00FFFF00FF9ECF9E33B23E45DD6A2BAF2FFF00FFFF00FF
      FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00
      FFD5EAD544B4471EA91FFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF
      00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF}
  end
  object GroupBox7: TGroupBox
    Left = 487
    Top = 8
    Width = 233
    Height = 121
    Caption = 'Containers'
    TabOrder = 1
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
      AlignWithMargins = True
      Left = 5
      Top = 18
      Width = 137
      Height = 98
      Align = alLeft
      ItemIndex = 0
      Items.Strings = (
        'None'
        'Direct to'
        'Through Embedded')
      TabOrder = 1
      OnClick = ContainersRadioGroupClick
    end
  end
  object GroupBox2: TGroupBox
    Left = 487
    Top = 135
    Width = 233
    Height = 121
    Caption = 'LCD/Keyboard'
    TabOrder = 2
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
      AlignWithMargins = True
      Left = 5
      Top = 18
      Width = 137
      Height = 98
      Align = alLeft
      ItemIndex = 0
      Items.Strings = (
        'None'
        'Direct to'
        'Through Embedded')
      TabOrder = 1
      OnClick = LCDRadioGroupClick
    end
  end
  object GroupBox3: TGroupBox
    Left = 248
    Top = 135
    Width = 233
    Height = 121
    Caption = 'MCB'
    TabOrder = 3
    object MCBComboBox: TComboBox
      Left = 148
      Top = 56
      Width = 73
      Height = 21
      Style = csDropDownList
      ItemHeight = 0
      TabOrder = 0
      OnDropDown = ComComboBoxDropDown
    end
    object MCBRadioGroup: TRadioGroup
      AlignWithMargins = True
      Left = 5
      Top = 18
      Width = 137
      Height = 98
      Align = alLeft
      ItemIndex = 0
      Items.Strings = (
        'None'
        'Direct to'
        'Through Embedded')
      TabOrder = 1
      OnClick = MCBRadioGroupClick
    end
  end
  object GroupBox4: TGroupBox
    Left = 726
    Top = 8
    Width = 233
    Height = 121
    Caption = 'OCB'
    TabOrder = 4
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
      AlignWithMargins = True
      Left = 5
      Top = 18
      Width = 137
      Height = 98
      Align = alLeft
      ItemIndex = 0
      Items.Strings = (
        'None'
        'Direct to'
        'Through Embedded')
      TabOrder = 1
      OnClick = OCBRadioGroupClick
    end
  end
  object GroupBox5: TGroupBox
    Left = 726
    Top = 135
    Width = 233
    Height = 121
    Caption = 'OHDB'
    TabOrder = 5
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
      AlignWithMargins = True
      Left = 5
      Top = 18
      Width = 137
      Height = 98
      Align = alLeft
      ItemIndex = 0
      Items.Strings = (
        'None'
        'Direct to'
        'Through Embedded'
        'Through OCB')
      TabOrder = 1
      OnClick = OHDBRadioGroupClick
    end
  end
  object GroupBox6: TGroupBox
    Left = 248
    Top = 8
    Width = 233
    Height = 121
    Caption = 'Embedded'
    TabOrder = 6
    object EmbeddedComboBox: TComboBox
      Left = 148
      Top = 57
      Width = 73
      Height = 21
      Style = csDropDownList
      ItemHeight = 0
      TabOrder = 0
      OnDropDown = ComComboBoxDropDown
    end
    object EmbeddedRadioGroup: TRadioGroup
      AlignWithMargins = True
      Left = 5
      Top = 18
      Width = 137
      Height = 98
      Align = alLeft
      ItemIndex = 0
      Items.Strings = (
        'None'
        'Direct to')
      TabOrder = 1
      OnClick = EmbeddedRadioGroupClick
    end
  end
  object BitBtn1: TBitBtn
    Left = 392
    Top = 389
    Width = 89
    Height = 33
    Cancel = True
    Caption = 'Cancel'
    ModalResult = 2
    TabOrder = 7
    Glyph.Data = {
      36030000424D3603000000000000360000002800000010000000100000000100
      1800000000000003000000000000000000000000000000000000FF00FFFF00FF
      7878D25C5CC9FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF6D6D
      D88888E4FF00FFFF00FFFF00FF6969CD0808B70B0BBA4141C0FF00FFFF00FFFF
      00FFFF00FFFF00FFFF00FF5050CF3A3AEE4242F78181E6FF00FF8D8DD90909B5
      0808B70B0BBA0E0EBD4141C1FF00FFFF00FFFF00FFFF00FF4C4CCB3131E43838
      EC3D3DF23F3FF48888E37F7FD55656CE0B0BB70A0AB90D0DBC1010C04242C3FF
      00FFFF00FF4A4AC82828DA2E2EE03333E63737EB3838EC6D6DD7FF00FF6868CD
      5757CF0D0DB90C0CBB0F0FBF1313C34343C34646C62020D12525D62929DB2D2D
      E03030E34F4FCEFF00FFFF00FFFF00FF6868CD5757D00E0EBB0E0EBE1111C115
      15C51919C91D1DCE2121D22424D62727D94C4CCBFF00FFFF00FFFF00FFFF00FF
      FF00FF6969CD5858D01010BC1010BF1313C31616C61919CA1C1CCD2020D14949
      C8FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF6969CE4F4FCF0E0EBD10
      10C01313C31616C61818C94646C5FF00FFFF00FFFF00FFFF00FFFF00FFFF00FF
      FF00FFFF00FF4E4EC33030C41F1FC11010BE1010C01212C21515C54343C3FF00
      FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF4F4FC43636C43434C53232C531
      31C62A2AC52222C42121C52121C64C4CC5FF00FFFF00FFFF00FFFF00FFFF00FF
      5050C43B3BC43838C43636C43434C53232C56A6AD73333C73030C73030C82F2F
      C94C4CC4FF00FFFF00FFFF00FF5353C54040C53D3DC53B3BC43838C43636C44E
      4EC37070D07272D93434C63131C63030C63030C74E4EC4FF00FF8787D95656CC
      4343C64040C53D3DC53B3BC44F4FC4FF00FFFF00FF7171D07373D83535C53232
      C53232C53232C56969CCA6A6E39898E65555CC4343C64040C55050C4FF00FFFF
      00FFFF00FFFF00FF7171D07474D83737C53434C43434C48484D6FF00FF9F9FE2
      9898E65656CC5353C5FF00FFFF00FFFF00FFFF00FFFF00FFFF00FF7171D07676
      D83A3AC47B7BD3FF00FFFF00FFFF00FFA6A6E38787D9FF00FFFF00FFFF00FFFF
      00FFFF00FFFF00FFFF00FFFF00FF8585D79797DCFF00FFFF00FF}
  end
  object GroupBox8: TGroupBox
    Left = 248
    Top = 262
    Width = 233
    Height = 121
    Caption = 'OCB Simulator'
    TabOrder = 8
    object OCBSimComboBox: TComboBox
      Left = 148
      Top = 52
      Width = 73
      Height = 21
      Style = csDropDownList
      ItemHeight = 0
      TabOrder = 0
      OnDropDown = ComComboBoxDropDown
    end
    object OCBSimRadioGroup: TRadioGroup
      AlignWithMargins = True
      Left = 5
      Top = 18
      Width = 137
      Height = 98
      Align = alLeft
      ItemIndex = 0
      Items.Strings = (
        'None'
        'Direct to')
      TabOrder = 1
      OnClick = OCBSimRadioGroupClick
    end
  end
  object GroupBox9: TGroupBox
    Left = 487
    Top = 262
    Width = 233
    Height = 121
    Caption = 'RFID'
    TabOrder = 9
    object RFIDRadioGroup: TRadioGroup
      AlignWithMargins = True
      Left = 5
      Top = 18
      Width = 137
      Height = 98
      Align = alLeft
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
      ItemHeight = 0
      TabOrder = 1
      OnDropDown = ComComboBoxDropDown
    end
  end
  object LogGroupbox: TGroupBox
    Left = 8
    Top = 8
    Width = 233
    Height = 375
    Caption = 'Log sections'
    TabOrder = 10
  end
end
