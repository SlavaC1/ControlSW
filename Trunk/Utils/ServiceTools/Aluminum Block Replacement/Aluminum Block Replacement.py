import os
import sys
import subprocess
import csv
import ctypes
from shutil import copyfile

sys.path.append( os.path.join( os.getcwd(), "Pefile" ) )
import pefile

ROOT_DRIVE					= "C:\\"
PARAMS_EX_IM_RELATIVE_PATH 	= "ServiceTools\ParametersExportAndImport\ParamsExportAndImport.exe"
PATH_CSV_FILE		= "ImportList.csv"
PATH_REF_FILE		= "ReplaceList.csv"
INPUT_FOLDER				= os.path.join( os.getcwd(), "INPUTS" )
MessageBox 					= ctypes.windll.user32.MessageBoxA
UPDATE_METHOD_1				= 0 #path to output file
UPDATE_METHOD_2				= 1 #path to output folder

IsMachineInstalled = False

class ParamsUpdater:	
	
	def __init__( self, _inputPath, _cvsReaderCfg, _cvsReaderRef, _paramsExImpPath, _machineName ):
		self.cvsReaderCfg 		= _cvsReaderCfg
		self.cvsReaderRef 		= _cvsReaderRef
		self.paramsExImpPath 	= _paramsExImpPath
		self.inputPath 			= _inputPath
		self.updateMethod		= UPDATE_METHOD_1
		self.machineName		= _machineName
		return
		
	def UpdateCfgUsingExe( self, inputFilePath, outputFilePath ):
		errorCode = subprocess.call([self.paramsExImpPath, "export",inputFilePath,outputFilePath], shell=True )
		if errorCode != 0:
			MessageBox( None, "Error with \"ParamsExportAndImport.exe\"!", 'Error!', 0 )
			sys.exit()
		return
	
	def UpdateCfgManually( self, inputFilePath, outputFilePath ):		
		tempOutputFilePath  = outputFilePath + ".temp"
		inputFile		  	= None
		outputFile			= None
		outputList			= list()
				
		try:
			inputFile = open(inputFilePath,'r')
		except:
			MessageBox( None, "Error! Can't open " + inputFilePath + ". Terminating.", 'Error!', 0 )
			sys.exit()						
		
		#get new parameters
		for line in inputFile:
			if line == "" or line =="\n":
				continue
			if not "\n" in line:
				line= line + "\n"				
			outputList.append(line)		
		inputFile.close()
			
		try:
			outputFile = open(outputFilePath,'r')
		except:
			inputFile.close()
			MessageBox( None, "Error! Can't open " + outputFilePath + ". Terminating.", 'Error!', 0 )
			sys.exit()				
		
		#get old parameters	except ones changed	
		for line in outputFile:
			if line == "" or line =="\n":
				continue
			flag = True
			for item in outputList:
				if item.split("=")[0].lower() == line.split("=")[0].lower():
					flag = False
					break
			if flag:
				if not "\n" in line:
					line = line + "\n"
				outputList.append(line)
		outputFile.close()		
		outputList.sort()
		
		try:
			int(outputList[0])
			item = outputList.pop(0)
			outputList.append(item)
		except:
			None
			
		#write upated params to temp file
		outputFileTemp = open(tempOutputFilePath,'w')	
		for item in outputList:			
			outputFileTemp.write(item)							
		outputFileTemp.close()
		
		#replace old params file with new params file
		os.remove(outputFilePath)
		os.rename(tempOutputFilePath,outputFilePath)
		
		#delete .bak file
		try:
			FNULL = open(os.devnull, 'w')
			subprocess.call( ["del",outputFilePath + ".bak"], shell=True, stdout=FNULL )
		except:
			None 
		
		return

	def UpdateRefs(self):
		for row in self.cvsReaderRef:
			if len( row ) <= 0: #empty line
				continue
				
			inputFilePath 	= os.path.join( self.inputPath, row[0] )
			outputFilePath	= os.path.join( row[1], row[0] ) #file to update to new parameters
			
			if not os.path.isfile( inputFilePath ):
				MessageBox( None, "Error! Can't find " + inputFilePath, 'Error!', 0 )
				sys.exit()				
			if not os.path.isfile( outputFilePath ):
				MessageBox( None, "Error! Can't find " + outputFilePath, 'Error!', 0 )
				sys.exit()				
			copyfile( inputFilePath, outputFilePath )
		return
	
	def UpdateParams(self):		
		row = self.cvsReaderCfg.next()
		
		if row[0].isdigit():
			self.updateMethod = int( row[0] )
		else:
			MessageBox( None, "ImportList.csv under " + self.inputPath + " is corrupted!" , 'Error!', 0 )
			sys.exit()		
				
		for row in self.cvsReaderCfg:		
			if len( row ) < 2: 
				continue			
			isGeneratingChecksum = ( row[0].lower() == "q2rt.cfg" ) or ( self.machineName[:6].lower() == "connex" and ( row[0].lower() == "~permachinedefault.cfg" )	)		
			inputFilePath 	= os.path.join( self.inputPath, row[0] )
			outputFilePath 	= os.path.join( row[1], row[0] )			
			
			if not os.path.isfile( inputFilePath ):
				MessageBox( None, "Error! Can't find  " + inputFilePath, 'Error!', 0 )
				sys.exit()
			if not os.path.exists( outputFilePath ):
				MessageBox( None, "Error! Can't find " + outputFilePath, 'Error!', 0 )
				sys.exit()
			
			self.UpdateCfgManually( inputFilePath, outputFilePath )				
			
			if isGeneratingChecksum:
				
				inputFilePath = outputFilePath + ".aaa"
				FNULL = open(os.devnull, 'w')
				#delete q2rt.cfg.bak
				try:
					subprocess.call( ["del",outputFilePath + ".bak"], shell=True,stdout=FNULL )
				except:
					None 
				#copy q2rt.cfg to q2rt.aaa
				subprocess.call(["copy",outputFilePath,inputFilePath], shell=True, stdout=FNULL )
								
				if self.updateMethod == UPDATE_METHOD_2:
					outputFilePath = row[1]		
				#generate checksum					
				self.UpdateCfgUsingExe( inputFilePath,outputFilePath )				
				#delete q2rt.cfg.aaa
				subprocess.call( ["del",inputFilePath], shell=True, stdout=FNULL )

		self.UpdateRefs()
		return
	
	
