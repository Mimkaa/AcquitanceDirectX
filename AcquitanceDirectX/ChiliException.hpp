#pragma once
# include <exception>
#include <string>
class ChiliException:public std::exception
{
public:
	ChiliException(int line, const char* file) noexcept;
	const char* what() const noexcept override;
	virtual const char* GetType() const noexcept;
	int GetLine() const noexcept;
	// const after the method definition does not allow modifiction of "this" implicit parameter
	// e.g. : void f(int x) const = void f(const Obj* this, int x)
	std::string GetOriginString() const noexcept;
	const std::string& GetFile() const noexcept;
private:
	int line;
	std::string file;
protected:
	mutable std::string whatBuffer;
};

