################################################################################################
#
# 	Description: Configuration backup for host script
#
# 	Version:	$Rev: 4494 $
# 	Date:		$Date: 2009-11-19 09:38:39 +0200 (Thu, 19 Nov 2009) $
# 	Path:		$HeadURL: svn://octopus/ControlSW/Trunk/ConnexFamily/Base/PythonScripts/Scripts/~CreateBackupForHost.py $ 
#
################################################################################################

from Q2RT import *
import os,re,zipfile
from datetime import *

# Last ZIP file name
ZIP_FILE_NAME = 'ConfigBackupForHost.zip'

# Regular expression for matching and extracting Backup ZIP files names
FileMatcher = re.compile(r'(\d{2})-(\d{2})-(\d{4})_(\d{2})-(\d{2})\.zip')

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
    mr = FileMatcher.match(i)

    if mr:
      # Create date/time object for each file
                    
      AllFiles.append(i)

  # Return last 15 log files
  return AllFiles[-15:]

# Add file to the zip archieve only if the file exists
def SafeAddFile(ZipFile,FileName):
  if os.access(FileName,os.F_OK):
    ZipFile.write(FileName)

# Create the maintenance info zip file
def CreateZipFile(OutFileName):
  z = zipfile.ZipFile(OutFileName,'w',zipfile.ZIP_DEFLATED)

  # Add config files
  CurrDir = DirRedirection + 'ConfigBackup\\'
  Files = GetFiles(CurrDir)
  for i in Files:
    SafeAddFile(z,CurrDir + i)


  z.close()


# Verify that the output directory exists
if not os.access(OutputDir,os.F_OK):
  # Create sub directory
  os.mkdir(OutputDir)

# Create zip files
CreateZipFile(OutputDir + "\\" + ZIP_FILE_NAME)


