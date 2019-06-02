/* DO NOT EDIT THIS FILE - it is machine generated */
#include "jni.h"
/* Header for class org_iota_mobile_Interface */

#ifndef _Included_org_iota_mobile_Interface
#define _Included_org_iota_mobile_Interface
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     org_iota_mobile_Interface
 * Method:    iota_pow_trytes
 * Signature: (Ljava/lang/String;I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_iota_mobile_Interface_iota_1pow_1trytes(JNIEnv *, jclass, jstring, jint);

/*
 * Class:     org_iota_mobile_Interface
 * Method:    iota_pow_bundle
 * Signature:
 * ([Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;I)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_org_iota_mobile_Interface_iota_1pow_1bundle(JNIEnv *, jclass, jobjectArray, jstring,
                                                                                jstring, jint);

/*
 * Class:     org_iota_mobile_Interface
 * Method:    iota_sign_address_gen_trytes
 * Signature: (Ljava/lang/String;II)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_iota_mobile_Interface_iota_1sign_1address_1gen_1trytes(JNIEnv *, jclass, jstring,
                                                                                          jint, jint);

/*
 * Class:     org_iota_mobile_Interface
 * Method:    iota_sign_address_gen_trits
 * Signature: ([Ljava/lang/Byte;II)[Ljava/lang/Byte;
 */
JNIEXPORT jbyteArray JNICALL Java_org_iota_mobile_Interface_iota_1sign_1address_1gen_1trits(JNIEnv *, jclass,
                                                                                            jbyteArray, jint, jint);

/*
 * Class:     org_iota_mobile_Interface
 * Method:    iota_sign_signature_gen_trytes
 * Signature: (Ljava/lang/String;IILjava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_iota_mobile_Interface_iota_1sign_1signature_1gen_1trytes(JNIEnv *, jclass, jstring,
                                                                                            jint, jint, jstring);

/*
 * Class:     org_iota_mobile_Interface
 * Method:    iota_sign_signature_gen_trits
 * Signature: ([Ljava/lang/Byte;II[Ljava/lang/Byte;)[Ljava/lang/Byte;
 */
JNIEXPORT jbyteArray JNICALL Java_org_iota_mobile_Interface_iota_1sign_1signature_1gen_1trits(JNIEnv *, jclass,
                                                                                              jbyteArray, jint, jint,
                                                                                              jbyteArray);

/*
 * Class:     org_iota_mobile_Interface
 * Method:    iota_digest
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_iota_mobile_Interface_iota_1digest(JNIEnv *, jclass, jstring);

#ifdef __cplusplus
}
#endif
#endif
