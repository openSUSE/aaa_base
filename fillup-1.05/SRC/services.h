
/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Copyrights to S.u.S.E. GmbH Fuerth (c) 1998                                */
/*                                                                            */
/* Time-stamp:                                                                */
/* Project:    fillup                                                         */
/* Module:     services                                                       */
/* Filename:   services.h                                                     */
/* Author:     Joerg Dippel (jd)                                              */
/* Description:                                                               */
/*                                                                            */
/*     export interface for services                                          */
/*                                                                            */
/*----------------------------------------------------------------------------*/

#ifndef __SERVICES_H__
#define __SERVICES_H__

/*--------------------------------- IMPORTS ----------------------------------*/

#include <stdio.h>

#include "portab.h"

/*---------------------------------- TYPES -----------------------------------*/

typedef enum
{
    FormatException,
    NumberOfFormatException,
    DefaultBranchException,
    ConfigurationException,
    ServiceException
} Exception_t;

typedef enum
{
    Success,
    Error,
    FileOpened,
    FileOpenError
} Service_t;

typedef enum
{
    Smaller,
    Equal,
    Greater
} StringOrder_t;

/*------------------- FUNCTION DECLARATIONS (PROTOTYPES) ---------------------*/

BOOLEAN
compareStrings
(
    const char    * firstString,              /* in */
    const char    * secondString              /* in */
);

BOOLEAN 
compareStringsExactly
(    
    const char    * firstString,              /* in */
    const char    * secondString              /* in */
);

StringOrder_t
sortStrings
(
    const char    * firstString,              /* in */
    const char    * secondString              /* in */
);

long
stringLength
(
    const char    * String                    /* in */
);

void
createNewBaseFileName
(
    const char    * oldString,                /* in */
          char    * newString                 /* in */
);

void
exitOnFailure
(
    void
);

void
fillup_exception
(
    const char    * fileName,                 /* in */
          int       lineNumber,               /* in */
    Exception_t     exceptionType,            /* in */
    const char    * description               /* in */
);

void
displayStderrString
(
    const char    * string                    /* in */
);

void
displayString
(
    const char    * string                    /* in */
);

void
displayValue
(
    long            value                    /* in */
);

void
displayCharacter
(   
    char            character                /* in */
);

void
displayVersion
(
    void
);

unsigned long
getCardinal
(
    const char    * string                    /* in */
);

Service_t
openFileForReading
(
    const char    * filename,                 /* in */
    FILE         ** filePointer              /* out */
);

void
openFileForWriting
(
    const char    * filename                  /* in */
);

Service_t
closeFile
(
    FILE          * filePointer               /* in */
);

void
closeFileForWriting
(
    void
);

Service_t
getFileLength
(
    FILE          * filePointer,              /* in */
    long          * fileLength               /* out */
);

Service_t
readFileToBuffer
(
    FILE          * filePointer,              /* in */
    long            fileLength,               /* in */
    char         ** fileBuffer               /* out */
);

void
writeVariableBlock
(
    char          * buffer,                   /* in */
    long            length                    /* in */
);

Service_t
allocateBuffer
(
    long            fileLength,               /* in */
    char         ** buffer                   /* out */
);

Service_t
freeBuffer
(
    char                     ** buffer         /* in */
);

/*----------------------------------------------------------------------------*/

#endif  __SERVICES_H__
