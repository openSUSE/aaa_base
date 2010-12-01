
/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Copyrights to S.u.S.E. GmbH Fuerth (c) 1996                                */
/*                                                                            */
/* Time-stamp: <96/04/23 11:14:20 maddin>				      */
/* Project:    fillup							      */
/* Module:     fillup							      */
/* Filename:   fillup.c							      */
/* Author:     Martin Scherbaum (maddin)				      */
/* Description:                                                               */
/*                                                                            */
/*     fillup checks two files (basefile and newfile) for variables           */
/*     determined by optional parameter -d. if not default character =        */
/*     is used. variables found in newfile which are not already contained    */
/*     in basefile are appended to basefile. if an optional outfile is        */
/*     specified, the differing variables are written to that file.           */
/*     for more information read ../SGML/fillup.info.gz or ../SGML/fillup.txt */
/*                                                                            */
/*----------------------------------------------------------------------------*/

/*--------------------------------- IMPORTS ----------------------------------*/

#include "import.h"
#include "defs.h"
#include "file.h"
#include "parse.h"
#include "err.h"

/*--------------------------------- EXPORTS ----------------------------------*/

#include "export.h"
#include "fillup.h"
#include "vars.h"

/*--------------------------------- DEFINES ----------------------------------*/
/*--------------------------------- MACROS -----------------------------------*/

/* exit and close / free anything opened / allocated before */
#define EXIT(ECODE) \
{ \
  if (basefp != NULL) \
    fclose (basefp); \
  if (newfp != NULL) \
    fclose (newfp); \
  if (outfp != NULL) \
    fclose (outfp); \
  if (basebuf != NULL) \
    free (basebuf); \
  if (newbuf != NULL) \
    free (newbuf); \
  freeArray (baseArray); \
  freeArray (newArray); \
  exit (ECODE); \
} while (0)

/* the help message in case of error or -h switch */
#define usage() \
do \
{ \
  printMess (stderr, QUIET, \
             "usage: %s [options] <base file> <additional file>\n" \
             "  valid options are:\n" \
	     "    -h             this message\n" \
             "    -q             no output to screen\n" \
             "    -s             suppress output of comments\n" \
             "    -v             maximum output to screen\n" \
	     "    -c <char>      use <char> as comment marker\n" \
	     "    -d <char>      use <char> as delimitor\n" \
	     "    -o <filename>  output to <filename> instead of base file\n" \
	     "\n", \
	     progname); \
} while (0)

/*---------------------------------- TYPES -----------------------------------*/
/*-------------------------------- VARIABLES ---------------------------------*/

GLOBAL char      delim[DELIM_LEN];
GLOBAL char      comment[DELIM_LEN];

/*-------------------------------- FUNCTIONS ---------------------------------*/

/*----------------------------- IMPLEMENTATION -------------------------------*/

/*-------------------- compareArrays --------------------*/
LOCAL int compareArrays (VAR_ARRAY *base,
			 int       baseCnt,
			 VAR_ARRAY *new,
			 int       newCnt)
{
  int bcnt;
  int ncnt;
  int fcnt = newCnt;
  
  for (ncnt = 0; ncnt < newCnt; ncnt++)
  {
    new[ncnt].copy = TRUE;
    
    for (bcnt = 0; bcnt < baseCnt; bcnt++)
    {
      if (strcmp (base[bcnt].name, new[ncnt].name) == 0)
      {
	new[ncnt].copy = FALSE;
	
	printMess (stdout, NORMAL, "%s> warning: variable '%s' in both files.\n",
		   progname, base[bcnt].name);
	fcnt--;
	break;
      }
    }
    
    if (new[ncnt].copy)
    {
      if (
	  (new[ncnt].comment != NULL) && 
	  (commentMode == SAVE_COMMENT)
	  )
      {
	/* only add a newline explicitely if there's a comment */
	printMess (stdout, VERBOSE, "%s\n", new[ncnt].comment); 
      }
      
      printMess (stdout, NORMAL, new[ncnt].name); 
      
      printMess (stdout, VERBOSE, delim); 
      
      if (new[ncnt].value != NULL)
	printMess (stdout, VERBOSE, new[ncnt].value); 
      
      printMess (stdout, NORMAL, "\n"); 
    }
  }
  
  /* number of variables really found */
  return fcnt;
} 

