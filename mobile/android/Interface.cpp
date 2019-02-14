#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "jni.h"

#include "common/helpers/digest.h"
#include "common/helpers/pow.h"
#include "common/helpers/sign.h"

#include "Interface.h"

extern "C" {
JNIEXPORT jstring JNICALL Java_org_iota_mobile_Interface_iota_1pow_1trytes(
    JNIEnv* env, jclass thiz, jstring jtrytes, jint mwm) {
  const char* trytes = env->GetStringUTFChars(jtrytes, 0);
  char* nonce = (char*)calloc(27 + 1, sizeof(char));

  char* foundNonce = iota_pow_trytes((const char*)trytes, mwm);
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
JNIEXPORT jobjectArray JNICALL Java_org_iota_mobile_Interface_iota_1pow_1bundle(
    JNIEnv* env, jclass thiz, jobjectArray txsTrytes, jstring trunk,
    jstring branch, jint mwm) {
  bundle_transactions_t* bundle = NULL;
  iota_transaction_t tx;
  iota_transaction_t* curTx = NULL;
  flex_trit_t serializedFlexTrits[FLEX_TRIT_SIZE_8019];
  char serializedTrytes[NUM_TRYTES_SERIALIZED_TRANSACTION + 1] = {0};
  flex_trit_t flexTrunk[FLEX_TRIT_SIZE_243];
  flex_trit_t flexBranch[FLEX_TRIT_SIZE_243];
  jobjectArray outputTxsTrytes = NULL;
  jstring outputTxtrytes;
  size_t i = 0;

  flex_trits_from_trytes(flexTrunk, NUM_TRITS_TRUNK,
                         (tryte_t*)env->GetStringUTFChars(trunk, 0),
                         NUM_TRYTES_TRUNK, NUM_TRYTES_TRUNK);
  flex_trits_from_trytes(flexBranch, NUM_TRITS_BRANCH,
                         (tryte_t*)env->GetStringUTFChars(branch, 0),
                         NUM_TRYTES_BRANCH, NUM_TRYTES_BRANCH);

  bundle_transactions_new(&bundle);

  int txNum = env->GetArrayLength(txsTrytes);
  for (int i = 0; i < txNum; i++) {
    jstring txString = (jstring)(env->GetObjectArrayElement(txsTrytes, i));
    const char* txTrytes = env->GetStringUTFChars(txString, 0);
    flex_trits_from_trytes(serializedFlexTrits,
                           NUM_TRITS_SERIALIZED_TRANSACTION, (tryte_t*)txTrytes,
                           NUM_TRYTES_SERIALIZED_TRANSACTION,
                           NUM_TRYTES_SERIALIZED_TRANSACTION);
    transaction_deserialize_from_trits(&tx, serializedFlexTrits, false);
    bundle_transactions_add(bundle, &tx);
  }

  if (iota_pow_bundle(bundle, flexTrunk, flexBranch, mwm) != RC_OK) {
    goto done;
  }

  outputTxsTrytes =
      env->NewObjectArray(txNum, env->FindClass("java/lang/String"), 0);

  BUNDLE_FOREACH(bundle, curTx) {
    transaction_serialize_on_flex_trits(curTx, serializedFlexTrits);
    flex_trits_to_trytes((tryte_t*)serializedTrytes,
                         NUM_TRYTES_SERIALIZED_TRANSACTION, serializedFlexTrits,
                         NUM_TRITS_SERIALIZED_TRANSACTION,
                         NUM_TRITS_SERIALIZED_TRANSACTION);
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
JNIEXPORT jstring JNICALL
Java_org_iota_mobile_Interface_iota_1sign_1address_1gen_1trytes(
    JNIEnv* env, jclass thiz, jstring jseed, jint index, jint security) {
  const char* seed = env->GetStringUTFChars(jseed, 0);

  char* address = iota_sign_address_gen_trytes(seed, index, security);
  std::memset((void*)seed, 0, 81);

  jstring out = env->NewStringUTF(address);
  free(address);

  return out;
}

/*
 * Class:     org_iota_mobile_Interface
 * Method:    iota_sign_address_gen_trits
 * Signature: ([Ljava/lang/Byte;II)[Ljava/lang/Byte;
 */
JNIEXPORT jbyteArray JNICALL
Java_org_iota_mobile_Interface_iota_1sign_1address_1gen_1trits(
    JNIEnv* env, jclass thiz, jbyteArray jseed, jint index, jint security) {
  const trit_t* seed = (trit_t*)env->GetByteArrayElements(jseed, 0);

  trit_t* address = iota_sign_address_gen_trits(seed, index, security);
  std::memset((void*)seed, 0, 243);

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
JNIEXPORT jstring JNICALL
Java_org_iota_mobile_Interface_iota_1sign_1signature_1gen_1trytes(
    JNIEnv* env, jclass thiz, jstring jseed, jint index, jint security,
    jstring jBundleHash) {
  const char* seed = env->GetStringUTFChars(jseed, 0);
  const char* bundleHash = env->GetStringUTFChars(jBundleHash, 0);

  char* signature =
      iota_sign_signature_gen_trytes(seed, index, security, bundleHash);
  std::memset((void*)seed, 0, 81);

  jstring out = env->NewStringUTF(signature);
  free(signature);

  return out;
}

/*
 * Class:     org_iota_mobile_Interface
 * Method:    iota_sign_signature_gen_trits
 * Signature: ([Ljava/lang/Byte;II[Ljava/lang/Byte;)[Ljava/lang/Byte;
 */
JNIEXPORT jbyteArray JNICALL
Java_org_iota_mobile_Interface_iota_1sign_1signature_1gen_1trits(
    JNIEnv* env, jclass thiz, jbyteArray jseed, jint index, jint security,
    jbyteArray jBundleHash) {
  const trit_t* seed = (trit_t*)env->GetByteArrayElements(jseed, 0);
  const trit_t* bundleHash = (trit_t*)env->GetByteArrayElements(jBundleHash, 0);

  trit_t* signature =
      iota_sign_signature_gen_trits(seed, index, security, bundleHash);
  std::memset((void*)seed, 0, 243);

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
JNIEXPORT jstring JNICALL Java_org_iota_mobile_Interface_iota_1digest(
    JNIEnv* env, jclass thiz, jstring jtrytes) {
  const char* trytes = env->GetStringUTFChars(jtrytes, 0);

  char* digest = iota_digest(trytes);
  jstring out = env->NewStringUTF(digest);
  free(digest);

  return out;
}
}
