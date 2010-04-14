/*** Copyright (c), The Regents of the University of California            ***
 *** For more information please refer to files in the COPYRIGHT directory ***/
/* rsBulkDataObjPut.c. See bulkDataObjReg.h for a description of
 * this API call.*/

#include "apiHeaderAll.h"
#include "objMetaOpr.h"
#include "dataObjOpr.h"
#include "miscServerFunct.h"
#include "rcGlobalExtern.h"
#include "reGlobalsExtern.h"

int
rsBulkDataObjPut (rsComm_t *rsComm, dataObjInp_t *dataObjInp,
bytesBuf_t *dataObjInpBBuf)
{
    int status;
    int remoteFlag;
    rodsServerHost_t *rodsServerHost;
    specCollCache_t *specCollCache = NULL;

    resolveLinkedPath (rsComm, dataObjInp->objPath, &specCollCache,
      &dataObjInp->condInput);

    remoteFlag = getAndConnRemoteZone (rsComm, dataObjInp, &rodsServerHost,
      REMOTE_CREATE);

    if (remoteFlag < 0) {
        return (remoteFlag);
    } else if (remoteFlag == LOCAL_HOST) {
        status = _rsBulkDataObjPut (rsComm, dataObjInp, dataObjInpBBuf);
    } else {
        status = rcBulkDataObjPut (rodsServerHost->conn, dataObjInp,
          dataObjInpBBuf);
    }
    return status;
}

int
_rsBulkDataObjPut (rsComm_t *rsComm, dataObjInp_t *dataObjInp,
bytesBuf_t *dataObjInpBBuf)
{
    int status;
    int remoteFlag;
    rodsServerHost_t *rodsServerHost;
    rescInfo_t *rescInfo;
    char *inpRescGrpName;
    char phyBunDir[MAX_NAME_LEN];
    rescGrpInfo_t *myRescGrpInfo = NULL;
    int flags = BULK_OPR_FLAG;

    inpRescGrpName = getValByKey (&dataObjInp->condInput, RESC_GROUP_NAME_KW);

    /* query rcat for resource info and sort it */

    status = getRescGrpForCreate (rsComm, dataObjInp, &myRescGrpInfo);
    if (status < 0) return status;

    /* just take the top one */
    rescInfo = myRescGrpInfo->rescInfo;

    remoteFlag = resolveHostByRescInfo (rescInfo, &rodsServerHost);

    if (remoteFlag == REMOTE_HOST) {
        addKeyVal (&dataObjInp->condInput, DEST_RESC_NAME_KW,
          rescInfo->rescName);
	if (inpRescGrpName == NULL && 
	  strlen (myRescGrpInfo->rescGroupName) > 0) {
            addKeyVal (&dataObjInp->condInput, RESC_GROUP_NAME_KW,
              myRescGrpInfo->rescGroupName);
	}
        if ((status = svrToSvrConnect (rsComm, rodsServerHost)) < 0) {
            return status;
        }
        status = rcBulkDataObjPut (rodsServerHost->conn, dataObjInp, 
          dataObjInpBBuf);
	return status;
    }
    status = chkCollForExtAndReg (rsComm, dataObjInp->objPath);
    if (status < 0) return status;

    status = createBunDirForBulkPut (rsComm, dataObjInp, rescInfo, phyBunDir);
    if (status < 0) return status;

    status = untarBuf (phyBunDir, dataObjInpBBuf);
    if (status < 0) {
        rodsLog (LOG_ERROR,
          "_rsBulkDataObjPut: untarBuf for dir %s. stat = %d",
          phyBunDir, status);
        return status;
    }

    if (strlen (myRescGrpInfo->rescGroupName) > 0) 
        inpRescGrpName = myRescGrpInfo->rescGroupName;

    if (getValByKey (&dataObjInp->condInput, FORCE_FLAG_KW) != NULL) {
        flags = flags | FORCE_FLAG_FLAG;
    }

    status = regUnbunSubfiles (rsComm, rescInfo, inpRescGrpName,
      dataObjInp->objPath, phyBunDir, flags);

    if (status == CAT_NO_ROWS_FOUND) {
        /* some subfiles have been deleted. harmless */
        status = 0;
    } else if (status < 0) {
        rodsLog (LOG_ERROR,
          "_rsBulkDataObjPut: regUnbunPhySubfiles for dir %s. stat = %d",
          phyBunDir, status);
    }

    return status;
}

int
createBunDirForBulkPut (rsComm_t *rsComm, dataObjInp_t *dataObjInp,
rescInfo_t *rescInfo, char *phyBunDir)
{
    dataObjInfo_t dataObjInfo;
    struct stat statbuf;
    int status;

    if (dataObjInp == NULL || rescInfo == NULL || phyBunDir == NULL) 
	return USER__NULL_INPUT_ERR;

    bzero (&dataObjInfo, sizeof (dataObjInfo));
    rstrcpy (dataObjInfo.objPath, dataObjInp->objPath, MAX_NAME_LEN);
    rstrcpy (dataObjInfo.rescName, rescInfo->rescName, NAME_LEN);
    dataObjInfo.rescInfo = rescInfo;

    status = getFilePathName (rsComm, &dataObjInfo, dataObjInp);
    if (status < 0) {
        rodsLog (LOG_ERROR,
          "createBunDirForBulkPut: getFilePathName err for %s. status = %d",
          dataObjInp->objPath, status);
        return (status);
    }
    do {
	snprintf (phyBunDir, MAX_NAME_LEN, "%s/%s.%d", dataObjInfo.filePath,
	  TMP_PHY_BUN_DIR, (int) random ());
	status =  stat (phyBunDir, &statbuf);
    } while (status == 0);

    mkdirR ("/", phyBunDir, getDefDirMode ());

    return 0;
}

