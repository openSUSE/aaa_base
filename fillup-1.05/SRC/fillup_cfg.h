
/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Copyrights to S.u.S.E. GmbH Fuerth (c) 1998                                */
/*                                                                            */
/* Time-stamp:                                                                */
/* Project:    fillup                                                         */
/* Module:     configuration                                                  */
/* Filename:   fillup_cfg.c                                                   */
/* Author:     Joerg Dippel (jd)                                              */
/* Description:                                                               */
/*                                                                            */
/*     export interface                                                       */
/*                                                                            */
/*----------------------------------------------------------------------------*/

/*--------------------------------- DEFINES ----------------------------------*/

#define cfg_baseFileBlocks            500
#define cfg_additionalFileBlocks      500
#define cfg_forbiddenFileBlocks       100

#define cfg_MaxVariableLength         256

/*-------------------------------- VARIABLES ---------------------------------*/

const char    * cfg_delimiter;
const char    * cfg_commentMarker;
const char    * cfg_quotingMarker;

/*----------------------------------------------------------------------------*/

