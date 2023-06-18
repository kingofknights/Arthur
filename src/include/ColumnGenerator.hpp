//
// Created by VIKLOD on 25-01-2023.
//

#ifndef ARTHUR_INCLUDE_COLUMN_GENERATOR_HPP
#define ARTHUR_INCLUDE_COLUMN_GENERATOR_HPP
#pragma once
#include <string>
#include <unordered_map>

enum DataType;

#define COLUMN_GENERATOR_WINDOW "Column Generator Window"
class ColumnGenerator {
public:
	void paint(bool *show_);

private:
	std::string GetConfig();
	void		ParseConfig(std::string_view config_);
	void		DrawTable();
	void		AppendNewParameter();
	void		SetDefaultValue(DataType dataType_);

private:
	std::string _strategyName;
	std::string _parameterName;
	std::string _parameterValue;
	std::string _strategyLoad;
	std::string _selectedRow{};
	int			_parameterType = 0;

	using ColumnInfoT = struct ColumnInfoT {
		DataType	Type;
		std::string Value;
	};

	using ColumnParameterList = std::unordered_map<std::string, ColumnInfoT>;
	ColumnParameterList _parameterList;
};

#endif	// ARTHUR_INCLUDE_COLUMN_GENERATOR_HPP
