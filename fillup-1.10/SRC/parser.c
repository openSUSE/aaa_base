
/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Copyrights to S.u.S.E. GmbH Fuerth (c) 1998                                */
/* Copyrights to SuSE GmbH            (c) until 2001                          */
/* Copyrights to SuSE Linux AG        (c) 2002                                */
/*                                                                            */
/* Time-stamp:                                                                */
/* Project:    fillup                                                         */
/* Module:     parser                                                         */
/* Filename:   parser.c                                                       */
/* Author:     Joerg Dippel (jd)                                              */
/* Description:                                                               */
/*                                                                            */
/*     parses files                                                           */
/*                                                                            */
/*----------------------------------------------------------------------------*/

/*--------------------------------- IMPORTS ----------------------------------*/

#include <ctype.h>

#include "portab.h"
#include "variableblock.h"
#include "parameters.h"
#include "fillup_cfg.h"
#include "parser.h"

/*---------------------------------- MACROS ----------------------------------*/

#define  CONVCASE( a )          case ( a ): return( # a ); break

/*-------------------------------- VARIABLES ---------------------------------*/

static  char                  * baseFileBuffer;
static  char                  * additionalFileBuffer;
static  char                  * forbiddenFileBuffer;
static  long                    baseFileBufferLength;
static  long                    additionalFileBufferLength;
static  long                    forbiddenFileBufferLength;

static  VariableBlock_t       * baseFileBlock;
static  VariableBlock_t       * additionalFileBlock;
static  VariableBlock_t       * forbiddenFileBlock;
static  long                    baseFileBlocksLength;
static  long                    additionalFileBlocksLength;
static  long                    forbiddenFileBlocksLength;
static  VariableBlock_t       * headOfBaseFileList;
static  VariableBlock_t       * headOfAdditionalFileList;
static  VariableBlock_t       * headOfForbiddenFileList;

static  long                    numberOfUsedBaseBlocks;
static  long                    numberOfUsedAdditionalBlocks;
static  long                    numberOfUsedForbiddenBlocks;

/*---------------------------- LOCAL PROTOTYPES ------------------------------*/

static
unsigned long
countDelimiters
(
    char               delimiterChar,         /* in */
    char             * buffer,                /* in */
    long               bufferLength           /* in */
);

static
void
createAdministrationInfo
(
   void
);

static
long
consumeSpaces
(
    char           * buffer,                 /* in */
    long             lineLength              /* in */
);

static
long
consumePossibleVariableName
(
    char             delimiterStart,         /* in */
    char           * buffer,                 /* in */
    long             lineLength              /* in */
);

static
void
copyVariableName
(
    VariableBlock_t            * outputBuffer,     /* in */
    char                       * buffer            /* out */
);

static
char *
getClassifierSymbol
(
    BlockClass_t    Classifier                /* in */
);

static
char *
getEvalSymbol
(
    Eval_t          Class                     /* in */
);

static
void
getVariable
(
    VariableBlock_t            * outputBuffer,     /* in/out */
    long                         remainingInputChars   /* in */
);

static
void
printBlockInfo
(
    long                         outputIndex,     /* in */
    VariableBlock_t            * outputBuffer     /* in */
);

static
void
printList
(
    VariableBlock_t            * list         /* in */
);

static
void
displayVerboseString
( 
    char                       * verboseString  /* in */
);

static
void
displayVerboseValue
( 
    long                         verboseValue   /* in */
);

static
void
displayVerboseBuffer
( 
    char      * verboseBuffer,               /* in */
    long        verboseLength                /* in */
);

static
void
displayVerbose
( 
    char                       * verboseString, /* in */
    VariableBlock_t            * block          /* in */
);

static
void
sortBlockIntoList
(
    VariableBlock_t           ** head,
    VariableBlock_t           ** current,
    VariableBlock_t            * block
);

static
void
parseFile
(
    ParameterSpecification_t    parameterType,        /* in */
    long                      * numberOfUsedBlocks   /* out */
);

static
void
markList
(
    VariableBlock_t      * list,              /* in */
    Eval_t                 EvaluationClass,   /* in */
    char                 * verboseString      /* in */
);

static
BOOLEAN
checkForClassifier
(
    VariableBlock_t      * list, 
    char                 * variableName,
    BlockClass_t           classifier
);

static 
BOOLEAN 
checkForAssignment 
( 
    VariableBlock_t      * list, 
    char                 * variableName
); 

static
void
handleEqualVariableBlockIdentifiers
(
    void
);

static
void
setNext
(
    VariableBlock_t           ** list,      /* out */
    char                       * buffer     /* out */
);

static
void
evaluateTrailingComments
(
    void
);

static
void
writeBaseFileHeader
(
    FILE                      * filePointer
);

static
void
writeOutput
(
    void
);

/*----------------------------- IMPLEMENTATION -------------------------------*/

/*---------------- countDelimiters -----------------*\
|
|   Purpose:  For each variable block there is an administration overhead -- 
|             this memory is static, but configurable.
|             Only to estimate the configuration size this function is used.
|
\* -------------------------------------------------*/

static
unsigned long
countDelimiters
(
    char               delimiterChar,         /* in */
    char             * buffer,                /* in */
    long               bufferLength           /* in */
)
{
    unsigned long      Counter;
    unsigned long      Loop;
    char               readCharacter;
    BOOLEAN            carriageReturnDetected;


    Counter = 0;
    carriageReturnDetected = TRUE;
    for( Loop = 0; Loop < bufferLength; Loop++ )
    {
        readCharacter = buffer[ Loop ];
        if( ( readCharacter == delimiterChar ) && 
            ( carriageReturnDetected == TRUE ) )
        {
            carriageReturnDetected = FALSE;
            Counter++;
        }

        if( readCharacter == '\n' )
        {
            /* only one delimiter is counted per line */
            carriageReturnDetected = TRUE;             
        }
    }

    return( Counter );
}

/*------------ createAdministrationInfo ------------*\
|
|   Purpose:  This function creates buffers that can hold the
|             administration info for all variable blocks.
|
\* -------------------------------------------------*/

