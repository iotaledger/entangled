#include <jni.h>
#include <stdlib.h>
#include <stdio.h>

#include "Interface.h"


#include "mobile/interface/bindings.h"


JNIEXPORT jstring JNICALL Java_org_iota_mobile_Interface_doPOW(JNIEnv* env,
                                                               jobject thiz,
                                                               jstring jtrytes,
                                                               jint mwm) {
  const char* trytes = env->GetStringUTFChars(jtrytes, 0);

  char* nonce = do_pow((const char*) trytes, mwm);
  jstring out = env->NewStringUTF(nonce);
  free(nonce);

  return out;
}
