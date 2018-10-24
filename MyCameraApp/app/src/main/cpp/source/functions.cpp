//
// Created by ubuntu on 18-10-20.
//

#include <jni.h>
#include <cvtest.h>
#include <mylog.h>
#include <functions.h>

ImageUtil* g_imageUtil;

jboolean initialization(JNIEnv* env, jobject* jobject) {
    if (g_imageUtil) {
        delete(g_imageUtil);
    }
    g_imageUtil = new ImageUtil();
    return JNI_TRUE;
}

jboolean fetch_image(JNIEnv* env, jobject obj, jbyteArray input, jint width, jint height) {
    if (!g_imageUtil) {
        LOGE(">>>JNI [fetch image] use initialization first!!!");
        return JNI_FALSE;
    }

    jboolean inputCopy;
    jbyte* data = env->GetByteArrayElements(input, &inputCopy);
    if (data == NULL) {
        LOGE(">>> [fetch image] some error occur!!!");
        return JNI_FALSE;
    }
    LOGD(">>> [fetch image] begin fetch!!!");
    Mat YUVImage(height*3/2, width, CV_8UC1, data);
    Mat Image(height, width, CV_8UC3);
    cvtColor(YUVImage, Image, cv::COLOR_YUV420p2RGB);
    transpose(Image, Image);
    flip(Image, Image, 1);
    g_imageUtil->fetch_image(Image);
    // release old data
    env->ReleaseByteArrayElements(input, data, 0);
    return JNI_TRUE;
}

jintArray get_result(JNIEnv* env, jobject jobject, jint width, jint height) {
    if (!g_imageUtil) {
        LOGE(">>>JNI [fetch image] use initialization first!!!");
        return env->NewIntArray(0);
    }
    // do something
    Mat result = g_imageUtil->get_result(width, height);
    LOGD(">>> [get_result] w: %d, h:%d", width, height);
    // mat to jint[]
    jintArray ret = env->NewIntArray(width*height);
    env->SetIntArrayRegion(ret, 0, width*height, (const jint*)result.data);
    return ret;
}

jintArray get_detection(JNIEnv* env, jobject jobject, jint width, jint height) {
    if (!g_imageUtil) {
        LOGE(">>>JNI [fetch image] use initialization first!!!");
        return env->NewIntArray(0);
    }
    // do something
    Mat result = g_imageUtil->get_detection_result(width, height);
    LOGD(">>> [get_result] w: %d, h:%d", width, height);
    // mat to jint[]
    jintArray ret = env->NewIntArray(width*height);
    env->SetIntArrayRegion(ret, 0, width*height, (const jint*)result.data);
    return ret;
}

jint image_width(JNIEnv* env, jobject jobject) {
    if (g_imageUtil)
        return g_imageUtil->get_width();
    return 0;
}

jint image_height(JNIEnv* env, jobject jobject) {
    if (g_imageUtil)
        return g_imageUtil->get_height();
    return 0;
}

jboolean set_model(JNIEnv* env, jobject jobject, jstring model, jstring weight) {
    const char* model_chars = env->GetStringUTFChars(model, JNI_FALSE);
    const char* weight_chars = env->GetStringUTFChars(weight, JNI_FALSE);
    LOGD("%%% get string!!!");
    cv::String model_path(model_chars);
    cv::String weight_path(weight_chars);
    jboolean ret = JNI_FALSE;
    if (g_imageUtil) {
        g_imageUtil->set_model_path(model_path, weight_path);
        ret = JNI_TRUE;
    }
    env->ReleaseStringUTFChars(model, model_chars);
    env->ReleaseStringUTFChars(weight, weight_chars);
    return ret;
}

//jboolean read_model(JNIEnv* env, jobject jobject, jobject asset, jstring model, jstring weight) {
//    AAssetManager* mgr = AAssetManager_fromJava(env, asset);
//    if(mgr==NULL)
//    {
//        LOGI(" %s","AAssetManager==NULL");
//        return JNI_FALSE;
//    }
//    const char* model_chars = env->GetStringUTFChars(model, JNI_FALSE);
//    const char* weight_chars = env->GetStringUTFChars(weight, JNI_FALSE);
//
//    AAsset* asset_model  = AAssetManager_open(mgr, model_chars, AASSET_MODE_UNKNOWN);
//    AAsset* asset_weight = AAssetManager_open(mgr, weight_chars, AASSET_MODE_UNKNOWN);
//
//    env->ReleaseStringUTFChars(model, model_chars);
//    env->ReleaseStringUTFChars(weight, weight_chars);
//
//    if(asset==NULL)
//    {
//        LOGI(" %s","asset==NULL");
//        return JNI_FALSE;
//    }
//    /*获取文件大小*/
//    off_t bufferSize = AAsset_getLength(asset_model);
//    off_t bufferSize = AAsset_getLength(asset_weight);
//    LOGI("file size : %d\n",bufferSize);
//    char *buffer = (char *)malloc(bufferSize+1);
//    buffer[bufferSize]=0;
//    int numBytesRead = AAsset_read(asset, buffer, bufferSize);
//    LOGI(": %s",buffer);
//    free(buffer);
//    /*关闭文件*/
//    AAsset_close(asset);
//
//}
