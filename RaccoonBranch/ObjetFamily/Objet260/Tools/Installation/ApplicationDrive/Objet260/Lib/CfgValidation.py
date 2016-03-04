import string
import os
import sys
import fpformat


"""
This library module provides an API for automatic validation(sanity checks) of Objet parameter (*.cfg) files.
The validation is done by comparing each non-string parameter value in each *.cfg file to the range for that parameter
defined in a Parameter Reference (*.ref) file with the same name in the same folder as the *.cfg file.

Main sevices:
- To validate all *.cfg files in a directory use the function validateAllCfgFilesInDirectoryTree
- To validate a single *.cfg file use the function compareCfgToRef
- To generate *.ref files for all *.cfg files in a directory tree, use createRefFromAllCfgFilesInDirectoryTree
- To generate a *.ref file for a single *.cfg file, use createRefFromCfg

See more documentation below, near each function.
"""

# ========================= Exceptions ================================

class BadCfgFileFormatException(Exception):
	def __init__(self, value):
		self.value = value
	def __str__(self):
		return repr(self.value)

class FileAccessException(Exception):
	def __init__(self, value):
		self.value = value
	def __str__(self):
		return repr(self.value)

# =========================== Utility funcs ================================
		
def readFileLinesToList(path):
	try:
		file = open(path, 'r')
		try:
			cfgLinesList = file.readlines() # reads all the lines of the file into a list
		finally:
			file.close()			
	except IOError, (errno, strerror):
		raise FileAccessException("I/O error(%s): %s. Path: %s" % (errno, strerror, path))
	
	return cfgLinesList

class prettyfloat(float):
	def __repr__(self):
		return "%0.2f" % self	

def IsSmaller(a, b):
	x = int(a * 100)
	y = int(b * 100)
	if(x<y):
		return True
	return False
	
def IsBigger(a, b):
	x = int(a * 100)
	y = int(b * 100)
	if(x>y):
		return True
	return False
		
def cfgLinesToDictionary(cfgLinesList):

	dictionary = {} # creates an empty dictionary
	lineNum = 1
	for line in cfgLinesList:
		line = line.split(';')[0] # get rid of comments

		line = line.strip(' \n\t')
		if 0 == len(line): continue # skip empty lines
		
		equalSignPos = line.find('=')
		if equalSignPos < 1: continue # ignore non- key=value  lines
		
		key = line[:equalSignPos].strip(' \t')
		rawValue = line[equalSignPos + 1 :]

		value = parseStringValue(rawValue)
		l= list()
		#newTuple=tuple()
		if type(value) is tuple:
			f = 0
			for val in value:
				if type(val) is float: #if val is float
					f = 1
			#create correct result:
			if f ==1:
				newTuple = tuple (map(prettyfloat, value))
				dictionary[key] = newTuple
				lineNum = lineNum + 1
			else:
				dictionary[key] = value
				lineNum = lineNum + 1
		else:	
			dictionary[key] = value
			lineNum = lineNum + 1

	return dictionary
# end of readCfgFileToDictionary

def parseStringValue(stringValue):
	stringValue = stringValue.strip(' \n\t')
	if "" == stringValue: return ""
	if (string.digits + '-{').find(stringValue[0]) != -1: # if startrs with a digit a '{' or a '-'
		try:
			value = eval(stringValue)
		except:
			value = stringValue
	else:
		value = stringValue
	return value
# end of parseStringValue

def formatCfgException(key, cfgExceptionData):
	if cfgExceptionData.has_key('recommended'):
		return "%s = %s \t Recommended value: %s" % (key, str(cfgExceptionData['value']), str(cfgExceptionData['recommended']))
	else:
		return "%s = %s \t Recommended range: %s - %s" % \
								(key, str(cfgExceptionData['value']), str(cfgExceptionData['min']), str(cfgExceptionData['max']))

def formatRetrunValue(key, cfgExceptionData):
	if cfgExceptionData.has_key('recommended'):
		if type(cfgExceptionData['value']) is float:
			return "%s = %s \t" % (key, fpformat.fix(cfgExceptionData['value'], 2))
		else:
			return "%s = %s \t" % (key, str(cfgExceptionData['value']))
	else:
		if type(cfgExceptionData['value']) is float:
			return "%s = %s \t" % (key, fpformat.fix(cfgExceptionData['value'], 2))
		else:
			return "%s = %s \t" % (key, str(cfgExceptionData['value']))
								
