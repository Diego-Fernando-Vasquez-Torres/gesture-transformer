#ifndef EXTERNAL_PROCESSOR_H
#define EXTERNAL_PROCESSOR_H

#include <string>
#include <map>
#include <opencv2/opencv.hpp>
#include "../../modulo1-adquisicion-inferencia/hand_detector.h"
#include "class_name_mapper.h"

class ExternalProcessor {
public:
    ExternalProcessor(HandDetector& det, const std::string& outPath);
    
    // Procesa carpeta con subcarpetas por clase
    bool processFolder(const std::string& inputFolder);
    
    // Procesa video, extrayendo 1 de cada N frames
    bool processVideoFile(const std::string& videoPath,
                          const std::string& gestureName,
                          int frameSkip = 5);
    
    void printReport() const;

private:
    HandDetector& detector;
    std::string outputPath;
    int totalProcessed;
    int totalValid;
    std::map<std::string, int> classCounts;

     // Extrae nombre de gesto de carpetas tipo: gesto0_puno_external_imgs → puno
    std::string parseGestureName(const std::string& folderName) const;
    bool isValidGesture(const std::string& name) const;
};

#endif