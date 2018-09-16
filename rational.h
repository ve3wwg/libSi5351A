//////////////////////////////////////////////////////////////////////
// rational.h -- Rational best approximation function
// Date: Sun Sep 16 16:54:59 2018
///////////////////////////////////////////////////////////////////////

#include <stdint.h>

void rational_best_approximation(
  uint32_t given_numerator,
  uint32_t given_denominator,
  uint32_t max_numerator,
  uint32_t max_denominator,
  uint32_t *best_numerator,
  uint32_t *best_denominator
);

// End rational.h