static
void
createAdministrationInfo
(
   void
)
{
    unsigned long      Size;
    char             * delimiterString;
    char               delimiterChar;

    queryStringParameter( Delimiter, &delimiterString );
    delimiterChar = delimiterString[ 0 ];


    baseFileBlocksLength =
        countDelimiters( delimiterChar, baseFileBuffer, baseFileBufferLength );
    baseFileBlocksLength++;    /* add possible trailing comment */
    Size = baseFileBlocksLength * sizeof( VariableBlock_t );
    if( Success != allocateBuffer( Size, ( void ** )&baseFileBlock ) )
    {
        fillup_exception( __FILE__, __LINE__, ConfigurationException,
                          "createAdministrationInfo" );
        exitOnFailure( );
    }

    additionalFileBlocksLength = countDelimiters( 
        delimiterChar, additionalFileBuffer, additionalFileBufferLength );
    additionalFileBlocksLength++;    /* add possible trailing comment */
    Size = additionalFileBlocksLength * sizeof( VariableBlock_t );
    if( Success != allocateBuffer( Size, ( void ** )&additionalFileBlock ) )
    {
        fillup_exception( __FILE__, __LINE__, ConfigurationException,
                          "createAdministrationInfo" );
        exitOnFailure( );
    }

    if( queryParameter( ForbiddenFile ) == TRUE )
    {
        forbiddenFileBlocksLength = countDelimiters( 
            delimiterChar, forbiddenFileBuffer, forbiddenFileBufferLength );
        forbiddenFileBlocksLength++;    /* add possible trailing comment */
        Size = forbiddenFileBlocksLength * sizeof( VariableBlock_t );
        if( Success != allocateBuffer( Size, ( void ** )&forbiddenFileBlock ) )
        {
            fillup_exception( __FILE__, __LINE__, ConfigurationException,
                              "createAdministrationInfo" );
            exitOnFailure( );
        }
    }
}

/*--------------- associateBuffer -----------------*/

void
associateBuffer
(
    ParameterSpecification_t    parameterType,     /* in */
    long                        bufferLength,      /* in */
    char                     ** buffer             /* in */
)
{
    if( parameterType == BaseFile )
    {
        baseFileBuffer = *buffer;
        baseFileBufferLength = bufferLength;
    }
    else if( parameterType == AdditionalFile )
    {
        additionalFileBuffer = *buffer;
        additionalFileBufferLength = bufferLength;
    }
    else     /* parameterType == ForbiddenFile */
    {
        forbiddenFileBuffer = *buffer;
        forbiddenFileBufferLength = bufferLength;
    }
}

/*---------------- consumeSpaces ------------------*/

static
long
consumeSpaces
(
    char           * buffer,                 /* in */
    long             lineLength              /* in */
)
{
    long             Counter;

    Counter = 0;
    while( ( Counter < lineLength ) && ( isspace( *buffer ) ) )
    {
        Counter++;
        buffer++;
    }

    return( Counter );
}

/*---------- consumePossibleVariableName ----------*/

static
long
consumePossibleVariableName
(
    char             delimiterStart,         /* in */
    char           * buffer,                 /* in */
    long             lineLength              /* in */
)
{
    long             Counter;

    if( ( isalpha( *buffer ) )  || ( *buffer == '_' ) )
    {
        Counter = 1;
        buffer++;
        while( ( Counter < lineLength )        &&
               ( ( isalnum( *buffer ) )   ||
                 ( *buffer == '_' )       ||
                 ( *buffer == '.' ) )          &&
               ( *buffer != delimiterStart ) )
        {
            Counter++;
            buffer++;
        }
    }
    else
    {
        Counter = 0;
    }

    return( Counter );
}

/*-------------- copyVariableName -----------------*\
|
|   Purpose:  This function copies the variable name of a variable block to a
|             buffer.
|             The buffer of size cfg_MaxVariableLength must be provided by 
|             the calling function.
|
\* -------------------------------------------------*/

static
void
copyVariableName
(
    VariableBlock_t            * outputBuffer,     /* in */
    char                       * buffer            /* out */
)
{
    char                       * charPointer;
    long                         variableLength;
    long                         bufferIndex;

    variableLength = getVOffsetOfDelimiter( outputBuffer ) - 
                     getVOffsetOfVariableName( outputBuffer );
    if( variableLength < ( cfg_MaxVariableLength - 1 ) )
    {
        getVBeginOfBlock( outputBuffer, &charPointer );
        charPointer = charPointer + getVOffsetOfVariableName( outputBuffer );
        for( bufferIndex = 0; bufferIndex < variableLength; bufferIndex++ )
        {
            buffer[ bufferIndex ] = *charPointer;
            charPointer++;
        }
        buffer[ bufferIndex ] = '\0';
    }
    else
    {
        fillup_exception( __FILE__, __LINE__, ConfigurationException,
                          "copyVariableName" );
        exitOnFailure( );
    }
}

/*------------- getClassifierSymbol ---------------*/

static
char *
getClassifierSymbol
(
    BlockClass_t    Classifier                /* in */
)
{
    switch( Classifier )
    {
        CONVCASE( UndefinedBlock );
        CONVCASE( CompleteVariableBlock );
        CONVCASE( CommentedVariableBlock );
        CONVCASE( TrailingCommentBlock );
        CONVCASE( AssignmentBlock );
        CONVCASE( VariableNameBlock );
        default:
            return( "unknown classifier" );
    }
}

/*---------------- getEvalSymbol ------------------*/

static
char *
getEvalSymbol
(
    Eval_t          Class                     /* in */
)
{
    switch( Class )
    {
        CONVCASE( Undefined );
        CONVCASE( Ignored );
        CONVCASE( IgnoredButRemoved );
        CONVCASE( Output );
        CONVCASE( OutputButRemoved );
        default:
            return( "unknown evaluation class" );
    }
}

/*----------------- getVariable --------------------*\
|
|   Purpose:  This function sets the administration info for one variable block.
|
\* -------------------------------------------------*/

