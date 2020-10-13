#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include "FakeCam.h"

// More Formats at: https://linuxtv.org/downloads/v4l-dvb-apis/uapi/v4l/videodev.html
JNIEXPORT jint JNICALL Java_uk_ac_ic_matterialize_camera_V4L2Lib_open(JNIEnv* env, jobject obj, jstring device, jint width, jint height)
{
    const char *deviceChar = (*env)->GetStringUTFChars(env, device, 0);

    int dev_fd = open(deviceChar, O_WRONLY);
    if (dev_fd == -1) {
      return dev_fd;
    }

   struct v4l2_format v;
    v.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
    if (ioctl(dev_fd, VIDIOC_G_FMT, &v) == -1) {
        return -2;
    }
    v.fmt.pix.width = width;
    v.fmt.pix.height = height;
    v.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    int frameSize = width * height * 2;
    v.fmt.pix.sizeimage = frameSize;

    if (ioctl(dev_fd, VIDIOC_S_FMT, &v) == -1) {
        return -3;
    }

    (*env)->ReleaseStringUTFChars(env, device, deviceChar);
    return dev_fd;
}

JNIEXPORT jboolean JNICALL Java_uk_ac_ic_matterialize_camera_V4L2Lib_writeFrame(JNIEnv* env, jobject obj, jint dev_fd, jbyteArray frame)
{
    int frameSize = (*env)->GetArrayLength(env, frame);

    jbyte* array = (*env)->GetByteArrayElements(env, frame, 0);
    char* frameBytes = (char*) array;

    int result = write(dev_fd, frameBytes, frameSize);

    (*env)->ReleaseByteArrayElements(env, frame, array, 0);
    return result == frameSize;
}

JNIEXPORT jint JNICALL Java_uk_ac_ic_matterialize_camera_V4L2Lib_close(JNIEnv* env, jobject obj, jint dev_fd)
{
    return close(dev_fd);
}