CXX := clang++
PKG_CONFIG := pkg-config

TARGET ?= pipeline_ejecutable
BUILD_DIR ?= build

SOURCES := main.cpp camera_manager.cpp hand_detector.cpp landmark_visualizer.cpp
OBJECTS := $(SOURCES:%.cpp=$(BUILD_DIR)/%.o)
DEPENDENCIES := $(OBJECTS:.o=.d)

CXXFLAGS := -std=c++17 -Wall -Wextra
OPENCV_CFLAGS := $(shell $(PKG_CONFIG) --cflags opencv4 2>/dev/null)
OPENCV_LIBS := $(shell $(PKG_CONFIG) --libs opencv4 2>/dev/null)

.PHONY: all check-deps run clean

all: check-deps $(TARGET)

check-deps:
	@printf "[CHECK] Verificando clang++ y OpenCV...\n"
	@command -v $(CXX) >/dev/null 2>&1 || { printf "[ERROR] No se encontro $(CXX). Instala las herramientas de Xcode con: xcode-select --install\n"; exit 1; }
	@command -v $(PKG_CONFIG) >/dev/null 2>&1 || { printf "[ERROR] No se encontro pkg-config. Instalalo con: brew install pkg-config\n"; exit 1; }
	@$(PKG_CONFIG) --exists opencv4 || { printf "[ERROR] No se encontro OpenCV 4. Instalalo con: brew install opencv\n"; exit 1; }
	@printf "[CHECK] Dependencias disponibles.\n\n"

$(TARGET): $(OBJECTS)
	@printf "[LINK] Enlazando %s\n" "$@"
	$(CXX) $(OBJECTS) -o $@ $(OPENCV_LIBS)
	@printf "\n[OK] Compilacion terminada: %s\n" "$@"

$(BUILD_DIR)/%.o: %.cpp | $(BUILD_DIR)
	@printf "[CXX] Compilando %s\n" "$<"
	$(CXX) $(CXXFLAGS) $(OPENCV_CFLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR):
	@printf "[MKDIR] Creando %s\n" "$@"
	mkdir -p $@

run: all
	@printf "[RUN] Ejecutando ./%s\n" "$(TARGET)"
	./$(TARGET)

clean:
	@printf "[CLEAN] Eliminando %s y %s\n" "$(BUILD_DIR)" "$(TARGET)"
	rm -rf $(BUILD_DIR) $(TARGET)

-include $(DEPENDENCIES)
