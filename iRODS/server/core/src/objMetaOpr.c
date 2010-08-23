/*** Copyright (c), The Regents of the University of California            ***
 *** For more information please refer to files in the COPYRIGHT directory ***/
/* objMetaOpr.c - metadata operation at the object level */

#ifndef windows_platform
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#endif
#include "objMetaOpr.h"
#include "resource.h"
#include "collection.h"
#include "specColl.h"
#if 0
#include "modDataObjMeta.h"
#include "ruleExecSubmit.h"
#include "ruleExecDel.h"
#include "reSysDataObjOpr.h"
#endif
#include "genQuery.h"
#include "icatHighLevelRoutines.h"
#include "miscUtil.h"
#include "rodsClient.h"
#include "rsIcatOpr.h"


int
svrCloseQueryOut (rsComm_t *rsComm, genQueryOut_t *genQueryOut)
{
    genQueryInp_t genQueryInp;
    genQueryOut_t *junk = NULL;
    int status;

    if (genQueryOut->continueInx <= 0) {
        return (0);
    }

    memset (&genQueryInp, 0, sizeof (genQueryInp_t));

    /* maxRows = 0 specifies that the genQueryOut should be closed */
    genQueryInp.maxRows = 0;;
    genQueryInp.continueInx = genQueryOut->continueInx;

    status =  rsGenQuery (rsComm, &genQueryInp, &junk);

    return (status);
}

int
isData (rsComm_t *rsComm, char *objName, rodsLong_t *dataId)
{
    genQueryInp_t genQueryInp;
    genQueryOut_t *genQueryOut = NULL;
    char tmpStr[MAX_NAME_LEN];
    char logicalEndName[MAX_NAME_LEN];
    char logicalParentDirName[MAX_NAME_LEN];
    int status;

    status = splitPathByKey(objName,
			    logicalParentDirName, logicalEndName, '/');
    memset (&genQueryInp, 0, sizeof (genQueryInp_t));
    snprintf (tmpStr, MAX_NAME_LEN, "='%s'", logicalEndName);
    addInxVal (&genQueryInp.sqlCondInp, COL_DATA_NAME, tmpStr);
    addInxIval (&genQueryInp.selectInp, COL_D_DATA_ID, 1);
    snprintf (tmpStr, MAX_NAME_LEN, "='%s'", logicalParentDirName);
    addInxVal (&genQueryInp.sqlCondInp, COL_COLL_NAME, tmpStr);
    addInxIval (&genQueryInp.selectInp, COL_COLL_ID, 1);
    genQueryInp.maxRows = 2;
    status =  rsGenQuery (rsComm, &genQueryInp, &genQueryOut);
    if (status >= 0) {
        sqlResult_t *dataIdRes;

        if ((dataIdRes = getSqlResultByInx (genQueryOut, COL_D_DATA_ID)) ==
          NULL) {
            rodsLog (LOG_ERROR,
              "isData: getSqlResultByInx for COL_D_DATA_ID failed");
            return (UNMATCHED_KEY_OR_INDEX);
        }
	if (dataId != NULL) {
            *dataId = strtoll (dataIdRes->value, 0, 0);
	}
	freeGenQueryOut (&genQueryOut);
    }

    clearGenQueryInp (&genQueryInp);
    return(status);
}

int
isCollAllKinds (rsComm_t *rsComm, char *objName, rodsLong_t *collId)
{
    dataObjInp_t dataObjInp;
    int status;
    rodsObjStat_t *rodsObjStatOut = NULL;

    bzero (&dataObjInp, sizeof (dataObjInp));
    rstrcpy (dataObjInp.objPath, objName, MAX_NAME_LEN);
    status = collStatAllKinds (rsComm, &dataObjInp, &rodsObjStatOut);
    if (status >= 0 && collId != NULL) {
        *collId = strtoll (rodsObjStatOut->dataId, 0, 0);
    }
    if (rodsObjStatOut != NULL)
        freeRodsObjStat (rodsObjStatOut);
    return status;
}

