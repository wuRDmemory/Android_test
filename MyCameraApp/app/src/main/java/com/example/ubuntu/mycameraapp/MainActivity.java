package com.example.ubuntu.mycameraapp;

import android.Manifest;
import android.app.Activity;
import android.content.Context;
import android.content.pm.PackageManager;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.ImageFormat;
import android.graphics.Rect;
import android.hardware.camera2.CameraAccessException;
import android.hardware.camera2.CameraCaptureSession;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraDevice;
import android.hardware.camera2.CameraManager;
import android.hardware.camera2.CaptureRequest;
import android.hardware.camera2.CaptureResult;
import android.hardware.camera2.TotalCaptureResult;
import android.hardware.camera2.params.StreamConfigurationMap;
import android.media.Image;
import android.media.ImageReader;
import android.support.annotation.NonNull;
import android.support.v4.app.ActivityCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.lang.reflect.Array;
import java.nio.ByteBuffer;
import java.util.Arrays;

public class MainActivity extends AppCompatActivity {

    private Button mBtnOpenCamera, mBtnDetection;
    private SurfaceView mSurfaceView;
    private String mCameraID=null;
    private String TAG = "MainActivity";
    private CameraDevice mCameraDevice;
    private CameraCaptureSession mCameraSession;
    private ImageReader mImageReader;
    private boolean mDetection = false;

