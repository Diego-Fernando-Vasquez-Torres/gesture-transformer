#include "external_processor.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;

ExternalProcessor::ExternalProcessor(HandDetector& det, const std::string& outPath) 
    : detector(det), outputPath(outPath), totalProcessed(0), totalValid(0) {}

std::string ExternalProcessor::parseGestureName(const std::string& folderName) const {
    // Formato esperado: gesto0_puno_external_imgs
    // Extraer la parte entre el primer _ y _external
    
    size_t firstUnderscore = folderName.find('_');
    if (firstUnderscore == std::string::npos) return "";
    
    size_t externalPos = folderName.find("_external");
    if (externalPos == std::string::npos) return "";
    
    // Extraer: puno (entre firstUnderscore+1 y externalPos)
    std::string gesture = folderName.substr(firstUnderscore + 1, externalPos - firstUnderscore - 1);
    
    // Reemplazar _ por espacio si hay (aunque no debería)
    std::replace(gesture.begin(), gesture.end(), '_', ' ');
    
    return gesture;
}

bool ExternalProcessor::isValidGesture(const std::string& name) const {
    // Lista de gestos válidos internos
    static const std::vector<std::string> validGestures = {
        "puno", "paz", "pulgar_arriba", "pulgar_abajo", "rock",
        "call_me", "indice_arriba", "ok", "dedos_cruzados", "cinco",
        "saludo_alien", "metal", "dedos_juntos", "dedo_medio"
    };
    
    for (const auto& g : validGestures) {
        if (g == name) return true;
    }
    return false;
}

bool ExternalProcessor::processFolder(const std::string& inputFolder) {
    
    std::ofstream csv(outputPath);
    if (!csv.is_open()) {
        std::cerr << "[ERROR] No se pudo crear: " << outputPath << std::endl;
        return false;
    }
    
    // Header CSV
    csv << "gesto";
    for (int i = 0; i < 21; ++i) {
        csv << ",x" << i << ",y" << i << ",z" << i;
    }
    csv << "\n";
    
    // Recorrer subcarpetas
    for (const auto& entry : fs::directory_iterator(inputFolder)) {
        if (!entry.is_directory()) continue;
        
        std::string folderName = entry.path().filename().string();
        std::string gestureName = parseGestureName(folderName);
        
        if (gestureName.empty()) {
            std::cout << "[SKIP] Formato no reconocido: " << folderName << std::endl;
            continue;
        }
        
        if (!isValidGesture(gestureName)) {
            std::cout << "[SKIP] Gesto no válido: " << gestureName 
                      << " (de " << folderName << ")" << std::endl;
            continue;
        }
        
        std::cout << "[PROCESANDO] " << folderName << " → " << gestureName << std::endl;
        
        int classValid = 0;
        int classTotal = 0;
        
        // Procesar imágenes de esta carpeta
        for (const auto& imgEntry : fs::directory_iterator(entry.path())) {
            if (!imgEntry.is_regular_file()) continue;
            
            std::string ext = imgEntry.path().extension().string();
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
            
            if (ext != ".jpg" && ext != ".jpeg" && ext != ".png" && 
                ext != ".bmp" && ext != ".webp") {
                continue;
            }
            
            cv::Mat img = cv::imread(imgEntry.path().string());
            if (img.empty()) {
                std::cerr << "[WARN] No se pudo leer: " << imgEntry.path() << std::endl;
                continue;
            }
            
            HandLandmarks landmarks;
            if (detector.detectHand(img, landmarks) && landmarks.size() == 21) {
                // Guardar en CSV
                csv << gestureName;
                for (const auto& lm : landmarks) {
                    csv << "," << std::fixed << std::setprecision(6) 
                        << lm.x << "," << lm.y << "," << lm.z;
                }
                csv << "\n";
                
                totalValid++;
                classValid++;
                classCounts[gestureName]++;
            }
            
            totalProcessed++;
            classTotal++;
        }
        
        std::cout << "  → " << gestureName << ": " << classValid 
                  << "/" << classTotal << " válidas\n";
    }
    
    csv.close();
    printReport();
    return true;
}

bool ExternalProcessor::processVideoFile(const std::string& videoPath,
                                          const std::string& gestureName,
                                          int frameSkip) {
    
    cv::VideoCapture cap(videoPath);
    if (!cap.isOpened()) {
        std::cerr << "[ERROR] No se pudo abrir video: " << videoPath << std::endl;
        return false;
    }
    
    bool fileExists = fs::exists(outputPath);
    std::ofstream csv(outputPath, std::ios::app);
    if (!csv.is_open()) {
        std::cerr << "[ERROR] No se pudo abrir CSV: " << outputPath << std::endl;
        return false;
    }
    
    if (!fileExists) {
        csv << "gesto";
        for (int i = 0; i < 21; ++i) {
            csv << ",x" << i << ",y" << i << ",z" << i;
        }
        csv << "\n";
    }
    
    cv::Mat frame;
    int frameCount = 0;
    int savedCount = 0;
    
    while (cap.read(frame)) {
        if (frameCount % frameSkip == 0) {
            HandLandmarks landmarks;
            if (detector.detectHand(frame, landmarks) && landmarks.size() == 21) {
                csv << gestureName;
                for (const auto& lm : landmarks) {
                    csv << "," << std::fixed << std::setprecision(6) 
                        << lm.x << "," << lm.y << "," << lm.z;
                }
                csv << "\n";
                savedCount++;
                totalValid++;
                classCounts[gestureName]++;
            }
            totalProcessed++;
        }
        frameCount++;
    }
    
    cap.release();
    csv.close();
    
    std::cout << "[VIDEO] " << videoPath << ": " << savedCount 
              << " muestras de " << frameCount << " frames\n";
    
    return true;
}

void ExternalProcessor::printReport() const {
    std::cout << "\n========== RESUMEN ==========\n";
    std::cout << "Total imágenes:  " << totalProcessed << "\n";
    std::cout << "Muestras válidas: " << totalValid << "\n";
    std::cout << "Descartadas:      " << (totalProcessed - totalValid) << "\n";
    std::cout << "Tasa de éxito:    " 
              << (totalProcessed > 0 ? 100.0f * totalValid / totalProcessed : 0) << "%\n\n";
    
    std::cout << "Por clase:\n";
    for (const auto& [cls, count] : classCounts) {
        std::cout << "  " << std::left << std::setw(20) << cls << ": " << count << "\n";
    }
    std::cout << "=============================\n";
}