int
isColl (rsComm_t *rsComm, char *objName, rodsLong_t *collId)
{
    genQueryInp_t genQueryInp;
    genQueryOut_t *genQueryOut = NULL;
    char tmpStr[MAX_NAME_LEN];
    int status;

    memset (&genQueryInp, 0, sizeof (genQueryInp_t));
    snprintf (tmpStr, MAX_NAME_LEN, "='%s'", objName);
    addInxVal (&genQueryInp.sqlCondInp, COL_COLL_NAME, tmpStr);
    addInxIval (&genQueryInp.selectInp, COL_COLL_ID, 1);
    genQueryInp.maxRows = 2;
    status =  rsGenQuery (rsComm, &genQueryInp, &genQueryOut);
    if (status >= 0) {
        sqlResult_t *collIdRes;

        if ((collIdRes = getSqlResultByInx (genQueryOut, COL_COLL_ID)) ==
          NULL) {
            rodsLog (LOG_ERROR,
              "isColl: getSqlResultByInx for COL_D_DATA_ID failed");
            return (UNMATCHED_KEY_OR_INDEX);
        }

        if (collId != NULL) {
            *collId = strtoll (collIdRes->value, 0, 0);
        }
        freeGenQueryOut (&genQueryOut);
    }

    clearGenQueryInp (&genQueryInp);
    return(status);
}

int
isUser(rsComm_t *rsComm, char *objName)
{
    genQueryInp_t genQueryInp;
    genQueryOut_t *genQueryOut = NULL;
    char tmpStr[NAME_LEN];
    int status;

    memset (&genQueryInp, 0, sizeof (genQueryInp_t));
    snprintf (tmpStr, NAME_LEN, "='%s'", objName);
    addInxVal (&genQueryInp.sqlCondInp, COL_USER_NAME, tmpStr);
    addInxIval (&genQueryInp.selectInp, COL_USER_ID, 1);
    genQueryInp.maxRows = 2;
    status =  rsGenQuery (rsComm, &genQueryInp, &genQueryOut);
    freeGenQueryOut (&genQueryOut);
    clearGenQueryInp (&genQueryInp);
    return(status);
}

int
isResc (rsComm_t *rsComm, char *objName)
{
    genQueryInp_t genQueryInp;
    genQueryOut_t *genQueryOut = NULL;
    char tmpStr[NAME_LEN];
    int status;

    memset (&genQueryInp, 0, sizeof (genQueryInp_t));
    snprintf (tmpStr, NAME_LEN, "='%s'", objName);
    addInxVal (&genQueryInp.sqlCondInp, COL_R_RESC_NAME, tmpStr);
    addInxIval (&genQueryInp.selectInp, COL_R_RESC_ID, 1);
    genQueryInp.maxRows = 2;
    status =  rsGenQuery (rsComm, &genQueryInp, &genQueryOut);
    freeGenQueryOut (&genQueryOut);
    clearGenQueryInp (&genQueryInp);
    return(status);
}

int
isRescGroup (rsComm_t *rsComm, char *objName)
{
    genQueryInp_t genQueryInp;
    genQueryOut_t *genQueryOut = NULL;
    char tmpStr[NAME_LEN];
    int status;

    memset (&genQueryInp, 0, sizeof (genQueryInp_t));
    snprintf (tmpStr, NAME_LEN, "='%s'", objName);
    addInxVal (&genQueryInp.sqlCondInp, COL_RESC_GROUP_NAME, tmpStr);
    addInxIval (&genQueryInp.selectInp, COL_RESC_GROUP_ID, 1);
    genQueryInp.maxRows = 2;
    status =  rsGenQuery (rsComm, &genQueryInp, &genQueryOut);
    freeGenQueryOut (&genQueryOut);
    clearGenQueryInp (&genQueryInp);
    return(status);
}

