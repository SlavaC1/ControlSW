################################################################################################
#
# 	Description: Parameters values sanity check
#
# 	Version:	$Rev: 18516 $
# 	Date:		$Date: 2014-05-12 13:42:12 +0300 (Mon, 12 May 2014) $
# 	Path:		$HeadURL: http://srv-ire-svn/svn/ControlSW/Trunk/ObjetFamily/Base/PythonScripts/BIT/Parameters.py $ 
#
################################################################################################

from Q2RT_BIT import *
import CfgValidation
import os
import time

class ParameterTest(SingleTest):

	def GetSteps(Self):
		return ['Testing Parameter Values']

	def Start(Self):
		Self.resultDescription = ""
		Self.errstr = ""
		pass

	def End(Self):
		Self.SetFlagResultWithoutHTML(0)
		#print(str(Self.GetFlagResultWithoutHTML()))
		pass
		

		
		
	def reportErrors(Self, errorsDictionary):

		if len(errorsDictionary) > 0:
			for cfgPath, errors in errorsDictionary.iteritems():
				Self.resultDescription += "Errors in %s:                                  \n" % cfgPath
				Self.resultDescription += "===============================================\n"
				if type(errors) is str:
					Self.resultDescription += errors + '\n'
				else:
					for k,v in errors.iteritems():
						Self.resultDescription += "%s\n" % CfgValidation.formatCfgException(k, v)
				Self.resultDescription += "--------------------------------------------\n\n"
				
	def reportData(Self, dumpLines):
		Self.resultDescription += "                                                     "
		Self.resultDescription += "<BR>Location: "+ Application.AppFilePath + ":<BR><BR>" 
		Self.resultDescription += "<table border=1>\n<BR>"
		Self.resultDescription += "<td cellpadding=2 width=28%>\n"
		Self.resultDescription += "<b><font color=black size=4>File name</font></b>\n"
		Self.resultDescription += "</td>\n"
		Self.resultDescription += "<td cellpadding=2 width=28%>\n"
		Self.resultDescription += "<b><font color=black size=4>Result</font></b>\n"
		Self.resultDescription += "</td>\n"
		Self.resultDescription += "<td cellpadding=2 width=36%>\n"
		Self.resultDescription += "<b><font color=black size=4>Recommended</font></b>\n"
		Self.resultDescription += "<td cellpadding=2 width=8%>\n"
		Self.resultDescription += "<b><font color=black size=4>Conclusion</font></b>\n"
		Self.resultDescription += "</td>\n"
		Self.resultDescription += "</tr>\n"
		i=0
		while (i< len(dumpLines) -1):
			Self.resultDescription += "<td cellpadding=2 >\n"
			Self.resultDescription += dumpLines[i] +"\n"	
			Self.resultDescription += "</td>\n"
			Self.resultDescription += "<td cellpadding=2 >\n"
			Self.resultDescription += dumpLines[i+1] +"\n" 		
			Self.resultDescription += "</td>\n"
			Self.resultDescription += "<td cellpadding=2 >\n"
			Self.resultDescription += dumpLines[i+2] +"\n"
			Self.resultDescription += "<td cellpadding=2 >\n"
			Self.resultDescription += dumpLines[i+3]+"\n"	
			Self.resultDescription += "</td>\n"
			Self.resultDescription += "</tr>\n"
			i+=4
		Self.resultDescription += "</table>\n"
		Self.resultDescription += "</body>\n"
		Self.resultDescription += "</html>\n"
		
	def Execute(Self, Step):
		#Self.resultDescription += "Started at: " + time.strftime("%H:%M:%S", time.localtime()) + "\n"
		#Self.resultDescription += "Current directory: - " + os.getcwd() + "\n"
		errors = {}
		dumpLines = []
		CfgValidation.validateAllCfgFilesInDirectoryTree(Application.AppFilePath + r"Configs", errors, dumpLines)
		CfgValidation.validateAllCfgFilesInDirectoryTree(Application.AppFilePath + r"Modes", errors, dumpLines)

		#Self.reportErrors(errors)
		Self.reportData(dumpLines)
		dumpFilePath = os.path.join(Application.AppFilePath, r"BIT\ParametersTest.html")
		#dumpFilePath = os.path.join(Application.AppFilePath, r"BIT\ParametersTest.txt")
		reportFile = open(dumpFilePath, 'w')
		try:
			reportFile.write("Parameters test results:\n")
			reportFile.write("<html>\n")
			reportFile.write("<head>\n")
			reportFile.write("<title>Parameters test results</title><BR><BR>\n")
			reportFile.write("<font color=Blue size=5><B>Parameters test results:</B></font><BR>\n")
			reportFile.write("</head>\n")
			reportFile.write("<body>\n")
			reportFile.write("Parameters Location: %s:<BR><BR>" %Application.AppFilePath)
			#start table here:
			reportFile.write("<table border=1>\n")
			#reportFile.write("<tr bgcolor=olive><BR>\n") #color the first row of the table (the headings)
			reportFile.write("<td cellpadding=2 width=28%>\n")
			reportFile.write("<font color=red size=4>File name</font>\n")
			reportFile.write("</td>\n")
			reportFile.write("<td cellpadding=2 width=28%>\n")
			reportFile.write("<font color=red size=4>Result</font>\n")
			reportFile.write("</td>\n")
			reportFile.write("<td cellpadding=2 width=36%>\n")
			reportFile.write("<font color=red size=4>Recommended</font>\n")
			reportFile.write("<td cellpadding=2 width=8%>\n")
			reportFile.write("<font color=red size=4>Conclusion</font>\n")
			reportFile.write("</td>\n")
			reportFile.write("</tr>\n")
			i=0
			while (i< len(dumpLines) -1):
				reportFile.write("<td cellpadding=2 >\n")
				reportFile.write("%s\n" 	%dumpLines[i]	)
				reportFile.write("</td>\n")
				reportFile.write("<td cellpadding=2 >\n")
				reportFile.write("%s\n" 	%dumpLines[i+1]	)
				reportFile.write("</td>\n")
				reportFile.write("<td cellpadding=2 >\n")
				reportFile.write("%s\n" 	%dumpLines[i+2]	)
				reportFile.write("<td cellpadding=2 >\n")
				reportFile.write("%s\n"		%dumpLines[i+3]	)
				reportFile.write("</td>\n")
				reportFile.write("</tr>\n")
				i+=4
			reportFile.write("</table>\n")
			reportFile.write("</body>\n")
			reportFile.write("</html>\n")	
			#print("Parameters validation report has been generated in " + dumpFilePath)
		finally:
			reportFile.close()
		
		#Self.SetActualVsRequested('', '', "</a href="+ dumpFilePath +">parameters test result</a>")		
		#Self.SetActualVsRequested('', '', "See <LINK href="+ dumpFilePath +">Test Results .  for details.")
		
		#set the Flag for not transliting to HTML text
		Self.SetFlagResultWithoutHTML(1)
		#print(str(Self.GetFlagResultWithoutHTML()))
		#Self.resultDescription += "Finished at: " + time.strftime("%H:%M:%S", time.localtime())
		Self.SetResultDescription(Self.resultDescription)
		
		pass # TODO: handle exceptions
			
		if 0 == len(errors):
			return trGo 
		else:
			return trNoGo



def GetTests():
	return [ParameterTest('Parameter Sanity Check', taVeryHighPriority)]