def formatRetrunRecomm(key, cfgExceptionData):
	if cfgExceptionData.has_key('recommended'):
		return "%s" % (str(cfgExceptionData['recommended']))
	else:
		return "%s - %s" % (str(cfgExceptionData['min']), str(cfgExceptionData['max']))															
# ===================================== High Level Operations ======================================
#breaks a list of resin names separeted by " ," to a list of resin names
def returnResinNames(ResinName):
	listResin = string.split(ResinName, ',')
	return listResin
	#for resin in listResin:
		#print ("resin: " + resin)

#retrun 1 if the resin exists in the list of parameters and 0 otherwise. 
def findResininList(path,ResinName,RefListResins):
	#ref file handle
	#check if the ResinName contains couple of resin names
	numResin = 0 
	listResin = []
	listResin = returnResinNames(ResinName)
	numResin = len(listResin)
	result = 0 
	stringPath = path + "\BIT\Ref.ini"
	txt = open(stringPath).read()
	resins = string.split(txt, '[')
	INResinList = 0 
	for resin in listResin:
		for resinInRef in RefListResins:
			if(resinInRef == resin): #the resin exist in the list
				result = result + 1
	
	
	if(result == numResin):
		return 1
		
	return 0	

def compareCfgToRef(cfg, ref,pathApp):

	class InvalidReferenceException:
		pass
		
	class OutOfLimitsException:
		pass

	d = {}
	ListError=[]
	for k,v in ref.iteritems():
		try:
			
			isOutOfLimits = False
			if type(v) is str:
				if (cfg[k] != v):
					d[k] = {'value':cfg[k], 'recommended':v}
				continue
			
			#GoodKeys[k] = {'value':cfg[k], 'recommended':v}
			
			#conversion from int to float if one of the numbers are float:
			#if(type(v['base']) == int and (type(v['downDelta']) == float or type(v['upDelta']== float))):
			#	v['base'] = float(v['base'])
			if(type(v['base']) == float and (type(v['downDelta']) == int or type(v['upDelta']== int))):
				v['downDelta'] = float(v['downDelta'] )	
				v['upDelta']   = float(v['downDelta'] ) 
				
				
			if (type(v['base']) != type(v['downDelta']) or type(v['base']) != type(v['upDelta']) )and v['str'] != 1:
				raise InvalidReferenceException
				
			if (v['str'] != 1):	
				if type(v['base']) is tuple:
					if len(v['base']) != len(v['downDelta']) or len(v['base']) != len(v['upDelta']):
						raise InvalidReferenceException
					
					
					min = tuple(map(prettyfloat,(map(lambda a,b: a - b, v['base'], v['downDelta']))))
					max = tuple(map(prettyfloat,(map(lambda a,b: a + b, v['base'], v['upDelta']))))
				
					if (type(v['base']) != type(cfg[k]) ):
						isOutOfLimits = True
					elif (len(v['base']) != len(cfg[k])):
						isOutOfLimits = True
					else:
						for val, m, M in zip(cfg[k], min, max):
							if ( IsSmaller( M, m )):
								raise InvalidReferenceException
								
							if ( IsSmaller(val,m) or IsSmaller(M,val) ):
								isOutOfLimits = True
								break
							
				else:
					if v['downDelta'] < 0 or v['upDelta'] < 0:
						raise InvalidReferenceException
					
					min = (v['base'] - v['downDelta'])
					max = (v['base'] + v['upDelta'])
					if (cfg[k] < min) or (max < cfg[k]):
						isOutOfLimits = True

					
				if 	isOutOfLimits:
					if min != max:
						d[k] = {'value':cfg[k], 'min':min,  'max':max}
					else:
						d[k] = {'value':cfg[k], 'recommended':min}
			
			#chekcs strings: string to string(one to one)
			else:
				
				if(v['str'] != 0):
					if(v['list'] == 0 and v['no check'] == 0): #serach in a pre-defined list
						if v['base'] != cfg[k]:
							#wrong string:
							d[k] = {'value':cfg[k], 'recommended':v['base']}
							
					elif(v['list'] == 1): #serach in a pre-defined list	
						ResinExist = findResininList(pathApp,cfg[k],v['data'])
						if (ResinExist == 0): #doesn't exists
							d[k] = {'value':cfg[k], 'recommended':v['data']}
						#ResinExist = findResininList(Application.AppFilePath,cfg[k])
					
		except KeyError:
			if type(v) is str:
				d[k] = {'value':None, 'recommended':v}
			else:
				if min == max:
					d[k] = {'value':None, 'recommended':min}
				else:
					d[k] = {'value':None, 'min':min, 'max':max}
					
		except InvalidReferenceException:
			d[k] = {'value':cfg[k], 'recommended': "INVALID REFERENCE"}
				
				
		
	return d

