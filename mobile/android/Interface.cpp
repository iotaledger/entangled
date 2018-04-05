#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "jni.h"

#include "mobile/interface/bindings.h"

#include "Interface.h"

extern "C" {
JNIEXPORT jstring JNICALL Java_org_iota_mobile_Interface_doPOW(JNIEnv* env,
                                                               jclass thiz,
                                                               jstring jtrytes,
                                                               jint mwm) {
  const char* trytes = env->GetStringUTFChars(jtrytes, 0);
  char* nonce = (char*)calloc(27 + 1, sizeof(char));

  char* foundNonce = do_pow((const char*)trytes, mwm);
  memcpy(nonce, foundNonce, 27);
  free(foundNonce);

  jstring out = env->NewStringUTF(nonce);
  free(nonce);

  return out;
}

JNIEXPORT jstring JNICALL Java_org_iota_mobile_Interface_generateAddress(
    JNIEnv* env, jclass thiz, jstring jseed, jint index, jint security) {
  const char* seed = env->GetStringUTFChars(jseed, 0);

  char* address = generate_address(seed, index, security);
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
JNIEXPORT jstring JNICALL Java_org_iota_mobile_Interface_generateSignature(
    JNIEnv* env, jclass thiz, jstring jseed, jint index, jint security,
    jstring jBundleHash) {
  const char* seed = env->GetStringUTFChars(jseed, 0);
  const char* bundleHash = env->GetStringUTFChars(jBundleHash, 0);

  char* signature = generate_signature(seed, index, security, bundleHash);
  std::memset((void*)seed, 0, 81);

  jstring out = env->NewStringUTF(signature);
  free(signature);

  return out;
}
}
