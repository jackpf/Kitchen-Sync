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
    KitchenSync ks(nullptr);
    return env->NewStringUTF(ks.getVersion());
}

JNIEXPORT jboolean JNICALL Java_com_jackpf_kitchensync_CInterface_CInterface_hasDecoderFor
  (JNIEnv * env, jobject obj, jstring jFilename)
{
    const char *filename;
    bool r;

    filename = env->GetStringUTFChars(jFilename, 0);

    try {
        RunParameters params;
        params.inFileName = filename;
        KitchenSync ks(&params);
        r = true;
    } catch (const std::runtime_error &e) {
        r = false;
    }

    env->ReleaseStringUTFChars(jFilename, filename);

    return (jboolean) r;
}

JNIEXPORT jfloat JNICALL Java_com_jackpf_kitchensync_CInterface_CInterface_getBpm
  (JNIEnv *env, jobject obj, jstring jFilename)
{
    const char *filename;
    float bpm;

    filename = env->GetStringUTFChars(jFilename, 0);

    try {
        RunParameters params;
        params.inFileName = filename;
        KitchenSync ks(&params);
        bpm = ks.getBpm();
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
        RunParameters params;
        params.inFileName = inFilename;
        params.outFileName = outFilename;
        KitchenSync ks(&params);
        ks.setBpm((float) fromBpm, (float) toBpm);
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
        RunParameters params;
        params.inFileName = filename;
        KitchenSync ks(&params);
        quality = ks.getQuality();
    } catch (const std::runtime_error &e) {
        throwException(env, e);
    }

    env->ReleaseStringUTFChars(jFilename, filename);

    return (jfloat) quality;
}