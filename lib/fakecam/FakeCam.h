/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>

#ifndef _Included_FakeCamera
#define _Included_FakeCamera
#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jint JNICALL Java_uk_ac_ic_matterialize_camera_V4L2Lib_open (JNIEnv *, jobject, jstring, jint, jint);

JNIEXPORT jboolean JNICALL Java_uk_ac_ic_matterialize_camera_V4L2Lib_writeFrame (JNIEnv *, jobject, jint, jbyteArray);

JNIEXPORT jint JNICALL Java_uk_ac_ic_matterialize_camera_V4L2Lib_close (JNIEnv *, jobject, jint);

#ifdef __cplusplus
}
#endif
#endif