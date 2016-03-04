################################################################################################
#
# 	Description:
#
# 	Version:	$Rev: 12462 $
# 	Date:		$Date: 2012-07-31 16:13:43 +0300 (Tue, 31 Jul 2012) $
# 	Path:		$HeadURL: http://srv-ire-svn/svn/ControlSW/Branches/Slava/RaccoonForObjet260Release/ObjetFamily/Base/PythonScripts/Scripts/DefaultRefFileGenerator.py $ 
#
################################################################################################

from Q2RT import *
import CfgValidation

"""
!!!! WARNING !!!! 

This script generates a default *.ref file for each *.cfg file in the Objet Embedded applicatin directory tree.
ALL THE EXISTING *.ref WILL BE OVERWRITTEN.

"""

CfgValidation.createRefFromAllCfgFilesInDirectoryTree(Application.AppFilePath)
