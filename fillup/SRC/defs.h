
/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Copyrights to S.u.S.E. GmbH Fuerth (c) 1996                                */
/*                                                                            */
/* Time-stamp: <96/04/03 17:55:01 maddin>				      */
/* Project:    fillup							      */
/* Module:     defs							      */
/* Filename:   defs.h							      */
/* Author:     Martin Scherbaum (maddin)				      */
/* Description:                                                               */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/

#ifndef __DEFS_H__
#define __DEFS_H__

/*---------------------------------- DEFINES ---------------------------------*/

#define DELIM_LEN      5
#define DEF_DELIM      "="
#define DEF_COMMENT    "#"

#define BUF_LEN        10000
#define FILENAME_LEN   256

#define NO_COMMENT     0
#define SAVE_COMMENT   1

#define VARNAME_NUM    500
#define VARNAME_LEN    50

/*---------------------------------- MACROS ----------------------------------*/
/*----------------------------------- TYPES ----------------------------------*/

typedef enum {

  QUIET,
  NORMAL,
  VERBOSE

} VERB_MODE;

typedef struct {

  int  copy;
  char name[VARNAME_LEN];
  char *value;
  char *comment;

} VAR_ARRAY;

/*--------------------------------- VARIABLES --------------------------------*/
/*--------------------------------- FUNCTIONS --------------------------------*/

#endif /* __DEFS_H__ */

/*----------------------------------------------------------------------------*/