    private String getPath(String file, Context context) {
        AssetManager assetManager = context.getAssets();
        BufferedInputStream inputStream = null;
        try {
            // Read data from assets.
            inputStream = new BufferedInputStream(assetManager.open(file));
            byte[] data = new byte[inputStream.available()];
            inputStream.read(data);
            inputStream.close();
            // Create copy file in storage.
            File outFile = new File(context.getFilesDir(), file);
            FileOutputStream os = new FileOutputStream(outFile);
            os.write(data);
            os.close();
            // Return a path to file which may be read in common way.
            return outFile.getAbsolutePath();
        } catch (IOException ex) {
            Log.i(TAG, "Failed to upload a file");
        }
        return "";
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        /* get handler of controller */
        mBtnOpenCamera = findViewById(R.id.btn_open_camera);
        mBtnDetection  = findViewById(R.id.btn_detection);
        mSurfaceView = findViewById(R.id.surface_pic);

        mBtnOpenCamera.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Log.e(TAG, "onClick: we got a click");
                CameraManager cameraManager = (CameraManager) getSystemService(CAMERA_SERVICE);

                try {
                    for (String cameraID : cameraManager.getCameraIdList()) {
                        CameraCharacteristics cameraCharacteristics = cameraManager.getCameraCharacteristics(cameraID);
                        Integer facing = cameraCharacteristics.get(CameraCharacteristics.LENS_FACING);

                        if (facing != null && facing == CameraCharacteristics.LENS_FACING_FRONT) {
                            continue;
                        }

                        StreamConfigurationMap map = cameraCharacteristics.get(CameraCharacteristics.SCALER_STREAM_CONFIGURATION_MAP);
                        if (map == null) {
                            continue;
                        }

                        mCameraID = cameraID;
                        Log.e(TAG, ">>> We have a camera!!!");
                    }

                    if (mCameraID==null) return;

                    checkPermission(MainActivity.this, Manifest.permission.CAMERA);
                    cameraManager.openCamera(mCameraID, mStateCallback, null);
                } catch (CameraAccessException e) {
                    e.printStackTrace();
                }
            }
        });

        mBtnDetection.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                String proto = getPath("yolov3.cfg", MainActivity.this);
                String weights = getPath("yolov3.weights", MainActivity.this);
                Singleton_Native instance = Singleton_Native.getIntance();
                if (instance.set_model(proto, weights)) {
                    Log.d(TAG, "%%% [load model] success");
                    mDetection = true;
                } else {
                    mDetection = false;
                    Log.d(TAG, "%%% [load model] failed!!!!!!!!!!");
                }
            }
        });
    }

    // check user's permission
    private void checkPermission(Activity activity, String... requestPermission) {
        for (String request : requestPermission) {
            if (ActivityCompat.checkSelfPermission(activity, request) != PackageManager.PERMISSION_GRANTED) {
                Log.d(this.TAG, ">>> we don't have "+request);
                Log.d(this.TAG, ">>> now we apply "+request);
                ActivityCompat.requestPermissions(activity, new String[]{request}, 1010);
            }
        }
    }

    // print out result
    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        switch (requestCode) {
            case 1010:
                Log.d(TAG, "onRequestPermissionsResult: we got permission!!!!");
                break;
        }
    }

    private ImageReader.OnImageAvailableListener mOnImageAvailableListener = new ImageReader.OnImageAvailableListener() {
        @Override
        public void onImageAvailable(ImageReader imageReader) {
            try {
                final Image image = imageReader.acquireNextImage();
                // get image data
                ByteBuffer buffer;
                int width = image.getWidth(), height = image.getHeight(), offset = 0;
                Image.Plane[] planes = image.getPlanes();
                byte[] data = new byte[width * height * ImageFormat.getBitsPerPixel(ImageFormat.YUV_420_888) / 8];
                byte[] rowData = new byte[planes[0].getRowStride()];

                for (int i = 0; i < planes.length; i++) {
                    buffer = planes[i].getBuffer();
                    int rowStride = planes[i].getRowStride();
                    int pixelStride = planes[i].getPixelStride();
                    int w = (i == 0) ? width : width / 2;
                    int h = (i == 0) ? height : height / 2;
                    if (i==0) {
                        Log.d(TAG, String.format("??? w %d, h %d", w, h));
                    }

                    for (int row = 0; row < h; row++) {
                        int bytesPerPixel = ImageFormat.getBitsPerPixel(ImageFormat.YUV_420_888) / 8;
                        if (pixelStride == bytesPerPixel) {
                            int length = w * bytesPerPixel;
                            buffer.get(data, offset, length);
                            if (h - row != 1)
                                buffer.position(buffer.position() + rowStride - length);
                            offset += length;
                        } else {
                            if (h - row == 1)
                                buffer.get(rowData, 0, width - pixelStride + 1);
                            else
                                buffer.get(rowData, 0, rowStride);
                            for (int col = 0; col < w; col++)
                                data[offset++] = rowData[col * pixelStride];
                        }
                    }
                }
                // use cpp
                Singleton_Native singleton_native = Singleton_Native.getIntance();
                if (singleton_native.fetch_image(data, width, height)) {
                    int surface_width = mSurfaceView.getWidth(), surface_height = mSurfaceView.getHeight();
                    int[] surface_data = null;
                    if (mDetection) {
                        surface_data = singleton_native.get_detection(surface_width, surface_height);
                    } else {
                        surface_data = singleton_native.get_result(surface_width, surface_height);
                    }
                    Bitmap resultImage = Bitmap.createBitmap(surface_width, surface_height, Bitmap.Config.ARGB_8888);
                    resultImage.setPixels(surface_data,0, surface_width, 0, 0, surface_width, surface_height);
                    Canvas canvas = mSurfaceView.getHolder().lockCanvas();
                    canvas.drawBitmap(resultImage, null, new Rect(0, 0, surface_width, surface_height), null);
                    mSurfaceView.getHolder().unlockCanvasAndPost(canvas);
                }
                image.close();
            } catch (Exception e) {
                Log.e(TAG, ">>> onImageAvailable: something wrong!!!");
            }
        }
    };

    private CameraDevice.StateCallback mStateCallback = new CameraDevice.StateCallback() {
        @Override
        public void onOpened(@NonNull CameraDevice cameraDevice) {
            mCameraDevice = cameraDevice;
            // create session
            try {
                final CaptureRequest.Builder mPreviewRequest = mCameraDevice.createCaptureRequest(CameraDevice.TEMPLATE_PREVIEW);

                mImageReader = ImageReader.newInstance(640, 480, ImageFormat.YUV_420_888, 1);
                mImageReader.setOnImageAvailableListener(mOnImageAvailableListener, null);

                mPreviewRequest.addTarget(mImageReader.getSurface());
                mCameraDevice.createCaptureSession(Arrays.asList(mImageReader.getSurface()), new CameraCaptureSession.StateCallback() {
                    @Override
                    public void onConfigured(@NonNull CameraCaptureSession cameraCaptureSession) {
                        if (mCameraDevice == null) {
                            return;
                        }
                        mCameraSession = cameraCaptureSession;

                        CaptureRequest captureRequest = mPreviewRequest.build();
                        try {
                            mCameraSession.setRepeatingRequest(captureRequest, null, null);
                        } catch (CameraAccessException e) {
                            e.printStackTrace();
                        }
                    }

                    @Override
                    public void onConfigureFailed(@NonNull CameraCaptureSession cameraCaptureSession) {
                        Log.e(TAG, ">>> Open camera failed!!!");
                    }
                }, null);

            } catch (CameraAccessException e) {
                Log.e(TAG, ">>> Access camera failed!!!");
                e.printStackTrace();
            }
        }

        @Override
        public void onDisconnected(@NonNull CameraDevice cameraDevice) {
            mCameraDevice.close();
            mCameraDevice = null;
        }

        @Override
        public void onError(@NonNull CameraDevice cameraDevice, int i) {
            mCameraDevice.close();
            mCameraDevice = null;
        }
    };
}
