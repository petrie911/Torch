#pragma once

#include "ResourceType.h"
#include "n64/Cartridge.h"
#include <cstdint>
#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <optional>
#include <yaml-cpp/yaml.h>
#include <strhash64/StrHash64.h>
#include <binarytools/BinaryWriter.h>
#include <binarytools/BinaryReader.h>

#define REGISTER(type, c) { ExportType::type, std::make_shared<c>() },

#define SEGMENT_OFFSET(a) ((uint32_t)(a) & 0x00FFFFFF)
#define SEGMENT_NUMBER(x) (((uint32_t)(x) >> 24) & 0xFF)
#define IS_SEGMENTED(x) (((uint32_t)(x) > 0x01000000) && (SEGMENT_NUMBER(x) < 0x20))

#define tab "\t"
#define fourSpaceTab "    "

enum class ExportType {
    Header,
    Code,
    Binary,
    Modding,
};

template<typename T>
std::optional<T> GetNode(YAML::Node& node, const std::string& key) {
    if(!node[key]) {
        return std::nullopt;
    }

    return std::optional<T>(node[key].as<T>());
}

template<typename T>
T GetSafeNode(YAML::Node& node, const std::string& key) {
    if(!node[key]) {
        throw std::runtime_error("Failed to find " + key + " in yaml");
    }

    return node[key].as<T>();
}

template<typename T>
T GetSafeNode(YAML::Node& node, const std::string& key, const T& def) {
    if(!node[key]) {
        return def;
    }

    return node[key].as<T>();
}

class IParsedData {};

template<typename T>
class GenericData : public IParsedData {
public:
    T mData;
};

class BaseExporter {
public:
    virtual void Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) = 0;
    static void WriteHeader(LUS::BinaryWriter& write, LUS::ResourceType resType, int32_t version);
};

class BaseFactory {
public:
    virtual std::optional<std::shared_ptr<IParsedData>> parse(std::vector<uint8_t>& buffer, YAML::Node& data) = 0;
    virtual std::optional<std::shared_ptr<IParsedData>> parse_modding(std::vector<uint8_t>& buffer, YAML::Node& data) = 0;
    std::optional<std::shared_ptr<BaseExporter>> GetExporter(ExportType type) {
        auto exporters = this->GetExporters();
        if (exporters.find(type) != exporters.end()) {
            return exporters[type];
        }
        return std::nullopt;
    }
    virtual bool SupportModdedAssets() = 0;
private:
    virtual std::unordered_map<ExportType, std::shared_ptr<BaseExporter>> GetExporters() = 0;
};