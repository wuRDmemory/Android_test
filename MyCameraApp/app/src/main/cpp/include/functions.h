//
// Created by ubuntu on 18-10-20.
//

#ifndef MYCAMERAAPP_FUNCTIONS_H
#define MYCAMERAAPP_FUNCTIONS_H

#include <jni.h>

extern jboolean initialization(JNIEnv* env, jobject* jobject);
extern jboolean fetch_image(JNIEnv* env, jobject obj, jbyteArray input, jint width, jint height);
extern jintArray get_result(JNIEnv* env, jobject jobject, jint width, jint height);
extern jintArray get_detection(JNIEnv* env, jobject jobject, jint width, jint height);
extern jboolean set_model(JNIEnv* env, jobject jobject, jstring model, jstring weight);
extern jint image_width(JNIEnv* env, jobject jobject);
extern jint image_height(JNIEnv* env, jobject jobject);

#endif //MYCAMERAAPP_FUNCTIONS_H
