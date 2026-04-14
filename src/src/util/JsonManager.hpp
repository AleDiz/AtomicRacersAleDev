#pragma once

#ifndef JSONMANAGER_H
#define JSONMANAGER_H

#include <iostream>
#include <fstream>
#include <sstream>


#include "../util/BasicUtils.hpp"
#include "../util/chronoAPI.hpp"
#include "../../rapidjson/document.h"
#include "../../rapidjson/writer.h"
#include "../../rapidjson/prettywriter.h"
#include "../../rapidjson/stringbuffer.h"

#include <array>
#include <vector>
#include <unordered_map>

#include "ObjectRigidType.hpp"

enum class types{
    WAYPOINT,
    BOXPOWERUP,
    BOOSTGROUND,
    WALL, 
    TYPE_WALL_SPECIAL
};

struct data{
    int id; 
    std::string shade;
    std::string typeRigidBody;
};

struct dataDuplicates{
    int id;
    std::string pathPositions;  
    std::string shade;
    types type;
};

struct dataSprite{
    int id;
    int posX;
    int posY;
    int alpha;
    int button;
};

struct Vertex{
    float x, y, z;
    Vertex(float x=0, float y=0, float z= 0) : x(x), y(y), z(z) {};
};
struct Face{
    int v1, v2, v3;
    Face(int v1, int v2, int v3) : v1(v1), v2(v2), v3(v3){};
};
struct dataObj{
    std::vector<Vertex> vertices_;
    std::vector<Face> faces_;
};

class JSONManager {
private:
    rapidjson::Document configData;

public:
    // Cargar el archivo JSON
    bool loadJSON(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: No se pudo abrir el archivo " << filename << std::endl;
            return false;
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();

        configData.Parse(buffer.str().c_str());
        if (configData.HasParseError()) {
            std::cerr << "Error al parsear el JSON" << std::endl;
            return false;
        }
        return true;
    }

    // Guardar el archivo JSON
    bool saveJSON(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: No se pudo abrir el archivo " << filename << " para guardar" << std::endl;
            return false;
        }

        rapidjson::StringBuffer buffer;
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
        configData.Accept(writer);

