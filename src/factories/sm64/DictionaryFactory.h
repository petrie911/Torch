#pragma once

#include "../BaseFactory.h"

namespace SM64 {

class DictionaryData : public IParsedData {
public:
    std::unordered_map<std::string, std::vector<uint8_t>> mDictionary;

    DictionaryData(std::unordered_map<std::string, std::vector<uint8_t>> dictionary) : mDictionary(dictionary) {}
};

class DictionaryBinaryExporter : public BaseExporter {
    void Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class DictionaryFactory : public BaseFactory {
public:
    std::optional<std::shared_ptr<IParsedData>> parse(std::vector<uint8_t>& buffer, YAML::Node& data) override;
    std::optional<std::shared_ptr<IParsedData>> parse_modding(std::vector<uint8_t>& buffer, YAML::Node& data) override {
        return std::nullopt;
    }
    inline std::unordered_map<ExportType, std::shared_ptr<BaseExporter>> GetExporters() override {
        return { REGISTER(Binary, DictionaryBinaryExporter) };
    }
    bool SupportModdedAssets() override { return false; }
};
}