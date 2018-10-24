#include <mylog.h>
#include <cvtest.h>
#include <functions.h>

using namespace std;

// very important is ; object class need this
JNINativeMethod nativeMethod[] = {
    {"initialization", "()Z", (jboolean *)initialization},
    {"fetch_image", "([BII)Z", (jboolean *)fetch_image},
    {"get_result", "(II)[I", (jintArray *)get_result},
    {"image_width", "()I", (jint *)image_width},
    {"image_height", "()I", (jint *)image_height},
    {"get_detection", "(II)[I", (jintArray *)get_detection},
    {"set_model", "(Ljava/lang/String;Ljava/lang/String;)Z", (jboolean *)set_model},
};

extern "C"
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* jvm, void* reserved) {
    JNIEnv *env=NULL;
    // 最最最最重要的是先获取env
    if (jvm->GetEnv((void**)&env, JNI_VERSION_1_4)!=JNI_OK) {
        return -1;
    }
    LOGD("### start load native function!!!");
    // 获取app所在的类
    jclass jcl = env->FindClass("com/example/ubuntu/mycameraapp/Singleton_Native");
    env->RegisterNatives(jcl, nativeMethod, sizeof(nativeMethod)/ sizeof(nativeMethod[0]));
    return JNI_VERSION_1_4;
}