def main():
	updatedMachines	= ""	

	#register DLLS
	subprocess.call("Register.bat", shell=True)

	#for each machine installed, update parameters
	for machineName in os.listdir( INPUT_FOLDER ):
		MachineFolderPath 	= os.path.join( ROOT_DRIVE, machineName )
		MachinePath			= os.path.join( MachineFolderPath, machineName + ".exe" )
		if os.path.isdir( MachineFolderPath ):
			IsMachineInstalled = True
		else:
			continue
		
		#get machine version to determine parameters' path. Assumes machine exe file exists
		pe = None
		try:
			pe = pefile.PE(MachinePath)
		except:
			continue
		
		machineVersion	= str(pe.FileInfo[0].StringTable[0].entries['FileVersion']	).split('.')
		machineVersion	= machineVersion[0] + "." + machineVersion[1] + "." + machineVersion[2]
		ParamsExImpPath = os.path.join( MachineFolderPath, PARAMS_EX_IM_RELATIVE_PATH )
		InputPath 		= os.path.join( INPUT_FOLDER, machineName )		
		
		if not os.path.isdir( InputPath ):
			MessageBox( None, "Error! cannot find patch input folder for installation" + machineName + ". Terminating.", 'Error!', 0 )
			sys.exit()
		if not os.path.isfile( ParamsExImpPath ):
			messageStr = "Error! can't find " + ParamsExImpPath
			MessageBox( None, messageStr, 'Error!', 0 )
			sys.exit()		
		
		InputPath = os.path.join( InputPath, machineVersion )
		if not os.path.isdir( InputPath ):
			MessageBox( None, "Error! Version " + machineVersion + " for installation: " + machineName +" Is not supported. Terminating.", 'Error!', 0 )
			sys.exit()
		
		#open csv file that holds paths to prameter files to update
		try:
			cvsfile = open( os.path.join( InputPath, PATH_CSV_FILE ), 'rb' )
			cvsReaderCfg = csv.reader(cvsfile , delimiter=',', quotechar='|')
		except:		
			MessageBox( None, "Error! Can't open " + os.path.join( InputPath, PATH_CSV_FILE ) + ". Terminating.", 'Error!', 0 )
			sys.exit()						
		#open csv file that holds paths to ref files to replace			
		try:
			refFile = open( os.path.join( InputPath, PATH_REF_FILE), 'rb' )
			cvsReaderRef = csv.reader(refFile, delimiter=',', quotechar='|')
		except:		
			MessageBox( None, "Error! Can't open " + str( os.path.join( InputPath, PATH_REF_FILE ) ) + ". Terminating.", 'Error!', 0 )
			sys.exit()
		
		paramsUpdater = ParamsUpdater( InputPath, cvsReaderCfg, cvsReaderRef, ParamsExImpPath, machineName )
		paramsUpdater.UpdateParams()
		refFile.close()
		cvsfile.close()
		updatedMachines = updatedMachines + machineName + ", "
	
	if IsMachineInstalled is False:	
		MessageBox( None, "No 3D printers installed!", 'Patch Done.', 0 )
	else:
		MessageBox( None, "3D printers successfully updated: " + updatedMachines[:-2] + ".", 'Patch Done.', 0 )
			
if __name__ == '__main__': 
 main() 
