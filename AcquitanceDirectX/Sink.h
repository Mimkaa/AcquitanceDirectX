#pragma once
#include <string>
#include <memory>
#include <sstream>
#include <type_traits>
#include "Source.h"


class Sink
{
public:
	Sink(std::string regiseredNam)
		:
		registeredName{ std::move(regiseredNam) }
	{}
		
	std::string GetRegisteredName() const
	{
		return registeredName;
	}
	std::string GetSourceName() const
	{
		return sourceName;
	}
	std::string GetPassName() const
	{
		return passName;
	}
	void SetPass(std::string pass)
	{
		passName = std::move(pass);
	}
	void SetSource(std::string source)
	{
		sourceName = std::move(source);
	}
	virtual void PostLinkValidation() const
	{}
	virtual void Bind(std::unique_ptr<Source>& source ) = 0;

private:
	std::string passName;
	std::string sourceName;
	std::string registeredName;
	
};


template<class T>
class DirectPassSink : public Sink
{
public:
	static std::unique_ptr<Sink> Make (std::string registeredNam, std::shared_ptr<T>& target_in)
	{
		return std::make_unique<DirectPassSink>(std::move(registeredNam), target_in);
	}
	DirectPassSink(std::string registeredName, std::shared_ptr<T>& target_in)
		:
		Sink{ std::move(registeredName) },
		target(target_in)
	{
		linked = false;
	}

	void PostLinkValidation() const override
	{
		if (!linked)
		{
	 		throw std::runtime_error("one of the sinks is not linked");
		}
	}
	void Bind(std::unique_ptr<Source>& source)
	{
		auto p = std::dynamic_pointer_cast<T>(source->YeildBuffer());
		/*auto refff = source->YeildBuffer();
		

		auto t = GetRegisteredName();
		auto s = GetSourceName();
		auto x = GetPassName();
		bool is_null = refff == nullptr;
		bool com = !p;*/


		//auto ptr0 = refff.get(); // yeild buffer autism when u call it twice
		//auto ptr1 = target.get();

		//std::string name_0, name_1;

		//if (ptr0)
		//	name_0 = typeid(*ptr0).name();

		//if (ptr1)
		//	name_1 = typeid(*ptr1).name();
		

		if (!p)
		{
			
			std::ostringstream oss;
			oss << "you fuched up the class:" << typeid(this).name() << " with registered name:" << GetRegisteredName() <<
				" pointing to the source:" << GetSourceName() << " attached to :" << GetPassName() << " the types are not compatible";

			throw std::runtime_error(oss.str());
		}
		target = std::move(p);
		linked = true;
	}
	
private:
	bool linked;
	std::shared_ptr<T>& target;

};


template<class T>
class BindPassSink : public Sink
{
public:
	static std::unique_ptr<Sink> Make(std::string registeredName, std::shared_ptr<T>& target_in)
	{
		return std::make_unique<BindPassSink>(std::move(registeredName), target_in);
	}
	BindPassSink(std::string registeredName, std::shared_ptr<T>& target_in)
		:
		Sink{ std::move(registeredName) },
		target(target_in)
	{
		linked = false;
	}

	
	void Bind(std::unique_ptr<Source>& source)
	{
		auto p = std::dynamic_pointer_cast<T>(source->YieldBindable());
		if (!p)
		{
			std::ostringstream oss;
			oss << "you fucked up the class:" << typeid(this).name() << " with registered name:" << GetRegisteredName() <<
				"pointing to the source:" << GetSourceName() << "attached to :" << GetPassName() << "the types are not compatible";

			throw std::runtime_error(oss.str());
		}
		target = std::move(p);
		linked = true;
	}
private:
	bool linked;
	std::shared_ptr<T>& target;

};