################################################################################################
#
# 	Description:
#
# 	Version:	$Rev: 4494 $
# 	Date:		$Date: 2009-11-19 09:38:39 +0200 (Thu, 19 Nov 2009) $
# 	Path:		$HeadURL: svn://octopus/ControlSW/Trunk/ConnexFamily/Base/PythonScripts/Scripts/~SmartAppend.py $ 
#
################################################################################################

from Q2RT import *

Args = AppParams.MaterialPythonArgs.split("\,")
Res = ''
for x in Args:
	if x != '':
		if Res.find(x) == -1:
			Res += x + '\n\r'
AppParams.MaterialPythonRes = Res