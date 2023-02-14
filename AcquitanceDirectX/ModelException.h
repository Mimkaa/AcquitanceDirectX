#pragma once
#include <string>
#include "ChiliException.hpp"

class ModelException :public ChiliException
{
public:
	ModelException(int line, const char* file, std::string note) noexcept;
	const char* what() const noexcept override;
	const std::string& GetNote()const noexcept;
	const char* GetType() const noexcept override;
private:
	std::string note;
};