//
// Created by ubuntu on 18-9-21.
//

#ifndef MYCAMERAAPP_MYLOG_H
#define MYCAMERAAPP_MYLOG_H

#include <jni.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <android/log.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/bitmap.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdint.h>

#define TAG "Native" // 这个是自定义的LOG的标识
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG ,__VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG ,__VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,TAG ,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG ,__VA_ARGS__)
#define LOGF(...) __android_log_print(ANDROID_LOG_FATAL,TAG ,__VA_ARGS__)

typedef unsigned char uint8;
typedef unsigned int  uint32;

using namespace std;
using namespace cv;

#endif //MYCAMERAAPP_MYLOG_H
