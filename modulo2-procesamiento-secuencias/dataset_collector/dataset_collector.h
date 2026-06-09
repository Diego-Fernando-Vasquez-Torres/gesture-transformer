// tools/dataset_collector/dataset_collector.h
#ifndef DATASET_COLLECTOR_H
#define DATASET_COLLECTOR_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <random>

#include <opencv2/opencv.hpp>
#include "../../modulo1-adquisicion-inferencia/hand_detector.h"
struct Landmark; //forward declaration
using HandLandmarks = std::vector < Landmark > ;

struct GestureClass {
    int id;
    std::string name;
    std::string emoji;
    char hotkey;
};

extern const std::vector < GestureClass > GESTURE_CLASSES; // Declaración externa

// DATASET MANAGER
class DatasetManager {
public:
    explicit DatasetManager(const std::string& filename);
    bool open(); // Abre el archivo CSV y escribe el header
    bool saveSample(const std::string& gestureName, const HandLandmarks& landmarks);

    void printStats() const;
    void close();
    const std::map<std::string, int>& getCounts() const;
    int getTotal() const;

private:
    std::string csvFilename;
    std::ofstream csvFile;
    std::map<std::string, int> counts;
    int totalSamples;
};

// DATASET SPLITTER
class DatasetSplitter {
public:
    static bool split(const std::string& inputFile,
        const std::string& trainFile,
        const std::string& valFile,
        const std::string& testFile,
        float trainRatio = 0.8f,
        float valRatio = 0.1f);

private:
    static void writeFile(const std::string& filename,
        const std::string& header,
        const std::vector<std::string>& lines,
        size_t start,
        size_t end);
};

// CAPTURE UI
class CaptureUI {
public:
    CaptureUI();

    void render(cv::Mat& frame,
        const HandLandmarks& landmarks,
        const GestureClass& currentGesture,
        int currentCount,
        const std::map<std::string, int>& allCounts,
        bool handDetected,
        bool justSaved);

    void drawLandmarks(cv::Mat& frame, const HandLandmarks& landmarks);

private:
    void drawPanel(cv::Mat& frame, cv::Point topLeft, cv::Point bottomRight);
};

#endif // DATASET_COLLECTOR_H