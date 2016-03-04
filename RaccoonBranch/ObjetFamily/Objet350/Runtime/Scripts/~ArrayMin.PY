################################################################################################
#
# 	Description:
#
# 	Version:	$Rev: 12462 $
# 	Date:		$Date: 2012-07-31 16:13:43 +0300 (Tue, 31 Jul 2012) $
# 	Path:		$HeadURL: http://srv-ire-svn/svn/ControlSW/Branches/Slava/RaccoonForObjet260Release/ObjetFamily/Base/PythonScripts/Scripts/~ArrayMin.PY $ 
#
################################################################################################

from Q2RT import *

Args = AppParams.MaterialPythonArgs.split("\,")
ElementSize = len(Args) / AppParams.MaterialPythonArraySize
Res = range(0, ElementSize, 1)
for x in Res:
        Res[x] = min(Args[x::ElementSize])
AppParams.MaterialPythonRes = ",".join(Res)