/*-------------------- freeArray --------------------*/
LOCAL int freeArray (VAR_ARRAY *array)
{
  int n;

  for (n = 0; n < VARNAME_NUM; n++)
  {
    if (array[n].name[0] == EOS)
      break;

    if (array[n].value != NULL)
      free (array[n].value);

    if (array[n].comment != NULL)
      free (array[n].comment);
  } 

  return TRUE;
}

/*-------------------- main --------------------*/
GLOBAL void main (int argc, char **argv) 
{  
  int       args;
  char      *basebuf = NULL;	/* file content buffers */
  char      *newbuf  = NULL;

  char      outName[FILENAME_LEN];
  char      baseName[FILENAME_LEN];
  char      newName[FILENAME_LEN];
  
  FILE      *basefp = NULL;
  FILE      *newfp  = NULL;
  FILE      *outfp  = NULL;
  
  int       baseOk = TRUE;
  int       newOk  = TRUE;

  VAR_ARRAY baseArray[VARNAME_NUM] = { FALSE, "", NULL, NULL};
  VAR_ARRAY newArray [VARNAME_NUM] = { FALSE, "", NULL, NULL};
  int       baseCnt = 0;
  int       newCnt  = 0;

  int       allCnt  = 0;

  progname = *argv;
  argv++;
  argc--;

  /* reset buffers */
  memset (outName,  0, FILENAME_LEN); 
  memset (baseName, 0, FILENAME_LEN); 
  memset (newName,  0, FILENAME_LEN); 

  /* preset the delimitor */
  strcpy (delim, DEF_DELIM);
  delimLen = 1;
  
  /* preset the comment char */
  strcpy (comment, DEF_COMMENT);
  commentLen = 1;
  
  /* normal output */
  verboseMode = NORMAL;
  commentMode = SAVE_COMMENT;
  processMode = FALSE;

  /* no limit on comment length */
  maxComment = -1;
  extreme = FALSE;

  /* parse command line */
  while (argc > 0)
  {
    /* check for delimitor */
    if (strcmp (*argv, "-d") == 0)
    {
      strcpy (delim, *(++argv));
      delimLen = strlen (delim);
      argc -= 2;
      argv++;
    }
    else
    {
      /* check for max comment lines */
      if (strcmp (*argv, "-n") == 0)
      {
	maxComment = atoi (*(++argv));
        argc -= 2;
        argv++;
      }
      else
      {
        /* check for verbose */
	if (strcmp (*argv, "-v") == 0)
        {
	  verboseMode = VERBOSE;
          argc--;
          argv++;
        }
        else
        {
	  /* check for quiet mode */
	  if (strcmp (*argv, "-q") == 0)
	  {
	    verboseMode = QUIET;
	    argc--;
	    argv++;
	  }
	  else
	  {
	    /* suppress comments */
	    if (strcmp (*argv, "-s") == 0)
	    {
	      commentMode = NO_COMMENT;
	      argc--;
	      argv++;
	    }
	    else
	    {
	      /* check for separate output file */
	      if (strcmp (*argv, "-o") == 0)
	      {
		strcpy (outName, *(++argv));
		argc -= 2;
		argv++;
	      }
	      else
	      {
		/* check for comment char */
		if (strcmp (*argv, "-c") == 0)
		{
		  strcpy (comment, *(++argv));
		  commentLen = strlen (comment);
		  argc -= 2;
		  argv++;
		}
		else
		{
		  /* check for help */
		  if ((strcmp (*argv, "-h") == 0)   ||
		      (strcmp (*argv, "--help") == 0))
		  {
		    usage ();
		    EXIT (0);
		  }
		  else
		  {
		    /* check for extreme output */
		    /* this is an undocumented feature for debug */
		    if (strcmp (*argv, "-x") == 0)
		    {
		      extreme = TRUE;
		      argc--;
		      argv++;
		    }
		    else
		    {
		      /* check for unknown option */
		      if (**argv == '-')
		      {
			printMess (stderr, QUIET, 
				   "%s> invalid option: '%s'!\n", 
				   progname, *argv);
			usage ();
			EXIT (-1);
		      }
		      else
		      {
			/* here all the switches should be done */
			break;
		      }
		    }
		  }
		}
	      }
	    }
	  }
	}
      }
    }
  }

  /* set required arguments counter */
  args = 2;

  /* parse command line */
  while (argc > 0)
  {
    switch (args) 
    {
      /* get base file name */
      case 2:
      {
	strcpy (baseName, *argv);
	argc--;
	argv++;
	args--;
	continue;
      }
      /* get new file name */
      case 1:
      {
	strcpy (newName, *argv);
	argc--;
	argv++;
	args--;
	continue;
      }
      /* all done */
      case 0:
      {
	break;
      }
    }
  }

  if (args > 0)
  {
    printMess (stderr, QUIET, 
	       "%s> too few arguments in command line!\n", 
	       progname);
    usage ();
    EXIT (-1);
  }

  /* check if files can be opened */

  /* open base file for reading */
  if ((basefp = fopen (baseName, "r")) == NULL)
  {
    printMess (stderr, QUIET, "%s> warning: file '%s' could not be opened for reading!\n", 
	       progname, baseName);

    baseOk = FALSE;
  }

  /* open new file for reading */
  if ((newfp = fopen (newName, "r")) == NULL)
  {
    printMess (stderr, QUIET, "%s> warning: file '%s' could not be opened for reading!\n", 
	       progname, newName);
    newOk = FALSE;
  }
  
  /* check if there's an out file specified in command line */
  if (outName[0] == 0)
  {
    /* if not then copy the base name */
    strcpy (outName, baseName);

    /* open the out file for appending */
    if ((outfp = fopen (outName, "a")) == NULL)
    {
      printMess (stderr, QUIET, "%s> target file '%s' could not be opened for appending!\n", 
		 progname, outName);
      EXIT (-1);
    }
  }
  else
  {
    /* open the out file for appending */
    if ((outfp = fopen (outName, "w")) == NULL)
    {
      printMess (stderr, QUIET, 
		 "%s> target file '%s' could not be opened for writing!\n", 
		 progname, outName);
      EXIT (-1);
    }
  }

  if (baseOk)
  {
    /* read the base file to a buffer */
    if ((baseOk = readFile (basefp, baseName, &basebuf)) == -1)
    {
      printMess (stderr, QUIET, 
		 "%s> error while reading file '%s'!\n", 
		 progname, baseName);
      EXIT (-1);
    }

    fclose (basefp);
    basefp = NULL;
  }

  if (newOk)
  {
    /* read the new file to a buffer */
    if ((newOk = readFile (newfp, newName, &newbuf)) == -1)
    {
      printMess (stderr, QUIET, "%s> error while reading file '%s'!\n", progname, newName);
      EXIT (-1);
    }

    fclose (newfp);
    newfp = NULL;
  }

#ifdef DEBUG
  printMess (stdout, VERBOSE, "%s> processing: %s...\n", progname, baseName);
#endif

  if (baseOk)
  {
    /* only non-empty files are parsed */
    if ((baseCnt = parseFile ((void*)baseArray, basebuf, NO_COMMENT)) == -1)
    {
      printMess (stderr, QUIET, "%s> error while parsing file '%s'!\n", progname, baseName);
      EXIT (-1);
    }
  }

#ifdef DEBUG
  printMess (stdout, VERBOSE, "%s> done.\n", progname);
#endif
  
#ifdef DEBUG
  printMess (stdout, VERBOSE, "%s> processing: %s...\n", progname, newName);
#endif

  if (newOk)
  {
    /* only non-empty files are parsed */
    if ((newCnt = parseFile ((void*)newArray, newbuf,  commentMode)) == -1)
    {
      printMess (stderr, QUIET, "%s> error while parsing file '%s'!\n", progname, newName);
      EXIT(-1);
    }
  }

#ifdef DEBUG
  printMess (stdout, VERBOSE, "%s> done.\n", progname);
#endif
  
#ifdef DEBUG
  printMess (stdout, VERBOSE, "%s> comparing...\n", progname);
#endif

  compareArrays (baseArray, baseCnt,
		 newArray,  newCnt);

#ifdef DEBUG
  printMess (stdout, VERBOSE, "%s> done.\n", progname);
#endif
  
#ifdef DEBUG
  printMess (stdout, VERBOSE, "%s> writing to file '%s'...\n", progname, outName);
#endif

  /* write the resting variable with comments */
  allCnt = writeFile (outfp, outName, newArray, newCnt);

#ifdef DEBUG
  printMess (stdout, VERBOSE, "%s> finished processing.\n", progname);
#endif

  /* exit and free all resources */
  EXIT (allCnt);
}

/*----------------------------------------------------------------------------*/




