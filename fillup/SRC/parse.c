
/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Copyrights to S.u.S.E. GmbH Fuerth (c) 1996                                */
/*                                                                            */
/* Time-stamp: <96/04/23 11:14:31 maddin>				      */
/* Project:    fillup							      */
/* Module:     parse							      */
/* Filename:   parse.c							      */
/* Author:     Martin Scherbaum (maddin)				      */
/* Description:                                                               */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/

/*--------------------------------- IMPORTS ----------------------------------*/

#include "import.h"
#include "defs.h"
#include "vars.h"
#include "err.h"

/*--------------------------------- EXPORTS ----------------------------------*/

#include "export.h"
#include "parse.h"

/*--------------------------------- DEFINES ----------------------------------*/
/*--------------------------------- MACROS -----------------------------------*/

#define onEofReturn(VAL) \
{ \
  if (eofOccured) \
    return (VAL); \
} while (0)

#define onEofWarn(VAL) \
{ \
  if (eofOccured) \
  { \
    printMess (stderr, QUIET, \
	       "%s> line %d: warning: file ended before end of line! \n", \
	       progname, actLine); \
    return VAL; \
  } \
} while (0) \

#define extremeMess(MESS) \
{ \
  if (extreme) \
    printMess (stderr, QUIET, \
	       "function %s\n", MESS); \
 \
} while (0)

#define charMess() \
{ \
  if (extreme) \
    printMess (stderr, QUIET, \
	       "function readNextChar; char: %s\n", \
	       (actChar == EOL) \
	         ? "EOL" \
	         : (actChar == 32) \
	           ? "SPC" \
	           : (char*)&actChar); \
 \
} while (0)

/*---------------------------------- TYPES -----------------------------------*/
/*-------------------------------- VARIABLES ---------------------------------*/

LOCAL  char       *actCharPtr;
LOCAL  char       actChar;
LOCAL  long       actIndex;
LOCAL  int        eofOccured;
LOCAL  int        actLine;

LOCAL  VAR_ARRAY  *arrayPtr;
LOCAL  int        arrayCnt;

LOCAL  char       commentbuf[BUF_LEN];
LOCAL  char       *commentPtr;

LOCAL  char       valuebuf[BUF_LEN];
LOCAL  char       *valuePtr;

LOCAL  char       varbuf[BUF_LEN];
LOCAL  char       *varPtr;

LOCAL  int 	  foundVariable;
LOCAL  int 	  foundValue;
LOCAL  int 	  foundComment;
LOCAL  int        foundAnyText;

LOCAL  int        localCommentMode;

/*-------------------------------- FUNCTIONS ---------------------------------*/

LOCAL  int 	  readNextChar 	 _((void));

LOCAL  int 	  isCharP 	 _((void));

LOCAL  int 	  isSpaceP 	 _((void));
LOCAL  int 	  isNewlineP 	 _((void));
LOCAL  int 	  isCommentP 	 _((void));
LOCAL  int 	  isDelimitorP 	 _((void));

LOCAL  int 	  eatSpaces 	 _((void));
LOCAL  int 	  eatNewline 	 _((void));
LOCAL  int 	  eatDelimitor 	 _((void));
LOCAL  int 	  eatCommentLine _((void));
LOCAL  int 	  eatVarname 	 _((void));
LOCAL  int 	  eatValue 	 _((void));
LOCAL  int 	  eatLongLine 	 _((void));
LOCAL  int 	  eatContentLine _((void));
LOCAL  int 	  eatLine 	 _((void));

/*----------------------------- IMPLEMENTATION -------------------------------*/

/*-------------------- readNextChar --------------------*/
LOCAL int readNextChar ()
{
  charMess ();

  if (eofOccured)
    /* do not increment buffer pointer         */
    /* if we are beyond the end of file marker */
    return FALSE;

  /* this is the new char to be looked at */
  actChar = *(++actCharPtr);
  actIndex++;

  /* set a flag when we reach the end of file marker */
  if (actChar == EOF)
    eofOccured = TRUE;

  /* return FALSE if an end of file occured */
  return (eofOccured == FALSE);
}

/*-------------------- isCharP --------------------*/
LOCAL int isCharP ()
{
  extremeMess ("isCharP");

  return (
	  (actChar != EOS)    &&
	  (actChar != '#')    &&
	  (actChar != '\\')   &&
	  (! isNewlineP ())   &&
	  (! isSpaceP ())     &&
	  (! isDelimitorP ()) 
	 );
}

/*-------------------- isSpaceP --------------------*/
LOCAL int isSpaceP ()
{
  extremeMess ("isSpaceP");

  return (
	  (actChar == 32)  ||
	  (actChar == 9)
	 );
}

/*-------------------- isNewlineP --------------------*/
LOCAL int isNewlineP ()
{
  extremeMess ("isNewlineP");

  return (actChar == EOL);
}

/*-------------------- isDelimitorP --------------------*/
LOCAL int isDelimitorP ()
{
  extremeMess ("isDelimitorP");

  return (actChar == delim[0]);
}

