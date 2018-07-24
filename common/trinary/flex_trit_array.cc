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
FlexTritArray FlexTritArray::createFromTrits(const std::vector<trit_t> trits) {
  size_t flex_len = FlexTritArray::sizeForTrits(trits.size());
  std::vector<flex_trit_t> flex_trits;
  flex_trits.resize(flex_len, 0);
  int8_to_flex_trit_array(flex_trits.data(), flex_len, trits.data(),
                          trits.size(), trits.size());
  return FlexTritArray(flex_trits, trits.size());
}

FlexTritArray FlexTritArray::createFromTrytes(
    const std::vector<tryte_t> trytes) {
  size_t num_trits = trytes.size() * 3;
  size_t flex_len = FlexTritArray::sizeForTrits(num_trits);
  std::vector<flex_trit_t> flex_trits;
  flex_trits.resize(flex_len, 0);
  tryte_to_flex_trit(flex_trits.data(), num_trits, trytes.data(), trytes.size(),
                     trytes.size());
  return FlexTritArray(flex_trits, num_trits);
}

FlexTritArray FlexTritArray::slice(size_t start, size_t num_trits) {
  assert(start < num_trits_);
  assert(start + num_trits <= num_trits_);
  size_t flex_len = FlexTritArray::sizeForTrits(num_trits);
  std::vector<flex_trit_t> flex_trits;
  flex_trits.resize(flex_len, 0);
  flex_trit_array_slice(flex_trits.data(), flex_len, flex_trits_.data(),
                        num_trits_, start, num_trits);
  return FlexTritArray(std::move(flex_trits), num_trits);
}

size_t FlexTritArray::insert(FlexTritArray &flex_trit_array, size_t start) {
  assert(start < num_trits_);
  assert(start + flex_trit_array.num_trits_ <= num_trits_);
  flex_trit_array_insert(
      flex_trits_.data(), num_trits_, flex_trit_array.flex_trits_.data(),
      flex_trit_array.num_trits_, start, flex_trit_array.num_trits_);
  return flex_trit_array.num_trits_;
}

std::vector<trit_t> FlexTritArray::trits() {
  std::vector<trit_t> trits;
  trits.resize(num_trits_, 0);
  flex_trit_array_to_int8(trits.data(), num_trits_, flex_trits_.data(),
                          num_trits_, num_trits_);
  return trits;
}

std::vector<tryte_t> FlexTritArray::trytes() {
  size_t num_trytes = (num_trits_ + 2) / 3;
  std::vector<tryte_t> trytes;
  trytes.resize(num_trytes, '9');
  flex_trit_to_tryte(trytes.data(), num_trytes, flex_trits_.data(), num_trits_,
                     num_trits_);
  return trytes;
}
}  // namespace trinary
}  // namespace iota
