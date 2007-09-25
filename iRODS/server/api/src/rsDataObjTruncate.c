/*** Copyright (c), The Regents of the University of California            ***
 *** For more information please refer to files in the COPYRIGHT directory ***/
/* This is script-generated code (for the most part).  */ 
/* See dataObjTruncate.h for a description of this API call.*/

#include "dataObjTruncate.h"
#include "rodsLog.h"
#include "icatDefines.h"
#include "fileTruncate.h"
#include "unregDataObj.h"
#include "objMetaOpr.h"
#include "rsGlobalExtern.h"
#include "rcGlobalExtern.h"
#include "reGlobalsExtern.h"
#include "reDefines.h"
#include "rmColl.h"
#include "modDataObjMeta.h"
#include "bunSubTruncate.h"

int
rsDataObjTruncate (rsComm_t *rsComm, dataObjInp_t *dataObjTruncateInp)
{
    int status;
    dataObjInfo_t *dataObjInfoHead = NULL;

    dataObjTruncateInp->openFlags = O_WRONLY;  /* set the permission checking */
    status = getDataObjInfoIncSpecColl (rsComm, dataObjTruncateInp, 
      &dataObjInfoHead);

    if (status < 0) return (status);

    status = _rsDataObjTruncate (rsComm, dataObjTruncateInp, dataObjInfoHead);

    return (status);

}

int
_rsDataObjTruncate (rsComm_t *rsComm, dataObjInp_t *dataObjTruncateInp,
dataObjInfo_t *dataObjInfoHead)
{
    int status;
    int retVal = 0;
    dataObjInfo_t *tmpDataObjInfo;

    tmpDataObjInfo = dataObjInfoHead;
    while (tmpDataObjInfo != NULL) {
	status = dataObjTruncateS (rsComm, dataObjTruncateInp, tmpDataObjInfo);
	if (status < 0) {
	    if (retVal == 0) {
	        retVal = status;
	    }
	}
        if (dataObjTruncateInp->specColl != NULL) 	/* do only one */
	    break;
	tmpDataObjInfo = tmpDataObjInfo->next;
    }

    freeAllDataObjInfo (dataObjInfoHead);  

    return (retVal);
}

int
dataObjTruncateS (rsComm_t *rsComm, dataObjInp_t *dataObjTruncateInp,
dataObjInfo_t *dataObjInfo)
{
    int status;

    status = l3Truncate (rsComm, dataObjInfo);

    if (status < 0) {
	int myError = getErrno (status);
        rodsLog (LOG_NOTICE,
          "dataObjTruncateS: l3Truncate error for %s. status = %d",
          dataObjTruncateInp->objPath, status);
	/* allow ENOENT to go on and unregister */
	if (myError != ENOENT && myError != EACCES) {
	    return (status);
	}
    }

    if (dataObjInfo->specColl == NULL) {
	/* reigister the new size */
        keyValPair_t regParam;
	modDataObjMeta_t modDataObjMetaInp;

	memset (&regParam, 0, sizeof (regParam));
	memset (&modDataObjMetaInp, 0, sizeof (modDataObjMetaInp));

        snprintf (tmpStr, MAX_NAME_LEN, "%lld", dataObjTruncateInp->dataSize);
        addKeyVal (&regParam, DATA_SIZE_KW, tmpStr);
	addKeyVal (&regParam, CHKSUM_KW, "");

        modDataObjMetaInp.dataObjInfo = dataObjInfo;
        modDataObjMetaInp.regParam = &regParam;
        status = rsModDataObjMeta (rsComm, &modDataObjMetaInp);
	clearKeyVal (&regParam);
        if (status < 0) {
            rodsLog (LOG_NOTICE,
              "dataObjTruncateS: rsModDataObjMeta error for %s. status = %d",
              dataObjTruncateInp->objPath, status);
	}
    }
    return (status);
}

int
l3Truncate (rsComm_t *rsComm, dataObjInfo_t *dataObjInfo)
{
    int rescTypeInx;
    fileOpenInp_t fileTruncateInp;
    int status;

    if (getBunType (dataObjInfo->specColl) >= 0) {
        subFile_t subFile;
        memset (&subFile, 0, sizeof (subFile));
        rstrcpy (subFile.subFilePath, dataObjInfo->subPath,
          MAX_NAME_LEN);
        rstrcpy (subFile.addr.hostAddr, dataObjInfo->rescInfo->rescLoc,
          NAME_LEN);
        subFile.specColl = dataObjInfo->specColl;
	subFile.offset = fileTruncateInp.dataSize;
        status = rsBunSubTruncate (rsComm, &subFile);
    } else {
        rescTypeInx = dataObjInfo->rescInfo->rescTypeInx;

        switch (RescTypeDef[rescTypeInx].rescCat) {
          case FILE_CAT:
            memset (&fileTruncateInp, 0, sizeof (fileTruncateInp));
            fileTruncateInp.fileType = RescTypeDef[rescTypeInx].driverType;
            rstrcpy (fileTruncateInp.fileName, dataObjInfo->filePath, 
	      MAX_NAME_LEN);
            rstrcpy (fileTruncateInp.addr.hostAddr, 
	      dataObjInfo->rescInfo->rescLoc, NAME_LEN);
	    fileTruncateInp.dataSize = dataObjInfo->dataSize;
            status = rsFileTruncate (rsComm, &fileTruncateInp);
            break;

          default:
            rodsLog (LOG_NOTICE,
              "l3Truncate: rescCat type %d is not recognized",
              RescTypeDef[rescTypeInx].rescCat);
            status = SYS_INVALID_RESC_TYPE;
            break;
	}
    }
    return (status);
}