/*-------------------- isCommentP --------------------*/
LOCAL int isCommentP ()
{
  extremeMess ("isCommentP");

  return (actChar == comment[0]);
}

/*-------------------- eatSpaces --------------------*/
LOCAL int eatSpaces ()
{
  extremeMess ("eatSpaces");

  while ( isSpaceP () )
  {
    readNextChar ();

    if (eofOccured)
      break;
  }

  return TRUE;
}

/*-------------------- eatNewline --------------------*/
LOCAL int eatNewline ()
{
  extremeMess ("eatNewline");

  if (! isNewlineP ())
    return FALSE;

  actLine++;

  readNextChar ();
  
  return TRUE;
}

/*-------------------- eatDelimitor --------------------*/
LOCAL int eatDelimitor ()
{
  extremeMess ("eatDelimitor");

  if (! isDelimitorP ())
    return FALSE;

  readNextChar ();
  
  return TRUE;
}

/*-------------------- eatCommentLine --------------------*/
LOCAL int eatCommentLine ()
{
  extremeMess ("eatCommentLine");

  /* at least on line of comments was found! */
  foundComment = TRUE;

  /* till the end of line... */
  while (! isNewlineP ())
  {
    /* keep comment in commentbuf */
    *(commentPtr++) = actChar;

    readNextChar ();

    /* eof while comment is not wanted but allowed */
    onEofWarn (TRUE);
  }

  /* terminate the read comment line */
  *(commentPtr++) = EOL;

  /* eat the newline char */
  return (eatNewline ());
}

/*-------------------- eatVarname --------------------*/
LOCAL int eatVarname ()
{
  int n = 0;
  int buflen = VARNAME_LEN;

  extremeMess ("eatVarname");

  /* do not exceed var buffer length! */
  for (n = 0; n < buflen; n++)
  {
    /* check newline */
    if (isNewlineP ()) 
    {
      /* there was anytext, but not a valid variable */
      return TRUE;
    }

    /* check comment */
    if (isCommentP ()) 
    {
      printMess (stderr, QUIET, 
		 "%s> line %d: variable name should not end with comment! "
		 "(missing or wrong delimitor?)\n", 
		 progname, actLine); 
      return FALSE;
    }

    /* only chars are allowed */
    if (isCharP ()  || 
	isSpaceP () ||
	isDelimitorP ()) 
    {
      if (isDelimitorP () && 
	  ! foundAnyText)
      {
	/* this is a real variable */
	return TRUE;
      }

      /* this indicates if we did find an non-variable line */
      if (isSpaceP ())
      {
	foundAnyText = TRUE; 

	/* enlarge the allowed buffer size */
	buflen       = BUF_LEN; 
      }

      /* save the character */
      *(varPtr++) = actChar;

      readNextChar ();

      /* no eof may occur while reading a variable name */
      if (eofOccured)
      {
	printMess (stderr, QUIET, "%s> line %d: eof in variable name!\n", progname, actLine);
	return FALSE;
      }

      continue;
    }

    /* variable entirely read */
    return TRUE;
  }

  /* normally this shouldn't happen! */
  printMess (stderr, QUIET, 
	     "%s> line %d: variable name length exceeded (> %d)!\n", 
	     progname, actLine, VARNAME_LEN);

  return FALSE;
}

/*-------------------- eatValue --------------------*/
LOCAL int eatValue ()
{
  extremeMess ("eatValue");

  while  (isCharP () || 
	  isSpaceP () ||
	  isDelimitorP ())
  {
    if (isCharP ())
      /* real values aren't empty! */
      foundValue = TRUE;

    /* save the value */
    *(valuePtr++) = actChar;

    readNextChar ();

    onEofReturn (TRUE);
  }
  
  return eatLongLine ();
}

/*-------------------- isConnectorP --------------------*/
LOCAL int isConnectorP ()
{
  extremeMess ("isConnectorP");

  return (actChar == '\\');
}

/*-------------------- eatConnector --------------------*/
LOCAL int eatConnector ()
{
  extremeMess ("eatConnector");

  if (! isConnectorP ())
    return FALSE;

  readNextChar ();

  return TRUE;
}

/*-------------------- eatLongLine --------------------*/
LOCAL int eatLongLine ()
{
  extremeMess ("eatLongLine");

  /* did we find a line connector? */
  if (! eatConnector ())
    return TRUE;

  /* replace line connector by a space */
  *(valuePtr++) = ' ';

  onEofReturn (TRUE);
  
  eatSpaces ();

  onEofReturn (TRUE);

  /* if normal chars follow the line connector, 
     this is an error, cause there must be a newline */
  if (! eatNewline ())
  {
    printMess (stderr, QUIET, 
	       "%s> line %d: line connector may not be followed by characters!\n", 
	       progname, actLine);
    return FALSE;
  }

  /* remove leading white space in connected line */
  eatSpaces ();

  onEofWarn (TRUE);

  return (eatValue ());
}

