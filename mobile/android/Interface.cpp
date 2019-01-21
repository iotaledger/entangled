#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "jni.h"

#include "common/helpers/pow.h"
#include "common/helpers/sign.h"

#include "Interface.h"

extern "C" {
JNIEXPORT jstring JNICALL Java_org_iota_mobile_Interface_iota_pow(
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
    JNIEnv*, jclass, jobjectArray, jstring, jstring, jint) {
  return NULL;
}

JNIEXPORT jstring JNICALL Java_org_iota_mobile_Interface_iota_sign_address_gen(
    JNIEnv* env, jclass thiz, jstring jseed, jint index, jint security) {
  const char* seed = env->GetStringUTFChars(jseed, 0);

  char* address = iota_sign_address_gen(seed, index, security);
  std::memset((void*)seed, 0, 81);

  jstring out = env->NewStringUTF(address);
  free(address);

  return out;
}

/*
 * Class:     org_iota_mobile_Interface
 * Method:    generateSignature
 * Signature: (Ljava/lang/String;IILjava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_org_iota_mobile_Interface_iota_sign_signature_gen(JNIEnv* env, jclass thiz,
                                                       jstring jseed,
                                                       jint index,
                                                       jint security,
                                                       jstring jBundleHash) {
  const char* seed = env->GetStringUTFChars(jseed, 0);
  const char* bundleHash = env->GetStringUTFChars(jBundleHash, 0);

  char* signature = iota_sign_signature_gen(seed, index, security, bundleHash);
  std::memset((void*)seed, 0, 81);

  jstring out = env->NewStringUTF(signature);
  free(signature);

  return out;
}
}
