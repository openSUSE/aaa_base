
/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Copyrights to S.u.S.E. GmbH Fuerth (c) 1998                                */
/*                                                                            */
/* Time-stamp:                                                                */
/* Project:    fillup                                                         */
/* Module:     parser                                                         */
/* Filename:   parser.h                                                       */
/* Author:     Joerg Dippel (jd)                                              */
/* Description:                                                               */
/*                                                                            */
/*     export interface                                                       */
/*                                                                            */
/*----------------------------------------------------------------------------*/

#ifndef    __PARSER_H__
#define    __PARSER_H__

/*--------------------------------- IMPORTS ----------------------------------*/

#include "parameters.h"

/*------------------------------- FUNCTIONS ----------------------------------*/

void
associateBuffer
(   
    ParameterSpecification_t    parameterType,     /* in */
    long                        bufferLength,      /* in */
    char                     ** buffer             /* in */
);

void
startParser
(
    void
);

/*----------------------------------------------------------------------------*/

#endif  /* __PARSER_H__ */
