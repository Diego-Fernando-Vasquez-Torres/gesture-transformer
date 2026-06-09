// tools/dataset_collector/dataset_collector.cpp
#include "dataset_collector.h"
// DEFINICIÓN DE GESTURE_CLASSES

const std::vector < GestureClass > GESTURE_CLASSES = {
    {0,  "puño",           "✊",  '0'},
    {1,  "paz",            "✌️",  '1'},
    {2,  "pulgar_arriba",  "👍",  '2'},
    {3,  "pulgar_abajo",   "👎",  '3'},
    {4,  "rock",           "🤟",  '4'},
    {5,  "call_me",        "🤙",  '5'},
    {6,  "indice_arriba",  "☝️",  '6'},
    {7,  "ok",             "👌",  '7'},
    {8,  "dedos_cruzados", "🤞",  '8'},
    {9,  "cinco",          "🖐️",  '9'},
    {10, "saludo_alien",   "🖖",  'a'},
    {11, "metal",          "🤘",  'b'},
    {12, "dedos_juntos",   "🤌",  'c'},
    {13, "dedo_medio",     "🖕",  'd'}
};

// DATASET MANAGER
DatasetManager::DatasetManager(const std::string& filename)
    : csvFilename(filename), totalSamples(0) {
    for (const auto& g : GESTURE_CLASSES) {
        counts[g.name] = 0;
    }
}

bool DatasetManager::open() {
    csvFile.open(csvFilename, std::ios::out);
    if (!csvFile.is_open()) {
        std::cerr << "[ERROR] No se pudo crear: " << csvFilename << std::endl;
        return false;
    }

    
    csvFile << "gesto"; // Header: gesto + 63 columnas de landmarkss
    for (int i = 0; i < 21; ++i) {
        csvFile << ",x" << i << ",y" << i << ",z" << i;
    }
    csvFile << "\n";

    std::cout << "[INFO] Dataset creado: " << csvFilename << std::endl;
    return true;
}

bool DatasetManager::saveSample(const std::string& gestureName,
    const HandLandmarks& landmarks) {
    if (landmarks.size() != 21) {
        std::cerr << "[WARN] Landmarks incompletos (" << landmarks.size()
            << "/21), omitiendo." << std::endl;
        return false;
    }

    csvFile << gestureName;
    for (const auto& lm : landmarks) {
        csvFile << "," << std::fixed << std::setprecision(6)
            << lm.x << "," << lm.y << "," << lm.z;
    }
    csvFile << "\n";

    counts[gestureName]++;
    totalSamples++;
    return true;
}

void DatasetManager::printStats() const {
    std::cout << "\n========== ESTADÍSTICAS DE CAPTURA ==========\n";
    for (const auto& g : GESTURE_CLASSES) {
        auto it = counts.find(g.name);
        int count = (it != counts.end()) ? it->second : 0;
        std::cout << "  " << g.emoji << " " << std::left << std::setw(18)
            << g.name << " : " << count << " muestras\n";
    }
    std::cout << "  TOTAL: " << totalSamples << " muestras\n";
    std::cout << "=============================================\n";
}

void DatasetManager::close() {
    if (csvFile.is_open()) csvFile.close();
}

const std::map<std::string, int>& DatasetManager::getCounts() const {
    return counts;
}

int DatasetManager::getTotal() const {
    return totalSamples;
}

// DATASET SPLITTER
bool DatasetSplitter::split(const std::string& inputFile,
    const std::string& trainFile,
    const std::string& valFile,
    const std::string& testFile,
    float trainRatio,
    float valRatio) {

    std::ifstream in(inputFile);
    if (!in.is_open()) {
        std::cerr << "[ERROR] No se pudo leer: " << inputFile << std::endl;
        return false;
    }

    
    std::string header;
    std::getline(in, header); // Leer header

    std::vector<std::string> lines;
    std::string line;
    while (std::getline(in, line)) {
        if (!line.empty()) lines.push_back(line);
    }
    in.close();
    if (lines.empty()) {
        std::cerr << "[WARN] Archivo vacío: " << inputFile << std::endl;
        return false;
    }


    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(lines.begin(), lines.end(), g);

    size_t total = lines.size();
    size_t trainEnd = static_cast<size_t>(total * trainRatio);
    size_t valEnd = trainEnd + static_cast<size_t>(total * valRatio);    // Calcular índices de split

    writeFile(trainFile, header, lines, 0, trainEnd);
    writeFile(valFile, header, lines, trainEnd, valEnd);
    writeFile(testFile, header, lines, valEnd, total);

    std::cout << "\n========== SPLIT COMPLETADO ==========\n";
    std::cout << "  Total:   " << total << " muestras\n";
    std::cout << "  Train:   " << trainEnd << " ("
        << (100.0f * trainEnd / total) << "%)\n";
    std::cout << "  Val:     " << (valEnd - trainEnd) << " ("
        << (100.0f * (valEnd - trainEnd) / total) << "%)\n";
    std::cout << "  Test:    " << (total - valEnd) << " ("
        << (100.0f * (total - valEnd) / total) << "%)\n";
    std::cout << "======================================\n";

    return true;
}

