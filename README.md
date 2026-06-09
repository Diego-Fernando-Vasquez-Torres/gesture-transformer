# Sistema de Reconocimiento de Gestos y Lenguaje de Señas Basado en Arquitecturas Transformer

## Descripción General del Proyecto
Este proyecto de investigación y desarrollo implementa un pipeline integral de visión computacional y aprendizaje profundo (Deep Learning) diseñado para la adquisición, procesamiento, modelado secuencial y clasificación de gestos complejos y lenguaje de señas en tiempo real.  

La arquitectura se fundamenta en la extracción desacoplada de características espaciales mediante modelos de estimación de pose en combinación con una red neuronal basada en mecanismos de auto-atención (Transformer Encoder) para capturar las dependencias temporales de los movimientos.

---

## Arquitectura de Directorios del Repositorio

El espacio de trabajo se organiza mediante una estructura modular de cinco etapas independientes pero interconectadas:

```text
gesture-transformer/
├── .gitignore
├── README.md
├── modulo1-adquisicion-inferencia/
│   ├── main.cpp
│   ├── Makefile
│   ├── camera_manager.cpp
│   ├── camera_manager.h
│   ├── hand_detector.cpp
│   ├── hand_detector.h
│   ├── landmark_visualizer.cpp
│   └── landmark_visualizer.h
├── modulo2-procesamiento-secuencias/
├── modulo3-arquitectura-transformer/
├── modulo4-entrenamiento-optimizacion/
└── modulo5-despliegue-integracion/

```

---

## Estructura Detallada de Módulos (Plantilla de Desarrollo)

### Módulo 1: Adquisición de Video e Inferencia de Características Local (Completado)

* **Descripción:** Encargado de la captura de flujo de video nativo, inferencia en tiempo real mediante backend DNN para la extracción tridimensional de los 21 puntos anatómicos, y la **exportación automatizada de secuencias temporales en formato CSV** (`landmarks_dataset.csv`) para alimentar el entrenamiento del Módulo 2.

### Módulo 2: Procesamiento de Secuencias y Ventanas Temporales

* **Descripción:** Bloque dedicado a la acumulación de coordenadas consecutivas desde el CSV generado, aplicando técnicas de remuestreo y normalización para generar tensores de entrada invariantes a la escala y posición.

### Módulo 3: Arquitectura e Implementación del Transformer

* **Descripción:** Definición matemática de la red neuronal. Incorpora codificación posicional (Positional Encoding) y Multi-Head Attention para el análisis temporal.

### Módulo 4: Entrenamiento, Validación y Optimización del Modelo

* **Descripción:** Scripts orientados a la ingesta del dataset (CSV), funciones de pérdida, optimizadores (AdamW) y conversión a formatos eficientes (ONNX/TensorRT).

### Módulo 5: Interfaz de Despliegue e Integración de Sistemas

* **Descripción:** Aplicación final orientada al usuario final, integrando módulos de traducción visual o síntesis de voz basada en predicciones.

---

## Instrucciones de Réplica y Ejecución

El Módulo 1 está optimizado para correr de forma nativa en C++, mientras genera los datos en texto plano (CSV) para que los Módulos 2 a 5 puedan ser desarrollados íntegramente en Python (independientemente del SO).

### Opción A: Compilación en macOS / Linux

Requiere GCC/Clang, OpenCV 4.x y `pkg-config`. En macOS, instale las dependencias con Homebrew:

```bash
brew install opencv pkg-config
```

Luego, navegue a la carpeta del Módulo 1 y ejecute:

```bash
cd modulo1-adquisicion-inferencia
g++ -std=c++17 main.cpp camera_manager.cpp hand_detector.cpp landmark_visualizer.cpp -o pipeline_ejecutable `pkg-config --cflags --libs opencv4`
./pipeline_ejecutable

```

### Opción B: Makefile para macOS con progreso visible

Como alternativa al comando manual, el Módulo 1 incluye un `Makefile` para macOS. Este muestra cada archivo que se compila, los comandos ejecutados y la etapa de enlazado, evitando que la compilación parezca detenida.

Después de instalar OpenCV y `pkg-config` con Homebrew, ejecute:

```bash
cd modulo1-adquisicion-inferencia
make
make run
```

Para eliminar los archivos generados y volver a compilar desde cero:

```bash
make clean
make
```

### Opción C: Ejecución en Windows (C++)

Para compilar nativamente en Windows sin modificar el código:

1. Instalar [MSYS2](https://www.msys2.org/) y utilizar el entorno `UCRT64`.
2. Instalar el compilador y dependencias mediante la terminal de MSYS2:
`pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-opencv mingw-w64-ucrt-x86_64-pkgconf`
3. Ejecutar exactamente el mismo comando de compilación (`g++ -std=c++17...`) descrito en la Opción A desde la terminal UCRT64.

### Puente de Datos para el Módulo 2 (Multiplataforma)

**Nota para el equipo:** Si tu entorno de desarrollo para el Módulo 2 es Python (Windows/Mac), **no es estrictamente necesario compilar el Módulo 1**. Puedes ejecutar la captura desde un equipo ya configurado (ej. macOS), generar el archivo `landmarks_dataset.csv` que contiene el rastreo espaciotemporal, y consumir directamente ese archivo desde scripts de Python usando `pandas` o `NumPy` para estructurar los tensores del Transformer.

---

### Calibración de la Relación de Aspecto

Debido a las variaciones entre los sensores de cámara (16:9 o 16:10) y las restricciones geométricas del modelo (1:1), el archivo `hand_detector.cpp` expone variables de ajuste en sus líneas de calibración:

* `SCALE_X` / `SCALE_Y`: Coeficientes para expandir/contraer el esqueleto.
* `OFFSET_X` / `OFFSET_Y`: Traslación espacial para centrar el modelo predictivo sobre el plano real.
