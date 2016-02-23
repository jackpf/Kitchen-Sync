#include <jni.h>
#include <stdio.h>
#include <iostream>

#include "com_jackpf_kitchensync_CInterface_CInterface.h"
#include "KitchenSync.h"

static void throwException(JNIEnv *env, const std::runtime_error &e) {
    const char pre[] = "Kitchen Sync error : ";
    char mBuf[strlen(pre) + strlen(e.what()) + 1];

    sprintf(mBuf, "%s%s", pre, e.what());

    fprintf(stderr, "%s\n", mBuf);

    env->ThrowNew(env->FindClass("java/lang/Exception"), mBuf);
}

JNIEXPORT jstring JNICALL Java_com_jackpf_kitchensync_CInterface_CInterface_getVersion
  (JNIEnv *env, jobject obj)
{
    return env->NewStringUTF(KitchenSync::getVersion());
}

JNIEXPORT jfloat JNICALL Java_com_jackpf_kitchensync_CInterface_CInterface_getBpm
  (JNIEnv *env, jobject obj, jstring jFilename)
{
    const char *filename;
    float bpm;

    filename = env->GetStringUTFChars(jFilename, 0);

    try {
        bpm = KitchenSync::getBpm(filename);
    } catch (const std::runtime_error &e) {
        throwException(env, e);
    }

    env->ReleaseStringUTFChars(jFilename, filename);

    return (jfloat) bpm;
}

JNIEXPORT void JNICALL Java_com_jackpf_kitchensync_CInterface_CInterface_setBpm
  (JNIEnv *env, jobject obj, jstring jInFilename, jstring jOutFilename, jfloat fromBpm, jfloat toBpm)
{
    const char *inFilename;
    const char *outFilename;

    inFilename = env->GetStringUTFChars(jInFilename, 0);
    outFilename = env->GetStringUTFChars(jOutFilename, 0);

    try {
        KitchenSync::setBpm(inFilename, outFilename, (float) fromBpm, (float) toBpm);
    } catch (const std::runtime_error &e) {
        throwException(env, e);
    }

    env->ReleaseStringUTFChars(jInFilename, inFilename);
    env->ReleaseStringUTFChars(jOutFilename, outFilename);
}

JNIEXPORT jfloat JNICALL Java_com_jackpf_kitchensync_CInterface_CInterface_getQuality
  (JNIEnv *env, jobject obj, jstring jFilename)
{
    const char *filename;
    float quality;

    filename = env->GetStringUTFChars(jFilename, 0);

    try {
        quality = KitchenSync::getQuality(filename);
    } catch (const std::runtime_error &e) {
        throwException(env, e);
    }

    env->ReleaseStringUTFChars(jFilename, filename);

    return (jfloat) quality;
}