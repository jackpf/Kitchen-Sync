#include <jni.h>
#include <stdio.h>
#include <soundtouch/SoundTouch.h>

JNIEXPORT jstring JNICALL Java_CInterface_getBpm
  (JNIEnv *env, jobject obj)
{
    return env->NewStringUTF("blaaah");
}