################################################################################################
#
# 	Description:
#
# 	Version:	$Rev: 4494 $
# 	Date:		$Date: 2009-11-19 09:38:39 +0200 (Thu, 19 Nov 2009) $
# 	Path:		$HeadURL: svn://octopus/ControlSW/Trunk/ConnexFamily/Base/PythonScripts/Scripts/ParametersValidation.py $ 
#
################################################################################################

from Q2RT import *
from Q2RT_BIT import *
import CfgValidation
import os
import time

"""
This script performs validation(sanity checks) of Objet parameter (*.cfg) files.
The validation is done by comparing each non-string parameter value in each *.cfg file to the range for that parameter
defined in a Parameter Reference (*.ref) file with the same name in the same folder as the *.cfg file.
The output is written to <ApplicationDir>\BIT\ParametersTest.txt

Use CfgValidation.createRefFromAllCfgFilesInDirectoryTree(path) and createRefFromCfg(path) to generate default *.ref files.

See documentation in <ApplicationDir>\Lib\CfgValidation.py for more details.
"""
#space =""
errors = {}
dumpLines = []
CfgValidation.validateAllCfgFilesInDirectoryTree(Application.AppFilePath + r"Configs", errors, dumpLines)
CfgValidation.validateAllCfgFilesInDirectoryTree(Application.AppFilePath + r"Modes", errors, dumpLines)

#dumpFilePath = os.path.join(Application.AppFilePath, r"BIT\ParametersTest.txt")
dumpFilePath = os.path.join(Application.AppFilePath, r"BIT\ParametersTest.html")

# Exceptions in the following code are caught by the Objet Embedded application environment.
reportFile = open(dumpFilePath, 'w') 
try:

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
	reportFile.write("<td cellpadding=2 width=52%>\n")
	reportFile.write("<font color=red size=4>File name</font>\n")
	reportFile.write("</td>\n")
	reportFile.write("<td cellpadding=2 width=30%>\n")
	reportFile.write("<font color=red size=4>Result</font>\n")
	reportFile.write("</td>\n")
	reportFile.write("<td cellpadding=2 width=10%>\n")
	reportFile.write("<font color=red size=4>Recommended</font>\n")
	reportFile.write("<td cellpadding=2 width=8%>\n")
	reportFile.write("<font color=red size=4>Conclusion</font>\n")
	reportFile.write("</td>\n")
	reportFile.write("</tr>\n")
    #
	i=0 
	while (i< len(dumpLines) - 1):
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
	#
	reportFile.write("</table>\n")
	reportFile.write("</body>\n")
	reportFile.write("</html>\n")
	#reportFile.writelines(dumpLines)
	#reportFile.write("done.")
	print("Parameters validation report has been generated in " + dumpFilePath)
finally:
	reportFile.close()