static
void
getVariable
(
    VariableBlock_t            * outputBuffer,     /* in/out */
    long                         remainingInputChars   /* in */
)
{
    char    * Line;
    char    * LinePointer;
    char    * commentMarkerString;
    char    * delimiterString;
    long      lineLength;
    long      lineIndex;
    long      currentLengthOfBlock;
    long      offsetVariableName;
    long      offsetDelimiter;

    getVBeginOfBlock( outputBuffer, &Line );
    Line = Line + getVLength( outputBuffer );
    LinePointer = Line;
    for( lineIndex = 0; 
         ( *LinePointer != EOF ) && ( *LinePointer != '\n' ); 
         lineIndex++ )
    {
        LinePointer++;  /* concerns only the current line */
    }
    lineLength = lineIndex;
    if( *LinePointer == '\n' )
    {
        lineLength++;
    }
    queryStringParameter( CommentMarker, &commentMarkerString );
    queryStringParameter( Delimiter, &delimiterString );

    lineIndex = consumeSpaces( Line, lineLength );
    if( lineIndex == lineLength )
    {
        /* this is an empty line */
        addVLength( outputBuffer, lineLength );
        setVOffsetOfVariableName( outputBuffer, 0 );
        setVOffsetOfDelimiter( outputBuffer, 0 );
        incVNumberOfEmptyLines( outputBuffer );
        incVNumberOfCommentLines( outputBuffer );
        if( lineLength == remainingInputChars )
        {
            setVClassifier( outputBuffer, TrailingCommentBlock );
        }
    }
    else if( ( ( lineIndex + stringLength( commentMarkerString ) ) < lineLength )
         &&  ( Equal == 
               compareStringsExactly( commentMarkerString, &Line[ lineIndex ] ) ) )
    {
        /* here the comment marker is detected */
        lineIndex = lineIndex + stringLength ( commentMarkerString );
        lineIndex += consumeSpaces( &Line[ lineIndex ], lineLength - lineIndex );

        offsetVariableName = lineIndex;
        lineIndex = lineIndex +
            consumePossibleVariableName( 
                delimiterString[ 0 ], 
                &Line[ lineIndex ], 
                lineLength - lineIndex );
        offsetDelimiter = lineIndex;

        if( ( ( lineIndex + stringLength( delimiterString ) ) < lineLength ) &&
            ( Equal == 
              compareStringsExactly( delimiterString, &Line[ lineIndex ] ) ) )
        {
            /* this is an assignment within a comment */
            setVClassifier( outputBuffer, CommentedVariableBlock );
            currentLengthOfBlock = getVLength( outputBuffer );
            setVOffsetOfVariableName( outputBuffer,  
                currentLengthOfBlock + offsetVariableName );
            setVOffsetOfDelimiter( outputBuffer,
                currentLengthOfBlock + offsetDelimiter );
            addVLength( outputBuffer, lineLength );
        }
        else
        {
            /* this is a line of comment */
            addVLength( outputBuffer, lineLength );
            incVNumberOfCommentLines( outputBuffer );
            if( lineLength == remainingInputChars )
            {
                setVClassifier( outputBuffer, TrailingCommentBlock );
            }
        }
    }
    else
    {
        offsetVariableName = lineIndex;
        offsetDelimiter = lineIndex;
        lineIndex = lineIndex +
            consumePossibleVariableName( 
                delimiterString[ 0 ], 
                &Line[ lineIndex ], 
                lineLength - lineIndex );

        if( lineIndex == offsetDelimiter )
        {
            /* this is unspecified behaviour because   */
            /* consumePossibleVariableName() == 0      */

            /* last line starts with optional white spaces    */
            /* followed by something that's neither a comment */
            /* nor a possible variable name                   */
            setVClassifier( outputBuffer, UndefinedBlock );
        }
        else
        {
            offsetDelimiter = lineIndex;

            if( ( ( lineIndex + stringLength( delimiterString ) ) < lineLength ) &&
                ( Equal == 
                  compareStringsExactly( delimiterString, &Line[ lineIndex ] ) ) )
            {
                /* this is an assignment */
                if( getVNumberOfCommentLines( outputBuffer ) > 0 )
                {
                    setVClassifier( outputBuffer, CompleteVariableBlock );
                }
                else
                {
                    setVClassifier( outputBuffer, AssignmentBlock );
                }
            }
            else
            {
                /* this a variable, maybe followed by something */
                setVClassifier( outputBuffer, VariableNameBlock );
            }
        }
        currentLengthOfBlock = getVLength( outputBuffer );
        setVOffsetOfVariableName( outputBuffer,  
            currentLengthOfBlock + offsetVariableName );
        setVOffsetOfDelimiter( outputBuffer,
            currentLengthOfBlock + offsetDelimiter );
        addVLength( outputBuffer, lineLength );
    }
}

/*---------------- printBlockInfo ------------------*/

static
void
printBlockInfo
(
    long                         outputIndex,     /* in */
    VariableBlock_t            * outputBuffer     /* in */
)
{
    char                         variableName[ cfg_MaxVariableLength ];
    char                       * blockBuffer;

    displayVerboseString( "\n\n##################################\n" );
    displayVerboseString( " VariableBlockNo: " );
    displayVerboseValue( outputIndex );
    copyVariableName( outputBuffer, variableName );
    displayVerboseString( "\n VariableName:    " );
    displayVerboseString( variableName );
    displayVerboseString( "\n##################################\n" );
    displayVerboseString( " Classifier:      " );
    displayVerboseString( getClassifierSymbol( outputBuffer->Classifier ) );
    displayVerboseString( "\n##################################\n" );
    displayVerboseString( " VariableBlock:\n" );
    getVBeginOfBlock( outputBuffer, &blockBuffer );
    displayVerboseBuffer( blockBuffer, getVLength( outputBuffer ) );
    displayVerboseString( "##################################\n\n\n" );
}

/*------------------ printList ---------------------*/

static
void
printList
(
    VariableBlock_t            * list         /* in */
)
{
    char                         variableName[ cfg_MaxVariableLength ];

    while( list != NULL )
    {
        copyVariableName( list, variableName );
        displayVerboseString( "<<<List>>> VariableName:    " );
        displayVerboseString( variableName );
        displayVerboseString( "\n" );
        list = getVSucc( list );
    }
}

/*------------ displayVerboseString ---------------*/

static
void
displayVerboseString
( 
    char                       * verboseString  /* in */
)
{
    if( ( TRUE == queryParameter( Verbose ) ) )
    {
        displayString( verboseString );
    }
}

/*------------ displayVerboseValue ----------------*/

static
void
displayVerboseValue
( 
    long                         verboseValue   /* in */
)
{
    if( ( TRUE == queryParameter( Verbose ) ) )
    {
        displayValue( verboseValue );
    }
}

/*------------ displayVerboseBuffer ---------------*/

static
void
displayVerboseBuffer
( 
    char      * verboseBuffer,               /* in */
    long        verboseLength                /* in */
)
{
    long        index;

    if( ( TRUE == queryParameter( Verbose ) ) )
    {
        for( index = 0; index < verboseLength; index++ )
        {
             displayCharacter( *verboseBuffer );
             verboseBuffer++;
        }
    }
}

