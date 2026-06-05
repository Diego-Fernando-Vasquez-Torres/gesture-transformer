#ifndef LANDMARK_VISUALIZER_H
#define LANDMARK_VISUALIZER_H

#include <opencv2/opencv.hpp>
#include "hand_detector.h"

class LandmarkVisualizer {
public:
    LandmarkVisualizer();
    ~LandmarkVisualizer();
    void renderLandmarks(cv::Mat& frame, const HandLandmarks& landmarks);
    void renderNoHandAlert(cv::Mat& frame);
    void renderFPS(cv::Mat& frame, float fps);
};

#endif 