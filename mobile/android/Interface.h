/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class org_iota_mobile_Interface */

#ifndef _Included_org_iota_mobile_Interface
#define _Included_org_iota_mobile_Interface
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     org_iota_mobile_Interface
 * Method:    iota_pow
 * Signature: (Ljava/lang/String;I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_iota_mobile_Interface_iota_1pow
  (JNIEnv *, jclass, jstring, jint);

/*
 * Class:     org_iota_mobile_Interface
 * Method:    iota_sign_address_gen
 * Signature: (Ljava/lang/String;II)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_iota_mobile_Interface_iota_1sign_1address_1gen
  (JNIEnv *, jclass, jstring, jint, jint);

/*
 * Class:     org_iota_mobile_Interface
 * Method:    iota_sign_signature_gen
 * Signature: (Ljava/lang/String;IILjava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_iota_mobile_Interface_iota_1sign_1signature_1gen
  (JNIEnv *, jclass, jstring, jint, jint, jstring);

#ifdef __cplusplus
}
#endif
#endif
