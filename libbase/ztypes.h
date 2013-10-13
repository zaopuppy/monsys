#ifndef _Z_TYPES_H__
#define _Z_TYPES_H__

#ifndef WIN

#include <stdint.h>

#else

typedef signed char int8_t;
typedef unsigned int uint8_t;

typedef signed short int16_t;
typedef unsigned short uint16_t;

typedef signed int int32_t;
typedef unsigned int uint32_t;

typedef signed long long int32_t;
typedef unsigned long long uint32_t;

#endif

#endif // _Z_TYPES_H__