        file << buffer.GetString();
        file.close();
        return true;
    }

    void getInfoID(int id, std::array<std::string, 3>& direcciones){
        if(configData.HasMember("traducciones") && configData["traducciones"].IsArray()){
            for(const auto& item : configData["traducciones"].GetArray()){
                if(item[0].IsInt() && item[0].GetInt() == id){
                    direcciones[0] = item[1].GetString();
                    direcciones[1] = item[2].GetString();
                    direcciones[2] = item[3].GetString();
                }
            }
        }
    }

    void getInfoRarerity(const int rareza, std::vector<int>& ids){
        if(configData.HasMember("opciones") && configData["opciones"].IsArray()){
            for(const auto& item : configData["opciones"].GetArray()){
                if (item.HasMember("rareza") && item["rareza"].IsInt() &&
                    item.HasMember("idOBJ") && item["idOBJ"].IsInt()) {
                    if (item["rareza"].GetInt() == rareza) {
                        ids.push_back(item["idOBJ"].GetInt());
                    }
                }

            }
        }
    }

    bool addCarToCollection(const int id){
        using namespace rapidjson;
        Document::AllocatorType& allocator = configData.GetAllocator();

        if (!configData.HasMember("coches"))
            configData.AddMember("coches", Value(kArrayType), allocator);
        if (!configData.HasMember("coches_desbloqueados"))
            configData.AddMember("coches_desbloqueados", Value(kArrayType), allocator);

        for (const auto& item : configData["coches_desbloqueados"].GetArray()) {
            if (item.IsInt() && item.GetInt() == id) {
                int monedas = configData["monedas"].GetInt();
                configData["monedas"].SetInt(monedas + 100);
                return false;
            }
        }

        std::string fecha = obtenerFechaActual();
        Value fechaValor;
        fechaValor.SetString(fecha.c_str(), static_cast<SizeType>(fecha.length()), allocator);
        


        Value coche(kObjectType);
        coche.AddMember("idOBJ", id, allocator);
        coche.AddMember("fecha_obtenido", fechaValor, allocator);

        configData["coches"].PushBack(coche, allocator);
        configData["coches_desbloqueados"].PushBack(id, allocator);
        return true;
    }

    std::vector<int> getUnlockCars()
    {
        std::vector<int> unlockedCars;

        if(configData.HasMember("coches_desbloqueados") && configData["coches_desbloqueados"].IsArray()){
            for(const auto& item : configData["coches_desbloqueados"].GetArray()){
                if(item.IsInt()){
                    unlockedCars.push_back(item.GetInt());
                }
            }
        } else {
            std::cerr << "Error: 'coches_desbloqueados' no existe o no es un array." << std::endl;
        }

        return unlockedCars;
    }

    void getIntWithKey(const std::string& key, int& value) {
        if (configData.HasMember(key.c_str()) && configData[key.c_str()].IsInt()) {
            value = configData[key.c_str()].GetInt();
        } else {
            std::cerr << "Error: La clave " << key << " no existe o no es una cadena" << std::endl;
        }
    }
    
    void getFloatWithKey(const std::string& key, float& value) {
        if (configData.HasMember(key.c_str()) && configData[key.c_str()].IsNumber()) {
            value = configData[key.c_str()].GetFloat();
        } else {
            std::cerr << "Error: La clave " << key << " no existe o no es una cadena" << std::endl;
        }
    }

    void setIntWithKey(const std::string& key, int value) {
        if (configData.HasMember(key.c_str()) && configData[key.c_str()].IsInt()) {
            configData[key.c_str()].SetInt(value);
        } else {
            std::cerr << "Error: La clave " << key << " no existe o no es una cadena" << std::endl;
        }
    }

    void getInfo2D(int id, std::array<std::string, 4>& direcciones){
        if(configData.HasMember("sprites") && configData["sprites"].IsArray()){
            for(const auto& item : configData["sprites"].GetArray()){
                if(item[0].IsInt() && item[0].GetInt() == id){
                    direcciones[0] = item[1].GetString();
                    direcciones[1] = item[2].GetString();
                    direcciones[2] = item[3].GetString();
                    direcciones[3] = item[4].GetString();
                }
            }
        }
    }

    // Obtener un Vector3 (usando myVector3)
    myVector3 getVector3(const std::string& key) {
        myVector3 vec = {0.0f, 0.0f, 0.0f};
        if (configData.HasMember(key.c_str()) && configData[key.c_str()].IsArray() && configData[key.c_str()].Size() == 3) {
            vec.x = configData[key.c_str()][0].GetFloat();
            vec.y = configData[key.c_str()][1].GetFloat();
            vec.z = configData[key.c_str()][2].GetFloat();
        } else {
            std::cerr << "Error: La clave " << key << " no contiene un Vector3 válido" << std::endl;
        }
        return vec;
    }

    // Establecer un Vector3
    void setVector3(const std::string& key, const myVector3& vec) {
        rapidjson::Document::AllocatorType& allocator = configData.GetAllocator();
        rapidjson::Value array(rapidjson::kArrayType);
        array.PushBack(vec.x, allocator).PushBack(vec.y, allocator).PushBack(vec.z, allocator);
        configData[key.c_str()] = array;
    }

    // Obtener un valor entero
    int getInt(const std::string& key) {
        return configData.HasMember(key.c_str()) && configData[key.c_str()].IsInt() ? configData[key.c_str()].GetInt() : -1;
    }

    // Establecer un valor entero
    void setInt(const std::string& key, int value) {
        if (configData.HasMember(key.c_str()) && configData[key.c_str()].IsInt()) {
            configData[key.c_str()].SetInt(value);
        } else {
            std::cerr << "Error: La clave " << key << " no existe o no es una cadena" << std::endl;
        }
    }

    // Obtener un valor string
    std::string getString(const std::string& key) {
        return configData.HasMember(key.c_str()) && configData[key.c_str()].IsString() ? configData[key.c_str()].GetString() : "";
    }

    // Establecer un valor string
    void setString(const std::string& key, const std::string& value) {
        configData[key.c_str()].SetString(value.c_str(), configData.GetAllocator());
    }

    // Leer las posiciones de un tipo de objecto desde un JSON
    //----------------------------------------------------------
    // Input: Ruta del JSON, myVector4 donde guardar las posiciones y rotacion (si la tienen)
    // Output: myVector4 lleno de posiciones leidas
    void leerposiciones(const std::string& pathJSON, std::vector<myVector4>& boxPowerUpPositions){
        std::ifstream file(pathJSON);

        if(!file){
            std::cerr << "ERROR: No se pudo abrir el archivo JSON.\n";
        }
        else {
            std::stringstream buffer;
            buffer << file.rdbuf();
            std::string json = buffer.str();
            file.close();

            size_t startType = json.find("\"type\": ");
            size_t endType = json.find("\",");
            std::string type = json.substr(startType + 9, endType - startType - 9);


            size_t pos = json.find("\"positions\": [");
            if(pos == std::string::npos){
                std::cerr << "ERROR: No se encontraron posiciones en JSON.\n";
            }
            else {
                // Saltar el texto positions: [
                pos += 14;

                while (true) {
                    size_t start = json.find('[', pos);
                    size_t end = json.find(']', start);
                    if(start == std::string::npos || end == std::string::npos) break;

                    std::string vectorStr = json.substr(start +1 , end - start - 1);
                    std::stringstream vectorStream(vectorStr);
                    std::vector<float> values;
                    std::string num;

                    while (getline(vectorStream, num, ',')){
                        //num = cleanString(num);

                        if(!num.empty()) {
                            values.push_back(std::stof(num));  
                        }
                    }

                    if(values.size() == 3 || values.size() == 4){
                        boxPowerUpPositions.push_back({values[0], values[1], values[2], values[3]});
                    }

                    pos = end + 1;
                }

                // Imprimir las posiciones leídas
                int cant {0};
                for(myVector4& position : boxPowerUpPositions){
                    cant += 1;
                }
                
            }
        }
    }

    // Lee el JSON del nivel y carga la información en los vectores
    // ------------------------------------------------------------
    // IMPORTANTE: Cargar el JSON correspondiente antes de llamar a la función
    // INPUT: path del JSON del nivel, vector<data> donde guardar información de objetos únicos y vector<dataDuplicates> para objetos duplicados
    // OUTPUT: vectores llenos de información (ID, formaColision, tipo), (ID, posiciones, formaColision, tipo)
    void loadLevelData(std::vector<data>& dataVector, std::vector<dataDuplicates>& dataDuplicatesVector){
        // Abrir archivo JSON
        
        // Leer los valores
        if(configData.HasMember("dataMapObjectUnic") && configData["dataMapObjectUnic"].IsArray()){
            for(const auto& item : configData["dataMapObjectUnic"].GetArray()){
                if(item.IsArray() && item.Size() == 3) {
                    data obj;
                    obj.id = item[0].GetInt();
                    obj.shade = item[1].GetString();
                    obj.typeRigidBody = item[2].GetString();
                    dataVector.push_back(obj);
                }
            }
        }
        if(configData.HasMember("dataMapObjectDuplicates") && configData["dataMapObjectDuplicates"].IsArray()){
            for(const auto& item : configData["dataMapObjectDuplicates"].GetArray()){
                if(item.IsArray() && item.Size() == 4) {
                    dataDuplicates obj;
                    obj.id = item[0].GetInt();
                    obj.pathPositions = item[1].GetString();
                    obj.shade = item[2].GetString();
                    std::string readType = item[3].GetString();
                    if(readType == "WAYPOINT"){
                        obj.type = types::WAYPOINT;
                    }
                    else if(readType == "BOXPOWERUP"){
                        obj.type = types::BOXPOWERUP;
                    }
                    else if(readType == "BOOSTGROUND"){
                        obj.type = types::BOOSTGROUND;
                    }
                    else if(readType == "WALL"){
                        obj.type = types::WALL;
                    }
                    else if(readType == "TYPE_WALL_SPECIAL"){
                        obj.type = types::TYPE_WALL_SPECIAL;
                    }
                    dataDuplicatesVector.push_back(obj);
                }
            }
        }

        
        
    }

    bool loadOBJData(const std::string& pathToOBJ, dataObj& data){
        std::ifstream file(pathToOBJ);
        if(!file){
            std::cerr << "Error opening file: " << pathToOBJ << std::endl;
            return false;
        }

        std::string line;
        while(getline(file, line)){
            std::istringstream iss(line);
            std::string type;
            iss >> type;

            if(type == "v"){
                float x, y, z;
                iss >> x >> y >> z;
                data.vertices_.emplace_back(x, y, z);
            }
            else if(type == "f"){
                std::string v1, v2, v3;
                iss >> v1 >> v2 >> v3;
                
                auto extractVertexIndex = [](const std::string& faceElement) -> int {
                    size_t fristSlash = faceElement.find('/');
                    return std::stoi(faceElement.substr(0, fristSlash)) - 1;
                };
                int idx1 = extractVertexIndex(v1);
                int idx2 = extractVertexIndex(v2);
                int idx3 = extractVertexIndex(v3);

                data.faces_.emplace_back(idx1, idx2, idx3);
            }

        }
        return true;
    }

    // type must be: "small", "meidum", "large"
    std::unordered_map<std::string, float> getVehicleConfiguration(const std::string type){
        std::unordered_map<std::string, float> config;
        if(configData.HasMember(type.c_str()) && configData[type.c_str()].IsObject()){
            const rapidjson::Value& vehicleConfig = configData[type.c_str()];

            for (auto it = vehicleConfig.MemberBegin(); it != vehicleConfig.MemberEnd(); it++){
                if(it->value.IsNumber()){
                    config[it->name.GetString()] = it->value.GetFloat();
                }
            }
            return config;
        }
        
    }

    void loadMenuItems(std::vector<dataSprite>& dataMenu){
        if (!configData.HasMember("items") || !configData["items"].IsArray()) {
            std::cerr << "Error: JSON inválido o clave 'items' no encontrada" << std::endl;
            return;
        }

        for (const auto& item : configData["items"].GetArray()) {
            if (item.IsArray() && item.Size() == 5) {
                dataSprite spriteData;
                if (item[0].IsInt()) spriteData.id = item[0].GetInt();
                if (item[1].IsInt()) spriteData.posX = item[1].GetInt();
                if (item[2].IsInt()) spriteData.posY = item[2].GetInt();
                if (item[3].IsInt()) spriteData.alpha = item[3].GetInt();
                if (item[4].IsInt()) spriteData.button = item[4].GetInt();
                dataMenu.push_back(spriteData);
            }
        }
    }

    int getButtonMapping(int first) {
        if (configData.HasMember("button") && configData["button"].IsArray()) {
            for (const auto& item : configData["button"].GetArray()) {
                if (item.IsArray() && item.Size() == 2 && item[0].IsInt() && item[1].IsInt()) {
                    if (item[0].GetInt() == first) {
                        return item[1].GetInt();
                    }
                }
            }
        }
        return -1; // Retornar -1 si no se encuentra
    }

    bool guardarJSON(const std::string& filename) {
        using namespace rapidjson;
        StringBuffer buffer;
        Writer<StringBuffer> writer(buffer);
        configData.Accept(writer);

        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error al guardar el archivo JSON\n";
            return false;
        }

        file << buffer.GetString();
        file.close();
        return true;
    }
};

#endif
