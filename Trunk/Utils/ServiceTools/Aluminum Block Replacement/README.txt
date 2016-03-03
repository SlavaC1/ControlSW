Created by Doron Or


Updating the batch:
-------------------------

To add a new machine:
----------------------------------- 
add a directory under INPUTS named after the machine installation's directory.
Underwhich the version directories will sit.

Adding a new version supported by the batch:
--------------------------------------------
To add a new version y.z.t.? of a machine named "X":
Create a directory under INPUTS\"X" called "y.z.t". Under the directory place CFG, REF, and cvs files as nessecary.

CFG files:
-------------
Sits under version directory. CFG files hold the parameters to update in this format: param_name=x
while param_name is the parameter's name to be updated\added and x is the value given to it.
The file name must be the same as the destination's file's name. (ex: Q2rt.cfg->Q2rt.cfg)

Ref files:
-----------
Sits under version directory. Ref files will hold ALL of the parameters, even those you haven't changed and is named as the destination file. (ex: Q2rt.ref->Q2rt.ref)
They are simply the original ref files with your changes.

ImportList.csv
---------------
Must be present in all version directories. Holds all the cfg files to be updated.
If there are none can be empty but must be present in the directory.
On it's first column\row holds the Update method (more on it later) in a numeric fashion.
The rest will hold the following information:
col0: cfg_file_name, col(0)file_destination_folder_path

example:
row0: 2
row1: Q2rt.cfg, C:\EDEN260V\Configs


ReplaceList.csv
----------------
Must be present in all version directories. Holds all the reffiles to be updated.
If there are none can be empty but must be present in the directory.

Will hold the following information:
col0: ref_file_name, col(0)file_destination_folder_path

example:
row0: Q2rt.ref, C:\EDEN260V\Configs


Update Method:
----------------
The batch uses the machine's "ParamsExportAndImport.exe". 
In different machines the command line for "ParamsExportAndImport.exe" is different.
I've noticed 5 different versions of it numbered: 0,1,2,3.
The best way to determin the correct method is trial and error.
If none of them works the batch file itself needs to be modfied...