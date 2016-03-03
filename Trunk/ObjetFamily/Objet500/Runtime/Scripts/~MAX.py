################################################################################################
#
# 	Description:
#
# 	Version:	$Rev: 4494 $
# 	Date:		$Date: 2009-11-19 09:38:39 +0200 (Thu, 19 Nov 2009) $
# 	Path:		$HeadURL: svn://octopus/ControlSW/Trunk/ConnexFamily/Base/PythonScripts/Scripts/~MAX.py $ 
#
################################################################################################

from Q2RT import *

Args = AppParams.MaterialPythonArgs.split("\,")
AppParams.MaterialPythonRes = Args[0]
for x in Args:
	if float(x) > float(AppParams.MaterialPythonRes):
		AppParams.MaterialPythonRes = x

