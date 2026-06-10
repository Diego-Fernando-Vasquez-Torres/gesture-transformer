#ifndef CLASS_NAME_MAPPER_H
#define CLASS_NAME_MAPPER_H

#include <string>
#include <map>

class ClassNameMapper {
public:
    ClassNameMapper();
    
    // Mapea nombre externo → nombre interno. Devuelve "" si no hay match.
    std::string map(const std::string& externalName) const;
    
    // Verifica si un nombre interno es válido
    bool isValidInternal(const std::string& internalName) const;
    
    // Imprime el mapeo cargado
    void printMapping() const;

private:
    std::map<std::string, std::string> mapping;
    
    void addMapping(const std::string& external, const std::string& internal);
};

#endif