int
isMeta(rsComm_t *rsComm, char *objName)
{
    /* needs to be filled in later */
    return(INVALID_OBJECT_TYPE);
}

int
isToken(rsComm_t *rsComm, char *objName)
{
    /* needs to be filled in later */
    return(INVALID_OBJECT_TYPE);
}

int
getObjType(rsComm_t *rsComm, char *objName, char * objType)
{
    if (isData(rsComm, objName, NULL) >= 0)
      strcpy(objType,"-d");
    else if (isColl(rsComm, objName, NULL) >= 0)
      strcpy(objType,"-c");
    else if (isResc(rsComm, objName) == 0)
      strcpy(objType,"-r");
    else if (isRescGroup(rsComm, objName) == 0)
      strcpy(objType,"-g");
    else if (isUser(rsComm, objName) == 0)
      strcpy(objType,"-u");
    else if (isMeta(rsComm, objName) == 0)
      strcpy(objType,"-m");
    else if (isToken(rsComm, objName) == 0)
      strcpy(objType,"-t");
    else
      return(INVALID_OBJECT_TYPE);
    return (0);
}

int
addAVUMetadataFromKVPairs (rsComm_t *rsComm, char *objName, char *inObjType,
			   keyValPair_t *kVP)
{
  int i,j;
  char  objType[10];
  modAVUMetadataInp_t modAVUMetadataInp;

  bzero (&modAVUMetadataInp, sizeof (modAVUMetadataInp));
  if (strcmp(inObjType,"-1")) {
    strcpy(objType,inObjType);
  }
  else {
    i = getObjType(rsComm, objName,objType);
    if (i < 0)
      return(i);
  }

  modAVUMetadataInp.arg0 = "add";
  for (i = 0; i < kVP->len ; i++) {
     /* Call rsModAVUMetadata to call chlAddAVUMetadata.
        rsModAVUMetadata connects to the icat-enabled server if the
        local host isn't.
     */
    modAVUMetadataInp.arg1 = objType;
    modAVUMetadataInp.arg2 = objName;
    modAVUMetadataInp.arg3 = kVP->keyWord[i];
    modAVUMetadataInp.arg4 = kVP->value[i];
    modAVUMetadataInp.arg5 = "";
    j = rsModAVUMetadata (rsComm, &modAVUMetadataInp);
    if (j < 0)
      return(j);
  }
  return(0);
}

int
getStructFileType (specColl_t *specColl)
{
    if (specColl == NULL) {
	return (-1);
    }

    if (specColl->collClass == STRUCT_FILE_COLL) {  
	return ((int) specColl->type);
    } else {
	return (-1);
    }
}

int
removeAVUMetadataFromKVPairs (rsComm_t *rsComm, char *objName, char *inObjType,
                           keyValPair_t *kVP)
{
  int i,j;
  char  objType[10];
  modAVUMetadataInp_t modAVUMetadataInp;

  if (strcmp(inObjType,"-1")) {
    strcpy(objType,inObjType);
  }
  else {
    i = getObjType(rsComm, objName,objType);
    if (i < 0)
      return(i);
  }

  modAVUMetadataInp.arg0 = "rm";
  for (i = 0; i < kVP->len ; i++) {
     /* Call rsModAVUMetadata to call chlAddAVUMetadata.
        rsModAVUMetadata connects to the icat-enabled server if the
        local host isn't.
     */
    modAVUMetadataInp.arg1 = objType;
    modAVUMetadataInp.arg2 = objName;
    modAVUMetadataInp.arg3 = kVP->keyWord[i];
    modAVUMetadataInp.arg4 = kVP->value[i];
    modAVUMetadataInp.arg5 = "";
    j = rsModAVUMetadata (rsComm, &modAVUMetadataInp);
    if (j < 0)
      return(j);
  }
  return(0);
}

