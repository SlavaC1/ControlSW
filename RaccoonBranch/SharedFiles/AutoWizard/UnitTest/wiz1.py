from Q2RTWizard import *

a = GenericCustomWizardPage('test')
a.SubTitle = 'ran peleg tgfgf gfgd gdgfd nhfghf gdd gdd gddd lkl lml lml'
a.Args = ['Arg #1','Arg #2']
a.ImageID = 0

def GetPages():
  return [a]

def OnPageEvents(Page):
  if Page == a:
    Monitor.NotificationMessage('Hello form python!')
