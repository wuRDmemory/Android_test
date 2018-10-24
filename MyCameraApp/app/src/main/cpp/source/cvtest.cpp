//
// Created by ubuntu on 18-9-22.
//

#include "cvtest.h"
#include "mylog.h"
#include <string>

vector<string> classes{"person",
                       "bicycle", "car", "motorbike", "aeroplane", "bus", "train", "truck", "boat",
                       "traffic light", "fire hydrant", "stop sign", "parking meter", "bench", "bird", "cat", "dog",
                       "horse", "sheep", "cow", "elephant", "bear", "zebra", "giraffe", "backpack",
                       "umbrella", "handbag", "tie", "suitcase", "frisbee", "skis", "snowboard", "sports ball",
                       "kite", "baseball bat", "baseball glove", "skateboard", "surfboard", "tennis racket", "bottle", "wine glass",
                       "cup", "fork", "knife", "spoon", "bowl", "banana", "apple", "sandwich",
                       "orange", "broccoli", "carrot", "hot dog", "pizza", "donut", "cake", "chair",
                       "sofa", "pottedplant", "bed", "diningtable", "toilet", "tvmonitor", "laptop", "mouse",
                       "remote", "keyboard", "cell phone", "microwave", "oven", "toaster", "sink", "refrigerator"};

bool ImageUtil::fetch_image(Mat &rgb) {
    unique_lock<mutex> lock1(m_mutex);
    m_image = rgb.clone();
    return true;
}

Mat ImageUtil::get_result(int width, int height) {
    Mat ret;
    resize(m_image, ret, Size(width, height));
    cvtColor(ret, ret, cv::COLOR_RGB2RGBA);
    return ret;
}

Mat ImageUtil::get_detection_result(int width, int height) {
    int IN_WIDTH = 300;
    int IN_HEIGHT = 300;
    float WH_RATIO = (float)IN_WIDTH / IN_HEIGHT;
    double IN_SCALE_FACTOR = 0.007843;
    double MEAN_VAL = 127.5;
    double THRESHOLD = 0.2;
    // Get a new frame
    Mat frame;
    {
        unique_lock<mutex> lock1(m_mutex);
        m_image.copyTo(frame);
    }

    // Forward image through network.
//    Mat blob = dnn::blobFromImage(m_image, IN_SCALE_FACTOR,
//                                 Size(IN_WIDTH, IN_HEIGHT),
//                                 Scalar(MEAN_VAL, MEAN_VAL, MEAN_VAL), /*swapRB*/false, /*crop*/false);

    Mat blob = cv::dnn::blobFromImage(frame, 1.0/255, Size(416, 416), Scalar(0,0,0), true, false);

    m_net.setInput(blob);
    vector<Mat> outs;
    m_net.forward(outs, getOutputsNames(m_net));
    process_image(frame, outs);

    resize(frame, frame, Size(width, height));
    cvtColor(frame, frame, cv::COLOR_RGB2RGBA);
    return frame;
}

bool ImageUtil::set_model_path(cv::String& model, cv::String& weight) {
    m_net = dnn::readNetFromDarknet(model, weight);
    if (m_net.empty()) {
        return false;
    }
    m_net.setPreferableBackend(dnn::DNN_BACKEND_OPENCV);
    LOGD(">>> [load model] load model successfully!!!");
    return true;
}

vector<String> ImageUtil::getOutputsNames(const dnn::Net &net) {
    static vector<String> names;
    if (names.empty())
    {
        // Get the indices of the output layers, i.e. the layers with unconnected outputs
        // usually, the output layers is unconections
        vector<int> outLayers = net.getUnconnectedOutLayers();

        // get the names of all the layers in the network
        vector<String> layersNames = net.getLayerNames();

        // Get the names of the output layers in names
        names.resize(outLayers.size());
        for (size_t i = 0; i < outLayers.size(); ++i)
            names[i] = layersNames[outLayers[i] - 1];
    }
    return names;
}

void ImageUtil::drawPred(int classId, float conf, int left, int top, int right, int bottom,
                         Mat &frame) {
    //Draw a rectangle displaying the bounding box
    rectangle(frame, Point(left, top), Point(right, bottom), Scalar(255, 178, 50), 1);

    //Get the label for the class name and its confidence
    string label = format("%.2f", conf);
    if (!classes.empty())
    {
        CV_Assert(classId < (int)classes.size());
        label = classes[classId] + ":" + label;
    }

    //Display the label at the top of the bounding box
    int baseLine;
    Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.15, 1, &baseLine);
    top = max(top, labelSize.height);
    rectangle(frame, Point(left, top - round(1.5*labelSize.height)), Point(left + round(1.5*labelSize.width), top + baseLine), Scalar(255, 255, 255), FILLED);
    putText(frame, label, Point(left, top), FONT_HERSHEY_SIMPLEX, 0.15, Scalar(0,0,0), 1);
}

void ImageUtil::process_image(Mat &frame, vector<Mat> &outs) {
    vector<int> classIds;
    vector<float> confidences;
    vector<Rect> boxes;

    for (int i = 0; i < outs.size(); ++i) {
        float* data = outs[i].ptr<float>();
        for (int j = 0; j < outs[i].rows; ++j, data += outs[i].cols) {
            Mat score = outs[i].row(j).colRange(5, outs[i].cols);
            Point classIdPoint;
            double confidence;

            minMaxLoc(score, 0, &confidence, 0, &classIdPoint);
            if (confidence > 0.50f) {
                int centerX = (int)(data[0]*frame.cols);
                int centerY = (int)(data[1]*frame.rows);
                int width   = (int)(data[2]*frame.cols);
                int height  = (int)(data[3]*frame.rows);
                int left = centerX - width / 2;
                int top  = centerY - height / 2;

                classIds.emplace_back(classIdPoint.x);
                confidences.emplace_back((float)confidence);
                boxes.emplace_back(Rect(left, top, width, height));
            }
        }
    }

    vector<int> indices;
    dnn::NMSBoxes(boxes, confidences, 0.5f, 0.4f, indices);
    for (int k = 0; k < indices.size(); ++k) {
        int idx = indices[k];
        Rect box = boxes[idx];
        drawPred(classIds[idx], confidences[idx], box.x, box.y,
                 box.x + box.width, box.y + box.height, frame);
    }
}




