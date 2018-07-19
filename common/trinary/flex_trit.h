/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef COMMON_TRINARY_FLEX_TRIT_H_
#define COMMON_TRINARY_FLEX_TRIT_H_

#include <cstdint>
#include "common/trinary/trit_array.h"

namespace iota {
namespace trinary {

class FlexTrit {
  class FlexTritProxy {
   private:
    FlexTrit &flex_trit;
    int index;

   public:
    FlexTritProxy(FlexTrit &flex_trit, int index)
        : flex_trit(flex_trit), index(index) {}
    operator trit_t() const {
      return flex_trit_array_at(flex_trit.trit_array, flex_trit.len, index);
    }
    FlexTritProxy operator=(trit_t t) {
      flex_trit_array_set_at(flex_trit.trit_array, flex_trit.len, index, t);
      return *this;
    }
  };

 private:
  flex_trit_t *trit_array;
  size_t len;

 public:
  FlexTrit(flex_trit_t *trit_array, size_t len)
      : trit_array(trit_array), len(len) {}
  ~FlexTrit(){};

  FlexTritProxy operator[](const int i) { return FlexTritProxy(*this, i); }
  const FlexTritProxy operator[](const int i) const {
    return FlexTritProxy(const_cast<FlexTrit &>(*this), i);
  }
};
}  // namespace trinary
}  // namespace iota

#endif  // COMMON_TRINARY_FLEX_TRIT_H_