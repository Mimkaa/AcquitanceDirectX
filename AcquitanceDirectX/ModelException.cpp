#include "ModelException.h"
#include <sstream> 
ModelException::ModelException(int line, const char* file, std::string note) noexcept
	:
	ChiliException(line, file),
	note(note)
{
}

const char* ModelException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Origin]:" << GetOriginString() << std::endl
		<< "[Message]" << GetNote() << std::endl;
	whatBuffer = oss.str();
	return whatBuffer.c_str();

}

// exception
const std::string& ModelException::GetNote() const noexcept
{
	return note;
}

const char* ModelException::GetType() const noexcept
{
	return "Chilli Model Exception";
}