/*--------------- displayVerbose ------------------*/

static
void
displayVerbose
( 
    char                       * verboseString, /* in */
    VariableBlock_t            * block          /* in */
)
{
    char                VariableNameOfBlock[ cfg_MaxVariableLength ];

    if( ( block != NULL ) && ( TRUE == queryParameter( Verbose ) ) )
    {
        copyVariableName( block, VariableNameOfBlock );
        displayString( "Evaluating " );
        displayString( verboseString );
        displayString( " file:  " );
        displayString( VariableNameOfBlock );
        displayString( "  ->  " );
        displayString( getEvalSymbol( getVEvaluationClass( block ) ) );
        displayString( "  -  " );
        displayString( getClassifierSymbol( getVClassifier( block ) ) );
        displayString( "\n" );
    }
}

/*-------------- sortBlockIntoList ----------------*\
|
|   Purpose:  This function sorts the variable blocks within a double chained
|             list -- the chain is created via setVPred and setVSucc.
|
\* ------------------------------------------------*/

static
void
sortBlockIntoList
(
    VariableBlock_t           ** head,
    VariableBlock_t           ** current,
    VariableBlock_t            * block
)
{
    char                VariableNameOfBlock[ cfg_MaxVariableLength ];
    char                VariableNameOfCurrentListElement[ cfg_MaxVariableLength ];
    StringOrder_t       serviceResult;
    VariableBlock_t   * predecessor;
    VariableBlock_t   * successor;

    copyVariableName( block, VariableNameOfBlock );
    copyVariableName( *current, VariableNameOfCurrentListElement );

    serviceResult = 
        compareStrings( VariableNameOfBlock, VariableNameOfCurrentListElement );
    if( serviceResult == Equal )
    {
        /* insert the block after *current directly */
        setVPred( block, *current );
        setVSucc( block, getVSucc( *current ) );
        setVSucc( *current, block );
        *current = block;
    }
    else if( serviceResult == Smaller )
    {
        /* insert the block before *current */
        predecessor = getVPred( *current );
        if( predecessor != NULL )
        {
            copyVariableName( predecessor, VariableNameOfCurrentListElement );
            serviceResult = compareStrings( 
                VariableNameOfBlock, VariableNameOfCurrentListElement );
        }
        while( ( predecessor != NULL ) && ( serviceResult == Smaller ) )
        {
            *current = predecessor;            
            predecessor = getVPred( *current );
            if( predecessor != NULL )
            {
                copyVariableName( predecessor, VariableNameOfCurrentListElement );
                serviceResult = compareStrings( 
                    VariableNameOfBlock, VariableNameOfCurrentListElement );
            }
        }
        if( predecessor != NULL )
        {
            setVPred( block, predecessor );
            setVSucc( block, *current );
            setVPred( *current, block );
            setVSucc( predecessor, block );
        }
        else
        {
            setVPred( block, NULL );
            setVSucc( block, *current );
            setVPred( *current, block );
            *head = block;
        }
        *current = block;
    }
    else     /* serviceResult == Greater */
    {
        /* insert the block after *current */
        successor = getVSucc( *current );
        if( successor != NULL )
        {
            copyVariableName( successor, VariableNameOfCurrentListElement );
            serviceResult = compareStrings( 
                VariableNameOfBlock, VariableNameOfCurrentListElement );
        }
        while( ( successor != NULL ) && ( serviceResult != Smaller ) )
        {
            *current = successor;            
            successor = getVSucc( *current );
            if( successor != NULL )
            {
                copyVariableName( successor, VariableNameOfCurrentListElement );
                serviceResult = compareStrings( 
                    VariableNameOfBlock, VariableNameOfCurrentListElement );
            }
        }
        if( successor != NULL )
        {
            setVPred( block, *current );
            setVSucc( block, successor );
            setVSucc( *current, block );
            setVPred( successor, block );
        }
        else
        {
            setVPred( block, *current );
            setVSucc( block, NULL );
            setVSucc( *current, block );
        }
        *current = block;
    }
}

/*------------------ parseFile --------------------*\
|
|   Purpose:  This function sets the segments for variable blocks
|             within the buffer that holds the complete file:
|             the administration structure is created and assigned.
|
\* ------------------------------------------------*/

static
void
parseFile
(
    ParameterSpecification_t    parameterType,        /* in */
    long                      * numberOfUsedBlocks   /* out */
)
{
    char                       * inputBuffer;
    long                         inputLength;
    VariableBlock_t            * outputBuffer;
    long                         outputLength;
    long                         outputIndex;
    VariableBlock_t            * headOfList;
    VariableBlock_t            * currentOfList;

    switch( parameterType )
    {
        case BaseFile:
            inputBuffer = baseFileBuffer;
            inputLength = baseFileBufferLength;
            outputBuffer = baseFileBlock;
            outputLength = baseFileBlocksLength;
            break;

        case AdditionalFile:
            inputBuffer = additionalFileBuffer;
            inputLength = additionalFileBufferLength;
            outputBuffer = additionalFileBlock;
            outputLength = additionalFileBlocksLength;
            break;

        case ForbiddenFile:
            inputBuffer = forbiddenFileBuffer;
            inputLength = forbiddenFileBufferLength;
            outputBuffer = forbiddenFileBlock;
            outputLength = forbiddenFileBlocksLength;
            break;

         default:
             fillup_exception( __FILE__, __LINE__, DefaultBranchException,
                               "parseFile" );
             break;
    }

    headOfList = NULL;
    currentOfList = NULL;

    outputIndex = 0;
    while( ( inputLength > 0 ) && ( outputIndex < outputLength ) )
    {
        setVClassifier( &outputBuffer[ outputIndex ], UndefinedBlock );
        setVEvaluationClass( &outputBuffer[ outputIndex ], Undefined );
        setVAssociation( &outputBuffer[ outputIndex ], NULL );
        setVBeginOfBlock( &outputBuffer[ outputIndex ], inputBuffer );
        setVLength( &outputBuffer[ outputIndex ], 0 );
        setVOffsetOfVariableName( &outputBuffer[ outputIndex ], 0 ); 
        setVOffsetOfDelimiter( &outputBuffer[ outputIndex ], 0 );
        setVNumberOfEmptyLines( &outputBuffer[ outputIndex ], 0 );
        setVNumberOfCommentLines( &outputBuffer[ outputIndex ], 0 );

        while( outputBuffer[ outputIndex ].Classifier == UndefinedBlock )
        {
            getVariable( &outputBuffer[ outputIndex ], 
                inputLength - getVLength( &outputBuffer[ outputIndex ] ) );
        } 

        /* now sort the current variable block list */
        if( headOfList == NULL )
        {
            headOfList = &outputBuffer[ outputIndex ];
            currentOfList = &outputBuffer[ outputIndex ];

            setVPred( &outputBuffer[ outputIndex ], NULL );
            setVSucc( &outputBuffer[ outputIndex ], NULL );
        }
        else
        {
            sortBlockIntoList( 
                &headOfList, &currentOfList, &outputBuffer[ outputIndex ] );
        }

#if DEBUG
        printBlockInfo( outputIndex, &outputBuffer[ outputIndex ] ); 
        printList( headOfList );
#endif

        inputBuffer = inputBuffer + getVLength( &outputBuffer[ outputIndex ] );
        inputLength = inputLength - getVLength( &outputBuffer[ outputIndex ] );
        outputIndex++;
    }

    switch( parameterType )
    {
        case BaseFile:
            headOfBaseFileList = headOfList;
            break;

        case AdditionalFile:
            headOfAdditionalFileList = headOfList;
            break;

        case ForbiddenFile:
            headOfForbiddenFileList = headOfList;
            break;

         default:
             fillup_exception( __FILE__, __LINE__, DefaultBranchException,
                               "parseFile" );
             break;
    }
    *numberOfUsedBlocks = outputIndex;
}