void DatasetSplitter::writeFile(const std::string& filename,
    const std::string& header,
    const std::vector<std::string>& lines,
    size_t start,
    size_t end) {
    std::ofstream out(filename);
    if (!out.is_open()) {
        std::cerr << "[ERROR] No se pudo crear: " << filename << std::endl;
        return;
    }

    out << header << "\n";
    for (size_t i = start; i < end && i < lines.size(); ++i) {
        out << lines[i] << "\n";
    }
    out.close();

    std::cout << "[INFO] " << filename << " (" << (end - start) << " muestras)\n";
}

// CAPTURE UI
CaptureUI::CaptureUI() {}

void CaptureUI::render(cv::Mat& frame,
    const HandLandmarks& landmarks,
    const GestureClass& currentGesture,
    int currentCount,
    const std::map<std::string, int>& allCounts,
    bool handDetected,
    bool justSaved) {

    drawPanel(frame, cv::Point(0, 0), cv::Point(360, 480));
    cv::putText(frame, "DATASET COLLECTOR", cv::Point(10, 30),
        cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 255, 255), 2);

    std::string currentText = "ACTUAL: " + currentGesture.emoji + " " + currentGesture.name;
    cv::Scalar currentColor = handDetected ? cv::Scalar(0, 255, 0) : cv::Scalar(0, 0, 255);
    cv::putText(frame, currentText, cv::Point(10, 70),
        cv::FONT_HERSHEY_SIMPLEX, 0.6, currentColor, 2);

    std::string countText = "Muestras: " + std::to_string(currentCount);
    cv::putText(frame, countText, cv::Point(10, 100),
        cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);

    std::string status = handDetected ? "MANO DETECTADA" : "SIN MANO";
    cv::Scalar statusColor = handDetected ? cv::Scalar(0, 255, 0) : cv::Scalar(0, 0, 255);
    cv::putText(frame, status, cv::Point(10, 140),
        cv::FONT_HERSHEY_SIMPLEX, 0.5, statusColor, 2);

    if (justSaved) {
        cv::putText(frame, "GUARDADO!", cv::Point(10, 180),
            cv::FONT_HERSHEY_SIMPLEX, 0.9, cv::Scalar(0, 255, 0), 3);
    }

    // Lista de todos los gestos con contadores
    cv::putText(frame, "--- GESTOS ---", cv::Point(10, 220),
        cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(200, 200, 200), 1);

    int y = 250;
    for (const auto& g : GESTURE_CLASSES) {
        auto it = allCounts.find(g.name);
        int count = (it != allCounts.end()) ? it->second : 0;

        std::string line = std::string(1, g.hotkey) + ") " + g.emoji + " " + g.name;
        line += " (" + std::to_string(count) + ")";

        cv::Scalar color = (g.id == currentGesture.id) ?
            cv::Scalar(0, 255, 255) : cv::Scalar(180, 180, 180);
        cv::putText(frame, line, cv::Point(10, y),
            cv::FONT_HERSHEY_SIMPLEX, 0.4, color, 1);
        y += 22;
    }

    cv::putText(frame, "SPACE:capturar | N:next | P:prev | ESC:salir",
        cv::Point(10, 470), cv::FONT_HERSHEY_SIMPLEX, 0.4,
        cv::Scalar(255, 255, 0), 1);

    if (handDetected) {
        drawLandmarks(frame, landmarks);     // Dibujar landmarks

    }
}

void CaptureUI::drawLandmarks(cv::Mat& frame, const HandLandmarks& landmarks) {
    for (size_t i = 0; i < landmarks.size(); ++i) {
        int cx = static_cast<int>(landmarks[i].x * frame.cols);
        int cy = static_cast<int>(landmarks[i].y * frame.rows);

        if (cx >= 0 && cx < frame.cols && cy >= 0 && cy < frame.rows) {
            cv::circle(frame, cv::Point(cx, cy), 5, cv::Scalar(0, 255, 0), -1); //articulacion

            if (i > 0 && i != 5 && i != 9 && i != 13 && i != 17) { //conexiones
                int px = static_cast<int>(landmarks[i - 1].x * frame.cols);
                int py = static_cast<int>(landmarks[i - 1].y * frame.rows);
                cv::line(frame, cv::Point(px, py), cv::Point(cx, cy),
                    cv::Scalar(255, 0, 0), 2);
            }
        }
    }
}

void CaptureUI::drawPanel(cv::Mat& frame, cv::Point topLeft, cv::Point bottomRight) {
    cv::Mat overlay;
    frame.copyTo(overlay);
    cv::rectangle(overlay, topLeft, bottomRight, cv::Scalar(0, 0, 0), -1);
    cv::addWeighted(overlay, 0.65, frame, 0.35, 0, frame);
}