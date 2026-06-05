#include <iostream>
#include <chrono>
#include "camera_manager.h"
#include "hand_detector.h"
#include "landmark_visualizer.h"

int main() {
    std::cout << "=========================================================" << std::endl;
    std::cout << "     PIPELINE MÓDULO 1: ADQUISICIÓN E INFERENCIA REAL    " << std::endl;
    std::cout << "=========================================================" << std::endl;

    CameraManager camManager(0);
    HandDetector detector;
    LandmarkVisualizer visualizer;

    if (!camManager.openCamera(640, 480)) return -1;


if (!detector.initialize("handpose_estimation_mediapipe_2023feb.onnx")) {
    std::cerr << "[CRITICO] Falló la inicialización del modelo unificado." << std::endl;
    return -1;
}

    cv::Mat frame;
    HandLandmarks landmarks;

    auto startTime = std::chrono::high_resolution_clock::now();
    int totalFrames = 0;
    float currentFPS = 0.0f;

    std::cout << "\n[OK] Sistema operativo y red neuronal en marcha. Presiona 'ESC' para cerrar." << std::endl;

    while (true) {
        if (!camManager.getFrame(frame)) break;

        // Inferencia en tiempo real buscando el patrón geométrico de la mano humana
        bool hasHand = detector.detectHand(frame, landmarks);

        if (hasHand) {
            visualizer.renderLandmarks(frame, landmarks);
        } else {
            visualizer.renderNoHandAlert(frame);
        }

        // Medidor de fotogramas por segundo de alta precisión
        totalFrames++;
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsed = currentTime - startTime;
        if (elapsed.count() >= 1.0f) {
            currentFPS = totalFrames / elapsed.count();
            totalFrames = 0;
            startTime = currentTime;
        }

        visualizer.renderFPS(frame, currentFPS);
        cv::imshow("Modulo 1 - Captura y Detección de Mano", frame);

        if (cv::waitKey(1) == 27) break; // Terminar ejecución con la tecla ESC de forma limpia
    }

    camManager.releaseCamera();
    cv::destroyAllWindows();
    std::cout << "[INFO] Recursos de la webcam liberados correctamente." << std::endl;
    return 0;
}