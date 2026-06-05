#include "landmark_visualizer.h"

LandmarkVisualizer::LandmarkVisualizer() {}
LandmarkVisualizer::~LandmarkVisualizer() {}

void LandmarkVisualizer::renderLandmarks(cv::Mat& frame, const HandLandmarks& landmarks) {
    for (size_t i = 0; i < landmarks.size(); ++i) {
        // Desnormalizar los valores [0.0, 1.0] devueltos por el detector al tamaño real de tu pantalla (640x480)
        int cx = static_cast<int>(landmarks[i].x * frame.cols);
        int cy = static_cast<int>(landmarks[i].y * frame.rows);

        if (cx >= 0 && cx < frame.cols && cy >= 0 && cy < frame.rows) {
            // Dibujar articulación anatómica (Verde sólido)
            cv::circle(frame, cv::Point(cx, cy), 5, cv::Scalar(0, 255, 0), cv::FILLED);
            
            // Trazar las líneas del esqueleto conectando los nodos secuenciales
            if (i > 0 && i != 5 && i != 9 && i != 13 && i != 17) {
                int px = static_cast<int>(landmarks[i-1].x * frame.cols);
                int py = static_cast<int>(landmarks[i-1].y * frame.rows);
                cv::line(frame, cv::Point(px, py), cv::Point(cx, cy), cv::Scalar(255, 0, 0), 2);
            }
        }
    }
}

void LandmarkVisualizer::renderNoHandAlert(cv::Mat& frame) {
    cv::putText(frame, "STATUS: SEARCHING FOR HAND...", cv::Point(30, 80),
                cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 0, 255), 2);
}

void LandmarkVisualizer::renderFPS(cv::Mat& frame, float fps) {
    std::string fpsText = "REALTIME FPS: " + std::to_string(static_cast<int>(fps));
    cv::putText(frame, fpsText, cv::Point(30, 40),
                cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(255, 255, 0), 2);
}