# end of compareCfgToRef

#sort the dict - and retrun a list
def sortDict(DictToSort):
	#returns a dictionary sorted by keys """
	our_list = DictToSort.items()
	our_list.sort()
	return 	our_list


# end of sortDict
	
def validateAllCfgFilesInDirectoryTree(path, cfgFileErrorsDictionary, dumpLines):
	#length of path:
	#global space
	lenPath =len( path ) 
	#print("path: " +path[-2:])
	if (path[-2:] == 'es'): #modes
		lenPath -=5 
	if (path[-2:] == 'gs'): #configs
		lenPath -=7
	
	pathApp = path[:lenPath] 	
	#print ("lenPath: " +str(lenPath) )
	for root, dirs, files in os.walk(path):
		for file in files:
			if file[-4:] == ".cfg":
				try:
					cfgPath = os.path.join(root, file)
					refPath = os.path.join(root, file[:-4] + ".ref")
					refLines = readFileLinesToList(refPath)
					cfgLines = readFileLinesToList(cfgPath)
					ref = cfgLinesToDictionary(refLines)
					cfg = cfgLinesToDictionary(cfgLines)
					badKeys  = compareCfgToRef(cfg, ref, pathApp)
					if len(badKeys) > 0:
						
						#for sorting Dict - return a list of sorted items 
						badKeys_list = sortDict(badKeys)
						cfgFileErrorsDictionary[cfgPath] = badKeys
						#for k,v in badKeys.iteritems():
						for item in badKeys_list:
							dumpLines += ["<b>%s</b><BR>" %cfgPath[lenPath:]]
							#dumpLines += ["\t%s<BR>" % formatCfgException(k, v) for k,v in badKeys.iteritems()]						
							dumpLines += ["%s" % formatRetrunValue(item[0],item[1]  ) ]
							dumpLines += ["%s" % formatRetrunRecomm(item[0],item[1] ) ]
							dumpLines += ["<b>NOT OK</b>"]
					else:
						
						dumpLines += ["%s\n" %cfgPath[lenPath:]]
						dumpLines += ["&nbsp<BR>\n" ] #alignment with space
						dumpLines += ["&nbsp<BR>\n" ] #alignment with space
						dumpLines += ["OK<BR>\n" ] #alignment with space
						
				except FileAccessException, e:
					cfgFileErrorsDictionary[cfgPath] = e.value
					dumpLines += ["%s<BR>\n" % e.value]
					dumpLines += ["&nbsp<BR>\n"   ]
					dumpLines += ["&nbsp<BR>\n"   ]
					dumpLines += ["%NOT OK<BR>\n" ]
				except TypeError:
					raise BadCfgFileFormatException('In ' + cfgPath + ' or ' + refPath) # TODO: should tell in which line is the problem
#end of validateAllCfgFilesInDirectoryTree

# ============================= Functions for Test cases generation ========================
#breaks a list of resin names separeted by " ," to a list of resin names
def ListResins(path):
	listResin = []
	stringPath = path + "\BIT\Ref.ini"
	txt = open(stringPath).read()
	sects = string.split(txt, '[')
	INResinList = 0 
	for sect in sects:
		lines = string.split(sect, '\n') 
		for line in lines:
			lenLine = len(line) - 1
			if(line[lenLine:] == ']'): #section name
				sectionName = line[:lenLine]
				if (sectionName == 'Resin'): # now check if one of the lines is the nedded
					INResinList =1
				else:
					INResinList =0
			elif(INResinList == 1): #we are in the resin list
				listResin += [line]	
								
	return listResin
