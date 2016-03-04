################################################################################################
#
# 	Description:
#
# 	Version:	$Rev: 12462 $
# 	Date:		$Date: 2012-07-31 16:13:43 +0300 (Tue, 31 Jul 2012) $
# 	Path:		$HeadURL: http://srv-ire-svn/svn/ControlSW/Branches/Slava/RaccoonForObjet260Release/ObjetFamily/Base/PythonScripts/Scripts/~MIN.py $ 
#
################################################################################################

from Q2RT import *

Args = AppParams.MaterialPythonArgs.split("\,")
AppParams.MaterialPythonRes = Args[0]
for x in Args:
	if float(x) < float(AppParams.MaterialPythonRes):
		AppParams.MaterialPythonRes = x

