#include <jni.h>
#include <stdio.h>

#include <soundtouch/SoundTouch.h>

#include "CInterface.h"

JNIEXPORT jstring JNICALL Java_CInterface_getBpm
  (JNIEnv *env, jobject obj)
{
    soundtouch::SoundTouch soundtouch;
    return env->NewStringUTF(soundtouch.getVersionString());
}