#ifndef HLC_RANDOM_H
#define HLC_RANDOM_H

#include <stddef.h>

#include "api.h"
#include "layout.h"

HLC_DECLARATIONS_BEGIN

typedef struct hlc_Random hlc_Random;

/// @memberof hlc_Random
extern HLC_API const hlc_Layout hlc_random_layout;

/// @memberof hlc_Random
/// @brief Creates a new random number generator.
/// @pre random != NULL
HLC_API void hlc_random_create(hlc_Random* random);

/// @memberof hlc_Random
/// @brief Creates a new random number generator.
/// @pre random != NULL
HLC_API void hlc_random_create_with(hlc_Random* random, unsigned long seed);

/// @memberof hlc_Random
/// @brief Produces a random unsigned char in the range [min, max].
/// @pre random != NULL && min <= max
HLC_API unsigned char hlc_random_uchar_in(hlc_Random* random, unsigned char min, unsigned char max);

/// @memberof hlc_Random
/// @brief Produces a random unsigned short in the range [min, max].
/// @pre random != NULL && min <= max
HLC_API unsigned short hlc_random_ushort_in(hlc_Random* random, unsigned short min, unsigned short max);

/// @memberof hlc_Random
/// @brief Produces a random unsigned int in the range [min, max].
/// @pre random != NULL && min <= max
HLC_API unsigned hlc_random_uint_in(hlc_Random* random, unsigned min, unsigned max);

/// @memberof hlc_Random
/// @brief Produces a random unsigned long in the range [min, max].
/// @pre random != NULL && min <= max
HLC_API unsigned long hlc_random_ulong_in(hlc_Random* random, unsigned long min, unsigned long max);

/// @memberof hlc_Random
/// @brief Produces a random unsigned long long in the range [min, max].
/// @pre random != NULL && min <= max
HLC_API unsigned long long hlc_random_ullong_in(hlc_Random* random, unsigned long long min, unsigned long long max);

/// @memberof hlc_Random
/// @brief Produces a random size_t in the range [min, max].
/// @pre random != NULL && min <= max
HLC_API size_t hlc_random_size_in(hlc_Random* random, size_t min, size_t max);

HLC_DECLARATIONS_END

#endif
