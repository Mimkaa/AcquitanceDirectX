#pragma once
#include <string>
#include "BufferResource.h"
#include <memory>
#include  <stdexcept>
namespace Bind
{
	class Bindable;
};
class Source
{
public:
	Source(const std::string& name_in)
		:
		registeredName(name_in)
	{}
	std::string GetRegisteredName() const
	{
		return registeredName;
	}
	virtual std::shared_ptr<BufferResource> YeildBuffer()
	{
		throw std::runtime_error("this is a base class of direct source, you are doing something wrong");
	}
	virtual std::shared_ptr<Bind::Bindable> YieldBindable()
	{
		throw std::runtime_error("this is a base class of bind source, you are doing something wrong");
	}
	

private:
	std::string registeredName;

};

template<class T>
class DirectPassSource:public Source
{
public:
	DirectPassSource(std::string regName,std::shared_ptr<T>& target_in)
		:
		Source(std::move(regName)),
		bind{target_in}
	{
		linked = false;
	}
	static std::unique_ptr<DirectPassSource<T>> Make (std::string registeredName, std::shared_ptr<T>& target_in)
	{
		return std::make_unique<DirectPassSource<T>>(std::move(registeredName), target_in);
	}
	std::shared_ptr<BufferResource> YeildBuffer() override
	{
		
			if (linked)
			{
				std::ostringstream oss;
				oss << "you fuched up the class:" << typeid(this).name() << " with registered name:" << GetRegisteredName() <<
					"something is already liked to this crap";
				throw std::runtime_error(oss.str());
			}
			linked = true;
			return bind;
		
	}

private:
	bool linked;
	std::shared_ptr<T>& bind;
};


template<class T>
class BindPassSource :public Source
{
public:
	BindPassSource(std::string regName, std::shared_ptr<T>& target_in)
		:
		Source(std::move(regName)),
		bind{ target_in }
	{}
	static std::unique_ptr<BindPassSource<T>> Make(std::string registeredName, std::shared_ptr<T>& target_in)
	{
		return std::make_unique<BindPassSource<T>>(std::move(registeredName), target_in);
	}
	std::shared_ptr<Bind::Bindable> YieldBindable() override
	{
		
		return bind;
	}
	
private:
	std::shared_ptr<T>& bind;
};
