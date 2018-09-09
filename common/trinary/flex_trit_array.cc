/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/trinary/flex_trit_array.h"
#include <cassert>

namespace iota {
namespace trinary {

FlexTritArray FlexTritArray::createFromTrits(const std::vector<trit_t> &trits) {
  size_t num_trits = trits.size();
  FlexTritArray ft = FlexTritArray(num_trits);
  flex_trits_from_trits(ft.flex_trits_.data(), num_trits, trits.data(),
                        num_trits, num_trits);
  return ft;
}

FlexTritArray FlexTritArray::createFromTrytes(
    const std::vector<tryte_t> &trytes) {
  size_t num_trytes = trytes.size();
  size_t num_trits = num_trytes * 3;
  FlexTritArray ft = FlexTritArray(num_trits);
  flex_trits_from_trytes(ft.flex_trits_.data(), num_trits, trytes.data(),
                         num_trytes, num_trytes);
  return ft;
}

FlexTritArray::FlexTritArray(size_t num_trits) : num_trits_(num_trits) {
  size_t flex_len = FlexTritArray::numBytesForTrits(num_trits);
  flex_trits_.resize(flex_len, FLEX_TRIT_NULL_VALUE);
}

FlexTritArray FlexTritArray::slice(size_t start, size_t num_trits) {
  assert(start < num_trits_);
  assert(start + num_trits <= num_trits_);
  FlexTritArray ft = FlexTritArray(num_trits);
  flex_trits_slice(ft.flex_trits_.data(), num_trits, flex_trits_.data(),
                   num_trits_, start, num_trits);
  return ft;
}

size_t FlexTritArray::insert(const FlexTritArray &flex_trit_array,
                             size_t start) {
  assert(start < num_trits_);
  assert(start + flex_trit_array.num_trits_ <= num_trits_);
  flex_trits_insert(
      flex_trits_.data(), num_trits_, flex_trit_array.flex_trits_.data(),
      flex_trit_array.num_trits_, start, flex_trit_array.num_trits_);
  return flex_trit_array.num_trits_;
}

std::vector<trit_t> FlexTritArray::trits() {
  std::vector<trit_t> trits;
  trits.resize(num_trits_, 0);
  flex_trits_to_trits(trits.data(), num_trits_, flex_trits_.data(), num_trits_,
                      num_trits_);
  return trits;
}

std::vector<tryte_t> FlexTritArray::trytes() {
  size_t num_trytes = (num_trits_ + 2) / 3;
  std::vector<tryte_t> trytes;
  trytes.resize(num_trytes, '9');
  flex_trits_to_trytes(trytes.data(), num_trytes, flex_trits_.data(),
                       num_trits_, num_trits_);
  return trytes;
}
}  // namespace trinary
}  // namespace iota
