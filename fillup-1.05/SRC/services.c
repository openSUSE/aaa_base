
/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Copyrights to S.u.S.E. GmbH Fuerth (c) 1998                                */
/*                                                                            */
/* Time-stamp:                                                                */
/* Project:    fillup                                                         */
/* Module:     services                                                       */
/* Filename:   services.c                                                     */
/* Author:     Joerg Dippel (jd)                                              */
/* Description:                                                               */
/*                                                                            */
/*     Standard library functions are not called directly. To avoid           */
/*     problems in sublayers there are service function that call these       */
/*     standard library functions.
/*                                                                            */
/*----------------------------------------------------------------------------*/

/*--------------------------------- IMPORTS ----------------------------------*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <errno.h>

#include "portab.h"

#include "services.h"

/*-------------------------------- VARIABLES ---------------------------------*/

static    FILE  * filePointerForWriting;

/*----------------------------- IMPLEMENTATION -------------------------------*/

/*----------------- compareStrings -----------------*/

BOOLEAN
compareStrings
(
    const char    * firstString,              /* in */
    const char    * secondString              /* in */
)
{
    BOOLEAN    returnValue;

    if( strcmp( firstString, secondString ) == 0 )
    {
        returnValue = TRUE;
    }
    else
    {
        returnValue = FALSE;
    }

    return( returnValue );
}

/*------------- compareStringsExactly --------------*/

BOOLEAN
compareStringsExactly
(
    const char    * firstString,              /* in */
    const char    * secondString              /* in */
)
{
    BOOLEAN    returnValue;

    if( strncmp( firstString, secondString, strlen( firstString ) ) == 0 )
    {
        returnValue = TRUE;
    }
    else
    {
        returnValue = FALSE;
    }

    return( returnValue );
}

/*------------------ sortStrings -------------------*/

StringOrder_t
sortStrings
(
    const char    * firstString,              /* in */
    const char    * secondString              /* in */
)
{
    int             serviceResult;
    StringOrder_t   returnValue;

    serviceResult = strcmp( firstString, secondString );

    if( serviceResult < 0 )
    {
        returnValue = Smaller;
    }
    else if( serviceResult == 0 )
    {
        returnValue = Equal;
    }
    else
    {
        returnValue = Greater;
    }

    return( returnValue );
}

/*------------------ stringLength ------------------*/

long
stringLength
(
    const char    * String                    /* in */
)
{
    return( ( long )strlen( String ) );
}

/*------------- createNewBaseFileName --------------*/

void
createNewBaseFileName
(
    const char    * oldString,                /* in */
          char    * newString                 /* in */
)
{
    strcpy( newString, oldString );
    strcat( newString, ".new" );
}

/*----------------- exitOnFailure ------------------*/

void
exitOnFailure
(
    void
)
{
    exit( EXIT_FAILURE );
}

/*--------------- fillup_exception -----------------*/

void
fillup_exception
(
    const char    * fileName,                 /* in */
          int       lineNumber,               /* in */
    Exception_t     exceptionType,            /* in */
    const char    * description               /* in */
)
{
    ( void )fprintf( stderr, 
                     "\tException in %s, line %d concerning %s\n", 
                     fileName, 
                     lineNumber, 
                     description );
}

/*-------------- displayStderrString ---------------*/

void
displayStderrString
(
    const char    * string                    /* in */
)
{
    if( strlen( string ) != ( fprintf( stderr, "%s\n", string ) - 1 ) )
    {
        fillup_exception( __FILE__, __LINE__, ServiceException, 
                          "displayStderrString" );
        exitOnFailure( );
    }
}

/*----------------- displayString ------------------*/

void
displayString
(
    const char    * string                   /* in */
)
{
    if( strlen( string ) != fprintf( stderr, "%s", string ) )
    {
        fillup_exception( __FILE__, __LINE__, ServiceException, 
                          "displayString" );
        exitOnFailure( );
    }
}

/*----------------- displayValue -------------------*/

void
displayValue
(
    long            value                    /* in */
)
{
    ( void )fprintf( stderr, "%ld", value );
}

/*--------------- displayCharacter -----------------*/

void
displayCharacter
(
    char            character                /* in */
)
{
    ( void )fputc( character, stderr );
}

/*---------------- displayVersion ------------------*/

void
displayVersion
(
    void
)
{
    static const char *versionString = 
                      "This is fillup, version %1.2f, compiled on %s\n\n";

    if( ( strlen( versionString ) + strlen( __DATE__ ) - 3 ) != 
        fprintf( stderr, versionString, VERSION, __DATE__ ) )
    {
        fillup_exception( __FILE__, __LINE__, ServiceException, 
                          "displayVersion" );
        exitOnFailure( );
    }
}

/*------------------ getCardinal -------------------*/

