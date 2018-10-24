//
// Created by ubuntu on 18-9-22.
//


#ifndef MYCAMERAAPP_CVTEST_H
#define MYCAMERAAPP_CVTEST_H

#include <jni.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <mutex>

using namespace std;
using namespace cv;

class ImageUtil {
public:
    bool fetch_image(Mat& rgb);
    bool set_model_path(cv::String& model, cv::String& weight);

public:
    // getter
    int get_width() { return m_image.cols; }
    int get_height() { return m_image.rows; }
    Mat get_result(int width, int height);
    Mat get_detection_result(int width, int height);

private:
    vector<String> getOutputsNames(const dnn::Net& net);
    void drawPred(int classId, float conf, int left, int top, int right, int bottom, Mat& frame);
    void process_image(Mat& frame, vector<Mat>& outs);

private:
    Mat m_image;
    dnn::Net m_net;
    mutex m_mutex;
};

#endif //MYCAMERAAPP_CVTEST_H


