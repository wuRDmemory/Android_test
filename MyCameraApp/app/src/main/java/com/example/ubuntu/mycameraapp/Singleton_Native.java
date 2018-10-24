package com.example.ubuntu.mycameraapp;

import android.graphics.Bitmap;
import android.graphics.Canvas;

public class Singleton_Native {
    static {
//        System.loadLibrary("libopencv_java3");
        System.loadLibrary("native-lib");

    }

    // instance
    private static Singleton_Native singleton_native;
    // constructor
    private Singleton_Native() {
        // initial jni pointer
        initialization();
    }
    // singleton instance
    public static Singleton_Native getIntance() {
        if (singleton_native == null) {
            singleton_native = new Singleton_Native();
        }
        return singleton_native;
    }

    public native boolean initialization();

    public native boolean set_model(String model, String weight);

    public native int image_width();

    public native int image_height();

    public native boolean fetch_image(byte[] bitmap, int width, int height);

    public native int[] get_result(int width, int height);

    public native int[] get_detection(int width, int height);

}
