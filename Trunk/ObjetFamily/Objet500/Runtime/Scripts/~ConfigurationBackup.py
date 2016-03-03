################################################################################################
#
# 	Description: Configuration backup script
#
# 	Version:	$Rev: 4494 $
# 	Date:		$Date: 2009-11-19 09:38:39 +0200 (Thu, 19 Nov 2009) $
# 	Path:		$HeadURL: svn://octopus/ControlSW/Trunk/ConnexFamily/Base/PythonScripts/Scripts/~ConfigurationBackup.py $ 
#
################################################################################################

from Q2RT import *
import os,os.path,re,zipfile
from datetime import *

# Last ZIP file name
LAST_FILE_NAME = 'ConfigBackup.zip'

# Output directory for the ZIP files
OutputDir = Application.AppFilePath + 'ConfigBackup'

# only for debug
DirRedirection = ''

# Return a list of all the files in a given directory
def GetFiles(BaseDir):
  # Verify path
  if not os.access(BaseDir,os.F_OK):
    return []
    
  Temp = os.listdir(BaseDir)
  
  AllFiles = []
  for i in Temp:
    IsDir = os.path.isdir(BaseDir + i)
    if not IsDir: 
      AllFiles.append(i)

  return AllFiles

# Add file to the zip archieve only if the file exists
def SafeAddFile(ZipFile,FileName):
  if os.access(FileName,os.F_OK):
    ZipFile.write(FileName)

# Create the maintenance info zip file
def CreateZipFile(OutFileName):
  z = zipfile.ZipFile(OutFileName,'w',zipfile.ZIP_DEFLATED)

  # Add config files
  CurrDir = DirRedirection + 'Configs\\'
  Files = GetFiles(CurrDir)
  for i in Files:
    SafeAddFile(z,CurrDir + i)

  # Add General Modes files
  List = ModesManager.GetModesDirectoriesList().split(",")
  for Dir in List:
	CurrDir = DirRedirection + 'Modes\\' + Dir + "\\"
	Files = GetFiles(CurrDir)
	for i in Files:
		SafeAddFile(z,CurrDir + i)

  # add two last log files

  # Add UV History Log File
  SafeAddFile(z,r'ServiceTools\UV Lamps\UV Lamps History.txt')

  # Add Service Notepad File
  SafeAddFile(z,r'ServiceTools\Service Notepad\Service Notepad.txt')

  z.close()

# Create a zip file name according to the current date and time
CurrTime = datetime.today()
ZipFileName = '%.2d-%.2d-%.4d_%.2d-%.2d.zip' % (CurrTime.day,CurrTime.month,
                                                CurrTime.year,CurrTime.hour,CurrTime.minute)

# Verify that the output directory exists
if not os.access(OutputDir,os.F_OK):
  # Create sub directory
  os.mkdir(OutputDir)

# Create zip files
CreateZipFile(OutputDir + "\\" + ZipFileName)
CreateZipFile(OutputDir + "\\" + LAST_FILE_NAME)