/*-------------------- markList --------------------*/

static
void
markList
(
    VariableBlock_t      * list,              /* in */
    Eval_t                 EvaluationClass,   /* in */
    char                 * verboseString      /* in */
)
{
    while( list != NULL )
    {
        setVEvaluationClass( list, EvaluationClass );
        displayVerbose( verboseString, list );
        list = getVSucc( list );
    }
}

/*------------- checkForClassifier ------------*\
|
|   Purpose:  Here a variable block list is checked
|             for a specific evaluation class.
|
\* -------------------------------------------------*/

static
BOOLEAN
checkForClassifier
(
    VariableBlock_t      * list, 
    char                 * variableName,
    BlockClass_t           classifier
)
{
    BOOLEAN                returnValue;
    char                   buffer[ cfg_MaxVariableLength ];

    returnValue = FALSE;

    copyVariableName( list, buffer );
    while( ( list != NULL ) && 
           ( Equal == compareStrings( variableName, buffer ) ) )
    {
        if( classifier == getVClassifier( list ) )
        {
            returnValue = TRUE;
            break;
        }
        else
        {
            list = getVSucc( list );
            if( list != NULL ) copyVariableName( list, buffer );
        }
    }

    return( returnValue );
}

/*--------------- checkForAssignment ---------------*\
|
|   Purpose:  Here a variable block list is checked
|             whether the last line holds an assignemnt.
|             For ignoreDefinites option only the
|             variable name has to be provided.
|
\* -------------------------------------------------*/

static
BOOLEAN
checkForAssignment
(
    VariableBlock_t      * list, 
    char                 * variableName
)
{
    BOOLEAN                returnValue;

    returnValue = FALSE;

    if( TRUE == checkForClassifier( list, variableName, CompleteVariableBlock ) )
    {
        returnValue = TRUE;
    }
    else if( TRUE == checkForClassifier( list, variableName, AssignmentBlock ) )
    {
        returnValue = TRUE;
    }
    else if( ( TRUE == queryParameter( IgnoreDefinites ) ) &&
             ( TRUE == checkForClassifier( list, variableName, VariableNameBlock ) ) )
    {
        returnValue = TRUE;
    }
    else if( ( VariableNameBlock == getVClassifier( list ) )  &&
           ( Equal == compareStrings( variableName, "test" ) ) )
    {
        /* this is a very special case for /etc/rc.config -
           a historical relict:
           ##
           ## Formatting the boot script messages, see /etc/rc.status.
           ## Source /etc/rc.status if rc_done isn't defined
           ##
           test "$rc_done"= = = -a -e /etc/rc.status && . /etc/rc.status
         */
        returnValue = TRUE;
    }

    return( returnValue );
}

/*------ handleEqualVariableBlockIdentifiers -------*\
|
|   Purpose:  Here the functionality is located concerning the options --remove
|             and the complementairy pair --maintain and --exchange.
|
\* -------------------------------------------------*/

