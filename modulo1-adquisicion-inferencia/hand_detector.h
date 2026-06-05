#ifndef HAND_DETECTOR_H
#define HAND_DETECTOR_H

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <vector>

struct Landmark {
    float x;
    float y;
    float z;
};

using HandLandmarks = std::vector<Landmark>;

class HandDetector {
public:
    HandDetector();
    ~HandDetector();
    
    bool initialize(const std::string& modelPath);
    bool detectHand(const cv::Mat& frame, HandLandmarks& output);

private:
    cv::dnn::Net net;
    bool is_initialized;
};

#endif 