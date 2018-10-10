/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef COMMON_TRINARY_FLEX_TRIT_ARRAY_H_
#define COMMON_TRINARY_FLEX_TRIT_ARRAY_H_

#include <cstdint>
#include <vector>
#include "common/trinary/flex_trit.h"

namespace iota {
namespace trinary {

class FlexTritArray {
  class FlexTritArrayProxy {
   private:
    FlexTritArray &flex_trit_array;
    int index;

   public:
    FlexTritArrayProxy(FlexTritArray &flex_trit_array, int index)
        : flex_trit_array(flex_trit_array), index(index) {}
    operator trit_t() const {
      return flex_trits_at(flex_trit_array.flex_trits_.data(),
                           flex_trit_array.num_trits_, index);
    }
    FlexTritArrayProxy operator=(trit_t t) {
      flex_trits_set_at(flex_trit_array.flex_trits_.data(),
                        flex_trit_array.num_trits_, index, t);
      return *this;
    }
  };

 public:
  /// Returns the number of bytes needed to store a given number of trits in the
  /// current memory model.
  /// @param[in] num_trits - number of trits to store
  /// @return size_t - the number of bytes need
  static size_t numBytesForTrits(size_t num_trits) {
    return NUM_FLEX_TRITS_FOR_TRITS(num_trits);
  };

  /// Returns a new FlexTritArray from a vector of trits.
  /// @param[in] trits - a vector of trits
  /// @return FlexTritArray
  static FlexTritArray createFromTrits(const std::vector<trit_t> &trits);

  /// Returns a new FlexTritArray from a vector of trytes.
  /// @param[in] trytes - a vector of trytes
  /// @return FlexTritArray
  static FlexTritArray createFromTrytes(const std::vector<tryte_t> &trytes);

  /// Constructor that returns a new FlexTritArray capable of holding num_trits
  /// trits.
  /// @param[in] num_trits - the number of trits that can be stored
  /// @return FlexTritArray
  FlexTritArray(size_t num_trits);

  /// Constructor that returns a new FlexTritArray from a vector of flex_trits.
  /// @param[in] flex_trits - a vector of flex_trits
  /// @param[in] num_trits - the number of trits encoded in the vector of
  /// flex_trits
  /// @return FlexTritArray
  FlexTritArray(const std::vector<flex_trit_t> &flex_trits, size_t num_trits)
      : flex_trits_(flex_trits), num_trits_(num_trits){};

  /// Copy Constructor
  /// @param[in] other - an other FlexTritArray
  /// @return FlexTritArray
  FlexTritArray(const FlexTritArray &other)
      : flex_trits_(other.flex_trits_), num_trits_(other.num_trits_){};

  /// Destructor
  ~FlexTritArray(){};

  FlexTritArrayProxy operator[](int i) { return FlexTritArrayProxy(*this, i); }
  const FlexTritArrayProxy operator[](int i) const {
    return FlexTritArrayProxy(const_cast<FlexTritArray &>(*this), i);
  }

  /// Returns the number of trits in the receiver
  /// @return size_t - the number of trits
  size_t size() { return num_trits_; }

  /// Returns a new FlexTritArray that contains a range of the receiver
  /// starting at index start and of length num_trits.
  /// @param[in] start - the start index in the receiver
  /// @param[in] num_trits - the number of trits to copy over
  /// @return FlexTritArray - a new FlexTritArray
  FlexTritArray slice(size_t start, size_t num_trits);

  /// Inserts into the receiver the contents of the flex_trit_array
  /// starting at index start.
  /// @param[in] start - the start index in the receiver
  /// @return size_t - the number of inserted trits.
  size_t insert(const FlexTritArray &flex_trit_array, size_t start);

  /// Returns an array of trits regardless of the current memory storage scheme
  std::vector<trit_t> trits();

  /// Returns an array of trytes regardless of the current memory storage scheme
  std::vector<tryte_t> trytes();

 private:
  std::vector<flex_trit_t> flex_trits_;
  size_t num_trits_;
};
}  // namespace trinary
}  // namespace iota

#endif  // COMMON_TRINARY_FLEX_TRIT_ARRAY_H_
