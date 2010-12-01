/*------------------ EXPORT.H -----------------------------*/
/*                                                         */
/*---------------------------------------------------------*/

#ifndef __EXPORT__
#define __EXPORT__

#ifdef IMPORT
#undef IMPORT
#endif

#ifndef EXPORT
#define EXPORT
#endif

#ifdef GLOBAL
#undef GLOBAL
#endif

#define GLOBAL

#endif /* __EXPORT __ */

