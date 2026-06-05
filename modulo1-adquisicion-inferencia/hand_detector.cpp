#include "hand_detector.h"
#include <iostream>
#include <vector>
#include <cmath>

HandDetector::HandDetector() : is_initialized(false) {}
HandDetector::~HandDetector() {}

bool HandDetector::initialize(const std::string& modelPath) {
    try {
        net = cv::dnn::readNetFromONNX(modelPath);
        net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
        net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
        
        is_initialized = true;
        std::cout << "Modelo Oficial OpenCV Zoo inicializado correctamente." << std::endl;
        return true;
    } catch (const cv::Exception& e) {
        std::cerr << "Error al cargar el modelo ONNX: " << e.what() << std::endl;
        return false;
    }
}

bool HandDetector::detectHand(const cv::Mat& frame, HandLandmarks& output) {
    output.clear();
    if (!is_initialized || frame.empty()) return false;

    try {
        cv::Mat blob = cv::dnn::blobFromImage(
            frame, 
            1.0f / 255.0f,               
            cv::Size(256, 256),          
            cv::Scalar(0, 0, 0),         
            true,                        
            false,                       
            CV_32F                       
        );
        
        cv::Mat modelInput;
        std::vector<int> order = {0, 2, 3, 1}; 
        cv::transposeND(blob, order, modelInput);
        
        net.setInput(modelInput);
        
        std::vector<cv::Mat> outs;
        net.forward(outs, net.getUnconnectedOutLayersNames());

        if (outs.empty() || outs[0].empty()) return false;

        cv::Mat results = outs[0];
        int total_elements = results.total();
        float* data = results.ptr<float>();

        if (total_elements < 63) return false;

        int start_index = 0;
        int step = 3;

        if (total_elements == 63) {
            start_index = 0; step = 3;
        } else if (total_elements == 105) {
            start_index = 0; step = 5;
        } else if (total_elements > 63 && total_elements < 100) {
            start_index = total_elements - 63;
            step = 3;
        }

        float sample_val = data[start_index];
        float divisor = 1.0f;
        if (std::abs(sample_val) > 1.0f) {
            divisor = 256.0f;
        }

        // PANEL DE CALIBRACIÓN EN TIEMPO REAL
        // =================================================================
        float SCALE_X = 1.15f;   // Ancho total del esqueleto
        float OFFSET_X = 0.01f;  // Negativo izquierda, positivo derecha
        float SCALE_Y = 1.05f;   // Alto total del esqueleto
        float OFFSET_Y = 0.03f; // Negativo sube, positivo baja
        // =================================================================

        for (int i = 0; i < 21; ++i) {
            int idx = start_index + i * step;
            if (idx + 2 < total_elements) {
                Landmark lm;
                
                float raw_x = data[idx] / divisor;
                float raw_y = data[idx + 1] / divisor;
                float raw_z = data[idx + 2] / divisor;
                lm.x = raw_x * SCALE_X + OFFSET_X;
                lm.y = raw_y * SCALE_Y + OFFSET_Y;
                lm.z = raw_z;
                
                output.push_back(lm);
            }
        }

        return !output.empty();
    } catch (...) {
        return false;
    }
}