unsigned long
getCardinal
(
    const char    * string                    /* in */
)
{
    long int        returnValue;

    returnValue = strtol( string, ( char ** )NULL, 10 );
    if( errno == ERANGE )
    {
        if( returnValue == LONG_MIN )
        {
            fillup_exception( __FILE__, __LINE__, ServiceException, 
                              "value overflow in getCardinal" );
        }
        else
        {
            fillup_exception( __FILE__, __LINE__, ServiceException, 
                              "value underflow in getCardinal" );
        }
        errno = 0;     /* reset of errno variable */
    }
    else if( returnValue < 0 )
    {
        fillup_exception( __FILE__, __LINE__, ServiceException, 
                          "negative value in getCardinal" );
    }

    return( ( unsigned long )returnValue );
}

/*-------------- openFileForReading ----------------*/

Service_t
openFileForReading
(
    const char    * filename,                 /* in */
    FILE         ** filePointer              /* out */
)
{
    Service_t       returnValue;

    *filePointer = fopen( filename, "r" );
    if( *filePointer == NULL )
    {
        fillup_exception( __FILE__, __LINE__, ServiceException, 
                          "file not opened" );
        returnValue = FileOpenError;
    }
    else
    {
        returnValue = FileOpened;
    }

    return( returnValue );
}

/*-------------- openFileForWriting ----------------*/

void
openFileForWriting
(
    const char    * filename                  /* in */
)
{
    filePointerForWriting = fopen( filename, "w" );
    if( filePointerForWriting == NULL )
    {
        fillup_exception( __FILE__, __LINE__, ServiceException, 
                          "file not opened" );
    }
}

/*------------------- closeFile --------------------*/

Service_t
closeFile
(
    FILE          * filePointer               /* in */
)
{
    Service_t       returnValue;

    if( 0 == fclose( filePointer ) )
    {
        returnValue = Success;
    }
    else
    {
        returnValue = Error;
    }

    return( returnValue );
}

/*-------------- closeFileForWriting ---------------*/

void
closeFileForWriting
(
    void
)
{
    ( void )fclose( filePointerForWriting );
}

/*----------------- getFileLength ------------------*/

Service_t
getFileLength
(
    FILE          * filePointer,              /* in */
    long          * fileLength               /* out */
)
{
    Service_t       returnValue;

    if( 0 == fseek( filePointer, 0, SEEK_END ) )
    {
        *fileLength = ftell( filePointer );
        if( *fileLength < 0 )
        {
            fillup_exception( __FILE__, __LINE__, ServiceException, 
                              "ftell" );
            returnValue = Error;
        }
        else
        {
            /* change within specification: empty files are now valid */
            returnValue = Success;
        }
    }
    else
    {
        fillup_exception( __FILE__, __LINE__, ServiceException, 
                          "fseek" );
        returnValue = Error;
    }

    return( returnValue );
}

/*--------------- readFileToBuffer -----------------*/

Service_t
readFileToBuffer
(
    FILE          * filePointer,              /* in */
    long            fileLength,               /* in */
    char         ** fileBuffer               /* out */
)
{
    Service_t       returnValue;

    if( 0 == fseek( filePointer, 0, SEEK_SET ) )
    {
        if( fileLength == 
            fread( *fileBuffer, sizeof( char ), fileLength, filePointer ) )
        {
            returnValue = Success;
        }
        else
        {
            fillup_exception( __FILE__, __LINE__, ServiceException, 
                              "fread" );
            returnValue = Error;
        }
    }
    else
    {
        fillup_exception( __FILE__, __LINE__, ServiceException, 
                          "fseek" );
        returnValue = Error;
    }
   
    return( returnValue );
}

/*-------------- writeVariableBlock ----------------*/

void
writeVariableBlock
(
    char          * buffer,                   /* in */
    long            length                    /* in */
)
{
    if( length != 
        fwrite( buffer, sizeof( char ), length, filePointerForWriting ) )
    {
        fillup_exception( __FILE__, __LINE__, ServiceException, 
                          "fwrite" );
    }
}

/*------------------ allocateBuffer -----------------*/

Service_t
allocateBuffer
(
    long            fileLength,                /* in */
    char         ** buffer                    /* out */
)
{
    Service_t       returnValue;

    *buffer = ( char * )malloc( fileLength + 1 );
    if( *buffer == NULL )
    {
        fillup_exception( __FILE__, __LINE__, ServiceException, 
                          "malloc" );
        returnValue = Error;
    }
    else
    {
        /* reset the buffer */
        ( void )memset( *buffer, EOF, fileLength + 1 );

        returnValue = Success;
    }
   
    return( returnValue );
}

/*-------------------- freeBuffer -------------------*/

Service_t
freeBuffer
(
    char                     ** buffer         /* in */
)
{
    Service_t       returnValue;

    free( *buffer );
    returnValue = Success;
   
    return( returnValue );
}

/*----------------------------------------------------------------------------*/
