#include "camera_manager.h"
#include <iostream>

CameraManager::CameraManager(int deviceId) : deviceId(deviceId) {}
CameraManager::~CameraManager() { releaseCamera(); }

bool CameraManager::openCamera(int width, int height) {
    cap.open(deviceId);
    if (!cap.isOpened()) {
        std::cerr << "[ERROR] Hardware: No se pudo acceder a la webcam." << std::endl;
        return false;
    }
    cap.set(cv::CAP_PROP_FRAME_WIDTH, width);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, height);
    return true;
}

bool CameraManager::getFrame(cv::Mat& frame) {
    if (!cap.isOpened()) return false;
    cap >> frame;
    if (frame.empty()) return false;
    cv::flip(frame, frame, 1); // Efecto espejo natural para el usuario
    return true;
}

void CameraManager::releaseCamera() {
    if (cap.isOpened()) cap.release();
}