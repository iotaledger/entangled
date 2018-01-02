#include <jni.h>
#include <stdlib.h>
#include <stdio.h>
#include <syslog.h>

#include "Interface.h"


#include "mobile/interface/bindings.h"


JNIEXPORT jstring JNICALL Java_org_iota_mobile_Interface_doPOW(JNIEnv* env,
                                                               jobject thiz,
                                                               jstring jtrytes,
                                                               jint mwm) {
  const char* trytes = env->GetStringUTFChars(jtrytes, 0);

  char* nonce = do_pow((const char*) trytes, mwm);

  jstring out = env->NewString((jchar*) nonce, 27);
  free(nonce);


  return out;
}