static
void
handleEqualVariableBlockIdentifiers
(
    void
)
{
    char             firstBuffer[ cfg_MaxVariableLength ];
    char             secondBuffer[ cfg_MaxVariableLength ];
    Eval_t           evaluationClass;

    copyVariableName( headOfBaseFileList, firstBuffer );
    copyVariableName( headOfAdditionalFileList, secondBuffer );

    /* the following three conditions are fullfilled:           */
    /* ( headOfBaseFileList != NULL )                           */
    /* ( headOfAdditionalFileList != NULL )                     */ 
    /* ( Equal == compareStrings( firstBuffer, secondBuffer ) ) */

    if( TRUE == queryParameter( Exchange ) )
    {
        if( ( TRUE == checkForAssignment( headOfBaseFileList, firstBuffer ) ) &&
            ( TRUE == checkForAssignment( headOfAdditionalFileList, secondBuffer ) ) )
        {
            if( TRUE == queryParameter( Remove) )
            {
                setVEvaluationClass( headOfBaseFileList, IgnoredButRemoved );
            }
            else
            {
                setVEvaluationClass( headOfBaseFileList, Ignored );
            }
            displayVerbose( "base", headOfBaseFileList );
            setVEvaluationClass( headOfAdditionalFileList, Output );
            displayVerbose( "additional", headOfAdditionalFileList );
        } 
        else
        {
            setVEvaluationClass( headOfBaseFileList, Output );
            displayVerbose( "base", headOfBaseFileList );
            setVEvaluationClass( headOfAdditionalFileList, Ignored );
            displayVerbose( "additional", headOfAdditionalFileList );
        }
    }
    else    /* ( TRUE == queryParameter( Maintain ) ) */
    {
        if( ( TRUE == checkForAssignment( headOfBaseFileList, firstBuffer ) ) &&
            ( TRUE == checkForAssignment( headOfAdditionalFileList, secondBuffer ) ) )
        {
            if( TRUE == queryParameter( Remove) )
            {
                setVEvaluationClass( headOfBaseFileList, OutputButRemoved );
            }
            else
            {
                setVEvaluationClass( headOfBaseFileList, Output );
            }
            displayVerbose( "base", headOfBaseFileList );
            setVEvaluationClass( headOfAdditionalFileList, Ignored );
            displayVerbose( "additional", headOfAdditionalFileList );
        }
        else
        {
            if( FALSE == checkForAssignment( headOfBaseFileList, firstBuffer ) )
            {
                /* doesn't make sense to add only CommentedVariableBlocks */
                setVEvaluationClass( headOfBaseFileList, IgnoredButRemoved );
            }
            else
            {
                setVEvaluationClass( headOfBaseFileList, Ignored );
            }
            displayVerbose( "base", headOfBaseFileList );
            if( FALSE == checkForAssignment( headOfAdditionalFileList, secondBuffer ) )
            {
                /* doesn't make sense to add only CommentedVariableBlocks */
                setVEvaluationClass( headOfAdditionalFileList, Ignored );
            }
            else
            {
                setVEvaluationClass( headOfAdditionalFileList, Output );
            }
            displayVerbose( "additional", headOfAdditionalFileList );
        }
    }

    evaluationClass = getVEvaluationClass( headOfBaseFileList );
    setNext( &headOfBaseFileList, firstBuffer );
    while( ( headOfBaseFileList != NULL ) &&
           ( Equal == compareStrings( firstBuffer, secondBuffer ) ) )
    {
        /* same vairiable name => same evaluation class */
        setVEvaluationClass( headOfBaseFileList, evaluationClass );
        displayVerbose( "base", headOfBaseFileList );
        setNext( &headOfBaseFileList, firstBuffer );
    }

    evaluationClass = getVEvaluationClass( headOfAdditionalFileList );
    setNext( &headOfAdditionalFileList, firstBuffer );
    while( ( headOfAdditionalFileList != NULL ) &&
           ( Equal == compareStrings( firstBuffer, secondBuffer ) ) ) 
    {
        /* same vairiable name => same evaluation class */
        setVEvaluationClass( headOfAdditionalFileList, evaluationClass ); 
        displayVerbose( "additional", headOfAdditionalFileList );
        setNext( &headOfAdditionalFileList, firstBuffer );
    }
}

/*-------------------- setNext ---------------------*\
|
|   Purpose:  Set the list to the successor within the double chained list and
|             copies the variable name into the provided buffer.
|
\* -------------------------------------------------*/

static
void
setNext
(
    VariableBlock_t           ** list,      /* out */
    char                       * buffer     /* out */
)
{
   *list = getVSucc( *list );
   if( *list != NULL )
   {
       copyVariableName( *list, buffer );
   }
}

/*------------ evaluateTrailingComments ------------*\
|
|   Purpose:  Trailing comments are marked as a 'TrailingCommentBlock', but also
|             they contain an empty variable name. Therefore a string comparison
|             would fail and the comments have to be removed from the list before.
|
\* -------------------------------------------------*/

static
void
evaluateTrailingComments
(
    void
)
{
    while( ( headOfBaseFileList != NULL ) &&
           ( TrailingCommentBlock == getVClassifier( headOfBaseFileList ) ) )
    {
        if( TRUE == queryParameter( TrailingComment ) )
        {
            setVEvaluationClass( headOfBaseFileList, Output );
        }
        else
        {
            setVEvaluationClass( headOfBaseFileList, Ignored );
        }
        displayVerbose( "base", headOfBaseFileList );
        headOfBaseFileList = getVSucc( headOfBaseFileList );
    }

    /* ignore empty variable names within additional file */
    while( ( headOfAdditionalFileList != NULL ) &&
           ( TrailingCommentBlock == getVClassifier( headOfAdditionalFileList ) ) )
    {
        setVEvaluationClass( headOfAdditionalFileList, Ignored );
        displayVerbose( "additional", headOfAdditionalFileList );
        headOfAdditionalFileList = getVSucc( headOfAdditionalFileList );
    }

    /* ignore empty variable names within forbidden file */
    while( ( headOfForbiddenFileList != NULL ) &&
           ( TrailingCommentBlock == getVClassifier( headOfForbiddenFileList ) ) )
    {
        setVEvaluationClass( headOfForbiddenFileList, Ignored );
        displayVerbose( "forbidden", headOfForbiddenFileList );
        headOfForbiddenFileList = getVSucc( headOfForbiddenFileList );
    }
}

/*------------- evaluateParsingResult --------------*\
|
|   Purpose:  The lists of variable blocks that are generated from the files
|             are there. Now the heads of the list are compared and the lists
|             are evaluated successively.
|             The chained (and sorted) lists are reduced but the unsorted lists
|             remain accessible.
|
\* -------------------------------------------------*/

