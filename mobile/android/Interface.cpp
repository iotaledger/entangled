#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "jni.h"

#include "common/helpers/digest.h"
#include "common/helpers/pow.h"
#include "common/helpers/sign.h"
#include "utils/bundle_miner.h"
#include "utils/memset_safe.h"

#include "Interface.h"

extern "C" {
JNIEXPORT jstring JNICALL Java_org_iota_mobile_Interface_iota_1pow_1trytes(JNIEnv* env, jclass, jstring jtrytes,
                                                                           jint mwm) {
  char* foundNonce = NULL;
  char const* trytes = env->GetStringUTFChars(jtrytes, 0);
  char* nonce = NULL;

  if ((nonce = (char*)calloc(27 + 1, sizeof(char))) == NULL) {
    return NULL;
  }

  if ((foundNonce = iota_pow_trytes((char const*)trytes, mwm)) == NULL) {
    env->ReleaseStringUTFChars(jtrytes, trytes);
    free(nonce);
    return NULL;
  }
  env->ReleaseStringUTFChars(jtrytes, trytes);
  memcpy(nonce, foundNonce, 27);
  free(foundNonce);

  jstring out = env->NewStringUTF(nonce);
  free(nonce);

  return out;
}

/*
 * Class:     org_iota_mobile_Interface
 * Method:    iota_pow_bundle
 * Signature:
 * ([Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;I)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_org_iota_mobile_Interface_iota_1pow_1bundle(JNIEnv* env, jclass,
                                                                                jobjectArray txsTrytes, jstring trunk,
                                                                                jstring branch, jint mwm) {
  bundle_transactions_t* bundle = NULL;
  iota_transaction_t tx;
  iota_transaction_t* curTx = NULL;
  flex_trit_t serializedFlexTrits[FLEX_TRIT_SIZE_8019];
  char serializedTrytes[NUM_TRYTES_SERIALIZED_TRANSACTION + 1] = {0};
  flex_trit_t flexTrunk[FLEX_TRIT_SIZE_243];
  flex_trit_t flexBranch[FLEX_TRIT_SIZE_243];
  jobjectArray outputTxsTrytes = NULL;
  jstring outputTxtrytes = NULL;
  size_t i = 0;

  char const* ctrunk = env->GetStringUTFChars(trunk, 0);
  char const* cbranch = env->GetStringUTFChars(branch, 0);
  flex_trits_from_trytes(flexTrunk, NUM_TRITS_TRUNK, (tryte_t*)ctrunk, NUM_TRYTES_TRUNK, NUM_TRYTES_TRUNK);
  flex_trits_from_trytes(flexBranch, NUM_TRITS_BRANCH, (tryte_t*)cbranch, NUM_TRYTES_BRANCH, NUM_TRYTES_BRANCH);
  env->ReleaseStringUTFChars(trunk, ctrunk);
  env->ReleaseStringUTFChars(branch, cbranch);

  bundle_transactions_new(&bundle);

  int txNum = env->GetArrayLength(txsTrytes);
  for (int i = 0; i < txNum; i++) {
    jstring txString = (jstring)(env->GetObjectArrayElement(txsTrytes, i));
    char const* txTrytes = env->GetStringUTFChars(txString, 0);
    flex_trits_from_trytes(serializedFlexTrits, NUM_TRITS_SERIALIZED_TRANSACTION, (tryte_t*)txTrytes,
                           NUM_TRYTES_SERIALIZED_TRANSACTION, NUM_TRYTES_SERIALIZED_TRANSACTION);
    env->ReleaseStringUTFChars(txString, txTrytes);
    transaction_deserialize_from_trits(&tx, serializedFlexTrits, false);
    bundle_transactions_add(bundle, &tx);
  }

  if (iota_pow_bundle(bundle, flexTrunk, flexBranch, mwm) != RC_OK) {
    goto done;
  }

  outputTxsTrytes = env->NewObjectArray(txNum, env->FindClass("java/lang/String"), 0);

  BUNDLE_FOREACH(bundle, curTx) {
    transaction_serialize_on_flex_trits(curTx, serializedFlexTrits);
    flex_trits_to_trytes((tryte_t*)serializedTrytes, NUM_TRYTES_SERIALIZED_TRANSACTION, serializedFlexTrits,
                         NUM_TRITS_SERIALIZED_TRANSACTION, NUM_TRITS_SERIALIZED_TRANSACTION);
    outputTxtrytes = env->NewStringUTF(serializedTrytes);
    env->SetObjectArrayElement(outputTxsTrytes, i, outputTxtrytes);
    i++;
  }

done:
  bundle_transactions_free(&bundle);
  return outputTxsTrytes;
}

/*
 * Class:     org_iota_mobile_Interface
 * Method:    iota_sign_address_gen_trytes
 * Signature: (Ljava/lang/String;II)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_iota_mobile_Interface_iota_1sign_1address_1gen_1trytes(JNIEnv* env, jclass,
                                                                                          jstring jseed, jint index,
                                                                                          jint security) {
  char* address = NULL;
  char const* seed = env->GetStringUTFChars(jseed, 0);

  if ((address = iota_sign_address_gen_trytes(seed, index, security)) == NULL) {
    env->ReleaseStringUTFChars(jseed, seed);
    memset_safe((void*)seed, 81, 0, 81);
    return NULL;
  }
  env->ReleaseStringUTFChars(jseed, seed);
  memset_safe((void*)seed, 81, 0, 81);

  jstring out = env->NewStringUTF(address);
  free(address);

  return out;
}

/*
 * Class:     org_iota_mobile_Interface
 * Method:    iota_sign_address_gen_trits
 * Signature: ([Ljava/lang/Byte;II)[Ljava/lang/Byte;
 */
JNIEXPORT jbyteArray JNICALL Java_org_iota_mobile_Interface_iota_1sign_1address_1gen_1trits(JNIEnv* env, jclass,
                                                                                            jbyteArray jseed,
                                                                                            jint index, jint security) {
  trit_t* address = NULL;
  trit_t const* seed = (trit_t*)env->GetByteArrayElements(jseed, 0);

  if ((address = iota_sign_address_gen_trits(seed, index, security)) == NULL) {
    env->ReleaseByteArrayElements(jseed, (jbyte*)seed, 0);
    memset_safe((void*)seed, 243, 0, 243);
    return NULL;
  }
  env->ReleaseByteArrayElements(jseed, (jbyte*)seed, 0);
  memset_safe((void*)seed, 243, 0, 243);

  jbyteArray out = env->NewByteArray(243);
  env->SetByteArrayRegion(out, 0, 243, (const jbyte*)address);
  free(address);

  return out;
}

/*
 * Class:     org_iota_mobile_Interface
 * Method:    iota_sign_signature_gen_trytes
 * Signature: (Ljava/lang/String;IILjava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_iota_mobile_Interface_iota_1sign_1signature_1gen_1trytes(JNIEnv* env, jclass,
                                                                                            jstring jseed, jint index,
                                                                                            jint security,
                                                                                            jstring jBundleHash) {
  char* signature = NULL;
  char const* seed = env->GetStringUTFChars(jseed, 0);
  char const* bundleHash = env->GetStringUTFChars(jBundleHash, 0);

  if ((signature = iota_sign_signature_gen_trytes(seed, index, security, bundleHash)) == NULL) {
    env->ReleaseStringUTFChars(jseed, seed);
    env->ReleaseStringUTFChars(jBundleHash, bundleHash);
    memset_safe((void*)seed, 81, 0, 81);
    return NULL;
  }
  env->ReleaseStringUTFChars(jseed, seed);
  env->ReleaseStringUTFChars(jBundleHash, bundleHash);
  memset_safe((void*)seed, 81, 0, 81);

  jstring out = env->NewStringUTF(signature);
  free(signature);

  return out;
}

/*
 * Class:     org_iota_mobile_Interface
 * Method:    iota_sign_signature_gen_trits
 * Signature: ([Ljava/lang/Byte;II[Ljava/lang/Byte;)[Ljava/lang/Byte;
 */
JNIEXPORT jbyteArray JNICALL Java_org_iota_mobile_Interface_iota_1sign_1signature_1gen_1trits(JNIEnv* env, jclass,
                                                                                              jbyteArray jseed,
                                                                                              jint index, jint security,
                                                                                              jbyteArray jBundleHash) {
  trit_t* signature = NULL;
  trit_t const* seed = (trit_t*)env->GetByteArrayElements(jseed, 0);
  trit_t const* bundleHash = (trit_t*)env->GetByteArrayElements(jBundleHash, 0);

  if ((signature = iota_sign_signature_gen_trits(seed, index, security, bundleHash)) == NULL) {
    env->ReleaseByteArrayElements(jseed, (jbyte*)seed, 0);
    env->ReleaseByteArrayElements(jBundleHash, (jbyte*)bundleHash, 0);
    memset_safe((void*)seed, 243, 0, 243);
    return NULL;
  }
  env->ReleaseByteArrayElements(jseed, (jbyte*)seed, 0);
  env->ReleaseByteArrayElements(jBundleHash, (jbyte*)bundleHash, 0);
  memset_safe((void*)seed, 243, 0, 243);

  int signatureLength = 6561 * security;
  jbyteArray out = env->NewByteArray(signatureLength);
  env->SetByteArrayRegion(out, 0, signatureLength, (const jbyte*)signature);
  free(signature);

  return out;
}

/*
 * Class:     org_iota_mobile_Interface
 * Method:    iota_digest
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_iota_mobile_Interface_iota_1digest(JNIEnv* env, jclass, jstring jtrytes) {
  char* digest = NULL;
  char const* trytes = env->GetStringUTFChars(jtrytes, 0);

  if ((digest = iota_digest(trytes)) == NULL) {
    env->ReleaseStringUTFChars(jtrytes, trytes);
    return NULL;
  }
  env->ReleaseStringUTFChars(jtrytes, trytes);
  jstring out = env->NewStringUTF(digest);
  free(digest);

  return out;
}

/*
 * Class:     org_iota_mobile_Interface
 * Method:    bundle_miner_mine
 * Signature: ([Ljava/lang/Byte;I[Ljava/lang/Byte;IIII)J
 */
JNIEXPORT jlong JNICALL Java_org_iota_mobile_Interface_bundle_1miner_1mine(JNIEnv* env, jclass,
                                                                           jbyteArray jbundleNormalizedMax,
                                                                           jint jsecurity, jbyteArray jessence,
                                                                           jint jessenceLength, jint jcount,
                                                                           jint jnprocs, jint jminingThreshold) {
  retcode_t ret = RC_OK;
  uint64_t index = 0;

  byte_t const* bundleNormalizedMax = (byte_t*)env->GetByteArrayElements(jbundleNormalizedMax, 0);
  trit_t* essence = (trit_t*)env->GetByteArrayElements(jessence, 0);

  ret = bundle_miner_mine(bundleNormalizedMax, jsecurity, essence, jessenceLength, jcount, jnprocs, jminingThreshold,
                          &index);

  ret = bundle_miner_mine(bundleNormalizedMax, jsecurity, essence, jessenceLength, jcount, jnprocs, &index, ctxs,
                          num_ctxs, &found_optimal_index);

  env->ReleaseByteArrayElements(jbundleNormalizedMax, (jbyte*)bundleNormalizedMax, 0);
  env->ReleaseByteArrayElements(jessence, (jbyte*)essence, 0);

  if (ret != RC_OK) {
    return -1;
  }

  return index;
}
}
