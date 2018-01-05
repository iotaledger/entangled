#include "jni.h"
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "Interface.h"


#include "mobile/interface/bindings.h"


JNIEXPORT jstring JNICALL Java_org_iota_mobile_Interface_doPOW(JNIEnv* env,
                                                               jobject thiz,
                                                               jstring jtrytes,
                                                               jint mwm) {
  const char* trytes = env->GetStringUTFChars(jtrytes, 0);
  char* nonce = (char*) calloc(27 + 1, sizeof(char));

  char* foundNonce = do_pow((const char*) trytes, mwm);
  memcpy(nonce, foundNonce, 27);
  free(foundNonce);

  jstring out = env->NewStringUTF(nonce);
  free(nonce);

  return out;
}
