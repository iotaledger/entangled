//
//  Interface.m
//  EntangledKit
//
//
//  Copyright © 2019 IOTA Foundation. All rights reserved.
//

#import "Interface.h"
#import <Foundation/Foundation.h>

#include "common/helpers/digest.h"
#include "common/helpers/pow.h"
#include "common/helpers/sign.h"

@implementation EntangledIOSBindings

+ (NSArray*)iota_ios_pow_bundle:(NSArray*)txsTrytes
                          trunk:(NSString*)trunk
                         branch:(NSString*)branch
                            mwm:(int)mwm {
  bundle_transactions_t* bundle = NULL;
  iota_transaction_t tx;
  iota_transaction_t* curTx = NULL;
  flex_trit_t serializedFlexTrits[FLEX_TRIT_SIZE_8019];
  char serializedTrytes[NUM_TRYTES_SERIALIZED_TRANSACTION + 1] = {0};
  flex_trit_t flexTrunk[FLEX_TRIT_SIZE_243];
  flex_trit_t flexBranch[FLEX_TRIT_SIZE_243];
  NSMutableArray* outputTxsTrytes = [NSMutableArray array];
  NSMutableString* outputTxsTrytesSerialized = @"";
  size_t i = 0;

  flex_trits_from_trytes(
      flexTrunk, NUM_TRITS_TRUNK,
      (tryte_t*)[trunk cStringUsingEncoding:NSUTF8StringEncoding],
      NUM_TRYTES_TRUNK, NUM_TRYTES_TRUNK);
  flex_trits_from_trytes(
      flexBranch, NUM_TRITS_BRANCH,
      (tryte_t*)[branch cStringUsingEncoding:NSUTF8StringEncoding],
      NUM_TRYTES_BRANCH, NUM_TRYTES_BRANCH);
  bundle_transactions_new(&bundle);

  for (NSString* txString in txsTrytes) {
    flex_trits_from_trytes(
        serializedFlexTrits, NUM_TRITS_SERIALIZED_TRANSACTION,
        (tryte_t*)[txString cStringUsingEncoding:NSUTF8StringEncoding],
        NUM_TRYTES_SERIALIZED_TRANSACTION, NUM_TRYTES_SERIALIZED_TRANSACTION);
    transaction_deserialize_from_trits(&tx, serializedFlexTrits, false);
    bundle_transactions_add(bundle, &tx);
  }

  if (iota_pow_bundle(bundle, flexTrunk, flexBranch, mwm) != RC_OK) {
    goto done;
  }

  BUNDLE_FOREACH(bundle, curTx) {
    transaction_serialize_on_flex_trits(curTx, serializedFlexTrits);
    flex_trits_to_trytes((tryte_t*)serializedTrytes,
                         NUM_TRYTES_SERIALIZED_TRANSACTION, serializedFlexTrits,
                         NUM_TRITS_SERIALIZED_TRANSACTION,
                         NUM_TRITS_SERIALIZED_TRANSACTION);
    outputTxsTrytesSerialized =
        [NSString stringWithFormat:@"%s", serializedTrytes];
    [outputTxsTrytes addObject:outputTxsTrytesSerialized];
  }

done:
  bundle_transactions_free(&bundle);
  return outputTxsTrytes;
}

@end
