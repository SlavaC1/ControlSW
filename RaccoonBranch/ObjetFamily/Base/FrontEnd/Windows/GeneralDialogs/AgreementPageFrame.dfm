object AgreementFrame: TAgreementFrame
  Left = 0
  Top = 0
  Width = 385
  Height = 265
  TabOrder = 0
  object IDoNotAgreeRadioButton: TRadioButton
    Left = 3
    Top = 162
    Width = 113
    Height = 17
    Caption = 'I Do Not Agree'
    Checked = True
    TabOrder = 0
    TabStop = True
  end
  object IAgreeRadioButton: TRadioButton
    Left = 134
    Top = 162
    Width = 113
    Height = 17
    Caption = 'I Agree'
    TabOrder = 1
    TabStop = True
  end
  object ScrollBox1: TScrollBox
    Left = 3
    Top = 4
    Width = 354
    Height = 155
    TabOrder = 2
    object Label1: TLabel
      Left = 3
      Top = 0
      Width = 327
      Height = 78
      Caption = 
        'BY CLICKING ON THE "I AGREE" BUTTON, YOU ARE ACCEPTING THE FOLLO' +
        'WING CONDITION FOR THE SINGLE HEAD REPLACEMENT ("SHR").  IF YOU ' +
        'DO NOT AGREE TO THE FOLLOWING CONDITION, CLICK THE "I DO NOT AGR' +
        'EE" OR THE "CANCEL" BUTTON AND CONTACT OBJET CUSTOMER SUPPORT. '
      WordWrap = True
    end
    object Label2: TLabel
      Left = 53
      Top = 82
      Width = 209
      Height = 13
      Caption = 'IMPORTANT: PLEASE READ CAREFULLY'
    end
    object Label3: TLabel
      Left = 3
      Top = 98
      Width = 330
      Height = 66
      AutoSize = False
      Caption = 
        '1. Printing head(s) should only be replaced in case that a servi' +
        'ce call to Objet'#39's customer support has been initiated, and that' +
        ' troubleshooting performed by Customer according to Objet custom' +
        'er support'#39's instructions, did not result in problem resolution'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -9
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      WordWrap = True
    end
    object Label4: TLabel
      Left = 3
      Top = 153
      Width = 317
      Height = 45
      AutoSize = False
      Caption = 
        '2. Replacement of the printing head(s) should be performed while' +
        ' carefully following the instructions specified in the documenta' +
        'tion provided with each new SHR kit.'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -9
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      WordWrap = True
    end
    object Label5: TLabel
      Left = 3
      Top = 196
      Width = 322
      Height = 43
      AutoSize = False
      Caption = 
        'By checking the "I agree" box below, you indicate your acceptanc' +
        'e of the conditions and compliance with the guidelines and instr' +
        'uctions, specified in the Objet'#39's User Guide.'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -9
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      WordWrap = True
    end
  end
end
