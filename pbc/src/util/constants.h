#ifndef DYNALLOC_CONSTANTS_H_
#define DYNALLOC_CONSTANTS_H_

#if HAVE_STDBOOL_H
#  include <stdbool.h>
#else
typedef enum {false, true} bool;
#endif /* !HAVE_STDBOOL_H */

#define RT_SUCCESS 	0
#define RT_FAILURE 	-1

#define SIZE 8192

#define RT_Succeed 					0
#define RT_Failed  					1

#define HandShake_MSG "handshake-001"

#endif /* DYNALLOC_CONSTANTS_H_ */
