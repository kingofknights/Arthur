#pragma once

#include <string>
#include <unordered_map>

enum DataType : int;

class TemplateBuilder final {
    using ColumnInfoT = struct ColumnInfoT {
        DataType    _type;
        std::string _value;
    };
    using ContainerT = std::unordered_map<std::string, ColumnInfoT>;

  public:
    explicit TemplateBuilder(bool& show_);

    void Paint(bool* show_);

    static constexpr char BeginColumnGenerator[] = "Template Builder Window";

  protected:
    [[nodiscard]] auto GetConfig() -> std::string;

    void ParseConfig(std::string_view config_);

    void DrawTable();

    void AppendNewParameter();

    void SetDefaultValue(DataType dataType_);

  private:
    bool& _show;

    std::string _strategyName;
    std::string _parameterName;
    std::string _parameterValue;
    std::string _strategyLoad;
    std::string _selectedRow;
    int         _parameterType = 0;

    ContainerT _parameterList;
};
