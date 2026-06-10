#include "external_processor.h"
#include "../../modulo1-adquisicion-inferencia/hand_detector.h"

#include <iostream>
#include <cstdlib>
#include <fstream>

void printUsage(const char* programName) {
    std::cout << "Uso:\n";
    std::cout << "  Modo carpeta:\n";
    std::cout << "    " << programName << " folder <input_folder> <output.csv> [modelo.onnx]\n\n";
    std::cout << "  Modo video:\n";
    std::cout << "    " << programName << " video <video.mp4> <gesto_name> <output.csv> [frame_skip] [modelo.onnx]\n\n";
    std::cout << "Ejemplo carpeta:\n";
    std::cout << "  " << programName << " folder ../../data/external ../../data/external_processed.csv\n";
}

int main(int argc, char** argv) {
    if (argc < 4) {
        printUsage(argv[0]);
        return 1;
    }
    
    putenv((char*)"QT_QPA_PLATFORM=windows");
    
    std::string mode = argv[1];
    std::string modelPath = "../../modulo1-adquisicion-inferencia/handpose_estimation_mediapipe_2023feb.onnx";
    std::string inputPath, outputPath, gestureName;
    int frameSkip = 5;
    
    if (mode == "folder") {
        if (argc < 4) { printUsage(argv[0]); return 1; }
        inputPath = argv[2];
        outputPath = argv[3];
        if (argc > 4) modelPath = argv[4];
        
    } else if (mode == "video") {
        if (argc < 5) { printUsage(argv[0]); return 1; }
        inputPath = argv[2];
        gestureName = argv[3];
        outputPath = argv[4];
        if (argc > 5) frameSkip = std::stoi(argv[5]);
        if (argc > 6) modelPath = argv[6];
        
    } else {
        std::cerr << "[ERROR] Modo desconocido: " << mode << std::endl;
        return 1;
    }
    
    // Verificar modelo
    std::ifstream testModel(modelPath);
    if (!testModel.good()) {
        std::string altPath = "handpose_estimation_mediapipe_2023feb.onnx";
        std::ifstream testAlt(altPath);
        if (testAlt.good()) {
            modelPath = altPath;
        } else {
            std::cerr << "[ERROR] Modelo no encontrado: " << modelPath << std::endl;
            return -1;
        }
    }
    
    HandDetector detector;
    if (!detector.initialize(modelPath)) {
        std::cerr << "[ERROR] No se pudo cargar el modelo ONNX." << std::endl;
        return -1;
    }
    
    ExternalProcessor processor(detector, outputPath);
    bool success = false;
    
    if (mode == "folder") {
        std::cout << "========================================\n";
        std::cout << "   EXTERNAL PROCESSOR\n";
        std::cout << "========================================\n";
        std::cout << "Input:  " << inputPath << "\n";
        std::cout << "Output: " << outputPath << "\n";
        std::cout << "Modelo: " << modelPath << "\n\n";
        
        success = processor.processFolder(inputPath);
        
    } else if (mode == "video") {
        std::cout << "========================================\n";
        std::cout << "   EXTERNAL PROCESSOR - VIDEO\n";
        std::cout << "========================================\n";
        
        success = processor.processVideoFile(inputPath, gestureName, frameSkip);
    }
    
    if (success) {
        std::cout << "\n[INFO] Listo: " << outputPath << std::endl;
    } else {
        std::cerr << "\n[ERROR] Falló." << std::endl;
        return -1;
    }
    
    return 0;
}