static
void
evaluateParsingResult
(
    void
)
{
    char             baseBuffer[ cfg_MaxVariableLength ];
    char             additionalBuffer[ cfg_MaxVariableLength ];
    char             forbiddenBuffer[ cfg_MaxVariableLength ];
    BOOLEAN          thereIsAtLeastOneVariableBlock;
    StringOrder_t    serviceResult;

    evaluateTrailingComments( );

    thereIsAtLeastOneVariableBlock = FALSE;
    if( headOfBaseFileList != NULL )
    {
        copyVariableName( headOfBaseFileList, baseBuffer );
        thereIsAtLeastOneVariableBlock = TRUE;
    }
    if( headOfAdditionalFileList != NULL )
    {
        copyVariableName( headOfAdditionalFileList, additionalBuffer );
        thereIsAtLeastOneVariableBlock = TRUE;
    }
    if( headOfForbiddenFileList != NULL )
    {
        copyVariableName( headOfForbiddenFileList, forbiddenBuffer );
        thereIsAtLeastOneVariableBlock = TRUE;
    }

    while( thereIsAtLeastOneVariableBlock == TRUE )
    {
        if( headOfBaseFileList == NULL )
        {
            if( headOfAdditionalFileList == NULL )
            {
                /* nothing to mark */
                thereIsAtLeastOneVariableBlock = FALSE;
            }
            else    /* headOfAdditionalFileList != NULL */
            {
                markList( headOfAdditionalFileList, Output, "additional" );
                thereIsAtLeastOneVariableBlock = FALSE;
            }
        }
        else   /* headOfBaseFileList != NULL */
        {
            if( headOfAdditionalFileList == NULL )
            {
                if( headOfForbiddenFileList == NULL )
                {
                    if( TRUE == queryParameter( IgnoreDefinites ) )
                    {
                        markList( headOfBaseFileList, Ignored, "base" );
                    }
                    else
                    {
                        markList( headOfBaseFileList, Output, "base" );
                    }
                    thereIsAtLeastOneVariableBlock = FALSE;
                }
                else    /* headOfForbiddenFileList != NULL */
                {
                    serviceResult = compareStrings( baseBuffer, forbiddenBuffer );
                    if( serviceResult == Smaller )
                    {
                        setVEvaluationClass( headOfBaseFileList, Output );
                        displayVerbose( "base", headOfBaseFileList );
                        setNext( &headOfBaseFileList, baseBuffer );
                    }
                    else if( serviceResult == Greater )
                    {
                        displayVerbose( "forbidden", headOfForbiddenFileList );
                        setNext( &headOfForbiddenFileList, forbiddenBuffer );
                    }
                    else    /* serviceResult == Equal */
                    {
                        setVEvaluationClass( headOfBaseFileList, Output );
                        displayVerbose( "base", headOfBaseFileList );
                        setNext( &headOfBaseFileList, baseBuffer );
                    }
                }
            }
            else    /* headOfAdditionalFileList != NULL */
            {
                if( headOfForbiddenFileList == NULL )
                {
                    serviceResult = compareStrings( baseBuffer, additionalBuffer );
                    if( serviceResult == Smaller )
                    {
                        if( TRUE == queryParameter( IgnoreDefinites ) )
                        {
                            setVEvaluationClass( headOfBaseFileList, Ignored );
                        }
                        else
                        {
                            setVEvaluationClass( headOfBaseFileList, Output );
                        }
                        displayVerbose( "base", headOfBaseFileList );
                        setNext( &headOfBaseFileList, baseBuffer );
                    }
                    else if( serviceResult == Greater )
                    {
                        setVEvaluationClass( headOfAdditionalFileList, Output );
                        displayVerbose( "additional", headOfAdditionalFileList );
                        setNext( &headOfAdditionalFileList, additionalBuffer );
                    }
                    else    /* serviceResult == Equal */
                    {
                        handleEqualVariableBlockIdentifiers( ); 
                        
                        if( headOfAdditionalFileList != NULL )
                        {
                            copyVariableName( 
                                headOfAdditionalFileList, additionalBuffer );
                        }
                        if( headOfBaseFileList != NULL )
                        {
                            copyVariableName( headOfBaseFileList, baseBuffer );
                        }
                    }
                }
                else    /* headOfForbiddenFileList != NULL */
                {
                    serviceResult = compareStrings( baseBuffer, forbiddenBuffer );
                    if( serviceResult == Greater )
                    {
                        displayVerbose( "forbidden", headOfForbiddenFileList );
                        setNext( &headOfForbiddenFileList, forbiddenBuffer );
                    }
                    else if( serviceResult == Equal )
                    {
                        serviceResult = 
                            compareStrings( baseBuffer, additionalBuffer );
                        if( serviceResult == Smaller )
                        {
                            setVEvaluationClass( headOfBaseFileList, Output );
                            displayVerbose( "base", headOfBaseFileList );
                            setNext( &headOfBaseFileList, baseBuffer );
                        }
                        else
                        {
                            setVEvaluationClass( 
                                headOfAdditionalFileList, Ignored );
                            displayVerbose( 
                                "additional", headOfAdditionalFileList );
                            setNext( 
                                &headOfAdditionalFileList, additionalBuffer );
                        }
                    }
                    else    /* serviceResult == Smaller */
                    {
                        serviceResult = 
                            compareStrings( baseBuffer, additionalBuffer );
                        if( serviceResult == Greater )
                        {
                            setVEvaluationClass( 
                                headOfAdditionalFileList, Output );
                            displayVerbose( 
                                "additional", headOfAdditionalFileList );
                            setNext( 
                                &headOfAdditionalFileList, additionalBuffer );
                        }
                        else if( serviceResult == Equal )
                        {
                            handleEqualVariableBlockIdentifiers( );

                            if( headOfAdditionalFileList != NULL )
                            {
                                copyVariableName(
                                    headOfAdditionalFileList, additionalBuffer );
                            }
                            if( headOfBaseFileList != NULL )
                            {
                                copyVariableName( headOfBaseFileList, baseBuffer );
                            } 
                        }
                        else
                        {
                            if( TRUE == queryParameter( IgnoreDefinites ) )
                            {
                                setVEvaluationClass( headOfBaseFileList, Ignored );
                            }
                            else
                            {
                                setVEvaluationClass( headOfBaseFileList, Output );
                            }
                            displayVerbose( "base", headOfBaseFileList );
                            setNext( &headOfBaseFileList, baseBuffer );
                        }
                    }
                }
            }
        }
    }       
}

/*-------------- writeBaseFileHeader ---------------*/

static
void
writeBaseFileHeader
(
    FILE                      * filePointer
)
{
    char                      * baseFileHeader;
    char                      * commentMarkerString;
    char                      * delimiterString;
    long                        commentMarkerStringLength;
    long                        endOfHeader;
    BlockClass_t                classifier;
    long                        length;
    long                        loop;

    getVBeginOfBlock( baseFileBlock, &baseFileHeader );
    length = getVLength( baseFileBlock );

    queryStringParameter( CommentMarker, &commentMarkerString );
    queryStringParameter( Delimiter, &delimiterString );
    commentMarkerStringLength = stringLength( commentMarkerString );

    loop = 0;
    endOfHeader = 0;
    while( loop < length )
    {
       loop += consumeSpaces( &baseFileHeader[ loop ], length );
       if( ( loop + commentMarkerStringLength ) < length )
       {
           if( Equal ==
               compareStringsExactly( commentMarkerString, 
                                      &baseFileHeader[ loop ] ) )
           {
               /* here the comment marker is detected */
               /* and line is read upto newline       */
               loop += commentMarkerStringLength;
               while( loop < length )
               {
                   if( '\n' != baseFileHeader[ loop ] ) loop++;
                   else
                   {
                       endOfHeader = loop;
                       if( length < endOfHeader ) endOfHeader = 0; /*error*/ 
                       break;
                   }
               }

               /* check for end of header:                  */
               /* the criterion is a newline character next */
               if( '\n' == baseFileHeader[ endOfHeader + 1 ] )
               {
                   /* take also the newline itself */
                   endOfHeader += 2;
                   if( length < endOfHeader ) endOfHeader = 0; /*error*/
                   break;
               }
           }
           else
           {
               /* no comment */
               break;
           }
       }
       else
       {
           /* no comment */
           break;
       }

       loop++;
    }

    classifier = getVClassifier( baseFileBlock );
    switch( classifier )
    {
        case CompleteVariableBlock:
        case CommentedVariableBlock:
             {
                 displayVerboseString( "Header is written\n" );
                 writeVariableBlock( baseFileHeader, endOfHeader, filePointer );
             } break;

        case TrailingCommentBlock:
             {
                 displayVerboseString( "Single trailing comment as header is written\n" );
                 writeVariableBlock( baseFileHeader, endOfHeader, filePointer );
             } break;
             
        default: break;   /* do nothing */
    }
}

