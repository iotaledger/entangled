/*!
\file buffers.h
\brief Trits collection to simplify buffered operations.
*/
#pragma once

#include "mam/v2/defs.h"
#include "mam/v2/trits.h"

typedef struct {
  trits_t X;   /*!< the head */
  size_t n;    /*!< buffers count in the tail */
  trits_t *Xs; /*!< the tail buffers */
} buffers_t;

/*! \brief Init buffers with empty head. */
buffers_t buffers_init(size_t n, trits_t *Xs);

/*! \brief Check whether head is empty and there is no tail.
\note However if the tail buffers consist of empty buffers only
it will _not_ be considered empty!
*/
bool_t buffers_is_empty(buffers_t tb);

/*! \brief Sum of head and tail buffers sizes. */
size_t buffers_size(buffers_t tb);

/*! \brief Copy buffers `*tb` to a destination `buf`; return the number of trits
 * copied. */
size_t buffers_copy_to(buffers_t *tb, trits_t buf);

/*! \brief Copy buffers `*tb` from a source `buf`; return the number of trits
 * copied. */
size_t buffers_copy_from(buffers_t *tb, trits_t buf);
