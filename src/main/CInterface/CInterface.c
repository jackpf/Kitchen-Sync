#include <jni.h>
#include <stdio.h>

JNIEXPORT jstring JNICALL Java_CInterface_getBpm
  (JNIEnv *env, jobject obj)
{
    return (*env)->NewStringUTF(env, "blaaah");
}