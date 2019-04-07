/*
 * Copyright (c) 2019 IOTA Stiftung
 * Copyright (c) 2019 Cybercrypt A/S
 * https://github.com/iotaledger/entangled
 * Based on c-mnd implementation: https://github.com/c-mnd/troika
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software. THE SOFTWARE IS PROVIDED
 * "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
 * LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef __COMMON_FTROIKA_GENERAL_H__
#define __COMMON_FTROIKA_GENERAL_H__

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#define STR_EXPAND(tok) #tok
#define STR(tok) STR_EXPAND(tok)

#define COLUMNS 9
#define ROWS 3
#define SLICES 27
#define SLICESIZE (COLUMNS * ROWS)
#define STATESIZE (COLUMNS * ROWS * SLICES)
#define NUM_SBOXES (SLICES * ROWS * COLUMNS / 3)

#define NUM_ROUNDS 24
#define TROIKA_RATE 243

#define PADDING 0x1

#define _1(a) (a->p)
#define _2(a) (a->n)
#define _0(a) (~a->p & ~a->n)

#endif  //__COMMON_FTROIKA_GENERAL_H__