#retrun name of section if the parameters exist in the section
def findParameterSecInIniFile(path,parameterName):
	#ref file handle
	stringPath = path + "\BIT\Ref.ini"
	txt = open(stringPath).read()
	sects = string.split(txt, '[')
	for sect in sects:
		lines = string.split(sect, '\n') 
		for line in lines:
			lenLine = len(line) - 1
			if(line[lenLine:] == ']'): #section name
					sectionName = line[:lenLine]
					#print("sectionName: " + sectionName)
			if (line == parameterName): #found the parameter we are looking for
				#print("parameterName: " + parameterName + "\n")
				return sectionName
	return '0'	
	
def multiplyBy0(val):
	if type(val) is tuple:
		return tuple(map(lambda x: x*0, val))
	elif type(val) is int:
		return 0
	else:
		return 0.0

def createRefFromCfg(path,p):
	d = cfgLinesToDictionary(readFileLinesToList(path))
	outPath = path[:-4] + ".ref" # TODO: extract func
	f = open(outPath, 'w')
	returnList = []
	for k,v in d.iteritems():
		try:
			if not type(v) is str:
				delta = multiplyBy0(v)
				#ref = {'base':v, 'downDelta':delta, 'upDelta':delta, 'str':0}
				ref = "{" 
				ref +="'base': " + str(v) + ", "
				ref +="'upDelta': " + str(delta) + ", "
				ref +="'downDelta': " + str(delta) + ", "
				ref +="'str': 0"
				ref +="}"
				f.write(k + '=' + str(ref) + '\n')
			else: #in case of string - how to create the ref file
				#check if it is a  one value/list/no checking:
				res = findParameterSecInIniFile(p,k)
				returnList = ListResins(p) 
				#check for each "\": change to "\\"(double)
				v = v.replace("\\","\\\\")
				if (res == 'ListParameters' ):
					#ref = {'base':v, 'downDelta':0, 'upDelta':0, 'str':1, 'list': 1, 'no check':0, 'data': returnList}
					ref = "{" 
					ref +="'base': '" + str(v) + "', "
					ref +="'upDelta': 0, "
					ref +="'downDelta': 0, "
					ref +="'str': 1, "
					ref +="'list': 1, " 
					ref +="'no check': 0"
					ref +="'data': '" + str(returnList) + "'"
					ref +="}"
				elif (res == 'NoCheckParameters'):
					#ref = {'base':v, 'downDelta': 0, 'upDelta': 0, 'str':1, 'list': 0, 'no check':1}
					ref = "{" 
					ref +="'base': '" + str(v) + "', "
					ref +="'upDelta': 0, "
					ref +="'downDelta': 0, "
					ref +="'str': 1, "
					ref +="'list': 0, " 
					ref +="'no check': 1"
					ref +="}"
				else:
					#ref = {'base':v, 'downDelta':0, 'upDelta':0, 'str':1, 'list': 0, 'no check':0}
					ref = "{" 
					ref +="'base': '" + str(v) + "', "
					ref +="'upDelta': 0, "
					ref +="'downDelta': 0, "
					ref +="'str': 1, "
					ref +="'list': 0, " 
					ref +="'no check': 0"
					ref +="}"

				f.write(k + '=' + str(ref) + '\n')
		except:
			print("Could not create reference for file: " +path +" parameter: "+ k +" value: '" + v + "'\n")
			
	f.close
	
def createRefFromAllCfgFilesInDirectoryTree(path):
	for root, dirs, files in os.walk(path):
		for file in files:
			if file[-4:] == ".cfg":
				createRefFromCfg(os.path.join(root, file),path)
				
			
# os.chdir(r"C:\SVN\EdenCapture\trunk\Quadra2\Eden500\ProjectFiles\Windows")
# d = {}
# l = []
# validateAllCfgFilesInDirectoryTree(r"C:\SVN\EdenCapture\trunk\Quadra2\Eden500\ProjectFiles\Windows\Modes", d, l)
# validateAllCfgFilesInDirectoryTree(r"C:\SVN\EdenCapture\trunk\Quadra2\Eden500\ProjectFiles\Windows\Configs", d, l)
#validateAllCfgFilesInDirectoryTree(r"Modes", d, l)
# print l
