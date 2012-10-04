/*** Copyright (c), The Regents of the University of California            ***
 *** For more information please refer to files in the COPYRIGHT directory ***/
/* ncOpen.h
 */

#ifndef NC_OPEN_H
#define NC_OPEN_H

/* This is a NETCDF API call */

#include "rods.h"
#include "rcMisc.h"
#include "procApiRequest.h"
#include "apiNumber.h"
#include "initServer.h"
#include "dataObjInpOut.h"
#ifdef NETCDF_API
#include "netcdf.h"
#endif

typedef struct {
    char objPath[MAX_NAME_LEN];	/* for ncOpenGroup, this is the full group
				 * path */
    int mode;
    int rootNcid;		/* used only for ncOpenGroup */
    rodsULong_t	intialsz;	/* used for nc__open, nc__create */
    rodsULong_t bufrsizehint;	/* used for nc__open, nc__create */
    keyValPair_t condInput;	/* not used */
} ncOpenInp_t;

#define NcOpenInp_PI "str objPath[MAX_NAME_LEN]; int mode; int rootNcid; double intialsz; double bufrsizehint; struct KeyValPair_PI;"

/* data struct for aggregation of netcdf files. Our first attempt assumes
 * the aggregation is based on the time dimension - time series */ 
typedef struct {
    unsigned int startTime;
    unsigned int endTime;
    rodsLong_t arraylen;
    char fileName[MAX_NAME_LEN];
} ncAggElement_t;

typedef struct {
    int numFiles;
    int flags;		/* not used */
    char ncObjectName[MAX_NAME_LEN];
    ncAggElement_t *ncAggElement;	/* pointer to numFiles of 
                                         * ncAggElement_t */
} ncAggrInfo_t;
    
#define NcAggElement_PI "int startTime; int endTime; double arraylen; str fileName[MAX_NAME_LEN];"
#define NcAggrInfo_PI "int numFiles; int flags; str  ncObjectName[MAX_NAME_LEN]; struct *NcAggElement_PI(numFiles);"

#if defined(RODS_SERVER) && defined(NETCDF_API)
#define RS_NC_OPEN rsNcOpen
/* prototype for the server handler */
int
rsNcOpen (rsComm_t *rsComm, ncOpenInp_t *ncOpenInp, int **ncid);
#else
#define RS_NC_OPEN NULL
#endif

#ifdef  __cplusplus
extern "C" {
#endif

/* prototype for the client call */
/* rcNcOpen - netcdf open an iRODS data object (equivalent to nc_open.
 * Input - 
 *   rcComm_t *conn - The client connection handle.
 *   ncOpenInp_t *ncOpenInp - generic nc open/create input. Relevant items are:
 *	objPath - the path of the data object.
 *      mode - the mode of the open - valid values are given in netcdf.h -
 *       NC_NOWRITE (0), NC_WRITE (1)
 *	condInput - condition input (not used).
 * OutPut - 
 *   int the ncid of the opened object - an integer descriptor.   
 */

int
rcNcOpen (rcComm_t *conn, ncOpenInp_t *ncOpenInp, int *ncid);
int
_rcNcOpen (rcComm_t *conn, ncOpenInp_t *ncOpenInp, int **ncid);

#ifdef  __cplusplus
}
#endif

#endif	/* NC_OPEN_H */
