################################################################################################
#
# 	Description:
#
# 	Version:	$Rev: 4494 $
# 	Date:		$Date: 2009-11-19 09:38:39 +0200 (Thu, 19 Nov 2009) $
# 	Path:		$HeadURL: svn://octopus/ControlSW/Releases/Connex500_57.1.X.X/ConnexFamily/Base/PythonScripts/Scripts/~MIN.py $ 
#
################################################################################################

from Q2RT import *

Args = AppParams.MaterialPythonArgs.split("\,")
AppParams.MaterialPythonRes = Args[0]
for x in Args:
	if float(x) < float(AppParams.MaterialPythonRes):
		AppParams.MaterialPythonRes = x

