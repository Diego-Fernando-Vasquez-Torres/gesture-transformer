#ifndef CAMERA_MANAGER_H
#define CAMERA_MANAGER_H

#include <opencv2/opencv.hpp>

class CameraManager {
public:
    CameraManager(int deviceId = 0);
    ~CameraManager();
    bool openCamera(int width = 640, int height = 480);
    bool getFrame(cv::Mat& frame);
    void releaseCamera();

private:
    cv::VideoCapture cap;
    int deviceId;
};

#endif // CAMERA_MANAGER_H