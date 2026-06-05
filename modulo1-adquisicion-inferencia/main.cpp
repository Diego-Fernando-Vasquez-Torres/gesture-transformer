#include "camera_manager.h"
#include "hand_detector.h"
#include "landmark_visualizer.h"
#include <iostream>
#include <fstream>
#include <chrono>

int main() {
    CameraManager camera;
    HandDetector detector;
    LandmarkVisualizer visualizer;

    // AQUI ESTA LA CORRECCION 1
    if (!camera.openCamera()) {
        std::cerr << "No se pudo abrir la cámara." << std::endl;
        return -1;
    }

    if (!detector.initialize("handpose_estimation_mediapipe_2023feb.onnx")) {
        std::cerr << "Falló la inicialización del modelo unificado." << std::endl;
        return -1;
    }

    std::ofstream csvFile("landmarks_dataset.csv");
    if (csvFile.is_open()) {
        csvFile << "timestamp";
        for (int i = 0; i < 21; ++i) {
            csvFile << ",x" << i << ",y" << i << ",z" << i;
        }
        csvFile << "\n";
    }

    std::cout << "-----------------------------------------\n";
    std::cout << "   PIPELINE MÓDULO 1: ADQUISICIÓN REAL   \n";
    std::cout << "-----------------------------------------\n";
    std::cout << "Grabando telemetría en 'landmarks_dataset.csv'...\n";
    std::cout << "Presiona 'ESC' para cerrar y guardar el dataset.\n";

    cv::Mat frame;
    HandLandmarks landmarks;

    while (true) {
        if (!camera.getFrame(frame)) break;

        if (detector.detectHand(frame, landmarks)) {
            visualizer.renderLandmarks(frame, landmarks);

            if (csvFile.is_open()) {
                auto now = std::chrono::system_clock::now();
                auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
                csvFile << ms;
                for (const auto& lm : landmarks) {
                    csvFile << "," << lm.x << "," << lm.y << "," << lm.z;
                }
                csvFile << "\n";
            }
        }

        cv::imshow("Gesture Transformer - Adquisición", frame);

        if (cv::waitKey(1) == 27) { // Tecla ESC
            break;
        }
    }

    if (csvFile.is_open()) csvFile.close();
    std::cout << "[INFO] Recursos liberados. Dataset CSV generado exitosamente." << std::endl;
    return 0;
}