/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_jackpf_kitchensync_CInterface_CInterface */

#ifndef _Included_com_jackpf_kitchensync_CInterface_CInterface
#define _Included_com_jackpf_kitchensync_CInterface_CInterface
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_jackpf_kitchensync_CInterface_CInterface
 * Method:    getVersion
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_jackpf_kitchensync_CInterface_CInterface_getVersion
  (JNIEnv *, jobject);

/*
 * Class:     com_jackpf_kitchensync_CInterface_CInterface
 * Method:    getBpm
 * Signature: (Ljava/lang/String;)F
 */
JNIEXPORT jfloat JNICALL Java_com_jackpf_kitchensync_CInterface_CInterface_getBpm
  (JNIEnv *, jobject, jstring);

/*
 * Class:     com_jackpf_kitchensync_CInterface_CInterface
 * Method:    setBpm
 * Signature: (Ljava/lang/String;FF)V
 */
JNIEXPORT void JNICALL Java_com_jackpf_kitchensync_CInterface_CInterface_setBpm
  (JNIEnv *, jobject, jstring, jfloat, jfloat);

#ifdef __cplusplus
}
#endif
#endif
