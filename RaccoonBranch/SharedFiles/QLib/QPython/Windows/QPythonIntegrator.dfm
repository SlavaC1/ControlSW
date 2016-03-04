object QPythonIntegratorDM: TQPythonIntegratorDM
  OldCreateOrder = False
  OnCreate = DataModuleCreate
  OnDestroy = DataModuleDestroy
  Left = 437
  Top = 345
  Height = 179
  Width = 280
  object PythonEngine1: TPythonEngine
    AutoLoad = False
    DllName = 'python23.dll'
    APIVersion = 1012
    RegVersion = '2.3'
    UseLastKnownVersion = False
    IO = PythonInputOutput1
    Left = 24
    Top = 16
  end
  object PythonInputOutput1: TPythonInputOutput
    OnSendData = PythonInputOutput1SendData
    OnReceiveData = PythonInputOutput1ReceiveData
    Left = 112
    Top = 16
  end
  object PythonModule1: TPythonModule
    Engine = PythonEngine1
    OnInitialization = PythonModule1Initialization
    DocString.Strings = (
      'QLib/Python interface')
    ModuleName = 'QLib'
    Errors = <>
    Left = 24
    Top = 88
  end
end
