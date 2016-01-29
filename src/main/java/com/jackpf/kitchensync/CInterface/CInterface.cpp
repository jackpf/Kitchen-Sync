#include <jni.h>
#include <stdio.h>

#include "com_jackpf_kitchensync_CInterface_CInterface.h"
#include "KitchenSync.h"

JNIEXPORT jstring JNICALL Java_com_jackpf_kitchensync_CInterface_CInterface_getVersion
  (JNIEnv *env, jobject obj)
{
    return env->NewStringUTF(KitchenSync::getVersion());
}

JNIEXPORT jfloat JNICALL Java_com_jackpf_kitchensync_CInterface_CInterface_getBpm
  (JNIEnv *env, jobject obj, jstring jFilename)
{
    const char *filename = env->GetStringUTFChars(jFilename, 0);

    float bpm = KitchenSync::getBpm(filename);

    env->ReleaseStringUTFChars(jFilename, filename);

    return (jfloat) bpm;
}

JNIEXPORT void JNICALL Java_com_jackpf_kitchensync_CInterface_CInterface_setBpm
  (JNIEnv *env, jobject obj, jstring jInFilename, jstring jOutFilename, jfloat fromBpm, jfloat toBpm)
{
    const char *inFilename = env->GetStringUTFChars(jInFilename, 0);
    const char *outFilename = env->GetStringUTFChars(jOutFilename, 0);

    KitchenSync::setBpm(inFilename, outFilename, (float) fromBpm, (float) toBpm);

    env->ReleaseStringUTFChars(jInFilename, inFilename);
    env->ReleaseStringUTFChars(jOutFilename, outFilename);
}