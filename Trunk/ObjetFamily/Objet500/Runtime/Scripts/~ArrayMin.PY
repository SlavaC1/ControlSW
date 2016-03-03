################################################################################################
#
# 	Description:
#
# 	Version:	$Rev: 4494 $
# 	Date:		$Date: 2009-11-19 09:38:39 +0200 (Thu, 19 Nov 2009) $
# 	Path:		$HeadURL: svn://octopus/ControlSW/Trunk/ConnexFamily/Base/PythonScripts/Scripts/~ArrayMin.PY $ 
#
################################################################################################

from Q2RT import *

Args = AppParams.MaterialPythonArgs.split("\,")
ElementSize = len(Args) / AppParams.MaterialPythonArraySize
Res = range(0, ElementSize, 1)
for x in Res:
        Res[x] = min(Args[x::ElementSize])
AppParams.MaterialPythonRes = ",".join(Res)


