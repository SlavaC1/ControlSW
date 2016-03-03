################################################################################################
#
# 	Description:
#
# 	Version:	$Rev: 4494 $
# 	Date:		$Date: 2009-11-19 09:38:39 +0200 (Thu, 19 Nov 2009) $
# 	Path:		$HeadURL: svn://octopus/ControlSW/Trunk/ConnexFamily/Base/PythonScripts/Scripts/DefaultRefFileGenerator.py $ 
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
