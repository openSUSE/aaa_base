
/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Copyrights to S.u.S.E. GmbH Fuerth (c) 1998                                */
/*                                                                            */
/* Time-stamp:                                                                */
/* Project:    fillup                                                         */
/* Module:     fillup                                                         */
/* Filename:   getArguments.c                                                 */
/* Author:     Joerg Dippel (jd)                                              */
/* Description:                                                               */
/*                                                                            */
/*     fillup takes two files (basefile and addfile) for                      */
/*     variables and creates a new file (outputfile) containing variables.    */
/*     If basefile and outputfile are named identically only two parameters   */
/*     are necessary -- this supports the old form of fillup (1.04)           */
/*                                                                            */
/*     Each variable name is assigned one value. This assignment composes an  */
/*     entity with its related comment - the comment may be empty.            */
/*     This entity is named 'variable block'.                                 */
/*                                                                            */
/*     basefile, addfile and outputfile are files containing variable blocks. */
/*     addfile may contain only variable names if variable                    */
/*     blocks should be retrieved -- or only assignments if there should be   */
/*     assignment substitutions.                                              */
/*                                                                            */
/*     fillup now provides different kinds of functionality to create the     */
/*     outputfile. The content of the variable blocks is handled as           */
/*     transparent (this means no information is evaluated):                  */
/*     -- Variable blocks of basefile remain unchanged and                    */
/*        all variable blocks of basefile are added to the outputfile.        */
/*        Variable blocks of newfile are added to the outputfile only if      */
/*        they are not in the basefile.                                       */
/*        (This is the former functionality of fillup-1.04.)                  */
/*        Testseries 1XX: parameter -m (implicitely)                          */
/*     -- Variable blocks of basefile are deleted from the original basefile  */
/*        but all variable blocks of original basefile are added to the       */
/*        outputfile.                                                         */
/*        Variable blocks of newfile are added to the outputfile only if      */
/*        they are not in the original basefile.                              */
/*        Testseries 4XX: parameter -r                                        */
/*     -- Variable blocks of newfile remain unchanged and                     */
/*        all variable blocks of newfile are added to the outputfile.         */
/*        Variable blocks of basefile are added to the outputfile only if     */
/*        they are not in the newfile.                                        */
/*        Testseries 2XX: parameter -x                                        */
/*     -- Variable blocks of newfile remain unchanged and                     */
/*        all variable blocks of newfile are added to the outputfile.         */
/*        Variable blocks of original basefile are added to the outputfile    */
/*        only if they are not in the newfile. If that happens they are       */
/*        deleted from the original basefile.                                 */
/*        Testseries 5XX: parameter -x -r                                     */
/*     -- Variable blocks of basefile remain unchanged and a variable block   */
/*        of basefile is added to the outputfile only if there is a variable  */
/*        defined in the addfile with the same name -- otherwise the variable */
/*        is ignored.                                                         */
/*        Variable blocks of newfile are added to the outputfile only if      */
/*        they are not in the basefile.                                       */
/*        Testseries 3XX: parameter -i                                        */
/*                                                                            */
/*     For the future there are plans to extend functionality:                */
/*     Variable blocks of basefile remain unchanged and                       */
/*     variable blocks of basefile are added to the outputfile                */
/*     only if the associated variable of the variable block is listed        */
/*     within the definition file (filter functionality)                      */
/*                                                                            */
/*     Furthermore fillup now provides a functionality for substitution of    */
/*     assignments. The basefile is transformed into the outputfile --        */
/*     Within the transformation for each variable name which is not part     */
/*     of an assignment within the definition file the related variable       */
/*     block is transparently copied to the outputfile.                       */
/*     Otherwise if a variable name is part of an assignment within the       */
/*     definition file the assignment of the basefile is substituted by the   */
/*     assignment of the definitionfile and this new variable block is        */
/*     copied to the outputfile.                                              */
/*                                                                            */
/*----------------------------------------------------------------------------*/

/*--------------------------------- IMPORTS ----------------------------------*/

#include <stdlib.h>

