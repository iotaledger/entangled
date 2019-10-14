//
//  Interface.mm
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
#include "utils/bundle_miner.h"

@implementation EntangledIOSBindings

+ (NSString*)iota_ios_digest:(NSString*)trytes {
  const char* ctrytes = [trytes cStringUsingEncoding:NSUTF8StringEncoding];
  char* digest = NULL;
  if ((digest = iota_digest(ctrytes)) == NULL) {
    return NULL;
  }
  NSString* outputDigest = [NSString stringWithFormat:@"%s", digest];
  free(digest);
  return outputDigest;
}

+ (NSString*)iota_ios_pow_trytes:(NSString*)trytes mwm:(NSNumber*)mwm {
  const char* ctrytes = [trytes cStringUsingEncoding:NSUTF8StringEncoding];
  uint8_t cmwm = (uint8_t)[mwm unsignedCharValue];
  char* foundNonce = NULL;
  if ((foundNonce = iota_pow_trytes(ctrytes, cmwm)) == NULL) {
    return NULL;
  }
  NSString* outputNonce = [NSString stringWithFormat:@"%s", foundNonce];
  free(foundNonce);
  return outputNonce;
}

+ (int8_t*)iota_ios_sign_address_gen_trits:(int8_t*)seed index:(const int)index security:(const int)security {
  trit_t* address = NULL;
  address = iota_sign_address_gen_trits((trit_t*)seed, index, security);
  return (int8_t*)address;
}

+ (int8_t*)iota_ios_sign_signature_gen_trits:(int8_t*)seed
                                       index:(const int)index
                                    security:(const int)security
                                  bundleHash:(int8_t*)bundleHash {
  trit_t* signature = NULL;
  signature = iota_sign_signature_gen_trits((trit_t*)seed, index, security, (trit_t*)bundleHash);
  return (int8_t*)signature;
}

+ (NSArray*)iota_ios_pow_bundle:(NSArray*)txsTrytes trunk:(NSString*)trunk branch:(NSString*)branch mwm:(NSNumber*)mwm {
  bundle_transactions_t* bundle = NULL;
  iota_transaction_t tx;
  iota_transaction_t* curTx = NULL;
  flex_trit_t serializedFlexTrits[FLEX_TRIT_SIZE_8019];
  char serializedTrytes[NUM_TRYTES_SERIALIZED_TRANSACTION + 1] = {0};
  flex_trit_t flexTrunk[FLEX_TRIT_SIZE_243];
  flex_trit_t flexBranch[FLEX_TRIT_SIZE_243];
  NSMutableArray* outputTxsTrytes = [NSMutableArray array];
  NSMutableString* outputTxsTrytesSerialized = [NSMutableString string];
  uint8_t cmwm = (uint8_t)[mwm unsignedCharValue];

  flex_trits_from_trytes(flexTrunk, NUM_TRITS_TRUNK, (tryte_t*)[trunk cStringUsingEncoding:NSUTF8StringEncoding],
                         NUM_TRYTES_TRUNK, NUM_TRYTES_TRUNK);
  flex_trits_from_trytes(flexBranch, NUM_TRITS_BRANCH, (tryte_t*)[branch cStringUsingEncoding:NSUTF8StringEncoding],
                         NUM_TRYTES_BRANCH, NUM_TRYTES_BRANCH);
  bundle_transactions_new(&bundle);

  for (NSString* txString in txsTrytes) {
    flex_trits_from_trytes(serializedFlexTrits, NUM_TRITS_SERIALIZED_TRANSACTION,
                           (tryte_t*)[txString cStringUsingEncoding:NSUTF8StringEncoding],
                           NUM_TRYTES_SERIALIZED_TRANSACTION, NUM_TRYTES_SERIALIZED_TRANSACTION);
    transaction_deserialize_from_trits(&tx, serializedFlexTrits, false);
    bundle_transactions_add(bundle, &tx);
  }

  if (iota_pow_bundle(bundle, flexTrunk, flexBranch, cmwm) != RC_OK) {
    goto done;
  }

  BUNDLE_FOREACH(bundle, curTx) {
    transaction_serialize_on_flex_trits(curTx, serializedFlexTrits);
    flex_trits_to_trytes((tryte_t*)serializedTrytes, NUM_TRYTES_SERIALIZED_TRANSACTION, serializedFlexTrits,
                         NUM_TRITS_SERIALIZED_TRANSACTION, NUM_TRITS_SERIALIZED_TRANSACTION);
    outputTxsTrytesSerialized = [NSMutableString stringWithFormat:@"%s", serializedTrytes];
    [outputTxsTrytes addObject:outputTxsTrytesSerialized];
  }

done:
  bundle_transactions_free(&bundle);
  return outputTxsTrytes;
}

+ (NSNumber*)bundle_miner_mine:(int8_t*)bundleNormalizedMax
                      security:(NSNumber*)security
                       essence:(int8_t*)essence
                 essenceLength:(NSNumber*)essenceLength
                         count:(NSNumber*)count
                        nprocs:(NSNumber*)nprocs {
  uint64_t cindex = 0;

  byte_t const* cbundleNormalizedMax = (byte_t*)bundleNormalizedMax;

  cindex = bundle_miner_mine(cbundleNormalizedMax, (uint8_t)[security unsignedCharValue], (trit_t*)essence,
                                 (size_t)[essenceLength unsignedLongValue], (uint32_t)[count unsignedIntValue],
                                 (uint8_t)[nprocs unsignedCharValue]);

  NSNumber* index = [NSNumber numberWithUnsignedLongLong:cindex];
  return index;
}

@end
