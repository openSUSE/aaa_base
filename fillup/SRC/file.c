
/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Copyrights to S.u.S.E. GmbH Fuerth (c) 1996                                */
/*                                                                            */
/* Time-stamp: <96/04/23 11:14:11 maddin>				      */
/* Project:    fillup							      */
/* Module:     file							      */
/* Filename:   file.c							      */
/* Author:     Martin Scherbaum (maddin)				      */
/* Description:                                                               */
/*                                                                            */
/*----------------------------------------------------------------------------*/

/*--------------------------------- IMPORTS ----------------------------------*/

#include "import.h"
#include "defs.h"
#include "vars.h"
#include "err.h"

/*--------------------------------- EXPORTS ----------------------------------*/

#include "export.h"
#include "file.h"

/*--------------------------------- DEFINES ----------------------------------*/
/*--------------------------------- MACROS -----------------------------------*/
/*---------------------------------- TYPES -----------------------------------*/
/*-------------------------------- VARIABLES ---------------------------------*/
/*-------------------------------- FUNCTIONS ---------------------------------*/


/*----------------------------- IMPLEMENTATION -------------------------------*/

/*-------------------- readFile --------------------*/
GLOBAL INT readFile (FILE *fp, 
		     char *filename,
		     char **bufPtr)
{
  long fileLen = 0;
  long readLen = 0;
  char *buffer = NULL;

  /* check if fp is an open file */
  if (fp == NULL)
  {
    printMess (stderr, QUIET, 
	       "%s> file '%s' is not open.\n", 
	       progname, filename);
    return -1;
  }

  /* go to the end */
  fseek (fp, 0, SEEK_END);

  /* get the file length */
  fileLen = ftell (fp);

  if (fileLen == 0)
  {
    printMess (stderr, VERBOSE, 
	       "%s> warning: file '%s' has no content.\n", 
	       progname, filename);
    return 0;
  }

  /* get a buffer for the file */
  if ((buffer = malloc (fileLen + 1)) == NULL)
  {
    printMess (stderr, QUIET, 
	       "%s> could not get memory for read buffer.\n", 
	       progname);
    return -1;
  }

  /* reset the buffer */
  memset (buffer, EOF, fileLen + 1);

  /* read from the start to the buffer */
  fseek (fp, 0, SEEK_SET);

  if ((readLen = fread (buffer, sizeof (char), fileLen, fp)) != fileLen)
  {
    printMess (stderr, QUIET, 
	       "%s> could not read from file '%s'.\n", 
	       progname, filename);
    free (buffer);
    return -1;
  }

  *bufPtr = buffer;

  /* return the found buffer, don't forget to free it later! */
  return readLen;
}

/*-------------------- writeFile --------------------*/
GLOBAL int writeFile (FILE      *fp,
		      char      *filename,
		      VAR_ARRAY *array,
		      int       arrayCnt)
{
  int n;
  int cnt = 0;

  /* check if fp is an open file */
  if (fp == NULL)
  {
    printMess (stderr, QUIET, 
	       "%s> file '%s' not open.\n", 
	       progname, filename);
    return -1;
  }

  /* write all entries */
  for (n = 0; n < arrayCnt; n++)
  {
    /* take only marked entries */
    if (array[n].copy)
    {
      /* check for presence of a comment */
      if (array[n].comment != NULL)
      {
	fprintf (fp, "%s\n", 
		 array[n].comment);
      }

      /* print the variable name and delimitor */
      fprintf (fp, "%s%s", 
	       array[n].name, 
	       delim); 

      /* print the value if there's one */
      if (array[n].value != NULL)
      {
	fprintf (fp, "%s", array[n].value);
      }

      /* everything is ended by a newline */
      fprintf (fp, "\n");

      cnt++;
    }
  }

  /* tell how many variables were written (for exit code)*/
  return cnt;
}

/*----------------------------------------------------------------------------*/