#include "portab.h"
#include "parameters.h"
#include "services.h"
#include "parser.h"

/*--------------------------------- DEFINES ----------------------------------*/

#define   numberOfMandatoryParameters       2 
#define   numberOfSimpleParameter           1
#define   numberOfPairParameter             2

/*-------------------------------- FUNCTIONS ---------------------------------*/

/*----------------------------- IMPLEMENTATION -------------------------------*/

/*--------------- checkArgument ----------------*/
static BOOLEAN 
checkArgument
( 
    const char *argument,                /* in */
    const char *firstAlternative,        /* in */
    const char *secondAlternative        /* in */
)
{
    BOOLEAN     returnValue;

    if( compareStrings( argument, firstAlternative ) == Equal ) 
    {
        returnValue = TRUE;
    }
    else if( compareStrings( argument, secondAlternative ) == Equal ) 
    {
        returnValue = TRUE;
    }
    else
    {
        returnValue = FALSE;
    }

    return( returnValue );
}

/*-------------------- main --------------------*/
int 
main 
(
    int     argc,                        /* in */
    char  **argv                         /* in */
) 
{  
    BOOLEAN   parsingState;
    BOOLEAN   InterruptCausingExceptionState;

    /* parse command line */
    initializeParameters( );
    if( argc > 1 ) /* there are arguments */
    {
        argv++;
        argc--;
        parsingState = TRUE;
        InterruptCausingExceptionState = TRUE;
    }
    else
    {
        parsingState = FALSE;
        InterruptCausingExceptionState = FALSE;
    }
    while( ( argc >= 0 ) && ( parsingState == TRUE ) )
    {
      /* check for delimiter */
      if( checkArgument( *argv, "-d", "--delim" ) == TRUE )
      {
          if( argc >= ( numberOfMandatoryParameters + numberOfPairParameter ) )
          {
              argv++;
              setStringParameter( Delimiter, *argv );
              argv++;
              argc -= 2;
          }
          else
          {
              fillup_exception( __FILE__, __LINE__, FormatException, 
                               "delimiter" );
              parsingState = FALSE;
          }
      }
      /* check for max comment lines */
      else if( checkArgument( *argv, "-n", "--num" ) == TRUE )
      {
          if( argc >= ( numberOfMandatoryParameters + numberOfPairParameter ) )
          {
              argv++;
              setNumericParameter( CommentLines, *argv );
              argv++;
              argc -= 2;
          }
          else
          {
              fillup_exception( __FILE__, __LINE__, FormatException, 
                                "comment lines" );
              parsingState = FALSE;
          }
      }
      /* check for verbose */
      else if( checkArgument( *argv, "-v", "--verbose" ) == TRUE )
      {
          if( argc >= ( numberOfMandatoryParameters + numberOfSimpleParameter ) )
          {
              setSimpleParameter( Verbose );
              argv++;
              argc--;
          }
          else
          {
              fillup_exception( __FILE__, __LINE__, FormatException, "verbose" );
              parsingState = FALSE;
          }
      }
      /* check for substitution (put) */
      else if( checkArgument( *argv, "-p", "--put" ) == TRUE )
      {
          if( argc >= ( numberOfMandatoryParameters + numberOfSimpleParameter ) )
          {
              setSimpleParameter( Put );
              argv++;
              argc--;
          }
          else
          {
              fillup_exception( __FILE__, __LINE__, FormatException, 
                               "substitution (put)" );
              parsingState = FALSE;
          }
      }
      /* check for extraction (get) */
      else if( checkArgument( *argv, "-g", "--get" ) == TRUE )
      {
          if( argc >= ( numberOfMandatoryParameters + numberOfSimpleParameter ) )
          {
              setSimpleParameter( Get );
              argv++;
              argc--;
          }
          else
          {
              fillup_exception( __FILE__, __LINE__, FormatException, 
                               "extraction (get)" );
              parsingState = FALSE;
          }
      }
      /* check for maintaining the basefile */
      else if( checkArgument( *argv, "-m", "--maintain" ) == TRUE )
      {
          if( argc >= ( numberOfMandatoryParameters + numberOfSimpleParameter ) )
          {
              setSimpleParameter( Maintain );
              argv++;
              argc--;
          }
          else
          {
              fillup_exception( __FILE__, __LINE__, FormatException, "maintain" );
              parsingState = FALSE;
          }
      }
      /* check for exchange */
      else if( checkArgument( *argv, "-x", "--exchange" ) == TRUE )
      {
          if( argc >= ( numberOfMandatoryParameters + numberOfSimpleParameter ) )
          {
              setSimpleParameter( Exchange );
              argv++;
              argc--;
          }
          else
          {
              fillup_exception( __FILE__, __LINE__, FormatException, "exchange" );
              parsingState = FALSE;
          }
      }
      /* check for removal within basefile */
      else if( checkArgument( *argv, "-r", "--remove" ) == TRUE )
      {
          if( argc >= ( numberOfMandatoryParameters + numberOfSimpleParameter ) )
          {
              setSimpleParameter( Remove );
              argv++;
              argc--;
          }
          else
          {
              fillup_exception( __FILE__, __LINE__, FormatException, "remove" );
              parsingState = FALSE;
          }
      }
      /* check for protocolize parameters (passes arguments) */
      else if( checkArgument( *argv, "-a", "--arguments" ) == TRUE )
      {
          if( argc >= ( numberOfMandatoryParameters + numberOfSimpleParameter ) )
          {
              setSimpleParameter( Parameters );
              argv++;
              argc--;
          }
          else
          {
              fillup_exception( __FILE__, __LINE__, FormatException, 
                                "protocol parameters" );
              parsingState = FALSE;
          }
      }
      /* check for quiet mode */
      else if( checkArgument( *argv, "-q", "--quiet" ) == TRUE )
      {
          if( argc >= ( numberOfMandatoryParameters + numberOfSimpleParameter ) )
          {
              setSimpleParameter( Quiet );
              argv++;
              argc--;
          }
          else
          {
              fillup_exception( __FILE__, __LINE__, FormatException, "quiet" );
              parsingState = FALSE;
          }
      }
      /* suppress comments */
      else if( checkArgument( *argv, "-s", "--suppress" ) == TRUE )
      {
          if( argc >= ( numberOfMandatoryParameters + numberOfSimpleParameter ) )
          {
              setSimpleParameter( Suppress );
              argv++;
              argc--;
          }
          else
          {
              fillup_exception( __FILE__, __LINE__, FormatException, 
                                "suppress comments" );
              parsingState = FALSE;
          }
      }
      /* check for comment marker */
      else if( checkArgument( *argv, "-c", "--char" ) == TRUE )
      {
          if( argc >= ( numberOfMandatoryParameters + numberOfPairParameter ) )
          {
              argv++;
              setStringParameter( CommentMarker, *argv );
              argv++;
              argc -= 2;
          }
          else
          {
              fillup_exception( __FILE__, __LINE__, FormatException, 
                                "comment marker" );
              parsingState = FALSE;
          }
      }
      /* check for quoting marker */
      /* l stands for lift - because other prefixes are already used */
      else if( checkArgument( *argv, "-l", "--quote" ) == TRUE )
      {
          if( argc >= ( numberOfMandatoryParameters + numberOfPairParameter ) )
          {
              argv++;
              setStringParameter( QuotingMarker, *argv );
              argv++;
              argc -= 2;
          }
          else
          {
              fillup_exception( __FILE__, __LINE__, FormatException, 
                                "quoting marker" );
              parsingState = FALSE;
          }
      }
      /* check for file, that denies (forbid) any changes for given variables */
      else if( checkArgument( *argv, "-f", "--forbidden" ) == TRUE )
      {
          if( argc >= ( numberOfMandatoryParameters + numberOfPairParameter ) )
          {
              argv++;
              setStringParameter( ForbiddenFile, *argv );
              argv++;
              argc -= 2;
          }
          else
          {
              fillup_exception( __FILE__, __LINE__, FormatException, 
                                "forbidden file name" );
              parsingState = FALSE;
          }
      }
      /* check for help */
      else if( checkArgument( *argv, "-h", "--help" ) == TRUE )
      {
          if( argc >= numberOfSimpleParameter )
          {
              setSimpleParameter( Help );
          }
          else
          {
              fillup_exception( __FILE__, __LINE__, FormatException, "help" );
          }
          parsingState = FALSE;
          InterruptCausingExceptionState = FALSE;
      }
      /* ignore end of file condition */
      else if( checkArgument( *argv, "-e", "--ignoreeof" ) == TRUE )
      {
          if( argc >= ( numberOfMandatoryParameters + numberOfSimpleParameter ) )
          {
              setSimpleParameter( IgnoreEOF );
              argv++;
              argc--;
          }
          else
          {
              fillup_exception( __FILE__, __LINE__, FormatException, 
                                "ignore EOF" );
              parsingState = FALSE;
          }
      }
      /* don't write variable blocks if they are defined only within base file */
      else if( checkArgument( *argv, "-i", "--ignoreDefinites" ) == TRUE )
      {
          if( argc >= ( numberOfMandatoryParameters + numberOfSimpleParameter ) )
          {
              setSimpleParameter( IgnoreDefinites );
              argv++;
              argc--;
          }
          else
          {
              fillup_exception( __FILE__, __LINE__, FormatException, 
                                "ignore Definites" );
              parsingState = FALSE;
          }
      }
      /* save trailing comments at the end of the file */
      else if( checkArgument( *argv, "-t", "--trailing" ) == TRUE )
      {
          if( argc >= ( numberOfMandatoryParameters + numberOfSimpleParameter ) )
          {
              setSimpleParameter( TrailingComment );
              argv++;
              argc--;
          }
          else
          {
              fillup_exception( __FILE__, __LINE__, FormatException, 
                                "trailing comment" );
              parsingState = FALSE;
          }
      }
      /* display current version */
      else if( checkArgument( *argv, "-V", "--version" ) == TRUE )
      {
          if( argc >= numberOfSimpleParameter )
          {
              setSimpleParameter( Version );
          }
          else
          {
              fillup_exception( __FILE__, __LINE__, FormatException, 
                                "current version" );
          }
          parsingState = FALSE;
          argc = 0;
      }
      /* check for unknown option */
      else if( compareStringsExactly( "-", *argv ) == Equal )
      {
          fillup_exception( __FILE__, __LINE__, FormatException, 
                            "invalid option" );
          parsingState = FALSE;
      }
      else
      {
          /* there are only the mandatory parameters */
          if( argc == ( numberOfMandatoryParameters + 1 ) )
          {
              setStringParameter( BaseFile, *argv );
              argv++;
              setStringParameter( AdditionalFile, *argv );
              argv++;
              setStringParameter( OutputFile, *argv );
              argc = 0;
          }
          /* presume that this is the old version of fillup */
          else if( argc == numberOfMandatoryParameters )
          {
              setStringParameter( BaseFile, *argv );
              setStringParameter( OutputFile, *argv );
              argv++;
              setStringParameter( AdditionalFile, *argv );
              argc = 0;
          }
          else
          {
              fillup_exception( __FILE__, __LINE__, FormatException, 
                                "mandatory  parameters" );
          }
          parsingState = FALSE;
      }
    }

    if( argc > 0)
    {
        displayUsageInformation( );
        if( InterruptCausingExceptionState == TRUE )
        {
            fillup_exception( __FILE__, __LINE__, NumberOfFormatException, 
                              "number of arguments" );
        }
        displayParameters( );
    }
    else
    {
        if( queryParameter( Parameters ) == TRUE )
        {
            displayParameters( );
        }

        if( queryParameter( Version ) == TRUE )
        {
            displayVersion( );
        }
        else
        {
            if( Success == instantiateParameters( ) )
            {
                startParser( );
            }
        }
    }

    return( EXIT_SUCCESS );
}

/*----------------------------------------------------------------------------*/