/*------------------ writeOutput -------------------*/

static
void
writeOutput
(
    void
)
{
    VariableBlock_t           * listPointer;
    Eval_t                      evaluationClass;
    long                        index;
    char                      * baseFileName;
    char                      * outputFileName;
    char                        newBaseFileName[ cfg_MaxVariableLength ];
    char                      * variableBlock;
    FILE                      * filePointer;

    
    if( TRUE == queryParameter( Remove ) )
    {
        displayVerboseString( "\nNow the new base file is displayed\n" );
        queryStringParameter( BaseFile, &baseFileName );
        if( ( Success == createNewBaseFileName( baseFileName, newBaseFileName ) ) 
        &&  ( FileOpened == openFileForWriting( newBaseFileName, &filePointer ) ) )
        {
            listPointer = baseFileBlock;
            evaluationClass = getVEvaluationClass( listPointer );
            switch( evaluationClass )
            {
                case Output:
                    displayVerbose( "base", listPointer );
                    getVBeginOfBlock( listPointer, &variableBlock );
                    writeVariableBlock( variableBlock, getVLength( listPointer ), filePointer );
                    break;

                case Ignored:
                    if( TrailingCommentBlock == getVClassifier( listPointer ) )
                    {
                       /* new functionality for fillup-1.10:           */
                       /* if basefile variables should be removed      */
                       /* and basefile holds only a trailing comment   */
                       /* which includes a header                      */
                       /* this header is preserved within basefile.new.*/
                       writeBaseFileHeader( filePointer );
                    }
                    else
                    {
                       displayVerbose( "base", listPointer );
                       getVBeginOfBlock( listPointer, &variableBlock );
                       writeVariableBlock( variableBlock, getVLength( listPointer ), filePointer );
                    }
                    break;

                default: 
                    /* new functionality for fillup-1.10:           */
                    /* only if basefile variables should be removed */
                    /* and the first variable includes a header     */
                    /* this header is preserved within basefile.new.*/
                    writeBaseFileHeader( filePointer );
                    break;
            }

            listPointer++;
            for( index = 1; index < numberOfUsedBaseBlocks; index++ )
            {
                evaluationClass = getVEvaluationClass( listPointer );
                switch( evaluationClass )
                {
                    case Output:
                    case Ignored:
                        displayVerbose( "base", listPointer );
                        getVBeginOfBlock( listPointer, &variableBlock );
                        writeVariableBlock( variableBlock, getVLength( listPointer ), filePointer );
                        break;

                    default: break;
                }
                listPointer++;
            }    
            closeFile( filePointer );
        }
    }

    displayVerboseString( "\nNow the output file is displayed\n" );
    queryStringParameter( OutputFile, &outputFileName );
    if( FileOpened == openFileForWriting( outputFileName, &filePointer ) )
    {
        listPointer = baseFileBlock;
        evaluationClass = getVEvaluationClass( listPointer );
        switch( evaluationClass )
        {
            case Output:
            case OutputButRemoved:
                displayVerbose( "base", listPointer );
                getVBeginOfBlock( listPointer, &variableBlock );
                writeVariableBlock( variableBlock,
                    getVLength( listPointer ), filePointer );
                break;

            case Ignored:
                if( TrailingCommentBlock == getVClassifier( listPointer ) )
                {
                   /* new functionality for fillup-1.10:           */
                   /* if basefile holds only a trailing comment    */
                   /* which includes a header                      */
                   /* this header is preserved within basefile.    */
                   writeBaseFileHeader( filePointer );
                }

            default: break;
        }

        listPointer++;
        for( index = 1; index < numberOfUsedBaseBlocks; index++ )
        {
            evaluationClass = getVEvaluationClass( listPointer );
            switch( evaluationClass )
            {
                case Output:
                case OutputButRemoved:
                    displayVerbose( "base", listPointer );
                    getVBeginOfBlock( listPointer, &variableBlock );
                    writeVariableBlock( variableBlock, 
                        getVLength( listPointer ), filePointer );
                    break;

                default: break;
            }
            listPointer++;
        }    
        listPointer = additionalFileBlock;
        for( index = 0; index < numberOfUsedAdditionalBlocks; index++ )
        {
            evaluationClass = getVEvaluationClass( listPointer );
            switch( evaluationClass )
            {
                case Output:
                    displayVerbose( "additional", listPointer );
                    getVBeginOfBlock( listPointer, &variableBlock );
                    writeVariableBlock( variableBlock, 
                        getVLength( listPointer ), filePointer );
                    break;

                default: break;
            }
            listPointer++;
        }    
        closeFile( filePointer );
    }
}

/*------------------ startParser -------------------*/

void
startParser
(
    void
)
{
    createAdministrationInfo( );
    parseFile( BaseFile, &numberOfUsedBaseBlocks );
    parseFile( AdditionalFile, &numberOfUsedAdditionalBlocks );
    if( queryParameter( ForbiddenFile ) == TRUE )
    {
        parseFile( ForbiddenFile, &numberOfUsedForbiddenBlocks );
    }
    else
    {
        headOfForbiddenFileList = NULL;
    }

    evaluateParsingResult( );
    writeOutput( );

    freeBuffer( &baseFileBuffer );
    freeBuffer( &additionalFileBuffer );
    if( queryParameter( ForbiddenFile ) == TRUE )
    {
        freeBuffer( &forbiddenFileBuffer );
    }
}

/*----------------------------------------------------------------------------*/

