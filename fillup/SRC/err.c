
/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Copyrights to S.u.S.E. GmbH Fuerth (c) 1996                                */
/*                                                                            */
/* Time-stamp: <96/04/17 14:03:26 maddin>				      */
/* Project:    fillup							      */
/* Module:     err							      */
/* Filename:   err.c							      */
/* Author:     Martin Scherbaum (maddin)				      */
/* Description:                                                               */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/

/*--------------------------------- IMPORTS ----------------------------------*/

#include "import.h"
#include <stdarg.h>             /* for printErr                               */

/*--------------------------------- EXPORTS ----------------------------------*/

#include "export.h"
#include "err.h"

/*--------------------------------- DEFINES ----------------------------------*/
/*--------------------------------- MACROS -----------------------------------*/
/*---------------------------------- TYPES -----------------------------------*/
/*-------------------------------- VARIABLES ---------------------------------*/
/*-------------------------------- FUNCTIONS ---------------------------------*/


/*----------------------------- IMPLEMENTATION -------------------------------*/

GLOBAL BOOLEAN printMess (FILE *fp,
			  INT  level,
                          CHAR *fmt,
                          ...)
{
  va_list args;                 /* variable list of arguments */
  
  va_start (args, fmt);

  /* if the level of the messaging system is low enough, then print */
  if (verboseMode >= level)
  {
    vfprintf (fp, fmt, args);
  }
  
  va_end   (args);

  fflush (fp);

  return TRUE;
}

/*----------------------------------------------------------------------------*/


