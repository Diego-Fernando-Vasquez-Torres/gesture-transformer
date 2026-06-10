#include "class_name_mapper.h"
#include <iostream>
#include <algorithm>
#include <cctype>

ClassNameMapper::ClassNameMapper() {
    // === MAPEO DE NOMBRES EXTERNOS → INTERNOS ===
    // Ajusta según el dataset que descargues
    
    // Puño
    addMapping("fist", "puño");
    addMapping("punch", "puño");
    addMapping("closed_hand", "puño");
    addMapping("zero", "puño");
    
    // Paz / Victoria
    addMapping("peace", "paz");
    addMapping("victory", "paz");
    addMapping("v_sign", "paz");
    addMapping("two", "paz");
    
    // Pulgar arriba
    addMapping("thumbs_up", "pulgar_arriba");
    addMapping("thumbsup", "pulgar_arriba");
    addMapping("like", "pulgar_arriba");
    addMapping("thumb_up", "pulgar_arriba");
    
    // Pulgar abajo
    addMapping("thumbs_down", "pulgar_abajo");
    addMapping("thumbsdown", "pulgar_abajo");
    addMapping("dislike", "pulgar_abajo");
    addMapping("thumb_down", "pulgar_abajo");
    
    // Rock (amor)
    addMapping("rock", "rock");
    addMapping("i_love_you", "rock");
    addMapping("love", "rock");
    
    // Call me
    addMapping("call_me", "call_me");
    addMapping("callme", "call_me");
    addMapping("shaka", "call_me");
    addMapping("hang_loose", "call_me");
    
    // Índice arriba
    addMapping("point_up", "indice_arriba");
    addMapping("one", "indice_arriba");
    addMapping("pointing", "indice_arriba");
    addMapping("index_up", "indice_arriba");
    
    // OK
    addMapping("ok", "ok");
    addMapping("okay", "ok");
    addMapping("perfect", "ok");
    
    // Dedos cruzados
    addMapping("crossed_fingers", "dedos_cruzados");
    addMapping("fingers_crossed", "dedos_cruzados");
    addMapping("luck", "dedos_cruzados");
    
    // Cinco / Mano abierta
    addMapping("five", "cinco");
    addMapping("open_hand", "cinco");
    addMapping("palm", "cinco");
    addMapping("high_five", "cinco");
    addMapping("stop", "cinco");
    
    // Saludo alien
    addMapping("vulcan", "saludo_alien");
    addMapping("salute_alien", "saludo_alien");
    addMapping("live_long", "saludo_alien");
    addMapping("spock", "saludo_alien");
    
    // Metal
    addMapping("metal", "metal");
    addMapping("devil_horns", "metal");
    addMapping("horns", "metal");
    
    // Dedos juntos
    addMapping("pinch", "dedos_juntos");
    addMapping("pinched_fingers", "dedos_juntos");
    addMapping("chef_kiss", "dedos_juntos");
    addMapping("italian", "dedos_juntos");
    
    // Dedo medio
    addMapping("middle_finger", "dedo_medio");
    addMapping("middlefinger", "dedo_medio");
    addMapping("flip_off", "dedo_medio");
}

void ClassNameMapper::addMapping(const std::string& external, const std::string& internal) {
    // Convertir a lowercase para comparación case-insensitive
    std::string extLower = external;
    std::transform(extLower.begin(), extLower.end(), extLower.begin(), ::tolower);
    mapping[extLower] = internal;
}

std::string ClassNameMapper::map(const std::string& externalName) const {
    std::string extLower = externalName;
    std::transform(extLower.begin(), extLower.end(), extLower.begin(), ::tolower);
    
    // Reemplazar espacios y guiones por underscore
    for (auto& c : extLower) {
        if (c == ' ' || c == '-') c = '_';
    }
    
    // Buscar match exacto
    auto it = mapping.find(extLower);
    if (it != mapping.end()) return it->second;
    
    // Buscar match parcial
    for (const auto& [key, val] : mapping) {
        if (extLower.find(key) != std::string::npos || 
            key.find(extLower) != std::string::npos) {
            return val;
        }
    }
    
    return ""; // No mapeable
}

bool ClassNameMapper::isValidInternal(const std::string& internalName) const {
    for (const auto& [key, val] : mapping) {
        if (val == internalName) return true;
    }
    return false;
}

void ClassNameMapper::printMapping() const {
    std::cout << "=== MAPEO DE CLASES ===\n";
    for (const auto& [ext, internal] : mapping) {
        std::cout << "  " << ext << " → " << internal << "\n";
    }
    std::cout << "=======================\n";
}