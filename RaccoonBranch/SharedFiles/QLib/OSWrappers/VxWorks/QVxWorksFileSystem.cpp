/********************************************************************
 *                        Objet Geometries LTD.                     *
 *                        ---------------------                     *
 * Project: QLib                                                    *
 * Module: QLib OS wrappers                                         *
 * Module Description: Cross-Platform file-system utilities and     *
 *                     support.                                     *
 *                                                                  *
 * Compilation: VxWorks, C++.                                       *
 *                                                                  *
 * Author: Ran Peleg.                                               *
 * Start date: 06/02/2002                                           *
 * Last upate: 06/02/2002                                           *
 ********************************************************************/

#include <drv/hdisk/ataDrv.h>
#include <usrLib.h>
#include <blkIo.h>

#include "QFileSystem.h"
#include "QErrors.h"


// Initialize the local file system
TQErrCode QFileSystemInit(char Partition)
{
  int rc = OK;
  int status;
  BLK_DEV  *atapBlk;
  DOS_VOL_DESC DosVolDscr;

  char PartStr[]="/?:/";
  PartStr[1] = Partition;

  status = usrAtaConfig(0,0,PartStr);

  if(status != OK)
    return Q_FILE_SYS_INIT_ERR;

  atapBlk = ataDevCreate(1,0,0,0);

  DosVolDscr = dosFsDevInit(PartStr,atapBlk,0);

 // if(DosVolDscr == NULL)
 //   return Q_FILE_SYS_INIT_ERR;

  return Q_NO_ERROR;
}

// Change the current work directory
TQErrCode QChangeDirectory(QString Path)
{
  QString VxWorksPath = QConvertPath(Path);

  if(cd(const_cast<char *>(VxWorksPath.c_str())) != OK)
    return Q_CHANGE_DIR_ERR;

  return Q_NO_ERROR;
}

// Convert a DOS/Windows path to local platform path
QString QConvertPath(QString DosPath)
{
  QString VxWorksPath = "/" + DosPath;

  for(int i = 0; i < VxWorksPath.length(); i++)
    if(VxWorksPath[i] == '\\')
      VxWorksPath[i] = '/';

  return VxWorksPath;
}

