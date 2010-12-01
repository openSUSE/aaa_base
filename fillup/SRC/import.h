
/**************************************************************************************/
/*                                                             			      */
/* IMPORT.H                                                    			      */
/* Datum: 27/07/93                                             			      */
/*                                                             			      */
/**************************************************************************************/

#ifndef __IMPORT__
#define __IMPORT__

#include <stdio.h>
#include <string.h>
#include "portab.h"

#if ANSI
#include <stdlib.h>
#else
#define abs(x)      ((x) <  0  ? -(x) : (x)) /* Absolut-Wert 			      */
#define labs(x)     abs (x)	             /* Langer Absolut-Wert 		      */
#define fabs(x)     abs (x)	             /* Double Absolut-Wert        	      */
#endif


/************************ DEFINES *****************************************************/   

#ifdef EXPORT			/* this is an import file, so disable export setting  */
#undef EXPORT
#endif

#ifndef IMPORT			/* tell that we're in importing condition  	      */
#define IMPORT
#endif

#ifdef GLOBAL			/* everything included now is in external modules     */
#undef GLOBAL
#endif

#define GLOBAL EXTERN

#ifndef max
#define max(a,b)    (((a) > (b)) ? (a) : (b)) /* Maximum-Funktion 		      */
#define min(a,b)    (((a) < (b)) ? (a) : (b)) /* Minimum Funktion 		      */
#endif

#define odd(i)      ((i) & 1)	/* ungerade 					      */

#endif /* __IMPORT__ */
