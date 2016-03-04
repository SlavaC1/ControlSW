################################################################################################
#
# 	Description: Data collection script
#
# 	Version:	$Rev: 12462 $
# 	Date:		$Date: 2012-07-31 16:13:43 +0300 (Tue, 31 Jul 2012) $
# 	Path:		$HeadURL: http://srv-ire-svn/svn/ControlSW/Branches/Slava/RaccoonForObjet260Release/ObjetFamily/Base/PythonScripts/Scripts/~CollectData.py $ 
#
################################################################################################

from Q2RT import *
import os,re,zipfile
from datetime import *

# computers' Name
ComputerName = '(' + Application.ComputerName + ')'

# Last ZIP file name
LAST_FILE_NAME = 'ServiceData' + ComputerName + '.zip'

# Log file regular expression group numbers
G_DAY,G_MONTH,G_YEAR,G_HOUR,G_MIN = range(1,6)

# Output directory for the ZIP files
OutputDir = Application.AppFilePath + 'ServiceData'

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

def FindTextFileRecursively(BaseDir):
	AllFiles = []
	LogFileMatcher = re.compile('.*\.txt')	
	# Verify path
	if not os.access(BaseDir,os.F_OK):
		return AllFiles
	Temp = os.listdir(BaseDir)
	for i in Temp:
		fullI = BaseDir + '\\' + i
		IsDir = os.path.isdir(fullI)
		if not IsDir:
			mr =  LogFileMatcher.match(fullI.lower())
			if mr:
				AllFiles.append(fullI)
		else:
			AllFiles += FindTextFileRecursively(fullI)
	return AllFiles

# Return a list of the last two log files (...or 1 ...or 0)
def FindTwoLastLogFiles(BaseDir):
  # Verify path
  if not os.access(BaseDir,os.F_OK):
    return []
  Temp = os.listdir(BaseDir)
  
  LogFiles = []
  # Regular expression for matching and extracting log files names
  LogFileMatcher = re.compile(r'(\d{2})-(\d{2})-(\d{2})-(\d{2})-(\d{2})\.log')
  for i in Temp:
    mr =  LogFileMatcher.match(i)

    if mr:
      # Create date/time object for each file
      dt = datetime(int(mr.group(G_YEAR)) + 2000,int(mr.group(G_MONTH)),int(mr.group(G_DAY)),
                    int(mr.group(G_HOUR)),int(mr.group(G_MIN)),0)
                    
      LogFiles.append((i,dt))

  # Sort log files list by date  
  LogFiles.sort(lambda a,b: cmp(a[1],b[1]))
    
  # Return last two log files
  return LogFiles[-2:]

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

  # Add Modes files  
  List = ModesManager.GetModesDirectoriesList().split(",")
  for Dir in List:
	CurrDir = DirRedirection + 'Modes\\' + Dir + "\\"
	Files = GetFiles(CurrDir)
	for i in Files:
		SafeAddFile(z,CurrDir + i)

  # add two last log files
  LogDir = 'Log\\'
  LastLogs = FindTwoLastLogFiles(LogDir)

  for i in LastLogs:
    SafeAddFile(z,LogDir + i[0])

  ServiceToolsDir = 'ServiceTools'
  ServiceToolsTxtFiles = FindTextFileRecursively(ServiceToolsDir)
  for i in ServiceToolsTxtFiles:
	SafeAddFile(z,i)
	
  z.close()


# Create a zip file name according to the current date and time
CurrTime = datetime.today()
ZipFileName = '%.2d-%.2d-%.4d_%.2d-%.2d' % (CurrTime.day,CurrTime.month,CurrTime.year,CurrTime.hour,CurrTime.minute)
ZipFileName += ComputerName + '.zip'

# Verify that the output directory exists
if not os.access(OutputDir,os.F_OK):
  # Create sub directory
  os.mkdir(OutputDir)

# Create zip files
CreateZipFile(OutputDir + '\\' + ZipFileName)
CreateZipFile(OutputDir + '\\' + LAST_FILE_NAME)


