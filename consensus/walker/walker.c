#include "consensus/walker/walker.h"
#include "utils/logger_helper.h"

#define WALKER_LOGGER_ID "consensus_walker"

retcode_t iota_consensus_walker_init(const tangle_t *tangle, double alpha,
                                     walker_t *walker) {
  logger_helper_init(WALKER_LOGGER_ID, LOGGER_INFO, true);
  walker->alpha = alpha;
  walker->tangle = tangle;
}

retcode_t iota_consensus_walker_walk(const walker_t *walker,
                                     const trit_array_p ep, cw_map_t ratings,
                                     const walker_validator_t *wv,
                                     trit_array_p tip) {
  return RC_CONSENSUS_NOT_IMPLEMENTED;
}

retcode_t iota_consensus_walker_destroy(walker_t *walker) {
  return RC_CONSENSUS_NOT_IMPLEMENTED;
}