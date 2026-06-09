// modulo2-procesamiento-secuencias/dataset_collector/main.cpp
#include "dataset_collector.h"
#include "../../modulo1-adquisicion-inferencia/camera_manager.h"   
#include "../../modulo1-adquisicion-inferencia/hand_detector.h"       
#include <cstdlib>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;
int main(int argc, char** argv) {
    // Fix Qt6 en MSYS2
    putenv((char*)"QT_QPA_PLATFORM=windows");

    std::string modelPath = "../../modulo1-adquisicion-inferencia/handpose_estimation_mediapipe_2023feb.onnx"; //handpose_estimation_mediapipe_2023feb.onnx
    std::string outputDir = "../../data/raw/";

    // Crear directorio
    //std::string cmd = "mkdir -p " + outputDir;
    //system(cmd.c_str());
    try { //crea directorio filesystem portable
        fs::create_directories(outputDir);
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "[WARN] No se pudo crear directorio: " << e.what() << std::endl;
    }
    std::string datasetName;
    std::cout << "========================================\n";
    std::cout << "   DATASET COLLECTOR - MODULO 2\n";
    std::cout << "========================================\n";
    std::cout << "Nombre de sesion (ej: gabriela_sesion1): ";
    std::cin >> datasetName;

    std::string csvPath = outputDir + datasetName + ".csv";

    // Verificar que el modelo existe antes de seguir
    if (!fs::exists(modelPath)) {
        std::cerr << "[ERROR] Modelo ONNX no encontrado: " << modelPath << std::endl;
        std::cerr << "[INFO] Buscando en rutas alternativas..." << std::endl;

        // Intentar ruta alternativa: mismo directorio del ejecutable
        std::string altPath = "handpose_estimation_mediapipe_2023feb.onnx";
        if (fs::exists(altPath)) {
            modelPath = altPath;
            std::cout << "[INFO] Modelo encontrado en: " << modelPath << std::endl;
        }
        else {
            std::cerr << "[ERROR] No se encontró el modelo. Verifica la ruta." << std::endl;
            return -1;
        }
    }
    // Inicializar componentes
    CameraManager camera(0);
    HandDetector detector;
    DatasetManager dataset(csvPath);
    CaptureUI ui;

    if (!camera.openCamera(640, 480)) {
        std::cerr << "[ERROR] No se pudo abrir la camara.\n";
        return -1;
    }
    if (!detector.initialize(modelPath)) {
        std::cerr << "[ERROR] No se pudo cargar el modelo.\n";
        return -1;
    }
    if (!dataset.open()) {
        std::cerr << "[ERROR] No se pudo crear el CSV.\n";
        return -1;
    }

    size_t currentGestureIdx = 0;
    bool justSaved = false;
    int saveCooldown = 0;

    cv::Mat frame;
    HandLandmarks landmarks;

    std::cout << "\n[INFO] SPACE=capturar | N=siguiente | P=anterior | ESC=salir\n\n";

    while (true) {
        if (!camera.getFrame(frame)) break;

        bool handDetected = detector.detectHand(frame, landmarks);

        if (saveCooldown > 0) {
            saveCooldown--;
            if (saveCooldown == 0) justSaved = false;
        }

        const auto& currentGesture = GESTURE_CLASSES[currentGestureIdx];
        int currentCount = dataset.getCounts().count(currentGesture.name) ?
            dataset.getCounts().at(currentGesture.name) : 0;

        ui.render(frame, landmarks, currentGesture, currentCount,
            dataset.getCounts(), handDetected, justSaved);

        cv::imshow("Dataset Collector", frame);

        int key = cv::waitKey(1) & 0xFF;
        if (key == 27) break; // ESC

        // Cambiar gesto con hotkeys
        for (const auto& g : GESTURE_CLASSES) {
            if (key == g.hotkey) {
                currentGestureIdx = g.id;
                justSaved = false;
                std::cout << "[GESTO] " << g.emoji << " " << g.name << std::endl;
                break;
            }
        }

        if (key == 'n' || key == 'N') {
            currentGestureIdx = (currentGestureIdx + 1) % GESTURE_CLASSES.size();
        }
        if (key == 'p' || key == 'P') {
            currentGestureIdx = (currentGestureIdx + GESTURE_CLASSES.size() - 1) % GESTURE_CLASSES.size();
        }

        // CAPTURAR
        if (key == ' ') {
            if (handDetected && landmarks.size() == 21) {
                if (dataset.saveSample(currentGesture.name, landmarks)) {
                    justSaved = true;
                    saveCooldown = 15;
                    std::cout << "[+] " << currentGesture.emoji << " "
                        << currentGesture.name << " #"
                        << dataset.getCounts().at(currentGesture.name) << std::endl;
                }
            }
            else {
                std::cout << "[!] No guardado: sin mano o landmarks incompletos" << std::endl;
            }
        }
    }

    camera.releaseCamera();
    dataset.close();
    cv::destroyAllWindows();

    dataset.printStats();

    // Split opcional
    char doSplit;
    std::cout << "\n¿Dividir en train/val/test? (s/n): ";
    std::cin >> doSplit;

    if (doSplit == 's' || doSplit == 'S') {
        DatasetSplitter::split(csvPath,
            outputDir + datasetName + "_train.csv",
            outputDir + datasetName + "_val.csv",
            outputDir + datasetName + "_test.csv");
    }

    std::cout << "\n[INFO] Dataset guardado en: " << csvPath << std::endl;
    return 0;
}