/*-------------------- eatContentLine --------------------*/
LOCAL int eatContentLine ()
{
  extremeMess ("eatContentLine");

  /* now we're scanning the var name */
  foundVariable = TRUE;
  
  if (! eatVarname ())
    return FALSE;

  /* terminate variable name resp. anytext */
  *varPtr = EOS;

  /* quit here for anytext */
  if (foundAnyText)
  {
    /* save anytext from varbuf to commentbuf */
    strcpy (commentPtr, varbuf);
    commentPtr += strlen (varbuf);

    /* append line end */
    *(commentPtr++) = EOL;

    /* reset variable name start */
    varPtr = varbuf;

    /* indeed, we did not find a variable but anytext */
    foundVariable = FALSE;

    /* but we handle it as a comment */
    foundComment = TRUE;

    return TRUE;
  }
  /* there may be spaces between variable and delimitor */
  eatSpaces ();

  /* no eof may occure here */
  if (eofOccured)
  {
    printMess (stderr, QUIET, 
	       "%s> line %d: end of file occured between variable name and delimitor!\n", 
	       progname, actLine);
    return FALSE;
  }

  /* check and eat delimitor */
  if (! eatDelimitor ())
  {
    printMess (stderr, QUIET, 
	       "%s> line %d: missing delimitor after variable name!\n", 
	       progname, actLine);
    return FALSE;
  }

  /* get the value of the variable */
  if (! eatValue ())
    return FALSE;

  /* terminate value buffer */
  *valuePtr = EOS;

  onEofWarn (TRUE);
  
  /* the thing must be updated by an end of line */
  return (eatNewline ());
}

/*-------------------- eatLine --------------------*/
LOCAL int eatLine ()
{
  extremeMess ("eatLine");

  /* skip leading spaces */
  eatSpaces ();

  onEofReturn (TRUE);

  /* look at the next character         */
  /*(first real character in this line) */
  if (isCommentP ())
  {
    /* found comment */
    /* skip the comment to the end of line */
    return (eatCommentLine ());
  }
  else
  { 
    if (isNewlineP ())
    {
      /* skip empty lines */
      return (eatNewline ());
    }
    else
    {
      if (! eatContentLine ())
	return FALSE;
      
      if (foundAnyText)
      {
	/* handle anytext as comment */
	foundAnyText = FALSE;

	return TRUE;
      }

      arrayPtr[arrayCnt].value   = NULL;
      arrayPtr[arrayCnt].comment = NULL;

      /* copy the variable name */
      strcpy (arrayPtr[arrayCnt].name, varbuf);
      
      /* copy value if one found */
      if (foundValue)
      {
	arrayPtr[arrayCnt].value = malloc (strlen (valuebuf) + 1);
	strcpy (arrayPtr[arrayCnt].value, valuebuf);
      }
      
      /* copy comments if found and no suppressing is wanted */
      if ((foundComment)                     && /* found comment */ 
	  (commentMode == SAVE_COMMENT)      && /* no supress    */
	  (localCommentMode == SAVE_COMMENT)    /* not base file */
	  ) 
      {
	/* terminate comment buffer before last newline! */
	*(--commentPtr) = EOS; 
	
	/* get memory for the comment and copy comment there */
	arrayPtr[arrayCnt].comment = malloc (strlen (commentbuf) + 1);
	strcpy (arrayPtr[arrayCnt].comment, commentbuf);
      }
    
      /* reset flags */
      foundVariable  = FALSE;
      foundComment   = FALSE;
      foundValue     = FALSE;
      foundAnyText   = FALSE;
      
      /* reset buffer pointers */
      commentPtr = commentbuf;
      valuePtr   = valuebuf;
      varPtr     = varbuf;

      /* one more variable read */
      arrayCnt++;
    }
  }

  /* well done */
  return TRUE;
}

/*-------------------- parseFile --------------------*/
GLOBAL int parseFile (void *arrayVPtr,
		      char *buffer,
		      int  parseMode)
{
  extremeMess ("parseFile");

  if ((buffer[0] == EOS) ||
      (buffer[0] == EOF))
    return 0;

  /* reset the lookup character */
  actCharPtr = buffer;
  actChar    = *actCharPtr;
  actIndex   = 0;
  actLine    = 1;

  /* reset local array pointer and counter */
  arrayPtr = (VAR_ARRAY*)arrayVPtr;
  arrayCnt = 0;

  /* clean array */
  memset (arrayPtr, 0, sizeof (VAR_ARRAY) * VARNAME_NUM);

  /* if base file or not */
  localCommentMode = parseMode;

  /* reset flags */
  foundVariable = FALSE;
  foundComment  = FALSE;
  foundValue    = FALSE;
  foundAnyText  = FALSE;

  /* reset buffers */
  memset (commentbuf, 0, BUF_LEN);
  commentPtr = commentbuf;
  
  memset (valuebuf, 0, BUF_LEN);
  valuePtr = valuebuf;

  memset (varbuf, 0, VARNAME_LEN);
  varPtr = varbuf;

  /* no eof so long */
  eofOccured = FALSE;

  while (! eofOccured)
  {
    if (! eatLine ())
      return -1;
  }

  return arrayCnt;
}


/*----------------------------------------------------------------------------*/











