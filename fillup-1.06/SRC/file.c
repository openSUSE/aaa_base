
/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Copyrights to S.u.S.E. GmbH Fuerth (c) 1996                                */
/*                                                                            */
/* Time-stamp:                                                                */
/* Project:    fillup                                                         */
/* Module:     file                                                           */
/* Filename:   file.c                                                         */
/* Author:     Joerg Dippel (jd )                                             */
/* Description:                                                               */
/*                                                                            */
/*----------------------------------------------------------------------------*/

/*--------------------------------- IMPORTS ----------------------------------*/

#include "parser.h"
#include "file.h"

/*-------------------------------- FUNCTIONS ---------------------------------*/

/*----------------------------- IMPLEMENTATION -------------------------------*/

/*-------------------- readFile --------------------*/
File_t
readFile 
(
    ParameterSpecification_t    fileSpecifier, /* in */
    const char                * filename       /* in */
)
{
    File_t                      returnValue;
    FILE                      * filePointer;
    long                        fileLength;
    char                      * buffer = NULL;

    if( FileOpened == openFileForReading( filename, &filePointer ) )
    {
        if( Success == getFileLength( filePointer, &fileLength ) )
        {
            if( Success ==
                allocateBuffer( fileLength, ( void ** )&buffer ) )
            {
                if( Success == 
                    readFileToBuffer( filePointer, fileLength, &buffer ) )
                {  
                    associateBuffer( fileSpecifier, fileLength, &buffer );
                    returnValue = FileOperationsSuccessful;
                }
                else
                {
                    freeBuffer( &buffer );
                    returnValue = FileOperationsFailed;
                }
            }
            else
            {
                returnValue = FileOperationsFailed;
            }
        }
        else
        {
            returnValue = FileOperationsFailed;
        }
        closeFile( filePointer );
    }
    else
    {
        returnValue = FileOperationsFailed;
    }

    return( returnValue );
}

/*----------------------------------------------------------------------------*/

